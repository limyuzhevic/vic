#include "BrainRefactor.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"

#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <mutex>

namespace nlm {

std::unique_ptr<Brain> BrainFactory::create(std::shared_ptr<Config> config) {
    return std::make_unique<Brain>(config);
}

bool BrainInitializer::initializeBrain(Brain* brain) {
    if (!brain) return false;
    
    auto& impl = *brain->pImpl;
    
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = impl.config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = impl.config->getOr<size_t>("region_count", 1);
    float connectionProbability = impl.config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions and neurons
    configureNeuralConnectivity(brain, neuronCount, regionCount, connectionProbability);
    
    // Initialize integrated systems
    initializeIntegratedSystems(brain, neuronCount);
    
    // Setup spike handlers
    setupSpikeHandlers(brain);
    
    // Configure checkpoint system
    configureCheckpointSystem(brain);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(BrainManager::getTotalNeuronCount(brain)));
    NLM_LOG_INFO("Total synapses: " + std::to_string(BrainManager::getTotalSynapseCount(brain)));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(impl.sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(impl.motorNeurons.size()));
    
    return true;
}

void BrainInitializer::configureNeuralConnectivity(Brain* brain, size_t neuronCount, 
                                                   size_t regionCount, float connectionProbability) {
    auto& impl = *brain->pImpl;
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = BrainManager::getRegion(brain, RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    impl.sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    impl.motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = BrainManager::getRegion(brain, RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*impl.rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainInitializer::initializeIntegratedSystems(Brain* brain, size_t neuronCount) {
    auto& impl = *brain->pImpl;
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    impl.workingMemory->initialize(brain);
    impl.workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    impl.episodicMemory->initialize(brain);
    impl.episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    impl.associativeMemory->initialize(brain);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    impl.planner->initialize(brain);
    impl.planner->setPlanningDepth(5);
    
    impl.conceptFormation->initialize(brain);
    
    impl.attention->initialize(brain);
    impl.attention->setInhibitionStrength(0.5f);
    impl.attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    impl.novelty->initialize(brain);
    impl.curiosity->initialize(brain);
}

void BrainInitializer::setupSpikeHandlers(Brain* brain) {
    auto& impl = *brain->pImpl;
    
    // Register spike handlers for event-driven processing
    impl.spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    impl.spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : brain->pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
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

void BrainInitializer::configureCheckpointSystem(Brain* brain) {
    auto& impl = *brain->pImpl;
    
    // Configure checkpoint manager
    std::string checkpointDir = impl.config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    impl.checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainSimulator::simulateStep(Brain* brain, SimulationStep currentStep) {
    Timestamp currentTime = static_cast<Timestamp>(currentStep) * 0.001;  // Convert to time units
    BrainSimulator::simulateStep(brain, currentStep, currentTime);
}

void BrainSimulator::simulateStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain) return;
    
    auto& impl = *brain->pImpl;
    
    // Reset spike counts for this step
    impl.totalSpikesThisStep = 0;
    
    // Process neural dynamics (spike generation, synaptic transmission)
    brain->pImpl->spikeSystem->processStep(currentStep);
    
    // Apply neuromodulatory signals
    // (Neuromodulation systems process automatically via spike handlers)
    
    // Apply plasticity rules (called automatically in step)
    brain->updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    brain->develop();
    
    // Update simulation state
    impl.currentStep = currentStep;
    impl.currentTime = currentTime;
}

void BrainIO::handleSensoryInput(Brain* brain, const SensoryInput& input) {
    if (!brain) return;
    
    // Inject current into sensory neurons based on input pattern
    brain->receiveSensoryInput(input);
}

void BrainIO::injectCurrent(Brain* brain, NeuronId neuron, MembranePotential current) {
    if (!brain) return;
    
    brain->injectCurrent(neuron, current);
}

void BrainIO::injectCurrentToNeurons(Brain* brain, NeuronType type, MembranePotential current) {
    if (!brain) return;
    
    brain->injectCurrentToNeurons(type, current);
}

std::unique_ptr<Action> BrainIO::produceMotorOutput(Brain* brain) {
    if (!brain) return nullptr;
    
    return brain->produceAction();
}

void BrainIO::applyNeuromodulation(Brain* brain, const Neuromodulator& signal) {
    if (!brain) return;
    
    brain->applyNeuromodulation(signal);
}

void BrainManager::updatePlasticity(Brain* brain) {
    if (!brain) return;
    brain->updatePlasticity();
}

void BrainManager::develop(Brain* brain) {
    if (!brain) return;
    brain->develop();
}

void BrainManager::reset(Brain* brain) {
    if (!brain) return;
    brain->reset();
}

bool BrainManager::saveBrainState(Brain* brain, const std::string& filepath) const {
    if (!brain) return false;
    return brain->save(filepath);
}

bool BrainManager::loadBrainState(Brain* brain, const std::string& filepath) {
    if (!brain) return false;
    return brain->load(filepath);
}

size_t BrainManager::getRegionCount(const Brain* brain) {
    if (!brain) return 0;
    return brain->getRegionCount();
}

std::vector<RegionId> BrainManager::getRegionIds(const Brain* brain) {
    if (!brain) return {};
    return brain->getRegionIds();
}

NeuralRegion* BrainManager::getRegion(Brain* brain, RegionId id) {
    if (!brain) return nullptr;
    return brain->getRegion(id);
}

const NeuralRegion* BrainManager::getRegion(const Brain* brain, RegionId id) const {
    if (!brain) return nullptr;
    return brain->getRegion(id);
}

float BrainStatistics::getExcitationInhibitionRatio(const Brain* brain) {
    if (!brain) return 0.0f;
    return brain->getExcitationInhibitionRatio();
}

size_t BrainStatistics::getTotalSpikeCount(const Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalSpikeCount();
}

size_t BrainStatistics::getPendingSpikeEventCount(const Brain* brain) {
    if (!brain) return 0;
    return brain->getPendingSpikeEventCount();
}

size_t BrainStatistics::getTotalNeuronCount(const Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalNeuronCount();
}

size_t BrainStatistics::getTotalSynapseCount(const Brain* brain) {
    if (!brain) return 0;
    return brain->getTotalSynapseCount();
}

size_t BrainStatistics::getActiveNeuronCount(const Brain* brain) {
    if (!brain) return 0;
    return brain->getActiveNeuronCount();
}

size_t BrainStatistics::getFiringNeuronCount(const Brain* brain) {
    if (!brain) return 0;
    return brain->getFiringNeuronCount();
}

float BrainStatistics::getAverageFiringRate(const Brain* brain) {
    if (!brain) return 0.0f;
    return brain->getAverageFiringRate();
}

void BrainStatistics::logStatus(const Brain* brain) {
    if (!brain) return;
    brain->logStatus();
}

} // namespace nlm
