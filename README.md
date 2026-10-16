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

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### NLM Application CLI

The NLM application includes comprehensive command-line options for configuring experiments, simulations, and output:

#### Experiment Selection
- `--phase P` or `-p P`: Select Phase (1-6) to control which experiment to run
- `--experiment-type TYPE` or `-e TYPE`: Specify experiment type (learning, connectivity, plasticity, stdp, structural, integrated)
- `--demo-mode` or `-d`: Enable demo mode for quick testing
- `--benchmark`: Run benchmark mode
- `--analyze`: Trigger analysis mode

#### Simulation Parameters
- `--steps S` or `-s S`: Set number of simulation steps
- `--timestep T` or `-t T`: Set simulation timestep (seconds)
- `--neuron-count N` or `-n N`: Set neuron count
- `--region-count R` or `-r R`: Set region count

#### Output Control
- `--verbose` or `-v`: Enable verbose logging
- `--quiet` or `-q`: Suppress most output
- `--output-file FILE`: Write output to file
- `--no-banner`: Suppress banner at startup

#### Checkpoint Management
- `--load-checkpoint`: Load from checkpoint
- `--save-checkpoint`: Save checkpoint
- `--checkpoint-path PATH`: Specify checkpoint path

#### Debug and Analysis
- `--debug`: Enable debug mode
- `--benchmark`: Run benchmark
- `--analyze`: Trigger analysis mode

#### Configuration
- `--config FILE`: Load configuration from file (legacy support)
- `--set KEY=VALUE`: Set individual config option

### Examples

```bash
# Run a specific phase with custom parameters
./nlm --phase 3 --steps 5000 --neuron-count 1000

# Quick demo mode
./nlm -d

# Benchmark with output file
./nlm --benchmark --output-file benchmark_results.txt

# Analysis mode
./nlm --analyze

# Set configuration options
./nlm --set random_seed=42 --set simulation_timestep=0.01

# Enable debug with verbose output
./nlm --debug -v
```

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
│   ├── core/                     # Core utilities
│   │   ├── Types/Types.cpp
│   │   ├── Config/Config.cpp
│   │   ├── Random/Random.cpp
│   │   ├── Logger/Logger.cpp
│   │   └── SimulationClock/SimulationClock.cpp
│   ├── brain/                    # Neural components (integrated)
│   │   ├── Neuron.cpp
│   │   ├── Synapse.cpp
│   │   ├── NeuralPopulation.cpp
│   │   ├── NeuralRegion.cpp
│   │   └── Brain.cpp
│   ├── dynamics/                 # Neural dynamics
│   │   ├── NeuralDynamics.cpp
│   │   └── SpikeSystem.cpp
│   ├── plasticity/               # Plasticity rules
│   │   ├── PlasticityRule.cpp
│   │   ├── Hebbian.cpp
│   │   ├── STDP.cpp
│   │   └── StructuralPlasticity.cpp
│   ├── development/              # Developmental system
│   │   ├── DevelopmentSystem.cpp
│   │   ├── Synaptogenesis.cpp
│   │   ├── Pruning.cpp
│   │   └── Maturation.cpp
│   ├── neuromodulation/          # Neuromodulators (integrated)
│   │   ├── Neuromodulator.cpp
│   │   ├── Reward.cpp
│   │   ├── PredictionError.cpp
│   │   ├── Novelty.cpp
│   │   └── Curiosity.cpp
│   ├── memory/                   # Memory systems (integrated)
│   │   ├── WorkingMemory.cpp
│   │   ├── EpisodicMemory.cpp
│   │   ├── SemanticMemory.cpp
│   │   ├── ProceduralMemory.cpp
│   │   ├── AssociativeMemory.cpp
│   │   ├── NeuralWorkingMemory.cpp
│   │   └── NeuralEpisodicMemory.cpp
│   ├── cognition/                # Cognitive mechanisms (integrated)
│   │   ├── ConceptFormation.cpp
│   │   └── NeuralPlanner.cpp
│   ├── sensory/                  # Sensory processing
│   │   ├── SensoryInput.cpp
│   │   ├── Vision.cpp
│   │   ├── Audio.cpp
│   │   └── InternalSignals.cpp
│   ├── motor/                    # Motor system
│   │   └── MotorSystem.cpp
│   ├── environment/              # Environment interface
│   │   ├── Environment.cpp
│   │   └── Observation.cpp
│   ├── experiments/              # Experiment framework (Phase 6)
│   │   ├── Experiment.cpp
│   │   ├── ExperimentRunner.cpp
│   │   ├── Metrics.cpp
│   │   ├── Phase3Experiment.cpp
│   │   ├── Phase3Demo.cpp
│   │   ├── Phase4Experiment.cpp
│   │   ├── Phase4Demo.cpp
│   │   ├── Phase5Experiment.cpp
│   │   ├── Phase6IntegratedExperiment.cpp
│   │   └── Phase6Demo.cpp
│   ├── agent/                    # Agent system
│   │   ├── SensoryPercept.cpp
│   │   └── AgentBrain.cpp
│   ├── visualization/            # Visualization
│   │   └── VisualizationInterface.cpp
│   └── world/                    # World system
│       └── SimpleWorld.cpp
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
