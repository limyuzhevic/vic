#include "TestFramework.hpp"
#include "brain/Brain.hpp"
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

struct TestFramework::Impl {
    std::unordered_map<std::string, std::unique_ptr<Test>> tests;
    std::unordered_map<std::string, std::string> defaultConfigs;
    
    Impl() = default;
};

TestFramework::TestFramework() : pImpl(std::make_unique<Impl>()) {}
TestFramework::~TestFramework() = default;

void TestFramework::registerTest(std::unique_ptr<Test> test) {
    if (test) {
        pImpl->tests[test->getName()] = std::move(test);
    }
}

std::shared_ptr<Test> TestFramework::getTest(const std::string& name) const {
    auto it = pImpl->tests.find(name);
    if (it != pImpl->tests.end()) {
        return std::shared_ptr<Test>(it->second.get());
    }
    return nullptr;
}

TestResult TestFramework::runTest(const std::string& name, std::shared_ptr<Brain> brain, const TestConfig& config) {
    TestResult result;
    result.testName = name;
    
    auto it = pImpl->tests.find(name);
    if (it == pImpl->tests.end()) {
        result.errorMessage = "Test not found: " + name;
        return result;
    }
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Setup
        if (!it->second->setup(brain, config)) {
            result.errorMessage = "Test setup failed";
            return result;
        }
        
        // Record initial state
        if (config.recordMetrics) {
            if (auto* region = brain->getRegion(RegionId(1))) {
                std::vector<float> weights;
                for (const auto& syn : region->getSynapses()) {
                    weights.push_back(syn->getWeight());
                }
                if (!weights.empty()) {
                    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
                    result.initialWeightMean = sum / weights.size();
                    result.neuronsTested = weights.size();
                }
            }
            result.spikesGenerated = brain->getTotalSpikeCount();
        }
        
        // Run test
        result = it->second->run(brain, config);
        
        // Record final state
        if (config.recordMetrics) {
            if (auto* region = brain->getRegion(RegionId(1))) {
                std::vector<float> weights;
                for (const auto& syn : region->getSynapses()) {
                    weights.push_back(syn->getWeight());
                }
                if (!weights.empty()) {
                    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
                    result.finalWeightMean = sum / weights.size();
                    result.weightChange = result.finalWeightMean - result.initialWeightMean;
                }
            }
            result.spikesGenerated = brain->getTotalSpikeCount() - result.spikesGenerated;
        }
        
        // Teardown
        it->second->teardown(brain, config);
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    // Record execution time
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Add to results
    results_.push_back(result);
    
    return result;
}

std::vector<TestResult> TestFramework::runBatch(const std::vector<std::string>& testNames, std::shared_ptr<Brain> brain, const TestConfig& config) {
    std::vector<TestResult> batchResults;
    
    for (const auto& testName : testNames) {
        batchResults.push_back(runTest(testName, brain, config));
    }
    
    return batchResults;
}

void TestFramework::addDefaultConfig(const std::string& key, const std::string& value) {
    pImpl->defaultConfigs[key] = value;
}

void TestFramework::setConfigValue(const std::string& key, const std::string& value) {
    pImpl->defaultConfigs[key] = value;
}

std::string TestFramework::getConfigValue(const std::string& key) const {
    auto it = pImpl->defaultConfigs.find(key);
    if (it != pImpl->defaultConfigs.end()) {
        return it->second;
    }
    return "";
}

double TestFramework::getAveragePassRate() const {
    if (results_.empty()) return 0.0;
    
    int passed = getPassedTestsCount();
    return static_cast<double>(passed) / results_.size() * 100.0;
}

size_t TestFramework::getPassedTestsCount() const {
    return std::count_if(results_.begin(), results_.end(),
        [](const TestResult& result) { return result.passed; });
}

} // namespace nlm