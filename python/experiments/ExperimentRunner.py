#!/usr/bin/env python3
"""
NLM Experiment Runner
High-level experiment execution for NLM simulations
"""

import pynlm
import json
import time
import threading
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass, field
from enum import Enum
import logging
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class ExperimentStatus(Enum):
    """Experiment execution status"""
    PENDING = "pending"
    RUNNING = "running"
    COMPLETED = "completed"
    PAUSED = "paused"
    CANCELLED = "cancelled"
    FAILED = "failed"

class ExperimentType(Enum):
    """Experiment types"""
    SILENT_BRAIN = "silent_brain"
    WORLD_EXPLORATION = "world_exploration"
    REWARD_LEARNING = "reward_learning"
    DEVELOPMENT = "development"
    COMPREHENSIVE = "comprehensive"

@dataclass
class ExperimentConfig:
    """Configuration for an experiment"""
    name: str
    exp_type: ExperimentType
    brain_config: Optional[Dict[str, Any]] = None
    world_config: Optional[Dict[str, Any]] = None
    max_steps: int = 1000
    timestep: float = 0.1
    description: str = ""
    tags: List[str] = field(default_factory=list)

@dataclass
class ExperimentResult:
    """Result from an experiment run"""
    success: bool
    steps_completed: int
    total_steps: int
    execution_time: float
    final_stats: Dict[str, Any]
    brain_config: Optional[Dict[str, Any]] = None
    metadata: Dict[str, Any] = field(default_factory=dict)
    error_message: Optional[str] = None

class ExperimentRunner:
    """
    High-level experiment runner for NLM simulations.
    
    Provides comprehensive experiment management including:
    - Experiment configuration and validation
    - Progress monitoring and callbacks
    - Multiple experiment types
    - Result aggregation and analysis
    - Performance profiling
    """
    
    def __init__(self, config_file: Optional[str] = None):
        """
        Initialize experiment runner.
        
        Args:
            config_file: Path to experiment configuration file
        """
        self.config_file = config_file
        self.experiments: Dict[str, ExperimentConfig] = {}
        self.results: Dict[str, ExperimentResult] = {}
        self.status: Dict[str, ExperimentStatus] = {}
        self._thread_pool: Dict[str, threading.Thread] = {}
        self._progress_callbacks: Dict[str, Callable] = {}
        
        if config_file:
            self.load_config(config_file)
    
    def load_config(self, config_file: str):
        """
        Load experiment configurations from JSON file.
        
        Args:
            config_file: Path to configuration file
        """
        try:
            with open(config_file, 'r') as f:
                config_data = json.load(f)
            
            for exp_name, exp_config in config_data.items():
                self.experiments[exp_name] = ExperimentConfig(
                    name=exp_name,
                    exp_type=ExperimentType(exp_config.get('type', 'comprehensive')),
                    brain_config=exp_config.get('brain_config', {}),
                    world_config=exp_config.get('world_config', {}),
                    max_steps=exp_config.get('max_steps', 1000),
                    timestep=exp_config.get('timestep', 0.1),
                    description=exp_config.get('description', ''),
                    tags=exp_config.get('tags', [])
                )
            
            logger.info(f"Loaded {len(self.experiments)} experiment configurations")
            
        except Exception as e:
            logger.error(f"Failed to load experiment config: {e}")
            raise
    
    def save_config(self, config_file: str):
        """
        Save experiment configurations to JSON file.
        
        Args:
            config_file: Path to save configuration
        """
        config_data = {}
        for exp_name, exp_config in self.experiments.items():
            config_data[exp_name] = {
                'type': exp_config.exp_type.value,
                'brain_config': exp_config.brain_config,
                'world_config': exp_config.world_config,
                'max_steps': exp_config.max_steps,
                'timestep': exp_config.timestep,
                'description': exp_config.description,
                'tags': exp_config.tags
            }
        
        with open(config_file, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        logger.info(f"Saved {len(self.experiments)} experiment configurations to {config_file}")
    
    def add_experiment(self, config: ExperimentConfig):
        """
        Add a new experiment configuration.
        
        Args:
            config: Experiment configuration
        """
        self.experiments[config.name] = config
        self.status[config.name] = ExperimentStatus.PENDING
        logger.info(f"Added experiment: {config.name}")
    
    def remove_experiment(self, name: str):
        """
        Remove an experiment configuration.
        
        Args:
            name: Experiment name
        """
        if name in self.experiments:
            del self.experiments[name]
        if name in self.status:
            del self.status[name]
        if name in self.results:
            del self.results[name]
        logger.info(f"Removed experiment: {name}")
    
    def set_progress_callback(self, experiment_name: str, callback: Callable):
        """
        Set progress callback for an experiment.
        
        Args:
            experiment_name: Name of experiment
            callback: Function to call with (step, total_steps) progress updates
        """
        self._progress_callbacks[experiment_name] = callback
    
    def run_experiment(self, name: str, progress_callback: Optional[Callable] = None) -> ExperimentResult:
        """
        Run a single experiment.
        
        Args:
            name: Experiment name
            progress_callback: Optional progress callback function
            
        Returns:
            Experiment result
        """
        if name not in self.experiments:
            raise ValueError(f"Experiment '{name}' not found")
        
        if self.status.get(name) == ExperimentStatus.RUNNING:
            raise RuntimeError(f"Experiment '{name}' is already running")
        
        self.status[name] = ExperimentStatus.RUNNING
        logger.info(f"Starting experiment: {name}")
        
        try:
            config = self.experiments[name]
            start_time = time.time()
            
            # Create brain with configuration
            brain_config = pynlm.createDefaultConfig()
            if config.brain_config:
                brain_config = pynlm.create_config_from_dict(config.brain_config)
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            # Setup world if configured
            world = None
            agent = None
            if config.world_config:
                world = pynlm.createSimpleWorld()
                world.configure(
                    config.world_config.get('width', 20),
                    config.world_config.get('height', 20),
                    config.world_config.get('vision_width', 8),
                    config.world_config.get('vision_height', 8)
                )
                world.reset()
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
            
            # Run experiment based on type
            if config.exp_type == ExperimentType.SILENT_BRAIN:
                self._run_silent_brain(brain, config.max_steps, progress_callback)
            elif config.exp_type == ExperimentType.WORLD_EXPLORATION:
                self._run_world_exploration(brain, world, agent, config.max_steps, progress_callback)
            elif config.exp_type == ExperimentType.REWARD_LEARNING:
                self._run_reward_learning(brain, world, agent, config.max_steps, progress_callback)
            elif config.exp_type == ExperimentType.DEVELOPMENT:
                self._run_development(brain, config.max_steps, progress_callback)
            else:  # COMPREHENSIVE
                self._run_comprehensive(brain, world, agent, config.max_steps, progress_callback)
            
            execution_time = time.time() - start_time
            
            # Get final statistics
            final_stats = {
                'total_neurons': brain.total_neuron_count(),
                'total_synapses': brain.total_synapse_count(),
                'firing_neurons': brain.firing_neuron_count(),
                'active_neurons': brain.active_neuron_count(),
                'total_spikes': brain.total_spike_count(),
                'average_firing_rate': brain.average_firing_rate(),
                'developmental_stage': brain.developmental_stage().value,
                'excitation_inhibition_ratio': brain.excitation_inhibition_ratio()
            }
            
            result = ExperimentResult(
                success=True,
                steps_completed=config.max_steps,
                total_steps=config.max_steps,
                execution_time=execution_time,
                final_stats=final_stats,
                brain_config=config.brain_config
            )
            
            self.results[name] = result
            self.status[name] = ExperimentStatus.COMPLETED
            
            logger.info(f"Experiment '{name}' completed successfully in {execution_time:.2f}s")
            return result
            
        except Exception as e:
            execution_time = time.time() - start_time
            error_msg = str(e)
            
            result = ExperimentResult(
                success=False,
                steps_completed=0,
                total_steps=config.max_steps if 'config' in locals() else 0,
                execution_time=execution_time,
                final_stats={},
                error_message=error_msg
            )
            
            self.results[name] = result
            self.status[name] = ExperimentStatus.FAILED
            
            logger.error(f"Experiment '{name}' failed: {error_msg}")
            raise
    
    def _run_silent_brain(self, brain, max_steps, progress_callback):
        """Run silent brain experiment (no world interaction)"""
        for step in range(max_steps):
            brain.step(step)
            if progress_callback:
                progress_callback(step + 1, max_steps)
    
    def _run_world_exploration(self, brain, world, agent, max_steps, progress_callback):
        """Run world exploration experiment"""
        for step in range(max_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if progress_callback:
                progress_callback(step + 1, max_steps)
    
    def _run_reward_learning(self, brain, world, agent, max_steps, progress_callback):
        """Run reward-based learning experiment"""
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        for step in range(max_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward based on agent's performance
            reward = 1.0 if action.getType() == pynlm.ActionType.Eat else 0.0
            predicted_reward = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted_reward)
            
            if progress_callback:
                progress_callback(step + 1, max_steps)
    
    def _run_development(self, brain, max_steps, progress_callback):
        """Run development experiment"""
        agent = None  # Would need proper setup for development
        
        for step in range(max_steps):
            brain.step(step)
            if step % 100 == 0:
                # Change developmental stage every 100 steps
                if step >= 300:
                    brain.set_developmental_stage(pynlm.DevelopmentalStage.Adult)
                elif step >= 200:
                    brain.set_developmental_stage(pynlm.DevelopmentalStage.Maturation)
                elif step >= 100:
                    brain.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)
            
            if progress_callback:
                progress_callback(step + 1, max_steps)
    
    def _run_comprehensive(self, brain, world, agent, max_steps, progress_callback):
        """Run comprehensive experiment with all features"""
        if world and agent:
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
        
        for step in range(max_steps):
            if world:
                world.update(0.1)
                percept = world.getSensoryPercept()
                if agent:
                    agent.processSensoryInput(percept)
            
            brain.step(step)
            
            if world and agent:
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward based on action
                if action.getType() == pynlm.ActionType.Eat:
                    agent.applyRewardModulation(1.0, agent.getPredictionError())
            
            if progress_callback:
                progress_callback(step + 1, max_steps)
    
    def run_experiments_batch(self, experiment_names: List[str]) -> Dict[str, ExperimentResult]:
        """
        Run multiple experiments in batch.
        
        Args:
            experiment_names: List of experiment names to run
            
        Returns:
            Dictionary of experiment results
        """
        results = {}
        
        for name in experiment_names:
            try:
                result = self.run_experiment(name)
                results[name] = result
            except Exception as e:
                logger.error(f"Batch experiment '{name}' failed: {e}")
                results[name] = ExperimentResult(
                    success=False,
                    steps_completed=0,
                    total_steps=self.experiments[name].max_steps,
                    execution_time=0.0,
                    final_stats={},
                    error_message=str(e)
                )
        
        return results
    
    def get_experiment_status(self, name: str) -> ExperimentStatus:
        """
        Get status of an experiment.
        
        Args:
            name: Experiment name
            
        Returns:
            Experiment status
        """
        return self.status.get(name, ExperimentStatus.PENDING)
    
    def get_experiment_result(self, name: str) -> ExperimentResult:
        """
        Get result of an experiment.
n        
        Args:
            name: Experiment name
            
        Returns:
            Experiment result
            
        Raises:
            ValueError: If experiment not found or no results available
        """
        if name not in self.results:
            raise ValueError(f"No results available for experiment '{name}'")
        return self.results[name]
    
    def list_experiments(self) -> Dict[str, Dict]:
        """
        List all experiments with their basic information.
        
        Returns:
            Dictionary with experiment information
        """
        experiment_info = {}
        for name, config in self.experiments.items():
            experiment_info[name] = {
                'type': config.exp_type.value,
                'description': config.description,
                'max_steps': config.max_steps,
                'tags': config.tags,
                'status': self.status.get(name, 'pending')
            }
        return experiment_info
    
    def export_results(self, filepath: str, experiment_names: Optional[List[str]] = None):
        """
        Export experiment results to JSON file.
        
        Args:
            filepath: Output file path
            experiment_names: Optional list of experiment names to export
        """
        if experiment_names is None:
            experiment_names = list(self.results.keys())
        
        export_data = {}
        for name in experiment_names:
            if name in self.results:
                export_data[name] = {
                    'config': self.experiments[name].__dict__ if name in self.experiments else {},
                    'result': self.results[name].__dict__,
                    'status': self.status.get(name, 'pending')
                }
        
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        logger.info(f"Exported results to {filepath}")
    
    def cleanup(self):
        """Clean up resources"""
        # Wait for any running threads to complete
        for name, thread in self._thread_pool.items():
            if thread.is_alive():
                thread.join(timeout=1.0)
        
        self._thread_pool.clear()
        self._progress_callbacks.clear()
        logger.info("Experiment runner cleaned up")


def main():
    """Example usage of ExperimentRunner"""
    # Create experiment runner
    runner = ExperimentRunner()
    
    # Add an experiment
    config = ExperimentConfig(
        name="silent_brain_test",
        exp_type=ExperimentType.SILENT_BRAIN,
        max_steps=100,
        description="Test silent brain experiment"
    )
    runner.add_experiment(config)
    
    # Run experiment
    result = runner.run_experiment("silent_brain_test")
    
    print(f"Experiment completed: {result.success}")
    print(f"Steps completed: {result.steps_completed}/{result.total_steps}")
    print(f"Execution time: {result.execution_time:.2f}s")
    print(f"Final stats: {result.final_stats}")
    
    # Clean up
    runner.cleanup()

if __name__ == "__main__":
    main()
