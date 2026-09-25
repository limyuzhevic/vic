// TestSuite.hpp
// Test suite management for NLM

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

namespace nlm {

class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual void run() = 0;
    virtual bool setup() { return true; }
    virtual void teardown() {}
};

class TestSuite {
public:
    TestSuite(const std::string& name);
    ~TestSuite();
    
    void addTest(const std::string& name, std::function<bool()> testFunc);
    void runAll();
    size_t getPassedCount() const { return passed_; }
    size_t getFailedCount() const { return failed_; }
    const std::string& getName() const { return name_; }
    
private:
    class TestCaseImpl : public TestCase {
    public:
        TestCaseImpl(const std::string& name, std::function<bool()> testFunc);
        std::string getName() const override { return name_; }
        void run() override;
        bool setup() override { return true; }
        void teardown() override {}
    
    private:
        std::string name_;
        std::function<bool()> testFunc_;
    };
    
    std::string name_;
    std::vector<TestCaseImpl*> tests_;
    size_t passed_;
    size_t failed_;
};

}} // namespace nlm