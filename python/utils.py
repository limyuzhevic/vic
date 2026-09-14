# NLM Utils Python Module

"""
Utility modules for NLM Python interface
"""

from . import __init__ as nlm_init
from typing import List, Optional, Union, Any, Dict, Tuple
from pathlib import Path
import json
import time

# Re-export from main module
Brain = nlm_init.Brain
Config = nlm_init.Config
createBrain = nlm_init.createBrain
createDefaultConfig = nlm_init.createDefaultConfig
SensoryPercept = nlm_init.SensoryPercept
Vision = nlm_init.Vision
Audio = nlm_init.Audio
AgentBody = nlm_init.AgentBody
ActionResult = nlm_init.ActionResult
Action = nlm_init.Action
ActionType = nlm_init.ActionType
MotorCommand = nlm_init.MotorCommand
WorldObject = nlm_init.WorldObject
WorldObjectType = nlm_init.WorldObjectType

# Data utilities
class DataExporter:
    """Export NLM data to various formats"""
    
    @staticmethod
    def to_json(data: Dict[str, Any], filepath: Union[str, Path]) -> None:
        """Export data to JSON file"""
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    
    @staticmethod
    def to_csv(data: Dict[str, List[Any]], filepath: Union[str, Path]) -> None:
        """Export data to CSV file"""
        import csv
        
        # Transpose data for CSV format
        keys = list(data.keys())
        if not keys:
            return
        
        # Ensure all lists have same length
        length = min(len(data[key]) for key in keys)
        
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(keys)
            for i in range(length):
                row = [data[key][i] for key in keys]
                writer.writerow(row)
    
    @staticmethod
    def save_brain_state(brain: Brain, filepath: Union[str, Path]) -> None:
        """Save brain state to file"""
        brain.save(filepath)
    
    @staticmethod
    def load_brain_state(brain: Brain, filepath: Union[str, Path]) -> None:
        """Load brain state from file"""
        brain.load(filepath)


class DataImporter:
    """Import NLM data from various formats"""
    
    @staticmethod
    def from_json(filepath: Union[str, Path]) -> Dict[str, Any]:
        """Import data from JSON file"""
        with open(filepath, 'r') as f:
            return json.load(f)
    
    @staticmethod
    def from_csv(filepath: Union[str, Path]) -> Dict[str, List[Any]]:
        """Import data from CSV file"""
        import csv
        
        with open(filepath, 'r') as f:
            reader = csv.DictReader(f)
            data = {key: [] for key in reader.fieldnames}
            
            for row in reader:
                for key in data:
                    # Try to convert to numeric if possible
                    value = row[key]
                    try:
                        if '.' in value:
                            data[key].append(float(value))
                        else:
                            data[key].append(int(value))
                    except (ValueError, TypeError):
                        data[key].append(value)
            
            return data


class MetricsCalculator:
    """Calculate statistics and metrics from NLM data"""
    
    @staticmethod
    def mean(data: List[float]) -> float:
        """Calculate mean of data"""
        if not data:
            return 0.0
        return sum(data) / len(data)
    
    @staticmethod
    def std(data: List[float]) -> float:
        """Calculate standard deviation of data"""
        if len(data) < 2:
            return 0.0
        
        m = MetricsCalculator.mean(data)
        variance = sum((x - m) ** 2 for x in data) / (len(data) - 1)
        return variance ** 0.5
    
    @staticmethod
    def min(data: List[float]) -> float:
        """Calculate minimum of data"""
        if not data:
            return 0.0
        return min(data)
    
    @staticmethod
    def max(data: List[float]) -> float:
        """Calculate maximum of data"""
        if not data:
            return 0.0
        return max(data)
    
    @staticmethod
    def calculate_statistics(data: Dict[str, List[float]]) -> Dict[str, Dict[str, float]]:
        """
        Calculate statistics for multiple datasets
        
        Args:
            data: Dictionary of dataset names to data lists
            
        Returns:
            Dictionary of statistics for each dataset
        """
        statistics = {}
        
        for name, values in data.items():
            statistics[name] = {
                "mean": MetricsCalculator.mean(values),
                "std": MetricsCalculator.std(values),
                "min": MetricsCalculator.min(values),
                "max": MetricsCalculator.max(values),
                "count": len(values)
            }
        
        return statistics
    
    @staticmethod
    def format_brain_stats(brain: Brain) -> Dict[str, Any]:
        """
        Format brain statistics into a dictionary
        
        Args:
            brain: Brain object
            
        Returns:
            Dictionary of brain statistics
        """
        return {
            "neurons": brain.getTotalNeuronCount(),
            "synapses": brain.getTotalSynapseCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
            "total_spikes": brain.getTotalSpikeCount(),
            "developmental_stage": brain.getDevelopmentalStage()
        }


class SimulationUtilities:
    """Utilities for simulation management"""
    
    @staticmethod
    def run_timed_simulation(brain: Brain, world: Any, steps: int, 
                           agent: Optional[Any] = None) -> Tuple[Dict[str, Any], float]:
        """
        Run a simulation with timing
        
        Args:
            brain: Brain object
            world: World object
            steps: Number of simulation steps
            agent: Optional agent brain interface
            
        Returns:
            Tuple of (results dictionary, elapsed time in seconds)
        """
        start_time = time.time()
        
        results = {
            "total_spikes": [],
            "firing_rates": [],
            "rewards": [],
            "curiosity_levels": [],
            "novelty_levels": [],
        }
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input if agent provided
            if agent:
                agent.processSensoryInput(percept)
            
            # Step brain
            brain.step(step)
            
            # Get action if agent provided
            if agent:
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            results["total_spikes"].append(brain.getTotalSpikeCount())
            results["firing_rates"].append(brain.getAverageFiringRate())
            
            if agent:
                results["curiosity_levels"].append(agent.getCuriosityLevel())
                results["novelty_levels"].append(agent.getNoveltyLevel())
        
        elapsed = time.time() - start_time
        return results, elapsed
    
    @staticmethod
    def save_results(results: Dict[str, Any], filepath: Union[str, Path]) -> None:
        """Save simulation results to file"""
        DataExporter.to_json(results, filepath)


class ConfigurationPresets:
    """Preset configurations for different use cases"""
    
    # Brain configurations
    BRAIN_PRESETS = {
        "minimal": {
            "neuron_count": 100,
            "region_count": 1,
            "connection_probability": 0.05,
            "plasticity_learning_rate": 0.01,
        },
        "balanced": {
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "plasticity_learning_rate": 0.05,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.015,
        },
        "research": {
            "neuron_count": 5000,
            "region_count": 4,
            "connection_probability": 0.15,
            "plasticity_learning_rate": 0.1,
            "stdp_ltp_weight": 0.05,
            "stdp_ltd_weight": 0.03,
            "development_synaptogenesis_rate": 0.002,
            "development_pruning_rate": 0.0002,
            "dopamine_modulation": 2.0,
        }
    }
    
    # Agent configurations
    AGENT_PRESETS = {
        "explorer": {
            "curiosity_enable": True,
            "novelty_enable": True,
            "structural_plasticity_enable": False,
            "development_enable": True,
            "reward_modulation_enable": True,
        },
        "builder": {
            "curiosity_enable": False,
            "novelty_enable": False,
            "structural_plasticity_enable": True,
            "development_enable": True,
            "reward_modulation_enable": True,
        },
        "learner": {
            "curiosity_enable": True,
            "novelty_enable": True,
            "structural_plasticity_enable": True,
            "development_enable": True,
            "reward_modulation_enable": True,
        }
    }
    
    @classmethod
    def create_config(cls, preset: str) -> Config:
        """
        Create configuration from preset
        
        Args:
            preset: One of "minimal", "balanced", "research"
            
        Returns:
            Configuration object
        """
        if preset not in cls.BRAIN_PRESETS:
            raise ValueError(f"Unknown preset: {preset}")
        
        config = createDefaultConfig()
        
        for key, value in cls.BRAIN_PRESETS[preset].items():
            config.set(key, value)
        
        return config


# Quick setup helpers

def setup_research_config() -> Config:
    """Set up NLM for research mode"""
    return ConfigurationPresets.create_config("research")


def setup_balanced_config() -> Config:
    """Set up NLM for balanced use"""
    return ConfigurationPresets.create_config("balanced")


def setup_minimal_config() -> Config:
    """Set up NLM for minimal testing"""
    return ConfigurationPresets.create_config("minimal")


def create_explorer_agent(config: Optional[Config] = None) -> tuple:
    """Create an explorer agent"""
    agent, brain = ConfigurationPresets.create_agent_with_preset("explorer", config)
    return agent, brain


def create_builder_agent(config: Optional[Config] = None) -> tuple:
    """Create a builder agent"""
    agent, brain = ConfigurationPresets.create_agent_with_preset("builder", config)
    return agent, brain


def create_learner_agent(config: Optional[Config] = None) -> tuple:
    """Create a learner agent"""
    agent, brain = ConfigurationPresets.create_agent_with_preset("learner", config)
    return agent, brain


# Add helper methods to ConfigurationPresets class
ConfigurationPresets.create_agent_with_preset = classmethod(
    lambda cls, preset, config=None: (
        globals()[f"create_{preset}_agent"](config) if config else globals()[f"create_{preset}_agent"]()
    )
)


__all__ = [
    "DataExporter", "DataImporter", "MetricsCalculator",
    "SimulationUtilities", "ConfigurationPresets",
    "setup_research_config", "setup_balanced_config", "setup_minimal_config",
    "create_explorer_agent", "create_builder_agent", "create_learner_agent"
]
