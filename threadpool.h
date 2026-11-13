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
#include <vector>
#include <thread>


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
    Threadsafe_queue(const std::queue<T>& queue) noexcept:
	m_queue{ queue }
    {}

    Threadsafe_queue(std::queue<T>&& queue) noexcept:
	m_queue{ std::move(queue) }
    {}

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


template<typename T>
void
Threadsafe_queue<T>::push(const T& value) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mut };
    m_queue.push(value);

    // Notify wait_and_pop();
    m_cond.notify_one();
}


template<typename T>
void
Threadsafe_queue<T>::push(T&& value) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mut };
    m_queue.push(std::move(value));

    // Notify wait_and_pop();
    m_cond.notify_one();
}


template<typename T>
bool
Threadsafe_queue<T>::try_pop(T& value) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mut };

    // If the queue is empty, return false
    if (m_queue.empty()) {
	return false;
    }

    // Otherwise, pop the value and return true
    value = m_queue.front();
    m_queue.pop();
    return true;
}


template<typename T>
T
Threadsafe_queue<T>::try_pop()
{
    std::lock_guard<std::mutex> lock{ m_mut };

    // If queue is empty, throw exception
    if (m_queue.empty()) {
	throw std::runtime_error{ "try_pop(): Queue is empty" };
    }

    // Otherwise, pop the value and return it
    auto value = m_queue.front();
    m_queue.pop();
    return value;
}


template<typename T>
T
Threadsafe_queue<T>::wait_and_pop() noexcept
{
    std::unique_lock lock{ m_mut };

    // Block until the queue has some value
    m_cond.wait(lock, [this]{ return !m_queue.empty(); });

    // Pop the value and return it
    auto value = m_queue.front();
    m_queue.pop();
    return value;
}


template<typename T>
bool
Threadsafe_queue<T>::empty() const noexcept
{
    std::lock_guard<std::mutex> lock{ m_mut };
    
    if (m_queue.empty()) {
	return true;
    }
    return false;
}


/* ============= Thread_vector ============ */
/**
 * Simple class to wrap std::vector<std::thread>
 * Provides functions to push the threads and
 * automatically joins the threads on destruction
 */

class Thread_vector
{
public:
    Thread_vector()=default;

    /**
     * Copying not allowed
     */
    Thread_vector(const Thread_vector&)=delete;
    Thread_vector& operator=(const Thread_vector&)=delete;

    /**
     * Move constructor and operator
     */
    Thread_vector(Thread_vector&& vector)=default;
    Thread_vector& operator=(Thread_vector&& vector)=default;

    /**
     * Constructor to wrap a simple vector of threads
     */
    Thread_vector(std::vector<std::thread>&& vector) noexcept:
	m_vector{ std::move(vector) }
    {}

    /**
     * Destructor joins the threads
     */
    ~Thread_vector();

    /**
     * Pushing in the vector is move-only as
     * threads can't be copied
     */
    void push_back(std::thread&& thread) noexcept;

private:
    std::vector<std::thread> m_vector{};
};


Thread_vector::~Thread_vector()
{
    // Join the threads
    for (auto& thread : m_vector) {
	if (thread.joinable()) {
	    thread.join();
	}
    }
}


void
Thread_vector::push_back(std::thread&& thread) noexcept
{
    m_vector.push_back(std::move(thread));
}
