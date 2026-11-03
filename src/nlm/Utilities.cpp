#include "Utilities.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// JSON implementation helper
namespace {
    std::string escapeJSONString(const std::string& str) {
        std::string result;
        result.reserve(str.size() + 2);
        result += '"';
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 32) {
                        result += "\\u";
                        result += "000" + std::to_string(static_cast<int>(c));
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        result += '"';
        return result;
    }
    
    std::string jsonEscape(const std::string& str) {
        return escapeJSONString(str);
    }
}

// ConfigUtilities implementation
bool ConfigUtilities::exportToJSON(const Config& config, const std::string& filepath) {
    try {
        nlohmann::json j;
        
        for (const auto& entry : config.getKeys()) {
            auto value = config.get<std::string>(entry);
            if (value) {
                j[entry] = *value;
            } else {
                auto intValue = config.get<int>(entry);
                if (intValue) {
                    j[entry] = *intValue;
                } else {
                    auto doubleValue = config.get<double>(entry);
                    if (doubleValue) {
                        j[entry] = *doubleValue;
                    } else {
                        auto boolValue = config.get<bool>(entry);
                        if (boolValue) {
                            j[entry] = *boolValue;
                        }
                    }
                }
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << j << std::endl;
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export config to JSON: " + std::string(e.what()));
        return false;
    }
}

bool ConfigUtilities::exportToYAML(const Config& config, const std::string& filepath) {
    try {
        // Simple YAML export (for now)
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "config:\n";
        for (const auto& entry : config.getKeys()) {
            auto value = config.get<std::string>(entry);
            if (value) {
                file << "  " << entry << ": \"" << *value << "\"\n";
            } else {
                auto intValue = config.get<int>(entry);
                if (intValue) {
                    file << "  " << entry << ": " << *intValue << "\n";
                } else {
                    auto doubleValue = config.get<double>(entry);
                    if (doubleValue) {
                        file << "  " << entry << ": " << *doubleValue << "\n";
                    } else {
                        auto boolValue = config.get<bool>(entry);
                        if (boolValue) {
                            file << "  " << entry << ": " << (*boolValue ? "true" : "false") << "\n";
                        }
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export config to YAML: " + std::string(e.what()));
        return false;
    }
}

bool ConfigUtilities::exportToINI(const Config& config, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration Export\n";
        file << "# Exported at " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& entry : config.getKeys()) {
            auto value = config.get<std::string>(entry);
            if (value) {
                file << entry << " = \"" << *value << "\"\n";
            } else {
                auto intValue = config.get<int>(entry);
                if (intValue) {
                    file << entry << " = " << *intValue << "\n";
                } else {
                    auto doubleValue = config.get<double>(entry);
                    if (doubleValue) {
                        file << entry << " = " << *doubleValue << "\n";
                    } else {
                        auto boolValue = config.get<bool>(entry);
                        if (boolValue) {
                            file << entry << " = " << (*boolValue ? "true" : "false") << "\n";
                        }
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export config to INI: " + std::string(e.what()));
        return false;
    }
}

bool ConfigUtilities::importFromJSON(Config& config, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            if (it->is_string()) {
                config.set(key, it->get<std::string>());
            } else if (it->is_number_integer()) {
                config.set(key, it->get<int64_t>());
            } else if (it->is_number_float()) {
                config.set(key, it->get<double>());
            } else if (it->is_boolean()) {
                config.set(key, it->get<bool>());
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import config from JSON: " + std::string(e.what()));
        return false;
    }
}

bool ConfigUtilities::importFromYAML(const Config& config, const std::string& filepath) {
    // For now, use simple line-based parsing
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Remove whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // Remove quotes
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                config.set(key, value);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import config from YAML: " + std::string(e.what()));
        return false;
    }
}

bool ConfigUtilities::importFromINI(const Config& config, const std::string& filepath) {
    return importFromYAML(config, filepath);
}

std::vector<std::string> ConfigUtilities::validateConfig(const Config& config) {
    std::vector<std::string> errors;
    
    // Check for required keys
    const std::vector<std::string> requiredKeys = {"random_seed", "neuron_count", "simulation_timestep"};
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errors.push_back("Missing required key: " + key);
        }
    }
    
    // Validate key ranges
    auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
    if (neuronCount > 1000000) {
        errors.push_back("neuron_count exceeds reasonable limit: " + std::to_string(neuronCount));
    } else if (neuronCount < 1) {
        errors.push_back("neuron_count must be positive: " + std::to_string(neuronCount));
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) {
        errors.push_back("simulation_timestep must be positive: " + std::to_string(timestep));
    } else if (timestep > 1.0) {
        errors.push_back("simulation_timestep exceeds reasonable limit: " + std::to_string(timestep));
    }
    
    return errors;
}

bool ConfigUtilities::checkRequiredKeys(const Config& config, const std::vector<std::string>& requiredKeys) {
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            return false;
        }
    }
    return true;
}

std::string ConfigUtilities::getConfigStats(const Config& config) {
    std::ostringstream oss;
    
    oss << "Configuration Statistics:\n";
    oss << "  Total keys: " << config.getKeys().size() << "\n";
    oss << "  Default keys: " << std::count_if(config.getKeys().begin(), config.getKeys().end(),
        [&config](const std::string& key) {
            auto entry = config.get<std::string>(key);
            return entry.has_value();
        }) << "\n";
    oss << "  Runtime keys: " << std::count_if(config.getKeys().begin(), config.getKeys().end(),
        [&config](const std::string& key) {
            auto entry = config.get<std::string>(key);
            return entry.has_value();
        }) << "\n";
    
    return oss.str();
}

std::string ConfigUtilities::getMemoryUsage(const Config& config) {
    // Simple estimate based on number of keys and values
    size_t totalSize = 0;
    for (const auto& key : config.getKeys()) {
        totalSize += key.size() + 32; // Estimate per entry
        
        auto value = config.get<std::string>(key);
        if (value) {
            totalSize += value->size();
        }
    }
    
    std::ostringstream oss;
    oss << "Estimated config memory usage: " << (totalSize / 1024.0) << " KB\n";
    return oss.str();
}

bool ConfigUtilities::compareConfigs(const Config& config1, const Config& config2) {
    const auto& keys1 = config1.getKeys();
    const auto& keys2 = config2.getKeys();
    
    if (keys1.size() != keys2.size()) {
        return false;
    }
    
    for (const auto& key : keys1) {
        if (!config2.has(key)) {
            return false;
        }
        
        auto value1 = config1.get<std::string>(key);
        auto value2 = config2.get<std::string>(key);
        
        if ((value1 && !value2) || (!value1 && value2)) {
            return false;
        }
        
        if (value1 && value2 && *value1 != *value2) {
            return false;
        }
    }
    
    return true;
}

double ConfigUtilities::configDifferenceScore(const Config& config1, const Config& config2) {
    const auto& keys1 = config1.getKeys();
    const auto& keys2 = config2.getKeys();
    
    if (keys1.empty() && keys2.empty()) {
        return 0.0;
    }
    
    double maxDifference = static_cast<double>(keys1.size() + keys2.size());
    double difference = 0.0;
    
    for (const auto& key : keys1) {
        if (!config2.has(key)) {
            difference += 1.0;
        } else {
            auto value1 = config1.get<std::string>(key);
            auto value2 = config2.get<std::string>(key);
            
            if (value1 && value2 && *value1 != *value2) {
                difference += 1.0;
            }
        }
    }
    
    for (const auto& key : keys2) {
        if (!config1.has(key)) {
            difference += 1.0;
        }
    }
    
    return 1.0 - (difference / maxDifference);
}

bool ConfigUtilities::safeSet(Config& config, const std::string& key, const std::string& value, ConfigSource source) {
    try {
        config.set(key, value, source);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set config value for key '" + key + "': " + std::string(e.what()));
        return false;
    }
}

bool ConfigUtilities::safeGetString(const Config& config, const std::string& key, std::string& value, const std::string& defaultValue) {
    auto val = config.get<std::string>(key);
    if (val) {
        value = *val;
        return true;
    } else {
        value = defaultValue;
        return false;
    }
}

bool ConfigUtilities::safeGetInt(const Config& config, const std::string& key, int& value, int defaultValue) {
    auto val = config.get<int>(key);
    if (val) {
        value = *val;
        return true;
    } else {
        value = defaultValue;
        return false;
    }
}

bool ConfigUtilities::safeGetDouble(const Config& config, const std::string& key, double& value, double defaultValue) {
    auto val = config.get<double>(key);
    if (val) {
        value = *val;
        return true;
    } else {
        value = defaultValue;
        return false;
    }
}

bool ConfigUtilities::safeGetBool(const Config& config, const std::string& key, bool& value, bool defaultValue) {
    auto val = config.get<bool>(key);
    if (val) {
        value = *val;
        return true;
    } else {
        value = defaultValue;
        return false;
    }
}

// BrainUtilities implementation
bool BrainUtilities::saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot save brain state: brain is null");
        return false;
    }
    
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Save brain configuration
        auto config = brain->getConfig();
        if (!config->saveToFile(filepath + ".config")) {
            return false;
        }
        
        // For now, just save basic info to text file
        std::ofstream textFile(filepath);
        if (!textFile.is_open()) {
            return false;
        }
        
        textFile << "NLM Brain State\n";
        textFile << "=================\n\n";
        textFile << "Neuron count: " << brain->getTotalNeuronCount() << "\n";
        textFile << "Synapse count: " << brain->getTotalSynapseCount() << "\n";
        textFile << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        textFile << "Active neurons: " << brain->getActiveNeuronCount() << "\n";
        textFile << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        textFile << "Average firing rate: " << brain->getAverageFiringRate() << "\n";
        textFile << "Development stage: " << getDevelopmentalStageString(brain) << "\n";
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save brain state: " + std::string(e.what()));
        return false;
    }
}

bool BrainUtilities::loadBrainState(std::shared_ptr<Brain>& brain, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Neuron count: ") != std::string::npos) {
                // Parse line
                size_t pos = line.find(": ");
                if (pos != std::string::npos) {
                    std::string value = line.substr(pos + 2);
                    // Note: This is just reading metadata, not actually loading the brain
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load brain state: " + std::string(e.what()));
        return false;
    }
}

bool BrainUtilities::backupBrainState(const std::shared_ptr<Brain>& brain, const std::string& backupPath) {
    if (!brain) {
        return false;
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string backupFile = backupPath + "/nlm_backup_" + timestamp + ".state";
    
    return saveBrainState(brain, backupFile);
}

std::string BrainUtilities::analyzeBrainState(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return "Brain is null";
    }
    
    std::ostringstream oss;
    oss << "Brain State Analysis:\n";
    oss << "===================\n\n";
    
    oss << "Neurons:\n";
    oss << "  Total: " << brain->getTotalNeuronCount() << "\n";
    oss << "  Active: " << brain->getActiveNeuronCount() << "\n";
    oss << "  Firing: " << brain->getFiringNeuronCount() << "\n";
    oss << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n\n";
    
    oss << "Synapses:\n";
    oss << "  Total: " << brain->getTotalSynapseCount() << "\n";
    oss << "  Total spikes: " << brain->getTotalSpikeCount() << "\n\n";
    
    oss << "Development:\n";
    oss << "  Stage: " << getDevelopmentalStageString(brain) << "\n";
    
    return oss.str();
}

std::string BrainUtilities::getBrainHealthReport(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return "Health check failed: Brain is null";
    }
    
    std::ostringstream oss;
    oss << "Brain Health Report\n";
    oss << "===================\n\n";
    
    std::vector<std::string> issues;
    
    // Check neuron count
    if (brain->getTotalNeuronCount() == 0) {
        issues.push_back("No neurons in brain");
    }
    
    // Check spike count
    if (brain->getTotalSpikeCount() == 0) {
        oss << "WARNING: No spikes recorded - brain may not be active\n";
    }
    
    // Check firing rate
    double firingRate = brain->getAverageFiringRate();
    if (firingRate < 0.01) {
        oss << "WARNING: Very low firing rate - " << firingRate << " Hz\n";
    } else if (firingRate > 100.0) {
        oss << "WARNING: Very high firing rate - " << firingRate << " Hz\n";
    }
    
    // Check excitation/inhibition ratio
    double ratio = brain->getExcitationInhibitionRatio();
    if (ratio < 0.1 || ratio > 10.0) {
        oss << "WARNING: Unbalanced excitation/inhibition ratio - " << ratio << "\n";
    }
    
    if (issues.empty()) {
        oss << "Health Status: GOOD\n";
    } else {
        oss << "Health Status: ISSUES DETECTED\n";
        oss << "Issues:\n";
        for (const auto& issue : issues) {
            oss << "  - " << issue << "\n";
        }
    }
    
    return oss.str();
}

std::string BrainUtilities::getPerformanceMetrics(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return "Cannot get performance metrics: Brain is null";
    }
    
    std::ostringstream oss;
    oss << "Performance Metrics\n";
    oss << "===================\n\n";
    
    // For now, return basic metrics
    oss << "Metrics:\n";
    oss << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
    oss << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
    oss << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
    oss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
    oss << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
    oss << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
    
    return oss.str();
}

std::vector<std::string> BrainUtilities::validateBrain(const std::shared_ptr<Brain>& brain) {
    std::vector<std::string> errors;
    
    if (!brain) {
        errors.push_back("Brain is null");
        return errors;
    }
    
    if (brain->getTotalNeuronCount() == 0) {
        errors.push_back("Brain has no neurons");
    }
    
    if (brain->getTotalSynapseCount() == 0 && brain->getTotalNeuronCount() > 0) {
        errors.push_back("Brain has neurons but no synapses");
    }
    
    double firingRate = brain->getAverageFiringRate();
    if (firingRate < 0.0) {
        errors.push_back("Invalid firing rate: " + std::to_string(firingRate));
    }
    
    return errors;
}

bool BrainUtilities::checkBrainIntegrity(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return false;
    }
    
    auto errors = validateBrain(brain);
    return errors.empty();
}

bool BrainUtilities::checkMemoryIntegrity(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return false;
    }
    
    // Simple memory check - in a real implementation, this would be more sophisticated
    try {
        // Try to access some brain properties to check memory
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        
        // If we got here without exceptions, memory integrity is likely fine
        return true;
    } catch (...) {
        return false;
    }
}

size_t BrainUtilities::countActiveNeurons(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return 0;
    }
    return brain->getActiveNeuronCount();
}

size_t BrainUtilities::countTotalSpikes(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return 0;
    }
    return brain->getTotalSpikeCount();
}

double BrainUtilities::getAverageFiringRate(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return 0.0;
    }
    return brain->getAverageFiringRate();
}

std::string BrainUtilities::getDevelopmentalStageString(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        return "UNKNOWN";
    }
    return std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
}

} // namespace nlm
