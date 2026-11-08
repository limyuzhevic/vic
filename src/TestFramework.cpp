// NLM (熙然) - Neural Learning Machine
// TestFramework implementation

#include "TestFramework.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>

using namespace std;

// Test result structure implementation
string TestResult::toJson() const {
    stringstream ss;
    ss << "{\"test_name\": \"" << testName << "\", \"status\": " << 
        (int)status << ", \"message\": \"" << message << "\", \"execution_time_ms\": " << 
        executionTime.count() << ", \"metrics\": [";
    
    for (size_t i = 0; i < metrics.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "{\"name\": \"" << metrics[i].first << "\", \"value\": " << metrics[i].second;
    }
    
    ss << "], \"errors\": [";
    for (size_t i = 0; i < errorMessages.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << errorMessages[i] << "\"";
    }
    ss << "]}";
    return ss.str();
}

// TestFramework implementation

TestFramework::TestFramework() : runAllTests_(false), verbose_(false), config_() {}

TestFramework::~TestFramework() = default;

void TestFramework::registerTest(shared_ptr<Test> test) {
    tests_[test->getName()] = test;
}

void TestFramework::setTestsToRun(const vector<string>& testNames) {
    if (testNames.empty()) {
        runAllTests_ = true;
        return;
    }
    
    unordered_set<string> testSet(testNames.begin(), testNames.end());
    runAllTests_ = (testSet.find("all") != testSet.end());
    testSet_ = testSet;
}

void TestFramework::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void TestFramework::setConfig(shared_ptr<Config> config) {
    config_ = config;
}

TestResults TestFramework::runAllTests(shared_ptr<Brain> brain, const Config& config) {
    TestResults results;
    
    for (const auto& pair : tests_) {
        const auto& testName = pair.first;
        auto test = pair.second;
        
        // Check if this test should be run
        bool shouldRun = false;
        if (runAllTests_) {
            shouldRun = true;
        } else if (testSet_.empty()) {
            shouldRun = true;  // Default behavior: run all tests
        } else {
            shouldRun = (testSet_.find(testName) != testSet_.end());
        }
        
        if (!shouldRun) {
            TestResult result(testName, TestStatus::SKIPPED);
            result.addMessage("INFO", "Test skipped by command line selection");
            results.addResult(result);
            continue;
        }
        
        // Run the test
        try {
            auto startTime = chrono::high_resolution_clock::now();
            TestResult result = test->execute(brain, config);
            auto endTime = chrono::high_resolution_clock::now();
            
            result.executionTime = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
            results.addResult(result);
            
            if (verbose_) {
                cout << "Test: " << testName << " - Status: " << 
                    (result.status == TestStatus::PASSED ? "PASSED" : 
                     result.status == TestStatus::FAILED ? "FAILED" : 
                     result.status == TestStatus::ERROR ? "ERROR" : "SKIPPED") << endl;
            }
            
        } catch (const exception& e) {
            TestResult result(testName, TestStatus::ERROR);
            result.addMessage("ERROR", string("Exception: ") + e.what());
            results.addResult(result);
            
            if (verbose_) {
                cout << "Test: " << testName << " - ERROR: " << e.what() << endl;
            }
        }
    }
    
    return results;
}

vector<string> TestFramework::getRegisteredTestNames() const {
    vector<string> names;
    names.reserve(tests_.size());
    for (const auto& pair : tests_) {
        names.push_back(pair.first);
    }
    sort(names.begin(), names.end());
    return names;
}
