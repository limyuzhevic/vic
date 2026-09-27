# Practical Examples: NLM Python Bindings Improvements

This file demonstrates the practical improvements made to the NLM Python bindings, showing the enhanced API in action.

## Example 1: Simple Brain Simulation with Enhanced API

```python
import pynlm

# Enhanced factory methods with presets
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)

# Initialize with validation
if not brain.ensure_initialized():
    print("Failed to initialize brain")
    exit(1)

# Batch simulation with profiling
print("Running simple simulation...")
profile = brain.profile_simulation(100)
print(f"Simulation completed in {profile.total_time:.2f} seconds")
print(f"Average step time: {profile.average_step_time:.4f} seconds")

# Export neural activity for analysis
brain.export_neural_activity("simple_brain_activity.json")
print("Neural activity exported to JSON")
```

## Example 2: Advanced Agent Simulation with Enhanced API

```python
import pynlm
import time

# Create complete agent setup
config = pynlm.create_default_config()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

brain = pynlm.create_brain(config)
world = pynlm.create_world_2d(width=30, height=30, vision_width=8, vision_height=8)
world.set_agent_start(15.0, 15.0)
world.set_max_energy(1000.0)

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable all features for advanced simulation
agent.enable_all_features()

print("Starting advanced agent simulation...")
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Expected sensory input size: {agent.getSensoryInputSize()}")
print(f"Expected motor output size: {agent.getMotorOutputSize()}")

# Enhanced simulation loop with monitoring
episode_stats = {
    'steps': 0,
    'total_spikes': 0,
    'curiosity_levels': [],
    'novelty_levels': [],
    'rewards': []
}

start_time = time.time()
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent (enhanced with validation)
    if agent.process_sensory_input(percept):
        # Process successfully
        episode_stats['steps'] += 1
    
    # Brain step with error handling
    brain.step(step)
    
    # Decode action with curiosity-based exploration
    action = agent.decode_motor_command()
    
    # Apply action with reward feedback
    if action:
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Get reward (simplified)
        reward = 1.0 if action.type == pynlm.ActionType.MoveForward else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        episode_stats['rewards'].append(reward)
    
    # Collect statistics
    episode_stats['total_spikes'] += brain.getFiringNeuronCount()
    episode_stats['curiosity_levels'].append(agent.getCuriosityLevel())
    episode_stats['novelty_levels'].append(agent.getNoveltyLevel())
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: "
              f"Firing neurons: {brain.getFiringNeuronCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}")

end_time = time.time()
print(f"Episode completed in {end_time - start_time:.2f} seconds")
print(f"Statistics: {episode_stats}")

# Export episode data
agent.export_episode_data("agent_episode_001.json")
brain.export_neural_activity("agent_brain_activity.json")
```

## Example 3: Learning and Development Simulation

```python
import pynlm
import json

# Create brain with development enabled
config = pynlm.create_default_config()
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.create_brain(config)
agent = pynlm.create_agent_brain(brain)

# Create a simple world with resources
world = pynlm.create_world_preset("explorer")
world.configure(width=20, height=20, vision_width=8, vision_height=8)
agent.initialize(world)

# Enable advanced learning features
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

print("Starting learning and development simulation...")
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Learning simulation with development tracking
learning_session = {
    'steps': 0,
    'development_stages': [],
    'learning_curves': {
        'plasticity': [],
        'curiosity': [],
        'novelty': []
    },
    'episodes': []
}

for episode in range(5):
    print(f"\n--- Episode {episode + 1} ---")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Reset agent for new episode
    agent.reset_for_episode()
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Run episode
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.process_sensory_input(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply action
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward modulation
        reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Record statistics
        learning_session['learning_curves']['plasticity'].append(
            agent.get_neuromodulation_level()
        )
        learning_session['learning_curves']['curiosity'].append(
            agent.get_curiosity_level()
        )
        learning_session['learning_curves']['novelty'].append(
            agent.get_novelty_level()
        )
        learning_session['development_stages'].append(
            brain.get_developmental_stage()
        )
        
        # Print development progress
        if step % 50 == 0:
            print(f"  Step {step}: "
                  f"Dev Stage: {brain.get_developmental_stage()}, "
                  f"Curiosity: {agent.get_curiosity_level():.3f}")
    
    # Record episode data
    learning_session['episodes'].append({
        'episode': episode + 1,
        'final_dev_stage': brain.get_developmental_stage(),
        'final_curiosity': agent.get_curiosity_level(),
        'final_novelty': agent.get_novelty_level(),
        'steps_completed': 200
    })
    
    print(f"Episode {episode + 1} completed")
    print(f"Final developmental stage: {brain.get_developmental_stage()}")

# Analyze learning patterns
print("\n=== Learning Analysis ===")
learning_session['average_plasticity'] = sum(
    learning_session['learning_curves']['plasticity']
) / len(learning_session['learning_curves']['plasticity'])
learning_session['average_curiosity'] = sum(
    learning_session['learning_curves']['curiosity']
) / len(learning_session['learning_curves']['curiosity'])
learning_session['average_novelty'] = sum(
    learning_session['learning_curves']['novelty']
) / len(learning_session['learning_curves']['novelty'])

print(f"Average plasticity: {learning_session['average_plasticity']:.3f}")
print(f"Average curiosity: {learning_session['average_curiosity']:.3f}")
print(f"Average novelty: {learning_session['average_novelty']:.3f}")

# Save learning session
try:
    with open("learning_session.json", 'w') as f:
        json.dump(learning_session, f, indent=2, default=str)
    print("Learning session saved to 'learning_session.json'")
except Exception as e:
    print(f"Error saving learning session: {e}")
```

## Example 4: Performance Optimization and Profiling

```python
import pynlm
import time
import matplotlib.pyplot as plt

# Create brain with performance monitoring
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.enable_performance_monitoring()

# Create agent
world = pynlm.create_world_2d(width=10, height=10, vision_width=6, vision_height=6)
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

print("Starting performance profiling...")
print(f"Brain configured with {brain.getTotalNeuronCount()} neurons")

# Run multiple simulations with different step counts
test_scenarios = [
    {'steps': 100, 'description': 'Short test'},
    {'steps': 500, 'description': 'Medium test'},
    {'steps': 1000, 'description': 'Long test'}
]

performance_results = []

for scenario in test_scenarios:
    print(f"\n--- {scenario['description']} ({scenario['steps']} steps) ---")
    
    # Reset brain for fair comparison
    brain.reset()
    agent.reset_for_episode()
    world.reset()
    
    # Run simulation
    start_time = time.time()
    for step in range(scenario['steps']):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
    
    end_time = time.time()
    
    # Collect performance metrics
    profile = brain.get_simulation_profile()
    
    result = {
        'description': scenario['description'],
        'total_steps': scenario['steps'],
        'wall_clock_time': end_time - start_time,
        'average_step_time': profile.average_step_time,
        'memory_usage': profile.memory_usage,
        'peak_memory_usage': profile.peak_memory_usage,
        'spike_count': brain.get_total_spike_count(),
        'firing_neuron_count': brain.get_firing_neuron_count(),
        'performance_score': profile.performance_score
    }
    
    performance_results.append(result)
    
    print(f"Wall-clock time: {result['wall_clock_time']:.2f} seconds")
    print(f"Average step time: {result['average_step_time']*1000:.2f} ms")
    print(f"Performance score: {result['performance_score']:.2f}")
    print(f"Total spikes: {result['spike_count']}")

# Generate performance report
print("\n=== Performance Analysis ===")
for result in performance_results:
    print(f"{result['description']}:")
    print(f"  Steps/sec: {result['total_steps'] / result['wall_clock_time']:.1f}")
    print(f"  Memory efficiency: {result['spike_count'] / result['peak_memory_usage']:.1f} spikes/MB")

# Create performance visualization
plt.figure(figsize=(15, 10))

plt.subplot(2, 2, 1)
plt.plot([r['total_steps'] for r in performance_results],
         [r['wall_clock_time'] for r in performance_results], 'bo-')
plt.xlabel('Number of Steps')
plt.ylabel('Wall-clock Time (seconds)')
plt.title('Performance Scaling')
plt.grid(True)

plt.subplot(2, 2, 2)
plt.bar([r['description'] for r in performance_results],
        [r['performance_score'] for r in performance_results])
plt.ylabel('Performance Score')
plt.title('Performance Comparison')
plt.grid(True)

plt.subplot(2, 2, 3)
step_counts = [r['total_steps'] for r in performance_results]
step_times = [r['average_step_time'] * 1000 for r in performance_results]
plt.plot(step_counts, step_times, 'ro-')
plt.xlabel('Number of Steps')
plt.ylabel('Average Step Time (ms)')
plt.title('Step Time Scaling')
plt.grid(True)

plt.subplot(2, 2, 4)
memory_usage = [r['peak_memory_usage'] for r in performance_results]
plt.bar([r['description'] for r in performance_results], memory_usage)
plt.ylabel('Peak Memory Usage (MB)')
plt.title('Memory Usage Comparison')
plt.grid(True)

plt.tight_layout()
plt.savefig('performance_analysis.png')
print("\nPerformance visualization saved to 'performance_analysis.png'")

# Generate optimization suggestions
optimization_suggestions = brain.get_optimization_suggestions()
print("\n=== Optimization Suggestions ===")
for i, suggestion in enumerate(optimization_suggestions, 1):
    print(f"{i}. {suggestion}")
```

## Example 5: Batch Operations and Memory Management

```python
import pynlm
import gc

# Create brain with memory tracking
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.enable_memory_tracking()

# Create world and agent
world = pynlm.create_world_preset("large")
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

print("Starting batch operations and memory management test...")

# Create test neurons for batch operations
neuron_ids = []
for i in range(100):
    # Add regions and get neurons
    region_id = brain.add_region(f"TestRegion_{i}")
    region = brain.get_region(region_id)
    if region:
        # Get neurons (implementation depends on actual API)
        neurons = region.getNeurons() if hasattr(region, 'getNeurons') else []
        for neuron in neurons[:2]:  # Take first 2 neurons per region
            neuron_ids.append(neuron.getId())

print(f"Created test neurons: {len(neuron_ids)}")

# Test batch operations
print("\n--- Testing Batch Operations ---")

# Test batch current injection
if len(neuron_ids) >= 10:
    test_neurons = neuron_ids[:10]
    current_values = [1.0 + i * 0.1 for i in range(10)]
    
    print("Testing batch current injection...")
    for neuron_id, current in zip(test_neurons, current_values):
        if brain.injectCurrent(neuron_id, current):
            print(f"  Injected {current} to neuron {neuron_id}")
        else:
            print(f"  Failed to inject to neuron {neuron_id}")

# Test batch action production
print("\nTesting batch action production...")
actions = []
for step in range(5):
    action = brain.produce_action()
    actions.append(action)
    print(f"  Step {step}: {action}")

# Test batch developmental stage changes
print("\nTesting batch developmental stage changes...")
for i in range(5):
    new_stage = pynlm.DevelopmentalStage.Maturation
    brain.set_developmental_stage(new_stage)
    print(f"  Set developmental stage to: {brain.get_developmental_stage()}")

# Memory management test
print("\n--- Memory Management Test ---")
print("Initial memory state:")
memory_stats = brain.get_memory_statistics()
for key, value in memory_stats.items():
    print(f"  {key}: {value}")

# Create some objects and force garbage collection
print("\nCreating objects and checking memory...")
temp_objects = []
for i in range(5):
    # Create temporary objects
    temp_objects.append(pynlm.create_brain(pynlm.create_default_config()))
    if i % 2 == 0:
        print(f"  Created batch {i//2 + 1} - checking memory...")
        gc.collect()
        memory_stats = brain.get_memory_statistics()
        print(f"    Active objects: {memory_stats.get('active_objects', 'N/A')}")

print(f"\nCreated {len(temp_objects)} temporary objects")
print("Cleaning up...")
temp_objects.clear()
gc.collect()
print("Memory cleanup completed")

# Final memory check
print("\nFinal memory state:")
memory_stats = brain.get_memory_statistics()
for key, value in memory_stats.items():
    print(f"  {key}: {value}")

print("\nBatch operations and memory management test completed!")
```

## Example 6: Advanced Configuration Management

```python
import pynlm
import json

# Create and manage advanced configuration
print("=== Advanced Configuration Management ===")

# Create base configuration
base_config = pynlm.create_default_config()

# Set advanced parameters
base_config.set("brain.neuron_count", 3000)
base_config.set("brain.synapse_density", 0.12)
base_config.set("brain.connection_probability", 0.08)
base_config.set("brain.v_thresh", -55.0)  # Higher threshold
base_config.set("brain.v_rest", -65.0)
base_config.set("brain.v_reset", -70.0)
base_config.set("brain.tau_mem", 25.0)  # Slower membrane time constant
base_config.set("brain.tau_ref", 3.0)

# Plasticity configuration
base_config.set("plasticity.stdp.enable", True)
base_config.set("plasticity.stdp.learning_rate", 0.002)
base_config.set("plasticity.stdp.tau_plus", 25.0)
base_config.set("plasticity.stdp.tau_minus", 25.0)
base_config.set("plasticity.hebbian.enable", True)
base_config.set("plasticity.structural.enable", True)
base_config.set("synaptogenesis_rate", 0.00005)
base_config.set("pruning_rate", 0.000005)

# Neuromodulation configuration
base_config.set("neuromod.dopamine.scale", 1.2)
base_config.set("neuromod.curiosity.enable", True)
base_config.set("neuromod.curiosity.baseline", 0.1)
base_config.set("neuromod.novelty.enable", True)
base_config.set("neuromod.novelty.sensitivity", 0.8)

# Development configuration
base_config.set("development.stage_duration", 120.0)
base_config.set("development.critical_period_end", 60.0)
base_config.set("development.maturation_start", 60.0)

# Simulation configuration
base_config.set("simulation.timestep", 0.001)
base_config.set("simulation.max_steps", 10000)
base_config.set("simulation.checkpoint_interval", 1000)

# Save configuration
config_file = "advanced_config.json"
base_config.save_to_file(config_file)
print(f"Configuration saved to '{config_file}'")

# Load and validate configuration
loaded_config = pynlm.create_default_config()
loaded_config.load_from_file(config_file)

print("\nLoaded configuration:")
print(f"Neuron count: {loaded_config.get('brain.neuron_count', 'N/A')}")
print(f"Synapse density: {loaded_config.get('brain.synapse_density', 'N/A')}")
print(f"STDP enabled: {loaded_config.get('plasticity.stdp.enable', 'N/A')}")
print(f"Curiosity enabled: {loaded_config.get('neuromod.curiosity.enable', 'N/A')}")

# Configuration validation
print("\nValidating configuration...")
if base_config.validate():
    print("✓ Configuration is valid")
else:
    print("✗ Configuration validation failed")

# Get configuration summary
print("\nConfiguration summary:")
summary = base_config.summary()
print(summary)

# Compare with default
print("\nComparing with default configuration...")
default_config = pynlm.create_default_config()
differences = []
for key in base_config.get_keys():
    default_value = default_config.get(key)
    current_value = base_config.get(key)
    if default_value != current_value:
        differences.append(f"{key}: {default_value} → {current_value}")

if differences:
    print(f"Differences ({len(differences)}):")
    for diff in differences:
        print(f"  {diff}")
else:
    print("No differences from default configuration")

# Generate configuration from preset
print("\nGenerating configuration from preset 'advanced_research'...")
preset_config = pynlm.generate_config_from_preset("advanced_research")
preset_config.save_to_file("preset_advanced_config.json")
print("Preset configuration saved to 'preset_advanced_config.json'")

# Get suggested parameters for specific use case
print("\nGetting suggested parameters for 'performance' use case...")
suggested_params = pynlm.get_suggested_parameters("performance")
for param, value in suggested_params.items():
    print(f"  {param}: {value}")

print("\nAdvanced configuration management completed!")
```

## Example 7: Debugging and Development Tools

```python
import pynlm

# Create brain with debugging enabled
print("=== Debugging and Development Tools ===")

config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.enable_debug_mode()

# Create world and agent
world = pynlm.create_world_2d(width=15, height=15, vision_width=8, vision_height=8)
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

print("Debug mode enabled. Starting simulation with monitoring...")

# Run simulation with debugging
for step in range(100):
    world.update(0.1)
    percept = world.get_sensory_percept()
    
    # Debug: Check brain state
    if brain.getFiringNeuronCount() == 0 and step > 10:
        print(f"Warning: No firing neurons at step {step}")
    
    # Debug: Check agent health
    if agent.getCuriosityLevel() > 0.9:
        print(f"High curiosity at step {step}: {agent.getCuriosityLevel():.3f}")
    
    # Debug: Check memory usage
    if step % 20 == 0:
        memory_stats = brain.get_memory_statistics()
        print(f"Step {step}: Memory usage - {memory_stats.get('active_objects', 'N/A')}")
    
    # Process sensory input
    agent.process_sensory_input(percept)
    
    # Brain step
    brain.step(step)
    
    # Debug: Check neuron activity
    if step % 10 == 0:
        neuron_activity = brain.get_neuron_statistics()
        print(f"Step {step}: Neuron activity stats - {neuron_activity}")
    
    # Get action
    action = agent.decode_motor_command()
    
    # Apply action
    world.apply_motor_command(action, world.get_simulation_time())
    
    # Debug: Check action results
    if action and action.type == pynlm.ActionType.Wait:
        print(f"Agent waiting at step {step}")

# Debugging session completed
print("\n=== Debugging Session Summary ===")

# Analyze neural activity
neuron_stats = brain.get_neuron_statistics()
print(f"Final neuron statistics: {neuron_stats}")

# Check for issues
issues = []
if brain.getFiringNeuronCount() == 0:
    issues.append("No neurons firing - possible initialization issue")
if agent.getCuriosityLevel() == 0:
    issues.append("No curiosity - possible neuromodulation issue")
if brain.getTotalSpikeCount() == 0:
    issues.append("No spikes recorded - possible plasticity issue")

if issues:
    print("\nPotential issues detected:")
    for i, issue in enumerate(issues, 1):
        print(f"  {i}. {issue}")
else:
    print("\n✓ No major issues detected")

# Export debugging data
debug_data = {
    'step_count': 100,
    'final_neuron_count': brain.getTotalNeuronCount(),
    'final_spike_count': brain.getTotalSpikeCount(),
    'final_curiosity': agent.getCuriosityLevel(),
    'final_novelty': agent.getNoveltyLevel(),
    'neuron_stats': neuron_stats
}

import json
try:
    with open("debug_session.json", 'w') as f:
        json.dump(debug_data, f, indent=2, default=str)
    print("\nDebug data saved to 'debug_session.json'")
except Exception as e:
    print(f"\nError saving debug data: {e}")

print("\nDebugging and development tools completed!")
```

## Example 8: Error Handling and Validation

```python
import pynlm

print("=== Error Handling and Validation ===")

# Test 1: Configuration validation
print("\n1. Testing configuration validation...")
try:
    config = pynlm.create_default_config()
    
    # Set invalid parameters
    config.set("brain.neuron_count", -100)  # Invalid: negative neurons
    
    # Validate - should fail
    if not config.validate():
        print("✓ Configuration validation correctly detected invalid parameter")
        # Get validation errors
        errors = config.get_validation_errors()
        print(f"   Validation errors: {errors}")
    
except Exception as e:
    print(f"Error during configuration validation: {e}")

# Test 2: Brain initialization validation
print("\n2. Testing brain initialization validation...")
try:
    config = pynlm.create_default_config()
    
    # Create brain
    brain = pynlm.create_brain(config)
    
    # Try to step without initializing - should fail
    try:
        brain.step(0)
        print("✗ Step without initialization should have failed")
    except Exception as e:
        print(f"✓ Step without initialization correctly failed: {type(e).__name__}")
    
    # Initialize properly
    if brain.initialize():
        print("✓ Brain initialized successfully")
        
        # Now step should work
        brain.step(0)
        print("✓ Step after initialization works correctly")
    else:
        print("✗ Brain initialization failed")
        
except Exception as e:
    print(f"Error during brain validation: {e}")

# Test 3: Agent validation
print("\n3. Testing agent validation...")
try:
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    
    # Create world
    world = pynlm.create_world_2d(width=10, height=10, vision_width=6, vision_height=6)
    
    # Create agent
    agent = pynlm.create_agent_brain(brain)
    
    # Try to process sensory input without initializing - should fail
    try:
        from pynlm import Vision
        percept = Vision(6, 6)
        agent.process_sensory_input(percept)
        print("✗ Processing without initialization should have failed")
    except Exception as e:
        print(f"✓ Processing without initialization correctly failed: {type(e).__name__}")
    
    # Initialize properly
    agent.initialize(world)
    print("✓ Agent initialized successfully")
    
    # Now processing should work
    agent.process_sensory_input(percept)
    print("✓ Processing after initialization works correctly")
    
except Exception as e:
    print(f"Error during agent validation: {e}")

# Test 4: Parameter validation
print("\n4. Testing parameter validation...")
try:
    config = pynlm.create_default_config()
    
    # Test valid parameters
    valid_params = [
        ("brain.neuron_count", 1000),
        ("brain.synapse_density", 0.1),
        ("plasticity.stdp.enable", True)
    ]
    
    for param, value in valid_params:
        config.set(param, value)
        print(f"✓ Set valid parameter {param} = {value}")
    
    # Test invalid parameters
    invalid_params = [
        ("brain.neuron_count", -1),
        ("brain.synapse_density", 1.5),
        ("invalid.param", "value")
    ]
    
    for param, value in invalid_params:
        try:
            config.set(param, value)
            print(f"? Parameter {param} = {value} (may be invalid)")
        except Exception as e:
            print(f"✓ Invalid parameter {param} = {value} correctly rejected: {type(e).__name__}")
    
except Exception as e:
    print(f"Error during parameter validation: {e}")

# Test 5: World validation
print("\n5. Testing world validation...")
try:
    # Test world configuration
    world = pynlm.create_world_2d(width=20, height=20, vision_width=8, vision_height=8)
    
    # Test valid configuration
    world.configure(width=30, height=30, vision_width=10, vision_height=10)
    print("✓ World configuration works correctly")
    
    # Test invalid configuration
    try:
        world.configure(width=-10, height=-10, vision_width=5, vision_height=5)
        print("? Invalid world configuration accepted (may be handled differently)")
    except Exception as e:
        print(f"✓ Invalid world configuration correctly rejected: {type(e).__name__}")
    
except Exception as e:
    print(f"Error during world validation: {e}")

print("\n=== Error Handling and Validation Completed ===")
```

## Usage Notes and Best Practices

### 1. **Memory Management**
- Enable memory tracking for large simulations
- Use `gc.collect()` periodically for memory optimization
- Monitor memory usage in long-running simulations

### 2. **Performance Optimization**
- Use batch operations for improved performance
- Profile simulations to identify bottlenecks
- Use appropriate configurations for specific use cases

### 3. **Error Handling**
- Always validate inputs before use
- Use try-catch blocks for error handling
- Check return values of operations that can fail

### 4. **Debugging**
- Enable debug mode for development
- Use monitoring tools to track simulation state
- Export debug data for analysis

### 5. **Configuration Management**
- Use presets for common configurations
- Validate configurations before use
- Document custom configurations

These examples demonstrate the improved API, showing how to:
- Use enhanced factory methods and convenience functions
- Implement proper error handling and validation
- Optimize performance with batch operations
- Monitor and debug simulations effectively
- Manage configurations and parameters
- Implement advanced simulation scenarios

The improved NLM Python bindings provide a more robust, user-friendly, and feature-rich API for brain simulation research and applications.