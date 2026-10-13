#!/usr/bin/env python3
"""
NLM - High-Level Python API

This module provides a higher-level, more user-friendly API for NLM
that simplifies common operations and provides more intuitive interfaces.
"""

import pynlm
import time
import math
from typing import List, Dict, Any, Optional, Callable
from enum import Enum

# Re-export core classes for convenience
NeuronType = pynlm.NeuronType
DevelopmentalStage = pynlm.DevelopmentalStage
ActionType = pynlm.ActionType

class NLMState:
    """
    High-level state management for NLM simulations.
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.agent = None
        self.world = None
        self.step_count = 0
        self.start_time = time.time()
        self.metrics = {}
    
    def add_agent(self, agent: pynlm.AgentBrain, world: pynlm.SimpleWorld):
        """Add agent and world to the state."""
        self.agent = agent
        self.world = world
        agent.initialize(*world)
    
    def get_brain_stats(self) -> Dict[str, Any]:
        """Get comprehensive brain statistics."""
        return self.brain.getStats()
    
    def get_agent_stats(self) -> Dict[str, Any]:
        """Get agent statistics if agent is present."""
        if not self.agent:
            return {}
        
        return {
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'neuromodulation': self.agent.getNeuromodulationLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'development_stage': self.agent.getDevelopmentalStage(),
        }
    
    def get_simulation_time(self) -> float:
        """Get elapsed simulation time."""
        return time.time() - self.start_time
    
    def reset(self):
        """Reset the simulation state."""
        self.brain.reset()
        self.step_count = 0
        self.metrics.clear()
        self.start_time = time.time()

class NLMSimulation:
    """
    High-level simulation manager for NLM.
    """
    
    def __init__(self, config: Optional[pynlm.Config] = None):
        if config is None:
            config = pynlm.createDefaultConfig()
        
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.state = NLMState(self.brain)
        self.callbacks = {
            'on_step': [],
            'on_brain_change': [],
            'on_agent_change': [],
            'on_simulation_complete': [],
        }
        
        self.running = False
        self.step_delay = 0.0
    
    def add_step_callback(self, callback: Callable[[int, Dict[str, Any]], None]):
        """Add a callback function called every step."""
        self.callbacks['on_step'].append(callback)
    
    def add_brain_change_callback(self, callback: Callable[[pynlm.Brain], None]):
        """Add a callback function called when brain changes."""
        self.callbacks['on_brain_change'].append(callback)
    
    def add_agent_change_callback(self, callback: Callable[[pynlm.AgentBrain], None]):
        """Add a callback function called when agent changes."""
        self.callbacks['on_agent_change'].append(callback)
    
    def set_step_delay(self, delay: float):
        """Set delay between steps in seconds."""
        self.step_delay = max(0.0, delay)
    
    def run_episode(self, steps: int, auto_step: bool = True) -> Dict[str, Any]:
        """
        Run a simulation episode.
        
        Args:
            steps: Number of steps to simulate
            auto_step: If True, automatically call brain.step() each iteration
            
        Returns:
            Dictionary with episode results
        """
        self.running = True
        results = {
            'steps': steps,
            'start_time': time.time(),
            'metrics': [],
            'final_stats': None,
            'brain': self.brain,
            'agent': self.state.agent,
            'world': self.state.world,
        }
        
        try:
            for step in range(steps):
                if not self.running:
                    break
                
                # Collect metrics
                metrics = {
                    'step': step,
                    'brain_stats': self.state.get_brain_stats(),
                    'agent_stats': self.state.get_agent_stats(),
                    'simulation_time': self.state.get_simulation_time(),
                }
                
                # Call step callbacks
                for callback in self.callbacks['on_step']:
                    callback(step, metrics)
                
                # Store metrics
                results['metrics'].append(metrics)
                
                # Automatically step brain if enabled
                if auto_step:
                    self.brain.step(step)
                
                # Delay if requested
                if self.step_delay > 0:
                    time.sleep(self.step_delay)
            
        except Exception as e:
            results['error'] = str(e)
            raise
        
        finally:
            self.running = False
        
        # Calculate final statistics
        results['final_stats'] = {
            'total_steps': steps,
            'final_brain_stats': self.state.get_brain_stats(),
            'final_agent_stats': self.state.get_agent_stats(),
            'total_simulation_time': self.state.get_simulation_time(),
            'steps_per_second': steps / self.state.get_simulation_time() if self.state.get_simulation_time() > 0 else 0,
        }
        
        # Call completion callbacks
        for callback in self.callbacks['on_simulation_complete']:
            callback(results)
        
        return results
    
    def run_learning_episode(self, steps: int, enable_all_subsystems: bool = True) -> Dict[str, Any]:
        """
        Run a learning episode with all subsystems enabled.
        
        Args:
            steps: Number of steps to simulate
            enable_all_subsystems: Whether to enable all learning subsystems
            
        Returns:
            Episode results
        """
        if not self.state.agent or not self.state.world:
            raise ValueError("Agent and world must be set before running learning episode")
        
        # Enable all learning subsystems
        if enable_all_subsystems:
            self.state.agent.enableRewardModulation(True)
            self.state.agent.enableCuriosity(True)
            self.state.agent.enableDevelopment(True)
            self.state.agent.enableStructuralPlasticity(True)
        
        return self.run_episode(steps)
    
    def save_state(self, filepath: str):
        """Save brain state to file."""
        return self.brain.save(filepath)
    
    def load_state(self, filepath: str):
        """Load brain state from file."""
        if self.brain.load(filepath):
            self.state.reset()
            return True
        return False

class NLMExperiment:
    """
    High-level experiment runner for NLM.
    """
    
    def __init__(self, config: Optional[pynlm.Config] = None):
        self.config = config or pynlm.createDefaultConfig()
        self.experiments = []
    
    def add_experiment(self, name: str, simulation: NLMSimulation, steps: int, **kwargs):
        """Add an experiment to run."""
        self.experiments.append({
            'name': name,
            'simulation': simulation,
            'steps': steps,
            'kwargs': kwargs,
        })
    
    def run_all(self) -> Dict[str, Any]:
        """Run all experiments and return results."""
        results = {}
        
        for exp in self.experiments:
            print(f"Running experiment: {exp['name']}")
            
            try:
                # Reset simulation
                exp['simulation'].run_episode(exp['steps'], **exp['kwargs'])
                
                # Store results
                results[exp['name']] = {
                    'success': True,
                    'simulation': exp['simulation'],
                }
                
                print(f"  Completed: {exp['name']}")
                
            except Exception as e:
                print(f"  Failed: {exp['name']} - {e}")
                results[exp['name']] = {
                    'success': False,
                    'error': str(e),
                }
        
        return results

class NLMVisualizer:
    """
    Basic visualization tools for NLM.
    """
    
    @staticmethod
    def plot_brain_activity(brain: pynlm.Brain, steps: int, output_file: str = None):
        """
        Plot brain activity over time.
        
        Note: This is a basic placeholder. For actual plotting, users
        would need to install matplotlib or use other visualization tools.
        """
        print(f"Brain activity visualization requested for {steps} steps")
        print(f"This would create a plot with:")
        print(f"  - Firing rate over time")
        print(f"  - Spike count over time")
        print(f"  - Development stage over time")
        
        if output_file:
            print(f"Plot would be saved to: {output_file}")
        
        # For now, just return sample data
        return {
            'firing_rate': [brain.getAverageFiringRate()] * steps,
            'spike_count': [brain.getTotalSpikeCount()] * steps,
            'development_stage': [brain.getDevelopmentalStage().value] * steps,
        }
    
    @staticmethod
    def create_brain_summary(brain: pynlm.Brain) -> str:
        """Create a text summary of brain state."""
        stats = brain.getStats()
        
        summary = f"""Brain Summary:
=================
Neurons: {stats['total_neurons']}
Synapses: {stats['total_synapses']}
Firing Neurons: {stats['firing_neurons']}
Active Neurons: {stats['active_neurons']}
Average Firing Rate: {stats['avg_firing_rate']:.3f}
Total Spikes: {stats['total_spikes']}
Development Stage: {stats['dev_stage']}
"""
        return summary

# Convenience functions
def create_simple_brain(neuron_count: int = 1000, region_count: int = 1) -> pynlm.Brain:
    """Create a simple brain with basic settings."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

def create_agent_world_simulation(brain: pynlm.Brain, world: pynlm.SimpleWorld, 
                                  steps: int, auto_run: bool = True) -> Dict[str, Any]:
    """
    Create a complete agent-world simulation.
    
    Args:
        brain: The brain to use
        world: The world to simulate in
        steps: Number of steps to simulate
        auto_run: Whether to run the simulation immediately
        
    Returns:
        Simulation results
    """
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable basic learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    if auto_run:
        # Run the simulation
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Return results
        return {
            'brain': brain,
            'agent': agent,
            'world': world,
            'steps': steps,
            'final_stats': {
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'development_stage': agent.getDevelopmentalStage(),
            }
        }
    else:
        # Return setup but don't run
        return {
            'brain': brain,
            'agent': agent,
            'world': world,
            'setup_complete': True,
            'steps': steps,
        }

def benchmark_brain_creation(num_brains: int = 10, neuron_counts: List[int] = None) -> Dict[str, Any]:
    """
    Benchmark brain creation with different sizes.
    
    Args:
        num_brains: Number of brains to create per size
        neuron_counts: List of neuron counts to test
        
    Returns:
        Benchmark results
    """
    if neuron_counts is None:
        neuron_counts = [100, 500, 1000, 2000]
    
    results = {
        'creation_times': {},
        'success_rates': {},
    }
    
    for neuron_count in neuron_counts:
        times = []
        successes = 0
        
        for i in range(num_brains):
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", neuron_count)
            
            start_time = time.time()
            try:
                brain = pynlm.createBrain(config)
                brain.initialize()
                end_time = time.time()
                
                times.append(end_time - start_time)
                successes += 1
                
            except Exception as e:
                print(f"Failed to create brain with {neuron_count} neurons: {e}")
        
        avg_time = sum(times) / len(times) if times else 0
        success_rate = successes / num_brains
        
        results['creation_times'][neuron_count] = avg_time
        results['success_rates'][neuron_count] = success_rate
    
    return results

# Main function for testing
def main():
    """Test the high-level API."""
    print("NLM High-Level API Test")
    print("=" * 40)
    
    # Test simple brain creation
    print("1. Testing simple brain creation...")
    brain = create_simple_brain(500)
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Test simulation
    print("\n2. Testing simulation...")
    sim = NLMSimulation()
    sim.state.add_agent(pynlm.createAgentBrain(sim.brain), pynlm.createSimpleWorld())
    
    results = sim.run_episode(100)
    print(f"   Simulation completed with {results['steps']} steps")
    print(f"   Final firing neurons: {results['final_stats']['final_brain_stats']['firing_neurons']}")
    
    # Test benchmark
    print("\n3. Testing benchmark...")
    benchmark = benchmark_brain_creation(5, [100, 500])
    for neuron_count, avg_time in benchmark['creation_times'].items():
        print(f"   {neuron_count} neurons: {avg_time:.3f}s average")
    
    print("\nAll tests completed!")

if __name__ == "__main__":
    main()
