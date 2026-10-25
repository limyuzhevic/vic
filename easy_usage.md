# NLM - Advanced Usage Guide for Power Users

This guide covers advanced features and configurations for experienced NLM users.

## 1. Advanced Brain Configuration

### Custom Neuron Parameters
```python
import pynlm

# Create custom configuration with advanced neuron parameters
config = pynlm.createDefaultConfig()

# Configure neuron dynamics
config.set("brain.v_thresh", -45.0)    # Higher threshold
config.set("brain.tau_mem", 25.0)     # Slower membrane time constant
config.set("brain.v_reset", -76.0)    # More negative reset
config.set("brain.tau_ref", 3.0)      # Longer refractory period

# Configure synaptic parameters
config.set("brain.initial_weight_mean", 0.3)
config.set("brain.initial_weight_std", 0.05)
config.set("synaptic_delay", 10.0)

# Create brain with custom parameters
brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Plasticity Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()

# Configure STDP with custom parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.02)
config.set("plasticity.stdp.tau_plus", 30.0)
config.set("plasticity.stdp.tau_minus", 30.0)
config.set("plasticity.stdp.max_weight", 2.0)
config.set("plasticity.stdp.min_weight", -2.0)

# Configure Hebbian learning with custom parameters
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.005)

# Configure structural plasticity
config.set("plasticity.structural.enable", True)
config.set("synaptogenesis_rate", 0.0005)
config.set("pruning_rate", 0.00005)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Neuromodulation Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()

# Configure dopamine neuromodulation
config.set("neuromod.dopamine.baseline", 0.2)
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.dopamine.decay_rate", 0.2)

# Configure curiosity with custom parameters
config.set("neuromod.curiosity.enable", True)
config.set("agent.curiosity_threshold", 0.5)
config.set("agent.noise_level", 0.2)

# Configure novelty detection
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.7)

# Configure prediction error
config.set("neuromod.prediction.error.scale", 1.0)

brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
brain.initialize()
agent.initialize(world)
```

## 2. Memory System Configuration

### Working Memory Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure working memory
if brain.getWorkingMemory():
    brain.getWorkingMemory().setCapacity(500)  # Large capacity
    brain.getWorkingMemory().setConsolidationThreshold(0.8)
    brain.getWorkingMemory().enableReplay(True)
```

### Episodic Memory Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure episodic memory
if brain.getEpisodicMemory():
    brain.getEpisodicMemory().setMaxEpisodes(5000)  # Store more episodes
    brain.getEpisodicMemory().setReplayInterval(50)   # More frequent replay
    brain.getEpisodicMemory().setConsolidationRate(0.5)
```

## 3. Development System Configuration

### Custom Developmental Stages
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure development system
if brain.getDevelopmentSystem():
    brain.getDevelopmentSystem().enable(True)
    
    # Modify developmental stage parameters
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage::CriticalPeriod)
    
    # Configure stage-specific plasticity rates
    brain.setPlasticityRate(0.9)  # High plasticity during critical period
```

## 4. Advanced Simulation Control

### Custom Simulation Loop with Advanced Features
```python
import pynlm

def advanced_simulation(brain, world, agent, num_steps=5000):
    """Advanced simulation with multiple features enabled"""
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Configure advanced agent behavior
    agent.setCuriosityThreshold(0.4)
    agent.setExplorationNoise(0.15)
    agent.setActionSelectionTemperature(1.2)
    
    # Run simulation with detailed monitoring
    metrics = {
        'total_reward': 0.0,
        'max_firing_rate': 0.0,
        'memory_usage': 0,
        'learning_events': 0,
        'novelty_detected': 0
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development periodically
        if step % 100 == 0:
            agent.updateDevelopment(0.1)
        
        # Collect metrics
        metrics['total_reward'] += reward
        metrics['max_firing_rate'] = max(metrics['max_firing_rate'], brain.getAverageFiringRate())
        
        if brain.getWorkingMemory():
            metrics['memory_usage'] = brain.getWorkingMemory().getActiveTraces()
        
        if brain.getEpisodicMemory():
            metrics['learning_events'] = brain.getEpisodicMemory().getEpisodeCount()
        
        if agent.getNoveltyLevel() > 0.5:
            metrics['novelty_detected'] += 1
        
        # Print progress every 500 steps
        if step % 500 == 0:
            print(f"Step {step}: "
                  f"Reward={metrics['total_reward']/(step+1):.3f}, "
                  f"Firing={brain.getAverageFiringRate():.3f}, "
                  f"Memory={metrics['memory_usage']}, "
                  f"Novelty={metrics['novelty_detected']}")
    
    return metrics
```

## 5. Configuration File Management

### Create Custom Configuration File
```python
import pynlm

# Create custom configuration
def create_custom_config(filename):
    config = pynlm.createDefaultConfig()
    
    # Custom neuron configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.v_thresh", -40.0)
    config.set("brain.tau_mem", 25.0)
    
    # Custom plasticity
    config.set("plasticity.stdp.learning_rate", 0.015)
    config.set("plasticity.hebbian.learning_rate", 0.002)
    
    # Custom neuromodulation
    config.set("neuromod.curiosity.scale", 1.5)
    config.set("agent.curiosity_threshold", 0.2)
    
    # Custom memory
    config.set("memory.working.capacity", 200)
    config.set("memory.episodic.max_episodes", 2000)
    
    # Save to file
    config.saveToFile(filename)
    print(f"Configuration saved to {filename}")

# Usage
create_custom_config("my_config.cfg")
```

### Load Configuration from File
```python
import pynlm

# Load custom configuration
config = pynlm.createDefaultConfig()
if config.loadFromFile("my_config.cfg"):
    print("Configuration loaded successfully")
    
    # Modify loaded configuration
    config.set("brain.neuron_count", 2000)
    
    # Create brain with custom configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
else:
    print("Failed to load configuration, using defaults")
```

## 6. Performance Monitoring and Optimization

### Advanced Monitoring
```python
import pynlm
import time
import psutil
import os

def monitor_performance(brain, world, agent, duration_seconds=300):
    """Monitor NLM performance during simulation"""
    
    process = psutil.Process(os.getpid())
    start_time = time.time()
    
    metrics = {
        'cpu_usage': [],
        'memory_usage': [],
        'simulation_steps': 0,
        'spike_count': 0,
        'learning_events': 0
    }
    
    step = 0
    while time.time() - start_time < duration_seconds:
        # Monitor system resources
        metrics['cpu_usage'].append(process.cpu_percent())
        metrics['memory_usage'].append(process.memory_info().rss / (1024 * 1024))  # MB
        
        # Run simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update metrics
        metrics['simulation_steps'] += 1
        metrics['spike_count'] += brain.getTotalSpikeCount()
        
        step += 1
        
        # Print performance report every 1000 steps
        if step % 1000 == 0:
            avg_cpu = sum(metrics['cpu_usage']) / len(metrics['cpu_usage'])
            avg_memory = sum(metrics['memory_usage']) / len(metrics['memory_usage'])
            
            print(f"Performance at step {step}:")
            print(f"  CPU usage: {avg_cpu:.1f}%")
            print(f"  Memory usage: {avg_memory:.1f} MB")
            print(f"  Steps/sec: {step / (time.time() - start_time):.1f}")
            print(f"  Total spikes: {metrics['spike_count']}")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print()
            
            # Reset metrics for next period
            metrics['cpu_usage'].clear()
            metrics['memory_usage'].clear()
            start_period = time.time()
    
    return metrics
```

## 7. Advanced Experimentation

### Multi-Environment Testing
```python
import pynlm

class AdvancedExperiment:
    def __init__(self, config_file=None):
        self.configs = {}
        self.results = []
        
        if config_file:
            config = pynlm.createDefaultConfig()
            config.loadFromFile(config_file)
            self.configs['default'] = config
        
        # Create additional configurations
        self.create_experimental_configs()
    
    def create_experimental_configs(self):
        """Create multiple configurations for experimentation"""
        
        # Configuration 1: High plasticity
        config1 = pynlm.createDefaultConfig()
        config1.set("plasticity.stdp.learning_rate", 0.03)
        config1.set("plasticity.hebbian.learning_rate", 0.01)
        config1.set("synaptogenesis_rate", 0.001)
        config1.set("neuromod.curiosity.scale", 2.0)
        self.configs['high_plasticity'] = config1
        
        # Configuration 2: Low plasticity
        config2 = pynlm.createDefaultConfig()
        config2.set("plasticity.stdp.learning_rate", 0.005)
        config2.set("plasticity.hebbian.learning_rate", 0.0001)
        config2.set("synaptogenesis_rate", 0.00001)
        config2.set("neuromod.curiosity.scale", 0.5)
        self.configs['low_plasticity'] = config2
        
        # Configuration 3: Development-focused
        config3 = pynlm.createDefaultConfig()
        config3.set("development.stage.critical_period.plasticity_rate", 1.5)
        config3.set("development.stage.maturation.plasticity_rate", 0.8)
        config3.set("memory.episodic.max_episodes", 5000)
        self.configs['development_focused'] = config3
    
    def run_experiment(self, config_name, steps=2000):
        """Run experiment with specified configuration"""
        if config_name not in self.configs:
            raise ValueError(f"Unknown configuration: {config_name}")
        
        config = self.configs[config_name]
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize all systems
        brain.initialize()
        agent.initialize(world)
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run experiment
        experiment_data = {
            'config': config_name,
            'steps': steps,
            'total_reward': 0.0,
            'avg_firing_rate': 0.0,
            'memory_episodes': 0,
            'novelty_events': 0
        }
        
        for step in range(steps):
            # Run simulation step
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect data
            experiment_data['total_reward'] += reward
            experiment_data['avg_firing_rate'] += brain.getAverageFiringRate()
            
            if brain.getEpisodicMemory():
                experiment_data['memory_episodes'] = brain.getEpisodicMemory().getEpisodeCount()
            
            if agent.getNoveltyLevel() > 0.5:
                experiment_data['novelty_events'] += 1
        
        # Calculate averages
        experiment_data['avg_firing_rate'] /= steps
        experiment_data['avg_reward'] = experiment_data['total_reward'] / steps
        
        self.results.append(experiment_data)
        return experiment_data
    
    def compare_results(self):
        """Compare results across all experiments"""
        print("=== Experiment Comparison ===")
        for i, result in enumerate(self.results):
            print(f"\nExperiment {i+1}: {result['config']}")
            print(f"  Average Reward: {result['avg_reward']:.3f}")
            print(f"  Average Firing Rate: {result['avg_firing_rate']:.3f}")
            print(f"  Memory Episodes: {result['memory_episodes']}")
            print(f"  Novelty Events: {result['novelty_events']}")
    
    def save_results(self, filename):
        """Save experimental results to file"""
        import json
        
        # Convert results to JSON-serializable format
        serializable_results = []
        for result in self.results:
            serializable_results.append(dict(result))
        
        with open(filename, 'w') as f:
            json.dump(serializable_results, f, indent=2)
        
        print(f"Results saved to {filename}")

# Usage example
if __name__ == "__main__":
    # Create advanced experiment
    experiment = AdvancedExperiment("my_config.cfg")
    
    # Run multiple experiments
    results1 = experiment.run_experiment('high_plasticity', steps=1000)
    results2 = experiment.run_experiment('low_plasticity', steps=1000)
    results3 = experiment.run_experiment('development_focused', steps=1000)
    
    # Compare results
    experiment.compare_results()
    
    # Save results
    experiment.save_results("experiment_results.json")
```

## 8. API Reference for Advanced Users

### Core Brain Class Methods
```python
# Configuration access
brain.getConfig().get("brain.neuron_count")
brain.getConfig().getOr("brain.v_thresh", -50.0)

# Neuron control
brain.getNeuron(NeuronId(123))
brain.getNeuronsOfType(NeuronType::Sensory)
brain.getActiveNeurons()

# Synaptic control
brain.getSynapse(SynapseId(456))
brain.getSynapsesFromNeuron(NeuronId(123))
brain.getSynapsesToNeuron(NeuronId(456))

# Memory system access
brain.getWorkingMemory()
brain.getEpisodicMemory()
brain.getAssociativeMemory()

# Neuromodulation access
brain.getDopamine()
brain.getCuriosity()
brain.getPredictionErrorSignal()

# Development access
brain.getDevelopmentSystem()
brain.getDevelopmentalStage()

# Prediction system access
brain.getPredictionSystem()

# Cognition system access
brain.getPlanner()
brain.getConceptFormation()
brain.getAttention()
```

### AgentBrain Class Methods
```python
# Agent initialization
agent.initialize(world)
agent.setBrain(brain)  # Alternative initialization

# Sensory processing
agent.processSensoryInput(percept)
agent.getPerceptBuffer()
agent.clearPerceptBuffer()

# Motor control
agent.decodeMotorCommand()
agent.setActionSelectionPolicy(policy)

# Neuromodulation
agent.enableRewardModulation(enable)
agent.enableCuriosity(enable)
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.enableDevelopment(enable)
agent.updateDevelopment(timestep)

# Configuration
agent.setCuriosityThreshold(threshold)
agent.setExplorationNoise(noise)
agent.setActionSelectionTemperature(temp)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### SimpleWorld Class Methods
```python
# World configuration
world.configure(width, height, visionWidth, visionHeight)
world.setAgentStart(x, y)
world.setRandomSeed(seed)

# Control
world.reset()
world.update(timestep)

# Object management
world.addObject(obj)
world.removeObject(x, y)
world.isValidPosition(x, y)

# Agent interaction
world.applyMotorCommand(action, time)
world.getSensoryPercept()
world.getAgentBody()
world.computeReward(brain)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(value)
```

## 9. Error Handling and Diagnostics

### Advanced Error Handling
```python
import pynlm

def safe_brain_initialization(config):
    """Initialize brain with comprehensive error handling"""
    try:
        brain = pynlm.createBrain(config)
        
        # Validate configuration before initialization
        if not validate_config(config):
            raise ConfigurationError("Invalid configuration detected")
        
        # Initialize with error handling
        if not brain.initialize():
            raise InitializationError("Brain initialization failed")
        
        # Verify all systems are working
        if not verify_systems(brain):
            raise SystemIntegrationError("Some brain systems failed to initialize")
        
        return brain
        
    except ConfigurationError as e:
        print(f"Configuration error: {e}")
        return None
    except InitializationError as e:
        print(f"Initialization error: {e}")
        return None
    except SystemIntegrationError as e:
        print(f"System integration error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

def validate_config(config):
    """Validate configuration parameters"""
    
    # Check required parameters
    required_params = ["brain.neuron_count", "region_count"]
    for param in required_params:
        if not config.has(param):
            print(f"Missing required parameter: {param}")
            return False
    
    # Check parameter ranges
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 10:
        print("Warning: Very low neuron count may affect performance")
    
    region_count = config.getOr("region_count", 1)
    if region_count < 1 || region_count > 100:
        print("Error: Invalid region count")
        return False
    
    # Check plasticity parameters
    stdp_rate = config.getOr("plasticity.stdp.learning_rate", 0.01)
    if stdp_rate < 0.0 or stdp_rate > 1.0:
        print("Error: Invalid STDP learning rate")
        return False
    
    return True

def verify_systems(brain):
    """Verify all brain systems are integrated"""
    
    # Check memory systems
    if brain.getWorkingMemory() is None:
        print("Warning: Working memory not available")
        return False
    
    if brain.getEpisodicMemory() is None:
        print("Warning: Episodic memory not available")
        return False
    
    # Check neuromodulation systems
    if brain.getDopamine() is None:
        print("Warning: Dopamine system not available")
        return False
    
    # Check cognition systems
    if brain.getPlanner() is None:
        print("Warning: Planner not available")
        return False
    
    return True
```

## 10. Performance Optimization Tips

### Neuron Optimization
```python
# Configure for high performance
config.set("brain.neuron_count", 1000)  # Optimal balance
config.set("brain.tau_mem", 20.0)     # Standard membrane time constant
config.set("brain.tau_ref", 2.0)      # Short refractory period

# Reduce memory footprint
config.set("memory.working.capacity", 100)
config.set("memory.episodic.max_episodes", 500)
```

### Plasticity Optimization
```python
# Configure for stability
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.learning_rate", 0.001)
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00001)

# Enable homeostatic plasticity
config.set("plasticity.homeostatic.enable", True)
config.set("plasticity.homeostatic.target_rate", 0.1)
```

### Simulation Optimization
```python
# Configure for fast simulation
config.set("simulation_timestep", 0.001)
config.set("max_steps", 10000)
config.set("replay_interval", 100)

# Enable event-driven simulation
config.set("spike_system.mode", "event_driven")
config.set("plasticity.update_frequency", "per_step")
```

## 11. Troubleshooting

### Common Issues and Solutions

**Issue: Brain initialization fails**
- Cause: Invalid configuration parameters
- Solution: Check configuration file and ensure all required parameters are set correctly

**Issue: No learning occurs**
- Cause: Plasticity disabled or parameters too small
- Solution: Enable plasticity and increase learning rates

**Issue: Memory usage too high**
- Cause: Large working/episodic memory capacity
- Solution: Reduce memory capacity or increase consolidation rate

**Issue: Simulation too slow**
- Cause: Too many neurons or complex plasticity rules
- Solution: Reduce neuron count or disable expensive plasticity mechanisms

**Issue: No motor activity**
- Cause: Motor neuron population too small or no action selection
- Solution: Increase motor neuron count or configure action selection policy

### Debug Commands
```python
# Debug brain configuration
brain.getConfig().printSummary()

# Debug neural activity
brain.getTotalSpikeCount()
brain.getFiringNeuronCount()
brain.getActiveNeuronCount()

# Debug memory systems
if brain.getWorkingMemory():
    brain.getWorkingMemory().printStats()
if brain.getEpisodicMemory():
    brain.getEpisodicMemory().printStats()

# Debug neuromodulation
if brain.getDopamine():
    print(f"Dopamine level: {brain.getDopamine().getLevel()}")
if brain.getCuriosity():
    print(f"Curiosity level: {brain.getCuriosity().getLevel()}")

# Debug development
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
if brain.getDevelopmentSystem():
    brain.getDevelopmentSystem().printStatus()
```

This guide provides comprehensive documentation for advanced NLM users, covering configuration, experimentation, performance optimization, and troubleshooting. Use these techniques to create sophisticated neural simulations with full control over all brain systems and parameters.