#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <algorithm>

namespace nlm {

void Brain::receiveSensoryInputInternal(const class SensoryInput& input) {
    if (input.pattern.empty() || sensoryNeurons.empty()) {
        return;
    }
    
    // Distribute sensory input across sensory neurons
    float inputPerNeuron = input.magnitude / sensoryNeurons.size();
    
    for (size_t i = 0; i < sensoryNeurons.size() && i < input.pattern.size(); ++i) {
        float scaledInput = inputPerNeuron * input.pattern[i];
        if (sensoryNeurons[i]) {
            sensoryNeurons[i]->injectCurrent(scaledInput);
        }
    }
    
    // Update working memory with sensory input
    if (workingMemory) {
        workingMemory->store(input.pattern, input.magnitude);
    }
    
    // Apply sensory input to prediction system
    if (predictionSystem) {
        predictionSystem->applySensoryInput(input);
    }
    
    NLM_LOG_DEBUG("Applied sensory input: pattern size=" + std::to_string(input.pattern.size()) + 
                 ", magnitude=" + std::to_string(input.magnitude));
}

void Brain::injectCurrentInternal(NeuronId neuron, MembranePotential current) {
    // Find the neuron and inject current
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* n : pop->getNeurons()) {
                if (n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    }
    
    NLM_LOG_WARNING("Attempted to inject current to non-existent neuron: " + std::to_string(neuron.value));
}

void Brain::injectCurrentToNeuronsInternal(NeuronType type, MembranePotential current) {
    // Inject current to all neurons of specified type
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

std::unique_ptr<class Action> Brain::produceActionInternal() {
    // Check for motor neuron activity
    bool hasMotorActivity = false;
    for (auto* neuron : motorNeurons) {
        if (neuron && neuron->isFiring() && neuron->getType() == NeuronType::Motor) {
            hasMotorActivity = true;
            break;
        }
    }
    
    if (!hasMotorActivity) {
        return nullptr; // No action produced
    }
    
    // Create motor action based on motor neuron activity
    auto action = std::make_unique<Action>();
    
    // Find most active motor neurons
    std::vector<std::pair<NeuronId, float>> motorActivity;
    for (auto* neuron : motorNeurons) {
        if (neuron && neuron->getType() == NeuronType::Motor) {
            motorActivity.emplace_back(neuron->getId(), neuron->getFiringRate());
        }
    }
    
    // Sort by activity level
    std::sort(motorActivity.begin(), motorActivity.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Add top motor neurons to action
    for (size_t i = 0; i < std::min(size_t(3), motorActivity.size()); ++i) {
        action->addMotorCommand(motorActivity[i].first, motorActivity[i].second);
    }
    
    return action;
}

void Brain::applyNeuromodulationInternal(const class Neuromodulator& signal) {
    // Apply neuromodulatory signal to appropriate neural populations
    float modulationStrength = signal.getStrength();
    
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Apply modulation based on neuromodulator type
                switch (signal.getType()) {
                    case NeuromodulatorType::Dopamine:
                        // Dopamine affects neural excitability
                        neuron->injectCurrent(modulationStrength * 0.5f);
                        break;
                    case NeuromodulatorType::Curiosity:
                        // Curiosity increases exploration tendency
                        neuron->increaseExplorationTendency(modulationStrength);
                        break;
                    case NeuromodulatorType::Novelty:
                        // Novelty enhances novelty detection
                        neuron->enhanceNoveltyDetection(modulationStrength);
                        break;
                    case NeuromodulatorType::PredictionError:
                        // Prediction error updates learning rate
                        neuron->updateLearningRate(modulationStrength);
                        break;
                    default:
                        // Generic modulation
                        neuron->injectCurrent(modulationStrength);
                        break;
                }
            }
        }
    }
}

void Brain::updatePlasticityInternal() {
    // Update plasticity systems with current neuromodulation state
    if (dopamine) {
        float plasticityFactor = dopamine->getPlasticityFactor();
        
        // Update STDP with neuromodulation
        if (stdp && plasticityFactor != 1.0f) {
            // The STDP system has been updated internally in stepCore
            // This method provides additional plasticity updates
        }
        
        // Update Hebbian learning
        if (hebbian) {
            // Additional Hebbian updates based on dopamine levels
        }
    }
}

void Brain::developInternal() {
    // Apply developmental changes based on current developmental stage
    if (developmentSystem) {
        developmentSystem->update(timestep);
        
        // Check for developmental stage transitions
        DevelopmentalStage currentStage = developmentSystem->getDevelopmentalStage();
        if (currentStage == DevelopmentalStage::CriticalPeriod) {
            // Enhance synaptic formation during critical period
            for (auto& region : regions) {
                region->enhanceSynaptogenesis(2.0f);
            }
        }
    }
}

void Brain::resetInternal() {
    // Reset all neurons to resting state
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->reset();
            }
        }
    }
    
    // Reset memory systems
    if (workingMemory) {
        workingMemory->clear();
    }
    if (episodicMemory) {
        episodicMemory->clear();
    }
    
    // Reset statistics
    totalSpikesTotal = 0;
    totalSpikesThisStep = 0;
    currentStep = 0;
    currentTime = 0.0;
    
    // Reset neuromodulation
    if (dopamine) {
        dopamine->reset();
    }
    if (curiosity) {
        curiosity->reset();
    }
    if (novelty) {
        novelty->reset();
    }
}

bool Brain::saveInternal(const std::string& filepath) const {
    if (!checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available");
        return false;
    }
    
    return checkpointManager->save(filepath, this);
}

bool Brain::loadInternal(const std::string& filepath) {
    if (!checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available");
        return false;
    }
    
    return checkpointManager->load(filepath, this);
}

RegionId Brain::addRegionInternal(const std::string& name) {
    // Create new region
    auto region = std::make_unique<NeuralRegion>(name);
    
    // Add to regions list
    RegionId regionId(nextRegionId++);
    region->setId(regionId);
    regions.push_back(std::move(region));
    
    return regionId;
}

NeuralRegion* Brain::getRegionInternal(RegionId id) {
    for (auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegionInternal(RegionId id) const {
    return const_cast<Brain*>(this)->getRegionInternal(id);
}

std::vector<RegionId> Brain::getRegionIdsInternal() const {
    std::vector<RegionId> ids;
    ids.reserve(regions.size());
    for (const auto& region : regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

size_t Brain::getTotalNeuronCountInternal() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCountInternal() const {
    size_t total = 0;
    for (const auto& region : regions) {
        total += region->getTotalSynapseCount();
    }
    return total;
}

size_t Brain::getActiveNeuronCountInternal() const {
    size_t total = 0;
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isActive()) {
                    total++;
                }
            }
        }
    }
    return total;
}

size_t Brain::getFiringNeuronCountInternal() const {
    size_t total = 0;
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    total++;
                }
            }
        }
    }
    return total;
}

float Brain::getAverageFiringRateInternal() const {
    size_t firingCount = getFiringNeuronCountInternal();
    size_t totalNeurons = getTotalNeuronCountInternal();
    
    if (totalNeurons == 0) {
        return 0.0f;
    }
    
    return static_cast<float>(firingCount) / totalNeurons;
}

size_t Brain::getPendingSpikeEventCountInternal() const {
    return spikeSystem ? spikeSystem->getEventCount() : 0;
}

float Brain::getExcitationInhibitionRatioInternal() const {
    size_t excitation = 0;
    size_t inhibition = 0;
    
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->getType() == NeuronType::Excitatory) {
                    excitation++;
                } else if (neuron->getType() == NeuronType::Inhibitory) {
                    inhibition++;
                }
            }
        }
    }
    
    size_t total = excitation + inhibition;
    if (total == 0) {
        return 1.0f;
    }
    
    return static_cast<float>(excitation) / inhibition;
}

void Brain::logStatusInternal() const {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Simulation step: " + std::to_string(currentStep));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCountInternal()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCountInternal()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCountInternal()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCountInternal()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRateInternal()));
    NLM_LOG_INFO("Spike events: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatioInternal()));
    
    if (workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
    }
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }
    
    NLM_LOG_INFO("======================");
}

void Brain::addInterRegionConnectionInternal(RegionId source, RegionId target, 
                                              float weight, Delay delay) {
    // Create connection
    interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnectionInternal(RegionId source, RegionId target) {
    // Remove connection
    auto it = std::remove_if(interRegionConnections.begin(), interRegionConnections.end(),
        [source, target](const InterRegionConnection& conn) {
            return conn.sourceRegion == source && conn.targetRegion == target;
        });
    interRegionConnections.erase(it, interRegionConnections.end());
}

} // namespace nlm