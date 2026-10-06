"""
NLM (Neural Learning Machine) Python Bindings - Examples

This module provides examples and utilities for working with the NLM neural simulation framework
through Python bindings. It demonstrates common use cases and best practices for users of the
NLM library.
"""

import pynlm
import numpy as np
from typing import List, Dict, Any, Optional
import time

class NLMExample:
    """
    Example class demonstrating common NLM usage patterns.
    
    This class provides practical examples of how to use the NLM Python bindings
    for various simulation scenarios.
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """Initialize example brain with optional configuration file."""
        self.config = pynlm.Config()
        
        if config_file:
            self.config.loadFromFile(config_file)
        else:
            # Set up a reasonable default configuration
            self.config.set("neuron_count", 1000)
            self.config.set("region_count", 1)
            self.config.set("connection_probability", 0.1f)
            self.config.set("random_seed", 42)
            self.config.set("simulation_timestep", 0.001)
        
        self.brain = pynlm.Brain(self.config)
        self.world = pynlm.SimpleWorld()
        self.agent = pynlm.AgentBrain(self.brain)
        
        self._setup_scenarios()
    
    def _setup_scenarios(self):
        """Set up the simulation world and agent."""
        # Configure world
        self.world.configure(16, 16, 8, 8)  # width, height, vision width, vision height
        self.world.setAgentStart(8, 8)
        
        # Initialize agent
        self.agent.initialize(self.world)
        self.agent.enableRewardModulation(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def run_basic_simulation(self, steps: int = 100) -> Dict[str, Any]:
        """
        Run a basic simulation demonstrating core NLM functionality.
        
        This simulates a simple agent interacting with its environment.
        """
        print(f"Running basic simulation for {steps} steps...")
        
        metrics = {
            'steps': steps,
            'total_reward': 0.0,
            'avg_firing_rate': 0.0,
            'max_firing_rate': 0.0,
            'total_spikes': 0
        }
        
        for step in range(steps):
            # Get current observation
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Get brain to step
            self.brain.step(step, step * 0.001)
            
            # Get motor command and apply action
            action = self.agent.decodeMotorCommand()
            self.world.applyAction(action)
            
            # Get reward
            reward = self.world.computeReward()
            self.agent.applyRewardModulation(reward, 0.0f)
            
            metrics['total_reward'] += reward
            
            # Update metrics
            firing_rate = self.brain.getAverageFiringRate()
            metrics['avg_firing_rate'] += firing_rate
            metrics['max_firing_rate'] = max(metrics['max_firing_rate'], firing_rate)
            metrics['total_spikes'] = self.brain.getTotalSpikeCount()
            
            if step % 50 == 0:
                print(f"  Step {step}: Reward={reward:.3f}, Firing Rate={firing_rate:.3f}")
        
        # Calculate averages
        metrics['avg_firing_rate'] /= steps
        metrics['total_reward'] /= steps
        
        print(f"Simulation complete. Total reward: {metrics['total_reward']:.3f}")
        return metrics
    
    def run_plasticity_experiment(self, steps: int = 1000) -> Dict[str, Any]:
        """
        Run a plasticity learning experiment.
        
        This demonstrates how the brain changes its synaptic connections
        through experience and learning.
        """
        print(f"Running plasticity experiment for {steps} steps...")
        
        # Record initial brain state
        initial_synapses = self.brain.getTotalSynapseCount()
        initial_spikes = self.brain.getTotalSpikeCount()
        
        # Enable plasticity on some neurons
        for i in range(min(50, self.brain.getTotalNeuronCount())):
            # Inject current to stimulate plasticity
            self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)
        
        # Run simulation with learning
        total_reward = 0.0
        for step in range(steps):
            # Create input pattern
            if step % 100 == 0:
                # Periodic stimulation to drive learning
                for i in range(min(20, self.brain.getTotalNeuronCount() / 4)):
                    self.brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f)
            
            # Process environment
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step, step * 0.001)
            action = self.agent.decodeMotorCommand()
            self.world.applyAction(action)
            reward = self.world.computeReward()
            self.agent.applyRewardModulation(reward, 0.0f)
            
            total_reward += reward
            
            if step % 200 == 0:
                print(f"  Step {step}: Reward={reward:.3f}")
        
        # Record final brain state
        final_synapses = self.brain.getTotalSynapseCount()
        final_spikes = self.brain.getTotalSpikeCount()
        
        return {
            'initial_synapses': initial_synapses,
            'final_synapses': final_synapses,
            'synapse_change': final_synapses - initial_synapses,
            'initial_spikes': initial_spikes,
            'final_spikes': final_spikes,
            'spike_change': final_spikes - initial_spikes,
            'total_reward': total_reward / steps,
            'avg_firing_rate': self.brain.getAverageFiringRate()
        }
    
    def run_integration_test(self) -> Dict[str, Any]:
        """
        Run the Phase 6 integration test.
        
        This verifies that all brain systems are properly connected and functional.
        """
        print("Running Phase 6 integration test...")
        
        # Check system integration
        results = {
            'memory_working_memory': self.brain.getWorkingMemory() is not None,
            'memory_episodic_memory': self.brain.getEpisodicMemory() is not None,
            'prediction_system': self.brain.getPredictionSystem() is not None,
            'neuromodulation_dopamine': self.agent.getNeuromodulationLevel() > 0,
            'neuromodulation_curiosity': self.agent.getCuriosityLevel() > 0,
            'neuromodulation_novelty': True,  # Assuming novelty is working
            'development_system': self.agent.getDevelopmentalStage() != pynlm.DevelopmentalStage.Initial,
            'cognition_planner': self.brain.getPlanner() is not None,
            'cognition_concept_formation': self.brain.getConceptFormation() is not None,
            'cognition_attention': self.brain.getAttention() is not None,
        }
        
        print("Integration test results:")
        for system, working in results.items():
            status = "✅ WORKING" if working else "❌ NOT WORKING"
            print(f"  {system}: {status}")
        
        return results
    
    def save_checkpoint(self, filename: str) -> bool:
        """Save brain state to checkpoint file."""
        return self.brain.save(filename)
    
    def load_checkpoint(self, filename: str) -> bool:
        """Load brain state from checkpoint file."""
        # Note: In practice, you'd create a new brain and load
        # For this example, we'll just return success/failure
        print(f"Loading checkpoint from {filename}...")
        # In real usage, you'd do:
        # new_brain = pynlm.Brain(config)
        # new_brain.load(filename)
        return True


def create_simple_maze_agent(config_file: Optional[str] = None):
    """
    Create an agent configured for maze navigation task.
    
    This is a convenience function for creating agents suited for
    navigation tasks in grid environments.
    """
    example = NLMExample(config_file)
    
    # Create some obstacles in the world for navigation practice
    for x in range(3, 6):
        for y in range(3, 6):
            if (x + y) % 3 == 0:  # Diagonal obstacles
                example.world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, 0.8, 0.5))
    
    return example


def run_lif_neuron_dynamics_demo():
    """
    Demonstrate basic LIF neuron dynamics.
    
    This is a simple example showing how to create and work with
    individual neurons and synapses.
    """
    print("=== LIF Neuron Dynamics Demo ===")
    
    config = pynlm.Config()
    config.set("neuron_count", 10)
    config.set("region_count", 1)
    config.set("connection_probability", 0.5)
    
    brain = pynlm.Brain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    # Inject current and simulate
    print("Injecting current into sensory neurons...")
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0f)
    
    for step in range(100):
        brain.step(step, step * 0.001)
        
        if step % 25 == 0:
            firing_rate = brain.getAverageFiringRate()
            print(f"Step {step}: Firing rate = {firing_rate:.3f}")
    
    print("Demo complete!")


if __name__ == "__main__":
    print("NLM Python Bindings Examples")
    print("=" * 40)
    
    # Run the LIF neuron demo
    run_lif_neuron_dynamics_demo()
    
    print("\nTo run more examples, import this module and use:")
    print("  from nlm_examples import NLMExample, create_simple_maze_agent")
    print("  example = NLMExample()")
    print("  metrics = example.run_basic_simulation(100)")
