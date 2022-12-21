#ifndef UWC_TEST_MOCK_WORD_SET_MOCK_H
#define UWC_TEST_MOCK_WORD_SET_MOCK_H

#include "uwc/include/word_set.h"

#include <gmock/gmock.h>

namespace uwc
{
namespace test
{
class WordSetMock
{
    MOCK_METHOD(void, Insert, (std::string));
    MOCK_METHOD(std::unordered_map<std::string>, Merge, ());
};
}  // namespace test
}  // namespace uwc

#endif
