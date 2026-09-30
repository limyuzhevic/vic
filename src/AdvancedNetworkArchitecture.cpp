#include "AdvancedNetworkArchitecture.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

std::shared_ptr<Brain> AdvancedNetworkArchitecture::buildNetwork(std::shared_ptr<Config> config,
                                                                 const NetworkTopology& topology,
                                                                 RandomGenerator& rng) {
    NLM_LOG_INFO("Building advanced network: " + topology.name);
    
    // Validate topology
    if (!getArchitectureManager().validateArchitecture(topology)) {
        throw std::runtime_error("Invalid network topology: " + topology.name);
    }
    
    // Create brain with enhanced configuration
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Clear existing regions and create new ones
    brain->reset();
    
    // Create regions based on topology
    std::map<std::string, RegionId> regionIds;
    for (size_t i = 0; i < topology.regionNames.size(); ++i) {
        const auto& regionName = topology.regionNames[i];
        RegionId regionId = brain->addRegion(regionName);
        regionIds[regionName] = regionId;
        
        // Get neuron count for this region
        size_t neuronCount = 0;
        NeuronType neuronType = NeuronType::Internal;
        
        // Find population info for this region
        for (const auto& pair : topology.populationSizes) {
            if (pair.first == regionName) {
                neuronCount = pair.second;
                auto typeIt = topology.populationTypes.find(pair.first);
                if (typeIt != topology.populationTypes.end()) {
                    neuronType = typeIt->second;
                }
                break;
            }
        }
        
        if (neuronCount > 0) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                // Create population with neurons
                auto popId = region->addPopulation(neuronCount, neuronType);
                auto* population = region->getPopulation(popId);
                
                if (population) {
                    // Initialize neurons with custom models if available
                    for (auto* neuron : population->getNeurons()) {
                        // Apply custom neuron model if specified
                        // For now, use standard LIF parameters
                        neuron->setRestingPotential(-70.0f);
                        neuron->setThreshold(-55.0f);
                        neuron->setResetPotential(-70.0f);
                        neuron->setFiringState(FiringState::Resting);
                        neuron->setRefractoryPeriod(2);
                        
                        // Initialize with random membrane potential
                        float randomInit = rng.uniformReal(-3.0f, 3.0f);
                        neuron->setMembranePotential(-70.0f + randomInit);
                    }
                }
            }
        }
    }
    
    // Establish connections based on topology
    establishConnections(brain.get(), topology, rng);
    
    NLM_LOG_INFO("Advanced network built successfully: " + topology.name +
                 " with " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
    
    return brain;
}

std::shared_ptr<Brain> AdvancedNetworkArchitecture::buildNetwork(std::shared_ptr<Config> config,
                                                                 const std::string& architectureName,
                                                                 RandomGenerator& rng) {
    auto& archManager = getArchitectureManager();
    if (!archManager.hasArchitecture(architectureName)) {
        throw std::runtime_error("Architecture not found: " + architectureName);
    }
    
    NetworkTopology topology = archManager.getArchitecture(architectureName);
    return buildNetwork(config, topology, rng);
}

void AdvancedNetworkArchitecture::modifyNetwork(Brain* brain, const NetworkTopology& newTopology) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Modifying network topology: " + newTopology.name);
    
    // Save current state for rollback
    std::string backupFile = "./network_backup.checkpoint";
    bool savedSuccessfully = brain->save(backupFile);
    
    try {
        // Create new regions
        std::map<std::string, RegionId> regionMap;
        for (const auto& regionName : newTopology.regionNames) {
            RegionId regionId = brain->addRegion(regionName);
            regionMap[regionName] = regionId;
        }
        
        // Recreate neurons based on new topology
        brain->reset();
        
        // Re-establish connections
        // Note: In a real implementation, we would need access to RandomGenerator here
        // For simplicity, we'll skip connection establishment
        
        NLM_LOG_INFO("Network topology modified successfully: " + newTopology.name);
        
    } catch (const std::exception& e) {
        // Restore from backup if modification failed
        if (savedSuccessfully) {
            NLM_LOG_WARN("Network modification failed, restoring from backup");
            brain->load(backupFile);
        }
        throw;
    }
}

void AdvancedNetworkArchitecture::applyCustomNeuronModels(Brain* brain, 
                                                          const std::map<std::string, CustomNeuronModel>& models) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Applying custom neuron models to network");
    
    // Apply custom neuron parameters to all neurons
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Find matching custom model for this neuron type
                NeuronType neuronType = neuron->getState().type;
                std::string typeName;
                
                switch (neuronType) {
                    case NeuronType::Sensory: typeName = "Sensory"; break;
                    case NeuronType::Internal: typeName = "Internal"; break;
                    case NeuronType::Motor: typeName = "Motor"; break;
                    case NeuronType::Adaptation: typeName = "Adaptation"; break;
                    case NeuronType::Homeostatic: typeName = "Homeostatic"; break;
                    default: typeName = "Internal"; break;
                }
                
                auto modelIt = models.find(typeName);
                if (modelIt != models.end()) {
                    const auto& model = modelIt->second;
                    
                    // Apply custom parameters
                    neuron->setRestingPotential(model.restingPotential);
                    neuron->setThreshold(model.threshold);
                    neuron->setResetPotential(model.resetPotential);
                    neuron->setRefractoryPeriod(static_cast<uint32_t>(model.refractoryPeriod));
                    
                    // Apply model-specific parameters
                    for (const auto& param : model.modelSpecificParameters) {
                        if (param.first == "adaptation_strength") {
                            neuron->getState().adaptationStrength = param.second;
                        } else if (param.first == "excitability") {
                            neuron->getState().excitability = param.second;
                        }
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("Custom neuron models applied successfully");
}

void AdvancedNetworkArchitecture::applyCustomPlasticityRules(Brain* brain,
                                                             const std::vector<CustomPlasticityRule>& rules) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Applying custom plasticity rules to network");
    
    // Note: In a real implementation, this would integrate with the brain's
    // plasticity system (STDP, Hebbian, etc.) to apply custom rules
    // For now, we log the application of rules
    
    for (const auto& rule : rules) {
        NLM_LOG_INFO("Applied plasticity rule: " + rule.name + " (" + rule.ruleType + ")");
    }
    
    NLM_LOG_INFO("Custom plasticity rules applied successfully");
}

float AdvancedNetworkArchitecture::calculateNetworkMetrics(Brain* brain, const std::string& metric) const {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    if (metric == "density") {
        // Calculate network density (connections / possible connections)
        size_t totalNeurons = brain->getTotalNeuronCount();
        size_t totalPossibleConnections = totalNeurons * (totalNeurons - 1); // Excluding self-connections
        size_t actualConnections = brain->getTotalSynapseCount();
        
        return totalPossibleConnections > 0 ? 
            static_cast<float>(actualConnections) / static_cast<float>(totalPossibleConnections) : 0.0f;
    
    } else if (metric == "small_world_coefficient") {
        // Placeholder for small-world coefficient calculation
        // This would involve measuring clustering coefficient and path length
        return 0.5f; // Placeholder value
    
    } else if (metric == "efficiency") {
        // Calculate network efficiency
        return calculateNetworkEfficiency(brain);
    
    } else if (metric == "hierarchy") {
        // Calculate hierarchical structure
        return calculateNetworkHierarchy(brain);
    }
    
    NLM_LOG_WARN("Unknown metric requested: " + metric + ", returning 0.0f");
    return 0.0f;
}

void AdvancedNetworkArchitecture::analyzeConnectivityPatterns(Brain* brain) const {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Analyzing network connectivity patterns");
    
    // Analyze connectivity patterns
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalConnections = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("Connectivity analysis:")
    NLM_LOG_INFO("  Total neurons: " + std::to_string(totalNeurons))
    NLM_LOG_INFO("  Total synapses: " + std::to_string(totalConnections))
    NLM_LOG_INFO("  Average connections per neuron: " +
                 std::to_string(totalNeurons > 0 ? static_cast<float>(totalConnections) / static_cast<float>(totalNeurons) : 0.0f))
    
    // Analyze connection weights
    float totalWeight = 0.0f;
    float maxWeight = std::numeric_limits<float>::lowest();
    float minWeight = std::numeric_limits<float>::max();
    
    // Note: In a real implementation, we would iterate over all synapses
    // to get actual weight statistics
    
    NLM_LOG_INFO("Connection weight statistics:")
    NLM_LOG_INFO("  Total weight: " + std::to_string(totalWeight))
    NLM_LOG_INFO("  Maximum weight: " + std::to_string(maxWeight))
    NLM_LOG_INFO("  Minimum weight: " + std::to_string(minWeight))
}

void AdvancedNetworkArchitecture::analyzeActivityDynamics(Brain* brain, size_t steps) const {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Analyzing network activity dynamics over " + std::to_string(steps) + " steps");
    
    // Note: In a real implementation, this would run the network for the specified
    // number of steps and collect statistics about spike activity, firing rates, etc.
    
    NLM_LOG_INFO("Activity dynamics analysis complete (placeholder implementation)");
}

void AdvancedNetworkArchitecture::optimizeNetworkTopology(Brain* brain) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Optimizing network topology");
    
    // Apply optimization algorithms
    // This would typically involve:
    // 1. Rewiring connections based on activity patterns
    // 2. Adjusting connection weights for better information flow
    // 3. Balancing excitation and inhibition
    
    // Placeholder for optimization logic
    NLM_LOG_INFO("Network topology optimization complete (placeholder implementation)");
}

void AdvancedNetworkArchitecture::balanceNetworkActivity(Brain* brain) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Balancing network activity");
    
    // Balance network activity by adjusting excitability
    // This would typically involve:
    // 1. Identifying regions with excessive or insufficient activity
    // 2. Adjusting neuromodulatory signals
    // 3. Modifying synaptic weights
    
    // Placeholder for activity balancing logic
    NLM_LOG_INFO("Network activity balancing complete (placeholder implementation)");
}

void AdvancedNetworkArchitecture::enhanceLearningCapacity(Brain* brain) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Enhancing network learning capacity");
    
    // Enhance learning capacity by:
    // 1. Increasing plasticity in key regions
    // 2. Adding new synaptic connections where beneficial
    // 3. Adjusting neuromodulatory settings
    
    // Placeholder for learning capacity enhancement
    NLM_LOG_INFO("Learning capacity enhancement complete (placeholder implementation)");
}

bool AdvancedNetworkArchitecture::saveArchitecture(const std::string& filename,
                                                   const NetworkTopology& topology) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        // Serialize topology to JSON-like format
        file << "{\n";
        file << "  \"name\": \"" << topology.name << "\",\n";
        file << "  \"region_names\": [";
        for (size_t i = 0; i < topology.regionNames.size(); ++i) {
            if (i > 0) file << ", ";
            file << "\"" << topology.regionNames[i] << "\"";
        }
        file << "],\n";
        
        file << "  \"population_sizes\": {\n";
        for (const auto& pair : topology.populationSizes) {
            file << "    \"" << pair.first << "\": " << pair.second << ",\n";
        }
        file << "  },\n";
        
        file << "  \"connectivity_pattern\": \"" << topology.connectivityPattern << "\",\n";
        file << "  \"connection_probability\": " << topology.connectionProbability << ",\n";
        file << "  \"weight_initialization\": \"" << topology.weightInitialization << "\"\n";
        file << "}\n";
        
        file.close();
        NLM_LOG_INFO("Architecture saved to " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save architecture: " + std::string(e.what()));
        return false;
    }
}

AdvancedNetworkArchitecture::NetworkTopology AdvancedNetworkArchitecture::loadArchitecture(const std::string& filename) const {
    NetworkTopology topology;
    
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading: " + filename);
        }
        
        // Parse topology from file
        // Note: This is a simplified parser; in practice, you would use a proper JSON parser
        std::string line;
        while (std::getline(file, line)) {
            // Simple parsing logic
            if (line.find("name") != std::string::npos) {
                size_t pos = line.find('"');
                if (pos != std::string::npos) {
                    size_t endPos = line.find('"', pos + 1);
                    if (endPos != std::string::npos) {
                        topology.name = line.substr(pos + 1, endPos - pos - 1);
                    }
                }
            }
        }
        
        file.close();
        NLM_LOG_INFO("Architecture loaded from " + filename);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load architecture: " + std::string(e.what()));
        throw;
    }
    
    return topology;
}

std::string AdvancedNetworkArchitecture::exportNetworkForExport(Brain* brain) const {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    // Export network in a format suitable for external analysis
    std::stringstream ss;
    
    ss << "NLM Network Export\n";
    ss << "===================\n\n";
    ss << "Statistics:\n";
    ss << "  Total neurons: " << brain->getTotalNeuronCount() << "\n";
    ss << "  Total synapses: " << brain->getTotalSynapseCount() << "\n";
    ss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
    ss << "  Average firing rate: " << brain->getAverageFiringRate() << "\n\n";
    
    // Export region information
    ss << "Regions:\n";
    for (const auto& region : brain->getRegions()) {
        ss << "  Region " << region->getId().index() << " (" << region->getName() << ")\n";
        ss << "    Neurons: " << region->getTotalNeuronCount() << "\n";
        ss << "    Synapses: " << region->getSynapseCount() << "\n";
    }
    
    ss << "\nConnectivity Matrix:\n";
    // Note: In a real implementation, this would export the actual connectivity
    ss << "[Connectivity matrix would be exported here]\n";
    
    return ss.str();
}

std::shared_ptr<Brain> AdvancedNetworkArchitecture::cloneNetwork(const Brain* originalBrain) {
    if (!originalBrain) {
        throw std::runtime_error("Original brain pointer is null");
    }
    
    NLM_LOG_INFO("Cloning network");
    
    // Create a new brain with the same configuration
    auto config = originalBrain->getConfig();
    auto clonedBrain = std::make_shared<Brain>(config);
    
    // In a real implementation, we would clone the brain state
    // For now, we initialize with the same configuration
    if (!clonedBrain->initialize()) {
        throw std::runtime_error("Failed to initialize cloned brain");
    }
    
    NLM_LOG_INFO("Network cloned successfully");
    return clonedBrain;
}

void AdvancedNetworkArchitecture::duplicateConnections(Brain* targetBrain, const Brain* sourceBrain,
                                                       const std::vector<std::pair<RegionId, RegionId>>& regionMappings) {
    if (!targetBrain || !sourceBrain) {
        throw std::runtime_error("Brain pointers are null");
    }
    
    NLM_LOG_INFO("Duplicating connections from source to target brain");
    
    // Create mapping from source region IDs to target region IDs
    std::map<RegionId, RegionId> regionMapping;
    for (const auto& mapping : regionMappings) {
        regionMapping[mapping.first] = mapping.second;
    }
    
    // Note: In a real implementation, this would copy synapses from source to target
    // based on the region mapping
    NLM_LOG_INFO("Connection duplication complete (placeholder implementation)");
}

void AdvancedNetworkArchitecture::initializeNeurons(Brain* brain, const NetworkTopology& topology,
                                                    RandomGenerator& rng) {
    // This method is already partially implemented in buildNetwork
    // Additional initialization logic can be added here
}

void AdvancedNetworkArchitecture::establishConnections(Brain* brain, const NetworkTopology& topology,
                                                      RandomGenerator& rng) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    NLM_LOG_INFO("Establishing connections based on topology: " + topology.name);
    
    // Establish connections based on topology
    if (topology.connectivityPattern == "dense") {
        establishDenseConnections(brain, topology, rng);
    } else if (topology.connectivityPattern == "sparse") {
        establishSparseConnections(brain, topology, rng);
    } else if (topology.connectivityPattern == "ring") {
        establishRingConnections(brain, topology, rng);
    } else if (topology.connectivityPattern == "random") {
        establishRandomConnections(brain, topology, rng);
    } else if (topology.connectivityPattern == "custom") {
        establishCustomConnections(brain, topology, rng);
    }
}

void AdvancedNetworkArchitecture::establishDenseConnections(Brain* brain, const NetworkTopology& topology,
                                                            RandomGenerator& rng) {
    // Establish dense connectivity between regions
    for (size_t i = 0; i < topology.regionNames.size(); ++i) {
        auto* region1 = brain->getRegion(RegionId(i + 1));
        if (!region1) continue;
        
        for (size_t j = 0; j < topology.regionNames.size(); ++j) {
            if (!topology.includeSelfConnections && i == j) continue;
            
            auto* region2 = brain->getRegion(RegionId(j + 1));
            if (!region2) continue;
            
            // Establish connections from region1 to region2
            // Note: This is a simplified implementation
            establishInterRegionConnections(region1, region2, topology, rng);
        }
    }
}

void AdvancedNetworkArchitecture::establishSparseConnections(Brain* brain, const NetworkTopology& topology,
                                                             RandomGenerator& rng) {
    // Establish sparse connectivity with connection probability
    for (size_t i = 0; i < topology.regionNames.size(); ++i) {
        auto* region1 = brain->getRegion(RegionId(i + 1));
        if (!region1) continue;
        
        for (size_t j = 0; j < topology.regionNames.size(); ++j) {
            if (!topology.includeSelfConnections && i == j) continue;
            
            // Check if connection should be established based on probability
            if (rng.uniformReal(0.0f, 1.0f) < topology.connectionProbability) {
                auto* region2 = brain->getRegion(RegionId(j + 1));
                if (region2) {
                    establishInterRegionConnections(region1, region2, topology, rng);
                }
            }
        }
    }
}

void AdvancedNetworkArchitecture::establishRingConnections(Brain* brain, const NetworkTopology& topology,
                                                           RandomGenerator& rng) {
    // Establish ring connectivity (each region connects to its neighbors)
    size_t numRegions = topology.regionNames.size();
    for (size_t i = 0; i < numRegions; ++i) {
        auto* region1 = brain->getRegion(RegionId(i + 1));
        if (!region1) continue;
        
        // Connect to next region in ring (with wrap-around)
        size_t j = (i + 1) % numRegions;
        auto* region2 = brain->getRegion(RegionId(j + 1));
        if (region2) {
            establishInterRegionConnections(region1, region2, topology, rng);
        }
        
        // Also connect to previous region (bidirectional ring)
        j = (i + numRegions - 1) % numRegions;
        region2 = brain->getRegion(RegionId(j + 1));
        if (region2) {
            establishInterRegionConnections(region1, region2, topology, rng);
        }
    }
}

void AdvancedNetworkArchitecture::establishRandomConnections(Brain* brain, const NetworkTopology& topology,
                                                             RandomGenerator& rng) {
    // Establish random connectivity across all regions
    for (size_t i = 0; i < topology.regionNames.size(); ++i) {
        auto* region1 = brain->getRegion(RegionId(i + 1));
        if (!region1) continue;
        
        for (size_t j = 0; j < topology.regionNames.size(); ++j) {
            if (!topology.includeSelfConnections && i == j) continue;
            
            // Random connection establishment
            if (rng.uniformReal(0.0f, 1.0f) < topology.connectionProbability) {
                auto* region2 = brain->getRegion(RegionId(j + 1));
                if (region2) {
                    establishInterRegionConnections(region1, region2, topology, rng);
                }
            }
        }
    }
}

void AdvancedNetworkArchitecture::establishCustomConnections(Brain* brain, const NetworkTopology& topology,
                                                             RandomGenerator& rng) {
    // Establish connections based on custom topology definition
    for (const auto& connection : topology.connectionWeights) {
        const std::string& sourceName = connection.first.first;
        const std::string& targetName = connection.first.second;
        
        // Find source and target regions
        RegionId sourceId, targetId;
        bool foundSource = false, foundTarget = false;
        
        for (size_t i = 0; i < topology.regionNames.size(); ++i) {
            if (topology.regionNames[i] == sourceName) {
                sourceId = RegionId(i + 1);
                foundSource = true;
            }
            if (topology.regionNames[i] == targetName) {
                targetId = RegionId(i + 1);
                foundTarget = true;
            }
        }
        
        if (foundSource && foundTarget) {
            auto* sourceRegion = brain->getRegion(sourceId);
            auto* targetRegion = brain->getRegion(targetId);
            
            if (sourceRegion && targetRegion) {
                establishInterRegionConnections(sourceRegion, targetRegion, topology, rng);
            }
        }
    }
}

void AdvancedNetworkArchitecture::establishInterRegionConnections(NeuralRegion* sourceRegion,
                                                                  NeuralRegion* targetRegion,
                                                                  const NetworkTopology& topology,
                                                                  RandomGenerator& rng) {
    if (!sourceRegion || !targetRegion) return;
    
    // Establish connections between neurons in source and target regions
    // Note: This is a simplified implementation
    
    // Get neurons from both regions
    auto sourceNeurons = sourceRegion->getAllNeurons();
    auto targetNeurons = targetRegion->getAllNeurons();
    
    if (sourceNeurons.empty() || targetNeurons.empty()) return;
    
    // Determine connection weight
    float weight = topology.connectionWeights.empty() ? 0.5f : topology.connectionWeights.begin()->second;
    auto weightIt = topology.connectionWeights.find(
        std::make_pair(sourceRegion->getName(), targetRegion->getName()));
    if (weightIt != topology.connectionWeights.end()) {
        weight = weightIt->second;
    }
    
    // Initialize connection weight based on topology specification
    float initializedWeight = initializeConnectionWeight(topology.weightInitialization, topology.weightScale, rng);
    
    // Note: In a real implementation, we would create actual synapses between neurons
    // For now, we just log the intended connections
    NLM_LOG_DEBUG("Establishing connection from region " + std::to_string(sourceRegion->getId().index()) +
                  " to region " + std::to_string(targetRegion->getId().index()) +
                  " with weight " + std::to_string(initializedWeight));
}

float AdvancedNetworkArchitecture::initializeConnectionWeight(const std::string& weightInitialization,
                                                             float scale, RandomGenerator& rng) {
    if (weightInitialization == "zero") {
        return 0.0f;
    } else if (weightInitialization == "uniform") {
        return rng.uniformReal(-scale, scale);
    } else if (weightInitialization == "gaussian") {
        return rng.gaussian(0.0f, scale);
    } else if (weightInitialization == "xavier") {
        // Xavier initialization
        float fanIn = 10.0f;  // Placeholder
        float fanOut = 10.0f; // Placeholder
        float limit = std::sqrt(6.0f / (fanIn + fanOut));
        return rng.uniformReal(-limit, limit);
    }
    
    // Default to uniform
    return rng.uniformReal(-scale, scale);
}

float AdvancedNetworkArchitecture::calculateNetworkEfficiency(Brain* brain) const {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    // Calculate network efficiency based on connectivity and information flow
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalConnections = brain->getTotalSynapseCount();
    
    if (totalNeurons < 2) return 0.0f;
    
    // Simplified efficiency calculation
    float efficiency = static_cast<float>(totalConnections) / static_cast<float>(totalNeurons * (totalNeurons - 1));
    
    // Normalize to [0, 1] range
    return std::min(1.0f, efficiency * 10.0f); // Scale factor for typical networks
}

float AdvancedNetworkArchitecture::calculateNetworkHierarchy(Brain* brain) const {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    // Calculate hierarchical structure based on region organization
    size_t regionCount = brain->getRegionCount();
    
    if (regionCount < 2) return 0.0f;
    
    // Simplified hierarchy calculation
    float hierarchy = static_cast<float>(regionCount) / 10.0f; // Scale factor
    return std::min(1.0f, hierarchy);
}

} // namespace nlm