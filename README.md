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

### Basic Build (C++ Core)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Building Python Bindings

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python module
pip install .  # Installs pynlm Python bindings
```

## Running Tests

### Phase 2: Real Neural Computation Tests

The Phase 2 tests have been refactored into a modular test suite with improved error handling and command-line options:

```bash
# Run all tests (default)
./nlm_test_refactored

# Run only basic connectivity test
./nlm_test_refactored --connectivity

# Run only plasticity learning experiment
./nlm_test_refactored --plasticity

# Run only STDP verification test
./nlm_test_refactored --stdp

# Run specific test by name
./nlm_test_refactored --test=connectivity

# Show help
./nlm_test_refactored --help
```

### Original Unit Tests

The original unit tests are still available for backward compatibility:

```bash
# Run unit tests
./nlm_test
```

### Original Main Executable

The main executable now uses the TestRunner for Phase 2 tests:

```bash
# Run main executable (runs all Phase 2 tests by default)
./nlm
```

## Project Structure (Updated)

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
├── src/tests/          # Modular test suite (NEW)
│   ├── BrainConnectivityTest.cpp
│   ├── BrainConnectivityTest.hpp
│   ├── PlasticityTest.cpp
│   ├── PlasticityTest.hpp
│   ├── StdpTest.cpp
│   ├── StdpTest.hpp
│   ├── LearningExperiment.hpp
│   ├── TestRunner.cpp
│   └── TestRunner.hpp
├── tests/             # Original unit tests (backward compatibility)
│   ├── CMakeLists.txt
│   ├── test_main.cpp
│   ├── test_types.cpp
│   ├── test_config.cpp
│   ├── test_random.cpp
│   ├── test_clock.cpp
│   ├── test_neuron.cpp
│   ├── test_synapse.cpp
│   ├── test_stdp.cpp
│   └── test_brain.cpp
└── configs/
```

## Key Improvements

### 1. Modular Test Structure
- Each test is now isolated in its own file
- Better separation of concerns
- Easier to maintain and extend

### 2. Enhanced Command-Line Interface
- Supports `--connectivity`, `--plasticity`, `--stdp` flags
- Supports `--test=<name>` for specific test selection
- Includes `--help` and `--version` options
- Backward compatible with original test sequence

### 3. Improved Error Handling
- Comprehensive try-catch blocks
- Detailed error logging
- Graceful failure reporting

### 4. Better Logging
- Consistent logging format
- Progress indicators
- Detailed status reports

## Running Complete Examples

### Full NLM Test Suite

```bash
# Build and run everything
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run all tests
ctest --output-on-failure
```

### Python Development Installation

```bash
# Install in development mode
pip install -e .

# Run Python tests
pytest tests/

# Import and test Python bindings
python3 -c "import pynlm; config = pynlm.createDefaultConfig(); brain = pynlm.createBrain(config); brain.initialize(); print('Successfully created brain with', brain.getTotalNeuronCount(), 'neurons')"
```

## Available Executables After Build

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main test runner (Phase 2 tests) |
| `nlm_test_refactored` | Executable | Modular Phase 2 test suite |
| `nlm_test` | Executable | Original unit test suite |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |

## Testing Notes

- The refactored test suite (`nlm_test_refactored`) maintains backward compatibility with the original test sequence
- Command-line arguments allow selective test execution
- Both test suites (`nlm_test_refactored` and `nlm_test`) are added to CTest
- Original functionality is preserved while improving modularity and maintainability

## Next Steps

1. **Build the project** using the instructions above
2. **Run the tests** to verify functionality
3. **Explore the Python bindings** for interactive experimentation
4. **Read documentation** in the `docs/` directory for deeper understanding
5. **Experiment with configurations** in the `configs/` directory

The NLM project now provides both the original unit tests for comprehensive component testing and a modular, configurable test suite for Phase 2 neural computation verification.

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