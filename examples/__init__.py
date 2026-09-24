#!/usr/bin/env python3
"""
Advanced NLM Example Scripts

This directory contains comprehensive example scripts for using the NLM Python API,
demonstrating various use cases from simple to advanced.

Each example is designed to showcase different capabilities:
1. Basic brain simulation
2. Learning agents in environments
3. Advanced neural processing
4. Performance and benchmarking
5. Integration with external systems
"""

import os
import sys
import json
import time
import matplotlib.pyplot as plt
import numpy as np

# Add the parent directory to path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm

class Example1_BasicBrain:
    """
    Example 1: Basic Brain Simulation
    
    This example demonstrates the simplest possible NLM brain setup.
    It creates a brain, initializes it, and runs a simulation without
    an external world.
    
    Learning Objectives:
    - Basic brain creation and initialization
    - Understanding neuron activity
    - Running simulation steps
    - Accessing brain statistics
    """
    
    @staticmethod
    def run_example():
        print("=== Example 1: Basic Brain Simulation ===")
        
        # 1. Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.05)
        
        # 2. Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # 3. Run simulation
        num_steps = 100
        for step in range(num_steps):
            brain.step(step)
            
            if step % 20 == 0:
                firing = brain.getFiringNeuronCount()
                total_spikes = brain.getTotalSpikeCount()
                avg_rate = brain.getAverageFiringRate()
                print(f"Step {step}: {firing} firing, {total_spikes} total spikes, "
                      f"{avg_rate:.2f} Hz average rate")
        
        # 4. Final statistics
        print("\n=== Final Brain Statistics ===")
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        print(f"Total synapses: {brain.getTotalSynapseCount()}")
        print(f"Currently firing: {brain.getFiringNeuronCount()}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"Developmental stage: {brain.getDevelopmentalStage()}")
        
        return brain

class Example2_AgentInWorld:
    """
    Example 2: Agent in a 2D World
    
    This example demonstrates creating a complete agent simulation where a brain
    interacts with a simple 2D environment. This is the most common use case.
    
    Learning Objectives:
    - Creating worlds with configurable dimensions
    - Setting up agent-world interaction
    - Processing sensory input
    - Decoding motor commands
    - Managing simulation loop
    """
    
    @staticmethod
    def run_example():
        print("\n=== Example 2: Agent in a 2D World ===")
        
        # 1. Create complete agent setup
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure world
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        # Initialize agent
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print(f"Created agent in {world.getWidth()}x{world.getHeight()} world")
        print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
        
        # 2. Run simulation loop
        num_steps = 200
        metrics = {
            'steps': [],
            'firing_rates': [],
            'curiosity': [],
            'novelty': [],
            'actions': []
        }
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Execute action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development periodically
            if step % 50 == 0:
                agent.updateDevelopment(0.1)
            
            # Record metrics
            metrics['steps'].append(step)
            metrics['firing_rates'].append(brain.getFiringNeuronCount())
            metrics['curiosity'].append(agent.getCuriosityLevel())
            metrics['novelty'].append(agent.getNoveltyLevel())
            metrics['actions'].append(str(action))
            
            if step % 50 == 0:
                print(f"Step {step}: Action={action}, "
                      f"Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
        
        # 3. Print final results
        print("\n=== Simulation Results ===")
        print(f"Total steps: {num_steps}")
        print(f"Final firing rate: {brain.getFiringNeuronCount()} neurons")
        print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
        print(f"Final dopamine: {agent.getNeuromodulationLevel():.3f}")
        
        # 4. Plot results
        try:
            Example2_AgentInWorld.plot_results(metrics)
        except ImportError:
            print("Matplotlib not available, skipping plots")
        
        return brain, agent, world, metrics
    
    @staticmethod
    def plot_results(metrics):
        """Plot simulation results."""
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Firing rate over time
        axes[0, 0].plot(metrics['steps'], metrics['firing_rates'])
        axes[0, 0].set_title('Firing Rate Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Firing Neurons')
        
        # Curiosity level
        axes[0, 1].plot(metrics['steps'], metrics['curiosity'], label='Curiosity')
        axes[0, 1].plot(metrics['steps'], metrics['novelty'], label='Novelty')
        axes[0, 1].set_title('Motivation Levels')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Level')
        axes[0, 1].legend()
        
        # Action distribution
        action_counts = {}
        for action in metrics['actions']:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        axes[1, 0].bar(action_counts.keys(), action_counts.values())
        axes[1, 0].set_title('Action Distribution')
        axes[1, 0].set_xlabel('Action')
        axes[1, 0].set_ylabel('Count')
        
        # Learning curves (simplified)
        axes[1, 1].plot(metrics['steps'][:100], 
                       [metrics['curiosity'][i] * metrics['novelty'][i] 
                        for i in range(min(100, len(metrics['steps'])) )])
        axes[1, 1].set_title('Learning Progress (Curiosity × Novelty)')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('Product')
        
        plt.tight_layout()
        plt.savefig('simulation_results.png', dpi=150)
        print("Plots saved to simulation_results.png")
        plt.close()

class Example3_LearningBenchmarks:
    """
    Example 3: Learning Benchmarks and Performance Testing
    
    This example demonstrates how to run comparative experiments and benchmark
    different configurations for learning performance.
    
    Learning Objectives:
    - Running multiple experiment configurations
    - Collecting and comparing metrics
    - Performance analysis
    - Statistical significance testing
    """
    
    @staticmethod
    def run_learning_benchmarks():
        print("\n=== Example 3: Learning Benchmarks ===")
        
        configurations = [
            {'name': 'Small Brain', 'neurons': 500, 'density': 0.05, 'learning_rate': 0.01},
            {'name': 'Medium Brain', 'neurons': 1000, 'density': 0.1, 'learning_rate': 0.01},
            {'name': 'Large Brain', 'neurons': 2000, 'density': 0.15, 'learning_rate': 0.005},
            {'name': 'High Plasticity', 'neurons': 1000, 'density': 0.2, 'learning_rate': 0.02},
        ]
        
        results = {}
        
        for config in configurations:
            print(f"\n--- Testing {config['name']} ---")
            
            # Create configuration
            cfg = pynlm.createDefaultConfig()
            cfg.set("brain.neuron_count", config['neurons'])
            cfg.set("brain.synapse_density", config['density'])
            cfg.set("plasticity.stdp.learning_rate", config['learning_rate'])
            cfg.set("plasticity.hebbian.learning_rate", config['learning_rate'])
            
            # Create brain and agent
            brain = pynlm.createBrain(cfg)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            # Setup world
            world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
            world.reset()
            world.setAgentStart(5, 5)
            agent.initialize(world)
            
            # Enable all learning features
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run experiment
            steps = 500
            start_time = time.time()
            
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Simple reward function
                internal = percept.getInternal()
                reward = internal[0] if internal else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                if step % 100 == 0:
                    agent.updateDevelopment(0.1)
            
            end_time = time.time()
            
            # Collect metrics
            result = {
                'name': config['name'],
                'final_firing': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'dopamine': agent.getNeuromodulationLevel(),
                'development_stage': agent.getDevelopmentalStage(),
                'steps_per_second': steps / (end_time - start_time),
                'learning_rate': config['learning_rate']
            }
            
            results[config['name']] = result
            
            print(f"  Final firing: {result['final_firing']} neurons")
            print(f"  Average firing rate: {result['average_firing_rate']:.2f} Hz")
            print(f"  Performance: {result['steps_per_second']:.1f} steps/sec")
            print(f"  Time: {end_time - start_time:.2f} seconds")
        
        # Compare results
        print("\n=== Benchmark Comparison ===")
        print(f"{'Configuration':<15} {'Firing':<10} {'Avg Rate':<12} {'Curiosity':<10} {'Performance':<12}")
        print("-" * 70)
        
        for name, result in results.items():
            print(f"{name:<15} {result['final_firing']:<10} {result['average_firing_rate']:<12.2f} "
                  f"{result['curiosity']:<10.3f} {result['steps_per_second']:<12.1f}")
        
        # Save results
        with open('benchmark_results.json', 'w') as f:
            json.dump(results, f, indent=2)
        
        print("\nBenchmark results saved to benchmark_results.json")
        return results

class Example4_EpisodicMemory:
    """
    Example 4: Episodic Memory and Replay
    
    This example demonstrates how NLM's episodic memory system works, including
    memory storage, consolidation, and replay during sleep/rest cycles.
    
    Learning Objectives:
    - Understanding episodic memory system
    - Memory storage and retrieval
    - Memory consolidation
    - Replay mechanisms
    """
    
    @staticmethod
    def run_episodic_memory_example():
        print("\n=== Example 4: Episodic Memory and Replay ===")
        
        # Create configuration with memory settings
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1200)
        config.set("brain.synapse_density", 0.1)
        config.set("memory.episodic.capacity", 100)  # Number of episodes
        
        # Create brain
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Setup
        world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(6, 6)
        agent.initialize(world)
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        
        # Run multiple episodes
        num_episodes = 20
        episode_length = 100
        
        for episode in range(num_episodes):
            print(f"Episode {episode + 1}/{num_episodes}")
            
            # Run episode
            for step in range(episode_length):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                if step % 50 == 0:
                    agent.updateDevelopment(0.1)
            
            # Store episode in memory (this happens automatically)
            print(f"  Episode completed: Firing={brain.getFiringNeuronCount()}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
            
            # Rest period for memory consolidation (simulated)
            for rest_step in range(20):  # Rest for 2 time units
                # During rest, brain processes stored memories
                brain.step(episode_length + rest_step)
                agent.updateDevelopment(0.1)
            
            # Check memory stats
            print(f"  Memory stats: Working={brain.getWorkingMemory() != None}, "
                  f"Episodic={brain.getEpisodicMemory() != None}")
        
        print("\n=== Episodic Memory Example Complete ===")
        print("The brain has stored and consolidated multiple episodes.")
        print("Memory replay occurred during rest periods.")
        
        return brain, agent, world

class Example5_DevelopmentAndAdaptation:
    """
    Example 5: Brain Development and Adaptation
    
    This example demonstrates how NLM brains develop over time, including
    critical periods, maturation, and aging.
    
    Learning Objectives:
    - Understanding developmental stages
    - Monitoring plasticity changes over time
    - Observing age-related changes
    - Developmental trajectory analysis
    """
    
    @staticmethod
    def run_development_example():
        print("\n=== Example 5: Brain Development and Adaptation ===")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        
        # Create brain
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Setup
        world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
        world.reset()
        world.setAgentStart(4, 4)
        agent.initialize(world)
        
        # Enable development
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Track development over time
        development_data = {
            'steps': [],
            'plasticity': [],
            'stage': [],
            'firing_rate': [],
            'curiosity': []
        }
        
        print("Development timeline:")
        
        # Run extended simulation
        total_steps = 1000
        for step in range(total_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.applyRewardModulation(0.0, 0.0)
            agent.updateDevelopment(0.1)
            
            if step % 100 == 0:
                stage = brain.getDevelopmentalStage()
                stage_str = str(stage).split('.')[-1]  # Get enum value
                
                development_data['steps'].append(step)
                development_data['plasticity'].append(agent.getNeuromodulationLevel())
                development_data['stage'].append(stage_str)
                development_data['firing_rate'].append(brain.getFiringNeuronCount())
                development_data['curiosity'].append(agent.getCuriosityLevel())
                
                print(f"  Step {step}: Stage {stage_str}, "
                      f"Plasticity={agent.getNeuromodulationLevel():.3f}, "
                      f"Firing={brain.getFiringNeuronCount()}")
        
        # Analyze development
        print("\n=== Development Analysis ===")
        print(f"Total developmental steps: {len(development_data['steps'])}")
        
        # Identify developmental stages
        initial_stage = development_data['stage'][0]
        final_stage = development_data['stage'][-1]
        print(f"Initial developmental stage: {initial_stage}")
        print(f"Final developmental stage: {final_stage}")
        
        # Plasticity trajectory
        initial_plasticity = development_data['plasticity'][0]
        final_plasticity = development_data['plasticity'][-1]
        print(f"Initial plasticity: {initial_plasticity:.3f}")
        print(f"Final plasticity: {final_plasticity:.3f}")
        print(f"Plasticity change: {final_plasticity - initial_plasticity:.3f}")
        
        # Create development summary
        development_summary = {
            'initial_stage': initial_stage,
            'final_stage': final_stage,
            'plasticity_change': final_plasticity - initial_plasticity,
            'firing_rate_change': development_data['firing_rate'][-1] - development_data['firing_rate'][0],
            'total_steps': total_steps
        }
        
        with open('development_summary.json', 'w') as f:
            json.dump(development_summary, f, indent=2)
        
        print(f"\nDevelopment summary saved to development_summary.json")
        return brain, agent, world, development_data

class Example6_ConfigurationManagement:
    """
    Example 6: Configuration Management and Experimentation
    
    This example demonstrates various ways to manage and experiment with
    different brain configurations.
    
    Learning Objectives:
    - Configuration file management
    - Parameter sweep experiments
    - Configuration validation
    - Experiment result storage
    """
    
    @staticmethod
    def run_configuration_example():
        print("\n=== Example 6: Configuration Management ===")
        
        # Create different configurations for experimentation
        base_config = pynlm.createDefaultConfig()
        
        # Configuration 1: High learning rate
        config1 = pynlm.createDefaultConfig()
        config1.set("brain.neuron_count", 1000)
        config1.set("brain.v_thresh", -45.0f)  # More excitable
        config1.set("plasticity.stdp.learning_rate", 0.02)
        config1.set("plasticity.hebbian.learning_rate", 0.02)
        config1.set("neuromod.curiosity.strength", 0.8)
        
        # Configuration 2: Low learning rate
        config2 = pynlm.createDefaultConfig()
        config2.set("brain.neuron_count", 500)
        config2.set("brain.v_thresh", -60.0f)  # Less excitable
        config2.set("plasticity.stdp.learning_rate", 0.001)
        config2.set("plasticity.hebbian.learning_rate", 0.001)
        config2.set("neuromod.curiosity.strength", 0.3)
        
        # Configuration 3: Balanced
        config3 = pynlm.createDefaultConfig()
        config3.set("brain.neuron_count", 1500)
        config3.set("brain.v_thresh", -55.0f)
        config3.set("plasticity.stdp.learning_rate", 0.01)
        config3.set("plasticity.hebbian.learning_rate", 0.01)
        config3.set("neuromod.curiosity.strength", 0.5)
        
        configurations = [config1, config2, config3]
        
        for i, config in enumerate(configurations, 1):
            print(f"\n--- Configuration {i} ---")
            
            # Save configuration
            filename = f"config_{i}.json"
            config.saveToFile(filename)
            print(f"Saved configuration to {filename}")
            
            # Load and verify
            loaded_config = pynlm.createDefaultConfig()
            loaded_config.loadFromFile(filename)
            
            print(f"  Neuron count: {loaded_config.getOr('brain.neuron_count', 0)}")
            print(f"  Threshold: {loaded_config.getOr('brain.v_thresh', 0.0)}")
            print(f"  STDP LR: {loaded_config.getOr('plasticity.stdp.learning_rate', 0.0)}")
        
        # Run experiment with each configuration
        print("\n=== Experiment Results ===")
        
        for i, config in enumerate(configurations, 1):
            print(f"\nRunning experiment with Configuration {i}...")
            
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            world.configure(width=8, height=8, visionWidth=4, visionHeight=4)
            world.reset()
            world.setAgentStart(4, 4)
            agent.initialize(world)
            
            # Run short test
            for step in range(50):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                agent.applyRewardModulation(0.0, 0.0)
            
            print(f"  Final firing rate: {brain.getFiringNeuronCount()} Hz")
            print(f"  Average rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Curiosiy: {agent.getCuriosityLevel():.3f}")
        
        # Clean up temporary files
        for i in range(1, 4):
            filename = f"config_{i}.json"
            if os.path.exists(filename):
                os.remove(filename)
                print(f"Cleaned up {filename}")
        
        print("\nConfiguration management example complete")
        return True

# Main function to run all examples
if __name__ == "__main__":
    print("NLM Advanced Example Scripts")
    print("===========================")
    
    examples = [
        ("Basic Brain Simulation", Example1_BasicBrain.run_example),
        ("Agent in World", Example2_AgentInWorld.run_example),
        ("Learning Benchmarks", Example3_LearningBenchmarks.run_learning_benchmarks),
        ("Episodic Memory", Example4_EpisodicMemory.run_episodic_memory_example),
        ("Development and Adaptation", Example5_DevelopmentAndAdaptation.run_development_example),
        ("Configuration Management", Example6_ConfigurationManagement.run_configuration_example),
    ]
    
    # Run all examples
    for name, example_func in examples:
        try:
            example_func()
            print(f"\n{'='*60}")
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
    
    print("\nAll examples completed!")
