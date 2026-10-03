// Main implementation of NLM CLI Tools
// Provides command-line interfaces for common NLM operations

#include "cli_tools.hpp"

namespace nlm {
namespace cli {

// Configuration validation implementation
bool ConfigValidator::validateConfig(const std::string& filepath) {
    std::cout << "Validating configuration file: " << filepath << std::endl;
    
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(filepath)) {
        std::cerr << "ERROR: Failed to load configuration file: " << filepath << std::endl;
        return false;
    }
    
    std::cout << "Configuration loaded successfully.\n";
    std::cout << config->summary() << std::endl;
    
    // Basic validation
    bool isValid = true;
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            std::cerr << "WARNING: Missing required key: " << key << std::endl;
            isValid = false;
        }
    }
    
    // Check for valid value ranges
    auto neuronCount = config->getOr<size_t>("neuron_count", 0);
    if (neuronCount == 0) {
        std::cerr << "ERROR: Invalid neuron_count (must be > 0)\n";
        isValid = false;
    }
    
    auto regionCount = config->getOr<size_t>("region_count", 0);
    if (regionCount == 0) {
        std::cerr << "ERROR: Invalid region_count (must be > 0)\n";
        isValid = false;
    }
    
    auto connectionProb = config->getOr<float>("connection_probability", -1.0f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        std::cerr << "ERROR: Invalid connection_probability (must be between 0.0 and 1.0)\n";
        isValid = false;
    }
    
    if (isValid) {
        std::cout << "✓ Configuration validation passed." << std::endl;
    } else {
        std::cout << "✗ Configuration validation failed." << std::endl;
    }
    
    return isValid;
}

bool ConfigValidator::validateJsonConfig(const std::string& filepath) {
    std::cout << "Validating JSON configuration file: " << filepath << std::endl;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open file: " << filepath << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();
    
    return validateJsonString(json);
}

bool ConfigValidator::validateJsonString(const std::string& json) {
    std::cout << "Validating JSON string..." << std::endl;
    
    // Simple JSON validation
    // Check for balanced braces and brackets
    int braceCount = 0;
    int bracketCount = 0;
    bool inString = false;
    bool escaped = false;
    
    for (char c : json) {
        if (escaped) {
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else if (c == '\"') {
            inString = !inString;
        } else if (!inString) {
            if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
            } else if (c == '[') {
                bracketCount++;
            } else if (c == ']') {
                bracketCount--;
            }
        }
    }
    
    if (braceCount != 0 || bracketCount != 0) {
        std::cerr << "ERROR: Unbalanced JSON braces or brackets\n";
        return false;
    }
    
    std::cout << "✓ JSON validation passed." << std::endl;
    return true;
}

bool ConfigValidator::validateYamlConfig(const std::string& filepath) {
    std::cout << "Validating YAML configuration file: " << filepath << std::endl;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open file: " << filepath << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string yaml = buffer.str();
    
    return validateYamlString(yaml);
}

bool ConfigValidator::validateYamlString(const std::string& yaml) {
    std::cout << "Validating YAML string..." << std::endl;
    
    // Simple YAML validation
    // Check for basic YAML syntax
    std::istringstream stream(yaml);
    std::string line;
    int indentLevel = 0;
    bool inBlockScalar = false;
    
    while (std::getline(stream, line)) {
        // Skip comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        // Check indentation
        size_t indent = line.find_first_not_of(" \t");
        if (indent < indentLevel) {
            std::cerr << "ERROR: Invalid indentation in YAML\n";
            return false;
        }
        
        // Check for block scalar indicators
        if (line.find("|") != std::string::npos || 
            line.find(">") != std::string::npos) {
            inBlockScalar = true;
        }
        
        indentLevel = indent;
    }
    
    std::cout << "✓ YAML validation passed." << std::endl;
    return true;
}

bool NetworkVisualizer::visualizeNetwork(const std::string& brainFile, const std::string& outputFile) {
    std::cout << "Visualizing neural network..." << std::endl;
    std::cout << "  Input: " << brainFile << std::endl;
    std::cout << "  Output: " << outputFile << std::endl;
    
    // Load brain from checkpoint
    auto brain = std::make_shared<Brain>(std::make_shared<Config>());
    if (!brain->load(brainFile)) {
        std::cerr << "ERROR: Failed to load brain from: " << brainFile << std::endl;
        return false;
    }
    
    // Generate visualization
    if (!generateGraphviz(brain, outputFile)) {
        std::cerr << "ERROR: Failed to generate visualization" << std::endl;
        return false;
    }
    
    std::cout << "✓ Network visualization completed." << std::endl;
    return true;
}

bool NetworkVisualizer::generateGraphviz(std::shared_ptr<Brain> brain, const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << "digraph NLM_Brain {\n";
    file << "  rankdir=TB;\n";
    file << "  node [shape=circle, style=filled];\n";
    
    // Add neurons
    for (size_t i = 0; i < brain->getTotalNeuronCount(); ++i) {
        file << "  neuron_" << i << " [fillcolor=" << getNeuronColor(brain, i) << "];\n";
    }
    
    // Add synapses
    for (const auto& region : brain->getRegions()) {
        for (const auto& synapse : region->getSynapses()) {
            file << "  neuron_" << synapse->getSourceNeuron().index() 
                 << " -> neuron_" << synapse->getDestinationNeuron().index()
                 << " [weight=" << synapse->getWeight() << "];\n";
        }
    }
    
    file << "}\n";
    return true;
}

std::string NetworkVisualizer::getNeuronColor(std::shared_ptr<Brain> brain, size_t neuronIndex) {
    // Get neuron state
    auto* region = brain->getRegion(brain->getRegionIds()[0]);
    if (!region) return "lightblue";
    
    auto* population = region->getPopulation(region->getPopulationIds()[0]);
    if (!population || neuronIndex >= population->getNeurons().size()) return "lightblue";
    
    auto* neuron = population->getNeurons()[neuronIndex];
    
    if (neuron->isFiring()) {
        return "red";
    } else if (neuron->isRefractory()) {
        return "orange";
    } else {
        return "lightblue";
    }
}

bool PerformanceProfiler::profileSimulation(const std::string& configFile, const std::string& outputFile) {
    std::cout << "Profiling simulation performance..." << std::endl;
    std::cout << "  Configuration: " << configFile << std::endl;
    std::cout << "  Output: " << outputFile << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configFile)) {
        std::cerr << "ERROR: Failed to load configuration" << std::endl;
        return false;
    }
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return false;
    }
    
    // Run simulation
    size_t maxSteps = config->getOr<size_t>("neuron_count", 1000);
    for (size_t step = 0; step < maxSteps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Save profiling results
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot create output file: " << outputFile << std::endl;
        return false;
    }
    
    file << "# Performance Profiling Results\n";
    file << "# Configuration: " << configFile << "\n";
    file << "# Duration: " << duration.count() << " ms\n";
    file << "# Steps per second: " << (maxSteps * 1000.0 / duration.count()) << "\n";
    file << "# Total spikes: " << brain->getTotalSpikeCount() << "\n";
    file << "# Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
    
    std::cout << "✓ Performance profiling completed." << std::endl;
    return true;
}

bool ParameterTuner::tuneParameters(const std::string& baseConfig, const std::string& outputDir) {
    std::cout << "Tuning neural network parameters..." << std::endl;
    std::cout << "  Base configuration: " << baseConfig << std::endl;
    std::cout << "  Output directory: " << outputDir << std::endl;
    
    // Create output directory
    if (!createDirectory(outputDir)) {
        std::cerr << "ERROR: Cannot create output directory: " << outputDir << std::endl;
        return false;
    }
    
    auto baseConfigObj = std::make_shared<Config>();
    if (!baseConfigObj->loadFromFile(baseConfig)) {
        std::cerr << "ERROR: Failed to load base configuration" << std::endl;
        return false;
    }
    
    // Simple parameter tuning
    std::vector<std::string> parameters = {
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "synaptogenesis_rate",
        "pruning_rate"
    };
    
    for (const auto& param : parameters) {
        std::cout << "Tuning parameter: " << param << std::endl;
        
        // Generate variations
        float originalValue = baseConfigObj->getOr<float>(param, 0.0f);
        
        // Test different values
        std::vector<float> testValues = {originalValue * 0.5f, originalValue, originalValue * 1.5f};
        
        float bestValue = originalValue;
        float bestPerformance = evaluateConfiguration(baseConfigObj, param, originalValue);
        
        for (float testValue : testValues) {
            if (testValue == originalValue) continue;
            
            baseConfigObj->set(param, testValue);
            float performance = evaluateConfiguration(baseConfigObj, param, testValue);
            
            if (performance > bestPerformance) {
                bestPerformance = performance;
                bestValue = testValue;
            }
            
            // Save test configuration
            std::string testConfigFile = outputDir + "/config_" + param + "=" + std::to_string(testValue) + ".txt";
            baseConfigObj->saveToFile(testConfigFile);
        }
        
        // Set best value
        baseConfigObj->set(param, bestValue);
        std::cout << "  Best value for " << param << ": " << bestValue << "\n";
    }
    
    // Save final configuration
    std::string finalConfigFile = outputDir + "/best_config.txt";
    baseConfigObj->saveToFile(finalConfigFile);
    
    std::cout << "✓ Parameter tuning completed." << std::endl;
    return true;
}

bool ParameterTuner::createDirectory(const std::string& path) {
    // Implementation for creating directory
    #ifdef _WIN32
        return _mkdir(path.c_str()) == 0;
    #else
        return mkdir(path.c_str(), 0777) == 0;
    #endif
}

float ParameterTuner::evaluateConfiguration(std::shared_ptr<Config> config, const std::string& param, float value) {
    // Simplified evaluation function
    // In practice, this would run simulations and measure performance
    return 100.0f; // Placeholder
}

bool SimulationManager::runSimulation(const std::string& configFile, size_t steps) {
    std::cout << "Running NLM simulation..." << std::endl;
    std::cout << "  Configuration: " << configFile << std::endl;
    std::cout << "  Steps: " << steps << std::endl;
    
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configFile)) {
        std::cerr << "ERROR: Failed to load configuration" << std::endl;
        return false;
    }
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0) {
            std::cout << "  Step: " << step << "/" << steps 
                      << " (spikes: " << brain->getTotalSpikeCount() 
                      << ", firing: " << brain->getFiringNeuronCount() << ")\n";
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "✓ Simulation completed in " << duration.count() << " seconds." << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return true;
}

} // namespace cli
} // namespace nlm

// Implementation of CLI tool entry points
int main(int argc, char** argv) {
    return nlm::cli::main(argc, argv);
}