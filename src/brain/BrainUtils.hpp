// Enhanced Brain Utility Functions
// Utility functions for brain inspection, analysis, and debugging
// These complement the enhanced main.cpp with additional brain analysis capabilities

#pragma once

#include "brain/Brain.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <sstream>

namespace nlm {
namespace brain_utils {

// Brain statistics and analysis
struct BrainStatistics {
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t activeNeurons = 0;
    size_t firingNeurons = 0;
    size_t totalSpikes = 0;
    float averageFiringRate = 0.0f;
    float excitationInhibitionRatio = 1.0f;
    std::vector<size_t> neuronCountsByRegion;
    std::vector<float> firingRatesByRegion;
    std::vector<size_t> spikeCountsByRegion;
    float averageWeight = 0.0f;
    float weightStdDev = 0.0f;
    float maxWeight = 0.0f;
    float minWeight = 0.0f;
    size_t maxConnections = 0;
    size_t minConnections = 0;
    float connectionDensity = 0.0f;
};

// Enhanced brain inspection with detailed analysis
class BrainInspector {
public:
    static BrainStatistics collectStatistics(std::shared_ptr<Brain> brain) {
        BrainStatistics stats;
        
        if (!brain) return stats;
        
        // Basic statistics
        stats.totalNeurons = brain->getTotalNeuronCount();
        stats.totalSynapses = brain->getTotalSynapseCount();
        stats.activeNeurons = brain->getActiveNeuronCount();
        stats.firingNeurons = brain->getFiringNeuronCount();
        stats.totalSpikes = brain->getTotalSpikeCount();
        stats.averageFiringRate = brain->getAverageFiringRate();
        stats.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        // Regional analysis
        auto regionIds = brain->getRegionIds();
        stats.neuronCountsByRegion.reserve(regionIds.size());
        stats.firingRatesByRegion.reserve(regionIds.size());
        stats.spikeCountsByRegion.reserve(regionIds.size());
        
        for (RegionId regionId : regionIds) {
            if (auto* region = brain->getRegion(regionId)) {
                stats.neuronCountsByRegion.push_back(region->getNeuronCount());
                
                // Calculate firing rate for this region
                float regionFiringRate = 0.0f;
                if (stats.neuronCountsByRegion.back() > 0) {
                    // Approximate by using total firing rate proportional to neuron count
                    regionFiringRate = stats.averageFiringRate * 
                                      static_cast<float>(stats.neuronCountsByRegion.back()) / 
                                      static_cast<float>(stats.totalNeurons);
                }
                stats.firingRatesByRegion.push_back(regionFiringRate);
                
                // For spikes, we would need more detailed tracking
                // For now, use a simplified approximation
                size_t regionSpikes = static_cast<size_t>(stats.totalSpikes * 
                                                       static_cast<float>(stats.neuronCountsByRegion.back()) / 
                                                       static_cast<float>(stats.totalNeurons));
                stats.spikeCountsByRegion.push_back(regionSpikes);
            }
        }
        
        // Weight statistics
        if (stats.totalSynapses > 0) {
            std::vector<float> weights;
            weights.reserve(stats.totalSynapses);
            
            // Collect weights from all regions
            for (RegionId regionId : regionIds) {
                if (auto* region = brain->getRegion(regionId)) {
                    auto& synapses = region->getSynapses();
                    for (const auto& synapse : synapses) {
                        weights.push_back(synapse->getWeight());
                    }
                }
            }
            
            if (!weights.empty()) {
                stats.averageWeight = std::accumulate(weights.begin(), weights.end(), 0.0f) / weights.size();
                
                // Calculate standard deviation
                float sumSquaredDiff = 0.0f;
                for (float weight : weights) {
                    float diff = weight - stats.averageWeight;
                    sumSquaredDiff += diff * diff;
                }
                stats.weightStdDev = std::sqrt(sumSquaredDiff / weights.size());
                
                // Find min and max
                stats.maxWeight = *std::max_element(weights.begin(), weights.end());
                stats.minWeight = *std::min_element(weights.begin(), weights.end());
            }
        }
        
        // Connection statistics
        if (stats.totalNeurons > 0) {
            size_t maxPossibleConnections = stats.totalNeurons * (stats.totalNeurons - 1);
            if (maxPossibleConnections > 0) {
                stats.connectionDensity = static_cast<float>(stats.totalSynapses) / 
                                         static_cast<float>(maxPossibleConnections);
            }
            
            // Estimate connection statistics
            // In a real implementation, we would track this more precisely
            stats.maxConnections = static_cast<size_t>(stats.totalSynapses * 1.5);  // Approximation
            stats.minConnections = std::max(static_cast<size_t>(10), 
                                          static_cast<size_t>(stats.totalSynapses * 0.5));
        }
        
        return stats;
    }
    
    // Format statistics as a readable string
    static std::string formatStatistics(const BrainStatistics& stats) {
        std::stringstream ss;
        
        ss << "=== Brain Statistics ===\n";
        ss << "Total Neurons: " << stats.totalNeurons << "\n";
        ss << "Total Synapses: " << stats.totalSynapses << "\n";
        ss << "Active Neurons: " << stats.activeNeurons << "\n";
        ss << "Firing Neurons: " << stats.firingNeurons << "\n";
        ss << "Total Spikes: " << stats.totalSpikes << "\n";
        ss << "Average Firing Rate: " << std::fixed << std::setprecision(3) 
           << stats.averageFiringRate << " Hz\n";
        ss << "Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(3) 
           << stats.excitationInhibitionRatio << "\n";
        ss << "\n=== Regional Statistics ===\n";
        
        for (size_t i = 0; i < stats.neuronCountsByRegion.size(); ++i) {
            ss << "Region " << i << ": "
               << stats.neuronCountsByRegion[i] << " neurons, "
               << std::fixed << std::setprecision(3) << stats.firingRatesByRegion[i]
               << " Hz firing rate\n";
        }
        
        ss << "\n=== Weight Statistics ===\n";
        ss << "Average Weight: " << std::fixed << std::setprecision(5) 
           << stats.averageWeight << "\n";
        ss << "Weight Standard Deviation: " << std::fixed << std::setprecision(5) 
           << stats.weightStdDev << "\n";
        ss << "Max Weight: " << std::fixed << std::setprecision(5) 
           << stats.maxWeight << "\n";
        ss << "Min Weight: " << std::fixed << std::setprecision(5) 
           << stats.minWeight << "\n";
        
        return ss.str();
    }
    
    // Create a simple visualization of neural network connectivity
    static std::string createNetworkVisualization(std::shared_ptr<Brain> brain, 
                                                  int maxRegions = 3,
                                                  int maxNeuronsPerRegion = 20) {
        if (!brain) return "No brain provided.\n";
        
        std::stringstream ss;
        ss << "=== Neural Network Visualization ===\n\n";
        
        auto regionIds = brain->getRegionIds();
        int regionsToShow = std::min(static_cast<int>(regionIds.size()), maxRegions);
        
        for (int regionIdx = 0; regionIdx < regionsToShow; ++regionIdx) {
            RegionId regionId = regionIds[regionIdx];
            if (auto* region = brain->getRegion(regionId)) {
                ss << "Region " << regionIdx << ": " << region->getName() << "\n";
                
                auto neurons = region->getAllNeurons();
                int neuronsToShow = std::min(static_cast<int>(neurons.size()), maxNeuronsPerRegion);
                
                for (int neuronIdx = 0; neuronIdx < neuronsToShow; ++neuronIdx) {
                    Neuron* neuron = neurons[neuronIdx];
                    ss << "  Neuron " << neuronIdx << 
                       " (ID: " << neuron->getId() << ") "
                       << "Type: " << static_cast<int>(neuron->getType())
                       << " Pot: " << std::fixed << std::setprecision(1)
                       << neuron->getMembranePotential() << " mV\n";
                    
                    // Show connections
                    auto incoming = neuron->getIncomingSynapses();
                    auto outgoing = neuron->getOutgoingSynapses();
                    if (!incoming.empty() || !outgoing.empty()) {
                        ss << "    Incoming: " << incoming.size() << " connections\n";
                        ss << "    Outgoing: " << outgoing.size() << " connections\n";
                    }
                }
                
                if (neurons.size() > neuronsToShow) {
                    ss << "  ... and " << (neurons.size() - neuronsToShow) 
                       << " more neurons\n";
                }
                ss << "\n";
            }
        }
        
        return ss.str();
    }
    
    // Analyze plasticity status
    static std::string analyzePlasticity(std::shared_ptr<Brain> brain) {
        if (!brain) return "No brain provided.\n";
        
        std::stringstream ss;
        ss << "=== Plasticity Analysis ===\n\n";
        
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structural = brain->getStructuralPlasticity();
        
        ss << "Plasticity Systems Status:\n";
        ss << (stdp ? (stdp->isEnabled() ? "✓ STDP: ENABLED\n" : "✗ STDP: DISABLED\n") 
                   : "? STDP: UNAVAILABLE\n");
        ss << (hebbian ? (hebbian->isEnabled() ? "✓ Hebbian: ENABLED\n" : "✗ Hebbian: DISABLED\n") 
                      : "? Hebbian: UNAVAILABLE\n");
        ss << (structural ? (structural->isEnabled() ? "✓ Structural Plasticity: ENABLED\n" 
                                                     : "✗ Structural Plasticity: DISABLED\n")
                         : "? Structural Plasticity: UNAVAILABLE\n");
        
        ss << "\nPlasticy Features:\n";
        
        // Check synapse plasticity flags
        size_t totalSynapses = brain->getTotalSynapseCount();
        size_t hebbianEnabled = 0;
        size_t stdpEnabled = 0;
        size_t rewardEnabled = 0;
        
        auto regionIds = brain->getRegionIds();
        for (RegionId regionId : regionIds) {
            if (auto* region = brain->getRegion(regionId)) {
                auto& synapses = region->getSynapses();
                for (const auto& synapse : synapses) {
                    const auto& flags = synapse->getPlasticityFlags();
                    if (flags.hebbian) ++hebbianEnabled;
                    if (flags.stdp) ++stdpEnabled;
                    if (flags.rewardModulated) ++rewardEnabled;
                }
            }
        }
        
        if (totalSynapses > 0) {
            ss << "  Hebbian-enabled synapses: " << hebbianEnabled << 
               " (" << (hebbianEnabled * 100.0 / totalSynapses) << "%)" << "\n";
            ss << "  STDP-enabled synapses: " << stdpEnabled << 
               " (" << (stdpEnabled * 100.0 / totalSynapses) << "%)" << "\n";
            ss << "  Reward-modulated synapses: " << rewardEnabled << 
               " (" << (rewardEnabled * 100.0 / totalSynapses) << "%)" << "\n";
        }
        
        return ss.str();
    }
    
    // Save brain state with enhanced error handling and metadata
    static bool saveBrainStateWithMetadata(std::shared_ptr<Brain> brain, 
                                          const std::string& filepath,
                                          const std::string& description = "") {
        try {
            // Create backup file if it exists
            std::ifstream src(filepath, std::ios::binary);
            std::string backupPath = filepath + ".backup";
            bool hadBackup = false;
            
            if (src.good()) {
                std::ofstream dst(backupPath, std::ios::binary);
                dst << src.rdbuf();
                hadBackup = true;
                src.close();
            }
            
            // Save brain state
            bool success = brain->save(filepath);
            
            if (success) {
                // Create metadata file
                std::string metadataPath = filepath + ".metadata";
                std::ofstream metadata(metadataPath);
                
                metadata << "NLM Brain State File\n";
                metadata << "Created: " << getCurrentTimestamp() << "\n";
                if (!description.empty()) {
                    metadata << "Description: " << description << "\n";
                }
                
                BrainStatistics stats = collectStatistics(brain);
                metadata << "Total Neurons: " << stats.totalNeurons << "\n";
                metadata << "Total Synapses: " << stats.totalSynapses << "\n";
                metadata << "Total Spikes: " << stats.totalSpikes << "\n";
                metadata << "Average Firing Rate: " << stats.averageFiringRate << "\n";
                
                metadata.close();
                
                NLM_LOG_INFO("Brain state saved with metadata to: " + filepath);
                NLM_LOG_INFO("Metadata file: " + metadataPath);
                
                if (hadBackup) {
                    NLM_LOG_INFO("Backup created: " + backupPath);
                }
            } else {
                NLM_LOG_ERROR("Failed to save brain state to: " + filepath);
                
                // Try to restore from backup if it exists
                if (hadBackup) {
                    std::ifstream backup(backupPath, std::ios::binary);
                    std::ofstream dst(filepath, std::ios::binary);
                    dst << backup.rdbuf();
                    backup.close();
                    NLM_LOG_INFO("Restored brain state from backup");
                }
            }
            
            return success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception while saving brain state: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load brain state with validation and rollback
    static bool loadBrainStateWithValidation(std::shared_ptr<Brain> brain,
                                            const std::string& filepath,
                                            bool autoRollback = true) {
        try {
            // Create backup before loading
            std::string backupPath = filepath + ".load_backup";
            bool createdBackup = false;
            
            if (brain->hasCurrentState()) {
                brain->save(backupPath);
                createdBackup = true;
            }
            
            // Load brain state
            bool success = brain->load(filepath);
            
            if (success) {
                // Validate loaded state
                if (validateBrainState(brain)) {
                    NLM_LOG_INFO("Brain state loaded and validated successfully from: " + filepath);
                    
                    // Check metadata if available
                    std::string metadataPath = filepath + ".metadata";
                    std::ifstream metadata(metadataPath);
                    if (metadata.good()) {
                        std::string line;
                        while (std::getline(metadata, line)) {
                            if (line.find("Created: ") == 0) {
                                NLM_LOG_INFO("Brain state created: " + line.substr(9));
                            }
                        }
                        metadata.close();
                    }
                    
                    return true;
                } else {
                    NLM_LOG_ERROR("Loaded brain state failed validation");
                    success = false;
                }
            }
            
            // Handle failure case
            if (!success && autoRollback && createdBackup) {
                NLM_LOG_INFO("Rolling back to previous state...");
                brain->load(backupPath);
                std::remove(backupPath.c_str());
                NLM_LOG_INFO("Rollback completed");
            }
            
            return success;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception while loading brain state: " + std::string(e.what()));
            return false;
        }
    }
    
    // Validate brain state integrity
    static bool validateBrainState(std::shared_ptr<Brain> brain) {
        if (!brain) {
            NLM_LOG_ERROR("Cannot validate null brain");
            return false;
        }
        
        bool isValid = true;
        
        // Check basic integrity
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        
        if (neuronCount == 0) {
            NLM_LOG_WARNING("Brain has no neurons");
        }
        
        if (synapseCount == 0) {
            NLM_LOG_WARNING("Brain has no synapses");
        }
        
        // Check regional consistency
        auto regionIds = brain->getRegionIds();
        for (RegionId regionId : regionIds) {
            if (auto* region = brain->getRegion(regionId)) {
                size_t regionNeuronCount = region->getNeuronCount();
                size_t regionSynapseCount = region->getSynapseCount();
                
                // Basic sanity checks
                if (regionNeuronCount == 0 && regionSynapseCount > 0) {
                    NLM_LOG_ERROR("Region " + std::to_string(regionId) + 
                                 " has synapses but no neurons");
                    isValid = false;
                }
                
                // Check neuron IDs are valid
                auto neurons = region->getAllNeurons();
                std::unordered_set<NeuronId> neuronIds;
                for (Neuron* neuron : neurons) {
                    if (neuronIds.find(neuron->getId()) != neuronIds.end()) {
                        NLM_LOG_ERROR("Duplicate neuron ID in region " + std::to_string(regionId));
                        isValid = false;
                    }
                    neuronIds.insert(neuron->getId());
                }
            }
        }
        
        return isValid;
    }
    
    // Get current timestamp as string
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Enhanced experiment utilities
namespace experiment_utils {
    // Calculate experiment performance metrics
    struct ExperimentMetrics {
        double successRate = 0.0;
        double averageExecutionTime = 0.0;
        double averageResult = 0.0;
        std::map<std::string, double> parameterEffects;
        double stabilityScore = 0.0;
        double learningRate = 0.0;
    };
    
    // Compare results between different experiment runs
    static ExperimentMetrics analyzeExperimentResults(
        const std::vector<std::map<std::string, double>>& results) {
        ExperimentMetrics metrics;
        
        if (results.empty()) return metrics;
        
        std::vector<double> successRates;
        std::vector<double> executionTimes;
        std::vector<double> resultValues;
        
        for (const auto& result : results) {
            double success = result.find("success") != result.end() ? result.at("success") : 0.0;
            double execTime = result.find("execution_time_ms") != result.end() ? 
                             result.at("execution_time_ms") : 0.0;
            double resultVal = result.find("result") != result.end() ? result.at("result") : 0.0;
            
            successRates.push_back(success);
            executionTimes.push_back(execTime);
            resultValues.push_back(resultVal);
        }
        
        // Calculate statistics
        metrics.successRate = std::accumulate(successRates.begin(), successRates.end(), 0.0) / successRates.size();
        metrics.averageExecutionTime = std::accumulate(executionTimes.begin(), executionTimes.end(), 0.0) / executionTimes.size();
        metrics.averageResult = std::accumulate(resultValues.begin(), resultValues.end(), 0.0) / resultValues.size();
        
        return metrics;
    }
    
    // Generate experiment report
    static std::string generateExperimentReport(const std::string& experimentName,
                                                const ExperimentMetrics& metrics,
                                                const std::vector<std::map<std::string, double>>& results) {
        std::stringstream ss;
        ss << "=== Experiment Report: " << experimentName << " ===\n\n";
        
        ss << "Performance Summary:\n";
        ss << "  Success Rate: " << (metrics.successRate * 100) << "%\n";
        ss << "  Average Execution Time: " << metrics.averageExecutionTime << " ms\n";
        ss << "  Average Result: " << metrics.averageResult << "\n";
        ss << "  Stability Score: " << metrics.stabilityScore << "\n";
        ss << "  Learning Rate: " << metrics.learningRate << "\n\n";
        
        ss << "Individual Runs:\n";
        for (size_t i = 0; i < results.size(); ++i) {
            ss << "  Run " << i + 1 << ": ";
            if (results[i].find("success") != results[i].end()) {
                ss << "Success: " << (results[i].at("success") > 0.5 ? "✓" : "✗") << " ";
            }
            if (results[i].find("execution_time_ms") != results[i].end()) {
                ss << "Time: " << results[i].at("execution_time_ms") << "ms ";
            }
            if (results[i].find("result") != results[i].end()) {
                ss << "Result: " << results[i].at("result");
            }
            ss << "\n";
        }
        
        return ss.str();
    }
}

// Configuration comparison utilities
namespace config_utils {
    // Compare two configuration states
    static std::string compareConfigs(const std::string& name1, std::shared_ptr<Config> config1,
                                     const std::string& name2, std::shared_ptr<Config> config2) {
        std::stringstream ss;
        ss << "=== Configuration Comparison ===\n";
        ss << "Comparing " << name1 << " vs " << name2 << "\n\n";
        
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        std::set<std::string> allKeys;
        for (const auto& key : keys1) allKeys.insert(key);
        for (const auto& key : keys2) allKeys.insert(key);
        
        bool differences = false;
        for (const auto& key : allKeys) {
            auto value1 = config1->get<std::string>(key);
            auto value2 = config2->get<std::string>(key);
            
            bool value1Exists = value1.has_value();
            bool value2Exists = value2.has_value();
            
            if (value1Exists && value2Exists) {
                if (value1.value() != value2.value()) {
                    ss << "DIFFERENCE: " << key << "\n";
                    ss << "  " << name1 << ": " << value1.value() << "\n";
                    ss << "  " << name2 << ": " << value2.value() << "\n";
                    differences = true;
                }
            } else if (value1Exists || value2Exists) {
                ss << "DIFFERENCE: " << key << "\n";
                ss << "  " << name1 << ": " << (value1Exists ? value1.value() : "(unset)") << "\n";
                ss << "  " << name2 << ": " << (value2Exists ? value2.value() : "(unset)") << "\n";
                differences = true;
            }
        }
        
        if (!differences) {
            ss << "No differences found.\n";
        } else {
            ss << "\n" << (allKeys.size() - differences) << " common settings.\n";
        }
        
        return ss.str();
    }
}

}} // namespace brain_utils

// Convenience functions for common operations
namespace nlm {
    // Print brain statistics
    inline void printBrainStats(std::shared_ptr<Brain> brain) {
        brain_utils::BrainInspector::printNeuralStatistics(brain);
    }
    
    // Save brain state with metadata
    inline bool saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath,
                             const std::string& description = "") {
        return brain_utils::BrainInspector::saveBrainStateWithMetadata(brain, filepath, description);
    }
    
    // Load brain state with validation
    inline bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain_utils::BrainInspector::loadBrainStateWithValidation(brain, filepath);
    }
    
    // Compare configurations
    inline std::string compareConfigs(const std::string& name1, std::shared_ptr<Config> config1,
                                     const std::string& name2, std::shared_ptr<Config> config2) {
        return config_utils::compareConfigs(name1, config1, name2, config2);
    }
    
    // Generate experiment report
    inline std::string generateExperimentReport(const std::string& experimentName,
                                                const std::vector<std::map<std::string, double>>& results) {
        auto metrics = experiment_utils::analyzeExperimentResults(results);
        return experiment_utils::generateExperimentReport(experimentName, metrics, results);
    }
}

// Additional utility functions for enhanced brain operations
namespace nlm {
namespace enhanced {
    // Create a simple neural network visualization
    std::string visualizeNetwork(std::shared_ptr<Brain> brain, 
                               int maxRegions = 3,
                               int maxNeuronsPerRegion = 20);
    
    // Analyze plasticity and learning dynamics
    std::string analyzePlasticity(std::shared_ptr<Brain> brain);
    
    // Compare multiple configuration states
    std::string compareConfigStates(const std::vector<std::pair<std::string, std::shared_ptr<Config>>>& configs);
    
    // Generate comprehensive experiment report
    std::string generateComprehensiveReport(const std::vector<std::pair<std::string, std::map<std::string, double>>>& experimentResults);
    
    // Validate brain state integrity
    bool validateBrainIntegrity(std::shared_ptr<Brain> brain);
    
    // Check for common brain issues
    std::vector<std::string> detectBrainIssues(std::shared_ptr<Brain> brain);
    
    // Create checkpoint with description and metadata
    bool createCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath,
                         const std::string& description = "",
                         const std::map<std::string, std::string>& metadata = {});
    
    // Restore from checkpoint with version checking
    bool restoreFromCheckpoint(std::shared_ptr<Brain> brain, const std::string& filepath,
                              bool autoRollback = true);
    
    // Get brain health score (0.0 to 1.0)
    double getBrainHealthScore(std::shared_ptr<Brain> brain);
    
    // Export brain state to various formats
    std::string exportToJSON(std::shared_ptr<Brain> brain);
    std::string exportToCSV(std::shared_ptr<Brain> brain);
    std::string exportToText(std::shared_ptr<Brain> brain);
}}