# NLM (Neural Learning Machine) - Quick Start Guide

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is **NOT**:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM **IS** intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start

### 1. Building from Source (Linux/macOS)

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run the Phase 2 demo (basic neural computation)
./nlm

# Run the Phase 3 demo (agent learning)
./nlm_phase3_demo

# Run the Phase 4 demo (social learning)
./nlm_phase4_demo

# Run performance benchmarks
./nlm_test
```

### 2. Running Integration Test (Phase 6)

```bash
# Phase 6 integration test
./nlm_phase6_demo

# This runs a comprehensive integration test verifying all brain systems are connected.
```

### 3. Python Usage

```bash
# Install Python dependencies
pip install pytest numpy

# Install NLM Python bindings
pip install .

# Or install in development mode for editing
pip install -e .
```

#### Basic Python Usage

```python
import pynlm

# Create and initialize a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
```

#### Advanced Python Usage (Agent Learning)

```python
import pynlm
import time

def run_complete_agent_simulation():
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(1000):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print("Agent simulation completed!")

# Run the simulation
run_complete_agent_simulation()
```

## Available Demos

### Phase 2: Real Neural Computation

```bash
./nlm [options]

Options:
  --config <file>    Load configuration from file
  --neuron-count <n> Set neuron count (default: 1000)
  --region-count <n> Set region count (default: 1)
  --steps <n>        Number of simulation steps (default: 1000)
  --help             Show help message
```

**What it tests:**
- Real Leaky Integrate-and-Fire neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

### Phase 3: Agent Learning

```bash
./nlm_phase3_demo [options]

Options:
  --episodes <n>    Number of episodes (default: 100)
  --world-width <w> World width (default: 20)
  --world-height <h> World height (default: 20)
  --help            Show help message
```

**What it tests:**
- Goal-directed learning with reward modulation
- Development and maturation stages
- Curiosity-driven exploration
- Novelty detection and prediction

### Phase 4: Social Learning

```bash
./nlm_phase4_demo [options]

Options:
  --population <n>     Population size (default: 10)
  --generations <n>    Number of generations (default: 50)
  --social-learning     Enable social learning
  --help               Show help message
```

**What it tests:**
- Social learning and cultural transmission
- Imitation and observational learning
- Group dynamics and coordination
- Evolution of social behaviors

### Phase 5: Performance Optimizations

```bash
./nlm_test [options]

Options:
  --benchmarks <list>  Benchmark configurations (e.g., "100x1000,500x1000")
  --help              Show help message
```

**What it tests:**
- Performance with different network sizes
- Memory usage optimization
- Parallel processing capabilities
- SIMD vectorization

### Phase 6: Integration Test

```bash
./nlm_phase6_demo

This runs a comprehensive integration test verifying all brain systems are connected:
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction system
- Cognition systems (planning, concept formation, attention)
- Development system
- Persistence and checkpointing
- Replay and consolidation
```

## Configuration

Configuration can be specified in:

### 1. Configuration File

Create a `config.json` or `default.cfg` file:

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.001,
            "pruning_rate": 0.0001
        }
    },
    "neuromodulation": {
        "dopamine": {
            "baseline": 0.1,
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "baseline": 0.5
        },
        "novelty": {
            "enable": true,
            "sensitivity": 0.5
        }
    },
    "environment": {
        "name": "GridWorld",
        "width": 20,
        "height": 20
    }
}
```

### 2. Command Line Arguments

```bash
./nlm --config config.json --neuron-count 2000 --steps 2000
```

Available parameters:

#### Brain Parameters
- `brain.neuron_count` (int): Total neurons (default: 1000)
- `brain.region_count` (int): Number of regions (default: 1)
- `brain.connection_probability` (float): Connection density (default: 0.1)

#### Plasticity Parameters
- `plasticity.stdp.enable` (bool): Enable STDP (default: true)
- `plasticity.stdp.learning_rate` (float): STDP learning rate (default: 0.001)
- `plasticity.hebbian.enable` (bool): Enable Hebbian (default: true)
- `plasticity.structural.enable` (bool): Enable structural plasticity (default: true)

#### Neuromodulation Parameters
- `neuromod.dopamine.scale` (float): Dopamine scaling (default: 1.0)
- `neuromod.curiosity.enable` (bool): Enable curiosity (default: true)
- `neuromod.novelty.enable` (bool): Enable novelty detection (default: true)

#### Development Parameters
- `development.synaptogenesis_rate` (float): Synaptogenesis rate (default: 0.001)
- `development.pruning_rate` (float): Pruning rate (default: 0.0001)

#### Simulation Parameters
- `simulation.timestep` (float): Simulation timestep (default: 0.001)
- `simulation.max_steps` (int): Maximum steps (default: 10000)

## Advanced Usage

### Running with Custom Configuration

```bash
# Create custom config
cat > my_config.json << EOF
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 3
    },
    "plasticity": {
        "stdp": { "learning_rate": 0.002 }
    }
}
EOF

# Run with custom config
./nlm --config my_config.json
```

### Running Multiple Experiments

```bash
#!/bin/bash

for neurons in 500 1000 2000; do
    for steps in 500 1000 2000; do
        echo "Running: $neurons neurons, $steps steps"
        ./nlm --neuron-count $neurons --steps $steps
        echo "Complete."
        sleep 1
    done
done
```

### Python Script Examples

#### Experiment Runner Script

```python
import pynlm
import time
import numpy as np

def run_experiment(neuron_counts, steps_list, output_file="results.csv"):
    """Run multiple experiments and save results."""
    
    results = []
    
    for neurons in neuron_counts:
        for steps in steps_list:
            print(f"Running experiment: {neurons} neurons, {steps} steps")
            
            start_time = time.time()
            
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neurons)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(steps):
                brain.step(step)
            
            end_time = time.time()
            
            result = {
                "neurons": neurons,
                "steps": steps,
                "time": end_time - start_time,
                "final_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "e_i_ratio": brain.getExcitationInhibitionRatio()
            }
            
            results.append(result)
            print(f"  Results: {result}")
    
    # Save to CSV
    import csv
    with open(output_file, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=results[0].keys())
        writer.writeheader()
        writer.writerows(results)
    
    print(f"Results saved to {output_file}")
    return results

# Run experiments
neuron_counts = [100, 500, 1000, 2000]
steps_list = [100, 500, 1000]

run_experiment(neuron_counts, steps_list)
```

#### Visualization Script

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def visualize_brain_activity(brain_file=None):
    """Visualize brain activity from simulation data."""
    
    if brain_file:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.load(brain_file)
    else:
        # Create a simple brain for demo
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some simulation
        for step in range(500):
            brain.step(step)
    
    # Get statistics
    neurons = brain.getTotalNeuronCount()
    synapses = brain.getTotalSynapseCount()
    firing = brain.getFiringNeuronCount()
    rate = brain.getAverageFiringRate()
    ei_ratio = brain.getExcitationInhibitionRatio()
    
    # Create visualization
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
    
    # Network statistics
    ax1.bar(['Neurons', 'Synapses', 'Firing', 'Avg Rate'], 
            [neurons, synapses, firing, rate], 
            color=['skyblue', 'lightgreen', 'salmon', 'gold'])
    ax1.set_title('Brain Statistics')
    ax1.set_ylabel('Count/Rate')
    
    # E/I ratio
    ax2.bar(['Excitatory/Inhibitory Ratio'], [ei_ratio], 
            color='lightblue')
    ax2.set_title('Excitation/Inhibition Balance')
    ax2.set_ylabel('Ratio')
    ax2.set_ylim(0, 2)
    
    # Spike activity over time (simplified)
    ax3.plot(range(0, min(100, neurons), 5), 
             np.random.poisson(10, neurons//5), 'b-')
    ax3.set_title('Spike Activity Pattern')
    ax3.set_xlabel('Time Steps')
    ax3.set_ylabel('Spikes')
    
    # Network diagram (simplified)
    ax4.scatter(np.random.uniform(0, 100, neurons),
                np.random.uniform(0, 100, neurons),
                c=np.random.uniform(0, 1, neurons), 
                alpha=0.6, cmap='viridis')
    ax4.set_title('Network Connectivity (simplified)')
    ax4.set_xlabel('X Position')
    ax4.set_ylabel('Y Position')
    
    plt.tight_layout()
    plt.show()
    
    print(f"Brain visualization complete!")
    print(f"  Total neurons: {neurons}")
    print(f"  Total synapses: {synapses}")
    print(f"  Currently firing: {firing}")
    print(f"  Average firing rate: {rate:.2f} Hz")
    print(f"  E/I ratio: {ei_ratio:.3f}")

# Create visualization
visualize_brain_activity()
```

## Getting Help

### Command Line Help

```bash
./nlm --help
./nlm_phase3_demo --help
./nlm_phase4_demo --help
```

### Python Documentation

```python
import pynlm
help(pynlm)  # Show Python module documentation

# Help for specific classes
help(pynlm.Brain)
help(pynlm.Config)
help(pynlm.SimpleWorld)
help(pynlm.AgentBrain)
```

### Online Documentation

- [NLM Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Scientific Papers](https://scholar.google.com/scholar?q=NLM+brain)

## Troubleshooting

### Common Issues

#### Build Errors
```bash
# Install required dependencies
pip install scikit-build-core pybind11 pytest numpy

# Configure with specific Python
cmake .. -DPython_EXECUTABLE=$(which python3)
```

#### Runtime Errors
```bash
# Segmentation fault - ensure initialization
./nlm --neuron-count 100 --steps 10
```

#### Python Import Errors
```bash
# Force reinstall
pip install --force-reinstall .
```

### Performance Tips

1. **For large networks**: Use `--neuron-count 1000` and `--steps 500`
2. **For development**: Use smaller networks like `--neuron-count 100`
3. **For production**: Consider using SIMD optimizations by building with `-O3` flag

## Examples and Tutorials

The repository includes several example scripts:

- `examples/simple_brain.py` - Basic brain creation and simulation
- `examples/agent_learning.py` - Complete agent learning example
- `examples/experiment_runner.py` - Batch experiment execution
- `examples/visualization.py` - Brain activity visualization

## Next Steps

1. **Explore Phase 3**: Try the agent learning demo with custom environments
2. **Learn Phase 4**: Experiment with social learning and group dynamics
3. **Scale Up**: Run large-scale simulations with thousands of neurons
4. **Contribute**: Add new features or fix bugs in the codebase

This project is under active development. Visit the GitHub repository for the latest updates and contributions.
