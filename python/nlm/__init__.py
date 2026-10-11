"""NLM (Neural Learning Machine) - Python API

NLM is an experimental computational brain project that creates a neural system
beginning in a primitive developmental state and acquiring increasingly complex
abilities through interaction with an environment.

This module provides a comprehensive Python interface to the NLM system,
offering advanced features for neural network management, memory systems,
learning algorithms, and experimentation.
"""

import numpy as np
from typing import Dict, List, Optional, Any, Union
import time

# Core modules
from . import config
from . import brain
from . import memory
from . import learning
from . import neuromodulation
from . import agent
from . import environment
from . import visualization
from . import automation
from . import ml

# Re-export core classes and functions from submodules

# Configuration Management
from .config import nlm_config, load_config_file, save_config, validate_config, compare_configs

# Brain and Neural Network Management
from .brain import BrainNetwork, create_network, import_brain, export_brain, simulate_brain

# Memory System Integration
from .memory import (
    MemoryType, MemoryEntry,
    WorkingMemory, EpisodicMemory,
    ConsolidationManager, MemoryProfiler, RetrievalOptimizer
)

# Learning and Plasticity Control
from .learning import (
    LearningRule, PlasticityType,
    LearningScheduler, PlasticityController,
    AdaptiveLearning, CuriosityDrivenLearning
)

# Neuromodulation and Reward Systems
from .neuromodulation import (
    NeuromodulatorType,
    DopamineSystem, RewardEngine, PredictionErrorTracker, MotivationSystem
)

# Agent Control and Management
from .agent import (
    AgentTrainer, Evaluator, CheckpointManager, PerformanceMonitor
)

# Environment and World Management
from .environment import (
    EnvironmentType,
    WorldSimulator, EnvironmentFactory,
    ScenarioManager, ReplaySystem
)

# Visualization and Analysis
from .visualization import (
    Plotter, ActivityAnalyzer, HeatmapGenerator, ActivityStatistics
)

# Automation and Scripting
from .automation import (
    ExperimentRunner, BatchProcessor, ResultAggregator, ReportGenerator
)

# Advanced Machine Learning Integration
from .ml import (
    NeuralNetworkTrainer, PolicyGradientLearner,
    ReinforcementLearning, MetaLearner
)

# Convenience classes and functions

class NLM:
    """High-level interface to NLM system for common operations."""
    
    def __init__(self, config_file: str = None):
        """
        Initialize NLM system.
n
        Args:
            config_file: Path to configuration file
        """
        if config_file:
            self.config = config.load_config_file(config_file)
        else:
            self.config = config.nlm_config()
        
        self.brain = None
        self.memory = None
        self.world = None
        
    def create_agent(self, brain_config: Dict[str, Any] = None) -> 'brain.BrainNetwork':
        """
        Create a new agent with specified brain configuration.
n
        Args:
            brain_config: Brain configuration
            
        Returns:
            BrainNetwork: Created brain network
        """
        if brain_config:
            config_obj = config.nlm_config()
            for key, value in brain_config.items():
                config_obj.setValue(key, value)
        else:
            config_obj = self.config
        
        self.brain = brain.create_network(config_obj)
        self.memory = memory.EpisodicMemory()
        self.world = environment.WorldSimulator()
        
        return self.brain
        
    def run_simulation(self, steps: int = 100, save_checkpoint: bool = False) -> Dict[str, Any]:
        """
        Run a simulation for specified number of steps.
n
        Args:
            steps: Number of simulation steps
            save_checkpoint: Whether to save checkpoint after simulation
            
        Returns:
            Dict[str, Any]: Simulation results
        """
        results = brain.simulate_brain(self.brain, steps)
        
        if save_checkpoint and self.brain:
            agent.CheckpointManager().save(self.brain, self.memory, self.world, "last_simulation")
            
        return results
        
    def save_checkpoint(self, name: str) -> None:
        """
        Save current agent state to checkpoint.
n
        Args:
            name: Checkpoint name
        """
        agent.CheckpointManager().save(self.brain, self.memory, self.world, name)
        
    def load_checkpoint(self, name: str) -> Tuple['brain.BrainNetwork', memory.EpisodicMemory, environment.WorldSimulator]:
        """
        Load agent state from checkpoint.
n
        Args:
            name: Checkpoint name
            
        Returns:
            Tuple[BrainNetwork, EpisodicMemory, WorldSimulator]: Restored components
        """
        return agent.CheckpointManager().load(name)
        
    def evaluate(self) -> Dict[str, Any]:
        """
        Evaluate current agent performance.
n
        Returns:
            Dict[str, Any]: Performance metrics
        """
        evaluator = agent.Evaluator()
        return evaluator.evaluate(self.brain, self.world)
        
    def run_experiment(self, experiment_config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Run automated experiment.
n
        Args:
            experiment_config: Experiment configuration
            
        Returns:
            Dict[str, Any]: Experiment results
        """
        experiment_runner = automation.ExperimentRunner()
        
        # Create experiment
        experiment_name = experiment_config.get("name", "experiment")
        experiment_runner.create_experiment(
            experiment_name,
            experiment_config.get("description", ""),
            experiment_config.get("parameters", {}),
            experiment_config.get("environment_config", {})
        )
        
        # Run experiment
        results = experiment_runner.run_experiment(
            experiment_name,
            iterations=experiment_config.get("iterations", 1),
            parallel=experiment_config.get("parallel", False)
        )
        
        return results
        
    def analyze_results(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Analyze experiment results.

        Args:
            results: List of experiment results
            
        Returns:
            Dict[str, Any]: Analysis results
        """
        analyzer = automation.ResultAggregator()
        return analyzer.aggregate_results(results)
        
    def generate_report(self, results: List[Dict[str, Any]], 
                       report_title: str = "Experiment Report",
                       report_format: str = "text") -> str:
        """
        Generate report from results.
n
        Args:
            results: Experiment results
            report_title: Report title
            report_format: Report format
            
        Returns:
            str: Generated report
        """
        report_generator = automation.ReportGenerator(report_format=report_format)
        
        # Create report data
        report_data = {
            "results_summary": {
                "total_experiments": len(results),
                "successful_experiments": len([r for r in results if r.get("status") == "completed"]),
                "total_reward": sum(r.get("metrics", {}).get("total_reward", 0) for r in results),
                "average_reward": np.mean([r.get("metrics", {}).get("total_reward", 0) for r in results]) if results else 0
            },
            "detailed_results": results,
            "timestamp": time.time()
        }
        
        # Create and generate report
        report = report_generator.create_report(
            "main_report",
            report_title,
            "Report generated from NLM experiments",
            report_data
        )
        
        report_generator.generate_report("main_report")
        
        return report_generator.get_report("main_report")

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__all__ = [
    # Configuration Management
    "nlm_config", "load_config_file", "save_config", "validate_config", "compare_configs",
    # Brain and Neural Network Management
    "BrainNetwork", "create_network", "import_brain", "export_brain", "simulate_brain",
    # Memory System Integration
    "MemoryType", "MemoryEntry",
    "WorkingMemory", "EpisodicMemory",
    "ConsolidationManager", "MemoryProfiler", "RetrievalOptimizer",
    # Learning and Plasticity Control
    "LearningRule", "PlasticityType",
    "LearningScheduler", "PlasticityController",
    "AdaptiveLearning", "CuriosityDrivenLearning",
    # Neuromodulation and Reward Systems
    "NeuromodulatorType",
    "DopamineSystem", "RewardEngine", "PredictionErrorTracker", "MotivationSystem",
    # Agent Control and Management
    "AgentTrainer", "Evaluator", "CheckpointManager", "PerformanceMonitor",
    # Environment and World Management
    "EnvironmentType",
    "WorldSimulator", "EnvironmentFactory",
    "ScenarioManager", "ReplaySystem",
    # Visualization and Analysis
    "Plotter", "ActivityAnalyzer", "HeatmapGenerator", "ActivityStatistics",
    # Automation and Scripting
    "ExperimentRunner", "BatchProcessor", "ResultAggregator", "ReportGenerator",
    # Advanced Machine Learning Integration
    "NeuralNetworkTrainer", "PolicyGradientLearner", "ReinforcementLearning", "MetaLearner",
    # Convenience classes
    "NLM"
]