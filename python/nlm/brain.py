"""Brain and neural network management for NLM Python API."""

import numpy as np
from typing import Dict, List, Optional, Any, Union
from pathlib import Path
import json
import pickle

from ..core.Config.Config import Config
from ..brain.Brain.hpp:4> # noqa: F401
from ..agent.AgentBrain.hpp:4> # noqa: F401
from ..world.SimpleWorld.hpp:4> # noqa: F401


class BrainNetwork:
    """
    High-level brain network class for neural simulation.
    
    This class provides a simplified interface to the underlying C++ Brain class,
    making it easier to create, configure, and simulate neural networks.
    """
    
    def __init__(self, config: Optional[Union[Config, Dict[str, Any]]] = None):
        """
        Initialize a brain network.
        
        Args:
            config: Brain configuration (Config object or dictionary)
            
        Example:
            >>> brain = BrainNetwork()
            >>> brain = BrainNetwork(config={"regions": 5, "neurons_per_region": 100})
        """
        if config is None:
            self._config = Config()
        elif isinstance(config, Config):
            self._config = config
        else:
            self._config = Config()
            for key, value in config.items():
                self._config.setValue(key, value)
        
        self._brain = None
        self._is_initialized = False
        
    @property
    def config(self) -> Config:
        """Get the brain configuration."""
        return self._config
        
    @property
    def is_initialized(self) -> bool:
        """Check if the brain is initialized."""
        return self._is_initialized
        
    def initialize(self) -> bool:
        """
        Initialize the brain with current configuration.
        
        Returns:
            bool: True if initialization successful
            
        Example:
            >>> brain.initialize()
        """
        try:
            # In a real implementation, this would create a C++ Brain object
            # For now, we'll simulate it
            self._brain = {"regions": [], "neurons": 0}
            self._is_initialized = True
            return True
        except Exception as e:
            print(f"Initialization failed: {e}")
            return False
            
    def add_region(self, name: str, neuron_count: int = 100, neuron_type: str = "excitatory"):
        """
        Add a new neural region to the brain.
        
        Args:
            name: Name of the region
            neuron_count: Number of neurons in the region
            neuron_type: Type of neurons ("excitatory", "inhibitory", etc.)
            
        Example:
            >>> brain.add_region("visual_cortex", 200)
            >>> brain.add_region("motor_cortex", 150, "motor")
        """
        if not self._is_initialized:
            self.initialize()
            
        # In real implementation, this would create a NeuralRegion in C++
        region = {
            "name": name,
            "neuron_count": neuron_count,
            "neuron_type": neuron_type,
            "neurons": np.random.randn(neuron_count)
        }
        self._brain["regions"].append(region)
        self._brain["neurons"] += neuron_count
        
    def step(self, dt: float = 0.1) -> Dict[str, Any]:
        """
        Perform a simulation step.
        
        Args:
            dt: Time step size
            
        Returns:
            Dict[str, Any]: Simulation results for this step
            
        Example:
            >>> results = brain.step(0.05)
        """
        if not self._is_initialized:
            raise RuntimeError("Brain not initialized. Call initialize() first.")
            
        # Simulate neural dynamics
        spike_counts = {}
        for region in self._brain["regions"]:
            # Simple spike generation simulation
            spikes = np.random.poisson(region["neuron_count"] * 0.1)
            spike_counts[region["name"]] = spikes
            
        return {
            "step": len(self._brain.get("step_count", 0)),
            "spike_counts": spike_counts,
            "total_spikes": sum(spike_counts.values()),
            "time_step": dt
        }
        
    def get_region_info(self, region_name: str) -> Dict[str, Any]:
        """
        Get information about a specific region.
        
        Args:
            region_name: Name of the region
            
        Returns:
            Dict[str, Any]: Region information
            
        Example:
            >>> region_info = brain.get_region_info("visual_cortex")
        """
        for region in self._brain["regions"]:
            if region["name"] == region_name:
                return region
        return {}
        
    def get_statistics(self) -> Dict[str, Any]:
        """
        Get brain network statistics.
        
        Returns:
            Dict[str, Any]: Network statistics
            
        Example:
            >>> stats = brain.get_statistics()
        """
        if not self._is_initialized:
            return {}
            
        total_neurons = self._brain["neurons"]
        total_regions = len(self._brain["regions"])
        
        region_types = {}
        for region in self._brain["regions"]:
            region_types[region["neuron_type"]] = region_types.get(region["neuron_type"], 0) + 1
            
        return {
            "total_neurons": total_neurons,
            "total_regions": total_regions,
            "region_types": region_types,
            "avg_neurons_per_region": total_neurons / max(total_regions, 1),
            "initialized": self._is_initialized
        }
        
    def save(self, filepath: str) -> None:
        """
        Save brain network to file.
        
        Args:
            filepath: Output file path
            
        Example:
            >>> brain.save("brain_state.json")
        """
        filepath = Path(filepath)
        filepath.parent.mkdir(parents=True, exist_ok=True)
        
        data = {
            "config": {},
            "brain": self._brain,
            "statistics": self.get_statistics()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
            
    def load(self, filepath: str) -> None:
        """
        Load brain network from file.
        
        Args:
            filepath: Input file path
            
        Example:
            >>> brain.load("brain_state.json")
        """
        filepath = Path(filepath)
        
        with open(filepath, 'r') as f:
            data = json.load(f)
            
        self._brain = data["brain"]
        self._is_initialized = True
        
        # Load config from data if available
        if "config" in data:
            for key, value in data["config"].items():
                self._config.setValue(key, value)
                
    def reset(self) -> None:
        """Reset brain network to initial state."""
        self._brain = None
        self._is_initialized = False


def create_network(config: Optional[Union[Config, Dict[str, Any]]] = None) -> BrainNetwork:
    """
    Create and configure neural networks.
    
    Args:
        config: Network configuration
        
    Returns:
        BrainNetwork: Created brain network
        
    Example:
        >>> brain = create_network({"regions": 3, "neurons_per_region": 100})
        >>> brain = create_network()  # Default configuration
    """
    return BrainNetwork(config)


def import_brain(filepath: str) -> BrainNetwork:
    """
    Import pre-trained brain models.
    
    Args:
        filepath: Path to saved brain model file
        
    Returns:
        BrainNetwork: Imported brain network
        
    Example:
        >>> brain = import_brain("pretrained_brain.json")
        >>> brain = import_brain("model.pkl", format="pickle")
    """
    filepath = Path(filepath)
    
    if filepath.suffix.lower() == ".json":
        with open(filepath, 'r') as f:
            data = json.load(f)
            
        brain = BrainNetwork()
        brain.load(filepath)
        
    elif filepath.suffix.lower() == ".pkl" or filepath.suffix.lower() == ".pickle":
        with open(filepath, 'rb') as f:
            brain_data = pickle.load(f)
            
        brain = BrainNetwork(brain_data.get("config", {}))
        brain._brain = brain_data.get("brain", {})
        brain._is_initialized = True
        
    else:
        raise ValueError(f"Unsupported file format: {filepath.suffix}")
    
    return brain


def export_brain(brain: BrainNetwork, filepath: str, format: str = "auto") -> None:
    """
    Export brain models for sharing.
    
    Args:
        brain: Brain network to export
        filepath: Output file path
        format: Export format ("auto", "json", or "pickle")
        
    Example:
        >>> export_brain(brain, "model.json")
        >>> export_brain(brain, "model.pkl", format="pickle")
    """
    filepath = Path(filepath)
    filepath.parent.mkdir(parents=True, exist_ok=True)
    
    data = {
        "config": {},
        "brain": brain._brain if brain._is_initialized else {},
        "metadata": {
            "version": "0.1.0",
            "export_date": str(Path(filepath).stat().st_mtime) if filepath.exists() else ""
        }
    }
    
    if format == "auto" or format == "json":
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    elif format == "pickle" or format == "pkl":
        with open(filepath, 'wb') as f:
            pickle.dump(data, f)
    else:
        raise ValueError(f"Unsupported format: {format}")


def simulate_brain(brain: BrainNetwork, steps: int = 100, dt: float = 0.1) -> Dict[str, Any]:
    """
    Run brain simulations with results.
    
    Args:
        brain: Brain network to simulate
        steps: Number of simulation steps
        dt: Time step size
        
    Returns:
        Dict[str, Any]: Simulation results and statistics
        
    Example:
        >>> results = simulate_brain(brain, steps=500, dt=0.05)
        >>> print(f"Total spikes: {results['total_spikes']}")
    """
    if not brain._is_initialized:
        raise RuntimeError("Brain not initialized. Call initialize() first.")
        
    simulation_results = {
        "steps": steps,
        "time_step": dt,
        "step_results": [],
        "total_spikes": 0,
        "region_spikes": {},
        "average_firing_rate": 0.0
    }
    
    region_spikes = {}
    
    for step in range(steps):
        step_result = brain.step(dt)
        simulation_results["step_results"].append(step_result)
        simulation_results["total_spikes"] += step_result["total_spikes"]
        
        for region_name, spike_count in step_result["spike_counts"].items():
            region_spikes[region_name] = region_spikes.get(region_name, 0) + spike_count
            
    simulation_results["region_spikes"] = region_spikes
    
    # Calculate average firing rate
    total_neurons = sum([region["neuron_count"] for region in brain._brain["regions"]])
    if total_neurons > 0:
        simulation_results["average_firing_rate"] = simulation_results["total_spikes"] / (steps * dt)
    
    return simulation_results


def create_agent_brain(brain: BrainNetwork) -> AgentBrain:
    """
    Create an agent brain interface from a brain network.
    
    Args:
        brain: Brain network to wrap
        
    Returns:
        AgentBrain: Agent brain interface
        
    Example:
        >>> agent_brain = create_agent_brain(brain)
    """
    # This would require access to the C++ AgentBrain class
    # For now, we'll return a simple wrapper
    return AgentBrain(brain)