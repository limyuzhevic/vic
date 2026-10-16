# Critical Integration Fixes for NLM Phase 6

## Current Integration Status

**✅ Working Systems:**
- NeuralWorkingMemory: Fully functional with competition, decay, rehearsal
- NeuralEpisodicMemory: Full storage, retrieval, replay capabilities
- NeuralAssociativeMemory: Pattern association and spreading activation
- Brain::save/load: Basic implementation (needs validation)
- Neuromodulation: Dopamine partially functional (PDE-based plasticity)

**⚠️ Issues Identified:**
1. PredictionSystem: Not integrated into brain step loop
2. Cognitive systems (NeuralPlanner, ConceptFormation): Not used in agent loop
3. Neuromodulation: ACh, NE, 5-HT are just placeholders
4. Memory integration: Working memory update during sensory processing incomplete
5. Episodic memory integration: Not connected to agent's decision making

## Implementation Plan

### Phase 1: Prediction System Integration

**Files to modify:**
- `src/brain/Brain.cpp` - Connect prediction to sensory processing
- `src/prediction/PredictionSystem.cpp` - Implement prediction methods
- `src/agent/AgentBrain.cpp` - Use predictions for decision making

**Key changes:**
1. Implement PredictionSystem::predictNextState()
2. Connect prediction errors to dopamine signaling
3. Use predictions in NeuralPlanner for action evaluation
4. Integrate prediction into working memory for state representation

### Phase 2: Cognitive System Integration

**Files to modify:**
- `src/agent/AgentBrain.cpp` - Integrate NeuralPlanner, ConceptFormation
- `src/brain/Brain.cpp` - Call cognitive system updates
- `src/cognition/` - Complete implementation

**Key changes:**
1. Make NeuralPlanner::planAction() use predictions and memory
2. Connect ConceptFormation to episodic memory for concept learning
3. Implement SelfModel for body schema and action effects
4. Integrate SocialLearning for imitation and cultural transmission

### Phase 3: Complete Neuromodulation

**Files to modify:**
- `src/neuromodulation/Acetylcholine.cpp` - Implement attention effects
- `src/neuromodulation/Norepinephrine.cpp` - Implement arousal effects
- `src/neuromodulation/Serotonin.cpp` - Implement mood effects
- `src/brain/Brain.cpp` - Connect neuromodulators to learning

**Key changes:**
1. Implement ACh effects on working memory and attention
2. Implement NE effects on arousal and memory consolidation
3. Implement 5-HT effects on behavioral flexibility and decision making
4. Connect neuromodulators to prediction error and learning

### Phase 4: Memory Integration

**Files to modify:**
- `src/brain/Brain.cpp` - Connect working memory to sensory input
- `src/agent/AgentBrain.cpp` - Use episodic memory for planning
- `src/memory/` - Enhance retrieval for agent use

**Key changes:**
1. Working memory: Update with sensory input immediately
2. Episodic memory: Connect to agent's decision history
3. Associative memory: Use for action-outcome associations
4. Implement memory-guided planning

### Phase 5: Advanced Features

**Files to create:**
- `tests/test_prediction_integration.cpp` - Test prediction system
- `tests/test_cognitive_integration.cpp` - Test cognitive systems
- `src/experiments/ContinualLearningExperiment.cpp` - Validate integration

**Key features:**
1. Replay mechanism during rest with neuromodulation
2. Ablation system for testing integration
3. Performance benchmarking
4. Developmental progression tracking

## Implementation Priorities

### Immediate (Week 1-2):
1. Fix PredictionSystem implementation
2. Connect prediction to neuromodulation
3. Basic cognitive system integration

### Medium-term (Week 3-4):
1. Complete neuromodulation system
2. Enhance memory-agent integration
3. Add replay mechanism

### Advanced (Week 5-6):
1. Complete cognitive system integration
2. Add advanced features
3. Comprehensive testing

## Expected Results

After implementation:
- Phase 6 completion: 95% → 100%
- Integrated brain-agent-world loop
- Full neuromodulatory control
- Working memory driving decision making
- Episodic memory guiding behavior
- Prediction-based planning
- All cognitive systems functional