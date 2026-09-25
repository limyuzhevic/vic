#!/usr/bin/env python3
"""
Advanced NLM Examples - Demonstrating Advanced Features and Techniques

This script demonstrates advanced NLM features including:
1. Multi-agent systems
2. Complex learning scenarios
3. Performance optimization
4. State management and persistence
5. Advanced configuration
6. Error handling and robustness

For best results, run with Python 3.8+.
"""

import pynlm
import time
import os

class AdvancedNMESystem:
    """
    Advanced NLM system demonstrating multiple agents and complex learning.
    """
    
    def __init__(self, num_agents=3):
        self.agents = []
        self.world = None
        self.num_agents = num_agents
        self.step_count = 0
        
    def create_agent_system(self, width=20, height=20):
        """Create a complete multi-agent system."""
        print(f"Creating advanced NLM system with {self.num_agents} agents...")
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=width, height=height, visionWidth=10, visionHeight=10)
        self.world.reset()
        
        # Create agents with different configurations
        for i in range(self.num_agents):
            # Different configurations for each agent
            if i == 0:
                config = pynlm.createLearningConfig()
            elif i == 1:
                config = pynlm.createExplorationConfig()
            else:
                config = pynlm.createConfigWithNeuronCount(1500)
            
            brain = pynlm.createBrain(config)
            agent = pynlm.createAgentBrain(brain)
            
            # Initialize with world
            brain.initialize()
            agent.initialize(self.world)
            
            # Configure based on agent type
            if i == 0:
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
            elif i == 1:
                agent.enableRewardModulation(False)  # No reward modulation
                agent.enableCuriosity(True)  # High curiosity
                agent.enableDevelopment(False)  # No development
            else:
                agent.enableRewardModulation(True)
                agent.enableCuriosity(False)
                agent.enableDevelopment(True)
            
            self.agents.append({
                'brain': brain,
                'agent': agent,
                'id': i,
                'position': (i * 5.0, i * 5.0)
            })
            
            self.world.setAgentStart(i * 5.0, i * 5.0)
        
        print(f"Created {len(self.agents)} agents")
        
    def run_advanced_simulation(self, num_steps):
        """Run advanced simulation with multiple agents."""
        print(f"Running advanced simulation for {num_steps} steps...")
        
        for step in range(num_steps):
            self.step_count = step
            
            # Update world
            self.world.update(0.1)
            
            # Each agent processes sensory input
            for agent_data in self.agents:
                agent = agent_data['agent']
                brain = agent_data['brain']
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process input
                agent.processSensoryInput(percept)
                
                # Brain processes
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation
                reward = self.get_reward_for_agent(agent_data)
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                self.print_step_summary(step)
        
        print("Advanced simulation complete!")
        
    def get_reward_for_agent(self, agent_data):
        """Calculate reward based on agent state."""
        agent = agent_data['agent']
        brain = agent_data['agent']
        
        # Simple reward function based on activity
        firing_rate = brain.getAverageFiringRate()
        curiosity = agent.getCuriosityLevel()
        
        # Reward based on firing rate and curiosity
        reward = firing_rate * 0.1 + curiosity * 0.5
        return max(0.0, min(1.0, reward))  # Clamp to [0, 1]
        
    def print_step_summary(self, step):
        """Print simulation step summary."""
        print(f"\n--- Step {step} Summary ---")
        
        for i, agent_data in enumerate(self.agents):
            agent = agent_data['agent']
            brain = agent_data['brain']
            
            print(f"Agent {i}:")
            print(f"  Neuropeptides: {brain.getTotalSpikeCount()} spikes")
            print(f"  Activity: {brain.getFiringNeuronCount()} firing neurons")
            print(f"  Curios: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
            
    def save_system_state(self, base_filename):
        """Save all agents and world state."""
        print(f"Saving advanced system state...")
        
        # Save each agent
        for i, agent_data in enumerate(self.agents):
            brain = agent_data['brain']
            filename = f"{base_filename}_agent_{i}.bin"
            pynlm.saveBrainWithMetadata(brain, filename, 
                                       f"Advanced agent {i} at step {self.step_count}")
            print(f"Saved agent {i} to {filename}")
        
        # Save world state (if supported)
        print("System state saved successfully!")

class PerformanceBenchmark:
    """
    Benchmark NLM performance with different configurations.
    """
    
    @staticmethod
    def benchmark_configuration(config_name, config, num_steps=500):
        """Benchmark a specific brain configuration."""
        print(f"\nBenchmarking {config_name} configuration...")
        
        start_time = time.time()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable all plasticity for learning config
        if config_name == 'learning':
            pynlm.configureBrainForLearning(brain)
        
        # Run benchmark
        for step in range(num_steps):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
            brain.step(step)
        
        end_time = time.time()
        
        elapsed = end_time - start_time
        steps_per_second = num_steps / elapsed
        
        print(f"Benchmark results for {config_name}:")
        print(f"  Time: {elapsed:.2f} seconds")
        print(f"  Steps per second: {steps_per_second:.1f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        # Get comprehensive stats
        stats = pynlm.getBrainStats(brain)
        print(f"  Brain stats: {stats[:200]}...")  # Truncate long output
        
        return {
            'name': config_name,
            'time': elapsed,
            'steps_per_second': steps_per_second,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate()
        }
        
    @staticmethod
    def run_full_benchmark():
        """Run comprehensive benchmark of different configurations."""
        print("Running comprehensive NLM performance benchmark...")
        
        configurations = {
            'small_learning': pynlm.createConfigWithNeuronCount(500),
            'medium_learning': pynlm.createLearningConfig(),
            'large_exploration': pynlm.createExplorationConfig(),
            'very_large': pynlm.createConfigWithNeuronCount(5000),
        }
        
        results = []
        
        for name, config in configurations.items():
            result = PerformanceBenchmark.benchmark_configuration(name, config, 300)
            results.append(result)
        
        print(f"\nBenchmark Summary:")
        print(f"{'Configuration':<20} {'Time (s)':<10} {'Steps/s':<10} {'Spikes':<10} {'Firing Rate (Hz)':<15}")
        print(f"{'-' * 70}")
        
        for result in results:
            print(f"{result['name']:<20} {result['time']:<10.2f} {result['steps_per_second']:<10.1f} " +
                  f"{result['total_spikes']:<10} {result['firing_rate']:<15.2f}")
        
        # Find best performing configuration
        best = max(results, key=lambda x: x['steps_per_second'])
        print(f"\nBest performing configuration: {best['name']} ({best['steps_per_second']:.1f} steps/s)")
        
        return results

class ExperimentalFramework:
    """
    Framework for running controlled experiments with NLM.
    """
    
    def __init__(self):
        self.experiments = []
        self.results = {}
        
    def add_experiment(self, name, description, config):
        """Add an experiment to the framework."""
        self.experiments.append({
            'name': name,
            'description': description,
            'config': config,
            'results': {}
        })
        print(f"Added experiment: {name}")
        
    def run_experiment(self, experiment_name, num_steps=200):
        """Run a specific experiment."""
        print(f"\nRunning experiment: {experiment_name}")
        print(f"Description: {self.get_experiment(experiment_name)['description']}")
        
        experiment = self.get_experiment(experiment_name)
        
        # Create brain
        brain = pynlm.createBrain(experiment['config'])
        brain.initialize()
        
        # Configure based on experiment type
        if 'learning' in experiment_name.lower():
            pynlm.configureBrainForLearning(brain)
        elif 'exploration' in experiment_name.lower():
            pynlm.configureBrainForStability(brain)
        
        # Run experiment
        start_time = time.time()
        
        for step in range(num_steps):
            # Experiment-specific behavior
            if 'reward' in experiment_name.lower():
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0f)
            elif 'novelty' in experiment_name.lower():
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
            else:
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
            
            brain.step(step)
            
            if step % 50 == 0:
                print(f"  Step {step}/{num_steps}: {brain.getTotalSpikeCount()} spikes")
        
        end_time = time.time()
        
        # Collect results
        experiment['results'] = {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage(),
            'time_elapsed': end_time - start_time
        }
        
        self.results[experiment_name] = experiment['results']
        
        print(f"Experiment complete!")
        print(f"Results: {experiment['results']}")
        
        return experiment['results']
        
    def get_experiment(self, name):
        """Get experiment by name."""
        for experiment in self.experiments:
            if experiment['name'] == name:
                return experiment
        return None
        
    def print_results_summary(self):
        """Print summary of all experiment results."""
        if not self.results:
            print("No results to display.")
            return
        
        print(f"\nExperiment Results Summary:")
        print(f"{'Experiment':<20} {'Spikes':<10} {'Firing Rate':<12} {'E/I Ratio':<10} {'Time (s)':<10}")
        print(f"{'-' * 70}")
        
        for name, result in self.results.items():
            print(f"{name:<20} {result['total_spikes']:<10} {result['firing_rate']:<12.2f} " +
                  f"{result['excitation_inhibition_ratio']:<10.3f} {result['time_elapsed']:<10.2f}")

# Example usage functions
def demonstrate_advanced_system():
    """Demonstrate the advanced multi-agent system."""
    print("=== Advanced Multi-Agent System Demo ===")
    
    system = AdvancedNMESystem(num_agents=3)
    system.create_agent_system(width=15, height=15)
    system.run_advanced_simulation(300)
    system.save_system_state("advanced_system")
    
    print("\nAdvanced system demo complete!")
    

def demonstrate_performance_benchmark():
    """Demonstrate performance benchmarking."""
    print("=== NLM Performance Benchmark Demo ===")
    
    results = PerformanceBenchmark.run_full_benchmark()
    
    print("\nBenchmark demo complete!")
    

def demonstrate_experimental_framework():
    """Demonstrate the experimental framework."""
    print("=== Experimental Framework Demo ===")
    
    framework = ExperimentalFramework()
    
    # Add experiments
    framework.add_experiment(
        "reward_learning",
        "Experiment testing reward-based learning",
        pynlm.createLearningConfig()
    )
    
    framework.add_experiment(
        "curiosity_exploration", 
        "Experiment testing curiosity-driven exploration",
        pynlm.createExplorationConfig()
    )
    
    framework.add_experiment(
        "baseline",
        "Baseline experiment with default configuration",
        pynlm.createDefaultConfig()
    )
    
    # Run experiments
    framework.run_experiment("reward_learning", 200)
    framework.run_experiment("curiosity_exploration", 200)
    framework.run_experiment("baseline", 200)
    
    # Print summary
    framework.print_results_summary()
    
    print("\nExperimental framework demo complete!")

def main():
    """Run all advanced demonstrations."""
    print("NLM Advanced Features Demonstration")
    print("=" * 50)
    
    try:
        demonstrate_advanced_system()
        demonstrate_performance_benchmark()
        demonstrate_experimental_framework()
        
        print("\n" + "=" * 50)
        print("All advanced demonstrations completed successfully!")
        print("\nKey features demonstrated:")
        print("1. Multi-agent systems with different configurations")
        print("2. Performance benchmarking of different brain setups")
        print("3. Experimental framework for controlled testing")
        print("4. Advanced state management and persistence")
        print("5. Complex learning scenarios with neuromodulation")
        
    except Exception as e:
        print(f"\nError during demonstration: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()