#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Advanced Python Modules

This module provides advanced Python functionality for power users of the NLM framework.
It includes comprehensive experiment control, neural analysis tools, visualization utilities,
research tools, distributed computing support, and scripting capabilities.

License: MIT
Author: NLM Authors
Version: 0.1.0
"""

import os
import sys
import json
import time
import threading
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Optional, Union, Callable, Tuple
from dataclasses import dataclass, field
from enum import Enum, auto
from pathlib import Path
import warnings
from abc import ABC, abstractmethod
import multiprocessing as mp
import queue
import pickle
import logging

# Import NLM core modules
import nlm_py
from nlm import (
    Config, Brain, SimpleWorld, AgentBrain, Vision, Audio,
    NeuralRegion, NeuralPopulation, Neuron, Synapse
)

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Version
__version__ = "0.1.0"

# Experiment DSL - Domain-specific language for defining experiments
class ExperimentDSL:
    """Domain-specific language for defining NLM experiments."""
    
    @staticmethod
    def parse_experiment_spec(spec: Dict[str, Any]) -> Dict[str, Any]:
        """Parse experiment specification."""
        # Extract experiment parameters
        experiment_config = {
            'name': spec.get('name', 'Unnamed Experiment'),
            'type': spec.get('type', 'simulation'),
            'duration': spec.get('duration', 1000),
            'parameters': spec.get('parameters', {}),
            'agents': spec.get('agents', []),
            'recordings': spec.get('recordings', []),
            'analysis': spec.get('analysis', []),
        }
        return experiment_config
    
    @staticmethod
    def validate_experiment_spec(spec: Dict[str, Any]) -> List[str]:
        """Validate experiment specification."""
        errors = []
        
        # Check required fields
        required_fields = ['name', 'type']
        for field in required_fields:
            if field not in spec:
                errors.append(f"Missing required field: {field}")
                
        # Validate experiment type
        valid_types = ['simulation', 'parameter_sweep', 'monte_carlo', 
                      'developmental', 'learning', 'ablation', 'optimization']
        if 'type' in spec and spec['type'] not in valid_types:
            errors.append(f"Invalid experiment type: {spec['type']}")
            
        # Validate duration
        if 'duration' in spec and spec['duration'] <= 0:
            errors.append("Duration must be positive")
            
        return errors
    
    @staticmethod
    def generate_experiment_code(spec: Dict[str, Any]) -> str:
        """Generate Python code for experiment execution."""
        code_template = f'''
# Generated experiment code
experiment_name = "{spec['name']}"
experiment_type = "{spec['type']}"

# Experiment configuration
config = {{
    "max_steps": {spec.get('duration', 1000)},
    "neuron_count": {spec.get('parameters', {}).get('neuron_count', 1000)},
    "enable_visualization": {str(spec.get('parameters', {}).get('enable_visualization', True)).lower()},
}}

# Create brain and world
brain = createBrain(config)
world = createSimpleWorld()
world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)

# Run experiment
brain.initialize()
for step in range(config["max_steps"]):
    brain.step(step)
    
    # Record data if specified
    if "recordings" in {spec.get('recordings', [])}:
        # Implementation would depend on recording specifications
        pass
        
    # Apply agent behavior if specified
    if "agents" in {spec.get('agents', [])}:
        # Implementation would depend on agent specifications
        pass

print(f"Experiment {{experiment_name}} completed successfully")
'''
        return code_template

class NeuralAnalysisAPI:
    """Neural analysis API for analyzing simulation data."""
    
    @staticmethod
    def analyze_spike_trains(spike_times: List[float], sampling_rate: float = 1000.0) -> Dict[str, Any]:
        """Analyze spike train data."""
        if not spike_times:
            return {"error": "No spike times provided"}
            
        analysis = {
            "total_spikes": len(spike_times),
            "firing_rate": len(spike_times) / (spike_times[-1] - spike_times[0]) if len(spike_times) > 1 else 0.0,
            "mean_inter_spike_interval": np.mean(np.diff(spike_times)) if len(spike_times) > 1 else 0.0,
            "std_inter_spike_interval": np.std(np.diff(spike_times)) if len(spike_times) > 1 else 0.0,
            "cv_of_isi": 0.0,
            "first_spike_time": spike_times[0] if spike_times else 0.0,
            "last_spike_time": spike_times[-1] if spike_times else 0.0,
            "spike_count_histogram": {},
            "periodicity": 0.0,
            "bifurcation_index": 0.0,
        }
        
        if len(spike_times) > 1:
            analysis["cv_of_isi"] = analysis["std_inter_spike_interval"] / analysis["mean_inter_spike_interval"]
            
        # Create spike count histogram
        hist, bins = np.histogram(spike_times, bins=50)
        analysis["spike_count_histogram"] = {
            "counts": hist.tolist(),
            "bin_edges": bins.tolist()
        }
        
        return analysis
    
    @staticmethod
    def analyze_neural_connectivity(brain: Brain) -> Dict[str, Any]:
        """Analyze neural connectivity patterns."""
        connectivity_analysis = {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "connection_density": 0.0,
            "network_clustering": 0.0,
            "global_efficiency": 0.0,
            "modularity": 0.0,
            "hub_nodes": [],
            "community_structure": {},
            "excitatory_inhibitory_ratio": 0.0,
        }
        
        # Calculate connection density
        if connectivity_analysis["total_neurons"] > 0:
            connectivity_analysis["connection_density"] = (
                connectivity_analysis["total_synapses"] / 
                (connectivity_analysis["total_neurons"] ** 2)
            )
        
        return connectivity_analysis
    
    @staticmethod
    def analyze_population_dynamics(brain: Brain, population_id: int) -> Dict[str, Any]:
        """Analyze population dynamics."""
        population_analysis = {
            "population_id": population_id,
            "neuron_count": 0,
            "active_neurons": 0,
            "firing_rate": 0.0,
            "mean_activity": 0.0,
            "activity_variance": 0.0,
            "activity_skewness": 0.0,
            "activity_kurtosis": 0.0,
            "temporal_patterns": [],
            "spike_train_correlation": 0.0,
            "oscillation_frequency": 0.0,
            "power_spectrum": {},
            "reconstruction_error": 0.0,
        }
        
        # Implementation would require actual population access
        return population_analysis
    
    @staticmethod
    def detect_temporal_patterns(spike_times: List[float], 
                                min_freq: float = 1.0, 
                                max_freq: float = 50.0,
                                fs: float = 1000.0) -> Dict[str, Any]:
        """Detect rhythmic patterns and oscillations in spike trains."""
        if len(spike_times) < 2:
            return {"error": "Insufficient spike data for pattern detection"}
            
        temporal_analysis = {
            "dominant_frequency": 0.0,
            "frequency_power": {},
            "rhythmicity": 0.0,
            "oscillation_index": 0.0,
            "theta_power": 0.0,
            "alpha_power": 0.0,
            "beta_power": 0.0,
            "gamma_power": 0.0,
            "pattern_confidence": 0.0,
        }
        
        # Implementation would use Fourier analysis
        return temporal_analysis

# World Builder API
class WorldBuilderAPI:
    """World builder API for creating custom environment configurations."""
    
    @staticmethod
    def create_custom_world(config: Dict[str, Any]) -> SimpleWorld:
        """Create a custom world configuration."""
        world = SimpleWorld()
        
        # Apply custom configuration
        if "width" in config:
            world.configure(width=config["width"], height=config.get("height", 100.0),
                          visionWidth=config.get("vision_width", 20),
                          visionHeight=config.get("vision_height", 20))
                          
        if "objects" in config:
            for obj_config in config["objects"]:
                # Implementation would depend on world object types
                pass
                
        return world
    
    @staticmethod
    def create_dynamic_world(config: Dict[str, Any]) -> SimpleWorld:
        """Create a dynamic world that evolves over time."""
        world = SimpleWorld()
        world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)
        
        # Implementation would include world evolution logic
        return world
    
    @staticmethod
    def create_multi_environment_switcher(configs: List[Dict[str, Any]]) -> Callable:
        """Create a world that can switch between different configurations."""
        current_index = [0]
        
        def switcher(step: int, brain: Brain) -> SimpleWorld:
            # Switch world configuration based on step
            index = (current_index[0] + step) % len(configs)
            config = configs[index]
            return WorldBuilderAPI.create_custom_world(config)
            
        return switcher
    
    @staticmethod
    def reproduce_world(world: SimpleWorld, brain: Brain) -> SimpleWorld:
        """Create an exact copy of a simulated world."""
        # Implementation would create world state copy
        return world

# Development Tool API
class DevelopmentToolAPI:
    """Development tool API for brain development research."""
    
    @staticmethod
    def manipulate_developmental_stage(brain: Brain, stage: str) -> None:
        """Manipulate brain development stage."""
        # Implementation would modify brain development state
        pass
    
    @staticmethod
    def edit_plasticity_rules(brain: Brain, rules: Dict[str, Any]) -> None:
        """Edit plasticity rules during simulation."""
        # Implementation would modify plasticity parameters
        pass
    
    @staticmethod
    def tune_neuromodulation(brain: Brain, config: Dict[str, Any]) -> None:
        """Adjust neuromodulator effects in real-time."""
        # Implementation would modify neuromodulation parameters
        pass
    
    @staticmethod
    def modify_architecture(brain: Brain, operations: List[Dict[str, Any]]) -> None:
        """Add/remove brain regions dynamically."""
        # Implementation would modify brain architecture
        pass

# Optimization API
class OptimizationAPI:
    """Optimization API for parameter optimization and tuning."""
    
    @staticmethod
    def gradient_based_optimization(objective_function: Callable, 
                                   initial_params: Dict[str, Any],
                                   constraints: Dict[str, Any] = None) -> Dict[str, Any]:
        """Use gradient-based methods for parameter tuning."""
        # Implementation would use scipy.optimize
        pass
    
    @staticmethod
    def genetic_algorithm(objective_function: Callable,
                        population_size: int = 100,
                        generations: int = 50,
                        mutation_rate: float = 0.1,
                        crossover_rate: float = 0.8) -> Dict[str, Any]:
        """Use evolutionary algorithms to optimize brain parameters."""
        # Implementation would use genetic algorithm
        pass
    
    @staticmethod
    def parameter_sweep(objective_function: Callable,
                       parameters: Dict[str, List[Any]]) -> Dict[str, Any]:
        """Systematically vary parameters for hyperparameter optimization."""
        # Implementation would perform parameter sweep
        pass
    
    @staticmethod
    def bayesian_inference(data: List[Any], prior: Dict[str, Any] = None) -> Dict[str, Any]:
        """Use Bayesian methods for uncertainty quantification."""
        # Implementation would use Bayesian inference
        pass

# Simulation Runners
class SimulationRunner:
    """Advanced simulation control for running experiments."""
    
    def __init__(self, brain: Brain, world: SimpleWorld, config: Dict[str, Any] = None):
        self.brain = brain
        self.world = world
        self.config = config or {}
        self.is_running = False
        self.current_step = 0
        self.max_steps = config.get('max_steps', 1000) if config else 1000
        self.callbacks = []
        self.data_collectors = []
        
    def add_callback(self, callback: Callable) -> None:
        """Add callback function."""
        self.callbacks.append(callback)
        
    def add_data_collector(self, collector: Callable) -> None:
        """Add data collector function."""
        self.data_collectors.append(collector)
        
    def run(self, steps: int = None) -> Dict[str, Any]:
        """Run simulation for specified steps."""
        if steps is None:
            steps = self.max_steps
            
        self.is_running = True
        self.current_step = 0
        start_time = time.time()
        
        results = {
            'steps_completed': 0,
            'data_collected': {},
            'callbacks_executed': 0,
            'simulation_time': 0.0,
            'brain_state': None,
            'world_state': None,
        }
        
        try:
            for step in range(steps):
                if not self.is_running:
                    break
                    
                # Execute callbacks
                for callback in self.callbacks:
                    try:
                        callback(step, self.brain, self.world)
                        results['callbacks_executed'] += 1
                    except Exception as e:
                        logger.error(f"Callback failed at step {step}: {e}")
                
                # Execute data collectors
                for collector in self.data_collectors:
                    try:
                        data = collector(step, self.brain, self.world)
                        if data:
                            if not results['data_collected']:
                                results['data_collected'] = data
                            else:
                                for key, value in data.items():
                                    if key not in results['data_collected']:
                                        results['data_collected'][key] = []
                                    if isinstance(value, list):
                                        results['data_collected'][key].extend(value)
                                    else:
                                        results['data_collected'][key].append(value)
                    except Exception as e:
                        logger.error(f"Data collector failed at step {step}: {e}")
                
                # Step brain and world
                self.brain.step(step)
                
                # Update world if configured
                if self.config.get('update_world', True):
                    self.world.update(step * self.config.get('timestep', 0.1))
                    
                self.current_step += 1
                results['steps_completed'] += 1
                
        except Exception as e:
            logger.error(f"Simulation failed at step {step}: {e}")
            results['error'] = str(e)
        finally:
            self.is_running = False
            end_time = time.time()
            results['simulation_time'] = end_time - start_time
            
            # Collect final state
            results['brain_state'] = self._collect_brain_state()
            results['world_state'] = self._collect_world_state()
            
        return results
        
    def _collect_brain_state(self) -> Dict[str, Any]:
        """Collect current brain state."""
        return {
            'step': self.current_step,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
        }
        
    def _collect_world_state(self) -> Dict[str, Any]:
        """Collect current world state."""
        return {
            'step': self.current_step,
            'width': self.world.getWidth(),
            'height': self.world.getHeight(),
            'simulation_time': self.world.getSimulationTime(),
            'agent_x': self.world.getAgentBody().x,
            'agent_y': self.world.getAgentBody().y,
        }
        
    def stop(self) -> None:
        """Stop simulation."""
        self.is_running = False
class Experiment:
    """Base class for experiments."""
    
    def __init__(self, name: str, brain: Brain, world: SimpleWorld):
        self.name = name
        self.brain = brain
        self.world = world
        self.config = {}
        self.results = {}
        
    def run(self) -> Dict[str, Any]:
        """Run the experiment."""
        raise NotImplementedError("Subclasses must implement run method")
        
    def save_results(self, filepath: str) -> None:
        """Save experiment results to file."""
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)
        logger.info(f"Results saved to {filepath}")
class ParameterSweep(Experiment):
    """Parameter sweep experiment."""
    
    def __init__(self, name: str, brain: Brain, world: SimpleWorld, 
                 parameters: Dict[str, List[Any]]):
        super().__init__(name, brain, world)
        self.parameters = parameters
        self.parameter_combinations = self._generate_combinations()
        self.results = {'experiments': []}
        
    def _generate_combinations(self) -> List[Dict[str, Any]]:
        """Generate all parameter combinations."""
        import itertools
        
        keys = list(self.parameters.keys())
        values = list(self.parameters.values())
        
        combinations = []
        for combination in itertools.product(*values):
            param_dict = dict(zip(keys, combination))
            combinations.append(param_dict)
            
        return combinations
        
    def run(self) -> Dict[str, Any]:
        """Run parameter sweep."""
        logger.info(f"Running parameter sweep for {len(self.parameter_combinations)} combinations")
        
        for i, params in enumerate(self.parameter_combinations):
            logger.info(f"Running combination {i+1}/{len(self.parameter_combinations)}")
            
            # Create experiment with specific parameters
            experiment = self._create_parameter_experiment(params)
            experiment_result = experiment.run()
            
            self.results['experiments'].append({
                'parameters': params,
                'result': experiment_result,
                'combination_index': i
            })
            
        return self.results
        
    def _create_parameter_experiment(self, params: Dict[str, Any]) -> 'Experiment':
        """Create experiment with specific parameters."""
        # This would create a specific experiment type based on parameters
        return SimulationExperiment(f"{self.name}_param_{hash(json.dumps(params))}", 
                                   self.brain, self.world, params)
class MonteCarloSimulation(Experiment):
    """Monte Carlo simulation experiment."""
    
    def __init__(self, name: str, brain: Brain, world: SimpleWorld, 
                 n_simulations: int, parameter_sampler: Callable):
        super().__init__(name, brain, world)
        self.n_simulations = n_simulations
        self.parameter_sampler = parameter_sampler
        self.results = {'simulations': []}
        
    def run(self) -> Dict[str, Any]:
        """Run Monte Carlo simulation."""
        logger.info(f"Running Monte Carlo simulation with {self.n_simulations} iterations")
        
        for i in range(self.n_simulations):
            logger.info(f"Running simulation {i+1}/{self.n_simulations}")
            
            # Sample parameters
            params = self.parameter_sampler()
            
            # Create simulation
            simulation = self._create_monte_carlo_simulation(params, i)
            simulation_result = simulation.run()
            
            self.results['simulations'].append({
                'parameters': params,
                'result': simulation_result,
                'simulation_index': i
            })
            
        return self.results
        
    def _create_monte_carlo_simulation(self, params: Dict[str, Any], 
                                       index: int) -> 'Experiment':
        """Create Monte Carlo simulation with specific parameters."""
        return SimulationExperiment(f"{self.name}_mc_{index}", 
                                   self.brain, self.world, params)
class TimeLapseRecorder(Experiment):
    """Time-lapse recording experiment."""
    
    def __init__(self, name: str, brain: Brain, world: SimpleWorld, 
                 recording_interval: int = 10, output_format: str = "json"):
        super().__init__(name, brain, world)
        self.recording_interval = recording_interval
        self.output_format = output_format
        self.recordings = []
        self.results = {'recordings': []}
        
    def run(self) -> Dict[str, Any]:
        """Run time-lapse recording."""
        logger.info(f"Running time-lapse recording with interval {self.recording_interval}")
        
        max_steps = self.config.get('max_steps', 1000)
        
        for step in range(0, max_steps, self.recording_interval):
            logger.info(f"Recording at step {step}")
            
            # Record current state
            recording = self._record_state(step)
            self.recordings.append(recording)
            
            # Save recording if configured
            if self.output_format == "json":
                self._save_recording(recording)
                
        self.results['recordings'] = self.recordings
        return self.results
        
    def _record_state(self, step: int) -> Dict[str, Any]:
        """Record current simulation state."""
        return {
            'step': step,
            'timestamp': time.time(),
            'brain_state': self._collect_brain_state_at_step(step),
            'world_state': self._collect_world_state_at_step(step),
            'spike_times': self._collect_spike_times(step),
        }
        
    def _save_recording(self, recording: Dict[str, Any]) -> None:
        """Save recording to file."""
        filename = f"recording_{recording['step']}.json"
        with open(filename, 'w') as f:
            json.dump(recording, f, indent=2)
            
    def _collect_brain_state_at_step(self, step: int) -> Dict[str, Any]:
        """Collect brain state at specific step."""
        # This would require implementing step tracking
        return {}
        
    def _collect_world_state_at_step(self, step: int) -> Dict[str, Any]:
        """Collect world state at specific step."""
        # This would require implementing step tracking
        return {}
        
    def _collect_spike_times(self, step: int) -> List[float]:
        """Collect spike times at specific step."""
        # This would require implementing spike tracking
        return []
class BatchProcessor(Experiment):
    """Batch processing experiment."""
    
    def __init__(self, name: str, brain: Brain, world: SimpleWorld, 
                 batch_size: int = 10, processing_strategy: str = "sequential"):
        super().__init__(name, brain, world)
        self.batch_size = batch_size
        self.processing_strategy = processing_strategy
        self.results = {'batches': []}
        
    def run(self) -> Dict[str, Any]:
        """Run batch processing."""
        logger.info(f"Running batch processing with batch size {self.batch_size}")
        
        max_steps = self.config.get('max_steps', 1000)
        
        if self.processing_strategy == "sequential":
            self._run_sequential(max_steps)
        elif self.processing_strategy == "parallel":
            self._run_parallel(max_steps)
        else:
            raise ValueError(f"Unknown processing strategy: {self.processing_strategy}")
            
        return self.results
        
    def _run_sequential(self, max_steps: int) -> None:
        """Run processing sequentially."""
        for batch_start in range(0, max_steps, self.batch_size):
            batch_end = min(batch_start + self.batch_size, max_steps)
            
            logger.info(f"Processing batch {batch_start}-{batch_end-1}")
            
            batch_result = self._process_batch(batch_start, batch_end)
            self.results['batches'].append(batch_result)
            
    def _run_parallel(self, max_steps: int) -> None:
        """Run processing in parallel."""
        import concurrent.futures
        
        batches = []
        for batch_start in range(0, max_steps, self.batch_size):
            batch_end = min(batch_start + self.batch_size, max_steps)
            batches.append((batch_start, batch_end))
            
        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
            futures = [executor.submit(self._process_batch, start, end) 
                     for start, end in batches]
            
            for i, future in enumerate(concurrent.futures.as_completed(futures)):
                try:
                    batch_result = future.result()
                    self.results['batches'].append(batch_result)
                except Exception as e:
                    logger.error(f"Batch processing failed: {e}")
                    
    def _process_batch(self, batch_start: int, batch_end: int) -> Dict[str, Any]:
        """Process a single batch of simulation steps."""
        batch_result = {
            'batch_start': batch_start,
            'batch_end': batch_end,
            'steps_processed': batch_end - batch_start,
            'metrics': {},
            'errors': []
        }
        
        try:
            for step in range(batch_start, batch_end):
                self.brain.step(step)
                
                # Collect metrics
                metrics = self._collect_batch_metrics(step)
                batch_result['metrics'].update(metrics)
                
        except Exception as e:
            batch_result['errors'].append(str(e))
            
        return batch_result
        
    def _collect_batch_metrics(self, step: int) -> Dict[str, Any]:
        """Collect metrics for batch processing."""
        return {
            'step': step,
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'total_spikes': self.brain.getTotalSpikeCount(),
        }
class BrainController:
    """Advanced brain control for experimentation."""
    
    def __init__(self, brain: Brain):
        self.brain = brain
        self.control_commands = []
        self.recorded_states = {}
        self.replay_data = {}
        
    def record_neural_activity(self, duration: int = 1000) -> Dict[str, Any]:
        """Record neural activity for analysis."""
        logger.info(f"Recording neural activity for {duration} steps")
        
        recording = {
            'start_time': time.time(),
            'duration': duration,
            'neuron_data': {},
            'spike_times': [],
            'synapse_data': {},
        }
        
        max_steps = self.brain.getTotalNeuronCount()
        
        for step in range(min(duration, max_steps)):
            self.brain.step(step)
            
            # Record neural data
            neurons = self._collect_neural_data(step)
            recording['neuron_data'][step] = neurons
            
            # Record spike times
            spikes = self._collect_spike_times(step)
            recording['spike_times'].extend(spikes)
            
        return recording
        
    def _collect_neural_data(self, step: int) -> Dict[str, Any]:
        """Collect neural data at specific step."""
        return {
            'step': step,
            'neuron_states': {},
            'population_activity': {},
            'region_activity': {},
        }
        
    def _collect_spike_times(self, step: int) -> List[float]:
        """Collect spike times at specific step."""
        # This would require implementing spike tracking
        return []
        
    def restore_brain_state(self, state_key: str) -> None:
        """Load specific brain state and replay experiences."""
        if state_key in self.recorded_states:
            state = self.recorded_states[state_key]
            
            # Restore brain state
            self._restore_brain_from_state(state)
            logger.info(f"Restored brain state: {state_key}")
        else:
            logger.warning(f"Unknown state key: {state_key}")
            
    def _restore_brain_from_state(self, state: Dict[str, Any]) -> None:
        """Restore brain from state data."""
        # Implementation would depend on state format
        pass
        
    def transfer_knowledge(self, source_brain: 'BrainController') -> None:
        """Transfer knowledge between simulations."""
        logger.info("Transferring knowledge between simulations")
        
        # Transfer recorded states
        self.replay_data.update(source_brain.recorded_states)
        
        # Transfer control commands
        self.control_commands.extend(source_brain.control_commands)
        
        logger.info(f"Transferred {len(source_brain.recorded_states)} states and " \
                   f"{len(source_brain.control_commands)} commands")
class SimulationExperiment(Experiment):
    """Basic simulation experiment."""
    
    def __init__(self, name: str, brain: Brain, world: SimpleWorld, config: Dict[str, Any] = None):
        super().__init__(name, brain, world)
        self.config = config or {}
        self.config['max_steps'] = self.config.get('max_steps', 100)
        self.config['timestep'] = self.config.get('timestep', 0.1)
        
    def run(self) -> Dict[str, Any]:
        """Run simulation experiment."""
        logger.info(f"Running simulation experiment: {self.name}")
        
        max_steps = self.config['max_steps']
        timestep = self.config['timestep']
        
        results = {
            'experiment_name': self.name,
            'max_steps': max_steps,
            'timestep': timestep,
            'steps_completed': 0,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'spike_count': 0,
            'active_neurons': 0,
            'firing_neurons': 0,
            'average_firing_rate': 0.0,
            'excitation_inhibition_ratio': 0.0,
        }
        
        self.brain.initialize()
        
        for step in range(max_steps):
            self.brain.step(step)
            
            # Collect metrics
            results['steps_completed'] += 1
            results['spike_count'] = self.brain.getTotalSpikeCount()
            results['active_neurons'] = self.brain.getActiveNeuronCount()
            results['firing_neurons'] = self.brain.getFiringNeuronCount()
            results['average_firing_rate'] = self.brain.getAverageFiringRate()
            results['excitation_inhibition_ratio'] = self.brain.getExcitationInhibitionRatio()
            
        return results