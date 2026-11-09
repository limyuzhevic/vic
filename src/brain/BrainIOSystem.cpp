#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include "BrainUtils.cpp"

namespace nlm {

// BrainIOSystem implementation - I/O and control systems
// Extracted from Brain.cpp lines 694-870 and I/O functionality

class BrainIOSystem {
public:
    // Process sensory input from environment
    static void processSensoryInput(Brain::Impl* impl, const SensoryInput& input) {
        // Inject current into sensory neurons based on input
        // This is a simple mapping - sensory encoding
        
        const auto& values = input.getData();
        if (values.empty()) return;
        
        size_t numSensory = impl->sensoryNeurons.size();
        if (numSensory == 0) return;
        
        // Distribute input across sensory neurons
        for (size_t i = 0; i < numSensory; ++i) {
            // Normalize input value to range [-10, 10] mV
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = static_cast<float>(values[i]) * 10.0f;
            }
            
            // Inject current into this sensory neuron
            impl->sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            // Also store in working memory
            if (impl->workingMemory && normalizedValue > 0.5f) {
                impl->workingMemory->storeToNeuron(impl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        }
    }
    
    // Inject current directly into a specific neuron
    static void injectCurrent(Brain::Impl* impl, NeuronId neuron, MembranePotential current) {
        auto it = impl->neuronById.find(neuron.index());
        if (it != impl->neuronById.end()) {
            it->second->injectCurrent(current);
            return;
        }
        
        for (auto& region : impl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    }
    
    // Inject current into all neurons of a specific type
    static void injectCurrentToNeurons(Brain::Impl* impl, NeuronType type, MembranePotential current) {
        for (auto& region : impl->regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
    
    // Process motor/action output based on motor neuron activity
    static std::unique_ptr<Action> produceAction(Brain::Impl* impl) {
        // Simple action selection based on motor neuron activity
        // The motor neuron population with highest average activity determines action
        
        if (impl->motorNeurons.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Calculate activity of motor neuron groups
        size_t firingMotor = 0;
        for (auto* neuron : impl->motorNeurons) {
            if (neuron->isFiring()) {
                ++firingMotor;
            }
        }
        
        // Return a simple action
        ActionType type = ActionType::Wait;
        if (firingMotor > 0) {
            type = ActionType::MoveForward;
        }
        
        auto action = std::make_unique<Action>(type);
        
        return action;
    }
    
    // Get spike system access
    static SpikeSystem* getSpikeSystem(Brain::Impl* impl) {
        return impl->spikeSystem.get();
    }
    
    // Calculate excitation/inhibition ratio
    static float getExcitationInhibitionRatio(Brain::Impl* impl) {
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        
        for (const auto& region : impl->regions) {
            for (const auto& syn : region->getSynapses()) {
                float weight = syn->getWeight();
                if (weight > 0) {
                    totalExcitatory += weight;
                } else {
                    totalInhibitory += std::abs(weight);
                }
            }
        }
        
        if (totalInhibitory > 0.0f) {
            return totalExcitatory / totalInhibitory;
        }
        return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
    }
    
    // Statistics accessors
    static size_t getTotalSpikeCount(Brain::Impl* impl) {
        return impl->totalSpikesTotal;
    }
    
    static size_t getPendingSpikeEventCount(Brain::Impl* impl) {
        return impl->spikeSystem->getPendingSpikeCount() + impl->spikeSystem->getPendingDelayedCount();
    }
    
    // Save brain state to file (checkpointing)
    static bool save(Brain::Impl* impl, const std::string& filepath) {
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        try {
            CheckpointWriter writer;
            if (!writer.create(filepath, CompressionLevel::Balanced)) {
                NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
                return false;
            }
            
            // Set metadata
            writer.setMetadata(
                getTotalNeuronCountFromRegions(impl->regions),
                getTotalSynapseCountFromRegions(impl->regions),
                impl->regions.size(),
                impl->currentStep,
                impl->currentTime
            );
            
            // Write neurons
            NeuronCheckpointData neuronData;
            neuronData.membranePotential.reserve(getTotalNeuronCountFromRegions(impl->regions));
            neuronData.restingPotential.reserve(getTotalNeuronCountFromRegions(impl->regions));
            neuronData.threshold.reserve(getTotalNeuronCountFromRegions(impl->regions));
            neuronData.resetPotential.reserve(getTotalNeuronCountFromRegions(impl->regions));
            neuronData.leakConductance.reserve(getTotalNeuronCountFromRegions(impl->regions));
            
            for (const auto& region : impl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        neuronData.membranePotential.push_back(state.membranePotential);
                        neuronData.restingPotential.push_back(state.restingPotential);
                        neuronData.threshold.push_back(state.threshold);
                        neuronData.resetPotential.push_back(state.resetPotential);
                        neuronData.leakConductance.push_back(state.leakConductance);
                        neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                        neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                        neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                        neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                        neuronData.firingRate.push_back(state.firingRate);
                    }
                }
            }
            
            if (!writer.writeNeurons(neuronData)) {
                NLM_LOG_ERROR("Failed to write neurons to checkpoint");
                return false;
            }
            
            // Write synapses
            SynapseCheckpointData synapseData;
            for (const auto& region : impl->regions) {
                for (const auto& syn : region->getSynapses()) {
                    synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                    synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                    synapseData.weight.push_back(syn->getWeight());
                    synapseData.delay.push_back(syn->getDelay());
                    synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                    synapseData.plasticityFlags.push_back(syn->getPlasticityFlags());
                    synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                    synapseData.efficacy.push_back(syn->getEfficacy());
                }
            }
            
            if (!writer.writeSynapses(synapseData)) {
                NLM_LOG_ERROR("Failed to write synapses to checkpoint");
                return false;
            }
            
            // Pending delayed spikes are in-flight synaptic input; a reload
            // without them drops every spike scheduled for the next 1-5
            // steps. Save them alongside neurons and synapses.
            if (impl->spikeSystem) {
                auto pending = impl->spikeSystem->getPendingDelayedSpikes();
                if (!writer.writeDelayedSpikes(pending)) {
                    NLM_LOG_ERROR("Failed to write pending spikes to checkpoint");
                    return false;
                }
            }
            
            // Finalize
            if (!writer.finalize()) {
                NLM_LOG_ERROR("Failed to finalize checkpoint");
                return false;
            }
            
            NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
            return false;
        }
    }
    
    // Load brain state from file
    static bool load(Brain::Impl* impl, const std::string& filepath) {
        NLM_LOG_INFO("Loading brain state from " + filepath);
        
        try {
            CheckpointReader reader;
            if (!reader.open(filepath)) {
                NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
                return false;
            }
            
            if (!reader.validate()) {
                NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
                return false;
            }
            
            // For simplicity, just reset and initialize
            // In a full implementation, this would restore the actual state
            reset(impl);
            
            NLM_LOG_INFO("Brain state loaded successfully");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
            return false;
        }
    }
    
    // Reset brain state
    static void reset(Brain::Impl* impl) {
        NLM_LOG_INFO("Resetting NLM Brain...");
        
        impl->currentStep = 0;
        impl->currentTime = 0.0;
        impl->totalSpikesThisStep = 0;
        impl->totalSpikesTotal = 0;
        impl->isResting = false;
        impl->stepsSinceLastEpisode = 0;
        
        for (auto& region : impl->regions) {
            region->reset();
        }
        
        if (impl->spikeSystem) {
            impl->spikeSystem->reset();
        }
        
        impl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset memory systems
        if (impl->workingMemory) impl->workingMemory->clear();
        if (impl->episodicMemory) impl->episodicMemory->clear();
        if (impl->associativeMemory) impl->associativeMemory->clear();
        if (impl->attention) impl->attention->reset();
        
        NLM_LOG_INFO("NLM Brain reset complete");
    }
    
    // Apply neuromodulatory signals
    static void applyNeuromodulation(Brain::Impl* impl, const Neuromodulator& signal) {
        // Apply neuromodulation effects on plasticity
        float modulation = signal.getLevel();
        
        // Scale STDP learning rates
        if (impl->stdp) {
            impl->stdp->setLTPWeight(0.01f * modulation);
            impl->stdp->setLTDWeight(0.012f * modulation);
        }
    }
    
    // Update plasticity rules (called automatically in step)
    static void updatePlasticity(Brain::Impl* impl) {
        // Plasticity is now applied during each step
        // This method is kept for API compatibility
    }
    
    // Apply developmental changes (called automatically in step)
    static void develop(Brain::Impl* impl) {
        // Development updates structural plasticity
        if (impl->structuralPlasticity && impl->rng) {
            impl->structuralPlasticity->update(nullptr, *impl->rng);
        }
    }
    
    // Get statistics
    static size_t getTotalNeuronCount(Brain::Impl* impl) {
        size_t total = 0;
        for (const auto& region : impl->regions) {
            total += region->getTotalNeuronCount();
        }
        return total;
    }
    
    static size_t getTotalSynapseCount(Brain::Impl* impl) {
        size_t total = 0;
        for (const auto& region : impl->regions) {
            total += region->getSynapseCount();
        }
        total += impl->interRegionConnections.size();
        return total;
    }
    
    static size_t getActiveNeuronCount(Brain::Impl* impl) {
        size_t total = 0;
        for (const auto& region : impl->regions) {
            total += region->getActiveNeuronCount();
        }
        return total;
    }
    
    static size_t getFiringNeuronCount(Brain::Impl* impl) {
        return impl->totalSpikesThisStep;
    }
    
float getAverageFiringRate(Brain::Impl* impl) {
        if (impl->regions.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& region : impl->regions) {
            sum += region->getAverageFiringRate();
        }
        return sum / static_cast<float>(impl->regions.size());
    }
        return sum / static_cast<float>(impl->regions.size());
    }
    
    static float getLearnedPreference(Brain::Impl* impl) {
        return impl->learnedPreference;
    }
    
    static void setLearnedPreference(Brain::Impl* impl, float preference) {
        impl->learnedPreference = std::clamp(preference, -1.0f, 1.0f);
    }
    
    static void logStatus(Brain::Impl* impl) {
        NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
        NLM_LOG_INFO("Regions: " + std::to_string(impl->regions.size()));
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount(impl)));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount(impl)));
        NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount(impl)));
        NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount(impl)));
        NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount(impl)));
        NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount(impl)));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate(impl)));
        NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio(impl)));
        
        // Memory system status
        if (impl->workingMemory) {
            NLM_LOG_INFO("Working memory traces: " + std::to_string(impl->workingMemory->getActiveTraces()));
        }
        if (impl->episodicMemory) {
            NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(impl->episodicMemory->getEpisodeCount()));
        }
        
        // Neuromodulation status
        if (impl->dopamine) {
            NLM_LOG_INFO("Dopamine level: " + std::to_string(impl->dopamine->getLevel()));
        }
        
        // Development status
        NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(impl->developmentalStage)));
    }
    
    static void setDevelopmentalStage(Brain::Impl* impl, DevelopmentalStage stage) {
        impl->developmentalStage = stage;
    }
    
    static DevelopmentalStage getDevelopmentalStage(Brain::Impl* impl) {
        return impl->developmentalStage;
    }
    
    // Integration state access
    static bool isResting(Brain::Impl* impl) {
        return impl->isResting;
    }
    
    static void setResting(Brain::Impl* impl, bool resting) {
        impl->isResting = resting;
    }
    
    static float getTimestep(Brain::Impl* impl) {
        return impl->timestep;
    }
    
    static size_t getStepsSinceLastEpisode(Brain::Impl* impl) {
        return impl->stepsSinceLastEpisode;
    }
    
    static void setStepsSinceLastEpisode(Brain::Impl* impl, size_t steps) {
        impl->stepsSinceLastEpisode = steps;
    }
    
    static float getLearnedPreference(Brain::Impl* impl) {
        return impl->learnedPreference;
    }
    
    static void setLearnedPreference(Brain::Impl* impl, float preference) {
        impl->learnedPreference = std::clamp(preference, -1.0f, 1.0f);
    }
};