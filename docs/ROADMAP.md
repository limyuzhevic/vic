# NLM Roadmap

## Phase 1 — Skeleton (COMPLETE)
**Status**: ✅ Complete

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
**Status**: ✅ Complete

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
**Status**: ✅ Complete

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
**Status**: ✅ Complete

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

## Phase 5 — Scaling + Research Platform (COMPLETE)
**Status**: ✅ Complete

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

## Phase 6 — Final Integration (COMPLETE)
**Status**: ✅ Complete

### Integration Achievements
- [x] **Memory Integration**: All memory systems (working, episodic, associative) connected to neural processing
- [x] **Neuromodulation Integration**: Dopamine, curiosity, and novelty fully integrated with plasticity
- [x] **Prediction Integration**: Prediction system integrated with learning and attention
- [x] **Cognition Integration**: Neural planner, concept formation, self-model, and attention all connected
- [x] **Development Integration**: Developmental stages affect plasticity rates and behavior
- [x] **Persistence**: Checkpoint system fully implemented and connected to Brain save/load
- [x] **Replay & Consolidation**: Episodic memory replay during simulation, consolidation for important episodes
- [x] **Full Integration**: All systems now function as a coherent artificial brain

### Key Features
- **Closed-loop brain architecture** with all systems in one unified framework
- **Experience-driven learning** with memory storage and retrieval
- **Developmental progression** affecting plasticity and behavior
- **Multi-system coordination** without artificial components
- **State persistence** across simulation runs
- **Replay-based consolidation** for maintaining important memories

### Complete Systems
- Neural dynamics (LIF neurons, spike propagation, synaptic transmission)
- Plasticity (STDP, Hebbian, structural)
- Memory (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction systems
- Cognitive mechanisms (attention, planning, concepts, self-model)
- Development stages
- Sensory-motor interface
- Reward learning
- Checkpoint persistence
- Performance infrastructure

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
- v0.6.0 (Phase 6): Final Integration - all systems connected into coherent artificial brain
