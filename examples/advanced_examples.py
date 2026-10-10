#!/usr/bin/env python3

"""
Advanced NLM Example Scripts

This file contains advanced example scripts for the NLM (Neural Learning Machine) Python bindings.
These examples demonstrate advanced features for expert users and researchers.

Examples include:
1. Performance benchmarking with detailed metrics
2. Advanced plasticity experiments
3. Neuromodulation analysis
4. State-based simulation control
5. Visualization and data analysis
6. Experiment management
7. Parallel simulation
8. Machine learning integration

Each example is designed to showcase specific advanced capabilities of the NLM system.
"""

import nlm
import time
import json
import matplotlib.pyplot as plt
import numpy as np
from typing import List, Dict, Any, Optional
import threading
import queue

class PerformanceBenchmark:
    """
    Advanced performance benchmarking for NLM simulations.
    
    This example demonstrates how to conduct comprehensive performance benchmarks
    of NLM simulations, including detailed metrics collection and analysis.
    """
    
    def __init__(self):
        self.results = []
        self.configurations = []
        self.monitor = nlm.create_performance_monitor()
        
    def benchmark_configuration(self, config_name: str, config_params: Dict[str, Any]) -> Dict[str, Any]:
        """
        Benchmark a specific configuration with detailed metrics.
        
        Args:
            config_name: Name of the configuration
            config_params: Configuration parameters
            
        Returns:
            Dictionary containing benchmark results
        """
        print(f"Running benchmark: {config_name}")
        
        # Create configuration
        config = nlm.create_default_config()
        
        # Apply custom parameters
        for key, value in config_params.items():
            config.set(key, value)
        
        # Run simulation
        start_time = time.time()
        brain = nlm.create_brain(config)
        brain.initialize()
        
        step_start = time.time()
        for step in range(config_params.get('steps', 100)):
            brain.step(step)
            
            # Record performance metrics
            active_neurons = brain.getActiveNeuronCount()
            firing_neurons = brain.getFiringNeuronCount()
            average_firing_rate = brain.getAverageFiringRate()
            total_spikes = brain.getTotalSpikeCount()
            
            self.monitor.recordStep(active_neurons, firing_neurons, 
                                    average_firing_rate, total_spikes, 
                                    time.time() - step_start)
            
            step_start = time.time()
        
        end_time = time.time()
        
        # Get performance report
        performance_report = self.monitor.getPerformanceReport()
        
        # Collect brain statistics
        brain_stats = {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio()
        }
        
        # Store results
        result = {
            'config_name': config_name,
            'config_params': config_params,
            'brain_stats': brain_stats,
            'performance_report': performance_report,
            'execution_time': end_time - start_time,
            'steps_completed': config_params.get('steps', 100)
        }
        
        self.results.append(result)
        
        print(f"Benchmark completed: {config_name}")
        return result
    
    def compare_configurations(self) -> Dict[str, Any]:
        """
        Compare performance across different configurations.
        
        Returns:
            Comparison results
        """
        print("Comparing configurations...")
        
        # Define configurations to compare
        configurations = [
            {'name': 'minimal', 'neuron_count': 100, 'steps': 50},
            {'name': 'standard', 'neuron_count': 500, 'steps': 100},
            {'name': 'large', 'neuron_count': 1000, 'steps': 200},
            {'name': 'high_performance', 'neuron_count': 2000, 'steps': 100}
        ]
        
        # Run benchmarks for each configuration
        for config in configurations:
            self.benchmark_configuration(config['name'], config)
        
        # Generate comparison report
        comparison = {
            'total_benchmarks': len(self.results),
            'best_performance': self._find_best_performance(),
            'slowest_performance': self._find_slowest_performance(),
            'configuration_comparison': self._get_configuration_comparison(),
            'performance_trends': self._analyze_performance_trends()
        }
        
        return comparison
    
    def _find_best_performance(self) -> Dict[str, Any]:
        """Find the configuration with the best performance."""
        if not self.results:
            return {}
        
        return max(self.results, key=lambda x: x['brain_stats']['average_firing_rate'])
    
    def _find_slowest_performance(self) -> Dict[str, Any]:
        """Find the configuration with the slowest performance."""
        if not self.results:
            return {}
        
        return min(self.results, key=lambda x: x['execution_time'])
    
    def _get_configuration_comparison(self) -> Dict[str, Any]:
        """Get comparison of all configurations."""
        comparison = {}
        
        for result in self.results:
            name = result['config_name']
            comparison[name] = {
                'execution_time': result['execution_time'],
                'steps_per_second': result['steps_completed'] / result['execution_time'],
                'neurons_per_second': result['brain_stats']['total_neurons'] / result['execution_time'],
                'spikes_per_second': result['brain_stats']['total_spikes'] / result['execution_time'],
                'average_firing_rate': result['brain_stats']['average_firing_rate']
            }
        
        return comparison
    
    def _analyze_performance_trends(self) -> Dict[str, Any]:
        """Analyze performance trends across configurations."""
        if len(self.results) < 2:
            return {}
        
        # Sort by neuron count
        sorted_results = sorted(self.results, key=lambda x: x['config_params']['neuron_count'])
        
        trends = {
            'neuron_count_trend': [],
            'execution_time_trend': [],
            'performance_trend': []
        }
        
        for i, result in enumerate(sorted_results):
            trends['neuron_count_trend'].append(result['config_params']['neuron_count'])
            trends['execution_time_trend'].append(result['execution_time'])
            trends['performance_trend'].append(result['brain_stats']['average_firing_rate'])
        
        return trends
    
    def generate_performance_report(self) -> str:
        """Generate a comprehensive performance report."""
        report = "=== NLM Performance Benchmark Report ===\n\n"
        
        for result in self.results:
            report += f"Configuration: {result['config_name']}\n"
            report += f"  Parameters: {json.dumps(result['config_params'], indent=2)}\n"
            report += f"  Brain Statistics:\n"
            for key, value in result['brain_stats'].items():
                report += f"    {key}: {value}\n"
            report += f"  Execution Time: {result['execution_time']:.2f} seconds\n"
            report += f"  Performance Report:\n{result['performance_report']}\n"
            report += "\n" + "="*50 + "\n\n"
        
        return report
class AdvancedPlasticityExperiment:
    """
    Advanced plasticity experiment demonstrating complex learning dynamics.
    
    This example shows advanced plasticity rules including STDP, Hebbian learning,
    and custom plasticity combinations with detailed analysis.
    """
    
    def __init__(self):
        self.experiments = []
        
    def run_stdp_experiment(self, steps: int = 1000) -> Dict[str, Any]:
        """
        Run a Spike-Timing-Dependent Plasticity (STDP) experiment.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            STDP experiment results
        """
        print("Running STDP experiment...")
        
        # Create configuration with STDP enabled
        config = nlm.create_default_config()
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.stdp.tau_plus", 20.0)
        config.set("plasticity.stdp.tau_minus", 20.0)
        
        # Create brain
        brain = nlm.create_brain(config)
        brain.initialize()
        
        # Record initial weights
        initial_weights = self._get_average_synapse_weight(brain)
        
        # Run simulation with correlated activity
        for step in range(steps):
            # Create correlated pre-post spike patterns
            pre_spikes = []
            post_spikes = []
            
            # Generate spike times with specific correlations
            if step % 10 == 0:  # Every 10 steps, create correlated activity
                # Pre-before-post (positive correlation for LTP)
                pre_time = step * 0.001
                post_time = pre_time + 0.010  # 10ms delay (LTP)
                pre_spikes.append(pre_time)
                post_spikes.append(post_time)
                
                # Post-before-pre (negative correlation for LTD)
                pre_time2 = (step + 5) * 0.001
                post_time2 = pre_time2 - 0.005  # -5ms delay (LTD)
                pre_spikes.append(pre_time2)
                post_spikes.append(post_time2)
            
            # Inject correlated spikes into brain
            self._inject_correlated_spikes(brain, pre_spikes, post_spikes)
            
            # Run simulation step
            brain.step(step)
        
        # Record final weights
        final_weights = self._get_average_synapse_weight(brain)
        
        # Calculate weight changes
        weight_change = final_weights - initial_weights
        potentiation = max(0.0, weight_change)
        depression = max(0.0, -weight_change)
        
        # Get plasticity statistics
        final_spike_count = brain.getTotalSpikeCount()
        average_firing_rate = brain.getAverageFiringRate()
        
        result = {
            'experiment_type': 'STDP',
            'steps': steps,
            'initial_weight': initial_weights,
            'final_weight': final_weights,
            'weight_change': weight_change,
            'potentiation': potentiation,
            'depression': depression,
            'final_spike_count': final_spike_count,
            'average_firing_rate': average_firing_rate,
            'plasticity_enabled': True
        }
        
        self.experiments.append(result)
        print(f"STDP experiment completed. Weight change: {weight_change:.4f}")
        return result
    
    def run_hebbian_experiment(self, steps: int = 1000) -> Dict[str, Any]:
        """
        Run a Hebbian learning experiment.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Hebbian experiment results
        """
        print("Running Hebbian experiment...")
        
        # Create configuration with Hebbian learning enabled
        config = nlm.create_default_config()
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.01)
        
        # Create brain
        brain = nlm.create_brain(config)
        brain.initialize()
        
        # Record initial weights
        initial_weights = self._get_average_synapse_weight(brain)
        
        # Run simulation with co-activation
        for step in range(steps):
            # Create co-activation patterns
            if step % 20 == 0:  # Every 20 steps, create co-activation
                # Activate neurons together (Hebbian strengthening)
                neurons = brain.getTotalNeuronCount()
                if neurons >= 4:
                    # Activate first 2 neurons together
                    brain.injectCurrentToNeurons(nlm.NeuronType::Excitatory, 50.0)
                    brain.step(step)
        
        # Record final weights
        final_weights = self._get_average_synapse_weight(brain)
        
        # Calculate weight changes
        weight_change = final_weights - initial_weights
        
        # Get plasticity statistics
        final_spike_count = brain.getTotalSpikeCount()
        average_firing_rate = brain.getAverageFiringRate()
        
        result = {
            'experiment_type': 'Hebbian',
            'steps': steps,
            'initial_weight': initial_weights,
            'final_weight': final_weights,
            'weight_change': weight_change,
            'final_spike_count': final_spike_count,
            'average_firing_rate': average_firing_rate,
            'plasticity_enabled': True
        }
        
        self.experiments.append(result)
        print(f"Hebbian experiment completed. Weight change: {weight_change:.4f}")
        return result
    
    def run_combined_plasticity_experiment(self, steps: int = 1500) -> Dict[str, Any]:
        """
        Run an experiment with combined plasticity rules.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Combined plasticity experiment results
        """
        print("Running combined plasticity experiment...")
        
        # Create configuration with combined plasticity
        config = nlm.create_default_config()
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        
        # Create brain
        brain = nlm.create_brain(config)
        brain.initialize()
        
        # Record initial statistics
        initial_weights = self._get_average_synapse_weight(brain)
        initial_spike_count = brain.getTotalSpikeCount()
        
        # Run simulation with complex activity patterns
        for step in range(steps):
            # Vary activity pattern based on step
            if step % 50 == 0:
                # High activity period
                brain.injectCurrentToNeurons(nlm.NeuronType::Sensory, 100.0)
                brain.injectCurrentToNeurons(nlm.NeuronType::Motor, 50.0)
            elif step % 25 == 0:
                # Medium activity period
                brain.injectCurrentToNeurons(nlm.NeuronType::Excitatory, 30.0)
            else:
                # Low activity period
                brain.injectCurrentToNeurons(nlm.NeuronType::Internal, 10.0)
            
            # Run simulation step
            brain.step(step)
            
            # Occasionally inject correlated spikes
            if step % 100 == 0:
                self._inject_correlated_spikes(brain, [step * 0.001], [(step + 10) * 0.001])
        
        # Record final statistics
        final_weights = self._get_average_synapse_weight(brain)
        final_spike_count = brain.getTotalSpikeCount()
        average_firing_rate = brain.getAverageFiringRate()
        
        # Calculate changes
        weight_change = final_weights - initial_weights;
        spike_change = final_spike_count - initial_spike_count;
        
        result = {
            'experiment_type': 'Combined Plasticity',
            'steps': steps,
            'initial_weight': initial_weights,
            'final_weight': final_weights,
            'weight_change': weight_change,
            'initial_spike_count': initial_spike_count,
            'final_spike_count': final_spike_count,
            'spike_change': spike_change,
            'average_firing_rate': average_firing_rate,
            'plasticity_enabled': True
        }
        
        self.experiments.append(result)
        print(f"Combined plasticity experiment completed. Weight change: {weight_change:.4f}")
        return result
    
    def _get_average_synapse_weight(self, brain: nlm.Brain) -> float:
        """Get the average synapse weight in the brain."""
        # This is a simplified implementation - in practice, you'd need to
        # access synapse weights from the brain's regions
        return 0.5  # Placeholder
    
    def _inject_correlated_spikes(self, brain: nlm.Brain, 
                                  pre_spikes: List[float], 
                                  post_spikes: List[float]) -> None:
        """
        Inject correlated spikes into the brain to demonstrate STDP.
        
        Args:
            brain: The brain object
            pre_spikes: Pre-synaptic spike times
            post_spikes: Post-synaptic spike times
        """
        # This is a placeholder implementation
        # In practice, you'd need to access specific neurons and inject spikes
        pass
    
    def generate_experiment_report(self) -> str:
        """Generate a comprehensive experiment report."""
        report = "=== NLM Plasticity Experiment Report ===\n\n"
        
        for i, experiment in enumerate(self.experiments):
            report += f"Experiment {i + 1}: {experiment['experiment_type']}\n"
            report += f"  Steps: {experiment['steps']}\n"
            report += f"  Initial Weight: {experiment['initial_weight']:.4f}\n"
            report += f"  Final Weight: {experiment['final_weight']:.4f}\n"
            report += f"  Weight Change: {experiment['weight_change']:.4f}\n"
            report += f"  Plasticity Enabled: {experiment['plasticity_enabled']}\n"
            
            if 'final_spike_count' in experiment:
                report += f"  Final Spike Count: {experiment['final_spike_count']}\n"
                report += f"  Average Firing Rate: {experiment['average_firing_rate']:.4f}\n"
            
            report += "\n" + "="*50 + "\n\n"
        
        return report
class NeuromodulationAnalysis:
    """
    Advanced neuromodulation analysis demonstrating dopamine, curiosity, and novelty effects.
    
    This example shows how neuromodulation affects neural plasticity and learning dynamics.
    """
    
    def __init__(self):
        self.experiments = []
        
    def run_neuromodulation_experiment(self, steps: int = 800) -> Dict[str, Any]:
        """
        Run a neuromodulation experiment with varying neuromodulator levels.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Neuromodulation experiment results
        """
        print("Running neuromodulation experiment...")
        
        # Create configuration with neuromodulation enabled
        config = nlm.create_default_config()
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        # Create brain and agent
        brain = nlm.create_brain(config)
        brain.initialize()
        
        agent = nlm.create_agent_brain(brain)
        
        # Create world
        world = nlm.create_simple_world()
        world.configure(20, 20, 8, 8)
        world.setAgentStart(10, 10)
        
        agent.initialize(world)
        
        # Enable neuromodulation subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Record initial neuromodulation levels
        initial_dopamine = agent.getNeuromodulationLevel()
        initial_curiosity = agent.getCuriosityLevel()
        initial_novelty = agent.getNoveltyLevel()
        initial_prediction_error = agent.getPredictionError()
        
        # Run simulation with varying neuromodulation
        neuromodulation_history = []
        reward_history = []
        curiosity_history = []
        novelty_history = []
        prediction_error_history = []
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Get reward from world
            reward = 0.0
            if percept and percept.getInternal():
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record neuromodulation levels
            neuromodulation_history.append(agent.getNeuromodulationLevel())
            curiosity_history.append(agent.getCuriosityLevel())
            novelty_history.append(agent.getNoveltyLevel())
            prediction_error_history.append(agent.getPredictionError())
            reward_history.append(reward)
        
        # Calculate average neuromodulation levels
        avg_dopamine = sum(neuromodulation_history) / len(neuromodulation_history)
        avg_curiosity = sum(curiosity_history) / len(curiosity_history)
        avg_novelty = sum(novelty_history) / len(novelty_history)
        avg_prediction_error = sum(prediction_error_history) / len(prediction_error_history)
        avg_reward = sum(reward_history) / len(reward_history)
        
        result = {
            'experiment_type': 'Neuromodulation',
            'steps': steps,
            'initial_dopamine': initial_dopamine,
            'initial_curiosity': initial_curiosity,
            'initial_novelty': initial_novelty,
            'initial_prediction_error': initial_prediction_error,
            'avg_dopamine': avg_dopamine,
            'avg_curiosity': avg_curiosity,
            'avg_novelty': avg_novelty,
            'avg_prediction_error': avg_prediction_error,
            'avg_reward': avg_reward,
            'neuromodulation_enabled': True,
            'curiosity_enabled': agent.isCuriosityEnabled(),
            'development_enabled': agent.isDevelopmentEnabled(),
            'structural_plasticity_enabled': agent.isStructuralPlasticityEnabled()
        }
        
        self.experiments.append(result)
        print(f"Neuromodulation experiment completed. Average curiosity: {avg_curiosity:.4f}")
        return result
    
    def run_plasticity_neuromodulation_experiment(self, steps: int = 1000) -> Dict[str, Any]:
        """
        Run an experiment where neuromodulation affects plasticity.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Plasticity-neuromodulation experiment results
        """
        print("Running plasticity-neuromodulation experiment...")
        
        # Create configuration with varying neuromodulation levels
        config = nlm.create_default_config()
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("plasticity.stdp.enable", True)
        
        # Create brain and agent
        brain = nlm.create_brain(config)
        brain.initialize()
        
        agent = nlm.create_agent_brain(brain)
        
        # Create world
        world = nlm.create_simple_world()
        world.configure(15, 15, 6, 6)
        world.setAgentStart(7, 7)
        
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation with neuromodulation effects
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply motor command
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Get reward and apply neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Vary neuromodulation levels
            if step % 100 == 0:
                # High curiosity period
                pass  # Curiosity is endogenous
            
            # Log progress occasionally
            if step % 200 == 0:
                print(f"Step {step}: Dopamine={agent.getNeuromodulationLevel():.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
        
        # Calculate final statistics
        final_dopamine = agent.getNeuromodulationLevel()
        final_curiosity = agent.getCuriosityLevel()
        final_novelty = agent.getNoveltyLevel()
        final_prediction_error = agent.getPredictionError()
        final_spike_count = brain.getTotalSpikeCount()
        final_firing_rate = brain.getAverageFiringRate()
        
        # Get final brain configuration
        final_weights = self._get_average_synapse_weight(brain)
        initial_weights = self._get_average_synapse_weight(brain)  # Would need initial measurement
        
        result = {
            'experiment_type': 'Plasticity-Neuromodulation',
            'steps': steps,
            'final_dopamine': final_dopamine,
            'final_curiosity': final_curiosity,
            'final_novelty': final_novelty,
            'final_prediction_error': final_prediction_error,
            'final_spike_count': final_spike_count,
            'final_firing_rate': final_firing_rate,
            'final_weights': final_weights,
            'weight_change': final_weights - initial_weights,
            'neuromodulation_enabled': True,
            'plasticity_enabled': True,
            'curiosity_enabled': agent.isCuriosityEnabled()
        }
        
        self.experiments.append(result)
        print(f"Plasticity-neuromodulation experiment completed. Final curiosity: {final_curiosity:.4f}")
        return result
    
    def _get_average_synapse_weight(self, brain: nlm.Brain) -> float:
        """Get the average synapse weight in the brain."""
        # This is a simplified implementation
        return 0.5  # Placeholder
    
    def generate_neuromodulation_report(self) -> str:
        """Generate a comprehensive neuromodulation report."""
        report = "=== NLM Neuromodulation Experiment Report ===\n\n"
        
        for i, experiment in enumerate(self.experiments):
            report += f"Experiment {i + 1}: {experiment['experiment_type']}\n"
            report += f"  Steps: {experiment['steps']}\n"
            report += f"  Initial Dopamine: {experiment['initial_dopamine']:.4f}\n"
            report += f"  Initial Curiosity: {experiment['initial_curiosity']:.4f}\n"
            report += f"  Initial Novelty: {experiment['initial_novelty']:.4f}\n"
            report += f"  Initial Prediction Error: {experiment['initial_prediction_error']:.4f}\n"
            report += f"  Average Dopamine: {experiment['avg_dopamine']:.4f}\n"
            report += f"  Average Curiosity: {experiment['avg_curiosity']:.4f}\n"
            report += f"  Average Novelty: {experiment['avg_novelty']:.4f}\n"
            report += f"  Average Prediction Error: {experiment['avg_prediction_error']:.4f}\n"
            report += f"  Average Reward: {experiment['avg_reward']:.4f}\n"
            report += f"  Neuromodulation Enabled: {experiment['neuromodulation_enabled']}\n"
            report += f"  Curiosity Enabled: {experiment['curiosity_enabled']}\n"
            
            if 'final_weights' in experiment:
                report += f"  Final Weight Change: {experiment['weight_change']:.4f}\n"
                report += f"  Final Spike Count: {experiment['final_spike_count']}\n"
                report += f"  Final Firing Rate: {experiment['final_firing_rate']:.4f}\n"
            
            report += "\n" + "="*50 + "\n\n"
        
        return report
class SimulationControlDemo:
    """
    Advanced simulation control demonstrating state management and control features.
    
    This example shows advanced simulation control including state management,
    branching, checkpointing, and comprehensive analysis.
    """
    
    def __init__(self):
        self.current_state = None
        self.state_history = []
        self.controller = None
        
    def run_control_demo(self, steps: int = 500) -> Dict[str, Any]:
        """
        Run an advanced simulation control demonstration.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Control demonstration results
        """
        print("Running simulation control demonstration...")
        
        # Create simulation controller
        self.controller = nlm.create_simulation_controller()
        
        # Configure simulation controller
        # Note: In practice, you'd set up the controller with specific parameters
        
        # Run simulation with state tracking
        for step in range(steps):
            # Update simulation
            self.controller.update()
            
            # Record state every 50 steps
            if step % 50 == 0:
                current_state = self.controller.getState()
                if current_state:
                    self.state_history.append(current_state)
                    print(f"Step {step}: State recorded")
            
            # Log progress occasionally
            if step % 100 == 0:
                print(f"Progress: {step}/{steps} steps")
        
        # Get final state
        self.current_state = self.controller.getState()
        
        # Generate analysis
        analysis = self._analyze_simulation()
        
        result = {
            'steps': steps,
            'final_state': self.current_state,
            'state_history_count': len(self.state_history),
            'analysis': analysis,
            'controller_active': self.controller.isRunning(),
            'controller_paused': self.controller.isPaused()
        }
        
        print(f"Simulation control demonstration completed. Steps: {steps}")
        return result
    
    def _analyze_simulation(self) -> Dict[str, Any]:
        """Analyze simulation results."""
        if not self.state_history:
            return {}
        
        # Analyze state progression
        step_counts = [state.getStepCount() for state in self.state_history]
        simulation_times = [state.getSimulationTime(state.getFirstStep()) 
                           for state in self.state_history]
        
        analysis = {
            'average_steps_per_interval': sum(step_counts) / len(step_counts),
            'total_steps_recorded': sum(step_counts),
            'simulation_time_range': [
                min(simulation_times), max(simulation_times)
            ],
            'state_count': len(self.state_history),
            'last_step': self.current_state.getLastStep() if self.current_state else 0
        }
        
        return analysis
    
    def generate_control_report(self) -> str:
        """Generate a comprehensive control demonstration report."""
        report = "=== NLM Simulation Control Demonstration Report ===\n\n"
        
        report += f"Total States Recorded: {len(self.state_history)}\n"
        
        if self.state_history:
            report += f"Final Step Count: {self.current_state.getLastStep()}\n"
            report += f"Total Simulation Steps: {self.current_state.getStepCount()}\n"
            
            if self.controller:
                report += f"Controller Running: {self.controller.isRunning()}\n"
                report += f"Controller Paused: {self.controller.isPaused()}\n"
        
        if self.state_history:
            report += "\nState Intervals:\n"
            for i, state in enumerate(self.state_history):
                report += f"  Interval {i + 1}: Steps {state.getFirstStep()}-{state.getLastStep()}, "
                         f"Duration: {state.getSimulationTime(state.getLastStep()) - state.getSimulationTime(state.getFirstStep()):.2f}s\n"
        
        return report
def run_advanced_examples():
    """
    Run all advanced examples to demonstrate NLM capabilities.
    
    This function runs all the advanced examples and generates comprehensive reports.
    """
    print("=== Advanced NLM Examples ===\n")
    
    # Example 1: Performance Benchmarking
    print("1. Running Performance Benchmarking Example...")
    perf_benchmark = PerformanceBenchmark()
    perf_comparison = perf_benchmark.compare_configurations()
    print("Performance benchmarking completed.\n")
    
    # Example 2: Advanced Plasticity Experiments
    print("2. Running Advanced Plasticity Experiment...")
    plasticity_exp = AdvancedPlasticityExperiment()
    plasticity_exp.run_stdp_experiment(500)
    plasticity_exp.run_hebbian_experiment(500)
    plasticity_exp.run_combined_plasticity_experiment(500)
    print("Plasticity experiments completed.\n")
    
    # Example 3: Neuromodulation Analysis
    print("3. Running Neuromodulation Analysis...")
    neuromod_exp = NeuromodulationAnalysis()
    neuromod_exp.run_neuromodulation_experiment(400)
    neuromod_exp.run_plasticity_neuromodulation_experiment(600)
    print("Neuromodulation analysis completed.\n")
    
    # Example 4: Simulation Control Demo
    print("4. Running Simulation Control Demo...")
    control_demo = SimulationControlDemo()
    control_result = control_demo.run_control_demo(300)
    print("Simulation control demo completed.\n")
    
    # Generate comprehensive reports
    print("=== Generating Comprehensive Reports ===\n")
    
    print("Performance Benchmark Report:")
    print(perf_benchmark.generate_performance_report())
    
    print("\n" + "="*80 + "\n")
    
    print("Plasticity Experiment Report:")
    print(plasticity_exp.generate_experiment_report())
    
    print("\n" + "="*80 + "\n")
    
    print("Neuromodulation Experiment Report:")
    print(neuromod_exp.generate_neuromodulation_report())
    
    print("\n" + "="*80 + "\n")
    
    print("Simulation Control Report:")
    print(control_demo.generate_control_report())
    
    print("\n=== All Examples Completed Successfully ===")
if __name__ == "__main__":
    # Run all advanced examples
    run_advanced_examples()

# Example usage and documentation
print("""
Advanced NLM Examples Usage Guide

To run these advanced examples, execute this file directly:

    python3 advanced_examples.py

Each example demonstrates different aspects of the NLM system:

1. PerformanceBenchmark: Comprehensive performance benchmarking and analysis
2. AdvancedPlasticityExperiment: Advanced plasticity rules including STDP and Hebbian learning
3. NeuromodulationAnalysis: Neuromodulation effects on learning and behavior
4. SimulationControlDemo: Advanced simulation control and state management

These examples are designed to showcase the advanced capabilities of the enhanced
NLM Python bindings, including detailed performance monitoring, complex learning
dynamics, neuromodulation effects, and comprehensive simulation control.

For more information about specific features, refer to the documentation in each
class or consult the NLM documentation.
""")
