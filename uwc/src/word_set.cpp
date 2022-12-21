#include "uwc/include/word_set.h"

namespace uwc
{
namespace
{
std::size_t RetrieveWordSetIndex(const std::string& word,
                                 const std::size_t num_hash_tables)
{
    return std::hash<std::string>{}(word) % num_hash_tables;
}
}  // namespace

WordSet::WordSet(const std::size_t num_hash_tables)
    : word_set_list_(num_hash_tables), set_mutex_list_(num_hash_tables)
{
}

void WordSet::Insert(std::string word)
{
    if (word.empty())
    {
        return;
    }

    const auto set_index = RetrieveWordSetIndex(word, set_mutex_list_.size());
    std::lock_guard<std::mutex> lock(set_mutex_list_.at(set_index));
    word_set_list_[set_index].emplace(std::move(word));
}

std::unordered_set<std::string> WordSet::Merge()
{
    std::size_t num_words = 0U;
    std::size_t i = 0;
    for (const auto& word_set : word_set_list_)
    {
        num_words += word_set.size();
        ++i;
    }

    std::unordered_set<std::string> result;
    result.reserve(num_words);
    for (auto&& word_set : word_set_list_)
    {
        result.merge(std::move(word_set));
    }

    return result;
}

}  // namespace uwc
