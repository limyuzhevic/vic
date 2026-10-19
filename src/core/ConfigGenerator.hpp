// nlm_config_generator.h - Configuration file generation utilities
//
// This header provides tools for generating NLM configuration files,
// validating configurations, and providing command-line configuration interfaces.

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <memory>

namespace nlm {

class ConfigGenerator {
public:
    struct ConfigValue {
        std::string key;
        std::string value;
        std::string type;
        std::string description;
        std::string defaultValue;
        bool required = false;
        std::vector<std::string> validValues;
        std::string validationPattern;
    };
    
    struct GeneratedConfig {
        std::string filename;
        std::string format;
        std::map<std::string, std::string> values;
        std::vector<std::string> comments;
        bool valid = false;
        std::string validationError;
    };
    
    ConfigGenerator() = default;
    
    // Generate configuration from defaults
    GeneratedConfig generateFromDefaults(const std::string& filename = "configs/default.cfg") {
        GeneratedConfig config;
        config.filename = filename;
        config.format = "nlm_cfg";
        
        // Get default configuration values
        config.values = getDefaultValues();
        config.comments = getDefaultComments();
        config.valid = true;
        
        // Save to file
        saveConfig(config);
        
        return config;
    }
    
    // Generate configuration from template
    GeneratedConfig generateFromTemplate(const std::string& templateFile, 
                                          const std::map<std::string, std::string>& customValues) {
        GeneratedConfig config;
        config.filename = templateFile;
        config.format = "template";
        
        // Load template
        if (!loadTemplate(templateFile, config.values, config.comments)) {
            config.valid = false;
            config.validationError = "Failed to load template file: " + templateFile;
            return config;
        }
        
        // Apply custom values
        for (const auto& pair : customValues) {
            if (config.values.find(pair.first) != config.values.end()) {
                config.values[pair.first] = pair.second;
            } else {
                std::cerr << "Warning: Unknown configuration key in template: " << pair.first << std::endl;
            }
        }
        
        // Validate configuration
        config.valid = validateConfig(config.values);
        if (!config.valid) {
            config.validationError = "Configuration validation failed";
        }
        
        return config;
    }
    
    // Generate configuration from command-line arguments
    GeneratedConfig generateFromCLI(int argc, char* argv[]) {
        GeneratedConfig config;
        config.filename = "configs/cli_generated.cfg";
        config.format = "cli";
        
        // Parse command-line arguments
        config.values = parseCLIArguments(argc, argv);
        
        // Add default values for missing keys
        auto defaultValues = getDefaultValues();
        for (const auto& pair : defaultValues) {
            if (config.values.find(pair.first) == config.values.end()) {
                config.values[pair.first] = pair.second;
            }
        }
        
        // Validate configuration
        config.valid = validateConfig(config.values);
        if (!config.valid) {
            config.validationError = "CLI configuration validation failed";
        }
        
        return config;
    }
    
    // Save configuration to file
    bool saveConfig(const GeneratedConfig& config) {
        std::ofstream file(config.filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing: " << config.filename << std::endl;
            return false;
        }
        
        // Write header
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated on: " << getCurrentTimestamp() << std::endl;
        file << "# Format: key = value" << std::endl;
        file << std::endl;
        
        // Write comments
        for (const auto& comment : config.comments) {
            file << "# " << comment << std::endl;
        }
        file << std::endl;
        
        // Write key-value pairs
        for (const auto& pair : config.values) {
            file << pair.first << " = " << pair.second << std::endl;
        }
        
        file.close();
        
        if (verboseLogging) {
            std::cout << "[ConfigGenerator] Configuration saved to: " << config.filename << std::endl;
        }
        
        return true;
    }
    
    // Load configuration from file
    GeneratedConfig loadConfig(const std::string& filename) {
        GeneratedConfig config;
        config.filename = filename;
        config.format = "file";
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open configuration file: " << filename << std::endl;
            config.valid = false;
            config.validationError = "File not found: " + filename;
            return config;
        }
        
        // Parse configuration file
        config.values = parseConfigFile(file);
        config.comments = extractCommentsFromFile(file);
        
        // Validate configuration
        config.valid = validateConfig(config.values);
        if (!config.valid) {
            config.validationError = "Configuration validation failed";
        }
        
        return config;
    }
    
    // Validate configuration
    bool validateConfig(const std::map<std::string, std::string>& values) {
        // Check required keys
        for (const auto& defaultPair : getDefaultValues()) {
            if (defaultPair.second.empty()) continue;  // Skip empty defaults
            if (defaultPair.second == "REQUIRED" && values.find(defaultPair.first) == values.end()) {
                std::cerr << "Error: Required configuration key missing: " << defaultPair.first << std::endl;
                return false;
            }
        }
        
        // Validate specific value ranges
        if (values.find("neuron_count") != values.end()) {
            int neuronCount = std::stoi(values.at("neuron_count"));
            if (neuronCount <= 0 || neuronCount > 100000) {
                std::cerr << "Error: Invalid neuron_count: " << neuronCount << std::endl;
                return false;
            }
        }
        
        if (values.find("max_steps") != values.end()) {
            uint64_t steps = std::stoull(values.at("max_steps"));
            if (steps == 0 || steps > 1000000000) {
                std::cerr << "Error: Invalid max_steps: " << steps << std::endl;
                return false;
            }
        }
        
        if (values.find("simulation_timestep") != values.end()) {
            double timestep = std::stod(values.at("simulation_timestep"));
            if (timestep <= 0.0 || timestep > 1.0) {
                std::cerr << "Error: Invalid simulation_timestep: " << timestep << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    // Convert configuration to C++ code
    std::string convertToCpp(const GeneratedConfig& config) {
        std::stringstream cppCode;
        
        cppCode << "// Auto-generated NLM configuration" << std::endl;
        cppCode << "// File: " << config.filename << std::endl;
        cppCode << "// Generated by NLM Config Generator" << std::endl;
        cppCode << std::endl;
        
        cppCode << "#include \"core/Config/Config.hpp\"" << std::endl;
        cppCode << std::endl;
        
        cppCode << "void setupDefaultConfig(std::shared_ptr<Config> config) {" << std::endl;
        
        for (const auto& pair : config.values) {
            // Parse value to determine type
            cppCode << "    config->set(\"" << pair.first << "\", ";
            
            try {
                int intValue = std::stoi(pair.second);
                cppCode << intValue << ", ConfigSource::Generated);" << std::endl;
            } catch (...) {
                try {
                    double floatValue = std::stod(pair.second);
                    cppCode << floatValue << "f, ConfigSource::Generated);" << std::endl;
                } catch (...) {
                    // String value
                    cppCode << "\"" << pair.second << "\", ConfigSource::Generated);" << std::endl;
                }
            }
        }
        
        cppCode << "}" << std::endl;
        
        return cppCode.str();
    }
    
    // Generate JSON configuration
    std::string generateJSON(const GeneratedConfig& config) {
        std::stringstream json;
        
        json << "{\n";
        json << "  \"nlm_version\": \"0.1.0\",\n";
        json << "  \"generated_on\": \"" << getCurrentTimestamp() << "\",\n";
        
        bool first = true;
        for (const auto& pair : config.values) {
            if (!first) json << ",\n";
            first = false;
            
            json << "  \"" << pair.first << "\": \"" << escapeJSONString(pair.second) << "\"";
        }
        
        json << "\n}\n";
        
        return json.str();
    }
    
    void setVerboseLogging(bool enable) {
        verboseLogging = enable;
    }
    
private:
    std::map<std::string, std::string> getDefaultValues() {
        return {
            {"random_seed", "42"},
            {"neuron_count", "1000"},
            {"region_count", "1"},
            {"connection_probability", "0.1"},
            {"simulation_timestep", "0.001"},
            {"max_steps", "10000"},
            {"stdp_ltp_weight", "0.02"},
            {"stdp_ltd_weight", "0.015"},
            {"stdp_tau", "20.0"},
            {"synaptogenesis_rate", "0.0001"},
            {"pruning_rate", "0.00001"},
            {"working_memory_capacity", "100"},
            {"episodic_memory_capacity", "1000"},
            {"enable_structural_plasticity", "true"},
            {"enable_development", "true"},
            {"enable_neuromodulation", "true"},
            {"enable_prediction", "true"},
            {"enable_curiosity", "true"},
            {"enable_novelty", "true"},
            {"enable_planning", "true"},
            {"enable_attention", "true"},
            {"enable_concept_formation", "true"},
            {"enable_social_learning", "false"},
            {"social_influence_strength", "0.5"},
            {"checkpoint_path", "./checkpoint.bin"},
            {"replay_enabled", "true"},
            {"replay_ratio", "0.1"},
            {"reward_function", "reward_prediction_error"},
            {"discount_factor", "0.95"},
            {"exploration_rate", "0.1"},
            {"temperature", "1.0"},
            {"entropy_weight", "0.01"},
            {"learning_rate", "0.001"},
            {"plasticity_threshold", "0.1"},
            {"development_rate", "0.001"},
            {"maturation_rate", "0.01"},
            {"pruning_threshold", "0.05"},
            {"synaptogenesis_threshold", "0.001"},
            {"dopamine_sensitivity", "1.0"},
            {"curiosity_sensitivity", "0.5"},
            {"novelty_sensitivity", "0.3"},
            {"reward_sensitivity", "1.0"},
            {"confidence_threshold", "0.7"},
            {"attention_span", "1000"},
            {"working_memory_decay", "0.01"},
            {"episodic_memory_decay", "0.005"},
            {"semantic_memory_growth", "0.001"},
            {"procedural_memory_strength", "0.5"},
            {"associative_memory_strength", "0.3"},
            {"prediction_horizon", "10"},
            {"prediction_confidence", "0.8"},
            {"planning_horizon", "20"},
            {"planning_confidence", "0.6"},
            {"concept_formation_threshold", "0.3"},
            {"concept_cohesion", "0.7"},
            {"social_learning_rate", "0.01"},
            {"social_influence_decay", "0.1"},
            {"developmental_stage_start", "0.0"},
            {"developmental_stage_end", "1.0"},
            {"structural_plasticity_rate", "0.0001"},
            {"structural_plasticity_decay", "0.00001"},
            {"connectivity_constraint", "0.8"},
            {"network_efficiency_target", "0.9"},
            {"learning_efficiency_target", "0.8"},
            {"memory_retention_target", "0.7"},
            {"prediction_accuracy_target", "0.85"},
            {"planning_success_rate_target", "0.75"},
            {"social_cohesion_target", "0.8"},
            {"developmental_maturity_target", "0.9"}
        };
    }
    
    std::vector<std::string> getDefaultComments() {
        return {
            "NLM Neural Learning Machine Configuration",
            "Generated by NLM Config Generator",
            "",
            "// Core Simulation Parameters",
            "// These settings control the basic simulation dynamics",
            "",
            "// Neural Architecture",
            "// Number and organization of neurons in the network",
            "",
            "// Plasticity Rules",
            "// Mechanisms for synaptic modification and learning",
            "",
            "// Memory Systems",
            "// Configuration for working, episodic, and semantic memory",
            "",
            "// Neuromodulation",
            "// Integration of dopamine, curiosity, and other neuromodulators",
            "",
            "// Cognitive Systems",
            "// Prediction, planning, attention, and concept formation",
            "",
            "// Developmental Processes",
            "// Age-dependent changes in neural properties and learning",
            "",
            "// Performance and Efficiency",
            "// Targets and constraints for optimization",
            ""
        };
    }
    
    bool loadTemplate(const std::string& filename, 
                      std::map<std::string, std::string>& values,
                      std::vector<std::string>& comments) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines
            if (line.empty()) continue;
            
            // Check for comment lines
            if (line.find("#") == 0 || line.find("//") == 0) {
                comments.push_back(line.substr(1)); // Remove # or //
                continue;
            }
            
            // Parse key-value pairs
            size_t equalsPos = line.find("=");
            if (equalsPos != std::string::npos) {
                std::string key = trim(line.substr(0, equalsPos));
                std::string value = trim(line.substr(equalsPos + 1));
                values[key] = value;
            }
        }
        
        return true;
    }
    
    std::map<std::string, std::string> parseCLIArguments(int argc, char* argv[]) {
        std::map<std::string, std::string> values;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg.find("--") == 0) {
                std::string key = arg.substr(2);
                std::string value = "true"; // Default for boolean flags
                
                // Check if this is a key=value pair
                size_t equalsPos = key.find("=");
                if (equalsPos != std::string::npos) {
                    value = key.substr(equalsPos + 1);
                    key = key.substr(0, equalsPos);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    // Next argument is the value
                    value = argv[++i];
                }
                
                values[key] = value;
            }
        }
        
        return values;
    }
    
    std::map<std::string, std::string> parseConfigFile(std::ifstream& file) {
        std::map<std::string, std::string> values;
        std::string line;
        
        // Store comments temporarily
        std::vector<std::string> comments;
        
        while (std::getline(file, line)) {
            // Reset file position for re-reading
            file.clear();
            file.seekg(0, std::ios::beg);
            
            // Skip empty lines
            if (line.empty()) continue;
            
            // Check for comment lines
            if (line.find("#") == 0) {
                comments.push_back(line.substr(1));
                continue;
            }
            
            // Parse key-value pairs
            size_t equalsPos = line.find("=");
            if (equalsPos != std::string::npos) {
                std::string key = trim(line.substr(0, equalsPos));
                std::string value = trim(line.substr(equalsPos + 1));
                values[key] = value;
            }
        }
        
        return values;
    }
    
    std::vector<std::string> extractCommentsFromFile(std::ifstream& file) {
        std::vector<std::string> comments;
        std::string line;
        
        // Reset file position
        file.clear();
        file.seekg(0, std::ios::beg);
        
        while (std::getline(file, line)) {
            if (line.find("#") == 0) {
                comments.push_back(line.substr(1));
            }
        }
        
        return comments;
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string escapeJSONString(const std::string& str) {
        std::string escaped = str;
        // Replace common escape sequences
        size_t pos = 0;
        while ((pos = escaped.find("\\", pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\\\");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find("\n", pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\n");
            pos += 2;
        }
        pos = 0;
        while ((pos = escaped.find("\t", pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\\t");
            pos += 2;
        }
        return escaped;
    }
    
    bool verboseLogging = false;
};