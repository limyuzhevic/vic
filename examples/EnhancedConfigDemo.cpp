// Enhanced Configuration System Demonstration
// This program demonstrates all the new features of the enhanced NLM configuration system

#include "core/Config/ConfigEnhanced.hpp"
#include <iostream>
#include <fstream>
#include <vector>

void printSection(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void demonstrateBasicConfiguration() {
    printSection("1. BASIC CONFIGURATION");
    
    nlm::Config config;
    
    // Set basic parameters (backward compatible)
    config.set("brain.region_count", 4);
    config.set("brain.neurons_per_region", 100);
    config.set("brain.inhibitory_ratio", 0.2);
    config.set("plasticity.stdp.learning_rate", 0.01);
    
    // Get values
    std::cout << "Brain regions: " << config.getOr<int>("brain.region_count", 0) << std::endl;
    std::cout << "Neurons per region: " << config.getOr<int>("brain.neurons_per_region", 0) << std::endl;
    std::cout << "Inhibitory ratio: " << config.getOr<double>("brain.inhibitory_ratio", 0.0) << std::endl;
    std::cout << "STDP learning rate: " << config.getOr<double>("plasticity.stdp.learning_rate", 0.0) << std::endl;
    
    // Show summary
    std::cout << "\nConfiguration summary:" << std::endl;
    std::cout << config.summary() << std::endl;
}

void demonstrateParameterGroups() {
    printSection("2. PARAMETER GROUPS");
    
    nlm::Config config;
    
    // Load a template to get all parameters
    config.loadFromTemplate("simple_brain");
    
    // Show available groups
    std::cout << "Available groups:" << std::endl;
    auto groups = config.getGroups();
    for (const auto& group : groups) {
        std::cout << "  - " << group << std::endl;
    }
    
    // Show parameters in each group
    std::cout << "\nParameters in 'brain' group:" << std::endl;
    auto brainParams = config.getParametersInGroup("brain");
    for (const auto& param : brainParams) {
        std::cout << "  - " << param << std::endl;
    }
    
    std::cout << "\nParameters in 'plasticity' group:" << std::endl;
    auto plasticityParams = config.getParametersInGroup("plasticity");
    for (const auto& param : plasticityParams) {
        std::cout << "  - " << param << std::endl;
    }
    
    std::cout << "\nParameters in 'neuromodulation' group:" << std::endl;
    auto neuromodulationParams = config.getParametersInGroup("neuromodulation");
    for (const auto& param : neuromodulationParams) {
        std::cout << "  - " << param << std::endl;
    }
    
    std::cout << "\nParameters in 'development' group:" << std::endl;
    auto developmentParams = config.getParametersInGroup("development");
    for (const auto& param : developmentParams) {
        std::cout << "  - " << param << std::endl;
    }
}

void demonstrateValidation() {
    printSection("3. VALIDATION");
    
    nlm::Config config;
    
    // Load template for validation testing
    config.loadFromTemplate("simple_brain");
    
    // Show validation results
    std::cout << "Validating entire configuration..." << std::endl;
    auto validation = config.validate();
    if (validation.isValid) {
        std::cout << "✓ Configuration is valid!" << std::endl;
    } else {
        std::cout << "✗ Configuration has errors:" << std::endl;
        std::cout << validation.getErrors() << std::endl;
    }
    
    // Show warnings if any
    if (!validation.warnings.empty()) {
        std::cout << "Warnings:" << std::endl;
        std::cout << validation.getWarnings() << std::endl;
    }
    
    // Validate individual parameters
    std::cout << "\nValidating individual parameters:" << std::endl;
    std::vector<std::string> testKeys = {
        "brain.region_count",
        "plasticity.stdp.learning_rate",
        "neuromodulation.reward.strength",
        "unknown.parameter"
    };
    
    for (const auto& key : testKeys) {
        if (config.isValid(key)) {
            std::cout << "✓ " << key << " is valid" << std::endl;
        } else {
            std::cout << "✗ " << key << " is invalid" << std::endl;
            std::cout << "  Errors: " << config.getValidationErrors(key) << std::endl;
        }
    }
}

void demonstrateTemplates() {
    printSection("4. CONFIGURATION TEMPLATES");
    
    // List available templates
    std::cout << "Available templates:" << std::endl;
    auto templates = nlm::Config::getAvailableTemplates();
    for (const auto& template : templates) {
        std::cout << "  - " << template << std::endl;
    }
    
    // Demonstrate each template
    std::vector<std::string> templateNames = {"simple_brain", "agent_learning", "research", "production"};
    
    for (const auto& templateName : templateNames) {
        std::cout << "\n" << std::string(40, '-') << std::endl;
        std::cout << "Template: " << templateName << std::endl;
        std::cout << std::string(40, '-') << std::endl;
        
        nlm::Config config;
        config.loadFromTemplate(templateName);
        
        // Show some key parameters for each template
        std::cout << "Key parameters:" << std::endl;
        std::cout << "  brain.region_count: " << config.getOr<int>("brain.region_count", 0) << std::endl;
        std::cout << "  plasticity.stdp.learning_rate: " << config.getOr<double>("plasticity.stdp.learning_rate", 0.0) << std::endl;
        
        if (templateName == "agent_learning") {
            std::cout << "  plasticity.hebbian.rate: " << config.getOr<double>("plasticity.hebbian.rate", 0.0) << std::endl;
            std::cout << "  neuromodulation.curiosity.epsilon: " << config.getOr<double>("neuromodulation.curiosity.epsilon", 0.0) << std::endl;
        } else if (templateName == "research") {
            std::cout << "  neuromodulation.novelty.sensitivity: " << config.getOr<double>("neuromodulation.novelty.sensitivity", 0.0) << std::endl;
            std::cout << "  development.synaptogenesis.initial_density: " << config.getOr<double>("development.synaptogenesis.initial_density", 0.0) << std::endl;
        } else if (templateName == "production") {
            std::cout << "  performance.optimization_level: " << config.getOr<int>("performance.optimization_level", 0) << std::endl;
            std::cout << "  performance.use_vectorization: " << (config.getOr<bool>("performance.use_vectorization", false) ? "true" : "false") << std::endl;
        }
    }
}

void demonstrateFileIO() {
    printSection("5. FILE I/O WITH VALIDATION");
    
    // Create and save configuration
    nlm::Config config;
    config.loadFromTemplate("agent_learning");
    
    // Save to file
    std::cout << "Saving configuration to 'demo_config.txt'..." << std::endl;
    config.saveToFile("demo_config.txt");
    
    // Load configuration
    nlm::Config config2;
    config2.loadFromFile("demo_config.txt");
    
    std::cout << "Loaded configuration summary:" << std::endl;
    std::cout << config2.summary() << std::endl;
    
    // Validate loaded configuration
    auto validation = config2.validate();
    if (validation.isValid) {
        std::cout << "✓ Loaded configuration is valid!" << std::endl;
    } else {
        std::cout << "✗ Loaded configuration has errors:" << std::endl;
        std::cout << validation.getErrors() << std::endl;
    }
}

void demonstrateSchemaAndDocumentation() {
    printSection("6. SCHEMA AND DOCUMENTATION");
    
    nlm::Config config;
    
    // Get schema and parameter descriptions
    std::cout << "Parameter descriptions from schema:" << std::endl;
    
    std::vector<std::string> importantParams = {
        "brain.region_count",
        "plasticity.stdp.learning_rate",
        "neuromodulation.reward.strength",
        "development.maturation.rate"
    };
    
    for (const auto& param : importantParams) {
        std::cout << "\n" << std::string(50, '-') << std::endl;
        std::cout << "Parameter: " << param << std::endl;
        std::cout << "Description:" << std::endl;
        std::cout << config.getParameterDescription(param) << std::endl;
    }
}

void demonstrateEnhancedConfigValues() {
    printSection("7. ENHANCED CONFIGURATION VALUES");
    
    nlm::Config config;
    
    // Demonstrate setting values using std::any
    std::cout << "Setting values using std::any:" << std::endl;
    
    config.set("test_int", std::any(42));
    config.set("test_double", std::any(3.14159));
    config.set("test_string", std::any(std::string("test_value")));
    config.set("test_bool", std::any(true));
    
    // Demonstrate vector values
    std::vector<int> intVector = {1, 2, 3, 4, 5};
    config.set("test_vector", std::any(intVector));
    
    std::vector<double> doubleVector = {1.1, 2.2, 3.3, 4.4, 5.5};
    config.set("test_double_vector", std::any(doubleVector));
    
    // Retrieve and display values
    std::cout << "Retrieved values:" << std::endl;
    std::cout << "test_int: " << config.getOr<int>("test_int", 0) << std::endl;
    std::cout << "test_double: " << config.getOr<double>("test_double", 0.0) << std::endl;
    std::cout << "test_string: " << config.getOr<std::string>("test_string", "") << std::endl;
    std::cout << "test_bool: " << (config.getOr<bool>("test_bool", false) ? "true" : "false") << std::endl;
    
    auto retrievedVector = config.get<std::vector<int>>("test_vector");
    if (retrievedVector.has_value()) {
        std::cout << "test_vector: [";
        for (size_t i = 0; i < retrievedVector.value().size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << retrievedVector.value()[i];
        }
        std::cout << "]" << std::endl;
    }
}

void demonstrateErrorHandling() {
    printSection("8. ERROR HANDLING");
    
    nlm::Config config;
    
    // Attempt to set invalid values
    std::cout << "Testing error handling for invalid values:" << std::endl;
    
    // These should fail if schema has validation
    bool success1 = config.set("brain.region_count", std::any(-1));  // Invalid (negative)
    std::cout << "Setting brain.region_count = -1: " << (success1 ? "✓ (accepted)" : "✗ (rejected)") << std::endl;
    
    bool success2 = config.set("brain.inhibitory_ratio", std::any(1.5));  // Invalid (> 1.0)
    std::cout << "Setting brain.inhibitory_ratio = 1.5: " << (success2 ? "✓ (accepted)" : "✗ (rejected)") << std::endl;
    
    bool success3 = config.set("brain.inhibitory_ratio", std::any(0.2));  // Valid
    std::cout << "Setting brain.inhibitory_ratio = 0.2: " << (success3 ? "✓ (accepted)" : "✗ (rejected)") << std::endl;
    
    // Check validation status
    std::cout << "\nValidation status for invalid value:" << std::endl;
    if (!config.isValid("brain.region_count")) {
        std::cout << "✗ brain.region_count is invalid (as expected)" << std::endl;
        std::cout << "  Errors: " << config.getValidationErrors("brain.region_count") << std::endl;
    }
    
    std::cout << "\nValidation status for valid value:" << std::endl;
    if (config.isValid("brain.inhibitory_ratio")) {
        std::cout << "✓ brain.inhibitory_ratio is valid (as expected)" << std::endl;
    }
}

void demonstratePythonLikeAPI() {
    printSection("9. PYTHON-LIKE API (convenience methods)");
    
    nlm::Config config;
    
    // Demonstrate Python-like convenience methods
    std::cout << "Using Python-like convenience methods:" << std::endl;
    
    // These are the same as the original methods but with more Pythonic naming
    config.set("int_param", 42);
    config.set("double_param", 3.14);
    config.set("string_param", std::string("hello"));
    config.set("bool_param", true);
    
    // Use convenience getOr methods
    int intVal = config.getOr<int>("int_param", 0);
    double doubleVal = config.getOr<double>("double_param", 0.0);
    std::string stringVal = config.getOr<std::string>("string_param", "");
    bool boolVal = config.getOr<bool>("bool_param", false);
    
    std::cout << "int_param: " << intVal << std::endl;
    std::cout << "double_param: " << doubleVal << std::endl;
    std::cout << "string_param: " << stringVal << std::endl;
    std::cout << "bool_param: " << (boolVal ? "true" : "false") << std::endl;
}

int main() {
    std::cout << "Enhanced Configuration System Demonstration" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    try {
        demonstrateBasicConfiguration();
        demonstrateParameterGroups();
        demonstrateValidation();
        demonstrateTemplates();
        demonstrateFileIO();
        demonstrateSchemaAndDocumentation();
        demonstrateEnhancedConfigValues();
        demonstrateErrorHandling();
        demonstratePythonLikeAPI();
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "Demonstration completed successfully!" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
