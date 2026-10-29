"""
NLM Test Framework - Comprehensive C++ testing framework with Catch2 integration
"""

// Core framework components
#include "test_framework/TestBase.hpp"
#include "test_framework/TestRunner.hpp"
#include "test_framework/TestResult.hpp"
#include "test_framework/TestFixture.hpp"
#include "test_framework/TestMacros.hpp"
#include "test_framework/Assert.hpp"
#include "test_framework/Expect.hpp"
#include "test_framework/TestCategories.hpp"
#include "test_framework/Benchmark.hpp"
#include "test_framework/PerformanceProfiler.hpp"
#include "test_framework/IntegrationTest.hpp"

// Test utilities
#include "test_framework/TestUtils.hpp"
#include "test_framework/Parameterizer.hpp"
#include "test_framework/ComparisonUtils.hpp"

// Performance and analysis
#include "test_framework/PerformanceAnalyzer.hpp"
#include "test_framework/RegressionDetector.hpp"

// Integration and end-to-end testing
#include "test_framework/IntegrationTestRunner.hpp"
#include "test_framework/SystemTest.hpp"

// Configuration and management
#include "test_framework/TestConfig.hpp"
#include "test_framework/TestSuite.hpp"

// Macro aliases for common test patterns
#define TEST_CASE(name) TEST_CASE_IMPL(name, __LINE__)
#define TEST_CASE_P(name, param) TEST_CASE_P_IMPL(name, param, __LINE__)
#define BENCHMARK(name, ...) BENCHMARK_IMPL(name, __LINE__, __VA_ARGS__)
#define INTEGRATION_TEST(name) INTEGRATION_TEST_IMPL(name, __LINE__)

// Export for dynamic test discovery
#ifdef NLM_TEST_EXPORTS
#define TEST_EXPORT __attribute__((visibility("default")))
#else
#define TEST_EXPORT
#endif

// Version information
#define NLM_TEST_FRAMEWORK_VERSION "1.0.0"
#define NLM_TEST_FRAMEWORK_DATE "2026-10-29"

// Feature detection
#ifdef __cpp_concepts
#define NLM_TEST_CONCEPTS_SUPPORT 1
#else
#define NLM_TEST_CONCEPTS_SUPPORT 0
#endif

#ifdef __cpp_coroutines
#define NLM_TEST_COROUTINES_SUPPORT 1
#else
#define NLM_TEST_COROUTINES_SUPPORT 0
#endif

// Configuration
namespace nlm::test {
    struct Configuration {
        bool enable_benchmarks = true;
        bool enable_regression_detection = false;
        bool enable_performance_profiling = true;
        bool enable_integration_tests = true;
        bool enable_parameterized_tests = true;
        std::string output_format = "console"; // console, json, xml
        std::string output_file = "";
        bool verbose = false;
        bool color_output = true;
        size_t max_failures = 10;
        double benchmark_threshold = 100.0; // milliseconds
    };
    
    Configuration getConfiguration();
    void setConfiguration(const Configuration& config);
}

// Entry point
namespace nlm::test {
    int runAllTests();
    int runTests(const std::vector<std::string>& categories);
    void registerTestCategories();
    TestResult getOverallResult();
}