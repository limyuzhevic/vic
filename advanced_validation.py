#!/usr/bin/env python3
"""
Advanced NLM Python Bindings with Type Checking and Validation

This module demonstrates advanced features of the NLM Python bindings,
including comprehensive type checking, validation, and error handling.
"""

import sys
import os
import json
from typing import Any, Dict, List, Optional, Union, Type, TypeVar, get_type_hints

# Add the parent directory to path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm

T = TypeVar('T')

class NLMValidationError(Exception):
    """Base exception for NLM validation errors."""
    pass

class NLMTypeError(NLMValidationError):
    """Exception for type validation errors."""
    pass

class NLMValueError(NLMValidationError):
    """Exception for value validation errors."""
    pass

def validate_type(value: Any, expected_type: Type, param_name: str) -> T:
    """Validate that a value matches the expected type."""
    if expected_type == int:
        if not isinstance(value, int) or isinstance(value, bool):
            raise NLMTypeError(f"Parameter '{param_name}' must be int, got {type(value).__name__}")
        return value
    elif expected_type == float:
        if not isinstance(value, (int, float)) or isinstance(value, bool):
            raise NLMTypeError(f"Parameter '{param_name}' must be float, got {type(value).__name__}")
        return float(value)
    elif expected_type == bool:
        if not isinstance(value, bool):
            raise NLMTypeError(f"Parameter '{param_name}' must be bool, got {type(value).__name__}")
        return value
    elif expected_type == str:
        if not isinstance(value, str):
            raise NLMTypeError(f"Parameter '{param_name}' must be str, got {type(value).__name__}")
        return value
    elif expected_type == list:
        if not isinstance(value, list):
            raise NLMTypeError(f"Parameter '{param_name}' must be list, got {type(value).__name__}")
        return value
    elif expected_type == dict:
        if not isinstance(value, dict):
            raise NLMTypeError(f"Parameter '{param_name}' must be dict, got {type(value).__name__}")
        return value
    else:
        # For custom types, try to check if it's an instance
        if hasattr(expected_type, '__name__') and expected_type.__name__ in ['NeuronId', 'SynapseId', 'RegionId', 'PopulationId']:
            # Check if it's a valid NLM type
            if not hasattr(value, 'value') or not hasattr(value, 'index'):
                raise NLMTypeError(f"Parameter '{param_name}' must be a valid {expected_type.__name__}")
        return value

def validate_range(value: Union[int, float], min_val: Optional[Union[int, float]] = None, 
                  max_val: Optional[Union[int, float]] = None, param_name: str = "value") -> Union[int, float]:
    """Validate that a value is within the specified range."""
    if min_val is not None and value < min_val:
        raise NLMValueError(f"Parameter '{param_name}' must be >= {min_val}, got {value}")
    if max_val is not None and value > max_val:
        raise NLMValueError(f"Parameter '{param_name}' must be <= {max_val}, got {value}")
    return value

def validate_string_not_empty(value: str, param_name: str) -> str:
    """Validate that a string is not empty."""
    if not value or not value.strip():
        raise NLMValueError(f"Parameter '{param_name}' cannot be empty")
    return value.strip()

def validate_list_non_empty(value: List, param_name: str) -> List:
    """Validate that a list is not empty."""
    if not value:
        raise NLMValueError(f"Parameter '{param_name}' cannot be empty")
    return value

def validate_configuration(config: pynlm.Config) -> pynlm.Config:
    """Validate a complete configuration object."""
    required_keys = ['brain.neuron_count', 'brain.v_thresh', 'brain.v_rest']
    
    for key in required_keys:
        if not config.has(key):
            raise NLMValidationError(f"Configuration missing required key: {key}")
    
    # Validate specific values
    neuron_count = config.getOr('brain.neuron_count', 0)
    validate_range(neuron_count, min_val=1, max_val=10000, param_name="brain.neuron_count")
    
    v_thresh = config.getOr('brain.v_thresh', 0.0)
    validate_range(v_thresh, min_val=-100.0, max_val=-20.0, param_name="brain.v_thresh")
    
    v_rest = config.getOr('brain.v_rest', 0.0)
    validate_range(v_rest, min_val=-100.0, max_val=-20.0, param_name="brain.v_rest")
    
    return config

def safe_brain_operation(brain: pynlm.Brain, operation_func, *args, **kwargs) -> Any:
    """Safely execute a brain operation with validation."""
    if not brain:
        raise NLMValidationError("Brain cannot be None")
    
    if not hasattr(brain, 'initialize'):
        raise NLMTypeError("Object is not a valid Brain instance")
    
    try:
        return operation_func(brain, *args, **kwargs)
    except Exception as e:
        if isinstance(e, (pynlm.NLMConfigError, pynlm.NLMBrainError, pynlm.NLMInputError)):
            raise
        raise NLMValidationError(f"Brain operation failed: {e}")

class ValidatedAgent:
    """Wrapper class for validated agent operations."""
    
    def __init__(self, brain: pynlm.Brain, world: pynlm.SimpleWorld):
        if not brain or not world:
            raise NLMValidationError("Brain and world cannot be None")
        
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
    
    def validated_process_step(self, step: int) -> Dict[str, Any]:
        """Process a single step with validation."""
        # Validate inputs
        validate_range(step, min_val=0, param_name="step")
        
        try:
            # Update world
            self.world.update(0.1)
            
            # Get and validate sensory input
            percept = self.world.getSensoryPercept()
            if not percept:
                raise NLMValidationError("Invalid sensory percept")
            
            # Process input
            self.agent.processSensoryInput(percept)
            
            # Brain thinks
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Validate action
            if action is None:
                raise NLMValidationError("Invalid motor command")
            
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and validate
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            validate_range(reward, min_val=-10.0, max_val=10.0, param_name="reward")
            
            # Apply neuromodulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Collect metrics
            metrics = {
                'step': step,
                'firing_rate': self.brain.getFiringNeuronCount(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'dopamine': self.agent.getNeuromodulationLevel(),
                'action': str(action),
                'reward': reward,
                'success': True
            }
            
            return metrics
            
        except Exception as e:
            return {
                'step': step,
                'error': str(e),
                'success': False
            }
    
    def run_validated_simulation(self, num_steps: int) -> List[Dict[str, Any]]:
        """Run a complete simulation with validation."""
        validate_range(num_steps, min_val=1, param_name="num_steps")
        
        results = []
        
        for step in range(num_steps):
            result = self.validated_process_step(step)
            results.append(result)
            
            # Log progress
            if step % 100 == 0:
                status = "SUCCESS" if result['success'] else "FAILED"
                print(f"Step {step}: {status} - "
                      f"Firing: {result.get('firing_rate', 'N/A')}, "
                      f"Curiosity: {result.get('curiosity', 0):.3f}")
        
        # Calculate summary statistics
        successful_steps = [r for r in results if r['success']]
        
        summary = {
            'total_steps': num_steps,
            'successful_steps': len(successful_steps),
            'success_rate': len(successful_steps) / num_steps if num_steps > 0 else 0.0,
            'final_firing_rate': self.brain.getFiringNeuronCount() if results else 0,
            'average_curiosity': sum(r.get('curiosity', 0) for r in successful_steps) / len(successful_steps) if successful_steps else 0.0,
            'average_novelty': sum(r.get('novelty', 0) for r in successful_steps) / len(successful_steps) if successful_steps else 0.0,
            'development_stage': self.agent.getDevelopmentalStage() if successful_steps else 0,
            'results': results
        }
        
        return summary

class AdvancedBrainManager:
    """Advanced brain management with comprehensive validation and error recovery."""
    
    def __init__(self):
        self.brains = {}
        self.configs = {}
        self.worlds = {}
        self.agents = {}
    
    def create_validated_brain(self, name: str, config: Optional[pynlm.Config] = None,
                               **kwargs) -> pynlm.Brain:
        """Create a brain with comprehensive validation."""
        validate_string_not_empty(name, "name")
        
        if config and not isinstance(config, pynlm.Config):
            raise NLMTypeError("config must be a pynlm.Config instance")
        
        # Create or use existing config
        if config:
            validated_config = validate_configuration(config)
            config_key = f"{name}_custom"
        else:
            # Create default config with validated parameters
            validated_config = pynlm.createDefaultConfig()
            
            # Apply validated parameters from kwargs
            neuron_count = kwargs.get('neuron_count', 1000)
            validate_range(neuron_count, min_val=100, max_val=10000, param_name="neuron_count")
            validated_config.set("brain.neuron_count", neuron_count)
            
            v_thresh = kwargs.get('v_thresh', -50.0)
            validate_range(v_thresh, min_val=-100.0, max_val=-20.0, param_name="v_thresh")
            validated_config.set("brain.v_thresh", v_thresh)
            
            v_rest = kwargs.get('v_rest', -70.0)
            validate_range(v_rest, min_val=-100.0, max_val=-20.0, param_name="v_rest")
            validated_config.set("brain.v_rest", v_rest)
            
            config_key = f"{name}_default"
        
        # Create brain
        brain = pynlm.createBrain(validated_config)
        brain.initialize()
        
        self.brains[name] = brain
        self.configs[config_key] = validated_config
        
        return brain
    
    def create_validated_agent(self, brain_name: str, world_name: str) -> ValidatedAgent:
        """Create a validated agent with proper error handling."""
        if brain_name not in self.brains:
            raise NLMValidationError(f"Brain '{brain_name}' not found")
        
        if world_name not in self.worlds:
            raise NLMValidationError(f"World '{world_name}' not found")
        
        agent = ValidatedAgent(self.brains[brain_name], self.worlds[world_name])
        
        self.agents[f"{brain_name}_{world_name}"] = agent
        return agent
    
    def run_advanced_experiment(self, brain_name: str, world_name: str, 
                               num_steps: int, learning_enabled: bool = True) -> Dict[str, Any]:
        """Run an advanced experiment with comprehensive validation."""
        # Validate inputs
        if brain_name not in self.brains:
            raise NLMValidationError(f"Brain '{brain_name}' not found")
        
        if world_name not in self.worlds:
            raise NLMValidationError(f"World '{world_name}' not found")
        
        validate_range(num_steps, min_val=1, max_val=100000, param_name="num_steps")
        
        # Get brain and world
        brain = self.brains[brain_name]
        world = self.worlds[world_name]
        
        # Create and configure agent
        agent = self.create_validated_agent(brain_name, world_name)
        
        # Configure learning
        if learning_enabled:
            agent.agent.enableRewardModulation(True)
            agent.agent.enableStructuralPlasticity(True)
            agent.agent.enableDevelopment(True)
            agent.agent.enableCuriosity(True)
        
        # Run simulation with validation
        print(f"Starting advanced experiment with {brain_name} in {world_name}")
        print(f"Running {num_steps} steps...")
        
        start_time = time.time()
        
        results = agent.run_validated_simulation(num_steps)
        
        end_time = time.time()
        
        # Compile final results
        experiment_result = {
            'brain_name': brain_name,
            'world_name': world_name,
            'num_steps': num_steps,
            'learning_enabled': learning_enabled,
            'simulation_time': end_time - start_time,
            'steps_per_second': num_steps / (end_time - start_time) if (end_time - start_time) > 0 else 0,
            'experiment_summary': results,
            'success': True
        }
        
        return experiment_result

# Demonstration function
def demonstrate_advanced_features():
    """Demonstrate advanced features with validation."""
    print("=== Advanced NLM Features Demonstration ===")
    
    # Create advanced brain manager
    manager = AdvancedBrainManager()
    
    # Create a world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(5, 5)
    
    manager.worlds['demo_world'] = world
    
    # Create brain with custom parameters
    brain = manager.create_validated_brain(
        name='advanced_brain',
        neuron_count=1500,
        v_thresh=-55.0,
        v_rest=-70.0
    )
    
    print(f"Created advanced brain with {brain.getTotalNeuronCount()} neurons")
    
    # Create agent
    agent = manager.create_validated_agent('advanced_brain', 'demo_world')
    
    # Run experiment
    result = manager.run_advanced_experiment(
        brain_name='advanced_brain',
        world_name='demo_world',
        num_steps=200,
        learning_enabled=True
    )
    
    print(f"\nExperiment completed!")
    print(f"Success rate: {result['experiment_summary']['success_rate']:.1%}")
    print(f"Average curiosity: {result['experiment_summary']['average_curiosity']:.3f}")
    print(f"Simulation speed: {result['steps_per_second']:.1f} steps/sec")
    
    return result

if __name__ == "__main__":
    import time
    
    print("Advanced NLM Python Bindings with Type Checking and Validation")
    print("=" * 70)
    
    try:
        # Run demonstration
        result = demonstrate_advanced_features()
        
        # Save results
        with open('advanced_features_demo.json', 'w') as f:
            json.dump(result, f, indent=2)
        
        print(f"\nResults saved to advanced_features_demo.json")
        print("Advanced NLM features demonstration completed successfully!")
        
    except Exception as e:
        print(f"\nError in demonstration: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
