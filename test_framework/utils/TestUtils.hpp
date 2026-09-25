// Test Utilities for NLM Testing Framework
// Provides parameterized test support and data generation utilities

#pragma once

#include "types/TestTypes.hpp"
#include "base/TestingFramework.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {
namespace test {

// Forward declarations
class ParameterGenerator;

// Test parameter utility functions
class TestUtils {
public:
    // Parameter conversion utilities
    static std::string toString(const ConfigValue& value);
    static std::string toString(const TestParameters& params);
    static std::string toString(const TestResult& result);
    
    // Parameter set utilities
    static TestParameters mergeParameters(const TestParameters& base, 
                                         const TestParameters& override);
    static bool hasParameter(const TestParameters& params, const std::string& key);
    static std::vector<std::string> getParameterKeys(const TestParameters& params);
    
    // Test data generation utilities
    static TestParameters generateGrid(int start, int end, int step);
    static TestParameters generateGrid(double start, double end, double step);
    static TestParameters generateRandom(const std::string& type, 
                                       size_t count = 100,
                                       double mean = 0.0,
                                       double stddev = 1.0);
    static TestParameters generateScenarios(const std::string& scenarioType);
    
    // Statistical analysis utilities
    static double calculateMean(const std::vector<double>& values);
    static double calculateStandardDeviation(const std::vector<double>& values, double mean = 0.0);
    static double calculateMedian(const std::vector<double>& values);
    static double calculatePercentile(const std::vector<double>& values, double percentile);
    
    // Test result analysis utilities
    static bool isTestPassed(const TestResult& result);
    static std::string getTestDurationString(const TestResult& result);
    static std::string getPerformanceClassification(double duration);
    
    // String and formatting utilities
    static std::string formatTestName(const std::string& testName, 
                                    const TestParameters& params);
    static std::string formatDuration(std::chrono::microseconds duration);
    static std::string formatNumber(double value, int precision = 6);
    
    // File and I/O utilities
    static bool saveTestResults(const std::string& filepath, 
                              const std::vector<TestResult>& results);
    static bool loadTestResults(const std::string& filepath, 
                               std::vector<TestResult>& results);
    static std::string generateTimestamp();
    
    // Test discovery utilities
    static std::vector<std::string> extractTestNamesFromFile(const std::string& filepath);
    static std::vector<std::string> extractTestCategoriesFromFile(const std::string& filepath);
    static std::vector<TestParameters> extractTestParametersFromFile(const std::string& filepath);
    
    // Performance comparison utilities
    static double comparePerformance(const std::vector<double>& baseline,
                                   const std::vector<double>& current);
    static double calculatePerformanceImprovement(const BenchmarkResult& baseline,
                                               const BenchmarkResult& current);
    
    // Validation utilities
    static bool validateParameters(const TestParameters& params);
    static bool validateTestResult(const TestResult& result);
    static bool validateBenchmarkResult(const BenchmarkResult& result);
    
    // Random number generation utilities
    static int generateRandomInt(int min, int max);
    static double generateRandomDouble(double min, double max);
    static bool generateRandomBool(double probability = 0.5);
    
    // Memory and resource utilities
    static size_t estimateMemoryUsage(const TestParameters& params);
    static std::chrono::microseconds estimateExecutionTime(const TestCase& testCase);
    
    // Output formatting utilities
    static std::string formatTestResults(const std::vector<TestResult>& results,
                                        OutputFormat format = OutputFormat::Console);
    static std::string formatBenchmarkResults(const std::vector<BenchmarkResult>& results,
                                            OutputFormat format = OutputFormat::Console);
    static void printTestResults(const std::vector<TestResult>& results,
                               OutputFormat format = OutputFormat::Console);
    static void printBenchmarkResults(const std::vector<BenchmarkResult>& results,
                                    OutputFormat format = OutputFormat::Console);
    
    // Test filtering and selection utilities
    static std::vector<TestCase*> filterTestsByCategory(std::vector<TestCase*>& tests,
                                                      const std::string& category);
    static std::vector<TestCase*> filterTestsByTag(std::vector<TestCase*>& tests,
                                                 const std::string& tag);
    static std::vector<TestCase*> filterTestsByNamePattern(std::vector<TestCase*>& tests,
                                                         const std::string& pattern);
    
    // Test execution utilities
    static void runTestSuite(TestSuite& suite, TestRegistry& registry);
    static void runTestCases(std::vector<TestCase*>& testCases);
    static void runParameterizedTestCase(TestCase* testCase,
                                       const std::vector<TestParameters>& parameterSets);
    
    // Performance monitoring utilities
    static void startPerformanceMonitoring();
    static void stopPerformanceMonitoring();
    static double getCurrentMemoryUsage();
    static std::chrono::microseconds getCurrentExecutionTime();
    
    // Configuration utilities
    static std::string getTestConfigurationFile();
    static bool setTestConfigurationFile(const std::string& filepath);
    static TestParameters getTestConfiguration();
    static bool setTestConfiguration(const TestParameters& config);
    
    // Error handling utilities
    static std::string getLastError();
    static void clearLastError();
    static bool hasLastError();
    static void logTestError(const std::string& testName,
                           const std::string& errorMessage,
                           const std::string& file = "",
                           int line = 0);
    
    // Test result utilities
    static TestResult createTestResult(const TestCase& testCase,
                                      TestStatus status,
                                      const std::string& message = "",
                                      const std::string& file = "",
                                      int line = 0);
    static BenchmarkResult createBenchmarkResult(const Benchmark& benchmark,
                                                const std::vector<double>& timings,
                                                const std::string& unit = "us");
    
    // Integration utilities
    static bool registerTestCase(TestCase* testCase);
    static bool unregisterTestCase(TestCase* testCase);
    static std::vector<TestCase*> getAllTestCases();
    static std::vector<TestSuite*> getAllTestSuites();
    static void clearAllTestRegistrations();
    
    // Utility constants and settings
    static const double DEFAULT_RANDOM_SEED;
    static const size_t DEFAULT_SAMPLE_SIZE;
    static const double DEFAULT_CONFIDENCE_INTERVAL;
    static const std::string DEFAULT_TIMESTAMP_FORMAT;
    static const std::string DEFAULT_TEST_NAME_DELIMITER;
    
private:
    static std::string convertConfigValueToString(const ConfigValue& value);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static bool containsSubstring(const std::string& str, const std::string& substr);
    static std::string trimString(const std::string& str);
    static void initializeRandomGenerator();
    static std::mt19937& getRandomGenerator();
    
    static std::string lastError_;
    static std::mt19937 randomGenerator_;
    static bool randomGeneratorInitialized_;
};

// Parameter generator utility functions
class ParameterGenerator {
public:
    // Grid generation utilities
    static TestParameters generateIntegerGrid(int start, int end, int step = 1);
    static TestParameters generateDoubleGrid(double start, double end, double step = 1.0);
    static TestParameters generateStringGrid(const std::vector<std::string>& values);
    static TestParameters generateBooleanGrid(bool value1 = false, bool value2 = true);
    
    // Random parameter generation utilities
    static TestParameters generateRandomIntegerParameters(const std::string& name, 
                                                         int min, int max,
                                                         size_t count = 100);
    static TestParameters generateRandomDoubleParameters(const std::string& name,
                                                        double min, double max,
                                                        size_t count = 100,
                                                        double mean = 0.0,
                                                        double stddev = 1.0);
    static TestParameters generateRandomStringParameters(const std::string& name,
                                                        const std::vector<std::string>& values,
                                                        size_t count = 100);
    static TestParameters generateRandomBooleanParameters(const std::string& name,
                                                         double trueProbability = 0.5,
                                                         size_t count = 100);
    
    // Scenario generation utilities
    static TestParameters generateSimplexScenario(size_t numVariables);
    static TestParameters generateLinearScenario(size_t numVariables);
    static TestParameters generateQuadraticScenario(size_t numVariables);
    static TestParameters generateCombinationScenario(const std::vector<TestParameters>& scenarios);
    
    // Parameter transformation utilities
    static TestParameters transformParameters(const TestParameters& params,
                                             std::function<void(TestParameters&)> transform);
    static TestParameters addParameters(const TestParameters& params,
                                       const TestParameters& additional);
    static TestParameters removeParameters(const TestParameters& params,
                                          const std::vector<std::string>& keysToRemove);
    
    // Parameter validation utilities
    static bool validateParameterType(const TestParameters& params, const std::string& key,
                                    const std::string& expectedType);
    static bool validateParameterRange(const TestParameters& params,
                                      const std::string& key,
                                      double min, double max);
    static bool validateParameterSet(const TestParameters& params,
                                    const std::unordered_map<std::string, std::string>& schema);
    
    // Parameter optimization utilities
    static TestParameters optimizeParameters(const TestParameters& params,
                                            const std::vector<double>& scores);
    static TestParameters selectBestParameters(const std::vector<TestParameters>& params,
                                              const std::vector<double>& scores);
    
    // Parameter export/import utilities
    static std::string exportParameters(const TestParameters& params);
    static bool importParameters(TestParameters& params, const std::string& data);
    static bool saveParametersToFile(const TestParameters& params, const std::string& filepath);
    static bool loadParametersFromFile(TestParameters& params, const std::string& filepath);
    
    // Parameter comparison utilities
    static bool areParametersEqual(const TestParameters& params1, const TestParameters& params2);
    static double calculateParameterDistance(const TestParameters& params1,
                                           const TestParameters& params2);
    static TestParameters findClosestParameters(const TestParameters& target,
                                               const std::vector<TestParameters>& candidates);
    
    // Parameter statistics utilities
    static double calculateParameterMean(const TestParameters& params, const std::string& key);
    static double calculateParameterVariance(const TestParameters& params, const std::string& key);
    static double calculateParameterStandardDeviation(const TestParameters& params, const std::string& key);
    static TestParameters calculateParameterStatistics(const TestParameters& params);
    
    // Parameter generation utilities
    static std::vector<TestParameters> generateParameterCombinations(const std::vector<TestParameters>& paramSets);
    static std::vector<TestParameters> generateParameterSensitivities(const TestParameters& baseParams,
                                                                      const std::vector<std::string>& keys);
    static TestParameters generateDefaultParameters();
    
private:
    static std::string convertValueToString(const ConfigValue& value);
    static ConfigValue convertStringToValue(const std::string& str, const std::string& typeHint);
};

// Test output formatters
class TestOutputFormatter {
public:
    // Console output formatters
    static std::string formatConsoleTestResult(const TestResult& result);
    static std::string formatConsoleBenchmarkResult(const BenchmarkResult& result);
    static std::string formatConsoleTestSummary(const std::vector<TestResult>& results);
    static std::string formatConsoleBenchmarkSummary(const std::vector<BenchmarkResult>& results);
    
    // JSON output formatters
    static std::string formatJsonTestResult(const TestResult& result);
    static std::string formatJsonBenchmarkResult(const BenchmarkResult& result);
    static std::string formatJsonTestSummary(const std::vector<TestResult>& results);
    static std::string formatJsonBenchmarkSummary(const std::vector<BenchmarkResult>& results);
    
    // XML output formatters
    static std::string formatXmlTestResult(const TestResult& result);
    static std::string formatXmlBenchmarkResult(const BenchmarkResult& result);
    static std::string formatXmlTestSummary(const std::vector<TestResult>& results);
    static std::string formatXmlBenchmarkSummary(const std::vector<BenchmarkResult>& results);
    
    // Markdown output formatters
    static std::string formatMarkdownTestResult(const TestResult& result);
    static std::string formatMarkdownBenchmarkResult(const BenchmarkResult& result);
    static std::string formatMarkdownTestSummary(const std::vector<TestResult>& results);
    static std::string formatMarkdownBenchmarkSummary(const std::vector<BenchmarkResult>& results);
    
    // HTML output formatters
    static std::string formatHtmlTestResult(const TestResult& result);
    static std::string formatHtmlBenchmarkResult(const BenchmarkResult& result);
    static std::string formatHtmlTestSummary(const std::vector<TestResult>& results);
    static std::string formatHtmlBenchmarkSummary(const std::vector<BenchmarkResult>& results);
    
    // Custom output formatters
    static std::string formatCustomTestResult(const TestResult& result,
                                            const std::string& formatTemplate);
    static std::string formatCustomBenchmarkResult(const BenchmarkResult& result,
                                                 const std::string& formatTemplate);
    
    // Output filtering utilities
    static std::vector<TestResult> filterTestResults(const std::vector<TestResult>& results,
                                                    const std::function<bool(const TestResult&)>& filter);
    static std::vector<BenchmarkResult> filterBenchmarkResults(const std::vector<BenchmarkResult>& results,
                                                             const std::function<bool(const BenchmarkResult&)>& filter);
    static TestResult sortTestResults(const std::vector<TestResult>& results,
                                    const std::string& sortBy = "name",
                                    bool ascending = true);
    static BenchmarkResult sortBenchmarkResults(const std::vector<BenchmarkResult>& results,
                                               const std::string& sortBy = "name",
                                               bool ascending = true);
    
    // Performance classification utilities
    static std::string classifyPerformance(double duration, PerformanceClass benchmark);
    static std::string getPerformanceColorCode(PerformanceClass benchmark);
    static std::string getStatusColorCode(TestStatus status);
    
    // Progress reporting utilities
    static void printProgress(const std::string& message);
    static void printTestProgress(size_t current, size_t total, const std::string& testName);
    static void printBenchmarkProgress(size_t current, size_t total, const std::string& benchmarkName);
    static void printCompletionMessage();
};

} // namespace test
} // namespace nlm
