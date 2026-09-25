# NLM Python API Reference

## Overview

The NLM Python API provides **complete access to the Phase 6 integrated artificial brain** through the `pynlm` module. This API enables users to create, configure, and run simulations of brain-inspired cognitive systems with full access to all memory, cognition, and neuromodulation systems.

> **Key Feature:** The Python API provides **unified access** to all Phase 6 cognitive systems including memory, prediction, planning, attention, and neuromodulation.

## Module Structure

```python
pynlm/
├── __init__.py                    # Core API and configuration classes
├── core/                          # Core brain utilities
│   ├── __init__.py
│   └── brain.py                   # Brain management and control
├── agent/                         # Agent system integration
│   ├── __init__.py
│   └── agent_brain.py             # Agent-brain integration
├── memory/                        # Memory systems
│   ├── __init__.py
│   ├── working_memory.py          # Working memory operations
│   ├── episodic_memory.py         # Episodic memory operations
│   └── associative_memory.py      # Associative memory operations
├── cognition/                     # Cognitive systems
│   ├── __init__.py
│   ├── attention.py               # Attention system
│   ├── planning.py                # Planning system
│   └── self_model.py              # Self-model system
├── neuromodulation/               # Neuromodulation systems
│   ├── __init__.py
│   ├── dopamine.py               # Dopamine system
│   ├── novelty.py                # Novelty detection
│   └── curiosity.py              # Curiosity system
├── prediction/                    # Prediction systems
│   ├── __init__.py
│   ├── temporal_prediction.py     # Temporal prediction
│   └── action_consequence.py     # Action consequence prediction
├── development/                   # Development system
│   ├── __init__.py
│   └── developmental_stages.py   # Developmental stages
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

## Core Classes and Functions

### Module Initialization

```python
import pynlm
from pynlm import *

# Module information
print(pynlm.__version__)                    # Version information
print(pynlm.__doc__)                        # Module documentation

# Check available features
print(hasattr(pynlm, 'createPhase6Config'))   # True
print(hasattr(pynlm, 'AdvancedConfig'))       # True
print(hasattr(pynlm, 'PerformanceMonitor'))   # True
```

### Configuration Classes

#### Base Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Configure basic parameters
config.set("simulation.steps", 1000)
config.set("simulation.time_step", 0.1)
config.set("brain.neuron_count", 1000)
config.set("world.width", 20)
config.set("world.height", 20)

# Get configuration values
print(config.get("brain.neuron_count"))
print(config.get("world.width", 10))  # Default if not set

# Check configuration status
print(config.validate())  # Returns True if valid
```

#### Phase 6 Configuration

```python
import pynlm

# Create comprehensive Phase 6 configuration
phase6_config = pynlm.createPhase6Config()

# Configure all cognitive systems
phase6_config.set("memory.working.enabled", True)
phase6_config.set("memory.working.capacity", 100)
phase6_config.set("memory.episodic.enabled", True)
phase6_config.set("memory.episodic.capacity", 50)
phase6_config.set("memory.associative.enabled", True)
phase6_config.set("memory.associative.strength", 0.9)

phase6_config.set("cognition.attention.enabled", True)
phase6_config.set("cognition.planning.enabled", True)
phase6_config.set("cognition.self_model.enabled", True)
phase6_config.set("cognition.social_learning.enabled", True)

phase6_config.set("neuromodulation.dopamine.enabled", True)
phase6_config.set("neuromodulation.curiosity.enabled", True)
phase6_config.set("neuromodulation.novelty.enabled", True)

phase6_config.set("prediction.temporal.enabled", True)
phase6_config.set("prediction.action_consequence.enabled", True)

# Development settings
phase6_config.set("development.stages", True)
phase6_config.set("development.plasticity", "age_dependent")

print("Phase 6 configuration created successfully")
```

#### Advanced Configuration

```python
import pynlm

# Create advanced configuration with optimization
advanced_config = pynlm.AdvancedConfig()

# Performance optimization
advanced_config.set("optimization.performance_mode", "maximum")
advanced_config.set("optimization.parallel_processing", True)
advanced_config.set("optimization.gpu_acceleration", True)

# Memory optimization
advanced_config.set("memory.cache_enabled", True)
advanced_config.set("memory.compression_enabled", True)
advanced_config.set("memory.evict_policy", "least_recently_used")

# Neuromodulation settings
advanced_config.set("neuromodulation.curiosity.strength", 0.8)
advanced_config.set("neuromodulation.novelty.threshold", 0.5)
advanced_config.set("neuromodulation.dopamine.gating_strength", 0.7)

# Cognitive settings
advanced_config.set("cognition.planning.horizon", 10)
advanced_config.set("cognition.attention.focus_strength", 0.6)

# Simulation settings
advanced_config.set("simulation.max_steps", 10000)
advanced_config.set("simulation.checkpoint_interval", 100)
advanced_config.set("simulation.logging_level", "info")

print("Advanced configuration created successfully")
```

### Brain Management Functions

#### Brain Creation and Initialization

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create brain with Phase 6 configuration
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Create advanced brain with enhanced features
brain = pynlm.createAdvancedBrain(pynlm.createPhase6Config())
brain.initialize()

# Check brain status
print(f"Brain initialized: {brain.isInitialized()}")
print(f"Neuron count: {brain.getNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Brain Control

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Run simulation for specified steps
for step in range(1000):
    brain.step(step)

# Get current simulation step
print(f"Current step: {brain.getCurrentStep()}")

# Get brain statistics
stats = brain.getStatistics()
print(f"Simulation time: {stats.simulation_time:.2f}")
print(f"Average firing rate: {stats.average_firing_rate:.2f} Hz")

# Save brain state
brain.save("brain_state.json")

# Load brain state
brain.load("brain_state.json")

# Shutdown brain
brain.shutdown()
```

#### Brain Systems Access

```python
import pynlm

brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Working memory system
working_memory = brain.getWorkingMemory()
print(f"Working memory: {'enabled' if working_memory else 'disabled'}")
if working_memory:
    working_memory.store("task", "solve_puzzle", "context")
    retrieved = working_memory.retrieve("task", threshold=0.7)

# Episodic memory system
episodic_memory = brain.getEpisodicMemory()
print(f"Episodic memory: {'enabled' if episodic_memory else 'disabled'}")
if episodic_memory:
    episodic_memory.store({
        "step": brain.getCurrentStep(),
        "input": "visual_pattern",
        "output": "action_taken",
        "reward": 0.8,
        "prediction_error": 0.1
    })

# Associative memory system
associative_memory = brain.getAssociativeMemory()
print(f"Associative memory: {'enabled' if associative_memory else 'disabled'}")
if associative_memory:
    associative_memory.associate("stimulus_A", "response_B")
    predicted = associative_memory.predict("stimulus_A")

# Prediction system
prediction_system = brain.getPredictionSystem()
print(f"Prediction system: {'enabled' if prediction_system else 'disabled'}")
if prediction_system:
    prediction_system.trainSequence([1, 2, 3, 4, 5])
    prediction = prediction_system.predictNext()

# Planning system
planner = brain.getPlanner()
print(f"Planning system: {'enabled' if planner else 'disabled'}")
if planner:
    plan = planner.planAction("explore", 10)
    print(f"Generated plan: {plan}")

# Attention system
attention = brain.getAttentionSystem()
print(f"Attention system: {'enabled' if attention else 'disabled'}")
if attention:
    attention.setPriority("task", 0.8)

# Self-model
self_model = brain.getSelfModel()
print(f"Self-model: {'enabled' if self_model else 'disabled'}")
if self_model:
    internal_state = self_model.getInternalState()
    consistency = self_model.checkConsistency()

# Neuromodulation system
neuromodulation = brain.getNeuromodulationSystem()
print(f"Neuromodulation: {'enabled' if neuromodulation else 'disabled'}")
if neuromodulation:
    curiosity = neuromodulation.getCuriosity()
    novelty = neuromodulation.getNovelty()
    dopamine = neuromodulation.getDopamine()

# Development system
if hasattr(brain, 'getDevelopmentSystem'):
    development = brain.getDevelopmentSystem()
    print(f"Development: {'enabled' if development else 'disabled'}")
```

### Agent System Functions

#### Agent Creation and Control

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

world = pynlm.createSimpleWorld(width=20, height=20)
world.configure(visionWidth=5, visionHeight=5)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run episode
for step in range(500):
    # Process sensory input
    percept = world.getSensoryPercept()
    processed_input = agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Action selection
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check for termination
    if world.isEpisodeComplete():
        world.reset()
        agent.initialize(world)

print("Episode completed")

# Shutdown agent
agent.shutdown()
```

### Performance Monitoring

#### Performance Monitor

```python
import pynlm

brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Create performance monitor
monitor = pynlm.PerformanceMonitor()
monitor.start()

# Run simulation
for step in range(100):
    brain.step(step)
    monitor.recordStep(step)

# Get performance statistics
stats = monitor.stop()

print(f"Performance Statistics:")
print(f"  Total time: {stats.total_time:.2f}s")
print(f"  Operations per second: {stats.operations_per_second:.2f}")
print(f"  Memory usage: {stats.memory_usage_mb:.2f} MB")
print(f"  CPU utilization: {stats.cpu_utilization:.2f}%")
print(f"  Spike rate: {stats.spike_rate:.2f} Hz")
print(f"  Memory efficiency: {stats.memory_efficiency:.2f}")
print(f"  Stability score: {stats.stability:.2f}")
```

### Error Handling

#### Exception Classes

```python
import pynlm

brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Custom error handler
def handle_errors():
    try:
        # Code that might raise errors
        brain = pynlm.createBrain(pynlm.createInvalidConfig())
        brain.initialize()
        
    except pynlm.NLMException as e:
        print(f"General NLM error: {e}")
        if hasattr(e, 'error_code'):
            print(f"Error code: {e.error_code}")
        if hasattr(e, 'suggestions'):
            print(f"Suggestions: {e.suggestions}")
            
    except pynlm.ConfigError as e:
        print(f"Configuration error: {e}")
        
    except pynlm.BrainError as e:
        print(f"Brain error: {e}")
        
    except pynlm.MemoryError as e:
        print(f"Memory error: {e}")
        
    except pynlm.WorldError as e:
        print(f"World error: {e}")
        
    except pynlm.PredictionError as e:
        print(f"Prediction error: {e}")
        
    except pynlm.NeuromodulationError as e:
        print(f"Neuromodulation error: {e}")

# Usage
handle_errors()
```

### Utility Functions

#### Help and Documentation

```python
import pynlm

# Get help information
help_info = pynlm.getHelp()
print(help_info)

# Get example code
examples = pynlm.getExamples()
for name, code in examples.items():
    print(f"\n=== {name} ===")
    print(code)
```

#### Debugging Utilities

```python
import pynlm

# Debug brain state
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Get debug information
debug_info = brain.getDebugInfo()
print(f"Brain debug info:")
print(f"  Step: {debug_info.step}")
print(f"  Neurons: {debug_info.neuron_count}")
print(f"  Memory usage: {debug_info.memory_usage}")
print(f"  Performance: {debug_info.performance}")

# Enable debug logging
import logging
logging.basicConfig(level=logging.DEBUG)
```

## Python API Reference Summary

### Core Modules
- **pynlm**: Main module with all API classes and functions
- **pynlm.core**: Core brain management utilities
- **pynlm.agent**: Agent system integration
- **pynlm.memory**: Memory systems (working, episodic, associative)
- **pynlm.cognition**: Cognitive systems (attention, planning, self-model)
- **pynlm.neuromodulation**: Neuromodulation systems (dopamine, curiosity, novelty)
- **pynlm.prediction**: Prediction systems (temporal, action-consequence)
- **pynlm.development**: Development system
- **pynlm.experiments**: Experimental framework

### Key Classes
- **Configuration**: Base configuration class
- **Phase6Config**: Phase 6 specific configuration
- **AdvancedConfig**: Advanced configuration with optimization settings
- **Brain**: Main brain simulation class
- **AgentBrain**: Agent integration class
- **WorkingMemory**: Working memory system
- **EpisodicMemory**: Episodic memory system
- **AssociativeMemory**: Associative memory system
- **Attention**: Attention system
- **Planner**: Planning system
- **SelfModel**: Self-model system
- **PerformanceMonitor**: Performance monitoring

### Key Functions
- **createDefaultConfig()**: Create default configuration
- **createPhase6Config()**: Create Phase 6 configuration
- **createBrain()**: Create brain instance
- **createAgentBrain()**: Create agent with brain integration
- **createSimpleWorld()**: Create simple world environment
- **createAdvancedConfig()**: Create advanced configuration
- **getHelp()**: Get help information
- **getExamples()**: Get example code

## API Usage Examples

### Basic Usage
```python
#!/usr/bin/env python3
"""
Basic NLM Python API usage example
"""
import pynlm

# Create Phase 6 brain
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")

# Clean up
brain.shutdown()
```

### Advanced Usage
```python
#!/usr/bin/env python3
"""
Advanced NLM Python API usage example with all cognitive systems
"""
import pynlm

# Create Phase 6 configuration
config = pynlm.createPhase6Config()
config.set("memory.working.capacity", 200)
config.set("memory.episodic.capacity", 50)
config.set("cognition.planning.enabled", True)
config.set("neuromodulation.curiosity.strength", 0.8)

# Create brain with all systems
brain = pynlm.createBrain(config)
brain.initialize()

# Access all cognitive systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()
attention = brain.getAttentionSystem()
self_model = brain.getSelfModel()
neuromodulation = brain.getNeuromodulationSystem()

# Use working memory
working_memory.store("task", "solve_puzzle", "context")
retrieved = working_memory.retrieve("task", threshold=0.7)

# Use episodic memory
episodic_memory.store({
    "step": brain.getCurrentStep(),
    "input": "visual_pattern",
    "output": "action_taken",
    "reward": 0.8
})

# Use associative memory
associative_memory.associate("stimulus_A", "response_B")
predicted = associative_memory.predict("stimulus_A")

# Use prediction system
prediction_system.trainSequence([1, 2, 3, 4, 5])
prediction = prediction_system.predictNext()

# Use planning system
plan = planner.planAction("explore", 10)

# Use attention system
attention.setPriority("task", 0.8)

# Use self-model
internal_state = self_model.getInternalState()
consistency = self_model.checkConsistency()

# Use neuromodulation
if neuromodulation:
    curiosity = neuromodulation.getCuriosity()
    novelty = neuromodulation.getNovelty()
    dopamine = neuromodulation.getDopamine()

print(f"Advanced simulation completed successfully")
print(f"  - Working memory entries: {working_memory.getEntryCount()}")
print(f"  - Episodic memory entries: {episodic_memory.getEntryCount()}")
print(f"  - Predictions made: {len(prediction_system.getTrainingHistory())}")
print(f"  - Plans generated: {planner.getPlanCount()}")
print(f"  - Curiosity active: {curiosity.isEnabled()}")

# Clean up
brain.shutdown()
```

### Performance Optimized Usage
```python
#!/usr/bin/env python3
"""
Performance optimized NLM Python API usage
"""
import pynlm
import time

# Create performance-optimized configuration
config = pynlm.createPhase6Config()
config.set("optimization.performance_mode", "maximum")
config.set("simulation.batch_size", 100)
config.set("memory.cache_enabled", True)
config.set("neuromodulation.lazy_evaluation", True)

# Create performance monitor
monitor = pynlm.PerformanceMonitor()
monitor.start()

# Create optimized brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run batch simulation
batch_size = 1000
start_time = time.time()

for batch in range(batch_size // 100):
    # Process batch of steps
    for step in range(100):
        brain.step(batch * 100 + step)

# Get performance statistics
performance_stats = monitor.stop()

print(f"Performance Summary:")
print(f"  Total time: {performance_stats.total_time:.2f}s")
print(f"  Operations per second: {performance_stats.operations_per_second:.2f}")
print(f"  Memory usage: {performance_stats.memory_usage_mb:.2f} MB")
print(f"  CPU utilization: {performance_stats.cpu_utilization:.2f}%")

# Clean up
brain.shutdown()
```

## API Compatibility

### Python Version Requirements
- **Python 3.8+**: Required for all Phase 6 features
- **NumPy 1.20+**: Required for array operations
- **Matplotlib 3.3+**: Optional for visualization
- **SciPy 1.6+**: Optional for advanced computations

### Module Dependencies
```bash
pip install pynlm[all]
# or
pip install pynlm[dev]
pip install pynlm[perf]
pip install pynlm[cog]
```

## API Versioning

### Version Information
```python
import pynlm

print(f"NLM Python API Version: {pynlm.__version__}")
print(f"Phase 6 Integration: {'enabled' if hasattr(pynlm, 'createPhase6Config') else 'disabled'}")
print(f"Performance Monitoring: {'enabled' if hasattr(pynlm, 'PerformanceMonitor') else 'disabled'}")
```

### Backward Compatibility
The NLM Python API maintains **backward compatibility** with previous versions while providing **enhanced functionality** for Phase 6 integration:

- **Core API unchanged**: All existing functions and classes remain functional
- **New features added**: Phase 6 configuration and enhanced systems
- **Performance improvements**: Optimized for Phase 6 capabilities
- **Documentation updated**: Comprehensive documentation for all features

## API Testing

### Running Tests
```bash
# Run Python API tests
python -m pytest tests/python_api/ -v

# Run Phase 6 integration tests
python -m pytest tests/phase6_integration.py -v

# Run performance tests
python -m pytest tests/performance_tests.py -v
```

### Test Coverage
The NLM Python API includes comprehensive test coverage:
- **Unit tests**: Individual component testing
- **Integration tests**: System-wide functionality verification
- **Performance tests**: Efficiency and scalability assessment
- **Stress tests**: Robustness and reliability testing
- **Compatibility tests**: Cross-version and cross-platform validation

## API Documentation Generation

### Generating Documentation
```bash
# Generate API documentation
pydoc -o docs/api/python pynlm

# Generate comprehensive documentation
sphinx-build -b html docs/api/python docs/api/html
```

### API Documentation Sources
- **Source code**: Inline docstrings and comments
- **Type hints**: Python type annotations
- **Examples**: Working code examples
- **Tutorials**: Step-by-step guides
- **Reference**: Complete API documentation