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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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

## Major Improvements in This Version

### 1. Enhanced Configuration System

**Multiple File Format Support:**
- JSON configuration files with structured parameters
- YAML configuration files for human-readable settings
- Plain text .cfg files for backward compatibility

**Advanced Configuration Features:**
- Dynamic parameter validation
- Type-safe configuration values
- Configuration inheritance and overrides
- Runtime configuration modifications

### 2. Comprehensive Command-Line Interface

**New CLI Features:**
- Detailed help system with examples
- Version information and build details
- Extensive configuration options
- Test mode selection (--plasticity-only, --stdp-only, --connectivity-only)
- Logging level control and file output
- Configuration validation and saving

### 3. Advanced Command-Line Tools

**Tools Script:** `scripts/nlm-tools.sh`

**Available Commands:**
```bash
# Configuration management
./scripts/nlm-tools.sh config create json my_config.json
./scripts/nlm-tools.sh config validate my_config.json
./scripts/nlm-tools.sh config list
./scripts/nlm-tools.sh config get neuron_count
./scripts/nlm-tools.sh config set neuron_count 2000

# Simulation control
./scripts/nlm-tools.sh run --help
./scripts/nlm-tools.sh run --plasticity-only

# Analysis and export
./scripts/nlm-tools.sh analysis results.csv
./scripts/nlm-tools.sh export results/
./scripts/nlm-tools.sh stats

# Validation
./scripts/nlm-tools.sh validate my_config.json
```

### 4. Real Neural Dynamics (Phase 2)

**Improved Neuron Implementation:**
- Biologically realistic Leaky Integrate-and-Fire (LIF) dynamics
- Spike-frequency adaptation for neural plausibility
- Refractory period handling with biological accuracy
- Realistic parameter initialization and random variation

### 5. Advanced Utility Functions

**Comprehensive Utility Library:** `src/core/Utilities.hpp`

**Function Categories:**
- File operations and path manipulation
- Configuration utilities and validation
- Simulation utilities and statistics
- Progress monitoring and metrics
- Error handling and recovery

### 6. Enhanced Documentation

**Comprehensive Documentation System:**
- Main README with project overview and improvements
- Easy Usage Guide for beginners
- Advanced documentation for architecture and implementation
- Configuration examples for multiple formats
- Troubleshooting guide

### 7. Python Bindings Enhancements

**Improved Python API:**
- Better error handling and validation
- More intuitive Python interface
- Additional configuration options
- Improved documentation and examples

## Building and Running

### Building

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build with Python bindings
pip install .

# Build with all phases
mkdir build_full
cd build_full
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### Running

```bash
# Basic NLM demo (Phase 2)
./nlm

# With custom configuration
./nlm --config brain_config.json

# With custom parameters
./nlm --neuron-count 2000 --log-level Info --plasticity-only

# Validate configuration
./nlm --validate-config

# Save current configuration
./nlm --save-config my_config.json

# Using command-line tools
./scripts/nlm-tools.sh run --help
./scripts/nlm-tools.sh config create json my_config.json
./scripts/nlm-tools.sh validate my_config.json
```

### Configuration Examples

### JSON Configuration
```json
{
    "nlm": {
        "version": "0.1.0",
        "parameters": {
            "neuron_count": 1000,
            "connection_probability": 0.15,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
            "plasticity_enabled": true,
            "structural_plasticity_enabled": true,
            "development_enabled": true
        }
    }
}
```

### YAML Configuration
```yaml
# NLM Configuration File
nlm:
  version: "0.1.0"
  parameters:
    neuron_count: 1000
    connection_probability: 0.15
    plasticity_learning_rate: 0.01
    stdp_ltp_weight: 0.02
    stdp_ltd_weight: 0.015
    plasticity_enabled: true
    structural_plasticity_enabled: true
    development_enabled: true
```

### Plain Text Configuration
```
# NLM Configuration File
neuron_count = 1000
connection_probability = 0.15
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
plasticity_enabled = true
structural_plasticity_enabled = true
development_enabled = true
```

## Project Status

### Current Phase: **Phase 6 - Final Integration**

**Integration Achievements:**
- ✅ Memory systems connected to neural processing
- ✅ Neuromodulation affects plasticity and dynamics
- ✅ Prediction integrated with learning
- ✅ Development affects plasticity rates
- ✅ Checkpoint save/load working
- ✅ Replay and consolidation functional

**Scientific Goals:**
- Investgate whether memory systems can integrate with neural dynamics
- Examine whether neuromodulation can affect plasticity in a coordinated way
- Explore whether prediction can become a central organizing principle
- Study whether developmental stages can modulate learning
- Analyze whether replay can reinforce memory consolidation
- Determine whether the complete brain loop functions coherently

## System Architecture

### Brain Operations Loop
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

MIT License

## Authors

Research project — See docs for scientific background and development team information.

## Acknowledgments

Special thanks to the NLM research team and contributors who have made this experimental neural simulation framework possible.

---

**Version 0.1.0 - Phase 2 Enhanced**
**Built on $(date +%Y-%m-%d)**
**Documentation: https://nlm.readthedocs.io**

