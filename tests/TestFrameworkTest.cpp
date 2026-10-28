// tests/TestFrameworkTest.cpp
#pragma once

#include "core/TestFramework.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace nlm {

class MockTest : public Test {
public:
    const std::string& getName() const override { static const std::string name = "MockTest"; return name; }
    const std::string& getDescription() const override { 
        static const std::string desc = "Mock test for framework testing"; 
        return desc; 
    }
    
    TestResult run(std::shared_ptr<Brain> brain, const TestConfig& config) override {
        TestResult result;
        result.testName = getName();
        result.passed = true;
        result.metrics["steps_executed"] = static_cast<double>(config.steps);
        return result;
    }
};

void runFrameworkTests() {
    TestFramework framework;
    
    // Register test
    framework.registerTest(std::make_unique<MockTest>());
    
    // Test 1: Get test by name
    auto test = framework.getTest("MockTest");
    assert(test != nullptr);
    std::cout << "✓ Test registration and retrieval working" << std::endl;
    
    // Test 2: Run test
    auto config = TestConfig("MockTest", "Mock test config");
    config.steps = 50;
    config.currentLevel = 10.0f;
    
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    brain->initialize();
    
    auto result = framework.runTest("MockTest", brain, config);
    assert(result.passed);
    assert(result.metrics["steps_executed"] == 50.0);
    std::cout << "✓ Test execution working" << std::endl;
    
    // Test 3: Configuration management
    framework.addDefaultConfig("test_key", "test_value");
    assert(framework.getConfigValue("test_key") == "test_value");
    std::cout << "✓ Configuration management working" << std::endl;
    
    std::cout << "All framework tests passed!" << std::endl;
}

} // namespace nlm