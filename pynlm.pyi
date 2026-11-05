"""
Type hints for pynlm (NLM Python bindings)

This module provides type hints for the NLM Python bindings.
It should be used together with the actual pynlm module.

Example usage with type hints:
    from pynlm import Brain, Config, Vision, Audio
    import numpy as np
    
    config = Config()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    
    brain = Brain(config)
    brain.initialize()
    
    # Create sensory input with NumPy arrays
    image_data = np.random.rand(224, 224, 3).astype(np.float32)
    vision = Vision(224, 224, 3)
    vision.setData(image_data)
    
    # Run simulation
    for i in range(100):
        brain.step()
    
    metrics = brain.getBenchmarkMetrics()
    print(f"Firing rate: {metrics.avgFiringRate}")
"""

from typing import List, Dict, Optional, Union, Any
import sys

if sys.version_info >= (3, 8):
    from typing import TypedDict
else:
    from typing_extensions import TypedDict

# Forward references for classes that need to be referenced before definition
class Config:
    pass

class Brain:
    pass

class Vision:
    pass

class Audio:
    pass

class SensoryInput:
    pass

class Action:
    pass

class AgentBrain:
    pass

class AgentBody:
    pass

class SimpleWorld:
    pass

class SensoryPercept:
    pass

# Enum types
class NeuronType:
    Excitatory: int = 0
    Inhibitory: int = 1
    Modulatory: int = 2
    Sensory: int = 3
    Motor: int = 4
    Internal: int = 5

class SynapseType:
    Excitatory: int = 0
    Inhibitory: int = 1
    Modulatory: int = 2
    Electrical: int = 3
    GapJunction: int = 4

class DevelopmentalStage:
    Initial: int = 0
    CriticalPeriod: int = 1
    Maturation: int = 2
    Adult: int = 3
    Aging: int = 4

class FiringState:
    Resting: int = 0
    Active: int = 1
    Refractory: int = 2
    Inhibited: int = 3

class ActionType:
    MoveForward: int = 0
    MoveBackward: int = 1
    MoveLeft: int = 2
    MoveRight: int = 3
    TurnLeft: int = 4
    TurnRight: int = 5
    Look: int = 6
    LookUp: int = 7
    LookDown: int = 8
    Interact: int = 9
    Eat: int = 10
    Drink: int = 11
    Rest: int = 12
    Wait: int = 13
    Custom: int = 14

class MotorCommand:
    MoveForward: int = 0
    MoveBackward: int = 1
    TurnLeft: int = 2
    TurnRight: int = 3
    LookLeft: int = 4
    LookRight: int = 5
    Interact: int = 6
    Wait: int = 7

class WorldObjectType:
    Empty: int = 0
    Resource: int = 1
    Hazard: int = 2
    Wall: int = 3
    Marker: int = 4

class LogLevel:
    Off: int = 0
    Error: int = 1
    Warn: int = 2
    Info: int = 3
    Debug: int = 4
    Trace: int = 5

# Configuration types
class ConfigSource:
    Default: int = 0
    File: int = 1
    CommandLine: int = 2
    Runtime: int = 3

class ConfigEntry(TypedDict):
    key: str
    value: Union[int, float, bool, str, List[Any], None]
    source: int
    description: str

# TypedDict for various configuration structures
class BenchmarkConfig(TypedDict, total=False):
    name: str
    scales: List[Dict[str, Any]]
    seeds: List[int]
    stepsPerScale: int
    warmupSteps: int
    runUntilStable: bool
    stabilityThreshold: float
    maxStabilityChecks: int
    enableMultithreading: bool
    enableSIMD: bool
    enableSparseConnectivity: bool
    enableEventDriven: bool
    threadCount: int
    outputDir: str
    generatePlots: bool
    generateReport: bool

class ScaleLevel(TypedDict):
    name: str
    neurons: int
    connectionProbability: float

class ScaleAnalysis(TypedDict, total=False):
    benchmarkRuns: List[Dict[str, Any]]
    projectedPerformance10x: float
    projectedPerformance100x: float

# Metrics types
class BenchmarkMetrics(TypedDict, total=False):
    avgFiringRate: float
    synapticWeightMean: float
    connectionDensity: float
    totalSteps: int
    simulationTime: float
    learningEfficiency: float
    memoryRetention: float
    predictionAccuracy: float
    explorationRate: float
    goalDirectedness: float
    adaptationSpeed: float

# Sensory input types
class SensoryInputData(TypedDict):
    type: str
    data: List[float]
    dimensions: int
    timestamp: float
    metadata: Dict[str, Any]

# Action types
class ActionParameters(TypedDict, total=False):
    force: float
    duration: float
    velocity: float
    angle: float
    target: Optional[Union[int, float, List[float]]]
    properties: Dict[str, Any]

class ActionResult(TypedDict, total=False):
    success: bool
    reward: float
    executionTime: float
    description: str

# Agent types
class Percept(TypedDict):
    type: str
    data: Dict[str, Any]
    confidence: float
    timestamp: float

class AgentState(TypedDict, total=False):
    position: List[float]
    orientation: List[float]
    health: float
    energy: float
    goals: List[str]
    beliefs: Dict[str, float]
    desires: List[str]
    capabilities: List[str]

# Environment types
class WorldState(TypedDict, total=False):
    time: float
    objects: List[Dict[str, Any]]
    agents: List[Dict[str, Any]]
    hazards: List[Dict[str, Any]]
    resources: List[Dict[str, Any]]

class WorldObject(TypedDict):
    id: int
    type: int
    position: List[float]
    size: List[float]
    properties: Dict[str, Any]
    active: bool

# Main module exports
__all__ = [
    # Core classes
    "Config",
    "Brain", 
    "Vision",
    "Audio",
    "SensoryInput",
    "Action",
    "AgentBrain",
    "AgentBody",
    "SimpleWorld",
    "SensoryPercept",
    
    # Enums
    "NeuronType",
    "SynapseType", 
    "DevelopmentalStage",
    "FiringState",
    "ActionType",
    "MotorCommand",
    "WorldObjectType",
    "LogLevel",
    "ConfigSource",
    
    # Types
    "ConfigEntry",
    "BenchmarkConfig",
    "ScaleLevel",
    "ScaleAnalysis",
    "BenchmarkMetrics",
    "SensoryInputData",
    "ActionParameters",
    "ActionResult",
    "Percept",
    "AgentState",
    "WorldState",
    "WorldObject",
    
    # Constants
    "TypedDict",
]
