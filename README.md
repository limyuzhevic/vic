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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/        # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
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
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 4 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 5 (Complete)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

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

## Enhanced Phase 2 Improvements

Phase 2 has been significantly enhanced with configurable, comprehensive testing capabilities:

### Enhanced Testing Framework

The main executable `nlm` now includes an **enhanced testing framework** with:

#### Configurable Test Scenarios
- **Test 1**: Basic neural connectivity with configurable neuron indices and input patterns
- **Test 2**: Plasticity learning experiments with customizable step counts and input patterns
- **Test 3**: Enhanced STDP verification with configurable trials and neuromodulation parameters
- **Test 4**: Advanced network dynamics with adjustable current injection and duration
- **Test 5**: Memory system integration with configurable episode counts

#### Command-Line Options
```bash
# Run basic tests
./nlm --config=myconfig.cfg

# Run with specific test configurations
./nlm --config=myconfig.cfg --test2_steps=2000 --test3_trials=100

# Run comprehensive integration tests
./nlm --config=myconfig.cfg --integration-tests

# View all options
./nlm --help
```

#### Advanced Configuration Options
The enhanced configuration system now supports:
- **Test-specific parameters** for granular control over individual tests
- **Integration test flags** to enable comprehensive system validation
- **Performance and stability settings** for stress testing
- **Research and experimentation options** for reproducible experiments

### Improved Brain Architecture

The `Brain.cpp` implementation has been restructured with:

#### Helper Functions
- **Modular initialization**: Separate functions for region creation, input/output setup, and system initialization
- **Improved organization**: Clear separation of concerns for memory, prediction, cognition, and neuromodulation systems
- **Better documentation**: Comprehensive comments explaining each component's role

#### Enhanced Integration
- **Complete system initialization**: All 6 phases integrated into a coherent brain loop
- **Memory system cohesion**: Working, episodic, and associative memories working together
- **Neuromodulation effects**: Dopamine, curiosity, and novelty systems affecting neural dynamics
- **Developmental processes**: Stage-based plasticity rate modulation

### Comprehensive Test Coverage

#### New Integration Tests
A new test file `test_integration.cpp` provides:
- **System integration validation**: Testing all brain subsystems together
- **Memory system cohesion**: Verifying working, episodic, and associative memories
- **Neuromodulation integration**: Testing dopamine, curiosity, and novelty systems
- **Developmental processes**: Testing brain maturation stages
- **Learning over time**: Extended simulations with plasticity observation
- **Error handling**: Robustness testing with invalid configurations
- **Performance and stability**: Large-scale network testing

#### Test Statistics and Reporting
The testing framework now provides:
- **Detailed test results**: Individual test pass/fail status with descriptions
- **Comprehensive statistics**: Total tests, passed/failed counts, and learning metrics
- **Integration test reports**: System-wide integration validation results
- **Performance metrics**: Network activity, firing rates, and system stability

### Enhanced Configuration System

#### Advanced Configuration File
The `configs/default.cfg` file now includes:
- **Fixed duplicate keys**: Resolved `dopamine_baseline` duplication
- **Advanced plasticity parameters**: STDP, Hebbian, and structural plasticity settings
- **Neuromodulation settings**: Dopamine scale and curiosity/novelty controls
- **Memory system parameters**: Working memory capacity and episodic storage limits
- **Development system parameters**: Stage-based plasticity modulation
- **Checkpoint system settings**: Backup and recovery configuration
- **Performance tuning**: Neuron parameters and synaptic dynamics
- **Testing options**: Integration test flags and stress testing settings
- **Research options**: Experiment identifiers and reproducibility settings

### Usage Examples

#### Basic Usage (Unchanged)
```bash
# Build and run basic tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run Phase 2 demonstration
./nlm
```

#### Enhanced Testing
```bash
# Run with custom test configuration
./nlm --config=custom_config.cfg --test2_steps=3000 --test3_trials=75

# Enable comprehensive integration tests
./nlm --config=myconfig.cfg --integration-tests

# View help and all options
./nlm --help
```

#### Integration Test Examples
```python
# Python script example for running integration tests
import subprocess
import sys

# Run comprehensive integration tests
result = subprocess.run([
    './nlm', 
    '--config=production_config.cfg',
    '--integration-tests'
], capture_output=True, text=True)

if result.returncode == 0:
    print("✓ All integration tests passed!")
else:
    print("✗ Integration tests failed!")
    print(result.stderr)
```

## License

MIT

## Authors

Research project — See docs for scientific background.
