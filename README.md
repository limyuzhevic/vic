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

## Phase 7: Enhanced Experiment Framework (NEW)

### Phase 7: Modular Experiment System
- **New experiment framework** with parameterized tests
- **Experiment orchestrator** for test management
- **Three core experiments** running on Phase 6 integrated brain:
  - Basic connectivity test
  - Plasticity learning experiment
  - STDP verification test
- **Enhanced command-line interface** with help, version, quiet, and benchmark options
- **Comprehensive error handling and logging**
- **Easy to extend** with new experiment types

### Key Features

**Parameterized Experiments:**
```bash
# Configure experiments via command line
./nlm --help
./nlm --version
./nlm --quiet
./nlm --benchmark
```

**Experiment Categories:**
- **Basic Connectivity**: Tests spike propagation through neural network
- **Plasticity Learning**: Demonstrates measurable synaptic changes through experience
- **STDP Verification**: Validates pre-before-post potentiation behavior

**Technical Improvements:**
- Modular architecture with clear separation of concerns
- Dependency injection for better testability
- Parameterized configuration for flexibility
- Enhanced error handling and reporting
- Comprehensive logging and monitoring

## Integration Benefits

**Phase 6 + Phase 7 Together:**
- Phase 6 provides the integrated brain system
- Phase 7 provides comprehensive testing framework
- **Experiments test the complete integrated system**
- **Results validate all systems work together**

**Validation Results:**
✅ Memory systems integrated with neural processing
✅ Neuromodulation affects plasticity and dynamics
✅ Prediction integrated with learning
✅ Development affects plasticity rates
✅ Replay and consolidation functional
✅ Complete brain loop verified

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

### Phase 7 Experiment Framework
```bash
./nlm

# Enhanced command-line options:
./nlm --help           # Show usage
./nlm --version        # Show version info
./nlm --quiet          # Suppress informational output
./nlm --benchmark      # Enable benchmark timing
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
│   ├── experiments/   # Experiment framework (Phase 7 - NEW)
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

### Phase 7 (NEW - Complete - Enhanced Experiment Framework)
- Modular experiment framework with parameterized tests
- Experiment orchestrator for test management
- Enhanced command-line interface and error handling
- All experiments test the Phase 6 integrated brain
- Results validate complete system integration

## Architectural Improvements (Phase 7)

### Before (Phase 6 Only)
```
main.cpp (449 lines)
├── Basic connectivity test
├── Plasticity learning experiment
├── STDP verification test
└── Configuration handling
```

### After (Phase 7)
```
NLMApplication class
├── Configuration management
├── Experiment orchestration
├── Error handling
└── Output reporting

BaseExperiment (abstract)
├── BasicConnectivityExperiment
├── PlasticityLearningExperiment
└── StdpVerificationExperiment

ExperimentOrchestrator
├── Experiment management
├── Brain reset handling
├── Result tracking
└── Summary reporting
```

### Key Improvements
- **Modularity**: Clear separation of concerns
- **Testability**: Dependency injection enabled
- **Parameterization**: Configurable experiment settings
- **Error Handling**: Comprehensive error checking
- **User Experience**: Enhanced command-line interface
- **Extensibility**: Easy to add new experiment types

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

Phase 7 (NEW) INVESTIGATES:
- Whether the experiment framework can properly test the integrated brain
- Whether parameterized experiments can validate system behavior
- Whether error handling can gracefully manage failures
- Whether logging can provide comprehensive insight
- Whether the command-line interface can provide good user experience

## Architecture Philosophy

The NLM brain operates as a closed-loop system (Phase 6) with comprehensive testing framework (Phase 7):

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

                     ↓
                 PHASE 7 EXPERIMENTS
                     ↓
              VALIDATION & ANALYSIS
```

## License

MIT

## Authors

Research project — See docs for scientific background.