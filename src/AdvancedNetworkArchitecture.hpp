#pragma once

#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <chrono>

namespace nlm {

/**
 * @brief Advanced network architecture builder for NLM
 * 
 * This class provides sophisticated network topology construction capabilities,
 * enabling experienced users to create custom neural network architectures
 * with precise control over connectivity patterns, neuron types, and regional organization.
 */
class AdvancedNetworkArchitecture {
public:
    /**
     * @brief Network topology definition
     */
    struct NetworkTopology {
        std::string name;
        std::vector<std::string> regionNames;
        std::map<std::string, size_t> populationSizes;
        std::map<std::string, NeuronType> populationTypes;
        std::map<std::pair<std::string, std::string>, float> connectionWeights;
        std::map<std::pair<std::string, std::string>, bool> connectionEnabled;
        std::string connectivityPattern;  // "dense", "sparse", "ring", "random", "custom"
        float connectionProbability;       // For random connectivity
        bool includeSelfConnections;       // Whether to include self-connections
        std::string weightInitialization; // "zero", "uniform", "gaussian", "xavier"
        float weightScale;
        
        NetworkTopology() : connectionProbability(0.1f), includeSelfConnections(false),
                          weightInitialization("uniform"), weightScale(1.0f) {}
    };

    /**
     * @brief Custom neuron model parameters
     */
    struct CustomNeuronModel {
        NeuronType type;
        std::string modelName;
        MembranePotential restingPotential;
        MembranePotential threshold;
        MembranePotential resetPotential;
        float membraneTimeConstant;
        float refractoryPeriod;
        float adaptationStrength;
        float excitability;
        std::map<std::string, float> modelSpecificParameters;
        
        CustomNeuronModel() : type(NeuronType::Internal), restingPotential(-70.0f),
                            threshold(-55.0f), resetPotential(-70.0f), 
                            membraneTimeConstant(20.0f), refractoryPeriod(2.0f),
                            adaptationStrength(0.0f), excitability(1.0f) {}
    };

    /**
     * @brief Custom plasticity rule definition
     */
    struct CustomPlasticityRule {
        std::string name;
        std::string ruleType;  // "stdp", "hebbian", "reward_modulated", "meta_learning"
        float learningRate;
        float baselineWeight;
        std::map<std::string, float> ruleParameters;
        std::function<float(const Synapse*, const Neuron*, const Neuron*, float)> weightUpdateFunction;
        
        CustomPlasticityRule() : learningRate(0.01f), baselineWeight(0.0f) {}
    };

    /**
     * @brief Network builder for complex architectures
     */
    class NetworkBuilder {
    public:
        NetworkBuilder() = default;
        ~NetworkBuilder() = default;
        
        // Builder pattern methods
        NetworkBuilder& setTopology(const NetworkTopology& topology) {
            topology_ = topology;
            return *this;
        }
        
        NetworkBuilder& addRegion(const std::string& name, size_t neuronCount, 
                                 NeuronType type = NeuronType::Internal) {
            topology_.regionNames.push_back(name);
            topology_.populationSizes[name] = neuronCount;
            topology_.populationTypes[name] = type;
            return *this;
        }
        
        NetworkBuilder& addConnection(const std::string& source, const std::string& target,
                                     float weight = 0.5f, bool enabled = true) {
            topology_.connectionWeights[{source, target}] = weight;
            topology_.connectionEnabled[{source, target}] = enabled;
            return *this;
        }
        
        NetworkBuilder& setConnectivityPattern(const std::string& pattern, float probability = 0.1f) {
            topology_.connectivityPattern = pattern;
            topology_.connectionProbability = probability;
            return *this;
        }
        
        NetworkBuilder& addCustomNeuronModel(const std::string& name, const CustomNeuronModel& model) {
            customModels_[name] = model;
            return *this;
        }
        
        NetworkBuilder& addCustomPlasticityRule(const CustomPlasticityRule& rule) {
            customPlasticityRules_.push_back(rule);
            return *this;
        }
        
        NetworkTopology getTopology() const { return topology_; }
        const std::map<std::string, CustomNeuronModel>& getCustomModels() const { return customModels_; }
        const std::vector<CustomPlasticityRule>& getCustomPlasticityRules() const { return customPlasticityRules_; }
        
    private:
        NetworkTopology topology_;
        std::map<std::string, CustomNeuronModel> customModels_;
        std::vector<CustomPlasticityRule> customPlasticityRules_;
    };

    /**
     * @brief Advanced network architecture manager
     */
    class ArchitectureManager {
    public:
        ArchitectureManager() = default;
        ~ArchitectureManager() = default;
        
        // Architecture registration and management
        void registerArchitecture(const std::string& name, const NetworkTopology& topology) {
            architectures_[name] = topology;
        }
        
        bool hasArchitecture(const std::string& name) const {
            return architectures_.find(name) != architectures_.end();
        }
        
        NetworkTopology getArchitecture(const std::string& name) const {
            if (!hasArchitecture(name)) {
                throw std::runtime_error("Architecture not found: " + name);
            }
            return architectures_.at(name);
        }
        
        std::vector<std::string> getArchitectureNames() const {
            std::vector<std::string> names;
            for (const auto& pair : architectures_) {
                names.push_back(pair.first);
            }
            return names;
        }
        
        // Architecture validation
        bool validateArchitecture(const NetworkTopology& topology) const {
            // Check for duplicate region names
            std::unordered_set<std::string> uniqueRegions;
            for (const auto& regionName : topology.regionNames) {
                if (uniqueRegions.find(regionName) != uniqueRegions.end()) {
                    return false; // Duplicate region name
                }
                uniqueRegions.insert(regionName);
            }
            
            // Check for valid connectivity patterns
            std::vector<std::string> validPatterns = {"dense", "sparse", "ring", "random", "custom"};
            if (std::find(validPatterns.begin(), validPatterns.end(), topology.connectivityPattern) == validPatterns.end()) {
                return false; // Invalid connectivity pattern
            }
            
            return true;
        }
        
        // Architecture comparison
        float calculateComplexityDifference(const NetworkTopology& topo1, const NetworkTopology& topo2) const {
            float complexity1 = calculateArchitectureComplexity(topo1);
            float complexity2 = calculateArchitectureComplexity(topo2);
            return std::abs(complexity1 - complexity2);
        }
        
    private:
        float calculateArchitectureComplexity(const NetworkTopology& topology) const {
            float complexity = 0.0f;
            
            // Add complexity from regions and populations
            complexity += static_cast<float>(topology.regionNames.size() * 10);
            complexity += static_cast<float>(topology.populationSizes.size() * 5);
            
            // Add complexity from connections
            complexity += static_cast<float>(topology.connectionWeights.size() * 2);
            
            // Add complexity from custom features
            if (topology.connectivityPattern != "dense") complexity *= 0.9f;
            
            return complexity;
        }
        
        std::map<std::string, NetworkTopology> architectures_;
    };

public:
    AdvancedNetworkArchitecture() = default;
    ~AdvancedNetworkArchitecture() = default;
    
    // Architecture building and management
    NetworkBuilder createBuilder() { return NetworkBuilder(); }
    ArchitectureManager& getArchitectureManager() { return architectureManager_; }
    
    // Network construction utilities
    std::shared_ptr<Brain> buildNetwork(std::shared_ptr<Config> config, 
                                       const NetworkTopology& topology,
                                       RandomGenerator& rng);
    
    std::shared_ptr<Brain> buildNetwork(std::shared_ptr<Config> config,
                                       const std::string& architectureName,
                                       RandomGenerator& rng);
    
    // Network modification utilities
    void modifyNetwork(Brain* brain, const NetworkTopology& newTopology);
    void applyCustomNeuronModels(Brain* brain, const std::map<std::string, CustomNeuronModel>& models);
    void applyCustomPlasticityRules(Brain* brain, const std::vector<CustomPlasticityRule>& rules);
    
    // Network analysis utilities
    float calculateNetworkMetrics(Brain* brain, const std::string& metric) const;
    void analyzeConnectivityPatterns(Brain* brain) const;
    void analyzeActivityDynamics(Brain* brain, size_t steps) const;
    
    // Network optimization utilities
    void optimizeNetworkTopology(Brain* brain);
    void balanceNetworkActivity(Brain* brain);
    void enhanceLearningCapacity(Brain* brain);
    
    // Serialization and export
    bool saveArchitecture(const std::string& filename, const NetworkTopology& topology) const;
    NetworkTopology loadArchitecture(const std::string& filename) const;
    std::string exportNetworkForExport(Brain* brain) const;
    
    // Network cloning and duplication
    std::shared_ptr<Brain> cloneNetwork(const Brain* originalBrain);
    void duplicateConnections(Brain* targetBrain, const Brain* sourceBrain,
                            const std::vector<std::pair<RegionId, RegionId>>& regionMappings);
    
protected:
    ArchitectureManager architectureManager_;
    
    // Helper functions
    void initializeNeurons(Brain* brain, const NetworkTopology& topology, RandomGenerator& rng);
    void establishConnections(Brain* brain, const NetworkTopology& topology, RandomGenerator& rng);
    NeuronType getNeuronTypeForPopulation(const std::string& populationName,
                                         const std::map<std::string, NeuronType>& populationTypes);
    float initializeConnectionWeight(const std::string& weightInitialization, float scale,
                                    RandomGenerator& rng);
};

} // namespace nlm