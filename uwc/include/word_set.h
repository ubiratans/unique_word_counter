#ifndef UWC_INCLUDE_WORD_SET_H
#define UWC_INCLUDE_WORD_SET_H

#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace uwc
{
/**
 * @brief Helper class to be used by multiple threads for storing strings while reducing
 * contention.
 */
class WordSet
{
  public:
    explicit WordSet(std::size_t num_hash_tables);

    // Thread-safe
    void Insert(std::string word);

    // Non-Thread safe
    std::unordered_set<std::string> Merge();

  private:
    /// @todo(ubiratan): research more about proper data structs to use in this case
    std::vector<std::unordered_set<std::string>> word_set_list_;
    std::vector<std::mutex> set_mutex_list_;
};
}  // namespace uwc

#endif
