#ifndef __FIT_THREAD_POOL__
#define __FIT_THREAD_POOL__

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <map>

class FitThreadPool
{
private:
    FitThreadPool(int n_threads, int n_gpu);
    
    virtual ~FitThreadPool();
    
    static FitThreadPool *instance;

public:
    FitThreadPool() = delete;
    FitThreadPool(const FitThreadPool &) = delete;
    FitThreadPool &operator=(const FitThreadPool &) = delete;
    FitThreadPool(FitThreadPool &&) = delete;
    FitThreadPool &operator=(FitThreadPool &&) = delete;

    static FitThreadPool &GetInstance(){return *instance;}

    static void Create(int n_threads, int n_gpu)
    {
        if(n_threads < n_gpu)
        {
            std::cout << "[FitThreadPool]: n_gpu has to be smaller than or equal to n_threads" << std::endl;
        }
        if(!instance)
        {
            instance = new FitThreadPool(n_threads, n_gpu);
        }
    }

    static void Destroy()
    {
        if(instance)
        {
            delete instance;
            instance = nullptr;
        }
    }

    // Enqueue a task to the ThreadPool and return a future
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using ReturnType = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = task->get_future();

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks.emplace([task]() {
                (*task)();
            });
        }

        condition.notify_one();

        return result;
    }

    std::map<std::thread::id, bool> threadToIfgpu;
protected:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

#endif /**__FIT_THREAD_POOL__**/