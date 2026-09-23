#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <cassert>
#include <iostream>

namespace nlm {

class TestBase {
public:
    virtual ~TestBase() = default;
    virtual std::string getName() const = 0;
    virtual void run() = 0;
};

class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name), passed_(0), failed_(0) {}
    
    void addTest(std::unique_ptr<TestBase> test) {
        tests_.push_back(std::move(test));
    }
    
    void run() {
        std::cout << "Running test suite: " << name_ << std::endl;
        std::cout << "=" << std::string(name_.size() + 22, '=') << std::endl;
        
        for (size_t i = 0; i < tests_.size(); ++i) {
            std::cout << "Test " << (i + 1) << "/" << tests_.size() << ": "
                      << tests_[i]->getName() << "... ";
            
            try {
                tests_[i]->run();
                std::cout << "PASSED" << std::endl;
                passed_++;
            } catch (const std::exception& e) {
                std::cout << "FAILED: " << e.what() << std::endl;
                failed_++;
            } catch (...) {
                std::cout << "FAILED: Unknown error" << std::endl;
                failed_++;
            }
        }
        
        std::cout << std::endl;
        std::cout << "Test suite summary:" << std::endl;
        std::cout << "  Passed: " << passed_ << std::endl;
        std::cout << "  Failed: " << failed_ << std::endl;
        std::cout << "  Total:  " << tests_.size() << std::endl;
        
        if (failed_ > 0) {
            throw std::runtime_error("Test suite failed");
        }
    }
    
    size_t getPassedCount() const { return passed_; }
    size_t getFailedCount() const { return failed_; }
    size_t getTotalCount() const { return tests_.size(); }
    
private:
    std::string name_;
    std::vector<std::unique_ptr<TestBase>> tests_;
    size_t passed_;
    size_t failed_;
};

} // namespace nlm