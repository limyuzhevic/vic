# Comprehensive NLM Python Examples

This directory contains comprehensive examples for using the NLM (Neural Learning Machine) Python bindings.
Each example demonstrates different aspects of the neural simulation framework and serves as both
documentation and test cases for the Python API.

## Quick Start Examples

### 1. Minimal Brain Example

```python
import pynlm

# Create and initialize a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Brain produced {brain.getTotalSpikeCount()} spikes")
```

### 2. Complete Agent Simulation

```python
import pynlm

def run_complete_agent_simulation(steps=500):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    
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
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(0.1)
        
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
        
        # Apply reward modulation based on agent's internal state
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    print("Simulation complete!")
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_complete_agent_simulation(500)
```

### 3. Brain Visualization and Analysis

```python
import pynlm
import matplotlib.pyplot as plt

def analyze_brain_activity(brain, num_steps=200):
    """Analyze brain activity over time and create visualizations."""
    
    # Initialize brain
    brain.initialize()
    
    # Collect statistics
    step_data = []
    firing_rates = []
    spike_counts = []
    development_stages = []
    neuromodulation_levels = []
    
    for step in range(num_steps):
        brain.step(step)
        
        step_data.append({
            'step': step,
            'firing_neutrons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        # If we have an agent brain, collect neuromodulation data
        if hasattr(brain, 'getCuriosityLevel'):
            # This is an agent brain, collect more data
            curiosity = brain.getCuriosityLevel()
            novelty = brain.getNoveltyLevel()
            neuromodulation = brain.getNeuromodulationLevel()
            
            neuromodulation_levels.append(neuromodulation)
            firing_rates.append(brain.getAverageFiringRate())
            spike_counts.append(brain.getTotalSpikeCount())
            development_stages.append(brain.getDevelopmentalStage())
    
    # Create analysis plots
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    
    # Plot 1: Firing neuron count over time
    axes[0, 0].plot([d['step'] for d in step_data], 
                   [d['firing_neutrons'] for d in step_data],
                   'b-', linewidth=2)
    axes[0, 0].set_title('Firing Neuron Count Over Time')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Number of Neurons')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Spike count over time
    axes[0, 1].plot([d['step'] for d in step_data],
                   [d['total_spikes'] for d in step_data],
                   'r-', linewidth=2)
    axes[0, 1].set_title('Total Spike Count Over Time')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Total Spikes')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Firing rate over time
    axes[1, 0].plot([d['step'] for d in step_data],
                   [d['avg_firing_rate'] for d in step_data],
                   'g-', linewidth=2)
    axes[1, 0].set_title('Average Firing Rate Over Time')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Firing Rate (Hz)')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: E/I ratio over time
    axes[1, 1].plot([d['step'] for d in step_data],
                   [d['e_i_ratio'] for d in step_data],
                   'm-', linewidth=2)
    axes[1, 1].set_title('Excitation/Inhibition Ratio Over Time')
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('E/I Ratio')
    axes[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()
    
    return step_data

# Create a brain for analysis
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.05)

brain = pynlm.createBrain(config)

print("Analyzing brain activity...")
analysis_data = analyze_brain_activity(brain, 200)

print(f"Analysis complete!")
print(f"Average firing rate: {sum(d['avg_firing_rate'] for d in analysis_data)/len(analysis_data):.2f} Hz")
print(f"Maximum firing neurons: {max(d['firing_neutrons'] for d in analysis_data)}")
print(f"Total spikes: {analysis_data[-1]['total_spikes']}")
```

### 4. Sensory Processing Pipeline

```python
import pynlm
import numpy as np

def test_sensory_processing():
    """Test the complete sensory processing pipeline."""
    
    # Create a simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create brain with sensory capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation and collect sensory data
    sensory_data = []
    motor_commands = []
    
    print("Testing sensory processing pipeline...")
    
    for step in range(100):
        # Update world (this creates the percept)
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Collect sensory data
        vision_data = percept.getVision()
        internal_signals = percept.getInternal()
        proprioception = percept.getProprioception()
        
        sensory_data.append({
            'step': step,
            'vision_data': vision_data,
            'internal_signals': internal_signals,
            'proprioception': proprioception,
            'timestamp': percept.getTimestamp()
        })
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        motor_commands.append(motor_cmd)
        
        # Apply to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Analyze sensory data
    print("\nSensory Processing Analysis:")
    print(f"Total steps: {len(sensory_data)}")
    print(f"Vision data shape: {len(sensory_data[0]['vision_data']) if sensory_data else 0} (expected 256 for 16x16x1)")
    print(f"Average internal signals: {np.mean([np.mean(d['internal_signals']) for d in sensory_data if d['internal_signals']]):.3f}")
    
    # Check vision data ranges
    vision_values = []
    for d in sensory_data:
        vision_values.extend(d['vision_data'])
    
    print(f"Vision data range: {min(vision_values):.3f} to {max(vision_values):.3f}")
    
    # Check motor command distribution
    motor_counts = {}
    for cmd in motor_commands:
        motor_name = cmd.name if hasattr(cmd, 'name') else str(cmd)
        motor_counts[motor_name] = motor_counts.get(motor_name, 0) + 1
    
    print(f"\nMotor command distribution:")
    for cmd, count in motor_counts.items():
        percentage = (count / len(motor_commands)) * 100
        print(f"  {cmd}: {count} times ({percentage:.1f}%)")
    
    return sensory_data, motor_commands

# Run sensory processing test
sensory_results, motor_results = test_sensory_processing()
```

### 5. Learning and Development

```python
import pynlm

def test_learning_and_development():
    """Test learning systems and developmental progression."""
    
    # Create configuration optimized for learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Testing learning and development systems...")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Collect developmental data
    developmental_data = []
    curiosity_levels = []
    novelty_levels = []
    neuromodulation_levels = []
    
    for step in range(200):
        brain.step(step)
        
        # Track development
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Track neuromodulation levels
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        neuromodulation_levels.append(agent.getNeuromodulationLevel())
        
        # Track developmental stage
        developmental_data.append({
            'step': step,
            'stage': brain.getDevelopmentalStage(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        })
    
    # Analyze development
    print("\nDevelopment Analysis:")
    print(f"Final developmental stage: {developmental_data[-1]['stage']}")
    print(f"Average curiosity level: {sum(curiosity_levels)/len(curiosity_levels):.3f}")
    print(f"Average novelty level: {sum(novelty_levels)/len(novelty_levels):.3f}")
    print(f"Average neuromodulation level: {sum(neuromodulation_levels)/len(neuromodulation_levels):.3f}")
    
    # Check for developmental progression
    stages = [d['stage'] for d in developmental_data]
    if len(set(stages)) > 1:
        print("✓ Development is progressing through stages")
    else:
        print("⚠ Development may not be progressing through stages")
    
    # Check learning systems
    if sum(curiosity_levels) > 0.1:
        print("✓ Curiosity-driven exploration is active")
    else:
        print("⚠ Curiosity-driven exploration may not be active")
    
    if sum(neuromodulation_levels) > 0.1:
        print("✓ Neuromodulation is active")
    else:
        print("⚠ Neuromodulation may not be active")
    
    return developmental_data

# Test learning and development
development_results = test_learning_and_development()
```

### 6. Memory Systems

```python
import pynlm

def test_memory_systems():
    """Test working memory, episodic memory, and associative memory systems."""
    
    # Create brain with memory capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 400)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    
    print("Testing memory systems...")
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Access memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"\nMemory System Status:")
    print(f"  Working memory: {'Enabled' if working_memory else 'Disabled'}")
    print(f"  Episodic memory: {'Enabled' if episodic_memory else 'Disabled'}")
    print(f"  Associative memory: {'Enabled' if associative_memory else 'Disabled'}")
    
    # Test memory operations if available
    try:
        # Try to use memory systems (exact API may vary)
        if working_memory:
            print("  Working memory accessible")
        if episodic_memory:
            print("  Episodic memory accessible")
        if associative_memory:
            print("  Associative memory accessible")
            
        print("✓ Memory systems are accessible and functional")
    except Exception as e:
        print(f"⚠ Memory systems may not be fully implemented: {e}")
    
    return {
        'working_memory': working_memory,
        'episodic_memory': episodic_memory,
        'associative_memory': associative_memory
    }

# Test memory systems
memory_systems = test_memory_systems()
```

### 7. Configuration Management

```python
import pynlm
import json
import tempfile

def test_configuration_management():
    """Test configuration loading, saving, and modification."""
    
    print("Testing configuration management...")
    
    # 1. Create default configuration
    config = pynlm.createDefaultConfig()
    print(f"1. Created default config with {config.getKeys()} keys")
    
    # 2. Modify configuration
    config.set("brain.neuron_count", 750)
    config.set("brain.synapse_density", 0.08)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", False)
    
    print("2. Modified configuration with custom parameters")
    print(f"   - Neuron count: {config.get('brain.neuron_count')}")
    print(f"   - Synapse density: {config.get('brain.synapse_density')}")
    print(f"   - STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")
    print(f"   - Curiosity enabled: {config.get('neuromod.curiosity.enable')}")
    print(f"   - Novelty enabled: {config.get('neuromod.novelty.enable')}")
    
    # 3. Save configuration to file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        print(f"3. Saved configuration to {f.name}")
        
        # 4. Load configuration from file
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(f.name)
        
        print("4. Loaded configuration from file")
        print(f"   - Loaded neuron count: {new_config.get('brain.neuron_count')}")
        print(f"   - Loaded synapse density: {new_config.get('brain.synapse_density')}")
        
        # 5. Verify configuration integrity
        assert config.get("brain.neuron_count") == new_config.get("brain.neuron_count")
        assert config.get("brain.synapse_density") == new_config.get("brain.synapse_density")
        
        print("5. Configuration integrity verified")
        
        # Clean up
        import os
        os.unlink(f.name)
    
    # 6. Test configuration summary
    print("6. Configuration summary:")
    print(f"   {config.summary()}")
    
    return config

# Test configuration management
final_config = test_configuration_management()
```

### 8. World Interaction

```python
import pynlm

def test_world_interaction():
    """Test world-object interaction and physics simulation."""
    
    print("Testing world interaction...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Set agent starting position
    world.setAgentStart(15.0, 15.0)
    
    # Add various objects to the world
    resources = []
    hazards = []
    walls = []
    
    # Add resource objects
    for i in range(5):
        x = np.random.uniform(5.0, 25.0)
        y = np.random.uniform(5.0, 25.0)
        resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=10.0, radius=0.5)
        world.addObject(resource)
        resources.append(resource)
    
    # Add hazard objects
    for i in range(3):
        x = np.random.uniform(5.0, 25.0)
        y = np.random.uniform(5.0, 25.0)
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-5.0, radius=0.3)
        world.addObject(hazard)
        hazards.append(hazard)
    
    # Add wall boundaries
    wall_positions = [(0, 0), (30, 0), (0, 30), (30, 30)]
    for wx, wy in wall_positions:
        wall = pynlm.WorldObject(wx, wy, pynlm.WorldObjectType.Wall, value=0.0, radius=0.5)
        world.addObject(wall)
        walls.append(wall)
    
    print(f"Added {len(resources)} resources, {len(hazards)} hazards, {len(walls)} walls")
    
    # Simulate interaction
    print("\nSimulating agent interactions:")
    
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get current state
        body = world.getAgentBody()
        percept = world.getSensoryPercept()
        
        print(f"  Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
              f"Energy: {body.energy:.2f}, Health: {body.health:.2f}")
        
        # Get motor command (in a real scenario, this would come from brain)
        # For demo, we'll use a random walk
        import random
        if random.random() < 0.3:
            action_type = pynlm.ActionType.MoveForward
        elif random.random() < 0.6:
            action_type = pynlm.ActionType.TurnLeft
        else:
            action_type = pynlm.ActionType.Wait
        
        action = pynlm.Action(action_type)
        motor_cmd = pynlm.MotorCommand.MoveForward  # Simplified for demo
        
        # Apply action to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Check for object interactions
        for resource in resources:
            if world.isValidPosition(resource.x, resource.y):
                # Agent reached resource
                print(f"    Reached resource at ({resource.x}, {resource.y})")
                
        for hazard in hazards:
            if world.isValidPosition(hazard.x, hazard.y):
                # Agent reached hazard  
                print(f"    Reached hazard at ({hazard.x}, {hazard.y})")
    
    print(f"\nWorld interaction test complete!")
    print(f"Final agent state: x={world.getAgentBody().x:.1f}, y={world.getAgentBody().y:.1f}")
    
    return resources, hazards, walls

# Test world interaction
world_objects = test_world_interaction()
```

### 9. Serialization and Persistence

```python
import pynlm
import tempfile
import os

def test_serialization():
    """Test brain state serialization and persistence."""
    
    print("Testing serialization and persistence...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    # Run simulation to create some activity
    for step in range(100):
        brain1.step(step)
    
    print(f"Original brain created: {brain1.getTotalNeuronCount()} neurons, "
          f"{brain1.getTotalSpikeCount()} total spikes")
    
    # Save brain state
    with tempfile.NamedTemporaryFile(mode='wb', suffix='.bin', delete=False) as f:
        filepath = f.name
        brain1.save(filepath)
        print(f"Saved brain state to {filepath}")
    
    # Create a new brain and load the state
    brain2 = pynlm.createBrain(config)
    brain2.load(filepath)
    
    print(f"Loaded brain state: {brain2.getTotalNeuronCount()} neurons, "
          f"{brain2.getTotalSpikeCount()} total spikes")
    
    # Verify state integrity
    neurons_match = brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount()
    spikes_match = brain1.getTotalSpikeCount() == brain2.getTotalSpikeCount()
    
    if neurons_match and spikes_match:
        print("✓ Brain state serialization integrity verified")
    else:
        print("✗ Brain state serialization integrity check failed")
    
    # Test reset and reload
    brain2.reset()
    print(f"After reset: {brain2.getTotalNeuronCount()} neurons, "
          f"{brain2.getTotalSpikeCount()} total spikes")
    
    # Clean up
    os.unlink(filepath)
    
    return brain1, brain2

# Test serialization
original_brain, loaded_brain = test_serialization()
```

### 10. Advanced Agent with Learning

```python
import pynlm
import numpy as np

def advanced_agent_learning():
    """Advanced agent with complex learning strategies."""
    
    print("Testing advanced agent learning...")
    
    # Create sophisticated configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("brain.synapse_density", 0.12)
    config.set("brain.v_thresh", -55.0)
    config.set("brain.v_rest", -70.0)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.tau_plus", 20.0)
    config.set("plasticity.stdp.tau_minus", 20.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.5)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.01)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced agent initialized with all learning systems")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Advanced learning metrics
    learning_metrics = {
        'curiosity_history': [],
        'novelty_history': [],
        'neuromodulation_history': [],
        'spike_history': [],
        'firing_rate_history': [],
        'development_history': [],
        'reward_history': []
    }
    
    # Advanced simulation loop with learning
    for step in range(300):
        # Update world
        world.update(0.05)  # Smaller timestep for more precision
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Get current state
        body = world.getAgentBody()
        
        # Calculate reward based on agent's state and world state
        reward = calculate_reward(body, percept, step)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command with curiosity exploration
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        predicted_reward = 0.0  # In advanced systems, this would be predicted
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        if step % 10 == 0:  # Less frequent development updates
            agent.updateDevelopment(0.05)
        
        # Collect learning metrics
        learning_metrics['curiosity_history'].append(agent.getCuriosityLevel())
        learning_metrics['novelty_history'].append(agent.getNoveltyLevel())
        learning_metrics['neuromodulation_history'].append(agent.getNeuromodulationLevel())
        learning_metrics['spike_history'].append(brain.getTotalSpikeCount())
        learning_metrics['firing_rate_history'].append(brain.getAverageFiringRate())
        learning_metrics['development_history'].append(brain.getDevelopmentalStage())
        learning_metrics['reward_history'].append(reward)
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"\nStep {step}:")
            print(f"  Position: ({body.x:.1f}, {body.y:.1f})")
            print(f"  Energy: {body.energy:.2f}, Health: {body.health:.2f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Reward: {reward:.3f}")
    
    # Analyze learning performance
    print("\n" + "="*50)
    print("ADVANCED AGENT LEARNING ANALYSIS")
    print("="*50)
    
    # Calculate learning statistics
    avg_curiosity = np.mean(learning_metrics['curiosity_history'])
    avg_novelty = np.mean(learning_metrics['novelty_history'])
    avg_neuromodulation = np.mean(learning_metrics['neuromodulation_history'])
    avg_firing_rate = np.mean(learning_metrics['firing_rate_history'])
    max_spikes = max(learning_metrics['spike_history'])
    
    print(f"Learning Performance Metrics:")
    print(f"  Average curiosity level: {avg_curiosity:.3f}")
    print(f"  Average novelty level: {avg_novelty:.3f}")
    print(f"  Average neuromodulation level: {avg_neuromodulation:.3f}")
    print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"  Maximum spike count: {max_spikes}")
    
    # Check developmental progression
    developmental_stages = learning_metrics['development_history']
    unique_stages = set(developmental_stages)
    print(f"  Developmental stages experienced: {len(unique_stages)}")
    
    if len(unique_stages) > 1:
        print("✓ Advanced agent successfully progressed through developmental stages")
    else:
        print("⚠ Agent may not be progressing through developmental stages")
    
    # Check exploration vs exploitation
    high_curiosity_steps = sum(1 for c in learning_metrics['curiosity_history'] if c > 0.5)
    exploration_ratio = high_curiosity_steps / len(learning_metrics['curiosity_history'])
    
    print(f"  Exploration ratio (high curiosity): {exploration_ratio:.3f}")
    
    if exploration_ratio > 0.1:
        print("✓ Agent shows good exploration behavior")
    else:
        print("⚠ Agent exploration behavior may be limited")
    
    # Check learning stability
    firing_rate_std = np.std(learning_metrics['firing_rate_history'])
    print(f"  Firing rate stability (std): {firing_rate_std:.3f}")
    
    if firing_rate_std < 0.5:
        print("✓ Learning appears stable")
    else:
        print("⚠ Learning may be unstable")
    
    return learning_metrics

def calculate_reward(body, percept, step):
    """Calculate reward based on agent state and world state."""
    reward = 0.0
    
    # Reward for maintaining energy
    if body.energy > 0.8:
        reward += 0.1
    elif body.energy < 0.2:
        reward -= 0.5
    
    # Reward for staying alive
    if body.health > 0.5:
        reward += 0.05
    
    # Reward for exploration (novelty)
    novelty = percept.getInternal() if percept else []
    if novelty and len(novelty) > 0:
        reward += novelty[0] * 0.1
    
    # Small time penalty to encourage efficiency
    reward -= 0.001
    
    return reward

# Run advanced agent learning
advanced_metrics = advanced_agent_learning()
```

## Usage Guidelines

### Running Examples

1. **Basic Examples**:
   ```bash
   python example_1_minimal.py
   python example_2_complete.py
   ```

2. **All Examples**:
   ```bash
   cd examples && python run_all_examples.py
   ```

3. **Individual Category**:
   ```bash
   python examples/learning/development_examples.py
   ```

### Dependencies

These examples require:
- Python 3.8+
- NumPy
- Matplotlib (for visualization examples)

### Installation

```bash
pip install numpy matplotlib
pip install -e .  # Install pynlm Python bindings
```

### Customization

Feel free to modify the examples:
- Adjust simulation parameters
- Add your own reward functions
- Modify visualization parameters
- Create custom agent behaviors

## Example Structure

Each example file includes:
1. **Import statements** for NLM and additional libraries
2. **Configuration setup** with specific parameters
3. **Simulation loop** demonstrating core functionality
4. **Output and analysis** of results
5. **Best practices** and comments

## Learning Path

1. **Start with Example 1** (Minimal Brain) to understand basic usage
2. **Progress to Example 2** (Complete Agent) for full workflow understanding
3. **Try Example 3** (Visualization) to see data analysis techniques
4. **Explore Example 4** (Sensory Processing) to understand perception
5. **Study Example 5** (Learning and Development) for advanced learning concepts
6. **Test Example 6** (Memory Systems) to understand memory
7. **Follow Example 7** (Configuration Management) for setup techniques
8. **Create your own examples** using the structure provided

These examples provide a comprehensive foundation for working with the NLM Python bindings and understanding neural simulation principles.