# NLM Advanced Python Examples
# Comprehensive examples for advanced NLM users

import sys
import os
import json
import time
import threading
import numpy as np
from typing import Dict, List, Any, Optional

# Add the current directory to path to import nlm
 sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    from nlm_advanced_utils import PerformanceProfiler, MemoryManager, BatchProcessor, SimulationController
except ImportError as e:
    print(f"Warning: Could not import NLM modules: {e}")
    print("Some examples may not work without NLM installed.")

class AdvancedNLMExamples:
    """
    Advanced examples for NLM users demonstrating:
    - Performance profiling and optimization
    - Memory management and checkpointing
    - Batch processing for large simulations
    - Simulation control and monitoring
    - Configuration management
    - Advanced agent behaviors
    """
    
    def __init__(self):
        self.profiler = PerformanceProfiler(enabled=True)
        self.memory_manager = MemoryManager()
        self.batch_processor = BatchProcessor(batch_size=500)
        self.controller = SimulationController()
        
    def example_1_basic_profiling(self):
        """
        Example 1: Basic performance profiling and analysis
        Demonstrates how to profile simulation performance and analyze bottlenecks
        """
        print("=" * 60)
        print("Example 1: Basic Performance Profiling")
        print("=" * 60)
        
        # Create simulation components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Start profiling
        self.profiler.start_timer("total_simulation")
        
        # Run simulation with profiling
        for step in range(200):
            self.profiler.start_timer(f"step_{step}")
            brain.step(step)
            elapsed = self.profiler.stop_timer(f"step_{step}")
            
            profile_data = self.profiler.profile_step(
                step, elapsed, brain.getTotalNeuronCount(), brain.getTotalSpikeCount()
            )
            
            if step % 50 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"{brain.getTotalSpikeCount()} total spikes, "
                      f"{elapsed:.4f}s elapsed")
        
        total_time = self.profiler.stop_timer("total_simulation")
        
        # Generate and display profile summary
        summary = self.profiler.get_summary()
        
        print("\nPerformance Summary:")
        print(f"  Total simulation time: {total_time:.2f}s")
        print(f"  Total steps: {len(summary.get('samples', []))}")
        print(f"  Average time per step: {summary.get('avg_time_per_step', 0):.4f}s")
        print(f"  Steps per second: {summary.get('steps_per_second', 0):.1f}")
        print(f"  Peak memory usage: {summary.get('peak_memory', {}).get('rss', 0)/1024/1024:.1f} MB")
        print(f"  Average spike rate: {summary.get('avg_spike_rate', 0):.2f} spikes/neuron")
        
        # Export profile data
        self.profiler.export_profile("simulation_profile.json")
        print(f"\nProfile data exported to simulation_profile.json")
        
        return summary
        
    def example_2_checkpointing_and_recovery(self):
        """
        Example 2: Checkpointing and simulation recovery
        Demonstrates memory management and checkpoint/restore functionality
        """
        print("\n" + "=" * 60)
        print("Example 2: Checkpointing and Simulation Recovery")
        print("=" * 60)
        
        # Create simulation components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create simple world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        print(f"Initial state - Neurons: {brain.getTotalNeuronCount()}, "
              f"Synapses: {brain.getTotalSynapseCount()}")
        
        # Run initial simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"After initial simulation - Spikes: {brain.getTotalSpikeCount()}, "
              f"Development Stage: {brain.getDevelopmentalStage()}")
        
        # Create checkpoint
        checkpoint_id = self.memory_manager.create_checkpoint("mid_simulation", brain, world)
        print(f"Checkpoint created: {checkpoint_id}")
        
        # Continue simulation
        for step in range(100, 200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 50 == 0:
                print(f"Step {step} - Spikes: {brain.getTotalSpikeCount()}")
        
        print(f"Final state - Spikes: {brain.getTotalSpikeCount()}")
        
        # Create another checkpoint
        checkpoint_id2 = self.memory_manager.create_checkpoint("late_simulation", brain, world)
        print(f"Second checkpoint created: {checkpoint_id2}")
        
        # Simulate recovery from first checkpoint
        print("\nSimulating recovery from first checkpoint...")
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        # In a real implementation, we would restore from checkpoint
        # For this example, we'll just show the memory usage
        current_memory = self.memory_manager._get_memory_usage()
        print(f"Current memory usage: {current_memory['percent']:.1f}%")
        print(f"Peak memory usage: {self.memory_manager.checkpoints[0]['memory_usage']['percent']:.1f}%")
        
        # Cleanup old checkpoints
        self.memory_manager.cleanup_old_checkpoints(max_checkpoints=5)
        print(f"Cleanup completed - {len(self.memory_manager.checkpoints)} checkpoints remaining")
        
    def example_3_batch_processing(self):
        """
        Example 3: Batch processing for large-scale simulations
        Demonstrates efficient processing of large numbers of simulation steps
        """
        print("\n" + "=" * 60)
        print("Example 3: Batch Processing for Large-Scale Simulations")
        print("=" * 60)
        
        # Create simulation components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print(f"Starting batch processing with {brain.getTotalNeuronCount()} neurons")
        print(f"Batch size: {self.batch_processor.batch_size} steps")
        
        # Process in batches
        results = self.batch_processor.process_batch(
            brain, world, agent, num_steps=1000,
            callback=lambda: None  # Process callback
        )
        
        # Analyze results
        if results:
            times = [r['elapsed_time'] for r in results]
            spike_rates = [r['spikes_per_neuron'] for r in results]
            
            print(f"\nBatch Processing Results:")
            print(f"  Total steps processed: {len(results)}")
            print(f"  Average time per step: {np.mean(times):.4f}s")
            print(f"  Min time per step: {np.min(times):.4f}s")
            print(f"  Max time per step: {np.max(times):.4f}s")
            print(f"  Time std deviation: {np.std(times):.4f}s")
            print(f"  Average spike rate: {np.mean(spike_rates):.2f} spikes/neuron")
            
            # Find bottleneck steps (slow steps)
            slow_steps = [(i, r['elapsed_time']) for i, r in enumerate(results) 
                         if r['elapsed_time'] > np.mean(times) + np.std(times)]
            
            if slow_steps:
                print(f"  Slow steps identified (>{np.mean(times) + np.std(times):.4f}s):")
                for step_num, elapsed in slow_steps[:5]:
                    print(f"    Step {step_num}: {elapsed:.4f}s")
                    
            # Performance statistics
            performance_stats = {
                'total_steps': len(results),
                'avg_time_per_step': np.mean(times),
                'performance_score': 1.0 / (np.mean(times) + 0.001),
                'memory_efficiency': sum(r['neuron_count'] for r in results) / len(results) if results else 0
            }
            
            print(f"\nPerformance Score: {performance_stats['performance_score']:.2f}")
            print(f"Memory Efficiency: {performance_stats['memory_efficiency']:.0f} neurons/batch")
            
        # Get batch statistics
        batch_results = self.batch_processor.get_batch_results()
        print(f"\nTotal batch results stored: {len(batch_results)}")
        
    def example_4_advanced_simulation_control(self):
        """
        Example 4: Advanced simulation control and monitoring
        Demonstrates real-time simulation control with monitoring
        """
        print("\n" + "=" * 60)
        print("Example 4: Advanced Simulation Control and Monitoring")
        print("=" * 60)
        
        # Create simulation components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print(f"Advanced simulation started with ID: {self.controller.simulation_id}")
        print(f"Initial brain configuration: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSynapseCount()} synapses")
        
        # Start monitoring in a separate thread
        def monitor_simulation():
            """Background monitoring thread"""
            step = 0
            while not self.controller.stop_requested:
                if step % 100 == 0:
                    print(f"[MONITOR] Step {step} - "
                          f"Firing: {brain.getFiringNeuronCount()}, "
                          f"Spikes: {brain.getTotalSpikeCount()}, "
                          f"Curiosity: {agent.getCuriosityLevel():.2f}")
                time.sleep(0.5)
                step += 1
        
        monitor_thread = threading.Thread(target=monitor_simulation, daemon=True)
        monitor_thread.start()
        
        # Configure simulation
        self.controller.configure({
            'max_steps': 500,
            'step_interval': 0.1,
            'enable_optimization': True,
            'save_checkpoints': True,
            'monitoring_interval': 1.0
        })
        
        # Run simulation with control
        try:
            summary = self.controller.run_simulation(
                brain, world, agent, num_steps=500,
                step_callback=lambda: None,
                error_callback=lambda error, step: print(f"Error at step {step}: {error}")
            )
            
            print(f"\nAdvanced Simulation Summary:")
            print(f"  Simulation ID: {summary['simulation_id']}")
            print(f"  Total steps completed: {summary['total_steps']}")
            print(f"  Total simulation time: {summary['total_time']:.2f}s")
            print(f"  Steps per second: {summary['steps_per_second']:.1f}")
            print(f"  Brain final statistics:")
            brain_stats = summary['brain_final_stats']
            print(f"    Neurons: {brain_stats['total_neurons']}")
            print(f"    Synapses: {brain_stats['total_synapses']}")
            print(f"    Total spikes: {brain_stats['total_spikes']}")
            print(f"    Development stage: {brain_stats['development_stage']}")
            
        except KeyboardInterrupt:
            print("\nSimulation interrupted by user")
            self.controller.stop()
            
        print(f"\nSimulation control demonstration completed")
        
    def example_5_advanced_configuration_management(self):
        """
        Example 5: Advanced configuration management
        Demonstrates complex configuration scenarios and validation
        """
        print("\n" + "=" * 60)
        print("Example 5: Advanced Configuration Management")
        print("=" * 60)
        
        # Create configuration manager
        config = pynlm.createDefaultConfig()
        
        # Set initial configuration
        config.set("brain.neuron_count", 2000, pynlm.ConfigSource.Runtime)
        config.set("brain.synapse_density", 0.15, pynlm.ConfigSource.Runtime)
        config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.Default)
        config.set("neuromod.dopamine.scale", 1.2, pynlm.ConfigSource.File)
        config.set("simulation.timestep", 0.001, pynlm.ConfigSource.CommandLine)
        
        # Display configuration
        print("Current Configuration:")
        config_str = config.summary()
        lines = config_str.split('\n')
        for line in lines[:20]:  # Show first 20 lines
            print(f"  {line}")
        if len(lines) > 20:
            print(f"  ... and {len(lines) - 20} more lines")
        
        # Demonstrate configuration operations
        print(f"\nConfiguration Operations:")
        print(f"  Has 'brain.neuron_count': {config.has('brain.neuron_count')}")
        print(f"  Has 'nonexistent.key': {config.has('nonexistent.key')}")
        
        # Get values with defaults
        neuron_count = config.getOr("brain.neuron_count", 1000)
        learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.01)
        max_energy = config.getOr("world.max_energy", 100.0)
        
        print(f"  Neuron count: {neuron_count}")
        print(f"  Learning rate: {learning_rate}")
        print(f"  Max energy: {max_energy}")
        
        # Configuration validation
        print(f"\nConfiguration Validation:")
        is_valid = config.validate()
        print(f"  Configuration valid: {is_valid}")
        print(f"  Validation errors: {config.getValidationErrors()}")
        
        # Configuration copy and merge
        config2 = pynlm.createDefaultConfig()
        config2.set("brain.neuron_count", 3000)
        config2.set("brain.synapse_density", 0.2)
        
        print(f"\nConfiguration operations:")
        print(f"  Original neuron count: {neuron_count}")
        print(f"  Config2 neuron count: {config2.getOr('brain.neuron_count', 0)}")
        
        # Merge configurations
        for key in config2.getKeys():
            val = config2.getOr(key, None)
            if val is not None:
                config.set(key, val, pynlm.ConfigSource.Runtime)
        
        print(f"  After merge - Neuron count: {config.getOr('brain.neuron_count', 0)}")
        
        # Configuration comparison
        diff_result = config.getOr("brain.synapse_density", 0.0) - config2.getOr("brain.synapse_density", 0.0)
        print(f"  Synapse density difference: {diff_result:.2f}")
        
        # Configuration backup and restore
        print(f"\nConfiguration backup demonstration:")
        backup_data = config.summary()
        print(f"  Configuration backup size: {len(backup_data)} characters")
        
        # Save to file (if supported)
        try:
            config.saveAsJSON("simulation_config.json")
            print(f"  Configuration saved to simulation_config.json")
        except Exception as e:
            print(f"  Configuration save failed: {e}")
            
    def example_6_advanced_agent_behavior(self):
        """
        Example 6: Advanced agent behaviors and learning
        Demonstrates complex agent behaviors and learning strategies
        """
        print("\n" + "=" * 60)
        print("Example 6: Advanced Agent Behaviors and Learning")
        print("=" * 60)
        
        # Create components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("Advanced Agent Configuration:")
        print(f"  Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
        print(f"  Structural plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
        print(f"  Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
        print(f"  Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
        
        # Agent learning strategies
        print(f"\nAgent Learning Metrics:")
        
        for step in range(200):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward (simplified)
            reward = 0.0
            if hasattr(world, 'getMaxEnergy'):
                current_energy = world.getAgentBody().energy if hasattr(world.getAgentBody(), 'energy') else 0
                max_energy = world.getMaxEnergy()
                reward = current_energy / max_energy if max_energy > 0 else 0
            
            # Apply learning
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Report progress
            if step % 50 == 0:
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                prediction_error = agent.getPredictionError()
                
                print(f"  Step {step}: "
                      f"Curiosity: {curiosity:.2f}, "
                      f"Novelty: {novelty:.2f}, "
                      f"Prediction Error: {prediction_error:.2f}, "
                      f"Firing Neurons: {brain.getFiringNeuronCount()}")
        
        print(f"\nAdvanced Learning Summary:")
        print(f"  Final curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"  Final novelty level: {agent.getNoveltyLevel():.2f}")
        print(f"  Final prediction error: {agent.getPredictionError():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        
        # Agent statistics
        print(f"\nAgent Statistics:")
        print(f"  Total simulation steps: {self.controller.step_count}")
        print(f"  Average curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"  Average novelty level: {agent.getNoveltyLevel():.2f}")
        
        # Demonstrate agent behavior patterns
        print(f"\nAgent Behavior Patterns:")
        
        # Create multiple agents for comparison
        agents = []
        for i in range(3):
            config_i = pynlm.createDefaultConfig()
            config_i.set("brain.neuron_count", 800)
            brain_i = pynlm.createBrain(config_i)
            brain_i.initialize()
            
            agent_i = pynlm.createAgentBrain(brain_i)
            agent_i.initialize(world)
            agent_i.enableCuriosity(True)
            agents.append(agent_i)
            
            # Run simulation with different curiosity levels
            for step in range(100):
                world.update(0.1)
                agent_i.processSensoryInput(world.getSensoryPercept())
                brain_i.step(step)
        
        # Compare agent behaviors
        print(f"  Agent comparison (after 100 steps):")
        for i, agent_i in enumerate(agents):
            curiosity = agent_i.getCuriosityLevel()
            novelty = agent_i.getNoveltyLevel()
            print(f"    Agent {i+1}: Curiosity: {curiosity:.2f}, Novelty: {novelty:.2f}")

# Main function to run all examples
def main():
    """Run all advanced NLM examples"""
    print("NLM Advanced Python Examples")
    print("=" * 60)
    print("This script demonstrates advanced NLM functionality including:")
    print("1. Performance profiling and analysis")
    print("2. Checkpointing and simulation recovery")
    print("3. Batch processing for large simulations")
    print("4. Advanced simulation control and monitoring")
    print("5. Advanced configuration management")
    print("6. Advanced agent behaviors and learning")
    print("=" * 60)
    
    examples = AdvancedNLMExamples()
    
    try:
        # Run examples (uncomment to run specific examples)
        print("\nRunning Example 1: Basic Performance Profiling")
        examples.example_1_basic_profiling()
        
        print("\nRunning Example 2: Checkpointing and Simulation Recovery")
        examples.example_2_checkpointing_and_recovery()
        
        print("\nRunning Example 3: Batch Processing for Large-Scale Simulations")
        examples.example_3_batch_processing()
        
        print("\nRunning Example 4: Advanced Simulation Control and Monitoring")
        examples.example_4_advanced_simulation_control()
        
        print("\nRunning Example 5: Advanced Configuration Management")
        examples.example_5_advanced_configuration_management()
        
        print("\nRunning Example 6: Advanced Agent Behaviors and Learning")
        examples.example_6_advanced_agent_behavior()
        
        print("\n" + "=" * 60)
        print("All examples completed successfully!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
        
    return 0
if __name__ == "__main__":
    sys.exit(main())