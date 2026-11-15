#!/usr/bin/env python3
"""
NLM Integration Examples and Templates
=====================================

Comprehensive examples showing how to use the NLM Python bindings
for various simulation scenarios and use cases.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import time
import threading
from typing import Optional, Callable, Tuple


def example_minimal_brain():
    """Example 1: Minimal brain simulation - just get it running."""
    print("=== Example 1: Minimal Brain ===")
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Make it think
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()


def example_brain_in_world():
    """Example 2: Complete agent simulation with world interaction."""
    print("=== Example 2: Brain in a World ===")
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if i % 10 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("Brain-in-world simulation complete!")
    print()


def example_advanced_simulation():
    """Example 3: Advanced simulation with all features enabled."""
    print("=== Example 3: Advanced Simulation ===")
    
    # Create complete simulation
    brain, agent, world = create_advanced_agent_simulation()
    
    def progress_callback(step: int, progress: float):
        bar_length = 30
        filled = int(bar_length * progress)
        bar = "█" * filled + "░" * (bar_length - filled)
        percent = progress * 100
        print(f"\r[{bar}] {percent:5.1f}% Step {step}/1000", end="", flush=True)
    
    # Run simulation with progress
    print("Running advanced simulation...")
    start_time = time.time()
    
    total_spikes, avg_rate = run_advanced_simulation(
        brain, agent, world, 1000, progress_callback)
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nAdvanced simulation complete!")
    print(f"Duration: {duration:.1f} seconds")
    print(f"Total spikes: {total_spikes}")
    print(f"Average firing rate: {avg_rate:.2f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
    print()


def example_config_management():
    """Example 4: Configuration management and validation."""
    print("=== Example 4: Configuration Management ===")
    
    # Create configuration using presets
    config = pynlm.getPresetConfiguration("phase6_full")
    print("Created Phase 6 full configuration")
    
    # Validate configuration
    validation = pynlm.validateConfig(config)
    print(f"Configuration valid: {validation['config_valid']}")
    
    if validation['issues_found']:
        print("Issues found:", validation['issues_found'])
        print("Fixes applied:", validation['fixes_applied'])
    
    # Get configuration summary
    summary = pynlm.getConfigSummary(config)
    print("\nConfiguration summary:")
    for key, value in summary.items():
        print(f"  {key}: {value}")
    
    # Export configuration
    pynlm.exportConfig(config, "my_config.json")
    print("Configuration exported to my_config.json")
    
    # Import configuration
    imported_config = pynlm.importConfig("my_config.json")
    print("Configuration imported successfully")
    print()


def example_multithreaded_simulation():
    """Example 5: Multi-threaded simulation (conceptual)."""
    print("=== Example 5: Multi-threaded Simulation (Conceptual) ===")
    
    print("This example demonstrates the concept of multi-threaded simulation.")
    print("While the NLM C++ core is single-threaded, Python provides")
    print("threading for pre/post-processing and visualization.")
    print()
    
    # Create simulation
    brain, agent, world = create_advanced_agent_simulation()
    
    def process_simulation_chunk(brain, agent, world, start_step, end_step):
        """Process a chunk of simulation in a separate thread."""
        for step in range(start_step, end_step):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(0.1, 0.0)  # Simple reward
            agent.updateDevelopment(0.1)
    
    # Example of how to structure multi-threaded processing
    print("Thread structure for multi-threaded simulation:")
    print("1. Main thread: Coordinate and manage threads")
    print("2. Worker threads: Process simulation chunks")
    print("3. Visualization thread: Display real-time results")
    print("4. Data collection thread: Gather statistics")
    print()
    print("Note: Actual implementation requires careful synchronization")
    print("and the NLM C++ core currently doesn't support parallel execution.")
    print()


def example_reinforcement_learning():
    """Example 6: Reinforcement learning scenario."""
    print("=== Example 6: Reinforcement Learning ===")
    
    # Create agent for RL
    brain = pynlm.createBrain(pynlm.getPresetConfiguration("high_learning"))
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 8, 8)
    world.reset()
    world.setAgentStart(7.0, 7.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable RL-specific features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    # RL training loop
    episode = 0
    max_episodes = 100
    steps_per_episode = 200
    
    while episode < max_episodes:
        episode_rewards = []
        
        for step in range(steps_per_episode):
            # Get state
            percept = world.getSensoryPercept()
            state = agent.processSensoryInput(percept)
            
            # Select action
            action = agent.decodeMotorCommand()
            
            # Execute action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            episode_rewards.append(reward)
            
            # Apply reward to agent
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Update world
            world.update(0.1)
        
        # Episode statistics
        avg_reward = sum(episode_rewards) / len(episode_rewards)
        print(f"Episode {episode + 1}/{max_episodes}: "
              f"Avg reward: {avg_reward:.3f}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}")
        
        episode += 1
    
    print("Reinforcement learning simulation complete!")
    print()


def example_checkpoints_and_save():
    """Example 7: Checkpointing and state management."""
    print("=== Example 7: Checkpointing and State Management ===")
    
    # Create and run simulation to generate state
    brain, agent, world = create_advanced_agent_simulation()
    
    # Run for some steps to create interesting state
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save state
    checkpoint_path = "brain_checkpoint.json"
    brain.save(checkpoint_path)
    print(f"Saved brain state to {checkpoint_path}")
    
    # Get some statistics about saved state
    print(f"Saved brain had {brain.getTotalNeuronCount()} neurons and "
          f"{brain.getTotalSynapseCount()} synapses")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    
    # Create fresh brain and load state
    fresh_brain = pynlm.createBrain(pynlm.getPresetConfiguration("balanced"))
    fresh_brain.initialize()
    
    # Load state
    load_success = fresh_brain.load(checkpoint_path)
    if load_success:
        print("Successfully loaded brain state!")
        print(f"Loaded brain has {fresh_brain.getTotalNeuronCount()} neurons")
        print(f"Average firing rate: {fresh_brain.getAverageFiringRate():.2f}")
    else:
        print("Failed to load brain state")
    
    print()


def example_performance_benchmark():
    """Example 8: Performance benchmarking."""
    print("=== Example 8: Performance Benchmarking ===")
    
    import time
    
    def benchmark_simulation(config_preset: str, steps: int, iterations: int = 5):
        """Benchmark simulation performance."""
        times = []
        spikes_list = []
        
        for i in range(iterations):
            # Create fresh simulation
            config = pynlm.getPresetConfiguration(config_preset)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(10, 10, 6, 6)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Time the simulation
            start_time = time.time()
            
            for step in range(steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            end_time = time.time()
            duration = end_time - start_time
            times.append(duration)
            spikes_list.append(brain.getTotalSpikeCount())
            
            print(f"  Iteration {i + 1}: {duration:.2f}s, {brain.getTotalSpikeCount()} spikes")
        
        avg_time = sum(times) / len(times)
        std_time = (sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5
        avg_spikes = sum(spikes_list) / len(spikes_list)
        
        print(f"\nPerformance Summary ({config_preset}):")
        print(f"  Average time per simulation: {avg_time:.2f}s ± {std_time:.2f}s")
        print(f"  Average spikes per simulation: {avg_spikes:.0f}")
        print(f"  Steps per second: {steps / avg_time:.0f}")
        print(f"  Spikes per second: {avg_spikes / avg_time:.0f}")
        
        return {
            'avg_time': avg_time,
            'std_time': std_time,
            'avg_spikes': avg_spikes,
            'steps_per_second': steps / avg_time,
            'spikes_per_second': avg_spikes / avg_time
        }
    
    # Run benchmarks
    print("Running performance benchmarks...")
    
    # Benchmark different configurations
    configs = ["minimal", "balanced", "phase6_full"]
    steps = 500
    
    results = {}
    for config in configs:
        print(f"\n--- Benchmarking {config} configuration ---")
        results[config] = benchmark_simulation(config, steps)
    
    print("\nBenchmark comparison:")
    for config, result in results.items():
        print(f"  {config:15s}: {result['steps_per_second']:8.0f} steps/sec, "
              f"{result['spikes_per_second']:8.0f} spikes/sec")
    
    print()


def create_advanced_agent_simulation():
    """Create an advanced agent simulation with all features."""
    brain = pynlm.createBrain(pynlm.getPresetConfiguration("phase6_full"))
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world


def run_advanced_simulation(brain: pynlm.Brain, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld, 
                           num_steps: int = 1000, progress_callback: Optional[Callable[[int, float], None]] = None) -> Tuple[int, float]:
    """Run an advanced simulation with all features."""
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
        
        if progress_callback:
            progress = step / num_steps
            progress_callback(step, progress)
        
        if step % 100 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain.getTotalSpikeCount(), brain.getAverageFiringRate()


def print_examples_menu():
    """Print the examples menu."""
    print("\n" + "="*60)
    print("NLM Integration Examples")
    print("="*60)
    print("1. Minimal Brain - Simple brain creation and simulation")
    print("2. Brain in World - Complete agent simulation with world")
    print("3. Advanced Simulation - Full feature demonstration")
    print("4. Configuration Management - Config validation and presets")
    print("5. Multi-threaded Simulation - Conceptual threading approach")
    print("6. Reinforcement Learning - RL-style training loop")
    print("7. Checkpoints and Save - State management")
    print("8. Performance Benchmark - Performance testing")
    print("0. Exit")
    print("="*60)


def main_examples():
    """Main entry point for examples."""
    if not hasattr(pynlm, 'createBrain'):
        print("Error: pynlm module not available or missing required functions")
        return
    
    print("NLM Integration Examples")
    print("="*50)
    
    while True:
        print_examples_menu()
        
        try:
            choice = input("Select an example (0-8): ").strip()
            
            if choice == "1":
                example_minimal_brain()
            elif choice == "2":
                example_brain_in_world()
            elif choice == "3":
                example_advanced_simulation()
            elif choice == "4":
                example_config_management()
            elif choice == "5":
                example_multithreaded_simulation()
            elif choice == "6":
                example_reinforcement_learning()
            elif choice == "7":
                example_checkpoints_and_save()
            elif choice == "8":
                example_performance_benchmark()
            elif choice == "0":
                print("Goodbye!")
                break
            else:
                print("Invalid choice. Please select 0-8.")
                
        except KeyboardInterrupt:
            print("\nInterrupted by user.")
            break
        except Exception as e:
            print(f"Error running example: {e}")
            import traceback
            traceback.print_exc()


if __name__ == "__main__":
    main_examples()
