// NLM Enhanced Test Configuration Implementation
// Phase 7: Comprehensive Test Suite

#include "test_infrastructure/utils/test_config.hpp"

// Global configuration manager
TestConfigManager global_test_config;

// Test configuration implementation
void TestConfigManager::register_test(const TestConfig& config) {
    test_configs.push_back(config);
    config_map[config.name] = config;
}

const TestConfig* TestConfigManager::get_test_config(const std::string& name) const {
    auto it = config_map.find(name);
    return (it != config_map.end()) ? &it->second : nullptr;
}

const std::vector<TestConfig>& TestConfigManager::get_all_configs() const {
    return test_configs;
}

std::vector<TestConfig> TestConfigManager::get_enabled_configs() const {
    std::vector<TestConfig> enabled;
    for (const auto& config : test_configs) {
        if (config.enabled) {
            enabled.push_back(config);
        }
    }
    return enabled;
}

void TestConfigManager::enable_test(const std::string& name, bool enabled) {
    auto it = config_map.find(name);
    if (it != config_map.end()) {
        it->second.enabled = enabled;
    }
}

void TestConfigManager::add_tag_to_test(const std::string& name, const std::string& tag) {
    auto it = config_map.find(name);
    if (it != config_map.end()) {
        it->second.tags.push_back(tag);
    }
}

std::vector<TestConfig> TestConfigManager::get_tests_by_tag(const std::string& tag) const {
    std::vector<TestConfig> tagged;
    for (const auto& config : test_configs) {
        if (std::find(config.tags.begin(), config.tags.end(), tag) != config.tags.end()) {
            tagged.push_back(config);
        }
    }
    return tagged;
}

void TestConfigManager::print_config_summary() const {
    std::cout << "=== Test Configuration Summary ===" << std::endl;
    std::cout << "Total Tests Registered: " << test_configs.size() << std::endl;
    std::cout << "Enabled Tests: " << std::count_if(test_configs.begin(), test_configs.end(),
        [](const TestConfig& c) { return c.enabled; }) << std::endl;
    std::cout << std::endl;
    
    for (const auto& config : test_configs) {
        std::cout << "  " << config.name << ": " << config.description << std::endl;
        std::cout << "    Enabled: " << (config.enabled ? "Yes" : "No") << std::endl;
        std::cout << "    Timeout: " << config.timeout << "s" << std::endl;
        std::cout << "    Stress Test: " << (config.stress_test ? "Yes" : "No") << std::endl;
        if (!config.tags.empty()) {
            std::cout << "    Tags: ";
            for (size_t i = 0; i < config.tags.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << config.tags[i];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

// Test configuration file operations
void TestConfigFileOperations::initialize_config_files() {
    std::cout << "Initializing test configuration files..." << std::endl;
    
    // Create configs directory if it doesn't exist
    std::ifstream test_config_file("configs/default_test_config.cfg");
    if (!test_config_file.good()) {
        std::cout << "Creating default test configuration file..." << std::endl;
        create_default_config_file();
    }
    
    test_config_file.close();
}

void TestConfigFileOperations::create_default_config_file() {
    std::ofstream file("configs/default_test_config.cfg");
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create default test configuration file");
    }
    
    file << "# NLM Test Configuration File" << std::endl;
    file << "# Default configuration for enhanced test suite" << std::endl;
    file << std::endl;
    file << "# Overall test suite settings" << std::endl;
    file << "overall_timeout = 3600" << std::endl; // 1 hour
    file << "enable_parallel_execution = true" << std::endl;
    file << "max_parallel_threads = 8" << std::endl;
    file << "generate_html_reports = true" << std::endl;
    file << "generate_json_reports = true" << std::endl;
    file << "generate_xml_reports = true" << std::endl;
    file << "generate_csv_reports = true" << std::endl;
    file << std::endl;
    file << "# Memory leak test settings" << std::endl;
    file << "memory_leak_detection_mode = TRACK_ALLOCS" << std::endl;
    file << "memory_leak_duration_seconds = 300" << std::endl;
    file << std::endl;
    file << "# Performance benchmark settings" << std::endl;
    file << "benchmark_iterations = 100" << std::endl;
    file << "benchmark_warmup_enabled = true" << std::endl;
    file << "benchmark_warmup_duration_ms = 100" << std::endl;
    file << std::endl;
    file << "# Stress test settings" << std::endl;
    file << "stress_test_duration_seconds = 300" << std::endl;
    file << "stress_test_target_load = 50.0" << std::endl;
    file << "stress_test_concurrent_threads = 4" << std::endl;
    file << std::endl;
    file << "# Integration test settings" << std::endl;
    file << "integration_test_timeout_seconds = 60" << std::endl;
    file << "integration_test_all_combinations = true" << std::endl;
    file << std::endl;
    file << "# Cross-platform test settings" << std::endl;
    file << "cross_platform_test_timeout_seconds = 120" << std::endl;
    file << "cross_platform_test_all_platforms = true" << std::endl;
    file << std::endl;
    file << "# Regression test settings" << std::endl;
    file << "regression_test_tolerance = 0.05" << std::endl;
    file << "regression_test_track_performance = true" << std::endl;
    file << "regression_test_track_behavior = true" << std::endl;
    file << "regression_test_track_memory = true" << std::endl;
    
    file.close();
}

void TestConfigFileOperations::backup_config_files() {
    std::cout << "Backing up configuration files..." << std::endl;
    
    std::time_t now = std::time(nullptr);
    std::string timestamp = std::to_string(now);
    
    // Backup current config
    std::ifstream src("configs/default_test_config.cfg");
    if (src.good()) {
        std::ofstream dst("configs/test_config_backup_" + timestamp + ".cfg");
        if (dst.is_open()) {
            dst << src.rdbuf();
            dst.close();
            std::cout << "Configuration backed up to: test_config_backup_" << timestamp << ".cfg" << std::endl;
        }
    }
    
    src.close();
}

void TestConfigFileOperations::restore_config_files() {
    std::cout << "Restoring configuration files..." << std::endl;
    
    // Find the most recent backup
    std::vector<std::string> backup_files;
    for (const auto& entry : std::filesystem::directory_iterator("configs")) {
        if (entry.path().filename().string().find("test_config_backup_") == 0 &&
            entry.path().filename().string().find(".cfg") != std::string::npos) {
            backup_files.push_back(entry.path().string());
        }
    }
    
    if (!backup_files.empty()) {
        // Sort by modification time (most recent first)
        std::sort(backup_files.begin(), backup_files.end(),
            [](const std::string& a, const std::string& b) {
                return std::filesystem::last_write_time(std::filesystem::path(a)) >
                       std::filesystem::last_write_time(std::filesystem::path(b));
            });
        
        std::ifstream backup(backup_files[0]);
        if (backup.is_open()) {
            std::ofstream current("configs/default_test_config.cfg");
            if (current.is_open()) {
                current << backup.rdbuf();
                current.close();
                std::cout << "Configuration restored from: " << backup_files[0] << std::endl;
            }
        }
        
        backup.close();
    } else {
        std::cout << "No backup files found to restore" << std::endl;
    }
}

void TestConfigFileOperations::export_config_to_json(const TestSuiteConfig& config, const std::string& filename) {
    std::cout << "Exporting configuration to JSON: " << filename << std::endl;
    
    // Simple JSON export for demonstration
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create JSON configuration file: " + filename);
    }
    
    file << "{\n";
    file << "  \"name\": \"" << config.name << "\",\n";
    file << "  \"description\": \"" << config.description << "\",\n";
    file << "  \"overall_timeout\": " << config.overall_timeout.count() << ",\n";
    file << "  \"enable_parallel_execution\": " << (config.enable_parallel_execution ? "true" : "false") << ",\n";
    file << "  \"max_parallel_threads\": " << config.max_parallel_threads << ",\n";
    file << "  \"scenarios_count\": " << config.scenarios.size() << ",\n";
    file << "  \"benchmarks_count\": " << config.benchmarks.size() << ",\n";
    file << "  \"memory_configs_count\": " << config.memory_configs.size() << ",\n";
    file << "  \"stress_configs_count\": " << config.stress_configs.size() << ",\n";
    file << "  \"integration_configs_count\": " << config.integration_configs.size() << ",\n";
    file << "  \"platform_configs_count\": " << config.platform_configs.size() << ",\n";
    file << "  \"regression_configs_count\": " << config.regression_configs.size() << "\n";
    file << "}\n";
    
    file.close();
}

void TestConfigFileOperations::import_config_from_json(TestSuiteConfig& config, const std::string& filename) {
    std::cout << "Importing configuration from JSON: " << filename << std::endl;
    
    // Simple JSON import for demonstration
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open JSON configuration file: " + filename);
    }
    
    // For demonstration, just load the config as-is
    config = TestConfigManager().get_config();
    
    file.close();
}

// Test configuration validation functions
bool TestConfigValidation::validate_scenario_timeout(const TestScenario& scenario) {
    if (scenario.timeout <= 0) {
        std::cerr << "ERROR: Scenario timeout must be positive (got " << scenario.timeout << ")" << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_scenario_dependencies(const TestScenario& scenario) {
    if (scenario.prerequisites.size() > 10) {
        std::cerr << "ERROR: Scenario has too many prerequisites (max 10, got " << scenario.prerequisites.size() << ")" << std::endl;
        return false;
    }
    
    for (const auto& dep : scenario.prerequisites) {
        if (dep.empty()) {
            std::cerr << "ERROR: Empty prerequisite found" << std::endl;
            return false;
        }
    }
    return true;
}

bool TestConfigValidation::validate_scenario_tags(const TestScenario& scenario) {
    std::set<std::string> unique_tags(scenario.tags.begin(), scenario.tags.end());
    if (unique_tags.size() != scenario.tags.size()) {
        std::cerr << "ERROR: Duplicate tags found in scenario: " << scenario.name << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_performance_benchmark_threshold(const PerformanceBenchmark& benchmark) {
    if (benchmark.threshold_value <= 0) {
        std::cerr << "ERROR: Benchmark threshold must be positive (got " << benchmark.threshold_value << ")" << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_performance_benchmark_iterations(const PerformanceBenchmark& benchmark) {
    if (benchmark.iterations <= 0) {
        std::cerr << "ERROR: Benchmark iterations must be positive (got " << benchmark.iterations << ")" << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_memory_leak_mode(const MemoryLeakConfig& config) {
    switch (config.mode) {
        case MemoryLeakMode::TRACK_ALLOCS:
        case MemoryLeakMode::TRACK_DEALLOCS:
        case MemoryLeakMode::FULL_STACK_TRACE:
        case MemoryLeakMode::HEAP_PROFILING:
        case MemoryLeakMode::ADDRESS_SANITIZER:
        case MemoryLeakMode::VALGRIND:
            break;
        default:
            std::cerr << "ERROR: Invalid memory leak mode" << std::endl;
            return false;
    }
    return true;
}

bool TestConfigValidation::validate_stress_test_duration(const StressTestConfig& config) {
    if (config.duration.count() <= 0) {
        std::cerr << "ERROR: Stress test duration must be positive" << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_stress_test_load(const StressTestConfig& config) {
    if (config.target_load < 0 || config.target_load > 100) {
        std::cerr << "ERROR: Stress test load must be between 0 and 100 (got " << config.target_load << ")" << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_integration_components(const IntegrationConfig& config) {
    if (config.component_names.empty()) {
        std::cerr << "ERROR: Integration test must have at least one component" << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_cross_platform_targets(const CrossPlatformConfig& config) {
    if (config.target_platforms.empty()) {
        std::cerr << "ERROR: Cross-platform test must have at least one target platform" << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_regression_baseline(const RegressionConfig& config) {
    if (config.baseline_file.empty()) {
        std::cerr << "ERROR: Regression test must have a baseline file" << std::endl;
        return false;
    }
    return true;
}

bool TestConfigValidation::validate_all_test_scenarios(const std::vector<TestScenario>& scenarios) {
    bool all_valid = true;
    for (const auto& scenario : scenarios) {
        if (!validate_scenario_timeout(scenario)) all_valid = false;
        if (!validate_scenario_dependencies(scenario)) all_valid = false;
        if (!validate_scenario_tags(scenario)) all_valid = false;
    }
    return all_valid;
}

bool TestConfigValidation::validate_all_performance_benchmarks(const std::vector<PerformanceBenchmark>& benchmarks) {
    bool all_valid = true;
    for (const auto& benchmark : benchmarks) {
        if (!validate_performance_benchmark_threshold(benchmark)) all_valid = false;
        if (!validate_performance_benchmark_iterations(benchmark)) all_valid = false;
    }
    return all_valid;
}

bool TestConfigValidation::validate_all_memory_configs(const std::vector<MemoryLeakConfig>& memory_configs) {
    bool all_valid = true;
    for (const auto& config : memory_configs) {
        if (!validate_memory_leak_mode(config)) all_valid = false;
    }
    return all_valid;
}

bool TestConfigValidation::validate_all_stress_configs(const std::vector<StressTestConfig>& stress_configs) {
    bool all_valid = true;
    for (const auto& config : stress_configs) {
        if (!validate_stress_test_duration(config)) all_valid = false;
        if (!validate_stress_test_load(config)) all_valid = false;
    }
    return all_valid;
}

bool TestConfigValidation::validate_all_integration_configs(const std::vector<IntegrationConfig>& integration_configs) {
    bool all_valid = true;
    for (const auto& config : integration_configs) {
        if (!validate_integration_components(config)) all_valid = false;
    }
    return all_valid;
}

bool TestConfigValidation::validate_all_cross_platform_configs(const std::vector<CrossPlatformConfig>& platform_configs) {
    bool all_valid = true;
    for (const auto& config : platform_configs) {
        if (!validate_cross_platform_targets(config)) all_valid = false;
    }
    return all_valid;
}

bool TestConfigValidation::validate_all_regression_configs(const std::vector<RegressionConfig>& regression_configs) {
    bool all_valid = true;
    for (const auto& config : regression_configs) {
        if (!validate_regression_baseline(config)) all_valid = false;
    }
    return all_valid;
}

bool TestConfigValidation::validate_test_suite_config(const TestSuiteConfig& config) {
    if (config.name.empty()) {
        std::cerr << "ERROR: Test suite name cannot be empty" << std::endl;
        return false;
    }
    
    if (config.overall_timeout.count() <= 0) {
        std::cerr << "ERROR: Test suite overall timeout must be positive" << std::endl;
        return false;
    }
    
    if (config.max_parallel_threads <= 0) {
        std::cerr << "ERROR: Max parallel threads must be positive" << std::endl;
        return false;
    }
    
    if (!validate_all_test_scenarios(config.scenarios)) return false;
    if (!validate_all_performance_benchmarks(config.benchmarks)) return false;
    if (!validate_all_memory_configs(config.memory_configs)) return false;
    if (!validate_all_stress_configs(config.stress_configs)) return false;
    if (!validate_all_integration_configs(config.integration_configs)) return false;
    if (!validate_all_cross_platform_configs(config.platform_configs)) return false;
    if (!validate_all_regression_configs(config.regression_configs)) return false;
    
    return true;
}

// Test configuration summary generator
std::string TestConfigSummaryGenerator::generate_test_suite_summary(const TestSuiteConfig& config) {
    std::stringstream ss;
    ss << "=== NLM Enhanced Test Suite Configuration ===" << std::endl;
    ss << "Test Suite: " << config.name << std::endl;
    ss << "Description: " << config.description << std::endl;
    ss << std::endl;
    
    ss << "=== Test Statistics ===" << std::endl;
    ss << "Total Test Scenarios: " << config.scenarios.size() << std::endl;
    ss << "Performance Benchmarks: " << config.benchmarks.size() << std::endl;
    ss << "Memory Leak Tests: " << config.memory_configs.size() << std::endl;
    ss << "Stress Tests: " << config.stress_configs.size() << std::endl;
    ss << "Integration Tests: " << config.integration_configs.size() << std::endl;
    ss << "Cross-Platform Tests: " << config.platform_configs.size() << std::endl;
    ss << "Regression Tests: " << config.regression_configs.size() << std::endl;
    ss << std::endl;
    
    ss << "=== Test Categories ===" << std::endl;
    std::map<TestCategory, int> category_counts;
    for (const auto& scenario : config.scenarios) {
        category_counts[scenario.category]++;
    }
    
    for (const auto& pair : category_counts) {
        std::string category_name = get_category_name(pair.first);
        ss << "  " << category_name << ": " << pair.second << std::endl;
    }
    
    ss << std::endl;
    ss << "=== Test Priority Levels ===" << std::endl;
    std::map<TestPriority, int> priority_counts;
    for (const auto& scenario : config.scenarios) {
        priority_counts[scenario.priority]++;
    }
    
    for (const auto& pair : priority_counts) {
        std::string priority_name = get_priority_name(pair.first);
        ss << "  " << priority_name << ": " << pair.second << std::endl;
    }
    
    ss << std::endl;
    ss << "=== Platform Targets ===" << std::endl;
    std::map<Platform, int> platform_counts;
    for (const auto& platform_config : config.platform_configs) {
        for (const auto& platform : platform_config.target_platforms) {
            platform_counts[platform]++;
        }
    }
    
    for (const auto& pair : platform_counts) {
        std::string platform_name = get_platform_name(pair.first);
        ss << "  " << platform_name << ": " << pair.second << std::endl;
    }
    
    ss << std::endl;
    ss << "=== Test Configuration Details ===" << std::endl;
    ss << "Overall Timeout: " << config.overall_timeout.count() << " seconds" << std::endl;
    ss << "Setup Timeout: " << config.setup_timeout.count() << " seconds" << std::endl;
    ss << "Teardown Timeout: " << config.teardown_timeout.count() << " seconds" << std::endl;
    ss << "Parallel Execution: " << (config.enable_parallel_execution ? "Enabled" : "Disabled") << std::endl;
    ss << "Max Parallel Threads: " << config.max_parallel_threads << std::endl;
    ss << std::endl;
    ss << "=== Reporting Configuration ===" << std::endl;
    ss << "HTML Reports: " << (config.generate_html_reports ? "Enabled" : "Disabled") << std::endl;
    ss << "JSON Reports: " << (config.generate_json_reports ? "Enabled" : "Disabled") << std::endl;
    ss << "XML Reports: " << (config.generate_xml_reports ? "Enabled" : "Disabled") << std::endl;
    ss << "CSV Reports: " << (config.generate_csv_reports ? "Enabled" : "Disabled") << std::endl;
    
    if (!config.report_output_directory.empty()) {
        ss << "Report Output Directory: " << config.report_output_directory << std::endl;
    }
    
    return ss.str();
}

std::string TestConfigSummaryGenerator::get_category_name(TestCategory category) {
    switch (category) {
        case TestCategory::MEMORY_LEAK: return "Memory Leak Detection";
        case TestCategory::STRESS: return "Stress Testing";
        case TestCategory::PERFORMANCE: return "Performance Benchmarking";
        case TestCategory::EDGE_CASE: return "Edge Case Testing";
        case TestCategory::INTEGRATION: return "Integration Testing";
        case TestCategory::CROSS_PLATFORM: return "Cross-Platform Testing";
        case TestCategory::REGRESSION: return "Regression Testing";
        case TestCategory::UNIT: return "Unit Testing";
        case TestCategory::SYSTEM: return "System Testing";
        case TestCategory::NETWORK: return "Network Testing";
        case TestCategory::BRAIN: return "Brain Testing";
        case TestCategory::AGENT: return "Agent Testing";
        case TestCategory::WORLD: return "World Testing";
        case TestCategory::PLASTICITY: return "Plasticity Testing";
        case TestCategory::NEUROMODULATION: return "Neuromodulation Testing";
        case TestCategory::DEVELOPMMENT: return "Development Testing";
        case TestCategory::PREDICTION: return "Prediction Testing";
        case TestCategory::COGNITION: return "Cognition Testing";
        case TestCategory::SENSOR: return "Sensory Testing";
        case TestCategory::MOTOR: return "Motor Testing";
        case TestCategory::ENVIRONMENT: return "Environment Testing";
        case TestCategory::EXPERIMENT: return "Experiment Testing";
        default: return "Unknown";
    }
}

std::string TestConfigSummaryGenerator::get_priority_name(TestPriority priority) {
    switch (priority) {
        case TestPriority::CRITICAL: return "Critical";
        case TestPriority::HIGH: return "High";
        case TestPriority::MEDIUM: return "Medium";
        case TestPriority::LOW: return "Low";
        case TestPriority::MINIMAL: return "Minimal";
        default: return "Unknown";
    }
}

std::string TestConfigSummaryGenerator::get_platform_name(Platform platform) {
    switch (platform) {
        case Platform::ANY: return "Any Platform";
        case Platform::LINUX: return "Linux";
        case Platform::WINDOWS: return "Windows";
        case Platform::MACOS: return "macOS";
        case Platform::ANDROID: return "Android";
        case Platform::IOS: return "iOS";
        case Platform::RASPBIAN: return "Raspberry Pi";
        default: return "Unknown";
    }
}

// Test configuration comparison
bool TestConfigComparison::are_configs_equal(const TestSuiteConfig& config1, const TestSuiteConfig& config2) {
    if (config1.name != config2.name) return false;
    if (config1.description != config2.description) return false;
    if (config1.overall_timeout != config2.overall_timeout) return false;
    if (config1.setup_timeout != config2.setup_timeout) return false;
    if (config1.teardown_timeout != config2.teardown_timeout) return false;
    if (config1.enable_parallel_execution != config2.enable_parallel_execution) return false;
    if (config1.max_parallel_threads != config2.max_parallel_threads) return false;
    if (config1.generate_html_reports != config2.generate_html_reports) return false;
    if (config1.generate_json_reports != config2.generate_json_reports) return false;
    if (config1.generate_xml_reports != config2.generate_xml_reports) return false;
    if (config1.generate_csv_reports != config2.generate_csv_reports) return false;
    if (config1.scenarios.size() != config2.scenarios.size()) return false;
    if (config1.benchmarks.size() != config2.benchmarks.size()) return false;
    if (config1.memory_configs.size() != config2.memory_configs.size()) return false;
    if (config1.stress_configs.size() != config2.stress_configs.size()) return false;
    if (config1.integration_configs.size() != config2.integration_configs.size()) return false;
    if (config1.platform_configs.size() != config2.platform_configs.size()) return false;
    if (config1.regression_configs.size() != config2.regression_configs.size()) return false;
    
    // Compare scenarios
    for (size_t i = 0; i < config1.scenarios.size(); ++i) {
        if (!are_test_scenarios_equal(config1.scenarios[i], config2.scenarios[i])) return false;
    }
    
    // Compare benchmarks
    for (size_t i = 0; i < config1.benchmarks.size(); ++i) {
        if (!are_performance_benchmarks_equal(config1.benchmarks[i], config2.benchmarks[i])) return false;
    }
    
    // Compare memory configs
    for (size_t i = 0; i < config1.memory_configs.size(); ++i) {
        if (!are_memory_configs_equal(config1.memory_configs[i], config2.memory_configs[i])) return false;
    }
    
    // Compare stress configs
    for (size_t i = 0; i < config1.stress_configs.size(); ++i) {
        if (!are_stress_configs_equal(config1.stress_configs[i], config2.stress_configs[i])) return false;
    }
    
    // Compare integration configs
    for (size_t i = 0; i < config1.integration_configs.size(); ++i) {
        if (!are_integration_configs_equal(config1.integration_configs[i], config2.integration_configs[i])) return false;
    }
    
    // Compare platform configs
    for (size_t i = 0; i < config1.platform_configs.size(); ++i) {
        if (!are_cross_platform_configs_equal(config1.platform_configs[i], config2.platform_configs[i])) return false;
    }
    
    // Compare regression configs
    for (size_t i = 0; i < config1.regression_configs.size(); ++i) {
        if (!are_regression_configs_equal(config1.regression_configs[i], config2.regression_configs[i])) return false;
    }
    
    return true;
}

bool TestConfigComparison::are_test_scenarios_equal(const TestScenario& scenario1, const TestScenario& scenario2) {
    return scenario1.name == scenario2.name &&
           scenario1.description == scenario2.description &&
           scenario1.category == scenario2.category &&
           scenario1.priority == scenario2.priority &&
           scenario1.timeout == scenario2.timeout &&
           scenario1.enabled == scenario2.enabled;
}

bool TestConfigComparison::are_performance_benchmarks_equal(const PerformanceBenchmark& benchmark1, const PerformanceBenchmark& benchmark2) {
    return benchmark1.name == benchmark2.name &&
           benchmark1.description == benchmark2.description &&
           benchmark1.mode == benchmark2.mode &&
           benchmark1.threshold_value == benchmark2.threshold_value &&
           benchmark1.iterations == benchmark2.iterations;
}

bool TestConfigComparison::are_memory_configs_equal(const MemoryLeakConfig& config1, const MemoryLeakConfig& config2) {
    return config1.mode == config2.mode &&
           config1.detection_duration == config2.detection_duration;
}

bool TestConfigComparison::are_stress_configs_equal(const StressTestConfig& config1, const StressTestConfig& config2) {
    return config1.name == config2.name &&
           config1.description == config2.description &&
           config1.duration == config2.duration &&
           config1.target_load == config2.target_load &&
           config1.concurrent_threads == config2.concurrent_threads;
}

bool TestConfigComparison::are_integration_configs_equal(const IntegrationConfig& config1, const IntegrationConfig& config2) {
    return config1.name == config2.name &&
           config1.description == config2.description &&
           config1.component_names == config2.component_names;
}

bool TestConfigComparison::are_cross_platform_configs_equal(const CrossPlatformConfig& config1, const CrossPlatformConfig& config2) {
    return config1.name == config2.name &&
           config1.description == config2.description &&
           config1.target_platforms == config2.target_platforms;
}

bool TestConfigComparison::are_regression_configs_equal(const RegressionConfig& config1, const RegressionConfig& config2) {
    return config1.name == config2.name &&
           config1.description == config2.description &&
           config1.baseline_file == config2.baseline_file &&
           config1.tolerance_value == config2.tolerance_value;
}

// Configuration merger
TestSuiteConfig TestConfigMerger::merge_configs(const TestSuiteConfig& config1, const TestSuiteConfig& config2) {
    TestSuiteConfig merged_config = config1;
    
    // Merge scenarios (config2 takes precedence)
    std::map<std::string, TestScenario> scenario_map;
    for (const auto& scenario : config1.scenarios) {
        scenario_map[scenario.name] = scenario;
    }
    for (const auto& scenario : config2.scenarios) {
        scenario_map[scenario.name] = scenario;
    }
    
    merged_config.scenarios.clear();
    for (const auto& pair : scenario_map) {
        merged_config.scenarios.push_back(pair.second);
    }
    
    // Merge benchmarks (config2 takes precedence)
    std::map<std::string, PerformanceBenchmark> benchmark_map;
    for (const auto& benchmark : config1.benchmarks) {
        benchmark_map[benchmark.name] = benchmark;
    }
    for (const auto& benchmark : config2.benchmarks) {
        benchmark_map[benchmark.name] = benchmark;
    }
    
    merged_config.benchmarks.clear();
    for (const auto& pair : benchmark_map) {
        merged_config.benchmarks.push_back(pair.second);
    }
    
    // Merge other config types
    // (similar merging for other config types)
    
    return merged_config;
}

TestSuiteConfig TestConfigMerger::merge_config_with_defaults(const TestSuiteConfig& config) {
    TestSuiteConfig merged_config = config;
    
    // Apply default values for missing settings
    if (merged_config.overall_timeout.count() == 0) {
        merged_config.overall_timeout = std::chrono::seconds(3600);
    }
    
    if (merged_config.setup_timeout.count() == 0) {
        merged_config.setup_timeout = std::chrono::seconds(300);
    }
    
    if (merged_config.teardown_timeout.count() == 0) {
        merged_config.teardown_timeout = std::chrono::seconds(300);
    }
    
    merged_config.enable_parallel_execution = true;
    
    if (merged_config.max_parallel_threads <= 0) {
        merged_config.max_parallel_threads = std::thread::hardware_concurrency();
    }
    
    if (merged_config.report_output_directory.empty()) {
        merged_config.report_output_directory = "test_reports";
    }
    
    return merged_config;
}

TestSuiteConfig TestConfigMerger::create_test_config_from_file(const std::string& filename) {
    TestConfigManager manager;
    if (manager.load_config(filename)) {
        return manager.get_config();
    }
    
    // Return empty config if loading fails
    return TestSuiteConfig("Empty Config", "Empty test configuration");
}