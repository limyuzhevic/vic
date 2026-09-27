// Test Registry for NLM Brain Simulation
// Test discovery, registration, and management system

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>
#include <algorithm>

namespace nlm {
namespace test {

// Test information for registration
struct TestInfo {
    std::string name;
    std::string testClass;
    std::string testSuite;
    std::vector<std::string> tags;
    std::string description;
    std::function<void()> testFunction;
    int order;
    bool enabled;
    std::chrono::nanoseconds timeout;
    
    TestInfo(const std::string& name, const std::string& testClass,
             const std::string& testSuite, const std::string& description,
             std::function<void()> testFunction, int order = 0,
             bool enabled = true, const std::vector<std::string>& tags = {},
             std::chrono::nanoseconds timeout = std::chrono::nanoseconds(30000000000)) // 30 seconds default
        : name(name), testClass(testClass), testSuite(testSuite),
          description(description), testFunction(testFunction), order(order),
          enabled(enabled), tags(tags), timeout(timeout) {}
};

// Test registration and discovery system
class TestRegistry {
private:
    std::vector<TestInfo> tests;
    std::map<std::string, std::vector<TestInfo*>> testsByTag;
    std::mutex mutex;
    
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }
    
    // Register a test with metadata
    void registerTest(const std::string& name,
                     const std::string& testClass,
                     const std::string& testSuite,
                     const std::string& description,
                     std::function<void()> testFunction,
                     int order = 0,
                     bool enabled = true,
                     const std::vector<std::string>& tags = {},
                     std::chrono::nanoseconds timeout = std::chrono::nanoseconds(30000000000)) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto testInfo = TestInfo(name, testClass, testSuite, description,
                                 testFunction, order, enabled, tags, timeout);
        
        tests.push_back(testInfo);
        
        for (const auto& tag : tags) {
            testsByTag[tag].push_back(&tests.back());
        }
    }
    
    // Get all registered tests
    const std::vector<TestInfo>& getAllTests() const {
        return tests;
    }
    
    // Get tests by tag
    std::vector<TestInfo*> getTestsByTag(const std::string& tag) {
        std::lock_guard<std::mutex> lock(mutex);
        return testsByTag[tag];
    }
    
    // Get enabled tests (not disabled)
    std::vector<TestInfo*> getEnabledTests() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<TestInfo*> enabledTests;
        for (auto& test : tests) {
            if (test.enabled) {
                enabledTests.push_back(&test);
            }
        }
        return enabledTests;
    }
    
    // Find a test by suite, class, and name
    TestInfo* findTest(const std::string& testSuite, const std::string& testClass,
                      const std::string& testName) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto& test : tests) {
            if (test.testSuite == testSuite && test.testClass == testClass && test.name == testName) {
                return &test;
            }
        }
        return nullptr;
    }
    
    // Enable/disable a test
    void enableTest(const std::string& testSuite, const std::string& testClass,
                   const std::string& testName, bool enabled = true) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto& test : tests) {
            if (test.testSuite == testSuite && test.testClass == testClass && test.name == testName) {
                test.enabled = enabled;
                return;
            }
        }
    }
    
    void disableTest(const std::string& testSuite, const std::string& testClass,
                    const std::string& testName) {
        enableTest(testSuite, testClass, testName, false);
    }
    
    // Clear all tests
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        tests.clear();
        testsByTag.clear();
    }
    
    // Get test count
    size_t count() const {
        return tests.size();
    }
    
    // Sort tests by order and name
    void sortTests() {
        std::lock_guard<std::mutex> lock(mutex);
        std::sort(tests.begin(), tests.end(),
                 [](const TestInfo& a, const TestInfo& b) {
                     if (a.order != b.order) {
                         return a.order < b.order;
                     }
                     return a.name < b.name;
                 });
    }
    
    // Filter tests by tags
    std::vector<TestInfo*> filterByTags(const std::vector<std::string>& tags) {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::vector<TestInfo*> filteredTests;
        for (auto& test : tests) {
            if (test.enabled) {
                bool matches = true;
                for (const auto& tag : tags) {
                    if (std::find(test.tags.begin(), test.tags.end(), tag) != test.tags.end()) {
                        matches = true;
                        break;
                    }
                }
                if (matches || tags.empty()) {
                    filteredTests.push_back(&test);
                }
            }
        }
        return filteredTests;
    }
    
    // Get tests by dependency
    std::vector<TestInfo*> getTestsWithDependencies() {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::vector<TestInfo*> dependentTests;
        for (auto& test : tests) {
            if (test.enabled) {
                dependentTests.push_back(&test);
            }
        }
        return dependentTests;
    }
    
    // Export test information to string
    std::string exportToString() const {
        std::stringstream ss;
        ss << "Registered Tests (" << tests.size() << "):\n";
        for (const auto& test : tests) {
            ss << "  " << test.order << ". " << test.testSuite << "." << test.testClass << "." << test.name << "\n";
            ss << "     Description: " << test.description << "\n";
            ss << "     Tags: ";
            for (size_t i = 0; i < test.tags.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << test.tags[i];
            }
            ss << "\n";
            ss << "     Enabled: " << (test.enabled ? "Yes" : "No") << "\n";
            ss << "     Timeout: " << std::chrono::duration_cast<std::chrono::milliseconds>(test.timeout).count() << "ms\n";
        }
        return ss.str();
    }
    
    // Import tests from file
    bool importFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        clear();
        
        std::string line;
        TestInfo currentTest("", "", "", "", nullptr);
        std::string currentSection;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line.find("Test:") == 0) {
                // Save previous test
                if (currentTest.testFunction) {
                    registerTest(currentTest.name, currentTest.testClass, currentTest.testSuite,
                                currentTest.description, currentTest.testFunction,
                                currentTest.order, currentTest.enabled, currentTest.tags, currentTest.timeout);
                }
                
                // Parse new test
                size_t colonPos = line.find(":");
                if (colonPos != std::string::npos) {
                    currentTest.name = line.substr(colonPos + 1);
                    currentTest.testFunction = nullptr;
                    currentTest.tags.clear();
                }
            } else if (line.find("Class:") == 0) {
                currentTest.testClass = line.substr(6);
            } else if (line.find("Suite:") == 0) {
                currentTest.testSuite = line.substr(6);
            } else if (line.find("Description:") == 0) {
                currentTest.description = line.substr(11);
            } else if (line.find("Enabled:") == 0) {
                currentTest.enabled = (line.substr(8) == "true" || line.substr(8) == "yes");
            } else if (line.find("Timeout:") == 0) {
                try {
                    int timeoutMs = std::stoi(line.substr(8));
                    currentTest.timeout = std::chrono::milliseconds(timeoutMs);
                } catch (...) {}
            } else if (line.find("Tag:") == 0) {
                currentTest.tags.push_back(line.substr(4));
            }
        }
        
        // Save last test
        if (currentTest.testFunction) {
            registerTest(currentTest.name, currentTest.testClass, currentTest.testSuite,
                        currentTest.description, currentTest.testFunction,
                        currentTest.order, currentTest.enabled, currentTest.tags, currentTest.timeout);
        }
        
        file.close();
        return true;
    }
    
    // Export tests to file
    bool exportToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Test Registry Export\n";
        file << "# Format: Test: <name>\n";
        file << "#        Class: <class>\n";
        file << "#        Suite: <suite>\n";
        file << "#        Description: <description>\n";
        file << "#        Enabled: <true/false>\n";
        file << "#        Timeout: <milliseconds>\n";
        file << "#        Tag: <tag>\n";
        file << "\n";
        
        for (const auto& test : tests) {
            file << "Test: " << test.name << "\n";
            file << "Class: " << test.testClass << "\n";
            file << "Suite: " << test.testSuite << "\n";
            file << "Description: " << test.description << "\n";
            file << "Enabled: " << (test.enabled ? "true" : "false") << "\n";
            file << "Timeout: " << std::chrono::duration_cast<std::chrono::milliseconds>(test.timeout).count() << "\n";
            for (const auto& tag : test.tags) {
                file << "Tag: " << tag << "\n";
            }
            file << "\n";
        }
        
        file.close();
        return true;
    }
};

} // namespace test
} // namespace nlm
