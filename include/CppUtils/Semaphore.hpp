///
/// \file 				Semaphore.hpp
/// \author 			Geoffrey Hunter (www.mbedded.ninja) <gbmhunter@gmail.com>
/// \edited             n/a
/// \created			2017-09-22
/// \last-modified		2017-09-22
/// \brief 				Contains the Semaphore class.
/// \details
///		See README.md in root dir for more info.

#ifndef MN_CPP_UTILS_SEMAPHORE_H_
#define MN_CPP_UTILS_SEMAPHORE_H_

// System includes
#include <mutex>
#include <condition_variable>

namespace mn {
    namespace CppUtils {

        /// \details    This class is neither movable nor copyable. Use a smart pointer if you need copy/move like
        ///             capabilities.
        class Semaphore {
        public:

            Semaphore() : count_(0) {

            }

            void Notify() {
                std::unique_lock<std::mutex> lock(mutex_);
                count_++;
                cv_.notify_one();
            }

            /// \brief      Blocks indefinitely until Notify() is called.
            void Wait() {
                std::unique_lock<std::mutex> lock(mutex_);
                while(count_ == 0) {
                    cv_.wait(lock);
                }
                count_--;
            }

            /// \brief      Blocks until either Notify() is called, or a timeout occurs.
            /// \returns    Returns true if Notify() was called before timeout occurred, otherwise false.
            bool TryWait(std::chrono::milliseconds timeout) {
                std::unique_lock<std::mutex> lock(mutex_);
                if(!cv_.wait_for(lock, timeout, [&] {
                    return count_ != 0;
                })) {
                    return false;
                }
                count_--;
                return true;
            }

        private:
            uint32_t count_;
            std::mutex mutex_;
            std::condition_variable cv_;

        };
    } // namespace CppUtils
} // namespace mn


#endif // MN_CPP_UTILS_SEMAPHORE_H_
