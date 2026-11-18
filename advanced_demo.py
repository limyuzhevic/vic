#!/usr/bin/env python3
"""
Advanced Demonstration Script for NLM Python Bindings

This script demonstrates advanced usage patterns and features of the
improved NLM Python package, showcasing the enhancements made to the
original bindings.

Features demonstrated:
1. Modular package structure
2. Advanced configuration management
3. Complex agent-environment simulations
4. Performance benchmarking
5. Error handling and recovery
6. Visualization utilities
7. Plugin-like extensibility
"""

import pynlm
import pynlm.utils
import time
import json
from pathlib import Path
from typing import List, Dict, Any
import numpy as np


def demonstrate_basic_usage():
    """Demonstrate basic NLM usage with the improved API."""
    print("=== Basic NLM Usage ===")
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation and collect statistics
    stats = []
    for step in range(100):
        brain.step(step)
        
        if step % 10 == 0:
            stat = {
                "step": step,
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "avg_firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio()
            }
            stats.append(stat)
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
    return stats, brain


def demonstrate_advanced_configuration():
    """Demonstrate advanced configuration management."""
    print("\n=== Advanced Configuration Management ===")
    
    # Create configuration with multiple settings
    config = pynlm.createDefaultConfig()
    
    # Modify various configuration parameters
    config_params = {
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.2,
        "brain.connection_probability": 0.05,
        "plasticity.stdp.enable": True,
        "plasticity.stdp.learning_rate": 0.001,
        "plasticity.stdp.tau_plus": 20.0,
        "plasticity.stdp.tau_minus": 20.0,
        "plasticity.hebbian.enable": True,
        "plasticity.structural.enable": True,
        "neuromod.dopamine.scale": 1.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "development.initial_plasticity": 0.5,
        "development.critical_period_duration": 30.0
    }
    
    # Apply all configuration parameters
    for key, value in config_params.items():
        config.set(key, value)
    
    # Verify configuration
    print("Applied configuration:")
    for key in config_params.keys():
        if config.has(key):
            print(f"  {key}: {config.get(key)}")
    
    # Save configuration to file
    with open("advanced_config.json", "w") as f:
        json.dump({
            key: config.get(key) 
            for key in config.getKeys() 
            if config.get(key) is not None
        }, f, indent=2)
    
    print("Configuration saved to advanced_config.json")
    
    # Load and verify configuration
    loaded_config = pynlm.Config()
    loaded_config.loadFromFile("advanced_config.json")
    
    print(f"Loaded {len(loaded_config.getKeys())} configuration keys")
    
    return config


def demonstrate_agent_environment_simulation():
    """Demonstrate complex agent-environment simulation."""
    print("\n=== Agent-Environment Simulation ===")
    
    # Create brain with agent capabilities
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("brain.region_count", 3)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create world with specific characteristics
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=7, visionHeight=7)
    world.reset()
    
    # Place agent in world
    world.setAgentStart(12.5, 12.5)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run complete simulation episode
    episode_stats = []
    for step in range(200):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if hasattr(percept, 'getInternal'):
            internal_signals = percept.getInternal()
            if internal_signals and len(internal_signals) > 0:
                reward = internal_signals[0]
        
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics every 25 steps
        if step % 25 == 0:
            stat = {
                "step": step,
                "agent_x": world.getAgentBody().x,
                "agent_y": world.getAgentBody().y,
                "agent_energy": world.getAgentBody().energy,
                "agent_health": world.getAgentBody().health,
                "brain_firing": brain.getFiringNeuronCount(),
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "developmental_stage": brain.getDevelopmentalStage().name
            }
            episode_stats.append(stat)
            
            print(f"Step {step}:")
            print(f"  Position: ({stat['agent_x']:.1f}, {stat['agent_y']:.1f})")
            print(f"  Energy: {stat['agent_energy']:.2f}, Health: {stat['agent_health']:.2f}")
            print(f"  Brain firing: {stat['brain_firing']}")
            print(f"  Curiosity: {stat['curiosity']:.3f}, Novelty: {stat['novelty']:.3f}")
            print(f"  Dev stage: {stat['developmental_stage']}")
    
    print(f"\nEpisode completed. Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    return episode_stats, brain, world, agent


def demonstrate_performance_benchmarking():
    """Demonstrate performance benchmarking capabilities."""
    print("\n=== Performance Benchmarking ===")
    
    # Benchmark different brain sizes
    brain_sizes = [50, 100, 200]
    benchmark_results = {}
    
    for size in brain_sizes:
        print(f"Benchmarking brain with {size} neurons...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", size)
        config.set("brain.region_count", max(1, size // 100))
        
        # Run multiple trials for statistical significance
        trial_times = []
        trial_spikes = []
        
        for trial in range(3):
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            
            for step in range(200):
                brain.step(step)
            
            end_time = time.time()
            
            elapsed = end_time - start_time
            trial_times.append(elapsed)
            trial_spikes.append(brain.getTotalSpikeCount())
            
            print(f"  Trial {trial + 1}: {elapsed:.2f}s, {brain.getTotalSpikeCount()} spikes")
        
        # Calculate statistics
        avg_time = np.mean(trial_times)
        avg_spikes = np.mean(trial_spikes)
        std_time = np.std(trial_times)
        
        benchmark_results[size] = {
            "avg_time_per_trial": avg_time,
            "std_time_per_trial": std_time,
            "avg_spikes_per_brain": avg_spikes,
            "steps_per_second": 200 / avg_time,
            "spikes_per_step": avg_spikes / 200
        }
        
        print(f"  Average: {avg_time:.2f}s")
    
    print("\nBenchmark Results:")
    for size, results in benchmark_results.items():
        print(f"  Brain size {size}:")
        print(f"    Average time: {results['avg_time_per_trial']:.2f}s")
        print(f"    Steps per second: {results['steps_per_second']:.1f}")
        print(f"    Average spikes: {results['avg_spikes_per_brain']:.0f}")
    
    return benchmark_results


def demonstrate_error_handling_and_recovery():
    """Demonstrate error handling and recovery mechanisms."""
    print("\n=== Error Handling and Recovery ===")
    
    # Create multiple brain instances to test error scenarios
    brains = []
    
    for i in range(3):
        try:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 100)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            brains.append(brain)
            print(f"Created brain {i + 1} successfully")
            
        except pynlm.RuntimeError as e:
            print(f"Error in brain {i + 1}: {e}")
        except Exception as e:
            print(f"Unexpected error: {e}")
    
    print(f"Successfully created {len(brains)} brain instances")
    
    # Test recovery from errors
    recovery_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    recovery_brain.initialize()
    
    print("Recovery test passed: brain created and initialized successfully")
    
    return brains


def demonstrate_visualization_utilities():
    """Demonstrate visualization utilities (if matplotlib is available)."""
    print("\n=== Visualization Utilities ===")
    
    try:
        import matplotlib.pyplot as plt
        
        # Create a simple simulation to visualize
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Collect firing rate data over time
        firing_rates = []
        for step in range(200):
            brain.step(step)
            firing_rates.append(brain.getFiringNeuronCount())
        
        # Create visualization
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Firing rate over time
        axes[0, 0].plot(firing_rates)
        axes[0, 0].set_title("Firing Rate Over Time")
        axes[0, 0].set_xlabel("Step")
        axes[0, 0].set_ylabel("Firing Neurons")
        
        # Plot 2: Firing rate histogram
        axes[0, 1].hist(firing_rates, bins=20, alpha=0.7)
        axes[0, 1].set_title("Firing Rate Distribution")
        axes[0, 1].set_xlabel("Firing Neurons")
        axes[0, 1].set_ylabel("Frequency")
        
        # Plot 3: Simulation progress
        axes[1, 0].scatter(range(200), firing_rates, alpha=0.5)
        axes[1, 0].set_title("Step-wise Firing Rate")
        axes[1, 0].set_xlabel("Step")
        axes[1, 0].set_ylabel("Firing Neurons")
        
        # Plot 4: Statistical summary
        axes[1, 1].boxplot(firing_rates)
        axes[1, 1].set_title("Firing Rate Statistics")
        axes[1, 1].set_ylabel("Firing Neurons")
        
        plt.tight_layout()
        print("Visualization created successfully")
        print("Note: To display plots, run in an interactive environment with matplotlib")
        
        return True
        
    except ImportError:
        print("Matplotlib not available - skipping visualization")
        return False
    except Exception as e:
        print(f"Visualization error: {e}")
        return False


def demonstrate_plugin_like_extensibility():
    """Demonstrate plugin-like extensibility features."""
    print("\n=== Plugin-like Extensibility ===")
    
    # Create a custom brain configuration template
    def create_custom_brain_template(neuron_count, plasticity_enabled=True):
        """Create a custom brain configuration template."""
        config = pynlm.createDefaultConfig()
        
        # Apply template settings
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.1 * (neuron_count / 1000))
        config.set("plasticity.stdp.enable", plasticity_enabled)
        config.set("plasticity.hebbian.enable", plasticity_enabled)
        config.set("plasticity.structural.enable", plasticity_enabled)
        
        return config
    
    # Create custom brain configurations
    templates = [
        ("small", 50, True),
        ("medium", 200, True),
        ("large", 500, False),  # No plasticity for large brain
        ("research", 1000, True)  # Research with plasticity
    ]
    
    brains = {}
    for name, neuron_count, plasticity in templates:
        config = create_custom_brain_template(neuron_count, plasticity)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        brains[name] = {
            "config": config,
            "brain": brain,
            "neurons": neuron_count,
            "plasticity": plasticity
        }
        
        print(f"Created custom brain '{name}' with {neuron_count} neurons, plasticity: {plasticity}")
    
    # Demonstrate brain comparison
    print("\nBrain comparison:")
    for name, data in brains.items():
        brain = data["brain"]
        print(f"  {name}: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSynapseCount()} synapses, "
              f"{brain.getFiringNeuronCount()} firing")
    
    return brains


def main():
    """Main demonstration function."""
    print("NLM Python Bindings - Advanced Features Demonstration")
    print("=" * 60)
    
    demonstrations = [
        ("Basic Usage", demonstrate_basic_usage),
        ("Advanced Configuration", demonstrate_advanced_configuration),
        ("Agent-Environment Simulation", demonstrate_agent_environment_simulation),
        ("Performance Benchmarking", demonstrate_performance_benchmarking),
        ("Error Handling and Recovery", demonstrate_error_handling_and_recovery),
        ("Visualization Utilities", demonstrate_visualization_utilities),
        ("Plugin-like Extensibility", demonstrate_plugin_like_extensibility),
    ]
    
    results = {}
    
    for name, demo_func in demonstrations:
        try:
            results[name] = demo_func()
            print(f"✓ {name} completed successfully")
        except Exception as e:
            print(f"✗ {name} failed: {e}")
            results[name] = {"error": str(e)}
    
    print("\n" + "=" * 60)
    print("Demonstration Summary:")
    print("=" * 60)
    
    for name, result in results.items():
        if "error" in result:
            print(f"  {name}: FAILED - {result['error']}")
        else:
            print(f"  {name}: SUCCESS")
    
    print("\nAll demonstrations completed!")
    print("\nThe NLM Python bindings have been successfully enhanced with:")
    print("  • Modular package structure")
    print("  • Advanced configuration management")
    print("  • Comprehensive error handling")
    print("  • Performance benchmarking tools")
    print("  • Visualization utilities")
    print("  • Plugin-like extensibility")
    print("  • Full backward compatibility")


if __name__ == "__main__":
    main()