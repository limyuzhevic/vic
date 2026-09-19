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

## Phase 6 — Partially Integrated - Systems Connected (IN PROGRESS)

**Status**: IN PROGRESS - INTEGRATION PARTIALLY ACHIEVED (35.8% complete)

**Goal**: CONNECT existing systems rather than add new disconnected components

### Current Integration Status

#### What's Working (Partially Integrated)
- ✅ **Neural Core**: LIF neurons with spikes and synaptic transmission
- ✅ **Basic Plasticity**: STDP, Hebbian, and structural plasticity (periodic)
- ✅ **Embodiment**: Basic sensory-motor loop via SimpleWorld
- ✅ **Simple Learning**: Reward-modulated STDP via dopamine
- ✅ **Development**: Age-based plasticity modifiers

#### What's Disconnected (Not Integrated)
- ❌ **Working Memory**: Defined but not connected to brain loop
- ❌ **Episodic Memory**: Stores experiences but never replayed or consolidated
- ❌ **Semantic Memory**: Empty implementation
- ❌ **Procedural Memory**: Empty implementation
- ❌ **Associative Memory**: Empty implementation
- ❌ **Prediction System**: Implemented but never trains
- ❌ **Neural Planner**: Implemented but never instantiated or used
- ❌ **Concept Formation**: Implemented but never processes experiences
- ❌ **Self-Model**: Implemented but never updates
- ❌ **Social Learning**: Implemented but never observes
- ❌ **Prediction System**: Implemented but never trains
- ❌ **Full Neuromodulation**: Only dopamine (ACh, NE, 5-HT are stubs)
- ❌ **Persistence**: Checkpoints not connected to Brain
- ❌ **Memory Consolidation**: No experience storage or replay
- ❌ **Planning**: No goal-directed behavior
- ❌ **Attention**: Not connected to perception/action
- ❌ **Development**: Only affects structural plasticity, not broader effects

### Critical Integration Gaps

1. **Memory Integration Gap**: Working memory exists but never receives sensory input or influences action selection
2. **Cognition Integration Gap**: All cognitive systems (planner, concept formation, self-model) are implemented but never instantiated or used
3. **Prediction Integration Gap**: Prediction system exists but never processes sensory data or affects learning
4. **Neuromodulation Integration Gap**: Only dopamine implemented (scales STDP), other modulators are stubs
5. **Persistence Integration Gap**: Checkpoint system exists but Brain::save/load are stubs

### Phase 6 Priority Actions

#### Immediate (Next Steps)
1. **Connect WorkingMemory** to sensory processing in Brain::step()
2. **Connect EpisodicMemory** to experience logging and replay
3. **Connect PredictionSystem** to sensory processing and learning
4. **Connect NeuralPlanner** to action selection and motivation
5. **Connect ConceptFormation** to experience processing and generalization

#### Phase 6 Completion Checklist
- [ ] Brain::getWorkingMemory() returns functional working memory
- [ ] Brain::getEpisodicMemory() stores and replays experiences
- [ ] Brain::getPredictionSystem() trains on sensory data
- [ ] NeuralPlanner instantiated and connects to action selection
- [ ] ConceptFormation processes experiences for generalization
- [ ] Full neuromodulation (ACh, NE, 5-HT) implemented
- [ ] Brain::save() and Brain::load() connected using CheckpointSystem
- [ ] Sleep/rest cycle with memory consolidation
- [ ] Replay mechanism
- [ ] Development effects on memory and attention

### Future Phase 7 Directions

After Phase 6 completion, Phase 7 could focus on:
- Advanced neuromodulator interactions
- Body model learning and prediction
- Hierarchical planning structures
- Full reinforcement learning integration
- Glial cell interactions
- Anatomically constrained connectivity
- Distributed simulation capabilities

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

### Version History

- v0.1.0 (Phase 1): Initial skeleton release
- v0.2.0 (Phase 2): Working neural core with real spiking dynamics
- v0.3.0 (Phase 3): World interaction and developmental mechanisms
- v0.4.0 (Phase 4): Emerging cognition - prediction, memory, attention, planning, self-model, social learning
- v0.5.0 (Phase 5): Scaling + Research Platform - performance optimizations, biological fidelity, lifelong learning
- v0.6.0 (Phase 6): Partial integration - systems connected (35.8% complete)

**Phase 6 Status**: Currently IN PROGRESS - The brain has basic neural dynamics and sensory-motor loops but most cognitive systems remain disconnected. The integration effort continues with focus on connecting existing systems rather than adding more components.