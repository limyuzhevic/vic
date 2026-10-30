# Enhanced NLM Python Wrapper
# This file provides a more Pythonic and user-friendly API for the NLM brain simulation framework.
# It builds upon the C++ bindings (pynlm module) to provide additional convenience methods,
# better error handling, configuration validation, and simulation utilities.

import json
import time
import threading
from typing import List, Dict, Any, Optional, Union, Callable
from enum import Enum
import numpy as np

# Import the core C++ bindings
import pynlm

class NLMError(Exception):
    """Base exception for NLM-related errors"""
    pass

class ConfigurationError(NLMError):
    """Error in configuration"""
    pass

class SimulationError(NLMError):
    """Error during simulation"""
    pass

class ActionType(Enum):
    """Python enum for action types (easier to use than C++ enum)"""
    MOVE_FORWARD = pynlm.ActionType.MoveForward
    MOVE_BACKWARD = pynlm.ActionType.MoveBackward
    MOVE_LEFT = pynlm.ActionType.MoveLeft
    MOVE_RIGHT = pynlm.ActionType.MoveRight
    TURN_LEFT = pynlm.ActionType.TurnLeft
    TURN_RIGHT = pynlm.ActionType.TurnRight
    LOOK = pynlm.ActionType.Look
    LOOK_UP = pynlm.ActionType.LookUp
    LOOK_DOWN = pynlm.ActionType.LookDown
    INTERACT = pynlm.ActionType.Interact
    EAT = pynlm.ActionType.Eat
    DRINK = pynlm.ActionType.Drink
    REST = pynlm.ActionType.Rest
    WAIT = pynlm.ActionType.Wait
    CUSTOM = pynlm.ActionType.Custom

class DevelopmentalStage(Enum):
    """Python enum for developmental stages"""
    INITIAL = pynlm.DevelopmentalStage.Initial
    CRITICAL_PERIOD = pynlm.DevelopmentalStage.CriticalPeriod
    MATURATION = pynlm.DevelopmentalStage.Maturation
    ADULT = pynlm.DevelopmentalStage.Adult
    AGING = pynlm.DevelopmentalStage.Aging

class ConfigValidator:
    """Validates NLM configuration parameters"""
    
    DEFAULT_SCHEMA = {
        "neuron_count": {"type": "int", "min": 10, "max": 10000, "default": 500},
        "region_count": {"type": "int", "min": 1, "max": 100, "default": 1},
        "connection_probability": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.15},
        "random_seed": {"type": "int", "min": 0, "max": 2147483647, "default": 42},
        "simulation_timestep": {"type": "float", "min": 0.0001, "max": 1.0, "default": 0.001},
        "stdp_ltp_weight": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.02},
        "stdp_ltd_weight": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.015},
        "stdp_tau": {"type": "float", "min": 1.0, "max": 1000.0, "default": 20.0},
        "synaptogenesis_rate": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.0001},
        "pruning_rate": {"type": "float", "min": 0.0, "max": 1.0, "default": 0.00001},
    }
    
    @staticmethod
    def validate_config(config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate and normalize configuration"""
        validated = {}
        
        for key, schema in ConfigValidator.DEFAULT_SCHEMA.items():
            if key in config:
                value = config[key]
                expected_type = schema["type"]
                
                # Type conversion and validation
                if expected_type == "int":
                    try:
                        value = int(value)
                    except (ValueError, TypeError):
                        raise ConfigurationError(f"Config parameter '{key}' must be an integer, got {type(value)}")
                    
                    if value < schema.get("min", float("-inf")):
                        raise ConfigurationError(f"Config parameter '{key}' must be >= {schema['min']}")
                    if value > schema.get("max", float("inf")):
                        raise ConfigurationError(f"Config parameter '{key}' must be <= {schema['max']}")
                
                elif expected_type == "float":
                    try:
                        value = float(value)
                    except (ValueError, TypeError):
                        raise ConfigurationError(f"Config parameter '{key}' must be a number, got {type(value)}")
                    
                    if value < schema.get("min", float("-inf")):
                        raise ConfigurationError(f"Config parameter '{key}' must be >= {schema['min']}")
                    if value > schema.get("max", float("inf")):
                        raise ConfigurationError(f"Config parameter '{key}' must be <= {schema['max']}")
                
                validated[key] = value
            else:
                # Use default value if not provided
                validated[key] = schema.get("default")
        
        return validated

class SensoryPerceptWrapper:
    """Enhanced wrapper for SensoryPercept with Pythonic API"""
    
    def __init__(self, percept: pynlm.SensoryPercept):
        self._percept = percept
        self._data = None
    
    def get_vision(self, width: Optional[int] = None, height: Optional[int] = None) -> np.ndarray:
        """Get vision data as numpy array"""
        return np.array(self._percept.getVision())
    
    def get_vision_shape(self) -> tuple:
        """Get vision data shape (width, height, channels)"""
        vision = self._percept.getVision()
        return (self._percept.getVisionWidth(), 
                self._percept.getVisionHeight(),
                3 if vision.size >= 3 else 1)
    
    def get_touch(self) -> np.ndarray:
        """Get touch data as numpy array"""
        return np.array(self._percept.getTouch())
    
    def get_internal(self) -> np.ndarray:
        """Get internal signals as numpy array"""
        return np.array(self._percept.getInternal())
    
    def get_proprioception(self) -> np.ndarray:
        """Get proprioception data as numpy array"""
        return np.array(self._percept.getProprioception())
    
    def get_audio(self) -> np.ndarray:
        """Get audio data as numpy array"""
        return np.array(self._percept.getAudio())
    
    def get_all_signals(self) -> np.ndarray:
        """Get all sensory signals flattened as numpy array"""
        return np.array(self._percept.getAllSignals())
    
    def get_timestamp(self) -> float:
        """Get timestamp of this percept"""
        return self._percept.getTimestamp()
    
    def set_timestamp(self, timestamp: float):
        """Set timestamp of this percept"""
        self._percept.setTimestamp(timestamp)
    
    def __repr__(self):
        vision_shape = self.get_vision_shape()
        return (f"<SensoryPercept vision={vision_shape}, touch={len(self.get_touch())}, "
                f"internal={len(self.get_internal())}, proprioception={len(self.get_proprioception())}, "
                f"audio={len(self.get_audio())}, timestamp={self.get_timestamp()}>"

class ActionWrapper:
    """Enhanced wrapper for Action with Pythonic API"""
    
    def __init__(self, action: pynlm.Action):
        self._action = action
    
    def get_type(self) -> ActionType:
        """Get action type as ActionType enum"""
        return ActionType(self._action.getType())
    
    def set_type(self, action_type: ActionType):
        """Set action type from ActionType enum"""
        self._action.setType(action_type.value)
    
    def get_parameters(self) -> np.ndarray:
        """Get action parameters as numpy array"""
        return np.array(self._action.getParameters())
    
    def set_parameters(self, params: Union[List[float], np.ndarray]):
        """Set action parameters from list or numpy array"""
        self._action.setParameters(list(params) if isinstance(params, np.ndarray) else params)
    
    def get_name(self) -> str:
        """Get human-readable action name"""
        return self._action.getName()
    
    def __repr__(self):
        return f"<Action type={self.get_type().name} params={self.get_parameters().tolist()}>"

class WorldWrapper:
    """Enhanced wrapper for SimpleWorld with Pythonic API"""
    
    def __init__(self, world: pynlm.SimpleWorld):
        self._world = world
        self._last_percept = None
    
    def configure(self, width: int, height: int, vision_width: int, vision_height: int):
        """Configure the world"""
        self._world.configure(width, height, vision_width, vision_height)
    
    def reset(self):
        """Reset world state"""
        self._world.reset()
        self._last_percept = None
    
    def update(self, timestep: float = 0.1):
        """Update world for one timestep"""
        self._world.update(timestep)
        self._last_percept = self.get_sensory_percept()
    
    def apply_motor_command(self, action: ActionWrapper, timestep: Optional[float] = None):
        """Apply motor command from action wrapper"""
        if timestep is None:
            timestep = self._world.getSimulationTime()
        self._world.applyMotorCommand(action._action, timestep)
    
    def get_sensory_percept(self) -> SensoryPerceptWrapper:
        """Get sensory percept as wrapper"""
        if self._last_percept is None:
            self._last_percept = self._world.getSensoryPercept()
        return SensoryPerceptWrapper(self._last_percept)
    
    def get_agent_body(self) -> Dict[str, float]:
        """Get agent body state as dictionary"""
        body = self._world.getAgentBody()
        return {
            'x': body.x,
            'y': body.y,
            'orientation': body.orientation,
            'velocity_x': body.velocityX,
            'velocity_y': body.velocityY,
            'angular_velocity': body.angularVelocity,
            'energy': body.energy,
            'health': body.health,
            'age': body.age,
            'is_moving': body.isMoving,
            'is_turning': body.isTurning,
            'last_action_time': body.lastActionTime
        }
    
    def add_world_object(self, obj: Dict[str, Any]):
        """Add a world object from dictionary"""
        world_obj = pynlm.WorldObject(
            obj['x'], obj['y'], 
            pynlm.WorldObjectType(obj['type']), 
            obj.get('value', 0.0),
            obj.get('radius', 0.5)
        )
        world_obj.active = obj.get('active', True)
        self._world.addObject(world_obj)
    
    def remove_world_object(self, x: float, y: float):
        """Remove world object at position"""
        self._world.removeObject(x, y)
    
    def is_valid_position(self, x: float, y: float) -> bool:
        """Check if position is valid"""
        return self._world.isValidPosition(x, y)
    
    def get_stats(self) -> Dict[str, Any]:
        """Get world statistics"""
        return {
            'width': self._world.getWidth(),
            'height': self._world.getHeight(),
            'max_energy': self._world.getMaxEnergy(),
            'energy_decay_rate': self._world.getEnergyDecayRate(),
            'simulation_time': self._world.getSimulationTime(),
            'random_seed': self._world.getRandomSeed()
        }
    
    def set_random_seed(self, seed: int):
        """Set random seed for reproducibility"""
        self._world.setRandomSeed(seed)
    
    def __repr__(self):
        stats = self.get_stats()
        return f"<WorldWrapper width={stats['width']}, height={stats['height']}, " \
               f"time={stats['simulation_time']:.3f}, seed={stats['random_seed']}>"

class AgentWrapper:
    """Enhanced wrapper for AgentBrain with Pythonic API"""
    
    def __init__(self, agent: pynlm.AgentBrain, brain: 'BrainWrapper', world: 'WorldWrapper'):
        self._agent = agent
        self._brain = brain
        self._world = world
    
    def process_sensory_input(self, percept: SensoryPerceptWrapper):
        """Process sensory input"""
        self._agent.processSensoryInput(percept._percept)
    
    def decode_motor_command(self) -> ActionWrapper:
        """Decode brain activity into motor command"""
        return ActionWrapper(self._agent.decodeMotorCommand())
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float):
        """Apply reward-based neuromodulation"""
        self._agent.applyRewardModulation(reward, predicted_reward)
    
    def update_development(self, timestep: float):
        """Update development system"""
        self._agent.updateDevelopment(timestep)
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
        return DevelopmentalStage(self._agent.getDevelopmentalStage())
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level"""
        return self._agent.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level"""
        return self._agent.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level"""
        return self._agent.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get prediction error"""
        return self._agent.getPredictionError()
    
    def reset(self):
        """Reset agent for new episode"""
        self._agent.reset()
    
    # Feature toggles for learning and development
    def enable_reward_modulation(self, enabled: bool):
        """Enable/disable reward modulation"""
        self._agent.enableRewardModulation(enabled)
    
    def enable_structural_plasticity(self, enabled: bool):
        """Enable/disable structural plasticity"""
        self._agent.enableStructuralPlasticity(enabled)
    
    def enable_development(self, enabled: bool):
        """Enable/disable development"""
        self._agent.enableDevelopment(enabled)
    
    def enable_curiosity(self, enabled: bool):
        """Enable/disable curiosity"""
        self._agent.enableCuriosity(enabled)
    
    def is_reward_modulation_enabled(self) -> bool:
        """Check if reward modulation is enabled"""
        return self._agent.isRewardModulationEnabled()
    
    def is_structural_plasticity_enabled(self) -> bool:
        """Check if structural plasticity is enabled"""
        return self._agent.isStructuralPlasticityEnabled()
    
    def is_development_enabled(self) -> bool:
        """Check if development is enabled"""
        return self._agent.isDevelopmentEnabled()
    
    def is_curiosity_enabled(self) -> bool:
        """Check if curiosity is enabled"""
        return self._agent.isCuriosityEnabled()
    
    def __repr__(self):
        return f"<AgentWrapper stage={self.get_developmental_stage().name}>"

class BrainWrapper:
    """Enhanced wrapper for Brain with Pythonic API"""
    
    def __init__(self, brain: pynlm.Brain):
        self._brain = brain
    
    def initialize(self, config: Optional[Dict[str, Any]] = None):
        """Initialize brain with optional configuration"""
        if config:
            # Create a temporary Config object and set parameters
            temp_config = pynlm.createDefaultConfig()
            for key, value in config.items():
                temp_config.set(key, value)
        return self._brain.initialize()
    
    def step(self, step_number: Optional[int] = None, time: Optional[float] = None):
        """Advance brain simulation by one step"""
        if time is not None:
            self._brain.step(step_number if step_number is not None else 0, time)
        else:
            self._brain.step(step_number if step_number is not None else 0)
    
    def receive_sensory_input(self, percept: SensoryPerceptWrapper):
        """Receive sensory input from environment"""
        self._brain.receiveSensoryInput(percept._percept)
    
    def inject_current(self, neuron_id: int, current: float):
        """Inject current into specific neuron"""
        self._brain.injectCurrent(neuron_id, current)
    
    def inject_current_to_neurons(self, neuron_type: str, current: float):
        """Inject current into all neurons of specified type"""
        # Convert string to NeuronType enum
        type_map = {
            'excitatory': pynlm.NeuronType.Excitatory,
            'inhibitory': pynlm.NeuronType.Inhibitory,
            'modulatory': pynlm.NeuronType.Modulatory,
            'sensory': pynlm.NeuronType.Sensory,
            'motor': pynlm.NeuronType.Motor,
            'internal': pynlm.NeuronType.Internal
        }
        if neuron_type.lower() not in type_map:
            raise NLMError(f"Unknown neuron type: {neuron_type}")
        
        self._brain.injectCurrentToNeurons(type_map[neuron_type.lower()], current)
    
    def produce_action(self) -> ActionWrapper:
        """Produce motor action based on neural activity"""
        return ActionWrapper(self._brain.produceAction())
    
    def reset(self):
        """Reset brain state"""
        self._brain.reset()
    
    def save(self, filepath: str):
        """Save brain state to file (checkpointing)"""
        return self._brain.save(filepath)
    
    def load(self, filepath: str):
        """Load brain state from file"""
        return self._brain.load(filepath)
    
    def add_region(self, name: str = "") -> int:
        """Add a new neural region, return its ID"""
        return self._brain.addRegion(name)
    
    def get_region(self, region_id: int):
        """Get region by ID (returns raw C++ object)"""
        return self._brain.getRegion(region_id)
    
    def get_region_count(self) -> int:
        """Get number of regions"""
        return self._brain.getRegionCount()
    
    def get_region_ids(self) -> List[int]:
        """Get all region IDs"""
        return self._brain.getRegionIds()
    
    def get_regions(self):
        """Get all regions (returns raw C++ objects)"""
        return self._brain.getRegions()
    
    # Statistics and monitoring
    def get_total_neuron_count(self) -> int:
        """Get total neuron count across all regions"""
        return self._brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count across all regions"""
        return self._brain.getTotalSynapseCount()
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons"""
        return self._brain.getActiveNeuronCount()
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons"""
        return self._brain.getFiringNeuronCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate across all neurons"""
        return self._brain.getAverageFiringRate()
    
    def get_excitation_inhibition_ratio(self) -> float:
        """Get excitation/inhibition balance ratio"""
        return self._brain.getExcitationInhibitionRatio()
    
    def get_total_spike_count(self) -> int:
        """Get total spike count"""
        return self._brain.getTotalSpikeCount()
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
        return DevelopmentalStage(self._brain.getDevelopmentalStage())
    
    def set_developmental_stage(self, stage: DevelopmentalStage):
        """Set developmental stage"""
        self._brain.setDevelopmentalStage(stage.value)
    
    def get_config(self) -> Dict[str, Any]:
        """Get configuration as dictionary (if available)"""
        # Note: This depends on whether the C++ Config class has a method to export to dict
        # For now, return a minimal representation
        config = self._brain.getConfig()
        return {"has_config": config is not None}
    
    def log_status(self):
        """Log brain status"""
        self._brain.logStatus()
    
    def get_spike_system(self):
        """Get spike system (returns raw C++ object)"""
        return self._brain.getSpikeSystem()
    
    def get_stdp(self):
        """Get STDP plasticity system (returns raw C++ object)"""
        return self._brain.getSTDP()
    
    def get_hebbian(self):
        """Get Hebbian plasticity system (returns raw C++ object)"""
        return self._brain.getHebbian()
    
    def get_structural_plasticity(self):
        """Get structural plasticity system (returns raw C++ object)"""
        return self._brain.getStructuralPlasticity()
    
    def update_plasticity(self):
        """Update plasticity rules (called automatically in step)"""
        self._brain.updatePlasticity()
    
    def develop(self):
        """Apply developmental changes (called automatically in step)"""
        self._brain.develop()
    
    def __repr__(self):
        return f"<BrainWrapper neurons={self.get_total_neuron_count()}, " \
               f"synapses={self.get_total_synapse_count()}, " \
               f"stage={self.get_developmental_stage().name}>"

# Global configuration and simulation utilities
_simulation_running = False
_simulation_thread = None
_simulation_callbacks = []

def create_brain(config: Optional[Dict[str, Any]] = None) -> BrainWrapper:
    """Create a new brain with optional configuration"""
    # Validate configuration if provided
    if config:
        config = ConfigValidator.validate_config(config)
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    return BrainWrapper(brain)

def create_world() -> WorldWrapper:
    """Create a new simple world"""
    world = pynlm.createSimpleWorld()
    return WorldWrapper(world)

def create_agent_brain(brain: BrainWrapper, world: WorldWrapper) -> AgentWrapper:
    """Create agent brain interface"""
    agent = pynlm.createAgentBrain(brain._brain)
    return AgentWrapper(agent, brain, world)

def add_simulation_callback(callback: Callable[[Dict[str, Any]], None], event_type: str = "step"):
    """Add a callback function for simulation events"""
    _simulation_callbacks.append((callback, event_type))

def remove_simulation_callback(callback: Callable):
    """Remove a simulation callback"""
    _simulation_callbacks = [cb for cb in _simulation_callbacks if cb[0] != callback]

def run_simulation(
    brain: BrainWrapper,
    world: WorldWrapper,
    agent: AgentWrapper,
    num_steps: int,
    timestep: float = 0.1,
    callbacks_enabled: bool = True
) -> Dict[str, Any]:
    """Run a complete simulation with optional callbacks"""
    global _simulation_running
    
    _simulation_running = True
    start_time = time.time()
    
    results = {
        'steps_completed': 0,
        'total_spikes': 0,
        'max_firing_rate': 0.0,
        'final_energy': 0.0,
        'learning_detected': False,
        'step_data': []
    }
    
    try:
        for step in range(num_steps):
            if not _simulation_running:
                break
            
            # 1. Update world
            world.update(timestep)
            
            # 2. Get what the agent sees
            percept = world.get_sensory_percept()
            
            # 3. Tell the brain
            agent.process_sensory_input(percept)
            
            # 4. Brain thinks
            brain.step(step)
            
            # 5. Get action from brain
            action = agent.decode_motor_command()
            
            # 6. Do action in world
            world.apply_motor_command(action, world.get_stats()['simulation_time'])
            
            # Collect step data
            step_data = {
                'step': step,
                'time': world.get_stats()['simulation_time'],
                'spikes': brain.get_total_spike_count(),
                'firing_neurons': brain.get_firing_neuron_count(),
                'firing_rate': brain.get_average_firing_rate(),
                'action_type': action.get_type().name,
                'agent_stage': agent.get_developmental_stage().name,
                'curiosity_level': agent.get_curiosity_level(),
                'novelty_level': agent.get_novelty_level()
            }
            results['step_data'].append(step_data)
            
            # Update cumulative results
            results['total_spikes'] = brain.get_total_spike_count()
            results['max_firing_rate'] = max(results['max_firing_rate'], 
                                           brain.get_average_firing_rate())
            results['final_energy'] = world.get_agent_body()['energy']
            
            # Check for learning
            if step % 100 == 0 and step > 0:
                prev_data = [d for d in results['step_data'] if d['step'] < step]
                if prev_data:
                    prev_rate = prev_data[-1]['firing_rate']
                    curr_rate = brain.get_average_firing_rate()
                    if abs(curr_rate - prev_rate) > 0.01:
                        results['learning_detected'] = True
            
            # Execute callbacks if enabled
            if callbacks_enabled and _simulation_callbacks:
                for callback, event_type in _simulation_callbacks:
                    if event_type == "step":
                        callback(step_data)
        
        results['steps_completed'] = num_steps
        
    except Exception as e:
        raise SimulationError(f"Simulation failed at step {results['steps_completed']}: {e}")
    
    finally:
        _simulation_running = False
        end_time = time.time()
        results['simulation_time'] = end_time - start_time
    
    return results

def save_simulation_results(results: Dict[str, Any], filepath: str):
    """Save simulation results to JSON file"""
    # Convert numpy arrays to lists for JSON serialization
    if 'step_data' in results:
        for step_data in results['step_data']:
            for key, value in step_data.items():
                if isinstance(value, np.ndarray):
                    step_data[key] = value.tolist()
    
    with open(filepath, 'w') as f:
        json.dump(results, f, indent=2, default=str)

def load_simulation_results(filepath: str) -> Dict[str, Any]:
    """Load simulation results from JSON file"""
    with open(filepath, 'r') as f:
        return json.load(f)

# Example usage and demonstrations
EXAMPLE_SCRIPTS = {
    'silent_brain': """
import pynlm

# Simplest example: Just neurons
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print(f"Silent brain test done! Neurons: {brain.getTotalNeuronCount()}")
""",

    'brain_watching_world': """
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print(f"Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
""",

    'complete_agent': """
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
"""
}

if __name__ == "__main__":
    # Run examples if executed directly
    print("NLM Enhanced Python Wrapper")
    print("Available examples:")
    for name, script in EXAMPLE_SCRIPTS.items():
        print(f"  {name}")

__all__ = [
    # Core classes
    'NLMError', 'ConfigurationError', 'SimulationError',
    'ActionType', 'DevelopmentalStage',
    'ConfigValidator',
    'SensoryPerceptWrapper', 'ActionWrapper',
    'WorldWrapper', 'AgentWrapper', 'BrainWrapper',
    # Factory functions
    'create_brain', 'create_world', 'create_agent_brain',
    'add_simulation_callback', 'remove_simulation_callback',
    'run_simulation',
    # Utilities
    'save_simulation_results', 'load_simulation_results',
    'EXAMPLE_SCRIPTS'
]
