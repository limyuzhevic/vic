# NLM Advanced User Guide

## Overview

This guide covers advanced usage of NLM (Neural Learning Machine) for experienced users who want to:
- Configure and tune complex brain simulations
- Create custom experiments
- Optimize performance
- Integrate NLM with external systems
- Use advanced features like structural plasticity, development, and neuromodulation

## Command-Line Interface

### Basic Commands

#### Run NLM Phase 6 Demo
```bash
./nlm_phase6_demo
```

#### Run with Custom Configuration
```bash
./nlm --config my_config.cfg
```

#### Run Specific Experiments
```bash
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo
```

### Advanced Configuration Options

#### Environment Variables
```bash
export NLM_LOG_LEVEL=DEBUG
export NLM_RANDOM_SEED=12345
export NLM_ENABLE_PROFILING=true
```

#### Command Line Arguments
```bash
./nlm [options]

options:
  --config FILE          Load configuration from file
  --steps N              Number of simulation steps (default: 1000)
  --seed N               Random seed (default: 42)
  --neurons N            Number of neurons (default: 1000)
  --regions N            Number of brain regions (default: 2)
  --timestep T            Simulation timestep in seconds (default: 0.001)
  --disable-learning      Disable plasticity for testing
  --checkpoint FILE       Save brain state to checkpoint
  --resume FILE           Load brain state from checkpoint
  --benchmark             Run performance benchmark
  --visualize             Enable visualization
  --help                 Show help message
```

## Advanced Configuration Examples

### 1. High-Performance Configuration

```
# File: high_performance.cfg
random_seed = 42
simulation_timestep = 0.0005  # Smaller timestep for accuracy
neuron_count = 5000             # More neurons for complexity
region_count = 4               # Multiple specialized regions
connection_probability = 0.15   # Denser connectivity

# Performance optimizations
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.01
stdp_time_constant = 15.0

development_synaptogenesis_rate = 0.00005
development_pruning_rate = 0.000005

enable_structural_plasticity = true
enable_development = true
enable_curiosity = true
enable_prediction = true

working_memory_capacity = 500
episodic_memory_capacity = 5000
semantic_memory_capacity = 20000
associative_memory_capacity = 10000

# Neuromodulation tuning
curiosity_threshold = 0.3
curiosity_gain = 0.15
novelty_threshold = 0.2
```

### 2. Learning-Focused Configuration

```
# File: learning_focused.cfg
random_seed = 123
neuron_count = 2000
region_count = 3
connection_probability = 0.2

# High plasticity rates for rapid learning
plasticity_learning_rate = 0.05
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.04
stdp_time_constant = 25.0

development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Enable all learning systems
enable_structural_plasticity = true
enable_development = true
enable_curiosity = true
enable_prediction = true

# Memory configuration for learning
working_memory_capacity = 200
episodic_memory_capacity = 2000
semantic_memory_capacity = 10000
associative_memory_capacity = 5000

# Curiosity-driven exploration
curiosity_threshold = 0.1
curiosity_gain = 0.2
novelty_threshold = 0.05
```

### 3. Development-Focused Configuration

```
# File: development_focused.cfg
random_seed = 999
neuron_count = 1500
region_count = 2
connection_probability = 0.08

# Slow, controlled development
plasticity_learning_rate = 0.001
stdp_ltp_weight = 0.005
stdp_ltd_weight = 0.006
stdp_time_constant = 30.0

development_synaptogenesis_rate = 0.01
 development_pruning_rate = 0.001

# Focus on developmental processes
enable_structural_plasticity = true
enable_development = true
enable_curiosity = false  # Reduce exploratory behavior
enable_prediction = false

# Large memory systems for developmental learning
working_memory_capacity = 300
episodic_memory_capacity = 3000
semantic_memory_capacity = 15000
associative_memory_capacity = 8000

# Different developmental stages
initial_stage = "CriticalPeriod"
maturation_target = "Adult"
```

## Advanced Python API Usage

### Custom Brain Configuration

```python
import pynlm

# Create custom config
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.2)

# Configure plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Configure neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.threshold", 0.3)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.threshold", 0.2)

# Configure memory systems
config.set("memory.working.capacity", 500)
config.set("memory.episodic.capacity", 5000)
config.set("memory.semantic.capacity", 20000)
config.set("memory.associative.capacity", 10000)

# Configure development
config.set("development.enable", True)
config.set("development.synaptogenesis.rate", 0.001)
config.set("development.pruning.rate", 0.0001)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Experiment Creation

```python
import pynlm

class AdvancedLearningExperiment:
    def __init__(self, config):
        self.brain = pynlm.createBrain(config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.results = {}
    
    def setup(self):
        # Configure world
        self.world.configure(
            width=50,
            height=50,
            visionWidth=12,
            visionHeight=12
        )
        self.world.reset()
        self.world.setAgentStart(25.0, 25.0)
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Enable advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        self.agent.enablePrediction(True)
    
    def run_episode(self, episode_length=1000):
        """Run a single episode with advanced logging and monitoring"""
        episode_start_spikes = self.brain.getTotalSpikeCount()
        episode_start_neurons = self.brain.getFiringNeuronCount()
        
        curiosity_levels = []
        novelty_levels = []
        prediction_errors = []
        development_stages = []
        
        for step in range(episode_length):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process in brain
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply advanced neuromodulation
            reward = self.calculate_reward(percept)
            predicted_reward = self.agent.getPredictionError()
            
            self.agent.applyRewardModulation(reward, predicted_reward)
            self.agent.updateDevelopment(0.1)
            
            # Collect metrics
            curiosity_levels.append(self.agent.getCuriosityLevel())
            novelty_levels.append(self.agent.getNoveltyLevel())
            prediction_errors.append(self.agent.getPredictionError())
            development_stages.append(self.brain.getDevelopmentalStage())
            
            # Periodic advanced logging
            if step % 100 == 0:
                self.log_advanced_metrics(step)
        
        # Collect episode statistics
        episode_end_spikes = self.brain.getTotalSpikeCount()
        episode_end_neurons = self.brain.getFiringNeuronCount()
        
        self.results['spike_count'] = episode_end_spikes - episode_start_spikes
        self.results['firing_neuron_change'] = episode_end_neurons - episode_start_neurons
        self.results['avg_curiosity'] = sum(curiosity_levels) / len(curiosity_levels)
        self.results['avg_novelty'] = sum(novelty_levels) / len(novelty_levels)
        self.results['avg_prediction_error'] = sum(prediction_errors) / len(prediction_errors)
        self.results['final_development_stage'] = development_stages[-1]
        
        return self.results
    
    def calculate_reward(self, percept):
        """Advanced reward calculation based on multiple factors"""
        reward = 0.0
        
        # Reward for reaching resources
        if percept.getInternal() and len(percept.getInternal()) > 0:
            reward += percept.getInternal()[0] * 10.0
        
        # Reward for exploration (curiosity)
        curiosity = self.agent.getCuriosityLevel()
        if curiosity > 0.7:
            reward += curiosity * 5.0
        
        # Penalty for hazards
        if percept.getHazard():
            reward -= 10.0
        
        return reward
    
    def log_advanced_metrics(self, step):
        """Advanced metric logging"""
        print(f"\nStep {step} Advanced Metrics:")
        print(f"  Brain Statistics:")
        print(f"    Total neurons: {self.brain.getTotalNeuronCount()}")
        print(f"    Total synapses: {self.brain.getTotalSynapseCount()}")
        print(f"    Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"    Average firing rate: {self.brain.getAverageFiringRate():.2f} Hz")
        print(f"    E/I ratio: {self.brain.getExcitationInhibitionRatio():.2f}")
        
        print(f"  Neuromodulation:")
        print(f"    Curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"    Novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"    Prediction error: {self.agent.getPredictionError():.3f}")
        
        print(f"  Development:")
        print(f"    Current stage: {self.brain.getDevelopmentalStage()}")
        print(f"    Stage progress: {self.agent.getDevelopmentProgress():.2%}")
        
        print(f"  Memory Usage:")
        print(f"    Working memory: {self.brain.getWorkingMemory().getUsage():.1%}")
        print(f"    Episodic memory: {self.brain.getEpisodicMemory().getUsage():.1%}")

# Example usage
experiment = AdvancedLearningExperiment(pynlm.createDefaultConfig())
experiment.setup()
results = experiment.run_episode(1000)

print("\nExperiment Results:")
for key, value in results.items():
    print(f"  {key}: {value}")
```

### Performance Monitoring and Profiling

```python
import pynlm
import time
import cProfile
import pstats

class PerformanceMonitor:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.timings = []
        self.memory_usage = []
        self.spike_counts = []
    
    def run_with_profiling(self, steps=1000):
        """Run simulation with performance profiling"""
        print(f"Running {steps} steps with profiling...")
        
        # Start profiler
        profiler = cProfile.Profile()
        profiler.enable()
        start_time = time.time()
        
        for step in range(steps):
            step_start = time.time()
            
            # Update world
            self.world.update(0.1)
            
            # Process in brain
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            step_end = time.time()
            self.timings.append(step_end - step_start)
            self.spike_counts.append(self.brain.getFiringNeuronCount())
            
            if step % 100 == 0:
                print(f"  Step {step}: {self.timings[-1]:.3f}s per step")
        
        end_time = time.time()
        profiler.disable()
        
        # Print profiling results
        profiler.print_stats(sort='cumulative')
        
        # Calculate statistics
        total_time = end_time - start_time
        avg_step_time = sum(self.timings) / len(self.timings)
        min_step_time = min(self.timings)
        max_step_time = max(self.timings)
        
        print(f"\nPerformance Summary:")
        print(f"  Total time: {total_time:.2f}s")
        print(f"  Average step time: {avg_step_time:.3f}s")
        print(f"  Min step time: {min_step_time:.3f}s")
        print(f"  Max step time: {max_step_time:.3f}s")
        print(f"  Steps per second: {steps / total_time:.1f}")
        print(f"  Average neurons firing: {sum(self.spike_counts) / len(self.spike_counts):.1f}")
        
        return {
            'total_time': total_time,
            'avg_step_time': avg_step_time,
            'min_step_time': min_step_time,
            'max_step_time': max_step_time,
            'steps_per_second': steps / total_time,
            'avg_firing_neurons': sum(self.spike_counts) / len(self.spike_counts)
        }
```

### Batch Experiment Runner

```python
import pynlm
import json
import os

class BatchExperimentRunner:
    def __init__(self, base_config, experiment_configs):
        self.base_config = base_config
        self.experiment_configs = experiment_configs
        self.results_dir = "experiment_results"
        
        if not os.path.exists(self.results_dir):
            os.makedirs(self.results_dir)
    
    def run_experiment(self, config_name, experiment_config):
        """Run a single experiment with given configuration"""
        print(f"\nRunning experiment: {config_name}")
        
        # Create config for this experiment
        config = self.base_config
        for key, value in experiment_config.items():
            config.set(key, value)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(
            width=experiment_config.get('world_width', 20),
            height=experiment_config.get('world_height', 20),
            visionWidth=experiment_config.get('vision_width', 8),
            visionHeight=experiment_config.get('vision_height', 8)
        )
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable specified features
        features = experiment_config.get('features', {})
        if features.get('reward_modulation', False):
            agent.enableRewardModulation(True)
        if features.get('structural_plasticity', False):
            agent.enableStructuralPlasticity(True)
        if features.get('development', False):
            agent.enableDevelopment(True)
        if features.get('curiosity', False):
            agent.enableCuriosity(True)
        if features.get('prediction', False):
            agent.enablePrediction(True)
        
        # Run simulation
        steps = experiment_config.get('steps', 1000)
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if features.get('reward_modulation', False):
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            if features.get('development', False):
                agent.updateDevelopment(0.1)
        
        # Collect results
        results = {
            'experiment': config_name,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'final_development_stage': brain.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        }
        
        # Save results
        filename = f"{self.results_dir}/{config_name}_results.json"
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"  Results saved to {filename}")
        return results
    
    def run_all_experiments(self):
        """Run all configured experiments"""
        all_results = {}
        
        for config_name, experiment_config in self.experiment_configs.items():
            results = self.run_experiment(config_name, experiment_config)
            all_results[config_name] = results
        
        # Save summary
        summary_filename = f"{self.results_dir}/batch_results_summary.json"
        with open(summary_filename, 'w') as f:
            json.dump(all_results, f, indent=2)
        
        print(f"\nBatch experiment complete!")
        print(f"Summary saved to {summary_filename}")
        
        return all_results

# Example usage
base_config = pynlm.createDefaultConfig()

experiment_configs = {
    "high_performance": {
        "brain.neuron_count": 5000,
        "plasticity.stdp.learning_rate": 0.01,
        "enable_structural_plasticity": True,
        "enable_development": True,
        "enable_curiosity": True,
        "steps": 500,
        "world_width": 30,
        "world_height": 30
    },
    "learning_focused": {
        "brain.neuron_count": 2000,
        "plasticity.stdp.learning_rate": 0.05,
        "plasticity.hebbian.enable": True,
        "enable_structural_plasticity": True,
        "enable_curiosity": True,
        "steps": 800,
        "world_width": 25,
        "world_height": 25
    },
    "development_focused": {
        "brain.neuron_count": 1500,
        "plasticity.stdp.learning_rate": 0.001,
        "enable_development": True,
        "enable_structural_plasticity": True,
        "steps": 1200,
        "world_width": 20,
        "world_height": 20
    }
}

runner = BatchExperimentRunner(base_config, experiment_configs)
runner.run_all_experiments()
```

## Advanced Configuration Options

### Custom Configuration File Format

NLM supports advanced configuration file formats:

```ini
# Example: Custom configuration with complex parameters
[brain]
neuron_count = 3000
region_count = 3
connection_probability = 0.12
synapse_density = 0.8

[brain.simulation]
timestep = 0.0005
max_steps = 5000
enable_structural_plasticity = true

[plasticity.stdp]
enabled = true
learning_rate = 0.02
time_constant = 15.0
gating_strength = 0.8

[plasticity.hebbian]
enabled = true
learning_rate = 0.01
learning_window = 20.0

[neuromodulation.dopamine]
baseline = 0.2
scale = 1.2
plasticity_modulation = 0.9

[neuromodulation.curiosity]
enabled = true
threshold = 0.4
gain = 0.15
memory_modulation = 0.7

[memory.working]
capacity = 300
decay_rate = 0.01
encoding_strength = 0.8

[memory.episodic]
capacity = 3000
replay_frequency = 0.1
replay_strength = 0.6

[development]
enabled = true
synaptogenesis_rate = 0.005
pruning_rate = 0.0005
critical_period = 0.3
maturation_speed = 0.8

[advanced]
enable_prediction = true
enable_attention = true
enable_concept_formation = true
enable_self_modeling = true
```

### Configuration Inheritance

You can create configuration templates and inherit from them:

```python
# base_config.cfg
[base]
simulation_timestep = 0.001
neuron_count = 1000

# learning_config.cfg
[learning]
base
plasticity.stdp.enable = true
plasticity.hebbian.enable = true
neuromodulation.curiosity.enable = true

# development_config.cfg
[development]
base
plasticity.structural.enable = true
development.enable = true
development.synaptogenesis_rate = 0.001
```

## API Reference

### Brain Class Methods

#### Advanced Brain Configuration
```python
# Batch configuration
brain.configure({
    "brain.neuron_count": 5000,
    "plasticity.stdp.enable": True,
    "neuromodulation.curiosity.threshold": 0.3,
    "memory.working.capacity": 500
})

# Get all brain statistics
brain_stats = {
    "neurons": brain.getTotalNeuronCount(),
    "synapses": brain.getTotalSynapseCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "e_i_ratio": brain.getExcitationInhibitionRatio(),
    "development_stage": brain.getDevelopmentalStage(),
    "spike_count": brain.getTotalSpikeCount(),
    "working_memory_usage": brain.getWorkingMemory().getUsage(),
    "episodic_memory_usage": brain.getEpisodicMemory().getUsage()
}
```

#### Advanced Brain Control
```python
# Complex brain reset with custom parameters
brain.reset()
brain.initialize()

# Gradual development
for stage in [DevelopmentalStage.CriticalPeriod,
              DevelopmentalStage.Maturation,
              DevelopmentalStage.Adult]:
    brain.setDevelopmentalStage(stage)
    for step in range(100):
        brain.step(step)
        time.sleep(0.001)  # Simulate real-time development
```

### AgentBrain Class Methods

#### Advanced Agent Configuration
```python
# Create agent with advanced features
agent = pynlm.createAgentBrain(brain)

# Configure all subsystems at once
agent.configure({
    "enable_reward_modulation": True,
    "enable_structural_plasticity": True,
    "enable_development": True,
    "enable_curiosity": True,
    "enable_prediction": True,
    "enable_attention": True,
    "enable_concept_formation": True
})

# Advanced neuromodulation
agent.apply_reward_modulation(reward, predicted_reward, confidence)
agent.update_development(timestep, environmental_stress)
agent.apply_curiosity_bonus(exploration_score)

# Advanced sensory processing
agent.process_sensory_input(percept, attention_weight, memory_bias)
agent.apply_prediction(percept, prediction_error, prediction_confidence)
```

### SimpleWorld Class Methods

#### Advanced World Configuration
```python
# Create complex world
world = pynlm.createSimpleWorld()

# Advanced world configuration
world.configure(
    width=50,
    height=50,
    visionWidth=16,
    visionHeight=16,
    maxEnergy=1000,
    enableDynamicObjects=True,
    enableHazards=True,
    enableResources=True,
    enableWeather=True,
    weatherSeed=42
)

# Advanced world control
world.reset()
world.update_timestep(0.05)  # Smaller timestep for accuracy
world.set_agent_position(25.0, 25.0)
world.set_agent_orientation(math.pi/4)  # 45 degrees

# Advanced object management
world.add_resource(10.0, 10.0, 100.0)
world.add_hazard(30.0, 30.0, 50.0)
world.add_wall_rectangle(0, 0, 50, 5)
world.add_wall_rectangle(0, 45, 50, 5)

# Advanced world state
world_state = {
    "position": world.get_agent_position(),
    "orientation": world.get_agent_orientation(),
    "energy": world.get_agent_energy(),
    "simulation_time": world.get_simulation_time(),
    "weather": world.get_weather_state(),
    "objects": world.get_all_objects(),
    "percept": world.get_sensory_percept()
}
```

## Troubleshooting and Optimization

### Common Issues and Solutions

#### Issue: Brain is not learning
**Solution**: Check that plasticity is enabled
```python
# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enablePrediction(True)

# Verify configuration
assert brain.getConfig().getOr<bool>("plasticity.stdp.enable", False)
assert brain.getConfig().getOr<bool>("neuromodulation.curiosity.enable", False)
```

#### Issue: Simulation is too slow
**Solution**: Optimize configuration and parameters
```python
# Use more efficient configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Reduce neuron count
config.set("brain.connection_probability", 0.05)  # Reduce connectivity
config.set("simulation.timestep", 0.001)  # Use efficient timestep

# Enable performance optimizations
config.set("enable_structural_plasticity", False)  # Disable expensive operations
config.set("enable_prediction", False)  # Disable prediction system
```

#### Issue: Memory usage is high
**Solution**: Configure memory systems efficiently
```python
# Configure memory capacity
brain.getWorkingMemory().setCapacity(100)
brain.getEpisodicMemory().setCapacity(1000)
brain.getSemanticMemory().setCapacity(5000)
brain.getAssociativeMemory().setCapacity(2000)

# Enable memory decay to prevent overflow
brain.getEpisodicMemory().enableDecay(True)
brain.getSemanticMemory().enableDecay(True)
```

### Performance Optimization Tips

1. **Use appropriate neuron counts**: 1000-5000 neurons is usually sufficient for most tasks
2. **Optimize connectivity**: Lower connection probability (0.05-0.1) reduces computation
3. **Use appropriate timesteps**: 0.001s is standard, larger values improve performance
4. **Disable unused features**: Only enable systems you need
5. **Configure memory appropriately**: Match memory capacity to task requirements
6. **Use efficient configurations**: Pre-tune configurations for specific tasks

## Extensibility and Customization

### Creating Custom Brain Types

```python
class CustomBrain:
    def __init__(self, base_brain, custom_config):
        self.base_brain = base_brain
        self.custom_config = custom_config
        self.custom_components = {}
    
    def add_custom_neuron_type(self, neuron_type, params):
        """Add custom neuron type"""
        # Implementation depends on NLM architecture
        self.custom_components[neuron_type] = params
    
    def add_custom_plasticity_rule(self, rule_name, rule_params):
        """Add custom plasticity rule"""
        self.custom_components[rule_name] = rule_params
    
    def add_custom_neuromodulator(self, modulator_name, modulator_params):
        """Add custom neuromodulator"""
        self.custom_components[modulator_name] = modulator_params
    
    def run_simulation(self, steps):
        """Run simulation with custom components"""
        # Integrate custom components
        # Run simulation loop
        for step in range(steps):
            self.base_brain.step(step)
            # Apply custom modifications
```

### Integrating with External Systems

```python
class ExternalIntegration:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.external_sensors = {}
        self.external_actuators = {}
    
    def add_external_sensor(self, sensor_name, sensor_interface):
        """Add external sensor"""
        self.external_sensors[sensor_name] = sensor_interface
    
    def add_external_actuator(self, actuator_name, actuator_interface):
        """Add external actuator"""
        self.external_actuators[actuator_name] = actuator_interface
    
    def run_with_external_input(self, steps):
        """Run simulation with external input"""
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get world percept
            world_percept = self.world.getSensoryPercept()
            
            # Get external sensor data
            external_percepts = {}
            for name, sensor in self.external_sensors.items():
                external_percepts[name] = sensor.get_data()
            
            # Combine percepts
            combined_percept = self.combine_percepts(world_percept, external_percepts)
            
            # Process in brain
            self.agent.processSensoryInput(combined_percept)
            self.brain.step(step)
            
            # Get brain action
            brain_action = self.agent.decodeMotorCommand()
            
            # Apply world action
            world_action = self.convert_to_world_action(brain_action)
            self.world.applyMotorCommand(world_action, self.world.getSimulationTime())
            
            # Apply external actuation if available
            if brain_action in self.external_actuators:
                self.external_actuators[brain_action].apply()
    
    def combine_percepts(self, world_percept, external_percepts):
        """Combine world and external percepts"""
        # Implementation depends on percept format
        # This could involve weighting, filtering, or fusion
        pass
    
    def convert_to_world_action(self, brain_action):
        """Convert brain action to world action"""
        # Implementation depends on action mappings
        pass
```

## Conclusion

This advanced user guide provides comprehensive documentation for using NLM at its full potential. The key takeaways are:

1. **Use advanced configuration files** for complex experiments
2. **Leverage batch experiment runners** for systematic testing
3. **Monitor performance** with profiling tools
4. **Optimize configurations** for specific tasks
5. **Extend the system** with custom components when needed
6. **Integrate with external systems** for real-world applications

NLM provides a powerful platform for creating artificial brains with learning, memory, and cognitive capabilities. By using these advanced features, you can create sophisticated simulations that push the boundaries of artificial intelligence.

---

**For more information:**
- Read `docs/ARCHITECTURE.md` for system architecture details
- Read `docs/SCIENCE.md` for scientific background
- Read `docs/EXPERIMENTS.md` for experiment descriptions
- Check the GitHub repository for additional examples and documentation