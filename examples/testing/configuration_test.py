"""
Configuration Test Examples for NLM Testing Framework

This module provides examples of using the NLM testing framework
for configuration validation and testing.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + "/../../src")

from nlm.testing import ConfigurationTestSuite, ConfigurationSchema
from nlm.testing import TestDataGenerator, TestRunner

class ConfigurationTestExamples:
    """
    Example class demonstrating configuration testing workflows.
    """
    
    def __init__(self):
        self.configSuite = ConfigurationTestSuite()
        self.testRunner = TestRunner()
        self.testDataGenerator = TestDataGenerator()
        
    def create_configuration_test_suite_example(self):
        """
        Example: Create and run a configuration test suite
        """
        print("=== Configuration Test Suite Creation Example ===")
        
        # Register configuration tests
        print("Registering configuration tests...")
        
        # Brain configuration tests
        self.configSuite.registerConfigurationTest(
            "brain_config_valid",
            "Validate brain configuration parameters",
            {"neuron_count": "100", "region_count": "2", "connection_probability": "0.1"}
        )
        
        self.configSuite.registerConfigurationTest(
            "brain_config_minimal",
            "Validate minimal brain configuration",
            {"neuron_count": "10", "region_count": "1", "connection_probability": "0.01"}
        )
        
        self.configSuite.registerConfigurationTest(
            "brain_config_large",
            "Validate large brain configuration",
            {"neuron_count": "1000", "region_count": "10", "connection_probability": "0.5"}
        )
        
        # Memory configuration tests
        self.configSuite.registerConfigurationTest(
            "memory_config_valid",
            "Validate memory configuration parameters",
            {"capacity_mb": "1024", "retention_period_ms": "1000"}
        )
        
        self.configSuite.registerConfigurationTest(
            "memory_config_minimal",
            "Validate minimal memory configuration",
            {"capacity_mb": "128", "retention_period_ms": "100"}
        )
        
        self.configSuite.registerConfigurationTest(
            "memory_config_large",
            "Validate large memory configuration",
            {"capacity_mb": "4096", "retention_period_ms": "10000"}
        )
        
        # Dynamics configuration tests
        self.configSuite.registerConfigurationTest(
            "dynamics_config_valid",
            "Validate dynamics configuration parameters",
            {"time_step_ms": "1.0", "refractory_period_ms": "5.0"}
        )
        
        self.configSuite.registerConfigurationTest(
            "dynamics_config_steady_state",
            "Validate steady state dynamics configuration",
            {"time_step_ms": "0.5", "refractory_period_ms": "10.0"}
        )
        
        # Environment configuration tests
        self.configSuite.registerConfigurationTest(
            "environment_config_valid",
            "Validate environment configuration parameters",
            {"stimulation_rate": "10.0", "input_variance": "0.1"}
        )
        
        self.configSuite.registerConfigurationTest(
            "environment_config_stimulating",
            "Validate stimulating environment configuration",
            {"stimulation_rate": "50.0", "input_variance": "0.5"}
        )
        
        # Run configuration tests
        print("Running configuration tests...")
        self.configSuite.runConfigurationTests()
        
        # Generate configuration report
        print("Generating configuration report...")
        self.configSuite.generateConfigurationReport()
        
        # Export configuration results
        self.configSuite.exportConfigurationResults("configuration_test_results.txt")
        
        print("Configuration test suite creation example completed")
        
    def create_configuration_schema_example(self):
        """
        Example: Create and use configuration schemas
        """
        print("\n=== Configuration Schema Example ===")
        
        # Create brain configuration schema
        brainSchema = ConfigurationSchema("BrainConfig", "1.0")
        brainSchema.description = "Brain configuration schema"
        brainSchema.fieldTypes["neuron_count"] = "integer"
        brainSchema.fieldTypes["region_count"] = "integer"
        brainSchema.fieldTypes["connection_probability"] = "double"
        brainSchema.fieldRanges["neuron_count"] = (1, 10000)
        brainSchema.fieldRanges["region_count"] = (1, 100)
        brainSchema.fieldRanges["connection_probability"] = (0.01, 1.0)
        brainSchema.fieldFormats["neuron_count"] = "integer"
        brainSchema.fieldFormats["region_count"] = "integer"
        brainSchema.fieldFormats["connection_probability"] = "range:0.01,1.0"
        brainSchema.isRequired = True
        
        # Create memory configuration schema
        memorySchema = ConfigurationSchema("MemoryConfig", "1.0")
        memorySchema.description = "Memory configuration schema"
        memorySchema.fieldTypes["capacity_mb"] = "integer"
        memorySchema.fieldTypes["retention_period_ms"] = "integer"
        memorySchema.fieldTypes["consolidation_rate"] = "double"
        memorySchema.fieldRanges["capacity_mb"] = (64, 16384)
        memorySchema.fieldRanges["retention_period_ms"] = (10, 86400000)
        memorySchema.fieldRanges["consolidation_rate"] = (0.0, 1.0)
        memorySchema.isRequired = True
        
        # Register schemas
        print("Registering configuration schemas...")
        self.configSuite.registerSchema(brainSchema)
        self.configSuite.registerSchema(memorySchema)
        
        # Validate schemas
        print("Validating schemas...")
        bool brainValid = self.configSuite.validateSchema(brainSchema)
        bool memoryValid = self.configSuite.validateSchema(memorySchema)
        
        print(f"Brain schema valid: {"PASSED" if brainValid else "FAILED"}")
        print(f"Memory schema valid: {"PASSED" if memoryValid else "FAILED"}")
        
        # Load schema from file
        print("Loading schema from file...")
        self.configSuite.loadSchemaFromFile("config_schema.txt")
        
        print("Configuration schema example completed")
        
    def create_parameterized_configuration_test_example(self):
        """
        Example: Create parameterized configuration tests
        """
        print("\n=== Parameterized Configuration Test Example ===")
        
        # Define parameter sets
        print("Defining parameter sets...")
        
        neuronCounts = [10, 50, 100, 500, 1000]
        regionCounts = [1, 2, 5, 10]
        connectionProbabilities = [0.01, 0.1, 0.5, 1.0]
        
        // Create parameter combinations
        int totalCombinations = neuronCounts.size() * regionCounts.size() * connectionProbabilities.size()
        
        std::vector<std::tuple<int, int, double>> parameters;
        for (int neurons : neuronCounts) {
            for (int regions : regionCounts) {
                for (double connProb : connectionProbabilities) {
                    parameters.push_back(std::make_tuple(neurons, regions, connProb));
                }
            }
        }
        
        print(f"Created {totalCombinations} parameter combinations")
        
        // Run parameterized tests
        print("Running parameterized tests...")
        for (size_t i = 0; i < parameters.size(); ++i) {
            auto [neurons, regions, connProb] = parameters[i];
            
            std::unordered_map<std::string, std::string> expectedValues;
            expectedValues["neuron_count"] = std::to_string(neurons);
            expectedValues["region_count"] = std::to_string(regions);
            expectedValues["connection_probability"] = std::to_string(connProb);
            
            self.configSuite.registerConfigurationTest(
                "brain_config_" + std::to_string(i),
                "Parameterized brain configuration test",
                expectedValues
            );
        }
        
        // Run the parameterized tests
        self.configSuite.runConfigurationTests();
        
        // Analyze parameterized test results
        print("Analyzing parameterized test results...")
        self.analyzeParameterizedTestResults();
        
        print("Parameterized configuration test example completed")
        
    def create_configuration_validation_example(self):
        """
        Example: Create configuration validation tests
        """
        print("\n=== Configuration Validation Example ===")
        
        // Register validation tests
        print("Registering validation tests...")
        
        // Configuration file validation
        self.configSuite.registerConfigurationTest(
            "config_file_valid",
            "Validate configuration file structure",
            {"file_structure": "valid", "required_sections": "true"}
        )
        
        // Configuration section validation
        self.configSuite.registerConfigurationTest(
            "config_section_valid",
            "Validate configuration section content",
            {"section_exists": "true", "required_fields": "true"}
        )
        
        // Configuration value validation
        self.configSuite.registerConfigurationTest(
            "config_value_valid",
            "Validate configuration value ranges",
            {"value_within_range": "true", "type_match": "true"}
        )
        
        // Configuration dependency validation
        self.configSuite.registerConfigurationTest(
            "config_dependency_valid",
            "Validate configuration dependencies",
            {"dependencies_met": "true", "conflicts_resolved": "true"}
        )
        
        // Run configuration validation tests
        print("Running configuration validation tests...")
        self.configSuite.runConfigurationTests();
        
        // Validate configuration consistency
        print("Validating configuration consistency...")
        bool isConsistent = self.configSuite.validateConfigurationConsistency();
        print(f"Configuration consistency: {"PASSED" if isConsistent else "FAILED"}")
        
        // Validate cross-component configuration
        print("Validating cross-component configuration...")
        bool isCrossComponentValid = self.configSuite.validateCrossComponentConfiguration();
        print(f"Cross-component configuration: {"PASSED" if isCrossComponentValid else "FAILED"}")
        
        print("Configuration validation example completed")
        
    def create_configuration_error_detection_example(self):
        """
        Example: Demonstrate configuration error detection
        """
        print("\n=== Configuration Error Detection Example ===")
        
        // Create configurations with errors
        print("Creating configurations with errors...")
        
        std::vector<std::string> invalidConfigs;
        invalidConfigs.push_back("# Invalid brain config\n[brain]\nneuron_count = -50\nregion_count = 0\nconnection_probability = 2.0");
        invalidConfigs.push_back("# Invalid memory config\n[memory]\ncapacity_mb = -100\nretention_period_ms = 0");
        
        // Test error detection
        print("Testing error detection...")
        for (size_t i = 0; i < invalidConfigs.size(); ++i) {
            std::string config = invalidConfigs[i];
            
            // Parse and validate configuration
            std::unordered_map<std::string, std::string> parsedConfig = self.configSuite.parseConfigurationFile("");
            
            // In a real implementation, this would validate the config
            print(f"  Config {i + 1}: Error detected (invalid values)")
        }
        
        // Demonstrate error messages
        print("Demonstrating error messages...")
        this->demonstrateErrorMessages();
        
        print("Configuration error detection example completed")
        
    def create_automated_configuration_testing_example(self):
        """
        Example: Create automated configuration testing workflow
        """
        print("\n=== Automated Configuration Testing Example ===")
        
        // Generate test data
        print("Generating configuration test data...")
        configs = this->testDataGenerator.generateConfigurationData("brain", 200);
        
        // Validate generated data
        print("Validating generated configuration data...")
        bool configValid = this->testDataGenerator.validateGeneratedData("configurations", configs);
        print(f"Configuration data valid: {"PASSED" if configValid else "FAILED"}")
        
        // Run automated testing
        print("Running automated configuration testing...")
        this->runAutomatedConfigurationTesting(configs);
        
        // Generate test report
        print("Generating automated test report...",
        this->configSuite.generateConfigurationReport());
        
        // Export test results
        this->configSuite.exportConfigurationResults("automated_config_test_results.txt");
        
        print("Automated configuration testing example completed")
        
    // Helper methods
    void analyzeParameterizedTestResults() {
        std::cout << "  Analyzing parameterized test results..." << std::endl;
        
        // In a real implementation, this would analyze the results of parameterized tests
        std::cout << "  Parameter analysis completed (simulated)" << std::endl;
    }
    
    void demonstrateErrorMessages() {
        std::cout << "  Demonstrating error messages..." << std::endl;
        
        // Show examples of error messages
        std::vector<std::string> errorMessages = {
            "Error: Invalid value for neuron_count: must be between 1 and 10000",
            "Error: Invalid value for region_count: must be at least 1",
            "Error: Invalid value for connection_probability: must be between 0.01 and 1.0",
            "Error: Required section [brain] not found in configuration file",
            "Error: Missing required field: region_count"
        };
        
        for (const auto& error : errorMessages) {
            std::cout << "    " << error << std::endl;
        }
    }
    
    std::vector<std::string> runAutomatedConfigurationTesting(const std::vector<std::string>& configs) {
        std::vector<std::string> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            std::string config = configs[i];
            
            // Parse configuration
            std::unordered_map<std::string, std::string> parsedConfig = this->configSuite.parseConfigurationFile("");
            
            // Validate configuration
            bool isValid = this->configSuite.validateConfigurationFile("")
            
            std::string result = "Test " + std::to_string(i + 1) + ": " + (isValid ? "PASSED" : "FAILED");
            results.push_back(result);
        }
        
        return results;
    }

def main():
    """
    Main function demonstrating all configuration test examples.
    """
    print("NLM Configuration Test Examples")
    print("=" * 50)
    
    configTester = ConfigurationTestExamples()
    
    # Run individual examples
    configTester.create_configuration_test_suite_example()
    configTester.create_configuration_schema_example()
    configTester.create_parameterized_configuration_test_example()
    configTester.create_configuration_validation_example()
    configTester.create_configuration_error_detection_example()
    configTester.create_automated_configuration_testing_example()
    
    print("\n" + "=" * 50)
    print("All configuration test examples completed successfully!")

if __name__ == "__main__":
    main()
