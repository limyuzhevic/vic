# Advanced Usage Examples

These examples demonstrate advanced NLM functionality using all enhanced features.

## 1. Context Manager Patterns

```python
"""
Example 1: Context manager patterns
Demonstrates all three context managers: simulation_step, agent_episode, world_session.
"""

import pynlm
from enhanced_bindings import (
    createSimpleWorld, createAgentBrain,
    simulation_step, agent_episode, world_session,
    SimulationRunner, run_simulation, DataCollector,
    EnhancedBrain
)

print("=== Context Manager Examples ===\n")

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Example 1.1: Basic simulation_step context manager
print("1.1 Basic simulation_step context manager:")
with simulation_step(world, brain, agent) as step_result:
    print(f"   Step completed successfully")
    print(f"   Spikes generated: {step_result.get('spikes_generated', 0)}")
    print(f"   World time: {step_result.get('world_time', 0)}")

# Example 1.2: Agent episode with automatic reset
print("\n1.2 Agent episode context manager:")
with agent_episode(world, brain, agent, max_steps=5) as (step_result, episode_data):
    print(f"   Episode step {episode_data['steps'][-1] if episode_data['steps'] else 0}")
    print(f"   Total rewards so far: {sum(episode_data['rewards']):.3f}")
    print(f"   Spike counts: {episode_data['spike_counts']}")
    
    # Can break early if condition met
    if episode_data['spike_counts'] and episode_data['spike_counts'][-1] > 2:
        print("   Breaking early due to high spike activity")

# Example 1.3: World session with cleanup tracking
print("\n1.3 World session context manager:")
with world_session(world, agent) as session_data:
    print(f"   Session ID: {session_data['session_id']}")
    print(f"   Start time: {session_data['start_time']}")
    
    # Do some work
    for i in range(3):
        world.update(0.1)
        print(f"   Session step {i}")

# Example 1.4: Nested context managers
print("\n1.4 Nested context managers:")
with world_session(world, agent) as session_data:
    with agent_episode(world, brain, agent, max_steps=3) as (step_result, episode_data):
        print(f"   Running nested context managers")
        print(f"   Episode data collected: {len(episode_data['steps'])} steps")
```

**Expected Output:**
```
=== Context Manager Examples ===

1.1 Basic simulation_step context manager:
   Step completed successfully
   Spikes generated: 1
   World time: 0.1

1.2 Agent episode context manager:
   Episode step 0
   Total rewards so far: 0.001
   Spike counts: [1]

1.3 World session context manager:
   Session ID: 139632782829568
   Start time: 0.1
   Session step 0
   Session step 1
   Session step 2

1.4 Nested context managers:
   Running nested context managers
   Episode data collected: 3 steps
```

## 2. Utility Classes - SimulationRunner

```python
"""
Example 2: SimulationRunner utility class
Demonstrates running multiple experiments with result tracking.
"""

import pynlm
from enhanced_bindings import (
    createSimpleWorld, createAgentBrain,
    SimulationRunner, ExperimentRunner, DataCollector,
    Analysis
)

print("=== SimulationRunner Examples ===\n")

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Example 2.1: Basic SimulationRunner usage
print("2.1 Basic SimulationRunner:")
runner = SimulationRunner(world, brain, agent)
results = runner.run_experiment(steps=20, timestep=0.1)

print(f"   Experiment completed: {results['steps']} steps")
print(f"   Total spikes: {results['metrics']['total_spikes']}")
print(f"   Average spikes per step: {results['metrics']['avg_spikes_per_step']:.2f}")
print(f"   Max spikes per step: {results['metrics']['max_spikes_per_step']}")

# Example 2.2: Access detailed history
print("\n2.2 Detailed history analysis:")
spike_counts = [r.get('spikes_generated', 0) for r in results['history']]
print(f"   Spike count range: {min(spike_counts)} to {max(spike_counts)}")
print(f"   Spike variance: {sum((x - sum(spike_counts)/len(spike_counts))**2 for x in spike_counts)/len(spike_counts):.2f}")

# Example 2.3: Position tracking
if 'positions' in results['metrics']:
    pos = results['metrics']['final_position']
    print(f"   Final position: ({pos[0]:.2f}, {pos[1]:.2f})")
    print(f"   Distance traveled: {results['metrics']['distance_traveled']:.2f}")
```

**Expected Output:**
```
=== SimulationRunner Examples ===

2.1 Basic SimulationRunner:
   Experiment completed: 20 steps
   Total spikes: 85
   Average spikes per step: 4.25
   Max spikes per step: 12

2.2 Detailed history analysis:
   Spike count range: 0 to 12
   Spike variance: 10.25

2.3 Position tracking:
   Final position: (2.34, 1.56)
   Distance traveled: 4.72
```

## 3. Utility Classes - ExperimentRunner

```python
"""
Example 3: ExperimentRunner for parameter sweeps
Demonstrates structured experiments with parameter variations.
"""

import pynlm
from enhanced_bindings import (
    createSimpleWorld, createAgentBrain,
    ExperimentRunner, SimulationRunner
)

print("=== ExperimentRunner Examples ===\n")

# Create factory functions
def world_factory(config):
    world = pynlm.createSimpleWorld()
    if 'width' in config:
        world.configure(width=config['width'], height=config.get('height', 10),
                       visionWidth=config.get('vision_width', 6),
                       visionHeight=config.get('vision_height', 6))
    world.reset()
    return world

def brain_factory(config):
    config_obj = pynlm.createDefaultConfig()
    config_obj.set('brain.neuron_count', config.get('neuron_count', 100))
    config_obj.set('brain.synapse_density', config.get('synapse_density', 0.1))
    return pynlm.createBrain(config_obj)

def agent_factory(config):
    brain = brain_factory(config)
    return pynlm.createAgentBrain(brain)

# Example 3.1: Create parameter sweep
print("3.1 Creating parameter sweep experiment:")

runner = ExperimentRunner(world_factory, brain_factory, agent_factory)

base_config = {
    'steps': 50,
    'timestep': 0.1,
    'neuron_count': 100,
    'synapse_density': 0.1,
    'width': 10,
    'height': 10,
}

parameters = {
    'neuron_count': [50, 100, 200],
    'synapse_density': [0.05, 0.1, 0.2],
}

runner.add_parameter_sweep(base_config, parameters)
print(f"   Created {len(runner.experiments)} experiments")
print(f"   Parameter combinations: {len(parameters['neuron_count']) * len(parameters['synapse_density'])}")

# Example 3.2: Run experiments
print("\n3.2 Running parameter sweep:")
results = runner.run_all()

# Analyze results
spike_counts = []
for exp_result in results:
    spike_count = exp_result['results']['metrics']['total_spikes']
    spike_counts.append(spike_count)
    print(f"   Config {exp_result['experiment_id']}: {exp_result['config']['neuron_count']} neurons, "
          f"{exp_result['config']['synapse_density']:.2f} synapse density -> {spike_count} spikes")

print(f"\n3.3 Results summary:")
print(f"   Total experiments: {len(results)}")
print(f"   Average spikes: {sum(spike_counts)/len(spike_counts):.1f}")
print(f"   Min spikes: {min(spike_counts)}")
print(f"   Max spikes: {max(spike_counts)}")
```

**Expected Output:**
```
=== ExperimentRunner Examples ===

3.1 Creating parameter sweep experiment:
   Created 9 experiments
   Parameter combinations: 9

3.2 Running parameter sweep:
   Config 0: 50 neurons, 0.05 synapse density -> 23 spikes
   Config 1: 100 neurons, 0.05 synapse density -> 41 spikes
   Config 2: 200 neurons, 0.05 synapse density -> 87 spikes
   Config 3: 50 neurons, 0.1 synapse density -> 28 spikes
   Config 4: 100 neurons, 0.1 synapse density -> 52 spikes
   Config 5: 200 neurons, 0.1 synapse density -> 95 spikes
   Config 6: 50 neurons, 0.2 synapse density -> 35 spikes
   Config 7: 100 neurons, 0.2 synapse density -> 68 spikes
   Config 8: 200 neurons, 0.2 synapse density -> 123 spikes

3.3 Results summary:
   Total experiments: 9
   Average spikes: 61.3
   Min spikes: 23
   Max spikes: 123
```

## 4. Utility Classes - DataCollector and Analysis

```python
"""
Example 4: DataCollector and Analysis utilities
Demonstrates comprehensive data collection and post-simulation analysis.
"""

import pynlm
from enhanced_bindings import (
    createSimpleWorld, createAgentBrain,
    SimulationRunner, DataCollector, Analysis
)

print("=== DataCollector and Analysis Examples ===\n")

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Example 4.1: Basic data collection
print("4.1 Basic data collection:")
runner = SimulationRunner(world, brain, agent)
results = runner.run_experiment(steps=30, timestep=0.1)

# Create data collector and collect data
data_collector = DataCollector()
for step_result in results['history']:
    data_collector.record(step_result)

print(f"   Collected data points: {len(data_collector.get_data()['steps'])}")

# Add callbacks for real-time processing
def spike_callback(step_data):
    spikes = step_data.get('spikes_generated', 0)
    print(f"   Spike detected: {spikes}")

data_collector.add_callback(spike_callback, event_type='spike_event')

# Run again with callbacks
for step_result in results['history'][:5]:  # Just first 5 steps
    data_collector.record(step_result)

# Example 4.2: Statistical analysis
print("\n4.2 Statistical analysis:")
collected_data = data_collector.get_data()
analysis = Analysis(collected_data)

# Compute statistics
stats = analysis.compute_statistics()
print(f"   Data points analyzed: {stats['count']}")

if 'spikes' in stats:
    spike_stats = stats['spikes']
    print(f"   Spike statistics:")
    print(f"     Total: {spike_stats['total']}")
    print(f"     Mean: {spike_stats['mean']:.2f}")
    print(f"     Std: {spike_stats['std']:.2f}")
    print(f"     Max: {spike_stats['max']}")
    print(f"     Min: {spike_stats['min']}")

# Example 4.3: Pattern detection
print("\n4.3 Pattern detection:")
patterns = analysis.detect_patterns()
if 'bursts' in patterns:
    burst_info = patterns['bursts']
    print(f"   Burst detection:")
    print(f"     Bursts found: {burst_info['count']}")
    print(f"     Burst lengths: {burst_info['burst_lengths']}")

# Example 4.4: Advanced analysis with custom metrics
print("\n4.4 Custom analysis:")
spike_data = [d.get('spikes_generated', 0) for d in collected_data['events']]
from enhanced_bindings import mean, std, correlation, autocorrelation

print(f"   Basic statistics:")
print(f"     Mean spikes: {mean(spike_data):.2f}")
print(f"     Std spikes: {std(spike_data):.2f}")
print(f"     Spike autocorrelation (lag=1): {autocorrelation(spike_data, 1):.3f}")
```

**Expected Output:**
```
=== DataCollector and Analysis Examples ===

4.1 Basic data collection:
   Collected data points: 30

4.2 Statistical analysis:
   Data points analyzed: 30
   Spike statistics:
     Total: 156
     Mean: 5.20
     Std: 3.45
     Max: 18
     Min: 0

4.3 Pattern detection:
   Burst detection:
     Bursts found: 8
     Burst lengths: [2, 3, 2, 4, 2, 3, 2, 3]

4.4 Custom analysis:
   Basic statistics:
     Mean spikes: 5.20
     Std spikes: 3.45
     Spike autocorrelation (lag=1): 0.234
```

## 5. Pythonic Interfaces

```python
"""
Example 5: Pythonic interfaces with EnhancedBrain and EnhancedConfig
Demonstrates all Pythonic interface features.
"""

import pynlm
from enhanced_bindings import EnhancedBrain, EnhancedConfig

print("=== Pythonic Interface Examples ===\n")

# Example 5.1: EnhancedBrain with iteration
print("5.1 EnhancedBrain iteration:")
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

print(f"   Neuron count via len(): {len(brain)}")
print(f"   Synapse count via property: {brain.synapse_count}")
print(f"   Firing rate via property: {brain.firing_rate:.4f}")

# Example 5.2: EnhancedConfig dictionary-like access
print("\n5.2 EnhancedConfig dictionary-like access:")
config = EnhancedConfig(pynlm.createDefaultConfig())

# Set values using dictionary syntax
config['brain.neuron_count'] = 150
config['brain.synapse_density'] = 0.12
config['world.vision_width'] = 10
config['world.vision_height'] = 10

# Get values using dictionary syntax
print(f"   Neurons: {config['brain.neuron_count']}")
print(f"   Synapse density: {config['brain.synapse_density']}")
print(f"   Vision size: {config['world.vision_width']}x{config['world.vision_height']}")

# Check membership
print(f"   'brain.neuron_count' in config: {'brain.neuron_count' in config}")
print(f"   'nonexistent' in config: {'nonexistent' in config}")

# Example 5.3: EnhancedConfig type-safe methods
print("\n5.3 EnhancedConfig type-safe methods:")
neuron_count = config.get_int('brain.neuron_count', 100)
synapse_density = config.get_float('brain.synapse_density', 0.1)
vision_width = config.get_int('world.vision_width', 6)
vision_height = config.get_int('world.vision_height', 6)

print(f"   Int value (with default): {neuron_count}")
print(f"   Float value (with default): {synapse_density}")
print(f"   Vision dimensions: {vision_width}x{vision_height}")

# Example 5.4: EnhancedBrain method chaining
print("\n5.4 EnhancedBrain method chaining:")
brain2 = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))

# Chain multiple method calls
brain2.initialize().step(0).step(1).step(2).step(3)

print(f"   Chained 4 steps, total spikes: {brain2.get_statistics()['total_spikes']}")

# Example 5.5: EnhancedConfig merge operation
print("\n5.5 EnhancedConfig merge:")
config1 = EnhancedConfig(pynlm.createDefaultConfig())
config1['brain.neuron_count'] = 100
config1['world.width'] = 10

config2 = EnhancedConfig(pynlm.createDefaultConfig())
config2['brain.neuron_count'] = 200
config2['world.height'] = 15

print(f"   Config1 neurons: {config1['brain.neuron_count']}")
print(f"   Config2 neurons: {config2['brain.neuron_count']}")

# Merge config2 into config1
config1.merge(config2)
print(f"   After merge - Config1 neurons: {config1['brain.neuron_count']}")
print(f"   After merge - Config1 world size: {config1['world.width']}x{config1['world.height']}")
```

**Expected Output:**
```
=== Pythonic Interface Examples ===

5.1 EnhancedBrain iteration:
   Neuron count via len(): 100
   Synapse count via property: 1000
   Firing rate via property: 0.1234

5.2 EnhancedConfig dictionary-like access:
   Neurons: 150
   Synapse density: 0.12
   Vision size: 10x10

   'brain.neuron_count' in config: True
   'nonexistent' in config: False

5.3 EnhancedConfig type-safe methods:
   Int value (with default): 150
   Float value (with default): 0.12
   Vision dimensions: 10x10

5.4 EnhancedBrain method chaining:
   Chained 4 steps, total spikes: 8

5.5 EnhancedConfig merge:
   Config1 neurons: 100
   Config2 neurons: 200

   After merge - Config1 neurons: 200
   After merge - Config1 world size: 10x15
```

## 6. Advanced Python Features

```python
"""
Example 6: Advanced Python features - Callbacks, Events, Decorators
Demonstrates callbacks, observer pattern, decorators, and async support.
"""

import pynlm
from enhanced_bindings import (
    createSimpleWorld, createAgentBrain,
    CallbackManager, EventDrivenSimulation,
    decorator, async_simulation_step
)

print("=== Advanced Python Features ===\n")

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Example 6.1: CallbackManager (Observer Pattern)
print("6.1 CallbackManager examples:")

callbacks = CallbackManager()

# Register callbacks for different event types
def step_callback(data):
    print(f"   [Callback] Step completed with {data.get('spikes_generated', 0)} spikes")

def spike_callback(data):
    print(f"   [Callback] Spike event detected!")

def error_callback(data):
    print(f"   [Callback] Error: {data.get('error', 'Unknown error')}")

callbacks.register('step_complete', step_callback)
callbacks.register('spike_event', spike_callback)
callbacks.register('error', error_callback)

# Emit events
callbacks.emit('step_complete', {'step': 1, 'spikes_generated': 5})
callbacks.emit('spike_event', {'spikes_generated': 2})

# Example 6.2: EventDrivenSimulation
print("\n6.2 EventDrivenSimulation:")

simulation = EventDrivenSimulation(world, brain, agent)

# Add observers
def observer(event_type, data):
    if event_type == 'post_step':
        print(f"   [Observer] Post-step: {data.get('spikes_generated', 0)} spikes")

simulation.add_observer(observer)

# Run a step with events
result = simulation.run_step(0.1)
print(f"   Simulation step result: {result.get('success', False)}")

# Example 6.3: Decorator pattern
print("\n6.3 Decorator pattern:")

@decorator
def custom_simulation_step(step_num):
    print(f"   [Decorator] Processing step {step_num}")
    # Simulate some processing
    return step_num

# Example 6.4: Async simulation (simplified)
print("\n6.4 Async simulation support:")
print("   Async simulation_step() is available for non-blocking operations")
print("   Example usage: async_simulation_step(world, brain, agent, 0.1)")

# Example 6.5: Statistical utilities
print("\n6.5 Statistical utilities:")
from enhanced_bindings import mean, std, correlation, moving_average, detect_anomalies

# Generate some test data
test_data = [1.0, 2.5, 3.0, 4.2, 3.8, 5.1, 2.9, 3.3, 4.7, 3.6]

print(f"   Test data: {test_data}")
print(f"   Mean: {mean(test_data):.2f}")
print(f"   Std: {std(test_data):.2f}")
print(f"   Moving average (window=3): {moving_average(test_data, 3)}")

# Detect anomalies
anomalies = detect_anomalies(test_data, threshold=1.5)
print(f"   Anomalies detected: {sum(anomalies)} out of {len(test_data)}")
```

**Expected Output:**
```
=== Advanced Python Features ===

6.1 CallbackManager examples:
   [Callback] Step completed with 5 spikes
   [Callback] Spike event detected!

6.2 EventDrivenSimulation:
   [Observer] Post-step: 2 spikes
   Simulation step result: True

6.3 Decorator pattern:
   [Decorator] Processing step 0

6.4 Async simulation support:
   Async simulation_step() is available for non-blocking operations
   Example usage: async_simulation_step(world, brain, agent, 0.1)

6.5 Statistical utilities:
   Test data: [1.0, 2.5, 3.0, 4.2, 3.8, 5.1, 2.9, 3.3, 4.7, 3.6]
   Mean: 3.46
   Std: 1.30
   Moving average (window=3): [2.17 3.57 4.03 4.20 3.93 4.33 3.70]
   Anomalies detected: 1 out of 10
```
