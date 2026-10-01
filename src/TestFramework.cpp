// TestFramework.cpp
// Implementation of the modular test framework for NLM

#include "TestFramework.hpp"
#include <fstream>
#include <algorithm>

using namespace nlm;

// Helper function to get test category string
static std::string getCategoryString(TestCategory category) {
    switch (category) {
        case TestCategory::Unit: return "Unit";
        case TestCategory::Integration: return "Integration";
        case TestCategory::Performance: return "Performance";
        case TestCategory::Stress: return "Stress";
        case TestCategory::Functional: return "Functional";
        case TestCategory::Regression: return "Regression";
        case TestCategory::Benchmark: return "Benchmark";
        default: return "Unknown";
    }
}

// Test implementation for common patterns
namespace {
    // Example test case implementations
    class ExampleNeuronTest : public TestCase {
    public:
        std::string getName() const override { return "NeuronCreation"; }
        TestCategory getCategory() const override { return TestCategory::Unit; }
        std::string getDescription() const override { return "Test basic neuron creation and initialization"; }
        bool run() override {
            // This would test actual neuron functionality
            // For now, just return true as a placeholder
            return true;
        }
    };
    
    class ExampleConfigTest : public TestCase {
    public:
        std::string getName() const override { return "ConfigLoadSave"; }
        TestCategory getCategory() const override { return TestCategory::Unit; }
        std::string getDescription() const override { return "Test configuration file loading and saving"; }
        bool run() override {
            // This would test config functionality
            return true;
        }
    };
    
    class ExamplePerformanceTest : public TestCase {
    public:
        std::string getName() const override { return "PerformanceBenchmark"; }
        TestCategory getCategory() const override { return TestCategory::Performance; }
        std::string getDescription() const override { return "Test performance with various network sizes"; }
        bool run() override {
            // This would test performance
            return true;
        }
    };
}}

// Global test framework instance
define_global_test_framework(TestFramework& framework) {
    static TestFramework globalFramework;
    return globalFramework;
}

TestFramework& get_global_test_framework() {
    static TestFramework globalFramework;
    return globalFramework;
}

void run_global_tests() {
    TestFramework& framework = get_global_test_framework();
    framework.runAllTests();
}

void add_example_tests_to_framework() {
    TestFramework& framework = get_global_test_framework();
    
    auto unitSuite = std::make_unique<TestSuite>("Unit Tests");
    unitSuite->addTest(std::make_unique<ExampleNeuronTest>());
    unitSuite->addTest(std::make_unique<ExampleConfigTest>());
    
    auto performanceSuite = std::make_unique<TestSuite>("Performance Tests");
    performanceSuite->addTest(std::make_unique<ExamplePerformanceTest>());
    
    framework.addSuite(std::move(unitSuite));
    framework.addSuite(std::move(performanceSuite));
}

// Convenience functions
void add_test(TestSuite& suite, std::unique_ptr<TestCase> test) {
    suite.addTest(std::move(test));
}

void set_test_framework_verbose(bool verbose) {
    TestFramework& framework = get_global_test_framework();
    framework.setVerbose(verbose);
}

void set_test_framework_quiet(bool quiet) {
    TestFramework& framework = get_global_test_framework();
    framework.setQuiet(quiet);
}

void set_test_output_file(const std::string& filename) {
    TestFramework& framework = get_global_test_framework();
    framework.setOutputFile(filename);
}

std::vector<TestResult> run_tests() {
    return get_global_test_framework().runAllTests();
}