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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/        # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Python Bindings (pynlm)

The project includes Python bindings via pybind11 for easy Python integration:

### Installation

```bash
pip install .
```

### Basic Usage

```python
from pynlm import Brain, Config

# Create brain with configuration
config = Config()
config.set("neuron_count", 1000)
brain = Brain(config)

# Initialize and run simulation
if brain.initialize():
    brain.step(0, 0.001)
    brain.reset()
```

### Python API Functions

#### Brain Class
- `initialize()`: Initialize the neural brain system
- `step(step, time)`: Run one simulation step
- `reset()`: Reset brain state
- `save(filename)`: Save brain state to checkpoint
- `load(filename)`: Load brain state from checkpoint

#### Configuration Management
- `Config()`: Create new configuration
- `config.set(key, value)`: Set configuration values
- `config.get(key)`: Get configuration values
- `config.loadFromFile(filename)`: Load configuration from file

#### Neural Components
- `Brain`: Core neural processing unit
- `Neuron`: Spiking neuron model
- `Synapse`: Neural connection between neurons
- `NeuralRegion`: Group of neurons with shared properties

#### Memory Systems
- `WorkingMemory`: Short-term memory for active neurons
- `EpisodicMemory`: Event-based memory for experiences
- `AssociativeMemory`: Pattern-based memory associations

#### Neuromodulation
- `Dopamine`: Reward prediction error signal
- `Curiosity`: Exploration drive
- `Novelty`: Change detection
- `PredictionError`: Prediction accuracy monitoring

#### Cognitive Systems
- `NeuralPlanner`: Action sequence evaluation
- `ConceptFormation`: Pattern abstraction and categorization
- `Attention`: Competitive selection of neural representations

#### Development
- `DevelopmentSystem`: Age-dependent neural changes
- `Synaptogenesis`: Formation of new connections
- `Pruning`: Elimination of weak connections
- `Maturation`: Neural development and refinement

### Available Configuration Keys

#### Core Simulation
- `neuron_count`: Total number of neurons (default: 1000)
- `region_count`: Number of neural regions (default: 1)
- `simulation_timestep`: Time step for simulation (default: 0.001)
- `random_seed`: Random seed for reproducibility

#### Neural Properties
- `neuron_resting_potential`: Neuron's resting voltage
- `neuron_threshold`: Spike threshold voltage
- `neuron_refractory_period`: Refractory period duration
- `connection_probability`: Probability of synaptic connections

#### Plasticity Rules
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Rate of new synapse formation
- `pruning_rate`: Rate of synapse elimination

#### Neuromodulation
- `dopamine_level`: Baseline dopamine level
- `curiosity_threshold`: Novelty detection threshold
- `novelty_decay_rate`: Rate of novelty signal decay

#### Memory Systems
- `working_memory_capacity`: Capacity of working memory
- `episodic_memory_max_episodes`: Maximum stored episodes
- `associative_memory_patterns`: Number of pattern associations

#### Advanced Features
- `enable_checkpointing`: Enable/disable checkpoint saving
- `checkpoint_dir`: Directory for checkpoint files
- `checkpoint_interval`: Interval between automatic checkpoints
- `replay_interval`: Memory replay frequency
- `consolidation_interval`: Memory consolidation frequency

### Performance Configuration

#### Memory Pools and Optimization
- `enable_memory_pools`: Enable memory pool allocation
- `enable_simd`: Enable SIMD vectorization
- `enable_multithreading`: Enable parallel processing
- `thread_count`: Number of worker threads

#### Visualization
- `enable_visualization`: Enable real-time visualization
- `visualization_update_rate`: Visualization refresh rate (Hz)

### Command Line Arguments

```bash
./nlm [options]

options:
  --config=file.cfg      Load configuration from file
  --neuron-count=N       Set number of neurons
  --timestep=T           Set simulation time step
  --random-seed=S        Set random seed for reproducibility
  --checkpoint-dir=D      Set checkpoint directory
  --help                 Show help message
```

### Example Scripts

#### Simple Learning Example

```python
#!/usr/bin/env python3
"""Example: Simple neural learning demonstration"""

from pynlm import Brain, Config
import time

def main():
    # Create and configure brain
    config = Config()
    config.set("neuron_count", 500)
    config.set("random_seed", 42)
    config.set("enable_checkpointing", True)
    
    brain = Brain(config)
    
    if not brain.initialize():
        print("Failed to initialize brain")
        return 1
    
    print("Brain initialized successfully")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation for 1000 steps
    for step in range(1000):
        brain.step(step, step * 0.001)
        
        # Apply sensory input every 100 steps
        if step % 100 == 0:
            # Simple input pattern
            input_data = [0.1, 0.2, 0.3, 0.4, 0.5]
            brain.receiveSensoryInput(input_data)
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, "
                  f"Firing={brain.getFiringNeuronCount()}")
    
    # Save final brain state
    brain.save("final_brain_state.checkpoint")
    print("Brain state saved to final_brain_state.checkpoint")
    
    # Reset and run plasticity experiment
    brain.reset()
    brain.initialize()
    
    print("\nPlasticity experiment completed")
    
    return 0

if __name__ == "__main__":
    exit(main())
```

#### Advanced Configuration Example

```python
from pynlm import Config
import json

# Create advanced configuration
config = Config()

# Core simulation parameters
config.set("neuron_count", 2000)
config.set("region_count", 4)
config.set("simulation_timestep", 0.0005)

# Connectivity and structure
config.set("connection_probability", 0.15)
config.set("neuron_resting_potential", -65.0)
config.set("neuron_threshold", -50.0)
config.set("neuron_refractory_period", 2.0)

# Plasticity rules (STDP)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)
config.set("stdp_tau", 20.0)

# Structural plasticity
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00005)

# Neuromodulation
config.set("dopamine_level", 0.1)
config.set("curiosity_threshold", 0.3)
config.set("novelty_decay_rate", 0.99)

# Memory systems
config.set("working_memory_capacity", 100)
config.set("episodic_memory_max_episodes", 5000)
config.set("associative_memory_patterns", 1000)

# Advanced features
config.set("enable_checkpointing", True)
config.set("checkpoint_dir", "./checkpoints")
config.set("checkpoint_interval", 1000)
config.set("replay_interval", 50)
config.set("consolidation_interval", 5000)

# Performance optimization
config.set("enable_memory_pools", True)
config.set("enable_simd", True)
config.set("enable_multithreading", True)
config.set("thread_count", 4)

# Visualization
config.set("enable_visualization", True)
config.set("visualization_update_rate", 60.0)

# Save to file
config.saveToFile("advanced_config.cfg")

# Load from file
config2 = Config()
config2.loadFromFile("advanced_config.cfg")

print(config2.summary())
```

### Error Handling and Troubleshooting

#### Common Issues

**1. Brain Initialization Failure**
```python
# Check configuration
if not brain.initialize():
    print("Brain initialization failed")
    # Check config for invalid values
    if config.get("neuron_count") < 100:
        config.set("neuron_count", 100)
        brain.initialize()
```

**2. Checkpoint Issues**
```python
# Ensure checkpoint directory exists
import os
os.makedirs("checkpoints", exist_ok=True)

# Save with error handling
try:
    brain.save("test.checkpoint")
    print("Checkpoint saved successfully")
except Exception as e:
    print(f"Checkpoint save failed: {e}")
```

**3. Memory Issues**
```python
# Monitor memory usage
import psutil
process = psutil.Process()
memory_mb = process.memory_info().rss / 1024 / 1024
print(f"Memory usage: {memory_mb:.1f} MB")

# Clear memory when done
brain.reset()
del brain
import gc
gc.collect()
```

### Performance Tips

1. **Use larger simulation timesteps** for faster but less accurate simulations
2. **Limit neuron count** for smaller systems
3. **Enable memory pools** for better performance
4. **Use multithreading** for parallel processing
5. **Configure checkpoint intervals** to balance memory and persistence

### Getting Help

1. **Documentation**: See `docs/` directory for detailed architecture and scientific background
2. **API Reference**: Generated documentation available online
3. **Community**: Join the NLM community for support and discussions
4. **Examples**: Check `examples/` directory for more use cases

### License

MIT License

### Authors

Research project — See docs for scientific background.
