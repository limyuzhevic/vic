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

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

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

## Python Usage

### Installation

```bash
pip install ./python
```

### Basic Usage

```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Create brain with configuration
brain = nlm.createBrain(config)

# Create world
world = nlm.createSimpleWorld()

# Create agent brain
agent_brain = nlm.createAgentBrain(brain)

# Initialize agent brain with world
agent_brain.initialize(world)

# Create neuron population
region = brain.addRegion("Sensory")

# Store experience in memory
pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
brain.getWorkingMemory().store(pattern)

# Plan next action
current_state = [0.2, 0.3, 0.4, 0.5, 0.6]
action = brain.getPlanner().planAction(current_state)
print(f"Planned action: {action}")

# Apply reward
brain.getDopamine().signalReward(0.8)
```

### Advanced Features

```python
# Use prediction system
prediction = brain.getPredictionSystem()
predicted = prediction.predictNextState(pattern)
print(f"Prediction error: {prediction.getPredictionError():.4f}")

# Use concept formation
concept_id = brain.getConceptFormation().presentExperience(
    pattern, [0.1, 0.2, 0.3, 0.4, 0.5], 0.7, 0
)
print(f"Concept formed: {concept_id}")

# Use attentional selection
attended = brain.getAttention().isAttended(neuron_id)
print(f"Neuron attended: {attended}")

# Use neuromodulation
brain.getCuriosity().setLevel(0.6)
brain.getNovelty().setLevel(0.8)
brain.getPredictionError().setLevel(0.3)

# Use episodic memory
episode = nlm.EpisodicMemoryItem()
episode.action = nlm.ActionType.MoveForward
episode.reward = 0.5
brain.getEpisodicMemory().storeEpisode(episode)

# Replay episodes
brain.getEpisodicMemory().replayEpisode(episode)
```

## Phase 6 Integration Test

### Running the Demo

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems work together:

- **Memory Integration**: Working, episodic, and associative memory systems are connected
- **Neuromodulation Integration**: Dopamine, curiosity, and novelty signals affect neural dynamics
- **Prediction Integration**: Predictive coding is active and used for planning
- **Development Integration**: Developmental stages affect plasticity and behavior
- **Checkpointing**: Brain states can be saved and loaded
- **Replay**: Memory replay supports consolidation

### System Status

After running the demo, you'll see:

```
=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING
Replay: WORKING
```

### Configuration

The demo uses the following configuration (see Phase6Config):

- `maxSteps`: Maximum simulation steps (default: 10000)
- `neuronCount`: Number of neurons (default: 1000)
- `regionCount`: Number of brain regions (default: 1)
- `connectionProbability`: Random connectivity probability (default: 0.1)
- `enableCheckpointing`: Enable checkpoint save/load (default: true)
- `enableReplay`: Enable memory replay (default: true)
- `enableDevelopment`: Enable developmental stages (default: true)

## Python API Reference

The Python bindings expose:

### Core Classes

- `Config`: Brain configuration
- `Brain`: Central neural processor
- `SimpleWorld`: Environment simulation
- `AgentBrain`: Interface between brain and world
- `Action`: Motor commands
- `SensoryInput`: Perceptual input
- `Vision`: Visual sensory modality
- `Audio`: Auditory sensory modality

### Advanced Systems

- `NeuralWorkingMemory`: Transient working memory
- `NeuralEpisodicMemory`: Episodic memory for experiences
- `NeuralAssociativeMemory`: Associative memory for pattern relationships
- `PredictionSystem`: Predictive coding system
- `NeuralPlanner`: Action planning system
- `ConceptFormation`: Concept discovery system
- `AttentionalSelection`: Competitive attentional selection

### Neuromodulation

- `Dopamine`: Reward and reinforcement signaling
- `Curiosity`: Exploration motivation
- `Novelty`: Novelty detection
- `PredictionError`: Prediction error signaling

### Constants

- `INVALID_NEURON_ID`: Invalid neuron identifier
- `INVALID_SYNAPSE_ID`: Invalid synapse identifier
- `INVALID_REGION_ID`: Invalid region identifier
- `INVALID_POPULATION_ID`: Invalid population identifier

### Helper Functions

- `getVersion()`: Get NLM version
- `getSystemInfo()`: Get system information and status

## System Architecture

NLM (熙然) implements a sophisticated **Phase 6 integrated artificial brain**:

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

## Key Improvements (Phase 6)

### Memory Systems Integration

1. **Working Memory**: Connected to sensory processing and attention
2. **Episodic Memory**: Stores experiences with temporal and spatial indexing
3. **Associative Memory**: Creates and retrieves pattern relationships

### Neuromodulation Integration

1. **Dopamine**: Reward prediction error signaling and plasticity modulation
2. **Curiosity**: Exploration motivation based on novelty and prediction error
3. **Novelty**: Sensory novelty detection and salience signaling

### Cognitive Integration

1. **Prediction System**: Real predictive coding using neural patterns
2. **Neural Planner**: Action planning using predictive simulations
3. **Concept Formation**: Pattern discovery and category learning
4. **Attentional Selection**: Competitive attentional focus mechanisms

### Development Integration

1. **Developmental Stages**: Initial → Critical Period → Maturation → Adult → Aging
2. **Stage-specific Plasticity**: Different plasticity rates per developmental stage
3. **Structural Development**: Synaptogenesis and pruning with age modulation

### Persistence and Learning

1. **Checkpointing**: Brain state serialization and deserialization
2. **Replay**: Memory replay during consolidation (sleep/rest cycle)
3. **Continual Learning**: Lifelong learning with developmental progression

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No realistic cortical architecture
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

## License

MIT

## Authors

Research project — See docs for scientific background.

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
