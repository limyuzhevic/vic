"""
NLM Python Error Handling Module

This module provides comprehensive error handling for NLM Python bindings.
"""

import pynlm
import traceback
from typing import Optional, Any, Dict, List
class NLMError(Exception):
    """Base exception for all NLM errors"""
    def __init__(self, message: str, error_code: str = "NLM_ERROR", details: Optional[Dict] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.stack_trace = traceback.format_exc()

    def __str__(self):
        return f"[{self.error_code}] {self.message}"
class BrainError(NLMError):
    """Brain-specific errors"""
    def __init__(self, message: str, brain_state: Optional[Dict] = None, details: Optional[Dict] = None):
        super().__init__(message, "BRAIN_ERROR", details)
        self.brain_state = brain_state or {}
class WorldError(NLMError):
    """World-specific errors"""
    def __init__(self, message: str, world_state: Optional[Dict] = None, details: Optional[Dict] = None):
        super().__init__(message, "WORLD_ERROR", details)
        self.world_state = world_state or {}
class AgentError(NLMError):
    """Agent-specific errors"""
    def __init__(self, message: str, agent_state: Optional[Dict] = None, details: Optional[Dict] = None):
        super().__init__(message, "AGENT_ERROR", details)
        self.agent_state = agent_state or {}
class MemoryError(NLMError):
    """Memory system errors"""
    def __init__(self, message: str, memory_type: str, memory_state: Optional[Dict] = None, details: Optional[Dict] = None):
        super().__init__(message, "MEMORY_ERROR", details)
        self.memory_type = memory_type
        self.memory_state = memory_state or {}
class ConfigurationError(NLMError):
    """Configuration errors"""
    def __init__(self, message: str, config_key: Optional[str] = None, details: Optional[Dict] = None):
        super().__init__(message, "CONFIG_ERROR", details)
        self.config_key = config_key
class SimulationError(NLMError):
    """Simulation runtime errors"""
    def __init__(self, message: str, step: Optional[int] = None, details: Optional[Dict] = None):
        super().__init__(message, "SIMULATION_ERROR", details)
        self.step = step
class PerformanceError(NLMError):
    """Performance-related errors"""
    def __init__(self, message: str, metric: Optional[str] = None, details: Optional[Dict] = None):
        super().__init__(message, "PERFORMANCE_ERROR", details)
        self.metric = metric
def validate_brain(brain, min_neurons: int = 1, max_neurons: int = 100000) -> bool:
    """Validate brain state and parameters"""
    try:
        neuron_count = brain.getTotalNeuronCount()
        if neuron_count < min_neurons:
            raise BrainError(f"Brain has too few neurons ({neuron_count} < {min_neurons})")
        if neuron_count > max_neurons:
            raise BrainError(f"Brain has too many neurons ({neuron_count} > {max_neurons})")
        
        if not brain.getConfig():
            raise BrainError("Brain configuration is missing")
            
        return True
    except Exception as e:
        if isinstance(e, NLMError):
            raise
        raise BrainError(f"Brain validation failed: {str(e)}")
def validate_world(world) -> bool:
    """Validate world state and parameters"""
    try:
        width = world.getWidth()
        height = world.getHeight()
        
        if width <= 0 or height <= 0:
            raise WorldError(f"Invalid world dimensions: {width}x{height}")
            
        if width > 1000 or height > 1000:
            raise WorldError(f"World dimensions too large: {width}x{height}")
            
        return True
    except Exception as e:
        if isinstance(e, NLMError):
            raise
        raise WorldError(f"World validation failed: {str(e)}")
def validate_agent(agent) -> bool:
    """Validate agent state and parameters"""
    try:
        if not agent.getBrain():
            raise AgentError("Agent has no brain attached")
            
        brain = agent.getBrain()
        if not brain.getConfig():
            raise AgentError("Agent's brain has no configuration")
            
        return True
    except Exception as e:
        if isinstance(e, NLMError):
            raise
        raise AgentError(f"Agent validation failed: {str(e)}")
def safe_simulation_step(brain, agent=None, world=None, step: int = 0) -> Dict:
    """
    Safely run a simulation step with comprehensive error handling.
    
    Returns:
        Dict with step results and any errors encountered
    """
    result = {
        "step": step,
        "success": False,
        "errors": [],
        "warnings": [],
        "stats": {}
    }
    
    try:
        # Validate inputs
        if not validate_brain(brain):
            raise BrainError(f"Brain validation failed at step {step}")
            
        if world and not validate_world(world):
            raise WorldError(f"World validation failed at step {step}")
            
        if agent and not validate_agent(agent):
            raise AgentError(f"Agent validation failed at step {step}")
        
        # Record pre-step state
        pre_spikes = brain.getTotalSpikeCount()
        pre_firing = brain.getFiringNeuronCount()
        
        # Execute step with error handling
        try:
            if agent and world:
                # Full agent-world simulation
                if world.getSimulationTime() == 0.0:
                    world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
                
            else:
                # Simple brain simulation
                brain.step(step)
                
        except Exception as e:
            if isinstance(e, NLMError):
                result["errors"].append(e)
                raise
            else:
                # Wrap unexpected errors
                sim_error = SimulationError(
                    f"Unexpected error during simulation step {step}: {str(e)}",
                    step,
                    {"exception_type": type(e).__name__, "trace": traceback.format_exc()}
                )
                result["errors"].append(sim_error)
                raise sim_error
        
        # Record post-step state
        post_spikes = brain.getTotalSpikeCount()
        post_firing = brain.getFiringNeuronCount()
        
        result["success"] = True
        result["stats"] = {
            "spikes_generated": post_spikes - pre_spikes,
            "firing_neurons": post_firing,
            "firing_rate": brain.getAverageFiringRate(),
            "e_i_ratio": brain.getExcitationInhibitionRatio()
        }
        
        # Check for warnings
        if result["stats"]["spikes_generated"] == 0:
            result["warnings"].append("No spikes generated in this step")
            
        if post_firing == 0:
            result["warnings"].append("No neurons are firing in this step")
            
        if result["stats"]["e_i_ratio"] > 10.0 or result["stats"]["e_i_ratio"] < 0.1:
            result["warnings"].append(f"Extreme E/I ratio: {result['stats']['e_i_ratio']:.3f}")
            
    except NLMError as e:
        result["errors"].append(e)
        result["success"] = False
    except Exception as e:
        # Wrap unexpected errors
        unexpected_error = NLMError(
            f"Unexpected error in safe_simulation_step: {str(e)}",
            "UNEXPECTED_ERROR",
            {"exception_type": type(e).__name__, "trace": traceback.format_exc()}
        )
        result["errors"].append(unexpected_error)
        result["success"] = False
    
    return result
def run_safe_simulation(brain, agent=None, world=None, num_steps: int = 1000, 
                         validate_every: int = 10) -> List[Dict]:
    """
    Run a complete simulation with comprehensive error handling and validation.
    
    Args:
        brain: Brain object
        agent: Optional agent brain interface
        world: Optional world object
        num_steps: Number of simulation steps
        validate_every: Validate every N steps
        
    Returns:
        List of step results with detailed error information
    """
    results = []
    
    for step in range(num_steps):
        # Periodic validation
        if step % validate_every == 0:
            try:
                if world:
                    validate_world(world)
                if agent:
                    validate_agent(agent)
                validate_brain(brain, min_neurons=10, max_neurons=50000)
            except NLMError as e:
                results.append({
                    "step": step,
                    "success": False,
                    "errors": [e],
                    "warnings": ["Validation failed, simulation stopping"],
                    "stats": {}
                })
                break
        
        # Run safe simulation step
        step_result = safe_simulation_step(brain, agent, world, step)
        results.append(step_result)
        
        # Log errors if any
        if step_result["errors"]:
            for error in step_result["errors"]:
                print(f"Step {step} Error: {error}")
                
        if step_result["warnings"]:
            for warning in step_result["warnings"]:
                print(f"Step {step} Warning: {warning}")
    
    # Summary statistics
    successful_steps = sum(1 for r in results if r["success"])
    total_errors = sum(len(r["errors"]) for r in results)
    total_warnings = sum(len(r["warnings"]) for r in results)
    
    print(f"Simulation completed: {successful_steps}/{num_steps} steps successful")
    print(f"Total errors: {total_errors}, Total warnings: {total_warnings}")
    
    return results
def recover_from_error(brain, filepath: str) -> bool:
    """
    Attempt to recover brain state from checkpoint after error.
    
    Args:
        brain: Brain object to recover
        filepath: Path to checkpoint file
        
    Returns:
        True if recovery successful, False otherwise
    """
    try:
        if brain.load(filepath):
            print(f"Successfully recovered brain state from {filepath}")
            return True
        else:
            raise ConfigurationError(f"Failed to load brain state from {filepath}")
    except NLMError as e:
        print(f"Recovery failed: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error during recovery: {e}")
        return False
def validate_configuration(config) -> List[NLMError]:
    """
    Validate configuration parameters.
    
    Args:
        config: Config object to validate
        
    Returns:
        List of configuration errors found
    """
    errors = []
    
    # Check for essential parameters
    if not config.has("neuron_count"):
        errors.append(ConfigurationError("Configuration missing neuron_count"))
    
    if config.has("neuron_count"):
        neuron_count = config.getOr<size_t>("neuron_count", 0)
        if neuron_count == 0:
            errors.append(ConfigurationError("neuron_count cannot be zero"))
        elif neuron_count > 1000000:
            errors.append(ConfigurationError("neuron_count too large (max: 1000000)"))
    
    # Validate plasticity settings
    if config.has("plasticity.stdp.enable"):
        stdp_enabled = config.getOr<bool>("plasticity.stdp.enable", false)
        if stdp_enabled:
            if not config.has("plasticity.stdp.learning_rate"):
                errors.append(ConfigurationError("STDP learning rate required when STDP is enabled"))
    
    # Validate neuromodulation settings
    if config.has("neuromod.dopamine.scale"):
        dopamine_scale = config.getOr<float>("neuromod.dopamine.scale", 1.0f)
        if dopamine_scale < 0.0 or dopamine_scale > 10.0:
            errors.append(ConfigurationError(f"Invalid dopamine scale: {dopamine_scale}"))
    
    return errors
class NLMErrorHandler:
    """Context manager for comprehensive error handling"""
    
    def __init__(self, brain=None, agent=None, world=None):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.errors = []
        self.warnings = []
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type is not None:
            if issubclass(exc_type, NLMError):
                # Convert to NLMError if it's already an NLMError
                if not isinstance(exc_val, NLMError):
                    self.add_error(NLMError(
                        f"NLM error occurred: {str(exc_val)}",
                        "CONTEXT_ERROR",
                        {"exception_type": exc_type.__name__}
                    ))
            else:
                # Wrap unexpected exceptions
                self.add_error(NLMError(
                    f"Unexpected error: {str(exc_val)}",
                    "UNEXPECTED_ERROR",
                    {"exception_type": exc_type.__name__, "trace": traceback.format_tb(exc_tb)}
                ))
        
        # Log all errors
        for error in self.errors:
            print(f"ERROR: {error}")
            if error.details and "trace" in error.details:
                print(f"Traceback: {error.details['trace']}")
        
        for warning in self.warnings:
            print(f"WARNING: {warning}")
        
        return False  # Don't suppress exceptions
    
    def add_error(self, error: NLMError):
        """Add an error to the handler"""
        self.errors.append(error)
    
    def add_warning(self, message: str):
        """Add a warning to the handler"""
        self.warnings.append(message)
    
    def handle_brain_error(self, error: NLMError):
        """Handle brain-specific errors"""
        self.add_error(error)
        if self.brain:
            print(f"Brain state: {self.brain.getTotalNeuronCount()} neurons, "
                  f"{self.brain.getTotalSynapseCount()} synapses")
    
    def handle_world_error(self, error: NLMError):
        """Handle world-specific errors"""
        self.add_error(error)
        if self.world:
            print(f"World state: {self.world.getWidth()}x{self.world.getHeight()}, "
                  f"simulation time: {self.world.getSimulationTime()}")
    
    def handle_agent_error(self, error: NLMError):
        """Handle agent-specific errors"""
        self.add_error(error)
        if self.agent:
            print(f"Agent state: neuromodulation={self.agent.getNeuromodulationLevel():.3f}, "
                  f"curiosity={self.agent.getCuriosityLevel():.3f}, "
                  f"development={self.agent.getDevelopmentalStage()}")
# Convenience functions for common error scenarios
def safe_brain_operation(operation, *args, **kwargs):
    """Wrapper for safe brain operations"""
    try:
        result = operation(*args, **kwargs)
        if not result:
            raise BrainError("Brain operation returned false")
        return result
    except NLMError:
        raise
    except Exception as e:
        raise BrainError(f"Brain operation failed: {str(e)}")
def safe_world_operation(operation, *args, **kwargs):
    """Wrapper for safe world operations"""
    try:
        result = operation(*args, **kwargs)
        return result
    except NLMError:
        raise
    except Exception as e:
        raise WorldError(f"World operation failed: {str(e)}")
def safe_agent_operation(operation, *args, **kwargs):
    """Wrapper for safe agent operations"""
    try:
        result = operation(*args, **kwargs)
        return result
    except NLMError:
        raise
    except Exception as e:
        raise AgentError(f"Agent operation failed: {str(e)}")

__all__ = [
    'NLMError',
    'BrainError',
    'WorldError',
    'AgentError',
    'MemoryError',
    'ConfigurationError',
    'SimulationError',
    'PerformanceError',
    'validate_brain',
    'validate_world',
    'validate_agent',
    'safe_simulation_step',
    'run_safe_simulation',
    'recover_from_error',
    'validate_configuration',
    'NLMErrorHandler',
    'safe_brain_operation',
    'safe_world_operation',
    'safe_agent_operation'
]

print("NLM Error Handling Module loaded successfully")
print("Available exception classes: NLMError, BrainError, WorldError, AgentError, MemoryError, ConfigurationError, SimulationError, PerformanceError")
print("Available functions: validate_brain, validate_world, validate_agent, safe_simulation_step, run_safe_simulation, recover_from_error, validate_configuration, NLMErrorHandler")
print("Available wrappers: safe_brain_operation, safe_world_operation, safe_agent_operation")