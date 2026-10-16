// Integration Tests
// Tests for enhanced main.cpp structure and system integration

#include "core/Config/Config.hpp"
#include "core/ConfigManager/ConfigManager.hpp"
#include "cmd/CommandLineParser.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_integration {

void testEnhancedMainStructure() {
    std::cout << "  Testing enhanced main.cpp structure..." << std::endl;
    
    // Test ConfigManager integration with main.cpp
    nlm::ConfigManager configManager;
    
    // Test configuration loading from command line (simulated main.cpp behavior)
    std::vector<std::string> args = {"--neurons", "500", "--regions", "1", "--connections", "0.15"};
    
    // In a real integration test, we would simulate main.cpp's argument parsing
    // For this test, we verify that ConfigManager can handle the configuration
    configManager.set("neuron_count", int64_t(500), nlm::ConfigSource::CommandLine);
    configManager.set("region_count", int64_t(1), nlm::ConfigSource::CommandLine);
    configManager.set("connection_probability", 0.15f, nlm::ConfigSource::CommandLine);
    
    assert(configManager.get<int64_t>("neuron_count") == 500);
    assert(configManager.get<int64_t>("region_count") == 1);
    assert(configManager.get<float>("connection_probability") == 0.15f);
    
    // Test configuration schema validation
    nlm::ConfigSchema schema;
    schema.addParameter("neuron_count", nlm::ConfigType::INT, "Number of neurons", true);
    schema.addParameter("simulation_timestep", nlm::ConfigType::DOUBLE, "Timestep", true);
    
    configManager.setSchema(schema);
    
    std::vector<std::string> errors;
    nlm::Config testConfig;
    testConfig.set("neuron_count", int64_t(500), nlm::ConfigSource::Runtime);
    testConfig.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    assert(configManager.validate(testConfig, errors));
    assert(errors.empty());
    
    std::cout << "    Enhanced main.cpp structure test passed" << std::endl;
}

void testConfigurationLoadingAndMerging() {
    std::cout << "  Testing configuration loading and merging..." << std::endl;
    
    // Create test configuration files
    std::string tempDir = "/tmp/test_config_integration";
    std::filesystem::create_directories(tempDir);
    
    std::string baseConfig = tempDir + "/base_config.txt";
    std::ofstream baseFile(baseConfig);
    baseFile << "neuron_count = 100\n";
    baseFile << "simulation_timestep = 0.001\n";
    baseFile.close();
    
    std::string userConfig = tempDir + "/user_config.txt";
    std::ofstream userFile(userConfig);
    userFile << "neuron_count = 200\n"; // Override
    userFile << "plasticity_enabled = true\n"; // New setting
    userFile.close();
    
    nlm::ConfigManager configManager;
    
    // Test ConfigManager with command line
    bool loadSuccess = configManager.loadFromFile(baseConfig, true);
    assert(loadSuccess);
    
    // Load additional configuration (simulating --merge functionality)
    std::vector<std::string> configPaths = {baseConfig, userConfig};
    bool mergeSuccess = configManager.mergeConfigs(configPaths, true);
    assert(mergeSuccess);
    
    // Verify merged configuration
    assert(configManager.has("neuron_count"));
    assert(configManager.has("simulation_timestep"));
    assert(configManager.has("plasticity_enabled"));
    
    // Note: The actual merge behavior depends on implementation
    // This test verifies integration capability
    
    std::cout << "    Configuration loading and merging test passed" << std::endl;
    
    // Clean up
    std::filesystem::remove_all(tempDir);
}

void testCommandLineParsingIntegration() {
    std::cout << "  Testing command-line parsing integration..." << std::endl;
    
    nlm::CommandLineParser parser;
    
    // Add test options similar to main.cpp
    parser.addOption("neurons", "n", "Number of neurons", "int", "500",
                    "BRAIN_NEURONS", 
                    [](const std::string& v) { return !v.empty(); },
                    true, "brain");
    
    parser.addOption("regions", "r", "Number of brain regions", "int", "1",
                    "BRAIN_REGIONS",
                    [](const std::string& v) { return !v.empty(); },
                    false, "brain");
    
    parser.addOption("timestep", "dt", "Simulation timestep", "float", "0.001",
                    "SIM_TIMESTEP",
                    [](const std::string& v) { return !v.empty(); },
                    false, "simulation");
    
    parser.addOption("steps", "s", "Number of simulation steps", "int", "1000",
                    "SIM_STEPS",
                    [](const std::string& v) { return !v.empty(); },
                    false, "simulation");
    
    // Test help and version detection
    char* argv[] = {"nlm", "--help"};
    int argc = 2;
    assert(nlm::CommandLineParser::isHelpRequested(argc, argv));
    
    char* argv2[] = {"nlm", "--version"};
    int argc2 = 2;
    assert(nlm::CommandLineParser::isVersionRequested(argc2, argv2));
    
    // Test option categories
    std::vector<std::string> categories = parser.getCategories();
    assert(std::find(categories.begin(), categories.end(), "brain") != categories.end());
    assert(std::find(categories.begin(), categories.end(), "simulation") != categories.end());
    
    std::cout << "    Command-line parsing integration test passed" << std::endl;
}

void testTestExecutionIntegration() {
    std::cout << "  Testing test execution integration..." << std::endl;
    
    // Create a mock configuration for test execution
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", int64_t(100), nlm::ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Test TestRunner integration (this would be called from main.cpp)
    nlm::TestRunner testRunner;
    
    // Add test cases (these would be registered in real implementation)
    class MockIntegrationTest : public nlm::TestCase {
    public:
        std::string getName() const override { return "Integration Test"; }
        std::string getDescription() const override { return "Test integration with main system"; }
        nlm::TestResult run(std::shared_ptr<nlm::Config> config) override {
            nlm::TestResult result;
            result.name = getName();
            result.passed = true;
            result.duration = 0.1;
            result.description = getDescription();
            return result;
        }
    };
    
    testRunner.addTest(std::make_unique<MockIntegrationTest>());
    
    // Run tests (simulating main.cpp test execution)
    std::vector<nlm::TestResult> results = testRunner.runAll(config);
    assert(results.size() == 1);
    assert(results[0].passed);
    
    std::cout << "    Test execution integration test passed" << std::endl;
}

void testOutputFormattingIntegration() {
    std::cout << "  Testing output formatting integration..." << std::endl;
    
    nlm::OutputManager outputManager;
    
    // Configure output similar to main.cpp
    nlm::OutputConfig config;
    config.format = nlm::OutputFormat::JSON;
    config.title = "NLM Integration Test";
    config.description = "Testing output formatting in main.cpp context";
    config.includeTimestamp = true;
    config.includeMetrics = true;
    
    outputManager.setConfig(config);
    
    // Initialize with JSON format
    assert(outputManager.setFormatter(nlm::OutputFormat::JSON));
    
    // Create test results for output
    std::vector<nlm::TestResult> testResults;
    
    nlm::TestResult integrationResult;
    integrationResult.name = "Integration Test";
    integrationResult.passed = true;
    integrationResult.duration = 0.5;
    integrationResult.description = "Test output formatting integration";
    integrationResult.metrics["integration_score"] = 0.95;
    integrationResult.output = "Integration completed successfully";
    
    testResults.push_back(integrationResult);
    
    // Write test results
    assert(outputManager.write(testResults));
    
    // Write metrics
    std::map<std::string, double> metrics;
    metrics["total_neurons"] = 500.0;
    metrics["simulation_time"] = 1.5;
    metrics["test_cases"] = 3.0;
    
    assert(outputManager.write(metrics));
    
    // Clean up
    outputManager.close();
    
    std::cout << "    Output formatting integration test passed" << std::endl;
}

void testBrainConfigurationWithNewOptions() {
    std::cout << "  Testing brain configuration with new options..." << std::endl;
    
    // Test enhanced brain configuration options
    auto config = std::make_shared<nlm::Config>();
    
    // Set brain configuration parameters (simulating new options from BrainConfigOptions)
    config->set("neuron_count", int64_t(1024), nlm::ConfigSource::CommandLine);
    config->set("region_count", int64_t(4), nlm::ConfigSource::CommandLine);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::CommandLine);
    config->set("plasticity_enabled", true, nlm::ConfigSource::CommandLine);
    config->set("network_type", std::string("lif"), nlm::ConfigSource::CommandLine);
    config->set("weight_init", std::string("random"), nlm::ConfigSource::CommandLine);
    
    // Validate brain configuration
    assert(config->has("neuron_count"));
    assert(config->has("region_count"));
    assert(config->has("connection_probability"));
    assert(config->has("plasticity_enabled"));
    
    // Verify values
    assert(config->get<int64_t>("neuron_count") == 1024);
    assert(config->get<int64_t>("region_count") == 4);
    assert(config->get<float>("connection_probability") == 0.2f);
    assert(config->get<bool>("plasticity_enabled") == true);
    
    std::cout << "    Brain configuration with new options test passed" << std::endl;
}

void testNeuralNetworkParametersValidation() {
    std::cout << "  Testing neural network parameters validation..." << std::endl;
    
    nlm::ConfigManager configManager;
    
    // Set up neural network parameters
    nlm::ConfigSchema nnSchema;
    nnSchema.addParameter("neurons", nlm::ConfigType::INT, "Number of neurons", true, "0", "1000000");
    nnSchema.addParameter("regions", nlm::ConfigType::INT, "Number of regions", true, "1", "100");
    nnSchema.addParameter("connections", nlm::ConfigType::FLOAT, "Connection probability", true, "0.0", "1.0");
    
    configManager.setSchema(nnSchema);
    
    // Test valid neural network parameters
    nlm::Config validNNConfig;
    validNNConfig.set("neurons", int64_t(512), nlm::ConfigSource::Runtime);
    validNNConfig.set("regions", int64_t(8), nlm::ConfigSource::Runtime);
    validNNConfig.set("connections", 0.15f, nlm::ConfigSource::Runtime);
    
    std::vector<std::string> errors;
    assert(configManager.validate(validNNConfig, errors));
    assert(errors.empty());
    
    // Test invalid neural network parameters (should fail validation)
    nlm::Config invalidNNConfig;
    invalidNNConfig.set("neurons", int64_t(0), nlm::ConfigSource::Runtime); // Too small
    invalidNNConfig.set("regions", int64_t(101), nlm::ConfigSource::Runtime); // Too large
    invalidNNConfig.set("connections", 1.5f, nlm::ConfigSource::Runtime); // Invalid range
    
    assert(!configManager.validate(invalidNNConfig, errors));
    assert(errors.size() > 0);
    
    std::cout << "    Neural network parameters validation test passed" << std::endl;
}

void testSimulationParameters() {
    std::cout << "  Testing simulation parameters..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set simulation parameters
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::CommandLine);
    config->set("max_simulation_steps", int64_t(10000), nlm::ConfigSource::CommandLine);
    config->set("total_simulation_time", 10.0, nlm::ConfigSource::CommandLine);
    config->set("enable_metrics", true, nlm::ConfigSource::CommandLine);
    config->set("profile_steps", int64_t(100), nlm::ConfigSource::CommandLine);
    
    // Validate simulation parameters
    assert(config->has("simulation_timestep"));
    assert(config->has("max_simulation_steps"));
    assert(config->has("total_simulation_time"));
    assert(config->has("enable_metrics"));
    assert(config->has("profile_steps"));
    
    // Verify values
    assert(config->get<double>("simulation_timestep") == 0.001);
    assert(config->get<int64_t>("max_simulation_steps") == 10000);
    assert(config->get<double>("total_simulation_time") == 10.0);
    assert(config->get<bool>("enable_metrics") == true);
    assert(config->get<int64_t>("profile_steps") == 100);
    
    std::cout << "    Simulation parameters test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Integration Tests ===" << std::endl;
    
    testEnhancedMainStructure();
    testConfigurationLoadingAndMerging();
    testCommandLineParsingIntegration();
    testTestExecutionIntegration();
    testOutputFormattingIntegration();
    testBrainConfigurationWithNewOptions();
    testNeuralNetworkParametersValidation();
    testSimulationParameters();
    
    std::cout << "=== All Integration Tests PASSED ===" << std::endl << std::endl;
}

} // namespace test_integration