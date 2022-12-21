#ifndef UWC_INCLUDE_WORKER_THREAD_WORKER_H
#define UWC_INCLUDE_WORKER_THREAD_WORKER_H

namespace uwc
{
template <typename T>
class ThreadWorker
{
  public:
    void operator()()
    {
        auto& t = static_cast<T&>(*this);

        while (!t.StopCondition())
        {
            t.Execute();
        }
    }
};
}  // namespace uwc

#endif
