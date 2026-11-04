/**
 * Copyright: Raghav Sharma (github.com/the-raghav-17)
 * License: GNU GPLv3
 */

/**
 * My own single-header custom threadpool
 */


#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>


/* ============= Threadsafe_queue ============ */

/**
 * Simple wrapper class around Standard Library
 * Queue to make it threadsafe for the purpose
 * of the threadpool
 */

template<typename T>
class Threadsafe_queue
{
public:
    Threadsafe_queue()=default;

    /**
     * Constructors to wrap a simple queue
     */
    Threadsafe_queue(const std::queue<T>&) noexcept;
    Threadsafe_queue(std::queue<T>&&) noexcept;

    /**
     * Copies the value
     */
    void push(const T&) noexcept;

    /**
     * Moves the value
     */
    void push(T&&) noexcept;

    /**
     * Pop the value if it exists and
     * return true. Otherwise return false
     */
    bool try_pop(T&) noexcept;

    /**
     * Pop the value if it exists or
     * throw an instance of runtime_error
     */
    T try_pop();

    /**
     * If queue isn't empty, immediately pop
     * the value and return it. Otherwise, wait
     * for the queue to fill with some value first,
     * and then pop and return it
     */
    T wait_and_pop() noexcept;

    /**
     * Return true if queue is
     * empty. Otherwise false
     */
    bool empty() const noexcept;

private:
    std::queue<T>           m_queue{};
    std::mutex              m_mut{};
    std::condition_variable m_cond{};
};
