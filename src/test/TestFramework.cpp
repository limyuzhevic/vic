// Test Framework Implementation
// This file implements the test framework defined in TestFramework.hpp

#include "TestFramework.hpp"
#include <algorithm>

namespace nlm {
namespace test {

// Test runner implementation
TestRunner& getTestRunner() {
    static TestRunner runner;
    return runner;
}

// Helper function to add test cases
void addTest(TestRunner& runner, const std::string& suiteName, 
             const std::string& testName, std::function<void()> testFunc) {
    static TestSuite* currentSuite = nullptr;
    
    if (!currentSuite) {
        currentSuite = new TestSuite("default");
        runner.addSuite(std::unique_ptr<TestSuite>(currentSuite));
    }
    
    currentSuite->addTestCase(std::make_unique<TemplateTestCase>(testName, suiteName, testFunc));
}

// Template test case implementation
TemplateTestCase::TemplateTestCase(const std::string& name, const std::string& suite, 
                                 const TestFunction& testFunc)
    : name_(name), suite_(suite), testFunc_(testFunc) {}

std::string TemplateTestCase::getName() const {
    return name_;
}

std::string TemplateTestCase::getSuite() const {
    return suite_;
}

void TemplateTestCase::run() {
    testFunc_();
}

// Test suite implementation
TestSuite::TestSuite(const std::string& name) : name_(name) {}

std::string TestSuite::getName() const {
    return name_;
}

void TestSuite::addTestCase(std::unique_ptr<TestCase> testCase) {
    testCases_.push_back(std::move(testCase));
}

void TestSuite::run() {
    for (auto& testCase : testCases_) {
        testCase->execute();
    }
}

const std::vector<std::unique_ptr<TestCase>>& TestSuite::getTestCases() const {
    return testCases_;
}

} // namespace test
} // namespace nlm
