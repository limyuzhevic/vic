"""
NLM Python Bindings
===================

This module provides Python bindings for the NLM (Neural Learning Machine) framework.
It enables users to work with neural simulations through a high-level Python API.

Key Features:
- Object-oriented interface to neural systems
- Experiment management and scripting
- Visualization and analysis tools
- Performance benchmarking
- Configuration management

Usage Example:

>>> import nlm
>>>
>>> # Create a basic brain simulation
>>> brain = nlm.createBrain()
>>> brain.initialize()
>>> 
>>> # Run simulation steps
>>> for step in range(1000):
...     brain.step(step, step * 0.001)
... 
>>> # Get statistics
>>> firing_rate = brain.getAverageFiringRate()
>>> print(f"Average firing rate: {firing_rate}")

Advanced Usage:

>>> # Run experiments with experiment management
>>> suite = nlm.ExperimentSuite("learning_experiments")
>>> suite.add_experiment("plasticity_test", {"neurons": 500, "steps": 2000})
>>> results = suite.run_all(nlm.createBrain)
>>> 
>>> # Analyze activity patterns
>>> analyzer = nlm.ActivityAnalyzer(brain)
>>> spike_patterns = analyzer.analyze_spike_patterns()
>>> 
>>> # Create custom scripts
>>> script = nlm.ExperimentScript("my_experiment", "Run learning test")
>>> script.set_variable("config", {"neurons": 1000})
>>> results = script.execute("create_brain\nrun_steps steps=1000")

For more detailed documentation, see the HOW_TO_USE.md file.
"""

# Import core types and classes from pybind11 bindings
from . import config_tools
from . import experiment_manager
from . import analysis
from . import scripting

# Re-export everything for easy import
from .config_tools import (
    ConfigFormat, ExperimentConfig, BenchmarkConfig,
    ValidationTools, PerformanceProfiler
)

from .experiment_manager import (
    ExperimentStatus, ExperimentResult, ExperimentRunner,
    ExperimentSuite, ParameterSweep
)

from .analysis import (
    AnalysisType, AnalysisResult, ActivityAnalyzer,
    MemoryAnalyzer, PerformanceAnalyzer, PlottingTools,
    DataExporter
)

from .scripting import (
    NLMScript, ScriptError, ExperimentScript, AnalysisScript,
    AutomationTools
)

# Convenience functions
def create_brain(config=None):
    """Create a new NLM brain with optional configuration."""
    if config is None:
        config = {}
    return experiment_manager.ExperimentRunner._create_brain_from_config(config)

def create_default_config():
    """Create a default configuration."""
    return config_tools.ExperimentConfig(
        name="default",
        description="Default NLM configuration",
        parameters={
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.15,
            "simulation_timestep": 0.001,
            "random_seed": 42
        }
    )

def run_basic_test(neurons=100, steps=1000):
    """Run a basic connectivity test."""
    config = create_default_config()
    config.parameters.update({
        "neuron_count": neurons,
        "steps": steps
    })
    
    brain = create_brain(config.to_dict())
    brain.initialize()
    
    for step in range(steps):
        brain.step(step, step * config.parameters["simulation_timestep"])
    
    return {
        "success": True,
        "neurons": neurons,
        "steps": steps,
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate()
    }

def benchmark_config(config, iterations=5):
    """Benchmark a configuration."""
    profiler = config_tools.PerformanceProfiler()
    
    results = []
    for i in range(iterations):
        brain = create_brain(config)
        brain.initialize()
        
        start_time = time.time()
        
        steps = config.get("steps", 1000)
        for step in range(steps):
            brain.step(step, step * config.get("timestep", 0.001))
        
        end_time = time.time()
        
        results.append({
            "duration": end_time - start_time,
            "steps_per_second": steps / (end_time - start_time),
            "total_spikes": brain.getTotalSpikeCount()
        })
    
    return results

# Python version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "contact@nlm-project.org"
__github__ = "https://github.com/nlm-project/nlm"

# Module metadata
__all__ = [
    # Core types and classes
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept',
    
    # Advanced tools
    'ConfigFormat', 'ExperimentConfig', 'BenchmarkConfig',
    'ValidationTools', 'PerformanceProfiler',
    
    # Experiment management
    'ExperimentStatus', 'ExperimentResult', 'ExperimentRunner',
    'ExperimentSuite', 'ParameterSweep',
    
    # Visualization and analysis
    'AnalysisType', 'AnalysisResult', 'ActivityAnalyzer',
    'MemoryAnalyzer', 'PerformanceAnalyzer', 'PlottingTools',
    'DataExporter',
    
    # Scripting
    'NLMScript', 'ScriptError', 'ExperimentScript', 'AnalysisScript',
    'AutomationTools',
    
    # Convenience functions
    'create_brain', 'create_default_config', 'run_basic_test',
    'benchmark_config'
]

# Initialize matplotlib for non-interactive use
import matplotlib
matplotlib.use('Agg')

# Import time for benchmarking
import time