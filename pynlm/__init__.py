#!/usr/bin/env python3

"""
NLM Python Bindings - High-level Python API for Neural Learning Machine

This package provides comprehensive Python bindings for the NLM (Neural Learning Machine) 
neural simulation framework. It offers intuitive, object-oriented interfaces to access
all NLM components including brain simulation, memory systems, neuromodulation, cognition,
and development systems.

Key Features:
- High-level Python API with fluent interface design
- Context manager support for simulation lifecycle
- NumPy integration for neural data processing
- Pandas DataFrame support for memory visualization
- Matplotlib plotting integration
- Jupyter notebook compatibility
- Real-time simulation monitoring
- Event-driven simulation callbacks
- Performance profiling tools
- Extension APIs for custom neural circuits
- Multi-processing and distributed simulation support
- Checkpoint/restore utilities
- Comprehensive error handling and validation

Usage Examples:

Basic Brain Simulation:
```python
from pynlm import Brain, Config, create_brain

# Create default configuration
config = Config()
config.set("neuron_count", 1000)
config.set("region_count", 3)

# Create and initialize brain
brain = Brain(config)
brain.initialize()

# Simulation context manager
with brain:
    # Run simulation steps
    for step in range(100):
        brain.step(step)
        action = brain.produce_action()
        # Process action...
```

Advanced Memory Operations:
```python
from pynlm import WorkingMemory, EpisodicMemory

# Working memory for active information
working_mem = WorkingMemory(brain)
working_mem.store([0.1, 0.5, 0.9], strength=0.8)
retrieved = working_mem.retrieve()

# Episodic memory for experiences
episodic_mem = EpisodicMemory(brain)
episodic_mem.record(episode_data, timestamp=time.time())

# Query and analyze memories
memories = episodic_mem.search(keywords=["exploration", "reward"])
```

NumPy Integration:
```python
import numpy as np
from pynlm import create_brain

# Neural data as NumPy arrays
neuron_activity = np.random.randn(1000, 50)  # 1000 neurons, 50 time steps
brain = create_brain(neuron_activity)

# Batch processing
batch_results = brain.process_batch(neuron_activity)

# Memory export to Pandas DataFrame
import pandas as pd
df = working_mem.export_to_dataframe()
```

Visualization and Analysis:
```python
import matplotlib.pyplot as plt
from pynlm import Visualization

# Create visualizer
visualizer = Visualization(brain)

# Plot neural activity
activity_data = brain.get_neural_activity()
visualizer.plot_neural_activity(activity_data, save_path="activity.png")

# Memory visualization
memory_viz = visualizer.memory_analysis(working_mem, episodic_mem)
memory_viz.show()
```

Real-time Monitoring:
```python
from pynlm import RealTimeMonitor

# Create monitor
monitor = RealTimeMonitor(brain)

# Register callbacks
@monitor.on_simulation_step
def on_step(step, brain):
    # Log step information
    print(f"Step {step}, Neurons active: {brain.get_active_neuron_count()}")

@monitor.on_memory_update
def on_memory_update(memory_type, data):
    # Handle memory updates
    print(f"Memory updated: {memory_type}")

# Start monitoring
monitor.start()
```

Plugin and Extension Development:
```python
from pynlm import ExtensionAPI, create_custom_circuit

# Create custom neural circuit
custom_circuit = create_custom_circuit()
custom_circuit.add_neurons(500, "cortical")
custom_circuit.add_connections("excitatory", probability=0.1)

# Register extension
ext_api = ExtensionAPI(brain)
ext_api.register_extension(custom_circuit, "custom_cortical")
```

Complete Example: Agent Simulation
```python
from pynlm import Brain, AgentBrain, SimpleWorld, create_brain

# Create simulation world
world = SimpleWorld(width=100, height=100)

# Create brain with agent interface
config = Config()
brain = Brain(config)
agent_brain = AgentBrain(brain, world)

# Initialize with world
agent_brain.initialize(world)

# Run complete agent simulation
with world:
    for episode in range(10):
        state = world.get_state()
        percept = agent_brain.process_sensory_input(state)
        
        for step in range(100):
            # Get action from brain
            action = agent_brain.decode_motor_command()
            
            # Apply action in world
            reward, success = world.apply_action(action)
            
            # Apply reward modulation
            agent_brain.apply_reward_modulation(reward, predicted_reward=0.0)
            
            # Update development
            agent_brain.update_development(0.1)
            
            if success:
                break
        
        # Reset for next episode
        agent_brain.reset()
        world.reset()
```

Documentation:
For complete documentation, API reference, and advanced examples, visit https://nlm.readthedocs.io

Module Structure:
- brain.py: Core Brain class and high-level simulation API
- memory.py: Memory system interfaces
- neuromodulation.py: Neuromodulation and affect systems
- cognition.py: Cognition and prediction systems
- development.py: Development and plasticity systems
- visualization.py: Visualization and debugging tools
- monitoring.py: Real-time monitoring and callbacks
- profiling.py: Performance profiling tools
- extensions.py: Extension APIs and plugin system
- integration.py: NumPy/Pandas/Matplotlib integration
- utils.py: Utility functions and helpers
- config.py: Configuration management
- examples/: Comprehensive example scripts
- tests/: Test suite and documentation examples

For issues, feedback, or feature requests, please visit:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Support: https://github.com/nlm-project/nlm/discussions
"""

__version__ = "0.1.0"
__author__ = "NLM Project"
__email__ = "info@nlm-project.org"
__license__ = "MIT"
__copyright__ = "Copyright 2026 NLM Project"

# Optional imports with error handling
try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    np = None
    NUMPY_AVAILABLE = False

try:
    import pandas as pd
    PANDAS_AVAILABLE = True
except ImportError:
    pd = None
    PANDAS_AVAILABLE = False

try:
    import matplotlib
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    matplotlib = None
    MATPLOTLIB_AVAILABLE = False

# Core imports - handle optional dependencies
try:
    from .brain import Brain, Config, create_brain, create_default_config
    CORE_AVAILABLE = True
except ImportError as e:
    if "numpy" in str(e).lower():
        # numpy is missing, but core classes can still be defined
        Brain = None
        Config = None
        create_brain = None
        create_default_config = None
        CORE_AVAILABLE = False
    else:
        raise

try:
    from .memory import WorkingMemory, EpisodicMemory, AssociativeMemory
    MEMORY_AVAILABLE = True
except ImportError as e:
    if "numpy" in str(e).lower():
        WorkingMemory = None
        EpisodicMemory = None
        AssociativeMemory = None
        MEMORY_AVAILABLE = False
    else:
        raise

try:
    from .neuromodulation import NeuromodulationSystem
    NEUROMODULATION_AVAILABLE = True
except ImportError as e:
    NeuromodulationSystem = None
    NEUROMODULATION_AVAILABLE = False

try:
    from .cognition import PredictionSystem, CognitionSystem
    COGNITION_AVAILABLE = True
except ImportError as e:
    PredictionSystem = None
    CognitionSystem = None
    COGNITION_AVAILABLE = False

try:
    from .development import DevelopmentSystem
    DEVELOPMENT_AVAILABLE = True
except ImportError as e:
    DevelopmentSystem = None
    DEVELOPMENT_AVAILABLE = False

try:
    from .visualization import Visualization
    VISUALIZATION_AVAILABLE = True
except ImportError as e:
    Visualization = None
    VISUALIZATION_AVAILABLE = False

try:
    from .monitoring import RealTimeMonitor
    MONITORING_AVAILABLE = True
except ImportError as e:
    RealTimeMonitor = None
    MONITORING_AVAILABLE = False

try:
    from .profiling import PerformanceProfiler
    PROFILING_AVAILABLE = True
except ImportError as e:
    PerformanceProfiler = None
    PROFILING_AVAILABLE = False

try:
    from .extensions import ExtensionAPI, create_custom_circuit
    EXTENSIONS_AVAILABLE = True
except ImportError as e:
    ExtensionAPI = None
    create_custom_circuit = None
    EXTENSIONS_AVAILABLE = False

# Integration imports (these may require additional dependencies)
try:
    from .integration.numpy_support import NumPyIntegration
    NUMPY_INTEGRATION_AVAILABLE = True
except ImportError as e:
    NumPyIntegration = None
    NUMPY_INTEGRATION_AVAILABLE = False

try:
    from .integration.pandas_support import PandasIntegration
    PANDAS_INTEGRATION_AVAILABLE = True
except ImportError as e:
    PandasIntegration = None
    PANDAS_INTEGRATION_AVAILABLE = False

try:
    from .integration.matplotlib_support import MatplotlibIntegration
    MATPLOTLIB_INTEGRATION_AVAILABLE = True
except ImportError as e:
    MatplotlibIntegration = None
    MATPLOTLIB_INTEGRATION_AVAILABLE = False

# Utility imports
try:
    from .utils.helpers import (
        simulation_loop, batch_processor, checkpoint_manager,
        performance_monitor, data_exporter, simulator_factory
    )
    UTILITIES_AVAILABLE = True
except ImportError as e:
    simulation_loop = batch_processor = checkpoint_manager = None
    performance_monitor = data_exporter = simulator_factory = None
    UTILITIES_AVAILABLE = False

# Re-export commonly used classes (only if available)
__all__ = [
    # Core classes
    'Brain', 'Config', 'create_brain', 'create_default_config',
    'WorkingMemory', 'EpisodicMemory', 'AssociativeMemory',
    'NeuromodulationSystem', 'PredictionSystem', 'CognitionSystem',
    'DevelopmentSystem',
    'Visualization', 'RealTimeMonitor', 'PerformanceProfiler',
    'ExtensionAPI', 'create_custom_circuit',
    # Integration classes
    'NumPyIntegration', 'PandasIntegration', 'MatplotlibIntegration',
    # Utilities
    'simulation_loop', 'batch_processor', 'checkpoint_manager',
    'performance_monitor', 'data_exporter', 'simulator_factory',
    # Configuration flags
    'NUMPY_AVAILABLE', 'PANDAS_AVAILABLE', 'MATPLOTLIB_AVAILABLE',
    'CORE_AVAILABLE', 'MEMORY_AVAILABLE', 'NEUROMODULATION_AVAILABLE',
    'COGNITION_AVAILABLE', 'DEVELOPMENT_AVAILABLE', 'VISUALIZATION_AVAILABLE',
    'MONITORING_AVAILABLE', 'PROFILING_AVAILABLE', 'EXTENSIONS_AVAILABLE',
    'NUMPY_INTEGRATION_AVAILABLE', 'PANDAS_INTEGRATION_AVAILABLE',
    'MATPLOTLIB_INTEGRATION_AVAILABLE', 'UTILITIES_AVAILABLE',
]

# Set up the module
def setup_module():
    """Initialize the NLM Python bindings module."""
    import sys
    import logging
    
    # Configure logging
    logging.basicConfig(level=logging.INFO)
    logger = logging.getLogger(__name__)
    
    # Verify dependencies
    try:
        import numpy
        import pandas
        import matplotlib
        logger.info("All dependencies verified")
    except ImportError as e:
        logger.warning(f"Missing optional dependency: {e}")
    
    # Initialize module components
    logger.info("NLM Python bindings module initialized")

# Run setup when module is imported
if __name__ != "__main__":
    setup_module()
