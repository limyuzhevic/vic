"""
Pythonic brain simulation class for NLM.

This module provides enhanced Pythonic APIs for brain simulation,
featuring simplified initialization, comprehensive statistics,
and improved error handling.
"""

import sys
from typing import Optional, List, Dict, Any, Union

# Add parent directory to path for imports
if len(sys.path) == 1:
    sys.path.append("..")

from nlm import Brain, Config, NeuronId, NeuronType, RegionId, Action, 
    DevelopmentalStage, SimulationStep, Timestamp, MembranePotential

class BrainError(Exception):
    """Exception raised for brain simulation errors."""
    pass

class BrainWrapper:
    """
    Enhanced Pythonic wrapper for NLM brain simulation.
    
    Provides simplified initialization, comprehensive statistics,
    and improved error handling for neural brain simulation.
    """
    
    def __init__(self, config: Optional[Config] = None, brain: Optional[Brain] = None):
        """
        Initialize brain wrapper.
        
        Args:
            config: Configuration for creating brain. If brain is provided, ignored.
            brain: Existing NLM Brain object to wrap
        """
        if brain is None:
            if config is None:
                config = Config()
            self.brain = Brain(config)
        else:
            self.brain = brain
    
    def initialize(self) -> bool:
        """
        Initialize brain with configuration.
        
        Returns:
            True if initialization successful, False otherwise
        
        Raises:
            BrainError: If initialization fails
        """
        success = self.brain.initialize()
        if not success:
            raise BrainError("Brain initialization failed")
        return success
    
    def step(self, step: Union[int, SimulationStep]) -> None:
        """
        Perform a simulation step.
        
        Args:
            step: Simulation step number (int or SimulationStep)
        """
        self.brain.step(SimulationStep(step))
    
    def step_with_time(self, step: Union[int, SimulationStep], 
                      current_time: Union[float, Timestamp]) -> None:
        """
        Perform a simulation step with timestamp.
        
        Args:
            step: Simulation step number
            current_time: Current simulation time
        """
        self.brain.step(SimulationStep(step), Timestamp(current_time))
    
    def inject_current(self, neuron: NeuronId, current: float) -> None:
        """
        Inject current into specific neuron.
        
        Args:
            neuron: Target neuron ID
            current: Current to inject (MembranePotential)
        """
        self.brain.injectCurrent(neuron, MembranePotential(current))
    
    def inject_current_to_type(self, neuron_type: NeuronType, current: float) -> None:
        """
        Inject current into all neurons of specific type.
        
        Args:
            neuron_type: Target neuron type
            current: Current to inject
        """
        self.brain.injectCurrentToNeurons(neuron_type, MembranePotential(current))
    
    def produce_action(self) -> Optional[Action]:
        """
        Produce motor action based on neural activity.
        
        Returns:
            Action object or None
        """
        action = self.brain.produceAction()
        return action
    
    # Statistics methods
    def get_total_neurons(self) -> int:
        """Get total number of neurons."""
        return self.brain.getTotalNeuronCount()
    
    def get_total_synapses(self) -> int:
        """Get total number of synapses."""
        return self.brain.getTotalSynapseCount()
    
    def get_active_neurons(self) -> int:
        """Get number of active neurons."""
        return self.brain.getActiveNeuronCount()
    
    def get_firing_neurons(self) -> int:
        """Get number of firing neurons."""
        return self.brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate in Hz."""
        return self.brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        return self.brain.getExcitationInhibitionRatio()
    
    def get_total_spikes(self) -> int:
        """Get total spike count."""
        return self.brain.getTotalSpikeCount()
    
    def get_pending_spikes(self) -> int:
        """Get number of pending spike events."""
        return self.brain.getPendingSpikeEventCount()
    
    # Development
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self.brain.getDevelopmentalStage()
    
    def set_developmental_stage(self, stage: DevelopmentalStage) -> None:
        """Set developmental stage."""
        self.brain.setDevelopmentalStage(stage)
    
    def get_learned_preference(self) -> float:
        """Get learned preference trait."""
        return self.brain.getLearnedPreference()
    
    def set_learned_preference(self, preference: float) -> None:
        """Set learned preference trait."""
        self.brain.setLearnedPreference(preference)
    
    # Memory systems
    def get_working_memory(self) -> Any:
        """Get working memory system."""
        return self.brain.getWorkingMemory()
    
    def get_episodic_memory(self) -> Any:
        """Get episodic memory system."""
        return self.brain.getEpisodicMemory()
    
    def get_associative_memory(self) -> Any:
        """Get associative memory system."""
        return self.brain.getAssociativeMemory()
    
    # Prediction system
    def get_prediction_system(self) -> Any:
        """Get prediction system."""
        return self.brain.getPredictionSystem()
    
    # Cognition systems
    def get_planner(self) -> Any:
        """Get neural planner."""
        return self.brain.getPlanner()
    
    def get_concept_formation(self) -> Any:
        """Get concept formation system."""
        return self.brain.getConceptFormation()
    
    def get_attention(self) -> Any:
        """Get attentional selection system."""
        return self.brain.getAttention()
    
    # Development system
    def get_development_system(self) -> Any:
        """Get development system."""
        return self.brain.getDevelopmentSystem()
    
    # Neuromodulation systems
    def get_dopamine(self) -> Any:
        """Get dopamine neuromodulator."""
        return self.brain.getDopamine()
    
    def get_curiosity(self) -> Any:
        """Get curiosity neuromodulator."""
        return self.brain.getCuriosity()
    
    def get_novelty(self) -> Any:
        """Get novelty neuromodulator."""
        return self.brain.getNovelty()
    
    def get_prediction_error_signal(self) -> Any:
        """Get prediction error signal."""
        return self.brain.getPredictionErrorSignal()
    
    # Region management
    def add_region(self, name: str = "") -> RegionId:
        """Add a new neural region."""
        return self.brain.addRegion(name)
    
    def get_region(self, region_id: RegionId) -> Any:
        """Get region by ID."""
        return self.brain.getRegion(region_id)
    
    def get_region_count(self) -> int:
        """Get number of regions."""
 quantifier
    def get_region_ids(self) -> List[RegionId]:
        """Get all region IDs."""
        return self.brain.getRegionIds()
    
    def get_regions(self) -> List[Any]:
        """Get all regions."""
        return self.brain.getRegions()
    
    # Inter-region connections
    def add_inter_region_connection(self, source: RegionId, target: RegionId,
                                   weight: float = 0.0, delay: int = 1) -> None:
        """Add inter-region connection."""
        self.brain.addInterRegionConnection(source, target, weight, delay)
    
    def remove_inter_region_connection(self, source: RegionId, target: RegionId) -> None:
        """Remove inter-region connection."""
        self.brain.removeInterRegionConnection(source, target)
    
    # Save and load
    def save(self, filepath: str) -> bool:
        """
        Save brain state to file (checkpointing).
        
        Args:
            filepath: Path to save brain state
        
        Returns:
            True if successful, False otherwise
        """
        return self.brain.save(filepath)
    
    def load(self, filepath: str) -> bool:
        """
        Load brain state from file.
        
        Args:
            filepath: Path to load brain state from
        
        Returns:
            True if successful, False otherwise
        """
        return self.brain.load(filepath)
    
    # Reset
    def reset(self) -> None:
        """Reset brain state."""
        self.brain.reset()
    
    # Configuration
    def get_config(self) -> Optional[Config]:
        """Get brain configuration."""
        return self.brain.getConfig()
    
    # Random generator
    def get_random_generator(self) -> Any:
        """Get random generator."""
        return self.brain.getRandomGenerator()
    
    # Logging
    def log_status(self) -> None:
        """Log brain status."""
        self.brain.logStatus()
    
    # Clone method
    def clone(self) -> 'BrainWrapper':
        """Create a copy of the brain."""
        # Note: Brain doesn't have a direct clone method
        # This would need to be implemented based on actual Brain class capabilities
        raise NotImplementedError("Brain cloning not yet implemented")
    
    def __repr__(self) -> str:
        """String representation."""
        if hasattr(self.brain, 'getTotalNeuronCount'):
            neurons = self.brain.getTotalNeuronCount()
            synapses = self.brain.getTotalSynapseCount()
            return f"<BrainWrapper: {neurons} neurons, {synapses} synapses>"
        else:
            return "<BrainWrapper: brain>"
    
    def __str__(self)Ser representation."""
        return self.get_status_summary()
    
    def get_status_summary(self) -> str:
        """Get comprehensive status summary."""
        neurons = self.get_total_neurons()
        synapses = self.get_total_synapses()
        firing = self.get_firing_neurons()
        rate = self.get_average_firing_rate()
        stage = self.get_developmental_stage()
        
        return (f"Brain: {neurons} neurons, {synapses} synapses, "
                f"{firing} firing, {rate:.2f} Hz, "
                f"Stage: {stage}")