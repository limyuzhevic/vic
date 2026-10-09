#!/usr/bin/env python3
"""
Advanced NLM Python API Usage Examples

This file contains comprehensive examples demonstrating advanced usage patterns
of the enhanced NLM Python API for neural simulation research and applications.

Each example demonstrates practical use cases with real-world scenarios,
performance optimization techniques, and step-by-step implementation guidance.
"""

import pynlm
import numpy as np
import time
import json
from typing import Dict, List, Any, Optional

def example_advanced_simulation_management():
    """
    Advanced Simulation Management: Parameter sweeps, batch processing, and
    multi-stage experiment execution with progress tracking and optimization.
    """
    print("=== Advanced Simulation Management Example ===")
    
    # 1. Parameter Sweep for Learning Rate Optimization
    print("\n1. Parameter Sweep for Learning Rate Optimization:")
    
    def run_simulation_with_config(learning_rate: float, neuron_count: int) -> Dict[str, Any]:
        """Run a single simulation with specific configuration"""
        config = pynlm.createDefaultConfig()
        config.set("plasticity.stdp.learning_rate", learning_rate)
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Run simulation with progress tracking
        print(f"  Running simulation with LR={learning_rate}, Neurons={neuron_count}")
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect results
        result = {
            "learning_rate": learning_rate,
            "neuron_count": neuron_count,
            "avg_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "curiosity_level": agent.getCuriosityLevel(),
            "dopamine_level": agent.getNeuromodulationLevel()
        }
        return result
    
    # Parameter sweep using batch processing
    learning_rates = [0.001, 0.005, 0.01, 0.05]
    neuron_counts = [500, 1000, 2000]
    
    sweep_results = []
    for lr in learning_rates:
        for neurons in neuron_counts:
            result = run_simulation_with_config(lr, neurons)
            sweep_results.append(result)
    
    print(f"  Completed parameter sweep with {len(sweep_results)} combinations")
    
    # Analyze results for optimization
    best_result = max(sweep_results, key=lambda x: x["avg_firing_rate"])
    print(f"  Best configuration: LR={best_result['learning_rate']}, Neurons={best_result['neuron_count']}")
    print(f"  Best performance: {best_result['avg_firing_rate']:.2f} avg firing rate")
    
    return sweep_results

def example_machine_learning_integration():
    """
    Machine Learning Integration: Training neural networks with NLM brains,
    reinforcement learning, and transfer learning.
    """
    print("\n=== Machine Learning Integration Example ===")
    
    # 1. Reinforcement Learning with NLM Brain
    print("\n1. Reinforcement Learning with NLM Brain:")
    
    def create_rl_agent(config: pynlm.Config) -> tuple:
        """Create an RL agent using NLM brain"""
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Enable learning capabilities
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Configure world for RL
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        return brain, world, agent
    
    def train_rl_agent(brain, world, agent, num_episodes: int) -> List[Dict[str, Any]]:
        """Train RL agent using reward-based learning"""
        training_history = []
        
        for episode in range(num_episodes):
            episode_start = time.time()
            
            # Reset environment
            world.reset()
            agent.reset()
            
            # Episode loop
            episode_reward = 0.0
            episode_steps = 0
            max_steps = 100
            
            for step in range(max_steps):
                # Select action using current brain state
                action = agent.decodeMotorCommand()
                
                # Execute action in environment
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get reward and observation
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                percept = world.getSensoryPercept()
                
                # Update brain with reward signal (reinforcement learning)
                agent.applyRewardModulation(reward, agent.getPredictionError())
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                episode_reward += reward
                episode_steps += 1
                
                # End episode if agent reached goal
                if episode_steps >= max_steps:
                    break
            
            episode_time = time.time() - episode_start
            
            # Store episode results
            episode_data = {
                "episode": episode,
                "reward": episode_reward,
                "steps": episode_steps,
                "time": episode_time,
                "curiosity": agent.getCuriosityLevel(),
                "dopamine": agent.getNeuromodulationLevel(),
                "development_stage": agent.getDevelopmentalStage().name
            }
            training_history.append(episode_data)
            
            print(f"  Episode {episode + 1}/{num_episodes}: Reward={episode_reward:.2f}, "
                  f"Steps={episode_steps}, Time={episode_time:.2f}s")
        
        return training_history
    
    # Create RL agent configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.learning_rate", 0.01)
    
    # Train RL agent
    brain, world, agent = create_rl_agent(config)
    print("  Starting RL training...")
    
    # Run different training regimes
    training_regimes = [
        {"episodes": 50, "description": "Quick training"},
        {"episodes": 200, "description": "Standard training"},
        {"episodes": 500, "description": "Extended training"}
    ]
    
    all_training_data = []
    for regime in training_regimes:
        print(f"\n  Running {regime['description']} ({regime['episodes']} episodes):")
        
        # Reset brain for each regime
        brain.reset()
        world.reset()
        agent.reset()
        
        # Train
        regime_data = train_rl_agent(brain, world, agent, regime["episodes"])
        all_training_data.extend(regime_data)
    
    # Analyze training results
    print("\n  Training Analysis:")
    final_performance = all_training_data[-1]
    print(f"    Final curiosity level: {final_performance['curiosity']:.3f}")
    print(f"    Final dopamine level: {final_performance['dopamine']:.3f}")
    print(f"    Final development stage: {final_performance['development_stage']}")
    
    return all_training_data

def example_performance_optimization():
    """
    Performance Optimization: Monitoring and optimizing neural simulation performance,
    memory usage analysis, and neuromodulation control.
    """
    print("\n=== Performance Optimization Example ===")
    
    # 1. Performance Monitoring and Optimization
    print("\n1. Performance Monitoring:")
    
    def create_performance_monitored_simulation(config: pynlm.Config) -> tuple:
        """Create a simulation with performance monitoring"""
        # Create brain with performance monitoring
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure performance monitoring
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Initialize world
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        return brain, world, agent
    
    def profile_simulation_performance(brain, world, agent, steps: int) -> Dict[str, Any]:
        """Profile simulation performance and resource usage"""
        print(f"  Profiling simulation with {steps} steps...")
        
        start_time = time.time()
        start_memory = None  # In real implementation, use memory_profiler
        
        total_reward = 0.0
        total_spikes = 0
        max_firing_rate = 0.0
        
        # Run simulation with detailed profiling
        for step in range(steps):
            # Time each step
            step_start = time.time()
            
            # Update world
            world.update(0.1)
            
            # Get sensory input and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            
            # Apply reward and decode action
            agent.applyRewardModulation(reward, 0.0)
            action = agent.decodeMotorCommand()
            
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update metrics
            total_reward += reward
            total_spikes += brain.getTotalSpikeCount()
            max_firing_rate = max(max_firing_rate, brain.getAverageFiringRate())
            
            # Performance per step
            step_time = time.time() - step_start
            
            # Optional: Log step performance for detailed analysis
            # print(f"    Step {step + 1}: {step_time*1000:.2f}ms, Reward={reward:.3f}")
        
        total_time = time.time() - start_time
        
        # Calculate performance metrics
        performance_metrics = {
            "total_time": total_time,
            "steps_per_second": steps / total_time,
            "total_reward": total_reward,
            "avg_reward_per_step": total_reward / steps,
            "total_spikes": total_spikes,
            "max_firing_rate": max_firing_rate,
            "avg_firing_rate": brain.getAverageFiringRate(),
            "steps_processed": steps,
            "time_per_step_ms": (total_time / steps) * 1000
        }
        
        print(f"  Performance summary:")
        print(f"    Total time: {total_time:.2f}s")
        print(f"    Steps/sec: {performance_metrics['steps_per_second']:.1f}")
        print(f"    Avg reward/step: {performance_metrics['avg_reward_per_step']:.3f}")
        print(f"    Total spikes: {performance_metrics['total_spikes']}")
        print(f"    Avg firing rate: {performance_metrics['avg_firing_rate']:.2f}")
        print(f"    Time per step: {performance_metrics['time_per_step_ms']:.2f}ms")
        
        return performance_metrics
    
    # Test different performance configurations
    config = pynlm.createDefaultConfig()
    brain, world, agent = create_performance_monitored_simulation(config)
    
    performance_tests = [
        {"steps": 50, "description": "Short simulation"},
        {"steps": 200, "description": "Medium simulation"},
        {"steps": 500, "description": "Long simulation"}
    ]
    
    performance_results = {}
    for test in performance_tests:
        # Reset for each test
        brain.reset()
        world.reset()
        agent.reset()
        
        result = profile_simulation_performance(brain, world, agent, test["steps"])
        performance_results[test["description"]] = result
    
    # Performance optimization recommendations
    print("\n  Performance Optimization Recommendations:")
    
    # Analyze scaling
    if performance_results["Short simulation"]["steps_per_second"] < 10:
        print("    ⚠️  Low performance detected - consider optimizing neural parameters")
    
    # Check memory usage (simplified)
    memory_efficient_config = pynlm.createDefaultConfig()
    memory_efficient_config.set("brain.neuron_count", 500)  # Smaller network
    memory_efficient_config.set("plasticity.stdp.enable", True)
    
    # Compare performance with optimized configuration
    brain_opt, world_opt, agent_opt = create_performance_monitored_simulation(memory_efficient_config)
    opt_result = profile_simulation_performance(brain_opt, world_opt, agent_opt, 200)
    
    original_result = performance_results["Medium simulation"]
    
    if opt_result["steps_per_second"] > original_result["steps_per_second"] * 1.2:
        print("    ✓ Optimized configuration shows performance improvement")
    
    return performance_results

def example_research_workflow():
    """
    Research Workflow: Large-scale neural network analysis, developmental trajectory
    studies, and reproducible research practices.
    """
    print("\n=== Research Workflow Example ===")
    
    # 1. Large-scale Neural Network Analysis
    print("\n1. Large-scale Neural Network Analysis:")
    
    def create_network_analysis_pipeline():
        """Create analysis pipeline for neural networks"""
        analysis_pipeline = {
            "network_configs": [],
            "performance_metrics": {},
            "connectivity_analysis": {},
            "learning_dynamics": {},
            "comparative_results": {}
        }
        return analysis_pipeline
    
    # Create multiple network configurations for comparative analysis
    network_configs = [
        {
            "name": "Minimal Network",
            "neuron_count": 500,
            "plasticity": "basic",
            "description": "Small network with basic plasticity"
        },
        {
            "name": "Standard Network",
            "neuron_count": 1000,
            "plasticity": "advanced",
            "description": "Medium network with advanced plasticity"
        },
        {
            "name": "High-performance Network",
            "neuron_count": 2000,
            "plasticity": "full",
            "description": "Large network with comprehensive plasticity"
        }
    ]
    
    analysis_results = {}
    
    for config_data in network_configs:
        print(f"\n  Analyzing {config_data['name']}:")
        print(f"    {config_data['description']}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_data["neuron_count"])
        
        # Set plasticity based on configuration
        if config_data["plasticity"] == "basic":
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", False)
            config.set("plasticity.structural.enable", False)
        elif config_data["plasticity"] == "advanced":
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", True)
            config.set("plasticity.structural.enable", False)
        else:  # full
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.hebbian.enable", True)
            config.set("plasticity.structural.enable", True)
        
        # Run network simulation
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Run simulation with detailed analysis
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Store analysis results
        analysis_results[config_data["name"]] = {
            "configuration": config_data,
            "neuron_count": config_data["neuron_count"],
            "plasticity": config_data["plasticity"],
            "performance": {
                "avg_firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
                "curiosity": agent.getCuriosityLevel(),
                "dopamine": agent.getNeuromodulationLevel()
            },
            "memory_systems": {
                "working_memory": brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0,
                "episodic_memory": brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
            }
        }
        
        print(f"    ✓ Analyzed {config_data['name']} network")
    
    # Comparative analysis
    print("\n  Comparative Analysis Results:")
    
    # Find best performing network for each metric
    best_by_firing_rate = max(analysis_results.values(), 
                            key=lambda x: x["performance"]["avg_firing_rate"])
    print(f"    Best firing rate: {best_by_firing_rate['name']} ({best_by_firing_rate['performance']['avg_firing_rate']:.2f})")
    
    best_by_stability = min(analysis_results.values(),
                          key=lambda x: abs(x["performance"]["e_i_ratio"] - 1.0))
    print(f"    Most stable E/I ratio: {best_by_stability['name']} ({best_by_stability['performance']['e_i_ratio']:.2f})")
    
    best_by_curiosity = max(analysis_results.values(),
                          key=lambda x: x["performance"]["curiosity"])
    print(f"    Highest curiosity: {best_by_curiosity['name']} ({best_by_curiosity['performance']['curiosity']:.3f})")
    
    return analysis_results

def example_reproducible_research():
    """
    Reproducible Research: Demonstrating reproducible research practices with
    configuration management, result serialization, and automated analysis.
    """
    print("\n=== Reproducible Research Example ===")
    
    # 1. Configuration Management for Reproducible Experiments
    print("\n1. Configuration Management:")
    
    def save_experiment_config(experiment_name: str, parameters: Dict[str, Any]) -> str:
        """Save experiment configuration to file"""
        config = pynlm.createDefaultConfig()
        
        # Set parameters
        if "neuron_count" in parameters:
            config.set("brain.neuron_count", parameters["neuron_count"])
        if "plasticity_rate" in parameters:
            config.set("plasticity.stdp.learning_rate", parameters["plasticity_rate"])
        if "world_size" in parameters:
            # Configure world (this would be in a real implementation)
            pass
        
        # Save configuration
        filename = f"experiment_{experiment_name}_config.json"
        config.saveToFile(filename)
        
        print(f"  Saved experiment configuration to {filename}")
        return filename
    
    def load_experiment_config(filename: str) -> pynlm.Config:
        """Load experiment configuration from file"""
        config = pynlm.createDefaultConfig()
        config.loadFromFile(filename)
        print(f"  Loaded experiment configuration from {filename}")
        return config
    
    # Create and save multiple experiment configurations
    experiments = [
        {"name": "control", "neuron_count": 1000, "plasticity_rate": 0.01},
        {"name": "treatment", "neuron_count": 2000, "plasticity_rate": 0.05},
        {"name": "baseline", "neuron_count": 500, "plasticity_rate": 0.001}
    ]
    
    experiment_files = {}
    for exp in experiments:
        filename = save_experiment_config(exp["name"], exp)
        experiment_files[exp["name"]] = filename
    
    # Run experiments using loaded configurations
    experiment_results = {}
    
    for exp_name, filename in experiment_files.items():
        print(f"\n  Running experiment: {exp_name}")
        
        config = load_experiment_config(filename)
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Save experiment results
        results = {
            "experiment": exp_name,
            "configuration": filename,
            "neuron_count": config.getOr("brain.neuron_count", 1000),
            "plasticity_rate": config.getOr("plasticity.stdp.learning_rate", 0.01),
            "performance": {
                "avg_firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount(),
                "curiosity": agent.getCuriosityLevel(),
                "dopamine": agent.getNeuromodulationLevel()
            }
        }
        
        # Save to JSON file
        result_filename = f"experiment_{exp_name}_results.json"
        with open(result_filename, 'w') as f:
            json.dump(results, f, indent=2)
        
        experiment_results[exp_name] = results
        print(f"    ✓ Completed experiment {exp_name}")
        print(f"    Results saved to {result_filename}")
    
    # Generate reproducibility report
    print("\n  Reproducibility Report:")
    print("    All experiments used identical code structure")
    print("    Each experiment had its own configuration file")
    print("    Results are saved in JSON format for analysis")
    print("    Configuration files can be shared for replication")
    
    return experiment_results

def main():
    """Run all advanced usage examples"""
    print("NLM Python API - Advanced Usage Examples")
    print("=" * 60)
    
    # Run all examples
    example_advanced_simulation_management()
    example_machine_learning_integration()
    example_performance_optimization()
    example_research_workflow()
    example_reproducible_research()
    
    print("\n" + "=" * 60)
    print("All advanced usage examples completed successfully!")
    print("\nKey takeaways:")
    print("• Parameter sweeps enable systematic exploration of neural parameters")
    print("• RL integration allows learning behavior training with NLM brains")
    print("• Performance monitoring helps optimize neural simulation efficiency")
    print("• Research workflows support large-scale neural network analysis")
    print("• Reproducible research practices ensure experiment replicability")
    
    print("\nFor more examples and documentation, see:")
    print("• NLM documentation: https://nlm.readthedocs.io")
    print("• Examples directory: ./examples/")
    print("• GitHub repository: https://github.com/nlm-project/nlm")

if __name__ == "__main__":
    main()
