// Visualization tools and configuration management for NLM
//
// This module provides advanced visualization and configuration management
// capabilities for NLM users.

#ifndef NLM_VISUALIZATION_CONFIG_HPP
#define NLM_VISUALIZATION_CONFIG_HPP

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <iomanip>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "DemoFramework.hpp"

namespace nlm {

/**
 * @brief Visualization tools for NLM
 */
class VisualizationTools {
public:
    struct VisualizationConfig {
        bool enableHeatmap = false;
        bool enableNetworkGraph = false;
        bool enableStatisticalReport = false;
        bool enableActivityPlot = false;
        bool enableMemoryMap = false;
        std::string outputDirectory = "visualizations";
        std::string imageFormat = "png";
        int imageWidth = 800;
        int imageHeight = 600;
        bool saveToFile = true;
        bool showConsoleOutput = true;
    };
    
    struct BrainActivityData {
        std::vector<float> firingRates;
        std::vector<float> memoryUsage;
        std::vector<float> energyLevels;
        std::vector<std::vector<float>> neuralConnections;
        std::vector<std::string> neuronTypes;
        std::chrono::system_clock::time_point timestamp;
    };
    
    struct NetworkGraphData {
        std::vector<std::pair<int, int>> connections;
        std::vector<std::string> neuronLabels;
        std::vector<float> connectionWeights;
        std::vector<std::string> connectionTypes;
        std::map<std::string, std::vector<float>> neuronAttributes;
    };
    
    struct StatisticalData {
        std::string title;
        std::vector<std::string> dataLabels;
        std::vector<std::vector<float>> dataSeries;
        std::map<std::string, std::string> metadata;
        std::chrono::system_clock::time_point timestamp;
    };
    
    VisualizationTools() : initialized_(false) {}
    
    void initialize(const VisualizationConfig& config) {
        config_ = config;
        initialized_ = true;
        
        // Create output directory
        if (config_.saveToFile && !config_.outputDirectory.empty()) {
            std::filesystem::create_directories(config_.outputDirectory);
        }
        
        std::cout << "VisualizationTools initialized with config:\n";
        std::cout << "  Heatmap: " << (config_.enableHeatmap ? "enabled" : "disabled") << "\n";
        std::cout << "  Network Graph: " << (config_.enableNetworkGraph ? "enabled" : "disabled") << "\n";
        std::cout << "  Statistical Report: " << (config_.enableStatisticalReport ? "enabled" : "disabled") << "\n";
        std::cout << "  Activity Plot: " << (config_.enableActivityPlot ? "enabled" : "disabled") << "\n";
        std::cout << "  Memory Map: " << (config_.enableMemoryMap ? "enabled" : "disabled") << "\n";
        std::cout << "  Output Directory: " << config_.outputDirectory << "\n";
        std::cout << "  Image Format: " << config_.imageFormat << "\n";
    }
    
    void updateBrainActivity(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                            std::shared_ptr<SimpleWorld> world) {
        if (!initialized_ || !brain || !agent || !world) {
            return;
        }
        
        BrainActivityData data;
        
        // Collect firing rates
        data.firingRates.clear();
        data.firingRates.reserve(brain->getTotalNeuronCount());
        
        // Collect activity data from brain regions
        for (const auto& region : brain->getRegions()) {
            for (const auto& neuron : region->getNeurons()) {
                data.firingRates.push_back(neuron->getActivityLevel());
                data.neuronTypes.push_back("standard");
            }
        }
        
        // Collect memory usage
        data.memoryUsage.clear();
        if (brain->getWorkingMemory()) {
            data.memoryUsage.push_back(static_cast<float>(brain->getWorkingMemory()->getMemoryNeuronCount()));
        }
        
        // Collect energy levels
        data.energyLevels.clear();
        data.energyLevels.push_back(world->getAgentBody().energy);
        
        // Collect neural connections
        data.neuralConnections.clear();
        for (const auto& region : brain->getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                data.neuralConnections.push_back(std::make_pair(
                    synapse->getPreNeuron()->getId(),
                    synapse->getPostNeuron()->getId()));
            }
        }
        
        data.timestamp = std::chrono::system_clock::now();
        
        // Generate visualizations
        generateActivityHeatmap(data);
        generateActivityPlot(data);
        
        // Generate statistical data
        StatisticalData stats = generateStatisticalData(brain, agent, world);
        generateStatisticalReport(stats);
        
        // Generate network graph
        NetworkGraphData networkData = generateNetworkGraphData(brain);
        generateNetworkGraph(networkData);
    }
    
    void generateActivityHeatmap(const BrainActivityData& data) {
        if (!config_.enableHeatmap || !initialized_) {
            return;
        }
        
        std::cout << "Generating activity heatmap...\n";
        
        // Create heatmap data (simplified)
        std::vector<std::vector<float>> heatmap(100, std::vector<float>(100, 0.0f));
        
        // Map neurons to heatmap grid
        for (size_t i = 0; i < data.firingRates.size() && i < data.neuronTypes.size(); ++i) {
            if (data.neuronTypes[i] == "standard") {
                // Simplified mapping - in real implementation, would use actual neuron positions
                int x = i % 100;
                int y = i / 100;
                if (x < 100 && y < 100) {
                    heatmap[y][x] += data.firingRates[i];
                }
            }
        }
        
        // Save heatmap
        if (config_.saveToFile) {
            saveHeatmap(heatmap, "activity_heatmap.txt");
        }
        
        if (config_.showConsoleOutput) {
            displayHeatmap(heatmap);
        }
    }
    
    void generateActivityPlot(const BrainActivityData& data) {
        if (!config_.enableActivityPlot || !initialized_) {
            return;
        }
        
        std::cout << "Generating activity plot...\n";
        
        // Create activity plot data
        std::ofstream file("activity_plot.txt");
        if (!file.is_open()) {
            std::cout << "Warning: Could not create activity plot file\n";
            return;
        }
        
        file << "Activity Plot Data\n";
        file << "===================\n\n";
        
        file << "Time series of neural activity:\n";
        for (size_t i = 0; i < data.firingRates.size(); ++i) {
            file << "Neuron " << i << ": ";
            for (size_t j = 0; j < data.firingRates.size(); ++j) {
                // This is simplified - in real implementation, would have time series data
                if (j % 10 == 0) {  // Sample every 10th point
                    file << data.firingRates[j] << " ";
                }
            }
            file << "\n";
        }
        
        file.close();
        
        if (config_.showConsoleOutput) {
            std::cout << "Activity plot data saved to activity_plot.txt\n";
        }
    }
    
    void generateNetworkGraph(const NetworkGraphData& data) {
        if (!config_.enableNetworkGraph || !initialized_) {
            return;
        }
        
        std::cout << "Generating network graph...\n";
        
        // Generate network graph data
        std::ofstream file("network_graph.txt");
        if (!file.is_open()) {
            std::cout << "Warning: Could not create network graph file\n";
            return;
        }
        
        file << "Network Graph Data\n";
        file << "===================\n\n";
        
        file << "Connections (pre-synaptic -> post-synaptic):\n";
        for (const auto& conn : data.connections) {
            file << conn.first << " -> " << conn.second << "\n";
        }
        
        file << "\nConnection weights:\n";
        for (size_t i = 0; i < data.connections.size() && i < data.connectionWeights.size(); ++i) {
            file << data.connections[i].first << " -> " << data.connections[i].second 
                 << " : " << data.connectionWeights[i] << "\n";
        }
        
        file.close();
        
        if (config_.showConsoleOutput) {
            std::cout << "Network graph data saved to network_graph.txt\n";
        }
    }
    
    void generateStatisticalReport(const StatisticalData& data) {
        if (!config_.enableStatisticalReport || !initialized_) {
            return;
        }
        
        std::cout << "Generating statistical report...\n";
        
        // Generate statistical report
        std::ofstream file("statistical_report.txt");
        if (!file.is_open()) {
            std::cout << "Warning: Could not create statistical report file\n";
            return;
        }
        
        file << data.title << "\n";
        file << std::string(data.title.length(), '=') << "\n\n";
        
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        // Display metadata
        file << "Metadata:\n";
        for (const auto& pair : data.metadata) {
            file << "  " << pair.first << ": " << pair.second << "\n";
        }
        
        // Display data series
        if (!data.dataLabels.empty() && !data.dataSeries.empty()) {
            file << "\nData:\n";
            for (size_t i = 0; i < data.dataLabels.size() && i < data.dataSeries.size(); ++i) {
                file << "  " << data.dataLabels[i] << ": ";
                for (size_t j = 0; j < data.dataSeries[i].size(); ++j) {
                    file << data.dataSeries[i][j];
                    if (j < data.dataSeries[i].size() - 1) {
                        file << ", ";
                    }
                }
                file << "\n";
            }
        }
        
        file.close();
        
        if (config_.showConsoleOutput) {
            std::cout << "Statistical report saved to statistical_report.txt\n";
        }
    }
    
    void generateMemoryMap(const BrainActivityData& data) {
        if (!config_.enableMemoryMap || !initialized_) {
            return;
        }
        
        std::cout << "Generating memory map...\n";
        
        // Generate memory map data
        std::ofstream file("memory_map.txt");
        if (!file.is_open()) {
            std::cout << "Warning: Could not create memory map file\n";
            return;
        }
        
        file << "Memory Map Data\n";
        file << "=================\n\n";
        
        // Memory information
        file << "Memory Usage:\n";
        for (size_t i = 0; i < data.memoryUsage.size(); ++i) {
            file << "  Component " << i << ": " << data.memoryUsage[i] << " MB\n";
        }
        
        file << "\nEnergy Levels:\n";
        for (size_t i = 0; i < data.energyLevels.size(); ++i) {
            file << "  Time " << i << ": " << data.energyLevels[i] << "\n";
        }
        
        file.close();
        
        if (config_.showConsoleOutput) {
            std::cout << "Memory map saved to memory_map.txt\n";
        }
    }
    
    StatisticalData generateStatisticalData(std::shared_ptr<Brain> brain,
                                            std::shared_ptr<AgentBrain> agent,
                                            std::shared_ptr<SimpleWorld> world) {
        StatisticalData data;
        data.title = "NLM Brain Statistical Data";
        data.timestamp = std::chrono::system_clock::now();
        
        // Add metadata
        data.metadata["brain_neurons"] = std::to_string(brain->getTotalNeuronCount());
        data.metadata["brain_synapses"] = std::to_string(brain->getTotalSynapseCount());
        data.metadata["brain_firing_rate"] = std::to_string(brain->getAverageFiringRate());
        data.metadata["agent_sensory_size"] = std::to_string(agent->getSensoryInputSize());
        data.metadata["agent_motor_size"] = std::to_string(agent->getMotorOutputSize());
        data.metadata["world_width"] = std::to_string(world->getEnvironmentWidth());
        data.metadata["world_height"] = std::to_string(world->getEnvironmentHeight());
        
        // Create data series
        data.dataLabels = {"Firing Rate", "Memory Usage", "Energy"};
        data.dataSeries.resize(data.dataLabels.size());
        
        if (!data.dataSeries.empty()) {
            // Firing rate data
            data.dataSeries[0] = {brain->getAverageFiringRate()};
            
            // Memory usage data
            if (brain->getWorkingMemory()) {
                data.dataSeries[1] = {static_cast<float>(brain->getWorkingMemory()->getMemoryNeuronCount())};
            } else {
                data.dataSeries[1] = {0.0f};
            }
            
            // Energy data
            data.dataSeries[2] = {world->getAgentBody().energy};
        }
        
        return data;
    }
    
    NetworkGraphData generateNetworkGraphData(std::shared_ptr<Brain> brain) {
        NetworkGraphData data;
        
        // Collect connection data
        data.connections.clear();
        data.connectionWeights.clear();
        data.connectionTypes.clear();
        data.neuronLabels.clear();
        data.neuronAttributes.clear();
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                // Add connection
                data.connections.push_back(std::make_pair(
                    synapse->getPreNeuron()->getId(),
                    synapse->getPostNeuron()->getId()));
                
                // Add weight
                data.connectionWeights.push_back(synapse->getWeight());
                
                // Add type
                data.connectionTypes.push_back("plastic");
                
                // Add neuron labels
                data.neuronLabels.push_back(std::to_string(synapse->getPreNeuron()->getId()));
                data.neuronLabels.push_back(std::to_string(synapse->getPostNeuron()->getId()));
                
                // Add attributes
                std::string preId = std::to_string(synapse->getPreNeuron()->getId());
                if (data.neuronAttributes.find(preId) == data.neuronAttributes.end()) {
                    data.neuronAttributes[preId] = std::vector<float>();
                }
                data.neuronAttributes[preId].push_back(synapse->getWeight());
            }
        }
        
        return data;
    }
    
private:
    void saveHeatmap(const std::vector<std::vector<float>>& heatmap, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Warning: Could not save heatmap file: " << filename << std::endl;
            return;
        }
        
        file << "Heatmap Data\n";
        file << "============\n\n";
        
        for (const auto& row : heatmap) {
            for (float value : row) {
                file << value << " ";
            }
            file << "\n";
        }
        
        file.close();
    }
    
    void displayHeatmap(const std::vector<std::vector<float>>& heatmap) {
        std::cout << "Activity Heatmap:\n";
        for (const auto& row : heatmap) {
            for (float value : row) {
                if (value > 0.8f) {
                    std::cout << "*";  // High activity
                } else if (value > 0.5f) {
                    std::cout << "#";  // Medium activity
                } else if (value > 0.2f) {
                    std::cout << ".";  // Low activity
                } else {
                    std::cout << " ";  // No activity
                }
            }
            std::cout << "\n";
        }
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    VisualizationConfig config_;
    bool initialized_;
};

/**
 * @brief Configuration management tools
 */
class ConfigurationManager {
public:
    struct ConfigurationInfo {
        std::string name;
        std::string path;
        std::string description;
        std::chrono::system_clock::time_point lastModified;
        size_t size;
        std::map<std::string, std::string> settings;
        bool isValid;
        std::vector<std::string> validationErrors;
    };
    
    ConfigurationManager() : autoSaveEnabled_(false), autoSaveInterval_(60.0) {}
    
    void setAutoSave(bool enabled, double interval = 60.0) {
        autoSaveEnabled_ = enabled;
        autoSaveInterval_ = interval;
        std::cout << "Auto-save " << (enabled ? "enabled" : "disabled") 
                  << " (interval: " << interval << " seconds)\n";
    }
    
    std::vector<ConfigurationInfo> scanConfigurations(const std::string& directory = "configs") {
        std::vector<ConfigurationInfo> configs;
        
        if (!std::filesystem::exists(directory)) {
            std::cout << "Configuration directory does not exist: " << directory << std::endl;
            return configs;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && 
                (entry.path().extension() == ".json" || entry.path().extension() == ".cfg" ||
                 entry.path().extension() == ".txt" || entry.path().extension() == ".xml")) {
                
                ConfigurationInfo info;
                info.path = entry.path().string();
                info.name = entry.path().filename().string();
                info.size = entry.file_size();
                info.lastModified = std::chrono::system_clock::from_time_t(
                    std::filesystem::last_write_time(entry));
                
                // Parse configuration file
                info.settings = parseConfigurationFile(info.path);
                
                // Validate configuration
                info.isValid = validateConfiguration(info.settings);
                
                // Set description based on file content
                if (info.name.find("phase") != std::string::npos) {
                    info.description = "Phase " + info.name.substr(5, 1) + " configuration";
                } else if (info.name.find("brain") != std::string::npos) {
                    info.description = "Brain configuration";
                } else if (info.name.find("world") != std::string::npos) {
                    info.description = "World configuration";
                } else {
                    info.description = "NLM configuration file";
                }
                
                configs.push_back(info);
            }
        }
        
        // Sort by last modified time (most recent first)
        std::sort(configs.begin(), configs.end(),
            [](const ConfigurationInfo& a, const ConfigurationInfo& b) {
                return a.lastModified > b.lastModified;
            });
        
        return configs;
    }
    
    bool validateConfiguration(const std::map<std::string, std::string>& settings) {
        std::vector<std::string> errors;
        
        // Required keys validation
        std::vector<std::string> requiredKeys = {
            "neuron_count", "region_count", "connection_probability", "random_seed"
        };
        
        for (const auto& key : requiredKeys) {
            if (settings.find(key) == settings.end()) {
                errors.push_back("Missing required key: " + key);
            }
        }
        
        // Value validation
        if (settings.find("neuron_count") != settings.end()) {
            try {
                int64_t neuronCount = std::stoll(settings.at("neuron_count"));
                if (neuronCount <= 0 || neuronCount > 1000000) {
                    errors.push_back("Invalid neuron_count value: " + std::to_string(neuronCount));
                }
            } catch (const std::exception& e) {
                errors.push_back("Invalid neuron_count format: " + std::string(e.what()));
            }
        }
        
        if (settings.find("connection_probability") != settings.end()) {
            try {
                float probability = std::stof(settings.at("connection_probability"));
                if (probability < 0.0f || probability > 1.0f) {
                    errors.push_back("Invalid connection_probability value: " + std::to_string(probability));
                }
            } catch (const std::exception& e) {
                errors.push_back("Invalid connection_probability format: " + std::string(e.what()));
            }
        }
        
        // Check for validation errors
        if (!errors.empty()) {
            for (const auto& error : errors) {
                std::cout << "Validation error: " << error << std::endl;
            }
            return false;
        }
        
        return true;
    }
    
    std::map<std::string, std::string> parseConfigurationFile(const std::string& filename) {
        std::map<std::string, std::string> settings;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cout << "Warning: Could not open configuration file: " << filename << std::endl;
            return settings;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments
            if (line.empty() || line.find('#') == 0) {
                continue;
            }
            
            // Parse key-value pairs (simplified)
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                settings[key] = value;
            }
        }
        
        return settings;
    }
    
    bool saveConfiguration(const std::map<std::string, std::string>& settings,
                           const std::string& filename,
                           const std::string& format = "text") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Warning: Could not write configuration file: " << filename << std::endl;
            return false;
        }
        
        if (format == "text") {
            for (const auto& pair : settings) {
                file << pair.first << " = " << pair.second << "\n";
            }
        } else {
            file << "# Configuration file in format: " << format << "\n";
            for (const auto& pair : settings) {
                file << pair.first << ": " << pair.second << "\n";
            }
        }
        
        file.close();
        
        std::cout << "Configuration saved to: " << filename << std::endl;
        return true;
    }
    
    std::map<std::string, std::string> loadConfiguration(const std::string& filename) {
        return parseConfigurationFile(filename);
    }
    
    bool exportConfiguration(std::shared_ptr<Config> config, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Warning: Could not write configuration file: " << filename << std::endl;
            return false;
        }
        
        file << "NLM Configuration Export\n";
        file << "========================\n\n";
        
        file << "Generated on: " << getCurrentTimestamp() << "\n\n";
        
        // Export all settings from config (simplified)
        if (config->hasKey("neuron_count")) {
            file << "neuron_count = " << config->getInt64("neuron_count") << "\n";
        }
        if (config->hasKey("region_count")) {
            file << "region_count = " << config->getInt64("region_count") << "\n";
        }
        if (config->hasKey("connection_probability")) {
            file << "connection_probability = " << config->getFloat("connection_probability") << "\n";
        }
        if (config->hasKey("random_seed")) {
            file << "random_seed = " << config->getInt64("random_seed") << "\n";
        }
        
        file.close();
        
        std::cout << "Configuration exported to: " << filename << std::endl;
        return true;
    }
    
    std::vector<ConfigurationInfo> getConfigurationList(const std::string& directory = "configs") {
        return scanConfigurations(directory);
    }
    
    void printConfigurationList(const std::vector<ConfigurationInfo>& configs) {
        std::cout << "Configuration Files:\n";
        std::cout << "===================\n\n";
        
        for (const auto& config : configs) {
            std::cout << "Name: " << config.name << "\n";
            std::cout << "Path: " << config.path << "\n";
            std::cout << "Description: " << config.description << "\n";
            std::cout << "Size: " << config.size << " bytes\n";
            std::cout << "Last Modified: " << getTimestampString(config.lastModified) << "\n";
            std::cout << "Valid: " << (config.isValid ? "Yes" : "No") << "\n";
            
            if (!config.validationErrors.empty()) {
                std::cout << "Validation Errors:\n";
                for (const auto& error : config.validationErrors) {
                    std::cout << "  - " << error << "\n";
                }
            }
            
            std::cout << "\n";
        }
    }
    
    std::string getTimestampString(const std::chrono::system_clock::time_point& tp) {
        auto time_t = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
private:
    bool autoSaveEnabled_;
    double autoSaveInterval_;
};

/**
 * @brief Configuration comparison and diff tools
 */
class ConfigurationComparator {
public:
    struct ConfigurationDiff {
        std::string file1;
        std::string file2;
        std::vector<Difference> differences;
        std::vector<Difference> similarities;
        float similarityScore;
    };
    
    struct Difference {
        std::string key;
        std::string value1;
        std::string value2;
        std::string differenceType;
        float similarity;
    };
    
    ConfigurationComparator() {}
    
    ConfigurationDiff compareConfigurations(const std::string& file1, const std::string& file2) {
        ConfigurationDiff diff;
        diff.file1 = file1;
        diff.file2 = file2;
        
        // Load configurations
        auto config1 = parseConfigurationFile(file1);
        auto config2 = parseConfigurationFile(file2);
        
        // Find differences
        std::set<std::string> allKeys;
        for (const auto& pair : config1) allKeys.insert(pair.first);
        for (const auto& pair : config2) allKeys.insert(pair.first);
        
        for (const std::string& key : allKeys) {
            auto it1 = config1.find(key);
            auto it2 = config2.find(key);
            
            std::string value1 = (it1 != config1.end()) ? it1->second : "[NOT SET]";
            std::string value2 = (it2 != config2.end()) ? it2->second : "[NOT SET]";
            
            if (value1 != value2) {
                Difference difference;
                difference.key = key;
                difference.value1 = value1;
                difference.value2 = value2;
                difference.differenceType = "DIFFERENT";
                difference.similarity = 0.0f;
                
                diff.differences.push_back(difference);
            } else if (value1 != "[NOT SET]") {
                Difference similarity;
                similarity.key = key;
                similarity.value1 = value1;
                similarity.value2 = value2;
                similarity.differenceType = "SAME";
                similarity.similarity = 1.0f;
                
                diff.similarities.push_back(similarity);
            }
        }
        
        // Calculate similarity score
        if (!config1.empty() && !config2.empty()) {
            float totalKeys = static_cast<float>(allKeys.size());
            float similarKeys = static_cast<float>(diff.similarities.size());
            diff.similarityScore = similarKeys / totalKeys;
        } else {
            diff.similarityScore = 0.0f;
        }
        
        return diff;
    }
    
    void printConfigurationDiff(const ConfigurationDiff& diff) {
        std::cout << "Configuration Comparison: " << diff.file1 << " vs " << diff.file2 << "\n";
        std::cout << "==================================================\n\n";
        
        std::cout << "Similarity Score: " << (diff.similarityScore * 100) << "%\n\n";
        
        if (!diff.differences.empty()) {
            std::cout << "Differences:\n";
            for (const auto& difference : diff.differences) {
                std::cout << "  - " << difference.key << "\n";
                std::cout << "    " << diff.file1 << ": " << difference.value1 << "\n";
                std::cout << "    " << diff.file2 << ": " << difference.value2 << "\n\n";
            }
        }
        
        if (!diff.similarities.empty()) {
            std::cout << "Similarities:\n";
            for (const auto& similarity : diff.similarities) {
                std::cout << "  + " << similarity.key << " = " << similarity.value1 << "\n";
            }
        }
    }
    
    std::string exportDiffReport(const ConfigurationDiff& diff, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return "Failed to open file: " + filename;
        }
        
        file << "Configuration Comparison Report\n";
        file << "==============================\n\n";
        
        file << "Files compared: " << diff.file1 << " and " << diff.file2 << "\n";
        file << "Similarity Score: " << (diff.similarityScore * 100) << "%\n\n";
        
        file << "Differences:\n";
        for (const auto& difference : diff.differences) {
            file << "Key: " << difference.key << "\n";
            file << "  " << diff.file1 << ": " << difference.value1 << "\n";
            file << "  " << diff.file2 << ": " << difference.value2 << "\n\n";
        }
        
        file << "Similarities:\n";
        for (const auto& similarity : diff.similarities) {
            file << "Key: " << similarity.key << " = " << similarity.value1 << "\n";
        }
        
        file.close();
        return "Report exported to: " + filename;
    }
};

/**
 * @brief Advanced visualization and analysis tools
 */
class AdvancedVisualizationTools {
public:
    struct VisualizationOptions {
        bool enable3D = false;
        bool enableAnimation = false;
        bool enableExport = false;
        std::string outputFormat = "png";
        int animationFPS = 30;
        double animationDuration = 10.0;
        std::string colormap = "viridis";
        bool showLegend = true;
        bool showAxes = true;
        bool showGrid = true;
    };
    
    AdvancedVisualizationTools() : initialized_(false) {}
    
    void initialize(const VisualizationOptions& options) {
        options_ = options;
        initialized_ = true;
        
        std::cout << "AdvancedVisualizationTools initialized with options:\n";
        std::cout << "  3D: " << (options_.enable3D ? "enabled" : "disabled") << "\n";
        std::cout << "  Animation: " << (options_.enableAnimation ? "enabled" : "disabled") << "\n";
        std::cout << "  Export: " << (options_.enableExport ? "enabled" : "disabled") << "\n";
        std::cout << "  Format: " << options_.outputFormat << "\n";
        std::cout << "  FPS: " << options_.animationFPS << "\n";
        std::cout << "  Duration: " << options_.animationDuration << "s\n";
        std::cout << "  Colormap: " << options_.colormap << "\n";
    }
    
    void generateBrainActivityVisualization(std::shared_ptr<Brain> brain,
                                            std::shared_ptr<AgentBrain> agent,
                                            std::shared_ptr<SimpleWorld> world,
                                            const std::string& filename = "brain_activity") {
        if (!initialized_ || !brain || !agent || !world) {
            return;
        }
        
        std::cout << "Generating advanced brain activity visualization...\n";
        
        // Collect comprehensive activity data
        std::vector<std::vector<float>> activityData;
        std::vector<std::string> timeLabels;
        
        // Simulate activity over time
        for (int timeStep = 0; timeStep < 100; ++timeStep) {
            std::vector<float> frameData;
            
            // Collect neuron activity
            for (const auto& region : brain->getRegions()) {
                for (const auto& neuron : region->getNeurons()) {
                    frameData.push_back(neuron->getActivityLevel());
                }
            }
            
            activityData.push_back(frameData);
            timeLabels.push_back("t=" + std::to_string(timeStep * 0.1) + "s");
        }
        
        // Generate visualization
        generateAdvancedVisualization(activityData, timeLabels, filename);
    }
    
    void generateNetworkVisualization(std::shared_ptr<Brain> brain,
                                       const std::string& filename = "network_graph") {
        if (!initialized_ || !brain) {
            return;
        }
        
        std::cout << "Generating advanced network visualization...\n";
        
        // Collect network data
        std::vector<std::pair<int, int>> connections;
        std::vector<float> connectionWeights;
        std::vector<std::string> neuronTypes;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                connections.push_back(std::make_pair(
                    synapse->getPreNeuron()->getId(),
                    synapse->getPostNeuron()->getId()));
                
                connectionWeights.push_back(synapse->getWeight());
                neuronTypes.push_back("standard");
            }
        }
        
        // Generate network visualization
        generateAdvancedNetworkVisualization(connections, connectionWeights, neuronTypes, filename);
    }
    
    void generateStatisticalVisualization(const std::string& title,
                                          const std::vector<std::string>& labels,
                                          const std::vector<std::vector<float>>& data,
                                          const std::string& filename = "statistical") {
        if (!initialized_) {
            return;
        }
        
        std::cout << "Generating statistical visualization...\n";
        
        // Generate statistical visualization
        generateAdvancedStatisticalVisualization(title, labels, data, filename);
    }
    
private:
    void generateAdvancedVisualization(const std::vector<std::vector<float>>& data,
                                       const std::vector<std::string>& timeLabels,
                                       const std::string& filename) {
        std::ofstream file(filename + ".txt");
        if (!file.is_open()) {
            std::cout << "Warning: Could not create visualization file: " << filename << std::endl;
            return;
        }
        
        file << "Advanced Brain Activity Visualization\n";
        file << "==================================\n\n";
        
        file << "Data collected over " << timeLabels.size() << " time steps\n";
        file << "Number of neurons: " << (data.empty() ? 0 : data[0].size()) << "\n\n";
        
        // Generate time series data
        file << "Time series data:\n";
        for (size_t i = 0; i < timeLabels.size(); ++i) {
            file << timeLabels[i] << ": ";
            if (i < data.size()) {
                for (size_t j = 0; j < data[i].size(); ++j) {
                    file << data[i][j];
                    if (j < data[i].size() - 1) {
                        file << ", ";
                    }
                }
            }
            file << "\n";
        }
        
        file.close();
        
        if (options_.enableExport) {
            std::cout << "Visualization exported to: " << filename << ".txt\n";
        }
    }
    
    void generateAdvancedNetworkVisualization(const std::vector<std::pair<int, int>>& connections,
                                               const std::vector<float>& weights,
                                               const std::vector<std::string>& types,
                                               const std::string& filename) {
        std::ofstream file(filename + ".txt");
        if (!file.is_open()) {
            std::cout << "Warning: Could not create network visualization file: " << filename << std::endl;
            return;
        }
        
        file << "Advanced Network Visualization\n";
        file << "=============================\n\n";
        
        file << "Connections: " << connections.size() << "\n";
        file << "Connection types: " << types.size() << "\n\n";
        
        file << "Network structure:\n";
        for (size_t i = 0; i < connections.size(); ++i) {
            file << connections[i].first << " -> " << connections[i].second 
                 << " (weight: " << weights[i] << ", type: " << types[i] << ")\n";
        }
        
        file.close();
        
        if (options_.enableExport) {
            std::cout << "Network visualization exported to: " << filename << ".txt\n";
        }
    }
    
    void generateAdvancedStatisticalVisualization(const std::string& title,
                                                  const std::vector<std::string>& labels,
                                                  const std::vector<std::vector<float>>& data,
                                                  const std::string& filename) {
        std::ofstream file(filename + ".txt");
        if (!file.is_open()) {
            std::cout << "Warning: Could not create statistical visualization file: " << filename << std::endl;
            return;
        }
        
        file << title << "\n";
        file << std::string(title.length(), '=') << "\n\n";
        
        file << "Data visualization:\n";
        for (size_t i = 0; i < labels.size() && i < data.size(); ++i) {
            file << labels[i] << ": ";
            for (size_t j = 0; j < data[i].size(); ++j) {
                file << data[i][j];
                if (j < data[i].size() - 1) {
                    file << ", ";
                }
            }
            file << "\n";
        }
        
        file.close();
        
        if (options_.enableExport) {
            std::cout << "Statistical visualization exported to: " << filename << ".txt\n";
        }
    }
    
    VisualizationOptions options_;
    bool initialized_;
};

} // namespace nlm

#endif // NLM_VISUALIZATION_CONFIG_HPP
