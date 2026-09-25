// TestSuite.cpp
// Implementation of test suite management for NLM

#include "TestSuite.hpp"
#include <iostream>

namespace nlm {

TestSuite::TestSuite(const std::string& name) : name_(name), passed_(0), failed_(0) {}

TestSuite::~TestSuite() {
    for (auto* test : tests_) {
        delete test;
    }
    tests_.clear();
}

void TestSuite::addTest(const std::string& name, std::function<bool()> testFunc) {
    tests_.push_back(new TestCaseImpl(name, testFunc));
}

void TestSuite::runAll() {
    std::cout << "Running test suite: " << name_ << std::endl;
    std::cout << "Total tests: " << tests_.size() << std::endl;
    
    for (auto* test : tests_) {
        try {
            if (test->run()) {
                passed_++;
                std::cout << "✓ " << test->getName() << std::endl;
            } else {
                failed_++;
                std::cout << "✗ " << test->getName() << ": test failed" << std::endl;
            }
        } catch (const std::exception& e) {
            failed_++;
            std::cout << "✗ " << test->getName() << ": " << e.what() << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Test results: " << passed_ << " passed, " << failed_ << " failed" << std::endl;
}

TestSuite::TestCaseImpl::TestCaseImpl(const std::string& name, std::function<bool()> testFunc)
    : name_(name), testFunc_(testFunc) {}

bool TestSuite::TestCaseImpl::run() {
    return testFunc_();
}

}} // namespace nlm