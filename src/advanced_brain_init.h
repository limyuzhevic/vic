// ADVANCED NLM BRAIN INITIALIZATION AND NEURON MANAGEMENT HEADER

#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <random>
#include <numeric>
#include <cmath>
#include <unordered_map>

// Forward declarations
namespace nlm {
    class PerformanceMonitor;
    class LearningExperiment;
    class EnhancedBrainInitializer;
}

using namespace nlm;

// Enhanced Brain initialization with validation and status tracking
class EnhancedBrainInitializer {
public:
    struct InitializationConfig {
        bool validateNeuronCount = true;
        bool validateConnectivity = true;
        bool enablePopulationAssignment = true;
        bool enableTypeBalancing = true;
        bool enableDevelopmentScheduling = true;
        size_t populationSizeFactor = 2;  // Neurons per population
        float typeBalance[6] = {0.3f, 0.2f, 0.15f, 0.15f, 0.1f, 0.1f};  // Neuron type distribution
    };
    
    struct InitializationResult {
        bool success;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        size_t neuronsCreated;
        size_t regionsCreated;
        size_t populationsCreated;
        float initializationTime;
        std::map<std::string, size_t> neuronTypeCounts;
        size_t connectionsCreated;
        bool stdpEnabled;
        bool memorySystemsEnabled;
        bool neuromodulationEnabled;
        bool developmentEnabled;
        bool checkpointingEnabled;
    };
    
    EnhancedBrainInitializer(std::shared_ptr<Config> config, const InitializationConfig& initConfig)
        : config(config), initConfig(initConfig) {}
    
    InitializationResult initializeBrain(std::shared_ptr<Brain> brain) {
        InitializationResult result;
        result.success = true;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Validate configuration before initialization
            validateConfiguration(brain, result);
            
            if (!result.errors.empty()) {
                result.success = false;
                return result;
            }
            
            // Create regions with optimal sizes
            createRegions(brain, result);
            
            // Create neurons with type balancing
            createBalancedNeuronPopulation(brain, result);
            
            // Setup connectivity patterns
            setupConnectivity(brain, result);
            
            // Initialize plasticity systems
            initializePlasticitySystems(brain, result);
            
            // Setup memory systems
            setupMemorySystems(brain, result);
            
            // Configure neuromodulation
            configureNeuromodulation(brain, result);
            
            // Initialize development system
            if (initConfig.enableDevelopmentScheduling) {
                initializeDevelopment(brain, result);
            }
            
            // Setup checkpoint system
            setupCheckpointing(brain, result);
            
        } catch (const std::exception& e) {
            result.errors.push_back(std::string("Exception during initialization: ") + e.what());
            result.success = false;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.initializationTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return result;
    }
    
private:
    void validateConfiguration(std::shared_ptr<Brain> brain, InitializationResult& result) {
        if (!config) {
            result.errors.push_back("Configuration is null");
            return;
        }
        
        // Validate neuron count ranges
        auto neuronCountOpt = config->get<int64_t>("neuron_count");
        if (neuronCountOpt) {
            if (*neuronCountOpt < 100) {
                result.warnings.push_back("Low neuron count (" + std::to_string(*neuronCountOpt) + ") may limit complexity");
            } else if (*neuronCountOpt > 10000) {
                result.warnings.push_back("High neuron count (" + std::to_string(*neuronCountOpt) + ") may impact performance");
            }
        }
        
        // Validate region count
        auto regionCountOpt = config->get<int64_t>("region_count");
        if (regionCountOpt && *regionCountOpt < 1) {
            result.errors.push_back("Region count must be at least 1");
        }
        
        // Validate connectivity probability
        auto connProbOpt = config->get<float>("connection_probability");
        if (connProbOpt) {
            if (*connProbOpt < 0.01) {
                result.warnings.push_back("Very low connectivity probability may result in isolated neurons");
            } else if (*connProbOpt > 0.5) {
                result.warnings.push_back("High connectivity probability may result in overly synchronized network");
            }
        }
        
        // Validate plasticity parameters
        auto stdpLtpOpt = config->get<float>("stdp_ltp_weight");
        auto stdpLtdOpt = config->get<float>("stdp_ltd_weight");
        if (stdpLtpOpt && stdpLtdOpt) {
            if (*stdpLtpOpt <= 0) {
                result.errors.push_back("STDP LTP weight must be positive");
            }
            if (*stdpLtdOpt <= 0) {
                result.errors.push_back("STDP LTD weight must be positive");
            }
        }
    }
    
    void createRegions(std::shared_ptr<Brain> brain, InitializationResult& result) {
        auto regionCountOpt = config->get<int64_t>("region_count");
        if (!regionCountOpt) return;
        
        size_t regionCount = static_cast<size_t>(*regionCountOpt);
        
        // Clear existing regions
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        brainImpl->regions.clear();
        
        // Create regions with balanced sizes
        size_t neuronsPerRegion = 0;
        auto neuronCountOpt = config->get<int64_t>("neuron_count");
        if (neuronCountOpt && regionCount > 0) {
            neuronsPerRegion = static_cast<size_t>(*neuronCountOpt) / regionCount;
        }
        
        for (size_t i = 0; i < regionCount; ++i) {
            RegionId regionId = brain->addRegion(std::string("Region_") + std::to_string(i + 1));
            auto* region = brain->getRegion(regionId);
            if (!region) {
                result.errors.push_back("Failed to create region " + std::to_string(i + 1));
                continue;
            }
            
            // Create neurons in this region
            size_t neuronsInRegion = neuronsPerRegion;
            if (i == regionCount - 1) {
                // Last region gets remaining neurons
                auto totalNeurons = neuronCountOpt ? *neuronCountOpt : 100;
                neuronsInRegion = totalNeurons - (neuronsPerRegion * (regionCount - 1));
            }
            
            for (size_t j = 0; j < neuronsInRegion; ++j) {
                NeuronId neuronId = region->addNeuron();
                auto* neuron = region->getNeuron(neuronId);
                if (!neuron) {
                    result.errors.push_back("Failed to create neuron in region " + std::to_string(i + 1) +
                                          ", neuron " + std::to_string(j + 1));
                    continue;
                }
                
                // Initialize neuron with random parameters
                auto rng = brain->getRandomGenerator();
                neuron->initializeRandom(*rng);
                
                // Assign population (optional)
                if (initConfig.enablePopulationAssignment && neuronsPerRegion > 10) {
                    PopulationId popId = region->addPopulation();
                    neuron->setPopulationId(popId);
                }
            }
            
            result.regionsCreated++;
            result.neuronsCreated += neuronsInRegion;
        }
    }
    
    void createBalancedNeuronPopulation(std::shared_ptr<Brain> brain, InitializationResult& result) {
        if (!initConfig.enableTypeBalancing) return;
        
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        // Create neurons for each type according to balanced distribution
        std::map<NeuronType, size_t> typeCounts;
        auto neuronCountOpt = config->get<int64_t>("neuron_count");
        if (!neuronCountOpt) return;
        
        size_t totalNeurons = static_cast<size_t>(*neuronCountOpt);
        size_t distributedCount = 0;
        
        for (int i = 0; i < 6; ++i) {
            NeuronType type = static_cast<NeuronType>(i);
            float ratio = initConfig.typeBalance[i];
            size_t count = static_cast<size_t>(totalNeurons * ratio);
            typeCounts[type] = count;
            distributedCount += count;
        }
        
        // Distribute remaining neurons proportionally
        size_t remaining = totalNeurons - distributedCount;
        if (remaining > 0) {
            for (auto& [type, count] : typeCounts) {
                size_t extra = static_cast<size_t>((static_cast<float>(remaining) * 
                                                  (initConfig.typeBalance[type] / 1.0f)));
                typeCounts[type] += extra;
                remaining -= extra;
                if (remaining == 0) break;
            }
        }
        
        // Assign types to neurons
        for (auto& region : brainImpl->regions) {
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) continue;
            
            // Distribute neuron types across regions
            size_t neuronsProcessed = 0;
            for (auto& [type, typeCount] : typeCounts) {
                if (typeCount <= 0) continue;
                
                size_t availableNeurons = neurons.size() - neuronsProcessed;
                size_t neuronsToAssign = std::min(typeCount, availableNeurons);
                
                for (size_t i = 0; i < neuronsToAssign; ++i) {
                    neurons[neuronsProcessed + i]->setType(type);
                }
                
                neuronsProcessed += neuronsToAssign;
                typeCounts[type] -= neuronsToAssign;
                
                if (neuronsProcessed >= neurons.size()) break;
            }
            
            // Set remaining neurons to internal type (default)
            while (neuronsProcessed < neurons.size()) {
                neurons[neuronsProcessed]->setType(NeuronType::Internal);
                neuronsProcessed++;
            }
            
            // Record type counts for this region
            for (int i = 0; i < 6; ++i) {
                NeuronType type = static_cast<NeuronType>(i);
                size_t count = 0;
                for (auto* neuron : neurons) {
                    if (neuron->getType() == type) count++;
                }
                result.neuronTypeCounts[std::to_string(type)] += count;
            }
        }
    }
    
    void setupConnectivity(std::shared_ptr<Brain> brain, InitializationResult& result) {
        if (!initConfig.validateConnectivity) return;
        
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        // Create balanced connectivity patterns
        size_t totalConnections = 0;
        
        for (auto& region : brainImpl->regions) {
            auto neurons = region->getAllNeurons();
            size_t regionConnections = 0;
            
            for (size_t i = 0; i < neurons.size(); ++i) {
                for (size_t j = 0; j < neurons.size(); ++j) {
                    if (i == j) continue;  // No self-connections
                    
                    float connectionProbability = 0.1f;  // Default
                    auto connProbOpt = config->get<float>("connection_probability");
                    if (connProbOpt) {
                        connectionProbability = *connProbOpt;
                    }
                    
                    if (rand() / (RAND_MAX + 1.0) < connectionProbability) {
                        region->addSynapse(neurons[i]->getId(), neurons[j]->getId());
                        regionConnections++;
                        
                        // Enable appropriate plasticity
                        auto* syn = region->getSynapse(neurons[i]->getId(), neurons[j]->getId());
                        if (syn) {
                            bool isExcitatory = (neurons[i]->getType() == NeuronType::Excitatory && 
                                                neurons[j]->getType() == NeuronType::Excitatory);
                            syn->setType(isExcitatory ? SynapseType::Excitatory : SynapseType::Inhibitory);
                        }
                    }
                }
            }
            
            totalConnections += regionConnections;
        }
        
        result.connectionsCreated = totalConnections;
    }
    
    void initializePlasticitySystems(std::shared_ptr<Brain> brain, InitializationResult& result) {
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        // Enable plasticity for all synapses
        for (auto& region : brainImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                // Enable all plasticity mechanisms
                syn->enablePlasticity(true, true, true);
                
                // Configure based on synapse type
                if (syn->isExcitatory()) {
                    syn->setWeight(0.1f + static_cast<float>(rand()) / RAND_MAX * 0.3f);
                } else {
                    syn->setWeight(-(0.1f + static_cast<float>(rand()) / RAND_MAX * 0.3f));
                }
            }
        }
        
        // Initialize STDP parameters from config
        auto stdpLtpOpt = config->get<float>("stdp_ltp_weight");
        auto stdpLtdOpt = config->get<float>("stdp_ltd_weight");
        auto stdpTimeOpt = config->get<float>("stdp_time_constant");
        
        if (brainImpl->stdp) {
            if (stdpLtpOpt) brainImpl->stdp->setLTPWeight(*stdpLtpOpt);
            if (stdpLtdOpt) brainImpl->stdp->setLTDWeight(*stdpLtdOpt);
            if (stdpTimeOpt) brainImpl->stdp->setTimeConstant(*stdpTimeOpt);
        }
        
        result.stdpEnabled = true;
    }
    
    void setupMemorySystems(std::shared_ptr<Brain> brain, InitializationResult& result) {
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        // Setup working memory capacity based on neuron count
        auto neuronCountOpt = config->get<int64_t>("neuron_count");
        if (brainImpl->workingMemory && neuronCountOpt) {
            size_t capacity = std::min(static_cast<size_t>(*neuronCountOpt) / 10, size_t(100));
            brainImpl->workingMemory->setCapacity(capacity);
        }
        
        // Setup episodic memory with appropriate capacity
        if (brainImpl->episodicMemory && neuronCountOpt) {
            size_t capacity = std::min(static_cast<size_t>(*neuronCountOpt) / 20, size_t(1000));
            brainImpl->episodicMemory->setCapacity(capacity);
        }
        
        result.memorySystemsEnabled = true;
    }
    
    void configureNeuromodulation(std::shared_ptr<Brain> brain, InitializationResult& result) {
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        // Configure neuromodulation parameters
        auto dopamineBaselineOpt = config->get<float>("dopamine_baseline");
        auto curiosityBaselineOpt = config->get<float>("curiosity_baseline");
        
        if (brainImpl->dopamine && dopamineBaselineOpt) {
            brainImpl->dopamine->setBaselineLevel(*dopamineBaselineOpt);
        }
        
        if (brainImpl->curiosity && curiosityBaselineOpt) {
            brainImpl->curiosity->setBaselineLevel(*curiosityBaselineOpt);
        }
        
        // Enable all neuromodulation subsystems
        if (brainImpl->novelty) {
            brainImpl->novelty->enable(true);
        }
        
        if (brainImpl->predictionError) {
            brainImpl->predictionError->enable(true);
        }
        
        result.neuromodulationEnabled = true;
    }
    
    void initializeDevelopment(std::shared_ptr<Brain> brain, InitializationResult& result) {
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        if (!brainImpl->developmentSystem) return;
        
        // Set initial developmental stage
        brainImpl->developmentSystem->setCurrentStage(DevelopmentalStage::Initial);
        
        // Configure developmental parameters
        auto synaptogenesisRateOpt = config->get<float>("synaptogenesis_rate");
        auto pruningRateOpt = config->get<float>("pruning_rate");
        
        if (synaptogenesisRateOpt) {
            brainImpl->developmentSystem->setSynaptogenesisRate(*synaptogenesisRateOpt);
        }
        
        if (pruningRateOpt) {
            brainImpl->developmentSystem->setPruningRate(*pruningRateOpt);
        }
        
        // Schedule developmental transitions
        brainImpl->developmentSystem->scheduleCriticalPeriod(1000);  // After 1000 steps
        
        result.developmentEnabled = true;
    }
    
    void setupCheckpointing(std::shared_ptr<Brain> brain, InitializationResult& result) {
        auto* brainImpl = static_cast<Brain::Impl*>(brain->pImpl);
        
        // Enable checkpointing for long-running simulations
        auto checkpointIntervalOpt = config->get<int64_t>("max_simulation_steps");
        if (checkpointIntervalOpt && *checkpointIntervalOpt > 10000) {
            if (brainImpl->checkpointManager) {
                brainImpl->checkpointManager->setCheckpointInterval(
                    std::min(static_cast<size_t>(*checkpointIntervalOpt) / 10, size_t(1000)));
                brainImpl->checkpointManager->enableAutoSave(true);
                result.checkpointingEnabled = true;
            }
        }
    }
    
    // Helper functions
    template<typename T, typename U>
    static bool contains(const std::map<T, U>& map, const T& key) {
        return map.find(key) != map.end();
    }
    
    std::shared_ptr<Config> config;
    InitializationConfig initConfig;
};