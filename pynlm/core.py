"""
Core classes for NLM neural simulation framework.
Provides main classes with type hints and Pythonic interfaces.
"""

from typing import Dict, List, Optional, Any, Union
from dataclasses import dataclass
import time
class SimulationStage:
    """Simulation stage enumeration."""
    INITIAL = "initial"
    CRITICAL_PERIOD = "critical_period"
    MATURATION = "maturation"
    ADULT = "adult"
    AGING = "aging"
    
    @classmethod
    def get_all_stages(cls) -> List[str]:
        """Get all simulation stages."""
        return [cls.INITIAL, cls.CRITICAL_PERIOD, cls.MATURATION, cls.ADULT, cls.AGING]
    
    @classmethod
    def is_developmental(cls, stage: str) -> bool:
        """Check if stage is developmental (plasticity active)."""
        return stage in [cls.CRITICAL_PERIOD, cls.MATURATION]
    
    @classmethod
    def get_plasticity_multiplier(cls, stage: str) -> float:
        """Get plasticity multiplier for stage."""
        multipliers = {
            cls.INITIAL: 1.0,
            cls.CRITICAL_PERIOD: 2.0,
            cls.MATURATION: 1.5,
            cls.ADULT: 1.0,
            cls.AGING: 0.8
        }
        return multipliers.get(stage, 1.0)
@dataclass
class NeuronMetrics:
    """Metrics for neuron performance."""
    
    total_neurons: int = 0
    active_neurons: int = 0
    firing_neurons: int = 0
    total_spikes: int = 0
    avg_firing_rate: float = 0.0
    excitation_inhibition_ratio: float = 1.0
    efficiency_score: float = 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary."""
        return {
            "total_neurons": self.total_neurons,
            "active_neurons": self.active_neurons,
            "firing_neurons": self.firing_neurons,
            "total_spikes": self.total_spikes,
            "avg_firing_rate": self.avg_firing_rate,
            "excitation_inhibition_ratio": self.excitation_inhibition_ratio,
            "efficiency_score": self.efficiency_score
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "NeuronMetrics":
        """Create metrics from dictionary."""
        return cls(**data)
class Brain:
    """Enhanced neural simulation brain class.
    
    This class provides a Pythonic interface to the NLM brain simulation engine.
    It includes context managers, batch processing, and advanced operations.
    """
    
    def __init__(self, config=None):
        """Initialize brain with optional configuration.
        
        Args:
            config: Configuration object or dictionary
        """
        self._config = config or self._create_default_config()
        self._neurons = []
        self._synapses = []
        self._regions = []
        self._is_initialized = False
        self._step_count = 0
        self._last_updated = time.time()
        
        # Initialize with configuration
        self._initialize_brain()
    
    def _create_default_config(self) -> Dict[str, Any]:
        """Create default configuration."""
        return {
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "brain.connection_probability": 0.05,
            "brain.initial_weight_mean": 0.5,
            "brain.initial_weight_std": 0.1,
            "brain.v_thresh": -50.0,
            "brain.v_rest": -70.0,
            "brain.v_reset": -75.0,
            "brain.tau_mem": 20.0,
            "brain.tau_ref": 2.0,
            "plasticity.stdp.enable": True,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "development.enable": True,
            "development.critical_period_duration": 100,
            "development.adult_age": 1000,
            "memory.working_memory.enable": True,
            "memory.episodic_memory.enable": True,
            "memory.associative_memory.enable": True,
            "prediction.enable": True,
            "cognition.enable": True
        }
    
    def _initialize_brain(self) -> None:
        """Initialize brain with neurons and connections."""
        neuron_count = self._config.get("brain.neuron_count", 1000)
        
        # Create neurons based on configuration
        self._neurons = []
        for i in range(neuron_count):
            neuron = self._create_neuron(i)
            self._neurons.append(neuron)
        
        # Create synaptic connections based on density
        self._create_connections()
        
        self._is_initialized = True
        self._last_updated = time.time()
    
    def _create_neuron(self, neuron_id: int) -> Dict[str, Any]:
        """Create a single neuron."""
        neuron_type = self._get_neuron_type(neuron_id)
        
        return {
            "id": neuron_id,
            "type": neuron_type,
            "v": -70.0,  # Resting potential
            "refractory": 0.0,
            "last_spike": -1000.0,
            "connections": [],
            "active": False,
            "fired_this_step": False
        }
    
    def _get_neuron_type(self, neuron_id: int) -> str:
        """Get neuron type based on ID and configuration."""
        # Simple distribution: 60% excitatory, 20% inhibitory, 20% sensory
        if neuron_id % 5 == 0:
            return "inhibitory"
        elif neuron_id % 3 == 0:
            return "sensory"
        else:
            return "excitatory"
    
    def _create_connections(self) -> None:
        """Create synaptic connections between neurons."""
        if not self._neurons:
            return
        
        density = self._config.get("brain.synapse_density", 0.1)
        num_neurons = len(self._neurons)
        
        for i, neuron in enumerate(self._neurons):
            # For each neuron, create connections to other neurons
            for j in range(num_neurons):
                if i == j:
                    continue
                
                # Randomly decide if connection exists based on density
                if self._random() < density:
                    weight = self._random_normal(
                        self._config.get("brain.initial_weight_mean", 0.5),
                        self._config.get("brain.initial_weight_std", 0.1)
                    )
                    
                    synapse = {
                        "source": i,
                        "target": j,
                        "weight": weight,
                        "delay": self._random() * 10.0,  # Random delay up to 10ms
                        "active": True,
                        "last_update": time.time()
                    }
                    
                    neuron["connections"].append(synapse)
                    self._synapses.append(synapse)
    
    def _random(self) -> float:
        """Get random float between 0 and 1."""
        import random
        return random.random()
    
    def _random_normal(self, mean: float, std: float) -> float:
        """Get random value from normal distribution."""
        import random
        return random.gauss(mean, std)
    
    def step(self, step_number: int, current_time: Optional[float] = None) -> None:
        """Perform a simulation step.
        
        Args:
            step_number: Current step number
            current_time: Current simulation time (defaults to step number)
        """
        if not self._is_initialized:
            self._initialize_brain()
        
        if current_time is None:
            current_time = float(step_number)
        
        # Reset firing state
        for neuron in self._neurons:
            neuron["fired_this_step"] = False
            neuron["active"] = False
        
        # Update neurons
        self._update_neurons(current_time)
        
        # Propagate spikes
        self._propagate_spikes(current_time)
        
        # Update connections
        self._update_connections(current_time)
        
        # Apply plasticity
        self._apply_plasticity(current_time)
        
        # Update development
        self._update_development(current_time)
        
        # Update statistics
        self._step_count += 1
        self._last_updated = time.time()
    
    def _update_neurons(self, current_time: float) -> None:
        """Update neuron membrane potentials."""
        for neuron in self._neurons:
            if neuron["refractory"] > 0:
                neuron["refractory"] -= 0.1
                neuron["v"] = neuron["v_reset"]
            else:
                # Simple LIF update
                neuron["v"] += self._get_input(neuron["id"]) - (neuron["v"] - neuron["v_rest"]) * 0.1
                
                # Check for spike
                if neuron["v"] >= self._config.get("brain.v_thresh", -50.0):
                    neuron["v"] = neuron["v_reset"]
                    neuron["refractory"] = self._config.get("brain.tau_ref", 2.0)
                    neuron["last_spike"] = current_time
                    neuron["active"] = True
    
    def _get_input(self, neuron_id: int) -> float:
        """Calculate total input to neuron from all synapses."""
        total_input = 0.0
        
        for synapse in self._synapses:
            if synapse["target"] == neuron_id and synapse["active"]:
                # Add weighted input with delay consideration
                time_since_last = current_time - synapse["last_update"]
                if time_since_last >= synapse["delay"]:
                    total_input += synapse["weight"]
        
        return total_input
    
    def _propagate_spikes(self, current_time: float) -> None:
        """Propagate spikes to target neurons."""
        for neuron in self._neurons:
            if neuron["active"]:
                # Process outgoing synapses
                for synapse in neuron["connections"]:
                    synapse["last_update"] = current_time
                    # Spike arrives at target neuron (will be processed in next step)
    
    def _update_connections(self, current_time: float) -> None:
        """Update connection states and delays."""
        for synapse in self._synapses:
            # Check if delay has passed
            time_since_last = current_time - synapse["last_update"]
            if time_since_last >= synapse["delay"]:
                synapse["last_update"] = current_time
    
    def _apply_plasticity(self, current_time: float) -> None:
        """Apply synaptic plasticity rules."""
        # Simple STDP implementation
        if self._config.get("plasticity.stdp.enable", True):
            learning_rate = self._config.get("plasticity.stdp.learning_rate", 0.001)
            
            for synapse in self._synapses:
                if synapse["active"]:
                    # Check for spike timing dependent plasticity
                    time_diff = abs(current_time - synapse["last_update"])
                    if time_diff < 20.0:  # Within STDP window
                        # Apply STDP rule
                        if current_time > synapse["last_update"]:
                            synapse["weight"] += learning_rate * time_diff
                        else:
                            synapse["weight"] -= learning_rate * time_diff
                    
                    # Ensure weight stays in valid range
                    synapse["weight"] = max(-1.0, min(1.0, synapse["weight"]))
    
    def _update_development(self, current_time: float) -> None:
        """Update developmental state."""
        if self._config.get("development.enable", True):
            age = self._step_count
            stage = self._get_developmental_stage(age)
            
            # Adjust plasticity based on developmental stage
            if stage == "critical_period":
                # Enhanced plasticity during critical period
                for synapse in self._synapses:
                    if synapse["active"]:
                        synapse["weight"] *= 1.2
            elif stage == "maturation":
                # Gradual maturation
                for synapse in self._synapses:
                    if synapse["active"]:
                        synapse["weight"] *= 1.05
    
    def _get_developmental_stage(self, step_count: int) -> str:
        """Get developmental stage based on step count."""
        critical_period_end = self._config.get("development.critical_period_duration", 100)
        adult_age = self._config.get("development.adult_age", 1000)
        
        if step_count < critical_period_end:
            return "critical_period"
        elif step_count < adult_age:
            return "maturation"
        else:
            return "adult"
    
    def initialize(self) -> None:
        """Initialize the brain with configuration."""
        if not self._is_initialized:
            self._initialize_brain()
    
    def reset(self) -> None:
        """Reset brain to initial state."""
        self._neurons = []
        self._synapses = []
        self._regions = []
        self._is_initialized = False
        self._step_count = 0
        self._last_updated = time.time()
    
    # Getter methods
    def getTotalNeuronCount(self) -> int:
        """Get total neuron count."""
        return len(self._neurons)
    
    def getTotalSynapseCount(self) -> int:
        """Get total synapse count."""
        return len(self._synapses)
    
    def getActiveNeuronCount(self) -> int:
        """Get count of active neurons."""
        return sum(1 for neuron in self._neurons if neuron["active"])
    
    def getFiringNeuronCount(self) -> int:
        """Get count of currently firing neurons."""
        return sum(1 for neuron in self._neurons if neuron["fired_this_step"])
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons."""
        if not self._neurons:
            return 0.0
        
        total_firing = sum(1 for neuron in self._neurons if neuron["fired_this_step"])
        return total_firing / len(self._neurons)
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get excitation/inhibition balance ratio."""
        exc_count = sum(1 for neuron in self._neurons if neuron["type"] == "excitatory")
        inh_count = sum(1 for neuron in self._neurons if neuron["type"] == "inhibitory")
        
        if exc_count + inh_count == 0:
            return 1.0
        
        return exc_count / max(1, inh_count)
    
    def getTotalSpikeCount(self) -> int:
        """Get total spike count."""
        return sum(1 for neuron in self._neurons if neuron["active"])
    
    def getDevelopmentalStage(self) -> str:
        """Get current developmental stage."""
        return self._get_developmental_stage(self._step_count)
    
    def getConfig(self) -> Dict[str, Any]:
        """Get the configuration."""
        return self._config.copy()
    
    # Configuration methods
    def set(self, key: str, value: Any) -> None:
        """Set configuration value.
        
        Args:
            key: Configuration key (dot notation)
            value: Configuration value
        """
        self._config[key] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value.
        
        Args:
            key: Configuration key (dot notation)
            default: Default value if key not found
            
        Returns:
            Configuration value
        """
        return self._config.get(key, default)
    
    def has(self, key: str) -> bool:
        """Check if configuration key exists.
        
        Args:
            key: Configuration key (dot notation)
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._config
    
    # Simulation control methods
    def stepWithTime(self, step_number: int, current_time: float) -> None:
        """Perform a simulation step with explicit timestamp.
        
        Args:
            step_number: Current step number
            current_time: Current simulation time
        """
        self.step(step_number, current_time)
    
    def runBatch(self, steps: int) -> "SimulationStats":
        """Run batch processing with performance statistics.
        
        Args:
            steps: Number of steps to run
            
        Returns:
            Simulation statistics
        """
        return run_batch(self, steps)
    
    # Advanced operations
    def moveToward(self, target_x: float, target_y: float, speed: float) -> Dict[str, Any]:
        """Move brain output toward target position.
        
        Args:
            target_x: Target x-coordinate
            target_y: Target y-coordinate
            speed: Movement speed
            
        Returns:
            Movement action
        """
        # Create movement action based on target
        return {
            "type": "move_toward",
            "target_x": target_x,
            "target_y": target_y,
            "speed": speed,
            "step": self._step_count
        }
    
    def isNear(self, x: float, y: float, threshold: float) -> bool:
        """Check if brain output is near target position.
        
        Args:
            x: Target x-coordinate
            y: Target y-coordinate
            threshold: Distance threshold
            
        Returns:
            True if near target, False otherwise
        """
        # Simple distance calculation
        distance = ((x - self._step_count) ** 2 + (y - self._step_count) ** 2) ** 0.5
        return distance <= threshold
    
    def findNearbyObjects(self, radius: float) -> List[Dict[str, Any]]:
        """Find nearby objects within radius.
        
        Args:
            radius: Search radius
            
        Returns:
            List of nearby objects
        """
        # Return empty list for now (object detection not implemented)
        return []
    
    def interactWithObject(self, object_id: str) -> Dict[str, Any]:
        """Interact with object by ID.
        
        Args:
            object_id: Object identifier
            
        Returns:
            Interaction result
        """
        return {
            "type": "interact",
            "object_id": object_id,
            "success": False,
            "step": self._step_count
        }
    
    # Statistics and metrics
    def getMetrics(self) -> NeuronMetrics:
        """Get neuron metrics.
        
        Returns:
            Neuron metrics object
        """
        metrics = NeuronMetrics(
            total_neurons=self.getTotalNeuronCount(),
            active_neurons=self.getActiveNeuronCount(),
            firing_neurons=self.getFiringNeuronCount(),
            total_spikes=self.getTotalSpikeCount(),
            avg_firing_rate=self.getAverageFiringRate(),
            excitation_inhibition_ratio=self.getExcitationInhibitionRatio(),
            efficiency_score=self._calculate_efficiency_score()
        )
        
        return metrics
    
    def _calculate_efficiency_score(self) -> float:
        """Calculate efficiency score."""
        neurons = self.getTotalNeuronCount()
        active = self.getActiveNeuronCount()
        firing = self.getFiringNeuronCount()
        
        if neurons == 0:
            return 0.0
        
        return (active + firing) / (neurons * 2)
    
    def getSimulationTime(self) -> float:
        """Get current simulation time."""
        return self._last_updated
    
    # Save/load (stub implementations)
    def save(self, filepath: str) -> bool:
        """Save brain state to file.
        
        Args:
            filepath: File path to save to
            
        Returns:
            True if saved successfully, False otherwise
        """
        try:
            # Placeholder for save implementation
            print(f"Saving brain state to {filepath}")
            return True
        except Exception:
            return False
    
    def load(self, filepath: str) -> bool:
        """Load brain state from file.
        
        Args:
            filepath: File path to load from
            
        Returns:
            True if loaded successfully, False otherwise
        """
        try:
            # Placeholder for load implementation
            print(f"Loading brain state from {filepath}")
            return True
        except Exception:
            return False
    
    # Logging and diagnostics
    def logStatus(self) -> None:
        """Log brain status information."""
        print(f"Brain Status at step {self._step_count}:")
        print(f"  Neurons: {self.getTotalNeuronCount()}")
        print(f"  Synapses: {self.getTotalSynapseCount()}")
        print(f"  Active neurons: {self.getActiveNeuronCount()}")
        print(f"  Firing neurons: {self.getFiringNeuronCount()}")
        print(f"  Average firing rate: {self.getAverageFiringRate():.3f}")
        print(f"  Developmental stage: {self.getDevelopmentalStage()}")
    
    # String representations
    def __repr__(self) -> str:
        return f"<Brain: {self.getTotalNeuronCount()} neurons, {self.getTotalSynapseCount()} synapses, stage={self.getDevelopmentalStage()}>"

    def __str__(self) -> str:
        return self.__repr__()
# Global functions for backward compatibility
def createBrain(config=None) -> Brain:
    """Create a brain with optional configuration.
    
    Args:
        config: Configuration object or dictionary
        
    Returns:
        Brain instance
    """
    return Brain(config)

def createDefaultConfig() -> Dict[str, Any]:
    """Create a default configuration.
    
    Returns:
        Default configuration dictionary
    """
    brain = Brain()
    return brain.getConfig()
# Global simulation functions
def run_batch(brain: Brain, steps: int) -> "SimulationStats":
    """Run batch processing with performance statistics.
    
    Args:
        brain: Brain instance
        steps: Number of steps to run
        
    Returns:
        Simulation statistics
    """
    import time
    
    start_time = time.time()
    spikes_generated = 0
    neurons_active = 0
    
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
        spikes_generated += brain.getTotalSpikeCount()
        neurons_active += brain.getFiringNeuronCount()
    
    end_time = time.time()
    duration = end_time - start_time
    
    stats = SimulationStats(
        steps_executed=steps,
        total_time_seconds=duration,
        steps_per_second=steps / duration if duration > 0 else 0,
        spikes_generated=spikes_generated,
        neurons_active=neurons_active,
        memory_traces=0,
        episodic_episodes=0
    )
    
    return stats
def run_episode(agent: AgentBrain, world: SimpleWorld, steps: int) -> "SimulationStats":
    """Run episode simulation with agent and world.
    
    Args:
        agent: Agent brain instance
        world: World instance
        steps: Number of steps to run
        
    Returns:
        Simulation statistics
    """
    import time
    
    start_time = time.time()
    
    agent.initialize(world)
    
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward_percept = world.getSensoryPercept()
        reward = 0.0
        if reward_percept.getInternal() and not reward_percept.getInternal()->empty():
            reward = (*reward_percept.getInternal())[0]
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    end_time = time.time()
    duration = end_time - start_time
    
    stats = SimulationStats(
        steps_executed=steps,
        total_time_seconds=duration,
        steps_per_second=steps / duration if duration > 0 else 0,
        spikes_generated=agent.getBrain()->getTotalSpikeCount(),
        neurons_active=agent.getBrain()->getFiringNeuronCount(),
        memory_traces=0,
        episodic_episodes=0
    )
    
    return stats
# Import additional classes that will be defined later
from .agent import AgentBrain
from .sensory import SimpleWorld, SensoryPercept
from .simulation import BrainContextManager, AgentBrainContextManager, SimpleWorldContextManager
from .profiler import SimulationStats

__all__ = [
    "Brain",
    "SimulationStage",
    "NeuronMetrics",
    "createBrain",
    "createDefaultConfig",
    "run_batch",
    "run_episode"
]
