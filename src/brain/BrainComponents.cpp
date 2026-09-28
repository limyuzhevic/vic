#include "BrainComponents.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <algorithm>

namespace nlm {

BrainInitializer::BrainInitializer(std::shared_ptr<Config> config, std::unique_ptr<RandomGenerator> rng)
    : config_(std::move(config)), rng_(std::move(rng)), nextRegionId_(1) {}

bool BrainInitializer::initializeRegions(Brain* brain, size_t neuronCount, size_t regionCount) {
    NLM_LOG_INFO("Initializing " + std::to_string(regionCount) + " brain regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
            return false;
        }
        
        // Add populations with balanced composition
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + ": " +
                    std::to_string(region->getPopulationCount()) + " populations, " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
    
    NLM_LOG_INFO("Region initialization complete");
    return true;
}

bool BrainInitializer::initializeMemorySystems(Brain* brain) {
    NLM_LOG_INFO("Initializing memory systems...");
    
    if (!brain->getWorkingMemory()) {
        NLM_LOG_ERROR("Working memory not available");
        return false;
    }
    
    if (!brain->getEpisodicMemory()) {
        NLM_LOG_ERROR("Episodic memory not available");
        return false;
    }
    
    // Set memory capacities based on brain size
    size_t capacity = brain->getTotalNeuronCount() / 10;
    brain->getWorkingMemory()->setCapacity(capacity);
    brain->getEpisodicMemory()->setMaxEpisodes(1000);
    
    NLM_LOG_INFO("Memory systems initialized with working memory capacity: " + std::to_string(capacity));
    return true;
}

bool BrainInitializer::initializePlasticitySystems(Brain* brain) {
    NLM_LOG_INFO("Initializing plasticity systems...");
    
    if (!brain->getSTDP()) {
        NLM_LOG_ERROR("STDP not available");
        return false;
    }
    
    if (!brain->getHebbian()) {
        NLM_LOG_ERROR("Hebbian not available");
        return false;
    }
    
    if (!brain->getStructuralPlasticity()) {
        NLM_LOG_ERROR("Structural plasticity not available");
        return false;
    }
    
    NLM_LOG_INFO("Plasticity systems initialized");
    return true;
}

bool BrainInitializer::initializeIntegrationPoints(Brain* brain) {
    NLM_LOG_INFO("Initializing integration points...");
    
    // Initialize cognitive systems
    if (brain->getPlanner()) {
        brain->getPlanner()->setPlanningDepth(5);
        NLM_LOG_INFO("Planner initialized with depth 5");
    }
    
    if (brain->getConceptFormation()) {
        NLM_LOG_INFO("Concept formation initialized");
    }
    
    if (brain->getAttention()) {
        brain->getAttention()->setInhibitionStrength(0.5f);
        brain->getAttention()->setExcitationStrength(1.5f);
        NLM_LOG_INFO("Attention system initialized with inhibition=0.5, excitation=1.5");
    }
    
    // Initialize development system
    if (brain->getDevelopmentSystem()) {
        NLM_LOG_INFO("Development system initialized");
    }
    
    // Initialize neuromodulation systems
    if (brain->getDopamine()) {
        NLM_LOG_INFO("Dopamine system initialized");
    }
    
    if (brain->getCuriosity()) {
        NLM_LOG_INFO("Curiosity system initialized");
    }
    
    if (brain->getNovelty()) {
        NLM_LOG_INFO("Novelty detection system initialized");
    }
    
    NLM_LOG_INFO("Integration points initialized");
    return true;
}

bool BrainInitializer::initializeCheckpointSystem(Brain* brain) {
    NLM_LOG_INFO("Initializing checkpoint system...");
    
    std::string checkpointDir = config_->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (brain->getCheckpointManager()) {
        brain->getCheckpointManager()->configure(checkpointDir, 10000, 5, true);
        NLM_LOG_INFO("Checkpoint system configured with directory: " + checkpointDir);
    }
    
    NLM_LOG_INFO("Checkpoint system initialization complete");
    return true;
}

NeuralProcessor::NeuralProcessor(Brain* brain) : brain_(brain) {
    if (!brain_) {
        NLM_LOG_ERROR("Brain pointer is null in NeuralProcessor constructor");
        return;
    }
    
    // Collect sensory and motor neurons for easy access
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    sensoryNeurons_.push_back(neuron);
                }
            } else if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    motorNeurons_.push_back(neuron);
                }
            }
        }
    }
    
    NLM_LOG_INFO("NeuralProcessor initialized with " + std::to_string(sensoryNeurons_.size()) + 
                " sensory neurons and " + std::to_string(motorNeurons_.size()) + " motor neurons");
}

void NeuralProcessor::processNeuronDynamics(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    // Update all neurons with LIF dynamics
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, brain->getTimestepDuration());
            }
        }
    }
}

void NeuralProcessor::detectSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    // Detect neurons that just fired this step
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain->getTimestepDuration() * 2.0f);

                if (justFired) {
                    // Neuron fired - this will be handled by the spike system
                    // Record post-synaptic spike for plasticity
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }
                }
            }
        }
    }
}

void NeuralProcessor::processDelayedSpikes(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    // Process delayed spike events from spike system
    if (brain->getSpikeSystem()) {
        brain->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
    }
}

void NeuralProcessor::applyPlasticity(Brain* brain, float plasticityMod, float dopamineLevel) {
    // Apply STDP and Hebbian plasticity
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation factor
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (brain->getSTDP()) {
                        brain->getSTDP()->update(syn, preSpikes, postSpikes, brain->getTimestepDuration());
                    }
                    
                    // Apply neuromodulation factor
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
                    if (brain->getHebbian()) {
                        brain->getHebbian()->update(syn, preSpikes, postSpikes, brain->getTimestepDuration());
                    }
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void NeuralProcessor::updateStructuralPlasticity(Brain* brain, size_t currentStep) {
    if (brain->getStructuralPlasticity() && currentStep % 100 == 0) {
        brain->getStructuralPlasticity()->update(brain, *brain->getRandomGenerator());
    }
}

SystemIntegrator::SystemIntegrator(Brain* brain) : brain_(brain) {
    if (!brain_) {
        NLM_LOG_ERROR("Brain pointer is null in SystemIntegrator constructor");
    }
}

void SystemIntegrator::coordinateMemorySystems(Brain* brain) {
    // Update working memory based on neural activity
    if (brain->getWorkingMemory()) {
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 10.0f;
                        brain->getWorkingMemory()->storeToNeuron(neuron->getId(), activation);
                    }
                }
            }
        }
        
        brain->getWorkingMemory()->update(brain->getTimestepDuration());
        NLM_LOG_DEBUG("Working memory updated with active traces: " + 
                     std::to_string(brain->getWorkingMemory()->getActiveTraces()));
    }
    
    // Update episodic memory periodically
    if (brain->getEpisodicMemory() && brain->getDopamine()) {
        static size_t stepsSinceLastEpisode = 0;
        stepsSinceLastEpisode++;
        
        if (stepsSinceLastEpisode >= 10) {
            stepsSinceLastEpisode = 0;
            
            EpisodicMemoryItem episode;
            episode.timestamp = brain->getCurrentStep();
            episode.reward = brain->getDopamine()->getLevel();
            
            // Store active neurons
            for (auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            brain->getEpisodicMemory()->storeEpisode(episode);
        }
    }
}

void SystemIntegrator::coordinateNeuromodulation(Brain* brain, float timestep) {
    if (brain->getDopamine()) {
        brain->getDopamine()->update(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = brain->getDopamine()->getLevel();
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    if (brain->getCuriosity()) {
        brain->getCuriosity()->update(timestep);
    }
    
    if (brain->getNovelty()) {
        brain->getNovelty()->update(timestep);
    }
}

void SystemIntegrator::coordinateCognitionSystems(Brain* brain, float timestep) {
    if (brain->getAttention()) {
        brain->getAttention()->update(timestep);
        
        // Apply attention to working memory winners
        if (brain->getWorkingMemory() && !brain->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain->getWorkingMemory()->getMemoryNeurons();
            brain->getAttention()->processCompetition(competitors);
        }
    }
    
    if (brain->getPlanner()) {
        // Simple planning integration - could be more sophisticated
        // For now, just update planner with current state
    }
    
    if (brain->getConceptFormation()) {
        // Concept formation based on current neural patterns
        // This requires more sophisticated implementation
    }
}

void SystemIntegrator::coordinateDevelopmentEffects(Brain* brain, size_t currentStep) {
    if (currentStep % 1000 == 0 && brain->getDevelopmentSystem()) {
        brain->getDevelopmentSystem()->update(brain, *brain->getRandomGenerator(), brain->getTimestepDuration() * 1000);
        
        // Development affects plasticity rates
        auto* sp = brain->getStructuralPlasticity();
        if (sp) {
            auto stage = brain->getDevelopmentalStage();
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial: plasticityMod = 1.0f; break;
                case DevelopmentalStage::CriticalPeriod: plasticityMod = 0.8f; break;
                case DevelopmentalStage::Maturation: plasticityMod = 0.5f; break;
                case DevelopmentalStage::Adult: plasticityMod = 0.2f; break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void SystemIntegrator::coordinateReplayAndConsolidation(Brain* brain, size_t currentStep) {
    // Replay important memories
    if (currentStep % 100 == 0 && brain->getEpisodicMemory()) {
        auto episodesToReplay = brain->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain->getEpisodicMemory()->replayEpisode(episode);
        }
    }
    
    // Periodic memory consolidation
    if (currentStep % 1000 == 0 && brain->getEpisodicMemory()) {
        brain->getEpisodicMemory()->consolidate(0.3f);
    }
}

void SystemIntegrator::collectStatistics(Brain* brain) const {
    BrainStatusCollector collector(brain);
    collector.collect(brain);
}

BrainStatusCollector::BrainStatusCollector(Brain* brain) : brain_(brain) {
    if (!brain_) {
        NLM_LOG_ERROR("Brain pointer is null in BrainStatusCollector constructor");
    }
}

void BrainStatusCollector::collect(Brain* brain) const {
    if (!brain_) {
        return;
    }
    
    // Collect basic metrics
    totalNeurons_ = brain->getTotalNeuronCount();
    totalSynapses_ = brain->getTotalSynapseCount();
    activeNeurons_ = brain->getActiveNeuronCount();
    totalSpikes_ = brain->getTotalSpikeCount();
    avgFiringRate_ = brain->getAverageFiringRate();
    eiRatio_ = brain->getExcitationInhibitionRatio();
    
    // Collect memory system metrics
    if (brain->getWorkingMemory()) {
        workingMemoryTraces_ = brain->getWorkingMemory()->getActiveTraces();
    }
    
    if (brain->getEpisodicMemory()) {
        episodicMemoryEpisodes_ = brain->getEpisodicMemory()->getEpisodeCount();
    }
    
    // Collect neuromodulation metrics
    if (brain->getDopamine()) {
        dopamineLevel_ = brain->getDopamine()->getLevel();
    }
    
    // Collect development metrics
    developmentalStage_ = brain->getDevelopmentalStage();
    
    // Collect region metrics
    for (const auto& region : brain->getRegions()) {
        RegionMetrics metrics;
        metrics.regionId = region->getId();
        metrics.regionName = region->getName();
        metrics.neuronCount = region->getTotalNeuronCount();
        metrics.synapseCount = region->getSynapseCount();
        metrics.avgWeight = region->getAverageSynapticWeight();
        regionMetrics_.push_back(metrics);
    }
}

std::string BrainStatusCollector::getStatusString() const {
    std::ostringstream oss;
    oss << "=== NLM Brain Status ===\n";
    oss << "Regions: " << brain_->getRegionCount() << "\n";
    oss << "Total neurons: " << totalNeurons_ << "\n";
    oss << "Total synapses: " << totalSynapses_ << "\n";
    oss << "Active neurons: " << activeNeurons_ << "\n";
    oss << "Firing neurons (this step): " << brain_->getFiringNeuronCount() << "\n";
    oss << "Total spikes: " << totalSpikes_ << "\n";
    oss << "Pending spike events: " << brain_->getPendingSpikeEventCount() << "\n";
    oss << "Average firing rate: " << avgFiringRate_ << "\n";
    oss << "E/I ratio: " << eiRatio_ << "\n";
    
    if (workingMemoryTraces_ > 0) {
        oss << "Working memory traces: " << workingMemoryTraces_ << "\n";
    }
    
    if (episodicMemoryEpisodes_ > 0) {
        oss << "Episodic memory episodes: " << episodicMemoryEpisodes_ << "\n";
    }
    
    if (dopamineLevel_ > 0) {
        oss << "Dopamine level: " << dopamineLevel_ << "\n";
    }
    
    oss << "Developmental stage: " << static_cast<int>(developmentalStage_) << "\n";
    
    for (const auto& metrics : regionMetrics_) {
        oss << "  Region " << metrics.regionId.index() << " (" << metrics.regionName << "):" <<
            " " << metrics.neuronCount << " neurons, " << metrics.synapseCount << " synapses, " <<
            "avg weight: " << metrics.avgWeight << "\n";
    }
    
    return oss.str();
}

} // namespace nlm
