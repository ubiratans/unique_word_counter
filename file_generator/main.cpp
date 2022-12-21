#include "file_generator/word_list.h"

#include <fstream>
#include <iostream>
#include <random>
#include <string_view>
#include <unordered_set>

/// @todo(ubiratan): Make it receive a file_name containing the words to be used instead
/// of having them hard-coded in word_list.h, as it takes a long time to compile.
int main(int argc, const char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Expected usage: file_generator <output_file> <size_in_mega_bytes>, "
                     "e.g.:\n"
                  << "  file_generator my_text_file.txt 1024\n"
                  << std::endl;
    }

    const auto file_name = argv[1];
    const auto file_size = std::stoull(argv[2]) * 1024 * 1024;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0,
                                                                  kWordList.size() - 1U);

    std::ofstream output_stream(file_name);
    if (!output_stream.is_open())
    {
        std::cerr << "Error opening file: " << file_name << std::endl;
        return -1;
    }

    std::size_t bytes_written = 0;
    std::unordered_set<std::string> unique_words;
    while (bytes_written < file_size)
    {
        const auto& word = kWordList[dist(rng)];
        bytes_written += word.length() + 1U;
        unique_words.emplace(word);

        output_stream << word << " ";
    }
    output_stream << std::flush;

    std::cout << "Wrote " << bytes_written / (1024 * 1024) << " MB in " << file_name
              << "\n"
              << "Number of unique words: " << unique_words.size() << std::endl;

    return 0;
}
