#ifndef UWC_INCLUDE_WORKER_SPLIT_WORKER_H
#define UWC_INCLUDE_WORKER_SPLIT_WORKER_H

#include "uwc/include/chunk_processor.h"
#include "uwc/include/worker/thread_worker.h"

#include <cassert>
#include <fstream>
#include <memory>

namespace uwc
{
template <typename ProcessorType>
class SplitWorker : public ThreadWorker<SplitWorker<ProcessorType>>
{
  public:
    using Chunk = typename ProcessorType::Chunk;

    SplitWorker(std::shared_ptr<ProcessorType> chunk_processor,
                std::shared_ptr<SynchronizedQueue<Chunk>> chunk_queue,
                std::istream& input_stream,
                const std::streamoff stream_end_offset,
                const std::streamsize chunk_size)
        : chunk_processor_(std::move(chunk_processor)),
          chunk_queue_(std::move(chunk_queue)),
          input_stream_(input_stream),
          stream_end_offset_(stream_end_offset),
          chunk_size_(chunk_size)
    {
    }

    bool StopCondition()
    {
        return chunk_queue_->IsComplete() || input_stream_.eof() ||
               input_stream_.tellg() >= stream_end_offset_;
    }

    void Execute()
    {
        chunk_queue_->Push(chunk_processor_->RetrieveChunk(
            input_stream_, stream_end_offset_, chunk_size_));
    }

  private:
    std::shared_ptr<ProcessorType> chunk_processor_;
    std::shared_ptr<SynchronizedQueue<Chunk>> chunk_queue_;
    std::istream& input_stream_;
    const std::streamoff stream_end_offset_;
    const std::streamsize chunk_size_;
};
}  // namespace uwc

#endif
