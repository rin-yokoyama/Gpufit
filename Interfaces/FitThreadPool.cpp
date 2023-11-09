#include "FitThreadPool.hpp"

FitThreadPool* FitThreadPool::instance = nullptr;

FitThreadPool::FitThreadPool(int n_threads, int n_gpu) : stop(false)
{
    for (int i = 0; i < n_threads; ++i)
    {
        workers.emplace_back([this]
                             {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });

                        if (stop && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                } });
    }
    for (int i=0; i< n_threads; ++i)
    {
        if (i<n_gpu)
        {
            threadToIfgpu[workers[i].get_id()] = true;
        }
        else{
            threadToIfgpu[workers[i].get_id()] = false;
        }
    }
}

FitThreadPool::~FitThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();

    for (std::thread &worker : workers)
    {
        worker.join();
    }
}
