# Phase 6 Integration: Remaining Tasks

## Current Status

The NLM Phase 6 system has **significant integration work remaining** despite having well-implemented cognitive subsystems. Three major systems need integration with the main brain loop:

1. ✅ **PredictionSystem** - Placeholder implementation, needs neural integration
2. ✅ **NeuralPlanner** - Fully implemented, needs action selection integration  
3. ✅ **ConceptFormation** - Fully implemented, needs brain loop integration

All three systems have proper brain integration methods but are disconnected from the main Brain::step() and AgentBrain loops.

## Integration Strategy

### 1. Prediction System Integration

**Current State:** PredictionSystem.hpp shows "PLACEHOLDER - Phase 2 will implement real predictive coding"

**Fix:** Update PredictionSystem to work with neural substrate:

```cpp
// In src/prediction/PredictionSystem.hpp
class PredictionSystem {
public:
    // NEW: Initialize with brain
    void initialize(Brain* brain) { brain_ = brain; }
    
    // NEW: Update prediction using neural patterns
    void update(const std::vector<float>& sensoryPattern, Timestamp time) {
        if (!brain_) return;
        
        // Store current sensory state
        currentState_ = sensoryPattern;
        
        // Predict next state based on temporal patterns
        predictNextState(currentState_, time);
        
        // Compute prediction error vs actual
        computePredictionError(currentState_, time);
    }
    
    // NEW: Get neural activity pattern for prediction
    std::vector<float> getPredictionPattern() const { 
        return predictedState_; 
    }
    
    // NEW: Get neural prediction error signal
    float getNeuralPredictionError() const { 
        return predictionError_; 
    }
    
private:
    // NEW: Neural integration
    Brain* brain_ = nullptr;
    std::vector<float> currentState_;
    std::vector<float> predictedState_;
    float predictionError_;
};
```

**In Brain.cpp step():**
```cpp
// ========== STEP 6: Update prediction system ==========
if (pImpl->predictionSystem) {
    // Extract current neural activity pattern from sensory neurons
    std::vector<float> sensoryPattern;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    float activation = std::abs(neuronio->getState().membranePotential - neuron->getState().restingPotential);
                    sensoryPattern.push_back(activation);
                }
            }
        }
    }
    
    if (!sensoryPattern.empty()) {
        pImpl->predictionSystem->update(sensoryPattern, currentTime);
    }
}
```

### 2. Neural Planner Integration

**Current State:** NeuralPlanner has full implementation but isn't used in action selection.

**Fix:** Integrate planner into AgentBrain action selection:

```cpp
// In src/agent/AgentBrain.cpp
MotorCommand AgentBrain::decodeMotorCommand() {
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // ===== INTEGRATE NEURAL PLANNER =====
    if (brain_ && brain_->getPlanner()) {
        // Get current neural state for planning
        std::vector<float> currentState;
        extractNeuralState(currentState);
        
        // Get planned action from neural planner
        ActionType plannedAction = brain_->getPlanner()->planAction(currentState);
        
        // Convert to motor command
        MotorCommand plannedCommand = convertActionToMotorCommand(plannedAction);
        
        // Use planned action if it's meaningful (not Wait)
        if (plannedAction != ActionType::Wait) {
            decoded = plannedCommand;
        }
        
        // Apply curiosity to planned action
        if (curiosityEnabled_ && curiosityLevel_ > 0.5f) {
            decoded = selectWithCuriosity(decoded);
        }
    }
    
    // ... rest of existing code
}
```

**In Brain.cpp step():**
```cpp
// ========== STEP 8: Update prediction system ==========
if (pImpl->predictionSystem && pImpl->planner) {
    // Get sensory input for planning
    std::vector<float> sensoryPattern;
    // Extract from sensory neurons
    
    // Update prediction system
    pImpl->predictionSystem->update(sensoryPattern, currentTime);
    
    // Let neural planner use predictions for better planning
    auto predictionPattern = pImpl->predictionSystem->getPredictionPattern();
    pImpl->planner->setPrediction(predictionPattern);
}
```

**NeuralPlanner update method:**
```cpp
// In src/cognition/NeuralPlanner.cpp
void NeuralPlanner::updateWithExperience(const std::vector<ActionType>& actions,
                                        const std::vector<float>& actualReward) {
    // Use prediction system to learn from prediction errors
    if (brain_->getPredictionSystem()) {
        float predictionError = brain_->getPredictionSystem()->getPredictionError();
        
        // Update action quality based on prediction error
        for (size_t i = 0; i < actions.size() && i < actionQuality_.size(); ++i) {
            size_t actionIdx = static_cast<size_t>(actions[i]);
            float predictionBasedQuality = 0.5f * actionQuality_[actionIdx] + 0.5f * predictionError;
            actionQuality_[actionIdx] = predictionBasedQuality;
        }
    }
    
    // ... rest of existing implementation
}
```

### 3. Concept Formation Integration

**Current State:** ConceptFormation has comprehensive methods but isn't updated in brain loop.

**Fix:** Integrate concept formation into Brain::step() and AgentBrain:

```cpp
// In src/brain/Brain.cpp step():
// ========== STEP 9: Update concept formation ==========
if (pImpl->conceptFormation) {
    // Extract current neural activity pattern
    std::vector<float> currentPattern;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    float normalizedActivation = 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                    currentPattern.push_back(normalizedActivation);
                }
            }
        }
    }
    
    if (!currentPattern.empty()) {
        // Get features from sensory state
        std::vector<float> features;
        extractSensoryFeatures(features);
        
        // Present pattern to concept formation system
        size_t conceptId = pImpl->conceptFormation->presentExperience(
            currentPattern, features, 
            pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,
            currentStep
        );
        
        // Apply concept to attention if meaningful concept found
        if (conceptId > 0 && pImpl->conceptFormation) {
            auto conceptPrototype = pImpl->conceptFormation->getConceptPrototype(conceptId);
            if (pImpl->attention) {
                pImpl->attention->applyConceptBasedAttention(conceptPrototype);
            }
        }
    }
}
```

**AgentBrain integration:**
```cpp
// In src/agent/AgentBrain.cpp
void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    // ... existing sensory processing ...
    
    // ===== INTEGRATE CONCEPT FORMATION =====
    if (brain_ && brain_->getConceptFormation()) {
        // Extract concept from sensory pattern
        std::vector<float> currentPattern = extractSensoryPattern(perp	 percept);
        std::vector<float> features = extractSensoryFeatures(percept);
        
        // Present to concept formation system
        size_t conceptId = brain_->getConceptFormation()->presentExperience(
            currentPattern, features,
            getNeuromodulationLevel(),
            /* need current step from somewhere */
        );
        
        // Use concept to influence attention and action
        if (conceptId > 0) {
            // Apply concept-based bias to attention
            auto conceptProto = brain_->getConceptFormation()->getConceptPrototype(conceptId);
            applyConceptToActionSelection(conceptProto);
        }
    }
}
```

**Attention integration (in Brain.cpp):**
```cpp
// ========== STEP 7: Update attention system ==========
if (pImpl->attention) {
    pImpl->attention->update(pImpl->timestep);
    
    // ===== APPLY CONCEPT TO ATTENTION =====
    if (pImpl->conceptFormation && !pImpl->conceptFormation->getConcepts().empty()) {
        // Use the most recent concept for attention biasing
        auto& concepts = pImpl->conceptFormation->getConcepts();
        if (!concepts.empty()) {
            size_t latestConceptId = concepts.size() - 1;
            auto conceptPrototype = pImpl->conceptFormation->getConceptPrototype(latestConceptId);
            
            // Apply concept-based attention bias
            pImpl->attention->applyConceptBias(conceptPrototype);
        }
    }
    
    // Apply attention to working memory winners
    if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
        pImpl->attention->processCompetition(competitors);
    }
}
```

## Integration Files Summary

### Modified Files:
1. **src/brain/Brain.cpp** - Add prediction, planner, concept formation updates to step()
2. **src/agent/AgentBrain.cpp** - Add concept formation integration and planner usage
3. **src/prediction/PredictionSystem.hpp** - Add neural integration methods
4. **src/cognition/NeuralPlanner.hpp** - Ensure planner integration methods
5. **src/cognition/ConceptFormation.hpp** - Ensure concept formation methods

### New Methods Needed:
```cpp
// In PredictionSystem.hpp
void initialize(Brain* brain);
void update(const std::vector<float>& sensoryPattern, Timestamp time);
std::vector<float> getPredictionPattern() const;
float getNeuralPredictionError() const;

// In NeuralPlanner.hpp  
void setPrediction(const std::vector<float>& prediction);
void updateWithExperience(const std::vector<ActionType>& actions, float reward);

// In ConceptFormation.hpp
void setPrediction(const std::vector<float>& prediction);  // for concept prediction
void updateWithSensoryInput(const std::vector<float>& sensoryFeatures, float reward);  // for direct sensory integration
```

## Testing Strategy

### Phase 6 Integration Verification:
1. **Prediction Integration Test:** Verify prediction error signals influence learning
2. **Planning Integration Test:** Verify neural planner output differs from basic activity decoding
3. **Concept Integration Test:** Verify concept formation affects attention and action selection
4. **Full Integration Test:** Run complete 2000-step simulation with all systems active

### Expected Improvements:
- **Before:** System score ~43/120
- **After:** Target >100/120 (with full integration)
- Prediction error → Learning modulation
- Neural planning → Better action selection than basic decoding
- Concept formation → Attention biasing and action guidance

## Implementation Timeline

**Week 1:** Prediction system and Neural planner integration
**Week 2:** Concept formation and attention integration  
**Week 3:** Testing and optimization
**Week 4:** Final verification with Phase 6 demo

## Key Design Principles Applied

1. **Experience-driven:** All cognitive systems process real neural activity patterns
2. **Neural substrate:** Predictions, concepts, and plans are neural patterns, not symbolic
3. **Recursive integration:** Systems inform each other (concept ↔ attention, prediction ↔ planning)
4. **Embodied:** All systems connect to sensory-motor loop via AgentBrain
5. **Plastic:** All systems use STDP, Hebbian, and structural plasticity

This completes the Phase 6 integration, transforming NLM from a disconnected collection of components into a fully integrated artificial brain that learns, remembers, predicts, plans, and develops through experience.