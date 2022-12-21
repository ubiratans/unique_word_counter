#include "uwc/include/file_processing.h"
#include "uwc/include/unique_words_chunk_processor.h"

#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

int main(int argc, const char* argv[])
{
    constexpr std::streamsize kChunkSize = 16 * 1024;  // 16 Kb
    constexpr std::size_t kNumHashTables =
        104147U;  // Make this dependent on the file size.

    if (argc < 2)
    {
        std::cout << "Expected usage: unique_word_counter <file>"
                     "\n\nExample: unique_word_counter ~/huge_filte.txt\n"
                  << std::endl;
    }

    const auto file_name = argv[1];

    const auto num_mapping_threads = std::thread::hardware_concurrency() > 0
                                         ? std::thread::hardware_concurrency() - 1U
                                         : 1U;

    auto chunk_processor = std::make_shared<uwc::UniqueWordsChunkProcessor>(
        std::make_unique<uwc::WordSet>(kNumHashTables));

    std::cout << "Processing file: " << file_name << " ..." << std::endl;

    try
    {
        const auto start_time = std::chrono::high_resolution_clock::now();
        auto result = uwc::ProcessFileInParallel(
            file_name, kChunkSize, num_mapping_threads, chunk_processor);
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::chrono::high_resolution_clock::now() - start_time)
                                  .count();

        std::cout << "Number of unique words: " << result.size()
                  << " [Duration: " << duration << " ms]" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
