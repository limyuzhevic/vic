"""
NLM (Neural Learning Machine) Python Bindings - Phase 6 Complete Integration

This module provides Python bindings for the complete NLM Phase 6 artificial brain.
It exposes all integrated systems: memory, neuromodulation, development, prediction, and cognition.

Key Features:
- Working memory with persistent neural activity
- Episodic memory for experience encoding and replay  
- Semantic memory for knowledge representation
- Neuromodulation (dopamine, curiosity, novelty, prediction error)
- Developmental system with stage progression
- Prediction system for temporal sequence learning
- Cognitive systems (attention, planning, concept formation)
- Full integration through AgentBrain/world interface
"""

import pybind11
import pybind11.stl
import pybind11.functional
import pybind11.chrono
from typing import List, Dict, Any, Optional, Union
import numpy as np

# Core NLM types
from ..core.types.types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    SimulationStep, Timestamp, TimestepDuration,
    NeuronType, SynapseType, DevelopmentalStage,
    FiringState, ActionType, MotorCommand,
    WorldObjectType, NeuronIndex, SynapseIndex,
    SynapticWeight, MembranePotential, FiringRate
)

# Core classes
from ..brain.brain import Brain
from ..core.config.config import Config
from ..agent.agent_brain import AgentBrain
from ..world.simple_world import SimpleWorld
from ..sensory.sensory_input import SensoryInput
from ..sensory.vision import Vision
from ..sensory.audio import Audio
from ..sensory.internal_signals import InternalSignals
from ..motor.action import Action
from ..motor.action_result import ActionResult
from ..agent.agent_body import AgentBody
from ..world.object import WorldObject

# Phase 6 systems
from ..memory.working_memory import WorkingMemory
from ..memory.neural_working_memory import NeuralWorkingMemory
from ..memory.neural_episodic_memory import NeuralEpisodicMemory
from ..memory.neural_associative_memory import NeuralAssociativeMemory
from ..neuromodulation.neuromodulator import Neuromodulator
from ..neuromodulation.dopamine import Dopamine
from ..neuromodulation.curiosity import Curiosity
from ..neuromodulation.prediction_error import PredictionError
from ..neuromodulation.novelty import Novelty
from ..neuromodulation.acetylcholine import Acetylcholine
from ..neuromodulation.norepinephrine import Norepinephrine
from ..neuromodulation.serotonin import Serotonin
from ..development.development_system import DevelopmentSystem
from ..prediction.prediction_system import PredictionSystem
from ..prediction.neural_prediction import NeuralPrediction
from ..cognition.neural_planner import NeuralPlanner
from ..cognition.concept_formation import ConceptFormation
from ..cognition.attentional_selection import AttentionalSelection

# Performance systems
from ..performance.memory_pool import MemoryPool
from ..performance.simd_vectorization import SIMDVectorization
from ..performance.event_queue import EventQueue
from ..performance.sparse_connectivity import SparseConnectivity
from ..performance.checkpoint_manager import CheckpointManager

# Special types for Phase 6
from ..memory.memory_types import EpisodicMemoryItem, MemoryTrace, Association

__all__ = [
    # Core
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'SimulationStep', 'Timestamp', 'TimestepDuration',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'MotorCommand',
    'WorldObjectType',
    'Brain', 'Config', 'AgentBrain', 'SimpleWorld',
    'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'ActionResult', 'AgentBody', 'WorldObject',
    
    # Phase 6 Memory Systems
    'WorkingMemory', 'NeuralWorkingMemory', 'NeuralEpisodicMemory',
    'NeuralAssociativeMemory', 'EpisodicMemoryItem', 'MemoryTrace',
    'Association',
    
    # Phase 6 Neuromodulation
    'Neuromodulator', 'Dopamine', 'Curiosity', 'PredictionError',
    'Novelty', 'Acetylcholine', 'Norepinephrine', 'Serotonin',
    
    # Phase 6 Development
    'DevelopmentSystem',
    
    # Phase 6 Prediction
    'PredictionSystem', 'NeuralPrediction',
    
    # Phase 6 Cognition
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    
    # Performance
    'MemoryPool', 'SIMDVectorization', 'EventQueue',
    'SparseConnectivity', 'CheckpointManager',
    
    # Helper functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld',
    'createAgentBrain', 'runPhase6Integration'
]
