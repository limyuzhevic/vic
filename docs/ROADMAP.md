# NLM Roadmap

## Phase 1 — Skeleton (COMPLETE)
**Status**: Complete

- [x] Project structure
- [x] C++20 compilation
- [x] Core types (NeuronId, SynapseId, etc.)
- [x] Configuration system
- [x] Random generator with deterministic seed
- [x] Simulation clock
- [x] Neuron interface
- [x] Synapse interface
- [x] NeuralPopulation
- [x] NeuralRegion
- [x] Brain core
- [x] Spike/event system skeleton
- [x] Plasticity interfaces (placeholder)
- [x] Development interfaces (placeholder)
- [x] Neuromodulation interfaces (placeholder)
- [x] Memory interfaces (placeholder)
- [x] Sensory interfaces (placeholder)
- [x] Motor interface (placeholder)
- [x] Environment interface
- [x] Prediction interface (placeholder)
- [x] Experiment framework
- [x] Checkpoint interface (placeholder)
- [x] Logging system
- [x] Documentation
- [x] Tests

## Phase 2 — Working Neural Core (COMPLETE)
**Status**: Complete

- [x] Real membrane potential dynamics (leaky integrate-and-fire)
- [x] Real synaptic transmission
- [x] Real STDP implementation
- [x] Real Hebbian learning
- [x] Event-driven spike processing
- [x] Refractory period implementation
- [x] Basic plasticity rules working
- [x] Synaptic weight initialization
- [x] Connection initialization
- [x] Spike propagation
- [x] Eligibility traces

## Phase 3 — Development + Environment (COMPLETE)
**Status**: Complete

- [x] Working environment interface (SimpleWorld)
- [x] Basic sensory processing (Vision, Touch, Internal, Proprioception)
- [x] Basic motor output (MotorCommand decoding)
- [x] Developmental system implementation (stages, plasticity modifiers)
- [x] Synaptogenesis implementation
- [x] Pruning implementation
- [x] Reward computation (prediction error)
- [x] Basic neuromodulation (dopamine-like signal)
- [x] Novelty detection
- [x] Curiosity mechanism
- [x] Eligibility traces for reward-modulated learning
- [x] Agent/Body system with energy
- [x] Phase 3 experiment demo

## Phase 4 — Emerging Cognition (COMPLETE)
**Status**: Complete

### Prediction Systems
- [x] NeuralPrediction - Temporal sequence learning
- [x] Action-consequence prediction
- [x] Multi-step prediction
- [x] PredictionErrorSignal - Error computation for learning

### Memory Systems
- [x] NeuralWorkingMemory - Persistent activity for temporary storage
- [x] NeuralEpisodicMemory - Experience encoding in neural patterns
- [x] NeuralAssociativeMemory - Experience-based associations
- [x] ConceptFormation - Pattern discovery without labels
- [x] SpatialRepresentation - Learned spatial relationships
- [x] TemporalRelation - Before/after/sequence encoding

### Cognitive Mechanisms
- [x] AttentionalSelection - Competition-based attention (NOT Transformer)
- [x] NeuralPlanner - Predictive action selection
- [x] SelfModel - Sensorimotor self-awareness
- [x] SocialLearning - Observation and imitation

### Experiments
- [x] TemporalPredictionExperiment
- [x] WorkingMemoryExperiment
- [x] EpisodicRecallExperiment
- [x] ConceptFormationExperiment
- [x] AttentionExperiment
- [x] PlanningExperiment
- [x] SelfModelExperiment
- [x] SocialLearningExperiment
- [x] ContinualLearningExperiment
- [x] GeneralizationExperiment
- [x] Phase4IntegratedExperiment
- [x] RepresentationAnalyzer tools

## Phase 5 — Scaling + Research Platform (IN PROGRESS)
**Status**: Implementation

### Performance Infrastructure
- [x] Performance.hpp - Configuration for optimizations
- [x] MemoryPool.hpp - Pre-allocated neuron/synapse pools
- [x] EventQueue.hpp - Lock-free ring buffers, time-bucketed delayed spikes
- [x] SparseConnectivity.hpp - Adjacency list connectivity
- [x] ParallelProcessing.hpp - Thread pool, parallel for, parallel neural processor
- [x] SIMDVectorization.hpp - Vectorized LIF updates
- [x] OptimizedBrain.hpp - High-performance brain interface

### Research Infrastructure
- [x] CheckpointSystem.hpp - Robust state serialization
- [x] AblationSystem.hpp - Mechanism ablation framework
- [x] ScalingBenchmark.hpp - Automated scaling experiments
- [x] Phase5Experiment.hpp - Lifetime learning experiments

### Biological Fidelity Improvements
- [x] Adaptive neuron models
- [x] Short-term plasticity (facilitation/depression)
- [x] Configurable inhibitory circuits
- [x] Homeostatic plasticity mechanisms
- [ ] Multiple neuron types (Izhikevich, conductance-based)
- [ ] Calcium-based STDP
- [ ] Interneuron diversity

### Scalability
- [x] Event-driven processing (skip inactive neurons)
- [x] Sparse connectivity representation
- [x] SoA neuron state layout
- [x] Multithreading support
- [x] SIMD vectorization
- [x] Memory-efficient synapse storage
- [ ] GPU acceleration
- [ ] Distributed simulation

### Documentation
- [x] docs/PERFORMANCE.md - Performance infrastructure
- [x] docs/BIOLOGICAL_FIDELITY.md - Biological modeling
- [x] docs/SCALING.md - Scaling strategies
- [x] docs/RESEARCH_METHODS.md - Scientific methodology
- [ ] docs/FAILURES.md - Failed experiments (to be created)

## Phase 6 — FUTURE INTEGRATION (UPCOMING)
**Status**: Planning

### Phase 6 Goals
Phase 6 will focus on integrating the existing disconnected systems into a coherent artificial brain. The brain has the *anatomy* of a cognitive system but lacks the *integration* that makes it function as a coherent artificial brain.

### Key Integration Challenges (Phase 6)

#### Priority 1: CONNECT EXISTING SYSTEMS
1. Connect NeuralWorkingMemory to sensory processing
2. Connect NeuralEpisodicMemory to experience logging
3. Connect PredictionSystem to sensory processing
4. Connect NeuralPlanner to action selection
5. Connect ConceptFormation to experience processing

#### Priority 2: IMPLEMENT MISSING MECHANISMS
1. Implement Brain::save() and Brain::load() using CheckpointSystem
2. Implement sleep/rest cycle with memory consolidation
3. Implement replay mechanism
4. Implement full dopamine effects on plasticity
5. Implement attention mechanisms for neuromodulation
6. Implement social learning integration

#### Priority 3: VALIDATE INTEGRATION
1. Test memory retention over time
2. Test prediction accuracy improvement
3. Test continual learning
4. Test developmental progression
5. Test multi-system interaction

#### Priority 4: OPTIMIZE
1. Profile and optimize hot paths
2. Integrate performance infrastructure
3. Enable multithreading where safe

### Phase 6 Integration Architecture
```
WORLD → SENSORY PERCEPT → AgentBrain.processSensoryInput() →
Brain.receiveSensoryInput() → Neural Processing → Memory Systems →
Prediction System → Neuromodulation → AgentBrain.decodeMotorCommand() →
WORLD.applyMotorCommand() → Reward → AgentBrain.applyRewardModulation()
```

## Anti-Transformer Reminder

Throughout all phases, NLM must NEVER become:
- A transformer architecture
- An LLM
- A pretrained model
- A conventional deep learning system
- "LLM + memory"
- "LLM + tools"

The architecture must always be based on:
- Individual neurons
- Synapses
- Spike-based communication
- Neural plasticity
- Developmental processes

If at any point the architecture begins to resemble a transformer, it should be redirected or the project renamed to clarify it is no longer NLM.

## Version History

- v0.1.0 (Phase 1): Initial skeleton release
- v0.2.0 (Phase 2): Working neural core with real spiking dynamics
- v0.3.0 (Phase 3): World interaction and developmental mechanisms
- v0.4.0 (Phase 4): Emerging cognition - prediction, memory, attention, planning, self-model, social learning
- v0.5.0 (Phase 5): Scaling + Research Platform - performance optimizations, biological fidelity, lifelong learning
- v0.6.0 (Phase 6): Final Integration - coherent artificial brain with all systems working together

## Anti-Transformer Reminder

Throughout all phases, NLM must NEVER become:
- A transformer architecture
- An LLM
- A pretrained model
- A conventional deep learning system
- "LLM + memory"
- "LLM + tools"

The architecture must always be based on:
- Individual neurons
- Synapses
- Spike-based communication
- Neural plasticity
- Developmental processes

If at any point the architecture begins to resemble a transformer, it should be redirected or the project renamed to clarify it is no longer NLM.

## Version History

- v0.1.0 (Phase 1): Initial skeleton release
- v0.2.0 (Phase 2): Working neural core with real spiking dynamics
- v0.3.0 (Phase 3): World interaction and developmental mechanisms
- v0.4.0 (Phase 4): Emerging cognition - prediction, memory, attention, planning, self-model, social learning
- v0.5.0 (Phase 5): Scaling + Research Platform - performance optimizations, biological fidelity, lifelong learning