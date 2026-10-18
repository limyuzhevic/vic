#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Endorphins.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/MetaplasticityManager.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <deque>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Endorphins> endorphins;
    std::unique_ptr<MetaplasticityManager> metaplasticity;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(200)  // Consolidate every 200 steps
    {
    }
    
    ~Impl() {
    }
    
    void initialize() {
        // Initialize components with proper order
        if (config) {
            // Configure timestep based on config
            timestep = config->getDouble("simulation/timestep", 0.001);
        }
        
        // Initialize random generator
        rng = std::make_unique<RandomGenerator>("default", config.get());
        
        // Create basic brain regions (sensory, motor, association)
        createBasicRegions();
        
        // Initialize memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        serotonin = std::make_unique<Serotonin>();
        norepinephrine = std::make_unique<Norepinephrine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        endorphins = std::make_unique<Endorphins>();
        metaplasticity = std::make_unique<MetaplasticityManager>();
        
        // Set all systems to reference the brain
        
        // Initialize neural computation components
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Set all systems to reference the brain
        workingMemory->initialize(this);
        episodicMemory->initialize(this);
        associativeMemory->initialize(this);
        predictionSystem->initialize(this);
        planner->initialize(this);
        conceptFormation->initialize(this);
        developmentSystem->initialize(this);
        dopamine->initialize(this);
        curiosity->initialize(this);
        predictionError->initialize(this);
        novelty->initialize(this);
        serotonin->initialize(this);
        norepinephrine->initialize(this);
        acetylcholine->initialize(this);
        endorphins->initialize(this);
        metaplasticity->initialize(this);
        spikeSystem->initialize(this);
        stdp->initialize(this);
        hebbian->initialize(this);
        structuralPlasticity->initialize(this);
        
        // Initialize attentional selection with neuromodulator references
        attention->initialize(this, acetylcholine.get(), norepinephrine.get(), 
                            dopamine.get(), serotonin.get(), curiosity.get());
        
        NLM_LOG_INFO("Brain initialized with all integrated systems");
    }
    
    void createBasicRegions() {
        // Create sensory input region
        RegionId sensoryId = addRegion("Sensory");
        NeuralRegion* sensoryRegion = getRegion(sensoryId);
        
        // Create motor output region
        RegionId motorId = addRegion("Motor");
        NeuralRegion* motorRegion = getRegion(motorId);
        
        // Create association region
        RegionId associationId = addRegion("Association");
        NeuralRegion* associationRegion = getRegion(associationId);
        
        // Connect regions with strong recurrent connections
        addInterRegionConnection(sensoryId, associationId, 0.8f, 1);
        addInterRegionConnection(associationId, motorId, 0.8f, 1);
        addInterRegionConnection(motorId, associationId, 0.5f, 1);
        addInterRegionConnection(associationId, sensoryId, 0.3f, 1);
        
        // Create place cells in association region for spatial memory
        for (float x = -5.0f; x <= 5.0f; x += 1.0f) {
            for (float y = -5.0f; y <= 5.0f; y += 1.0f) {
                // Create place cell neuron
                Neuron* placeCell = new Neuron(NeuronType::Association, x * 1000, y * 1000);
                placeCell->setRestingPotential(0.0f);
                placeCell->setThreshold(1.0f);
                
                // Add to association region
                associationRegion->addNeuron(placeCell);
                
                // Store for reference
                sensoryNeurons.push_back(placeCell);
            }
        }
    }
    
    void step(SimulationStep currentStep) {
        step(currentStep, 0.0);  // Default timestamp
    }
    
    void step(SimulationStep currentStep, Timestamp currentTime) {
        this->currentStep = currentStep;
        this->currentTime = currentTime;
        
        // Reset step statistics
        totalSpikesThisStep = 0;
        
        // ========== STEP 1: Update neuromodulation ==========
        if (dopamine) dopamine->update(pImpl->timestep);
        if (curiosity) curiosity->update(pImpl->timestep);
        if (predictionError) predictionError->update(pImpl->timestep);
        if (novelty) novelty->update(pImpl->timestep);
        if (serotonin) serotonin->update(pImpl->timestep);
        if (norepinephrine) norepinephrine->update(pImpl->timestep);
        if (acetylcholine) acetylcholine->update(pImpl->timestep);
        if (endorphins) endorphins->update(pImpl->timestep);
        
        // ========== STEP 2: Update spike dynamics ==========
        if (spikeSystem) {
            spikeSystem->update(this, pImpl->timestep);
            totalSpikesThisStep = spikeSystem->getTotalSpikeCount();
        }
        
        // ========== STEP 3: Update synaptic plasticity ==========
        if (stdp) stdp->update(this, pImpl->timestep);
        if (hebbian) hebbian->update(this, pImpl->timestep);
        
        // ========== STEP 4: Update working memory ==========
        if (workingMemory) {
            workingMemory->update(pImpl->timestep);
        }
        
        // ========== STEP 5: Update episodic memory ==========
        stepsSinceLastEpisode++;
        if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            stepsSinceLastEpisode = 0;
            
            if (episodicMemory) {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                episode.novelty = novelty ? novelty->getLevel() : 0.0f;
                
                // Store active neurons
                for (auto& region : regions) {
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
                
                // Store sensory state
                if (workingMemory) {
                    std::vector<float> wmPattern = workingMemory->retrieve();
                    if (!wmPattern.empty()) {
                        episode.sensoryState = wmPattern;
                    }
                }
                
                // Integrate episodic memory: store the episode
                episodicMemory->storeEpisode(episode);
                
                // Integrate episodic memory: use it for pattern completion
                if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                    // Create a query from working memory state
                    std::vector<float> wmPattern = workingMemory->retrieve();
                    
                    // Retrieve similar episodes from episodic memory
                    auto similarEpisodes = episodicMemory->retrieveSimilar(wmPattern, 3);
                    
                    // Use completed patterns to strengthen relevant synapses
                    if (dopamine) {
                        float learningRate = dopamine->getLevel() * 0.05f;
                        
                        for (const auto* episode : similarEpisodes) {
                            // Reinforce synapses that were active in the episode
                            for (size_t i = 0; i < episode->activeNeurons.size() && i < episode->neuronActivations.size(); ++i) {
                                NeuronId neuronId = episode->activeNeurons[i];
                                float activation = episode->neuronActivations[i];
                                
                                if (activation > 0.5f) {  // Strong activation
                                    for (auto& region : regions) {
                                        // Find neuron and strengthen connections
                                        for (auto& pop : region->getPopulations()) {
                                            for (auto* neuron : pop->getNeurons()) {
                                                if (neuron->getId() == neuronId) {
                                                    // Apply LTP based on episode similarity
                                                    for (auto* syn : region->getSynapsesFrom(neuron->getId())) {
                                                        float currentWeight = syn->getWeight();
                                                        if (currentWeight > 0) {
                                                            float weightChange = learningRate * activation;
                                                            syn->setWeight(std::min(1.0f, currentWeight + weightChange));
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Integrate episodic memory: use it to guide future actions
                if (planner && !similarEpisodes.empty() && currentStep % 100 == 0) {
                    // Use successful actions from similar episodes to guide planning
                    std::vector<ActionType> successfulActions;
                    
                    for (const auto* episode : similarEpisodes) {
                        if (episode->action != ActionType::Wait && episode->reward > 0) {
                            successfulActions.push_back(episode->action);
                        }
                    }
                    
                    if (!successfulActions.empty()) {
                        // Set goal based on successful action patterns
                        std::vector<float> goalPattern(successfulActions.size(), 0.5f);
                        planner->setCurrentGoal(goalPattern);
                    }
                }
            }
        }
        
        // ========== STEP 6: Update prediction system ==========
        if (predictionSystem) {
            // Integrate prediction system: use prediction errors to modulate learning
            if (predictionSystem && dopamine) {
                // Get prediction error from the system
                float predictionError = predictionSystem->getPredictionError();
                
                // Use prediction error to modulate dopamine (learning signal)
                if (predictionError > 0.1f || predictionError < -0.1f) {
                    // Strong prediction error should increase dopamine level
                    float errorMagnitude = std::abs(predictionError);
                    float dopamineMod = 1.0f + errorMagnitude * 2.0f; // Amplify error signal
                    float currentDopamine = dopamine->getLevel();
                    dopamine->setLevel(currentDopamine * dopamineMod);
                }
                
                // Use prediction error to modulate plasticity
                if (predictionError > 0.1f) {
                    // Positive prediction error (surprise) should enhance learning
                    for (auto& region : regions) {
                        for (auto& syn : region->getSynapses()) {
                            if (syn->getPlasticityFlags().stdp || syn->getPlasticityFlags().hebbian) {
                                float currentWeight = syn->getWeight();
                                float errorInfluence = predictionError * 0.01f;
                                
                                // Apply error-based weight modification
                                if (currentWeight > 0) {
                                    syn->setWeight(std::min(1.0f, currentWeight + errorInfluence));
                                } else {
                                    syn->setWeight(std::max(-1.0f, currentWeight - errorInfluence));
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // ========== STEP 7: Update attention system ==========
        if (attention) {
            attention->update(currentStep);
            
            // Apply attention to working memory winners
            if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
                attention->processCompetition(competitors);
            }
            
            // Integrate cognitive systems: use attention to focus on prediction errors
            if (predictionError && workingMemory) {
                float predictionErrorValue = predictionError->getError();
                if (std::abs(predictionErrorValue) > 0.5f) {
                    // Focus attention on high prediction error events
                    float errorFocus = std::min(1.0f, std::abs(predictionErrorValue));
                    
                    // Apply top-down attention bias to sensory processing areas
                    for (auto& region : regions) {
                        // Identify sensory regions
                        bool isSensory = false;
                        for (auto& pop : region->getPopulations()) {
                            if (pop->getNeuronType() == NeuronType::Sensory) {
                                isSensory = true;
                                break;
                            }
                        }
                        
                        if (isSensory && workingMemory) {
                            // Boost attentional gain on sensory regions when prediction error is high
                            for (auto* neuron : region->getAllNeurons()) {
                                attention->applyBottomUpSalience(neuron->getId(), errorFocus);
                            }
                        }
                    }
                }
            }
        }
        
        // ========== STEP 8: Update concept formation ==========
        if (conceptFormation) {
            // Integrate cognitive systems: use ConceptFormation to generalize from episodic memory
            if (episodicMemory) {
                // Get recent episodes to form concepts from
                auto recentEpisodes = episodicMemory->getRecentEpisodes(10);
                
                if (!recentEpisodes.empty()) {
                    // Extract sensory patterns from episodes
                    std::vector<std::vector<float>> episodePatterns;
                    for (const auto* episode : recentEpisodes) {
                        if (!episode->sensoryState.empty()) {
                            episodePatterns.push_back(episode->sensoryState);
                        }
                    }
                    
                    if (!episodePatterns.empty()) {
                        // Use concept formation to generalize patterns
                        for (const auto& pattern : episodePatterns) {
                            // Present patterns to concept formation system
                            size_t conceptId = conceptFormation->presentExperience(
                                pattern, std::vector<float>(pattern.size(), 1.0f), 0.5f, currentStep);
                            
                            // Integrate with NeuralPlanner
                            conceptFormation->integrateWithNeuralPlanner(conceptId);
                            
                            // Link to PredictionSystem
                            conceptFormation->linkToPredictionSystem(conceptId);
                            
                            // Connect to WorkingMemory
                            conceptFormation->connectToWorkingMemory(conceptId);
                        }
                        
                        // Apply concept-based plasticity to strengthen common patterns
                        if (dopamine) {
                            float learningRate = dopamine->getLevel() * 0.02f;
                            
                            // Apply concept-based learning to strengthen common patterns
                            for (const auto& pattern : episodePatterns) {
                                for (auto& region : regions) {
                                    for (auto& syn : region->getSynapses()) {
                                        // Concept-based plasticity: strengthen synapses that are
                                        // commonly active across similar experiences
                                        bool patternActive = false;
                                        for (NeuronId neuronId : syn->getSourceNeuron()) {
                                            // Check if neuron was active in any episode pattern
                                            for (float activation : pattern) {
                                                if (activation > 0.5f) {
                                                    patternActive = true;
                                                    break;
                                                }
                                            }
                                        }
                                        
                                        if (patternActive && (syn->getPlasticityFlags().stdp || syn->getPlasticityFlags().hebbian)) {
                                            float weightChange = learningRate * 0.1f;
                                            float currentWeight = syn->getWeight();
                                            syn->setWeight(std::min(1.0f, currentWeight + weightChange));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // ========== STEP 9: Apply structural plasticity periodically ==========
        if (currentStep % 100 == 0) {
            structuralPlasticity->update(this, *rng);
        }
        
    // ========== STEP 10: REPLAY IMPORTANT MEMORIES ==========
    if (currentStep % replayInterval == 0 && episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 11: ADVANCED SLEEP/REPLAY CYCLE ==========
    // Check if brain should enter rest state (e.g., after long period without stimulation)
    if (isResting && currentStep % 1000 == 0) {
        // Enter sleep state for offline replay and consolidation
        if (episodicMemory) {
            episodicMemory->enterSleepState();
            // Perform sleep-based replay for consolidation
            episodicMemory->replayDuringSleep(0.8f);
        }
    }
    
    if (currentStep % 5000 == 0) {
        // Exit rest state
        isResting = false;
        if (episodicMemory) {
            episodicMemory->exitSleepState();
        }
    }
    
    // ========== STEP 12: SYNAPTIC REPLAY ==========
    if (currentStep % 200 == 0 && episodicMemory) {
        // Select episodes based on priority for synaptic replay
        std::vector<size_t> priorityEpisodes = episodicMemory->selectPriorityEpisodes(2);
        if (!priorityEpisodes.empty()) {
            // Synaptic replay to strengthen connections
            episodicMemory->replaySynapticPatterns(priorityEpisodes, 1.2f);
        }
    }
    
    // ========== STEP 13: SEQUENCE REPLAY ==========
    if (currentStep % 300 == 0 && episodicMemory) {
        // Extract and replay temporal sequences
        std::vector<std::vector<size_t>> temporalSequences = 
            episodicMemory->extractTemporalSequences(3);
        for (const auto& sequence : temporalSequences) {
            if (!sequence.empty()) {
                episodicMemory->replayTemporalSequences(sequence, true);
            }
        }
    }
    
    // ========== STEP 14: DEVELOPMENTAL STAGE REPLAY ==========
    if (currentStep % 1000 == 0 && episodicMemory) {
        DevelopmentalStage stage = developmentSystem ? developmentSystem->getStage() : DevelopmentalStage::Adult;
        // Stage-specific replay strategies
        episodicMemory->replayForDevelopmentalStage(stage, 1.0f);
    }
    
    // ========== STEP 15: NEUROMODULATOR-GATED REPLAY ==========
    if (dopamine && currentStep % 150 == 0 && episodicMemory) {
        // Dopamine-gated replay for reward-based consolidation
        Neuromodulator dopamineSignal;
        dopamineSignal.type = NeuromodulatorType::Dopamine;
        dopamineSignal.level = dopamine->getLevel();
        episodicMemory->neuromulatorGatedReplay(dopamineSignal, 0.6f);
    }
    
    if (acetylcholine && currentStep % 200 == 0 && episodicMemory) {
        // Acetylcholine-gated replay for attention-based strengthening
        Neuromodulator acetylcholineSignal;
        acetylcholineSignal.type = NeuromodulatorType::Acetylcholine;
        acetylcholineSignal.level = acetylcholine->getLevel();
        episodicMemory->neuromodulatorGatedReplay(acetylcholineSignal, 0.7f);
    }
    
    // ========== STEP 16: METAPLASTIC REPLAY EFFECTS ==========
    if (currentStep % 800 == 0 && episodicMemory && metaplasticity) {
        // Apply metaplastic replay effects based on learning history
        std::vector<float> learningHistory;
        // Get learning history from metaplasticity manager
        // Simplified: use a fixed history for demonstration
        for (size_t i = 0; i < 5; ++i) {
            learningHistory.push_back(0.5f + 0.1f * std::sin(currentStep * 0.01f + i));
        }
        std::vector<size_t> episodesForMetaplastic = episodicMemory->selectPriorityEpisodes(3);
        episodicMemory->applyMetaplasticReplayEffects(episodesForMetaplastic, learningHistory);
    }
    
    // ========== STEP 17: MULTI-SYSTEM REPLAY ==========
    if (currentStep % 400 == 0 && episodicMemory && workingMemory && conceptFormation) {
        // Cross-system replay between working memory, episodic memory, and concepts
        std::vector<size_t> crossSystemEpisodes = episodicMemory->selectPriorityEpisodes(2);
        episodicMemory->crossSystemReplay(crossSystemEpisodes, workingMemory, conceptFormation);
    }
    
    // ========== STEP 18: CONSOLIDATION-GRADUATED REPLAY =========
    if (currentStep % 600 == 0 && episodicMemory) {
        // Progressive strengthening during consolidation
        // Use different consolidation stages
        size_t consolidationStage = (currentStep / 600) % 3;  // 0, 1, or 2
        episodicMemory->graduatedConsolidationReplay(consolidationStage, 1.1f);
    }
    
    // ========== STEP 19: ATTENTION-GUIDED REPLAY ==========
    if (attention && currentStep % 100 == 0 && episodicMemory && workingMemory) {
        // Get attention weights from attentional selection system
        std::vector<float> attentionWeights;
        const auto& winners = attention->getWinners();
        attentionWeights.reserve(winners.size());
        for (size_t i = 0; i < std::min<size_t>(winners.size(), 10); ++i) {
            attentionWeights.push_back(0.5f + 0.5f * (i + 1) / 10.0f);
        }
        std::vector<size_t> attentionEpisodes = episodicMemory->selectPriorityEpisodes(3);
        episodicMemory->attentionGuidedReplay(attentionEpisodes, attentionWeights);
    }
    
    // ========== STEP 20: PREDICTION ERROR-GUIDED REPLAY ==========
    if (predictionError && currentStep % 250 == 0 && episodicMemory) {
        // Use prediction error to guide replay
        float predictionErrorValue = predictionError->getError();
        std::vector<size_t> predictionEpisodes = episodicMemory->selectPriorityEpisodes(2);
        episodicMemory->predictionErrorGuidedReplay(predictionEpisodes, std::abs(predictionErrorValue));
    }
    
    // ========== STEP 21: NEUROMODULATOR-GATED REPLAY ==========
    // Dopamine-gated replay for reward-based consolidation
    if (dopamine && currentStep % 150 == 0 && episodicMemory) {
        Neuromodulator dopamineSignal;
        dopamineSignal.type = NeuromodulatorType::Dopamine;
        dopamineSignal.level = dopamine->getLevel();
        episodicMemory->neuromodulatorGatedReplay(dopamineSignal, 0.6f);
    }
    
    // Acetylcholine-gated replay for attention-based strengthening
    if (acetylcholine && currentStep % 200 == 0 && episodicMemory) {
        Neuromodulator acetylcholineSignal;
        acetylcholineSignal.type = NeuromodulatorType::Acetylcholine;
        acetylcholineSignal.level = acetylcholine->getLevel();
        episodicMemory->neuromodulatorGatedReplay(acetylcholineSignal, 0.7f);
    }
    
    // Norepinephrine-gated replay for salient event capture
    if (norepinephrine && currentStep % 180 == 0 && episodicMemory) {
        Neuromodulator norepinephrineSignal;
        norepinephrineSignal.type = NeuromodulatorType::Norepinephrine;
        norepinephrineSignal.level = norepinephrine->getLevel();
        episodicMemory->neuromodulatorGatedReplay(norepinephrineSignal, 0.5f);
    }
    
    // Serotonin-gated replay for emotional memory processing
    if (serotonin && currentStep % 220 == 0 && episodicMemory) {
        Neuromodulator serotoninSignal;
        serotoninSignal.type = NeuromodulatorType::Serotonin;
        serotoninSignal.level = serotonin->getLevel();
        episodicMemory->neuromodulatorGatedReplay(serotoninSignal, 0.4f);
    }
    
    // ========== STEP 22: CONSOLIDATE LONG-TERM MEMORIES ==========
    if (currentStep % consolidationInterval == 0) {
        if (episodicMemory) {
            episodicMemory->consolidateMemories(currentStep, 0.7f);
        }
        if (workingMemory) {
            workingMemory->consolidateMemories(currentStep, 0.5f);
        }
    }
    }
    
    void receiveSensoryInput(const SensoryInput& input) {
        // Inject sensory input into sensory neurons
        if (!input.getVision().empty() && sensoryNeurons.size() > 0) {
            const auto& vision = input.getVision();
            for (size_t i = 0; i < sensoryNeurons.size() && i < vision.size(); ++i) {
                if (sensoryNeurons[i]) {
                    // Inject current proportional to vision intensity
                    float current = vision[i] * 5.0f;  // Scale factor
                    sensoryNeurons[i]->injectCurrent(current);
                }
            }
        }
        
        // Update working memory with sensory input
        if (workingMemory && !input.getVision().empty()) {
            std::vector<float> visionPattern = input.getVision();
            if (!visionPattern.empty()) {
                workingMemory->store(visionPattern, 1.0f);
            }
        }
        
        // Update prediction system
        if (predictionSystem) {
            predictionSystem->update(input);
        }
        
        // Update novelty detection
        if (novelty) {
            float noveltyLevel = novelty->computeNovelty(input);
        }
        
        // Apply concept formation to sensory input
        if (conceptFormation && !input.getVision().empty()) {
            std::vector<float> pattern = input.getVision();
            std::vector<float> features = input.getTouch();
            float reward = input.getInternal().empty() ? 0.0f : input.getInternal()[0];
            
            size_t conceptId = conceptFormation->presentExperience(
                pattern, features, reward, currentStep);
        }
    }
    
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        // Find neuron and inject current
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuronPtr : pop->getNeurons()) {
                    if (neuronPtr->getId() == neuron) {
                        neuronPtr->injectCurrent(current);
                        return;
                    }
                }
            }
        }
    }
    
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        // Inject current to all neurons of specified type
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
    
    // Brain interface methods
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    const SpikeSystem* getSpikeSystem() const { return spikeSystem.get(); }
    
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
    float getExcitationInhibitionRatio() const {
        float totalExcitation = 0.0f;
        float totalInhibition = 0.0f;
        
        for (const auto& region : regions) {
            for (const auto& syn : region->getSynapses()) {
                if (syn->getWeight() > 0) {
                    totalExcitation += syn->getWeight();
                } else {
                    totalInhibition += std::abs(syn->getWeight());
                }
            }
        }
        
        return totalExcitation / (totalInhibition + 0.0001f);
    }
    
    size_t getTotalSpikeCount() const { return totalSpikesTotal; }
    size_t getPendingSpikeEventCount() const { return spikeSystem ? spikeSystem->getPendingSpikeEventCount() : 0; }
    
    std::unique_ptr<Action> produceAction() {
        // Generate action based on motor neuron activity
        std::unique_ptr<Action> action = std::make_unique<Action>();
        
        // Calculate average activity in motor regions
        float maxActivity = 0.0f;
        ActionType bestAction = ActionType::Wait;
        
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == NeuronType::Motor) {
                    for (auto* neuron : pop->getNeurons()) {
                        float activity = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                        if (activity > maxActivity) {
                            maxActivity = activity;
                            // Map neuron to action type
                            if (neuron->getId() % 6 == 0) bestAction = ActionType::MoveForward;
                            else if (neuron->getId() % 6 == 1) bestAction = ActionType::MoveBackward;
                            else if (neuron->getId() % 6 == 2) bestAction = ActionType::TurnLeft;
                            else if (neuron->getId() % 6 == 3) bestAction = ActionType::TurnRight;
                            else if (neuron->getId() % 6 == 4) bestAction = ActionType::Interact;
                            else bestAction = ActionType::Wait;
                        }
                    }
                }
            }
        }
        
        action->setAction(bestAction);
        action->setConfidence(maxActivity);
        
        return action;
    }
    
    void applyNeuromodulation(const Neuromodulator& signal) {
        switch (signal.type) {
            case NeuromodulatorType::Dopamine:
                if (dopamine) dopamine->applySignal(signal);
                break;
            case NeuromodulatorType::Curiosity:
                if (curiosity) curiosity->applySignal(signal);
                break;
            case NeuromodulatorType::PredictionError:
                if (predictionError) predictionError->applySignal(signal);
                break;
            case NeuromodulatorType::Novelty:
                if (novelty) novelty->applySignal(signal);
                break;
            case NeuromodulatorType::Serotonin:
                if (serotonin) serotonin->applySignal(signal);
                break;
            case NeuromodulatorType::Norepinephrine:
                if (norepinephrine) norepinephrine->applySignal(signal);
                break;
            case NeuromodulatorType::Acetylcholine:
                if (acetylcholine) acetylcholine->applySignal(signal);
                break;
            case NeuromodulatorType::Endorphins:
                if (endorphins) endorphins->applySignal(signal);
                break;
        }
    }
    
    void updatePlasticity() {
        if (stdp) stdp->update(this, timestep);
        if (hebbian) hebbian->update(this, timestep);
    }
    
    void develop() {
        if (developmentSystem) {
            developmentSystem->update(this, *rng, timestep * 100.0f);
        }
    }
    
    void reset() {
        currentStep = 0;
        currentTime = 0.0;
        
        // Reset all components
        if (spikeSystem) spikeSystem->reset();
        if (stdp) stdp->reset();
        if (hebbian) hebbian->reset();
        if (workingMemory) workingMemory->clear();
        if (episodicMemory) episodicMemory->clear();
        if (conceptFormation) conceptFormation->clear();
        if (attention) attention->clear();
        
        // Reset neuromodulators
        if (dopamine) dopamine->reset();
        if (curiosity) curiosity->reset();
        if (predictionError) predictionError->reset();
        if (novelty) novelty->reset();
        if (serotonin) serotonin->reset();
        if (norepinephrine) norepinephrine->reset();
        if (acetylcholine) acetylcholine->reset();
        if (endorphins) endorphins->reset();
        if (developmentSystem) developmentSystem->reset();
    }
    
    bool save(const std::string& filepath) const {
        // Save brain state to file
        std::ofstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        // Write header
        file.write("NLMBRAIN", 8);
        file.write(reinterpret_cast<const char*>(&currentStep), sizeof(SimulationStep));
        file.write(reinterpret_cast<const char*>(&currentTime), sizeof(Timestamp));
        
        // Save regions
        size_t regionCount = regions.size();
        file.write(reinterpret_cast<const char*>(&regionCount), sizeof(size_t));
        
        for (const auto& region : regions) {
            region->save(file);
        }
        
        file.close();
        return true;
    }
    
    bool load(const std::string& filepath) {
        // Load brain state from file
        std::ifstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        char header[9] = {0};
        file.read(header, 8);
        header[8] = '\0';
        
        if (strcmp(header, "NLMBRAIN") != 0) return false;
        
        file.read(reinterpret_cast<char*>(&currentStep), sizeof(SimulationStep));
        file.read(reinterpret_cast<char*>(&currentTime), sizeof(Timestamp));
        
        // Clear existing regions
        regions.clear();
        
        // Load regions
        size_t regionCount;
        file.read(reinterpret_cast<char*>(&regionCount), sizeof(size_t));
        
        for (size_t i = 0; i < regionCount; ++i) {
            regions.push_back(std::make_unique<NeuralRegion>());
            regions.back()->load(file);
        }
        
        file.close();
        return true;
    }
    
    // ========== MEMORY SYSTEM ACCESS ==========
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    const NeuralWorkingMemory* getWorkingMemory() const { return workingMemory.get(); }
    
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    const NeuralEpisodicMemory* getEpisodicMemory() const { return episodicMemory.get(); }
    
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    const NeuralAssociativeMemory* getAssociativeMemory() const { return associativeMemory.get(); }
    
    // ========== PREDICTION SYSTEM ==========
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    const PredictionSystem* getPredictionSystem() const { return predictionSystem.get(); }
    
    // ========== COGNITION SYSTEMS ==========
    NeuralPlanner* getPlanner() { return planner.get(); }
    const NeuralPlanner* getPlanner() const { return planner.get(); }
    
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    const ConceptFormation* getConceptFormation() const { return conceptFormation.get(); }
    
    AttentionalSelection* getAttention() { return attention.get(); }
    const AttentionalSelection* getAttention() const { return attention.get(); }
    
    // ========== DEVELOPMENT SYSTEM ==========
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    const DevelopmentSystem* getDevelopmentSystem() const { return developmentSystem.get(); }
    
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage; }
    void setDevelopmentalStage(DevelopmentalStage stage) { developmentalStage = stage; }
    
    // ========== NEUROMODULATION SYSTEMS ==========
    Dopamine* getDopamine() { return dopamine.get(); }
    const Dopamine* getDopamine() const { return dopamine.get(); }
    
    Curiosity* getCuriosity() { return curiosity.get(); }
    const Curiosity* getCuriosity() const { return curiosity.get(); }
    
    Novelty* getNovelty() { return novelty.get(); }
    const Novelty* getNovelty() const { return novelty.get(); }
    
    PredictionError* getPredictionErrorSignal() { return predictionError.get(); }
    const PredictionError* getPredictionErrorSignal() const { return predictionError.get(); }
    
    Serotonin* getSerotonin() { return serotonin.get(); }
    const Serotonin* getSerotonin() const { return serotonin.get(); }
    
    Norepinephrine* getNorepinephrine() { return norepinephrine.get(); }
    const Norepinephrine* getNorepinephrine() const { return norepinephrine.get(); }
    
    Acetylcholine* getAcetylcholine() { return acetylcholine.get(); }
    const Acetylcholine* getAcetylcholine() const { return acetylcholine.get(); }
    
    Endorphins* getEndorphins() { return endorphins.get(); }
    const Endorphins* getEndorphins() const { return endorphins.get(); }
    
    MetaplasticityManager* getMetaplasticity() { return metaplasticity.get(); }
    const MetaplasticityManager* getMetaplasticity() const { return metaplasticity.get(); }
    
    // ========== NEURAL COMPUTATION ==========
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    const SpikeSystem* getSpikeSystem() const { return spikeSystem.get(); }
    
    STDP* getSTDP() { return stdp.get(); }
    const STDP* getSTDP() const { return stdp.get(); }
    
    Hebbian* getHebbian() { return hebbian.get(); }
    const Hebbian* getHebbian() const { return hebbian.get(); }
    
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    const StructuralPlasticity* getStructuralPlasticity() const { return structuralPlasticity.get(); }
    
    // ========== CONFIGURATION ==========
    std::shared_ptr<const Config> getConfig() const { return config; }
    
    // ========== STATISTICS ==========
    RandomGenerator* getRandomGenerator() { return rng.get(); }
    const RandomGenerator* getRandomGenerator() const { return rng.get(); }
    
    void logStatus() const {
        NLM_LOG_INFO("Brain Status:");
        NLM_LOG_INFO("  Step: " + std::to_string(currentStep));
        NLM_LOG_INFO("  Time: " + std::to_string(currentTime));
        NLM_LOG_INFO("  Neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("  Synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("  Active Neurons: " + std::to_string(getActiveNeuronCount()));
        NLM_LOG_INFO("  Development Stage: " + std::to_string(static_cast<int>(developmentalStage)));
        NLM_LOG_INFO("  Memory Traces: " + std::to_string(workingMemory ? workingMemory->getActiveTraces() : 0));
        NLM_LOG_INFO("  Concepts: " + std::to_string(conceptFormation ? conceptFormation->getConceptCount() : 0));
    }
    
    // ========== REGION MANAGEMENT ==========
    RegionId addRegion(const std::string& name = "") {
        std::string regionName = name.empty() ? "Region" + std::to_string(nextRegionId) : name;
        
        auto region = std::make_unique<NeuralRegion>(regionName);
        RegionId regionId = nextRegionId++;
        region->setId(regionId);
        
        regions.push_back(std::move(region));
        return regionId;
    }
    
    NeuralRegion* getRegion(RegionId id) {
        for (auto& region : regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        for (const auto& region : regions) {
            if (region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    }
    
    size_t getRegionCount() const { return regions.size(); }
    std::vector<RegionId> getRegionIds() const {
        std::vector<RegionId> ids;
        for (const auto& region : regions) {
            ids.push_back(region->getId());
        }
        return ids;
    }
    
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const { return regions; }
    
    // ========== INTER-REGION CONNECTIONS ==========
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1) {
        InterRegionConnection conn(source, target, weight, delay);
        interRegionConnections.push_back(conn);
    }
    
    void removeInterRegionConnection(RegionId source, RegionId target) {
        interRegionConnections.erase(
            std::remove_if(interRegionConnections.begin(), interRegionConnections.end(),
                [source, target](const InterRegionConnection& conn) {
                    return conn.sourceRegion == source && conn.targetRegion == target;
                }),
            interRegionConnections.end()
        );
    }
    
    // ========== GLOBAL STATISTICS ==========
    size_t getTotalNeuronCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            total += region->getNeuronCount();
        }
        return total;
    }
    
    size_t getTotalSynapseCount() const {
        size_t total = 0;
        for (const auto& region : regions) {
            total += region->getSynapseCount();
        }
        return total;
    }
    
    size_t getActiveNeuronCount() const {
        size_t active = 0;
        for (const auto& region : regions) {
            active += region->getActiveNeuronCount();
        }
        return active;
    }
    
    size_t getFiringNeuronCount() const {
        size_t firing = 0;
        for (const auto& region : regions) {
            firing += region->getFiringNeuronCount();
        }
        return firing;
    }
    
    float getAverageFiringRate() const {
        size_t totalFiring = getFiringNeuronCount();
        size_t totalNeurons = getTotalNeuronCount();
        return totalNeurons > 0 ? (float)totalFiring / totalNeurons : 0.0f;
    }

private:
    Impl* pImpl;
    DevelopmentalStage developmentalStage;
    size_t nextRegionId;
};

} // namespace nlm
