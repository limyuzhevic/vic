"""
Enhanced NLM Package - Complete Implementation

This package provides advanced functionality for NLM (Neural Learning Machine) users,
including enhanced Python bindings with advanced features for research and development.
"""

# Main enhanced package __init__.py
# This file provides the main API for the enhanced NLM package

import sys
from pathlib import Path

# Import all enhanced components
from .validated_config import ValidatedConfig
from .advanced_simulation_manager import AdvancedSimulationManager
from .performance_monitor import PerformanceMonitor
from .component_factory import ComponentFactory
from .analysis_utils import AnalysisUtils
from .neural_network_builder import NeuralNetworkBuilder

# Try to import original NLM for backward compatibility
try:
    import pynlm
    # Re-export original classes
    from pynlm import Config, Brain, AgentBrain, SimpleWorld, Vision, Audio, Action
    from pynlm import SensoryInput, NeuronType, SynapseType, DevelopmentalStage
    from pynlm import FiringState, ActionType, WorldObjectType, MotorCommand
    from pynlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
except ImportError:
    # Create placeholder classes if original not available
    print("Warning: Original NLM bindings not found")
    
    class Config: pass
    class Brain: pass
    class AgentBrain: pass
    class SimpleWorld: pass
    class Vision: pass
    class Audio: pass
    class Action: pass
    class SensoryInput: pass
    
    # Define placeholder enums
    class NeuronType:
        Excitatory = 0; Inhibitory = 1; Modulatory = 2
        Sensory = 3; Motor = 4; Internal = 5
    
    class SynapseType:
        Excitatory = 0; Inhibitory = 1; Modulatory = 2
        Electrical = 3; GapJunction = 4
    
    class DevelopmentalStage:
        Initial = 0; CriticalPeriod = 1; Maturation = 2
        Adult = 3; Aging = 4
    
    class FiringState:
        Resting = 0; Active = 1; Refractory = 2; Inhibited = 3
    
    class ActionType:
        MoveForward = 0; MoveBackward = 1; MoveLeft = 2; MoveRight = 3
        TurnLeft = 4; TurnRight = 5; Look = 6; LookUp = 7; LookDown = 8
        Interact = 9; Eat = 10; Drink = 11; Rest = 12; Wait = 13; Custom = 14
    
    class WorldObjectType:
        Empty = 0; Resource = 1; Hazard = 2; Wall = 3; Marker = 4
    
    class MotorCommand:
        MoveForward = 0; MoveBackward = 1; TurnLeft = 2; TurnRight = 3
        LookLeft = 4; LookRight = 5; Interact = 6; Wait = 7
    
    def createDefaultConfig(): return Config()
    def createBrain(config): return Brain()
    def createSimpleWorld(): return SimpleWorld()
    def createAgentBrain(brain): return AgentBrain(brain)

# Convenience functions for common enhanced tasks
def create_enhanced_brain(config=None):
    """Create an enhanced brain with validated configuration"""
    if config is None:
        config = createDefaultConfig()
    
    # If config is ValidatedConfig, validation is already set up
    brain = createBrain(config)
    brain.initialize()
    return brain

def benchmark_network(brain, steps, runs=3):
    """Benchmark network performance"""
    import time
    
    results = []
    for run in range(runs):
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        results.append({
            'time': end_time - start_time,
            'firing_rate': brain.getAverageFiringRate(),
            'spikes': brain.getTotalSpikeCount()
        })
        
        # Reset for next run
        brain.reset()
        brain.initialize()
    
    avg_time = sum(r['time'] for r in results) / len(results)
    avg_firing = sum(r['firing_rate'] for r in results) / len(results)
    avg_spikes = sum(r['spikes'] for r in results) / len(results)
    
    return {
        'average_time': avg_time,
        'average_firing_rate': avg_firing,
        'average_spike_count': avg_spikes,
        'speed_steps_per_second': steps / avg_time
    }

# Export the enhanced builder
from .neural_network_builder import NeuralNetworkBuilder
builder = NeuralNetworkBuilder()

# Export public API
__all__ = [
    # Enhanced core classes
    'ValidatedConfig',
    'AdvancedSimulationManager',
    'PerformanceMonitor',
    'ComponentFactory', 
    'AnalysisUtils',
    'NeuralNetworkBuilder',
    
    # Original NLM classes (if available)
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld', 'Vision', 'Audio', 'Action',
    'SensoryInput', 'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'FiringState', 'ActionType', 'WorldObjectType', 'MotorCommand',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    
    # Convenience functions
    'create_enhanced_brain', 'benchmark_network',
    
    # Builder instance
    'builder'
]

__version__ = "2.0.0-enhanced"
__author__ = "NLM Research Team"
__description__ = "Enhanced Python bindings for NLM with advanced features"
