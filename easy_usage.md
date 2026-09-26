# NLM — Advanced Usage Guide for Experienced Users

**Note**: This guide assumes familiarity with the basic NLM concepts from `easy_usage.md`. If you're new to NLM, start with the beginner guide first.

## Quick Overview

NLM (熙然) is a **brain simulator** designed to research computational brain-like systems. Unlike traditional machine learning, NLM implements **real neural computation** with:
- **LIF neurons** with spiking dynamics
- **Event-driven spike propagation** 
- **Neuromodulated plasticity** (STDP, Hebbian, reward-modulated)
- **Developmental** maturation and stage progression
- **Memory systems** (working, episodic, associative)
- **Predictive cognition** (sequence learning, planning)
- **Attention mechanisms** (competitive selection)
- **Social learning** (observation/imitation)

## Core Architecture

### The Three-Layer Interface

NLM provides three levels of access to its neural computation engine:

1. **Direct C++ API**: For maximum performance and control
2. **PyNLM Python Bindings**: For research and experimentation
3. **Agent-Brain Interface**: For embodied agents in simulated worlds

### Key Components

| Component | Purpose | Access Level |
|-----------|---------|--------------|
| `Brain` | Central neural simulation | C++, Python, Agent |
| `SimpleWorld` | 2D simulation environment | Python, Agent |
| `AgentBrain` | Bridge between brain and world | Python, Agent |
| `Config` | Configuration system | C++, Python |

## Advanced Configuration

### Custom Brain Configurations

```python
import pynlm

# Load configuration from JSON file
config = pynlm.createDefaultConfig()
config.loadFromFile("configs/experiment.cfg")

# Modify configuration programmatically
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)
config.set("synaptogenesis_rate", 0.0002)
config.set("pruning_rate", 0.00005)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
```

### Advanced World Configuration

```python
import pynlm

# Create sophisticated world setup
world = pynlm.createSimpleWorld()
world.configure(
    width=100.0,           # Larger world
    height=100.0,
    visionWidth=16,       # Higher resolution vision
    visionHeight=16
)

# Configure agent properties
world.setAgentStart(50.0, 50.0)  # Center start position

# Adjust energy and decay
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.05)
```

## Memory Management

### Checkpointing and Persistence

NLM supports saving and loading brain states for long-running experiments:

```python
import pynlm

# Save brain state (checkpointing)
brain.save("checkpoints/experiment1_checkpoint.nlm")

# Load brain state
brain.load("checkpoints/experiment1_checkpoint.nlm")

# Use in long-running experiments
for step in range(10000):
    # ... simulation loop
    if step % 1000 == 0:
        brain.save(f"checkpoints/checkpoint_{step}.nlm")
```

### Memory Monitoring

```python
# Get brain statistics for monitoring
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

## Advanced Learning Patterns

### Multi-Stage Learning

Implement learning across developmental stages:

```python
def run_developmental_experiment(brain, world, agent, duration):
    stage_durations = [60.0, 300.0, 900.0, float('inf')]  # Initial, Critical, Maturation, Adult
    
    for step in range(duration):
        # Update development based on time
        agent.updateDevelopment(0.1)  # 0.1 second per step
        
        # World interaction
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action and learning
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply rewards based on stage
        if agent.getDevelopmentalStage() == DevelopmentalStage.Initial:
            reward = 1.0  # Encourage exploration
        elif agent.getDevelopmentalStage() == DevelopmentalStage.CriticalPeriod:
            reward = 0.5  # Balance exploration/exploitation
        else:
            reward = 0.1  # Encourage refinement
            
        agent.applyRewardModulation(reward, agent.getPredictionError())
```

### Reward-Modulated Learning

Enable sophisticated reward-based learning:

```python
# Create agent with all learning systems enabled
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run reinforcement learning experiment
for step in range(5000):
    # Agent explores and learns from consequences
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate prediction error for learning
    predicted = calculate_reward_prediction(action, world.getAgentBody())
    actual = result.reward
    
    agent.applyRewardModulation(actual, predicted)
```

## Performance Optimization

### Batch Processing

For processing multiple brains or worlds efficiently:

```python
def batch_simulation(brain_configs, world_configs, steps_per_brain):
    """Run multiple simulations in parallel"""
    import concurrent.futures
    
    def simulate_brain(config_data):
        brain_config, world_config = config_data
        
        brain = pynlm.createBrain(brain_config)
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for step in range(steps_per_brain):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        return brain.getTotalSpikeCount()
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
        configs = [(pynlm.createDefaultConfig(), w) for w in world_configs]
        results = list(executor.map(simulate_brain, configs))
    
    return results
```

### Memory-Efficient Processing

For large-scale simulations with memory constraints:

```python
def memory_efficient_simulation(brain, world, agent, total_steps, checkpoint_interval):
    """Simulate with periodic checkpointing to manage memory"""
    for step in range(total_steps):
        # Process one step at a time
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Periodic checkpointing
        if step % checkpoint_interval == 0:
            checkpoint_file = f"checkpoints/efficient_checkpoint_{step}.nlm"
            brain.save(checkpoint_file)
            
            # Optional: Load if memory becomes an issue
            if step % (checkpoint_interval * 10) == 0:
                # Keep only most recent checkpoint
                import os
                checkpoints = sorted([f for f in os.listdir('checkpoints') 
                                    if f.endswith('.nlm')])
                if len(checkpoints) > 5:
                    os.remove(os.path.join('checkpoints', checkpoints[0]))
```

## Debugging and Development

### Logging and Monitoring

Configure detailed logging for development:

```python
import pynlm
from pynlm import Logger, LogLevel

# Create custom logger
logger = Logger()
console_logger = Logger.setGlobal(consoleLogger)

# Add file logging
file_logger = Logger()
file_logger.setLevel(LogLevel.Debug)
file_logger.setOutputFile("simulation.log")

logger.addLogger(file_logger)

# Run simulation with detailed logging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        brain.logStatus()
```

### Visualization Integration

For research visualization, integrate with NLM's visualization capabilities:

```python
import pynlm

# Enable visualization in your research code
# (Assuming visualization is available in your build)
# This depends on how visualization is integrated

def visualize_brain_state(brain, step):
    """Visualize current brain state"""
    # This would depend on your visualization setup
    # Example for research code:
    
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
    print(f"  Weight stats: mean={calculate_mean_weight(brain)}")
    
    # Visualize connectivity pattern
    # visualize_connectivity_matrix(brain)
    
    # Visualize neural activity
    # visualize_neural_activity(brain)
    
    return {
        'step': step,
        'spike_count': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'plasticity_events': count_plasticity_events(brain)
    }
```

## Experiment Framework

### Custom Experiment Patterns

Create sophisticated research experiments:

```python
import pynlm
from pynlm import DevelopmentalStage

class NLMExperiment:
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        self.results = []
    
    def setup(self, config_type='default'):
        """Setup experiment with specific configuration"""
        if config_type == 'development':
            config = self.create_development_config()
        elif config_type == 'curiosity':
            config = self.create_curiosity_config()
        elif config_type == 'social':
            config = self.create_social_config()
        else:
            config = pynlm.createDefaultConfig()
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
        self.world.setAgentStart(25, 25)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def create_development_config(self):
        """Create configuration optimized for developmental research"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("region_count", 2)
        config.set("stdp_ltp_weight", 0.03)
        config.set("stdp_ltd_weight", 0.025)
        config.set("synaptogenesis_rate", 0.0005)
        config.set("pruning_rate", 0.00002)
        config.set("random_seed", 12345)  # Reproducible results
        return config
    
    def create_curiosity_config(self):
        """Create configuration optimized for curiosity-driven learning"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 300)
        config.set("region_count", 1)
        config.set("stdp_ltp_weight", 0.015)
        config.set("stdp_ltd_weight", 0.012)
        config.set("synaptogenesis_rate", 0.0001)
        config.set("curiosity_threshold", 0.8)
        return config
    
    def run_phase(self, duration, phase_name):
        """Run a specific phase of the experiment"""
        stage_durations = {
            'exploration': 60.0,
            'consolidation': 300.0,
            'refinement': 600.0
        }
        
        for step in range(duration):
            # Update developmental stage
            self.agent.updateDevelopment(0.1)
            
            # World interaction
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.agent.applyRewardModulation(0.1, 0.0)  # Mild reward for exploration
            self.brain.step(step)
            
            # Action
            action = self.agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Record results
            self.record_phase_data(step, action, result)
            
            if step % 100 == 0:
                print(f"{phase_name} step {step}: {brain.getFiringNeuronCount()} firing")
    
    def record_phase_data(self, step, action, result):
        """Record experiment data"""
        data = {
            'step': step,
            'action': action,
            'reward': result.reward,
            'success': result.success,
            'message': result.message,
            'neurons_firing': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'developmental_stage': self.agent.getDevelopmentalStage(),
            'neuromodulation': self.agent.getNeuromodulationLevel(),
            'curiosity': self.agent.getCuriosityLevel()
        }
        self.results.append(data)
    
    def save_results(self, filename):
        """Save experiment results"""
        import json
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2, default=str)
    
    def get_summary(self):
        """Get experiment summary statistics"""
        if not self.results:
            return None
        
        return {
            'total_steps': len(self.results),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'final_stage': self.agent.getDevelopmentalStage(),
            'avg_reward': sum(r['reward'] for r in self.results) / len(self.results),
            'success_rate': sum(1 for r in self.results if r['success']) / len(self.results),
            'final_curiosity': self.agent.getCuriosityLevel()
        }
```

## Research Integration

### Reproducible Research

Make your NLM research reproducible:

```python
import hashlib
import json
import pynlm

class ResearchExperiment:
    def __init__(self, experiment_name):
        self.name = experiment_name
        self.config = pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
    
    def setup_experiment(self, config_overrides=None):
        """Setup with configuration for reproducibility"""
        
        # Set deterministic seed
        seed = 42  # For reproducibility
        self.config.set("random_seed", seed)
        
        # Apply custom configuration
        if config_overrides:
            for key, value in config_overrides.items():
                self.config.set(key, value)
        
        # Create components
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def get_experiment_signature(self):
        """Get unique signature for experiment reproducibility"""
        config_dict = self.config.getKeys()
        signature_data = {
            'name': self.name,
            'config': config_dict,
            'timestamp': self.get_timestamp()
        }
        return hashlib.md5(json.dumps(signature_data, sort_keys=True).encode()).hexdigest()
    
    def save_experiment_state(self, path):
        """Save complete experiment state"""
        import pickle
        
        state = {
            'name': self.name,
            'config': self.config,
            'brain_state': self.brain.save_to_dict(),  # If available
            'world_state': self.get_world_state(),
            'timestamp': self.get_timestamp()
        }
        
        with open(path, 'wb') as f:
            pickle.dump(state, f)
    
    def load_experiment_state(self, path):
        """Load experiment state"""
        import pickle
        
        with open(path, 'rb') as f:
            state = pickle.load(f)
        
        self.name = state['name']
        self.config = state['config']
        self.brain = pynlm.createBrain(self.config)
        # Load brain state from saved data
        # Load world state from saved data
        
        return self
```

## Performance Monitoring

### System Resource Tracking

Monitor your NLM simulations:

```python
import time
import psutil
import pynlm

class PerformanceMonitor:
    def __init__(self):
        self.start_time = None
        self.samples = []
    
    def start_monitoring(self):
        """Start performance monitoring"""
        self.start_time = time.time()
        self.samples = []
    
    def sample(self, brain, world, agent, step):
        """Take performance sample"""
        process = psutil.Process()
        
        sample = {
            'timestamp': time.time() - self.start_time,
            'step': step,
            'cpu_percent': process.cpu_percent(),
            'memory_mb': process.memory_info().rss / 1024 / 1024,
            'neurons': brain.getTotalNeuronCount(),
            'synapses': brain.getTotalSynapseCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'simulation_time': world.getSimulationTime()
        }
        
        self.samples.append(sample)
        return sample
    
    def get_performance_report(self):
        """Get performance statistics"""
        if not self.samples:
            return None
        
        times = [s['timestamp'] for s in self.samples]
        cps = [s['spike_count'] for s in self.samples]
        
        return {
            'total_time': times[-1],
            'total_steps': len(self.samples),
            'steps_per_second': len(self.samples) / times[-1],
            'avg_cpu': sum(s['cpu_percent'] for s in self.samples) / len(self.samples),
            'max_cpu': max(s['cpu_percent'] for s in self.samples),
            'avg_memory_mb': sum(s['memory_mb'] for s in self.samples) / len(self.samples),
            'peak_memory_mb': max(s['memory_mb'] for s in self.samples),
            'avg_spike_rate': sum(cps[i] - cps[i-1] for i in range(1, len(cps))) / (len(cps) - 1) if len(cps) > 1 else 0,
            'final_spike_count': cps[-1]
        }
```

## Advanced Features

### Custom Action Types

Extend NLM with custom actions for research:

```python
# This would require extending the Action class
# and modifying the agent brain to decode custom patterns

class CustomAction(pynlm.Action):
    def __init__(self, action_type, parameters):
        super().__init__(action_type, parameters)
    
    def getSpecializedParameter(self, param_name):
        """Get custom parameter specific to action type"""
        return self.getParameters().get(param_name, 0.0)

# In research code where you need custom actions
# agent.enableCustomActionTypes(True)
```

### Multi-Agent Systems

Scale to multiple interacting agents:

```python
def run_multi_agent_experiment(agent_configs, world_config):
    """Run multiple interacting agents in the same world"""
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    
    agents = []
    brains = []
    
    for config in agent_configs:
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agents.append(agent)
        brains.append(brain)
    
    # Run multi-agent simulation
    for step in range(1000):
        world.update(0.1)
        
        # Each agent perceives the world
        for agent in agents:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        
        # All brains compute simultaneously
        for brain in brains:
            brain.step(step)
        
        # Agents decide and act
        for agent, brain in zip(agents, brains):
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    return agents, brains
```

## Troubleshooting Common Issues

### Performance Issues

```python
# Monitor and optimize performance

def diagnose_performance_issues(brain, world, agent):
    """Diagnose common performance issues"""
    
    # Check if brain is too large for memory
    neuron_count = brain.getTotalNeuronCount()
    synapse_count = brain.getTotalSynapseCount()
    
    print(f"Brain size: {neuron_count} neurons, {synapse_count} synapses")
    
    if neuron_count > 10000:
        print("WARNING: Large brain may cause memory issues")
    
    # Check for runaway activity
    spike_rate = brain.getAverageFiringRate()
    if spike_rate > 100:  # Hz
        print("WARNING: High firing rate may cause instability")
    
    # Check developmental stage
    stage = agent.getDevelopmentalStage()
    if stage == DevelopmentalStage.Aging:
        print("INFO: Brain in adult stage - plasticity reduced")
    
    # Check memory usage
    import psutil
    process = psutil.Process()
    memory_mb = process.memory_info().rss / 1024 / 1024
    print(f"Memory usage: {memory_mb:.1f} MB")
    
    if memory_mb > 2000:  # 2GB
        print("WARNING: High memory usage - consider checkpointing")
```

### Debugging Techniques

```python
def debug_simulation(brain, world, agent, step):
    """Debug simulation state"""
    
    # Check neural activity
    neurons_firing = brain.getFiringNeuronCount()
    total_spikes = brain.getTotalSpikeCount()
    
    print(f"Step {step}: {neurons_firing} neurons firing, {total_spikes} total spikes")
    
    # Check action selection
    action = agent.decodeMotorCommand()
    print(f"Selected action: {action}")
    
    # Check neuromodulation state
    neuromod = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    error = agent.getPredictionError()
    
    print(f"Neuromodulation: dopamine={neuromod:.3f}, curiosity={curiosity:.3f}, novelty={novelty:.3f}")
    print(f"Prediction error: {error:.3f}")
    
    # Check developmental stage
    stage = agent.getDevelopmentalStage()
    print(f"Developmental stage: {stage}")
    
    # Check brain connectivity
    region_count = brain.getRegionCount()
    print(f"Brain regions: {region_count}")
    
    # Check for potential issues
    if step > 100 and total_spikes == 0:
        print("WARNING: No spikes generated after initial steps")
    
    if neuromod > 0.9:
        print("INFO: High dopamine level - strong learning signal")
    
    if curiosity < 0.1:
        print("INFO: Low curiosity - agent may be settling into routine")
```

## Research Best Practices

### Reproducibility

1. **Always save configuration**: Save your `config.getKeys()` for complete reproducibility
2. **Use deterministic seeds**: Always set a random seed for scientific experiments
3. **Document hyperparameters**: Track all configuration parameters used
4. **Version control**: Store brain checkpoints and experiment data

### Experiment Design

1. **Include control conditions**: Run baseline experiments without key features
2. **Vary parameters systematically**: Test different configurations
3. **Include statistical analysis**: Run multiple trials and analyze variance
4. **Document all steps**: Keep detailed logs of experimental procedures

### Data Management

1. **Use structured formats**: Save results in JSON, CSV, or pickle formats
2. **Include metadata**: Store configuration and experimental metadata
3. **Back up regularly**: Use checkpointing for long-running experiments
4. **Organize by experiment**: Use clear directory structures

This advanced usage guide provides comprehensive information for researchers and advanced users working with NLM. The examples demonstrate sophisticated techniques for conducting research, optimizing performance, and ensuring reproducibility.