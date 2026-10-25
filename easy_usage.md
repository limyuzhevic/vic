---

## Advanced Usage Examples

When you're ready to move beyond the basics:

### Pattern 4: Advanced Configuration

```python
import pynlm

# Create a sophisticated configuration
config = pynlm.createDefaultConfig()

# Fine-tune parameters for specific tasks
config.set("brain.neuron_count", 2000)           # Larger brain
config.set("brain.region_count", 3)             # Multiple regions
config.set("brain.synapse_density", 0.05)        # Sparse connections
config.set("plasticity.stdp.enable", True)       # Enable STDP
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)    # Enable Hebbian
config.set("plasticity.structural.enable", True) # Enable structural plasticity
config.set("neuromod.dopamine.scale", 1.5)        # Enhanced reward
config.set("neuromod.curiosity.enable", True)    # Curiosity-driven exploration
config.set("neuromod.novelty.enable", True)      # Novelty detection
config.set("development.enable", True)           # Developmental stages

print("Advanced configuration created")
```

### Pattern 5: Complex Agent Behavior

```python
import pynlm

def create_advanced_agent(num_steps=5000):
    """Create an agent that learns and develops over time."""
    
    # Setup brain with learning capabilities
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with advanced features
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Create sophisticated world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Run extended simulation
    print("Running advanced agent simulation...")
    for step in range(num_steps):
        # Update world with time-based dynamics
        world.update(0.05)
        
        # Get comprehensive sensory input
        percept = world.getSensoryPercept()
        
        # Process everything in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get sophisticated action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Multi-factor learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # Update development (brain matures)
        agent.updateDevelopment(0.05)
        
        # Advanced logging and statistics
        if step % 500 == 0:
            dev_stage = brain.getDevelopmentalStage()
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"Step {step}: DevStage={dev_stage}, Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
    
    print(f"Simulation complete!")
    print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Neurons firing: {brain.getFiringNeuronCount()}")
    
    return brain, world, agent

# Run the advanced agent
brain, world, agent = create_advanced_agent(2000)
```

### Pattern 6: Checkpointing and Persistence

```python
import pynlm
import os

def create_persistent_agent(checkpoint_path="/tmp/nlm_agent_checkpoint.bin"):
    """Create an agent that can save and load its state."""
    
    # Create initial brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning and development
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running simulation...")
    
    # Phase 1: Learn and save
    for step in range(1000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Save progress every 200 steps
        if step % 200 == 0 and step > 0:
            print(f"Saving checkpoint at step {step}...")
            brain.save(checkpoint_path)
    
    print(f"Initial training complete. Final firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Phase 2: Reload and continue training
    print("Loading checkpoint for continued training...")
    
    # Create fresh brain
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    # Load saved state
    if brain2.load(checkpoint_path):
        print("Checkpoint loaded successfully!")
        
        # Create new agent for loaded brain
        agent2 = pynlm.createAgentBrain(brain2)
        agent2.initialize(world)
        
        # Continue training from loaded state
        for step in range(1000, 2000):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent2.processSensoryInput(percept)
            brain2.step(step)
            action = agent2.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 200 == 0:
                print(f"Continued training at step {step}...")
                brain2.save(checkpoint_path)  # Update checkpoint
        
        print(f"Continued training complete. Final firing rate: {brain2.getAverageFiringRate():.3f}")
    else:
        print("Failed to load checkpoint")
    
    return brain2, world, agent2

# Create persistent agent
brain, world, agent = create_persistent_agent()
```

### Pattern 7: Batch Processing with Multiple Brains

```python
import pynlm

def run_batch_simulation(num_brains=5, steps_per_brain=500):
    """Run multiple independent brain simulations in parallel."""
    
    print(f"Running batch simulation with {num_brains} brains...")
    
    results = []
    
    for brain_id in range(num_brains):
        print(f"  Training brain {brain_id + 1}/{num_brains}...")
        
        # Create unique configuration for each brain
        config = pynlm.createDefaultConfig()
        config.set("random_seed", 42 + brain_id)  # Different seed for diversity
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world for this brain
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        max_firing_rate = 0.0
        total_reward = 0.0
        
        for step in range(steps_per_brain):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect statistics
            firing_rate = brain.getAverageFiringRate()
            if firing_rate > max_firing_rate:
                max_firing_rate = firing_rate
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            total_reward += reward
        
        # Store results
        result = {
            "brain_id": brain_id,
            "max_firing_rate": max_firing_rate,
            "total_reward": total_reward,
            "final_development": brain.getDevelopmentalStage(),
            "final_curiosity": agent.getCuriosityLevel(),
            "final_novelty": agent.getNoveltyLevel()
        }
        
        results.append(result)
        
        # Save this brain for potential later analysis
        checkpoint_path = f"/tmp/brain_{brain_id}.bin"
        brain.save(checkpoint_path)
        print(f"    Brain {brain_id + 1} saved to {checkpoint_path}")
    
    # Print summary
    print("\n=== Batch Simulation Results ===")
    for result in results:
        print(f"Brain {result['brain_id'] + 1}:")
        print(f"  Max firing rate: {result['max_firing_rate']:.3f}")
        print(f"  Total reward: {result['total_reward']:.2f}")
        print(f"  Final development stage: {result['final_development']}")
        print(f"  Final curiosity: {result['final_curiosity']:.3f}")
        print(f"  Final novelty: {result['final_novelty']:.3f}")
        print()
    
    return results

# Run batch simulation
results = run_batch_simulation(3, 500)
```

---

## Error Handling and Troubleshooting

Here's how to handle common issues:

### Pattern 8: Robust Error Handling

```python
import pynlm

def robust_simulation(config=None, max_retries=3):
    """Run simulation with automatic recovery from errors."""
    
    for attempt in range(max_retries):
        try:
            print(f"Attempt {attempt + 1}/{max_retries}")
            
            # Create brain with error checking
            if config is None:
                config = pynlm.createDefaultConfig()
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Create agent with proper initialization
            agent = pynlm.createAgentBrain(brain)
            
            # Test basic functionality before proceeding
            if brain.getTotalNeuronCount() == 0:
                raise RuntimeError("Brain has no neurons")
            
            world = pynlm.createSimpleWorld()
            world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
            world.reset()
            
            agent.initialize(world)
            
            # Enable all systems
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation steps
            for step in range(100):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Check for problematic states
                if brain.getFiringNeuronCount() == 0:
                    print(f"Warning: No firing neurons at step {step}")
                
                # Check for NaN values
                if math.isnan(brain.getAverageFiringRate()):
                    raise RuntimeError(f"Invalid firing rate at step {step}")
            
            print("Simulation completed successfully")
            return brain, world, agent
            
        except Exception as e:
            print(f"Attempt {attempt + 1} failed: {e}")
            if attempt < max_retries - 1:
                print(f"Retrying in 2 seconds...")
                import time
                time.sleep(2)
            else:
                raise RuntimeError(f"All {max_retries} attempts failed")

# Run robust simulation
brain, world, agent = robust_simulation()
```

### Pattern 9: Performance Monitoring

```python
import pynlm
import time
import statistics

def monitor_simulation_performance(brain, world, agent, num_steps=1000):
    """Monitor simulation performance and collect metrics."""
    
    start_time = time.time()
    
    metrics = {
        'step_times': [],
        'firing_rates': [],
        'memory_usage': [],
        'cpu_usage': []
    }
    
    print("Starting performance monitoring simulation...")
    
    for step in range(num_steps):
        step_start = time.time()
        
        # Record baseline before operations
        baseline_time = time.time()
        
        # Execute simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics
        step_time = time.time() - step_start
        firing_rate = brain.getAverageFiringRate()
        
        metrics['step_times'].append(step_time)
        metrics['firing_rates'].append(firing_rate)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            avg_step_time = statistics.mean(metrics['step_times'][-10:])
            avg_firing_rate = statistics.mean(metrics['firing_rates'][-10:])
            print(f"Step {step}: Avg step time = {avg_step_time:.4f}s, "
                  f"Avg firing rate = {avg_firing_rate:.3f}")
    
    total_time = time.time() - start_time
    
    # Calculate performance statistics
    avg_step_time = statistics.mean(metrics['step_times'])
    max_step_time = max(metrics['step_times'])
    min_step_time = min(metrics['step_times'])
    
    avg_firing_rate = statistics.mean(metrics['firing_rates'])
    max_firing_rate = max(metrics['firing_rates'])
    min_firing_rate = min(metrics['firing_rates'])
    
    # Print performance summary
    print("\n=== Performance Summary ===")
    print(f"Total simulation time: {total_time:.2f}s")
    print(f"Average step time: {avg_step_time:.4f}s ({1000/avg_step_time:.1f} steps/sec)")
    print(f"Step time range: {min_step_time:.4f}s - {max_step_time:.4f}s")
    print(f"Average firing rate: {avg_firing_rate:.3f}")
    print(f"Firing rate range: {min_firing_rate:.3f} - {max_firing_rate:.3f}")
    
    return metrics, total_time

# Monitor performance of a simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

metrics, total_time = monitor_simulation_performance(brain, world, agent, 500)
```

---

## Integration with External Systems

### Pattern 10: Sensor Integration

```python
import pynlm
import numpy as np

def integrate_external_sensor(brain, sensor_data, sensor_type="vision"):
    """Integrate external sensor data into the brain."""
    
    if sensor_type == "vision":
        # Create Vision sensory input from camera data
        from pynlm import Vision
        
        # Assuming sensor_data is a 2D array (width x height)
        width, height = sensor_data.shape
        
        vision = Vision(width, height, 3)
        
        # Normalize and convert to float array
        normalized_data = sensor_data.astype(np.float32) / 255.0
        vision.setData(normalized_data.flatten())
        
        # Convert to SensoryPercept and process
        from pynlm import SensoryPercept
        percept = SensoryPercept()
        percept.setVision(vision)
        
    elif sensor_type == "audio":
        # Create Audio sensory input from microphone data
        from pynlm import Audio
        
        # Assuming sensor_data is audio samples
        audio = Audio()
        audio.setData(sensor_data)
        
        percept = SensoryPercept()
        percept.setAudio(audio)
        
    else:
        raise ValueError(f"Unsupported sensor type: {sensor_type}")
    
    # Process the integrated sensor data
    # (Note: In a real implementation, you'd need an agent to process this)
    # This would typically be done via an AgentBrain interface
    
    return percept

# Example usage
if __name__ == "__main__":
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate external sensor data (e.g., from a camera)
    # In real usage, this would come from actual hardware
    camera_data = np.random.rand(64, 64, 3) * 255  # Simulated image
    
    # Integrate sensor data
    percept = integrate_external_sensor(brain, camera_data, "vision")
    
    print(f"Integrated sensor data successfully")
    print(f"Vision dimensions: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
```

---

## Quick Reference: Common Problems and Solutions

| Problem | Solution |
|---------|----------|
| "My brain isn't doing anything" | Call `brain.initialize()` before stepping. Try increasing the number of steps. |
| "The agent isn't moving" | Call `world.applyMotorCommand()` after decoding actions. |
| "Everything is 0" | Brains need time to "warm up" - try more steps. Some neurons need input to fire. |
| "Performance is slow" | Reduce neuron count, use smaller world, decrease simulation steps. |
| "Memory is growing too large" | Disable structural plasticity or reduce brain size. |
| "Learning isn't happening" | Check that reward modulation and plasticity are enabled. |

---

## Next Steps

When you're comfortable with these patterns:

1. **Read `HOW_TO_USE.md`** for more detailed technical information
2. **Read `docs/ARCHITECTURE.md`** to understand the system architecture
3. **Experiment with different configurations** and observe the effects
4. **Look at the example source code** in `src/experiments/` for more complex examples
5. **Check the Python documentation** by running `help(pynlm)` in your Python interpreter

That's it! You're now ready to use NLM at an advanced level. Happy simulation!\n\n---\n\n## Quick Reference\n\n| What you want | Code |\n|--------------|------|\n| Create brain | `brain = pynlm.createBrain(config)` |\n| Start brain | `brain.initialize()` |\n| Make brain think | `brain.step(step)` |\n| Create world | `world = pynlm.createSimpleWorld()` |\n| Create agent | `agent = pynlm.createAgentBrain(brain)` |\n| Get brain stats | `brain.getFiringNeuronCount()` |\n| See world | `world.getSensoryPercept()` |\n| Make action | `agent.decodeMotorCommand()` |\n\n---\n\n## More Resources\n\n- [NLM Documentation](https://nlm.readthedocs.io)\n- [GitHub Repository](https://github.com/nlm-project/nlm)\n- [Scientific Papers](https://scholar.google.com\?q=nlm+neural+learning\ machine)\n- [Community Forum](https://community.nlm.org)\n\nThat's it! You're now ready to use NLM.
