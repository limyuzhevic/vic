# NLM Python Module
# Neural Learning Machine - Python Interface

__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Import the main bindings
from pynlm import (
    # Basic types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    ActionType,
    MotorCommand,
    WorldObjectType,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    AgentBody,
    SensoryPercept,
    WorldObject,
    ActionResult,
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Import enhanced bindings if available
try:
    from pynlm_enhanced import (
        # Enhanced simulation wrapper
        create_brain_simulation,
        create_agent_world_simulation,
        run_complete_simulation,
        # High-level APIs
        PyBrainSimulation,
    )
    ENHANCED_AVAILABLE = True
except ImportError:
    ENHANCED_AVAILABLE = False

# Re-export enhanced bindings
if ENHANCED_AVAILABLE:
    create_brain_simulation = create_brain_simulation
    create_agent_world_simulation = create_agent_world_simulation
    run_complete_simulation = run_complete_simulation
    PyBrainSimulation = PyBrainSimulation

# Version and info functions
def get_version():
    """Get the NLM Python bindings version."""
    return __version__

def get_info():
    """Get information about NLM bindings."""
    info = {
        "version": __version__,
        "author": __author__,
        "license": __license__,
        "enhanced_available": ENHANCED_AVAILABLE,
        "description": "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    }
    return info

def create_demo_brain():
    """Create a demo brain with sensible defaults for testing."""
    config = createDefaultConfig()
    # Set reasonable defaults for development
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    config.set("connection_probability", 0.15)
    
    brain = createBrain(config)
    brain.initialize()
    return brain

def run_demo(steps: int = 100):
    """Run a complete demo simulation."""
    brain = create_demo_brain()
    
    for step in range(steps):
        brain.step(step, step * 0.001)
    
    # Generate demo statistics
    demo_stats = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "active_neurons": brain.getActiveNeuronCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "exc_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "development_stage": brain.getDevelopmentalStage(),
        "simulation_steps": steps,
    }
    
    return demo_stats
# Initialize module-level logger
class Logger:
    """Simple logger for NLM Python module."""
    
    def __init__(self, level="INFO"):
        self.level = level
    
    def log(self, message, level="INFO"):
        """Log a message."""
        if level == "ERROR" or (level == "INFO" and self.level == "INFO"):
            print(f"[NLM] {level}: {message}")
    
    def info(self, message):
        """Log an info message."""
        self.log(message, "INFO")
    
    def error(self, message):
        """Log an error message."""
        self.log(message, "ERROR")

# Create default logger
logger = Logger()

# Module documentation
__doc__ = """
NLM (Neural Learning Machine) Python Bindings
=============================================

This module provides Python interface to the NLM neural simulation framework.
It includes both basic and enhanced bindings with different feature levels.

Basic Bindings:
---------------
The basic bindings provide access to core NLM classes including:
- Brain: Central neural simulation controller
- Config: Configuration management
- AgentBrain: Agent-brain interface
- SimpleWorld: Simulation environment
- SensoryInput: Sensory processing classes
- Action: Motor action representation
- Memory systems (working, episodic)
- Neuromodulation systems (dopamine, curiosity)

Enhanced Bindings:
------------------
When available, enhanced bindings provide:
- High-level simulation wrapper (PyBrainSimulation)
- Integrated memory and prediction systems
- Learning experiment automation
- Enhanced statistics and reporting
- Agent-world simulation helpers
- Save/load capabilities

Usage Examples:
---------------

Basic usage:
    import nlm
    brain = nlm.create_demo_brain()
    for step in range(100):
        brain.step(step, step * 0.001)

Enhanced usage:
    import nlm
    if nlm.ENHANCED_AVAILABLE:
        from nlm import PyBrainSimulation
        sim = PyBrainSimulation(steps=100)
        sim.run()
        stats = sim.getStats()

Memory statistics:
    memory_stats = sim.getMemoryStats()
    episodic_count = memory_stats.get('episodic_memory_count', 0)

Learning experiments:
    learning_results = sim.runLearningExperiment()
    learning_factor = learning_results.get('learning_factor', 0.0)

    
See the CLI tool for command-line usage: python -m nlm.main
"""
