#!/usr/bin/env python3
"""
Enhanced Python API for NLM - Convenience functions and utilities

This module provides an enhanced Python API for the NLM neural simulation framework,
building on the core bindings with additional convenience functions, better
documentation, and advanced features for both beginners and power users.

This is the main enhanced API module that provides all the new functionality
including presets, monitoring, analysis, and advanced tools.
"""

from pynlm import (
    # Core classes
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryPercept,
    Vision,
    Action,
    WorldObject,
    ActionType,
    MotorCommand,
    NeuronType,
    DevelopmentalStage,
    WorldObjectType,
)

from typing import List, Dict, Any, Optional, Callable, Union
from dataclasses import dataclass, field_default_factory
import json
import time
import logging
from enum import Enum
class BrainType(Enum):
    """Standard brain configurations"""
    
    @staticmethod
    def create_preset(preset_name: str, **kwargs) -> Dict[str, Any]:
        """Create a simulation preset with specified parameters"""
        presets = {
            "fast": {
                "brain": {"neuron_count": 500, "connection_probability": 0.05},
                "simulation": {"timestep": 0.005, "steps": 100},
                "world": {"width": 20, "height": 20}
            },
            "detailed": {
                "brain": {"neuron_count": 2000, "connection_probability": 0.15},
                "simulation": {"timestep": 0.001, "steps": 1000},
                "world": {"width": 50, "height": 50}
            },
            "research": {
                "brain": {"neuron_count": 5000, "connection_probability": 0.2},
                "simulation": {"timestep": 0.0005, "steps": 5000},
                "world": {"width": 100, "height": 100}
            }
        }
        
        if preset_name not in presets:
            raise ValueError(f"Unknown preset: {preset_name}. Available: {list(presets.keys())}")
        
        result = presets[preset_name].copy()
        
        # Override with user-specified parameters
        for category in ["brain", "simulation", "world"]:
            if category in kwargs:
                result[category].update(kwargs[category])
        
        return result
class SimulationMonitor:
    """Performance monitoring for neural simulations"""
    
    def __init__(self, brain=None, agent=None, world=None):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.start_time = None
        self.step_times = []
        self.metrics = {}
        self.logging_enabled = True
        self.log_level = logging.INFO
    
    def start(self):
        """Start monitoring a simulation"""
        self.start_time = time.time()
        self.step_times = []
        self.metrics = {}
    
    def step_end(self, step_num, timestep):
        """Record metrics for a completed step"""
        step_time = time.time() - self.start_time
        self.step_times.append(step_time)
        
        if self.brain:
            self.metrics[step_num] = {
                "firing_neurons": self.brain.getFiringNeuronCount(),
                "total_spikes": self.brain.getTotalSpikeCount(),
                "avg_firing_rate": self.brain.getAverageFiringRate(),
                "step_time_ms": step_time * 1000,
                "neurons_per_step": self.brain.getTotalNeuronCount(),
            }
        
        if self.log_level <= logging.INFO:
            print(f"Step {step_num}: {self.brain.getFiringNeuronCount()} firing, "
                  f"rate {self.brain.getAverageFiringRate():.2f}")
    
    def get_summary(self):
        """Get performance summary"""
        if not self.step_times:
            return {"error": "No data collected"}
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        total_time = sum(self.step_times)
        
        return {
            "total_steps": len(self.step_times),
            "avg_step_time_ms": avg_step_time * 1000,
            "total_time_s": total_time,
            "steps_per_second": len(self.step_times) / total_time if total_time > 0 else 0,
            "max_step_time_ms": max(self.step_times) * 1000,
            "min_step_time_ms": min(self.step_times) * 1000,
        }
    
    def get_detailed_metrics(self, last_n=100):
        """Get detailed metrics for last N steps"""
        if not self.metrics:
            return []
        
        steps = sorted(self.metrics.keys())
        if last_n > 0:
            steps = steps[-last_n:]
        
        return [self.metrics[step] for step in steps]
class CheckpointManager:
    """Enhanced checkpoint management for brain states"""
    
    def __init__(self, brain=None, agent=None):
        self.brain = brain
        self.agent = agent
        self.auto_save_interval = 100
        self.checkpoint_path = "/tmp/nlm_checkpoint"
        self.checkpoints_dir = "/tmp/nlm_checkpoints"
        self.max_checkpoints = 10
    
    def save_checkpoint(self, name=None, **metadata):
        """Save current simulation state to checkpoint"""
        if not self.brain:
            raise RuntimeError("No brain available for checkpointing")
        
        if name is None:
            timestamp = int(time.time())
            name = f"checkpoint_{timestamp}"
        
        path = f"{self.checkpoint_path}_{name}.bin"
        
        # Save brain state
        if not self.brain.save(path):
            raise RuntimeError(f"Failed to save checkpoint to {path}")
        
        # Save metadata
        metadata_path = f"{self.checkpoint_path}_{name}.json"
        with open(metadata_path, 'w') as f:
            json.dump({
                "name": name,
                "timestamp": time.time(),
                "brain_neuron_count": self.brain.getTotalNeuronCount(),
                "brain_synapse_count": self.brain.getTotalSynapseCount(),
                "firing_count": self.brain.getFiringNeuronCount(),
                "total_spikes": self.brain.getTotalSpikeCount(),
                "custom_metadata": metadata
            }, f, indent=2)
        
        # Clean up old checkpoints
        self._cleanup_old_checkpoints()
        
        return path
    
    def load_checkpoint(self, name):
        """Load simulation state from checkpoint"""
        if not self.brain:
            raise RuntimeError("No brain available for loading")
        
        path = f"{self.checkpoint_path}_{name}.bin"
        
        if not self.brain.load(path):
            raise RuntimeError(f"Failed to load checkpoint from {path}")
        
        # Load metadata
        metadata_path = f"{self.checkpoint_path}_{name}.json"
        with open(metadata_path, 'r') as f:
            return json.load(f)
    
    def list_checkpoints(self):
        """List available checkpoints"""
        checkpoints = []
        for i in range(self.max_checkpoints):
            for ext in ['.bin', '.json']:
                path = f"{self.checkpoint_path}_{i}{ext}"
                try:
                    with open(path, 'r' if ext == '.json' else 'rb') as f:
                        if ext == '.json':
                            meta = json.load(f)
                            meta['path'] = path
                            checkpoints.append(meta)
                        else:
                            checkpoints.append({
                                "path": path,
                                "size": len(f.read())
                            })
                except (FileNotFoundError, IOError, json.JSONDecodeError):
                    continue
        
        return sorted(checkpoints, key=lambda x: x.get('timestamp', 0), reverse=True)
    
    def _cleanup_old_checkpoints(self):
        """Remove old checkpoints to save space"""
        checkpoints = self.list_checkpoints()
        if len(checkpoints) <= self.max_checkpoints:
            return
        
        # Keep newest checkpoints
        for checkpoint in checkpoints[self.max_checkpoints:]:
            try:
                if checkpoint.get('path'):
                    import os
                    os.remove(checkpoint['path'])
            except (FileNotFoundError, OSError):
                pass
def create_config_from_preset(preset_name: str, **kwargs) -> Config:
    """Create configuration from named preset"""
    presets = {
        "standard": {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "brain.connection_probability": 0.05,
            "brain.v_thresh": -50.0,
            "brain.v_rest": -70.0,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
        },
        "large": {
            "brain.neuron_count": 5000,
            "brain.synapse_density": 0.15,
            "brain.connection_probability": 0.1,
            "brain.v_thresh": -45.0,
            "brain.v_rest": -70.0,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.dopamine.scale": 1.2,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
        },
        "small": {
            "brain.neuron_count": 100,
            "brain.synapse_density": 0.05,
            "brain.connection_probability": 0.02,
            "brain.v_thresh": -55.0,
            "brain.v_rest": -70.0,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": False,
            "neuromod.dopamine.scale": 0.8,
            "neuromod.curiosity.enable": False,
        },
        "research": {
            "brain.neuron_count": 10000,
            "brain.synapse_density": 0.2,
            "brain.connection_probability": 0.15,
            "brain.v_thresh": -40.0,
            "brain.v_rest": -70.0,
            "brain.tau_mem": 15.0,
            "brain.tau_ref": 3.0,
            "plasticity.stdp.enable": True,
            "plasticity.stdp.learning_rate": 0.002,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "development.enable": True,
            "neuromod.dopamine.scale": 1.5,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
        }
    }
    
    if preset_name not in presets:
        raise ValueError(f"Unknown preset: {preset_name}")
    
    config = Config()
    
    # Apply preset
    for key, value in presets[preset_name].items():
        config.set(key, value)
    
    # Override with user parameters
    for key, value in kwargs.items():
        config.set(key, value)
    
    return config
def get_standard_config() -> Config:
    """Get the standard configuration (convenience function)"""
    return create_config_from_preset("standard")
def create_brain_quick(brain_type: str = "standard", **kwargs) -> Brain:
    """Quick brain creation with preset configurations"""
    config = create_config_from_preset(brain_type, **kwargs)
    return createBrain(config)
def run_simulation_loop(brain, world=None, agent=None, 
                        steps=100, timestep=0.1, 
                        monitor=None, verbose=True):
    """Run a complete simulation loop with monitoring and error handling"""
    if monitor:
        monitor.start()
    
    try:
        for step in range(steps):
            if world is not None and agent is not None:
                # Full agent-world simulation
                if verbose and step % 20 == 0:
                    print(f"Step {step}/{steps}")
                
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Brain step
                brain.step(step, step * timestep)
                
                # Get action and apply
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, step * timestep)
                
            else:
                # Simple brain-only simulation
                if verbose and step % 20 == 0:
                    print(f"Step {step}/{steps}")
                brain.step(step, step * timestep)
            
            if monitor:
                monitor.step_end(step, timestep)
        
        if monitor:
            print("\nSimulation Summary:")
            print(json.dumps(monitor.get_summary(), indent=2))
        
        return True
    except Exception as e:
        if monitor:
            print(f"Simulation failed at step {step}: {e}")
        raise
def create_agent_with_features(brain, world, enable_reward=True, 
                               enable_curiosity=True, enable_development=True,
                               enable_plasticity=True):
    """Create agent with commonly enabled features"""
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    if enable_reward:
        agent.enableRewardModulation(True)
    if enable_curiosity:
        agent.enableCuriosity(True)
    if enable_development:
        agent.enableDevelopment(True)
    if enable_plasticity:
        agent.enableStructuralPlasticity(True)
    
    return agent
def analyze_brain_state(brain, include_detailed=True):
    """Analyze brain state and return comprehensive statistics"""
    if brain is None:
        return None
    
    analysis = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "developmental_stage": brain.getDevelopmentalStage(),
        "regions_count": brain.getRegionCount(),
    }
    
    if include_detailed:
        analysis["detailed"] = {
            "active_regions": [],
            "synaptic_weight_stats": {},
            "neuron_type_distribution": {},
        }
        
        # Get regions info
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            analysis["detailed"]["active_regions"].append({
                "id": region_id,
                "neuron_count": len(region.getAllNeurons()),
                "synapse_count": len(region.getSynapses()),
            })
    
    return analysis
def save_experiment_data(brain, agent=None, world=None, 
                         metadata=None, filepath=None):
    """Save experiment data with metadata"""
    if filepath is None:
        import uuid
        filepath = f"/tmp/nlm_experiment_{uuid.uuid4().hex}.json"
    
    experiment_data = {
        "timestamp": time.time(),
        "brain_analysis": analyze_brain_state(brain),
        "metadata": metadata or {},
    }
    
    if agent:
        experiment_data["agent_state"] = {
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "neuromodulation_level": agent.getNeuromodulationLevel(),
            "developmental_stage": agent.getDevelopmentalStage(),
        }
    
    if world:
        experiment_data["world_state"] = {
            "width": world.getWidth(),
            "height": world.getHeight(),
            "simulation_time": world.getSimulationTime(),
            "agent_body": {
                "x": world.getAgentBody().x,
                "y": world.getAgentBody().y,
                "orientation": world.getAgentBody().orientation,
            } if world.getAgentBody() else None,
        }
    
    with open(filepath, 'w') as f:
        json.dump(experiment_data, f, indent=2)
    
    return filepath
class NLMSimulationError(Exception):
    """Base exception for NLM simulation errors"""
    pass
class InvalidConfigurationError(NLMSimulationError):
    """Raised when configuration is invalid"""
    pass
class SimulationTimeoutError(NLMSimulationError):
    """Raised when simulation exceeds time limit"""
    pass
class ComponentError(NLMSimulationError):
    """Raised when component initialization or operation fails"""
    pass
# Deprecation warnings for old-style APIs
def _deprecated(func):
    """Decorator to mark deprecated functions"""
    import warnings
    
    def wrapper(*args, **kwargs):
        warnings.warn(
            f"{func.__name__} is deprecated and will be removed in future versions. "
            f"Use {func.__name__}_new instead.",
            DeprecationWarning,
            stacklevel=2
        )
        return func(*args, **kwargs)
    
    return wrapper
@_deprecated
def run_old_style_simulation(brain, steps=100):
    """Deprecated old-style simulation loop"""
    for i in range(steps):
        brain.step(i)
    return True
# Python version compatibility functions
def check_python_version(min_major=3, min_minor=8):
    """Check if Python version meets minimum requirements"""
    import sys
    return sys.version_info >= (min_major, min_minor)
# Enhanced documentation strings for type hints
def get_type_hints():
    """Return type hints for enhanced API functions"""
    return {
        "create_config_from_preset": "(preset_name: str, **kwargs) -> Config",
        "run_simulation_loop": "(brain, world=None, agent=None, steps=100, timestep=0.1, monitor=None, verbose=True) -> bool",
        "analyze_brain_state": "(brain, include_detailed=True) -> Dict[str, Any]",
        "save_experiment_data": "(brain, agent=None, world=None, metadata=None, filepath=None) -> str",
        "SimulationMonitor": "Class for monitoring simulation performance",
        "CheckpointManager": "Class for managing brain checkpoints",
        "BrainType": "Enum for brain configuration types",
        "SimulationPreset": "Class for simulation presets",
    }
# Module-level constants and utilities
MODULE_VERSION = "2.0.0"
MIN_PYTHON_VERSION = (3, 8)
SUPPORTED_BRAIN_TYPES = [bt.value for bt in BrainType]
SUPPORTED_PRESETS = ["standard", "large", "small", "fast", "detailed", "research"]

def get_module_info():
    """Return information about the enhanced NLM module"""
    return {
        "version": MODULE_VERSION,
        "description": "Enhanced Python API for NLM neural simulation framework",
        "features": [
            "Convenience functions and factory methods",
            "Performance monitoring tools",
            "Enhanced checkpoint management",
            "Brain state analysis utilities",
            "Experiment data management",
            "Simulation preset configurations",
            "Error handling and validation",
            "Backward compatibility with deprecation warnings",
        ],
        "minimum_python_version": MIN_PYTHON_VERSION,
        "supported_brain_types": SUPPORTED_BRAIN_TYPES,
        "supported_presets": SUPPORTED_PRESETS,
        "additional_modules": [
            "pynlm_enhanced",
            "pynlm_advanced",
            "pynlm_research",
        ]
    }