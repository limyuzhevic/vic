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

## Phase 2 — Working Neural Core
**Status**: Not Started

- [ ] Real membrane potential dynamics (leaky integrate-and-fire)
- [ ] Real synaptic transmission
- [ ] Real STDP implementation
- [ ] Real Hebbian learning
- [ ] Event-driven spike processing
- [ ] Refractory period implementation
- [ ] Basic plasticity rules working
- [ ] Synaptic weight initialization
- [ ] Connection initialization
- [ ] Spike propagation

## Phase 3 — Development + Environment
**Status**: Not Started

- [ ] Working environment interface
- [ ] Basic sensory processing
- [ ] Basic motor output
- [ ] Developmental system implementation
- [ ] Synaptogenesis implementation
- [ ] Pruning implementation
- [ ] Reward computation
- [ ] Basic neuromodulation
- [ ] Working memory implementation
- [ ] Episode recording

## Phase 4 — Emerging Cognition
**Status**: Not Started

- [ ] Prediction system implementation
- [ ] Curiosity-driven exploration
- [ ] Episodic memory implementation
- [ ] Semantic memory development
- [ ] Procedural memory (skill learning)
- [ ] Associative memory
- [ ] Attention mechanisms
- [ ] Hierarchical processing
- [ ] Working memory integration

## Phase 5 — Scaling + Research Platform
**Status**: Not Started

- [ ] Performance optimization
- [ ] Multithreading support
- [ ] SIMD support (optional)
- [ ] Large-scale simulation
- [ ] Visualization tools
- [ ] Experiment management
- [ ] Analysis tools
- [ ] Research workflows
- [ ] Documentation of findings

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
