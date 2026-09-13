# Command line interface for NLM

This document describes the command line interface (CLI) for the NLM project,
providing comprehensive information about available commands, options, and
advanced features for power users.

## Installation

### From Source (Recommended)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using Binary Release

```bash
# Download from releases page
# On Linux:
curl -L https://github.com/nlm-project/nlm/releases/download/v0.1.0/nlm_0.1.0_Linux_x86_64.tar.gz | tar xz
cd nlm_0.1.0_Linux_x86_64
./nlm
```

## Basic Commands

### `nlm` - Main executable

Run the basic NLM simulation:

```bash
nlm
```

This runs the Phase 2 demonstration with real neural computation.

### `nlm --help`

Show help information:

```bash
nlm --help
```

### `nlm --version`

Show version information:

```bash
nlm --version
```

## Advanced Commands

### `nlm --run-experiment <type>`

Run specific experiments:

```bash
nlm --run-experiment phase3-demo      # Phase 3 agent experiments
nlm --run-experiment phase4-demo      # Phase 4 cognitive experiments
nlm --run-experiment phase5-demo      # Phase 5 performance experiments
nlm --run-experiment phase6-demo      # Phase 6 integration experiments
nlm --run-experiment benchmark         # Performance benchmarks
```

### `nlm --config <file>`

Load configuration from file:

```bash
nlm --config my_config.cfg
```

### `nlm --output <dir>`

Set output directory for checkpoints and logs:

```bash
nlm --output ./results
```

### `nlm --checkpoint <file>`

Load/save brain state from checkpoint:

```bash
# Save to checkpoint
nlm --checkpoint save brain_state.bin

# Load from checkpoint  
nlm --checkpoint load brain_state.bin
```

### `nlm --log-level <level>`

Set logging level:

```bash
nlm --log-level DEBUG   # Most verbose
nlm --log-level INFO    # Default
nlm --log-level WARNING # Fewer messages
nlm --log-level ERROR   # Only errors
```

### `nlm --steps <number>`

Run specific number of simulation steps:

```bash
nlm --steps 10000
```

### `nlm --timestep <seconds>`

Set simulation timestep:

```bash
nlm --timestep 0.0001
```

## Agent Interface Commands

### `nlm --agent-mode`

Run in agent mode with world interaction:

```bash
nlm --agent-mode --world simple --visualize
```

### `nlm --enable-learning`

Enable learning subsystems:

```bash
nlm --enable-learning reward curiosity structural development
```

### `nlm --learning-rate <value>`

Set learning rate for plasticity:

```bash
nlm --learning-rate 0.001
```

## Python API Commands

### `python -m nlm <command>`

Access Python API directly:

```bash
# Create a simple brain
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize()"

# Run a complete simulation with world
python -c "
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print('Simulation complete!')
"

# Load/save brain state
python -c "
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.save('my_brain.bin')
# ... run simulation ...
brain.load('my_brain.bin')
"
```

## Performance Commands

### `nlm --performance <mode>`

Run with specific performance settings:

```bash
# Enable memory pools and event queues
nlm --performance memory-pools

# Enable SIMD acceleration  
nlm --performance simd

# Enable multithreading
nlm --performance threads

# All optimizations
nlm --performance all
```

### `nlm --benchmark`

Run performance benchmarks:

```bash
nlm --benchmark --mode real-time  # Real-time performance test
nlm --benchmark --mode scaled    # Scalability test (more neurons)
nlm --benchmark --mode accuracy  # Accuracy vs speed tradeoff
```

## Visualization Commands

### `nlm --visualize`

Run with visualization (if supported):

```bash
nlm --visualize --output video.mp4 --fps 30
```

### `nlm --save-viz <format>`

Save visualizations in different formats:

```bash
nlm --save-viz json    # Neural activity data
nlm --save-viz csv     # Time series data
nlm --save-viz dat     # Plot data
```

## Debugging Commands

### `nlm --debug <module>`

Enable debugging for specific modules:

```bash
nlm --debug neurons      # Debug neural dynamics
nlm --debug plasticity   # Debug learning rules
nlm --debug memory       # Debug memory systems
nlm --debug prediction   # Debug prediction system
```

### `nlm --trace <event>`

Trace specific events:

```bash
nlm --trace spikes      # Trace spike events
nlm --trace synapses     # Trace synaptic connections
nlm --trace weights      # Trace weight changes
```

### `nlm --validate`

Validate brain state:

```bash
nlm --validate --check-integrity --check-plasticity
```

## Scripting Examples

### Basic simulation script (nlm.py)

```python
#!/usr/bin/env python3
import pynlm
import time
import argparse

def main():
    parser = argparse.ArgumentParser(description='NLM simulation script')
    parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    parser.add_argument('--config', type=str, default='configs/default.cfg', help='Configuration file')
    parser.add_argument('--output', type=str, default='checkpoint.bin', help='Output checkpoint file')
    parser.add_argument('--load', type=str, default=None, help='Load checkpoint file')
    parser.add_argument('--enable-learning', action='store_true', help='Enable learning subsystems')
    parser.add_argument('--world', action='store_true', help='Include world simulation')
    
    args = parser.parse_args()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    if args.config:
        config.loadFromFile(args.config)
    
    # Configure
    config.set("brain.neuron_count", args.neurons)
    config.set("brain.connection_probability", 0.1)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if args.load:
        brain.load(args.load)
    
    # Create agent and world if requested
    agent = None
    world = None
    if args.world:
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        if args.enable_learning:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
    
    # Run simulation
    print(f"Starting NLM simulation with {args.steps} steps...")
    start_time = time.time()
    
    for step in range(args.steps):
        if args.world and world:
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        
        brain.step(step)
        
        if args.world and agent:
            action = agent.decodeMotorCommand()
            if world:
                world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # Save checkpoint
    brain.save(args.output)
    print(f"Simulation complete. Brain saved to {args.output}")
    print(f"Total simulation time: {time.time() - start_time:.2f}s")
    
    # Print statistics
    print("\nFinal Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    if agent:
        print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.2f}")
        print(f"  Prediction error: {agent.getPredictionError():.2f}")

if __name__ == "__main__":
    main()
```

### Experiment script (run_experiment.py)

```python
#!/usr/bin/env python3
import pynlm
import numpy as np
import json

def run_plasticity_experiment(steps=5000):
    """Run plasticity learning experiment"""
    print("Running Plasticity Experiment...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable plasticity
    if auto* stdp = brain.getSTDP()) {
        stdp->enablePlasticity(true, true, false);
    }
    
    # Record initial weights
    initial_weights = []
    auto* region = brain.getRegion(RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            initial_weights.push_back(syn->getWeight());
        }
    }
    
    # Apply repeated input pattern
    for step = 0; step < steps; ++step) {
        // Inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step);
        
        if (step % 500 == 0) {
            float weight_change = calculate_weight_change(initial_weights);
            print(f"Step {step}: Weight change = {weight_change:.4f}");
        }
    }
    
    // Record final weights and compute statistics
    float final_weight_mean = calculate_mean_weight();
    float initial_weight_mean = calculate_mean_weight();
    float learning_effectiveness = (final_weight_mean - initial_weight_mean) / initial_weight_mean;
    
    results = {
        "initial_weight_mean": initial_weight_mean,
        "final_weight_mean": final_weight_mean,
        "weight_change": final_weight_mean - initial_weight_mean,
        "learning_effectiveness": learning_effectiveness,
        "total_spikes": brain->getTotalSpikeCount()
    };
    
    return results;
}

def run_agent_experiment(steps=2000):
    """Run agent-environment interaction experiment"""
    print("Running Agent-Environment Experiment...")
    
    config = pynlm.createDefaultConfig();
    brain = pynlm.createBrain(config);
    brain->initialize();
    
    agent = pynlm.createAgentBrain(brain);
    world = pynlm.createSimpleWorld();
    world->configure(width=30, height=30, visionWidth=8, visionHeight=8);
    world->reset();
    agent->initialize(world);
    
    // Enable learning systems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    
    // Track performance metrics
    vector<float> rewards;
    vector<float> curiosity_levels;
    vector<float> novelty_levels;
    
    for (size_t step = 0; step < steps; ++step) {
        world->update(0.1);
        
        percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        brain->step(step);
        
        action = agent->decodeMotorCommand();
        result = world->applyMotorCommand(action, world->getSimulationTime());
        
        agent->applyRewardModulation(result.reward, 0.0);
        agent->updateDevelopment(0.1);
        
        rewards.push_back(result.reward);
        curiosity_levels.push_back(agent->getCuriosityLevel());
        novelty_levels.push_back(agent->getNoveltyLevel());
        
        if (step % 200 == 0) {
            float avg_reward = calculate_mean(rewards);
            print(f"Step {step}: Avg reward = {avg_reward:.2f}, "
                  f"Curiosity = {agent->getCuriosityLevel():.2f}");
        }
    }
    
    results = {
        "total_reward": accumulate(rewards),
        "avg_reward": calculate_mean(rewards),
        "max_reward": *max_element(rewards.begin(), rewards.end()),
        "curiosity_trajectory": curiosity_levels,
        "novelty_trajectory": novelty_levels,
        "final_curiosity": agent->getCuriosityLevel(),
        "final_novelty": agent->getNoveltyLevel()
    };
    
    return results;
}

int main() {
    try {
        // Run plasticity experiment
        auto plasticity_results = run_plasticity_experiment(5000);
        
        // Run agent experiment
        auto agent_results = run_agent_experiment(3000);
        
        // Combine and save results
        map<string, variant> all_results;
        all_results["plasticity"] = plasticity_results;
        all_results["agent"] = agent_results;
        
        // Save to JSON file
        ofstream outfile("experiment_results.json");
        ofstream(outfile);
        json::serializer serializer;
        serializer.serialize(all_results, outfile);
        
        cout << "\nExperiments completed successfully!" << endl;
        cout << "Results saved to experiment_results.json" << endl;
        
        return 0;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}
```

## Configuration File Examples

### Custom configuration (custom.cfg)

```ini
# Custom NLM configuration
random_seed = 12345
simulation_timestep = 0.0001
neuron_count = 5000
region_count = 3
connection_probability = 0.15

# Enhanced plasticity
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.01
stdp_time_constant = 15.0
plasticity_learning_rate = 0.02

# Development settings
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002

# Neuromodulation
dopamine_baseline = 0.1

# Memory settings
replay_interval = 50
consolidation_interval = 500

# Learning flags
enable_reward_modulation = true
enable_structural_plasticity = true
enable_development = true
enable_curiosity = true
enable_prediction = true

# Environment
environment_name = ComplexWorld
environment_width = 50
environment_height = 50

# Performance
performance_mode = memory-pools
simd_enabled = true
multithreading_enabled = true
```

## Python API Reference

### Quick Start Examples

```python
# 1. Basic Brain Creation and Simulation
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")

# 2. Complete Agent-World Simulation
import pynlm

def run_complete_simulation(steps=1000):
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(result.reward, 0.0)
        agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}: Reward={result.reward:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}")
    
    return brain, agent, world

brain, agent, world = run_complete_simulation()

# 3. Checkpointing and Persistence
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Save checkpoint
brain.save("brain_checkpoint.bin")

# ... run simulation ...

# Load checkpoint
brain.load("brain_checkpoint.bin")

# 4. Advanced Configuration
config = pynlm.createDefaultConfig()

# Set custom parameters
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# 5. Memory Inspection
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
prediction_system = brain.getPredictionSystem()

print(f"Working memory traces: {working_memory.getActiveTraces()}")
print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")

# 6. Development Tracking
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    print(f"Step {step}: Developmental stage = {stage}")

# 7. Neuromodulation Monitoring
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    # ... simulation steps ...
    
    dopamine = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    
    print(f"Neuromodulation - Dopamine: {dopamine:.2f}, "
          f"Curiosity: {curiosity:.2f}, Novelty: {novelty:.2f}, "
          f"Prediction Error: {prediction_error:.2f}")

# 8. Batch Processing
import pynlm
import time

def batch_simulations(num_simulations=10, steps_per_sim=500):
    results = []
    
    for sim in range(num_simulations):
        print(f"Running simulation {sim + 1}/{num_simulations}")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(steps_per_sim):
            brain.step(step)
        
        end_time = time.time()
        
        simulation_time = end_time - start_time
        
        results.append({
            'simulation_id': sim,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'simulation_time': simulation_time,
            'spikes_per_second': brain.getTotalSpikeCount() / simulation_time,
            'final_firing_rate': brain.getAverageFiringRate()
        })
        
        # Save intermediate results
        brain.save(f"batch_checkpoint_{sim}.bin")
    
    return results

batch_results = batch_simulations(5, 1000)
print(f"Batch simulation complete. Average spikes per second: {np.mean([r['spikes_per_second'] for r in batch_results]):.0f}")

# 9. Advanced Agent Features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Set goal and track progress
target_behavior = [1.0, 0.0, 0.0]  # Example goal
current_state = world.getSensoryPercept()

# Use neural planner if available
if auto* planner = brain.getPlanner()) {
    action = planner->planAction(current_state, 0.5);
} else {
    action = agent.decodeMotorCommand();
}

# 10. Performance Optimization
import pynlm
import pyperf

# Measure performance
config = pynlm.createDefaultConfig()

with pyperf.Timeit("brain.step"):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(1000):
        brain.step(step)

# Compare different configurations
configs_to_test = [
    {"neurons": 1000, "connections": 0.1},
    {"neurons": 5000, "connections": 0.05},
    {"neurons": 10000, "connections": 0.02}
]

for config_params in configs_to_test:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", config_params["neurons"])
    config.set("brain.connection_probability", config_params["connections"])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    with pyperf.Timeit(f"neurons={config_params['neurons']}"):
        for step in range(100):
            brain.step(step)
```

## Troubleshooting

### Common Issues and Solutions

#### "ImportError: No module named 'pynlm'"

**Solution:**

```bash
pip install --force-reinstall .
pip install -e .
```

#### "Segmentation fault when running simulations"

**Solution:**

```bash
# Check for memory issues
nlm --debug neurons

# Run with smaller network
nlm --steps 100 --neurons 100

# Check configuration
nlm --config your_config.cfg
```

#### "Learning not working properly"

**Solution:**

```bash
# Enable all learning systems
nlm --enable-learning reward curiosity structural development

# Check plasticity configuration
nlm --config custom.cfg
```

#### "Checkpoint save/load failing"

**Solution:**

```bash
# Check disk space
nlm --validate

# Try simpler checkpoint
nlm --checkpoint save simple_checkpoint.bin
```

## Advanced Configuration Parameters

### Core Parameters

- `random_seed`: Random seed for reproducibility
- `simulation_timestep`: Simulation time step (seconds)
- `neuron_count`: Total number of neurons
- `region_count`: Number of brain regions
- `connection_probability`: Synaptic connectivity probability

### Plasticity Parameters

- `plasticity_learning_rate`: Base learning rate
- `stdp_ltp_weight`: STDP long-term potentiation weight
- `stdp_ltd_weight`: STDP long-term depression weight
- `stdp_time_constant`: STDP time constant (ms)
- `synaptogenesis_rate`: Structural plasticity formation rate
- `pruning_rate`: Structural plasticity elimination rate

### Neuromodulation Parameters

- `dopamine_baseline`: Baseline dopamine level
- `dopamine_scale`: Dopamine modulation scale
- `curiosity_enable`: Enable curiosity-driven exploration
- `novelty_enable`: Enable novelty detection

### Memory Parameters

- `replay_interval`: Memory replay frequency
- `consolidation_interval`: Memory consolidation frequency
- `working_memory_capacity`: Working memory capacity
- `episodic_memory_max_episodes`: Maximum episodic memory episodes

### Performance Parameters

- `performance_mode`: Performance optimization mode
- `simd_enabled`: Enable SIMD acceleration
- `multithreading_enabled`: Enable multithreading
- `memory_pools_enabled`: Enable memory pools

## Building from Source

### Requirements

```bash
# C++ compiler with C++20 support
g++ 9+ or clang 12+ or MSVC 2019+

# Build tools
cmake >= 3.16
make or ninja

# Python and bindings
python 3.8+
pip
pybind11 >= 2.11.0
scikit-build-core >= 0.5.0
```

### Build Steps

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Configure build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release -- -j$(nproc)

# Install (optional)
cmake --install build --config Release
```

### Build Options

```bash
# Performance optimizations
cmake -DNLM_PERFORMANCE_ENABLED=ON

# SIMD acceleration
cmake -DNLM_SIMD_AVX2=ON

# Thread count
cmake -DNLM_THREAD_COUNT=4

# Custom installation prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local
```

## Python Package Development

### Development Installation

```bash
# Install in development mode
pip install -e .

# Install with all optional dependencies
pip install .[dev]

# Install specific build tools
pip install scikit-build-core pybind11 pytest numpy
```

### Building Python Package

```bash
# Build wheel
python -m build --wheel

# Build source distribution
python -m build --sdist

# Install from local build
pip install dist/*.whl
```

### Testing

```bash
# Run Python tests
pytest tests/ -v

# Run C++ tests
cd build && ctest --output-on-failure

# Run integration tests
pytest tests/integration/ -v
```

## Performance Monitoring

### Profiling

```bash
# Generate performance profiles
nlm --benchmark --profile cpu

# Memory profiling
valgrind --tool=massif nlm --steps 1000

# Call stack profiling
gprof nlm main.o gmon.out
```

### Metrics Collection

```python
# Performance monitoring script
import pynlm
import time
import psutil
import os

class PerformanceMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.metrics = {
            'cpu_usage': [],
            'memory_usage': [],
            'spike_rate': [],
            'firing_rate': []
        }
        self.start_time = time.time()
    
    def record_metrics(self):
        self.metrics['cpu_usage'].append(psutil.cpu_percent())
        self.metrics['memory_usage'].append(psutil.virtual_memory().percent)
        self.metrics['spike_rate'].append(self.brain.getTotalSpikeCount() / (time.time() - self.start_time))
        self.metrics['firing_rate'].append(self.brain.getAverageFiringRate())
    
    def get_summary(self):
        return {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_cpu': np.mean(self.metrics['cpu_usage']),
            'peak_memory': max(self.metrics['memory_usage']),
            'simulation_time': time.time() - self.start_time,
            'spikes_per_second': self.brain.getTotalSpikeCount() / (time.time() - self.start_time)
        }
```

## Future Enhancements

### Planned Features

1. **Advanced Agent Control**: Goal-directed behavior with hierarchical planning
2. **Multi-agent Systems**: Collaborative and competitive agent interactions
3. **Real-time Visualization**: Interactive neural network visualization
4. **Web Interface**: Browser-based control and monitoring
5. **Cloud Integration**: Distributed simulation across multiple machines
6. **Machine Learning Integration**: Training models using neural network dynamics
7. **Hardware Acceleration**: GPU support for massive scale simulations
8. **Sensorimotor Learning**: Active learning through environmental interaction
9. **Social Learning**: Cultural transmission and imitation
10. **Evolutionary Algorithms**: Evolution of neural architectures

### Research Directions

- Neuromodulation learning rules
- Memory consolidation mechanisms
- Predictive coding implementations
- Developmental trajectory modeling
- Scale-free network dynamics
- Energy-efficient neural computation
- Temporal pattern recognition
- Cross-modal integration

## License

MIT License. See LICENSE file for details.

## Acknowledgements

This project builds on decades of research in computational neuroscience,
neural network theory, and artificial intelligence. Special thanks to
contributors and researchers who have advanced the field of brain-inspired
computing.