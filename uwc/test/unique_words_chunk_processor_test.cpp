#include "uwc/include/unique_words_chunk_processor.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iterator>
#include <sstream>
#include <string_view>
#include <vector>

namespace
{
using uwc::UniqueWordsChunkProcessor;
using Chunk = UniqueWordsChunkProcessor::Chunk;

std::string_view RightTrim(const std::string& str)
{
    std::string_view string_view(str);
    while (std::isspace(string_view.back()))
    {
        string_view.remove_suffix(1);
    }

    return string_view;
}

std::vector<std::string> Split(const std::string& str)
{
    std::stringstream stream(str);
    std::vector<std::string> word_list;
    std::transform(std::istream_iterator<std::string>(stream),
                   std::istream_iterator<std::string>(),
                   std::back_inserter(word_list),
                   [](const auto& word) { return word; });

    return word_list;
}

// TEST(UniqueWordsChunkProcessor_RetrieveChunk,
//      ChunkSizeEqualToStreamSize_ExpectCompleteContentInResult)
// {
//     const std::string input_string =
//         "my complete string contains multiple words and might    contain multiple "
//         "consecutive    spaces";
//     const Chunk expected_chunk{input_string, input_string.length()};

//     std::stringstream input_stream(input_string);
//     const auto chunk_size = input_string.size();
//     const auto stream_end_offset = chunk_size;

//     UniqueWordsChunkProcessor word_processor;
//     const auto chunk =
//         word_processor.RetrieveChunk(input_stream, stream_end_offset, chunk_size);
//     EXPECT_EQ(expected_chunk.raw_string, chunk.raw_string);
//     EXPECT_EQ(expected_chunk.length, chunk.length);
// }

// TEST(UniqueWordsChunkProcessor_RetrieveChunk,
//      SingleWordStream_ChunkSizeSmallerThanStreamSize_ExpectSingleChunkContainingWholeWord)
// {
//     const std::string input_string = "horse";
//     std::stringstream input_stream(input_string);
//     const auto chunk_size = 2U;
//     const auto stream_end_offset = input_string.length();

//     UniqueWordsChunkProcessor word_processor;
//     const auto chunk =
//         word_processor.RetrieveChunk(input_stream, stream_end_offset, chunk_size);
//     EXPECT_EQ(input_string, RightTrim(chunk.raw_string));
//     EXPECT_EQ(input_string.length(), chunk.length);
// }

// TEST(UniqueWordsChunkProcessor_RetrieveChunk,
//      MultiWordStream_EveryWordFitsPerfectlyInChunk_ExpectEachChunkContainingOneWord)
// {
//     const std::string input_string = "horse tiger lemon koala knife house";
//     const auto chunk_size = 6U;
//     const auto stream_end_offset = input_string.length();

//     const std::vector<Chunk> expected_result_list = {{"horse ", 6U},
//                                                      {"tiger ", 6U},
//                                                      {"lemon ", 6U},
//                                                      {"koala ", 6U},
//                                                      {"knife ", 6U},
//                                                      {"house", 5U}};

//     std::stringstream input_stream(input_string);
//     UniqueWordsChunkProcessor word_processor;
//     for (const auto& expected_result : expected_result_list)
//     {
//         const auto chunk =
//             word_processor.RetrieveChunk(input_stream, stream_end_offset, chunk_size);
//         EXPECT_EQ(RightTrim(expected_result.raw_string), RightTrim(chunk.raw_string));
//         EXPECT_EQ(expected_result.length, chunk.length);
//     }
// }

// TEST(UniqueWordsChunkProcessor_RetrieveChunk,
//      MultiWordStream_SomeWordsDoNotFitChunkPerfectly_ExpectNoWordFragmentationInAnyChunk)
// {
//     const std::string input_string = "beatles rollingstones beachboys richiekotzen";
//     const auto chunk_size = 10U;
//     const std::vector<Chunk> expected_result_list = {
//         {"beatles rollingstones", 21U}, {" beachboys", 10U}, {" richiekotzen", 13U}};
//     const auto stream_end_offset = input_string.length();

//     std::stringstream input_stream(input_string);
//     uwc::UniqueWordsChunkProcessor word_processor;
//     for (const auto& expected_result : expected_result_list)
//     {
//         const auto chunk =
//             word_processor.RetrieveChunk(input_stream, stream_end_offset, chunk_size);
//         EXPECT_EQ(expected_result.raw_string, RightTrim(chunk.raw_string));
//         EXPECT_EQ(expected_result.length, chunk.length);
//     }
// }

// TEST(
//     UniqueWordsChunkProcessor_ProcessChunk,
//     ValidChunkWithManyConsecutiveSpacesButNoRepeatedWords_ExpectSpacesIgnoredAndEveryWordInResult)
// {
//     const std::vector<std::string> input_string_list = {
//         "           dreamtheater     journey kiss       legiao mrbig shaman sepultura
//         ", "pantera             ", "hehehhe  ", "   hehehe", "singleword", " many words
//         in a chunk is not that rare "};

//     std::vector<Chunk> input_chunks;
//     input_chunks.reserve(input_string_list.size());
//     std::transform(input_string_list.cbegin(),
//                    input_string_list.cend(),
//                    std::back_inserter(input_chunks),
//                    [](const auto& str) {
//                        const auto str_length =
//                            RightTrim(str).length() + (std::isspace(str.back()) ? 1U :
//                            0U);
//                        return Chunk{str, str_length};
//                    });

//     UniqueWordsChunkProcessor word_processor;
//     for (auto input_chunk : input_chunks)
//     {
//         const auto expected_result = Split(input_chunk.raw_string);
//         const auto result = word_processor.ProcessChunk(std::move(input_chunk));
//         EXPECT_THAT(result, ::testing::UnorderedElementsAreArray(expected_result));
//     }
// }

// TEST(UniqueWordsChunkProcessor_ProcessChunk,
//      ValidChunkContainingRepeatedWords_ExpectSpacesIgnoredAndEveryWordInResult)
// {

//     std::vector<std::string> input_string_list = {
//         "my house is not big but is not a small house either",
//         "  1 2    3  4 5 6 7 8 9 1 2   2  3 1 2 3 1 2 3",
//         "i have a dog but i do not have a cat"};
//     const std::vector<std::vector<std::string>> expected_result_list = {
//         {"my", "house", "is", "not", "big", "but", "a", "small", "either"},
//         {"1", "2", "3", "4", "5", "6", "7", "8", "9"},
//         {"i", "have", "a", "dog", "but", "do", "not", "cat"}};

//     uwc::UniqueWordsChunkProcessor word_processor;
//     for (std::size_t i = 0; i < input_string_list.size(); ++i)
//     {
//         const auto result =
//         word_processor.ProcessChunk(std::move(input_string_list[i]));
//         EXPECT_THAT(result,
//                     ::testing::UnorderedElementsAreArray(expected_result_list[i]));
//     }
// }

// TEST(UniqueWordsChunkProcessor_Aggregate,
//      ValidChunkContainingRepeatedWords_ExpectSpacesIgnoredAndEveryWordInResult)
// {
//     uwc::UniqueWordsChunkProcessor word_processor;
//     std::unordered_set<std::string> result;

//     {  // aggregate first chunk
//         std::unordered_set<std::string> processed_chunk = {
//             "my", "first", "processed", "chunk"};
//         word_processor.Aggregate(std::move(processed_chunk), result);
//         ASSERT_THAT(
//             result,
//             ::testing::UnorderedElementsAreArray({"my", "first", "processed",
//             "chunk"}));
//     }

//     {  // aggregate second chunk
//         std::unordered_set<std::string> processed_chunk = {
//             "my", "second", "processed", "chunk"};
//         word_processor.Aggregate(std::move(processed_chunk), result);
//         ASSERT_THAT(result,
//                     ::testing::UnorderedElementsAreArray(
//                         {"my", "first", "processed", "chunk", "second"}));
//     }

//     {  // aggregate third chunk
//         std::unordered_set<std::string> processed_chunk = {
//             "not", "related", "to", "the", "other", "chunks"};
//         word_processor.Aggregate(std::move(processed_chunk), result);
//         ASSERT_THAT(result,
//                     ::testing::UnorderedElementsAreArray({"my",
//                                                           "first",
//                                                           "processed",
//                                                           "chunk",
//                                                           "second",
//                                                           "not",
//                                                           "related",
//                                                           "to",
//                                                           "the",
//                                                           "other",
//                                                           "chunks"}));
//     }
// }
}  // namespace
