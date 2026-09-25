// NLM Test Framework Main Integration
// Provides integration between the enhanced testing framework and existing tests

#pragma once

#include "base/TestingFramework.hpp"
#include "types/TestTypes.hpp"
#include "benchmarks/Benchmarks.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_set>

namespace nlm {
namespace test {

// Test framework integration utilities
class TestFrameworkIntegration {
public:
    // Legacy test support
    static void runLegacyTest(const std::string& testName, 
                             std::function<void()> testFunction);
    static void runLegacyTestSuite(const std::vector<std::string>& testNames,
                                  const std::vector<std::function<void()>>& testFunctions);
    
    // Enhanced test support
    static void registerEnhancedTest(TestCase* testCase);
    static void registerEnhancedTestSuite(TestSuite* testSuite);
    static std::vector<TestCase*> getAllEnhancedTests();
    static std::vector<TestSuite*> getAllEnhancedSuites();
    
    // Test execution with framework integration
    static TestResult executeTestCase(TestCase* testCase);
    static std::vector<TestResult> executeTestSuite(TestSuite* testSuite);
    static std::vector<TestResult> executeAllEnhancedTests();
    static std::vector<TestResult> executeAllEnhancedSuites();
    
    // Backward compatibility
    static bool isLegacyTestSupported(const std::string& testName);
    static void runLegacyTests();
    static bool hasEnhancedTests();
    static bool hasEnhancedSuites();
    
    // Test discovery and registration
    static std::vector<TestCase*> discoverEnhancedTests();
    static std::vector<TestSuite*> discoverEnhancedSuites();
    static bool registerTestFromFile(const std::string& filepath);
    static bool registerSuiteFromFile(const std::string& filepath);
    
    // Test output and reporting
    static void printTestResults(const std::vector<TestResult>& results);
    static void printTestSummary(const std::vector<TestResult>& results);
    static void printTestDetails(const TestResult& result);
    static std::string formatTestResults(const std::vector<TestResult>& results,
                                        const std::string& format = "console");
    static bool saveTestResults(const std::vector<TestResult>& results,
                               const std::string& filepath,
                               const std::string& format = "json");
    
    // Test filtering and selection
    static std::vector<TestResult> filterTestResults(const std::vector<TestResult>& results,
                                                    const std::string& category = "",
                                                    const std::string& tag = "",
                                                    TestStatus status = TestStatus::Unknown);
    static std::vector<TestResult> selectTestResults(const std::vector<TestResult>& results,
                                                    const std::string& testPattern = "",
                                                    size_t maxResults = 0);
    
    // Test comparison and analysis
    static std::vector<TestComparison> compareTestResults(const std::vector<TestResult>& results1,
                                                         const std::vector<TestResult>& results2);
    static double calculateTestPassRate(const std::vector<TestResult>& results);
    static std::chrono::microseconds calculateTotalTestTime(const std::vector<TestResult>& results);
    
    // Benchmark integration
    static BenchmarkSuite convertTestSuiteToBenchmarkSuite(TestSuite* testSuite);
    static std::vector<BenchmarkResult> executeBenchmarks(const BenchmarkConfig& config = BenchmarkConfig());
    static void integrateBenchmarksWithTests(const std::vector<BenchmarkResult>& benchmarks,
                                            std::vector<TestResult>& testResults);
    
    // Configuration and settings
    static void loadTestConfiguration(const std::string& filepath);
    static void saveTestConfiguration(const std::string& filepath);
    static TestParameters getTestConfiguration();
    static void setTestConfiguration(const TestParameters& config);
    static BenchmarkConfig getBenchmarkConfiguration();
    static void setBenchmarkConfiguration(const BenchmarkConfig& config);
    
    // Test discovery from existing test files
    static std::vector<std::string> discoverLegacyTests(const std::string& directory);
    static std::vector<std::string> discoverEnhancedTests(const std::string& directory);
    static std::vector<std::string> discoverTestFiles(const std::string& directory,
                                                     const std::string& extension = ".cpp");
    static bool analyzeTestFile(const std::string& filepath,
                               std::vector<std::string>& testNames,
                               std::vector<std::string>& testCategories,
                               std::vector<std::string>& testTags);
    
    // Test framework setup and teardown
    static void initializeTestFramework();
    static void cleanupTestFramework();
    static bool isTestFrameworkInitialized();
    
    // Test statistics and metrics
    static TestStatistics calculateTestStatistics(const std::vector<TestResult>& results);
    static BenchmarkStatistics calculateBenchmarkStatistics(const std::vector<BenchmarkResult>& results);
    static std::string generateTestReport(const std::vector<TestResult>& testResults,
                                         const std::vector<BenchmarkResult>& benchmarkResults,
                                         const std::string& format = "html");
    
    // Performance monitoring and profiling
    static void startPerformanceMonitoring();
    static void stopPerformanceMonitoring();
    static double getCurrentMemoryUsage();
    static std::chrono::microseconds getCurrentExecutionTime();
    static PerformanceProfile getCurrentPerformanceProfile();
    
    // Test execution timing and profiling
    static std::chrono::microseconds measureTestExecutionTime(TestCase* testCase);
    static double measureTestMemoryUsage(TestCase* testCase);
    static PerformanceProfile profileTestExecution(TestCase* testCase);
    
    // Error handling and recovery
    static bool handleTestError(const std::string& testName,
                               const std::string& errorMessage,
                               const std::string& file = "",
                               int line = 0);
    static void recoverFromTestError(const TestResult& errorResult);
    static bool isTestErrorRecoverable(const TestResult& result);
    static void retryFailedTests(const std::vector<TestResult>& results,
                               size_t maxRetries = 3);
    
    // Test result validation
    static bool validateTestResult(const TestResult& result);
    static bool validateTestResults(const std::vector<TestResult>& results);
    static bool validateBenchmarkResult(const BenchmarkResult& result);
    static bool validateBenchmarkResults(const std::vector<BenchmarkResult>& results);
    
    // Test result comparison
    static bool areTestResultsEquivalent(const TestResult& result1, const TestResult& result2);
    static double calculateTestResultSimilarity(const TestResult& result1,
                                               const TestResult& result2);
    static std::string generateTestResultDiff(const TestResult& result1,
                                             const TestResult& result2);
    
    // Test output formatting
    static std::string formatTestResultForOutput(const TestResult& result,
                                                const std::string& format = "json");
    static std::string formatBenchmarkResultForOutput(const BenchmarkResult& result,
                                                     const std::string& format = "json");
    static std::string formatTestSuiteForOutput(const TestSuite& suite,
                                               const std::string& format = "json");
    static std::string formatBenchmarkSuiteForOutput(const BenchmarkSuite& suite,
                                                    const std::string& format = "json");
    
    // Integration with existing test main
    static int runIntegratedTestSuite(int argc, char** argv);
    static int runEnhancedTestSuite();
    static int runLegacyTestSuite();
    static int runBenchmarkSuite();
    
    // Test framework command line interface
    static void printHelp();
    static bool parseCommandLine(int argc, char** argv,
                                std::unordered_map<std::string, std::string>& options);
    static bool executeCommand(const std::string& command,
                              const std::unordered_map<std::string, std::string>& options);
    
    // Test framework status and information
    static std::string getFrameworkStatus();
    static std::string getFrameworkVersion();
    static std::string getFrameworkInfo();
    static bool isFrameworkHealthy();
    
private:
    static bool initialized_;
    static std::vector<TestCase*> enhancedTests_;
    static std::vector<TestSuite*> enhancedSuites_;
    static std::vector<std::string> legacyTests_;
    static TestParameters testConfig_;
    static BenchmarkConfig benchmarkConfig_;
    static std::mutex mutex_;
    
    static void loadConfiguration();
    static void saveConfiguration();
    static bool initializeEnhancedTests();
    static bool initializeEnhancedSuites();
    static bool initializeLegacyTests();
    static void cleanupEnhancedTests();
    static void cleanupEnhancedSuites();
    static void cleanupLegacyTests();
    
    static bool isTestCaseInLegacyList(const std::string& testName);
    static bool isTestCaseInEnhancedList(const std::string& testName);
    static TestCase* findEnhancedTestByName(const std::string& testName);
    static TestSuite* findEnhancedSuiteByName(const std::string& suiteName);
}; 

// Enhanced test main with framework integration
class EnhancedTestMain {
public:
    static int runAllTests(int argc, char** argv);
    static int runEnhancedTests(int argc, char** argv);
    static int runLegacyTests(int argc, char** argv);
    static int runBenchmarks(int argc, char** argv);
    static int runTestsByCategory(const std::string& category, int argc, char** argv);
    static int runTestsByTag(const std::string& tag, int argc, char** argv);
    static int runTestsByNamePattern(const std::string& pattern, int argc, char** argv);
    static int runBenchmarksByCategory(const std::string& category, int argc, char** argv);
    
    static void printTestBanner();
    static void printTestSummary(const std::vector<TestResult>& testResults,
                                const std::vector<BenchmarkResult>& benchmarkResults);
    static void printTestHelp();
    static void printBenchmarkHelp();
    
private:
    static std::string getTestVersion();
    static std::string getFrameworkInfo();
    static bool isTestArgument(const std::string& arg);
    static bool isBenchmarkArgument(const std::string& arg);
    static std::string extractTestCategoryFromArg(const std::string& arg);
    static std::string extractTestTagFromArg(const std::string& arg);
    static std::string extractTestNamePatternFromArg(const std::string& arg);
    static std::string extractBenchmarkCategoryFromArg(const std::string& arg);
};

// Test framework main function (entry point)
int testFrameworkMain(int argc, char** argv);

} // namespace test
} // namespace nlm
