# NLM Python Examples

This file contains practical examples for using the NLM Python API. Each example demonstrates different features and use cases.

## Example 1: Basic Brain Simulation

```python
"""
Example 1: Basic Brain Simulation

This example demonstrates how to create and run a simple neural simulation
without any external sensory input.
"""

import pynlm

def basic_brain_simulation():
    print("=== Basic Brain Simulation ===")
    
    # Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation for 100 steps
    for step in range(100):
        brain.step(step)
    
    print(f"Simulation complete!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return brain

if __name__ == "__main__":
    basic_brain_simulation()
```

## Example 2: Agent-World Interaction

```python
"""
Example 2: Agent-World Interaction

This example demonstrates how to create an agent that interacts with a world,
processes sensory input, and takes actions based on neural activity.
"""

import pynlm

def agent_world_interaction():
    print("=== Agent-World Interaction ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simple reward function
        reward = 0.0
        if world.getAgentBody().x > world.getWidth() * 0.8:
            reward = 1.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Print progress every 10 steps
        if step % 10 == 0:
            body = world.getAgentBody()
            print(f"Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print(f"Simulation complete!")
    print(f"Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain, agent, world

if __name__ == "__main__":
    brain, agent, world = agent_world_interaction()
```

## Example 3: Configuration Management

```python
"""
Example 3: Configuration Management

This example demonstrates how to create and manage complex configurations
for NLM simulations.
"""

import pynlm

def configuration_management():
    print("=== Configuration Management ===")
    
    # Example 1: High-performance configuration
    print("\n--- High-Performance Configuration ---")
    perf_config = pynlm.createDefaultConfig()
    perf_config.set("brain.neuron_count", 5000)
    perf_config.set("brain.connection_probability", 0.15)
    perf_config.set("simulation_timestep", 0.0001)
    perf_config.set("plasticity.stdp.learning_rate", 0.005)
    
    print(f"Neuron count: {perf_config.get('brain.neuron_count', 1000)}")
    print(f"Connection probability: {perf_config.get('brain.connection_probability', 0.1)}")
    print(f"Simulation timestep: {perf_config.get('simulation_timestep', 0.001)}")
    
    # Example 2: Learning-focused configuration
    print("\n--- Learning-Focused Configuration ---")
    learning_config = pynlm.createDefaultConfig()
    learning_config.set("brain.neuron_count", 1000)
    learning_config.set("brain.connection_probability", 0.2)
    learning_config.set("plasticity.stdp.learning_rate", 0.01)
    learning_config.set("plasticity.hebbian.enable", True)
    learning_config.set("neuromod.curiosity.enable", True)
    learning_config.set("neuromod.novelty.enable", True)
    
    print(f"Neuron count: {learning_config.get('brain.neuron_count', 1000)}")
    print(f"STDP learning rate: {learning_config.get('plasticity.stdp.learning_rate', 0.001)}")
    print(f"Hebbian enabled: {learning_config.get('plasticity.hebbian.enable', False)}")
    
    # Example 3: Developmental configuration
    print("\n--- Developmental Configuration ---")
    dev_config = pynlm.createDefaultConfig()
    dev_config.set("brain.neuron_count", 2000)
    dev_config.set("development_synaptogenesis_rate", 0.002)
    dev_config.set("development_pruning_rate", 0.0005)
    dev_config.set("neuromod.dopamine_baseline", 0.1)
    
    print(f"Neuron count: {dev_config.get('brain.neuron_count', 1000)}")
    print(f"Synaptogenesis rate: {dev_config.get('development_synaptogenesis_rate', 0.001)}")
    print(f"Pruning rate: {dev_config.get('development_pruning_rate', 0.0001)}")
    
    # Example 4: Using Pythonic setters
    print("\n--- Pythonic Configuration ---")
    pythonic_config = pynlm.createDefaultConfig()
    pythonic_config.set(
        brain_neuron_count=3000,
        brain_connection_probability=0.12,
        simulation_timestep=0.0005,
        plasticity_stdp_learning_rate=0.008
    )
    
    print(f"Neuron count: {pythonic_config.get('brain.neuron_count', 1000)}")
    print(f"Connection probability: {pythonic_config.get('brain.connection_probability', 0.1)}")
    
    # Example 5: Configuration validation
    print("\n--- Configuration Validation ---")
    test_config = pynlm.createDefaultConfig()
    test_config.set("brain.neuron_count", 5000)
    test_config.set("simulation_timestep", 0.001)
    test_config.set("connection_probability", 0.15)
    
    is_valid, errors = test_config.validate_config()
    if is_valid:
        print("Configuration is valid!")
    else:
        print(f"Configuration errors: {errors}")
    
    print("\nConfiguration management complete!")
    return perf_config, learning_config, dev_config, pythonic_config

if __name__ == "__main__":
    configs = configuration_management()
```

## Example 4: Memory Integration

```python
"""
Example 4: Memory Integration

This example demonstrates how to access and work with memory systems
integrated into the NLM brain.
"""

import pynlm

def memory_integration_example():
    print("=== Memory Integration Example ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Check memory systems integration
    print(f"Working memory available: {brain.getWorkingMemory() is not None}")
    print(f"Episodic memory available: {brain.getEpisodicMemory() is not None}")
    
    # Run simulation to generate memory
    for step in range(200):
        brain.step(step)
        
        # Inject some sensory activity to generate memory traces
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0f)
    
    # Check memory contents
    if brain.getWorkingMemory():
        active_traces = brain.getWorkingMemory().getActiveTraces()
        print(f"Working memory active traces: {active_traces}")
    
    if brain.getEpisodicMemory():
        episode_count = brain.getEpisodicMemory().getEpisodeCount()
        print(f"Episodic memory episodes: {episode_count}")
    
    # Create agent to demonstrate memory access
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    agent.initialize(world)
    
    print(f"Agent created with brain integration")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    
    print("\nMemory integration example complete!")
    return brain, agent

if __name__ == "__main__":
    brain, agent = memory_integration_example()
```

## Example 5: Advanced Configuration

```python
"""
Example 5: Advanced Configuration and Error Handling

This example demonstrates advanced configuration techniques and proper
error handling in NLM simulations.
"""

import pynlm

def advanced_configuration_example():
    print("=== Advanced Configuration Example ===")
    
    # Create configuration with validation
    config = pynlm.createDefaultConfig()
    
    # Set configuration using Pythonic interface
    config.set(
        brain_neuron_count=2000,
        brain_connection_probability=0.08,
        simulation_timestep=0.0005,
        plasticity_stdp_learning_rate=0.003,
        plasticity_hebbian_enable=True,
        neuromod_curiosity_enable=True,
        neuromod_novelty_enable=True,
        neuromod_dopamine_scale=1.2
    )
    
    # Validate configuration
    is_valid, errors = config.validate_config()
    if not is_valid:
        print("Configuration validation failed:")
        for error in errors:
            print(f"  - {error}")
        raise ValueError("Invalid configuration")
    
    print("Configuration validation passed!")
    
    # Create brain with validated configuration
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"Brain created successfully with {brain.getTotalNeuronCount()} neurons")
    except Exception as e:
        print(f"Failed to create brain: {e}")
        return None
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("All subsystems enabled")
    
    # Run simulation with error handling
    for step in range(100):
        try:
            # Update world
            world.update(0.1)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate and apply reward
            reward = 0.0
            body = world.getAgentBody()
            if body.x > world.getWidth() * 0.9:
                reward = 1.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Log progress
            if step % 20 == 0:
                print(f"Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                      f"Reward: {reward:.1f}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
                      
        except Exception as e:
            print(f"Error at step {step}: {e}")
            break
    
    # Get final statistics
    print(f"\nSimulation complete!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

if __name__ == "__main__":
    brain, agent, world = advanced_configuration_example()
```

## Example 6: Performance Monitoring

```python
"""
Example 6: Performance Monitoring

This example demonstrates how to monitor and measure performance
of NLM simulations.
"""

import pynlm
import time

def performance_monitoring_example():
    print("=== Performance Monitoring Example ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Measure performance metrics
    start_time = time.time()
    start_step = 0
    
    steps_completed = 0
    total_spikes = 0
    max_firing_rate = 0.0
    
    # Run simulation with monitoring
    for step in range(500):
        brain.step(step)
        
        steps_completed += 1
        total_spikes += brain.getTotalSpikeCount()
        
        current_firing_rate = brain.getAverageFiringRate()
        if current_firing_rate > max_firing_rate:
            max_firing_rate = current_firing_rate
        
        # Log performance every 100 steps
        if step % 100 == 0:
            elapsed = time.time() - start_time
            steps_per_second = steps_completed / elapsed if elapsed > 0 else 0
            
            print(f"Step {step}:")
            print(f"  Elapsed time: {elapsed:.2f}s")
            print(f"  Steps/second: {steps_per_second:.1f}")
            print(f"  Current firing rate: {current_firing_rate:.2f}")
            print(f"  Max firing rate: {max_firing_rate:.2f}")
            print(f"  Total spikes: {total_spikes}")
    
    # Calculate final performance metrics
    total_time = time.time() - start_time
    avg_firing_rate = total_spikes / steps_completed if steps_completed > 0 else 0
    steps_per_second = steps_completed / total_time if total_time > 0 else 0
    
    print(f"\n=== Performance Summary ===")
    print(f"Total simulation time: {total_time:.2f}s")
    print(f"Total steps: {steps_completed}")
    print(f"Average firing rate: {avg_firing_rate:.2f}")
    print(f"Maximum firing rate: {max_firing_rate:.2f}")
    print(f"Performance: {steps_per_second:.1f} steps/second")
    
    # Create agent for additional metrics
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    agent.initialize(world)
    
    print(f"Neuromodulation metrics:")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    return brain, agent, total_time, steps_completed

if __name__ == "__main__":
    brain, agent, total_time, steps_completed = performance_monitoring_example()
```

## Example 7: Experiment Management

```python
"""
Example 7: Experiment Management

This example demonstrates how to use the experiment framework to
manage and analyze NLM experiments.
"""

import pynlm

def experiment_management_example():
    print("=== Experiment Management Example ===")
    
    # Create experiment configuration
    from pynlm import Phase6Config, Phase6IntegratedExperiment
    
    config = Phase6Config()
    config.neuronCount = 500
    config.maxSteps = 1000
    config.enableCheckpointing = True
    config.enableReplay = True
    config.enableDevelopment = True
    
    print(f"Experiment configuration:")
    print(f"  Neuron count: {config.neuronCount}")
    print(f"  Max steps: {config.maxSteps}")
    print(f"  Checkpointing: {config.enableCheckpointing}")
    print(f"  Replay: {config.enableReplay}")
    print(f"  Development: {config.enableDevelopment}")
    
    # Run integration experiment
    experiment = Phase6IntegratedExperiment()
    
    print("\nRunning integration experiment...")
    result = experiment.run(config)
    
    # Analyze results
    print(f"\n=== Experiment Results ===")
    print(f"Total reward: {result.totalReward}")
    print(f"Average firing rate: {result.avgFiringRate}")
    print(f"Memory episodes stored: {result.memoryEpisodesStored}")
    print(f"Dopamine level: {result.dopamineLevel}")
    
    print(f"\n=== Integration Status ===")
    print(f"Working memory integrated: {result.memoryWorkingMemoryIntegrated}")
    print(f"Episodic memory integrated: {result.memoryEpisodicMemoryIntegrated}")
    print(f"Neuromodulation integrated: {result.neuromodulationIntegrated}")
    print(f"Prediction integrated: {result.predictionIntegrated}")
    print(f"Development integrated: {result.developmentIntegrated}")
    print(f"Checkpointing works: {result.checkpointingWorks}")
    print(f"Replay works: {result.replayWorks}")
    
    print(f"\n=== Timing ===")
    print(f"Total wall clock time: {result.totalWallClockTime:.2f}s")
    
    # Create analysis function
    def analyze_experiment_results(result):
        """Analyze experiment results and provide insights."""
        analysis = {
            "reward_efficiency": result.totalReward / result.totalWallClockTime,
            "firing_efficiency": result.avgFiringRate / result.totalWallClockTime,
            "memory_efficiency": result.memoryEpisodesStored / result.totalWallClockTime,
            "system_integration": sum([
                result.memoryWorkingMemoryIntegrated,
                result.memoryEpisodicMemoryIntegrated,
                result.neuromodulationIntegrated,
                result.predictionIntegrated,
                result.developmentIntegrated
            ]) / 5.0
        }
        
        print(f"\n=== Experiment Analysis ===")
        print(f"Reward efficiency: {analysis['reward_efficiency']:.3f} reward/s")
        print(f"Firing efficiency: {analysis['firing_efficiency']:.3f} Hz/s")
        print(f"Memory efficiency: {analysis['memory_efficiency']:.3f} episodes/s")
        print(f"Overall system integration: {analysis['system_integration']:.1f}/1.0")
        
        # Determine experiment success
        success_criteria = (
            analysis['reward_efficiency'] > 0.1 and
            analysis['system_integration'] > 0.8 and
            result.checkpointingWorks
        )
        
        if success_criteria:
            print("✅ Experiment SUCCESS!")
        else:
            print("❌ Experiment failed to meet success criteria")
        
        return analysis
    
    analysis = analyze_experiment_results(result)
    
    print(f"\nExperiment management example complete!")
    return result, analysis

if __name__ == "__main__":
    result, analysis = experiment_management_example()
```

## Example 8: Visualization

```python
"""
Example 8: Visualization (if visualization library is available)

This example demonstrates how to visualize neural simulation results.
Note: Requires visualization library to be installed.
"""

import pynlm

def visualization_example():
    print("=== Visualization Example ===")
    
    try:
        import matplotlib.pyplot as plt
        import numpy as np
        
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Collect firing rate data over time
        firing_rates = []
        spike_counts = []
        steps = []
        
        for step in range(500):
            brain.step(step)
            
            firing_rates.append(brain.getAverageFiringRate())
            spike_counts.append(brain.getTotalSpikeCount())
            steps.append(step)
        
        # Create visualization
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
        
        # Plot 1: Firing rate over time
        ax1.plot(steps, firing_rates, 'b-', linewidth=1, label='Average Firing Rate')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Firing Rate (Hz)')
        ax1.set_title('Neural Firing Rate Over Time')
        ax1.grid(True, alpha=0.3)
        ax1.legend()
        
        # Plot 2: Cumulative spike count over time
        ax2.plot(steps, spike_counts, 'r-', linewidth=1, label='Cumulative Spike Count')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Spike Count')
        ax2.set_title('Cumulative Spike Count Over Time')
        ax2.grid(True, alpha=0.3)
        ax2.legend()
        
        plt.tight_layout()
        plt.savefig('nlm_simulation_results.png', dpi=150)
        print("Visualization saved to 'nlm_simulation_results.png'")
        
        # Show statistics
        print(f"\nVisualization statistics:")
        print(f"Average firing rate: {np.mean(firing_rates):.2f} Hz")
        print(f"Maximum firing rate: {np.max(firing_rates):.2f} Hz")
        print(f"Final spike count: {spike_counts[-1]}")
        
        plt.close()
        
    except ImportError:
        print("Matplotlib not available. Skipping visualization example.")
        print("To install matplotlib: pip install matplotlib")
    
    return brain

if __name__ == "__main__":
    brain = visualization_example()
```

## Example 9: Complete Agent with Custom Reward

```python
"""
Example 9: Complete Agent with Custom Reward Function

This example demonstrates how to create an agent with a custom reward function
and more complex behavior.
"""

import pynlm

class AdvancedAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        
        # Enable all learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Custom state tracking
        self.step_count = 0
        self.reward_history = []
        self.position_history = []
    
    def calculate_custom_reward(self, brain, world, action):
        """Calculate custom reward based on complex criteria."""
        body = world.getAgentBody()
        
        # Multiple reward components
        reward = 0.0
        
        # 1. Position-based reward
        target_x = world.getWidth() * 0.8
        distance_to_target = abs(body.x - target_x)
        position_reward = max(0.0, 1.0 - distance_to_target / world.getWidth())
        reward += position_reward * 2.0
        
        # 2. Novelty-based exploration reward
        novelty_reward = self.agent.getNoveltyLevel() * 0.5
        reward += novelty_reward
        
        # 3. Curiosity-driven exploration bonus
        curiosity_reward = self.agent.getCuriosityLevel() * 0.3
        reward += curiosity_reward
        
        # 4. Safety penalty (avoid hazards)
        hazard_penalty = 0.0
        if hasattr(world, 'getHazards'):
            hazards = world.getHazards()
            for hazard in hazards:
                distance_to_hazard = ((body.x - hazard.x) ** 2 + 
                                    (body.y - hazard.y) ** 2) ** 0.5
                if distance_to_hazard < hazard.radius * 1.5:
                    hazard_penalty -= 1.0
        reward += hazard_penalty
        
        # 5. Progress reward based on distance traveled
        if len(self.position_history) > 0:
            last_pos = self.position_history[-1]
            distance_traveled = ((body.x - last_pos[0]) ** 2 + 
                               (body.y - last_pos[1]) ** 2) ** 0.5
            progress_reward = min(distance_traveled / 5.0, 0.5)
            reward += progress_reward
        
        # Store reward for analysis
        self.reward_history.append(reward)
        
        return reward
    
    def run_episode(self, max_steps=200):
        """Run an episode with the advanced agent."""
        print(f"Starting episode with {max_steps} steps")
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get current state
            body = self.world.getAgentBody()
            self.position_history.append((body.x, body.y))
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Custom action selection
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate and apply custom reward
            reward = self.calculate_custom_reward(self.brain, self.world, action)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Log progress
            if step % 20 == 0:
                print(f"Step {step}: Reward: {reward:.3f}, "
                      f"Position: ({body.x:.1f}, {body.y:.1f}), "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {self.agent.getNoveltyLevel():.3f}")
        
        # Calculate episode statistics
        if self.reward_history:
            avg_reward = sum(self.reward_history) / len(self.reward_history)
            max_reward = max(self.reward_history)
            min_reward = min(self.reward_history)
            
            print(f"\nEpisode complete!")
            print(f"Average reward: {avg_reward:.3f}")
            print(f"Max reward: {max_reward:.3f}")
            print(f"Min reward: {min_reward:.3f}")
            print(f"Total steps: {len(self.reward_history)}")
        
        return self.brain, self.agent, self.world

def advanced_agent_example():
    print("=== Advanced Agent Example ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    # Create advanced agent
    advanced_agent = AdvancedAgent(brain, world)
    
    # Run episode
    brain, agent, world = advanced_agent.run_episode(100)
    
    print(f"\nAdvanced agent example complete!")
    return brain, agent, world

if __name__ == "__main__":
    brain, agent, world = advanced_agent_example()
```

## Summary

These examples demonstrate the key features and capabilities of the NLM Python API:

1. **Basic Usage**: Simple brain simulations and agent-world interaction
2. **Configuration**: Advanced configuration management and validation
3. **Memory Integration**: Working with integrated memory systems
4. **Error Handling**: Robust error handling and logging
5. **Performance Monitoring**: Measuring and optimizing simulation performance
6. **Experiment Management**: Running and analyzing NLM experiments
7. **Visualization**: Visualizing simulation results
8. **Advanced Agents**: Custom agent behavior with complex reward functions

Each example can be run independently and serves as a template for building more complex NLM simulations.
