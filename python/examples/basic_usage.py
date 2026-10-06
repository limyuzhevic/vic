# NLM Python Usage Examples

## Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Create a simple world for interaction
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.setAgentStart(10, 10)

# Create an agent brain to connect the neural brain to the world
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

print("NLM brain initialized and ready for simulation!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Simulation Example

```python
import pynlm
import time

# Initialize simulation components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.setAgentStart(15, 15)

agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Set random seed for reproducible simulations
world.setRandomSeed(42)

# Run simulation
max_steps = 1000
print("Starting NLM simulation...")
print(f"Running {max_steps} steps...")

for step in range(max_steps):
    # Get current perception from world
    percept = world.getSensoryPercept()
    
    # Process sensory input through agent brain
    agent_brain.processSensoryInput(percept)
    
    # Update agent brain (includes development and neuromodulation)
    agent_brain.updateDevelopment(step * 0.001)
    
    # Get motor command from agent brain
    motor_command = agent_brain.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(motor_command, step * 0.001)
    
    # Log progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}: "
              f"Pos=({agent_brain.getBrain().getAgentBody().x:.1f}, "
              f"{agent_brain.getBrain().getAgentBody().y:.1f}), "
              f"Energy={agent_brain.getBrain().getAgentBody().energy:.1f}, "
              f"FiringRate={brain.getAverageFiringRate():.2f}Hz")
    
    # Check for early termination
    if agent_brain.getBrain().getAgentBody().health <= 0:
        print(f"Simulation terminated at step {step} - agent died")
        break

print(f"Simulation completed {max_steps} steps successfully!")
print(f"Final position: ({agent_brain.getBrain().getAgentBody().x:.1f}, "
      f"{agent_brain.getBrain().getAgentBody().y:.1f})")
print(f"Final health: {agent_brain.getBrain().getAgentBody().health}")
```

## Learning and Plasticity

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable different types of plasticity
print("Enabling plasticity mechanisms...")

# Enable STDP for spike-timing dependent plasticity
# (This is typically enabled automatically in integrated systems)

# Enable reward-modulated learning
# This allows dopamine to modulate synaptic learning

# Run learning simulation
learning_steps = 500
print(f"Running learning simulation for {learning_steps} steps...")

# Simulate reward-based learning
for step in range(learning_steps):
    # Create some sensory input
    from pynlm import Vision
    vision = Vision(width=10, height=10)
    
    # Fill with some pattern (simulating visual input)
    import random
    for y in range(10):
        for x in range(10):
            vision.setData([random.uniform(0, 1) for _ in range(3)])
    
    # Process through brain
    brain.receiveSensoryInput(vision)
    
    # Inject some current to drive activity
    from pynlm import NeuronType
    for neuron_id in range(10):
        brain.injectCurrent(neuron_id, random.uniform(20, 50))
    
    # Step the simulation
    brain.step(step, step * 0.001)
    
    # Simulate reward at random steps
    if step % 50 == 0:
        reward = random.uniform(0.5, 1.0)
        print(f"Step {step}: Receiving reward {reward:.2f}")
    
    if step % 100 == 0:
        print(f"  Step {step}: "
              f"Spikes={brain.getTotalSpikeCount()}, "
              f"FiringRate={brain.getAverageFiringRate():.2f}Hz")

print(f"Learning simulation completed!")
print(f"Final average firing rate: {brain.getAverageFiringRate():.2f}Hz")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

## Development and Aging

```python
import pynlm

# Create a developmental brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Starting developmental simulation...")
print("Brain will progress through developmental stages:")
print("1. Initial -> CriticalPeriod -> Maturation -> Adult -> Aging")

development_steps = 2000
for step in range(development_steps):
    # Step the simulation
    brain.step(step, step * 0.001)
    
    # Log developmental stage changes
    if step % 500 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Step {step}: Development stage = {stage}")
        
        # Log key developmental metrics
        print(f"  Plasticity factors: "
              f"dopamine={agent_brain.getNeuromodulationLevel():.2f}, "
              f"curiosity={agent_brain.getCuriosityLevel():.2f}, "
              f"novelty={agent_brain.getNoveltyLevel():.2f}")
    
    # Apply some developmental effects (simulated)
    if step % 100 == 0:
        # Random sensory input for development
        from pynlm import Vision
        vision = Vision(5, 5)
        brain.receiveSensoryInput(vision)

print(f"Developmental simulation completed!")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
print(f"Final performance metrics:")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}Hz")
print(f"  Memory capacity: {brain.getRegionCount()} regions")
```

## Checkpoint and Persistence

```python
import pynlm
import os

# Create and train a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Train for some time to generate interesting state
training_steps = 1000
print(f"Training brain for {training_steps} steps...")

for step in range(training_steps):
    brain.step(step, step * 0.001)
    
    if step % 200 == 0:
        print(f"Training step {step}/{training_steps}")

print("Training complete. Testing checkpoint/save/load...")

# Save the brain state
checkpoint_file = "test_checkpoint.bin"
brain.save(checkpoint_file)
print(f"Brain state saved to: {checkpoint_file}")

# Create a new brain and load the checkpoint
new_brain = pynlm.createBrain(config)
print("Loading checkpoint into new brain...")
new_brain.load(checkpoint_file)

# Verify the loaded state
print(f"Original brain spike count: {brain.getTotalSpikeCount()}")
print(f"Loaded brain spike count: {new_brain.getTotalSpikeCount()}")
print(f"Spike counts match: {brain.getTotalSpikeCount() == new_brain.getTotalSpikeCount()}")

# Clean up
if os.path.exists(checkpoint_file):
    os.remove(checkpoint_file)
    print(f"Cleaned up checkpoint file: {checkpoint_file}")
```

## Advanced Configuration

```python
import pynlm

# Create custom configuration
custom_config = pynlm.createDefaultConfig()

# Modify configuration parameters
print("Customizing NLM configuration...")

# Set random seed for reproducibility
custom_config.set("random_seed", 12345)

# Adjust neuron count for smaller simulation
custom_config.set("neuron_count", 100)

# Set simulation timestep
custom_config.set("simulation_timestep", 0.001)

# Configure plasticity parameters
custom_config.set("stdp_ltp_weight", 0.02)
custom_config.set("stdp_ltd_weight", 0.015)
custom_config.set("stdp_tau", 20.0)

# Configure memory parameters
custom_config.set("working_memory_capacity", 50)
custom_config.set("episodic_memory_max_episodes", 100)

# Configure neuromodulation
custom_config.set("dopamine_baseline", 0.1)
custom_config.set("curiosity_decay", 0.05)

# Create brain with custom configuration
brain = pynlm.createBrain(custom_config)
brain.initialize()

print("Custom brain initialized with the following settings:")
print(f"  Random seed: {custom_config.getOr('random_seed', 0)}")
print(f"  Neuron count: {custom_config.getOr('neuron_count', 500)}")
print(f"  Simulation timestep: {custom_config.getOr('simulation_timestep', 0.001)}")
print(f"  STDP LTP weight: {custom_config.getOr('stdp_ltp_weight', 0.02)}")
print(f"  STDP LTD weight: {custom_config.getOr('stdp_ltd_weight', 0.015)}")

# Print full configuration summary
print("\nFull configuration summary:")
print(custom_config.summary())
```

## Error Handling and Best Practices

```python
import pynlm
import sys

def safe_simulation(max_steps=1000, max_energy=0.0):
    """Example of safe simulation with error handling"""
    try:
        # Initialize components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10, 10)
        
        agent_brain = pynlm.createAgentBrain(brain)
        agent_brain.initialize(world)
        
        # Simulation loop with error checking
        for step in range(max_steps):
            # Check for termination conditions
            body = agent_brain.getBrain().getAgentBody()
            
            if body.health <= 0:
                print(f"Agent died at step {step}")
                break
                
            if body.energy <= 0:
                print(f"Agent starved at step {step}")
                break
                
            if body.age > max_steps * 2:
                print(f"Agent reached maximum age at step {step}")
                break
            
            # Normal simulation step
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            agent_brain.updateDevelopment(step * 0.001)
            
            motor_command = agent_brain.decodeMotorCommand()
            world.applyMotorCommand(motor_command, step * 0.001)
            
            # Log periodically
            if step % 100 == 0:
                print(f"Step {step}: "
                      f"Health={body.health:.1f}, "
                      f"Energy={body.energy:.1f}, "
                      f"Age={body.age}")
        
        return True
        
    except Exception as e:
        print(f"Simulation error: {e}")
        return False

# Run safe simulation
if __name__ == "__main__":
    success = safe_simulation(max_steps=500)
    sys.exit(0 if success else 1)
```