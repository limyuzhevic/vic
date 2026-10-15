#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Python Bindings
==================================================

A Python interface for the NLM C++ neural simulation framework.
Provides comprehensive bindings for all brain components, memory systems,
neuromodulation, prediction, cognition, and development systems.

The module exposes the complete NLM brain architecture as a Python API,
including simulation control, configuration management, and advanced
analysis tools.

Key Features:
- Complete bindings for Brain, AgentBrain, Neuron, Synapse, and all subsystems
- Memory management with garbage collection support
- Real-time simulation control and monitoring
- Advanced configuration management
- Visualization and debugging tools
- Performance profiling APIs
- Comprehensive error handling and validation

Usage Example:
```python
import nlm

# Create a default configuration
config = nlm.Config()

# Create a brain with the configuration
brain = nlm.Brain(config)

# Initialize the brain
brain.initialize()

# Run simulation
sim_step = 0
while sim_step < 100:
    brain.step(sim_step, sim_step * 0.1)
    sim_step += 1
```

For command-line interface, see nlm_cli.py
"""

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Core module imports
from .core import (
    # Core types and values
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    Timestamp, SimulationStep, TimestepDuration,
    MembranePotential, FiringRate, SynapticWeight,
    
    # Identifiers and constants
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

from .brain import (
    Neuron, Synapse, NeuralRegion, NeuralPopulation,
    Brain, AgentBrain,
)

from .memory import (
    WorkingMemory, EpisodicMemory, SemanticMemory,
    ProceduralMemory, AssociativeMemory, Memory,
)

from .plasticity import (
    STDP, Hebbian, StructuralPlasticity, PlasticityRule,
    RewardModulatedSTDP, TripletSTDP,
)

from .neuromodulation import (
    Neuromodulator, Dopamine, Acetylcholine, Norepinephrine,
    Serotonin, Curiosity, Novelty, PredictionError,
)

from .prediction import (
    NeuralPrediction, ActionConsequencePredictor, PredictionErrorSignal,
)

from .cognition import (
    NeuralPlanner, ConceptFormation, AttentionalSelection,
)

from .development import (
    DevelopmentSystem,
)

from .sensory import (
    SensoryInput, Vision, Audio, InternalSignals, SensoryPercept,
)

from .motor import (
    Action, MotorSystem,
)

from .world import (
    SimpleWorld, WorldObject, ActionResult,
)

from .core import Config

# System-level APIs
from .systems import (
    SimulationClock, RandomGenerator, Logger,
    SpikeSystem,
)

# Advanced features
from .advanced import (
    ModelLoader, ModelExporter, RealTimeController,
    ParameterTuner, CallbackManager, PerformanceProfiler,
    Visualizer, Debugger, AnalysisTools,
)

# Experiment management
from .experiment import (
    ExperimentManager, BatchProcessor, ParameterSweep, ExperimentTemplate,
    ExperimentResult, ResultAnalyzer, createExperimentManager,
    createBatchProcessor, createParameterSweep, createExperimentTemplate,
    createExperimentResult, createResultAnalyzer, createParameterSweepBatch,
    createHyperparameterConfig, createEvolutionConfig, createGeneticAlgorithmConfig,
    createParticleSwarmConfig, createSimulatedAnnealingConfig,
    createBayesianOptimizationConfig, createRandomSearchConfig, createGridSearchConfig,
)

# Export everything for convenience
__all__ = [
    # Core
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'Timestamp', 'SimulationStep', 'TimestepDuration',
    'MembranePotential', 'FiringRate', 'SynapticWeight',
    
    # Identifiers
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    
    # Brain components
    'Neuron', 'Synapse', 'NeuralRegion', 'NeuralPopulation',
    'Brain', 'AgentBrain',
    
    # Memory systems
    'WorkingMemory', 'EpisodicMemory', 'SemanticMemory',
    'ProceduralMemory', 'AssociativeMemory', 'Memory',
    
    # Plasticity
    'STDP', 'Hebbian', 'StructuralPlasticity', 'PlasticityRule',
    'RewardModulatedSTDP', 'TripletSTDP',
    
    # Neuromodulation
    'Neuromodulator', 'Dopamine', 'Acetylcholine', 'Norepinephrine',
    'Serotonin', 'Curiosity', 'Novelty', 'PredictionError',
    
    # Prediction
    'NeuralPrediction', 'ActionConsequencePredictor', 'PredictionErrorSignal',
    
    # Cognition
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    
    # Development
    'DevelopmentSystem',
    
    # Sensory and Motor
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    'Action', 'MotorSystem',
    
    # World
    'SimpleWorld', 'WorldObject', 'ActionResult',
    
    # Core
    'Config',
    
    # Configuration Management
    'ConfigValidator', 'ConfigManager', 'ConfigTemplate', 'ConfigHistory',
    
    # Systems
    'SimulationClock', 'RandomGenerator', 'Logger',
    'SpikeSystem',
    
    # Advanced
    'ModelLoader', 'ModelExporter', 'RealTimeController',
    'ParameterTuner', 'CallbackManager', 'PerformanceProfiler',
    'Visualizer', 'Debugger', 'AnalysisTools',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain', 'createConfigValidator', 'createConfigManager',
    'createConfigTemplate', 'createConfigHistory',
    'loadConfig', 'saveConfig', 'validateConfig', 'createConfigFromTemplate',
    'compareConfigurations',
]

# Version information
def get_version():
    """Return the version of the NLM Python bindings."""
    return __version__

def get_info():
    """Return comprehensive information about the NLM Python bindings."""
    return {
        'version': __version__,
        'author': __author__,
        'license': __license__,
        'description': __doc__,
        'modules': {
            'core': ['NeuronId', 'Config'],
            'brain': ['Brain', 'AgentBrain', 'Neuron'],
            'memory': ['WorkingMemory', 'EpisodicMemory'],
            'plasticity': ['STDP', 'Hebbian'],
            'neuromodulation': ['Dopamine', 'Curiosity'],
            'prediction': ['NeuralPrediction'],
            'cognition': ['NeuralPlanner'],
        'development': ['DevelopmentSystem'],
        'sensory': ['Vision', 'Audio'],
        'motor': ['Action'],
        'world': ['SimpleWorld'],
        'configuration': ['ConfigValidator', 'ConfigManager', 'ConfigTemplate', 'ConfigHistory'],
    }
    }

# Module initialization
import sys
if sys.version_info < (3, 8):
    raise ImportError("NLM Python bindings require Python 3.8 or higher")

# Internal imports that will be dynamically populated by C++ bindings
# These will be filled in when the pybind11 module is loaded
brain = None
memory = None
plasticity = None
neuromodulation = None
prediction = None
cognition = None
development = None
sensory = None
motor = None
world = None

# Global registry for model instances
_model_registry = {}

# Global event system
_event_callbacks = {}

# Performance monitoring
_performance_data = {
    'simulation_time': [],
    'neuron_count': [],
    'firing_rate': [],
    'memory_usage': [],
    'plasticity_events': [],
}

def clear_event_callbacks():
    """Clear all registered event callbacks."""
    global _event_callbacks
    _event_callbacks = {}

# Performance monitoring functions

def createPerformanceMonitor(config):
    """Create a performance monitor instance."""
    from .performance import PerformanceMonitor
    return PerformanceMonitor(config)

def createProfiler():
    """Create a profiler instance."""
    from .performance import Profiler
    return Profiler()

def createBenchmarkSuite():
    """Create a benchmark suite instance."""
    from .performance import Benchmark
    return Benchmark()

def createAnalyzer():
    """Create an analyzer instance."""
    from .performance import Analyzer
    return Analyzer()

def createPerformanceDashboard():
    """Create a performance dashboard instance."""
    from .visualization import PerformanceDashboard
    return PerformanceDashboard()

# Performance monitoring utilities

def startPerformanceMonitoring(brain, samplingInterval=100):
    """Start performance monitoring for a brain instance."""
    if hasattr(brain, '_performanceMonitor'):
        return brain._performanceMonitor.startMonitoring()
    return False

def stopPerformanceMonitoring(brain):
    """Stop performance monitoring for a brain instance."""
    if hasattr(brain, '_performanceMonitor'):
        return brain._performanceMonitor.stopMonitoring()
    return False

def startProfiling(brain, profilingInterval=1000):
    """Start profiling for a brain instance."""
    if hasattr(brain, '_profiler'):
        return brain._profiler.startProfiling()
    return False

def stopProfiling(brain):
    """Stop profiling for a brain instance."""
    if hasattr(brain, '_profiler'):
        return brain._profiler.stopProfiling()
    return False

def getPerformanceMetrics(brain):
    """Get current performance metrics for a brain instance."""
    if hasattr(brain, '_performanceMonitor'):
        return brain._performanceMonitor.getCurrentMetrics()
    return None

def exportPerformanceData(brain, filename):
    """Export performance data for a brain instance."""
    if hasattr(brain, '_performanceMonitor'):
        return brain._performanceMonitor.exportMetrics(filename)
    return False

def runBenchmark(brain, benchmarkName):
    """Run a benchmark on a brain instance."""
    if hasattr(brain, '_benchmark'):
        return brain._benchmark.runBenchmark(benchmarkName)
    return None

def analyzePerformance(brain, analysisName):
    """Analyze performance for a brain instance."""
    if hasattr(brain, '_analyzer'):
        return brain._analyzer.analyze(analysisName)
    return None

def createPerformanceReport(brain, reportType, filename):
    """Create a performance report for a brain instance."""
    if hasattr(brain, '_dashboard'):
        return brain._dashboard.generateReport(reportType, filename)
    return False

# Performance monitoring decorators

def monitor_performance(samplingInterval=100):
    """Decorator to add performance monitoring to a function."""
    def decorator(func):
        def wrapper(*args, **kwargs):
            # Start monitoring
            if args and hasattr(args[0], '_performanceMonitor'):
                args[0]._performanceMonitor.startMonitoring()
            
            # Record start time
            import time
            start_time = time.time()
            
            try:
                # Execute function
                result = func(*args, **kwargs)
                
                # Record metrics
                if args and hasattr(args[0], '_performanceMonitor'):
                    args[0]._performanceMonitor.collectMetrics()
                
                return result
            except Exception as e:
                # Record error
                if args and hasattr(args[0], '_performanceMonitor'):
                    args[0]._performanceMonitor.collectMetrics()
                raise e
            finally:
                # Stop monitoring
                if args and hasattr(args[0], '_performanceMonitor'):
                    args[0]._performanceMonitor.stopMonitoring()
        
        return wrapper
    return decorator

def profile_function(profilingInterval=1000):
    """Decorator to add profiling to a function."""
    def decorator(func):
        def wrapper(*args, **kwargs):
            # Start profiling
            if args and hasattr(args[0], '_profiler'):
                args[0]._profiler.startProfiling()
            
            try:
                # Execute function with profiling
                return func(*args, **kwargs)
            finally:
                # Stop profiling
                if args and hasattr(args[0], '_profiler'):
                    args[0]._profiler.stopProfiling()
        
        return wrapper
    return decorator

# Performance analysis functions

def analyzePerformanceTrends(brain, metrics):
    """Analyze performance trends for a brain instance."""
    if hasattr(brain, '_analyzer'):
        return brain._analyzer.analyzePerformanceTrends(metrics)
    return None

def detectPerformanceBottlenecks(brain):
    """Detect performance bottlenecks for a brain instance."""
    if hasattr(brain, '_analyzer'):
        return brain._analyzer.detectPerformanceBottlenecks()
    return None

def getPerformanceRecommendations(brain):
    """Get performance recommendations for a brain instance."""
    if hasattr(brain, '_analyzer'):
        return brain._analyzer.getPerformanceRecommendations()
    return None

# Performance dashboard functions

def createDashboard(brain, title="Performance Dashboard"):
    """Create a performance dashboard for a brain instance."""
    if hasattr(brain, '_dashboard'):
        dashboard = brain._dashboard
        dashboard.createDashboard(title)
        return dashboard
    return None

def updateDashboard(brain, data):
    """Update performance dashboard for a brain instance."""
    if hasattr(brain, '_dashboard'):
        brain._dashboard.updateDashboard(data)
        return True
    return False

def exportDashboard(brain, filename):
    """Export performance dashboard for a brain instance."""
    if hasattr(brain, '_dashboard'):
        return brain._dashboard.exportDashboard(filename)
    return False

# Example and test functions
def run_example_simulation(steps=10):
    """Run a simple example simulation for demonstration."""
    import time
    
    try:
        # Create configuration
        config = createDefaultConfig()
        
        # Create brain
        brain = createBrain(config)
        
        # Initialize
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        for step in range(steps):
            brain.step(step, step * 0.1)
        
        end_time = time.time()
        
        # Log results
        print(f"Example simulation completed in {steps} steps")
        print(f"Total simulation time: {end_time - start_time:.3f}s")
        print(f"Total neurons: {brain.getTotalNeuronCount()}")
        print(f"Total synapses: {brain.getTotalSynapseCount()}")
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        
        return brain
        
    except Exception as e:
        print(f"Example simulation failed: {e}")
        raise

# Context manager for simulation control
class SimulationSession:
    """Context manager for controlled simulation sessions."""
    
    def __init__(self, config=None, auto_save=True):
        self.config = config
        self.auto_save = auto_save
        self.brain = None
        self.session_id = None
    
    def __enter__(self):
        self.session_id = len(_model_registry)
        if self.config is None:
            self.config = createDefaultConfig()
        
        self.brain = createBrain(self.config)
        self.brain.initialize()
        
        # Register in global registry
        _model_registry[self.session_id] = {
            'brain': self.brain,
            'config': self.config,
            'start_time': None,
            'type': 'simulation_session'
        }
        
        return self
    
    def run_step(self, step):
        """Run a single simulation step."""
        if self.brain:
            self.brain.step(step, step * 0.1)
        return self.brain
    
    def run_steps(self, num_steps):
        """Run multiple simulation steps."""
        for step in range(num_steps):
            self.run_step(step)
    
    def save_state(self, filepath):
        """Save brain state to file."""
        if self.brain:
            return self.brain.save(filepath)
        return False
    
    def load_state(self, filepath):
        """Load brain state from file."""
        if self.brain:
            return self.brain.load(filepath)
        return False
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Save state if auto-save is enabled
        if self.auto_save and self.brain and self.session_id:
            filename = f"simulation_session_{self.session_id}_checkpoint.nlm"
            self.save_state(filename)
        
        # Clean up
        if self.session_id and self.session_id in _model_registry:
            del _model_registry[self.session_id]
        
        self.brain = None
        self.session_id = None

# Module-level convenience functions
def list_models():
    """List all registered model instances."""
    return list(_model_registry.keys())

def get_model(model_id):
    """Get a registered model instance."""
    return _model_registry.get(model_id)

def register_model(name, obj):
    """Register a model instance for tracking."""
    model_id = len(_model_registry)
    _model_registry[model_id] = {
        'name': name,
        'object': obj,
        'registered_at': time.time(),
        'type': type(obj).__name__
    }
    return model_id

# Configuration system factory functions
def createConfigValidator():
    """Create a configuration validator instance."""
    try:
        from .config import ConfigValidator
        return ConfigValidator()
    except ImportError:
        print("Warning: ConfigValidator not available")
        return None

def createConfigManager():
    """Create a configuration manager instance."""
    try:
        from .config import ConfigManager
        return ConfigManager()
    except ImportError:
        print("Warning: ConfigManager not available")
        return None

def createConfigTemplate(name):
    """Create a configuration template instance."""
    try:
        from .config import ConfigTemplate
        return ConfigTemplate(name)
    except ImportError:
        print("Warning: ConfigTemplate not available")
        return None

def createConfigHistory():
    """Create a configuration history instance."""
    try:
        from .config import ConfigHistory
        return ConfigHistory()
    except ImportError:
        print("Warning: ConfigHistory not available")
        return None

# Convenience functions for configuration management
def loadConfig(filepath):
    """Load configuration from file."""
    try:
        from .config import Config
        config = Config()
        if config.loadFromFile(filepath):
            return config
        else:
            print(f"Warning: Failed to load configuration from {filepath}")
            return None
    except ImportError:
        print("Warning: Config not available")
        return None

def saveConfig(filepath, config):
    """Save configuration to file."""
    if not config:
        return False
    try:
        return config.saveToFile(filepath)
    except Exception as e:
        print(f"Warning: Failed to save configuration: {e}")
        return False

def validateConfig(config):
    """Validate configuration."""
    if not config:
        return False
    try:
        validator = createConfigValidator()
        if validator:
            return validator.isValid(config)
        return False
    except Exception as e:
        print(f"Warning: Failed to validate configuration: {e}")
        return False

def createConfigFromTemplate(template_name):
    """Create configuration from template."""
    try:
        from .config import ConfigTemplate
        template = ConfigTemplate.createCustomTemplate(template_name)
        if template:
            return template.createConfig()
        return None
    except ImportError:
        print("Warning: ConfigTemplate not available")
        return None

def compareConfigurations(config1, config2):
    """Compare two configurations."""
    try:
        from .config import ConfigValidator
        validator = ConfigValidator()
        return validator.compareConfigurations(config1, config2)
    except ImportError:
        print("Warning: ConfigValidator not available")
        return False

# Initialize module
print(f"NLM Python bindings version {__version__} loaded successfully")
