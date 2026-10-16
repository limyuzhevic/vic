# NLM Cognitive Integration Guide

## Overview

Cognitive systems in NLM provide higher-level processing functions that go beyond basic neural dynamics. These systems integrate with sensory input, working memory, neuromodulation, and action selection to produce intelligent behavior. This guide documents the integration of all cognitive subsystems within the NLM Phase 6 architecture.

## Cognitive System Architecture

The NLM cognitive system consists of five primary components:

1. **Neural Planner**: Multi-step action sequence evaluation
2. **Concept Formation**: Pattern clustering and category discovery
3. **Self-Model**: Internal representation of self and body schema
4. **Social Learning**: Learning from observing other agents
5. **Spatial Representation**: Environmental spatial relationships

## Neural Planner Integration

### Core Mechanisms

The Neural Planner evaluates action sequences to make optimal decisions:

**Biological Inspiration:**
- Prefrontal cortex planning
- Basal ganglia action selection
- Working memory maintenance during planning
- Hierarchical action control

**Technical Implementation:**
```cpp
class NeuralPlanner {
public:
    // Plan next action given current state and goal
    ActionType planAction(const std::vector<float>& currentState,
                         float targetReward = 0.5f);
    
    // Evaluate a potential action sequence
    PlanningCandidate evaluateSequence(
        const std::vector<ActionType>& actions,
        const std::vector<float>& startState);
    
    // Update plans based on actual outcome
    void updatePlanQuality(const std::vector<ActionType>& plannedActions,
                          const std::vector<ActionType>& actualActions,
                          float actualReward);
    
    // Integrate with working memory
    void integrateWithWorkingMemory(
        const std::vector<Neuron*>& activeNeurons,
        const std::vector<float>& currentGoal);
    
    // Integrate with action selection
    void integrateWithActionSelection(
        const std::vector<ActionType>& availableActions);
};
```

### Integration Points

#### 1. Planning with Working Memory
```cpp
// Neural planner uses working memory for planning
void NeuralPlanner::integrateWithWorkingMemory(
    const std::vector<Neuron*>& activeNeurons,
    const std::vector<float>& currentGoal) {
    
    // Extract goal state from working memory
    std::vector<float> goalState = extractGoalState(currentGoal);
    
    // Use active neural patterns as planning context
    std::vector<float> contextPattern = extractContextPattern(activeNeurons);
    
    // Plan action to achieve goal
    ActionType plannedAction = planAction(goalState, contextPattern);
    
    // Store plan in working memory for execution
    storeActionInWorkingMemory(plannedAction, goalState);
}
```

#### 2. Integration with Action Selection
```cpp
// Neural planner influences action selection
void NeuralPlanner::integrateWithActionSelection(
    const std::vector<ActionType>& availableActions) {
    
    // Get neural planner's preference for each action
    std::vector<std::pair<ActionType, float>> actionPreferences;
    for (const ActionType& action : availableActions) {
        float preference = evaluateActionQuality(action);
        actionPreferences.push_back(std::make_pair(action, preference));
    }
    
    // Combine with neuromodulator influences
    std::vector<std::pair<ActionType, float>> finalPreferences = 
        combineWithNeuromodulators(actionPreferences);
    
    // Update action selection probabilities
    updateActionSelectionProbabilities(finalPreferences);
}
```

#### 3. Integration with Reward System
```cpp
// Neural planner uses reward predictions
void NeuralPlanner::integrateWithRewardSystem(
    const Reward& reward,
    const std::vector<ActionType>& plannedActions) {
    
    // Get expected reward for planned action sequence
    float expectedReward = reward.getExpectedReward(
        plannedActions[0], plannedActions); // Action + sequence
    
    // Get actual reward prediction error
    float predictionError = reward.computePredictionError(
        plannedActions[0], plannedActions);
    
    // Update planner based on prediction error
    updatePlanQuality(plannedActions, predictionError);
}
```

#### 4. Integration with Memory Systems
```cpp
// Neural planner uses episodic memory for learning
void NeuralPlanner::integrateWithEpisodicMemory(
    const std::vector<ActionType>& plannedActions,
    const std::vector<ActionType>& actualActions,
    float actualReward) {
    
    // Create episode with planning experience
    Episode episode;
    episode.plannedActions = plannedActions;
    episode.actualActions = actualActions;
    episode.outcomeReward = actualReward;
    
    // Store in episodic memory
    episodicMemory_->storeEpisode(episode);
    
    // Update planning based on episode
    updatePlanningFromEpisode(episode);
}
```

### Integration Features

**Planning Capabilities:**
- **Multi-step planning**: Consider sequences of actions
- **Goal-directed behavior**: Pursue specific goals with appropriate actions
- **Action evaluation**: Assess quality of action sequences
- **Plan adaptation**: Update plans based on experience

**Integration Dynamics:**
- **Working memory support**: Maintain plans during execution
- **Reward guidance**: Use reward predictions to evaluate actions
- **Memory learning**: Learn from planning successes and failures
- **Neuromodulator influence**: Adjust planning based on learning signals

**Biological Plausibility:**
- **Hierarchical planning**: Multiple levels of action abstraction
- **Temporal discounting**: Consider delayed rewards
- **Uncertainty handling**: Plan under uncertainty
- **Resource constraints**: Consider capacity limitations

## Concept Formation Integration

### Core Mechanisms

Concept Formation discovers recurring patterns from experience:

**Biological Inspiration:**
- Cortical column assembly formation
- Pattern completion in hippocampus
- Categorization in prefrontal cortex
- Generalization through similarity

**Technical Implementation:**
```cpp
class ConceptFormation {
public:
    // Present a new experience to concept formation system
    size_t presentExperience(const std::vector<float>& pattern,
                            const std::vector<float>& features,
                            float reward,
                            SimulationStep currentTime);
    
    // Get the concept that best matches a pattern
    size_t getMatchingConcept(const std::vector<float>& pattern,
                             float similarityThreshold = 0.7f) const;
    
    // Get concept prototype for a given concept ID
    std::vector<float> getConceptPrototype(size_t conceptId) const;
    
    // Integrate with working memory
    void integrateWithWorkingMemory(
        const std::vector<float>& currentPattern,
        const std::vector<size_t>& relevantConcepts);
    
    // Integrate with attention system
    void integrateWithAttention(
        const std::vector<size_t>& activeConceptIds,
        std::vector<size_t>& conceptRelevance);
};
```

### Integration Points

#### 1. Integration with Working Memory
```cpp
// Concept formation uses working memory patterns
void ConceptFormation::integrateWithWorkingMemory(
    const std::vector<float>& currentPattern,
    const std::vector<size_t>& relevantConcepts) {
    
    // Check if current pattern matches existing concepts
    size_t matchingConcept = getMatchingConcept(currentPattern);
    
    if (matchingConcept > 0) {
        // Pattern matches existing concept
        updateConceptWithNewInstance(
            matchingConcept, currentPattern, relevantConcepts);
    } else {
        // Create new concept from pattern
        size_t newConceptId = createNewConcept(
            currentPattern, relevantConcepts);
        
        // Present to working memory for attention competition
        presentConceptToWorkingMemory(newConceptId);
    }
}
```

#### 2. Integration with Attention System
```cpp
// Concept formation influences attentional selection
void ConceptFormation::integrateWithAttention(
    const std::vector<size_t>& activeConceptIds,
    std::vector<size_t>& conceptRelevance) {
    
    // Calculate relevance of each concept to current context
    for (size_t conceptId : activeConceptIds) {
        float relevance = calculateConceptRelevance(
            conceptId, currentProcessingContext_);
        
        conceptRelevance.push_back(conceptId);
    }
    
    // Sort concepts by relevance
    std::sort(conceptRelevance.begin(), conceptRelevance.end(),
              [&](size_t a, size_t b) {
                  return getConceptRelevance(a) > getConceptRelevance(b);
              });
}
```

#### 3. Integration with Memory Systems
```cpp
// Concept formation integrates with memory systems
void ConceptFormation::integrateWithMemorySystems(
    const std::vector<float>& pattern,
    const std::vector<float>& features,
    float reward,
    SimulationStep currentTime) {
    
    // Present experience to concept formation
    size_t conceptId = presentExperience(
        pattern, features, reward, currentTime);
    
    // Store pattern in episodic memory with concept ID
    Episode episode;
    episode.pattern = pattern;
    episode.conceptId = conceptId;
    episode.reward = reward;
    
    episodicMemory_->storeEpisode(episode);
    
    // Update semantic memory with concept
    semanticMemory_->integrateConcept(conceptId);
}
```

#### 4. Integration with Action Selection
```cpp
// Concepts guide action selection
void ConceptFormation::integrateWithActionSelection(
    const std::vector<float>& currentState,
    std::vector<ActionType>& availableActions) {
    
    // Find concepts matching current state
    std::vector<size_t> relevantConcepts = 
        findRelevantConcepts(currentState);
    
    // Filter actions based on concepts
    std::vector<ActionType> conceptFilteredActions;
    for (const ActionType& action : availableActions) {
        if (isConceptuallyAppropriate(action, relevantConcepts)) {
            conceptFilteredActions.push_back(action);
        }
    }
    
    // Use concept prototypes to guide action selection
    ActionType conceptGuidedAction = selectBasedOnConcepts(
        conceptFilteredActions, relevantConcepts);
}
```

### Integration Features

**Pattern Processing:**
- **Pattern clustering**: Group similar patterns into concepts
- **Prototype formation**: Create abstract representations of concepts
- **Instance tracking**: Maintain individual pattern instances
- **Stability measurement**: Determine concept stability over time

**Integration Dynamics:**
- **Working memory competition**: Concepts compete for attentional resources
- **Attentional guidance**: Concepts influence what receives processing priority
- **Memory learning**: Concepts stored and retrieved across memory systems
- **Action guidance**: Concepts filter and guide action selection

**Biological Plausibility:**
- **Hebbian learning**: Concepts formed through repeated pattern exposure
- **Pattern completion**: Partial patterns activate full concepts
- **Generalization**: Concepts handle variations in input patterns
- **Categorization**: Similar patterns grouped into meaningful categories

## Self-Model Integration

### Core Mechanisms

Self-Model represents the agent's internal model of itself:

**Biological Inspiration:**
- Body schema in somatosensory cortex
- Predictive coding of self-generated actions
- Forward models for self-action relationships
- Internal monitoring of agency

**Technical Implementation:**
```cpp
class SelfModel {
public:
    // Record that taking an action caused a specific sensory change
    void recordSelfAction(ActionType action,
                         const std::vector<float>& beforeState,
                         const std::vector<float>& afterState);
    
    // Predict sensory consequence of an action
    std::vector<float> predictActionConsequence(
        ActionType action,
        const std::vector<float>& currentState);
    
    // Get confidence in self-model for a given action
    float getSelfModelConfidence(ActionType action) const;
    
    // Compute self-generated likeness
    float computeSelfGeneratedLikeness(
        const std::vector<float>& beforeState,
        const std::vector<float>& afterState,
        ActionType action) const;
    
    // Integrate with working memory
    void integrateWithWorkingMemory(
        const std::vector<float>& currentState,
        ActionType selectedAction);
    
    // Integrate with action selection
    void integrateWithActionSelection(
        const std::vector<float>& predictedConsequences,
        std::vector<ActionType>& actionProbabilities);
};
```

### Integration Points

#### 1. Integration with Working Memory
```cpp
// Self-model uses working memory patterns
void SelfModel::integrateWithWorkingMemory(
    const std::vector<float>& currentState,
    ActionType selectedAction) {
    
    // Get current action from working memory
    ActionType actionFromWorkingMemory = 
        getActionFromWorkingMemory(currentState);
    
    // Record self-action relationship
    recordSelfAction(actionFromWorkingMemory, currentState,
                     getCurrentSensorimotorState());
    
    // Update self-model with new experience
    updateSelfModel(actionFromWorkingMemory, currentState);
}
```

#### 2. Integration with Action Selection
```cpp
// Self-model influences action selection
void SelfModel::integrateWithActionSelection(
    const std::vector<float>& predictedConsequences,
    std::vector<ActionType>& actionProbabilities) {
    
    // Get self-model predictions for each action
    std::vector<float> actionPredictions;
    for (ActionType action : availableActions_) {
        std::vector<float> predictedConsequence = 
            predictActionConsequence(action, currentState_);
        actionPredictions.push_back(predictedConsequence);
    }
    
    // Modify action selection probabilities based on self-model
    for (size_t i = 0; i < availableActions_.size(); ++i) {
        float predictionConfidence = getSelfModelConfidence(
            availableActions_[i]);
        
        // Boost actions with high self-model confidence
        actionProbabilities[i] *= 
            (1.0f + predictionConfidence * ACTION_CONFIDENCE_GAIN);
    }
}
```

#### 3. Integration with Episodic Memory
```cpp
// Self-model uses episodic memory for learning
void SelfModel::integrateWithEpisodicMemory(
    const std::vector<float>& sensoryInput,
    const std::vector<float>& motorOutput,
    float reward) {
    
    // Record self-action episode
    Episode episode;
    episode.sensoryInput = sensoryInput;
    episode.motorOutput = motorOutput;
    episode.reward = reward;
    episode.isSelfGenerated = true;
    
    // Store in episodic memory
    episodicMemory_->storeEpisode(episode);
    
    // Update self-model based on episode
    updateSelfModelFromEpisode(episode);
}
```

#### 4. Integration with Neuromodulation
```cpp
// Self-model integrates with neuromodulation
void SelfModel::integrateWithNeuromodulation(
    const Dopamine& dopamine,
    const Curiosity& curiosity) {
    
    // Get confidence in self-model actions
    float averageConfidence = getAverageSelfModelConfidence();
    
    // Use dopamine to modulate self-model strength
    float dopamineLevel = dopamine.getLevel();
    updateSelfModelStrength(averageConfidence * dopamineLevel);
    
    // Use curiosity to explore self-model uncertainty
    float curiosityLevel = curiosity.getLevel();
    exploreSelfModelUncertainty(curiosityLevel);
}
```

### Integration Features

**Self-Representation:**
- **Action-effect associations**: Map actions to sensory consequences
- **Predictive forward model**: Anticipate outcomes of own actions
- **Agency detection**: Distinguish self-generated from external events
- **Capability assessment**: Learn own strengths and limitations

**Integration Dynamics:**
- **Working memory support**: Maintain self-knowledge during processing
- **Episodic memory learning**: Store self-action experiences
- **Neuromodulation regulation**: Adjust self-model based on learning signals
- **Action selection guidance**: Use self-model to guide behavior

**Biological Plausibility:**
- **Forward model for self**: Predict consequences of self-actions
- **Internal monitoring**: Track self-generated vs. external events
- **Body schema**: Learn body structure and movement capabilities
- **Predictive coding**: Generate predictions about self-effects

## Social Learning Integration

### Core Mechanisms

Social Learning enables learning from observing other agents:

**Biological Inspiration:**
- Mirror neuron systems
- Observational learning in animals
- Cultural transmission in humans
- Imitation learning

**Technical Implementation:**
```cpp
class SocialLearning {
public:
    // Record observation of another agent's action and effect
    void observeAgentAction(ActionType observedAction,
                           const std::vector<float>& observerState,
                           const std::vector<float>& resultingState);
    
    // Can I imitate this observed action?
    bool canImitate(ActionType observedAction) const;
    
    // Get the best action to imitate given current state
    ActionType getImitationAction(const std::vector<float>& currentState);
    
    // Learn simple communication signal
    void learnCommunicationSignal(const std::vector<float>& signalPattern,
                                 float signalReward);
    
    // Detect if another agent is signaling
    bool detectSignal(const std::vector<float>& neuralPattern) const;
    
    // Integrate with working memory
    void integrateWithWorkingMemory(
        const std::vector<float>& observerState,
        const std::vector<float>& observedPattern);
    
    // Integrate with action selection
    void integrateWithActionSelection(
        const std::vector<ActionType>& possibleActions,
        const std::vector<std::vector<float>>& actionOutcomes);
};
```

### Integration Points

#### 1. Integration with Working Memory
```cpp
// Social learning uses working memory for imitation
void SocialLearning::integrateWithWorkingMemory(
    const std::vector<float>& observerState,
    const std::vector<float>& observedPattern) {
    
    // Store observed pattern in working memory
    workingMemory_->storePattern(observedPattern);
    
    // Compare with current state
    std::vector<float> stateDifference = 
        computeStateDifference(observerState, currentState_);
    
    // Record social learning experience
    recordSocialExperience(observerState, observedPattern, stateDifference);
}
```

#### 2. Integration with Action Selection
```cpp
// Social learning influences action selection
void SocialLearning::integrateWithActionSelection(
    const std::vector<ActionType>& possibleActions,
    const std::vector<std::vector<float>>& actionOutcomes) {
    
    // Evaluate actions based on observed outcomes
    std::vector<std::pair<ActionType, float>> actionValues;
    for (size_t i = 0; i < possibleActions.size(); ++i) {
        ActionType action = possibleActions[i];
        std::vector<float> outcome = actionOutcomes[i];
        
        // Compute action value from observed outcomes
        float actionValue = computeActionValueFromObservation(
            action, outcome, observerState_);
        
        actionValues.push_back(std::make_pair(action, actionValue));
    }
    
    // Select best action for imitation
    ActionType bestImitationAction = selectBestAction(actionValues);
    
    // Apply to action selection
    updateActionSelectionProbabilities(bestImitationAction);
}
```

#### 3. Integration with Episodic Memory
```cpp
// Social learning stores experiences in episodic memory
void SocialLearning::integrateWithEpisodicMemory(
    ActionType observedAction,
    const std::vector<float>& observerState,
    const std::vector<float>& resultingState) {
    
    // Create social learning episode
    Episode episode;
    episode.action = observedAction;
    episode.observerState = observerState;
    episode.resultingState = resultingState;
    episode.isSocialLearning = true;
    
    // Store in episodic memory
    episodicMemory_->storeEpisode(episode);
    
    // Update social learning knowledge
    updateSocialKnowledge(episode);
}
```

#### 4. Integration with Neuromodulation
```cpp
// Social learning integrates with neuromodulation
void SocialLearning::integrateWithNeuromodulation(
    const Dopamine& dopamine,
    const Curiosity& curiosity) {
    
    // Get dopamine level for reinforcement
    float dopamineLevel = dopamine.getLevel();
    
    // Get curiosity level for exploration
    float curiosityLevel = curiosity.getLevel();
    
    // Use dopamine to reinforce successful imitations
    reinforceSuccessfulImitations(dopamineLevel);
    
    // Use curiosity to explore novel social behaviors
    exploreNovelSocialBehaviors(curiosityLevel);
}
```

### Integration Features

**Observational Learning:**
- **Action-effect observation**: Learn from others' actions and outcomes
- **Imitation capability**: Copy successful behaviors from others
- **Communication signals**: Learn social signaling patterns
- **Cultural transmission**: Pass knowledge across agents

**Integration Dynamics:**
- **Working memory storage**: Store observed patterns and outcomes
- **Episodic memory learning**: Learn from social experiences
- **Neuromodulation regulation**: Adjust social learning based on signals
- **Action selection guidance**: Use social knowledge to guide behavior

**Biological Plausibility:**
- **Mirror neuron system**: Activate when observing others' actions
- **Observational learning**: Learn by watching others
- **Imitation behavior**: Copy successful behaviors
- **Social cognition**: Understand others' mental states

## Spatial Representation Integration

### Core Mechanisms

Spatial Representation learns environmental spatial relationships:

**Biological Inspiration:**
- Place cells in hippocampus
- Grid cells in entorhinal cortex
- Head-direction cells
- Path integration systems

**Technical Implementation:**
```cpp
class SpatialRepresentation {
public:
    // Record current position experience
    void recordPosition(float x, float y, 
                       const std::vector<float>& sensoryCues);
    
    // Get predicted position based on path integration
    std::pair<float, float> getPredictedPosition() const;
    
    // Update position estimate based on movement
    void integrateMovement(float dx, float dy);
    
    // Get position neuron activation for a given position
    float getPositionActivation(float x, float y) const;
    
    // Find place neurons near a position
    std::vector<NeuronId> getPlaceNeuronsNear(float x, float y, float radius) const;
    
    // Integrate with working memory
    void integrateWithWorkingMemory(
        const std::vector<float>& currentState,
        const std::pair<float, float>& currentPosition);
    
    // Integrate with action selection
    void integrateWithActionSelection(
        const std::vector<float>& currentState,
        const std::pair<float, float>& targetPosition,
        std::vector<ActionType>& actionOptions);
};
```

### Integration Points

#### 1. Integration with Working Memory
```cpp
// Spatial representation uses working memory
void SpatialRepresentation::integrateWithWorkingMemory(
    const std::vector<float>& currentState,
    const std::pair<float, float>& currentPosition) {
    
    // Extract spatial information from current state
    std::vector<float> spatialFeatures = 
        extractSpatialFeatures(currentState);
    
    // Update spatial representation
    updatePosition(currentPosition.x, currentPosition.y, spatialFeatures);
    
    // Store spatial context in working memory
    workingMemory_->storeSpatialContext(spatialFeatures);
}
```

#### 2. Integration with Action Selection
```cpp
// Spatial representation influences action selection
void SpatialRepresentation::integrateWithActionSelection(
    const std::vector<float>& currentState,
    const std::pair<float, float>& targetPosition,
    std::vector<ActionType>& actionOptions) {
    
    // Calculate navigation actions to target
    std::vector<ActionType> navigationActions = 
        calculateNavigationActions(currentPosition_, targetPosition);
    
    // Filter action options based on spatial context
    std::vector<ActionType> spatiallyAppropriateActions;
    for (const ActionType& action : actionOptions) {
        if (isSpatiallyAppropriate(action, currentPosition_, targetPosition)) {
            spatiallyAppropriateActions.push_back(action);
        }
    }
    
    // Use spatial representation to rank actions
    std::vector<ActionType> rankedActions = 
        rankActionsBySpatialRelevance(spatiallyAppropriateActions);
    
    // Update action selection probabilities
    updateActionSelectionProbabilities(rankedActions);
}
```

#### 3. Integration with Episodic Memory
```cpp
// Spatial representation stores navigation experiences
void SpatialRepresentation::integrateWithEpisodicMemory(
    float x, float y,
    const std::vector<float>& sensoryCues,
    const std::vector<ActionType>& actionsTaken) {
    
    // Create spatial episode
    Episode episode;
    episode.spatialPosition = std::make_pair(x, y);
    episode.sensoryCues = sensoryCues;
    episode.actionsTaken = actionsTaken;
    episode.isSpatialExperience = true;
    
    // Store in episodic memory
    episodicMemory_->storeEpisode(episode);
    
    // Update spatial knowledge
    updateSpatialKnowledge(episode);
}
```

#### 4. Integration with Neuromodulation
```cpp
// Spatial representation integrates with neuromodulation
void SpatialRepresentation::integrateWithNeuromodulation(
    const Dopamine& dopamine,
    const Curiosity& curiosity) {
    
    // Use dopamine for exploration in spatial tasks
    float dopamineLevel = dopamine.getLevel();
    encourageSpatialExploration(dopamineLevel);
    
    // Use curiosity for spatial discovery
    float curiosityLevel = curiosity.getLevel();
    exploreUnchartedAreas(curiosityLevel);
}
```

### Integration Features

**Spatial Processing:**
- **Place cells**: Encode specific spatial locations
- **Grid cells**: Encode continuous spatial coordinates
- **Path integration**: Track position through movement
- **Landmark association**: Link positions to environmental features

**Integration Dynamics:**
- **Working memory support**: Maintain spatial context during navigation
- **Episodic memory learning**: Learn from spatial experiences
- **Neuromodulation regulation**: Adjust spatial exploration based on signals
- **Action selection guidance**: Use spatial knowledge to guide movement

**Biological Plausibility:**
- **Hippocampal spatial coding**: Cell firing patterns encode space
- **Environmental exploration**: Navigate and learn spatial layout
- **Landmark-based navigation**: Use environmental cues for orientation
- **Cognitive map formation**: Develop internal representation of space

## Integration Performance Guidelines

### 1. Cognitive Integration Optimization

**Cognitive Processing:**
- **Working memory allocation**: Balance between different cognitive functions
- **Attention management**: Allocate attention to relevant cognitive tasks
- **Memory storage**: Efficiently store cognitive knowledge and experiences
- **Action planning**: Plan actions using cognitive representations

### 2. Integration Testing

**Unit Tests:**
```cpp
void testNeuralPlannerIntegration() {
    NeuralPlanner planner;
    // Test action planning
    ActionType plannedAction = planner.planAction(currentState_, goalState_);
    assert(plannedAction != ActionType::None);
    
    // Test action evaluation
    PlanningCandidate candidate = planner.evaluateSequence(
        {ActionType::MoveForward, ActionType::TurnLeft}, currentState_);
    assert(candidate.expectedReward > 0.0f);
}

void testConceptFormationIntegration() {
    ConceptFormation conceptFormation;
    // Test concept matching
    size_t conceptId = conceptFormation.getMatchingConcept(
        currentPattern_, SIMILARITY_THRESHOLD);
    assert(conceptId > 0);
    
    // Test concept formation
    size_t newConceptId = conceptFormation.presentExperience(
        pattern_, features_, reward_, currentStep_);
    assert(newConceptId > 0);
}
```

**Integration Tests:**
```cpp
void testCognitiveIntegration() {
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Verify cognitive systems functional
        assert(brain.getNeuralPlanner()->getPlanningDepth() > 0);
        assert(brain.getConceptFormation()->getConceptCount() > 0);
        assert(brain.getSelfModel()->hasSelfModel());
    }
}
```

## Best Practices for Cognitive Integration

### 1. Cognitive Architecture

**Ensure Architecture:**
- **Modular design**: Separate cognitive functions into modules
- **Clear interfaces**: Well-defined interfaces between cognitive components
- **Hierarchical organization**: Multiple levels of cognitive processing
- **Adaptive organization**: Dynamic reorganization based on performance

### 2. Integration Efficiency

**Effective Integration:**
- **Working memory support**: Efficient use of working memory for cognitive tasks
- **Attentional guidance**: Proper allocation of attentional resources
- **Memory learning**: Effective storage and retrieval of cognitive knowledge
- **Action planning**: Efficient planning and execution of actions

### 3. Error Handling

**Robust Cognitive Processing:**
- **Graceful degradation**: Fallback mechanisms when cognitive functions fail
- **Error recovery**: Recovery from cognitive processing errors
- **Adaptive behavior**: Adjustment of cognitive strategies based on errors
- **Learning from errors**: Use cognitive errors to improve performance

### 4. Scalability

**Growing Cognitive Systems:**
- **Distributed processing**: Parallel processing of cognitive functions
- **Hierarchical processing**: Use multiple levels of cognitive abstraction
- **Approximate processing**: Trade accuracy for efficiency where appropriate
- **Incremental learning**: Learn from experience incrementally

## Future Cognitive Integration Extensions

### 1. Advanced Cognitive Systems

**Metacognition:**
- Thinking about thinking
- Self-reflection and self-monitoring
- Cognitive strategy selection
- Performance evaluation and feedback

**Theory of Mind:**
- Understanding others' mental states
- Predicting others' behavior
- Perspective taking
- Empathy and social cognition

### 2. Cognitive Architecture Advances

**Hierarchical Processing:**
- Multiple levels of cognitive abstraction
- Top-down and bottom-up processing
- Cognitive hierarchy learning
- Emergent cognitive representations

**Dynamic Organization:**
- Reconfigurable cognitive architecture
- Experience-dependent cognitive organization
- Adaptive cognitive strategies
- Meta-learning of cognitive functions

### 3. Cognitive Neuroscience Integration

**Biological Plausibility:**
- Incorporate more detailed biological models
- Use neuroscientific data to guide cognitive modeling
- Integrate with detailed neural circuitry
- Respect biological constraints and limitations

**Computational Cognitive Science:**
- Bridge cognitive science and neuroscience
- Use cognitive theories to guide neural modeling
- Integrate multiple levels of analysis
- Connect behavior to neural mechanisms

## Conclusion

Cognitive systems in NLM provide higher-level processing functions that go beyond basic neural dynamics. Integration of cognitive systems requires:

1. **Careful Integration**: Proper integration of cognitive systems with neural processing
2. **Efficient Processing**: Efficient processing of cognitive functions
3. **Adaptive Behavior**: Adaptive behavior based on cognitive processing
4. **Comprehensive Testing**: Verification of cognitive system integration
5. **Continuous Refinement**: Refinement of cognitive integration based on performance

The integrated cognitive system enables the brain to:

- **Plan actions**: Plan and select actions to achieve goals
- **Form concepts**: Categorize and understand the environment
- **Model self**: Develop internal representation of self
- **Learn socially**: Learn from observing others
- **Navigate space**: Learn and use spatial relationships

Cognitive integration is essential for:

- **Goal-directed behavior**: Pursue goals with appropriate actions
- **Generalization**: Apply learned knowledge to new situations
- **Social interaction**: Interact with other agents
- **Environmental adaptation**: Adapt to changing environments
- **Intelligent behavior**: Produce intelligent, adaptive behavior

This foundation enables the investigation of how cognitive mechanisms support emergence, learning, and adaptation in a brain-like system.
