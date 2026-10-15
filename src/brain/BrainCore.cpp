#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

void BrainCore::initializeCore(std::shared_ptr<Brain> brain, size_t neuronCount, size_t regionCount,
                              float connectionProbability) {
    NLM_LOG_INFO("Initializing BrainCore with " + std::to_string(neuronCount) + 
                " neurons and " + std::to_string(regionCount) + " regions");
    
    // Core neural architecture initialization
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Core_Region_" + std::to_string(i + 1));
    }
    
    // Create balanced neuron populations
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Balanced population types
            size_t sensoryPopSize = neuronsPerRegion / 4;
            size_t internalPopSize = neuronsPerRegion / 2;
            size_t motorPopSize = neuronsPerRegion / 4;
            
            auto* sensoryPop = region->addPopulation(sensoryPopSize, NeuronType::Sensory);
            auto* internalPop = region->addPopulation(internalPopSize, NeuronType::Internal);
            auto* motorPop = region->addPopulation(motorPopSize, NeuronType::Motor);
            
            NLM_LOG_INFO("Core region " + std::to_string(i + 1) + ": " +
                        std::to_string(sensoryPopSize) + " sensory, " +
                        std::to_string(internalPopSize) + " internal, " +
                        std::to_string(motorPopSize) + " motor neurons");
        }
    }
}

void BrainCore::createNeuralArchitecture(std::shared_ptr<Brain> brain, size_t neuronCount, size_t regionCount) {
    // Enhanced architecture with better organization
    float minWeight = -0.5f;  // More balanced initial weights
    float maxWeight = 0.5f;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize with more sophisticated connectivity
            region->initializeRandomConnectivity(*brain->getRandomGenerator(),
                                                0.1f, minWeight, maxWeight);
            
            // Add initial patterns for learning
            if (i == 0) {  // First region as input buffer
                addLearningPatterns(region, brain->getRandomGenerator().get());
            }
        }
    }
}

void BrainCore::initializeConnectivity(std::shared_ptr<Brain> brain, float connectionProbability) {
    // Adaptive connectivity based on region size and type
    for (auto* region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t regionSize = neurons.size();
        
        // Adjust connection probability based on region size
        float adaptiveProbability = connectionProbability;
        if (regionSize < 100) adaptiveProbability *= 1.5f;  // Small regions, higher density
        else if (regionSize > 1000) adaptiveProbability *= 0.7f;  // Large regions, sparser
        
        region->initializeRandomConnectivity(*brain->getRandomGenerator(),
                                             adaptiveProbability, -0.3f, 0.3f);
    }
}

void BrainCore::processSpikes(std::shared_ptr<Brain> brain, SimulationStep currentStep, Timestamp currentTime) {
    // Optimized spike processing with parallel potential
    for (auto& region : brain->getRegions()) {
        processSpikesForRegion(region.get(), currentStep, currentTime);
    }
}

void BrainCore::updateNeurons(std::shared_ptr<Brain> brain, Timestamp currentTime, TimestepDuration timestep) {
    // Batch update for better cache locality
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            updateNeuronsForPopulation(pop.get(), currentTime, timestep);
        }
    }
}

void BrainCore::applyPlasticity(std::shared_ptr<Brain> brain, SimulationStep currentStep, Timestamp currentTime) {
    STDP* stdp = brain->getSTDP();
    Hebbian* hebbian = brain->getHebbian();
    
    float plasticityMod = 1.0f;
    if (auto* dopamine = brain->getDopamine()) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
    
    // Apply plasticity to all synapses in all regions
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            applyPlasticityForSynapse(syn.get(), currentStep, currentTime, 
                                    plasticityMod, stdp, hebbian);
        }
    }
}

void BrainCore::updateStructuralPlasticity(std::shared_ptr<Brain> brain, size_t currentStep) {
    // Adaptive structural plasticity based on activity and development
    if (auto* sp = brain->getStructuralPlasticity()) {
        // Calculate plasticity rate based on multiple factors
        float activityFactor = calculateActivityFactor(brain.get());
        DevelopmentalStage devStage = brain->getDevelopmentalStage();
        
        float plasticityRate = calculateStructuralPlasticityRate(activityFactor, devStage);
        
        sp->setSynaptogenesisRate(plasticityRate * 0.0001f);
        sp->setPruningRate(plasticityRate * 0.00001f);
        
        // Update structural plasticity
        sp->update(brain.get(), *brain->getRandomGenerator());
    }
}

size_t BrainCore::getTotalNeuronCount(const Brain* brain) const {
    return brain->getTotalNeuronCount();
}

size_t BrainCore::getTotalSynapseCount(const Brain* brain) const {
    return brain->getTotalSynapseCount();
}

size_t BrainCore::getActiveNeuronCount(const Brain* brain) const {
    return brain->getActiveNeuronCount();
}

size_t BrainCore::getFiringNeuronCount(const Brain* brain) const {
    return brain->getFiringNeuronCount();
}

float BrainCore::getAverageFiringRate(const Brain* brain) const {
    return brain->getAverageFiringRate();
}

float BrainCore::getExcitationInhibitionRatio(const Brain* brain) const {
    return brain->getExcitationInhibitionRatio();
}

void BrainCore::collectSensoryNeurons(Brain* brain, std::vector<Neuron*>& sensoryNeurons) {
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            }
        }
    }
}

void BrainCore::collectMotorNeurons(Brain* brain, std::vector<Neuron*>& motorNeurons) {
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

NeuralRegion* BrainCore::getRegion(Brain* brain, RegionId id) {
    return brain->getRegion(id);
}

const NeuralRegion* BrainCore::getRegion(const Brain* brain, RegionId id) const {
    return brain->getRegion(id);
}

void BrainCore::initializeRandomConnectivity(NeuralRegion* region, RandomGenerator& rng,
                                           float connectionProbability, float minWeight, float maxWeight) {
    region->initializeRandomConnectivity(rng, connectionProbability, minWeight, maxWeight);
}

void BrainCore::setupSpikeSystemHandlers(std::shared_ptr<Brain> brain) {
    // Enhanced spike handlers for better processing
    brain->getSpikeSystem()->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
    });
    
    // Enhanced delayed spike handler for synaptic transmission
    brain->getSpikeSystem()->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain->pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current with better scaling
                    MembranePotential synapticCurrent = event.weight * 15.0f;  // Better scaling factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

void BrainCore::processSpikesForRegion(NeuralRegion* region, SimulationStep currentStep, Timestamp currentTime) {
    auto neurons = region->getAllNeurons();
    for (auto* neuron : neurons) {
        // Check if neuron just fired this step
        const auto& state = neuron->getState();
        bool justFired = (state.firingState == FiringState::Refractory &&
                         state.lastSpikeTime >= 0.0f &&
                         std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < 0.002f);

        if (justFired) {
            // Record pre-synaptic spike for plasticity
            auto incomingSynapses = region->getSynapsesTo(neuron->getId());
            for (Synapse* syn : incomingSynapses) {
                syn->recordPostSpike(currentTime);
            }
        }
    }
}

void BrainCore::updateNeuronsForPopulation(NeuralPopulation* pop, Timestamp currentTime, TimestepDuration timestep) {
    for (auto* neuron : pop->getNeurons()) {
        neuron->stepLIF(currentTime, timestep);
    }
}

void BrainCore::applyPlasticityForSynapse(Synapse* syn, SimulationStep currentStep, Timestamp currentTime,
                                         float plasticityMod, STDP* stdp, Hebbian* hebbian) {
    // Apply STDP with neuromodulation
    if (syn->getPlasticityFlags().stdp) {
        const auto& preSpikes = syn->getPreSpikeHistory();
        const auto& postSpikes = syn->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            // Modify weight change based on neuromodulation
            stdp->update(syn, preSpikes, postSpikes, currentStep * 0.001f);
            float weight = syn->getWeight();
            weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
            syn->setWeight(weight);
        }
    }
    
    // Apply Hebbian learning
    if (syn->getPlasticityFlags().hebbian) {
        const auto& preSpikes = syn->getPreSpikeHistory();
        const auto& postSpikes = syn->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            hebbian->update(syn, preSpikes, postSpikes, currentStep * 0.001f);
        }
    }
    
    // Update synapse state
    syn->step(currentTime);
}

void BrainCore::addLearningPatterns(NeuralRegion* region, RandomGenerator* rng) {
    // Add initial patterns to help with learning
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 2) return;
    
    // Create initial correlated activity patterns
    for (size_t i = 0; i < std::min(size_t(3), neurons.size() / 2); ++i) {
        Neuron* preNeuron = neurons[i * 2];
        Neuron* postNeuron = neurons[i * 2 + 1];
        
        // Find existing synapse or create one
        bool found = false;
        for (auto* syn : region->getSynapsesFrom(preNeuron->getId())) {
            if (syn->getDestinationNeuron() == postNeuron->getId()) {
                found = true;
                syn->setWeight(0.2f);  // Initial weight for pattern learning
                break;
            }
        }
        
        if (!found) {
            float weight = (rng->nextFloat() > 0.5f) ? 0.2f : -0.1f;
            region->addSynapse(preNeuron->getId(), postNeuron->getId(), weight);
        }
    }
}

float BrainCore::calculateActivityFactor(const Brain* brain) const {
    size_t activeNeurons = brain->getActiveNeuronCount();
    size_t totalNeurons = brain->getTotalNeuronCount();
    if (totalNeurons == 0) return 1.0f;
    
    return static_cast<float>(activeNeurons) / totalNeurons;
}

float BrainCore::calculateStructuralPlasticityRate(float activityFactor, DevelopmentalStage devStage) const {
    // Higher activity and earlier development stages -> more plasticity
    float rate = activityFactor * 2.0f; // Scale activity factor
    
    switch (devStage) {
        case DevelopmentalStage::Initial:
            rate *= 1.5f; // High plasticity in initial stage
            break;
        case DevelopmentalStage::CriticalPeriod:
            rate *= 1.2f;
            break;
        case DevelopmentalStage::Maturation:
            rate *= 0.8f;
            break;
        case DevelopmentalStage::Adult:
            rate *= 0.5f; // Low plasticity in adult stage
            break;
    }
    
    // Clamp to reasonable range
    return std::max(0.1f, std::min(3.0f, rate));
}

} // namespace nlm