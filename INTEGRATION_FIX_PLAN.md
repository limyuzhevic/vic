# NLM Codebase Integration Fix Plan

## Summary
The NLM codebase has sophisticated architecture but functions as a basic neural simulator rather than an integrated artificial brain. The systems exist (working memory, episodic memory, neuromodulation, prediction, cognition) but they don't interact. Phase 6 must focus on **integration over new features**.

## Critical Integration Gaps (Phase 1: Fix Brain Loop)

### 1. Brain::step() Missing Key System Updates
**Files:** `src/brain/Brain.cpp` (lines 302-587)

**Current Status:** The 14-step brain loop has stubs/empty implementations for STEPS 8, 10, and others.

**What needs fixing:**
- **Step 8 (Prediction System)**: Empty comment, no actual prediction updates
- **Step 10 (Concept Formation)**: Empty comment, no concept processing
- **Step 12 (Development)**: Minimal integration, doesn't affect more than plasticity rates
- **Missing**: Working memory updates, episodic memory storage, neuromodulation effects

**Implementation:**
```cpp
// Step 8: Update prediction system
if (pImpl->predictionSystem && pImpl->sensoryNeurons.size() > 0) {
    // Get sensory activity
    std::vector<float> sensoryActivity;
    for (auto* neuron : pImpl->sensoryNeurons) {
        sensoryActivity.push_back(neuron->getMembranePotential());
    }
    
    // Create sensory input from neural activity
    auto sensoryInput = std::make_unique<SensoryInput>();
    sensoryInput->setData(sensoryActivity);
    
    // Predict next state
    auto predictedState = pImpl->predictionSystem->predictNextState(*sensoryInput);
    
    // Update with actual activity
    pImpl->predictionSystem->updatePredictions(*predictedState, *sensoryInput);
}
```

**Priority:** HIGH - Without this, prediction cannot work

### 2. Prediction System Integration
**Files:** `src/prediction/PredictionSystem.hpp`, `src/prediction/PredictionSystem.cpp`

**Current Status:** `predictNextState()` returns `currentState.clone()` - no actual prediction

**What needs fixing:**
- Use actual neural population activity for predictions
- Implement prediction errors based on neural discrepancies
- Connect prediction errors to neuromodulation

**Implementation:**
```cpp
std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use neural population activity for prediction
    // Get brain regions and extract activity patterns
    std::vector<float> prediction;
    
    for (const auto& region : brainRegions) {
        auto activity = region->getActivityPattern();
        prediction.insert(prediction.end(), activity.begin(), activity.end());
    }
    
    // Add temporal component based on history
    if (!errorHistory.empty()) {
        float avgError = 0.0f;
        for (float error : errorHistory) avgError += error;
        avgError /= errorHistory.size();
        
        // Scale prediction based on uncertainty
        float confidence = 1.0f - (avgError / 2.0f);
        confidence = std::clamp(confidence, 0.0f, 1.0f);
        
        for (float& p : prediction) {
            p *= confidence;
        }
    }
    
    auto result = std::make_unique<SensoryInput>();
    result->setData(prediction);
    return result;
}
```

**Priority:** HIGH - Core to brain function

### 3. Neuromodulation Implementation
**Files:** `src/neuromodulation/Neuromodulator.hpp`, `src/neuromodulation/Neuromodulator.cpp`, `src/neuromodulation/`

**Current Status:** Dopamine has basic stub, ACh/NE/5-HT are complete placeholders

**What needs fixing:**
- Implement dopamine dynamics with phasic/tonic components
- Add receptor-level effects on neurons and plasticity
- Implement acetylcholine effects on attention
- Add norepinephrine effects on arousal
- Implement serotonin effects on mood/behavior

**Implementation:**
```cpp
void Dopamine::update(TimestepDuration dt) {
    // Phasic component: decay towards baseline with adaptation
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt) * (1.0f + pImpl->adaptation));
    
    // Tonic component: slow increase based on sustained activity
    if (sustainedActivity > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.01f * static_cast<float>(dt));
    }
    
    // Store for downstream effects
    lastUpdate = currentTime;
}
```

**Priority:** HIGH - Critical for learning and attention

### 4. Sensory System Integration
**Files:** `src/sensory/Vision.hpp/cpp`, `src/sensory/Audio.hpp/cpp`, `src/sensory/InternalSignals.hpp/cpp`

**Current Status:** All sensory systems just pass through data with TODOs

**What needs fixing:**
- Connect sensory processing to brain neurons directly
- Implement real encoding (population, rate, temporal)
- Map sensory inputs to specific neural populations

**Implementation:**
```cpp
void VisionProcessor::process(const Vision& input) {
    // Encode visual input using brain neural populations
    std::vector<float> encoded;
    
    // Find visual processing region
    if (auto* region = brain->getRegion(RegionId(1))) {
        auto neurons = region->getPopulation(NeuronType::Sensory);
        if (!neurons.empty()) {
            // Rate encode visual features
            for (size_t i = 0; i < input.getDimensions(); ++i) {
                float feature = input.getData()[i];
                
                // Map feature to specific neuron populations
                if (i < neurons.size()) {
                    // Spike rate proportional to feature strength
                    float rate = std::min(100.0f, feature * 10.0f);  // Hz
                    neurons[i]->setFiringRate(rate);
                    
                    // Add to encoded representation
                    encoded.push_back(feature);
                }
            }
        }
    }
    
    pImpl->features = encoded;
}
```

**Priority:** MEDIUM - Important for perception but can be simplified initially

### 5. Motor System Implementation
**Files:** `src/motor/MotorSystem.hpp`, `src/motor/MotorSystem.cpp`

**Current Status:** `selectAction()` returns first available action (random)

**What needs fixing:**
- Implement action selection based on motor neuron competition
- Add decision-making through basal ganglia-like mechanisms
- Integrate with neuromodulation and attention

**Implementation:**
```cpp
std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    // Implement winner-take-all competition among motor neurons
    auto neurons = motorRegion.getAllNeurons();
    if (neurons.empty()) return std::make_unique<Action>(ActionType::Wait);
    
    // Find neuron with highest activity (accounting for neuromodulation)
    float maxActivity = -1.0f;
    Neuron* winner = nullptr;
    
    for (auto* neuron : neurons) {
        float activity = neuron->getMembranePotential();
        
        // Modulate by attention and dopamine
        if (dopamine) {
            float plasticityFactor = dopamine->getPlasticityFactor();
            activity *= plasticityFactor;
        }
        
        if (activity > maxActivity) {
            maxActivity = activity;
            winner = neuron;
        }
    }
    
    if (winner && maxActivity > 0.1f) {
        // Convert neural activity to specific motor command
        ActionType action = mapActivityToAction(maxActivity, winner->getId().index());
        
        // Record in history
        auto result = std::make_unique<Action>(action);
        pImpl->actionHistory.push_back(result.get());
        return result;
    }
    
    return std::make_unique<Action>(ActionType::Wait);
}
```

**Priority:** HIGH - Core to agent behavior

## Important Integration Issues (Phase 2: Memory Systems)

### 6. Working Memory Implementation
**Files:** `src/memory/NeuralWorkingMemory.hpp/cpp`

**Current Status:** Defined but returns nullptr from Brain::getWorkingMemory()

**What needs fixing:**
- Connect working memory storage to sensory input
- Implement working memory decay and competition
- Integrate with attention mechanisms

**Implementation:**
```cpp
defines memory::process(const SensoryInput& input) {
    // Store sensory input in working memory
    float importance = computeImportance(input);
    storeTrace(input, importance);
    
    // Update trace activities
    updateTraceActivities();
    
    // Apply competition
    selectWinners();
}
```

**Priority:** MEDIUM - Important for cognition

### 7. Episodic Memory Integration
**Files:** `src/memory/NeuralEpisodicMemory.hpp/cpp`

**Current Status:** Placeholder, never updated in brain loop

**What needs fixing:**
- Store experiences in episodic memory during brain step()
- Implement replay mechanism
- Add consolidation process

**Implementation:**
```cpp
void EpisodicMemory::storeEpisode(const Episode& episode) {
    // Store with capacity limits
    if (episodes.size() >= maxEpisodes) {
        // Remove oldest episodes
        episodes.erase(episodes.begin(), episodes.begin() + (episodes.size() - maxEpisodes + 1));
    }
    
    episodes.push_back(std::make_unique<Episode>(episode));
    
    // Update associative links
    for (auto& other : episodes) {
        if (other.get() != this) {
            // Find similarities and strengthen associations
            strengthenAssociations(*episodes.back(), *other);
        }
    }
}
```

**Priority:** MEDIUM - Important for learning

## Implementation Plan

### Wave 1: Critical Integration (First 2 Weeks)
1. Fix Brain::step() to call prediction system (1-2 days)
2. Implement basic prediction system (2-3 days)
3. Implement dopamine neuromodulation (3-4 days)
4. Fix motor system (4-5 days)

### Wave 2: Memory Systems (Weeks 3-4)
1. Connect working memory to brain step (3 days)
2. Implement episodic memory storage (4-5 days)
3. Add replay mechanism (3 days)

### Wave 3: Sensory Integration (Weeks 5-6)
1. Connect sensory processing to neurons (4-5 days)
2. Implement attention mechanisms (5-6 days)
3. Add acetylcholine effects (3 days)

### Wave 4: Advanced Integration (Weeks 7-8)
1. Implement norepinephrine and serotonin (3-4 days)
2. Add concept formation (4-5 days)
3. Implement planning (5-6 days)
4. Add self-model (4 days)

## Expected Results

After implementation:
- Brain score should improve from 43/120 to >80/120
- Agent should demonstrate learning and adaptation
- All major systems should interact
- Phase 6 integration experiment should run successfully
- Memory retention and replay should work

This is a focused effort to make the existing systems work together rather than adding new disconnected features.