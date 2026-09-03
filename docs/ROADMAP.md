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

## Phase 3 — Development + Environment
**Status**: In Progress

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
- [ ] Full episodic memory (Phase 4)
- [ ] Episode recording (Phase 4)

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
