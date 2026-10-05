"""
NLM (Neural Learning Machine) - Python Package

A Python interface for the NLM neural simulation framework with enhanced
Python features including numpy integration, debugging utilities, and
efficient data export/import.

Key Features:
- Automatic numpy array conversion for Vision and Audio sensory inputs
- Numpy-based visualization functions for brain state
- Numpy-based data export/import functionality
- Comprehensive type hints and documentation using pybind11's API
- Python-specific debugging and timing utilities
- Improved error formatting for Python users
- Advanced experiment control and analysis tools
- Comprehensive research and development capabilities
- Distributed computing support for large-scale simulations
- Python scripting engine for automation and customization

License: MIT
Author: NLM Authors
Version: 0.1.0
"""

from .version import __version__
from .core import *
from .brain import *
from .agent import *
from .world import *
from .visualization import *
from .experiments import *
from .analysis import *
from .research import *
from .distributed import *
from .scripting import *

__all__ = [
    # Core modules
    'Config',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Brain
    'Brain',
    
    # Agent
    'AgentBrain',
    
    # World
    'SimpleWorld',
    
    # Sensory
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    
    # Motor
    'Action', 'ActionResult', 'AgentBody', 'SensoryPercept',
    
    # World objects
    'WorldObject',
    
    # Python utilities
    'formatTraceback',
    'startSessionLogger',
    'createTimer',
    'checkNumpyArray',
    'getNumpyArrayDimensions',
    'convertVectorToNumpy',
    'convertNumpyToVector',
    'safeNumpyAccess',
    'exportBrainState',
    'importBrainState',
    'exportWorldState',
    'visualizeNeuronActivity',
    'visualizeSynapticConnections',
    'visualizeBrainRegions',
    'generateSpikeRaster',
    'visualizeNetworkTopology',
    
    # Configuration utilities
    'createDefaultConfig',
    'validateConfig',
    
    # Advanced modules
    'experiments',
    'analysis', 
    'visualization',
    'research',
    'distributed',
    'scripting',
    
    # Experiment-related classes
    'Experiment',
    'ParameterSweep',
    'MonteCarloSimulation',
    'TimeLapseRecorder',
    'BatchProcessor',
    'BrainController',
    'NeuralRecorder',
    'BrainStateRestorer',
    'CrossSimulationLearner',
    'DevelopmentalSimulator',
    'MemoryReplay',
    
    # Analysis-related classes
    'SpikeTrainAnalyzer',
    'NetworkVisualizer',
    'PopulationAnalyzer',
    'TemporalPatternDetector',
    'PerformanceProfiler',
    'ResourceMonitor',
    'StatisticalAnalyzer',
    'TimeSeriesAnalyzer',
    
    # Research-related classes
    'DevelopmentalStageManipulator',
    'PlasticityRuleEditor',
    'NeuromodulationTuner',
    'ArchitectureModifier',
    'AblationStudy',
    'GeneticAlgorithm',
    'ParameterOptimizer',
    'BayesianInference',
    
    # Distributed-related classes
    'AgentCoordinator',
    'MultiBrainLearner',
    'SocialSimulator',
    'CompetitiveLearner',
    'CustomWorldBuilder',
    'DynamicWorld',
    'MultiEnvironmentSwitcher',
    'WorldReplicator',
    
    # Scripting-related classes
    'ExperimentDSL',
    'NeuralAnalysisAPI',
    'WorldBuilderAPI',
    'DevelopmentToolAPI',
    'OptimizationAPI',
    'SimulationRunner',
    'ParameterSweeper',
    'MonteCarloRunner',
    'TimeLapseRecorder',
    'BatchProcessor',
    'NeuralRecorder',
    'BrainStateRestorer',
    
    # Version
    '__version__',
]

# Python utilities
import sys
import os
import logging
from typing import Dict, Any, Optional

# Session logging
_session_logger = None

def _init_session_logger():
    """Initialize Python session logger for debugging"""
    global _session_logger
    if _session_logger is None:
        from nlm.brain import PythonSessionLogger
        _session_logger = PythonSessionLogger()
    return _session_logger

# Configuration utilities
_config_defaults = {
    "simulation_steps": 1000,
    "enable_reward_modulation": True,
    "enable_structural_plasticity": True,
    "enable_development": True,
    "enable_curiosity": False,
    "thread_safe": False,
    "precision_mode": "single",
    "memory_pool_enabled": False,
}

def create_default_config() -> 'Config':
    """Create a default configuration for NLM brain simulation"""
    config = Config()
    for key, value in _config_defaults.items():
        config.set(key, value)
    return config

def validate_config(config: 'Config') -> Dict[str, str]:
    """Validate configuration and return any errors found"""
    return config.validate()

def get_version() -> str:
    """Get the current version of the package"""
    return __version__

# Module initialization
if __name__ != "__main__":
    # Initialize session logger when module is imported
    _init_session_logger()
    
    # Log module import
    logger = _init_session_logger()
    logger.logEntry(f"NLM Python package v{__version__} imported successfully")
