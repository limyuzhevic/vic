// NLM Brain Integration - Phase 6: Connect Existing Systems
// This document outlines the integration requirements for the NLM brain codebase
// The Brain.cpp file is already well-structured with system instantiation
// Key focus: Connect existing but disconnected systems into a coherent brain loop

INTEGRATION_REQUIREMENTS:
===================

1. MEMORY SYSTEM INTEGRATION:
   - WorkingMemory: Connect to sensory processing and experience storage
   - EpisodicMemory: Store and retrieve experiences in brain loop
   - AssociativeMemory: Create and use pattern relationships

2. PREDICTION SYSTEM INTEGRATION:
   - Connect prediction system to sensory input and learning
   - Update predictions based on experiences

3. COGNITION SYSTEM INTEGRATION:
   - NeuralPlanner: Connect to action selection and goals
   - ConceptFormation: Connect to memory consolidation and planning
   - SelfModel: Connect to sensorimotor learning and body awareness
   - SocialLearning: Integrate into action selection loop
   - SpatialRepresentation: Connect to navigation and planning

4. NEUROMODULATION INTEGRATION:
   - Beyond STDP scaling: connect to memory, attention, and learning
   - Dopamine: reward, reinforcement, and working memory
   - Serotonin: attention and impulsivity
   - Norepinephrine: arousal and vigilance
   - Acetylcholine: memory consolidation and attention

5. DEVELOPMENT SYSTEM INTEGRATION:
   - Beyond structural plasticity: affect plasticity, attention, memory consolidation, and neuromodulation

6. CHECKPOINT SYSTEM:
   - Implement Brain::save() and Brain::load() using existing CheckpointManager
   - Save memory states, prediction models, concepts, etc.

7. WORKING MEMORY CONNECTIONS:
   - Connect working memory to sensory processing in Brain::receiveSensoryInput()

8. ATTENTION AND CONCEPT FORMATION:
   - Connect attention and concept formation to memory consolidation and planning

9. PLANNING TO ACTION SELECTION:
   - Connect planning to action selection using predictions and goals

10. SELF-MODEL TO SENSORIMOTOR:
    - Connect self-model to sensorimotor learning and body awareness

CURRENT STATE:
=============

Brain.cpp already:
✓ Instantiates all systems
✓ Has a 15-step brain loop
✓ Has basic connections between some systems
✓ Has save() and load() methods (using CheckpointSystem)
✓ Has system accessors

Missing/Needs Integration:
✗ Full connectivity between all systems
✗ Neuromodulators only affect STDP, not other processes
✗ Development only affects structural plasticity
✗ Memory systems only minimally connected
✗ Concept formation and planning not integrated
✗ Self-model not connected to sensorimotor
✗ Social learning not integrated
✗ Spatial representation not integrated

IMMEDIATE INTEGRATION OPPORTUNITIES:
===================================

1. Brain::receiveSensoryInput() - Connect working memory
2. Brain::step() - Add concept formation, prediction updates, attention
3. Neuromodulation - Connect to memory, attention, learning beyond STDP
4. Development - Connect to plasticity rates, attention, memory consolidation
5. Action selection - Connect planning, self-model, social learning
6. Memory consolidation - Integrate attention and concept formation

PRIORITY 1: CONNECT EXISTING SYSTEMS
===================================

Focus on:
1. Working memory ↔ sensory processing
2. Prediction ↔ learning
3. Concept formation ↔ memory consolidation
4. Planning ↔ action selection
5. Neuromodulation ↔ memory, attention, learning
6. Attention ↔ concept formation

These are the most impactful changes for Phase 6 integration.
