# Type hints and utilities for NLM Python bindings

"""
Core types and type definitions for NLM Python API
"""

from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass, field
import pybind11
from typing import TYPE_CHECKING

# Forward references for type hints
if TYPE_CHECKING:
    from .brain import Brain
    from .world import SimpleWorld
    from .agent import AgentBrain

# Core scalar types used throughout NLM
Timestamp = float
SimulationStep = int
MembranePotential = float
NeuronIdValue = int
SynapseIdValue = int
RegionIdValue = int
PopulationIdValue = int
Delay = int
PlasticityFlags = int
NeuralActivity = float
MemoryStrength = float
Neuromodulation = float

# Type aliases for common NLM types
NeuronId = NeuronIdValue
SynapseId = SynapseIdValue  
RegionId = RegionIdValue
PopulationId = PopulationIdValue

# Unit types for NLM physics
Nanoseconds = float  # Time units for neural dynamics
Microvolts = float   # Voltage units for neuron membrane potential
Picoseconds = float  # Synaptic delay units

# Configuration type
ConfigDict = Dict[str, Any]
NeuronParams = Dict[str, Any]
SynapseParams = Dict[str, Any]
