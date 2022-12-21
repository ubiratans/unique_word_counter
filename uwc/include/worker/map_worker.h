#ifndef UWC_INCLUDE_WORKER_MAP_WORKER_H
#define UWC_INCLUDE_WORKER_MAP_WORKER_H

#include "uwc/include/chunk_processor.h"
#include "uwc/include/synchronized_queue.h"
#include "uwc/include/worker/thread_worker.h"

#include <cassert>
#include <memory>

namespace uwc
{
template <typename ProcessorType>
class MapWorker : public ThreadWorker<MapWorker<ProcessorType>>
{
  public:
    using Chunk = typename ProcessorType::Chunk;

    explicit MapWorker(std::shared_ptr<ProcessorType> chunk_processor,
                       std::shared_ptr<SynchronizedQueue<Chunk>> chunk_queue)
        : chunk_processor_(std::move(chunk_processor)),
          chunk_queue_(std::move(chunk_queue))
    {
        assert(chunk_processor_);
        assert(chunk_queue_);
    }

    bool StopCondition() { return chunk_queue_->IsComplete() && chunk_queue_->IsEmpty(); }

    void Execute()
    {
        auto chunk_opt = chunk_queue_->Pop();
        if (chunk_opt)
        {
            chunk_processor_->ProcessChunk(*std::move(chunk_opt));
        }
    }

  private:
    std::shared_ptr<ProcessorType> chunk_processor_;
    std::shared_ptr<SynchronizedQueue<Chunk>> chunk_queue_;
};
}  // namespace uwc

#endif
