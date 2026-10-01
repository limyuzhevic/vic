// Infrastructure Headers
// Test framework integration and cross-platform support headers

#ifndef NLM_INFRASTRUCTURE_TEST_FRAMEWORK_HPP
#define NLM_INFRASTRUCTURE_TEST_FRAMEWORK_HPP

#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace infrastructure {

    // Forward declarations
    class PlatformDetector;
    class CMakeTestIntegration;
    class CoverageReporter;
    class TestEnvironmentManager;
    class TestFrameworkIntegration;

    // Platform detection interface
    class PlatformDetector {
    public:
        struct PlatformInfo {
            std::string name;
            std::string version;
            std::string architecture;
            std::string compiler;
            std::string build_type;
            std::string timestamp;
        };

        static PlatformInfo detect();
        static std::string get_platform_string();
    };

    // CMake test integration interface
    class CMakeTestIntegration {
    public:
        struct CMakeTestInfo {
            std::string target_name;
            std::string test_type;
            std::string command;
            std::vector<std::string> dependencies;
            bool enabled;
            double timeout;
            std::string environment;
        };

        CMakeTestIntegration();
        std::vector<CMakeTestInfo> get_enabled_tests(const std::string& environment = "ci");
        CMakeTestInfo get_test(const std::string& test_type);
        std::string generate_cmake_test_script(const std::string& environment = "ci");
        bool run_test_suite(const std::string& environment = "ci");

    private:
        bool simulate_test_execution(const CMakeTestInfo& test);
        std::map<std::string, CMakeTestInfo> standard_tests_;
    };

    // Coverage reporter interface
    class CoverageReporter {
    public:
        struct CoverageInfo {
            std::string file_path;
            double line_coverage;
            double branch_coverage;
            double function_coverage;
            size_t total_lines;
            size_t executed_lines;
            size_t total_branches;
            size_t taken_branches;
            size_t total_functions;
            size_t called_functions;
        };

        struct CoverageReport {
            std::string generated_at;
            std::string platform_info;
            std::string git_commit;
            double overall_coverage;
            std::vector<CoverageInfo> file_coverage;
            std::map<std::string, double> coverage_by_type;
            std::vector<std::string> uncovered_files;
            std::vector<std::string> uncovered_lines;
        };

        CoverageReporter() = default;
        CoverageReport generate_report(const std::string& coverage_data_file = "coverage_data.txt");
        bool export_html_report(const CoverageReport& report, const std::string& filename);
        bool export_json_report(const CoverageReport& report, const std::string& filename);
    };

    // Test environment manager interface
    class TestEnvironmentManager {
    public:
        struct TestEnvironment {
            std::string name;
            std::string platform;
            std::map<std::string, std::string> environment_variables;
            std::vector<std::string> dependencies;
            std::chrono::system_clock::time_point created_at;
            bool enabled;
        };

        TestEnvironmentManager();
        TestEnvironment get_environment(const std::string& name);
        bool setup_environment(const std::string& name);
        void cleanup_environment(const std::string& name);
        std::vector<std::string> get_available_environments();
        bool is_environment_available(const std::string& name);

    private:
        bool check_dependency(const std::string& dependency);
        std::map<std::string, TestEnvironment> environments_;
    };

    // Test framework integration interface
    class TestFrameworkIntegration {
    public:
        struct IntegrationConfig {
            std::string test_environment;
            std::string coverage_report_file;
            std::string regression_baseline_file;
            std::string regression_current_file;
            std::string regression_report_file;
            bool enable_cmake_integration;
            bool enable_cross_platform_support;
            bool enable_performance_monitoring;
            bool enable_regression_testing;
        };

        TestFrameworkIntegration(const IntegrationConfig& config);
        void initialize();
        void finalize();
        void generate_final_reports();
        bool run_integrated_test_suite();

    private:
        IntegrationConfig config_;
        std::unique_ptr<PlatformDetector> platform_detector_;
        std::unique_ptr<CMakeTestIntegration> cmake_integration_;
        std::unique_ptr<CoverageReporter> coverage_reporter_;
        std::unique_ptr<TestEnvironmentManager> environment_manager_;
    };

} // namespace infrastructure

#endif // NLM_INFRASTRUCTURE_TEST_FRAMEWORK_HPP

// Test framework entry point
#ifndef NLM_TEST_FRAMEWORK_ENTRY_HPP
#define NLM_TEST_FRAMEWORK_ENTRY_HPP

#include <string>

namespace test_framework {
    void initialize_test_framework();
}

#endif // NLM_TEST_FRAMEWORK_ENTRY_HPP

// Platform detector implementation
#ifndef NLM_PLATFORM_DETECTOR_HPP
#define NLM_PLATFORM_DETECTOR_HPP

#include <string>
#include <sstream>
#include <sys/utsname.h>
#include <chrono>
#include <iomanip>
#include <ctime>

inline std::string get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

inline std::string get_compiler_version() {
#ifdef __VERSION__
    return __VERSION__;
#else
    return "Unknown";
#endif
}

inline infrastructure::PlatformDetector::PlatformInfo infrastructure::PlatformDetector::detect() {
    PlatformInfo info;
    
    // Get OS information
    struct utsname uname_data;
    if (uname(&uname_data) == 0) {
        info.name = uname_data.sysname;
        info.version = uname_data.release;
        info.architecture = uname_data.machine;
    }
    
    // Get compiler information
    info.compiler = get_compiler_version();
    
    // Get build type
#ifdef NDEBUG
    info.build_type = "Release";
#else
    info.build_type = "Debug";
#endif
    
    // Get timestamp
    info.timestamp = get_current_timestamp();
    
    return info;
}

inline std::string infrastructure::PlatformDetector::get_platform_string() {
    PlatformInfo info = detect();
    std::stringstream ss;
    ss << info.name << " " << info.version << " (" << info.architecture << ")";
    return ss.str();
}

#endif // NLM_PLATFORM_DETECTOR_HPP

// CMake test integration implementation
#ifndef NLM_CMAKE_TEST_INTEGRATION_HPP
#define NLM_CMAKE_TEST_INTEGRATION_HPP

#include <iostream>
#include <fstream>
#include <thread>
#include <cstdlib>

inline infrastructure::CMakeTestIntegration::CMakeTestIntegration() {
    // Register standard test targets
    register_standard_tests();
}

inline void infrastructure::CMakeTestIntegration::register_standard_tests() {
    // Register unit tests
    CMakeTestInfo unit_test;
    unit_test.target_name = "nlm_test";
    unit_test.test_type = "unit";
    unit_test.command = "nlm_test";
    unit_test.dependencies = {};
    unit_test.enabled = true;
    unit_test.timeout = 300.0;
    unit_test.environment = "ci";
    standard_tests_["unit"] = unit_test;
    
    // Register property-based tests
    CMakeTestInfo property_test;
    property_test.target_name = "nlm_property_tests";
    property_test.test_type = "property";
    property_test.command = "nlm_property_tests";
    property_test.dependencies = {"nlm_test"};
    property_test.enabled = true;
    property_test.timeout = 600.0;
    property_test.environment = "ci";
    standard_tests_["property"] = property_test;
    
    // Register performance benchmarks
    CMakeTestInfo performance_test;
    performance_test.target_name = "nlm_benchmarks";
    performance_test.test_type = "performance";
    performance_test.command = "nlm_benchmarks";
    performance_test.dependencies = {};
    performance_test.enabled = true;
    performance_test.timeout = 1800.0;
    performance_test.environment = "ci";
    standard_tests_["performance"] = performance_test;
    
    // Register integration tests
    CMakeTestInfo integration_test;
    integration_test.target_name = "nlm_integration_tests";
    integration_test.test_type = "integration";
    integration_test.command = "nlm_integration_tests";
    integration_test.dependencies = {"nlm_test", "nlm_property_tests"};
    integration_test.enabled = true;
    integration_test.timeout = 900.0;
    integration_test.environment = "ci";
    standard_tests_["integration"] = integration_test;
    
    // Register regression tests
    CMakeTestInfo regression_test;
    regression_test.target_name = "nlm_regression_tests";
    regression_test.test_type = "regression";
    regression_test.command = "nlm_regression_tests";
    regression_test.dependencies = {"nlm_test", "nlm_benchmarks"};
    regression_test.enabled = true;
    regression_test.timeout = 600.0;
    regression_test.environment = "ci";
    standard_tests_["regression"] = regression_test;
    
    // Register coverage reporting
    CMakeTestInfo coverage_test;
    coverage_test.target_name = "nlm_coverage_report";
    coverage_test.test_type = "coverage";
    coverage_test.command = "nlm_coverage_report";
    coverage_test.dependencies = {"nlm_test"};
    coverage_test.enabled = true;
    coverage_test.timeout = 300.0;
    coverage_test.environment = "ci";
    standard_tests_["coverage"] = coverage_test;
}

inline std::vector<infrastructure::CMakeTestIntegration::CMakeTestInfo>
infrastructure::CMakeTestIntegration::get_enabled_tests(const std::string& environment) {
    std::vector<CMakeTestInfo> enabled_tests;
    
    for (const auto& pair : standard_tests_) {
        if (pair.second.enabled && pair.second.environment == environment) {
            enabled_tests.push_back(pair.second);
        }
    }
    
    return enabled_tests;
}

inline infrastructure::CMakeTestIntegration::CMakeTestInfo
infrastructure::CMakeTestIntegration::get_test(const std::string& test_type) {
    auto it = standard_tests_.find(test_type);
    if (it != standard_tests_.end()) {
        return it->second;
    }
    
    CMakeTestInfo default_test;
    default_test.target_name = "unknown";
    default_test.test_type = test_type;
    default_test.command = "";
    default_test.enabled = false;
    default_test.timeout = 300.0;
    default_test.environment = "ci";
    
    return default_test;
}

inline std::string infrastructure::CMakeTestIntegration::generate_cmake_test_script(
    const std::string& environment) {
    
    std::stringstream ss;
    ss << "# NLM Test Suite - " << environment << " environment" << std::endl;
    ss << "# Generated by CMake Test Integration Framework" << std::endl;
    ss << std::endl;
    
    auto tests = get_enabled_tests(environment);
    
    for (const auto& test : tests) {
        ss << "# " << test.test_type << " test: " << test.target_name << std::endl;
        ss << "add_test(NAME " << test.target_name << " COMMAND " << test.command << ")" << std::endl;
        ss << "set_tests_properties(" << test.target_name << " PROPERTIES " << std::endl;
        ss << "    TIMEOUT " << test.timeout << std::endl;
        ss << "    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}" << std::endl;
        ss << "    ENVIRONMENT \"LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/lib\"" << std::endl;
        ss << ")" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

inline bool infrastructure::CMakeTestIntegration::run_test_suite(const std::string& environment) {
    std::cout << "=== Running NLM Test Suite - " << environment << " ===" << std::endl;
    
    auto tests = get_enabled_tests(environment);
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& test : tests) {
        std::cout << std::endl;
        std::cout << "Running test: " << test.target_name << " (" << test.test_type << ")" << std::endl;
        
        bool test_result = simulate_test_execution(test);
        
        if (test_result) {
            passed++;
            std::cout << "[PASS] " << test.target_name << std::endl;
        } else {
            failed++;
            std::cout << "[FAIL] " << test.target_name << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== Test Suite Summary ===" << std::endl;
    std::cout << "Total tests: " << tests.size() << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    
    return (failed == 0);
}

#endif // NLM_CMAKE_TEST_INTEGRATION_HPP

// Test environment manager implementation
#ifndef NLM_TEST_ENVIRONMENT_MANAGER_HPP
#define NLM_TEST_ENVIRONMENT_MANAGER_HPP

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sys/utsname.h>

inline infrastructure::TestEnvironmentManager::TestEnvironmentManager() {
    create_default_environments();
}

inline void infrastructure::TestEnvironmentManager::create_default_environments() {
    // CI environment
    TestEnvironment ci_env;
    ci_env.name = "ci";
    ci_env.platform = PlatformDetector::get_platform_string();
    ci_env.environment_variables = {
        {"NLM_TEST_MODE", "ci"},
        {"NLM_PERFORMANCE_THRESHOLD", "5.0"},
        {"NLM_COVERAGE_THRESHOLD", "80.0"},
        {"CI", "true"},
        {"GITHUB_ACTIONS", "true"}
    };
    ci_env.dependencies = {"cmake", "make", "g++"};
    ci_env.created_at = std::chrono::system_clock::now();
    ci_env.enabled = true;
    environments_["ci"] = ci_env;
    
    // Local development environment
    TestEnvironment local_env;
    local_env.name = "local";
    local_env.platform = PlatformDetector::get_platform_string();
    local_env.environment_variables = {
        {"NLM_TEST_MODE", "development"},
        {"NLM_PERFORMANCE_THRESHOLD", "10.0"},
        {"NLM_COVERAGE_THRESHOLD", "70.0"},
        {"CI", "false"},
        {"GITHUB_ACTIONS", "false"}
    };
    local_env.dependencies = {"cmake", "make", "g++"};
    local_env.created_at = std::chrono::system_clock::now();
    local_env.enabled = true;
    environments_["local"] = local_env;
}

inline infrastructure::TestEnvironmentManager::TestEnvironment
infrastructure::TestEnvironmentManager::get_environment(const std::string& name) {
    auto it = environments_.find(name);
    if (it != environments_.end()) {
        return it->second;
    }
    
    TestEnvironment default_env;
    default_env.name = name;
    default_env.platform = PlatformDetector::get_platform_string();
    default_env.environment_variables = {
        {"NLM_TEST_MODE", "unknown"},
        {"NLM_PERFORMANCE_THRESHOLD", "5.0"},
        {"NLM_COVERAGE_THRESHOLD", "80.0"}
    };
    default_env.dependencies = {};
    default_env.created_at = std::chrono::system_clock::now();
    default_env.enabled = false;
    
    return default_env;
}

inline bool infrastructure::TestEnvironmentManager::setup_environment(const std::string& name) {
    TestEnvironment env = get_environment(name);
    
    if (!env.enabled) {
        std::cout << "Environment \"" << name << "\" is disabled" << std::endl;
        return false;
    }
    
    std::cout << "Setting up environment: " << name << std::endl;
    std::cout << "Platform: " << env.platform << std::endl;
    
    for (const auto& pair : env.environment_variables) {
        std::cout << "  Setting: " << pair.first << " = " << pair.second << std::endl;
        setenv(pair.first.c_str(), pair.second.c_str(), 1);
    }
    
    std::cout << "Checking dependencies:" << std::endl;
    for (const auto& dependency : env.dependencies) {
        std::cout << "  Checking: " << dependency << "... ";
        if (check_dependency(dependency)) {
            std::cout << "Found" << std::endl;
        } else {
            std::cout << "Not found" << std::endl;
            return false;
        }
    }
    
    std::cout << "Environment setup completed successfully" << std::endl;
    return true;
}

inline void infrastructure::TestEnvironmentManager::cleanup_environment(const std::string& name) {
    TestEnvironment env = get_environment(name);
    
    std::cout << "Cleaning up environment: " << name << std::endl;
    
    for (const auto& pair : env.environment_variables) {
        unsetenv(pair.first.c_str());
    }
    
    std::cout << "Environment cleanup completed" << std::endl;
}

inline std::vector<std::string> infrastructure::TestEnvironmentManager::get_available_environments() {
    std::vector<std::string> environments;
    for (const auto& pair : environments_) {
        if (pair.second.enabled) {
            environments.push_back(pair.first);
        }
    }
    return environments;
}

inline bool infrastructure::TestEnvironmentManager::is_environment_available(const std::string& name) {
    auto it = environments_.find(name);
    return (it != environments_.end()) && it->second.enabled;
}

inline bool infrastructure::TestEnvironmentManager::check_dependency(const std::string& dependency) {
    if (dependency == "cmake") {
        return std::system("cmake --version > /dev/null 2>&1") == 0;
    } else if (dependency == "make") {
        return std::system("make --version > /dev/null 2>&1") == 0;
    } else if (dependency == "g++") {
        return std::system("g++ --version > /dev/null 2>&1") == 0;
    }
    return true;
}

#endif // NLM_TEST_ENVIRONMENT_MANAGER_HPP

// Coverage reporter implementation
#ifndef NLM_COVERAGE_REPORTER_HPP
#define NLM_COVERAGE_REPORTER_HPP

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

inline bool infrastructure::CoverageReporter::export_html_report(
    const CoverageReport& report, const std::string& filename) {
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open HTML report file: " << filename << std::endl;
        return false;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html><head><title>NLM Test Coverage Report</title>" << std::endl;
    file << "<style>body { font-family: Arial, sans-serif; margin: 20px; } "
         << "h1 { color: #333; } "
         << "h2 { color: #666; border-bottom: 1px solid #ccc; padding-bottom: 5px; } "
         << "table { border-collapse: collapse; width: 100%; margin-bottom: 20px; } "
         << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; } "
         << "th { background-color: #f2f2f2; } "
         << ".coverage-high { background-color: #ccffcc; } "
         << ".coverage-medium { background-color: #ffffcc; } "
         << ".coverage-low { background-color: #ffcccc; } "
         << "" << std::endl;
    file << "</style></head><body>" << std::endl;
    
    file << "<h1>NLM Test Coverage Report</h1>" << std::endl;
    file << "<p><strong>Generated:</strong> " << report.generated_at << "</p>" << std::endl;
    file << "<p><strong>Platform:</strong> " << report.platform_info << "</p>" << std::endl;
    file << "<p><strong>Overall Coverage:</strong> " << std::fixed << std::setprecision(2)
         << (report.overall_coverage * 100) << "%</p>" << std::endl;
    file << "<p><strong>Uncovered Files:</strong> " << report.uncovered_files.size() << "</p>" << std::endl;
    
    file << "<h2>File Coverage Details</h2>" << std::endl;
    file << "<table>" << std::endl;
    file << "<tr><th>File</th><th>Line Coverage</th><th>Branch Coverage</th><th>Function Coverage</th></tr>" << std::endl;
    
    for (const auto& coverage : report.file_coverage) {
        file << "<tr class=\"" << get_coverage_class(coverage.line_coverage) << "\">" << std::endl;
        file << "<td>" << coverage.file_path << "</td>" << std::endl;
        file << "<td>" << std::fixed << std::setprecision(2) << (coverage.line_coverage * 100) << "%</td>" << std::endl;
        file << "<td>" << std::fixed << std::setprecision(2) << (coverage.branch_coverage * 100) << "%</td>" << std::endl;
        file << "<td>" << std::fixed << std::setprecision(2) << (coverage.function_coverage * 100) << "%</td>" << std::endl;
        file << "</tr>" << std::endl;
    }
    
    file << "</table>" << std::endl;
    file << "</body></html>" << std::endl;
    file.close();
    
    return true;
}

inline bool infrastructure::CoverageReporter::export_json_report(
    const CoverageReport& report, const std::string& filename) {
    
    nlohmann::json json_data;
    
    json_data["generated_at"] = report.generated_at;
    json_data["platform_info"] = report.platform_info;
    json_data["git_commit"] = report.git_commit;
    json_data["overall_coverage"] = report.overall_coverage;
    json_data["uncovered_files"] = report.uncovered_files;
    json_data["uncovered_lines"] = report.uncovered_lines;
    
    nlohmann::json coverage_by_type_json;
    for (const auto& pair : report.coverage_by_type) {
        coverage_by_type_json[pair.first] = pair.second;
    }
    json_data["coverage_by_type"] = coverage_by_type_json;
    
    nlohmann::json file_coverage_json;
    for (const auto& coverage : report.file_coverage) {
        nlohmann::json file_item;
        file_item["file_path"] = coverage.file_path;
        file_item["line_coverage"] = coverage.line_coverage;
        file_item["branch_coverage"] = coverage.branch_coverage;
        file_item["function_coverage"] = coverage.function_coverage;
        file_item["total_lines"] = coverage.total_lines;
        file_item["executed_lines"] = coverage.executed_lines;
        file_item["total_branches"] = coverage.total_branches;
        file_item["taken_branches"] = coverage.taken_branches;
        file_item["total_functions"] = coverage.total_functions;
        file_item["called_functions"] = coverage.called_functions;
        
        file_coverage_json.push_back(file_item);
    }
    json_data["file_coverage"] = file_coverage_json;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << json_data.dump(2);
    file.close();
    
    return true;
}

inline std::string infrastructure::CoverageReporter::get_coverage_class(double coverage) {
    if (coverage >= 0.90) {
        return "coverage-high";
    } else if (coverage >= 0.70) {
        return "coverage-medium";
    } else {
        return "coverage-low";
    }
}

#endif // NLM_COVERAGE_REPORTER_HPP

// Test framework integration implementation
#ifndef NLM_TEST_FRAMEWORK_INTEGRATION_HPP
#define NLM_TEST_FRAMEWORK_INTEGRATION_HPP

#include <iostream>
#include <fstream>

inline void test_framework::initialize_test_framework() {
    std::cout << "=== NLM Comprehensive Testing Framework ===" << std::endl;
    std::cout << "Initializing enhanced test infrastructure..." << std::endl;
    
    // Default configuration
    infrastructure::TestFrameworkIntegration::IntegrationConfig config;
    config.test_environment = "ci";
    config.coverage_report_file = "test_coverage_report.html";
    config.regression_baseline_file = "regression_baseline.json";
    config.regression_current_file = "regression_current.json";
    config.regression_report_file = "regression_report.html";
    config.enable_cmake_integration = true;
    config.enable_cross_platform_support = true;
    config.enable_performance_monitoring = true;
    config.enable_regression_testing = true;
    
    // Create and initialize test framework integration
    infrastructure::TestFrameworkIntegration framework(config);
    framework.initialize();
    
    std::cout << std::endl;
    std::cout << "=== Test Framework Initialization Complete ===" << std::endl;
}

#endif // NLM_TEST_FRAMEWORK_ENTRY_HPP

// Ensure nlohmann/json is available
#ifndef NLOHMANN_JSON_HPP
#include <nlohmann/json.hpp>
#endif

#endif // NLM_INFRASTRUCTURE_TEST_FRAMEWORK_HPP

// Final implementation file
#include "test_framework_integration.cpp"
#include "coverage_reporter.cpp"
#include "test_environment_manager.cpp"
#include "cmake_test_integration.cpp"
#include "platform_detector.cpp"