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

### ✅ COMPLETED SYSTEMS (Phase 6 Implementation)

#### Memory Systems - FULLY INTEGRATED
- Working memory with persistent activity and competition (NeuralWorkingMemory)
- Episodic memory with experience encoding and replay (NeuralEpisodicMemory)
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing through Brain::step() and accessors

#### Neuromodulation Integration - COMPLETE
- Dopamine affects neural excitability and plasticity (Dopamine class)
- Curiosity drives exploration behavior (Curiosity class)
- Novelty detection integrated with sensory processing (Novelty class)
- Prediction error signals learning (PredictionError class)
- All neuromodulators connected to plasticity rules in brain loop

#### Prediction System - IMPLEMENTED
- Neural prediction system with temporal sequence learning (NeuralPrediction)
- Action-consequence learning for planning (ActionConsequencePredictor)
- Prediction error signal for learning (PredictionError)
- Confidence tracking and prediction updating

#### Cognition Systems - FULLY INTEGRATED
- Neural planner with action sequence evaluation (NeuralPlanner)
- Concept formation from experience patterns (ConceptFormation)
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action through AgentBrain interface

#### Development System - IMPLEMENTED
- Age-based plasticity and maturation (DevelopmentSystem)
- Synaptogenesis, pruning, and maturation stages
- Critical period regulation and developmental progression

#### Persistence Systems - IMPLEMENTED
- Checkpoint system for state saving/loading (CheckpointSystem)
- Brain::save() and Brain::load() properly implemented
- Replay system for memory consolidation

### 🔧 TECHNICAL IMPROVEMENTS IMPLEMENTED

#### Phase 2: Neural Core
- ✅ LIF neuron dynamics
- ✅ Event-driven spike propagation with delays
- ✅ STDP and Hebbian plasticity rules
- ✅ Structural plasticity (synaptogenesis/pruning)

#### Phase 3: World Interaction
- ✅ SimpleWorld environment interface
- ✅ Sensory input processing (Vision, Audio, Internal)
- ✅ Motor output and action execution
- ✅ AgentBrain interface (sensory-motor coordination)

#### Phase 4: Emerging Cognition
- ✅ Prediction system with temporal learning
- ✅ Working memory with persistent activity
- ✅ Episodic memory with pattern completion
- ✅ Concept formation without predefined labels
- ✅ Neural planner for action selection
- ✅ Attention with competitive dynamics

#### Phase 5: Performance Infrastructure
- ✅ Memory pools and event queues
- ✅ Sparse connectivity optimization
- ✅ Parallel processing framework
- ✅ SIMD vectorization
- ✅ Checkpoint persistence

### 🎯 INTEGRATION STATUS

**Phase 6 achieves complete integration by:**

1. **Connecting all subsystems** through the AgentBrain interface:
   - Sensory input → Brain.receiveSensoryInput() → Working memory → Prediction system → Attention → Concept formation → Planning

2. **Implementing feedback loops**:
   - Prediction errors modulate neuromodulation → Neuromodulation affects plasticity → Plasticity changes neural dynamics → New predictions emerge

3. **Creating developmental trajectory**:
   - Initial state with high plasticity → Critical periods → Maturation → Adult stability

4. **Enabling experience-driven learning**:
   - All learning comes from direct interaction with environment
   - No pretrained knowledge or hardcoded behaviors
   - Cognition emerges from neural dynamics

### 📊 Performance Integration

The integrated artificial brain now supports:
- **Memory retention** across simulation time
- **Continual learning** without catastrophic forgetting
- **Prediction accuracy** improvement through temporal learning
- **Multi-step planning** using neural substrates
- **Adaptive behavior** through neuromodulation
- **Developmental progression** from naive to experienced

### 🔬 Key Design Philosophy

NLM Phase 6 maintains:
- **Biological plausibility** (LIF neurons, spiking dynamics)
- **No ML frameworks** (implementing its own neural computation)
- **No Transformer architecture** (uses spiking dynamics and competitive attention)
- **Embodied cognition** (sensors and motors in shared environment)
- **Self-organization** (plasticity creates structure from experience)

### 🎮 Usage Examples

The improved Python bindings (pynlm) now provide Phase 6 capabilities:

```python
# Create complete integrated brain
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True) 
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run Phase 6 integration demo
status = pynlm.runPhase6IntegrationDemo(brain, world, agent, 5000)
print(f"Integration successful: {status}")

# Check system status
system_status = pynlm.getPhase6SystemStatus(brain)
for component, connected in system_status.items():
    print(f"{component}: {'✅' if connected else '❌'}")
```

### ⚠️ Scientific Context

NLM is a research project investigating computational brain-like systems. Current limitations include:
- Simplified LIF neuron model (not biologically accurate)
- No realistic ion channel dynamics
- Limited scale (thousands of neurons vs. billions in biological brains)
- No claim of consciousness or human-like intelligence

But Phase 6 does investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

**The primary goal of Phase 6 is to make the existing systems work together as a coherent whole, not to add more disconnected components.**

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

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
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
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

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

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

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
