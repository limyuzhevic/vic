"""
Advanced Configuration Presets for NLM Research

This module provides research-grade configuration presets for different domains and use cases.
It includes cognitive development, social learning, and performance-optimized configurations.
"""

import json
from typing import Dict, List, Optional
from dataclasses import dataclass, field
from enum import Enum
import pynlm

class ResearchDomain(Enum):
    COGNITIVE_DEVELOPMENT = "cognitive_development"
    SOCIAL_LEARNING = "social_learning"
    COGNITIVE_ARCHITECTURE = "cognitive_architecture"
    BEHAVIORAL_MODELING = "behavioral_modeling"
    NEUROSCIENCE_RESEARCH = "neuroscience_research"

@dataclass
class ResearchPreset:
    """Research-grade configuration preset."""
    
    name: str
    description: str
    domain: ResearchDomain
    config: Dict[str, Any] = field(default_factory=dict)
    
    def apply_to_brain(self) -> Any:
        """Apply preset to create brain configuration."""
        preset_config = pynlm.createExperimentConfig()
        
        # Apply all preset parameters
        for key, value in self.config.items():
            if isinstance(value, dict):
                # Handle nested configuration
                for sub_key, sub_value in value.items():
                    full_key = f"{key}.{sub_key}"
                    preset_config.set(full_key, sub_value)
            else:
                preset_config.set(key, value)
        
        return preset_config

class AdvancedConfigurationPresets:
    """
    Collection of advanced configuration presets for research applications.
    
    Each preset is optimized for specific research domains and includes
    parameters that have been validated through experimental research.
    """
    
    def __init__(self):
        self.presets = self._initialize_presets()
    
    def get_preset(self, name: str) -> Optional[ResearchPreset]:
        """Get a specific preset by name."""
        return self.presets.get(name)
    
    def get_presets_by_domain(self, domain: ResearchDomain) -> List[ResearchPreset]:
        """Get all presets for a specific domain."""
        return [p for p in self.presets.values() if p.domain == domain]
    
    def get_all_presets(self) -> Dict[str, ResearchPreset]:
        """Get all presets."""
        return self.presets
    
    def create_custom_preset(self, name: str, domain: ResearchDomain, 
                           base_preset: Optional[str] = None,
                           modifications: Optional[Dict[str, Any]] = None) -> ResearchPreset:
        """
        Create a custom preset based on an existing preset.
        
        Args:
            name: Name for the new preset
            domain: Research domain
            base_preset: Name of preset to base on (optional)
            modifications: Dictionary of parameter modifications
            
        Returns:
            New ResearchPreset instance
        """
        if base_preset and base_preset in self.presets:
            base = self.presets[base_preset]
            config = base.config.copy()
        else:
            config = {}
        
        if modifications:
            for key, value in modifications.items():
                if '.' in key:
                    # Handle nested keys
                    parts = key.split('.')
                    current = config
                    for part in parts[:-1]:
                        if part not in current:
                            current[part] = {}
                        current = current[part]
                    current[parts[-1]] = value
                else:
                    config[key] = value
        
        description = f"Custom {domain.value} preset based on research requirements"
        if base_preset:
            description += f", derived from {base_preset}"
        
        preset = ResearchPreset(name, description, domain, config)
        self.presets[name] = preset
        return preset
    
    def _initialize_presets(self) -> Dict[str, ResearchPreset]:
        """Initialize all research-grade presets."""
        presets = {}
        
        # Cognitive Development Preset
        presets["cognitive_development"] = ResearchPreset(
            name="cognitive_development",
            description="Optimized for studying cognitive development across lifespan",
            domain=ResearchDomain.COGNITIVE_DEVELOPMENT,
            config={
                "brain.neuron_count": 2000,
                "brain.region_count": 5,
                "brain.connection_probability": 0.08,
                "plasticity.learningRate": 0.02,
                "plasticity.stdp.learningRate": 0.015,
                "plasticity.stdp.tauPlus": 25.0,
                "plasticity.stdp.tauMinus": 30.0,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 1.2,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "simulation.timestep": 0.0005,
                "simulation.maxSteps": 50000,
                "development.enabled": True,
                "development.stage": "critical_period",
                "memory.working.capacity": 100,
                "memory.episodic.capacity": 50,
                "logging.level": "INFO",
                "visualization.enabled": False,
                "checkpoint.interval": 300.0
            }
        )
        
        # Social Learning Preset
        presets["social_learning"] = ResearchPreset(
            name="social_learning",
            description="Optimized for studying social learning and imitation",
            domain=ResearchDomain.SOCIAL_LEARNING,
            config={
                "brain.neuron_count": 1500,
                "brain.region_count": 4,
                "brain.connection_probability": 0.12,
                "plasticity.learningRate": 0.03,
                "plasticity.stdp.learningRate": 0.025,
                "plasticity.stdp.tauPlus": 15.0,
                "plasticity.stdp.tauMinus": 18.0,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 1.5,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "simulation.timestep": 0.001,
                "simulation.maxSteps": 30000,
                "development.enabled": False,
                "environment.type": "social",
                "environment.social_influence": 0.7,
                "environment.imitation_learning": True,
                "logging.level": "DEBUG",
                "visualization.enabled": True,
                "checkpoint.interval": 600.0
            }
        )
        
        # Cognitive Architecture Preset
        presets["cognitive_architecture"] = ResearchPreset(
            name="cognitive_architecture",
            description="Optimized for studying cognitive architectures and information processing",
            domain=ResearchDomain.COGNITIVE_ARCHITECTURE,
            config={
                "brain.neuron_count": 3000,
                "brain.region_count": 8,
                "brain.connection_probability": 0.06,
                "plasticity.learningRate": 0.01,
                "plasticity.stdp.learningRate": 0.008,
                "plasticity.stdp.tauPlus": 10.0,
                "plasticity.stdp.tauMinus": 12.0,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 1.0,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "simulation.timestep": 0.0002,
                "simulation.maxSteps": 100000,
                "development.enabled": False,
                "prediction.enabled": True,
                "cognition.planning.enabled": True,
                "cognition.attention.enabled": True,
                "cognition.concept_formation.enabled": True,
                "memory.working.capacity": 200,
                "memory.episodic.capacity": 100,
                "memory.associative.capacity": 500,
                "logging.level": "INFO",
                "visualization.enabled": True,
                "checkpoint.interval": 900.0,
                "output_format": "json"
            }
        )
        
        # Behavioral Modeling Preset
        presets["behavioral_modeling"] = ResearchPreset(
            name="behavioral_modeling",
            description="Optimized for behavioral modeling and reinforcement learning",
            domain=ResearchDomain.BEHAVIORAL_MODELING,
            config={
                "brain.neuron_count": 1000,
                "brain.region_count": 3,
                "brain.connection_probability": 0.15,
                "plasticity.learningRate": 0.05,
                "plasticity.stdp.learningRate": 0.04,
                "plasticity.stdp.tauPlus": 8.0,
                "plasticity.stdp.tauMinus": 10.0,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": False,
                "neuromod.dopamine.scale": 2.0,
                "neuromod.curiosity.enable": False,
                "neuromod.novelty.enable": False,
                "simulation.timestep": 0.002,
                "simulation.maxSteps": 20000,
                "development.enabled": False,
                "environment.type": "grid_world",
                "environment.reward_structure": "sparse",
                "environment.action_space": "continuous",
                "logging.level": "WARN",
                "visualization.enabled": False,
                "checkpoint.interval": 0.0,
                "output_format": "csv"
            }
        )
        
        # Neuroscience Research Preset
        presets["neuroscience_research"] = ResearchPreset(
            name="neuroscience_research",
            description="Optimized for neuroscience research and biological fidelity",
            domain=ResearchDomain.NEUROSCIENCE_RESEARCH,
            config={
                "brain.neuron_count": 5000,
                "brain.region_count": 6,
                "brain.connection_probability": 0.04,
                "plasticity.learningRate": 0.005,
                "plasticity.stdp.learningRate": 0.003,
                "plasticity.stdp.tauPlus": 50.0,
                "plasticity.stdp.tauMinus": 60.0,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.dopamine.scale": 0.8,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "simulation.timestep": 0.0001,
                "simulation.maxSteps": 200000,
                "development.enabled": True,
                "development.stage": "maturation",
                "biophysical.enabled": True,
                "biophysical.model_type": "morphological",
                "biophysical.synaptic_model": "double_exponential",
                "memory.working.capacity": 300,
                "memory.episodic.capacity": 200,
                "memory.associative.capacity": 1000,
                "logging.level": "DEBUG",
                "visualization.enabled": True,
                "checkpoint.interval": 1800.0,
                "output_format": "json",
                "parallel.processing": True,
                "use_gpu": True,
                "distributed": False
            }
        )
        
        return presets

class PerformanceOptimizedPresets:
    """
    Performance-optimized configuration presets for different hardware platforms.
    """
    
    def __init__(self):
        self.presets = {}
    
    def get_preset_for_hardware(self, cpu_cores: int, memory_mb: int, 
                               has_gpu: bool = False) -> ResearchPreset:
        """
        Get a performance-optimized preset based on available hardware.
        
        Args:
            cpu_cores: Number of CPU cores available
            memory_mb: Available memory in MB
            has_gpu: Whether GPU is available
            
        Returns:
            ResearchPreset optimized for the hardware
        """
        if has_gpu and memory_mb > 16000:
            # High-end workstation/server
            return self._create_high_end_preset()
        elif has_gpu:
            # Mid-range workstation
            return self._create_midrange_preset()
        elif memory_mb > 8000:
            # Standard workstation
            return self._create_standard_preset()
        else:
            # Limited resources
            return self._create_lightweight_preset()
    
    def _create_high_end_preset(self) -> ResearchPreset:
        """Create high-end performance preset."""
        return ResearchPreset(
            name="high_end_performance",
            description="Optimized for high-end hardware with GPU acceleration",
            domain=ResearchDomain.NEUROSCIENCE_RESEARCH,
            config={
                "brain.neuron_count": 10000,
                "brain.region_count": 10,
                "brain.connection_probability": 0.03,
                "plasticity.learningRate": 0.003,
                "plasticity.stdp.learningRate": 0.002,
                "simulation.timestep": 0.00005,
                "simulation.maxSteps": 500000,
                "parallel.processing": True,
                "use_gpu": True,
                "distributed": True,
                "checkpoint.interval": 3600.0,
                "visualization.enabled": True,
                "logging.level": "INFO",
                "memory_limit": memory_mb * 0.8
            }
        )
    
    def _create_midrange_preset(self) -> ResearchPreset:
        """Create mid-range performance preset."""
        return ResearchPreset(
            name="midrange_performance",
            description="Optimized for mid-range hardware with limited GPU",
            domain=ResearchDomain.NEUROSCIENCE_RESEARCH,
            config={
                "brain.neuron_count": 5000,
                "brain.region_count": 6,
                "brain.connection_probability": 0.04,
                "plasticity.learningRate": 0.005,
                "plasticity.stdp.learningRate": 0.003,
                "simulation.timestep": 0.0001,
                "simulation.maxSteps": 200000,
                "parallel.processing": True,
                "use_gpu": True,
                "distributed": False,
                "checkpoint.interval": 1800.0,
                "visualization.enabled": False,
                "logging.level": "WARN",
                "memory_limit": memory_mb * 0.7
            }
        )
    
    def _create_standard_preset(self) -> ResearchPreset:
        """Create standard performance preset."""
        return ResearchPreset(
            name="standard_performance",
            description="Optimized for standard workstations",
            domain=ResearchDomain.NEUROSCIENCE_RESEARCH,
            config={
                "brain.neuron_count": 3000,
                "brain.region_count": 5,
                "brain.connection_probability": 0.05,
                "plasticity.learningRate": 0.008,
                "plasticity.stdp.learningRate": 0.005,
                "simulation.timestep": 0.0002,
                "simulation.maxSteps": 100000,
                "parallel.processing": True,
                "use_gpu": False,
                "distributed": False,
                "checkpoint.interval": 900.0,
                "visualization.enabled": True,
                "logging.level": "INFO",
                "memory_limit": memory_mb * 0.6
            }
        )
    
    def _create_lightweight_preset(self) -> ResearchPreset:
        """Create lightweight performance preset."""
        return ResearchPreset(
            name="lightweight_performance",
            description="Optimized for limited resources",
            domain=ResearchDomain.NEUROSCIENCE_RESEARCH,
            config={
                "brain.neuron_count": 1000,
                "brain.region_count": 3,
                "brain.connection_probability": 0.08,
                "plasticity.learningRate": 0.02,
                "plasticity.stdp.learningRate": 0.01,
                "simulation.timestep": 0.001,
                "simulation.maxSteps": 20000,
                "parallel.processing": False,
                "use_gpu": False,
                "distributed": False,
                "checkpoint.interval": 0.0,
                "visualization.enabled": False,
                "logging.level": "ERROR",
                "memory_limit": memory_mb * 0.5
            }
        )

if __name__ == "__main__":
    # Example usage
    presets = AdvancedConfigurationPresets()
    perf_presets = PerformanceOptimizedPresets()
    
    # Get cognitive development preset
    cognitive_preset = presets.get_preset("cognitive_development")
    if cognitive_preset:
        config = cognitive_preset.apply_to_brain()
        print(f"Cognitive Development preset loaded with {len(cognitive_preset.config)} parameters")
    
    # Get performance preset based on hardware
    hardware_preset = perf_presets.get_preset_for_hardware(
        cpu_cores=8,
        memory_mb=16000,
        has_gpu=True
    )
    print(f"Hardware-optimized preset: {hardware_preset.name}")
    
    print("Advanced configuration presets initialized successfully")
