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

## Phase 6 — Final Integration (IN PROGRESS)
**Status**: Partially integrated (35.8% complete, 43/120 points achieved)

**Key Achievements:**
- ✅ Memory systems (Working Memory, Episodic Memory) integrated and operational
- ✅ Neuromodulation (Dopamine, Novelty, Curiosity) integrated into brain loop
- ✅ Basic plasticity rules (STDP, Hebbian, Structural) working
- ✅ Neural core (LIF dynamics, spike propagation) fully functional
- ✅ Development system partially integrated (affects structural plasticity)
- ✅ Performance infrastructure implemented but not fully integrated into Brain class

**What's Integrated (Phase 6 Goal - Connection over Addition):**
- Sensory input → Brain step loop
- Brain step loop → Working memory updates (basic)
- Working memory → Attention system (basic competition)
- Attention system → Working memory winner selection
- Neuromodulation → Plasticity rate modulation
- Episodic memory → Basic storage every 10 steps
- Replay mechanism → Memory replay every 100 steps
- Development → Structural plasticity rate modulation

**What Remains Not Integrated:**
- ❌ **NeuralPlanner**: Fully implemented but never called by AgentBrain or Brain
- ❌ **ConceptFormation**: Fully implemented but never processes sensory patterns
- ❌ **SelfModel**: Fully implemented but never updates with experience
- ❌ **SocialLearning**: Fully implemented but never observes other agents
- ❌ **PredictionSystem**: Fully implemented but never trained
- ❌ **Full Neuromodulation**: Only dopamine integrated; ACh, NE, 5-HT are stubs
- ❌ **Memory Consolidation**: Basic episodic storage but no long-term consolidation
- ❌ **Sleep/Rest Cycle**: Development stages exist but limited effect
- ❌ **Brain Persistence**: CheckpointSystem implemented but save/load are stubs
- ❌ **Performance Infrastructure**: MemoryPool, EventQueue, SparseConnectivity exist but not used in Brain

**Current Status vs. Claims:**
- **Documentation**: Claims Phase 4/6 complete but implementations remain disconnected
- **README**: Says "Phase 6: Final Integration - NOT STARTED" but integration is partially complete
- **Architecture**: Shows "Phase 6: FINAL INTEGRATION" but most systems are not connected

**Required for True Integration:**
1. **Connect existing cognition systems**: Call NeuralPlanner, ConceptFormation, SelfModel, SocialLearning
2. **Implement missing mechanisms**: Full save/load, sleep/rest cycle, proper neuromodulation
3. **Fix documentation inconsistencies**: Update ALL documentation to reflect actual status
4. **Establish proper integration patterns**: Ensure systems can influence each other

**Next Steps for Phase 6 Completion:**
1. Integrate cognition systems into brain loop
2. Implement full neuromodulation effects
3. Fix memory consolidation mechanisms
4. Update all documentation to reflect actual progress
5. Integrate performance infrastructure into core Brain class

**Phase 6 Goal Achievement:** The primary goal is **connection over addition** - making existing systems work together rather than adding more disconnected components. True integration requires:
- Shared data representations
- Mutual influence between systems  
- Consistent temporal dynamics
- Proper scaling of effects

### Future Directions (Post-Phase 6)
After Phase 6 completion, future work should focus on:
- Advanced neuron models (Hodgkin-Huxley, adaptive models)
- GPU acceleration for large-scale simulations
- Distributed simulation across machines
- More detailed neuromodulator interactions
- Cortical-style hierarchical processing
- Calcium-based plasticity mechanisms
- Glial cell interactions
- Anatomically constrained connectivity
- Reinforcement learning integration
- Hierarchical planning structures

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