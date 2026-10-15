#pragma once

#include "TestBase.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace nlm {

class TestRegistry {
public:
    static TestRegistry& getInstance();
    
    // Register a test
    void registerTest(std::unique_ptr<TestBase> test);
    
    // Get tests
    std::vector<std::unique_ptr<TestBase>> getAllTests() const;
    std::vector<std::unique_ptr<TestBase>> getTestsByCategory(const std::string& category) const;
    std::vector<std::unique_ptr<TestBase>> getTestsByTag(const std::string& tag) const;
    std::vector<std::unique_ptr<TestBase>> getTestsByPhase(const std::string& phase) const;
    
    // Get single test
    TestBase* getTest(const std::string& name);
    std::vector<std::string> getAllTestNames() const;
    std::vector<std::string> getCategories() const;
    std::vector<std::string> getTags() const;
    std::vector<std::string> getPhases() const;
    
    // Test filtering and selection
    std::vector<std::unique_ptr<TestBase>> filterTests(
        const std::vector<std::string>& categories = {},
        const std::vector<std::string>& tags = {},
        const std::vector<std::string>& phases = {},
        const std::vector<std::string>& exclude = {}) const;
    
    // Utility methods
    size_t getTestCount() const;
    bool hasTest(const std::string& name) const;
    void clear();
    
private:
    TestRegistry();
    ~TestRegistry() = default;
    TestRegistry(const TestRegistry&) = delete;
    TestRegistry& operator=(const TestRegistry&) = delete;
    
    std::vector<std::unique_ptr<TestBase>> tests_;
    std::unordered_map<std::string, std::unique_ptr<TestBase>> testMap_;
    mutable std::mutex mutex_;
    
    std::vector<std::string> categories_;
    std::vector<std::string> tags_;
    std::vector<std::string> phases_;
};

} // namespace nlm
