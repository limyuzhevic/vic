// Test framework implementation
#include "TestFramework.hpp"
#include <iostream>
#include <stdexcept>

namespace nlm {

struct TestFramework::Impl {
    std::vector<TestConfig> testConfigs;
};

TestFramework::TestFramework() : pImpl(std::make_unique<Impl>()) {}

void TestFramework::registerTest(const TestConfig& config) {
    pImpl->testConfigs.push_back(config);
}

void TestFramework::runAllTests() {
    std::cout << "Running " << pImpl->testConfigs.size() << " tests..." << std::endl;
    
    for (size_t i = 0; i < pImpl->testConfigs.size(); ++i) {
        const auto& config = pImpl->testConfigs[i];
        std::cout << "Test " << i+1 << ": " << config.name << std::endl;
        
        if (config.predicate) {
            bool result = config.predicate();
            std::cout << "  Result: " << (result ? "PASS" : "FAIL") << std::endl;
        }
    }
}

std::vector<TestConfig> TestFramework::getTestConfigs() const {
    return pImpl->testConfigs;
}

} // namespace nlm