#include "NLMConfigGenerator.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace nlm {

NLMConfigGenerator::NLMConfigGenerator() {
}

NLMConfigGenerator::~NLMConfigGenerator() {
}

int NLMConfigGenerator::generateFromTemplate(const std::string& templateName, 
                                            const std::string& outputFile,
                                            const std::map<std::string, std::string>& parameters) {
    std::cout << "Generating configuration from template: " << templateName << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    // Check if template exists
    auto templates = loadTemplates();
    bool templateExists = false;
    for (const auto& t : templates) {
        if (t == templateName) {
            templateExists = true;
            break;
        }
    }
    
    if (!templateExists) {
        std::cerr << "Error: Template not found: " << templateName << std::endl;
        return 1;
    }
    
    // Generate configuration
    std::string configContent = processTemplate(templateName, parameters);
    
    // Write to file
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return 1;
    }
    
    file << configContent;
    file.close();
    
    std::cout << "Configuration generated successfully to: " << outputFile << std::endl;
    return 0;
}

std::vector<std::string> NLMConfigGenerator::getAvailableTemplates() const {
    return loadTemplates();
}

int NLMConfigGenerator::validateConfig(const std::string& configFile) const {
    std::cout << "Validating configuration: " << configFile << std::endl;
    
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open configuration file: " << configFile << std::endl;
        return 1;
    }
    
    // Basic validation - check if file is not empty
    std::string line;
    bool hasContent = false;
    while (std::getline(file, line)) {
        if (!line.empty() && line.find('#') != 0) {
            hasContent = true;
            break;
        }
    }
    
    file.close();
    
    if (!hasContent) {
        std::cerr << "Error: Configuration file is empty or contains only comments" << std::endl;
        return 1;
    }
    
    std::cout << "Configuration validation passed" << std::endl;
    return 0;
}

int NLMConfigGenerator::mergeConfigs(const std::string& file1, const std::string& file2, const std::string& outputFile) {
    std::cout << "Merging configurations: " << file1 << " + " << file2 << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    // Parse both configurations
    auto config1 = parseConfigFile(file1);
    auto config2 = parseConfigFile(file2);
    
    // Merge config2 into config1 (config2 takes precedence)
    std::map<std::string, std::string> mergedConfig = config1;
    for (const auto& pair : config2) {
        mergedConfig[pair.first] = pair.second;
    }
    
    // Write merged configuration
    writeConfig(mergedConfig, outputFile);
    
    std::cout << "Configurations merged successfully to: " << outputFile << std::endl;
    return 0;
}

int NLMConfigGenerator::exportConfig(const std::string& configFile, const std::string& format, const std::string& outputFile) {
    std::cout << "Exporting configuration: " << configFile << " to format: " << format << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    // Parse configuration
    auto config = parseConfigFile(configFile);
    
    // Export based on format
    if (format == "yaml" || format == "yml") {
        // YAML export
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
            return 1;
        }
        
        file << "# Exported configuration" << std::endl;
        for (const auto& pair : config) {
            file << pair.first << ": " << pair.second << std::endl;
        }
        file.close();
    } else if (format == "json") {
        // JSON export
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
            return 1;
        }
        
        file << "{" << std::endl;
        bool first = true;
        for (const auto& pair : config) {
            if (!first) file << "," << std::endl;
            file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
            first = false;
        }
        file << std::endl << "}" << std::endl;
        file.close();
    } else {
        std::cerr << "Error: Unsupported format: " << format << std::endl;
        return 1;
    }
    
    std::cout << "Configuration exported successfully to: " << outputFile << std::endl;
    return 0;
}

int NLMConfigGenerator::importConfig(const std::string& inputFile, const std::string& format, const std::string& outputFile) {
    std::cout << "Importing configuration: " << inputFile << " from format: " << format << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    // Parse based on format
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open input file: " << inputFile << std::endl;
        return 1;
    }
    
    // For now, just copy the file as a simple implementation
    // TODO: Implement proper format parsing
    std::string line;
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        file.close();
        return 1;
    }
    
    while (std::getline(file, line)) {
        outFile << line << std::endl;
    }
    
    file.close();
    outFile.close();
    
    std::cout << "Configuration imported successfully from: " << inputFile << std::endl;
    return 0;
}

int NLMConfigGenerator::showConfig(const std::string& key, const std::string& configFile) {
    std::cout << "Showing configuration values" << std::endl;
    if (!key.empty()) {
        std::cout << "Key: " << key << std::endl;
    }
    
    // Parse configuration
    auto config = parseConfigFile(configFile);
    
    if (key.empty()) {
        // Show all configuration
        for (const auto& pair : config) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
    } else {
        // Show specific key
        auto it = config.find(key);
        if (it != config.end()) {
            std::cout << it->first << ": " << it->second << std::endl;
        } else {
            std::cerr << "Error: Key not found: " << key << std::endl;
            return 1;
        }
    }
    
    return 0;
}

std::vector<std::string> NLMConfigGenerator::loadTemplates() const {
    return {"quick_sim", "large_scale", "experiment"};
}

std::string NLMConfigGenerator::processTemplate(const std::string& templateName, 
                                               const std::map<std::string, std::string>& parameters) const {
    // Template definitions
    std::map<std::string, std::string> templates = {
        {"quick_sim", "neuron_count: 1000\nmax_steps: 10000\nenable_development: true\nenable_curiosity: true"},
        {"large_scale", "neuron_count: 100000\nmax_steps: 1000000\nenable_checkpointing: true\nenable_multithreading: true\nenable_vectorization: true"},
        {"experiment", "experiment_name: test_experiment\nneuron_count: 5000\nmax_steps: 5000\nenable_curiosity: true\nenable_development: true"}
    };
    
    auto it = templates.find(templateName);
    if (it == templates.end()) {
        return "# Unknown template: " + templateName + "\n";
    }
    
    std::string content = it->second;
    
    // Apply parameters
    for (const auto& param : parameters) {
        // Simple parameter replacement
        size_t pos = content.find("{" + param.first + "}");
        while (pos != std::string::npos) {
            content.replace(pos, param.first.length() + 2, param.second);
            pos = content.find("{" + param.first + "}", pos + param.second.length());
        }
    }
    
    return content;
}

std::map<std::string, std::string> NLMConfigGenerator::parseConfigFile(const std::string& configFile) const {
    std::map<std::string, std::string> config;
    std::ifstream file(configFile);
    
    if (!file.is_open()) {
        return config;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line.find('#') == 0) {
            continue;
        }
        
        // Parse key: value format
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            config[key] = value;
        }
    }
    
    file.close();
    return config;
}

void NLMConfigGenerator::writeConfig(const std::map<std::string, std::string>& config, const std::string& outputFile) const {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to create output file: " << outputFile << std::endl;
        return;
    }
    
    for (const auto& pair : config) {
        file << pair.first << ": " << pair.second << std::endl;
    }
    
    file.close();
}

} // namespace nlm
