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

### Phase 6 Integration Experiment
```bash
./nlm_phase6_experiment
```

This runs the full Phase 6 integration experiment with detailed metrics and analysis.

### Unit Tests
```bash
make run_tests
```

Or run directly with ctest:
```bash
./nlm_test
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
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase 6 Integration Improvements

### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Testing Improvements

### Comprehensive Test Suite

The project now includes extensive tests covering:

1. **Basic Brain Functionality**
   - Brain creation and initialization
   - Neural connectivity and spike propagation
   - Plasticity learning experiments
   - STDP verification

2. **Phase 6 Integration Tests**
   - Memory system integration
   - Prediction system integration
   - Cognition system integration
   - Neuromodulation integration
   - Development system integration

### Build Commands

```bash
# Build all targets including Phase 6 demo and experiments
make -j4

# Run Phase 6 integration demo
make run_phase6_demo

# Run Phase 6 comprehensive experiment
make run_phase6_experiment

# Run all unit tests
make run_tests

# Or run ctest directly
./nlm_test
```

## Performance and Reliability

### Error Handling
- Consistent error handling patterns across all brain systems
- Null pointer checks and validation
- Graceful failure handling with informative error messages

### Memory Management
- Proper cleanup of all dynamic allocations
- Memory pool optimizations for long-running simulations
- Checkpoint system for state persistence

### Code Quality
- Standardized coding conventions
- Comprehensive documentation
- Clean up of TODO markers and placeholder comments

## Example Usage

### Quick Integration Test
```bash
# Build and run Phase 6 integration demo
make run_phase6_demo
```

### Comprehensive Experiment
```bash
# Build and run full Phase 6 experiment
make run_phase6_experiment
```

### Development Testing
```bash
# Build with debug symbols for development
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
make run_tests
```

## Scientific Validation

The Phase 6 integration tests provide:

1. **Verification that all brain systems work together**
2. **Measurement of learning and adaptation capabilities**
3. **Validation of memory retention and recall**
4. **Testing of prediction accuracy and confidence**
5. **Assessment of neuromodulation effects on behavior**
6. **Demonstration of developmental changes over time**

## License

MIT
