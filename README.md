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

```bash
./nlm                 # Phase 2 demo in src/main.cpp (basic neural computation with LIF neurons)
./nlm_phase3_demo     # Phase 3 demo in src/experiments/Phase3Demo.cpp (world interaction loop)
./nlm_phase4_demo     # Phase 4 demo in src/experiments/Phase4Demo.cpp (emerging cognition experiments)
./nlm_phase5_demo     # Phase 5 demo in src/experiments/Phase5Demo.cpp (personality/behavior in sandbox)
./nlm_phase6_demo     # Phase 6 demo in src/experiments/Phase6Demo.cpp (integration test of all brain systems)
./nlm_benchmark       # Performance benchmark in benchmark.cpp (simulation performance metrics)
```

`./nlm` (Phase 2) - The core neural computation demo:
- Demonstrates real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons
- Shows event-driven spike propagation with synaptic delays
- Implements STDP and Hebbian plasticity rules
- Demonstrates structural plasticity (synaptogenesis/pruning)
- Validates basic neural connectivity and plasticity through three test stages

`./nlm_phase3_demo` (Phase 3) - World interaction loop demo:
- Simulates an agent interacting with a 2D grid world
- Implements sensory input and motor output processing
- Demonstrates reward prediction error learning
- Includes developmental stages affecting plasticity rates
- Tests novelty and curiosity-driven exploration
- Accepts command-line arguments for number of episodes and steps per episode
- Example: `./nlm_phase3_demo 10 500` (10 episodes, 500 steps each)

`./nlm_phase4_demo` (Phase 4) - Emerging cognition demo:
- Demonstrates neural prediction systems and working memory
- Shows episodic-like memory and concept formation
- Implements neural attention mechanisms (not Transformer)
- Features predictive planning and self-modeling
- Includes social learning capabilities
- Tests continual learning across multiple cognitive domains
- Accepts command-line argument for number of trials (default: 50)
- Example: `./nlm_phase4_demo 100`

`./nlm_phase5_demo` (Phase 5) - Personality/sandbox demo:
- Implements performance optimizations and memory pools
- Demonstrates SIMD vectorization and parallel processing
- Shows checkpoint system with save/load functionality
- Simulates brain-personality traits learned through experience
- Uses a sandbox environment for agent behavior testing
- Measures personality traits like risk preference, exploration/exploitation balance
- Demonstrates how learned preferences survive brain reloads and affect behavior

`./nlm_phase6_demo` (Phase 6) - Integration test:
- Comprehensive test verifying all brain systems are properly connected
- Tests integration of memory systems (working, episodic, associative)
- Validates neuromodulation system integration (dopamine, curiosity, novelty)
- Checks prediction system integration with learning loops
- Verifies cognition system integration (planning, concepts, attention)
- Tests development system integration (plasticity rates with age)
- Demonstrates persistence through checkpoint save/load
- Shows replay and consolidation in memory systems
- Example: `./nlm_phase6_demo` (runs integration verification)

`./nlm_benchmark` (Benchmark):
- Measures simulation performance metrics
- Tests scalability with different brain sizes
- Benchmarks various plasticity rule implementations
- Provides performance comparisons across different configurations


`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── PYTHON_BINDINGS.md          # Python bindings status and TODO
├── pyproject.toml            # Python bindings configuration
├── easy_usage.md             # Python usage documentation (for when bindings are implemented)
├── HOW_TO_USE.md              # Comprehensive documentation
├── LICENSE
├── src/                       # C++ source code
│   ├── brain/                 # Neural components
│   ├── core/                  # Core utilities
│   ├── experiments/           # Experiment implementations
│   ├── performance/           # Performance optimizations
│   ├── sensory/               # Sensory processing
│   ├── motor/                 # Motor system
│   ├── environment/           # Environment interface
│   └── ...
├── tests/                     # C++ tests
└── configs/                   # Configuration files
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
