"""NLM (Neural Learning Machine) - Python API

NLM is an experimental computational brain project that creates a neural system
beginning in a primitive developmental state and acquiring increasingly complex
abilities through interaction with an environment.

This module provides a comprehensive Python interface to the NLM system,
offering advanced features for neural network management, memory systems,
learning algorithms, and experimentation.
"""

from .config import nlm_config, load_config_file, save_config, validate_config, compare_configs
from .brain import BrainNetwork, create_network, import_brain, export_brain, simulate_brain
from .memory import EpisodicMemory, ConsolidationManager, MemoryProfiler, RetrievalOptimizer
from .learning import LearningScheduler, PlasticityController, AdaptiveLearning, CuriosityDrivenLearning
from .neuromodulation import DopamineSystem, RewardEngine, PredictionErrorTracker, MotivationSystem
from .agent import AgentTrainer, Evaluator, CheckpointManager, PerformanceMonitor
from .environment import WorldSimulator, EnvironmentFactory, ScenarioManager, ReplaySystem
from .visualization import Plotter, ActivityAnalyzer, HeatmapGenerator, ActivityStatistics
from .automation import ExperimentRunner, BatchProcessor, ResultAggregator, ReportGenerator
from .ml import NeuralNetworkTrainer, PolicyGradientLearner, ReinforcementLearning, MetaLearner

__version__ = "0.1.0"
__author__ = "NLM Authors"
__all__ = [
    # Configuration Management
    "nlm_config", "load_config_file", "save_config", "validate_config", "compare_configs",
    # Brain and Neural Network Management
    "BrainNetwork", "create_network", "import_brain", "export_brain", "simulate_brain",
    # Memory System Integration
    "EpisodicMemory", "ConsolidationManager", "MemoryProfiler", "RetrievalOptimizer",
    # Learning and Plasticity Control
    "LearningScheduler", "PlasticityController", "AdaptiveLearning", "CuriosityDrivenLearning",
    # Neuromodulation and Reward Systems
    "DopamineSystem", "RewardEngine", "PredictionErrorTracker", "MotivationSystem",
    # Agent Control and Management
    "AgentTrainer", "Evaluator", "CheckpointManager", "PerformanceMonitor",
    # Environment and World Management
    "WorldSimulator", "EnvironmentFactory", "ScenarioManager", "ReplaySystem",
    # Visualization and Analysis
    "Plotter", "ActivityAnalyzer", "HeatmapGenerator", "ActivityStatistics",
    # Automation and Scripting
    "ExperimentRunner", "BatchProcessor", "ResultAggregator", "ReportGenerator",
    # Advanced Machine Learning Integration
    "NeuralNetworkTrainer", "PolicyGradientLearner", "ReinforcementLearning", "MetaLearner",
]

# Convenience classes and functions

class NLM:
    """High-level interface to NLM system for common operations."""
    
    def __init__(self, config_file=None):
        """
        Initialize NLM system.
        
        Args:
            config_file (str, optional): Path to configuration file
        """
        if config_file:
            self.config = load_config_file(config_file)
        else:
            self.config = nlm_config()
        
        self.brain = None
        self.memory = None
        self.world = None
        
    def create_agent(self, brain_config=None):
        """
        Create a new agent with specified brain configuration.
        
        Args:
            brain_config (dict, optional): Brain configuration
            
        Returns:
            AgentBrain: Created agent brain
        """
        from .brain import create_agent_brain
        if brain_config:
            config = nlm_config()
            for key, value in brain_config.items():
                config.set_value(key, value)
        else:
            config = self.config
        
        self.brain = create_network(config)
        self.memory = EpisodicMemory()
        self.world = WorldSimulator()
        
        return self.brain
        
    def run_simulation(self, steps=100, save_checkpoint=False):
        """
        Run a simulation for specified number of steps.
        
        Args:
            steps (int): Number of simulation steps
            save_checkpoint (bool): Whether to save checkpoint after simulation
            
        Returns:
            dict: Simulation results
        """
        results = simulate_brain(self.brain, steps)
        
        if save_checkpoint and self.brain:
            self.save_checkpoint("last_simulation")
            
        return results
        
    def save_checkpoint(self, name):
        """
        Save current agent state to checkpoint.
        
        Args:
            name (str): Checkpoint name
        """
        CheckpointManager().save(self.brain, self.memory, self.world, name)
        
    def load_checkpoint(self, name):
        """
        Load agent state from checkpoint.
        
        Args:
            name (str): Checkpoint name
            
        Returns:
            tuple: (brain, memory, world) restored from checkpoint
        """
        return CheckpointManager().load(name)
        
    def evaluate(self):
        """
        Evaluate current agent performance.
        
        Returns:
            dict: Performance metrics
        """
        evaluator = Evaluator()
        return evaluator.evaluate(self.brain, self.world)