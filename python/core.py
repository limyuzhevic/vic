"""
Python core module for NLM bindings

This module provides the main Python interfaces to the NLM C++ framework.
It includes the core classes and factory functions for creating and managing
neural simulations.
"""

import sys
from typing import Dict, List, Optional, Union, Any, Callable

# Import from C++ bindings
try:
    from .bindings import (
        Config,
        Brain,
        AgentBrain,
        SimpleWorld,
        AgentBody,
        SensoryPercept,
        Vision,
        Audio,
        InternalSignals,
        Action,
        WorldObject,
        ActionResult,
        createDefaultConfig,
        createBrain,
        createAgentBrain,
        createSimpleWorld,
        # Enumerations
        NeuronType,
        SynapseType,
        DevelopmentalStage,
        FiringState,
        ActionType,
        MotorCommand,
        WorldObjectType,
        # Identifiers
        NeuronId,
        SynapseId,
        RegionId,
        PopulationId,
        # Constants
        INVALID_NEURON_ID,
        INVALID_SYNAPSE_ID,
        INVALID_REGION_ID,
        INVALID_POPULATION_ID,
    )
except ImportError:
    # Fallback: create stub classes if C++ bindings not available
    # This allows the module to be imported without compilation
    import warnings
    warnings.warn(
        "C++ bindings not available. Creating stub classes.",
        ImportWarning,
        stacklevel=2,
    )
    
    # Create stub classes for development and testing
    class Config:
        """Stub Config class for development"""
        def __init__(self):
            self._data = {}
            
        def loadFromFile(self, filepath: str) -> bool:
            """Load configuration from JSON file"""
            print(f"[STUB] Loading config from {filepath}")
            return True
            
        def loadFromArgs(self, argc: int, argv: List[str]) -> bool:
            """Load configuration from command line arguments"""
            print(f"[STUB] Loading config from args")
            return True
            
        def saveToFile(self, filepath: str) -> bool:
            """Save configuration to JSON file"""
            print(f"[STUB] Saving config to {filepath}")
            return True
            
        def has(self, key: str) -> bool:
            """Check if configuration key exists"""
            return key in self._data
            
        def getKeys(self) -> List[str]:
            """Get all configuration keys"""
            return list(self._data.keys())
            
        def clear(self) -> None:
            """Clear all configuration entries"""
            self._data.clear()
            
        def summary(self) -> str:
            """Get configuration summary"""
            return f"Config with {len(self._data)} entries"
            
        def __repr__(self) -> str:
            return f"<Config: {self.summary()}>"
    
    class Brain:
        """Stub Brain class for development"""
        def __init__(self, config):
            self.config = config
            
        def initialize(self) -> None:
            """Initialize the brain"""
            print("[STUB] Brain initialized")
            
        def step(self, step: int, time: Optional[float] = None) -> None:
            """Perform a simulation step"""
            print(f"[STUB] Brain step {step}")
            
        def receiveSensoryInput(self, input_data) -> None:
            """Receive sensory input"""
            print("[STUB] Brain received sensory input")
            
        def getTotalNeuronCount(self) -> int:
            """Get total neuron count"""
            return 1000
            
        def getTotalSynapseCount(self) -> int:
            """Get total synapse count"""
            return 10000
            
        def getFiringNeuronCount(self) -> int:
            """Get firing neuron count"""
            return 50
            
        def getTotalSpikeCount(self) -> int:
            """Get total spike count"""
            return 150
            
        def getAverageFiringRate(self) -> float:
            """Get average firing rate"""
            return 2.5
            
        def getExcitationInhibitionRatio(self) -> float:
            """Get E/I ratio"""
            return 1.5
            
        def save(self, filepath: str) -> bool:
            """Save brain state"""
            print(f"[STUB] Saving brain to {filepath}")
            return True
            
        def load(self, filepath: str) -> bool:
            """Load brain state"""
            print(f"[STUB] Loading brain from {filepath}")
            return True
            
        def reset(self) -> None:
            """Reset brain state"""
            print("[STUB] Brain reset")
            
        def logStatus(self) -> None:
            """Log brain status"""
            print("[STUB] Brain status logged")
    
    class AgentBrain:
        """Stub AgentBrain class for development"""
        def __init__(self, brain):
            self.brain = brain
            
        def initialize(self, world) -> None:
            """Initialize with world"""
            print("[STUB] AgentBrain initialized with world")
            
        def processSensoryInput(self, percept) -> None:
            """Process sensory input"""
            print("[STUB] AgentBrain processed sensory input")
            
        def decodeMotorCommand(self) -> Any:
            """Decode motor command"""
            print("[STUB] AgentBrain decoded motor command")
            return None
            
        def getCuriosityLevel(self) -> float:
            """Get curiosity level"""
            return 0.5
            
        def getNoveltyLevel(self) -> float:
            """Get novelty level"""
            return 0.3
            
        def enableRewardModulation(self, enable: bool) -> None:
            """Enable/disable reward modulation"""
            print(f"[STUB] Reward modulation {'enabled' if enable else 'disabled'}")
            
        def enableStructuralPlasticity(self, enable: bool) -> None:
            """Enable/disable structural plasticity"""
            print(f"[STUB] Structural plasticity {'enabled' if enable else 'disabled'}")
            
        def enableDevelopment(self, enable: bool) -> None:
            """Enable/disable development"""
            print(f"[STUB] Development {'enabled' if enable else 'disabled'}")
            
        def enableCuriosity(self, enable: bool) -> None:
            """Enable/disable curiosity"""
            print(f"[STUB] Curiosity {'enabled' if enable else 'disabled'}")
    
    class SimpleWorld:
        """Stub SimpleWorld class for development"""
        def __init__(self):
            self.width = 0.0
            self.height = 0.0
            
        def configure(self, width: float, height: float, 
                     visionWidth: int, visionHeight: int) -> None:
            """Configure world dimensions"""
            self.width = width
            self.height = height
            print(f"[STUB] World configured: {width}x{height}, vision: {visionWidth}x{visionHeight}")
            
        def reset(self) -> None:
            """Reset world"""
            print("[STUB] World reset")
            
        def update(self, timestep: float) -> None:
            """Update world"""
            print(f"[STUB] World updated with timestep {timestep}")
            
        def getSensoryPercept(self):
            """Get sensory percept"""
            print("[STUB] Getting sensory percept")
            return None
            
        def getAgentBody(self):
            """Get agent body"""
            print("[STUB] Getting agent body")
            return None
            
        def applyMotorCommand(self, cmd, currentTime: float) -> Any:
            """Apply motor command"""
            print(f"[STUB] Applied motor command at time {currentTime}")
            return None
    
    class AgentBody:
        """Stub AgentBody class for development"""
        def __init__(self):
            pass
            
        def reset(self) -> None:
            """Reset body state"""
            print("[STUB] AgentBody reset")
    
    class SensoryPercept:
        """Stub SensoryPercept class for development"""
        def __init__(self):
            pass
            
    class Vision:
        """Stub Vision class for development"""
        def __init__(self, width: int = 0, height: int = 0, channels: int = 0):
            pass
            
        def setData(self, data: List[float]) -> None:
            """Set vision data"""
            pass
    
    class Audio:
        """Stub Audio class for development"""
        def __init__(self, sampleRate: int = 0, numSamples: int = 0):
            pass
            
        def setData(self, data: List[float]) -> None:
            """Set audio data"""
            pass
    
    class InternalSignals:
        """Stub InternalSignals class for development"""
        def __init__(self):
            pass
            
        def addSignal(self, value: float) -> None:
            """Add internal signal"""
            pass
            
        def clearSignals(self) -> None:
            """Clear internal signals"""
            pass
    
    class Action:
        """Stub Action class for development"""
        def __init__(self, action_type: Any = None, parameters: Optional[List[float]] = None):
            pass
            
        def getType(self) -> Any:
            """Get action type"""
            return None
            
        def getParameters(self) -> List[float]:
            """Get action parameters"""
            return []
            
        def getName(self) -> str:
            """Get action name"""
            return "Action"
            
        def clone(self) -> Any:
            """Clone action"""
            return None
    
    class WorldObject:
        """Stub WorldObject class for development"""
        def __init__(self, x: float = 0.0, y: float = 0.0, 
                    obj_type: Any = None, value: float = 0.0, 
                    radius: float = 0.5):
            pass
    
    class ActionResult:
        """Stub ActionResult class for development"""
        def __init__(self, reward: float = 0.0, success: bool = False, 
                    message: str = ""):
            pass
    
    # Stub factory functions
    def createDefaultConfig():
        """Create default configuration"""
        return Config()
        
    def createBrain(config):
        """Create brain with configuration"""
        return Brain(config)
        
    def createAgentBrain(brain):
        """Create agent brain interface"""
        return AgentBrain(brain)
        
    def createSimpleWorld():
        """Create simple world"""
        return SimpleWorld()

# Factory functions for convenience
def run_agent_simulation(num_steps: int = 1000, **kwargs) -> None:
    """
    Convenience function to run a complete agent simulation.
    
    This function provides a high-level interface for running simulations
    with commonly used configurations and patterns.
    
    Args:
        num_steps: Number of simulation steps to run
        **kwargs: Additional configuration options
        
    Example:
        >>> pynlm.run_agent_simulation(1000)
        >>> pynlm.run_agent_simulation(500, enable_learning=True, curiosity=True)
    """
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning subsystems if requested
    if kwargs.get('enable_learning', False):
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
    print(f"Simulation complete: {num_steps} steps")

def get_system_info() -> Dict[str, Any]:
    """
    Get information about the NLM system configuration.
    
    Returns:
        Dictionary containing system information
    """
    return {
        'version': __version__,
        'description': 'NLM (Neural Learning Machine) Python Bindings',
        'features': [
            'Brain simulation',
            'World interaction',
            'Memory systems',
            'Neuromodulation',
            'Prediction systems',
            'Development',
            'Cognition',
            'Plasticity',
        ],
        'file': __file__,
    }