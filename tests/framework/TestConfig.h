// Test Configuration
// Configuration file for the NLM Test Framework
// This file provides configuration options for the test framework

#ifndef NLM_TEST_FRAMEWORK_CONFIG_H
#define NLM_TEST_FRAMEWORK_CONFIG_H

#include <string>
#include <vector>
#include <map>

namespace nlm::test {

// Test framework version
#define NLM_TEST_FRAMEWORK_VERSION "1.0.0"

// Configuration options
struct TestFrameworkConfig {
    // General settings
    bool enabled = true;
    std::string name = "NLM Test Framework";
    std::string version = NLM_TEST_FRAMEWORK_VERSION;
    
    // Test categories
    bool enableUnitTests = true;
    bool enableIntegrationTests = true;
    bool enablePerformanceTests = false;
    bool enableMemoryTests = false;
    bool enableEdgeCaseTests = true;
    bool enableStressTests = false;
    bool enablePropertyTests = false;
    bool enableRegressionTests = true;
    
    // Test execution
    int maxTestCount = 1000;
    int maxTestTimeSeconds = 300;
    bool parallelExecution = false;
    int parallelThreads = std::thread::hardware_concurrency();
    
    // Test reporting
    std::string reportFormat = "text";
    std::string reportOutputDirectory = "./test_reports";
    bool generateHTMLReports = true;
    bool generateXMLReports = true;
    bool generateJSONReports = false;
    bool generateJUnitXML = true;
    bool generateCoverageReports = false;
    
    // Test output
    bool verboseOutput = false;
    bool showProgress = true;
    bool colorOutput = true;
    bool printTestNames = true;
    bool printTimingInfo = true;
    bool printMemoryUsage = false;
    
    // Test utilities
    bool enableTestTimer = true;
    bool enableMemoryTracker = true;
    bool enableResourceMonitor = true;
    bool enableDataGenerator = true;
    bool enableAssertionUtilities = true;
    bool enableComparisonUtilities = true;
    
    // Test infrastructure
    bool integrateWithCMake = true;
    bool integrateWithBuildSystem = true;
    bool crossPlatformSupport = true;
    std::vector<std::string> supportedPlatforms = {"linux", "windows", "macos"};
    
    // Test dependencies
    bool requireCatch2 = false;
    bool requireBoost = false;
    bool requireGoogleTest = false;
    bool enableMockFramework = false;
    
    // Legacy compatibility
    bool maintainBackwardCompatibility = true;
    bool migrateGradually = true;
    bool runLegacyTests = true;
    bool runModernTests = true;
    
    // Test quality
    bool enforceTestNaming = true;
    bool enforceTestIsolation = true;
    bool enforceTestDeterminism = true;
    bool enforceTestRepeatability = true;
    bool enforceTestObservability = true;
    bool enforceTestControllability = true;
    
    // Performance
    bool optimizeForSpeed = true;
    bool optimizeForMemory = false;
    bool optimizeForCPU = true;
    
    // Security
    bool sanitizeMemory = false;
    bool sanitizeThread = false;
    bool enableAddressSanitizer = false;
    bool enableUndefinedBehaviorSanitizer = false;
    
    // Logging
    std::string logLevel = "info";
    std::string logFile = "test_framework.log";
    bool logToFile = true;
    bool logToConsole = true;
};

// Default configuration
extern TestFrameworkConfig defaultConfig;

// Configuration helper functions
void loadConfiguration(const std::string& configFile);
void saveConfiguration(const std::string& configFile);
void resetConfiguration();
TestFrameworkConfig& getConfiguration();
void setConfiguration(const TestFrameworkConfig& config);

// Configuration validation
bool validateConfiguration(const TestFrameworkConfig& config);
std::vector<std::string> getConfigurationErrors(const TestFrameworkConfig& config);

// Configuration macros
#define NLM_TEST_FRAMEWORK_ENABLED (nlm::test::getConfiguration().enabled)
#define NLM_TEST_FRAMEWORK_VERBOSE (nlm::test::getConfiguration().verboseOutput)
#define NLM_TEST_FRAMEWORK_REPORT_FORMAT (nlm::test::getConfiguration().reportFormat)

// Platform detection
bool isPlatformSupported(const std::string& platform);
bool isPlatformAvailable(const std::string& platform);

// Compiler detection
bool isCompilerSupported(const std::string& compiler);
bool isCompilerAvailable(const std::string& compiler);

// Dependency detection
bool hasDependency(const std::string& dependency);
bool isDependencyAvailable(const std::string& dependency);

// Configuration file format
#define NLM_CONFIG_FILE_EXTENSION ".json"
#define NLM_CONFIG_FILE_EXTENSION_XML ".xml"
#define NLM_CONFIG_FILE_EXTENSION_INI ".ini"

// Configuration keys
#define NLM_CONFIG_ENABLED "enabled"
#define NLM_CONFIG_NAME "name"
#define NLM_CONFIG_VERSION "version"
#define NLM_CONFIG_UNIT_TESTS "enableUnitTests"
#define NLM_CONFIG_INTEGRATION_TESTS "enableIntegrationTests"
#define NLM_CONFIG_PERFORMANCE_TESTS "enablePerformanceTests"
#define NLM_CONFIG_MEMORY_TESTS "enableMemoryTests"
#define NLM_CONFIG_EDGE_CASE_TESTS "enableEdgeCaseTests"
#define NLM_CONFIG_STRESS_TESTS "enableStressTests"
#define NLM_CONFIG_PROPERTY_TESTS "enablePropertyTests"
#define NLM_CONFIG_REGRESSION_TESTS "enableRegressionTests"
#define NLM_CONFIG_MAX_TEST_COUNT "maxTestCount"
#define NLM_CONFIG_MAX_TEST_TIME "maxTestTimeSeconds"
#define NLM_CONFIG_PARALLEL "parallelExecution"
#define NLM_CONFIG_PARALLEL_THREADS "parallelThreads"
#define NLM_CONFIG_REPORT_FORMAT "reportFormat"
#define NLM_CONFIG_REPORT_OUTPUT_DIR "reportOutputDirectory"
#define NLM_CONFIG_GENERATE_HTML "generateHTMLReports"
#define NLM_CONFIG_GENERATE_XML "generateXMLReports"
#define NLM_CONFIG_GENERATE_JSON "generateJSONReports"
#define NLM_CONFIG_GENERATE_JUNIT_XML "generateJUnitXML"
#define NLM_CONFIG_GENERATE_COVERAGE "generateCoverageReports"
#define NLM_CONFIG_VERBOSE "verboseOutput"
#define NLM_CONFIG_SHOW_PROGRESS "showProgress"
#define NLM_CONFIG_COLOR_OUTPUT "colorOutput"
#define NLM_CONFIG_PRINT_TEST_NAMES "printTestNames"
#define NLM_CONFIG_PRINT_TIMING "printTimingInfo"
#define NLM_CONFIG_PRINT_MEMORY "printMemoryUsage"
#define NLM_CONFIG_ENABLE_TIMER "enableTestTimer"
#define NLM_CONFIG_ENABLE_MEMORY_TRACKER "enableMemoryTracker"
#define NLM_CONFIG_ENABLE_RESOURCE_MONITOR "enableResourceMonitor"
#define NLM_CONFIG_ENABLE_DATA_GENERATOR "enableDataGenerator"
#define NLM_CONFIG_ENABLE_ASSERTION_UTILITIES "enableAssertionUtilities"
#define NLM_CONFIG_ENABLE_COMPARISON_UTILITIES "enableComparisonUtilities"
#define NLM_CONFIG_INTEGRATE_CMAKE "integrateWithCMake"
#define NLM_CONFIG_INTEGRATE_BUILD_SYSTEM "integrateWithBuildSystem"
#define NLM_CONFIG_CROSS_PLATFORM "crossPlatformSupport"
#define NLM_CONFIG_SUPPORTED_PLATFORMS "supportedPlatforms"
#define NLM_CONFIG_REQUIRE_CATCH2 "requireCatch2"
#define NLM_CONFIG_REQUIRE_BOOST "requireBoost"
#define NLM_CONFIG_REQUIRE_GOOGLE_TEST "requireGoogleTest"
#define NLM_CONFIG_ENABLE_MOCK "enableMockFramework"
#define NLM_CONFIG_MAINTAIN_BACKWARD_COMPATIBILITY "maintainBackwardCompatibility"
#define NLM_CONFIG_MIGRATE_GRADUALLY "migrateGradually"
#define NLM_CONFIG_RUN_LEGACY_TESTS "runLegacyTests"
#define NLM_CONFIG_RUN_MODERN_TESTS "runModernTests"
#define NLM_CONFIG_ENFORCE_NAMING "enforceTestNaming"
#define NLM_CONFIG_ENFORCE_ISOLATION "enforceTestIsolation"
#define NLM_CONFIG_ENFORCE_DETERMINISM "enforceTestDeterminism"
#define NLM_CONFIG_ENFORCE_REPEATABILITY "enforceTestRepeatability"
#define NLM_CONFIG_ENFORCE_OBSERVABILITY "enforceTestObservability"
#define NLM_CONFIG_ENFORCE_CONTROLLABILITY "enforceTestControllability"
#define NLM_CONFIG_OPTIMIZE_SPEED "optimizeForSpeed"
#define NLM_CONFIG_OPTIMIZE_MEMORY "optimizeForMemory"
#define NLM_CONFIG_OPTIMIZE_CPU "optimizeForCPU"
#define NLM_CONFIG_SANITIZE_MEMORY "sanitizeMemory"
#define NLM_CONFIG_SANITIZE_THREAD "sanitizeThread"
#define NLM_CONFIG_ADDRESS_SANITIZER "enableAddressSanitizer"
#define NLM_CONFIG_UB_SANITIZER "enableUndefinedBehaviorSanitizer"
#define NLM_CONFIG_LOG_LEVEL "logLevel"
#define NLM_CONFIG_LOG_FILE "logFile"
#define NLM_CONFIG_LOG_TO_FILE "logToFile"
#define NLM_CONFIG_LOG_TO_CONSOLE "logToConsole"

// Configuration default values
#define NLM_CONFIG_DEFAULT_ENABLED true
#define NLM_CONFIG_DEFAULT_NAME "NLM Test Framework"
#define NLM_CONFIG_DEFAULT_VERSION NLM_TEST_FRAMEWORK_VERSION
#define NLM_CONFIG_DEFAULT_UNIT_TESTS true
#define NLM_CONFIG_DEFAULT_INTEGRATION_TESTS true
#define NLM_CONFIG_DEFAULT_PERFORMANCE_TESTS false
#define NLM_CONFIG_DEFAULT_MEMORY_TESTS false
#define NLM_CONFIG_DEFAULT_EDGE_CASE_TESTS true
#define NLM_CONFIG_DEFAULT_STRESS_TESTS false
#define NLM_CONFIG_DEFAULT_PROPERTY_TESTS false
#define NLM_CONFIG_DEFAULT_REGRESSION_TESTS true
#define NLM_CONFIG_DEFAULT_MAX_TEST_COUNT 1000
#define NLM_CONFIG_DEFAULT_MAX_TEST_TIME 300
#define NLM_CONFIG_DEFAULT_PARALLEL false
#define NLM_CONFIG_DEFAULT_PARALLEL_THREADS 0
#define NLM_CONFIG_DEFAULT_REPORT_FORMAT "text"
#define NLM_CONFIG_DEFAULT_REPORT_OUTPUT_DIR "./test_reports"
#define NLM_CONFIG_DEFAULT_GENERATE_HTML true
#define NLM_CONFIG_DEFAULT_GENERATE_XML true
#define NLM_CONFIG_DEFAULT_GENERATE_JSON false
#define NLM_CONFIG_DEFAULT_GENERATE_JUNIT_XML true
#define NLM_CONFIG_DEFAULT_GENERATE_COVERAGE false
#define NLM_CONFIG_DEFAULT_VERBOSE false
#define NLM_CONFIG_DEFAULT_SHOW_PROGRESS true
#define NLM_CONFIG_DEFAULT_COLOR_OUTPUT true
#define NLM_CONFIG_DEFAULT_PRINT_TEST_NAMES true
#define NLM_CONFIG_DEFAULT_PRINT_TIMING true
#define NLM_CONFIG_DEFAULT_PRINT_MEMORY false
#define NLM_CONFIG_DEFAULT_ENABLE_TIMER true
#define NLM_CONFIG_DEFAULT_ENABLE_MEMORY_TRACKER true
#define NLM_CONFIG_DEFAULT_ENABLE_RESOURCE_MONITOR true
#define NLM_CONFIG_DEFAULT_ENABLE_DATA_GENERATOR true
#define NLM_CONFIG_DEFAULT_ENABLE_ASSERTION_UTILITIES true
#define NLM_CONFIG_DEFAULT_ENABLE_COMPARISON_UTILITIES true
#define NLM_CONFIG_DEFAULT_INTEGRATE_CMAKE true
#define NLM_CONFIG_DEFAULT_INTEGRATE_BUILD_SYSTEM true
#define NLM_CONFIG_DEFAULT_CROSS_PLATFORM true
#define NLM_CONFIG_DEFAULT_REQUIRE_CATCH2 false
#define NLM_CONFIG_DEFAULT_REQUIRE_BOOST false
#define NLM_CONFIG_DEFAULT_REQUIRE_GOOGLE_TEST false
#define NLM_CONFIG_DEFAULT_ENABLE_MOCK false
#define NLM_CONFIG_DEFAULT_MAINTAIN_BACKWARD_COMPATIBILITY true
#define NLM_CONFIG_DEFAULT_MIGRATE_GRADUALLY true
#define NLM_CONFIG_DEFAULT_RUN_LEGACY_TESTS true
#define NLM_CONFIG_DEFAULT_RUN_MODERN_TESTS true
#define NLM_CONFIG_DEFAULT_ENFORCE_NAMING true
#define NLM_CONFIG_DEFAULT_ENFORCE_ISOLATION true
#define NLM_CONFIG_DEFAULT_ENFORCE_DETERMINISM true
#define NLM_CONFIG_DEFAULT_ENFORCE_REPEATABILITY true
#define NLM_CONFIG_DEFAULT_ENFORCE_OBSERVABILITY true
#define NLM_CONFIG_DEFAULT_ENFORCE_CONTROLLABILITY true
#define NLM_CONFIG_DEFAULT_OPTIMIZE_SPEED true
#define NLM_CONFIG_DEFAULT_OPTIMIZE_MEMORY false
#define NLM_CONFIG_DEFAULT_OPTIMIZE_CPU true
#define NLM_CONFIG_DEFAULT_SANITIZE_MEMORY false
#define NLM_CONFIG_DEFAULT_SANITIZE_THREAD false
#define NLM_CONFIG_DEFAULT_ADDRESS_SANITIZER false
#define NLM_CONFIG_DEFAULT_UB_SANITIZER false
#define NLM_CONFIG_DEFAULT_LOG_LEVEL "info"
#define NLM_CONFIG_DEFAULT_LOG_FILE "test_framework.log"
#define NLM_CONFIG_DEFAULT_LOG_TO_FILE true
#define NLM_CONFIG_DEFAULT_LOG_TO_CONSOLE true

#endif // NLM_TEST_FRAMEWORK_CONFIG_H
