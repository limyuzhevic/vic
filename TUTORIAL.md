# NLM Tutorial: Getting Started with Phase 4 Integration

## Introduction

Welcome to the NLM (Neural Learning Machine) tutorial! This guide will walk you through using the current Phase 4 implementation, which integrates all cognitive systems into a coherent artificial brain. The tutorial is designed for readers who want to understand how to:

- Create and run NLM simulations
- Use the AgentBrain interface to interact with environments
- Configure NLM for different experimental scenarios
- Analyze and interpret simulation results

## Prerequisites

Before starting, you should have:

- **Built NLM from source** (Phase 4 demo) or installed via pip
- **Basic understanding of neural networks** (spiking dynamics, synaptic plasticity)
- **Familiarity with C++/Python programming** (optional)
- **A text editor/IDE** for reading code examples

### Building NLM (C++ Version)

If you have the source code, build it first:

```bash
cd /path/to/nlm
catkin mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

Then run Phase 4 demo:
```bash
./nlm_phase4_demo
```

### Installing Python Bindings

For Python users, install the NLM Python package:

```bash
# From source (recommended)
pip install .

# Or install specific dependencies
pip install pybind11 scikit-build-core numpy
pip install -e .  # Development mode
```

Verify installation:
```python
import pynlm
print(f"NLM version: {pynlm.__version__}")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

## Phase 1: Brain Basics

### What is a Virtual Brain?

Think of NLM's brain as:

- **1,000+ neurons** spiking and learning
- **Hundreds of thousands of synapses** connecting them
- **Multiple brain regions** with specialized functions
- **Plasticity rules** modifying connections based on activity

### Creating Your First Brain

**Method 1: Using Python API** (Recommended for most users)

```python
# nlm_tutorial.py
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain (required before use)
brain.initialize()

# Run simulation
print("Starting simulation...")
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**Method 2: Using Command Line** (C++ version)

```bash
./nlm_phase4_demo 10000 --neurons=1000
```

### Exploring Brain Statistics

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get brain statistics
print("Brain Statistics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Currently firing: {brain.getFiringNeuronCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
```

## Phase 2: Adding Environment Interaction

### Understanding the Agent-Brain Interface

NLM's **Phase 3/4 integration** connects brains to environments through an **AgentBrain**:

1. **SensoryTransduction**: Converts world percepts → neural input
2. **MotorDecoding**: Converts neural activity → motor commands
3. **Neuromodulation**: Applies reward signals → plasticity changes
4. **Development**: Updates developmental stage based on experience

### Complete Agent Example

```python
# agent_tutorial.py
import pynlm

def run_experiment(num_steps=1000):
    """Complete Phase 3/4 experiment setup"""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)  # Larger brain for learning
    
    # 2. Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world (time passes)
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if world.getSensoryPercept().getInternal():
            reward = world.getSensoryPercept().getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            
            # Check agent's internal state
            body = agent.getBrainState()
            print(f"  Memory firing: {body.getMemoryFiring()}")
    
    print(f"\nExperiment complete after {num_steps} steps!")
    return brain, world, agent

# Run the experiment
if __name__ == "__main__":
    brain, world, agent = run_experiment(1000)
```

### Understanding the Simulation Loop

Each simulation step follows this sequence:

```
TIME PASSES → WORLD UPDATE → PERCEPT GENERATION
    ↓                           ↓
SENSORIAL PERCEPT          WORLD STATE
    ↓                           ↓
BRAIN INPUT → NEURAL PROCESSING → MOTOR OUTPUT
    ↓                           ↓
MOTOR COMMAND → WORLD ACTION → NEW PERCEPT
    ↓                           ↓
REWARD CALCULATION → NEUROMODULATION → PLASTICITY
    ↓                           ↓
DEVELOPMENT → BEHAVIOR CHANGE → NEXT CYCLE
```

## Phase 3: Configuration Exploration

### Brain Configuration Parameters

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Common configurations:

# 1. Small learning brain (fast, simple)
config.set("brain.neuron_count", 100)
config.set("plasticity_learning_rate", 0.05)
config.set("dopamine_baseline", 0.5)

# 2. Large developmental brain (slow, complex)
config.set("brain.neuron_count", 5000)
config.set("development_synaptogenesis_rate", 0.01)
config.set("development_pruning_rate", 0.001)

# 3. High-plasticity learning brain
config.set("plasticity_learning_rate", 0.1)
config.set("stdp_time_constant", 5.0)
config.set("visualization_enabled", True)

# Check and save configuration
print(f"Neuron count: {config.get('brain.neuron_count')}")
config.saveToFile("my_config.json")
```

### Configuration File Format

Configuration files use a simple key-value format:

```
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99
```

## Phase 4: Experiment Analysis

### Common Analysis Patterns

```python
import pynlm

def analyze_brain_performance(brain, agent, world, steps):
    """Analyze a complete simulation run"""
    
    # Track metrics over time
    metrics = {
        'firing_rate': [],
        'curiosity': [],
        'novelty': [],
        'prediction_error': [],
        'memory_usage': [],
        'developmental_stage': []
    }
    
    # You would run the simulation here and collect data
    # This is just an example of analysis patterns
    
    return metrics

def check_for_emergence(brain_stats_before, brain_stats_after):
    """Check if cognitive functions emerged"""
    
    # Look for patterns indicating emergence
    changes = {}
    
    # Check for working memory emergence (persistent activity)
    memory_firing_changed = (
        brain_stats_after['memory_firing'] > brain_stats_before['memory_firing']
    )
    changes['working_memory'] = memory_firing_changed
    
    # Check for prediction emergence (low prediction error)
    prediction_improved = (
        brain_stats_after['prediction_error'] < brain_stats_before['prediction_error']
    )
    changes['prediction'] = prediction_improved
    
    # Check for developmental progression
    development_advanced = (
        brain_stats_after['developmental_stage'] > brain_stats_before['developmental_stage']
    )
    changes['development'] = development_advanced
    
    return changes

def visualize_brain_activity(brain, num_steps=1000):
    """Generate visualizations of brain activity"""
    
    # Collect data
    spike_times = []
    firing_rates = []
    
    for step in range(num_steps):
        brain.step(step)
        spike_times.append(brain.getSpikeCountAtStep(step))
        firing_rates.append(brain.getFiringNeuronCount())
    
    # Create plots (you'd need matplotlib/seaborn)
    # This is pseudocode for analysis patterns
    
    return spike_times, firing_rates
```

## Phase 5: Advanced Topics

### Running Multiple Experiments

```python
import pynlm

def run_multiple_trials(config, num_trials=10, steps_per_trial=500):
    """Run multiple trials with different random seeds"""
    
    all_results = []
    
    for trial in range(num_trials):
        # Create new config for each trial
        trial_config = pynlm.createDefaultConfig()
        
        # Vary random seed for each trial
        trial_config.set("random_seed", 42 + trial)
        
        # Copy other settings from base config
        trial_config.set("brain.neuron_count", config.get("brain.neuron_count"))
        
        # Run trial
        brain = pynlm.createBrain(trial_config)
        brain.initialize()
        
        for step in range(steps_per_trial):
            brain.step(step)
        
        # Record results
        result = {
            'trial': trial,
            'seed': 42 + trial,
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'final_development': brain.getDevelopmentalStage()
        }
        
        all_results.append(result)
    
    return all_results

# Run experiment suite
results = run_multiple_trials(config, num_trials=20)
```

### Configuration Comparison

```python
def compare_configurations(config_list, steps=1000):
    """Compare performance across different configurations"""
    
    comparison = {}
    
    for i, config in enumerate(config_list):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
        
        comparison[f'config_{i}'] = {
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e/i_ratio': brain.getExcitationInhibitionRatio(),
            'development': brain.getDevelopmentalStage(),
            'neurons_firing': brain.getFiringNeuronCount()
        }
    
    return comparison

# Compare different config approaches
configs = [
    small_config,
    medium_config,
    large_config
]

results = compare_configurations(configs)
```

## Phase 6: Troubleshooting

### Common Issues and Solutions

**Issue: "My brain isn't doing anything"**
- **Solution**: Always call `brain.initialize()` before running steps
- **Example**: Make sure you have `brain.initialize()` in your code

**Issue: "The agent isn't moving"**
- **Solution**: Ensure `world.update()` is called and `world.applyMotorCommand()` is connected
- **Debug**: Check that motor commands are being generated

**Issue: "Everything is 0"**
- **Solution**: Brains need time to "warm up" - try more steps
- **Tip**: Some neurons need input to fire - ensure sensory input is connected

**Issue: Memory usage is too high**
- **Solution**: Reduce `neuron_count` or enable more aggressive pruning
- **Config**: Try `development_pruning_rate = 0.01`

### Error Messages

```python
import pynlm

# Enable error checking
config = pynlm.createDefaultConfig()

# Try operations that might fail
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
    print("Success!")
except Exception as e:
    print(f"Error: {e}")
    # Handle specific error types
    if "initialize" in str(e).lower():
        print("Did you forget to call brain.initialize()?")
```

## Phase 7: Project Extensions

### Creating Custom Worlds

```python
# You can extend the SimpleWorld class for custom environments
import pynlm

class CustomWorld(pynlm.SimpleWorld):
    def __init__(self):
        super().__init__()
    
    def createCustomEnvironment(self):
        # Add your custom environment logic
        pass
    
    def customInteractionLogic(self, agent):
        # Define how your world interacts with agents
        pass

# Usage
custom_world = CustomWorld()
```

### Developing New Plasticity Rules

NLM's plasticity system is extensible:

```cpp
// Example of creating a new plasticity rule
class MyPlasticityRule : public PlasticityRule {
public:
    void apply(NeuronId pre, NeuronId post, double time_diff) override {
        // Your custom learning rule
        double weight_change = calculate_weight_change(pre, post, time_diff);
        update_synapse_weight(pre, post, weight_change);
    }
};
```

## Further Reading

### Documentation Resources

1. **[CONFIG_REFERENCE.md](CONFIG_REFERENCE.md)** - Complete configuration parameter documentation
2. **[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)** - Technical system architecture
3. **[docs/SCIENCE.md](docs/SCIENCE.md)** - Scientific background and biological inspiration
4. **[docs/EXPERIMENTS.md](docs/EXPERIMENTS.md)** - Detailed experiment descriptions and procedures

### Community Resources

- **NLM GitHub Repository**: Check for updates and contributions
- **Phase 6 Documentation**: The latest integration documentation
- **Performance Documentation**: Scaling and optimization guides
- **Limitations Documentation**: Understanding what NLM cannot do

## Conclusion

You've completed the NLM tutorial! You now know how to:

✅ Create and initialize virtual brains
✅ Run simulations with environment interaction
✅ Configure NLM for different experimental scenarios
✅ Analyze brain activity and performance
✅ Troubleshoot common issues
✅ Extend NLM for custom applications

## Next Steps

1. **Try the built-in demos**: Run `./nlm_phase4_demo` for a complete demonstration
2. **Explore the experiments**: Try different Phase 4 experiment configurations
3. **Create your own projects**: Build custom environments and agents
4. **Contribute to NLM**: Add documentation, fix bugs, or extend functionality
5. **Join the research**: Publish findings from your NLM experiments

Remember: NLM is a research platform for investigating brain-inspired computation, not a commercial AI system. The goal is to understand principles of neural computation, not to create human-like intelligence.

Happy experimenting with your artificial brain!

