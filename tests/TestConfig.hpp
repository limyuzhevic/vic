#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>

/**
 * Test framework configuration for NLM
 */

struct TestConfig {
    // General configuration
    std::string name;
    std::string description;
    bool enabled = true;
    std::string log_level = "INFO";
    bool verbose = false;
    bool show_progress = true;
    
    // Test framework configuration
    struct FrameworkConfig {
        bool enable_assertions = true;
        bool enable_benchmarks = true;
        bool enable_profiling = false;
        std::string assertion_failure_behavior = "THROW_EXCEPTION"; // "THROW_EXCEPTION", "ABORT", "LOG_ONLY"
        std::string output_format = "CONSOLE"; // "CONSOLE", "JSON", "XML"
        std::string output_file = "";
        bool color_output = true;
        bool parallel_execution = false;
        size_t max_threads = std::thread::hardware_concurrency();
        double timeout_per_test = 30.0; // seconds
    } framework;
    
    // Test organization configuration
    struct TestOrganizationConfig {
        std::vector<std::string> test_categories = {"Unit", "Integration", "Performance", "General"};
        std::map<std::string, std::string> category_descriptions;
        bool organize_by_type = true;
        bool include_nested_tests = false;
        std::string test_pattern = "*"; // Wildcard pattern for test discovery
    } organization;
    
    // Performance configuration
    struct PerformanceConfig {
        bool enable_performance_monitoring = true;
        bool enable_profiling = true;
        double memory_limit_mb = 1024.0;
        double cpu_limit_percent = 100.0;
        size_t max_memory_samples = 100;
        std::vector<std::string> profiling_tools = {"TIMER", "MEMORY", "CPU"};
    } performance;
    
    // Test data configuration
    struct DataConfig {
        bool generate_random_data = true;
        unsigned int random_seed = std::chrono::system_clock::now().time_since_epoch().count();
        bool persist_test_data = false;
        std::string data_directory = "./test_data";
        size_t max_data_size_mb = 100;
    } data;
    
    // Reporting configuration
    struct ReportingConfig {
        bool enable_console_output = true;
        bool enable_file_output = false;
        std::string console_output_format = "VERBOSE"; // "QUIET", "SUMMARY", "VERBOSE"
        std::string file_output_format = "JSON"; // "JSON", "XML", "CSV"
        std::string report_file = "test_report.json";
        bool generate_html_report = true;
        bool generate_xml_report = false;
        bool generate_csv_report = false;
        bool generate_pdf_report = false;
    } reporting;
    
    // Coverage configuration
    struct CoverageConfig {
        bool enable_coverage = false;
        double coverage_threshold = 80.0; // percentage
        std::vector<std::string> excluded_files = {
            "*test*.cpp",
            "*test*.hpp",
            "*_test.cpp",
            "*_test.hpp"
        };
        bool generate_coverage_report = true;
        std::string coverage_report_file = "coverage_report.html";
    } coverage;
    
    // Integration configuration
    struct IntegrationConfig {
        bool enable_integration_tests = true;
        bool run_integration_tests_with_unit = false;
        bool skip_integration_if_unit_failed = false;
        std::vector<std::string> integration_test_categories = {"Phase2Integration", "Phase6Integration", "EndToEnd"};
        bool enable_experiment_integration = true;
    } integration;
    
    // Environment configuration
    struct EnvironmentConfig {
        std::string platform = "unknown";
        std::string compiler = "unknown";
        std::string build_type = "Release";
        bool use_container = false;
        std::map<std::string, std::string> environment_variables;
    } environment;
    
    // Test execution configuration
    struct ExecutionConfig {
        bool enable_test_reuse = false;
        bool enable_test_caching = true;
        size_t test_cache_size = 100;
        bool enable_test_optimization = true;
        bool enable_early_exit = false;
        size_t max_concurrent_tests = 4;
        bool enable_test_retry = true;
        size_t max_test_retries = 3;
    } execution;
    
    // Analysis configuration
    struct AnalysisConfig {
        bool enable_statistical_analysis = true;
        bool enable_performance_analysis = true;
        bool enable_regression_analysis = true;
        std::vector<std::string> analysis_metrics = {"mean", "median", "stddev", "min", "max", "range"};
        bool generate_analysis_report = true;
        std::string analysis_report_file = "analysis_report.json";
    } analysis;
};

// Default test configuration
inline TestConfig getDefaultTestConfig() {
    TestConfig config;
    
    config.name = "NLM Test Framework";
    config.description = "Comprehensive test framework for NLM (Neural Learning Machine)";
    config.enabled = true;
    config.log_level = "INFO";
    config.verbose = false;
    config.show_progress = true;
    
    // Framework configuration
    config.framework.enable_assertions = true;
    config.framework.enable_benchmarks = true;
    config.framework.enable_profiling = false;
    config.framework.assertion_failure_behavior = "THROW_EXCEPTION";
    config.framework.output_format = "CONSOLE";
    config.framework.output_file = "";
    config.framework.color_output = true;
    config.framework.parallel_execution = false;
    config.framework.max_threads = std::thread::hardware_concurrency();
    config.framework.timeout_per_test = 30.0;
    
    // Test organization configuration
    config.organization.test_categories = {"Unit", "Integration", "Performance", "General"};
    config.organization.category_descriptions = {
        {"Unit", "Unit tests for individual components"},
        {"Integration", "Integration tests for system components"},
        {"Performance", "Performance benchmarks and profiling"},
        {"General", "General utility and integration tests"}
    };
    config.organization.organize_by_type = true;
    config.organization.include_nested_tests = false;
    config.organization.test_pattern = "*";
    
    // Performance configuration
    config.performance.enable_performance_monitoring = true;
    config.performance.enable_profiling = true;
    config.performance.memory_limit_mb = 1024.0;
    config.performance.cpu_limit_percent = 100.0;
    config.performance.max_memory_samples = 100;
    config.performance.profiling_tools = {"TIMER", "MEMORY", "CPU"};
    
    // Test data configuration
    config.data.generate_random_data = true;
    config.data.random_seed = std::chrono::system_clock::now().time_since_epoch().count();
    config.data.persist_test_data = false;
    config.data.data_directory = "./test_data";
    config.data.max_data_size_mb = 100;
    
    // Reporting configuration
    config.reporting.enable_console_output = true;
    config.reporting.enable_file_output = false;
    config.reporting.console_output_format = "VERBOSE";
    config.reporting.file_output_format = "JSON";
    config.reporting.report_file = "test_report.json";
    config.reporting.generate_html_report = true;
    config.reporting.generate_xml_report = false;
    config.reporting.generate_csv_report = false;
    config.reporting.generate_pdf_report = false;
    
    // Coverage configuration
    config.coverage.enable_coverage = false;
    config.coverage.coverage_threshold = 80.0;
    config.coverage.excluded_files = {"*test*.cpp", "*test*.hpp", "*_test.cpp", "*_test.hpp"};
    config.coverage.generate_coverage_report = true;
    config.coverage.coverage_report_file = "coverage_report.html";
    
    // Integration configuration
    config.integration.enable_integration_tests = true;
    config.integration.run_integration_tests_with_unit = false;
    config.integration.skip_integration_if_unit_failed = false;
    config.integration.integration_test_categories = {"Phase2Integration", "Phase6Integration", "EndToEnd"};
    config.integration.enable_experiment_integration = true;
    
    // Environment configuration
    config.environment.platform = "unknown";
    config.environment.compiler = "unknown";
    config.environment.build_type = "Release";
    config.environment.use_container = false;
    
    // Test execution configuration
    config.execution.enable_test_reuse = false;
    config.execution.enable_test_caching = true;
    config.execution.test_cache_size = 100;
    config.execution.enable_test_optimization = true;
    config.execution.enable_early_exit = false;
    config.execution.max_concurrent_tests = 4;
    config.execution.enable_test_retry = true;
    config.execution.max_test_retries = 3;
    
    // Analysis configuration
    config.analysis.enable_statistical_analysis = true;
    config.analysis.enable_performance_analysis = true;
    config.analysis.enable_regression_analysis = true;
    config.analysis.analysis_metrics = {"mean", "median", "stddev", "min", "max", "range"};
    config.analysis.generate_analysis_report = true;
    config.analysis.analysis_report_file = "analysis_report.json";
    
    return config;
}

// Environment detection
inline std::string detectPlatform() {
#ifdef __linux__
    return "Linux";
#elif __APPLE__
    return "macOS";
#elif _WIN32
    return "Windows";
#else
    return "Unknown";
#endif
}

inline std::string detectCompiler() {
#ifdef __GNUC__
    return "GCC";
#elif __clang__
    return "Clang";
#elif _MSC_VER
    return "MSVC";
#else
    return "Unknown";
#endif
}

inline std::string getCurrentBuildType() {
#ifdef NDEBUG
    return "Release";
#else
    return "Debug";
#endif
}

#endif // TEST_CONFIG_HPP