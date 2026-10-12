#!/usr/bin/env python3

"""
NLM (Neural Learning Machine) Python Bindings - Enhanced Power User API

This module provides comprehensive access to advanced NLM functionality for power users,
including benchmarking, serialization, visualization, configuration management,
experiment management, and performance monitoring.

The API is organized into the following modules:
- nlm: Core brain simulation functionality
- nlm.benchmark: Advanced benchmarking tools
- nlm.serialization: Checkpointing and state management
- nlm.visualization: Brain state exploration and analysis
- nlm.config: Configuration management and templates
- nlm.experiments: Experiment management and comparison
- nlm.monitoring: Real-time performance monitoring

Example usage:

# Basic brain simulation
import nlm

# Configure brain
cfg = nlm.Config()
cfg.set("brain.neuron_count", 10000)
cfg.set("brain.connection_probability", 0.1)

# Create and initialize brain
brain = nlm.Brain(cfg)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

# Get performance metrics
metrics = nlm.computeMetrics(brain)
print(f"Firing rate: {metrics['avg_firing_rate']:.2f} Hz")
print(f"Active neurons: {metrics['active_neurons']}")

# Advanced benchmarking
benchmark = nlm.benchmark.ScalingBenchmark()
config = nlm.benchmark.BenchmarkConfig()
config.neuronCount = 1000
config.stepsPerScale = 1000
result = benchmark.run(config, create_brain_func, run_simulation_func, collect_metrics_func)

# Save brain state for later
nlm.saveBrain(brain, "brain_checkpoint.bin")

# Load and continue
loaded_brain = nlm.loadBrain("brain_checkpoint.bin")

# Visualization
viz = nlm.visualization.VisualizationInterface()
viz.initialize()
viz.visualizeNetwork(loaded_brain)
viz.render()
viz.close()

# Experiment management
experiment = nlm.experiments.createExperiment("memory_test")
experiment.recordMetric("total_reward", 150.5)
experiment.recordMetric("success_rate", 0.85)

# Compare experiments
comparison = nlm.experiments.compareExperiments([exp1, exp2, exp3])
print(f"Best performing experiment: {comparison.best_experiment}")

# Performance monitoring
monitor = nlm.monitoring.PerformanceMonitor()
monitor.start()

# Run simulation while monitoring
for step in range(1000):
    brain.step(step)
    monitor.recordStep(step)

stats = monitor.getCurrentMetrics()
print(f"Performance: {stats.neuronsPerSecond:.0f} neurons/sec")
print(f"Memory usage: {stats.totalMemoryMB} MB")
monitor.stop()
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

# Core imports
from nlm.core.brain import Brain
from nlm.core.config import Config
from nlm.core.world import SimpleWorld
from nlm.core.agent import AgentBrain
from nlm.core.sensory import Vision, Audio, InternalSignals
from nlm.core.action import Action, MotorCommand
from nlm.core.types import *

# Benchmark module imports
from nlm.benchmark.scaling import ScalingBenchmark
from nlm.benchmark.performance import PerformanceMonitor
from nlm.benchmark.memory import MemoryProfiler

# Serialization module imports
from nlm.serialization.checkpoint import CheckpointSystem
from nlm.serialization.state import StateManager

# Visualization module imports
from nlm.visualization.interface import VisualizationInterface
from nlm.visualization.analysis import BrainStateAnalyzer
from nlm.visualization.plotting import PlotGenerator

# Configuration module imports
from nlm.config.manager import ConfigManager
from nlm.config.templates import ConfigTemplate
from nlm.config.profiling import ConfigProfiler

# Experiment module imports
from nlm.experiments.manager import ExperimentManager
from nlm.experiments.comparison import ExperimentComparator
from nlm.experiments.analysis import ExperimentAnalyzer

# Monitoring module imports
from nlm.monitoring.real_time import RealTimeMonitor
from nlm.monitoring.profiler import SystemProfiler
from nlm.monitoring.allocator import MemoryAllocatorProfiler

# Convenience functions for common power user operations
from nlm.utils.performance import run_performance_benchmark
from nlm.utils.serialization import save_experiment_state
from nlm.utils.visualization import create_brain_snapshot
from nlm.utils.analysis import analyze_brain_dynamics

# Version and metadata
__all__ = [
    # Core
    "Brain", "Config", "SimpleWorld", "AgentBrain",
    "Vision", "Audio", "InternalSignals", "Action", "MotorCommand",
    # Types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "ActionType", "MotorCommand",
    # Benchmark
    "ScalingBenchmark", "PerformanceMonitor", "MemoryProfiler",
    # Serialization
    "CheckpointSystem", "StateManager",
    # Visualization
    "VisualizationInterface", "BrainStateAnalyzer", "PlotGenerator",
    # Configuration
    "ConfigManager", "ConfigTemplate", "ConfigProfiler",
    # Experiment
    "ExperimentManager", "ExperimentComparator", "ExperimentAnalyzer",
    # Monitoring
    "RealTimeMonitor", "SystemProfiler", "MemoryAllocatorProfiler",
    # Utils
    "run_performance_benchmark", "save_experiment_state",
    "create_brain_snapshot", "analyze_brain_dynamics",
    # Version
    "__version__", "__author__"
]

# Default convenience functions
def computeBrainMetrics(brain):
    """Compute comprehensive brain metrics."""
    return brain.computeMetrics()

def runOptimizedSimulation(brain, world, agent, steps):
    """Run a complete simulation episode."""
    return brain.runCompleteSimulation(world, agent, steps)

def saveExperimentData(experiment, filepath):
    """Save experiment data with automatic metadata."""
    return experiment.save(filepath)

def loadExperimentData(filepath):
    """Load experiment data with metadata."""
    return Experiment.load(filepath)

# Common benchmark configurations
def getStandardBenchmarks():
    """Get standard benchmark configurations for different scales."""
    return {
        "small": {"neurons": 1000, "steps": 1000},
        "medium": {"neurons": 10000, "steps": 5000},
        "large": {"neurons": 100000, "steps": 10000},
        "huge": {"neurons": 1000000, "steps": 50000},
    }

# Common visualization presets
def getVisualizationPresets():
    """Get common visualization presets."""
    return {
        "network": "neural_network",
        "activity": "firing_patterns",
        "spikes": "spike_raster",
        "weights": "synaptic_weights",
        "trajectories": "neuron_trajectories",
        "memory": "memory_system",
        "development": "developmental_stages",
    }

# Error handling
class NLMError(Exception):
    """Base exception for NLM Python bindings."""
    pass

class ConfigError(NLMError):
    """Configuration error."""
    pass

class BenchmarkError(NLMError):
    """Benchmark error."""
    pass

class VisualizationError(NLMError):
    """Visualization error."""
    pass

class SerializationError(NLMError):
    """Serialization error."""
    pass

# Context manager for resource management
class NLMSession:
    """Context manager for NLM sessions."""
    
    def __init__(self, auto_save=True, auto_profiling=False):
        self.auto_save = auto_save
        self.auto_profiling = auto_profiling
        self.session_id = id(self)
        self.start_time = None
        self.brain = None
        self.experiment = None
        
    def __enter__(self):
        import time
        self.start_time = time.time()
        print(f"NLM Session {self.session_id} started")
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type:
            print(f"NLM Session {self.session_id} ended with error: {exc_val}")
        else:
            duration = time.time() - self.start_time
            print(f"NLM Session {self.session_id} completed successfully in {duration:.2f}s")
        
        # Auto-save if configured
        if self.auto_save and self.brain:
            self.saveSession()
            
    def createBrain(self, config):
        """Create brain for this session."""
        self.brain = Brain(config)
        self.brain.initialize()
        return self.brain
        
    def createExperiment(self, name):
        """Create experiment for this session."""
        self.experiment = ExperimentManager.createExperiment(name)
        return self.experiment
        
    def saveSession(self, prefix="session"):
        """Save current session state."""
        timestamp = time.time()
        brain_file = f"{prefix}_{self.session_id}_brain.bin"
        exp_file = f"{prefix}_{self.session_id}_experiment.json"
        
        if self.brain:
            saveBrain(self.brain, brain_file)
            
        if self.experiment:
            saveExperimentData(self.experiment, exp_file)
            
        return brain_file, exp_file
        
    def loadSession(self, brain_file, exp_file):
        """Load session state."""
        if brain_file:
            self.brain = loadBrain(brain_file)
            
        if exp_file:
            self.experiment = loadExperimentData(exp_file)
            
        return self

# Session factory
def createSession(auto_save=True, auto_profiling=False):
    """Create a new NLM session."""
    return NLMSession(auto_save, auto_profiling)

# Quick access helpers
brain = type('BrainProxy', (), {})()
brain.create = lambda config: Brain(config)
brain.save = lambda b, f: saveBrain(b, f)
brain.load = lambda f: loadBrain(f)

config = type('ConfigProxy', (), {})()
config.create = lambda: Config()
config.load = lambda f: Config().loadFromFile(f)
config.save = lambda c, f: c.saveToFile(f)

print(f"NLM Python Bindings v{__version__} loaded successfully")
print("Advanced power user features available via:")
print("  - nlm.benchmark (Scalable benchmarking)")
print("  - nlm.serialization (Checkpointing)")
print("  - nlm.visualization (Analysis tools)")
print("  - nlm.config (Configuration management)")
print("  - nlm.experiments (Experiment comparison)")
print("  - nlm.monitoring (Performance monitoring)")
print("  - nlm.utils (Convenience functions)")
