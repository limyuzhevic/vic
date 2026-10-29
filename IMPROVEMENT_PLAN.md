# Project Improvement Plan

## Overview
This document outlines comprehensive improvements for the NLM/熙然 project, focusing on integrating disconnected systems and enhancing functionality based on the Phase 6 Final Audit analysis.

## Priority 1: CONNECT EXISTING SYSTEMS (IMMEDIATE)

### 1.1 Memory Integration Improvements

**Issues Identified:**
- Working memory initialized but not used for attention selection
- Episodic memory stores experiences but no consolidation based on importance
- Semantic memory is completely empty
- Procedural memory is completely empty
- Associative memory is completely empty

**Improvements Needed:**

#### A. Enhance Working Memory Integration
**File:** `src/brain/Brain.cpp`

Replace current Step 4 (lines 404-407):
```cpp
// REPLACE with enhanced working memory integration:
pImpl->stepsSinceLastEpisode++;
if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
    pImpl->stepsSinceLastEpisode = 0;
    
    if (pImpl->episodicMemory) {
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : pImpl->regions) {
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
        
        // Store reward in episode
        episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        
        pImpl->episodicMemory->storeEpisode(episode);
    }
}

// ADD after Step 4: Enhanced working memory for attention
if (pImpl->workingMemory && pImpl->attention) {
    // Update working memory with recent sensory input patterns
    pImpl->workingMemory->update(pImpl->timestep);
    
    // Select attention winner based on working memory content
    std::vector<NeuronId> memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
    if (!memoryNeurons.empty()) {
        pImpl->attention->processCompetition(memoryNeurons);
    }
}
```

#### B. Complete Semantic Memory Implementation
**File:** `src/memory/SemanticMemory.hpp` and `src/memory/SemanticMemory.cpp`

Implement actual semantic memory with:
- Pattern recognition from episodic memory
- Concept formation capabilities
- Associative linking between concepts
- Activation spreading mechanism

#### C. Complete Procedural Memory Implementation
**File:** `src/memory/ProceduralMemory.hpp` and `src/memory/ProceduralMemory.cpp`

Implement with:
- Action planning integration
- Skills learning from successful actions
- Procedural knowledge storage
- Motor pattern refinement

### 1.2 Cognition Integration Improvements

**Issues Identified:**
- Neural planner exists but never used in action selection
- Concept formation never processes experiences
- Self-model never updates from experiences
- Social learning never observes
- Spatial representation never used

**Improvements Needed:**

#### A. Integrate Neural Planner into Action Selection
**Files:** `src/brain/Brain.cpp`, `src/agent/AgentBrain.cpp`, `src/experiments/Phase6Experiment.cpp`

Enhance `Brain::produceAction()` to use planner:
```cpp
// REPLACE current simple action selection (lines 692-717) with:
std::unique_ptr<class Action> Brain::produceAction() {
    // Try to use neural planner if available
    if (pImpl->planner) {
        // Get attention winner for current focus
        auto attentionWinner = pImpl->attention ? 
            pImpl->attention->getSelectedNeuronId() : 
            pImpl->workingMemory ? pImpl->workingMemory->getMemoryNeurons().empty() ? 
                NeuronId() : pImpl->workingMemory->getMemoryNeurons()[0] : 
                NeuronId();
        
        // Generate action plan using neural planner
        std::vector<MotorCommand> plan = pImpl->planner->generatePlan(attentionWinner);
        
        if (!plan.empty()) {
            // Convert first action in plan to Action object
            MotorCommand cmd = plan[0];
            return std::make_unique<Action>(cmd);
        }
    }
    
    // Fallback to original motor neuron based action selection
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    auto calcActivity = [](const std::vector<Neuron*>& neurons) -> float {
        if (neurons.empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
        return sum / neurons.size();
    };
    
    float forwardAct = calcActivity(pImpl->motorForward_);
    float backwardAct = calcActivity(pImpl->motorBackward_);
    float leftAct = calcActivity(pImpl->motorTurnLeft_);
    float rightAct = calcActivity(pImpl->motorTurnRight_);
    float interactAct = calcActivity(pImpl->motorInteract_);
    float waitAct = calcActivity(pImpl->motorWait_);
    
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    if (bestActivity < 0.5f) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    return std::make_unique<Action>(best);
}
```

#### B. Enhance Concept Formation Integration
**File:** `src/brain/Brain.cpp`

Replace current Step 10 (lines 529-533):
```cpp
// REPLACE Step 10 (lines 529-533):
// ========== STEP 10: Update concept formation ==========
if (pImpl->conceptFormation) {
    // Process current neural activity patterns to form concepts
    // This requires sensory state encoding - enhanced with working memory
    
    if (pImpl->workingMemory) {
        // Get recent working memory traces as concept training data
        std::vector<float> memoryTrace;
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        for (NeuronId id : memoryNeurons) {
            // Find corresponding neuron and get its activity
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == id) {
                            float activity = 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                            memoryTrace.push_back(activity);
                            break;
                        }
                    }
                }
            }
        }
        
        if (!memoryTrace.empty()) {
            // Create concept from memory trace
            pImpl->conceptFormation->processPattern(memoryTrace);
        }
    }
    
    // Also process episodic memory to reinforce concepts
    if (pImpl->episodicMemory) {
        auto currentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
        for (const auto* episode : currentEpisodes) {
            std::vector<float> episodePattern;
            for (float activation : episode->neuronActivations) {
                episodePattern.push_back(activation);
            }
            pImpl->conceptFormation->learnPattern(episodePattern);
        }
    }
}
```

#### C. Implement Self-Model Integration
**File:** `src/brain/Brain.cpp`

Add Step after concept formation:
```cpp
// Add new Step 10.5: Update self-model
if (pImpl->conceptFormation) {
    // Get current self-model from concept formation
    // This could be the most prominent concept learned so far
    auto selfModel = pImpl->conceptFormation->getSelfModel();
    
    // Update self-model with current experience
    if (selfModel.has_value()) {
        // Store self-model in episodic memory for persistence
        if (pImpl->episodicMemory) {
            EpisodicMemoryItem selfModelEpisode;
            selfModelEpisode.timestamp = currentStep;
            selfModelEpisode.selfModel = selfModel.value();
            pImpl->episodicMemory->storeSelfModel(selfModelEpisode);
        }
    }
}
```

#### D. Implement Social Learning Integration
**File:** `src/agent/AgentBrain.cpp`

Add social learning observation:
```cpp
// In AgentBrain::processSensoryInput():
if (sensoryTouch_.size() > 0 && i < touch.size()) {
    float current = touch[i] * 8.0f;  // Collision signal
    sensoryTouch_[i]->injectCurrent(current);
    
    // Detect potential social interaction (collision with another agent)
    if (current > 10.0f) {
        // This could be social learning - another agent is nearby
        if (brain_ && pImpl->conceptFormation) {
            // Social learning - observe and learn from other agent's behavior
            pImpl->conceptFormation->observeSocialBehavior(current, currentTime);
        }
    }
}
```

### 1.3 Prediction System Integration Improvements

**Issues Identified:**
- Prediction system exists but never trained
- No use of predictions for action selection
- Prediction error not properly integrated with learning

**Improvements Needed:**

#### A. Enhance Prediction System Updates
**File:** `src/brain/Brain.cpp`

Replace current Step 8 (lines 512-516):
```cpp
// REPLACE Step 8 (lines 512-516):
// ========== STEP 8: Update prediction system ==========
if (pImpl->predictionSystem) {
    // Create sensory prediction from current sensory input
    // This requires the brain to have access to sensory state
    
    // Get current sensory input (this would need to be stored in brain)
    if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
        // Use working memory as basis for prediction
        std::vector<float> sensoryPrediction;
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        for (size_t i = 0; i < std::min<size_t>(memoryNeurons.size(), 10); ++i) {
            // Get activity of a few representative neurons
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == memoryNeurons[i]) {
                            sensoryPrediction.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential));
                            break;
                        }
                    }
                }
            }
        }
        
        if (!sensoryPrediction.empty()) {
            // Generate next state prediction
            pImpl->predictionSystem->updatePrediction(sensoryPrediction);
            
            // Compare with actual outcome to compute prediction error
            float predictionError = pImpl->predictionSystem->getPredictionError();
            
            // Store prediction error in neuromodulation system
            if (pImpl->predictionError) {
                pImpl->predictionError->update(predictionError);
            }
        }
    }
}
```

#### B. Integrate Prediction into Action Selection
**File:** `src/agent/AgentBrain.cpp`

Enhance decodeMotorCommand() to use predictions:
```cpp
// In decodeMotorCommand():
MotorCommand decoded = decodeFromMotorNeurons();

// Apply curiosity-based exploration (existing)
if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
    decoded = selectWithCuriosity(decoded);
}

// NEW: Apply prediction-based action selection
if (brain_ && brain_->getPredictionSystem()) {
    decoded = selectWithPrediction(decoded);
}

return decoded;
```

Add selectWithPrediction method:
```cpp
MotorCommand AgentBrain::selectWithPrediction(MotorCommand defaultCmd) {
    if (!brain_ || !pImpl->predictionSystem) return defaultCmd;
    
    // Get prediction for current state
    float prediction = pImpl->predictionSystem->getCurrentPrediction();
    float predictionConfidence = pImpl->predictionSystem->getPredictionConfidence();
    
    // High confidence predictions should lead to more deterministic actions
    if (predictionConfidence > 0.8f) {
        // Choose action that matches prediction if possible
        if (prediction < 0.3f) {
            // Low expected reward - exploratory action
            return selectWithCuriosity(defaultCmd);
        } else if (prediction > 0.7f) {
            // High expected reward - deterministic action
            return defaultCmd;
        }
    }
    
    // Medium confidence - balanced exploration/exploitation
    if (curiosityLevel_ > 0.5f) {
        return selectWithCuriosity(defaultCmd);
    }
    
    return defaultCmd;
}
```

### 1.4 Enhanced Neuromodulation Integration

**Issues Identified:**
- Dopamine only scales STDP weights
- Other neuromodulators are stubs
- No connection between neuromodulation and memory consolidation

**Improvements Needed:**

#### A. Implement Full Neuromodulation Effects
**File:** `src/brain/Brain.cpp`

Replace Step 5 (lines 410-440) with enhanced neuromodulation:
```cpp
// REPLACE Step 5 (lines 410-440) with enhanced:
// ========== STEP 5: Apply neuromodulation effects ==========
// Update novelty detection
if (pImpl->novelty) {
    pImpl->novelty->update(pImpl->timestep);
}

// Update curiosity
if (pImpl->curiosity) {
    pImpl->curiosity->update(pImpl->timestep);
}

// Update prediction error
if (pImpl->predictionError) {
    pImpl->predictionError->update(pImpl->timestep);
}

// Update dopamine (reward prediction error) - ENHANCED
if (pImpl->dopamine) {
    pImpl->dopamine->update(pImpl->timestep);
    
    // Apply dopamine effects on neural excitability (existing)
    float dopamineLevel = pImpl->dopamine->getLevel();
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float excitabilityMod = dopamineLevel * 0.5f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
    
    // ENHANCED: Apply dopamine effects on working memory consolidation
    if (pImpl->workingMemory) {
        float consolidationStrength = 0.5f + 0.5f * dopamineLevel; // Higher dopamine = stronger consolidation
        consolidationStrength = std::clamp(consolidationStrength, 0.1f, 1.0f);
        
        // Boost important working memory traces based on dopamine
        pImpl->workingMemory->consolidate(consolidationStrength);
    }
    
    // ENHANCED: Apply dopamine effects on episodic memory
    if (pImpl->episodicMemory) {
        float memoryPriority = 0.5f + 0.5f * dopamineLevel; // Higher dopamine = more memorable
        memoryPriority = std::clamp(memoryPriority, 0.2f, 1.0f);
        
        // Increase priority of recent episodes based on dopamine
        pImpl->episodicMemory->prioritizeRecentEpisodes(memoryPriority);
    }
}

// ENHANCED: Implement serotonin effects on attention and mood
if (pImpl->config->hasParameter("serotonin_level")) {
    float serotoninLevel = pImpl->config->get<float>("serotonin_level");
    if (pImpl->attention) {
        // Serotonin modulates attention stability
        float attentionStability = 0.5f + 0.5f * serotoninLevel; // Higher serotonin = more stable attention
        pImpl->attention->setStabilityFactor(attentionStability);
    }
}

// ENHANCED: Implement norepinephrine effects on arousal and learning
if (pImpl->config->hasParameter("noradrenaline_level")) {
    float noradrenalineLevel = pImpl->config->get<float>("noradrenaline_level");
    
    // Increase neural excitability
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float arousalBoost = noradrenalineLevel * 0.3f;
                neuron->injectCurrent(arousalBoost);
            }
        }
    }
    
    // Enhance novelty detection sensitivity
    if (pImpl->novelty) {
        pImpl->novelty->setSensitivity(1.0f + noradrenalineLevel * 0.5f);
    }
}

// ENHANCED: Implement acetylcholine effects on attention and memory encoding
if (pImpl->config->hasParameter("acetylcholine_level")) {
    float acetylcholineLevel = pImpl->config->get<float>("acetylcholine_level");
    
    if (pImpl->attention) {
        // Acetylcholine modulates attention selection
        float attentionSelection = 0.5f + 0.5f * acetylcholineLevel; // Higher ACh = more competitive attention
        pImpl->attention->setSelectionStrength(attentionSelection);
    }
    
    if (pImpl->workingMemory) {
        // Acetylcholine enhances working memory encoding
        float encodingStrength = 0.5f + 0.5f * acetylcholineLevel; // Higher ACh = stronger encoding
        pImpl->workingMemory->setEncodingStrength(encodingStrength);
    }
}
```

### 1.5 Enhanced Persistence Integration

**Issues Identified:**
- CheckpointSystem exists but not connected to Brain
- Brain save/load are stubs

**Improvements Needed:**

#### A. Connect Brain Save/Load to CheckpointSystem
**File:** `src/brain/Brain.cpp`

Enhance Brain::save() and Brain::load():
```cpp
// ENHANCED save() method:
bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        // Use checkpointManager if available
        if (pImpl->checkpointManager) {
            // Store brain state in checkpoint system
            CheckpointWriter writer;
            if (!writer.create(filepath, CompressionLevel::Balanced)) {
                NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
                return false;
            }
            
            // Set metadata
            writer.setMetadata(
                getTotalNeuronCount(),
                getTotalSynapseCount(),
                getRegionCount(),
                pImpl->currentStep,
                pImpl->currentTime
            );
            
            // Write brain state using checkpoint manager
            if (!pImpl->checkpointManager->saveBrainState(writer)) {
                NLM_LOG_ERROR("Failed to save brain state to checkpoint");
                return false;
            }
            
            // Write memory systems state
            if (pImpl->workingMemory && !pImpl->workingMemory->save(writer)) {
                NLM_LOG_ERROR("Failed to save working memory");
                return false;
            }
            
            if (pImpl->episodicMemory && !pImpl->episodicMemory->save(writer)) {
                NLM_LOG_ERROR("Failed to save episodic memory");
                return false;
            }
            
            // Write other integrated systems
            if (pImpl->planner && !pImpl->planner->save(writer)) {
                NLM_LOG_ERROR("Failed to save planner");
                return false;
            }
            
            if (pImpl->conceptFormation && !pImpl->conceptFormation->save(writer)) {
                NLM_LOG_ERROR("Failed to save concept formation");
                return false;
            }
            
            if (pImpl->attention && !pImpl->attention->save(writer)) {
                NLM_LOG_ERROR("Failed to save attention system");
                return false;
            }
            
            // Finalize
            if (!writer.finalize()) {
                NLM_LOG_ERROR("Failed to finalize checkpoint");
                return false;
            }
            
            NLM_LOG_INFO("Brain state saved successfully to " + filepath);
            return true;
        } else {
            // Fallback to original implementation
            return Brain::save(filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

// ENHANCED load() method:
bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        // Use checkpointManager if available
        if (pImpl->checkpointManager) {
            CheckpointReader reader;
            if (!reader.open(filepath)) {
                NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
                return false;
            }
            
            if (!reader.validate()) {
                NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
                return false;
            }
            
            // Load brain state from checkpoint system
            if (!pImpl->checkpointManager->loadBrainState(reader)) {
                NLM_LOG_ERROR("Failed to load brain state from checkpoint");
                return false;
            }
            
            // Load memory systems state
            if (pImpl->workingMemory && !pImpl->workingMemory->load(reader)) {
                NLM_LOG_ERROR("Failed to load working memory");
                return false;
            }
            
            if (pImpl->episodicMemory && !pImpl->episodicMemory->load(reader)) {
                NLM_LOG_ERROR("Failed to load episodic memory");
                return false;
            }
            
            // Load other integrated systems
            if (pImpl->planner && !pImpl->planner->load(reader)) {
                NLM_LOG_ERROR("Failed to load planner");
                return false;
            }
            
            if (pImpl->conceptFormation && !pImpl->conceptFormation->load(reader)) {
                NLM_LOG_ERROR("Failed to load concept formation");
                return false;
            }
            
            if (pImpl->attention && !pImpl->attention->load(reader)) {
                NLM_LOG_ERROR("Failed to load attention system");
                return false;
            }
            
            NLM_LOG_INFO("Brain state loaded successfully from " + filepath);
            return true;
        } else {
            // Fallback to original implementation
            return Brain::load(filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}
```

## Priority 2: PERFORMANCE OPTIMIZATIONS

### 2.1 Enhanced Performance Infrastructure Integration

**Issues Identified:**
- Memory pools, event queues, SIMD optimization not integrated into Brain

**Improvements Needed:**

#### A. Integrate Memory Pools into Brain
**File:** `src/brain/Brain.cpp`

Add memory pool initialization:
```cpp
// In Brain::initialize():
// Initialize memory pools for performance
if (config->hasParameter("enable_memory_pools") && 
    config->get<bool>("enable_memory_pools")) {
    
    // Initialize neuron memory pool
    pImpl->neuronMemoryPool = std::make_unique<NeuronMemoryPool>(neuronCount);
    
    // Initialize synapse memory pool
    pImpl->synapseMemoryPool = std::make_unique<SynapseMemoryPool>(neuronCount * connectionProbability);
    
    // Initialize memory pools for integrated systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->setMemoryPool(pImpl->neuronMemoryPool.get());
    }
    
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->setMemoryPool(pImpl->neuronMemoryPool.get());
    }
    
    if (pImpl->planner) {
        pImpl->planner->setMemoryPool(pImpl->neuronMemoryPool.get());
    }
    
    NLM_LOG_INFO("Memory pools enabled for performance optimization");
}
```

#### B. Enhance Event Queue System
**File:** `src/brain/Brain.cpp`

Integrate event queue system:
```cpp
// In Brain::initialize():
// Initialize event queue system
pImpl->eventQueue = std::make_unique<EventQueueManager>();
if (pImpl->eventQueue) {
    // Register event handlers for integrated systems
    pImpl->eventQueue->registerHandler(EventType::Spike, [this](const Event& event) {
        // Process spike events for all systems
        if (pImpl->workingMemory) {
            pImpl->workingMemory->handleEvent(event);
        }
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->handleEvent(event);
        }
    });
    
    pImpl->eventQueue->registerHandler(EventType::Plasticity, [this](const Event& event) {
        // Process plasticity events
        if (pImpl->predictionSystem) {
            pImpl->predictionSystem->handleEvent(event);
        }
    });
    
    NLM_LOG_INFO("Event queue system initialized");
}
```

#### C. Optimize Spike System with SIMD
**File:** `src/brain/Brain.cpp`

Enhance spike processing:
```cpp
// In step() method for spike processing:
// Use SIMD for spike event processing
if (pImpl->spikeSystem) {
    // Process spike events using SIMD when available
    if (pImpl->spikeSystem->isSIMDCompatible()) {
        pImpl->spikeSystem->processDelayedSpikesSIMD(currentStep, currentTime);
    } else {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    // Use SIMD for spike detection
    pImpl->spikeSystem->detectSpikesSIMD();
}
```

### 2.2 Advanced Integration Features

#### A. Multi-Agent Learning
**File:** `src/agent/AgentBrain.cpp`

Add multi-agent coordination:
```cpp
// Add to AgentBrain class:
class MultiAgentCoordinator {
public:
    virtual void coordinateWithOtherAgents(const Brain& otherBrain) = 0;
    virtual void learnFromOtherAgents(const std::vector<Brain*>& otherBrains) = 0;
};

// In AgentBrain::processSensoryInput():
// Add social learning from nearby agents
if (brain_ && config->hasParameter("multi_agent_mode") && 
    config->get<bool>("multi_agent_mode")) {
    
    // Find other agents within range
    std::vector<Brain*> nearbyAgents = findNearbyAgents();
    
    if (!nearbyAgents.empty()) {
        // Coordinate with nearby agents
        if (multiAgentCoordinator) {
            multiAgentCoordinator->coordinateWithOtherAgents(nearbyAgents[0]);
        }
        
        // Learn from other agents
        if (pImpl->conceptFormation) {
            for (Brain* otherBrain : nearbyAgents) {
                pImpl->conceptFormation->observeOtherAgent(*otherBrain);
            }
        }
    }
}
```

#### B. Sleep/Consolidation Cycle
**File:** `src/brain/Brain.cpp`

Implement advanced sleep cycle:
```cpp
// Add sleep cycle management to Brain::Impl:
void Brain::Impl::enterRestPhase() {
    isResting = true;
    restStartTime = currentTime;
    restPhase = RestPhase::Sleep;
    
    // Prepare memories for consolidation
    if (episodicMemory) {
        episodicMemory->prepareForConsolidation();
    }
    
    // Boost slow-wave activity for memory replay
    enableSlowWaveActivity(true);
}

void Brain::Impl::updateRestCycle() {
    if (!isResting) return;
    
    double elapsedRest = currentTime - restStartTime;
    
    if (restPhase == RestPhase::Sleep && elapsedRest >= 10.0) {  // 10 time units sleep
        restPhase = RestPhase::Replay;
        replayStartTime = currentTime;
        
        // Enter high replay mode
        enableReplayMode(true);
    }
    
    if (restPhase == RestPhase::Replay && elapsedRest >= 5.0) {  // 5 time units replay
        restPhase = RestPhase::Consolidation;
        consolidationStartTime = currentTime;
        
        // Enter consolidation mode
        enableConsolidationMode(true);
    }
    
    if (restPhase == RestPhase::Consolidation && elapsedRest >= 8.0) {  // 8 time units consolidation
        restPhase = RestPhase::Wakeup;
        wakeupStartTime = currentTime;
    }
    
    if (restPhase == RestPhase::Wakeup && elapsedRest >= 2.0) {  // 2 time units wakeup
        // Exit rest cycle
        isResting = false;
        restPhase = RestPhase::Awake;
        
        // Finalize consolidation
        if (episodicMemory) {
            episodicMemory->finalizeConsolidation();
        }
        
        // Reset replay and consolidation settings
        enableReplayMode(false);
        enableConsolidationMode(false);
        enableSlowWaveActivity(false);
        
        // Apply development changes that occurred during rest
        if (developmentSystem && !restSleepHistory.empty()) {
            developmentSystem->applyRestChanges(restSleepHistory);
        }
    }
}
```

## Priority 3: USER INTERFACE AND COMMAND LINE ENHANCEMENTS

### 3.1 Enhanced Command Line Interface

**File:** `src/main.cpp`

Add advanced commands:
```cpp
// Add to main() function:

// Command: save and quit
system("save"):
    if (brain) {
        std::string filename;
        std::cout << "Enter checkpoint filename: ";
        std::cin >> filename;
        if (brain->save(filename)) {
            std::cout << "Brain state saved successfully to " << filename << std::endl;
            return 0; // Exit
        } else {
            std::cout << "Failed to save brain state" << std::endl;
        }
    }
    break;

// Command: load and continue
system("load"):
    if (brain) {
        std::string filename;
        std::cout << "Enter checkpoint filename: ";
        std::cin >> filename;
        if (brain->load(filename)) {
            std::cout << "Brain state loaded successfully from " << filename << std::endl;
            // Continue simulation
        } else {
            std::cout << "Failed to load brain state" << std::endl;
        }
    }
    break;

// Command: multi-step execution
system("run", steps):
    if (brain) {
        int steps;
        std::cout << "Enter number of steps to run: ";
        std::cin >> steps;
        
        std::cout << "Running " << steps << " simulation steps..." << std::endl;
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
            
            // Save checkpoint periodically
            if (i % 1000 == 0) {
                brain->save("checkpoint_" + std::to_string(i) + ".bin");
            }
            
            // Show progress
            if (i % 1000 == 0) {
                std::cout << "Step " << i << "/" << steps << std::endl;
            }
        }
        
        std::cout << "Completed " << steps << " simulation steps" << std::endl;
    }
    break;

// Command: system analysis
system("analyze"):
    if (brain) {
        std::cout << "=== NLM Brain Analysis ===" << std::endl;
        brain->logStatus();
        
        // Additional analysis
        std::cout << "\n=== Advanced Analysis ===" << std::endl;
        
        if (brain->getWorkingMemory()) {
            std::cout << "Working memory activity: " 
                      << brain->getWorkingMemory()->getActiveTraces() << " traces" << std::endl;
        }
        
        if (brain->getEpisodicMemory()) {
            std::cout << "Episodic memory episodes: " 
                      << brain->getEpisodicMemory()->getEpisodeCount() << std::endl;
        }
        
        if (brain->getPredictionSystem()) {
            std::cout << "Prediction confidence: " 
                      << brain->getPredictionSystem()->getPredictionConfidence() * 100 << "%" << std::endl;
        }
        
        if (brain->getPlanner()) {
            std::cout << "Planner: Active action plans: " 
                      << brain->getPlanner()->getActivePlanCount() << std::endl;
        }
    }
    break;

// Command: interactive control
system("interactive"):
    if (brain) {
        std::cout << "=== Interactive Mode ===" << std::endl;
        std::cout << "Commands: status, save <filename>, load <filename>, "
                  << "run <steps>, analyze, sleep, wake, help, quit" << std::endl;
        
        while (true) {
            std::cout << "> ";
            std::string command;
            std::getline(std::cin, command);
            
            if (command == "quit") {
                break;
            } else if (command == "status") {
                brain->logStatus();
            } else if (command.substr(0, 5) == "save ") {
                std::string filename = command.substr(5);
                brain->save(filename);
            } else if (command.substr(0, 5) == "load ") {
                std::string filename = command.substr(5);
                brain->load(filename);
            } else if (command.substr(0, 4) == "run ") {
                int steps = std::stoi(command.substr(4));
                for (int i = 0; i < steps; ++i) {
                    brain->step(i);
                }
                std::cout << "Ran " << steps << " steps" << std::endl;
            } else if (command == "analyze") {
                brain->logStatus();
                if (brain->getWorkingMemory()) {
                    std::cout << "Working memory traces: " 
                              << brain->getWorkingMemory()->getActiveTraces() << std::endl;
                }
            } else if (command == "sleep") {
                brain->enterRestPhase();
                std::cout << "Entered rest phase" << std::endl;
            } else if (command == "wake") {
                brain->exitRestPhase();
                std::cout << "Woke up from rest phase" << std::endl;
            } else if (command == "help") {
                // Show help
            } else {
                std::cout << "Unknown command. Type 'help' for available commands." << std::endl;
            }
        }
    }
    break;
```

### 3.2 Enhanced Configuration System

**File:** `src/core/Config/Config.hpp` and `src/core/Config/Config.cpp`

Add advanced configuration options:
```cpp
// Add to Config class:

// Memory system configuration
void setWorkingMemoryCapacity(size_t capacity);
size_t getWorkingMemoryCapacity() const;

void setEpisodicMemoryMaxEpisodes(size_t maxEpisodes);
size_t getEpisodicMemoryMaxEpisodes() const;

void setSemanticMemoryLearningRate(float rate);
float getSemanticMemoryLearningRate() const;

void setProceduralMemoryExplorationRate(float rate);
float getProceduralMemoryExplorationRate() const;

// Cognition system configuration
void setPlannerPlanningDepth(int depth);
int getPlannerPlanningDepth() const;

void setConceptFormationLearningRate(float rate);
float getConceptFormationLearningRate() const;

void setSelfModelUpdateRate(float rate);
float getSelfModelUpdateRate() const;

// Prediction system configuration
void setPredictionTimeHorizon(double horizon);
double getPredictionTimeHorizon() const;

void setPredictionConfidenceThreshold(float threshold);
float getPredictionConfidenceThreshold() const;

// Neuromodulation configuration
void setDopamineBaseLevel(float level);
float getDopamineBaseLevel() const;

void setSerotoninBaseLevel(float level);
float getSerotoninBaseLevel() const;

void setNorepinephrineBaseLevel(float level);
float getNorepinephrineBaseLevel() const;

void setAcetylcholineBaseLevel(float level);
float getAcetylcholineBaseLevel() const;

// Development configuration
void setDevelopmentStageDuration(DevelopmentalStage stage, double duration);
double getDevelopmentStageDuration(DevelopmentalStage stage) const;

void setPlasticityModifier(DevelopmentalStage stage, float modifier);
float getPlasticityModifier(DevelopmentalStage stage) const;

// Performance configuration
void enableMemoryPools(bool enable);
bool areMemoryPoolsEnabled() const;

void enableSIMD(bool enable);
bool isSIMDEnabled() const;

void enableMultithreading(bool enable);
bool isMultithreadingEnabled() const;

void setEventQueueSize(size_t size);
size_t getEventQueueSize() const;

// Advanced integration configuration
void enableMultiAgentMode(bool enable);
bool isMultiAgentModeEnabled() const;

void setSocialLearningRate(float rate);
float getSocialLearningRate() const;

void setSleepConsolidationInterval(double interval);
double getSleepConsolidationInterval() const;
```

## Priority 4: VALIDATION AND TESTING ENHANCEMENTS

### 4.1 Enhanced Test Suite

**File:** `src/experiments/Phase6IntegrationTest.cpp`

Enhance Phase 6 integration test:
```cpp
// Add comprehensive validation tests:

class Phase6IntegrationTest {
public:
    bool testMemoryIntegration() {
        // Test working memory integration
        bool workingMemoryTest = testWorkingMemoryIntegration();
        
        // Test episodic memory integration
        bool episodicMemoryTest = testEpisodicMemoryIntegration();
        
        // Test semantic memory integration
        bool semanticMemoryTest = testSemanticMemoryIntegration();
        
        // Test procedural memory integration
        bool proceduralMemoryTest = testProceduralMemoryIntegration();
        
        return workingMemoryTest && episodicMemoryTest && 
               semanticMemoryTest && proceduralMemoryTest;
    }
    
    bool testCognitiveIntegration() {
        // Test neural planner integration
        bool plannerTest = testNeuralPlannerIntegration();
        
        // Test concept formation integration
        bool conceptFormationTest = testConceptFormationIntegration();
        
        // Test self-model integration
        bool selfModelTest = testSelfModelIntegration();
        
        return plannerTest && conceptFormationTest && selfModelTest;
    }
    
    bool testPredictionIntegration() {
        // Test prediction system integration
        bool predictionTest = testPredictionSystemIntegration();
        
        // Test prediction error integration
        bool predictionErrorTest = testPredictionErrorIntegration();
        
        return predictionTest && predictionErrorTest;
    }
    
    bool testNeuromodulationIntegration() {
        // Test dopamine integration
        bool dopamineTest = testDopamineIntegration();
        
        // Test serotonin integration
        bool serotoninTest = testSerotoninIntegration();
        
        // Test norepinephrine integration
        bool norepinephrineTest = testNorepinephrineIntegration();
        
        // Test acetylcholine integration
        bool acetylcholineTest = testAcetylcholineIntegration();
        
        return dopamineTest && serotoninTest && norepinephrineTest && acetylcholineTest;
    }
    
    bool testPersistenceIntegration() {
        // Test save/load integration
        bool saveLoadTest = testSaveLoadIntegration();
        
        // Test checkpoint system integration
        bool checkpointTest = testCheckpointSystemIntegration();
        
        return saveLoadTest && checkpointTest;
    }
    
    bool testDevelopmentIntegration() {
        // Test developmental stages integration
        bool developmentTest = testDevelopmentStageIntegration();
        
        // Test plasticity modulation integration
        bool plasticityTest = testPlasticityModulationIntegration();
        
        return developmentTest && plasticityTest;
    }
    
    bool runAllTests() {
        std::cout << "Running Phase 6 Integration Tests..." << std::endl;
        
        bool allPassed = true;
        
        allPassed &= testMemoryIntegration();
        allPassed &= testCognitiveIntegration();
        allPassed &= testPredictionIntegration();
        allPassed &= testNeuromodulationIntegration();
        allPassed &= testPersistenceIntegration();
        allPassed &= testDevelopmentIntegration();
        
        if (allPassed) {
            std::cout << "All Phase 6 integration tests PASSED!" << std::endl;
        } else {
            std::cout << "Some Phase 6 integration tests FAILED!" << std::endl;
        }
        
        return allPassed;
    }
};
```

### 4.2 Performance Monitoring

**File:** `src/experiments/Phase6IntegrationTest.cpp`

Add performance monitoring:
```cpp
class PerformanceMonitor {
public:
    struct Metrics {
        double simulationTime;
        size_t neuronsProcessed;
        size_t spikesHandled;
        size_t plasticityOperations;
        size_t memoryOperations;
        size_t integrationOperations;
        float memoryUsage;
        float cpuUsage;
    };
    
    void startMonitoring() { /* Start timing */ }
    
    Metrics stopMonitoring() { /* Return collected metrics */ }
    
    void logPerformance(const Metrics& metrics) {
        std::cout << "=== Performance Metrics ===" << std::endl;
        std::cout << "Simulation time: " << metrics.simulationTime << " seconds" << std::endl;
        std::cout << "Neurons processed: " << metrics.neuronsProcessed << std::endl;
        std::cout << "Spikes handled: " << metrics.spikesHandled << std::endl;
        std::cout << "Plasticity operations: " << metrics.plasticityOperations << std::endl;
        std::cout << "Memory operations: " << metrics.memoryOperations << std::endl;
        std::cout << "Integration operations: " << metrics.integrationOperations << std::endl;
        std::cout << "Memory usage: " << metrics.memoryUsage << " MB" << std::endl;
        std::cout << "CPU usage: " << metrics.cpuUsage << "%" << std::endl;
    }
};
```

## Implementation Timeline

### Week 1: Core Integration (Memory Systems)
- Days 1-2: Implement enhanced working memory integration
- Days 3-4: Complete semantic memory implementation
- Days 5-6: Complete procedural memory implementation
- Day 7: Test memory integration

### Week 2: Cognitive Integration
- Days 8-9: Integrate neural planner into action selection
- Days 10-11: Enhance concept formation integration
- Days 12-13: Implement self-model integration
- Days 14-15: Implement social learning integration

### Week 3: Prediction and Neuromodulation
- Days 16-17: Enhance prediction system integration
- Days 18-19: Integrate prediction into action selection
- Days 20-21: Implement enhanced neuromodulation integration
- Day 22: Test prediction and neuromodulation integration

### Week 4: Persistence and Performance
- Days 23-24: Connect brain save/load to checkpoint system
- Days 25-26: Integrate performance infrastructure (memory pools, event queues, SIMD)
- Days 27-28: Implement multi-agent learning
- Day 29: Test persistence and performance improvements

### Week 5: User Interface and Validation
- Days 30-31: Add enhanced command line interface
- Days 32-33: Add advanced configuration options
- Days 34-35: Implement comprehensive validation and testing
- Days 36-37: Performance monitoring and optimization
- Days 38-39: Final integration testing
- Day 40: Documentation and final review

## Files Modified

### Core Files
1. `src/brain/Brain.cpp` - Enhanced step(), produceAction(), save(), load()
2. `src/agent/AgentBrain.cpp` - Enhanced decodeMotorCommand(), added social learning
3. `src/main.cpp` - Added enhanced command line interface

### New/Extended Files
1. `src/memory/SemanticMemory.cpp` - Complete semantic memory implementation
2. `src/memory/ProceduralMemory.cpp` - Complete procedural memory implementation
3. `src/experiments/Phase6IntegrationTest.cpp` - Enhanced integration test suite
4. `src/experiments/Phase6AdvancedExperiment.cpp` - Advanced integration experiments

## Expected Outcomes

### After Implementation
1. **Integration Score**: Increase from 43/120 (35.8%) to 110/120 (91.7%)
2. **Cognitive Function**: Full brain loop integration with all cognitive systems active
3. **Performance**: 2-5x speedup from performance infrastructure integration
4. **User Experience**: Rich command line interface with interactive control
5. **Research Capabilities**: Advanced multi-agent and sleep consolidation features

### Scientific Impact
1. **Memory Research**: Demonstrated memory retention, consolidation, and recall
2. **Learning Research**: Showed continual learning without catastrophic forgetting
3. **Developmental Research**: Validated developmental progression effects
4. **Social Learning**: Demonstrated learning from other agents
5. **Prediction Research**: Validated predictive coding capabilities

The NLM/熙然 project will transition from a basic neural simulator to a fully integrated artificial brain capable of complex cognition, learning, and behavior.