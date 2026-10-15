#include "TestRegistry.hpp"
#include <algorithm>

namespace nlm {

TestRegistry& TestRegistry::getInstance() {
    static TestRegistry instance;
    return instance;
}

TestRegistry::TestRegistry() {
}

void TestRegistry::registerTest(std::unique_ptr<TestBase> test) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!test) return;
    
    const auto& metadata = test->getMetadata();
    
    // Add to main collection
    tests_.push_back(std::move(test));
    
    // Add to map for quick lookup
    testMap_[metadata.name] = tests_.back().get();
    
    // Update categories, tags, and phases
    if (!metadata.category.empty()) {
        categories_.push_back(metadata.category);
    }
    
    for (const auto& tag : metadata.tags) {
        if (std::find(tags_.begin(), tags_.end(), tag) == tags_.end()) {
            tags_.push_back(tag);
        }
    }
    
    if (!metadata.phase.empty()) {
        phases_.push_back(metadata.phase);
    }
}

std::vector<std::unique_ptr<TestBase>> TestRegistry::getAllTests() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tests_;
}

std::vector<std::unique_ptr<TestBase>> TestRegistry::getTestsByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::unique_ptr<TestBase>> result;
    
    for (const auto& test : tests_) {
        if (test->getMetadata().category == category) {
            result.push_back(std::make_unique<TestBase>(*test));
        }
    }
    
    return result;
}

std::vector<std::unique_ptr<TestBase>> TestRegistry::getTestsByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::unique_ptr<TestBase>> result;
    
    for (const auto& test : tests_) {
        const auto& metadata = test->getMetadata();
        if (std::find(metadata.tags.begin(), metadata.tags.end(), tag) != metadata.tags.end()) {
            result.push_back(std::make_unique<TestBase>(*test));
        }
    }
    
    return result;
}

std::vector<std::unique_ptr<TestBase>> TestRegistry::getTestsByPhase(const std::string& phase) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::unique_ptr<TestBase>> result;
    
    for (const auto& test : tests_) {
        if (test->getMetadata().phase == phase) {
            result.push_back(std::make_unique<TestBase>(*test));
        }
    }
    
    return result;
}

TestBase* TestRegistry::getTest(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = testMap_.find(name);
    return it != testMap_.end() ? it->second : nullptr;
}

std::vector<std::string> TestRegistry::getAllTestNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(testMap_.size());
    
    for (const auto& pair : testMap_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> TestRegistry::getCategories() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> uniqueCategories;
    std::unordered_set<std::string> seen;
    
    for (const auto& test : tests_) {
        const auto& category = test->getMetadata().category;
        if (!category.empty() && seen.find(category) == seen.end()) {
            uniqueCategories.push_back(category);
            seen.insert(category);
        }
    }
    
    return uniqueCategories;
}

std::vector<std::string> TestRegistry::getTags() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tags_;
}

std::vector<std::string> TestRegistry::getPhases() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> uniquePhases;
    std::unordered_set<std::string> seen;
    
    for (const auto& test : tests_) {
        const auto& phase = test->getMetadata().phase;
        if (!phase.empty() && seen.find(phase) == seen.end()) {
            uniquePhases.push_back(phase);
            seen.insert(phase);
        }
    }
    
    return uniquePhases;
}

std::vector<std::unique_ptr<TestBase>> TestRegistry::filterTests(
    const std::vector<std::string>& categories,
    const std::vector<std::string>& tags,
    const std::vector<std::string>& phases,
    const std::vector<std::string>& exclude) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::unique_ptr<TestBase>> result;
    
    for (const auto& test : tests_) {
        const auto& metadata = test->getMetadata();
        
        // Check category filter
        bool categoryMatch = categories.empty() || 
            (std::find(categories.begin(), categories.end(), metadata.category) != categories.end());
        
        // Check tag filter
        bool tagMatch = tags.empty();
        if (!tagMatch) {
            for (const auto& tag : tags) {
                if (std::find(metadata.tags.begin(), metadata.tags.end(), tag) != metadata.tags.end()) {
                    tagMatch = true;
                    break;
                }
            }
        }
        
        // Check phase filter
        bool phaseMatch = phases.empty() || 
            (std::find(phases.begin(), phases.end(), metadata.phase) != phases.end());
        
        // Check exclude list
        bool excluded = std::find(exclude.begin(), exclude.end(), metadata.name) != exclude.end();
        
        if (categoryMatch && tagMatch && phaseMatch && !excluded) {
            result.push_back(std::make_unique<TestBase>(*test));
        }
    }
    
    return result;
}

size_t TestRegistry::getTestCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tests_.size();
}

bool TestRegistry::hasTest(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return testMap_.find(name) != testMap_.end();
}

void TestRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    tests_.clear();
    testMap_.clear();
    categories_.clear();
    tags_.clear();
    phases_.clear();
}

} // namespace nlm
