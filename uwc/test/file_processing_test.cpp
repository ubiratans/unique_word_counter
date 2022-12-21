#include "uwc/include/file_processing.h"

#include "uwc/include/unique_words_chunk_processor.h"

#include <gtest/gtest.h>

#include <iterator>
#include <memory>
#include <sstream>
#include <string_view>
#include <thread>
#include <vector>

namespace
{
using uwc::ChunkProcessor;
using uwc::UniqueWordsChunkProcessor;

const auto kMaxThreads = std::thread::hardware_concurrency() - 1U;
const auto kHalfThreads = kMaxThreads / 2U;
constexpr std::size_t kNumHashTables = 104147U;

/// @todo(ubiratan): Pass filename and expected result through environment variables
const std::string kFileName = "/home/ubiratans/personal/keepit/test_file_10gb.txt";
const std::size_t kExpectedUniqueWordsCounter = 58109;

class UniqueWordsCounterInFileFixture : public ::testing::TestWithParam<std::streamsize>
{
};

INSTANTIATE_TEST_CASE_P(UniqueWordsCounterTests,
                        UniqueWordsCounterInFileFixture,
                        ::testing::Values(8 * 1024,
                                          16 * 1024,
                                          64 * 1024,
                                          256 * 1024,
                                          1024 * 1024,
                                          4 * 1024 * 1024));

TEST_P(UniqueWordsCounterInFileFixture, NaiveTest_MaxThreads_Chunk)
{
    const auto chunk_size = GetParam();

    std::cout << "filename: " << kFileName << "\nChunk size: " << chunk_size / 1024
              << " Kb"
              << "\nNum Threads: " << kMaxThreads + 1U << std::endl;

    auto chunk_processor = std::make_shared<UniqueWordsChunkProcessor>(
        std::make_unique<uwc::WordSet>(kNumHashTables));
    auto result =
        uwc::ProcessFileInParallel(kFileName, chunk_size, kMaxThreads, chunk_processor);

    EXPECT_EQ(kExpectedUniqueWordsCounter, result.size());
}

TEST_P(UniqueWordsCounterInFileFixture, NaiveTest_HalfThreads_Chunk)
{
    const auto chunk_size = GetParam();

    std::cout << "filename: " << kFileName << "\nChunk size: " << chunk_size / 1024
              << " Kb"
              << "\nNum Threads: " << kHalfThreads + 1U << std::endl;

    auto chunk_processor = std::make_shared<UniqueWordsChunkProcessor>(
        std::make_unique<uwc::WordSet>(kNumHashTables));
    auto result =
        uwc::ProcessFileInParallel(kFileName, chunk_size, kHalfThreads, chunk_processor);

    EXPECT_EQ(kExpectedUniqueWordsCounter, result.size());
}

TEST(UniqueWordsCounterInFile, NaiveTestSequential)
{
    std::ifstream stream(kFileName);

    std::unordered_set<std::string> words;
    std::string word;
    while (stream >> word)
    {
        words.emplace(word);
    }

    EXPECT_EQ(kExpectedUniqueWordsCounter, words.size());
}
}  // namespace
