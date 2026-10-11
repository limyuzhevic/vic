# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Neural Core
- Working LIF neuron dynamics with event-driven spike propagation
- Synaptic transmission with delays and excitation/inhibition
- STDP and Hebbian plasticity rules
- Structural plasticity with synaptogenesis/pruning
- Spike propagation system
- Embodied agent with sensory/motor capabilities

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity (STDP scaling)
- Curiosity drives exploration behavior (integrated into AgentBrain)
- Novelty detection integrated with sensory processing
- Prediction error signals available
- Limited other neuromodulators (stubbed)

#### Memory Systems (DISCONNECTED)
- Working memory with persistent activity and competition (NeuralWorkingMemory defined but disconnected)
- Episodic memory with experience encoding (NeuralEpisodicMemory defined but disconnected)  
- Associative memory (partial implementation, disconnected)
- All other memory types: placeholder/stub

#### Prediction System (DISCONNECTED)
- Next state prediction (PredictionSystem implemented but disconnected)
- Prediction error computation
- Confidence tracking
- History tracking

#### Cognition Systems (DISCONNECTED)
- Neural planner with action sequence evaluation (implemented but not used)
- Concept formation from experience patterns (implemented but not used)
- Attention with competitive selection dynamics (partial implementation)
- Self-model, social learning, spatial/temporal relations (implemented but disconnected)

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development
- Limited integration - mostly only affects structural plasticity rates

#### Persistence (DISCONNECTED)
- Checkpoint system (fully implemented but disconnected)
- Brain save/load functions are stubs
- Not connected to main brain

#### Replay and Consolidation (DISCONNECTED)
- Episodic memory replay during simulation (not implemented)
- Memory consolidation for important episodes (not implemented)
- Integration with sleep/rest cycle (not implemented)

#### Integration Status: ~36% COMPLETE
Based on Phase 6 Final Audit - 43/120 points achieved
Critical gap: Most systems exist but are disconnected from the main brain loop

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   ├── PHASE6_FINAL_AUDIT.md      # Phase 6 audit
│   └── PHASE6_FINAL_RESULTS.md   # Current status report
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated/different levels)
│   ├── memory/        # Memory systems (defined but mostly disconnected)
│   ├── prediction/     # Prediction systems (defined but disconnected)
│   ├── cognition/      # Cognitive mechanisms (defined but disconnected)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (In Progress - Final Integration)
- ALL SYSTEMS DEFINED but mostly DISCONNECTED
- Critical gap: Integration, not more features
- ~36% complete according to audit (43/120 points)
- Focus on connecting existing systems, not building new ones
- Must complete integration before claiming functional brain

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 investigates:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Current Architecture Philosophy (Reality Check)

The NLM brain currently operates as:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes) ✅
  ↓
INTERNAL STATE (working memory, attention) ❌ NOT CONNECTED
  ↓
MEMORY / PREDICTION ❌ NOT CONNECTED
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity) ⚠️ PARTIAL
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural) ✅
  ↓
MEMORY / DEVELOPMENT ⚠️ LIMITED
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

**Key Reality:** Most connections are broken. The brain has the anatomy of a cognitive system but lacks the integration that makes it function as a coherent artificial brain.

## Integration Status

- **Complete:** Neural core, basic reward processing, embodied agent
- **Partial:** Neuromodulation (dopamine only), development (limited)
- **Disconnected:** Memory systems, prediction, most cognition, persistence, replay
- **Priority:** CONNECT existing systems, not build more

## License

MIT

## Authors

Research project — See docs for scientific background.
