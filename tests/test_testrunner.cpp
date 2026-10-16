// TestRunner Tests
// Comprehensive tests for the enhanced TestRunner module

#include "src/test/TestRunner.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <chrono>

namespace test_testrunner {

void testTestRunnerCreation() {
    std::cout << "  Testing TestRunner creation..." << std::endl;
    nlm::TestRunner runner;
    
    // Test basic functionality
    assert(runner.getTestNames().size() == 0);
    
    // Set verbose output
    runner.setVerbose(true);
    runner.setOutputFormat("json");
    
    std::cout << "    TestRunner creation test passed" << std::endl;
}

void testTestCaseManagement() {
    std::cout << "  Testing test case management..." << std::endl;
    nlm::TestRunner runner;
    
    // Create a mock test case for testing
    class MockTestCase : public nlm::TestCase {
    public:
        std::string getName() const override { return "Mock Test"; }
        std::string getDescription() const override { return "Mock test for testing TestRunner"; }
        nlm::TestResult run(std::shared_ptr<nlm::Config> config) override {
            nlm::TestResult result;
            result.name = getName();
            result.passed = true;
            result.description = getDescription();
            result.duration = 0.1;
            result.output = "Mock test output";
            return result;
        }
    };
    
    // Add test
    runner.addTest(std::make_unique<MockTestCase>());
    
    // Verify test was added
    assert(runner.getTestNames().size() == 1);
    assert(runner.getTestNames()[0] == "Mock Test");
    
    // Run test
    auto config = std::make_shared<nlm::Config>();
    nlm::TestResult result = runner.runTest("Mock Test", config);
    
    assert(result.passed);
    assert(result.name == "Mock Test");
    
    std::cout << "    Test case management test passed" << std::endl;
}

void testRunAllTests() {
    std::cout << "  Testing run all tests..." << std::endl;
    nlm::TestRunner runner;
    
    // Add multiple mock test cases
    class MockTest1 : public nlm::TestCase {
    public:
        std::string getName() const override { return "Test 1"; }
        std::string getDescription() const override { return "First mock test"; }
        nlm::TestResult run(std::shared_ptr<nlm::Config> config) override {
            nlm::TestResult result;
            result.name = getName();
            result.passed = true;
            result.duration = 0.5;
            return result;
        }
    };
    
    class MockTest2 : public nlm::TestCase {
    public:
        std::string getName() const override { return "Test 2"; }
        std::string getDescription() const override { return "Second mock test"; }
        nlm::TestResult run(std::shared_ptr<nlm::Config> config) override {
            nlm::TestResult result;
            result.name = getName();
            result.passed = false;
            result.duration = 0.3;
            result.errorMessage = "Mock test failure";
            return result;
        }
    };
    
    runner.addTest(std::make_unique<MockTest1>());
    runner.addTest(std::make_unique<MockTest2>());
    
    auto config = std::make_shared<nlm::Config>();
    std::vector<nlm::TestResult> results = runner.runAll(config);
    
    assert(results.size() == 2);
    
    // Check results
    bool test1Found = false, test2Found = false;
    for (const auto& result : results) {
        if (result.name == "Test 1") {
            assert(result.passed);
            assert(result.duration == 0.5);
            test1Found = true;
        } else if (result.name == "Test 2") {
            assert(!result.passed);
            assert(result.duration == 0.3);
            test2Found = true;
        }
    }
    
    assert(test1Found && test2Found);
    
    std::cout << "    Run all tests test passed" << std::endl;
}

void testProgressReportingIntegration() {
    std::cout << "  Testing progress reporting integration..." << std::endl;
    nlm::TestRunner runner;
    nlm::ProgressReporter reporter;
    
    // Add mock tests
    for (int i = 0; i < 5; ++i) {
        class MockTest : public nlm::TestCase {
        public:
            int id;
            MockTest(int id) : id(id) {}
            std::string getName() const override { return "Mock Test " + std::to_string(id); }
            std::string getDescription() const override { return "Mock test " + std::to_string(id); }
            nlm::TestResult run(std::shared_ptr<nlm::Config> config) override {
                nlm::TestResult result;
                result.name = getName();
                result.passed = true;
                result.duration = 0.1 + id * 0.05;
                return result;
            }
        };
        runner.addTest(std::make_unique<MockTest>(i));
    }
    
    auto config = std::make_shared<nlm::Config>();
    std::vector<nlm::TestResult> results = runner.runAllWithProgress(config, reporter);
    
    assert(results.size() == 5);
    
    std::cout << "    Progress reporting integration test passed" << std::endl;
}

void testTestResultsSummary() {
    std::cout << "  Testing test results summary..." << std::endl;
    nlm::TestRunner runner;
    
    // Generate summary from mock results
    std::vector<nlm::TestResult> mockResults;
    
    nlm::TestResult result1;
    result1.name = "Test 1";
    result1.passed = true;
    result1.duration = 0.5;
    mockResults.push_back(result1);
    
    nlm::TestResult result2;
    result2.name = "Test 2";
    result2.passed = false;
    result2.duration = 0.3;
    result2.errorMessage = "Test failed";
    mockResults.push_back(result2);
    
    nlm::TestResult result3;
    result3.name = "Test 3";
    result3.passed = true;
    result3.duration = 0.8;
    mockResults.push_back(result3);
    
    std::string summary = runner.generateSummary(mockResults);
    
    assert(summary.find("Test Summary") != std::string::npos);
    assert(summary.find("Total tests: 3") != std::string::npos);
    assert(summary.find("Passed: 2") != std::string::npos);
    assert(summary.find("Failed: 1") != std::string::npos);
    assert(summary.find("Success rate: 66.67%") != std::string::npos);
    
    std::cout << "    Test results summary test passed" << std::endl;
}

void testResultsPersistence() {
    std::cout << "  Testing results persistence..." << std::endl;
    nlm::TestRunner runner;
    
    // Create mock results
    std::vector<nlm::TestResult> results;
    
    nlm::TestResult result1;
    result1.name = "Test 1";
    result1.passed = true;
    result1.duration = 0.5;
    result1.description = "First test case";
    results.push_back(result1);
    
    nlm::TestResult result2;
    result2.name = "Test 2";
    result2.passed = false;
    result2.duration = 0.3;
    result2.errorMessage = "Test failed due to mock error";
    results.push_back(result2);
    
    // Test JSON saving
    std::string jsonFile = "/tmp/test_results.json";
    bool jsonSaved = runner.saveResults(results, jsonFile, "json");
    assert(jsonSaved);
    
    // Verify file was created
    std::ifstream jsonFileStream(jsonFile);
    assert(jsonFileStream.good());
    jsonFileStream.close();
    
    // Test text format saving
    std::string textFile = "/tmp/test_results.txt";
    bool textSaved = runner.saveResults(results, textFile, "text");
    assert(textSaved);
    
    // Verify file was created
    std::ifstream textFileStream(textFile);
    assert(textFileStream.good());
    textFileStream.close();
    
    // Clean up
    std::remove(jsonFile.c_str());
    std::remove(textFile.c_str());
    
    std::cout << "    Results persistence test passed" << std::endl;
}

void testIntegrationWithTestRunner() {
    std::cout << "  Testing integration with IntegrationTestRunner..." << std::endl;
    nlm::IntegrationTestRunner integrationRunner;
    
    auto config = std::make_shared<nlm::Config>();
    std::vector<nlm::TestResult> results = integrationRunner.runAllIntegrationTests(config);
    
    // Should have at least the default test cases
    assert(results.size() >= 5);
    
    // Generate report
    std::string report = integrationRunner.generateIntegrationReport(results);
    assert(report.find("Integration Test Report") != std::string::npos);
    
    // Save report
    std::string reportFile = "/tmp/integration_report.html";
    bool reportSaved = integrationRunner.saveIntegrationReport(results, reportFile, "html");
    assert(reportSaved);
    
    // Clean up
    std::remove(reportFile.c_str());
    
    std::cout << "    Integration with TestRunner test passed" << std::endl;
}

void runAll() {
    std::cout << "=== TestRunner Tests ===" << std::endl;
    
    testTestRunnerCreation();
    testTestCaseManagement();
    testRunAllTests();
    testProgressReportingIntegration();
    testTestResultsSummary();
    testResultsPersistence();
    testIntegrationWithTestRunner();
    
    std::cout << "=== All TestRunner Tests PASSED ===" << std::endl << std::endl;
}

} // namespace test_testrunner