# NLM Documentation Guide

## Overview

NLM (Neural Learning Machine) is an experimental computational brain project that implements a complete neural simulation framework with integrated memory systems, prediction, cognition, neuromodulation, and development.

## Architecture Phases

### Phase 2: Real Neural Computation (main.cpp)
**Demonstrates core neural dynamics:**
- **Leaky Integrate-and-Fire (LIF) neuron dynamics**
- **Event-driven spike propagation with synaptic delays**
- **STDP and Hebbian plasticity rules**
- **Structural plasticity (synaptogenesis/pruning)**

This is the primary demonstration showing real neural computation.

### Phase 6: Final Integration (Brain.cpp)
**Complete integrated artificial brain:**
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Prediction System**: Predictive processing and error tracking
- **Cognition Systems**: Neural planner, concept formation, attention
- **Development**: Developmental stages affecting plasticity
- **Checkpoint System**: Save/load functionality
- **Replay and Consolidation**: Memory replay and strengthening

## System Components

### 1. Neural Core
- **Neuron class**: LIF dynamics implementation
- **Synapse class**: Synaptic transmission
- **Spike system**: Event-driven spike processing
- **Plasticity rules**: STDP, Hebbian, structural plasticity

### 2. Memory Systems
- **Working Memory**: Persistent activity and competition
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations

### 3. Neuromodulation
- **Dopamine**: Reward prediction error, neural excitability
- **Curiosity**: Exploration behavior
- **Novelty**: Surprise detection
- **Prediction Error**: Learning signal

### 4. Cognition
- **Neural Planner**: Action sequence evaluation
- **Concept Formation**: Pattern abstraction
- **Attention**: Competitive selection

### 5. Development
- **Developmental Stages**: Initial → Critical Period → Maturation → Adult → Aging
- **Plasticity Modulation**: Age-dependent plasticity rates
- **Structural Development**: Synaptogenesis and pruning

### 6. World Interaction
- **Sensory Processing**: Vision, audio, internal signals
- **Motor Control**: Action selection and execution
- **Environment**: Grid world simulation

## Python Interface

The `pynlm` Python bindings provide full access to the NLM system:

### Installation
```bash
pip install pynlm
```

### Basic Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Process sensory input
# (Implementation-dependent)
```

### Core Classes
- **Config**: Configuration management
- **Brain**: Main neural simulation engine
- **AgentBrain**: Interface for agent-world interaction
- **SimpleWorld**: Environment simulation
- **SensoryInput**: Sensory data containers
- **Action**: Motor output representation

### Advanced Usage
```python
# Create world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)  # width, height, vision_width, vision_height

# Create agent brain
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Main simulation loop
while not episode_done:
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step, step * 0.001)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.001)
    
    # Handle reward and neuromodulation
    reward = world.computeReward()
    agent.applyRewardModulation(reward, predicted_reward)
```

## Experiment Management

The `ExperimentManager` provides comprehensive experiment management:

### Installation
```bash
# The manager is included in the NLM package
# Usage: python experiment_manager.py [command]
```

### Key Features
- **Experiment Creation**: Create and configure experiments
- **Batch Execution**: Run multiple experiments simultaneously
- **Parameter Sweeps**: Systematically vary parameters
- **Result Analysis**: Automatic analysis and visualization
- **Comparison Tools**: Compare experiment results
- **Checkpointing**: Save/restore experiment states
- **Visualization**: Generate plots and analysis reports

### Example Usage
```python
from experiment_manager import ExperimentManager

# Initialize manager
manager = ExperimentManager()

# Create a new experiment
config = {
    "name": "learning_experiment",
    "neuron_count": 500,
    "max_steps": 5000,
    "enable_development": True,
    "enable_replay": True,
    "exploration_bonus": 0.2
}

manager.create_experiment("learning_experiment", config)

# Run experiment
result = manager.run_experiment("learning_experiment")
print(f"Results: Reward={result.total_reward:.4f}, Firing Rate={result.avg_firing_rate:.4f}Hz")
```

### Available Commands
```bash
# Create experiment
python experiment_manager.py create "my_experiment" --config my_config.json

# Update experiment parameters
python experiment_manager.py update "my_experiment" --param neuron_count 1000 --param max_steps 10000

# Run experiment
python experiment_manager.py run "my_experiment" --output ./results/my_experiment

# List experiments
python experiment_manager.py list

# Delete experiment
python experiment_manager.py delete "my_experiment"

# Compare experiments
python experiment_manager.py compare "experiment1" "experiment2" "experiment3"

# Run batch experiments
python experiment_manager.py batch "exp1" "exp2" "exp3" --output ./batch_results

# Load configuration
python experiment_manager.py load-config my_config.json
```

## Visualization

### Built-in Visualization
The `ExperimentManager` includes automatic visualization capabilities:

- **Learning Curves**: Performance over time
- **Neural Activity**: Firing rate traces
- **Memory Strength**: Memory trace dynamics
- **Synaptic Weights**: Weight changes over time

### Custom Visualization
Users can create custom visualizations by accessing experiment data:
```python
# Access experiment results
results_dir = Path("results/my_experiment")

# Load metrics
step_metrics = pd.read_csv(results_dir / "step_metrics.csv")
learning_curve = pd.read_csv(results_dir / "learning_curve.csv")

# Create custom plots
import matplotlib.pyplot as plt

plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(step_metrics["step"], step_metrics["reward"])
plt.title("Reward Over Time")
plt.xlabel("Step")
plt.ylabel("Reward")

plt.subplot(2, 2, 2)
plt.plot(step_metrics["step"], step_metrics["firing_rate"])
plt.title("Neural Firing Rate")
plt.xlabel("Step")
plt.ylabel("Firing Rate (Hz)")

plt.tight_layout()
plt.savefig(results_dir / "custom_visualization.png")
```

## Configuration

### Default Configuration File
```ini
# configs/default.cfg

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Experiment Configuration
```json
{
    "name": "learning_experiment",
    "description": "Experiment investigating memory and learning",
    
    "max_steps": 10000,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.1,
    
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    
    "dopamine_baseline": 0.1,
    "curiosity_drive": 0.5,
    "novelty_sensitivity": 0.3,
    
    "reward_discount_factor": 0.99,
    "exploration_bonus": 0.1,
    
    "developmental_stages": 4,
    "critical_period_length": 1000,
    
    "environment_type": "grid_world",
    "environment_width": 10,
    "environment_height": 10,
    
    "working_memory_capacity": 100,
    "episodic_memory_max_episodes": 1000,
    
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true,
    "checkpoint_interval": 1000,
    "replay_interval": 100,
    "consolidation_interval": 5000,
    
    "visualization_enabled": true,
    "visualization_fps": 30,
    "visualization_update_rate": 10,
    
    "analysis_output_dir": "./experiment_results",
    "save_raw_traces": true,
    "save_spike_raster": true,
    "save_weights_over_time": true,
    "save_memory_traces": true
}
```

## Building and Running

### Building
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Examples
```bash
# Phase 2 demo (basic neural computation)
./nlm

# Phase 6 integration demo
./nlm_phase6_demo

# Phase 3 demo (world interaction)
./nlm_phase3_demo

# Phase 4 demo (cognition)
./nlm_phase4_demo
```

## Testing

### Running Tests
```bash
cd build
ctest
```

### Test Suite
- **test_neuron.cpp**: Neuron functionality tests
- **test_stdp.cpp**: STDP plasticity tests
- **test_synapse.cpp**: Synapse transmission tests
- **test_clock.cpp**: Simulation clock tests
- **test_config.cpp**: Configuration tests
- **test_main.cpp**: Integration tests

## Scientific Background

### Computational Principles
1. **LIF Neurons**: Leaky Integrate-and-Fire dynamics provide a balance of biological realism and computational efficiency
2. **Event-Driven Processing**: Spike-based communication enables efficient simulation of sparse activity
3. **Hebbian Learning**: "Neurons that fire together, wire together"
4. **Anti-Hebbian Learning**: STDP implements timing-based credit assignment
5. **Neuromodulation**: Multiple neuromodulators coordinate learning and behavior

### Research Questions
NLM investigates:

- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

### Limitations

- **Simplified Neuron Model**: LIF is not as detailed as Hodgkin-Huxley
- **No Realistic Ion Channels**: Simplified electrophysiology
- **Limited Dorsal/ventral Separation**: Simplified brain architecture
- **Computational Constraints**: Limited by available resources
- **No Consciousness Claims**: Not intended to produce subjective experience

## Advanced Features

### Custom Environments
```python
# Create custom environment
class CustomEnvironment:
    def __init__(self):
        self.width = 50
        self.height = 50
        self.resources = []
        self.hazards = []
    
    def reset(self):
        # Reset environment state
        pass
    
    def step(self, action):
        # Apply action and return observation, reward, done
        pass
```

### Custom Plasticity Rules
```python
class CustomPlasticityRule:
    def update(self, synapse, pre_spikes, post_spikes, dt):
        # Custom plasticity implementation
        # Called during brain.step()
        pass
```

### Multi-threading Support
```python
# Enable parallel processing
import pynlm
config = pynlm.createDefaultConfig()
config.parallel_processing = True
config.num_threads = 4

brain = pynlm.createBrain(config)
brain.initialize()
```

## Troubleshooting

### Common Issues

1. **Python Import Error**
   - Ensure pynlm is installed: `pip install pynlm`
   - Check Python version compatibility

2. **Compilation Errors**
   - Install build dependencies: `sudo apt-get install build-essential cmake python3-dev pybind11-dev`
   - Ensure C++20 compiler support

3. **Memory Issues**
   - Reduce neuron count in configuration
   - Increase system memory or use memory-mapped files

4. **Performance Issues**
   - Use larger timesteps for faster simulation
   - Enable checkpointing for long runs
   - Consider GPU acceleration if available

### Getting Help

- **Documentation**: This guide and the NLM documentation in `docs/`
- **Examples**: `examples/` directory contains additional usage examples
- **GitHub Issues**: Report bugs and feature requests
- **Scientific Contact**: See `docs/ARCHITECTURE.md` for research contacts

## Future Directions

### Planned Features
1. **GPU Acceleration**: SIMD and GPU support for faster simulation
2. **Advanced Visualizations**: 3D neural renderings
3. **Cloud Integration**: Distributed computing support
4. **Real-world Applications**: Sensor fusion and control
5. **Enhanced Learning**: Meta-learning and transfer learning

### Research Extensions
1. **Cognitive Architecture**: More complex cognitive structures
2. **Social Learning**: Multi-agent coordination
3. **Developmental Psychology**: More realistic development stages
4. **Neurological Models**: Disease modeling and treatment research

## License

MIT License

See `LICENSE` file for details.

## Authors

Research project - See `docs/ARCHITECTURE.md` for scientific background and author information.
