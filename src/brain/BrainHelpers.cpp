// Brain Helper Functions Implementation
// These helper functions break down the large Brain.cpp into more manageable pieces

#include "Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <set>

namespace nlm {
namespace brain_helpers {

// Initialize neuron population and synaptic connections
bool initializeNeuronsAndSynapses(Brain::Impl& impl) {
    impl.neuronById.clear();
    impl.sensoryNeurons.clear();
    impl.motorNeurons.clear();
    
    // Get configuration parameters
    size_t regionCount = impl.config->getOr<int64_t>("region_count", 1);
    size_t totalNeurons = impl.config->getOr<int64_t>("neuron_count", 100);
    size_t neuronsPerRegion = totalNeurons / regionCount;
    float connectionProbability = impl.config->getOr<float>("connection_probability", 0.1f);
    
    // Create neurons in regions
    for (size_t regionIdx = 0; regionIdx < regionCount; ++regionIdx) {
        RegionId regionId(regionIdx + 1);
        auto region = std::make_unique<NeuralRegion>(regionId, "Region " + std::to_string(regionId.index()));
        impl.regions.push_back(std::move(region));
        
        // Create neurons in this region
        auto& regionPtr = impl.regions.back();
        
        // Create different types of neurons
        size_t sensoryNeurons = static_cast<size_t>(neuronsPerRegion * 0.1f);
        size_t motorNeurons = static_cast<size_t>(neuronsPerRegion * 0.1f);
        size_t internalNeurons = neuronsPerRegion - sensoryNeurons - motorNeurons;
        
        // Create sensory neurons
        for (size_t i = 0; i < sensoryNeurons; ++i) {
            NeuronId neuronId(totalNeurons * regionIdx + i);
            auto neuron = std::make_unique<Neuron>(neuronId);
            neuron->setType(NeuronType::Sensory);
            neuron->setRegionId(regionId);
            neuron->setPopulationId(PopulationId(0)); // Sensory population
            neuron->initializeRandom(*impl.rng);
            
            // Add to region
            regionPtr->addNeuron(std::move(neuron));
            
            // Add to lookup table
            impl.neuronById[neuronId.value] = regionPtr->getNeurons().back().get();
            
            // Add to sensory list
            impl.sensoryNeurons.push_back(impl.neuronById[neuronId.value]);
        }
        
        // Create motor neurons
        for (size_t i = 0; i < motorNeurons; ++i) {
            NeuronId neuronId(totalNeurons * regionIdx + sensoryNeurons + i);
            auto neuron = std::make_unique<Neuron>(neuronId);
            neuron->setType(NeuronType::Motor);
            neuron->setRegionId(regionId);
            neuron->setPopulationId(PopulationId(1)); // Motor population
            neuron->initializeRandom(*impl.rng);
            
            // Add to region
            regionPtr->addNeuron(std::move(neuron));
            
            // Add to lookup table
            impl.neuronById[neuronId.value] = regionPtr->getNeurons().back().get();
            
            // Add to motor list
            impl.motorNeurons.push_back(impl.neuronById[neuronId.value]);
        }
        
        // Create internal neurons (excitatory, inhibitory, modulatory)
        for (size_t i = 0; i < internalNeurons; ++i) {
            NeuronId neuronId(totalNeurons * regionIdx + 
                             sensoryNeurons + motorNeurons + i);
            auto neuron = std::make_unique<Neuron>(neuronId);
            neuron->setType(static_cast<NeuronType>(i % 3)); // Cycle through types
            neuron->setRegionId(regionId);
            neuron->setPopulationId(PopulationId(2)); // Internal population
            neuron->initializeRandom(*impl.rng);
            
            // Add to region
            regionPtr->addNeuron(std::move(neuron));
            
            // Add to lookup table
            impl.neuronById[neuronId.value] = regionPtr->getNeurons().back().get();
        }
    }
    
    // Create synapses between neurons
    createRandomConnectivity(impl, connectionProbability);
    
    return true;
}

// Create random connectivity between neurons
void createRandomConnectivity(Brain::Impl& impl, float connectionProbability) {
    for (auto& region : impl.regions) {
        auto& neurons = region->getNeurons();
        
        // Create connections between neurons in the same region
        for (size_t preIdx = 0; preIdx < neurons.size(); ++preIdx) {
            for (size_t postIdx = 0; postIdx < neurons.size(); ++postIdx) {
                if (preIdx == postIdx) continue; // No self-connections
                
                if (impl.rng->nextFloat() < connectionProbability) {
                    SynapseId synId(preIdx * 100 + postIdx);
                    
                    auto synapse = std::make_unique<Synapse>(synId,
                        neurons[preIdx].get(), neurons[postIdx].get());
                    
                    // Set random initial weight
                    float weight = impl.rng->nextFloat() * 2.0f - 1.0f; // -1.0 to 1.0
                    synapse->setWeight(weight);
                    
                    // Enable appropriate plasticity
                    synapse->enablePlasticity(true, true, false); // Hebbian and STDP
                    
                    // Add synapse to both neurons
                    neurons[preIdx]->addOutgoingSynapse(synId);
                    neurons[postIdx]->addIncomingSynapse(synId);
                    
                    // Add to region
                    region->addSynapse(std::move(synapse));
                }
            }
        }
    }
}

// Register spike handlers for event-driven computation
void registerSpikeHandlers(Brain::Impl& impl) {
    // Register neurons with spike system
    for (auto& region : impl.regions) {
        for (auto& neuron : region->getNeurons()) {
            impl.spikeSystem->registerNeuron(neuron.get());
        }
    }
    
    impl.spikeHandlersRegistered = true;
}

// Update spike propagation system
void updateSpikeSystem(Brain::Impl& impl, SimulationStep step, Timestamp time) {
    // Process pending spike events
    auto events = impl.spikeSystem->processEvents(time);
    
    // Apply spike effects to postsynaptic neurons
    for (const auto& event : events) {
        // Find postsynaptic neuron
        auto it = impl.neuronById.find(event.destinationNeuron.value);
        if (it != impl.neuronById.end()) {
            // Apply synaptic input
            if (event.isExcitatory) {
                it->second->receiveExcitatoryInput(event.weight);
            } else {
                it->second->receiveInhibitoryInput(event.weight);
            }
            
            // Record spike in synapse history for STDP
            if (auto* synapse = findSynapse(impl, event.synapseId)) {
                synapse->recordPostSpike(time);
            }
            
            // Increment spike counter
            impl.totalSpikesThisStep++;
        }
    }
    
    // Let spike system handle neuron spiking
    impl.spikeSystem->update(step, time);
    
    // Record pre-synaptic spikes for STDP
    for (auto& region : impl.regions) {
        for (auto& synapse : region->getSynapses()) {
            synapse->recordPreSpike(time);
        }
    }
}

// Apply neuromodulatory signals to neural populations
void applyNeuromodulationToNeurons(Brain::Impl& impl) {
    // Apply dopamine to excitability
    if (impl.dopamine) {
        float dopamineLevel = impl.dopamine->getLevel();
        float excitabilityMod = dopamineLevel * 0.5f;
        
        if (excitabilityMod > 0.0f) {
            for (auto* neuron : impl.sensoryNeurons) {
                neuron->injectCurrent(excitabilityMod);
            }
        }
    }
    
    // Apply curiosity to exploration drive
    if (impl.curiosity) {
        float curiosityLevel = impl.curiosity->getLevel();
        if (curiosityLevel > 0.5f) {
            // Enhance sensory processing
            for (auto* neuron : impl.sensoryNeurons) {
                neuron->injectCurrent(10.0f * (curiosityLevel - 0.5f) * 2.0f);
            }
        }
    }
}

// Apply plasticity rules to synaptic connections
void applyPlasticityRules(Brain::Impl& impl, float plasticityMod, Timestamp time) {
    for (auto& region : impl.regions) {
        for (auto& synapse : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (synapse->getPlasticityFlags().stdp) {
                const auto& preSpikes = synapse->getPreSpikeHistory();
                const auto& postSpikes = synapse->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    impl.stdp->update(synapse.get(), preSpikes, postSpikes, impl.timestep);
                    float weight = synapse->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    synapse->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (synapse->getPlasticityFlags().hebbian) {
                const auto& preSpikes = synapse->getPreSpikeHistory();
                const auto& postSpikes = synapse->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    impl.hebbian->update(synapse.get(), preSpikes, postSpikes, impl.timestep);
                }
            }
            
            // Update synapse state
            synapse->step(time);
        }
    }
}

// Update homeostatically with synaptic scaling
void applySynapticScaling(Brain::Impl& impl, SimulationStep step) {
    // Every 100 steps, cap each neuron's total incoming excitatory weight
    if (step > 0 && step % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        
        for (auto& region : impl.regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }
                
                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
        }
    }
}

// Update episodic memory with recent neural activity
void updateEpisodicMemory(Brain::Impl& impl, SimulationStep step, float reward) {
    impl.stepsSinceLastEpisode++;
    if (impl.stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        impl.stepsSinceLastEpisode = 0;
        
        if (impl.episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = reward;
            
            // Store active neurons
            for (auto& region : impl.regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                        }
                    }
                }
            }
            
            impl.episodicMemory->storeEpisode(episode);
        }
    }
}

// Create motor action from current neural state
std::unique_ptr<Action> createMotorAction(const Brain::Impl& impl) {
    // Simple motor action generation based on motor neuron activity
    if (impl.motorNeurons.empty()) {
        return std::make_unique<Action>(MotorCommand::Wait);
    }
    
    // Find most active motor neuron
    float maxActivity = 0.0f;
    Neuron* mostActive = nullptr;
    
    for (auto* neuron : impl.motorNeurons) {
        if (neuron->getFiringRate() > maxActivity) {
            maxActivity = neuron->getFiringRate();
            mostActive = neuron;
        }
    }
    
    if (!mostActive || maxActivity < 0.1f) {
        return std::make_unique<Action>(MotorCommand::Wait);
    }
    
    // Determine motor command based on neuron type and activity
    MotorCommand command = MotorCommand::Wait;
    
    if (mostActive->getType() == NeuronType::Motor) {
        command = MotorCommand::MoveForward;
    } else if (mostActive->getType() == NeuronType::Sensory) {
        command = MotorCommand::Interact;
    }
    
    return std::make_unique<Action>(command);
}

// Save brain state to checkpoint file
bool saveBrainState(const Brain::Impl& impl, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for saving: " + filepath);
        return false;
    }
    
    // Write checkpoint header
    constexpr char HEADER[] = "NLM_CHECKPOINT_v1";
    file.write(HEADER, sizeof(HEADER));
    
    // Serialize brain state
    // For now, write a simple representation
    uint64_t regionCount = impl.regions.size();
    file.write(reinterpret_cast<const char*>(&regionCount), sizeof(regionCount));
    
    // Write configuration
    impl.config->saveToFile(filepath + ".config");
    
    // Write neural state summary
    size_t totalNeurons = impl.getTotalNeuronCount();
    size_t totalSynapses = impl.getTotalSynapseCount();
    file.write(reinterpret_cast<const char*>(&totalNeurons), sizeof(totalNeurons));
    file.write(reinterpret_cast<const char*>(&totalSynapses), sizeof(totalSynapses));
    
    NLM_LOG_INFO("Saved brain state to: " + filepath);
    return true;
}

// Load brain state from checkpoint file
bool loadBrainState(Brain::Impl& impl, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for loading: " + filepath);
        return false;
    }
    
    // Read checkpoint header
    char header[16];
    file.read(header, sizeof(header));
    if (std::string(header, sizeof(header)) != "NLM_CHECKPOINT_v1") {
        NLM_LOG_ERROR("Invalid checkpoint file format");
        return false;
    }
    
    // Deserialize brain state
    uint64_t regionCount;
    file.read(reinterpret_cast<char*>(&regionCount), sizeof(regionCount));
    
    // For now, just log that loading is not fully implemented
    NLM_LOG_WARNING("Checkpoint loading is a stub implementation - full loading requires additional work");
    
    return true;
}

// Find synapse by ID
Synapse* findSynapse(Brain::Impl& impl, SynapseId synId) {
    for (auto& region : impl.regions) {
        for (auto& synapse : region->getSynapses()) {
            if (synapse->getId() == synId) {
                return synapse.get();
            }
        }
    }
    return nullptr;
}

} // namespace brain_helpers

} // namespace nlm
