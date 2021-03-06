///
/// \file 				MsgQueueTests.cpp
/// \author 			Geoffrey Hunter (www.mbedded.ninja) <gbmhunter@gmail.com>
/// \edited             n/a
/// \created			2017-10-24
/// \last-modified		2017-10-24
/// \brief 				Contains tests for the MsgQueue class.
/// \details
///		See README.md in root dir for more info.

// System includes
#include <future>
#include <iostream>
#include <memory>
#include <thread>

// 3rd party includes

#include "gtest/gtest.h"

// User includes
#include "CppUtils/MsgQueue.hpp"

using namespace mn::CppUtils::MsgQueue;

namespace {

    class MsgQueueTests : public ::testing::Test {
    protected:
        MsgQueueTests() {}
        virtual ~MsgQueueTests() {}
    };


    class Thread1 {
    public:
        Thread1() {
            thread_ = std::thread(&Thread1::Process, this);
        }

        ~Thread1() {
            if(thread_.joinable()) {
                queue_.Push(TxMsg("EXIT"));
                thread_.join();
            }
        }

        void SetData(std::string data) {
            auto dataOnHeap = std::make_shared<std::string>(data);
            queue_.Push(TxMsg("SET_DATA", dataOnHeap));
        }

        std::string GetData() {
            TxMsg msg("GET_DATA", ReturnType::RETURN_DATA);
            queue_.Push(msg);
            auto retVal = msg.WaitForData();
            return *std::static_pointer_cast<std::string>(retVal);
        }

    private:

        void Process() {

            RxMsg msg;

            // This loop can be broken by sending the "EXIT" msg!
            while(true) {
                queue_.Pop(msg);

                //==============================================//
                //============= MSG PROCESSING LOOP ============//
                //==============================================//
                if(msg.GetId() == "SET_DATA") {
                    auto data = std::static_pointer_cast<std::string>(msg.GetData()); // Cast back to exact data type
                    data_ = *data;
                } else if(msg.GetId() == "GET_DATA") {
                    auto retData = std::make_shared<std::string>(data_);
                    msg.ReturnData(retData);
                    break;
                } else if(msg.GetId() == "EXIT") {
                    // Break from infinite while loop, which will mean that
                    // this function will return and then thread.join() will
                    // return
                    break;
                } else
                    throw std::runtime_error("Command name not recognized.");
            }
        }

        std::thread thread_;
        MsgQueue queue_;

        std::string data_;
    };

    TEST_F(MsgQueueTests, BasicTest) {
        Thread1 thread1;
        thread1.SetData("Hello");
        auto returnedData = thread1.GetData();
        EXPECT_EQ("Hello", returnedData);
    }
}  // namespace