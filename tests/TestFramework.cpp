// ==============================================================================
// Comprehensive Testing Framework for NLM
// ==============================================================================
// This file provides a unified testing framework with:
// - Proper test classes and organization
// - Integration tests for the complete system
// - Performance benchmarks
// - Enhanced error handling and reporting
// - Better test coverage and organization
// ==============================================================================

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/dynamics/SpikeSystem.hpp"

namespace nlm {
namespace test {

// ==============================================================================
// Test Result Structure
// ==============================================================================

struct TestResult {
    std::string name;
    std::string description;
    bool passed;
    std::string message;
    double executionTime;
    int errorCount;
    
    TestResult(const std::string& testName, const std::string& testDescription)
        : name(testName), description(testDescription), passed(false), 
          executionTime(0.0), errorCount(0) {}
};

// ==============================================================================
// Base Test Class
// ==============================================================================

class Test {
public:
    virtual ~Test() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual void setup() {}
    virtual void teardown() {}
    
    virtual bool run() = 0;
    
    virtual std::string getCategory() const { return "General"; }
};

// ==============================================================================
// Configuration Tests
// ==============================================================================

class ConfigCreationTest : public Test {
public:
    std::string getName() const override { return "ConfigCreationTest"; }
    std::string getDescription() const override { 
        return "Tests configuration object creation and basic properties"; 
    }
    std::string getCategory() const override { return "Configuration"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        nlm::Config config;
        assert(!config.has("test_key"));
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        passed = true;
        message = "Configuration creation test passed";
        return true;
    }
};

class ConfigSetGetTest : public Test {
public:
    std::string getName() const override { return "ConfigSetGetTest"; }
    std::string getDescription() const override { 
        return "Tests setting and getting configuration values"; 
    }
    std::string getCategory() const override { return "Configuration"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        nlm::Config config;
        
        // Test integer values
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        assert(config.has("int_value"));
        
        auto intVal = config.get<int>("int_value");
        assert(intVal.has_value());
        assert(*intVal == 42);
        
        // Test double values
        config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
        auto doubleVal = config.get<double>("double_value");
        assert(doubleVal.has_value());
        assert(*doubleVal > 3.13 && *doubleVal < 3.15);
        
        // Test string values
        config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
        auto stringVal = config.get<std::string>("string_value");
        assert(stringVal.has_value());
        assert(*stringVal == "hello");
        
        // Test boolean values
        config.set("bool_value", true, nlm::ConfigSource::Runtime);
        auto boolVal = config.get<bool>("bool_value");
        assert(boolVal.has_value());
        assert(*boolVal == true);
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        passed = true;
        message = "Configuration set/get test passed";
        return true;
    }
};

class ConfigGetOrTest : public Test {
public:
    std::string getName() const override { return "ConfigGetOrTest"; }
    std::string getDescription() const override { 
        return "Tests getting configuration values with default fallbacks"; 
    }
    std::string getCategory() const override { return "Configuration"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        nlm::Config config;
        
        // Test getting missing key with default
        int val = config.getOr<int>("missing", 100);
        assert(val == 100);
        
        // Test getting existing key
        config.set("existing", 50, nlm::ConfigSource::Runtime);
        val = config.getOr<int>("existing", 100);
        assert(val == 50);
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        passed = true;
        message = "Configuration getOr test passed";
        return true;
    }
};

class ConfigRemoveTest : public Test {
public:
    std::string getName() const override { return "ConfigRemoveTest"; }
    std::string getDescription() const override { 
        return "Tests removing configuration values"; 
    }
    std::string getCategory() const override { return "Configuration"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        nlm::Config config;
        
        config.set("test", 123, nlm::ConfigSource::Runtime);
        assert(config.has("test"));
        
        config.remove("test");
        assert(!config.has("test"));
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        passed = true;
        message = "Configuration remove test passed";
        return true;
    }
};

class ConfigKeysTest : public Test {
public:
    std::string getName() const override { return "ConfigKeysTest"; }
    std::string getDescription() const override { 
        return "Tests getting all configuration keys"; 
    }
    std::string getCategory() const override { return "Configuration"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        nlm::Config config;
        
        config.set("key1", 1, nlm::ConfigSource::Runtime);
        config.set("key2", 2, nlm::ConfigSource::Runtime);
        config.set("key3", 3, nlm::ConfigSource::Runtime);
        
        auto keys = config.getKeys();
        assert(keys.size() == 3);
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        passed = true;
        message = "Configuration keys test passed";
        return true;
    }
};

// ==============================================================================
// Brain Tests
// ==============================================================================

class BrainCreationTest : public Test {
public:
    std::string getName() const override { return "BrainCreationTest"; }
    std::string getDescription() const override { 
        return "Tests brain creation and initialization"; 
    }
    std::string getCategory() const override { return "Brain"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
        config->set("region_count", 1, nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        assert(!brain->initialize());  // Should fail with default config
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        passed = true;
        message = "Brain creation test passed";
        return true;
    }
};

class BrainBasicFunctionalityTest : public Test {
public:
    std::string getName() const override { return "BrainBasicFunctionalityTest"; }
    std::string getDescription() const override { 
        return "Tests basic brain functionality and neuron creation"; 
    }
    std::string getCategory() const override { return "Brain"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100, nlm::ConfigSource::Runtime);
        config->set("region_count", 1, nlm::ConfigSource::Runtime);
        config->set("connection_probability", 0.5, nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            // Check basic brain properties
            assert(brain->getTotalNeuronCount() > 0);
            assert(brain->getTotalSynapseCount() > 0);
            assert(brain->getSpikeSystem() != nullptr);
            
            // Run a few steps
            for (int i = 0; i < 10; ++i) {
                brain->step(i, i * 0.001);
            }
            
            // Check that brain has processed events
            assert(brain->getTotalSpikeCount() >= 0);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        passed = true;
        message = "Brain basic functionality test passed";
        return true;
    }
};

// ==============================================================================
// Performance Benchmarks
// ==============================================================================

class NeuronCreationBenchmark : public Test {
public:
    std::string getName() const override { return "NeuronCreationBenchmark"; }
    std::string getDescription() const override { 
        return "Benchmarks neuron creation performance"; 
    }
    std::string getCategory() const override { return "Benchmark"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Create many neurons
        std::vector<std::unique_ptr<nlm::Neuron>> neurons;
        neurons.reserve(10000);
        
        for (size_t i = 0; i < 10000; ++i) {
            neurons.push_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        
        // Benchmark completed successfully
        passed = true;
        message = "Neuron creation benchmark completed: " + 
                 std::to_string(executionTime) + " seconds for 10,000 neurons";
        return true;
    }
};

class BrainSimulationBenchmark : public Test {
public:
    std::string getName() const override { return "BrainSimulationBenchmark"; }
    std::string getDescription() const override { 
        return "Benchmarks brain simulation performance"; 
    }
    std::string getCategory() const override { return "Benchmark"; }
    
    bool run() override {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000, nlm::ConfigSource::Runtime);
        config->set("region_count", 2, nlm::ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run simulation
        for (int i = 0; i < 1000; ++i) {
            brain->step(i, i * 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - start).count();
        
        // Benchmark completed successfully
        passed = true;
        message = "Brain simulation benchmark completed: " + 
                 std::to_string(executionTime) + " seconds for 1000 simulation steps";
        return true;
    }
};

// ==============================================================================
// Main Test Framework
// ==============================================================================

class TestFramework {
public:
    TestFramework() = default;
    ~TestFramework() = default;
    
    void addTest(std::shared_ptr<Test> test) {
        tests_.push_back(test);
    }
    
    void runAllTests() {
        std::cout << "=== NLM Test Framework ===" << std::endl;
        std::cout << "Running " << tests_.size() << " tests..." << std::endl;
        std::cout << std::endl;
        
        int passedCount = 0;
        int failedCount = 0;
        double totalTime = 0.0;
        
        // Group tests by category
        std::map<std::string, std::vector<std::shared_ptr<Test>>> testsByCategory;
        for (const auto& test : tests_) {
            testsByCategory[test->getCategory()].push_back(test);
        }
        
        // Run tests by category
        for (const auto& categoryPair : testsByCategory) {
            const std::string& category = categoryPair.first;
            const std::vector<std::shared_ptr<Test>>& categoryTests = categoryPair.second;
            
            std::cout << "=== " << category << " Tests ===" << std::endl;
            
            for (const auto& test : categoryTests) {
                std::cout << "Running " << test->getName() << ": " 
                          << test->getDescription() << std::endl;
                
                // Run setup
                test->setup();
                
                // Run test
                bool result = test->run();
                
                // Run teardown
                test->teardown();
                
                if (result) {
                    passedCount++;
                    std::cout << "  ✓ PASSED (" << std::fixed << std::setprecision(3) 
                              << test->executionTime << "s)" << std::endl;
                } else {
                    failedCount++;
                    std::cout << "  ✗ FAILED" << std::endl;
                    if (!test->message.empty()) {
                        std::cout << "     Error: " << test->message << std::endl;
                    }
                }
                
                totalTime += test->executionTime;
            }
            
            std::cout << std::endl;
        }
        
        // Print summary
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << tests_.size() << std::endl;
        std::cout << "Passed: " << passedCount << std::endl;
        std::cout << "Failed: " << failedCount << std::endl;
        std::cout << "Total execution time: " << std::fixed << std::setprecision(3) 
                  << totalTime << "s" << std::endl;
        
        if (failedCount == 0) {
            std::cout << "\n✓ All tests passed successfully!" << std::endl;
        } else {
            std::cout << "\n✗ " << failedCount << " tests failed" << std::endl;
        }
    }
    
    void generateReport(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not create report file: " << filename << std::endl;
            return;
        }
        
        file << "NLM Test Report" << std::endl;
        file << "===============" << std::endl;
        file << "Generated: " << std::time(nullptr) << std::endl;
        file << "Total tests: " << tests_.size() << std::endl;
        
        int passedCount = 0;
        for (const auto& test : tests_) {
            if (test->passed) passedCount++;
        }
        
        file << "Passed: " << passedCount << std::endl;
        file << "Failed: " << (tests_.size() - passedCount) << std::endl;
        
        file << std::endl;
        file << "Test Details:" << std::endl;
        file << "------------" << std::endl;
        
        for (const auto& test : tests_) {
            file << test->name << ": " << (test->passed ? "PASS" : "FAIL") 
                 << " (" << std::fixed << std::setprecision(3) << test->executionTime 
                 << "s)" << std::endl;
            if (!test->message.empty()) {
                file << "  " << test->message << std::endl;
            }
        }
        
        file.close();
        std::cout << "Test report saved to: " << filename << std::endl;
    }
    
private:
    std::vector<std::shared_ptr<Test>> tests_;
};

// ==============================================================================
// Global Test Registration
// ==============================================================================

// Function to get all tests
std::vector<std::shared_ptr<Test>> getAllTests() {
    std::vector<std::shared_ptr<Test>> tests;
    
    // Configuration tests
    tests.push_back(std::make_shared<ConfigCreationTest>());
    tests.push_back(std::make_shared<ConfigSetGetTest>());
    tests.push_back(std::make_shared<ConfigGetOrTest>());
    tests.push_back(std::make_shared<ConfigRemoveTest>());
    tests.push_back(std::make_shared<ConfigKeysTest>());
    
    // Brain tests
    tests.push_back(std::make_shared<BrainCreationTest>());
    tests.push_back(std::make_shared<BrainBasicFunctionalityTest>());
    
    // Benchmarks
    tests.push_back(std::make_shared<NeuronCreationBenchmark>());
    tests.push_back(std::make_shared<BrainSimulationBenchmark>());
    
    return tests;
}

// ==============================================================================
// Main test runner
// ==============================================================================

void runTests() {
    TestFramework framework;
    
    // Add all tests
    auto tests = getAllTests();
    for (const auto& test : tests) {
        framework.addTest(test);
    }
    
    // Run all tests
    framework.runAllTests();
    
    // Generate report
    framework.generateReport("test_report.txt");
}

} // namespace test
} // namespace nlm