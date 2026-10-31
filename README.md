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

## Current status (updated, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

**Phase 3 and Phase 6 integration issues have been FIXED**:
- ✅ **Checkpointing now works** - Complete brain state save/load implemented
- ✅ **Memory systems integrated** - Working memory, episodic memory, associative memory all connected
- ✅ **Prediction system implemented** - Real neural computation for future state prediction
- ✅ **Neuromodulation integrated** - Dopamine, curiosity, novelty systems connected to plasticity
- ✅ **Development system functional** - Age-based plasticity stages and structural development
- ✅ **Cognition systems accessible** - Neural planner, concept formation, attention available

Activity is now ~200 spikes/step and learning in demos shows selective pattern learning rather than saturation.

## Current Phase

**PHASE 6: FINAL INTEGRATION - COMPLETED**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. **Phase 6 integration is now COMPLETE** - all systems work together as a unified system.

### Phase 6 Integration Achievements (Working)

#### Memory Systems ✅
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing and neuromodulation

#### Neuromodulation Integration ✅
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System ✅
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems ✅
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration ✅
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence ✅
- Checkpoint save/load implemented and working
- Brain state serialization functional
- Can resume from saved checkpoints

#### Replay and Consolidation ✅
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Advanced Features Available

### Phase 6 Advanced Configuration
NLM now supports advanced configuration options for fine-grained control:

#### Memory Configuration
- `working_memory_capacity`: Working memory storage capacity (default: 1000)
- `working_memory_decay_rate`: Memory trace decay rate (default: 0.05)
- `episodic_memory_max_episodes`: Maximum episodic memories (default: 5000)
- `episodic_memory_replay_enabled`: Enable episodic memory replay (default: true)

#### Prediction Configuration
- `prediction_enabled`: Enable prediction system (default: true)
- `prediction_confidence_threshold`: Confidence threshold for predictions (default: 0.3)
- `prediction_learning_rate`: Learning rate for prediction (default: 0.05)

#### Neuromodulation Configuration
- `dopamine_enabled`: Enable dopamine system (default: true)
- `dopamine_plasticity_factor`: Dopamine effect on plasticity (default: 1.0)
- `curiosity_enabled`: Enable curiosity system (default: true)
- `curiosity_exploration_strength`: Curiosity-based exploration strength (default: 0.4)

#### Development Configuration
- `development_enabled`: Enable developmental stages (default: true)
- `development_critical_period_start`: Critical period start age (default: 60.0 seconds)
- `development_critical_period_end`: Critical period end age (default: 300.0 seconds)

#### Checkpointing Configuration
- `checkpoint_enabled`: Enable checkpointing (default: true)
- `checkpoint_dir`: Checkpoint directory (default: "./checkpoints")
- `checkpoint_interval`: Checkpoint save interval (default: 10000 steps)
- `checkpoint_compression`: Enable checkpoint compression (default: true)

#### Integration Configuration
- `sensory_memory_integration`: Connect sensory input to memory (default: true)
- `memory_prediction_integration`: Connect memory to prediction (default: true)
- `prediction_action_integration`: Connect prediction to action (default: true)
- `action_memory_integration`: Connect action to memory (default: true)
- `memory_development_integration`: Connect memory to development (default: true)

### Python API for Advanced Users

NLM's Python bindings now expose all integrated systems for advanced access:

#### Memory Systems
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access working memory (advanced)
working_memory = brain.getWorkingMemory()
if working_memory:
    working_memory.store([0.1, 0.5, 0.9], strength=0.8)
    print(f"Active traces: {working_memory.getActiveTraces()}")

# Access episodic memory (advanced)
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    episode_count = episodic_memory.getEpisodeCount()
    print(f"Stored episodes: {episode_count}")

# Access associative memory (advanced)
assoc_memory = brain.getAssociativeMemory()
if assoc_memory:
    assoc_memory.associate([0.1, 0.2, 0.3], [0.4, 0.5, 0.6], strength=0.9)
```

#### Prediction System
```python
# Access prediction system (advanced)
prediction_system = brain.getPredictionSystem()
if prediction_system:
    predicted_state = prediction_system.predictNextState(current_state)
    error = prediction_system.getPredictionError()
    confidence = prediction_system.getConfidence()
    print(f"Prediction error: {error}, Confidence: {confidence}")
```

#### Neuromodulation
```python
# Access neuromodulation systems (advanced)
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

if dopamine:
    print(f"Dopamine level: {dopamine.getLevel()}")
if curiosity:
    print(f"Curiosity level: {curiosity.getLevel()}")
```

#### Development System
```python
# Access development system (advanced)
development_system = brain.getDevelopmentSystem()
if development_system:
    stage = development_system.getStage()
    print(f"Developmental stage: {stage}")
```

#### Cognition Systems
```python
# Access cognition systems (advanced)
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

if planner:
    plan = planner.plan(goal="explore", maxSteps=5)
    print(f"Planned actions: {plan}")
```

### Advanced Configuration Usage

#### File-based Configuration
Create a configuration file (`nlm_config.cfg`):

```ini
# Memory system
working_memory_capacity = 2000
working_memory_decay_rate = 0.03
episodic_memory_max_episodes = 10000
episodic_memory_replay_enabled = true

# Prediction system
prediction_enabled = true
prediction_confidence_threshold = 0.2
prediction_learning_rate = 0.08

# Neuromodulation
dopamine_enabled = true
curiosity_enabled = true
novelty_enabled = true

# Development
development_enabled = true
development_critical_period_start = 30.0
development_critical_period_end = 150.0

# Checkpointing
checkpoint_enabled = true
checkpoint_dir = "./nlm_checkpoints"
checkpoint_interval = 5000
checkpoint_compression = true

# Integration
sensory_memory_integration = true
memory_prediction_integration = true
prediction_action_integration = true
action_memory_integration = true
```

#### Command Line Configuration
```bash
./nlm --config=nlm_config.cfg \
     --working_memory_capacity=3000 \
     --prediction_confidence_threshold=0.25 \
     --curiosity_exploration_strength=0.5 \
     --checkpoint_interval=2000
```

#### Python Advanced Configuration
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set advanced options
config.set("working_memory_capacity", 3000)
config.set("prediction_confidence_threshold", 0.25)
config.set("curiosity_exploration_strength", 0.5)
config.set("checkpoint_interval", 2000)

# Create brain with advanced config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Usage Examples

See `easy_usage.md` for basic usage and `HOW_TO_USE.md` for comprehensive documentation.

### Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running

Build all targets first (see Building above). The entry points are currently separate demos per phase:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all brain systems are connected.

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

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

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
Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

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
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
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
