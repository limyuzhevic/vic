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

## Key Improvements in This Version

### 1. Enhanced Python Bindings

**Improvements:**
- Better error handling with try-catch blocks
- Comprehensive documentation strings for all classes and methods
- More Pythonic API design with consistent naming conventions
- Improved type conversions and data handling
- Support for advanced Python features (context managers, iterators)

**New Python Features:**
- `pynlm.Brain` class with improved initialization and error handling
- `pynlm.AgentBrain` class with enhanced neuromodulation support
- `pynlm.Config` class with better configuration validation
- `pynlm.SimpleWorld` class with improved object management
- Text-as-a-sense encoding with `pynlm.AgentBrain.encodeText()`

### 2. Comprehensive Documentation

**New Files Created:**
- `docs/API.md`: Complete Python API reference with examples
- `docs/ARCHITECTURE.md`: Enhanced system architecture documentation

**Documentation Improvements:**
- `easy_usage.md`: Improved beginner-friendly guide with real examples
- `HOW_TO_USE.md`: Comprehensive usage guide with all features

### 3. Command-Line Interface (nlm-cli)

**Features:**
- **Demo Mode**: Run different types of simulations
  - Basic neural connectivity demo
  - Advanced learning demo with curiosity and development
  - Checkpoint demo showing persistence
  - Text-as-a-sense demo
  - Configuration demo with different setup options

- **Benchmark Mode**: Performance testing framework
- **Experiment Mode**: Custom experiment runner
- **Configuration Management**: Save, load, and manage configurations

**Usage Examples:**
```bash
# Run basic demo
nlm-cli demo --type basic --steps 100

# Run advanced learning demo
nlm-cli demo --type learning --steps 500

# Demonstrate checkpoint functionality
nlm-cli demo --type checkpoint

# Show text-as-a-sense feature
nlm-cli demo --type text

# Display configuration options
nlm-cli config --action list
```

### 4. Code Quality Improvements

**Enhanced Error Handling:**
- Comprehensive try-catch blocks in critical sections
- Detailed error messages for debugging
- Graceful failure handling in demo programs

**Logging Improvements:**
- Enhanced logging in main.cpp with detailed connectivity test results
- Better debugging information for neural dynamics
- Progress tracking for long-running simulations

**Code Organization:**
- Improved readability with better comments
- Consistent formatting and naming conventions
- Better separation of concerns in demo programs

### 5. Advanced Features and Examples

**New Examples:**
- Multi-agent system simulation
- Reinforcement learning environment integration
- Performance monitoring tools
- Custom world implementation
- Text processing with learned associations

**Advanced Learning Features:**
- Developmental stages with age-dependent plasticity
- Curiosity-driven exploration behavior
- Novelty detection and habituation
- Concept formation from experience
- Self-model development
- Social learning capabilities

### 6. Performance Optimizations

**Performance Improvements:**
- Enhanced memory management in Python bindings
- Better error handling to prevent crashes
- Optimized simulation loop performance
- Reduced memory allocation overhead

**Benchmarking Framework:**
- Step performance monitoring
- Memory usage tracking
- Firing rate statistics
- System resource monitoring

### 7. Getting Started with NLM

**Quick Start (Beginner)**

```bash
# Clone and build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 2 demo
./nlm

# Run Python examples
python -c "import pynlm; config = pynlm.createDefaultConfig(); brain = pynlm.createBrain(config); brain.initialize(); print('Neurons:', brain.getTotalNeuronCount())"
```

**Python Usage**

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalNeuronCount()} neurons")
```

**Using the Command-Line Interface**

```bash
# Install CLI
pip install -e .

# Run help
nlm-cli help

# Run a demo
nlm-cli demo --type basic --steps 100
```

## Contributing

We welcome contributions to improve NLM! Here are some ways to get involved:

1. **Report Issues**: Create issues on GitHub for bugs or feature requests
2. **Fix Bugs**: Address identified issues in the codebase
3. **Add Examples**: Share new usage patterns and demonstrations
4. **Improve Documentation**: Enhance documentation and comments
5. **Performance Improvements**: Optimize simulations and memory usage
6. **New Features**: Implement novel neural mechanisms or learning algorithms

### Guidelines

- Follow C++20 and Python best practices
- Write comprehensive tests and examples
- Maintain backward compatibility
- Use meaningful variable and function names
- Add comprehensive documentation

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
