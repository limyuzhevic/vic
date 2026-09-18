# NeuralPlanner Integration Implementation

## Current State Analysis

The NeuralPlanner is **fully implemented but completely disconnected from action selection**. Here's the problem:

### What's Broken:
1. **NeuralPlanner exists** in `src/cognition/NeuralPlanner.hpp/.cpp` with sophisticated planning capabilities
2. **Never instantiated** in Brain::initialize() 
3. **Never used** in action selection
4. **Disconnected from perception-action loop**

### The Integration Gap:
```cpp
// Current Brain::Impl (from Brain.cpp:39-42):
pImpl->planner = std::make_unique<NeuralPlanner>();

// In Brain::step() - Step 10 (from Brain.cpp:530-533):
// ========== STEP 10: Update concept formation ==========
if (pImpl->conceptFormation) {
    // Would process current neural activity patterns to form concepts
    // This requires sensory state encoding
}
```

### NeuralPlanner Capabilities:
- **Action sequence planning** with cost evaluation
- **Temporal planning** with step constraints
- **Reward integration** for goal-directed behavior
- **Uncertainty handling** with confidence weights
- **Multi-objective optimization** for complex decisions

## Solution Plan

### 1. Integrate NeuralPlanner into Brain::initialize()

**Current Gap**: NeuralPlanner initialized but never configured for planning.

**Fix**: Configure NeuralPlanner properly:

```cpp
// In Brain.cpp, after other system initializations (around line 249):

// Initialize neural planner
pImpl->planner->initialize(this);
pImpl->planner->setPlanningDepth(5);           // Plan ahead 5 steps
pImpl->planner->setMaxActions(10);           // Consider up to 10 actions
pImpl->planner->setPlanningHorizon(100.0f); // 100ms planning horizon
pImpl->planner->setUncertaintyWeight(0.3f);  // 30% uncertainty
pImpl->planner->setGoalWeight(1.0f);         // Strong goal influence
pImpl->planner->setRiskWeight(0.5f);         // Medium risk sensitivity
```

### 2. Connect NeuralPlanner to Brain::step()

**Current Gap**: NeuralPlanner never updated in brain loop.

**Fix**: Integrate NeuralPlanner with action planning:

```cpp
// In Brain::step() after concept formation update:

// STEP 11: Update neural planner with current state
if (pImpl->planner && pImpl->attention) {
    // Get current perceptual state
    auto currentState = getCurrentPerceptualState();
    
    // Get current motor command from brain
    auto currentAction = produceAction();
    
    // Update planner with current situation
    pImpl->planner->update(currentState, currentAction, currentTime);
    
    // Generate action plans
    std::vector<PlannedAction> plans = pImpl->planner->planActions();
    
    // Store plans in working memory for attention
    if (pImpl->workingMemory && !plans.empty()) {
        pImpl->workingMemory->storePlanningContext(plans);
    }
}
```

### 3. Connect NeuralPlanner to Action Selection

**Current Gap**: AgentBrain.action selection is simple and disconnected.

**Fix**: Enhance AgentBrain with neural planner integration:

```cpp
// In AgentBrain::decodeMotorCommand():
MotorCommand AgentBrain::decodeMotorCommand() {
    MotorCommand decoded = decodeFromMotorNeurons();
    
    // NEW: Apply neural planning
    if (brain_ && brain_->getPlanner() && curiosityEnabled_) {
        // Get current perceptual state
        auto currentState = getCurrentState();
        
        // Get current motor command
        auto currentAction = actionFromMotorCommand(decoded);
        
        // Get action plans from neural planner
        std::vector<PlannedAction> plans = brain_->getPlanner()->planActions();
        
        // Apply neural planning to action selection
        decoded = selectWithNeuralPlanner(
            decoded, currentState, currentAction, plans);
    }
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return decoded;
}

MotorCommand AgentBrain::selectWithNeuralPlanner(
    MotorCommand defaultCmd,
    const std::vector<float>& currentState,
    ActionType currentAction,
    const std::vector<PlannedAction>& plans) {
    
    if (plans.empty()) {
        return defaultCmd;
    }
    
    // Evaluate each plan
    std::vector<std::pair<MotorCommand, float>> plannedActions;
    
    for (const auto& plan : plans) {
        if (!plan.actions.empty()) {
            // Convert planned action to motor command
            MotorCommand cmd = convertActionToMotorCommand(plan.actions[0]);
            
            // Weight by plan quality
            float weight = plan.expectedReward - plan.expectedCost;
            weight *= (1.0f + plan.uncertainty * uncertaintyPenalty);
            
            plannedActions.push_back({cmd, weight});
        }
    }
    
    // Select best planned action
    if (!plannedActions.empty()) {
        std::sort(plannedActions.begin(), plannedActions.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Use planned action if significantly better than default
        if (plannedActions[0].second > 0.5f) {
            return plannedActions[0].first;
        }
    }
    
    return defaultCmd;
}
```

### 4. Connect NeuralPlanner to Reward System

**Current Gap**: NeuralPlanner not integrated with reward learning.

**Fix**: Enhance reward integration:

```cpp
// In AgentBrain::applyRewardModulation():
void AgentBrain::applyRewardModulation(float reward, float predictedReward) {
    // ... existing reward modulation ...
    
    // NEW: Update neural planner with reward prediction error
    if (brain_ && brain_->getPlanner()) {
        // Compute prediction error
        float predictionError = reward - predictedReward;
        
        // Update planner's value estimates
        brain_->getPlanner()->updateValueEstimates(predictionError);
        
        // Adjust planning strategies based on prediction error
        adjustPlanningStrategy(predictionError);
    }
}

void AgentBrain::adjustPlanningStrategy(float predictionError) {
    // Use prediction error to modulate planning behavior
    if (std::abs(predictionError) > 0.5f) {
        // High error = increase exploration
        plasticityModifier_ *= 1.2f;
    } else if (predictionError > 0.0f) {
        // Positive error = reinforce current strategy
        plasticityModifier_ *= 0.9f;
    } else {
        // Negative error = explore alternatives
        // Neural planner already handles this with uncertainty
    }
}
```

### 5. Connect NeuralPlanner to Concept Formation

**Current Gap**: NeuralPlanner and ConceptFormation disconnected.

**Fix**: Integrate concept-based planning:

```cpp
// In Brain::step() after neural planner update:

// Connect neural planner to concept formation
if (pImpl->planner && pImpl->conceptFormation) {
    // Get current neural activity patterns
    auto currentPatterns = getCurrentNeuralPatterns();
    
    // Let concept formation identify abstract concepts
    std::vector<Concept> concepts = 
        pImpl->conceptFormation->identifyConcepts(currentPatterns);
    
    // Update planner with concepts
    pImpl->planner->updateConcepts(concepts);
    
    // Generate concept-based action plans
    auto conceptPlans = 
        pImpl->planner->planWithConcepts(concepts);
    
    // Store concept plans in working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->storeConceptPlans(conceptPlans);
    }
}
```

### 6. Connect NeuralPlanner to Memory Systems

**Current Gap**: NeuralPlanner not integrated with memory systems.

**Fix**: Enhanced memory integration:

```cpp
// In Brain::step() after neural planner update:

// Connect neural planner to episodic memory
if (pImpl->planner && pImpl->episodicMemory) {
    // Get planned actions
    std::vector<PlannedAction> plans = pImpl->planner->planActions();
    
    // Store successful plans in episodic memory
    for (const auto& plan : plans) {
        if (plan.expectedReward > 0.5f) {
            // Create planning episode
            EpisodicMemoryItem planEpisode;
            planEpisode.timestamp = currentStep;
            planEpisode.planningContext = plan.context;
            planEpisode.actionsPlanned = plan.actions;
            planEpisode.expectedReward = plan.expectedReward;
            planEpisode.planningConfidence = plan.confidence;
            
            pImpl->episodicMemory->storeEpisode(planEpisode);
        }
    }
}
```

### 7. Connect NeuralPlanner to Development System

**Current Gap**: NeuralPlanner not affected by development stages.

**Fix**: Development-aware planning:

```cpp
// In AgentBrain::updateDevelopment():
void AgentBrain::updateDevelopment(double timestep) {
    // ... existing development ...
    
    // NEW: Update neural planner with developmental stage
    if (brain_ && brain_->getPlanner()) {
        // Adjust planning parameters based on developmental stage
        DevelopmentalStage stage = brain_->getDevelopmentalStage();
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                brain_->getPlanner()->setPlanningDepth(2);      // Simple plans
                brain_->getPlanner()->setMaxActions(5);         // Fewer options
                brain_->getPlanner()->setUncertaintyWeight(0.8f); // High uncertainty
                break;
                
            case DevelopmentalStage::CriticalPeriod:
                brain_->getPlanner()->setPlanningDepth(3);      // Medium plans
                brain_->getPlanner()->setMaxActions(8);         // More options
                brain_->getPlanner()->setUncertaintyWeight(0.5f); // Medium uncertainty
                break;
                
            case DevelopmentalStage::Maturation:
                brain_->getPlanner()->setPlanningDepth(5);      // Full plans
                brain_->getPlanner()->setMaxActions(10);        // Many options
                brain_->getPlanner()->setUncertaintyWeight(0.3f); // Low uncertainty
                break;
                
            case DevelopmentalStage::Adult:
                brain_->getPlanner()->setPlanningDepth(3);      // Strategic plans
                brain_->getPlanner()->setMaxActions(8);         // Balanced options
                brain_->getPlanner()->setUncertaintyWeight(0.1f); // Low uncertainty
                break;
        }
    }
}
```

## Implementation Priority

1. ✅ Identify disconnected NeuralPlanner system (completed)
2. 🔄 Configure NeuralPlanner in Brain::initialize()
3. 🔄 Connect NeuralPlanner to Brain::step()
4. 🔄 Enhance AgentBrain with neural planning
5. 🔄 Connect to reward system
6. 🔄 Connect to concept formation
7. 🔄 Connect to memory systems
8. 🔄 Integrate with development system
9. 🔄 Test integration
10. 🔄 Add to Phase 6 demo

## Testing Implementation

```bash
# Build and run Phase 6 demo with neural planner integration
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_phase6_demo

# Expected outputs:
# - Neural planner generating action sequences
# - Planning-based action selection
# - Concept-guided planning
# - Development-aware planning
# - Reward-based plan evaluation
# - Memory integration for learning
```

## Expected Improvements

1. **Strategic Planning**: Brain plans ahead rather than just reacting
2. **Action Optimization**: Actions selected based on predicted outcomes
3. **Concept-Based Planning**: Plans based on abstract concepts rather than just patterns
4. **Development-Aware Planning**: Planning complexity adapts with development
5. **Reward-Aware Planning**: Plans evaluated with future rewards in mind
6. **Memory-Guided Planning**: Past experiences inform action planning
7. **Uncertainty Handling**: Plans account for prediction uncertainty

This integration makes neural planning the central decision-making system, connecting perception, memory, and action through sophisticated planning algorithms.