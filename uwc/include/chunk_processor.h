#ifndef UWC_INCLUDE_PARALLEL_FILE_PROCESSOR_H
#define UWC_INCLUDE_PARALLEL_FILE_PROCESSOR_H

#include <string>
#include <type_traits>
#include <unordered_set>

/// @todo(ubiratan): There is probably a better way to define this 'interface'
/// @todo(ubiratan): Rethink the need of this ChunkProcessor
namespace uwc
{
/**
 * @brief ChunkProcessor interface - It provides the core unit logic for processing a
 file
 * in parallel using an approach based on MapReduce model. Whereas RetrieveChunk is the
 * logic to be used in the Split phase, ProcessChunk in the mapping phase and Aggregate
 * for the merging phase.
 *
 * @tparam T Type containing the concrete implementation for the interface.
 */
template <typename T>
class ChunkProcessor : public T
{
  public:
    using Chunk = typename std::result_of<decltype (
        &T::RetrieveChunk)(T, std::istream&, std::streamoff, std::streamsize)>::type;
    using Result = typename std::result_of<decltype (&T::Aggregate)(T)>::type;

    /**
     * @brief Retrieve a chunk_size chunk from the given input_stream.
     *
     * @param input_stream Source stream
     * @param stream_end_offset Offset indicating the last position on which the stream
     * should be consumed
     * @param chunk_size Chunk size in bytes
     * @return Chunk
     */
    Chunk RetrieveChunk(std::istream& input_stream,
                        const std::streamoff stream_end_offset,
                        const std::streamsize chunk_size)
    {
        return static_cast<T&>(*this).RetrieveChunk(
            input_stream, stream_end_offset, chunk_size);
    }

    /**
     * @brief Transforms a given chunk into ProcessedChunk
     *
     * @param chunk
     * @return ProcessedChunk
     */
    void ProcessChunk(Chunk&& chunk)
    {
        static_cast<T&>(*this).ProcessChunk(std::move(chunk));
    }

    /**
     * @brief Merges a given processed chunk into the global result
     *
     * @param processed_chunk Processed chunk
     * @param result Global result
     */
    Result Aggregate() { return static_cast<T&>(*this).Aggregate(); }
};
}  // namespace uwc

#endif
