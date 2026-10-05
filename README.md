# NLM System - Getting Started Guide

This document provides an overview of the NLM (Neural Learning Machine) system and its improvements.

## Overview

NLM is an experimental artificial developmental brain project that implements:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Multiple neuromodulators (dopamine, curiosity, reward, novelty)
- Memory systems (working, episodic)
- Development and maturation systems
- Sensorimotor control and learning

## Key Improvements

The NLM system has been significantly enhanced with the following improvements:

### 1. Advanced Agent Controller

**File**: `advanced_agent_controller.py`

Provides enhanced functionality for:
- Advanced experiment configuration
- Enhanced brain creation with custom parameters
- Advanced world simulation with object management
- Agent brain interface with mode selection
- Simulation monitoring and performance tracking
- Data export and visualization
- Callback system for event-driven programming

**Key Features**:
- Multiple configuration options for brain, world, and agent
- Real-time performance metrics and monitoring
- Support for different agent modes (exploration, exploitation, etc.)
- Comprehensive error handling and recovery
- Data export in multiple formats (JSON, CSV)

### 2. Interactive Agent System

**File**: `interactive_agent_system.py`

Provides a comprehensive interactive interface for:
- Command-line interface (CLI) for system control
- Multiple agent types (Explorer, Learner, Adaptive, Social, Creative, Basic)
- Advanced environment profiles and configuration
- Experiment management and tracking
- Performance monitoring and analysis
- Callback system and event handling
- Data export and reporting

**Key Features**:
- Full interactive CLI with help system
- Six distinct agent types with unique behaviors
- Comprehensive experiment setup and management
- Real-time system monitoring
- Event-driven programming with callbacks
- Multiple data export formats

### 3. Demonstration System

**File**: `demo_improvements.py`

Provides comprehensive demonstrations of:
- Advanced agent controller functionality
- Interactive agent system capabilities
- Performance monitoring features
- Multiple agent types and behaviors
- Experiment management and analysis

**Purpose**: Educational and testing purposes to showcase all system improvements.

### 4. Documentation and Examples

**Files**:
- `IMPROVEMENTS_SUMMARY.md` - Comprehensive documentation of all improvements
- `easy_usage.md` - Enhanced beginner documentation
- `HOW_TO_USE.md` - Comprehensive usage guide

## System Architecture

### Core Components

1. **Brain System** (`src/brain/`)
   - Neuron simulation (LIF model)
   - Synapse management and plasticity
   - Neural populations and regions
   - Memory systems integration

2. **Agent System** (`src/agent/`)
   - Agent brain interface
   - Sensory processing and transduction
   - Motor decoding and action selection
   - Neuromodulation and reward processing

3. **World System** (`src/world/`)
   - Environment simulation
   - Agent interaction
   - Sensory input generation
   - Action execution and feedback

4. **Plasticity Systems** (`src/plasticity/`)
   - Spike-Timing Dependent Plasticity (STDP)
   - Hebbian learning
   - Structural plasticity
   - Development and maturation

5. **Neuromodulation Systems** (`src/neuromodulation/`)
   - Dopamine system
   - Curiosity and novelty detection
   - Reward processing
   - Prediction error signals

### New Components (Improvements)

1. **Advanced Configuration** (`src/core/Config/`)
   - Enhanced parameter management
   - File-based configuration
   - Command-line argument parsing
   - Configuration validation

2. **Performance Monitoring** (`PerformanceMonitor` class)
   - Real-time system metrics
   - Resource usage tracking
   - Performance analysis
   - Benchmarking tools

3. **Experiment Management**
   - Complete experiment lifecycle
   - Result tracking and analysis
   - Multiple experiment execution
   - Data persistence and export

4. **Agent Types and Behaviors**
   - Explorer: High curiosity, random exploration
   - Learner: Focused learning, reward-based
   - Adaptive: Dynamic adaptation to environment
   - Social: Cooperative and social behaviors
   - Creative: Innovative and exploratory behavior
   - Basic: Simple, straightforward behavior

## Installation and Setup

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11 2.11.0+
- scikit-build-core 0.5.0+
- pytest
- numpy

### Building the System

1. **Build the C++ core**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Install Python bindings**
   ```bash
   cd /path/to/nlm
   pip install pybind11 scikit-build-core pytest numpy
   pip install -e .
   ```

### Running Examples

#### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete. Neurons: {brain.getTotalNeuronCount()}")
```

#### Advanced Agent Controller Example

```python
from advanced_agent_controller import AdvancedAgentController

# Create controller
controller = AdvancedAgentController()

# Define advanced experiment
experiment_config = {
    "brain_config": {
        "neuron_count": 1000,
        "synapse_density": 0.15,
        "plasticity": {"stdp": {"enable": True}}
    },
    "world_config": {
        "width": 20,
        "height": 20,
        "visionWidth": 8,
        "visionHeight": 8
    },
    "agent_config": {
        "features": {
            "reward_modulation": True,
            "curiosity": True,
            "development": True
        }
    }
}

# Run experiment
results = controller.run_experiment(experiment_config)
```

#### Interactive Agent System Example

```python
from interactive_agent_system import InteractiveAgentSystem

# Create interactive system
system = InteractiveAgentSystem()

# Create experiment setup
experiment = system.create_experiment_setup(
    name="demo_experiment",
    agent_type="explorer",
    duration=100.0,
    environment_size=15
)

# Run simulation
results = system.run_advanced_simulation(experiment)
```

#### Using Different Agent Types

```python
from interactive_agent_system import AgentProfile, AgentType

# Create different agent profiles
agents = {
    "explorer": AgentProfile(
        name="explorer_agent",
        type=AgentType.EXPLORER,
        curiosity_threshold=0.7,
        exploration_rate=0.5
    ),
    "learner": AgentProfile(
        name="learner_agent",
        type=AgentType.LEARNER,
        curiosity_threshold=0.3,
        exploration_rate=0.2
    ),
    "adaptive": AgentProfile(
        name="adaptive_agent",
        type=AgentType.ADAPTIVE,
        curiosity_threshold=0.5,
        exploration_rate=0.4
    )
}

# Use agents in experiments
for agent_name, agent_profile in agents.items():
    experiment = system.create_experiment_setup(
        name=f"{agent_name}_experiment",
        agent_profile=agent_profile,
        duration=200.0
    )
    results = system.run_advanced_simulation(experiment)
```

## Features and Capabilities

### 1. Advanced Configuration

- **Brain Configuration**: Neuron count, synapse density, plasticity parameters
- **World Configuration**: Environment dimensions, vision range, object properties
- **Agent Configuration**: Agent type, behavior parameters, feature toggles

### 2. Agent Types and Behaviors

#### Explorer Agent
- High curiosity and exploration rates
- Random action selection
- Novelty-seeking behavior
- Adaptive learning from new experiences

#### Learner Agent
- Focused on reward-based learning
- Stable and consistent behavior
- Efficient skill acquisition
- Reduced exploration in favor of exploitation

#### Adaptive Agent
- Dynamic adaptation to environmental changes
- Developmental plasticity
- Structural plasticity for connection growth
- Balance between exploration and exploitation

#### Social Agent
- Cooperative and social behaviors
- Group coordination and interaction
- Social learning and imitation
- Community-based decision making

#### Creative Agent
- Innovative and exploratory behavior
- High curiosity and novelty seeking
- Structural adaptation and innovation
- Boundary-pushing and novel solution finding

#### Basic Agent
- Simple, straightforward behavior
- Essential functionality only
- Baseline for comparison
- Minimal configuration requirements

### 3. Performance Monitoring

- **Real-time Metrics**: CPU, memory, thread usage
- **Performance Analysis**: Benchmarks and comparisons
- **Resource Monitoring**: System resource utilization
- **Progress Tracking**: Step-by-step progress monitoring

### 4. Experiment Management

- **Experiment Setup**: Complete experiment configuration
- **Execution Control**: Start, pause, resume, stop
- **Result Tracking**: Comprehensive result collection
- **Data Export**: Multiple export formats
- **Analysis Tools**: Performance analysis and reporting

### 5. Data Export and Visualization

- **JSON Export**: Structured data export
- **CSV Export**: Tabular data export
- **Performance Reports**: Detailed performance analysis
- **Result Summaries**: Concise experiment summaries

### 6. Error Handling and Recovery

- **Comprehensive Error Detection**: All failure modes
- **Graceful Degradation**: Fallback mechanisms
- **Detailed Logging**: Comprehensive error logging
- **Recovery Mechanisms**: Automatic recovery where possible

## Usage Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simple simulation
for i in range(50):
    brain.step(i)

print(f"Simulation complete!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 2: Complete Agent Simulation

```python
import pynlm
from interactive_agent_system import InteractiveAgentSystem

# Setup interactive system
system = InteractiveAgentSystem()

# Create advanced experiment
experiment = system.create_experiment_setup(
    name="agent_learning_demo",
    agent_type="explorer",
    duration=300.0,
    environment_size=20,
    timestep=0.1
)

# Run experiment
print(f"Starting experiment: {experiment.name}")
print(f"Agent type: {experiment.agent_profile.type.value}")
print(f"Environment: {experiment.environment_profile.width}x{experiment.environment_profile.height}")

results = system.run_advanced_simulation(experiment)

print(f"\nExperiment completed!")
print(f"Steps: {results['steps_completed']}")
print(f"Duration: {results['duration_seconds']:.2f}s")
print(f"Average firing rate: {results['brain_statistics']['average_firing_rate']:.2f}")
```

### Example 3: Performance Monitoring

```python
from interactive_agent_system import PerformanceMonitor

# Create and start monitoring
monitor = PerformanceMonitor(sample_rate=0.1)
monitor.start_monitoring()

# Perform some work
for i in range(10):
    time.sleep(0.1)
    metrics = monitor.get_current_metrics()
    print(f"Step {i}: Memory={metrics['memory_usage']['rss_mb']:.1f} MB, "
          f"CPU={metrics['cpu_usage']:.1f}%")

# Stop monitoring
monitor.stop_monitoring()

# Get statistics
stats = monitor.get_statistics()
print(f"\nPerformance Statistics:")
print(f"  Duration: {stats['duration']:.2f}s")
print(f"  Memory Avg: {stats['memory_avg_mb']:.2f} MB")
print(f"  CPU Avg: {stats['cpu_avg_percent']:.1f}%")
```

## Getting Help

### Documentation

- **README.md**: Overview and basic usage
- **easy_usage.md**: Beginner-friendly guide
- **HOW_TO_USE.md**: Comprehensive usage documentation
- **IMPROVEMENTS_SUMMARY.md**: Detailed improvements documentation

### Examples

- `demo_improvements.py`: Comprehensive demonstration script
- `advanced_agent_controller.py`: Advanced agent controller examples
- `interactive_agent_system.py`: Interactive system examples

### Support

For issues or questions:
1. Check the documentation
2. Run the demonstration scripts
3. Review example code
4. Check log files for errors

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgements

This project builds on extensive research in:
- Computational neuroscience
- Neural network simulations
- Artificial intelligence
- Machine learning
- Neuromodulation systems

Special thanks to all contributors and researchers in the field.
