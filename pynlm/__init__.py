"""
NLM (Neural Learning Machine) Python Bindings

Advanced Python bindings for the NLM C++ neural simulation framework.

This package provides:
- Core NLM functionality (brain simulation, world modeling, agent interfaces)
- Advanced Python bindings for expert users
- Performance monitoring and optimization tools
- Neural network analysis utilities
- Configuration management with JSON/YAML support
- Real-time monitoring and visualization tools

The NLM framework implements an experimental artificial developmental brain
with real spiking neural computation, plasticity mechanisms, and memory systems.

Quick Start:
------------
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete agent simulation
performance_data = run_advanced_simulation(brain, world, agent, 1000, enable_profiling=True)

Expert Features:
---------------
- Advanced neural network analysis
- Performance monitoring and optimization
- Parallel simulation execution
- Custom configuration management
- Real-time analysis and monitoring
- Machine learning integration
- Advanced visualization tools

For more information:
- Read documentation in the docs/ directory
- Check examples in the examples/ directory
- Visit the NLM project website for documentation
"""

__version__ = "2.0.0-advanced"
__author__ = "NLM Research Team"
__license__ = "MIT"
__description__ = "Advanced Python bindings for NLM neural simulation framework"

# Import core functionality
from pynlm.bindings import (
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    createDefaultConfig,
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    ActionType,
    MotorCommand,
    WorldObjectType,
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept
)

# Import expert utilities
from pynlm.expert_utils import (
    PerformanceOptimizer,
    MemoryManager,
    PerformanceMonitor,
    ExpertUtils
)

# Import analysis utilities
from pynlm.analysis import (
    NeuralNetworkAnalyzer,
    RealTimeAnalyzer,
    ConfigurationManager
)

# Import visualization utilities
from pynlm.visualization import (
    NeuralVisualizer,
    ActivityPlotter,
    NetworkGrapher
)

# Import advanced simulation utilities
from pynlm.parallel import (
    ParallelSimulationManager,
    SimulationInstance
)

# Import performance utilities
from pynlm.performance import (
    PerformanceProfiler,
    MemoryProfiler,
    SpeedProfiler
)

# Import monitoring utilities
from pynlm.monitoring import (
    RealTimeMonitor,
    MetricsCollector,
    AlertManager
)

# Initialize package
import sys
import os

# Add expert utilities to path if available
expert_path = os.path.join(os.path.dirname(__file__), 'expert_utils')
if os.path.exists(expert_path):
    sys.path.insert(0, expert_path)

# Initialize analysis module
import pynlm.analysis
import pynlm.visualization
import pynlm.parallel
import pynlm.performance
import pynlm.monitoring

# Package metadata
metadata = {
    'version': __version__,
    'author': __author__,
    'license': __license__,
    'description': __description__,
    'features': [
        'core_neural_simulation',
        'advanced_bindings',
        'performance_monitoring',
        'neural_analysis',
        'visualization',
        'parallel_simulation',
        'configuration_management',
        'real_time_monitoring',
        'machine_learning_integration'
    ],
    'dependencies': [
        'numpy>=1.20',
        'scipy>=1.7',
        'matplotlib>=3.3',
        'scikit-learn>=0.24',
        'pandas>=1.2',
        'pyyaml>=5.4',
        'json5>=0.9'
    ]
}

# Welcome message for first-time users
def welcome_new_user():
    if not os.path.exists('.nlm_welcome'):
        print("Welcome to NLM Advanced Python Bindings!")
        print("For getting started, try:")
        print("  import pynlm")
        print("  brain = pynlm.createBrain(pynlm.createDefaultConfig())")
        print("  brain.initialize()")
        print("  for step in range(100):")
        print("      brain.step(step)")
        print("")
        print("For advanced features:")
        print("  pynlm.run_advanced_simulation()")
        print("  pynlm.PerformanceOptimizer.optimize_neural_network_access(brain)")
        print("  pynlm.NeuralNetworkAnalyzer.analyze_network_topology(brain)")
        print("")
        print("Documentation available in the docs/ directory.")
        
        # Create welcome flag
        with open('.nlm_welcome', 'w') as f:
            f.write("Welcome message shown")
    
    return True

# Auto-welcome (can be disabled)
if os.environ.get('NLM_AUTO_WELCOME', 'true').lower() == 'true':
    welcome_new_user()

__all__ = [
    # Core classes
    'createBrain', 'createSimpleWorld', 'createAgentBrain', 'createDefaultConfig',
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept',
    
    # Expert utilities
    'PerformanceOptimizer', 'MemoryManager', 'PerformanceMonitor',
    'ExpertUtils',
    
    # Analysis utilities
    'NeuralNetworkAnalyzer', 'RealTimeAnalyzer', 'ConfigurationManager',
    
    # Visualization utilities
    'NeuralVisualizer', 'ActivityPlotter', 'NetworkGrapher',
    
    # Parallel simulation utilities
    'ParallelSimulationManager', 'SimulationInstance',
    
    # Performance utilities
    'PerformanceProfiler', 'MemoryProfiler', 'SpeedProfiler',
    
    # Monitoring utilities
    'RealTimeMonitor', 'MetricsCollector', 'AlertManager',
    
    # Package metadata
    '__version__', '__author__', '__license__', '__description__',
    'metadata', 'welcome_new_user'
]