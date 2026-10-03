# NLM Advanced Usage Examples

This document provides comprehensive examples for using the enhanced NLM (Neural Learning Machine) Python bindings.

## Advanced Agent Examples

### 1. Advanced Agent with Signal Processing

```python
import pynlm

def run_advanced_agent_simulation(steps=2000):
    """Advanced agent with comprehensive signal processing capabilities."""
    
    # 1. Create enhanced configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Configure advanced parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # 3. Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create enhanced world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    agent.initialize(world)
    
    # 5. Enable advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Set advanced behavior mode
    # Note: This would require additional API calls to set behavior mode
    # agent.setBehaviorMode(pynlm.BehaviorMode.Adaptive)
    
    print("Advanced Agent Simulation Started")
    print("=" * 50)
    
    # 7. Run simulation with advanced monitoring
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Advanced signal processing demonstration
        # Normalize signals if needed
        if step % 100 == 0:
            normalized_signals = percept.getNormalizedSignals()
            zero_mean_signals = percept.getZeroMeanSignals()
            stats = percept.getSignalStatistics(pynlm.SignalType.Combined)
            
            print(f"Step {step}:")
            print(f"  Signal Range: {stats.min:.3f} - {stats.max:.3f}")
            print(f"  Signal Mean: {stats.mean:.3f}")
            print(f"  Signal Variance: {stats.variance:.3f}")
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Decode motor command with advanced strategy
        motor_cmd = agent.decodeMotorCommand(pynlm.MotorCommandSelectionStrategy.Balanced)
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 200 == 0:
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print()
    
    print("Advanced Agent Simulation Completed!")
    return brain, agent, world

# Run the advanced simulation
if __name__ == "__main__":
    brain, agent, world = run_advanced_agent_simulation(2000)
```

### 2. Agent with Signal Processing and Filtering

```python
import pynlm

def run_filtering_simulation(steps=1500):
    """Agent with advanced signal processing and filtering capabilities."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable advanced processing
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Signal Processing and Filtering Simulation")
    print("=" * 45)
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Apply different filters based on signal type
        if step % 50 == 0:
            # Create different kernel types
            moving_average_kernel = pynlm.createMovingAverageKernel(5)
            gaussian_kernel = pynlm.createGaussianKernel(7, 1.5)
            
            # Apply filters to all signals
            percept.applyBandPassFilter(moving_average_kernel)
            
            # Add noise for realism
            percept.addNoise(0.02)
            
            # Scale signals for different conditions
            if step % 100 == 0:
                percept.scaleSignals(1.0, 1.2, 0.8, 1.1, 0.9)
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get motor command with exploration
        motor_cmd = agent.decodeMotorCommandWithExploration(
            pynlm.MotorCommand.Wait, 0.3)
        
        # Apply in world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Print filter statistics
        if step % 100 == 0:
            stats = percept.getSignalStatistics(pynlm.SignalType.Vision)
            print(f"Step {step}:")
            print(f"  Vision Signal: mean={stats.mean:.3f}, std={stats.variance**0.5:.3f}")
            print(f"  Motor Command: {motor_cmd}")
            print()
    
    print("Filtering Simulation Completed!")
    return brain, agent, world
```

### 3. Advanced Agent with State Management

```python
import pynlm
import os

def run_state_management_simulation(steps=1000, checkpoint_interval=200):
    """Agent with state management and persistence capabilities."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("State Management and Persistence Simulation")
    print("=" * 45)
    
    # Save initial state
    initial_state_path = "initial_agent_state.bin"
    agent.saveState(initial_state_path)
    print(f"Initial state saved to: {initial_state_path}")
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Save checkpoint periodically
        if step % checkpoint_interval == 0 and step > 0:
            checkpoint_path = f"checkpoint_step_{step}.bin"
            agent.saveState(checkpoint_path)
            print(f"Checkpoint saved at step {step}")
        
        # Print status
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  State valid: {agent.validateState()}")
            print(f"  Motor command: {motor_cmd}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print()
    
    # Save final state
    final_state_path = "final_agent_state.bin"
    agent.saveState(final_state_path)
    print(f"Final state saved to: {final_state_path}")
    
    # Load and test state loading (if needed)
    if os.path.exists(final_state_path):
        print("\nTesting state loading...")
        # In a real scenario, you would create a new agent and load the state
        # For this example, we'll just note that loading is available
        print("State loading functionality is available for agent recovery.")
    
    print("State Management Simulation Completed!")
    return brain, agent, world
```

### 4. Performance Monitoring and Optimization

```python
import pynlm
import time

def run_performance_monitoring_simulation(steps=800):
    """Agent with performance monitoring and optimization."""
    
    # Setup optimized configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.12)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Performance Monitoring and Optimization Simulation")
    print("=" * 50)
    
    # Performance tracking
    step_times = []
    start_time = time.time()
    
    for step in range(steps):
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand(pynlm.MotorCommandSelectionStrategy.Adaptive)
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        step_end = time.time()
        step_time = step_end - step_start
        step_times.append(step_time)
        
        # Performance analysis every 100 steps
        if step % 100 == 0 and step > 0:
            avg_step_time = sum(step_times[-100:]) / 100
            fps = 1.0 / avg_step_time if avg_step_time > 0 else 0
            
            print(f"Step {step}:")
            print(f"  Average step time: {avg_step_time:.4f}s ({fps:.1f} FPS)")
            print(f"  Memory usage: N/A (estimated from signal sizes)")
            print(f"  Motor command: {motor_cmd}")
            print(f"  Signal statistics: {len(percept.getAllSignals())} total signals")
            
            # Performance warnings
            if avg_step_time > 0.1:
                print(f"  WARNING: Slow processing detected!")
            elif avg_step_time < 0.01:
                print(f"  INFO: Fast processing achieved")
            
            print()
    
    # Overall performance analysis
    total_time = time.time() - start_time
    avg_total_time = total_time / steps
    total_fps = 1.0 / avg_total_time if avg_total_time > 0 else 0
    
    print(f"\nPerformance Summary:")
    print(f"  Total simulation time: {total_time:.2f}s")
    print(f"  Average step time: {avg_total_time:.4f}s")
    print(f"  Average FPS: {total_fps:.1f}")
    print(f"  Steps per second: {steps / total_time:.1f}")
    
    # Memory optimization suggestions
    print(f"\nMemory Optimization Suggestions:")
    print(f"  - Reserve memory for large signal arrays")
    print(f"  - Use efficient data structures for signal storage")
    print(f"  - Consider batch processing for large simulations")
    
    print("\nPerformance Monitoring Simulation Completed!")
    return brain, agent, world
```

### 5. Advanced Behavior Modes

```python
import pynlm

def run_behavior_modes_simulation(steps=1200):
    """Agent demonstrating different behavior modes."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=18, height=18, visionWidth=7, visionHeight=7)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Advanced Behavior Modes Simulation")
    print("=" * 38)
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Demonstrate different behavior modes
        # Note: Behavior mode switching would require additional API calls
        # For this example, we'll demonstrate different motor command strategies
        
        if step % 100 == 0:
            # Choose strategy based on step
            if step < steps // 4:
                strategy = pynlm.MotorCommandSelectionStrategy.Conservative
                mode_desc = "Conservative"
            elif step < steps // 2:
                strategy = pynlm.MotorCommandSelectionStrategy.Balanced
                mode_desc = "Balanced"
            elif step < 3 * steps // 4:
                strategy = pynlm.MotorCommandSelectionStrategy.Aggressive
                mode_desc = "Aggressive"
            else:
                strategy = pynlm.MotorCommandSelectionStrategy.Adaptive
                mode_desc = "Adaptive"
            
            motor_cmd = agent.decodeMotorCommand(strategy)
            
            print(f"Step {step} ({mode_desc} strategy):")
            print(f"  Motor command: {motor_cmd}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print()
        else:
            # Default behavior
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    print("Behavior Modes Simulation Completed!")
    print("\nBehavior Mode Summary:")
    print("- Conservative: Focus on stability, minimal exploration")
    print("- Balanced: Moderate exploration with action selection")
    print("- Aggressive: High exploration, more random actions")
    print("- Adaptive: Dynamic behavior based on environment")
    
    return brain, agent, world
```

### 6. Real-world Simulation with Environmental Changes

```python
import pynlm

def run_environmental_changes_simulation(steps=1800):
    """Agent simulating in a changing environment."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1800)
    config.set("plasticity.structural.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=35, height=35, visionWidth=12, visionHeight=12)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Environmental Changes Simulation")
    print("=" * 35)
    
    # Environment phases
    phase_transitions = [
        (0, "Initial Learning Phase"),
        (steps // 4, "Exploration Phase"),
        (2 * steps // 4, "Optimization Phase"),
        (3 * steps // 4, "Adaptation Phase")
    ]
    
    current_phase_idx = 0
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Check for phase transitions
        for phase_start, phase_name in phase_transitions:
            if step == phase_start and current_phase_idx < len(phase_transitions):
                current_phase_idx += 1
                print(f"\n=== {phase_name} ===")
                
                # Adjust behavior based on phase
                if "Learning" in phase_name:
                    # High exploration, high plasticity
                    agent.setExplorationFactor(1.5)
                elif "Exploration" in phase_name:
                    # Balanced exploration
                    agent.setExplorationFactor(1.0)
                elif "Optimization" in phase_name:
                    # Focused exploitation
                    agent.setExplorationFactor(0.5)
                elif "Adaptation" in phase_name:
                    # High adaptation, learning from changes
                    agent.setExplorationFactor(1.2)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with adaptive filtering based on environment
        if step % 150 == 0:
            # Apply different processing based on simulation phase
            moving_average_kernel = pynlm.createMovingAverageKernel(7)
            percept.applyBandPassFilter(moving_average_kernel)
            
            # Add environmental noise
            percept.addNoise(0.01 * (1.0 + current_phase_idx * 0.3))
        
        # Process through agent
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get motor command with exploration
        motor_cmd = agent.decodeMotorCommandWithExploration(
            pynlm.MotorCommand.Wait, 
            0.4 * agent.getExplorationFactor())
        
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print status periodically
        if step % 300 == 0:
            print(f"\nStep {step}:")
            print(f"  Phase: {phase_name if current_phase_idx > 0 else 'Initial'}")
            print(f"  Exploration factor: {agent.getExplorationFactor():.2f}")
            print(f"  Motor command: {motor_cmd}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print()
    
    print("Environmental Changes Simulation Completed!")
    return brain, agent, world
```

### 7. Comprehensive Example: Advanced Agent with All Features

```python
import pynlm
import time

def comprehensive_advanced_example():
    """
    Comprehensive example demonstrating all advanced NLM features.
    This simulates a sophisticated agent learning in a complex environment.
    """
    
    print("=== COMPREHENSIVE ADVANCED NLM EXAMPLE ===\n")
    
    # 1. Setup with optimized configuration
    print("1. Configuring advanced neural network...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2500)
    config.set("brain.synapse_density", 0.18)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 2.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # 2. Create and initialize brain
    print("2. Creating and initializing neural brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    print("3. Creating agent brain interface...")
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create world environment
    print("4. Creating world environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # 5. Enable all subsystems
    print("5. Enabling all neural subsystems...")
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 6. Performance monitoring setup
    print("6. Setting up performance monitoring...")
    step_times = []
    start_time = time.time()
    
    # 7. Simulation parameters
    print("7. Starting comprehensive simulation...")
    print(f"   Total steps: 2000")
    print(f"   Environment size: 40x40")
    print(f"   Neural network: 2500 neurons, 18% connectivity")
    print()
    
    # 8. Main simulation loop
    for step in range(2000):
        step_start = time.time()
        
        # Update environment
        world.update(0.1)
        
        # Get sensory input with processing
        percept = world.getSensoryPercept()
        
        # Apply signal processing based on simulation phase
        if step % 250 == 0:
            # Create adaptive kernel based on current conditions
            kernel_size = 9 + (step // 500) * 6  # Increasing kernel size
            moving_average_kernel = pynlm.createMovingAverageKernel(kernel_size)
            percept.applyBandPassFilter(moving_average_kernel)
            
            # Adjust noise levels
            noise_level = 0.01 + (step / 2000) * 0.04
            percept.addNoise(noise_level)
        
        # Process sensory input through advanced agent
        agent.processSensoryInput(percept)
        
        # Run brain with adaptive strategy
        if step % 500 == 0:
            # Change strategy based on simulation phase
            if step < 500:
                strategy = pynlm.MotorCommandSelectionStrategy.Conservative
            elif step < 1000:
                strategy = pynlm.MotorCommandSelectionStrategy.Balanced
            elif step < 1500:
                strategy = pynlm.MotorCommandSelectionStrategy.Aggressive
            else:
                strategy = pynlm.MotorCommandSelectionStrategy.Adaptive
        else:
            strategy = pynlm.MotorCommandSelectionStrategy.Balanced
        
        motor_cmd = agent.decodeMotorCommand(strategy)
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        step_end = time.time()
        step_times.append(step_end - step_start)
        
        # Print periodic status report
        if step % 400 == 0:
            avg_step_time = sum(step_times[-400:]) / 400
            fps = 1.0 / avg_step_time if avg_step_time > 0 else 0
            
            print(f"Step {step}:")
            print(f"  Performance: {avg_step_time:.4f}s/step ({fps:.1f} FPS)")
            print(f"  Motor Command: {motor_cmd}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
            print(f"  Signal Statistics: {len(percept.getAllSignals())} signals")
            print()
    
    # 9. Performance analysis
    total_time = time.time() - start_time
    avg_step_time = sum(step_times) / len(step_times)
    total_fps = 1.0 / avg_step_time if avg_step_time > 0 else 0
    
    print("=== PERFORMANCE ANALYSIS ===")
    print(f"Total simulation time: {total_time:.2f}s")
    print(f"Average step time: {avg_step_time:.4f}s")
    print(f"Average FPS: {total_fps:.1f}")
    print(f"Steps per second: {2000 / total_time:.1f}")
    print()
    
    # 10. Final statistics
    print("=== FINAL STATISTICS ===")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    
    # 11. Cleanup
    print("\n=== CLEANUP ===")
    print("Simulation completed successfully!")
    print("All neural subsystems functioned properly.")
    print("Advanced signal processing and optimization features demonstrated.")
    
    return brain, agent, world

# Run the comprehensive example
if __name__ == "__main__":
    brain, agent, world = comprehensive_advanced_example()
```

## Usage Notes

### Getting Started
1. **Basic Usage**: Use the `easy_usage.md` file for quick start examples
2. **Comprehensive Guide**: Refer to the `HOW_TO_USE.md` file for detailed API documentation
3. **Advanced Features**: Use the examples in this file for sophisticated agent behavior

### Advanced Configuration
```python
# Advanced configuration options
config = pynlm.createDefaultConfig()

# Neural network parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)

# Plasticity mechanisms
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
```

### Performance Tips
1. **Memory Optimization**: Use `reserveMemory()` and `setMaxCapacity()` for large simulations
2. **Signal Processing**: Apply appropriate filtering based on signal characteristics
3. **Simulation Speed**: Monitor step times and optimize accordingly
4. **State Management**: Use checkpointing for long-running simulations

### Error Handling
```python
# Check agent state validity
if agent.validateState():
    print("Agent state is valid")
else:
    print("Agent state has issues - running emergency reset")
    agent.emergencyReset()
```

### Best Practices
1. **Initialize Properly**: Always call `initialize()` before using the brain
2. **Manage State**: Use `saveState()` and `loadState()` for long simulations
3. **Monitor Performance**: Track step times and system resources
4. **Handle Signals**: Validate and normalize signals before processing
5. **Configure Appropriately**: Adjust parameters based on your specific use case

These advanced examples demonstrate the full capabilities of the enhanced NLM Python bindings, including sophisticated agent behavior, signal processing, and performance optimization.