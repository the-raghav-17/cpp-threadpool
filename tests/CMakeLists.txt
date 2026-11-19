include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
    GIT_TAG         v3.16.1
)

FetchContent_MakeAvailable(Catch2)

add_executable(test_threadsafe_queue test_threadsafe_queue.cpp)
target_link_libraries(test_threadsafe_queue PRIVATE Catch2::Catch2WithMain threadpool)
