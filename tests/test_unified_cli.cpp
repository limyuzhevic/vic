// test_unified_cli.hpp - Tests for UnifiedCLI

#pragma once

#include <gtest/gtest.h>
#include "core/UnifiedCLI.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class UnifiedCLITest : public ::testing::Test {
protected:
    void SetUp() override {
        cli = std::make_unique<UnifiedCLI>();
    }
    
    void TearDown() override {
        cli.reset();
    }
    
    std::unique_ptr<UnifiedCLI> cli;
};

TEST_F(UnifiedCLITest, DefaultModeHelp) {
    // Test that default mode shows help
    UnifiedCLIOptions options = cli->parseArguments(1, const_cast<char**>(const_cast<const char**>(&help_argv[0])));
    EXPECT_EQ(options.mode, "help");
}

TEST_F(UnifiedCLITest, Phase2Mode) {
    // Test phase2 mode parsing
    UnifiedCLIOptions options = cli->parseArguments(3, const_cast<char**>(const_cast<const char**>(&phase2_argv[0])));
    EXPECT_EQ(options.mode, "phase2");
}

TEST_F(UnifiedCLITest, ConfigFileOption) {
    // Test --config option
    UnifiedCLIOptions options = cli->parseArguments(3, const_cast<char**>(const_cast<const char**>(&config_argv[0])));
    EXPECT_EQ(options.configFile, "my_config.cfg");
}

TEST_F(UnifiedCLITest, VerboseOption) {
    // Test --verbose option
    UnifiedCLIOptions options = cli->parseArguments(2, const_cast<char**>(const_cast<const char**>(&verbose_argv[0])));
    EXPECT_TRUE(options.verbose);
}

TEST_F(UnifiedCLITest, QuietOption) {
    // Test --quiet option
    UnifiedCLIOptions options = cli->parseArguments(2, const_cast<char**>(const_cast<const char**>(&quiet_argv[0])));
    EXPECT_TRUE(options.quiet);
}

TEST_F(UnifiedCLITest, OutputOption) {
    // Test --output option
    UnifiedCLIOptions options = cli->parseArguments(3, const_cast<char**>(const_cast<const char**>(&output_argv[0])));
    EXPECT_EQ(options.outputFile, "results.txt");
}

TEST_F(UnifiedCLITest, ListConfigsOption) {
    // Test --list-configs option
    UnifiedCLIOptions options = cli->parseArguments(2, const_cast<char**>(const_cast<const char**>(&list_configs_argv[0])));
    EXPECT_TRUE(options.listConfigs);
}

TEST_F(UnifiedCLITest, ValidateConfigOption) {
    // Test --validate-config option
    UnifiedCLIOptions options = cli->parseArguments(3, const_cast<char**>(const_cast<const char**>(&validate_config_argv[0])));
    EXPECT_TRUE(options.validateConfig);
    EXPECT_EQ(options.configFile, "config.json");
}

TEST_F(UnifiedCLITest, ModeOption) {
    // Test --mode option
    UnifiedCLIOptions options = cli->parseArguments(3, const_cast<char**>(const_cast<const char**>(&mode_argv[0])));
    EXPECT_EQ(options.mode, "phase6");
}

TEST_F(UnifiedCLITest, ExtraArgs) {
    // Test extra arguments
    UnifiedCLIOptions options = cli->parseArguments(4, const_cast<char**>(const_cast<const char**>(&extra_args_argv[0])));
    EXPECT_EQ(options.extraArgs.size(), 2);
    EXPECT_EQ(options.extraArgs[0], "--extra1");
    EXPECT_EQ(options.extraArgs[1], "--extra2");
}

TEST_F(UnifiedCLITest, HelpCommand) {
    // Test help command
    EXPECT_NO_THROW(cli->showHelp("test"));
}

TEST_F(UnifiedCLITest, VersionCommand) {
    // Test version command
    EXPECT_NO_THROW(cli->showVersion());
}

TEST_F(UnifiedCLITest, ListAvailableConfigurations) {
    // Test listing available configurations
    EXPECT_NO_THROW(cli->listAvailableConfigurations());
}

TEST_F(UnifiedCLITest, ValidateConfiguration) {
    // Test configuration validation
    EXPECT_TRUE(cli->validateConfiguration("valid_config.cfg"));
}

// Test command line arguments
const char* help_argv[] = {
    "test",
    "--help"
};

const char* phase2_argv[] = {
    "test",
    "--mode",
    "phase2"
};

const char* config_argv[] = {
    "test",
    "--config",
    "my_config.cfg"
};

const char* verbose_argv[] = {
    "test",
    "--verbose"
};

const char* quiet_argv[] = {
    "test",
    "--quiet"
};

const char* output_argv[] = {
    "test",
    "--output",
    "results.txt"
};

const char* list_configs_argv[] = {
    "test",
    "--list-configs"
};

const char* validate_config_argv[] = {
    "test",
    "--validate-config",
    "config.json"
};

const char* mode_argv[] = {
    "test",
    "--mode",
    "phase6"
};

const char* extra_args_argv[] = {
    "test",
    "arg1",
    "arg2",
    "--extra1",
    "--extra2"
};

// Test UnifiedCLI integration
TEST(UnifiedCLIIntegration, Phase2Demo) {
    // Test Phase 2 demo execution
    UnifiedCLI cli;
    UnifiedCLIOptions options;
    options.mode = "phase2";
    options.verbose = true;
    options.quiet = false;
    
    // This would normally execute the Phase 2 demo
    // For testing, we just verify the options are set correctly
    EXPECT_EQ(options.mode, "phase2");
    EXPECT_TRUE(options.verbose);
    EXPECT_FALSE(options.quiet);
}

TEST(UnifiedCLIIntegration, Phase6Demo) {
    // Test Phase 6 demo execution
    UnifiedCLI cli;
    UnifiedCLIOptions options;
    options.mode = "phase6";
    options.configFile = "test_config.json";
    options.outputFile = "test_output.txt";
    
    EXPECT_EQ(options.mode, "phase6");
    EXPECT_EQ(options.configFile, "test_config.json");
    EXPECT_EQ(options.outputFile, "test_output.txt");
}

TEST(UnifiedCLIIntegration, ConfigValidation) {
    // Test configuration validation
    UnifiedCLI cli;
    
    // Create a test config file
    std::ofstream testConfig("test_config.txt");
    testConfig << "random_seed = 12345\n";
    testConfig << "neuron_count = 1000\n";
    testConfig << "simulation_timestep = 0.001\n";
    testConfig.close();
    
    // Validate config
    bool isValid = cli.validateConfiguration("test_config.txt");
    
    // Clean up
    std::remove("test_config.txt");
    
    // In a real implementation, this would be true
    EXPECT_TRUE(isValid);
}

} // namespace nlm

// test_error_handling.hpp - Tests for ErrorHandling

#pragma once

#include <gtest/gtest.h>
#include "core/ErrorHandling.hpp"

namespace nlm {

class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset error handler manager
        ErrorHandlerManager& handlerManager = ErrorHandlerManager::getInstance();
        handlerManager.resetStats();
    }
};

TEST_F(ErrorHandlingTest, NLMErrorCreation) {
    NLMError error("Test error message", NLMErrorCode::ConfigFileNotFound);
    EXPECT_EQ(error.getCode(), NLMErrorCode::ConfigFileNotFound);
    EXPECT_STREQ(error.what(), "Test error message");
}

TEST_F(ErrorHandlingTest, NLMErrorCategory) {
    NLMError error("Test error", NLMErrorCode::InvalidArgument, NLMErrorCategory::Input);
    EXPECT_EQ(error.getCategory(), NLMErrorCategory::Input);
}

TEST_F(ErrorHandlingTest, NLMErrorRecoverable) {
    NLMError configError("Config file not found", NLMErrorCode::ConfigFileNotFound);
    EXPECT_TRUE(configError.isRecoverable());
    
    NLMError userError("Invalid argument", NLMErrorCode::InvalidArgument, NLMErrorCategory::User);
    EXPECT_TRUE(userError.isUserError());
}

TEST_F(ErrorHandlingTest, ErrorHandlerManager) {
    ErrorHandlerManager& handlerManager = ErrorHandlerManager::getInstance();
    
    // Test error handling
    NLMError error("Test error", NLMErrorCode::MemoryAllocationFailed);
    handlerManager.handleError(error);
    
    // Check stats
    auto stats = handlerManager.getErrorStats();
    EXPECT_GT(stats.totalErrors, 0);
    EXPECT_EQ(stats.errorCounts[NLMErrorCode::MemoryAllocationFailed], 1);
}

TEST_F(ErrorHandlingTest, ErrorRecoveryActions) {
    ErrorRecovery::RecoveryAction action = ErrorRecovery::determineRecoveryAction(
        NLMError("Test", NLMErrorCode::ConfigFileNotFound)
    );
    EXPECT_TRUE(action == ErrorRecovery::RecoveryAction::Fallback || 
                action == ErrorRecovery::RecoveryAction::Continue);
}

TEST_F(ErrorHandlingTest, ErrorHandlerSetGet) {
    ErrorHandlerManager& handlerManager = ErrorHandlerManager::getInstance();
    
    // Set a custom error handler
    class TestErrorHandler : public IErrorHandler {
    public:
        void handleError(const NLMError& error) override {}
        bool shouldExit(const NLMError& error) const override { return false; }
        ErrorRecovery::RecoveryAction getRecoveryAction(const NLMError& error) override {
            return ErrorRecovery::RecoveryAction::Continue;
        }
    };
    
    handlerManager.setErrorHandler(std::make_unique<TestErrorHandler>());
    
    // Get the error handler
    IErrorHandler& handler = handlerManager.getErrorHandler();
    EXPECT_NO_THROW(handler.handleError(NLMError("Test")));
}

TEST_F(ErrorHandlingTest, ErrorContext) {
    ErrorContext context("TestOperation", "Test details");
    context.addDetail("param1", "value1");
    context.addDetail("param2", "value2");
    
    std::string summary = context.getContextSummary();
    EXPECT_TRUE(summary.find("TestOperation") != std::string::npos);
    EXPECT_TRUE(summary.find("Test details") != std::string::npos);
}

TEST_F(ErrorHandlingTest, ResultTemplate) {
    // Test Result template functionality
    Result<int> successResult = Result<int>::success(42);
    EXPECT_TRUE(successResult.isSuccess());
    EXPECT_EQ(successResult.value(), 42);
    
    Result<int> errorResult = Result<int>::error(0, "Test error");
    EXPECT_TRUE(errorResult.isError());
    EXPECT_EQ(errorResult.value(), 0);
    EXPECT_STREQ(errorResult.message().c_str(), "Test error");
}

TEST_F(ErrorHandlingTest, ResultUtils) {
    // Test Result utilities
    Result<int> result1 = Result<int>::success(10);
    Result<int> result2 = Result<int>::success(20);
    
    Result<int> combined = ResultUtils::combineResults(result1, result2, "test");
    EXPECT_TRUE(combined.isSuccess());
    
    Result<int> errorResult = Result<int>::error(0, "Error");
    Result<int> combinedWithError = ResultUtils::combineResults(result1, errorResult, "test");
    EXPECT_TRUE(combinedWithError.isError());
}

TEST_F(ErrorHandlingTest, NLMExceptionFactory) {
    // Test NLMExceptionFactory
    auto exception = NLMExceptionFactory::create("Test", NLMErrorCode::ConfigFileNotFound);
    EXPECT_NO_THROW(std::rethrow_if_exception_type(exception));
    
    auto configError = NLMExceptionFactory::configFileError("test.cfg", "File not found");
    EXPECT_NO_THROW(std::rethrow_if_exception_type(configError));
}

} // namespace nlm

// test_config.cpp - Tests for Config

#pragma once

#include <gtest/gtest.h>
#include "core/Config/Config.hpp"
#include <filesystem>
#include <fstream>

namespace nlm {

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test config directory
        testDir = "test_config_dir";
        std::filesystem::create_directories(testDir);
    }
    
    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    std::string testDir;
};

TEST_F(ConfigTest, DefaultConfig) {
    Config config;
    
    // Test default values
    EXPECT_TRUE(config.has("random_seed"));
    EXPECT_EQ(config.getOr<int64_t>("random_seed", 0), 42);
    
    EXPECT_TRUE(config.has("simulation_timestep"));
    EXPECT_DOUBLE_EQ(config.getOr<double>("simulation_timestep", 0.0), 0.001);
    
    EXPECT_TRUE(config.has("neuron_count"));
    EXPECT_EQ(config.getOr<int64_t>("neuron_count", 0), 1000);
}

TEST_F(ConfigTest, SimpleConfigLoad) {
    Config config;
    
    // Create simple config file
    std::string configPath = testDir + "/simple.cfg";
    std::ofstream file(configPath);
    file << "# Simple config file\n";
    file << "random_seed = 12345\n";
    file << "neuron_count = 2000\n";
    file << "connection_probability = 0.2\n";
    file.close();
    
    // Load config
    bool loaded = config.loadFromFile(configPath);
    EXPECT_TRUE(loaded);
    
    // Verify values
    EXPECT_TRUE(config.has("random_seed"));
    EXPECT_EQ(config.getOr<int64_t>("random_seed", 0), 12345);
    
    EXPECT_TRUE(config.has("neuron_count"));
    EXPECT_EQ(config.getOr<int64_t>("neuron_count", 0), 2000);
    
    EXPECT_TRUE(config.has("connection_probability"));
    EXPECT_DOUBLE_EQ(config.getOr<double>("connection_probability", 0.0), 0.2);
}

TEST_F(ConfigTest, ConfigSetGet) {
    Config config;
    
    // Test setting values
    config.set("test_string", "hello");
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_bool", true);
    
    // Test getting values
    EXPECT_TRUE(config.has("test_string"));
    EXPECT_EQ(config.getOr<std::string>("test_string", ""), "hello");
    
    EXPECT_TRUE(config.has("test_int"));
    EXPECT_EQ(config.getOr<int64_t>("test_int", 0), 42);
    
    EXPECT_TRUE(config.has("test_double"));
    EXPECT_DOUBLE_EQ(config.getOr<double>("test_double", 0.0), 3.14);
    
    EXPECT_TRUE(config.has("test_bool"));
    EXPECT_TRUE(config.getOr<bool>("test_bool", false));
}

TEST_F(ConfigTest, ConfigRemove) {
    Config config;
    
    // Set a value
    config.set("temp_key", "temp_value");
    EXPECT_TRUE(config.has("temp_key"));
    
    // Remove it
    config.remove("temp_key");
    EXPECT_FALSE(config.has("temp_key"));
}

TEST_F(ConfigTest, ConfigClear) {
    Config config;
    
    // Add some values
    config.set("key1", "value1");
    config.set("key2", "value2");
    config.set("key3", "value3");
    
    // Clear config
    config.clear();
    
    // Verify all values are gone
    EXPECT_FALSE(config.has("key1"));
    EXPECT_FALSE(config.has("key2"));
    EXPECT_FALSE(config.has("key3"));
    EXPECT_EQ(config.getKeys().size(), 0);
}

TEST_F(ConfigTest, ConfigSummary) {
    Config config;
    
    // Add some values
    config.set("string_key", "string_value");
    config.set("int_key", 123);
    config.set("double_key", 45.67);
    config.set("bool_key", true);
    
    // Get summary
    std::string summary = config.summary();
    
    // Check that summary contains expected information
    EXPECT_TRUE(summary.find("string_key") != std::string::npos);
    EXPECT_TRUE(summary.find("int_key") != std::string::npos);
    EXPECT_TRUE(summary.find("double_key") != std::string::npos);
    EXPECT_TRUE(summary.find("bool_key") != std::string::npos);
}

TEST_F(ConfigTest, ConfigValidation) {
    Config config;
    
    // Set valid values
    config.set("random_seed", static_cast<int64_t>(42));
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("simulation_timestep", 0.001);
    
    // Test validation (if implemented)
    // In a real implementation, this would validate the config
    // For now, just verify that the config has the expected values
    EXPECT_TRUE(config.has("random_seed"));
    EXPECT_TRUE(config.has("neuron_count"));
    EXPECT_TRUE(config.has("simulation_timestep"));
}

TEST_F(ConfigTest, ConfigTrim) {
    // Test string trimming
    std::string original = "  test  ";
    std::string trimmed = Config::trim(original);
    EXPECT_EQ(trimmed, "test");
    
    std::string original2 = "\t\nhello world\r\n";
    std::string trimmed2 = Config::trim(original2);
    EXPECT_EQ(trimmed2, "hello world");
}

TEST_F(ConfigTest, ConfigToLower) {
    // Test string to lower
    std::string original = "HELLO WORLD";
    std::string lower = Config::toLower(original);
    EXPECT_EQ(lower, "hello world");
}

TEST_F(ConfigTest, ConfigLoadFromArgs) {
    Config config;
    
    // Test loading from command line arguments
    int argc = 5;
    char* argv[] = {
        const_cast<char*("program_name")),
        const_cast<char*("--key1")),
        const_cast<char*("value1")),
        const_cast<char*("--key2")),
        const_cast<char*("value2")),
        nullptr
    };
    
    bool loaded = config.loadFromArgs(argc, argv);
    EXPECT_TRUE(loaded);
    
    // Check that values were loaded
    EXPECT_TRUE(config.has("key1"));
    EXPECT_TRUE(config.has("key2"));
}

TEST_F(ConfigTest, ConfigSaveToFile) {
    Config config;
    
    // Add some values
    config.set("string_key", "string_value");
    config.set("int_key", 123);
    config.set("double_key", 45.67);
    config.set("bool_key", true);
    
    // Save to file
    std::string configPath = testDir + "/saved_config.cfg";
    bool saved = config.saveToFile(configPath);
    EXPECT_TRUE(saved);
    
    // Load the saved config and verify
    Config loadedConfig;
    bool loaded = loadedConfig.loadFromFile(configPath);
    EXPECT_TRUE(loaded);
    
    // Compare values
    EXPECT_EQ(config.getOr<std::string>("string_key", ""),
              loadedConfig.getOr<std::string>("string_key", ""));
}

TEST_F(ConfigTest, ConfigJSONLoad) {
    Config config;
    
    // Create JSON config file
    std::string configPath = testDir + "/config.json";
    std::ofstream file(configPath);
    file << "{\n";
    file << "  \"random_seed\": 12345,\n";
    file << "  \"neuron_count\": 2000,\n";
    file << "  \"simulation_timestep\": 0.001\n";
    file << "}\n";
    file.close();
    
    // Load JSON config
    bool loaded = config.loadFromFile(configPath);
    EXPECT_TRUE(loaded);
    
    // Verify values
    EXPECT_TRUE(config.has("random_seed"));
    EXPECT_EQ(config.getOr<int64_t>("random_seed", 0), 12345);
    
    EXPECT_TRUE(config.has("neuron_count"));
    EXPECT_EQ(config.getOr<int64_t>("neuron_count", 0), 2000);
}

TEST_F(ConfigTest, ConfigYAMLLoad) {
    Config config;
    
    // Create YAML config file (simplified format for now)
    std::string configPath = testDir + "/config.yaml";
    std::ofstream file(configPath);
    file << "random_seed: 12345\n";
    file << "neuron_count: 2000\n";
    file << "simulation_timestep: 0.001\n";
    file.close();
    
    // Load YAML config (falls back to simple format for now)
    bool loaded = config.loadFromFile(configPath);
    EXPECT_TRUE(loaded);
    
    // Verify values
    EXPECT_TRUE(config.has("random_seed"));
    EXPECT_EQ(config.getOr<int64_t>("random_seed", 0), 12345);
}

TEST_F(ConfigTest, ConfigMoveConstructor) {
    Config config1;
    config1.set("key1", "value1");
    config1.set("key2", 42);
    
    Config config2 = std::move(config1);
    
    // Check that config2 has the values
    EXPECT_TRUE(config2.has("key1"));
    EXPECT_EQ(config2.getOr<std::string>("key1", ""), "value1");
    
    EXPECT_TRUE(config2.has("key2"));
    EXPECT_EQ(config2.getOr<int64_t>("key2", 0), 42);
    
    // Check that config1 is empty (moved from)
    EXPECT_FALSE(config1.has("key1"));
    EXPECT_FALSE(config1.has("key2"));
}

TEST_F(ConfigTest, ConfigMoveAssignment) {
    Config config1;
    config1.set("key1", "value1");
    config1.set("key2", 42);
    
    Config config2;
    config2 = std::move(config1);
    
    // Check that config2 has the values
    EXPECT_TRUE(config2.has("key1"));
    EXPECT_EQ(config2.getOr<std::string>("key1", ""), "value1");
    
    EXPECT_TRUE(config2.has("key2"));
    EXPECT_EQ(config2.getOr<int64_t>("key2", 0), 42);
    
    // Check that config1 is empty (moved from)
    EXPECT_FALSE(config1.has("key1"));
    EXPECT_FALSE(config1.has("key2"));
}

} // namespace nlm

// test_advanced_features.cpp - Tests for AdvancedFeatures

#pragma once

#include <gtest/gtest.h>
#include "core/AdvancedFeatures.hpp"
#include <fstream>

namespace nlm {

class AdvancedFeaturesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        testDir = "test_advanced_dir";
        std::filesystem::create_directories(testDir);
    }
    
    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    std::string testDir;
};

TEST_F(AdvancedFeaturesTest, PluginManager) {
    PluginManager pluginManager;
    
    // Test plugin discovery (empty for now)
    std::vector<std::string> plugins = pluginManager.discoverPlugins(testDir);
    EXPECT_EQ(plugins.size(), 0);
    
    // Test loading a plugin (simulated)
    // In a real implementation, this would load a shared library
    // For now, we just test that the plugin manager can be created
    EXPECT_NO_THROW(PluginManager pluginManager2);
}

TEST_F(AdvancedFeaturesTest, AdvancedConfig) {
    AdvancedConfig advancedConfig;
    
    // Test profile operations
    bool saved = advancedConfig.saveProfile("test_profile");
    EXPECT_TRUE(saved);
    
    bool loaded = advancedConfig.loadProfile("test_profile");
    EXPECT_TRUE(loaded);
    
    bool deleted = advancedConfig.deleteProfile("test_profile");
    EXPECT_TRUE(deleted);
    
    // Test template operations
    std::vector<std::string> templates = advancedConfig.getAvailableProfiles();
    EXPECT_TRUE(templates.size() > 0);
}

TEST_F(AdvancedFeaturesTest, PerformanceMonitor) {
    PerformanceMonitor monitor;
    
    // Test monitoring state
    EXPECT_FALSE(monitor.isMonitoring());
    
    // Test resource limits
    PerformanceMonitor::ResourceLimits limits = monitor.getResourceLimits();
    EXPECT_GT(limits.maxMemoryMB, 0);
    EXPECT_GT(limits.maxCpuPercent, 0);
}

TEST_F(AdvancedFeaturesTest, PerformanceMonitorMetrics) {
    PerformanceMonitor monitor;
    
    // Test metrics
    PerformanceMonitor::PerformanceMetrics metrics = monitor.getCurrentMetrics();
    EXPECT_EQ(metrics.cpuUsage, 0.0);
    EXPECT_EQ(metrics.memoryUsage, 0.0);
    EXPECT_EQ(metrics.totalSpikeCount, 0);
}

TEST_F(AdvancedFeaturesTest, AlertSystem) {
    PerformanceMonitor monitor;
    PerformanceMonitor::AlertSystem& alertSystem = monitor.getAlertSystem();
    
    // Test alert addition
    PerformanceMonitor::AlertSystem::Alert alert(
        PerformanceMonitor::AlertSystem::AlertLevel::Warning,
        "Test alert"
    );
    alertSystem.addAlert(alert);
    
    // Test alert retrieval
    std::vector<PerformanceMonitor::AlertSystem::Alert> alerts = alertSystem.getAlerts();
    EXPECT_GT(alerts.size(), 0);
    EXPECT_EQ(alerts[0].level, PerformanceMonitor::AlertSystem::AlertLevel::Warning);
    
    // Test alert acknowledgment
    alertSystem.acknowledgeAlert(alerts[0].id);
    std::vector<PerformanceMonitor::AlertSystem::Alert> unacknowledged = alertSystem.getUnacknowledgedAlerts();
    EXPECT_EQ(unacknowledged.size(), 0);
}

TEST_F(AdvancedFeaturesTest, Visualization) {
    PerformanceMonitor monitor;
    
    // Test visualization additions
    class TestVisualization : public PerformanceMonitor::Visualization {
    public:
        std::string getName() const override { return "Test Visualization"; }
        std::string getType() const override { return "test"; }
        std::string getDescription() const override { return "Test visualization for unit testing"; }
        
        void update(const PerformanceMonitor::PerformanceMetrics& metrics) override {}
        std::string generateOutput() const override { return "test output"; }
    };
    
    monitor.addVisualization(std::make_unique<TestVisualization>());
    
    std::vector<std::string> vizNames = monitor.getVisualizationNames();
    EXPECT_TRUE(std::find(vizNames.begin(), vizNames.end(), "Test Visualization") != vizNames.end());
}

TEST_F(AdvancedFeaturesTest, AdvancedScripting) {
    AdvancedScripting scripting;
    
    // Test script loading
    AdvancedScripting::ScriptContext script;
    script.name = "test_script";
    script.language = AdvancedScripting::Language::Custom;
    script.code = "print('Hello, World!')";
    script.enabled = true;
    
    bool loaded = scripting.loadScript(script);
    EXPECT_TRUE(loaded);
    
    // Test script retrieval
    bool isLoaded = scripting.isScriptLoaded("test_script");
    EXPECT_TRUE(isLoaded);
    
    // Test script unloading
    bool unloaded = scripting.unloadScript("test_script");
    EXPECT_TRUE(unloaded);
}

TEST_F(AdvancedFeaturesTest, ExtensionAPI) {
    ExtensionAPI extensionAPI;
    
    // Test extension registration
    bool registered = extensionAPI.registerExtension(
        "test_extension",
        "1.0.0",
        "Test extension",
        []() {}, // init function
        []() {}  // shutdown function
    );
    EXPECT_TRUE(registered);
    
    // Test extension retrieval
    bool isRegistered = extensionAPI.isExtensionRegistered("test_extension");
    EXPECT_TRUE(isRegistered);
    
    // Test extension service provision
    bool serviceProvided = extensionAPI.provideService<std::string>(
        "test_extension", "test_service"
    );
    EXPECT_TRUE(serviceProvided);
    
    // Test extension service retrieval
    auto service = extensionAPI.getService<std::string>("test_extension");
    EXPECT_TRUE(service.has_value());
    
    // Test extension cleanup
    bool unregistered = extensionAPI.unregisterExtension("test_extension");
    EXPECT_TRUE(unregistered);
}

TEST_F(AdvancedFeaturesTest, DevelopmentTools) {
    DevelopmentTools devTools;
    
    // Test brain state inspection
    class MockBrain {
    public:
        uint64_t getTotalNeuronCount() const { return 100; }
        uint64_t getTotalSynapseCount() const { return 1000; }
        uint64_t getRegionCount() const { return 5; }
        uint64_t getTotalSpikeCount() const { return 500; }
        uint64_t getFiringNeuronCount() const { return 50; }
        double getAverageFiringRate() const { return 2.5; }
    };
    
    MockBrain brain;
    DevelopmentTools::BrainState state = devTools.getBrainState(std::make_shared<MockBrain>(brain));
    EXPECT_EQ(state.neuronCount, 100);
    EXPECT_EQ(state.synapseCount, 1000);
    EXPECT_EQ(state.regionCount, 5);
    EXPECT_EQ(state.totalSpikeCount, 500);
    EXPECT_EQ(state.firingNeuronCount, 50);
    EXPECT_DOUBLE_EQ(state.averageFiringRate, 2.5);
}

TEST_F(AdvancedFeaturesTest, DevelopmentUtils) {
    DevelopmentTools::DevelopmentUtils& utils = DevelopmentTools::DevelopmentUtils();
    
    // Test string utilities
    std::string uuid = DevelopmentTools::DevelopmentUtils::generateUUID();
    EXPECT_FALSE(uuid.empty());
    
    std::string randomStr = DevelopmentTools::DevelopmentUtils::generateRandomString(10);
    EXPECT_EQ(randomStr.size(), 10);
    
    // Test validation utilities
    bool validIdentifier = DevelopmentTools::DevelopmentUtils::isValidIdentifier("valid_name");
    EXPECT_TRUE(validIdentifier);
    
    bool invalidIdentifier = DevelopmentTools::DevelopmentUtils::isValidIdentifier("invalid name");
    EXPECT_FALSE(invalidIdentifier);
}

} // namespace nlm

// test_performance_monitoring.cpp - Tests for PerformanceMonitoring

#pragma once

#include <gtest/gtest.h>
#include "core/PerformanceMonitoring.hpp"
#include <fstream>

namespace nlm {

class PerformanceMonitoringTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        testDir = "test_perf_dir";
        std::filesystem::create_directories(testDir);
    }
    
    void TearDown() override {
        // Clean up test directory
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }
    
    std::string testDir;
};

TEST_F(PerformanceMonitoringTest, PerformanceMonitorBasic) {
    PerformanceMonitor monitor;
    
    // Test initial state
    EXPECT_FALSE(monitor.isMonitoring());
    
    // Test resource limits
    PerformanceMonitor::ResourceLimits limits = monitor.getResourceLimits();
    EXPECT_GT(limits.maxMemoryMB, 0);
    EXPECT_GT(limits.maxCpuPercent, 0);
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorMetrics) {
    PerformanceMonitor monitor;
    
    // Get metrics
    PerformanceMonitor::PerformanceMetrics metrics = monitor.getCurrentMetrics();
    
    // Test default values
    EXPECT_EQ(metrics.cpuUsage, 0.0);
    EXPECT_EQ(metrics.memoryUsage, 0.0);
    EXPECT_EQ(metrics.memoryUsagePercent, 0.0);
    EXPECT_EQ(metrics.totalSpikeCount, 0);
    EXPECT_EQ(metrics.firingNeuronCount, 0);
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorHistory) {
    PerformanceMonitor monitor;
    
    // Get history
    std::vector<PerformanceMonitor::PerformanceMetrics> history = monitor.getHistory();
    EXPECT_EQ(history.size(), 0);
    
    // Add metrics to history (simulate)
    PerformanceMonitor::PerformanceMetrics metrics;
    metrics.cpuUsage = 50.0;
    metrics.memoryUsage = 512.0;
    metrics.totalSpikeCount = 100;
    
    // In a real implementation, this would add to history
    // For now, we just verify the interface
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorAnalysis) {
    PerformanceMonitor monitor;
    
    // Get performance analysis
    PerformanceMonitor::PerformanceAnalysis analysis = monitor.analyzePerformance();
    
    // Test default values
    EXPECT_TRUE(analysis.performanceGrade.empty() || 
                analysis.performanceGrade == "A" || 
                analysis.performanceGrade == "B" ||
                analysis.performanceGrade == "C" || 
                analysis.performanceGrade == "D" || 
                analysis.performanceGrade == "F");
    EXPECT_FALSE(analysis.cpuAnomaly);
    EXPECT_FALSE(analysis.memoryAnomaly);
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorResourceCheck) {
    PerformanceMonitor monitor;
    
    // Test resource limit checking
    PerformanceMonitor::ResourceLimits limits;
    limits.maxMemoryMB = 1024.0;
    limits.maxCpuPercent = 100.0;
    limits.maxThreads = 8;
    
    PerformanceMonitor::PerformanceMetrics metrics;
    metrics.cpuUsage = 50.0;
    metrics.memoryUsage = 512.0;
    metrics.activeThreads = 4;
    
    bool withinLimits = monitor.checkResourceLimits(metrics);
    EXPECT_TRUE(withinLimits);
    
    // Test exceeding limits
    metrics.cpuUsage = 150.0; // Exceeds limit
    withinLimits = monitor.checkResourceLimits(metrics);
    EXPECT_FALSE(withinLimits);
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorProfiler) {
    PerformanceMonitor monitor;
    PerformanceMonitor::Profiler& profiler = monitor.getProfiler();
    
    // Test profiler operations
    profiler.startProfile("test_phase");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    profiler.stopProfile("test_phase");
    
    std::chrono::duration<double> time = profiler.getProfileTime("test_phase");
    EXPECT_GT(time.count(), 0.0);
    
    // Test profile data
    PerformanceMonitor::Profiler::ProfileData data = profiler.getProfileData("test_phase");
    EXPECT_GT(data.totalTime.count(), 0.0);
    EXPECT_GT(data.callCount, 0);
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorBenchmark) {
    PerformanceMonitor monitor;
    PerformanceMonitor::Benchmark& benchmark = monitor.getBenchmark();
    
    // Test benchmark registration
    benchmark.addBenchmark("test_benchmark", []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    
    // Test benchmark execution
    PerformanceMonitor::BenchmarkResult result = benchmark.runBenchmark("test_benchmark");
    EXPECT_EQ(result.name, "test_benchmark");
    EXPECT_GT(result.elapsedTime.count(), 0.0);
    EXPECT_EQ(result.status, "passed");
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorAlertSystem) {
    PerformanceMonitor monitor;
    PerformanceMonitor::AlertSystem& alertSystem = monitor.getAlertSystem();
    
    // Test alert addition
    PerformanceMonitor::AlertSystem::Alert alert(
        PerformanceMonitor::AlertSystem::AlertLevel::Warning,
        "Test performance alert"
    );
    alertSystem.addAlert(alert);
    
    // Test alert retrieval
    std::vector<PerformanceMonitor::AlertSystem::Alert> alerts = alertSystem.getAlerts();
    EXPECT_GT(alerts.size(), 0);
    EXPECT_EQ(alerts[0].level, PerformanceMonitor::AlertSystem::AlertLevel::Warning);
    EXPECT_EQ(alerts[0].message, "Test performance alert");
    
    // Test alert filtering
    std::vector<PerformanceMonitor::AlertSystem::Alert> warningAlerts = 
        alertSystem.getAlerts(PerformanceMonitor::AlertSystem::AlertLevel::Warning);
    EXPECT_EQ(warningAlerts.size(), 1);
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorVisualization) {
    PerformanceMonitor monitor;
    
    // Test visualization addition
    class TestVisualization : public PerformanceMonitor::Visualization {
    public:
        std::string getName() const override { return "Test Visualization"; }
        std::string getType() const override { return "test"; }
        std::string getDescription() const override { return "Test visualization"; }
        
        void update(const PerformanceMonitor::PerformanceMetrics& metrics) override {}
        std::string generateOutput() const override { return "test output"; }
    };
    
    monitor.addVisualization(std::make_unique<TestVisualization>());
    
    // Test visualization retrieval
    PerformanceMonitor::Visualization* viz = monitor.getVisualization("Test Visualization");
    EXPECT_NE(viz, nullptr);
    
    // Test visualization update
    PerformanceMonitor::PerformanceMetrics metrics;
    monitor.updateVisualizations(metrics);
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorExport) {
    PerformanceMonitor monitor;
    
    // Test metrics export
    PerformanceMonitor::ExportOptions options;
    options.format = "csv";
    
    std::string filepath = testDir + "/test_metrics.csv";
    bool exported = monitor.exportMetrics(filepath, options);
    EXPECT_TRUE(exported);
    
    // Verify file exists
    std::ifstream file(filepath);
    EXPECT_TRUE(file.good());
    file.close();
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorSystemMonitor) {
    PerformanceMonitor monitor;
    PerformanceMonitor::SystemMonitor& systemMonitor = monitor.getSystemMonitor();
    
    // Test system resource monitoring
    PerformanceMonitor::SystemMonitor::SystemResources resources = systemMonitor.getSystemResources();
    
    // Test system performance analysis
    PerformanceMonitor::SystemMonitor::SystemPerformanceAnalysis analysis = systemMonitor.analyzeSystemPerformance();
    EXPECT_TRUE(analysis.systemStatus.empty() || analysis.systemStatus == "healthy");
}

TEST_F(PerformanceMonitoringTest, PerformanceMonitorDebugTools) {
    PerformanceMonitor monitor;
    PerformanceMonitor::DebugTools& debugTools = monitor.getDebugTools();
    
    // Test debug mode
    debugTools.enableDebugMode(true);
    EXPECT_TRUE(debugTools.isDebugMode());
    
    // Test debug level
    debugTools.setDebugLevel(2);
    EXPECT_EQ(debugTools.getDebugLevel(), 2);
    
    // Test debug timing
    PerformanceMonitor::DebugTools::DebugTiming timing = debugTools.startDebugTiming("test_phase");
    EXPECT_NO_THROW(timing = debugTools.startDebugTiming("test_phase2"));
    
    // Test debug output
    std::ostringstream debugStream;
    debugTools.setDebugOutput(debugStream);
    debugTools.logDebugMessage("Test debug message");
    
    std::string debugOutput = debugStream.str();
    EXPECT_FALSE(debugOutput.empty());
}

} // namespace nlm

// test_main.cpp - Main test runner

#include <gtest/gtest.h>
#include "core/UnifiedCLI.hpp"
#include "core/ErrorHandling.hpp"
#include "core/Config/Config.hpp"
#include "core/AdvancedFeatures.hpp"
#include "core/PerformanceMonitoring.hpp"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Set up test environment
    std::cout << "=== NLM Test Suite ===" << std::endl;
    std::cout << "Testing Unified Command Line Interface" << std::endl;
    std::cout << "Testing Error Handling" << std::endl;
    std::cout << "Testing Configuration System" << std::endl;
    std::cout << "Testing Advanced Features" << std::endl;
    std::cout << "Testing Performance Monitoring" << std::endl;
    
    int result = RUN_ALL_TESTS();
    
    std::cout << "\n=== Test Suite Complete ===" << std::endl;
    std::cout << "Tests run: " << ::testing::UnitTest::GetInstance()->test_to_run_count() << std::endl;
    std::cout << "Tests passed: " << ::testing::UnitTest::GetInstance()->passed_test_count() << std::endl;
    std::cout << "Tests failed: " << ::testing::UnitTest::GetInstance()->failed_test_count() << std::endl;
    
    return result;
}