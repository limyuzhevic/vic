# NLM Advanced User Interface and Configuration

## Command Line Interface

### Basic Commands

```bash
# Run the default Phase 6 integration demo
./nlm

# Run with custom configuration
./nlm --config custom_config.cfg

# Run specific test mode
./nlm --test-mode phase6-integration

# Run with verbose logging
./nlm --verbose

# Set simulation parameters
./nlm --neuron-count 2000 --max-steps 5000

# Enable/disable specific features
./nlm --enable-checkpointing --disable-replay --enable-development
```

### Advanced Commands

```bash
# Run with multiple simulation runs
./nlm --multi-seed 5 --runs-per-seed 3

# Run performance benchmark
./nlm --benchmark --benchmark-duration 60

# Export brain state
./nlm --export-brain brain_state.bin

# Import brain state
./nlm --import-brain saved_brain.bin

# Run ablation study
./nlm --ablation study_name --compare-with baseline

# Generate performance report
./nlm --generate-report --output-format json

# Run with custom environment
./nlm --environment-size 30x30 --vision-range 8
```

## Configuration File

### Default Configuration (`configs/default.cfg`)

```ini
# Core simulation parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.1
random_seed = 42
simulation_timestep = 0.001

# Performance settings
enable_multithreading = true
enable_SIMD = true
max_memory_usage_mb = 2048
checkpoint_interval = 1000
replay_interval = 100

# Feature toggles
enable_checkpointing = true
enable_replay = true
enable_development = true
enable_curiosity = true
enable_structural_plasticity = true

# Neuromodulation parameters
dopamine_sensitivity = 1.0
curiosity_threshold = 0.3
novelty_decay_rate = 0.99

# Memory system parameters
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
associative_memory_size = 500

# Sensory/Motor parameters
vision_width = 16
vision_height = 16
touch_sensory_neurons = 8
internal_sensory_neurons = 4
proprioception_neurons = 6
motor_actions = 6

# Development parameters
initial_plasticity = 1.0
critical_period_plasticity = 0.8
maturation_plasticity = 0.5
adult_plasticity = 0.2

# Plasticity rule parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
hebbian_threshold = 0.5

# Experimental parameters
experiment_duration = 60000
reward_function = exponential
energy_cost_per_action = 0.1
```

### Advanced Configuration (`configs/advanced.cfg`)

```ini
# High-performance settings
neuron_count = 5000
region_count = 4
connection_probability = 0.05

# Parallel processing
enable_multithreading = true
max_threads = 8
enable_SIMD = true
vectorization_width = 16

# Memory management
memory_pool_size = 10000
event_queue_size = 50000
gc_interval = 5000

# Advanced neuromodulation
enable_adaptive_curiosity = true
adaptation_rate = 0.01
memory_based_curiosity = true

# Complex development
enable_phases = true
initial_phase_duration = 1000
critical_period_duration = 5000
maturation_phase_duration = 15000

# Advanced plasticity
enable_eligibility_traces = true
eligibility_decay_rate = 0.1
 neuromodulation_integration = true
plasticity_modulation_strength = 0.8

# Cognitive complexity
enable_predictive_coding = true
prediction_horizon = 10
concept_lifetime = 30000
attention_mechanism = competitive

# Social and environmental complexity
enable_social_learning = true
social_group_size = 5
social_interaction_rate = 0.3
environment_change_rate = 0.01
```

## Python Interface (pynlm)

### Basic Usage

```python
import pynlm

# Initialize brain
brain = pynlm.Brain(neuron_count=1000, random_seed=42)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Get metrics
print(f"Total neurons: {brain.get_total_neuron_count()}")
print(f"Total synapses: {brain.get_total_synapse_count()}")
print(f"Firing rate: {brain.get_average_firing_rate()}")

# Access memory systems
working_memory = brain.get_working_memory()
episodic_memory = brain.get_episodic_memory()

# Save/load checkpoint
brain.save("brain_state.bin")
brain2 = pynlm.Brain("configs/advanced.cfg")
brain2.load("brain_state.bin")
```

### Advanced Python API

```python
import pynlm
import numpy as np

class NLMSession:
    def __init__(self, config_file=None):
        self.config = config_file or "configs/default.cfg"
        self.brain = None
        self.agent = None
        self.world = None
        
    def setup(self, **kwargs):
        """Setup simulation with custom parameters"""
        config = pynlm.Config(self.config)
        
        # Override with custom parameters
        for key, value in kwargs.items():
            config.set(key, value)
            
        self.brain = pynlm.Brain(config)
        self.world = pynlm.SimpleWorld(config)
        self.agent = pynlm.AgentBrain(self.brain)
        
        self.brain.initialize()
        self.agent.initialize(self.world)
        
    def run_episode(self, max_steps=1000):
        """Run a single episode and return metrics"""
        episode_reward = 0.0
        episode_steps = 0
        
        for step in range(max_steps):
            # Get observation
            percept = self.world.observe(self.brain.get_region(0))
            
            # Process sensory input
            self.agent.process_sensory_input(percept)
            
            # Run brain
            self.brain.step(step, step * 0.001)
            
            # Get action
            command = self.agent.decode_motor_command()
            
            # Apply action
            result = self.world.apply_action(self.brain.get_region(0), command)
            
            # Apply reward
            self.agent.apply_reward_modulation(result.reward, 0.0)
            
            # Update development
            self.agent.update_development(0.001)
            
            episode_reward += result.reward
            episode_steps += 1
            
        metrics = {
            'reward': episode_reward / episode_steps,
            'steps': episode_steps,
            'neurons': self.brain.get_total_neuron_count(),
            'synapses': self.brain.get_total_synapse_count(),
            'firing_rate': self.brain.get_average_firing_rate(),
            'memory_episodes': self.brain.get_episodic_memory().get_episode_count()
        }
        
        return metrics
        
    def training_loop(self, episodes=100, steps_per_episode=200):
        """Run multiple training episodes"""
        history = []
        
        for episode in range(episodes):
            metrics = self.run_episode(steps_per_episode)
            metrics['episode'] = episode
            history.append(metrics)
            
            # Print progress
            if episode % 10 == 0:
                print(f"Episode {episode}: Reward={metrics['reward']:.3f}, "
                      f"Neurons={metrics['neurons']}, "
                      f"Episodes stored={metrics['memory_episodes']}")
                
        return history
        
    def save_session(self, path):
        """Save entire session state"""
        import json
        import pickle
        
        session_data = {
            'config': self.config,
            'brain_state': self.brain.save_to_dict(),
            'history': getattr(self, 'history', [])
        }
        
        with open(path, 'wb') as f:
            pickle.dump(session_data, f)
            
    def load_session(self, path):
        """Load session state from file"""
        import pickle
        
        with open(path, 'rb') as f:
            session_data = pickle.load(f)
            
        self.config = session_data['config']
        self.setup()
        self.brain.load_from_dict(session_data['brain_state'])
        
        return session_data.get('history', [])
```

## Interactive Command Line Interface

```bash
# Start interactive session
./nlm-interactive

# Commands available:
help                    Show available commands
config set <key> <value> Set configuration value
config get <key>        Get configuration value
brain initialize        Initialize brain
brain reset             Reset brain
brain save <path>       Save brain state
brain load <path>       Load brain state
world reset             Reset environment
world step             Execute one world step
metrics print           Print current metrics
memory info            Show memory statistics
attention              Show attention distribution
replay                 Trigger memory replay
checkpoint              Create checkpoint
status                  Show system status
quit                   Exit interactive session

# Example session:
> help
> config set simulation_timestep 0.01
> world reset
> brain initialize
> status
> world step
> metrics print
> brain save my_brain.bin
```

## Visualization and Monitoring

### Real-time Monitoring

```bash
# Launch real-time monitor
./nlm-monitor

# Monitor metrics
./nlm-monitor --metrics firing_rate,reward,memory,energy

# Monitor neural activity
./nlm-monitor --neural-activity --neuron-id 42

# Monitor memory systems
./nlm-monitor --memory working,episodic,associative

# Export monitoring data
./nlm-monitor --export-csv monitoring_data.csv --interval 1.0
```

### Visualization Commands

```bash
# Generate neural network visualization
./nlm-visualize --output-type pdf --highlight-activity --neuron-threshold 0.5

# Create training progress plot
./nlm-visualize --type progress --input metrics.json --output progress.png

# Generate memory heatmap
./nlm-visualize --type memory --brain-state brain.bin --output memory_heatmap.png

# Create 3D brain model
./nlm-visualize --type 3d --brain-state brain.bin --output brain_3d.glb
```

## Performance Optimization Commands

### Tuning Parameters

```bash
# Profile current configuration
./nlm --profile --profile-output profile.txt

# Run performance benchmark
./nlm --benchmark --benchmark-config benchmark.cfg

# Optimize for specific hardware
./nlm --optimize --target-cpu-architecture AVX512

# Analyze bottlenecks
./nlm --analyze-bottlenecks --output bottleneck_report.txt

# Run stress test
./nlm --stress-test --duration 300 --neuron-scale 2.0
```

### Custom Configuration Commands

```bash
# Create custom configuration from template
./nlm config create my_config.cfg --template custom

# Merge configurations
./nlm config merge base_config.cfg overrides.cfg --output merged_config.cfg

# Validate configuration
./nlm config validate my_config.cfg

# Test configuration
./nlm config test my_config.cfg --runs 5
```

## Research and Experiment Commands

### Experiment Management

```bash
# Run predefined experiment suite
./nlm --experiment-suite phase6-integration --runs 10

# Create custom experiment
./nlm --experiment create custom_exp --type continual_learning

# Run experiment with specific parameters
./nlm --experiment run custom_exp --parameters "{'learning_rate': 0.1, 'decay': 0.001}"

# Compare experiment results
./nlm --experiment compare exp1 exp2 --metrics reward,learning_speed,memory_retention

# Export experiment data
./nlm --experiment export custom_exp --format json --output experiment_results.json
```

### Scientific Data Export

```bash
# Export neural data for analysis
./nlm --export-neural-data brain.bin --spike-traces --synapse-weights --output neural_data.h5

# Export behavioral data
./nlm --export-behavioral-data --actions --rewards --observations --output behavioral_data.csv

# Export memory data
./nlm --export-memory-data --episodes --activations --patterns --output memory_data.mat

# Create analysis dataset
./nlm --create-analysis-dataset --input brain.bin --output analysis_dataset.tar.gz
```

## Configuration Schema

### Configuration File Format

```ini
# Comments start with #
# Sections are in [section_name] format
# Values can be numbers, strings, or booleans

[core]
# Basic simulation parameters
neuron_count = 1000
region_count = 1
random_seed = 42
simulation_timestep = 0.001

[performance]
# Performance optimization settings
enable_multithreading = true
max_threads = 8
enable_SIMD = true
memory_limit_mb = 2048
checkpoint_interval = 1000

[features]
# Feature toggles
enable_checkpointing = true
enable_replay = true
enable_development = true
enable_curiosity = true
enable_structural_plasticity = true

[neuromodulation]
# Neuromodulator parameters
dopamine_sensitivity = 1.0
curiosity_threshold = 0.3
novelty_decay_rate = 0.99

[memory]
# Memory system parameters
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
associative_memory_size = 500

[sensory]
# Sensory system parameters
vision_width = 16
vision_height = 16
touch_sensory_neurons = 8
internal_sensory_neurons = 4
proprioception_neurons = 6

[motor]
# Motor system parameters
motor_actions = 6
action_time_steps = 10
energy_cost_per_action = 0.1
```

## Advanced Configuration Examples

### High-Performance Configuration

```ini
[core]
neuron_count = 5000
region_count = 4
random_seed = 12345

[performance]
enable_multithreading = true
max_threads = 16
enable_SIMD = true
memory_limit_mb = 8192
checkpoint_interval = 5000

[features]
enable_checkpointing = true
enable_replay = true
enable_development = true
enable_curiosity = true
enable_structural_plasticity = true

[neuromodulation]
dopamine_sensitivity = 1.5
curiosity_threshold = 0.2
novelty_decay_rate = 0.95
```

### Research Configuration

```ini
[core]
neuron_count = 2000
region_count = 2
random_seed = 42

[performance]
enable_multithreading = false  # For reproducibility
enable_SIMD = true
memory_limit_mb = 4096

[features]
enable_checkpointing = true
enable_replay = true
enable_development = true
enable_curiosity = true

[experiment]
experiment_type = continual_learning
num_tasks = 10
task_steps = 1000
reward_function = sparse
```

### Educational Configuration

```ini
[core]
neuron_count = 500
region_count = 1
random_seed = 42

[performance]
enable_multithreading = true
enable_SIMD = true

[features]
enable_checkpointing = true
enable_replay = false
enable_development = true
enable_curiosity = true

[educational]
show_learning_progress = true
enable_explanations = true
interactive_mode = true

[neuromodulation]
dopamine_sensitivity = 0.5
curiosity_threshold = 0.4
novelty_decay_rate = 0.9
```

## Installation and Setup

### System Requirements

```bash
# Minimum requirements
RAM: 2 GB
CPU: Single core (SIMD support recommended)
Disk: 100 MB free space

# Recommended requirements
RAM: 8 GB or more
CPU: Multi-core with SIMD support
GPU: For accelerated computation (optional)
Disk: 1 GB free space for experiments
```

### Installation Steps

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build from source (Linux/macOS)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20
make -j$(nproc)

# Install Python bindings
pip install -e .

# Test installation
./nlm --help
python -c "import pynlm; print(pynlm.__version__)"

# Run basic test
./nlm --test-mode basic
python -c "import pynlm; brain = pynlm.Brain(); brain.initialize(); print('Success')"
```

### Development Setup

```bash
# Install development dependencies
apt-get update
apt-get install -y cmake build-essential python3-dev python3-pip
pip install pytest pytest-cov black flake8 mypy

# Install additional tools for development
apt-get install -y valgrind gdb

# Set up development environment
cat > .dev-env << 'EOF'
export NLM_HOME=$(pwd)
export PATH=$NLM_HOME/bin:$PATH
export PYTHONPATH=$NLM_HOME/python:$PYTHONPATH
EOF

# Create development workspace
mkdir -p dev_workspace/experiments
git clone https://github.com/nlm-project/nlm-examples.git dev_workspace/examples
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: "Failed to initialize brain: Configuration is null"

**Solution:**
```bash
# Check configuration file
cat configs/default.cfg

# Fix configuration
./nlm config validate configs/default.cfg

# Reinitialize with correct configuration
./nlm --config configs/default.cfg
```

#### Issue: Memory allocation fails

**Solution:**
```bash
# Increase memory limits
./nlm --config-set memory_limit_mb=4096

# Disable memory-intensive features
./nlm --disable-replay --disable-checkpointing

# Run with reduced neuron count
./nlm --neuron-count 500
```

#### Issue: Performance is too slow

**Solution:**
```bash
# Enable SIMD optimization
./nlm --enable-SIMD

# Enable multithreading
./nlm --enable-multithreading

# Optimize configuration
./nlm --optimize-config
```

#### Issue: Neural network visualization fails

**Solution:**
```bash
# Install required dependencies
./nlm install-deps viz

# Check OpenGL support
glxinfo | grep -i rendering

# Try simpler visualization
./nlm-visualize --output-type svg
```

### Getting Help

```bash
# Online documentation
https://nlm.readthedocs.io

# GitHub repository
https://github.com/nlm-project/nlm

# Issue tracker
https://github.com/nlm-project/nlm/issues

# Discussion forum
https://discourse.nlm-project.org

# User support
./nlm --support
python -c "import pynlm; pynlm.help()"
```

## Future Enhancements

### Planned Features

1. **Advanced Learning Algorithms**
   - Meta-learning capabilities
   - Transfer learning between tasks
   - Few-shot learning

2. **Enhanced Social Learning**
   - Multi-agent interactions
   - Cultural transmission
   - Social hierarchies

3. **Improved Memory Systems**
   - Semantic memory
   - Procedural memory
   - Working memory optimization

4. **Advanced Neuromodulation**
   - Affective modulation
   - Stress response
   - Sleep-wake cycles

5. **Real-world Integration**
   - Sensor interface
   - Motor control
   - Environmental interaction

6. **Cloud and Distributed Computing**
   - Multi-GPU support
   - Cloud deployment
   - Distributed training

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

This file contains all the advanced user interface commands, configuration options, and documentation for the improved NLM system. It provides comprehensive guidance for users ranging from beginners to advanced researchers and developers.