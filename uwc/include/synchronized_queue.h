#ifndef UWC_INCLUDE_SYNCHRONIZED_QUEUE_H
#define UWC_INCLUDE_SYNCHRONIZED_QUEUE_H

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace uwc
{
/**
 * @brief Minimalist thread-safe queue providing some facilities for communicating between
 * threads, using Producer-Consumer approach.
 *
 * @tparam T Type to be held by the queue
 */
template <typename T>
class SynchronizedQueue
{
  public:
    SynchronizedQueue(std::size_t queue_max_size)
        : queue_max_size_(queue_max_size), queue_size_(0), complete_flag_(false)
    {
        assert(queue_max_size_ > 0);
    }

    /**
     * @brief Inserts a new entry in the queue.
     *
     * @details It blocks in case the queue is full and it doesn't perform any action in
     * case the queue is marked as complete.
     *
     * @param entry Entry to be added
     */
    void Push(T&& entry)
    {
        if (complete_flag_)
        {
            return;
        }

        {
            std::unique_lock<std::mutex> full_queue_lock(push_mutex_);
            queue_full_token_.wait(full_queue_lock, [this]() {
                return queue_size_ < queue_max_size_ || complete_flag_;
            });

            if (!complete_flag_)
            {
                std::lock_guard<std::mutex> lock(pop_mutex_);
                queue_.push_back(std::move(entry));
                ++queue_size_;
            }
        }

        sync_token_.notify_one();
    }

    /**
     * @brief Extracts the entry corresponding to the top of the queue.
     *
     * @details It blocks in case the queue is empty and not flagged as complete. If the
     * queue is flagged as complete and empty, then a std::nullopt is returned.
     *
     * @return Extracted entry
     */
    std::optional<T> Pop()
    {
        std::unique_lock<std::mutex> lock(pop_mutex_);
        sync_token_.wait(lock, [this]() { return complete_flag_ || queue_size_ > 0; });

        if (complete_flag_ && queue_.empty())
        {
            return std::nullopt;
        }

        auto entry = std::make_optional(std::move(queue_.front()));
        queue_.pop_front();
        --queue_size_;
        lock.unlock();

        {  // avoid race condition with Push thread(s)
           // https://stackoverflow.com/questions/17101922/do-i-have-to-acquire-lock-before-calling-condition-variable-notify-one
            std::lock_guard<std::mutex> full_queue_lock(push_mutex_);
        }
        queue_full_token_.notify_one();

        return entry;
    }

    /**
     * @brief Is the queue empty?
     *
     * @return true, in case there is not entry added in the queue
     * @return false, otherwise
     */
    bool IsEmpty() const { return queue_size_ == 0; }

    /**
     * @brief Returns information about the queue 'completeness'. The queue is flagged as
     * complete if no more entries will be added to it.
     *
     * @return true, queue is marked as complete
     * @return false, otherwise
     */
    bool IsComplete() const { return complete_flag_; }

    /**
     * @brief Specify that no new entries will be added to the queue.
     */
    void MarkAsComplete()
    {
        complete_flag_ = true;
        queue_full_token_.notify_all();
        sync_token_.notify_all();
    }

  private:
    std::deque<T> queue_;
    const std::size_t queue_max_size_;
    std::atomic_size_t queue_size_;
    std::mutex pop_mutex_;
    std::mutex push_mutex_;
    std::condition_variable queue_full_token_;
    std::condition_variable sync_token_;
    std::atomic_bool complete_flag_;
};
}  // namespace uwc

#endif
