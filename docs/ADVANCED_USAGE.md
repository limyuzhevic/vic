# Advanced NLM Usage Guide

This guide covers advanced features and power user techniques for NLM, building on the basic usage concepts from [easy_usage.md](./easy_usage.md) and [HOW_TO_USE.md](./HOW_TO_USE.md).

## Advanced Brain Configuration

### Custom Network Creation

```python
import pynlm

def create_deep_brain(num_layers=3, neurons_per_layer=100, connectivity=0.2):
    """Create a deep neural network with multiple layers and specific connectivity."""
    config = pynlm.createDefaultConfig()
    
    # Configure custom network parameters
    config.set_advanced(
        neuron_count=num_layers * neurons_per_layer,
        brain_synapse_density=connectivity,
        brain_connection_probability=0.1,
        plasticity_stdp_enable=True,
        plasticity_hebbian_enable=True,
        plasticity_structural_enable=True,
        neuromod_dopamine_scale=1.0,
        neuromod_curiosity_enable=True,
        neuromod_novelty_enable=True
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create multiple regions for different layers
    regions = []
    for i in range(num_layers):
        region_id = brain.createRegion(f"layer_{i}")
        regions.append(region_id)
    
    # Create inter-region connections (feedforward)
    for i in range(num_layers - 1):
        brain.addInterRegionConnection(regions[i], regions[i + 1], 0.5, 1)
    
    return brain, regions

# Use the advanced brain
brain, regions = create_deep_brain(4, 200, 0.15)
for step in range(500):
    brain.step(step)
    if step % 100 == 0:
        print(f"Layer 0 firing: {brain.getRegion(regions[0]).getFiringNeuronCount()}")
```

### Advanced Plasticity Control

```python
import pynlm

def create_advanced_learning_brain():
    """Create a brain with sophisticated learning capabilities."""
    config = pynlm.createDefaultConfig()
    
    # Enable all plasticity mechanisms
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable all plasticity at once
    brain.enableAllPlasticity()
    
    # Configure specific plasticity parameters
    brain.getSTDP().setLearningRate(0.01)
    brain.getSTDP().setTimeConstant(20.0, 20.0)
    
    brain.getHebbian().setLearningRate(0.005)
    brain.getHebbian().setWeightRange(0.1, 1.0)
    
    brain.getStructuralPlasticity().setSynaptogenesisRate(0.001)
    brain.getStructuralPlasticity().setPruningRate(0.0001)
    
    return brain

brain = create_advanced_learning_brain()
```

## Batch Operations for Performance

### Efficient Simulation Processing

```python
import pynlm
import time

def run_batch_simulation(brain, world, agent, steps, batch_size=100):
    """Run simulation with optimized batch processing."""
    start_time = time.time()
    
    for batch_start in range(0, steps, batch_size):
        batch_end = min(batch_start + batch_size, steps)
        
        # Process batch of steps
        for step in range(batch_start, batch_end):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation in batches
            if step % 10 == 0:
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
        
        # Performance statistics
        batch_time = time.time() - start_time
        print(f"Batch {batch_start}-{batch_end}: {batch_time:.2f}s")
    
    total_time = time.time() - start_time
    print(f"Total simulation time: {total_time:.2f}s")
    print(f"Performance: {steps/total_time:.1f} steps/second")

# Create and run batch simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableAllFeatures(True)  # Enable all features

world = pynlm.createSimpleWorld()
world.configure(50, 50, 8, 8)
world.reset()

run_batch_simulation(brain, world, agent, 1000)
```

## Experimental Control and Tracking

### Behavioral Strategy Framework

```python
import pynlm
from enum import Enum
import time

class BehavioralStrategy(Enum):
    EXPLORATION = "exploration"
    EXPLOITATION = "exploitation"
    BALANCED = "balanced"
    NOVELTY_SEEKING = "novelty_seeking"
    HABITUATION = "habituation"

class BehavioralController:
    def __init__(self, agent, strategy=BehavioralStrategy.BALANCED):
        self.agent = agent
        self.strategy = strategy
        self.strategy_start_time = time.time()
        self.strategy_duration = 60.0  # seconds per strategy
        self.exploration_count = 0
        self.exploitation_count = 0
        
    def create_behavioral_strategy(self, strategy):
        """Create a behavioral strategy with specific neuromodulation settings."""
        self.strategy = strategy
        self.strategy_start_time = time.time()
        
        if strategy == BehavioralStrategy.EXPLORATION:
            self.agent.setCuriosityScale(2.0)  # High curiosity
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(False)
            
        elif strategy == BehavioralStrategy.EXPLOITATION:
            self.agent.setCuriosityScale(0.2)  # Low curiosity
            self.agent.enableRewardModulation(True)
            self.agent.enableStructuralPlasticity(True)
            
        elif strategy == BehavioralStrategy.BALANCED:
            self.agent.setCuriosityScale(1.0)  # Medium curiosity
            self.agent.enableAllFeatures(True)
            
        elif strategy == BehavioralStrategy.NOVELTY_SEEKING:
            self.agent.setCuriosityScale(3.0)
            self.agent.enableNoveltyDetection(True)
            
        elif strategy == BehavioralStrategy.HABITUATION:
            self.agent.setCuriosityScale(0.1)
            self.agent.enableCuriosity(False)
    
    def get_current_strategy_performance(self):
        """Get performance metrics for current strategy."""
        time_in_strategy = time.time() - self.strategy_start_time
        
        return {
            'strategy': self.strategy.value,
            'time_in_strategy': time_in_strategy,
            'exploration_ratio': self.exploration_count / max(1, self.exploration_count + self.exploitation_count),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError()
        }

# Create behavioral controller
agent = pynlm.createAgentBrain(brain)
controller = BehavioralController(agent)

# Switch strategies over time
strategies = list(BehavioralStrategy)
for i, strategy in enumerate(strategies):
    controller.create_behavioral_strategy(strategy)
    
    for step in range(100):
        # ... your simulation loop ...
        pass
        
    performance = controller.get_current_strategy_performance()
    print(f"Strategy {strategy.value} performance: {performance}")
```

## Dynamic Environment Creation

### Procedural Environment Generation

```python
import pynlm
import random

class DynamicEnvironment:
    def __init__(self, width, height, seed=None):
        self.width = width
        self.height = height
        self.seed = seed
        self.random = random.Random(seed)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 8, 8)
        self.world.reset()
    
    def create_dynamic_environment(self, density=0.3, resource_prob=0.6, hazard_prob=0.3):
        """Create a procedurally generated dynamic environment."""
        # Clear existing objects
        # (Implementation would clear world objects)
        
        # Add static structures (walls)
        self._add_walls()
        
        # Add dynamic objects
        for x in range(self.width):
            for y in range(self.height):
                if self.random.random() < density:
                    obj_type = self.random.random()
                    if obj_type < resource_prob:
                        value = self.random.uniform(0.5, 2.0)
                        world_obj = pynlm.WorldObject(
                            x, y, pynlm.WorldObjectType.Resource, value
                        )
                        self.world.addObject(world_obj)
                    elif obj_type < resource_prob + hazard_prob:
                        damage = self.random.uniform(0.2, 1.0)
                        world_obj = pynlm.WorldObject(
                            x, y, pynlm.WorldObjectType.Hazard, damage
                        )
                        self.world.addObject(world_obj)
        
        # Add agent start position
        start_x = self.random.uniform(2, self.width - 3)
        start_y = self.random.uniform(2, self.height - 3)
        self.world.setAgentStart(start_x, start_y)
        
        return self.world
    
    def _add_walls(self):
        """Add boundary walls to the environment."""
        wall_thickness = 1.0
        for x in range(self.width):
            for y in range(self.height):
                is_wall = (
                    x < wall_thickness or 
                    x >= self.width - wall_thickness or
                    y < wall_thickness or 
                    y >= self.height - wall_thickness
                )
                if is_wall:
                    world_obj = pynlm.WorldObject(
                        x, y, pynlm.WorldObjectType.Wall
                    )
                    self.world.addObject(world_obj)
    
    def spawn_resources_batch(self, count, resource_type='random'):
        """Spawn multiple resources efficiently."""
        for _ in range(count):
            x = self.random.uniform(1, self.width - 2)
            y = self.random.uniform(1, self.height - 2)
            
            if not self.world.isValidPosition(x, y):
                continue
                
            if resource_type == 'random':
                value = self.random.uniform(0.5, 2.0)
            elif resource_type == 'high':
                value = self.random.uniform(2.0, 5.0)
            elif resource_type == 'low':
                value = self.random.uniform(0.1, 0.5)
            
            resource = pynlm.WorldObject(
                x, y, pynlm.WorldObjectType.Resource, value
            )
            self.world.addObject(resource)
        
        return count

# Create dynamic environment
env = DynamicEnvironment(20, 20, seed=42)
dynamic_world = env.create_dynamic_environment(density=0.2)

# Spawn resources in batches
env.spawn_resources_batch(50, 'random')
env.spawn_resources_batch(10, 'high')
```

## Advanced Configuration Management

### Configuration Validation and Management

```python
import pynlm
import json

class AdvancedConfigManager:
    def __init__(self):
        self.config_history = []
        self.config_templates = {}
    
    def validate_config(self, config):
        """Validate configuration with detailed error reporting."""
        errors = []
        warnings = []
        
        # Check required configuration
        required_keys = [
            'brain.neuron_count',
            'brain.synapse_density',
            'brain.v_thresh',
            'brain.v_reset'
        ]
        
        for key in required_keys:
            if not config.has(key):
                errors.append(f"Missing required configuration: {key}")
        
        # Validate configuration ranges
        if config.has('brain.neuron_count'):
            neuron_count = config.get('brain.neuron_count')
            if neuron_count < 10 or neuron_count > 1000000:
                warnings.append(f"Unusual neuron count: {neuron_count}")
        
        if config.has('brain.synapse_density'):
            density = config.get('brain.synapse_density')
            if density < 0.0 or density > 1.0:
                errors.append(f"Invalid synapse density: {density}")
        
        # Check for duplicate values (like the dopamine_baseline bug we fixed)
        all_keys = config.getKeys()
        if len(all_keys) != len(set(all_keys)):
            # This would catch duplicate keys in some configs
            warnings.append("Possible duplicate configuration keys")
        
        return errors, warnings
    
    def get_config_range(self, config, start_key, end_key):
        """Get configuration range with detailed information."""
        keys = config.getKeys()
        keys.sort()  # Sort alphabetically
        
        if start_key not in keys:
            raise KeyError(f"Start key '{start_key}' not found in config")
        if end_key not in keys:
            raise KeyError(f"End key '{end_key}' not found in config")
        
        start_index = keys.index(start_key)
        end_index = keys.index(end_key)
        
        if start_index > end_index:
            start_index, end_index = end_index, start_index
        
        range_keys = keys[start_index:end_index + 1]
        range_values = {}
        
        for key in range_keys:
            value = config.get(key)
            range_values[key] = value
        
        return range_values
    
    def set_advanced_config(self, config, **kwargs):
        """Advanced configuration setter with validation."""
        errors, warnings = self.validate_config(config)
        
        if errors:
            raise ValueError(f"Configuration validation errors: {errors}")
        
        for key, value in kwargs.items():
            config.set(key, value)
        
        if warnings:
            print(f"Configuration warnings: {warnings}")
        
        # Save to history
        self.config_history.append(dict(config.getKeys()))
    
    def create_config_template(self, name, config):
        """Create a reusable configuration template."""
        self.config_templates[name] = {
            'config': config,
            'created_at': time.time()
        }
    
    def load_config_template(self, name):
        """Load a configuration template."""
        if name not in self.config_templates:
            raise KeyError(f"Template '{name}' not found")
        
        template = self.config_templates[name]['config']
        return pynlm.createBrain(template)

# Use advanced configuration manager
config_manager = AdvancedConfigManager()

# Validate existing config
config = pynlm.createDefaultConfig()
errors, warnings = config_manager.validate_config(config)
if errors:
    print(f"Config errors: {errors}")
if warnings:
    print(f"Config warnings: {warnings}")

# Set advanced configuration
config_manager.set_advanced_config(
    config,
    brain_neuron_count=500,
    plasticity_stdp_enable=True,
    neuromod_curiosity_enable=True
)

# Get configuration range
range_config = config_manager.get_config_range(
    config, 'brain.', 'neuromod.'
)
print(f"Configuration range: {range_config}")
```

## Performance Monitoring and Analytics

### Advanced Performance Statistics

```python
import pynlm
import time
import json

class PerformanceMonitor:
    def __init__(self):
        self.step_times = []
        self.memory_usage = []
        self.neuron_activity = []
        self.plasticity_stats = []
    
    def record_step_performance(self, brain, step_time):
        """Record performance metrics for a simulation step.""
        self.step_times.append(step_time)
        
        # Record neuron statistics
        self.neuron_activity.append({
            'step': len(self.step_times),
            'firing_rate': brain.getAverageFiringRate(),
            'active_neurons': brain.getFiringNeuronCount(),
            'total_neurons': brain.getTotalNeuronCount()
        })
        
        # Record plasticity statistics
        if brain.getSTDP():
            self.plasticity_stats.append({
                'step': len(self.step_times),
                'stdp_weight_change': brain.getSTDP().getTotalWeightChange(),
                'hebbian_weight_change': brain.getHebbian().getTotalWeightChange()
            })
    
    def get_performance_stats(self):
        """Get comprehensive performance statistics.""
        if not self.step_times:
            return {}
        
        return {
            'simulation_stats': {
                'total_steps': len(self.step_times),
                'avg_step_time': sum(self.step_times) / len(self.step_times),
                'min_step_time': min(self.step_times),
                'max_step_time': max(self.step_times),
                'steps_per_second': len(self.step_times) / sum(self.step_times)
            },
            'neuron_stats': {
                'avg_firing_rate': sum(n['firing_rate'] for n in self.neuron_activity) / len(self.neuron_activity),
                'max_firing_rate': max(n['firing_rate'] for n in self.neuron_activity),
                'activity_variance': self._calculate_variance([n['firing_rate'] for n in self.neuron_activity])
            },
            'plasticity_stats': {
                'total_stdp_changes': sum(s['stdp_weight_change'] for s in self.plasticity_stats),
                'total_hebbian_changes': sum(s['hebbian_weight_change'] for s in self.plasticity_stats),
                'avg_stdp_change': sum(s['stdp_weight_change'] for s in self.plasticity_stats) / len(self.plasticity_stats) if self.plasticity_stats else 0
            }
        }
    
    def _calculate_variance(self, values):
        """Calculate variance of a list of values.""
        if len(values) < 2:
            return 0.0
        mean = sum(values) / len(values)
        return sum((x - mean) ** 2 for x in values) / len(values)
    
    def export_for_analysis(self, filepath):
        """Export performance data for external analysis.""
        data = {
            'performance_stats': self.get_performance_stats(),
            'neuron_activity': self.neuron_activity,
            'plasticity_stats': self.plasticity_stats,
            'timestamp': time.time()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)

# Create performance monitor
monitor = PerformanceMonitor()

# Run simulation with monitoring
def run_monitored_simulation(brain, steps=500):
    start_time = time.time()
    
    for step in range(steps):
        step_start = time.time()
        brain.step(step)
        step_time = time.time() - step_start
        
        monitor.record_step_performance(brain, step_time)
        
        if step % 100 == 0:
            print(f"Step {step} completed in {step_time:.4f}s")
    
    # Get and display performance stats
    stats = monitor.get_performance_stats()
    print(f"Performance: {stats['simulation_stats']}")
    
    # Export for analysis
    monitor.export_for_analysis("simulation_performance.json")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
run_monitored_simulation(brain, 500)
```

## Integration with External Tools

### Export and Analysis Integration

```python
import pynlm
import numpy as np

class AnalysisIntegration:
    def __init__(self):
        self.export_format = 'json'
        self.precision = 6
    
    def export_for_export(self, brain, filepath=None):
        """Export brain state for external analysis tools."""
        # Get comprehensive brain state
        state = {
            'neuron_data': {
                'total_neurons': brain.getTotalNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount()
            },
            'region_data': {
                'region_count': brain.getRegionCount(),
                'region_ids': [str(rid) for rid in brain.getRegionIds()]
            },
            'plasticity_data': {
                'stdp_enabled': brain.getSTDP() is not None,
                'hebbian_enabled': brain.getHebbian() is not None,
                'structural_enabled': brain.getStructuralPlasticity() is not None
            },
            'development_data': {
                'developmental_stage': brain.getDevelopmentalStage().name,
                'is_developing': brain.getDevelopmentalStage() != pynlm.DevelopmentalStage.Adult
            },
            'metadata': {
                'export_timestamp': time.time(),
                'export_format': self.export_format,
                'neuron_precision': self.precision
            }
        }
        
        if filepath:
            with open(filepath, 'w') as f:
                json.dump(state, f, indent=2)
        
        return state
    
    def import_from_external(self, filepath):
        """Import brain state from external analysis tools."""
        with open(filepath, 'r') as f:
            state = json.load(f)
        
        # Create new brain with imported state
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Import neuron data
        if 'neuron_data' in state:
            # Implementation would import neuron states
            pass
        
        return brain
    
    def create_brain_snapshot(self, brain, filename):
        """Create a snapshot of brain state for comparison."""
        snapshot = self.export_for_export(brain)
        snapshot['snapshot_name'] = filename
        snapshot['snapshot_timestamp'] = time.time()
        
        # Save snapshot
        snapshot_file = f"snapshots/{filename}_{int(time.time())}.json"
        with open(snapshot_file, 'w') as f:
            json.dump(snapshot, f, indent=2)
        
        return snapshot_file

# Use analysis integration
analyzer = AnalysisIntegration()

# Create brain snapshot
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

# Create snapshot for later comparison
snapshot_file = analyzer.create_brain_snapshot(brain, "initial_state")
print(f"Created brain snapshot: {snapshot_file}")

# Export for analysis
training_state = analyzer.export_for_export(brain, "training_state.json")
print("Exported brain state for analysis")
```

## Complete Advanced Example

```python
import pynlm
import time

def run_advanced_simulation(num_steps=1000):
    """Complete advanced simulation with all features."""
    
    # 1. Create advanced brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent with all features enabled
    agent = pynlm.createAgentBrain(brain)
    agent.enableAllFeatures(True)  # Enable all advanced features
    
    # 3. Create dynamic environment
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 12, 12)
    world.reset()
    world.setRandomSeed(42)  # Reproducible environment
    
    agent.initialize(world)
    
    # 4. Create behavioral controller
    from enum import Enum
    class Strategy(Enum):
        EXPLORATION = "exploration"
        EXPLOITATION = "exploitation"
        BALANCED = "balanced"
    
    class AdvancedBehavioralController:
        def __init__(self, agent):
            self.agent = agent
            self.strategy = Strategy.BALANCED
            self.strategy_changes = 0
        
        def switch_strategy(self, new_strategy):
            """Switch behavioral strategy with smooth transition.""
            self.strategy = new_strategy
            self.strategy_changes += 1
            
            # Smooth transition of neuromodulation levels
            if new_strategy == Strategy.EXPLORATION:
                self.agent.setCuriosityScale(2.0)
                self.agent.enableNoveltyDetection(True)
            elif new_strategy == Strategy.EXPLOITATION:
                self.agent.setCuriosityScale(0.2)
                self.agent.enableNoveltyDetection(False)
            else:  # BALANCED
                self.agent.setCuriosityScale(1.0)
                self.agent.enableNoveltyDetection(True)
    
    controller = AdvancedBehavioralController(agent)
    
    # 5. Performance monitoring
    class AdvancedPerformanceMonitor:
        def __init__(self):
            self.metrics = []
        
        def record_step(self, step, brain, agent, world_time):
            self.metrics.append({
                'step': step,
                'time': world_time,
                'neuron_count': brain.getTotalNeuronCount(),
                'firing_count': brain.getFiringNeuronCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'developmental_stage': brain.getDevelopmentalStage().name,
                'reward_modulation': agent.isRewardModulationEnabled()
            })
        
        def get_summary(self):
            if not self.metrics:
                return {}
            
            return {
                'total_steps': len(self.metrics),
                'average_firing_rate': np.mean([m['firing_rate'] for m in self.metrics]),
                'max_firing_rate': np.max([m['firing_rate'] for m in self.metrics]),
                'min_firing_rate': np.min([m['firing_rate'] for m in self.metrics]),
                'final_curiosity': self.metrics[-1]['curiosity'],
                'final_novelty': self.metrics[-1]['novelty'],
                'strategy_changes': len(set(m['developmental_stage'] for m in self.metrics))
            }
    
    monitor = AdvancedPerformanceMonitor()
    
    # 6. Main simulation loop
    strategy_switch_interval = 200
    
    for step in range(num_steps):
        # Switch strategies at intervals
        if step % strategy_switch_interval == 0:
            strategy = list(Strategy)[(step // strategy_switch_interval) % len(Strategy)]
            controller.switch_strategy(strategy)
        
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record metrics
        monitor.record_step(step, brain, agent, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing rate: {brain.getAverageFiringRate():.3f}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Strategy: {controller.strategy.value}")
    
    # 7. Final analysis
    summary = monitor.get_summary()
    print(f"\nSimulation Summary:")
    print(f"Total steps: {summary['total_steps']}")
    print(f"Average firing rate: {summary['average_firing_rate']:.3f}")
    print(f"Max firing rate: {summary['max_firing_rate']:.3f}")
    print(f"Final curiosity level: {summary['final_curiosity']:.3f}")
    print(f"Strategy changes: {summary['strategy_changes']}")
    
    # 8. Export results
    analyzer = AnalysisIntegration()
    analyzer.create_brain_snapshot(brain, "final_state")
    
    return brain, agent, world, summary

# Run the complete advanced simulation
print("Starting advanced NLM simulation...")
brain, agent, world, summary = run_advanced_simulation(1000)
print("Simulation completed successfully!")
```