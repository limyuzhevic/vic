// NLM Enhanced Test Configuration System
// Phase 7: Comprehensive Test Suite
// Test configuration management and parameter handling

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

// Test configuration categories
enum class TestCategory {
    MEMORY_LEAK,
    STRESS,
    PERFORMANCE,
    EDGE_CASE,
    INTEGRATION,
    CROSS_PLATFORM,
    REGRESSION,
    UNIT,
    SYSTEM,
    NETWORK,
    BRAIN,
    AGENT,
    WORLD,
    PLASTICITY,
    NEUROMODULATION,
    DEVELOPMMENT,
    PREDICTION,
   COGNITION,
    SENSOR,
    MOTOR,
    ENVIRONMENT,
    EXPERIMENT
};

// Test priority levels
enum class TestPriority {
    CRITICAL,
    HIGH,
    MEDIUM,
    LOW,
    MINIMAL
};

// Test result status
enum class TestStatus {
    NOT_STARTED,
    RUNNING,
    PASSED,
    FAILED,
    SKIPPED,
    TIMEOUT,
    ERROR
};

// Performance measurement modes
enum class PerformanceMode {
    MICROSECONDS,
    MILLISECONDS,
    SECONDS,
    OPERATIONS_PER_SECOND,
    MEMORY_BYTES,
    BANDWIDTH_MBPS
};

// Memory leak detection modes
enum class MemoryLeakMode {
    TRACK_ALLOCS,
    TRACK_DEALLOCS,
    FULL_STACK_TRACE,
    HEAP_PROFILING,
    ADDRESS_SANITIZER,
    VALGRIND
};

// Test platform compatibility
enum class Platform {
    ANY,
    LINUX,
    WINDOWS,
    MACOS,
    ANDROID,
    IOS,
    RASPBIAN
};

// Test configuration parameter
struct TestParameter {
    std::string name;
    std::string value;
    std::string type;
    std::string description;
    bool required = false;
    std::vector<std::string> allowed_values;
    double min_value = -std::numeric_limits<double>::max();
    double max_value = std::numeric_limits<double>::max();
    
    TestParameter(const std::string& param_name, const std::string& param_value,
                  const std::string& param_type = "string",
                  const std::string& param_desc = "",
                  bool param_required = false)
        : name(param_name), value(param_value), type(param_type),
          description(param_desc), required(param_required) {}
};

// Test scenario configuration
struct TestScenario {
    std::string name;
    std::string description;
    TestCategory category;
    TestPriority priority;
    std::vector<TestParameter> parameters;
    std::vector<std::string> tags;
    bool enabled = true;
    double timeout = 30.0;
    std::vector<std::string> prerequisites;
    std::vector<std::string> depends_on;
    Platform platform = Platform::ANY;
    std::string platform_specific_config;
    
    TestScenario(const std::string& scenario_name,
                 const std::string& scenario_desc,
                 TestCategory scenario_category,
                 TestPriority scenario_priority)
        : name(scenario_name), description(scenario_desc),
          category(scenario_category), priority(scenario_priority) {}
};

// Performance benchmark configuration
struct PerformanceBenchmark {
    std::string name;
    std::string description;
    PerformanceMode mode;
    double threshold_value;
    std::string threshold_unit;
    bool should_fail_if_exceeded = true;
    std::vector<std::string> metrics_to_measure;
    int iterations = 100;
    bool warm_up = true;
    std::chrono::milliseconds warmup_duration = std::chrono::milliseconds(100);
    std::map<std::string, std::string> options;
    
    PerformanceBenchmark(const std::string& bench_name,
                         const std::string& bench_desc,
                         PerformanceMode bench_mode,
                         double bench_threshold,
                         const std::string& bench_unit)
        : name(bench_name), description(bench_desc),
          mode(bench_mode), threshold_value(bench_threshold),
          threshold_unit(bench_unit) {}
};

// Memory leak detection configuration
struct MemoryLeakConfig {
    MemoryLeakMode mode;
    std::chrono::seconds detection_duration;
    std::vector<std::string> tracking_patterns;
    std::vector<std::string> exclusion_patterns;
    std::string output_file;
    bool generate_report = true;
    std::map<std::string, std::string> options;
    
    MemoryLeakConfig(MemoryLeakMode leak_mode,
                     const std::chrono::seconds& duration,
                     const std::vector<std::string>& patterns,
                     const std::string& output_path)
        : mode(leak_mode), detection_duration(duration),
          tracking_patterns(patterns), output_file(output_path) {}
};

// Stress test configuration
struct StressTestConfig {
    std::string name;
    std::string description;
    std::chrono::seconds duration;
    double target_load;
    int concurrent_threads;
    std::vector<std::string> stress_patterns;
    std::map<std::string, double> resource_limits;
    bool should_recover = true;
    std::vector<std::string> failure_recovery_actions;
    
    StressTestConfig(const std::string& test_name,
                     const std::string& test_desc,
                     const std::chrono::seconds& test_duration,
                     double test_load)
        : name(test_name), description(test_desc),
          duration(test_duration), target_load(test_load) {}
};

// Integration test configuration
struct IntegrationConfig {
    std::string name;
    std::string description;
    std::vector<std::string> component_names;
    std::vector<std::string> interaction_types;
    std::chrono::seconds timeout_per_interaction;
    bool test_all_combinations = true;
    std::map<std::string, std::string> integration_parameters;
    std::vector<std::string> expected_integration_points;
    
    IntegrationConfig(const std::string& int_name,
                      const std::string& int_desc,
                      const std::vector<std::string>& components)
        : name(int_name), description(int_desc),
          component_names(components) {}
};

// Cross-platform test configuration
struct CrossPlatformConfig {
    std::string name;
    std::string description;
    std::vector<Platform> target_platforms;
    std::vector<std::string> platform_specific_tests;
    std::chrono::seconds platform_timeout;
    std::map<Platform, std::string> platform_configs;
    bool test_all_platforms = true;
    bool skip_unsupported_platforms = true;
    
    CrossPlatformConfig(const std::string& xpl_name,
                        const std::string& xpl_desc,
                        const std::vector<Platform>& platforms)
        : name(xpl_name), description(xpl_desc),
          target_platforms(platforms) {}
};

// Regression test configuration
struct RegressionConfig {
    std::string name;
    std::string description;
    std::string baseline_file;
    double tolerance_value;
    std::string tolerance_unit;
    std::vector<std::string> regression_scenarios;
    bool track_performance_regression = true;
    bool track_behavior_regression = true;
    bool track_memory_regression = true;
    std::map<std::string, std::string> regression_checks;
    
    RegressionConfig(const std::string& reg_name,
                     const std::string& reg_desc,
                     const std::string& baseline_path,
                     double reg_tolerance)
        : name(reg_name), description(reg_desc),
          baseline_file(baseline_path), tolerance_value(reg_tolerance) {}
};

// Test suite configuration
struct TestSuiteConfig {
    std::string name;
    std::string description;
    std::vector<TestScenario> scenarios;
    std::vector<PerformanceBenchmark> benchmarks;
    std::vector<MemoryLeakConfig> memory_configs;
    std::vector<StressTestConfig> stress_configs;
    std::vector<IntegrationConfig> integration_configs;
    std::vector<CrossPlatformConfig> platform_configs;
    std::vector<RegressionConfig> regression_configs;
    
    std::chrono::seconds overall_timeout;
    std::chrono::seconds setup_timeout;
    std::chrono::seconds teardown_timeout;
    bool enable_parallel_execution = true;
    int max_parallel_threads = std::thread::hardware_concurrency();
    bool generate_html_reports = true;
    bool generate_json_reports = true;
    bool generate_xml_reports = true;
    bool generate_csv_reports = true;
    std::string report_output_directory;
    std::string baseline_directory;
    std::string configuration_file;
    
    TestSuiteConfig(const std::string& suite_name,
                    const std::string& suite_desc)
        : name(suite_name), description(suite_desc) {}
};

// Test results data structure
struct TestResultData {
    std::string test_name;
    TestStatus status;
    std::chrono::duration<double> execution_time;
    size_t memory_used;
    size_t peak_memory;
    std::vector<std::string> messages;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, double> metrics;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;
    std::string platform;
    std::string configuration;
    
    TestResultData(const std::string& result_name)
        : test_name(result_name), status(TestStatus::NOT_STARTED) {}
};

// Test execution statistics
struct TestExecutionStats {
    size_t total_tests = 0;
    size_t passed_tests = 0;
    size_t failed_tests = 0;
    size_t skipped_tests = 0;
    size_t timeout_tests = 0;
    double total_execution_time = 0.0;
    size_t total_memory_used = 0;
    size_t peak_memory_used = 0;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;
    
    double get_pass_rate() const {
        return total_tests > 0 ? (static_cast<double>(passed_tests) / total_tests) * 100.0 : 0.0;
    }
    
    double get_failure_rate() const {
        return total_tests > 0 ? (static_cast<double>(failed_tests) / total_tests) * 100.0 : 0.0;
    }
    
    double get_timeout_rate() const {
        return total_tests > 0 ? (static_cast<double>(timeout_tests) / total_tests) * 100.0 : 0.0;
    }
};

// Configuration file parser
class ConfigFileParser {
public:
    static TestSuiteConfig parse_config_file(const std::string& filename) {
        TestSuiteConfig config("Loaded Config", "Configuration loaded from file");
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open configuration file: " + filename);
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Simple line-based parsing for demonstration
            if (line.empty() || line[0] == '#') continue;
            
            size_t separator = line.find('=');
            if (separator != std::string::npos) {
                std::string key = line.substr(0, separator);
                std::string value = line.substr(separator + 1);
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\n\r"));
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                
                // Parse configuration key-value pairs
                if (key == "overall_timeout") {
                    config.overall_timeout = std::chrono::seconds(std::stoi(value));
                } else if (key == "enable_parallel_execution") {
                    config.enable_parallel_execution = (value == "true" || value == "1");
                } else if (key == "max_parallel_threads") {
                    config.max_parallel_threads = std::stoi(value);
                }
            }
        }
        
        file.close();
        return config;
    }
    
    static void save_config_file(const TestSuiteConfig& config, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot write configuration file: " + filename);
        }
        
        file << "# NLM Test Configuration" << std::endl;
        file << "# Generated automatically by test configuration system" << std::endl;
        file << std::endl;
        file << "overall_timeout = " << config.overall_timeout.count() << std::endl;
        file << "enable_parallel_execution = " << (config.enable_parallel_execution ? "true" : "false") << std::endl;
        file << "max_parallel_threads = " << config.max_parallel_threads << std::endl;
        
        file.close();
    }
};

// Test configuration validator
class TestConfigValidator {
public:
    static bool validate_test_scenario(const TestScenario& scenario) {
        if (scenario.name.empty()) {
            std::cerr << "Validation error: Test scenario name cannot be empty" << std::endl;
            return false;
        }
        
        if (scenario.timeout <= 0) {
            std::cerr << "Validation error: Test scenario timeout must be positive" << std::endl;
            return false;
        }
        
        if (!scenario.prerequisites.empty()) {
            for (const auto& prereq : scenario.prerequisites) {
                if (prereq.empty()) {
                    std::cerr << "Validation error: Empty prerequisite in scenario " << scenario.name << std::endl;
                    return false;
                }
            }
        }
        
        return true;
    }
    
    static bool validate_performance_benchmark(const PerformanceBenchmark& benchmark) {
        if (benchmark.name.empty()) {
            std::cerr << "Validation error: Benchmark name cannot be empty" << std::endl;
            return false;
        }
        
        if (benchmark.threshold_value < 0) {
            std::cerr << "Validation error: Benchmark threshold must be non-negative" << std::endl;
            return false;
        }
        
        if (benchmark.iterations <= 0) {
            std::cerr << "Validation error: Benchmark iterations must be positive" << std::endl;
            return false;
        }
        
        return true;
    }
    
    static bool validate_memory_leak_config(const MemoryLeakConfig& config) {
        if (config.detection_duration.count() <= 0) {
            std::cerr << "Validation error: Memory leak detection duration must be positive" << std::endl;
            return false;
        }
        
        return true;
    }
    
    static bool validate_stress_test_config(const StressTestConfig& config) {
        if (config.duration.count() <= 0) {
            std::cerr << "Validation error: Stress test duration must be positive" << std::endl;
            return false;
        }
        
        if (config.target_load < 0 || config.target_load > 100) {
            std::cerr << "Validation error: Stress test load must be between 0 and 100" << std::endl;
            return false;
        }
        
        if (config.concurrent_threads <= 0) {
            std::cerr << "Validation error: Stress test thread count must be positive" << std::endl;
            return false;
        }
        
        return true;
    }
    
    static bool validate_integration_config(const IntegrationConfig& config) {
        if (config.name.empty()) {
            std::cerr << "Validation error: Integration config name cannot be empty" << std::endl;
            return false;
        }
        
        if (config.component_names.empty()) {
            std::cerr << "Validation error: Integration config must have at least one component" << std::endl;
            return false;
        }
        
        return true;
    }
    
    static bool validate_cross_platform_config(const CrossPlatformConfig& config) {
        if (config.name.empty()) {
            std::cerr << "Validation error: Cross-platform config name cannot be empty" << std::endl;
            return false;
        }
        
        if (config.target_platforms.empty()) {
            std::cerr << "Validation error: Cross-platform config must have at least one target platform" << std::endl;
            return false;
        }
        
        return true;
    }
    
    static bool validate_regression_config(const RegressionConfig& config) {
        if (config.name.empty()) {
            std::cerr << "Validation error: Regression config name cannot be empty" << std::endl;
            return false;
        }
        
        if (config.baseline_file.empty()) {
            std::cerr << "Validation error: Regression config baseline file cannot be empty" << std::endl;
            return false;
        }
        
        if (config.tolerance_value < 0) {
            std::cerr << "Validation error: Regression tolerance must be non-negative" << std::endl;
            return false;
        }
        
        return true;
    }
    
    static bool validate_test_suite_config(const TestSuiteConfig& config) {
        if (config.name.empty()) {
            std::cerr << "Validation error: Test suite name cannot be empty" << std::endl;
            return false;
        }
        
        if (config.overall_timeout.count() <= 0) {
            std::cerr << "Validation error: Test suite overall timeout must be positive" << std::endl;
            return false;
        }
        
        if (config.max_parallel_threads <= 0) {
            std::cerr << "Validation error: Max parallel threads must be positive" << std::endl;
            return false;
        }
        
        // Validate all sub-configurations
        for (const auto& scenario : config.scenarios) {
            if (!validate_test_scenario(scenario)) return false;
        }
        
        for (const auto& benchmark : config.benchmarks) {
            if (!validate_performance_benchmark(benchmark)) return false;
        }
        
        for (const auto& memory_config : config.memory_configs) {
            if (!validate_memory_leak_config(memory_config)) return false;
        }
        
        for (const auto& stress_config : config.stress_configs) {
            if (!validate_stress_test_config(stress_config)) return false;
        }
        
        for (const auto& integration_config : config.integration_configs) {
            if (!validate_integration_config(integration_config)) return false;
        }
        
        for (const auto& platform_config : config.platform_configs) {
            if (!validate_cross_platform_config(platform_config)) return false;
        }
        
        for (const auto& regression_config : config.regression_configs) {
            if (!validate_regression_config(regression_config)) return false;
        }
        
        return true;
    }
};

// Test configuration reporter
class TestConfigReporter {
public:
    static std::string generate_config_summary(const TestSuiteConfig& config) {
        std::stringstream ss;
        ss << "=== Test Suite Configuration Summary ===" << std::endl;
        ss << "Test Suite Name: " << config.name << std::endl;
        ss << "Description: " << config.description << std::endl;
        ss << "Overall Timeout: " << config.overall_timeout.count() << " seconds" << std::endl;
        ss << "Setup Timeout: " << config.setup_timeout.count() << " seconds" << std::endl;
        ss << "Teardown Timeout: " << config.teardown_timeout.count() << " seconds" << std::endl;
        ss << std::endl;
        
        ss << "Test Categories:" << std::endl;
        ss << "  Scenarios: " << config.scenarios.size() << std::endl;
        ss << "  Performance Benchmarks: " << config.benchmarks.size() << std::endl;
        ss << "  Memory Leak Tests: " << config.memory_configs.size() << std::endl;
        ss << "  Stress Tests: " << config.stress_configs.size() << std::endl;
        ss << "  Integration Tests: " << config.integration_configs.size() << std::endl;
        ss << "  Cross-Platform Tests: " << config.platform_configs.size() << std::endl;
        ss << "  Regression Tests: " << config.regression_configs.size() << std::endl;
        ss << std::endl;
        
        ss << "Execution Settings:" << std::endl;
        ss << "  Parallel Execution: " << (config.enable_parallel_execution ? "Enabled" : "Disabled") << std::endl;
        ss << "  Max Parallel Threads: " << config.max_parallel_threads << std::endl;
        ss << std::endl;
        
        ss << "Reporting Settings:" << std::endl;
        ss << "  HTML Reports: " << (config.generate_html_reports ? "Enabled" : "Disabled") << std::endl;
        ss << "  JSON Reports: " << (config.generate_json_reports ? "Enabled" : "Disabled") << std::endl;
        ss << "  XML Reports: " << (config.generate_xml_reports ? "Enabled" : "Disabled") << std::endl;
        ss << "  CSV Reports: " << (config.generate_csv_reports ? "Enabled" : "Disabled") << std::endl;
        
        if (!config.report_output_directory.empty()) {
            ss << "  Report Output Directory: " << config.report_output_directory << std::endl;
        }
        
        return ss.str();
    }
    
    static std::string generate_test_categories_summary(const TestSuiteConfig& config) {
        std::stringstream ss;
        ss << "=== Test Categories ===" << std::endl;
        
        std::map<TestCategory, int> category_counts;
        for (const auto& scenario : config.scenarios) {
            category_counts[scenario.category]++;
        }
        
        for (const auto& pair : category_counts) {
            std::string category_name;
            switch (pair.first) {
                case TestCategory::MEMORY_LEAK: category_name = "Memory Leak"; break;
                case TestCategory::STRESS: category_name = "Stress"; break;
                case TestCategory::PERFORMANCE: category_name = "Performance"; break;
                case TestCategory::EDGE_CASE: category_name = "Edge Case"; break;
                case TestCategory::INTEGRATION: category_name = "Integration"; break;
                case TestCategory::CROSS_PLATFORM: category_name = "Cross-Platform"; break;
                case TestCategory::REGRESSION: category_name = "Regression"; break;
                case TestCategory::UNIT: category_name = "Unit"; break;
                case TestCategory::SYSTEM: category_name = "System"; break;
                case TestCategory::NETWORK: category_name = "Network"; break;
                case TestCategory::BRAIN: category_name = "Brain"; break;
                case TestCategory::AGENT: category_name = "Agent"; break;
                case TestCategory::WORLD: category_name = "World"; break;
                case TestCategory::PLASTICITY: category_name = "Plasticity"; break;
                case TestCategory::NEUROMODULATION: category_name = "Neuromodulation"; break;
                case TestCategory::DEVELOPMMENT: category_name = "Development"; break;
                case TestCategory::PREDICTION: category_name = "Prediction"; break;
                case TestCategory::COGNITION: category_name = "Cognition"; break;
                case TestCategory::SENSOR: category_name = "Sensory"; break;
                case TestCategory::MOTOR: category_name = "Motor"; break;
                case TestCategory::ENVIRONMENT: category_name = "Environment"; break;
                case TestCategory::EXPERIMENT: category_name = "Experiment"; break;
                default: category_name = "Unknown"; break;
            }
            ss << "  " << category_name << ": " << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
    static std::string generate_priority_summary(const TestSuiteConfig& config) {
        std::stringstream ss;
        ss << "=== Test Priority Summary ===" << std::endl;
        
        std::map<TestPriority, int> priority_counts;
        for (const auto& scenario : config.scenarios) {
            priority_counts[scenario.priority]++;
        }
        
        for (const auto& pair : priority_counts) {
            std::string priority_name;
            switch (pair.first) {
                case TestPriority::CRITICAL: priority_name = "Critical"; break;
                case TestPriority::HIGH: priority_name = "High"; break;
                case TestPriority::MEDIUM: priority_name = "Medium"; break;
                case TestPriority::LOW: priority_name = "Low"; break;
                case TestPriority::MINIMAL: priority_name = "Minimal"; break;
                default: priority_name = "Unknown"; break;
            }
            ss << "  " << priority_name << ": " << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
    static std::string generate_platform_summary(const TestSuiteConfig& config) {
        std::stringstream ss;
        ss << "=== Platform Summary ===" << std::endl;
        
        std::map<Platform, int> platform_counts;
        for (const auto& platform_config : config.platform_configs) {
            for (const auto& platform : platform_config.target_platforms) {
                platform_counts[platform]++;
            }
        }
        
        for (const auto& pair : platform_counts) {
            std::string platform_name;
            switch (pair.first) {
                case Platform::ANY: platform_name = "Any"; break;
                case Platform::LINUX: platform_name = "Linux"; break;
                case Platform::WINDOWS: platform_name = "Windows"; break;
                case Platform::MACOS: platform_name = "macOS"; break;
                case Platform::ANDROID: platform_name = "Android"; break;
                case Platform::IOS: platform_name = "iOS"; break;
                case Platform::RASPBIAN: platform_name = "Raspberry Pi"; break;
                default: platform_name = "Unknown"; break;
            }
            ss << "  " << platform_name << ": " << pair.second << std::endl;
        }
        
        return ss.str();
    }
    
    static std::string generate_configuration_summary(const TestSuiteConfig& config) {
        std::stringstream ss;
        ss << generate_config_summary(config);
        ss << std::endl;
        ss << generate_test_categories_summary(config);
        ss << std::endl;
        ss << generate_priority_summary(config);
        ss << std::endl;
        ss << generate_platform_summary(config);
        return ss.str();
    }
};

// Configuration manager
class TestConfigManager {
private:
    TestSuiteConfig current_config;
    bool config_loaded = false;
    std::string last_config_file;
    
public:
    bool load_config(const std::string& filename) {
        try {
            current_config = ConfigFileParser::parse_config_file(filename);
            if (TestConfigValidator::validate_test_suite_config(current_config)) {
                config_loaded = true;
                last_config_file = filename;
                std::cout << "Configuration loaded successfully from: " << filename << std::endl;
                return true;
            } else {
                std::cerr << "Configuration validation failed" << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading configuration: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool load_default_config() {
        return load_config("configs/default_test_config.cfg");
    }
    
    bool save_config(const std::string& filename) {
        try {
            ConfigFileParser::save_config_file(current_config, filename);
            last_config_file = filename;
            std::cout << "Configuration saved successfully to: " << filename << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error saving configuration: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool is_config_loaded() const {
        return config_loaded;
    }
    
    const TestSuiteConfig& get_config() const {
        return current_config;
    }
    
    const std::string& get_last_config_file() const {
        return last_config_file;
    }
    
    void print_config_summary() const {
        if (config_loaded) {
            std::cout << TestConfigReporter::generate_configuration_summary(current_config);
        } else {
            std::cout << "No configuration loaded" << std::endl;
        }
    }
    
    void create_default_scenario(const std::string& name, const std::string& description) {
        TestScenario scenario(name, description, TestCategory::UNIT, TestPriority::MEDIUM);
        current_config.scenarios.push_back(scenario);
    }
    
    void create_memory_leak_scenario(const std::string& name) {
        TestScenario scenario(name, "Memory leak detection test", TestCategory::MEMORY_LEAK, TestPriority::HIGH);
        scenario.timeout = 60.0;
        scenario.tags.push_back(MEMORY_TESTS);
        current_config.scenarios.push_back(scenario);
    }
    
    void create_stress_test_scenario(const std::string& name) {
        TestScenario scenario(name, "Stress test scenario", TestCategory::STRESS, TestPriority::HIGH);
        scenario.timeout = 120.0;
        scenario.tags.push_back(STRESS_TESTS);
        current_config.stress_configs.push_back(StressTestConfig(name, "Stress test", std::chrono::seconds(120), 50.0));
    }
    
    void create_performance_benchmark(const std::string& name) {
        PerformanceBenchmark benchmark(name, "Performance benchmark", PerformanceMode::MILLISECONDS, 1000.0, "ms");
        benchmark.iterations = 50;
        current_config.benchmarks.push_back(benchmark);
    }
    
    void create_integration_scenario(const std::string& name, const std::vector<std::string>& components) {
        TestScenario scenario(name, "Integration test", TestCategory::INTEGRATION, TestPriority::CRITICAL);
        scenario.tags.push_back(INTEGRATION_TESTS);
        scenario.integration_configs.push_back(IntegrationConfig(name, "Integration", components));
        current_config.scenarios.push_back(scenario);
    }
    
    void create_regression_scenario(const std::string& name, const std::string& baseline_file) {
        TestScenario scenario(name, "Regression test", TestCategory::REGRESSION, TestPriority::CRITICAL);
        scenario.tags.push_back(REGRESSION_TESTS);
        scenario.regression_configs.push_back(RegressionConfig(name, "Regression test", baseline_file, 0.05));
        current_config.scenarios.push_back(scenario);
    }
    
    const std::vector<TestScenario>& get_scenarios() const {
        return current_config.scenarios;
    }
    
    const std::vector<PerformanceBenchmark>& get_benchmarks() const {
        return current_config.benchmarks;
    }
    
    const std::vector<MemoryLeakConfig>& get_memory_configs() const {
        return current_config.memory_configs;
    }
    
    const std::vector<StressTestConfig>& get_stress_configs() const {
        return current_config.stress_configs;
    }
    
    const std::vector<IntegrationConfig>& get_integration_configs() const {
        return current_config.integration_configs;
    }
    
    const std::vector<CrossPlatformConfig>& get_platform_configs() const {
        return current_config.platform_configs;
    }
    
    const std::vector<RegressionConfig>& get_regression_configs() const {
        return current_config.regression_configs;
    }
};

// Global configuration manager
extern TestConfigManager global_test_config;

// Configuration factory
class TestConfigFactory {
public:
    static TestSuiteConfig create_comprehensive_config() {
        TestSuiteConfig config("Comprehensive Test Suite", "Full test suite for NLM Phase 7");
        
        // Memory leak tests
        config.create_memory_leak_scenario("test_memory_leak_neuron_creation");
        config.create_memory_leak_scenario("test_memory_leak_synapse_creation");
        config.create_memory_leak_scenario("test_memory_leak_brain_creation");
        config.create_memory_leak_scenario("test_memory_leak_config_system");
        
        // Performance benchmarks
        config.create_performance_benchmark("benchmark_neuron_creation");
        config.create_performance_benchmark("benchmark_synapse_creation");
        config.create_performance_benchmark("benchmark_brain_simulation");
        config.create_performance_benchmark("benchmark_plasticity_updates");
        
        // Stress tests
        config.create_stress_test_scenario("test_stress_high_load");
        config.create_stress_test_scenario("test_stress_memory_pressure");
        config.create_stress_test_scenario("test_stress_concurrent_access");
        
        // Integration tests
        config.create_integration_scenario("test_integration_brain_memory", {"brain", "memory", "neuromodulation"});
        config.create_integration_scenario("test_integration_plasticity_devel", {"plasticity", "development", "agent"});
        config.create_integration_scenario("test_integration_prediction_action", {"prediction", "motor", "environment"});
        
        // Regression tests
        config.create_regression_scenario("test_regression_neuron_behavior", "baselines/neuron_behavior.json");
        config.create_regression_scenario("test_regression_plasticity", "baselines/plasticity.json");
        config.create_regression_scenario("test_regression_memory", "baselines/memory.json");
        
        // Basic scenario
        config.create_default_scenario("test_basic_functionality", "Basic functionality test");
        
        return config;
    }
    
    static TestSuiteConfig create_lightweight_config() {
        TestSuiteConfig config("Lightweight Test Suite", "Minimal test suite for quick validation");
        
        config.create_default_scenario("test_basic_functionality", "Basic functionality test");
        
        return config;
    }
};