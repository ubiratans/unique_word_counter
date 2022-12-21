#ifndef UWC_INCLUDE_FILE_PROCESSING_H
#define UWC_INCLUDE_FILE_PROCESSING_H

#include "uwc/include/chunk_processor.h"
#include "uwc/include/synchronized_queue.h"
#include "uwc/include/worker/map_worker.h"
#include "uwc/include/worker/split_worker.h"
#include "uwc/include/worker/thread_worker.h"

#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace uwc
{
/// @todo(ubiratan): Refactor this file in order to not expose unnecessary functions.
/// Convert ProcessFileInParallel function to a class?
template <typename ChunkProcessorType, typename ChunkQueueType>
std::vector<std::thread> CreateAndStartMapWorkerThreads(
    ChunkProcessorType& chunk_processor,
    ChunkQueueType& chunk_sync_queue,
    const std::uint8_t num_threads)
{
    std::vector<std::thread> mapping_worker_threads;
    mapping_worker_threads.reserve(num_threads);
    for (std::uint8_t i = 0; i < num_threads; ++i)
    {
        mapping_worker_threads.emplace_back(MapWorker(chunk_processor, chunk_sync_queue));
    }

    return mapping_worker_threads;
}

void JoinThreads(std::vector<std::thread>& threads)
{
    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

/**
 * @brief Process a given file using a MapReduce alike parallel approach. It uses the
 * calling thread to read the file in chunks and it creates a specified number of threads
 * for processing the chunks. After all file is read and processed, the processed chunks
 * are merged, in the calling thread, and returned.
 *
 * @note The chunk processing begins as soon as at least one chunk is read from the file.
 * @note There is a limit on how many chunks are stored at a given time. This limit is
 * equal to num_mapping_threads*10. (This might be better received as a parameter)
 *
 * @tparam Processor Class providing chunk extraction, processing and merging functions
 * following the ChunkProcessor interface. (This is outdated, the ChunkProcessor is not
 * being currently used)
 *
 * @param file_name File to be read and processed
 * @param chunk_size Size in bytes of read chunks from the file
 * @param num_mapping_threads Number of threads for processing the read chunks
 * @param chunk_processor Chunk processor instance
 * @return File processing result - Processor::Aggregate() result
 */
template <typename Processor>
auto ProcessFileInParallel(const std::string& file_name,
                           const std::streamsize chunk_size,
                           const std::uint8_t num_mapping_threads,
                           std::shared_ptr<Processor> chunk_processor)
{
    using Chunk = typename Processor::Chunk;

    /// @todo(ubiratan): Improve this function, split it and make it more
    /// readable/intuitive
    /// @todo(ubiratan): Implement a proper error handling mechanism
    // Open File
    std::ifstream file_stream(file_name);
    if (!file_stream.is_open() || num_mapping_threads == 0)
    {
        throw std::runtime_error(
            "Could not open file or num_mapping_threads is equal to 0");
    }

    file_stream.seekg(std::ios_base::beg, std::ios_base::end);
    const auto file_stream_end_offset = file_stream.tellg();
    file_stream.seekg(std::ios_base::beg);

    const auto max_chunks = num_mapping_threads * 10U;
    auto chunk_sync_queue = std::make_shared<SynchronizedQueue<Chunk>>(max_chunks);

    // Start MapWorker Threads
    auto map_worker_threads = CreateAndStartMapWorkerThreads(
        chunk_processor, chunk_sync_queue, num_mapping_threads);

    // Start split worker in current thread
    SplitWorker split_worker(chunk_processor,
                             chunk_sync_queue,
                             file_stream,
                             file_stream_end_offset,
                             chunk_size);
    split_worker();
    chunk_sync_queue->MarkAsComplete();

    // Wait for Mapping Worker Threads
    JoinThreads(map_worker_threads);
    return chunk_processor->Aggregate();
}
}  // namespace uwc

#endif
