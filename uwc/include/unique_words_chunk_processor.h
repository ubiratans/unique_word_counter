#ifndef UWC_INCLUDE_UNIQUE_WORDS_CHUNK_PROCESSOR_H
#define UWC_INCLUDE_UNIQUE_WORDS_CHUNK_PROCESSOR_H

#include "uwc/include/chunk_processor.h"
#include "uwc/include/word_set.h"

#include <istream>
#include <memory>
#include <string>
#include <unordered_set>

namespace uwc
{
/**
 * @brief Concrete implementation of ChunkProcessor, with the purpose of storing unique
 * words of a given input stream.
 *
 */

/// @todo(ubiratans): Templatize this processor in order to make it unit-testable
class UniqueWordsChunkProcessor
{
  public:
    explicit UniqueWordsChunkProcessor(std::unique_ptr<WordSet> word_set);

    struct Chunk
    {
        std::string raw_string;
        std::size_t length;
    };

    /**
     * @brief Extracts a single part of a given stream into a string object (chunk). The
     * returned chunk only contain whole words (non-fragmented).
     *
     * @param input_stream Source stream
     * @param stream_end_offset Offset indicating the last position on which the stream
     * should be consumed
     * @param chunk_size Size in bytes of the extracted chunk
     * @return String object containing the stream's extracted content
     *
     * @note It is assumed that the input_stream only contains ASCII letters and spaces.
     * @note The chunk may be greater than the chunk_size in order to avoid word
     * fragmentation.
     */
    Chunk RetrieveChunk(std::istream& input_stream,
                        std::streamoff stream_end_offset,
                        std::streamsize chunk_size);

    /**
     * @brief Process chunk and store all its unique words internally.
     *
     * @param chunk String containing only ASCII letters (from a to z) and spaces.
     */
    void ProcessChunk(Chunk&& chunk);

    /**
     * @brief Merges all intermediate processed chunks into a set containing all the
     * processed unique words.
     *
     * @return Set containing unique words from processed chunks
     */
    std::unordered_set<std::string> Aggregate();

  private:
    std::unique_ptr<WordSet> word_set_;
};
}  // namespace uwc

#endif
