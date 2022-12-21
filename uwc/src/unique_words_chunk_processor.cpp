#include "uwc/include/unique_words_chunk_processor.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <sstream>

namespace uwc
{
using Chunk = UniqueWordsChunkProcessor::Chunk;

namespace
{
constexpr std::size_t kAdditionalBufferSize = 30U;

bool DoesChunkContainFragmentedWord(std::istream& input_stream,
                                    const std::istream::pos_type stream_end_offset,
                                    const std::string& chunk)
{
    const auto current_offset = input_stream.tellg();
    const auto num_read_chars = input_stream.gcount();
    assert(num_read_chars > 0);
    const auto chunk_last_char_index = num_read_chars - 1;

    return !input_stream.eof() && current_offset < stream_end_offset &&
           !std::isspace(chunk[chunk_last_char_index]) &&
           !std::isspace(input_stream.peek());
}
}  // namespace

UniqueWordsChunkProcessor::UniqueWordsChunkProcessor(std::unique_ptr<WordSet> word_set)
    : word_set_(std::move(word_set))
{
}

Chunk UniqueWordsChunkProcessor::RetrieveChunk(std::istream& input_stream,
                                               std::streamoff stream_end_offset,
                                               const std::streamsize chunk_size)
{
    assert(input_stream.good() && !input_stream.eof());
    const auto current_pos = input_stream.tellg();
    assert(current_pos <= stream_end_offset);

    const auto remaining_num_chars =
        static_cast<std::streamsize>(stream_end_offset - current_pos);
    const auto num_chars = std::min(chunk_size, remaining_num_chars);
    const auto extra_buffer_size =
        num_chars == remaining_num_chars ? 0U : kAdditionalBufferSize;

    // include extra bytes in buffer, in order to try to avoid reallocations in case the
    // chunk size is not enough to hold all 'intersected' words while preventing word
    // fragmentation.
    std::string str_chunk(num_chars + extra_buffer_size, ' ');
    input_stream.read(&str_chunk[0], num_chars);
    const auto num_read_chars = input_stream.gcount();

    Chunk chunk{std::move(str_chunk), static_cast<std::size_t>(num_read_chars)};
    if (DoesChunkContainFragmentedWord(input_stream, stream_end_offset, chunk.raw_string))
    {
        std::string last_word;
        // do formatted extraction with operator>> in order to avoid word fragmentation
        input_stream >> last_word;
        chunk.length += last_word.length();
        chunk.raw_string.replace(num_read_chars, last_word.length(), last_word);
    }

    return chunk;
}

void UniqueWordsChunkProcessor::ProcessChunk(Chunk&& chunk)
{
    auto end_it = std::next(chunk.raw_string.cbegin(), chunk.length);
    auto is_not_space = [](const auto c) { return !std::isspace(c); };
    auto is_space = [](const auto c) { return std::isspace(c); };

    auto word_begin_it = std::find_if(chunk.raw_string.cbegin(), end_it, is_not_space);
    auto word_end_it = std::find_if(word_begin_it, end_it, is_space);
    while (word_begin_it < end_it)
    {
        word_set_->Insert({word_begin_it, word_end_it});

        word_begin_it = std::find_if(word_end_it, end_it, is_not_space);
        word_end_it = std::find_if(word_begin_it, end_it, is_space);
    }
}

std::unordered_set<std::string> UniqueWordsChunkProcessor::Aggregate()
{
    return word_set_->Merge();
}
}  // namespace uwc
