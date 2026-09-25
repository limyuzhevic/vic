# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a **coherent artificial brain** implementing brain-inspired spiking neural networks in C++20. It features **Phase 6 integration** where all cognitive systems work together as a unified neural system, simulating the complete brain loop: perception → processing → memory → action → world interaction.

**Current Status:** Phase 6 - Final Integration (September 2026)
**Documentation Version:** 1.0.0 (Phase 6 Enhanced)

This document covers:
1. Building the C++ project with Phase 6 integration
2. Installing the enhanced Python library with all cognitive features
3. Using the comprehensive NLM Python API with Phase 6 capabilities
4. Advanced features and integrated cognitive systems
5. Performance optimization and batch processing with new enhancements

> **Note:** NLM has moved beyond having cognitive system "anatomy" to having the "functionality" of a complete brain. All previously designed components are now integrated and working together as a unified artificial intelligence.

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 11+, Clang 14+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **Optional**: NVIDIA GPU acceleration for CUDA builds

### Enhanced Build Steps with Phase 6 Integration

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with Phase 6 integration and enhanced features
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_PHASE6_INTEGRATION=ON \
    -DENABLE_ADVANCED_COGNITION=ON \
    -DENABLE_PERFORMANCE_OPTIMIZATIONS=ON \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DPYTHON_EXECUTABLE=$(which python3)

# Build all targets with maximum optimization
make -j$(nproc) VERBOSE=1

# Run comprehensive tests including Phase 6 integration tests
ctest --output-on-failure -L "phase6"
```

### Build Outputs with Enhanced Features

After successful compilation with Phase 6 integration, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_phase6_demo` | Executable | **Phase 6 integration demo (enhanced)** |
| `nlm_performance_bench` | Executable | Performance benchmarking tool |
| `nlm_advanced_cognition_demo` | Executable | Advanced cognition demo |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_memory` | Static Library | **Memory systems (enhanced Phase 6)** |
| `nlm_prediction` | Static Library | **Prediction system (Phase 4+)** |
| `nlm_planner` | Static Library | **Planning system (Phase 4+)** |
| `nlm_python_bindings` | Library | Python bindings library |

### Enhanced Prerequisites for Phase 6 Integration

- **C++ Compiler**: GCC 11+, Clang 14+, or MSVC 2019+
- **CMake**: Version 3.16 or higher (with modern features)
- **Python**: 3.8+ (for Phase 6 enhanced Python bindings)
- **Build Tools**: Make 4.1+ or Ninja (for optimized compilation)
- **Dependencies**: 
  - **pybind11**: Version 2.11.0+ (required for Python bindings)
  - **Optional Dependencies**: 
    - NVIDIA GPU acceleration for CUDA builds (Phase 6 enhanced)
    - MPI for distributed computing (Phase 6 scaling)
    - OpenMP for multithreading optimization
- **Phase 6 Required Features**: All cognitive systems integration enabled

### Enhanced Build Steps with Full Phase 6 Integration

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with complete Phase 6 integration and all enhancements
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_PHASE6_INTEGRATION=ON \
    -DENABLE_ADVANCED_COGNITION=ON \
    -DENABLE_PERFORMANCE_OPTIMIZATIONS=ON \
    -DENABLE_BIOLOGICAL_FIDELITY=ON \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DENABLE_CUDA=ON -DCUDA_ARCH=7.0 \
    -DENABLE_MPI=ON \
    -DPYTHON_EXECUTABLE=$(which python3)

# Build all targets with maximum optimization for Phase 6
make -j$(nproc) VERBOSE=1

# Run comprehensive Phase 6 integration tests
ctest --output-on-failure -L "phase6" --output-on-failure
```

### Build Outputs with Enhanced Phase 6 Features

After successful compilation with full Phase 6 integration, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable with integrated cognitive systems |
| `nlm_phase6_demo` | Executable | **Phase 6 integration demo (complete)** - Demonstrates all integrated systems |
| `nlm_performance_bench` | Executable | Performance benchmarking tool with cognitive metrics |
| `nlm_advanced_cognition_demo` | Executable | **Advanced cognition demo** - Shows Phase 4+ cognitive capabilities |
| `nlm_research_suite` | Executable | Research experiment framework with integration testing |
| `nlm_core` | Static Library | Core brain components (Phase 6 optimized) |
| `nlm_agent` | Static Library | Agent system with cognitive integration |
| `nlm_memory` | Static Library | **Memory systems (enhanced Phase 6)** - Working, episodic, associative |
| `nlm_prediction` | Static Library | **Prediction system (Phase 4+)** - Temporal and action-consequence |
| `nlm_planner` | Static Library | **Planning system (Phase 4+)** - Neural planning with forward simulation |
| `nlm_python_bindings` | Library | **Enhanced Python bindings** with Phase 6 cognitive API |

### Enhanced Build Options with Phase 6 Features

```bash
# Build with complete Phase 6 integration (recommended for research)
cmake .. -DENABLE_PHASE6_INTEGRATION=ON -DENABLE_ADVANCED_COGNITION=ON

# Build with specific Phase 6 cognitive features
cmake .. -DENABLE_COGNITION_PLANNING=ON -DENABLE_COGNITION_PREDICTION=ON

# Build with advanced neuromodulation
cmake .. -DENABLE_NEUROMODULATION_DOPAMINE=ON -DENABLE_NEUROMODULATION_CURIOUSITY=ON -DENABLE_NEUROMODULATION_NOVELTY=ON

# Build with Phase 6 memory systems
cmake .. -DENABLE_MEMORY_WORKING=ON -DENABLE_MEMORY_EPISODIC=ON -DENABLE_MEMORY_ASSOCIATIVE=ON

# Build with CUDA acceleration for enhanced performance
cmake .. -DENABLE_CUDA=ON -DCUDA_ARCH=7.0

# Build with MPI support for distributed computing
cmake .. -DENABLE_MPI=ON

# Build with custom configuration for research
cmake .. -DCONFIG_FILE=/path/to/custom_config.json

# Install to custom prefix
# cmake .. -DCMAKE_INSTALL_PREFIX=/opt/nlm
# make install
```

---

## Part 2: Installing the Python Library with Enhanced Bindings

### Enhanced Python Package Installation

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy matplotlib

# Build and install enhanced Python bindings with Phase 6 features
pip install .

# Or install in development mode with all enhancements
pip install -e .[dev,perf]
```

### Enhanced Python Package Structure

The enhanced Python bindings now include:

```
pynlm/
├── __init__.py                    # Enhanced Python bindings
├── core/                          # Core utilities
│   ├── __init__.py
│   └── brain.py                   # Enhanced brain API
├── agent/                         # Agent system
│   ├── __init__.py
│   └── agent_brain.py             # Enhanced agent brain
├── memory/                        # Memory systems
│   ├── __init__.py
│   ├── working_memory.py          # Working memory (Phase 6 enhanced)
│   ├── episodic_memory.py         # Episodic memory (Phase 6 enhanced)
│   └── associative_memory.py      # Associative memory (Phase 6 enhanced)
├── cognition/                     # Cognitive systems
│   ├── __init__.py
│   ├── attention.py               # Attention system (Phase 4+)
│   ├── planning.py                # Planning system (Phase 4+)
│   └── self_model.py              # Self-model (Phase 4+)
├── neuromodulation/               # Neuromodulation
│   ├── __init__.py
│   ├── dopamine.py               # Dopamine system
│   ├── novelty.py                # Novelty detection
│   └── curiosity.py              # Curiosity system
├── experiments/                   # Experimental framework
│   ├── __init__.py
│   ├── phase3_experiments.py      # Phase 3 experiments
│   ├── phase4_experiments.py      # Phase 4 experiments
│   └── phase6_integrated.py       # Phase 6 integration experiments
└── utils/                         # Utilities
    ├── __init__.py
    ├── config_utils.py            # Configuration utilities
    └── performance_utils.py       # Performance monitoring
```

### Enhanced Python Package Installation with Phase 6 Bindings

```bash
# Install build dependencies for Phase 6 enhanced Python bindings
pip install pybind11 scikit-build-core pytest numpy matplotlib \
    numpy matplotlib pandas seaborn scipy

# Build and install enhanced Python bindings with all Phase 6 features
pip install .

# Or install in development mode with complete Phase 6 capabilities
pip install -e .[dev,perf,cog]

# Verify installation with Phase 6 capability check
python3 -c "import pynlm; print('Phase 6 NLM Python bindings installed successfully'); print('Available features:', hasattr(pynlm, 'createPhase6Config'), hasattr(pynlm, 'AdvancedConfig'), hasattr(pynlm, 'PerformanceMonitor'))"
```

### Enhanced Python Package Structure with Phase 6 Features

The enhanced Python bindings now include **complete Phase 6 integration**:

```
pynlm/
├── __init__.py                    # Enhanced Python bindings with Phase 6 API
├── core/                          # Core utilities
│   ├── __init__.py
│   └── brain.py                   # Enhanced brain API with cognitive integration
├── agent/                         # Agent system with cognitive features
│   ├── __init__.py
│   └── agent_brain.py             # Enhanced agent brain with Phase 6 cognition
├── memory/                        # **Phase 6 Enhanced Memory Systems**
│   ├── __init__.py
│   ├── working_memory.py          # Working memory (Phase 6 persistent activity)
│   ├── episodic_memory.py         # Episodic memory (Phase 6 experience encoding)
│   └── associative_memory.py      # Associative memory (Phase 6 pattern associations)
├── cognition/                     # **Phase 4+ Enhanced Cognitive Systems**
│   ├── __init__.py
│   ├── attention.py               # Attention system (Phase 4+ competitive selection)
│   ├── planning.py                # Planning system (Phase 4+ neural planning)
│   └── self_model.py              # Self-model (Phase 4+ sensorimotor awareness)
├── neuromodulation/               # **Phase 6 Enhanced Neuromodulation**
│   ├── __init__.py
│   ├── dopamine.py               # Dopamine system (Phase 6 reward prediction)
│   ├── novelty.py                # Novelty detection (Phase 6 change detection)
│   └── curiosity.py              # Curiosity system (Phase 6 exploration motivation)
├── prediction/                    # Prediction system (Phase 4+)
│   ├── __init__.py
│   ├── temporal_prediction.py     # Temporal sequence forecasting
│   └── action_consequence.py     # Action-effect prediction
├── development/                   # Development system (Phase 3+)
│   ├── __init__.py
│   └── developmental_stages.py   # Age-dependent plasticity and maturation
├── experiments/                   # **Phase 6 Integrated Experimental Framework**
│   ├── __init__.py
│   ├── phase3_experiments.py      # Phase 3 core functionality
│   ├── phase4_experiments.py      # Phase 4 cognitive systems
│   └── phase6_integrated.py       # **Complete Phase 6 integration experiments**
└── utils/                         # Utilities
    ├── __init__.py
    ├── config_utils.py            # Configuration utilities with Phase 6 settings
    └── performance_utils.py       # Performance monitoring and optimization
```

### Enhanced Python Installation Verification

```python
#!/usr/bin/env python3
"""
Enhanced verification script for NLM Phase 6 Python installation
"""
import sys
import pynlm

def verify_enhanced_phase6_installation():
    print("=== NLM Phase 6 Enhanced Python Installation Verification ===\n")
    
    # Test 1: Basic functionality
    print("1. Testing basic NLM Phase 6 functionality...")
    config = pynlm.createDefaultConfig()
    print(f"   ✓ Default configuration created: {config.get('brain.neuron_count', 'default')} neurons")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("   ✓ Brain created and initialized")
    
    # Test 2: Phase 6 core configuration
    print("\n2. Testing Phase 6 core features...")
    if hasattr(pynlm, 'createPhase6Config'):
        phase6_config = pynlm.createPhase6Config()
        print("   ✓ Phase 6 configuration available")
        print(f"     - Memory systems enabled: {phase6_config.get('memory.working.enabled', 'default')}")
        print(f"     - Cognitive systems enabled: {phase6_config.get('cognition.planning.enabled', 'default')}")
        print(f"     - Neuromodulation enabled: {phase6_config.get('neuromodulation.curiosity.enabled', 'default')}")
    else:
        print("   ✗ Phase 6 configuration not available")
    
    # Test 3: Enhanced memory systems
    print("\n3. Testing Phase 6 enhanced memory systems...")
    if hasattr(brain, 'getWorkingMemory') and brain.getWorkingMemory():
        print("   ✓ Working memory system available")
        working_memory = brain.getWorkingMemory()
        working_memory.store("test_pattern", 0.8)
        retrieved = working_memory.retrieve("test_pattern", 0.1)
        print(f"     - Memory storage/retrieval working: {retrieved is not None}")
        
        if hasattr(brain, 'getEpisodicMemory') and brain.getEpisodicMemory():
            print("   ✓ Episodic memory system available")
            episodic_memory = brain.getEpisodicMemory()
            episodic_memory.store({
                "step": brain.getCurrentStep(),
                "input": "sensory_pattern",
                "output": "action_taken",
                "reward": 0.8
            })
            retrieved_episodes = episodic_memory.retrieveSimilar("sensory_pattern", top_k=5)
            print(f"     - Episodic memory storage/retrieval: {len(retrieved_episodes)} similar episodes")
    
    # Test 4: Prediction and planning systems
    print("\n4. Testing Phase 4+ cognitive systems...")
    if hasattr(brain, 'getPredictionSystem') and brain.getPredictionSystem():
        print("   ✓ Prediction system available")
        pred_system = brain.getPredictionSystem()
        pred_system.trainSequence([1, 2, 3, 4, 5])
        prediction = pred_system.predictNext()
        print(f"     - Prediction system working: predicted {prediction}")
    
    if hasattr(brain, 'getPlanner') and brain.getPlanner():
        print("   ✓ Planning system available")
        planner = brain.getPlanner()
        plan = planner.planAction("explore", 100)
        print(f"     - Planning system working: {len(plan)} steps")
    
    # Test 5: Neuromodulation integration
    print("\n5. Testing enhanced neuromodulation features...")
    if hasattr(brain, 'getNeuromodulationSystem') and brain.getNeuromodulationSystem():
        print("   ✓ Neuromodulation system available")
        neuromod = brain.getNeuromodulationSystem()
        
        curiosity = neuromod.getCuriosity()
        novelty = neuromod.getNovelty()
        dopamine = neuromod.getDopamine()
        
        print(f"     - Curiosity system: {'enabled' if curiosity.isEnabled() else 'disabled'}")
        print(f"     - Novelty detection: {'enabled' if novelty.isEnabled() else 'disabled'}")
        print(f"     - Dopamine processing: {'enabled' if dopamine.isEnabled() else 'disabled'}")
    
    # Test 6: Performance monitoring
    print("\n6. Testing performance monitoring...")
    if hasattr(pynlm, 'PerformanceMonitor'):
        monitor = pynlm.PerformanceMonitor()
        monitor.start()
        
        # Run some operations
        for i in range(50):
            brain.step(i)
        
        stats = monitor.stop()
        print(f"   ✓ Performance monitoring available")
        print(f"     - Total time: {stats.total_time:.2f}s")
        print(f"     - Operations per second: {stats.operations_per_second:.2f}")
        print(f"     - Memory usage: {stats.memory_usage_mb:.2f} MB")
    
    # Test 7: Advanced configuration
    print("\n7. Testing advanced Phase 6 configuration...")
    if hasattr(pynlm, 'AdvancedConfig'):
        advanced_config = pynlm.AdvancedConfig()
        advanced_config.set("optimization.performance_mode", "maximum")
        advanced_config.set("neuromodulation.curiosity.strength", 0.9)
        advanced_config.set("memory.episodic.capacity", 200)
        advanced_config.set("cognition.planning.enabled", True)
        
        print("   ✓ Advanced configuration available")
        print(f"     - Performance mode: {advanced_config.get('optimization.performance_mode')}")
        print(f"     - Curiosity strength: {advanced_config.get('neuromodulation.curiosity.strength')}")
        print(f"     - Episodic memory capacity: {advanced_config.get('memory.episodic.capacity')}")
    
    # Test 8: Error handling
    print("\n8. Testing enhanced error handling...")
    error_types = [
        'NLMException',
        'ConfigError', 
        'BrainError',
        'WorldError',
        'MemoryError',
        'PredictionError'
    ]
    
    for error_type in error_types:
        if hasattr(pynlm, error_type):
            print(f"   ✓ {error_type} exception type available")
    
    print("\n=== All Phase 6 enhanced NLM Python features verified! ===")

if __name__ == "__main__":
    verify_enhanced_phase6_installation()
```

---

## Part 3: Enhanced Python API Usage

### 1. Phase 6 Enhanced Brain API

```python
import pynlm

# Create a Phase 6 enhanced brain configuration
phase6_config = pynlm.createPhase6Config()
phase6_config.set("simulation.steps", 1000)
phase6_config.set("memory.working.capacity", 100)
phase6_config.set("cognition.planning.enabled", True)
phase6_config.set("neuromodulation.curiosity.strength", 0.8)

# Create brain with Phase 6 integration
brain = pynlm.createBrain(phase6_config)
brain.initialize()

# Get integrated systems
working_memory = brain.getWorkingMemory()
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()
neuromodulation = brain.getNeuromodulationSystem()

print(f"✓ Phase 6 integrated brain created")
print(f"  - Working memory: {'enabled' if working_memory else 'disabled'}")
print(f"  - Prediction system: {'enabled' if prediction_system else 'disabled'}")
print(f"  - Planning system: {'enabled' if planner else 'disabled'}")
```

### 2. Advanced Memory Management

```python
import pynlm

# Create brain with enhanced memory systems
config = pynlm.createDefaultConfig()
config.set("memory.working.capacity", 200)
config.set("memory.episodic.capacity", 50)
config.set("memory.associative.strength", 0.9)

brain = pynlm.createBrain(config)
brain.initialize()

working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Working memory operations
working_memory.store("current_task", "task_objective", "context")
retrieved_task = working_memory.retrieve("current_task", threshold=0.7)
print(f"Retrieved task: {retrieved_task}")

# Episodic memory operations
episodic_memory.store({
    "time": brain.getCurrentStep(),
    "input": "sensory_pattern",
    "output": "action_taken",
    "reward": 0.8
})

retrieved_episodes = episodic_memory.retrieveSimilar("sensory_pattern", top_k=5)
print(f"Retrieved {len(retrieved_episodes)} similar episodes")

# Associative memory operations
associative_memory.associate("stimulus_A", "response_B")
predicted_response = associative_memory.predict("stimulus_A")
print(f"Predicted response: {predicted_response}")
```

### 3. Advanced Cognition Systems

```python
import pynlm

# Create brain with enhanced cognition
config = pynlm.createPhase4Config()
config.set("cognition.attention.enabled", True)
config.set("cognition.planning.enabled", True)
config.set("cognition.self_model.enabled", True)
config.set("cognition.social_learning.enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Attention system
attention = brain.getAttentionSystem()
sensory_input = brain.getSensoryInput()
attended_signal = attention.selectAttention(sensory_input)
print(f"Attention selected signal: {attended_signal}")

# Planning system
planner = brain.getPlanner()
possible_actions = ["move", "grasp", "explore", "communicate"]
selected_plan = planner.planActionSequence(possible_actions, 10)
print(f"Generated plan: {selected_plan}")

# Self-model
self_model = brain.getSelfModel()
agent_state = self_model.getInternalState()
model_consistency = self_model.checkConsistency()
print(f"Self-model consistency: {model_consistency}")

# Social learning
social_learner = brain.getSocialLearner()
trainer_observation = "agent_moved_to_location"
social_learner.processObservation(trainer_observation)

inferred_rule = social_learner.inferRules()
print(f"Inferred social rule: {inferred_rule}")
```

### 4. Neuromodulation and Learning

```python
import pynlm
import time

# Create brain with enhanced neuromodulation
config = pynlm.createDefaultConfig()
config.set("neuromodulation.dopamine.enabled", True)
config.set("neuromodulation.curiosity.enabled", True)
config.set("neuromodulation.novelty.enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Set up environment for neuromodulated learning
world = pynlm.createSimpleWorld(width=20, height=20)
world.configure(visionWidth=5, visionHeight=5)
world.reset()

# Create agent with neuromodulated brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run neuromodulated learning episode
for step in range(500):
    # Sensory processing
    percept = world.getSensoryPercept()
    processed_input = agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Action selection (neuromodulated)
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for termination
    if world.isEpisodeComplete():
        world.reset()
        agent.initialize(world)
```

### 5. Performance Optimization

```python
import pynlm
import time

# Create performance-optimized configuration
config = pynlm.createDefaultConfig()
config.set("optimization.performance_mode", "maximum")
config.set("simulation.batch_size", 100)
config.set("memory.cache_enabled", True)
config.set("neuromodulation.lazy_evaluation", True)

# Performance monitoring
performance_monitor = pynlm.PerformanceMonitor()
performance_monitor.start()

# Create optimized brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run batch simulation
batch_size = 1000
start_time = time.time()

for batch in range(batch_size // 100):
    batch_start = time.time()
    
    # Process batch of steps
    for step in range(100):
        brain.step(batch * 100 + step)
    
    batch_time = time.time() - batch_start
    operations_per_second = 100 / batch_time if batch_time > 0 else 0
    
    print(f"Batch {batch}: {operations_per_second:.2f} ops/sec")

# Get performance statistics
performance_stats = performance_monitor.stop()

print(f"\nPerformance Summary:")
print(f"  Total time: {performance_stats.total_time:.2f}s")
print(f"  Operations per second: {performance_stats.operations_per_second:.2f}")
print(f"  Memory usage: {performance_stats.memory_usage_mb:.2f} MB")
print(f"  CPU utilization: {performance_stats.cpu_utilization:.2f}%")
```

### 6. Error Handling and Debugging

```python
import pynlm

# Configure enhanced error handling
import logging
logging.basicConfig(level=logging.DEBUG)

try:
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("validation.enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test invalid operation
    invalid_action = "invalid_action_that_does_not_exist"
    action = agent.decodeMotorCommand(invalid_action)
    
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
    print(f"Error code: {e.error_code}")
    print(f"Suggestions: {e.suggestions}")
    
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
    
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
    
except pynlm.WorldError as e:
    print(f"World error: {e}")
    
except pynlm.NLMException as e:
    print(f"General NLM error: {e}")
    
finally:
    # Cleanup
    if 'brain' in locals():
        brain.shutdown()
    if 'agent' in locals():
        agent.shutdown()
    if 'world' in locals():
        world.reset()
```

---

## Part 4: Advanced Features and Phase 6 Integration

### 1. Phase 6 Integration Demonstration

```bash
# Run Phase 6 integration demonstration
./nlm_phase6_demo --integration-test --verbose --save-results results_phase6.json

# Command-line options:
# --integration-test      Run comprehensive integration tests
# --verbose              Enable verbose output
# --save-results FILE    Save results to JSON file
# --performance-benchmark Run performance benchmark
# --memory-analysis      Analyze memory usage
```

### 2. Advanced CLI Options

```bash
# Enhanced CLI options for all NLM executables
# Example: ./nlm --help

# Common enhanced options:
# --help                          Show help message
# --version                       Show version information
# --verbose                        Enable verbose logging
# --log-level LEVEL               Set logging level (debug/info/warn/error)
# --config FILE                    Load configuration from file
# --validate-config                Validate configuration and exit
# --benchmark                      Run performance benchmark
# --save-results FILE              Save results to file
# --load-results FILE              Load previous results
# --parallel                      Enable parallel processing
# --mpi-rank RANK                  Set MPI rank
# --gpu-acceleration               Enable GPU acceleration
# --debug                          Enable debug mode
# --simulation-mode MODE           Set simulation mode (real-time, fast, debug)
# --checkpoint FREQUENCY           Set checkpoint frequency
# --environment TYPE               Set environment type (simple, complex, custom)
```

### 3. Batch Processing and Scripting

```bash
#!/bin/bash
# Enhanced batch processing script

# Configuration
BATCH_SIZE=1000
EPISODES=50
OUTPUT_FILE="batch_results.json"
CONFIG_FILE="config/batch_config.json"

# Create batch configuration
cat > $CONFIG_FILE << EOF
{
    "simulation": {
        "batch_size": $BATCH_SIZE,
        "episodes": $EPISODES,
        "step_size": 1
    },
    "output": {
        "file": "$OUTPUT_FILE",
        "format": "json"
    },
    "logging": {
        "level": "info",
        "file": "batch.log"
    },
    "performance": {
        "parallel": true,
        "optimization_level": 3
    }
}
