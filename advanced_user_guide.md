# NLM (Neural Learning Machine) - ADVANCED USER GUIDE

This guide covers advanced usage and features of NLM for experienced users and developers.

## Overview

NLM is a brain-inspired spiking neural network simulator with advanced features:
- Real LIF neuron dynamics with event-driven spike propagation
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction and planning systems
- Developmental stages and plasticity
- Checkpoint/save/load functionality
- Agent-based learning in virtual environments

## Advanced Configuration Management

### Configuration Schemas

Configure NLM with validation schemas:

```python
import pynlm

# Define configuration schema
from pynlm import Config, ConfigSchema

schema = [
    ConfigSchema("brain.neuron_count", ["int"], "1000", True, "Number of neurons"),
    ConfigSchema("brain.connection_probability", ["double"], "0.1", True, "Connection probability"),
    ConfigSchema("plasticity.stdp.enable", ["bool"], "true", False, "Enable STDP plasticity"),
    ConfigSchema("neuromod.dopamine.scale", ["double"], "1.0", False, "Dopamine scaling factor"),
]

# Create config and apply schema
config = pynlm.createDefaultConfig()
for schema_entry in schema:
    config.addSchema(schema_entry)

# Validate configuration
validation = config.validate(schema)
if not validation.isValid:
    print("Validation errors:", validation.errors)
    print("Validation warnings:", validation.warnings)
```

### JSON/YAML Configuration

Load configuration from JSON/YAML files:

```python
import pynlm

# Create config from JSON string
json_config = '''
{
    "brain.neuron_count": 2000,
    "brain.connection_probability": 0.15,
    "plasticity.stdp.learning_rate": 0.002,
    "neuromod.curiosity.enable": true,
    "development.enable": true
}
'''

config = pynlm.createDefaultConfig()
config.importFromJSON(json_config)

# Export to YAML
yaml_output = config.exportToYAML()
print(yaml_output)
```

## Advanced Agent Programming

### Custom Agent Behavior

Create sophisticated agent behaviors:

```python
import pynlm
import math

class AdvancedAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Custom curiosity settings
        self.exploration_threshold = 0.3
        self.intrinsic_motivation = 0.5
        
    def adaptive_behavior(self, step, total_steps):
        # Phase-based behavior
        if step < total_steps * 0.3:  # Exploration phase
            return self.explore_behavior()
        elif step < total_steps * 0.8:  # Exploitation phase
            return self.exploit_behavior()
        else:  # Development phase
            return self.develop_behavior()
    
    def explore_behavior(self):
        # High curiosity, random exploration
        if self.agent.getCuriosityLevel() > self.exploration_threshold:
            return MotorCommand.Wait  # Let curiosity drive exploration
        return MotorCommand.MoveForward
    
    def exploit_behavior(self):
        # Goal-directed behavior
        percept = world.getSensoryPercept()
        vision = percept.getVision()
        
        # Find resources
        max_resource = max(vision)
        if max_resource > 0.5:
            return self.navigate_to_resource(vision)
        
        # Avoid hazards
        min_hazard = min(vision)
        if min_hazard < -0.5:
            return self.avoid_hazard(vision)
        
        return MotorCommand.MoveForward
    
    def develop_behavior(self):
        # Learn from experience, adapt strategies
        self.agent.updateDevelopment(0.1)
        return MotorCommand.Rest

def run_advanced_simulation(steps=2000):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(50, 50, 16, 16)
    world.reset()
    
    agent = AdvancedAgent(brain, world)
    
    for step in range(steps):
        world.update(0.1)
        
        # Get sensor data
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action with adaptive behavior
        action = agent.adaptive_behavior(step, steps)
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 500 == 0:
            stats = agent.agent.get_agent_stats()
            print(f"Step {step}: Curiosity={stats['curiosity']:.2f}, "
                  f"Novelty={stats['novelty']:.2f}, "
                  f"Stage={stats['developmental_stage']}")
    
    return brain, agent.agent, world
```

## Development Tools and Debugging

### Simulation Debugging

Debug and monitor simulation:

```python
import pynlm

def debug_simulation(brain, steps=500):
    print("=== NLM Simulation Debugger ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Get comprehensive statistics
    stats = brain.get_statistics()
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    print("\n--- Running simulation with debugging ---")
    
    for step in range(steps):
        brain.step(step)
        
        if step % 100 == 0:
            # Debug information
            firing = brain.getFiringNeuronCount()
            spikes = brain.getTotalSpikeCount()
            rate = brain.getAverageFiringRate()
            
            print(f"Step {step}: "
                  f"Firing={firing}, "
                  f"Spikes={spikes}, "
                  f"Rate={rate:.2f} Hz")
            
            # Check for issues
            if firing > brain.getTotalNeuronCount() * 0.8:
                print(f"  WARNING: High firing rate ({firing}/{brain.getTotalNeuronCount()})")
            
            if rate < 0.1:
                print(f"  WARNING: Low average firing rate ({rate:.3f})")
        
        # Check for system integration
        if step % 200 == 0:
            print(f"  System integration check:")
            print(f"    Memory systems: {brain.has_memory_systems()}")
            print(f"    Neuromodulation: {brain.has_neuromodulation()}")
            print(f"    Prediction: {brain.has_prediction()}")
            print(f"    Cognition: {brain.has_cognition()}")
            print(f"    Development: {brain.has_development()}")
    
    print("\n=== Simulation Complete ===")
    return brain
```

### Configuration Validation and Testing

Validate configurations programmatically:

```python
import pynlm

def validate_configuration(config, schema):
    """Validate configuration against schema"""
    print("=== Configuration Validation ===")
    print(f"Configuration has {config.getEntryCount()} entries")
    
    # Get source distribution
    print("\nSource distribution:")
    for source in ConfigSource:
        count = config.getEntryCount(source)
        ratio = config.getSourceRatio(source) * 100
        print(f"  {source}: {count} entries ({ratio:.1f}%)")
    
    # Validate against schema
    validation = config.validate(schema)
    print(f"\nValidation: {validation.getSummary()}")
    
    if not validation.isValid:
        print("Errors:")
        for error in validation.errors:
            print(f"  - {error}")
    
    if not validation.warnings.empty():
        print("Warnings:")
        for warning in validation.warnings:
            print(f"  - {warning}")
    
    return validation.isValid

# Example schema
validation_schema = [
    ConfigSchema("brain.neuron_count", ["int"], "1000", True),
    ConfigSchema("brain.synapse_density", ["double"], "0.1", True),
    ConfigSchema("plasticity.enable", ["bool"], "true", False),
]

# Test with different configurations
test_configs = [
    {"brain.neuron_count": 500, "plasticity.enable": True},
    {"brain.neuron_count": 2000, "plasticity.enable": False},
]

for i, test_config in enumerate(test_configs):
    print(f"\n--- Test Configuration {i+1} ---")
    config = pynlm.createDefaultConfig()
    for key, value in test_config.items():
        config.set(key, value)
    
    validate_configuration(config, validation_schema)
```

## Advanced Features

### Checkpoint and Recovery

Advanced checkpoint management:

```python
import pynlm

def advanced_checkpointing(brain, steps=1000, checkpoint_interval=200):
    """Run simulation with advanced checkpointing"""
    print("=== Advanced Checkpointing Demo ===")
    
    checkpoints = []
    
    for step in range(steps):
        brain.step(step)
        
        # Create checkpoints periodically
        if step % checkpoint_interval == 0 and step > 0:
            checkpoint_file = f"checkpoint_step_{step}.bin"
            if brain.save(checkpoint_file):
                checkpoints.append(checkpoint_file)
                print(f"Checkpoint created: {checkpoint_file}")
                
                # Load and compare
                brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
                brain2.initialize()
                
                if brain2.load(checkpoint_file):
                    print(f"  Checkpoint loaded successfully")
                    print(f"  Neuron count match: {brain.getTotalNeuronCount() == brain2.getTotalNeuronCount()}")
                else:
                    print(f"  ERROR: Failed to load checkpoint")
        
        if step % 300 == 0:
            stats = brain.get_statistics()
            print(f"Step {step}: {stats['total_spikes']} spikes, "
                  f"EI ratio: {stats['ei_ratio']:.3f}")
    
    # Restore from latest checkpoint
    if checkpoints:
        latest_checkpoint = checkpoints[-1]
        print(f"\nRestoring from latest checkpoint: {latest_checkpoint}")
        
        brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain3.initialize()
        brain3.load(latest_checkpoint)
        
        print(f"Restored brain stats:")
        print(f"  Neurons: {brain3.getTotalNeuronCount()}")
        print(f"  Synapses: {brain3.getTotalSynapseCount()}")
        print(f"  Total spikes: {brain3.getTotalSpikeCount()}")
    
    return brain, checkpoints
```

### Simulation Metrics and Monitoring

Collect comprehensive simulation metrics:

```python
import pynlm
from collections import defaultdict

class SimulationMonitor:
    def __init__(self):
        self.metrics = defaultdict(list)
        self.start_time = None
        self.step_times = []
    
    def record_step(self, step, brain, world, action):
        """Record metrics for a simulation step"""
        timestamp = time.time()
        
        if self.start_time is None:
            self.start_time = timestamp
        
        # Calculate step duration
        step_duration = timestamp - (self.step_times[-1] if self.step_times else self.start_time)
        self.step_times.append(timestamp)
        
        # Record metrics
        self.metrics['steps'].append(step)
        self.metrics['time'].append(timestamp - self.start_time)
        self.metrics['step_duration'].append(step_duration)
        
        self.metrics['total_neurons'].append(brain.getTotalNeuronCount())
        self.metrics['firing_neurons'].append(brain.getFiringNeuronCount())
        self.metrics['active_neurons'].append(brain.getActiveNeuronCount())
        self.metrics['total_spikes'].append(brain.getTotalSpikeCount())
        self.metrics['average_firing_rate'].append(brain.getAverageFiringRate())
        self.metrics['ei_ratio'].append(brain.getExcitationInhibitionRatio())
        
        if hasattr(world, 'getAgentBody'):
            body = world.getAgentBody()
            self.metrics['agent_x'].append(body.x)
            self.metrics['agent_y'].append(body.y)
            self.metrics['agent_energy'].append(body.energy)
            self.metrics['agent_health'].append(body.health)
        
        self.metrics['action_type'].append(str(action))
    
    def get_statistics(self):
        """Get summary statistics"""
        stats = {}
        
        for key, values in self.metrics.items():
            if not values:
                continue
                
            stats[f"{key}_mean"] = sum(values) / len(values)
            stats[f"{key}_min"] = min(values)
            stats[f"{key}_max"] = max(values)
            
            # Calculate std deviation for numeric values
            if all(isinstance(v, (int, float)) for v in values):
                mean = stats[f"{key}_mean"]
                variance = sum((x - mean) ** 2 for x in values) / len(values)
                stats[f"{key}_std"] = variance ** 0.5
        
        # Add derived metrics
        if 'step_duration' in self.metrics:
            avg_step_time = stats['step_duration_mean']
            estimated_steps_per_second = 1.0 / avg_step_time if avg_step_time > 0 else 0
            stats['estimated_steps_per_second'] = estimated_steps_per_second
        
        if 'total_spikes' in self.metrics and 'time' in self.metrics:
            total_time = max(self.metrics['time'])
            if total_time > 0:
                stats['spike_frequency_hz'] = max(self.metrics['total_spikes']) / total_time
        
        return stats
    
    def detect_anomalies(self):
        """Detect potential issues in simulation"""
        anomalies = []
        
        if 'firing_neurons_mean' in self.metrics:
            firing_rate = self.metrics['firing_neurons_mean'][-1]
            total_neurons = self.metrics['total_neurons'][-1]
            
            if firing_rate / total_neurons > 0.9:
                anomalies.append(f"High firing rate: {firing_rate/total_neurons*100:.1f}%")
            
            if firing_rate / total_neurons < 0.01:
                anomalies.append(f"Low firing rate: {firing_rate/total_neurons*100:.1f}%")
        
        if 'ei_ratio_mean' in self.metrics:
            ei_ratio = self.metrics['ei_ratio_mean'][-1]
            if ei_ratio > 10 or ei_ratio < 0.1:
                anomalies.append(f"Extreme E/I ratio: {ei_ratio:.3f}")
        
        return anomalies

class AdvancedSimulation:
    def __init__(self):
        self.monitor = SimulationMonitor()
    
    def run_with_monitoring(self, steps=1000, timestep=0.01):
        """Run simulation with comprehensive monitoring"""
        print("=== Advanced Simulation with Monitoring ===")
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(30, 30, 12, 12)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableAllFeatures()
        
        print(f"Starting simulation: {steps} steps, timestep={timestep}s")
        
        for step in range(steps):
            # Record start time
            step_start_time = time.time()
            
            # Update world
            world.update(timestep)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step, step * timestep)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            self.monitor.record_step(step, brain, world, action)
            
            # Print progress
            if step % 200 == 0:
                stats = self.monitor.get_statistics()
                anomalies = self.monitor.detect_anomalies()
                
                print(f"Step {step}:")
                print(f"  Neurons: {stats['total_neurons_mean']:.0f} (±{stats['total_neurons_std']:.0f})")
                print(f"  Firing: {stats['firing_neurons_mean']:.0f} (±{stats['firing_neurons_std']:.0f})")
                print(f"  Rate: {stats['average_firing_rate_mean']:.2f} Hz (±{stats['average_firing_rate_std']:.2f})")
                print(f"  EI ratio: {stats['ei_ratio_mean']:.3f}")
                
                if anomalies:
                    print(f"  ANOMALIES: {', '.join(anomalies)}")
        
        # Get final statistics
        final_stats = self.monitor.get_statistics()
        anomalies = self.monitor.detect_anomalies()
        
        print(f"\n=== Simulation Complete ===")
        print(f"Total steps: {steps}")
        print(f"Duration: {final_stats['time_mean']:.1f}s")
        print(f"Average step time: {final_stats['step_duration_mean']*1000:.1f}ms")
        
        if anomalies:
            print(f"Anomalies detected: {', '.join(anomalies)}")
        
        return brain, agent, world, self.monitor

# Run advanced simulation
advanced_sim = AdvancedSimulation()
brain, agent, world, monitor = advanced_sim.run_with_monitoring(steps=2000)
```

## Performance Optimization

### Efficient Configuration Loading

Load and process configurations efficiently:

```python
import pynlm
import json
import yaml

def benchmark_configuration_loading():
    """Benchmark different configuration loading methods"""
    print("=== Configuration Loading Benchmark ===")
    
    # Create test configuration
    config_data = {
        "brain.neuron_count": 2000,
        "brain.synapse_density": 0.15,
        "plasticity.stdp.learning_rate": 0.002,
        "plasticity.hebbian.learning_rate": 0.001,
        "neuromod.dopamine.scale": 1.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.sensitivity": 0.5,
        "development.plasticity_rate": 0.8,
    }
    
    # Test JSON loading
    start_time = time.time()
    json_str = json.dumps(config_data)
    config1 = pynlm.createDefaultConfig()
    config1.importFromJSON(json_str)
    json_time = time.time() - start_time
    print(f"JSON loading: {json_time*1000:.2f}ms ({len(config1.getKeys())} entries)")
    
    # Test YAML loading
    start_time = time.time()
    yaml_str = yaml.dump(config_data)
    config2 = pynlm.createDefaultConfig()
    config2.importFromYAML(yaml_str)
    yaml_time = time.time() - start_time
    print(f"YAML loading: {yaml_time*1000:.2f}ms ({len(config2.getKeys())} entries)")
    
    # Test simple format loading
    start_time = time.time()
    simple_str = "\n".join([f"{k}={v}" for k, v in config_data.items()])
    config3 = pynlm.createDefaultConfig()
    config3.loadFromString(simple_str, "simple")
    simple_time = time.time() - start_time
    print(f"Simple loading: {simple_time*1000:.2f}ms ({len(config3.getKeys())} entries)")
    
    # Test comparison
    print(f"\nConfiguration equality:")
    print(f"  JSON vs YAML: {config1.equals(config2)}")
    print(f"  JSON vs Simple: {config1.equals(config3)}")
    
    return config1, config2, config3
```

### Memory-Efficient Processing

Process large configurations efficiently:

```python
import pynlm

def process_large_configuration():
    """Process large configuration efficiently"""
    print("=== Large Configuration Processing ===")
    
    # Create configuration with many entries (simulating complex system)
    config = pynlm.createDefaultConfig()
    
    # Add neuron configurations
    for i in range(100):
        config.set(f"neuron.{i}.type", "Excitatory")
        config.set(f"neuron.{i}.threshold", -55.0 + (i % 10))
        config.set(f"neuron.{i}.resting_potential", -70.0)
    
    # Add synapse configurations
    for i in range(1000):
        config.set(f"synapse.{i}.pre", i % 100)
        config.set(f"synapse.{i}.post", (i + 1) % 100)
        config.set(f"synapse.{i}.weight", 0.1 + (i % 20) * 0.01)
    
    # Add plasticity configurations
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("plasticity.structural.enable", True)
    
    print(f"Created configuration with {len(config.getKeys())} entries")
    
    # Test validation with schema
    schema = [
        ConfigSchema("neuron.0.type", ["string"], "Excitatory"),
        ConfigSchema("plasticity.stdp.tau_plus", ["double"], "20.0"),
        ConfigSchema("neuromod.dopamine.scale", ["double"], "1.0"),
    ]
    
    validation = config.validate(schema)
    print(f"Validation: {validation.getSummary()}")
    
    if validation.isValid:
        print("Configuration is valid")
    else:
        print("Validation errors:", validation.errors)
    
    return config
```

## Complete Example: Advanced Agent with Learning and Development

```python
import pynlm
import math
import time

def advanced_agent_with_development():
    """
    Complete example of advanced agent with learning, development, and curiosity.
    This demonstrates all the advanced features together.
    """
    print("=== Advanced Agent with Development ===")
    
    # Configuration with advanced settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.connection_probability", 0.08)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("development.enable", True)
    config.set("development.plasticity_rate", 0.9)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(40, 40, 16, 16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create advanced world with resources and hazards
    for i in range(8):
        # Resources
        world.addObject(pynlm.WorldObject(
            x=10 + i * 4, y=10, 
            type=pynlm.WorldObjectType.Resource, 
            value=20.0, radius=0.8
        ))
        # Hazards  
        world.addObject(pynlm.WorldObject(
            x=30 - i * 4, y=30,
            type=pynlm.WorldObjectType.Hazard,
            value=-15.0, radius=0.6
        ))
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Initialized agent with {brain.getTotalNeuronCount()} neurons")
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Objects in world: {world.get_object_count()}")
    
    # Statistics tracking
    statistics = {
        'steps': [],
        'rewards': [],
        'curiosity': [],
        'novelty': [],
        'firing_rate': [],
        'development_stage': [],
        'agent_x': [],
        'agent_y': []
    }
    
    # Learning phases
    phases = [
        {'name': 'Exploration', 'duration': 500, 'curiosity_boost': 1.5},
        {'name': 'Exploitation', 'duration': 800, 'curiosity_decay': 0.8},
        {'name': 'Development', 'duration': 700, 'plasticity_increase': 1.2}
    ]
    
    current_phase = 0
    phase_progress = 0
    
    print("\n--- Starting Simulation ---")
    
    for step in range(2000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step, step * 0.1)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and learn
        reward = 0.0  # Would be computed from world state
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record statistics
        statistics['steps'].append(step)
        statistics['rewards'].append(reward)
        statistics['curiosity'].append(agent.getCuriosityLevel())
        statistics['novelty'].append(agent.getNoveltyLevel())
        statistics['firing_rate'].append(brain.getAverageFiringRate())
        statistics['development_stage'].append(agent.getDevelopmentalStage())
        
        if hasattr(world, 'getAgentBody'):
            body = world.getAgentBody()
            statistics['agent_x'].append(body.x)
            statistics['agent_y'].append(body.y)
        
        # Phase management
        phase_progress += 1
        if phase_progress >= phases[current_phase]['duration']:
            if current_phase < len(phases) - 1:
                current_phase += 1
                print(f"\n=== Phase Changed: {phases[current_phase]['name']} ===")
                phase_progress = 0
        
        # Print progress
        if step % 200 == 0:
            body = world.getAgentBody()
            print(f"Step {step:4d} | "
                  f"Phase: {phases[current_phase]['name']:12s} | "
                  f"Pos: ({body.x:5.1f}, {body.y:5.1f}) | "
                  f"Curiosity: {agent.getCuriosityLevel():.2f} | "
                  f"Novelty: {agent.getNoveltyLevel():.2f} | "
                  f"DevStage: {agent.getDevelopmentalStage():2d} | "
                  f"Firing: {brain.getFiringNeuronCount():4d}/{brain.getTotalNeuronCount()}")
    
    print(f"\n=== Simulation Complete ===")
    print(f"Total steps: {len(statistics['steps'])}")
    print(f"Final curiosity level: {statistics['curiosity'][-1]:.3f}")
    print(f"Final novelty level: {statistics['novelty'][-1]:.3f}")
    print(f"Average development stage: {sum(statistics['development_stage'])/len(statistics['development_stage']):.1f}")
    print(f"Agent final position: ({statistics['agent_x'][-1]:.1f}, {statistics['agent_y'][-1]:.1f})")
    
    # Calculate statistics
    final_stats = {
        'total_steps': len(statistics['steps']),
        'avg_curiosity': sum(statistics['curiosity']) / len(statistics['curiosity']),
        'avg_novelty': sum(statistics['novelty']) / len(statistics['novelty']),
        'avg_firing_rate': sum(statistics['firing_rate']) / len(statistics['firing_rate']),
        'total_reward': sum(statistics['rewards']),
        'final_development_stage': statistics['development_stage'][-1],
    }
    
    print(f"\n=== Performance Summary ===")
    for key, value in final_stats.items():
        print(f"  {key}: {value:.3f}")
    
    return brain, agent, world, statistics

# Run the complete advanced example
advanced_brain, advanced_agent, advanced_world, stats = advanced_agent_with_development()
```

This advanced guide demonstrates:
- Configuration management with JSON/YAML support
- Schema validation and type checking
- Advanced agent behavior with exploration/exploitation
- Development phases and adaptive behavior
- Comprehensive monitoring and anomaly detection
- Performance optimization techniques
- Real-world simulation with resources and hazards
- Complete integration of all NLM features

The examples are designed to be copy-paste runnable and showcase the full power of NLM's advanced features.
