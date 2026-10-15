#include "BrainIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

void BrainIntegration::initializeIntegrationSystems(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Initializing integrated brain systems...");
    
    // Configure neuromodulation systems
    if (auto* novelty = brain->getNovelty()) novelty->initialize(brain.get());
    if (auto* curiosity = brain->getCuriosity()) curiosity->initialize(brain.get());
    if (auto* dopamine = brain->getDopamine()) dopamine->initialize(brain.get());
    if (auto* predictionError = brain->getPredictionErrorSignal()) predictionError->initialize(brain.get());
    
    // Configure attention system
    if (auto* attention = brain->getAttention()) {
        attention->initialize(brain.get());
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    // Setup inter-system connections
    setupInterSystemConnections(brain);
}

void BrainIntegration::setupInterSystemConnections(std::shared_ptr<Brain> brain) {
    // Connect memory systems to neural processing
    if (auto* wm = brain->getWorkingMemory()) {
        wm->setCapacity(brain->getTotalNeuronCount() / 10);
    }
    
    if (auto* em = brain->getEpisodicMemory()) {
        em->setMaxEpisodes(1000);
        em->setReplayInterval(getReplayInterval(brain->getConfig()));
    }
    
    if (auto* am = brain->getAssociativeMemory()) {
        am->setLearningRate(0.01f);
    }
    
    // Connect neuromodulation to plasticity
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->registerPlasticityModulator(
            [brain](float level) {
                if (auto* sp = brain->getSTDP()) {
                    sp->setLTPWeight(0.01f * level);
                    sp->setLTDWeight(0.012f * level);
                }
            }
        );
    }
    
    // Connect attention to working memory
    if (auto* attention = brain->getAttention() && auto* wm = brain->getWorkingMemory()) {
        attention->registerWorkingMemoryUpdater(
            [wm](const std::vector<NeuronId>& competitors) {
                wm->processCompetition(competitors);
            }
        );
    }
}

void BrainIntegration::initializeMemorySystems(std::shared_ptr<Brain> brain, size_t neuronCount) {
    // Enhanced memory system initialization
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->initialize(brain.get());
        workingMemory->setCapacity(neuronCount / 10);
        workingMemory->setDecayRate(0.95f); // Better decay control
        workingMemory->setCompetitionThreshold(0.5f);
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        episodicMemory->initialize(brain.get());
        episodicMemory->setMaxEpisodes(1000);
        episodicMemory->setReplayInterval(getReplayInterval(brain->getConfig()));
        episodicMemory->setConsolidationInterval(getConsolidationInterval(brain->getConfig()));
        episodicMemory->setConsolidationThreshold(0.3f);
    }
    
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        associativeMemory->initialize(brain.get());
        associativeMemory->setLearningRate(0.01f);
        associativeMemory->setPatternSimilarityThreshold(0.7f);
        associativeMemory->setPatternCapacity(500);
    }
    
    setupMemoryNeurons(brain);
}

void BrainIntegration::updateMemorySystems(std::shared_ptr<Brain> brain, SimulationStep currentStep,
                                          Timestamp currentTime) {
    // Update working memory with current neural activity
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->update(0.001f); // Use timestep from brain
        integrateMemoryWithNeurons(brain);
    }
    
    // Update episodic memory periodically
    if (currentStep % getReplayInterval(brain->getConfig()) == 0 && 
        currentStep > 0) {
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            // Capture current brain state
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = (brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f);
            
            // Store active neurons with activations
            captureCurrentBrainState(brain, episode);
            
            episodicMemory->storeEpisode(episode);
        }
    }
    
    // Update memory replay during rest
    updateMemoryReplay(brain, currentStep);
}

void BrainIntegration::integrateMemoryWithNeurons(std::shared_ptr<Brain> brain) {
    if (auto* workingMemory = brain->getWorkingMemory()) {
        auto memoryNeurons = workingMemory->getMemoryNeurons();
        
        // Enhance selected neurons based on memory
        float enhancementStrength = 0.5f; // Tunable parameter
        for (NeuronId neuronId : memoryNeurons) {
            brain->injectCurrent(neuronId, enhancementStrength);
        }
        
        // Inhibit competing neurons
        if (auto* attention = brain->getAttention()) {
            attention->inhibitNonSelectedNeurons(memoryNeurons);
        }
    }
}

void BrainIntegration::initializePredictionSystem(std::shared_ptr<Brain> brain) {
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        predictionSystem->initialize(brain.get());
        setupPredictionNeurons(brain);
        integratePredictionWithLearning(brain);
    }
}

void BrainIntegration::updatePredictionSystem(std::shared_ptr<Brain> brain) {
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        predictionSystem->update(brain.get());
        
        // Integrate prediction with current sensory input
        if (auto* novelty = brain->getNovelty()) {
            novelty->update(0.001f);
        }
        
        // Calculate prediction errors
        calculatePredictionErrors(brain);
    }
}

void BrainIntegration::integratePredictionWithLearning(std::shared_ptr<Brain> brain) {
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        predictionSystem->registerPredictionErrorHandler(
            [brain](float error) {
                if (auto* dopamine = brain->getDopamine()) {
                    dopamine->receivePredictionError(error);
                }
            }
        );
    }
}

void BrainIntegration::initializeCognitionSystems(std::shared_ptr<Brain> brain) {
    // Initialize neural planner with enhanced capabilities
    if (auto* planner = brain->getPlanner()) {
        planner->initialize(brain.get());
        planner->setPlanningDepth(8); // Deeper planning
        planner->setActionSelectionThreshold(0.6f);
    }
    
    // Initialize concept formation with better pattern extraction
    if (auto* conceptFormation = brain->getConceptFormation()) {
        conceptFormation->initialize(brain.get());
        conceptFormation->setPatternSimilarityThreshold(0.8f);
        conceptFormation->setConceptPersistence(0.95f);
    }
    
    // Initialize attention system with competitive dynamics
    if (auto* attention = brain->getAttention()) {
        attention->initialize(brain.get());
        attention->setInhibitionStrength(0.6f);
        attention->setExcitationStrength(2.0f);
        attention->setSelectionThreshold(0.7f);
    }
    
    setupCognitionNeurons(brain);
}

void BrainIntegration::updateCognitionSystems(std::shared_ptr<Brain> brain, SimulationStep currentStep) {
    // Update concept formation from current neural patterns
    if (auto* conceptFormation = brain->getConceptFormation()) {
        // Extract current neural patterns
        std::vector<float> currentPattern;
        extractNeuralPattern(brain, currentPattern);
        
        conceptFormation->processPattern(currentPattern);
    }
    
    // Update attention and planning
    if (auto* attention = brain->getAttention() && 
        auto* workingMemory = brain->getWorkingMemory()) {
        
        auto competitors = workingMemory->getMemoryNeurons();
        if (!competitors.empty()) {
            attention->processCompetition(competitors);
        }
    }
    
    if (auto* planner = brain->getPlanner()) {
        planner->update(0.001f);
    }
    
    // Integrate cognition with memory
    integrateCognitionWithMemory(brain);
}

void BrainIntegration::integrateCognitionWithMemory(std::shared_ptr<Brain> brain) {
    // Use episodic memory to guide planning
    if (auto* planner = brain->getPlanner() &&
        auto* episodicMemory = brain->getEpisodicMemory()) {
        
        auto pastEpisodes = episodicMemory->getRecentEpisodes(5); // Look at recent experience
        for (const auto* episode : pastEpisodes) {
            planner->considerPreviousOutcome(episode->reward);
        }
    }
    
    // Use working memory to focus attention
    if (auto* attention = brain->getAttention() &&
        auto* workingMemory = brain->getWorkingMemory()) {
        
        auto activeNeurons = workingMemory->getActiveNeurons();
        attention->focusOnActiveNeurons(activeNeurons);
    }
}

void BrainIntegration::initializeNeuromodulationSystems(std::shared_ptr<Brain> brain) {
    // Enhanced neuromodulation initialization
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->initialize(brain.get());
        dopamine->setPlasticityFactor(1.0f);
        dopamine->setResponseTime(0.1f);
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->initialize(brain.get());
        curiosity->setExplorationRate(0.3f);
        curiosity->setNoveltyThreshold(0.5f);
    }
    
    if (auto* novelty = brain->getNovelty()) {
        novelty->initialize(brain.get());
        novelty->setSensitivity(0.8f);
        novelty->setDecayRate(0.9f);
    }
    
    if (auto* predictionError = brain->getPredictionErrorSignal()) {
        predictionError->initialize(brain.get());
        predictionError->setErrorThreshold(0.3f);
        predictionError->setLearningRate(0.05f);
    }
    
    setupNeuromodulationNeurons(brain);
}

void BrainIntegration::updateNeuromodulationSystems(std::shared_ptr<Brain> brain) {
    // Update novelty detection based on sensory changes
    if (auto* novelty = brain->getNovelty()) {
        novelty->update(0.001f);
    }
    
    // Update curiosity based on novelty and exploration rewards
    if (auto* curiosity = brain->getCuriosity()) {
        float noveltyLevel = (brain->getNovelty() ? brain->getNovelty()->getLevel() : 0.0f);
        curiosity->update(noveltyLevel, 0.001f);
    }
    
    // Update dopamine (reward prediction error)
    if (auto* dopamine = brain->getDopamine()) {
        // Calculate prediction error from reward
        float reward = (brain->getEnvironment() ? 
                       brain->getEnvironment()->getLastReward() : 0.0f);
        dopamine->update(reward, 0.001f);
    }
    
    // Update prediction error signal
    if (auto* predictionError = brain->getPredictionErrorSignal()) {
        predictionError->update(0.001f);
    }
    
    // Integrate neuromodulation with plasticity
    integrateNeuromodulationWithPlasticity(brain);
}

void BrainIntegration::integrateNeuromodulationWithPlasticity(std::shared_ptr<Brain> brain) {
    // Apply neuromodulatory effects to plasticity
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        
        // Scale plasticity based on dopamine
        float plasticityScale = 1.0f + dopamineLevel * 0.5f; // Enhanced plasticity
        
        if (auto* sp = brain->getSTDP()) {
            sp->setLTPWeight(0.01f * plasticityScale);
            sp->setLTDWeight(0.012f * plasticityScale);
        }
        
        if (auto* hebbian = brain->getHebbian()) {
            hebbian->setLearningRate(0.01f * plasticityScale);
        }
    }
    
    // Apply curiosity effects to exploration
    if (auto* curiosity = brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        
        // Increase exploration in motor neuron populations
        if (curiosityLevel > 0.5f) {
            brain->injectCurrentToNeurons(NeuronType::Motor, curiosityLevel * 0.1f);
        }
    }
}

void BrainIntegration::initializeDevelopmentSystem(std::shared_ptr<Brain> brain) {
    if (auto* developmentSystem = brain->getDevelopmentSystem()) {
        developmentSystem->initialize(brain.get());
        
        // Set developmental parameters
        developmentSystem->setMaturationRate(0.001f);
        developmentSystem->setPruningRate(0.0001f);
        developmentSystem->setSynaptogenesisRate(0.0002f);
    }
    
    // Initialize with initial developmental stage
    brain->setDevelopmentalStage(DevelopmentalStage::Initial);
}

void BrainIntegration::updateDevelopmentSystem(std::sharedpected<Brain> brain, SimulationStep currentStep,
                                              Timestamp currentTime, TimestepDuration timestep) {
    if (auto* developmentSystem = brain->getDevelopmentSystem()) {
        developmentSystem->update(brain.get(), *brain->getRandomGenerator(), timestep * 1000);
    }
    
    // Update developmental stage based on time and activity
    updateDevelopmentalStage(brain, currentStep, currentTime);
    
    // Apply development effects to plasticity
    if (currentStep % 1000 == 0) { // Every 1000 steps
        integrateDevelopmentWithPlasticity(brain, brain->getDevelopmentalStage());
    }
}

void BrainIntegration::integrateDevelopmentWithPlasticity(std::shared_ptr<Brain> brain, DevelopmentalStage stage) {
    if (auto* structuralPlasticity = brain->getStructuralPlasticity()) {
        float plasticityMod = 1.0f;
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.5f; // High plasticity in initial stage
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.5f; // Low plasticity in adult
                break;
        }
        
        // Apply to structural plasticity rates
        float synaptogenesisRate = getSynaptogenesisRate(brain->getConfig(), stage);
        float pruningRate = getPruningRate(brain->getConfig(), stage);
        
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate * plasticityMod);
        structuralPlasticity->setPruningRate(pruningRate * (2.0f - plasticityMod));
    }
}

void BrainIntegration::setupCheckpointSystem(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (auto* checkpointManager = brain->getCheckpointManager()) {
        std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
}

bool BrainIntegration::saveState(std::shared_ptr<Brain> brain, const std::string& filepath) const {
    // Use checkpoint system to save state
    if (auto* checkpointManager = brain->getCheckpointManager()) {
        return checkpointManager->save(brain.get(), filepath);
    }
    return false;
}

bool BrainIntegration::loadState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    // Use checkpoint system to load state
    if (auto* checkpointManager = brain->getCheckpointManager()) {
        return checkpointManager->load(brain.get(), filepath);
    }
    return false;
}

void BrainIntegration::updateMemoryReplay(std::shared_ptr<Brain> brain, SimulationStep currentStep) {
    if (currentStep % getReplayInterval(brain->getConfig()) == 0 && currentStep > 0) {
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            // Replay recent episodes during rest
            if (!brain->isResting()) {
                brain->enterRestState();
            }
            
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
            }
            
            brain->exitRestState();
        }
    }
}

void BrainIntegration::updateMemoryConsolidation(std::shared_ptr<Brain> brain, SimulationStep currentStep) {
    if (currentStep % getConsolidationInterval(brain->getConfig()) == 0 && currentStep > 0) {
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            // Consolidate important memories
            episodicMemory->consolidate(0.3f); // Consolidate 30% of memories
        }
    }
}

void BrainIntegration::integrateSleepCycle(std::shared_ptr<Brain> brain, SimulationStep currentStep) {
    // Implement sleep/rest cycle for memory consolidation
    static SimulationStep lastSleepStep = 0;
    static bool inSleep = false;
    
    if (currentStep - lastSleepStep >= 10000) { // Sleep every 10000 steps
        if (!inSleep) {
            brain->enterRestState();
            inSleep = true;
            lastSleepStep = currentStep;
        } else {
            brain->exitRestState();
            inSleep = false;
        }
    }
}

void BrainIntegration::updateIntegrationStatistics(std::shared_ptr<Brain> brain) {
    // Update cross-system statistics
    if (auto* workingMemory = brain->getWorkingMemory()) {
        // Update memory statistics
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        // Update memory statistics
    }
    
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        // Update prediction statistics
    }
}

void BrainIntegration::logIntegrationStatus(const Brain* brain) const {
    NLM_LOG_INFO("=== Integrated Brain Status ===");
    
    // Log each integrated system status
    if (auto* workingMemory = brain->getWorkingMemory()) {
        NLM_LOG_INFO("Working Memory: " + std::to_string(workingMemory->getActiveTraces()) + " traces");
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        NLM_LOG_INFO("Episodic Memory: " + std::to_string(episodicMemory->getEpisodeCount()) + " episodes");
    }
    
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        NLM_LOG_INFO("Prediction System: Active");
    }
    
    if (auto* planner = brain->getPlanner()) {
        NLM_LOG_INFO("Cognition: Planning depth " + std::to_string(planner->getPlanningDepth()));
    }
    
    if (auto* dopamine = brain->getDopamine()) {
        NLM_LOG_INFO("Neuromodulation: Dopamine level " + std::to_string(dopamine->getLevel()));
    }
}

void BrainIntegration::resetAllIntegratedSystems(std::shared_ptr<Brain> brain) {
    // Reset all integrated systems
    if (auto* workingMemory = brain->getWorkingMemory()) workingMemory->clear();
    if (auto* episodicMemory = brain->getEpisodicMemory()) episodicMemory->clear();
    if (auto* associativeMemory = brain->getAssociativeMemory()) associativeMemory->clear();
    if (auto* attention = brain->getAttention()) attention->reset();
    if (auto* predictionSystem = brain->getPredictionSystem()) predictionSystem->reset();
    if (auto* planner = brain->getPlanner()) planner->reset();
    if (auto* conceptFormation = brain->getConceptFormation()) conceptFormation->reset();
    if (auto* dopamine = brain->getDopamine()) dopamine->reset();
    if (auto* curiosity = brain->getCuriosity()) curiosity->reset();
    if (auto* novelty = brain->getNovelty()) novelty->reset();
    if (auto* predictionError = brain->getPredictionErrorSignal()) predictionError->reset();
}

void BrainIntegration::updatePlasticityFromNeuromodulation(std::shared_ptr<Brain> brain) {
    // Apply neuromodulatory effects to plasticity rules
    if (auto* dopamine = brain->getDopamine()) {
        float plasticityFactor = dopamine->getPlasticityFactor();
        
        // Scale STDP learning rates
        if (auto* sp = brain->getSTDP()) {
            sp->setLTPWeight(0.01f * plasticityFactor);
            sp->setLTDWeight(0.012f * plasticityFactor);
        }
        
        // Scale Hebbian learning
        if (auto* hebbian = brain->getHebbian()) {
            hebbian->setLearningRate(0.01f * plasticityFactor);
        }
    }
}

void BrainIntegration::applyDevelopmentEffectsToPlasticity(std::shared_ptr<Brain> brain, DevelopmentalStage stage) {
    if (auto* structuralPlasticity = brain->getStructuralPlasticity()) {
        float plasticityMod = 1.0f;
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.5f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 1.2f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.5f;
                break;
        }
        
        structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
        structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

void BrainIntegration::setupMemoryNeurons(std::shared_ptr<Brain> brain) {
    // Connect memory systems to specific neuron populations
    if (auto* region = brain->getRegion(RegionId(2))) {
        // Create dedicated memory region
        auto* wmPop = region->addPopulation(100, NeuronType::Internal);
        auto* emPop = region->addPopulation(50, NeuronType::Internal);
        auto* amPop = region->addPopulation(50, NeuronType::Internal);
        
        NLM_LOG_INFO("Created memory system neurons: " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
}

void BrainIntegration::setupPredictionNeurons(std::shared_ptr<Brain> brain) {
    // Create prediction system neurons
    if (auto* region = brain->getRegion(RegionId(3))) {
        // Prediction system neurons for forward model
        auto* predPop = region->addPopulation(75, NeuronType::Internal);
        NLM_LOG_INFO("Created prediction system neurons: " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
}

void BrainIntegration::setupCognitionNeurons(std::shared_ptr<Brain> brain) {
    // Create cognition system neurons
    if (auto* region = brain->getRegion(RegionId(4))) {
        // Planning and attention neurons
        auto* plannerPop = region->addPopulation(50, NeuronType::Internal);
        auto* attentionPop = region->addPopulation(50, NeuronType::Internal);
        auto* conceptPop = region->addPopulation(30, NeuronType::Internal);
        
        NLM_LOG_INFO("Created cognition system neurons: " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
}

void BrainIntegration::setupNeuromodulationNeurons(std::shared_ptr<Brain> brain) {
    // Create neuromodulation system neurons
    if (auto* region = brain->getRegion(RegionId(5))) {
        // Neuromodulator neuron populations
        auto* dopaminePop = region->addPopulation(40, NeuronType::Internal);
        auto* curiosityPop = region->addPopulation(30, NeuronType::Internal);
        auto* noveltyPop = region->addPopulation(30, NeuronType::Internal);
        auto* errorPop = region->addPopulation(30, NeuronType::Internal);
        
        NLM_LOG_INFO("Created neuromodulation system neurons: " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
    }
}

void BrainIntegration::captureCurrentBrainState(std::shared_ptr<Brain> brain, EpisodicMemoryItem& episode) {
    // Capture current brain state for episodic memory
    episode.activeNeurons.clear();
    episode.neuronActivations.clear();
    
    // Sample active neurons from all regions
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                if (neuron->isFiring() || 
                    std::abs(state.membranePotential - state.restingPotential) > 5.0f) {
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(
                        std::abs(state.membranePotential - state.restingPotential) / 20.0f);
                }
            }
        }
    }
}

void BrainIntegration::extractNeuralPattern(std::shared_ptr<Brain> brain, std::vector<float>& pattern) const {
    // Extract current neural activity pattern for concept formation
    pattern.clear();
    
    // Sample neural activity from key regions
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            // Calculate average activation for region
            float totalActivation = 0.0f;
            for (auto* neuron : neurons) {
                const auto& state = neuron->getState();
                totalActivation += std::abs(state.membranePotential - state.restingPotential);
            }
            float avgActivation = totalActivation / neurons.size();
            pattern.push_back(avgActivation);
        }
    }
}

void BrainIntegration::calculatePredictionErrors(std::shared_ptr<Brain> brain) {
    if (auto* predictionError = brain->getPredictionErrorSignal()) {
        // Simple prediction error calculation based on novelty
        float noveltyLevel = (brain->getNovelty() ? brain->getNovelty()->getLevel() : 0.0f);
        predictionError->updatePredictionError(noveltyLevel);
    }
}

void BrainIntegration::calculateNoveltySignals(std::shared_ptr<Brain> brain) {
    if (auto* novelty = brain->getNovelty()) {
        // Calculate novelty based on changes in neural activity
        float activityVariance = calculateNeuralActivityVariance(brain.get());
        novelty->updateNovelty(activityVariance);
    }
}

void BrainIntegration::calculateCuriositySignals(std::shared_ptr<Brain> brain) {
    if (auto* curiosity = brain->getCuriosity()) {
        // Calculate curiosity based on prediction errors and novelty
        float predictionError = (brain->getPredictionErrorSignal() ? 
                               brain->getPredictionErrorSignal()->getLevel() : 0.0f);
        float noveltyLevel = (brain->getNovelty() ? brain->getNovelty()->getLevel() : 0.0f);
        
        curiosity->updateCuriosity(predictionError + noveltyLevel, 0.001f);
    }
}

float BrainIntegration::calculateNeuralActivityVariance(const Brain* brain) const {
    std::vector<float> activities;
    
    // Collect neural activities from all regions
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                activities.push_back(std::abs(state.membranePotential - state.restingPotential));
            }
        }
    }
    
    if (activities.empty()) return 0.0f;
    
    // Calculate variance
    float mean = std::accumulate(activities.begin(), activities.end(), 0.0f) / activities.size();
    float variance = 0.0f;
    for (float activity : activities) {
        variance += (activity - mean) * (activity - mean);
    }
    variance /= activities.size();
    
    return variance;
}

size_t BrainIntegration::getReplayInterval(std::shared_ptr<Config> config) const {
    return config->getOr<size_t>("replay_interval", 100);
}

size_t BrainIntegration::getConsolidationInterval(std::shared_ptr<Config> config) const {
    return config->getOr<size_t>("consolidation_interval", 1000);
}

float BrainIntegration::getSynaptogenesisRate(std::shared_ptr<Config> config, DevelopmentalStage stage) const {
    float baseRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            return baseRate * 1.5f;
        case DevelopmentalStage::CriticalPeriod:
            return baseRate * 1.2f;
        case DevelopmentalStage::Maturation:
            return baseRate * 0.8f;
        case DevelopmentalStage::Adult:
            return baseRate * 0.5f;
        default:
            return baseRate;
    }
}

float BrainIntegration::getPruningRate(std::shared_ptr<Config> config, DevelopmentalStage stage) const {
    float baseRate = config->getOr<float>("pruning_rate", 0.00001f);
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            return baseRate * 0.5f;
        case DevelopmentalStage::CriticalPeriod:
            return baseRate * 0.8f;
        case DevelopmentalStage::Maturation:
            return baseRate * 1.2f;
        case DevelopmentalStage::Adult:
            return baseRate * 1.5f;
        default:
            return baseRate;
    }
}

} // namespace nlm