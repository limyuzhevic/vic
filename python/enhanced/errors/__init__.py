"""Error handling and validation for NLM.

This module provides error handling and validation for the NLM (Neural Learning Machine)
framework. It builds upon the original error handling to provide more Pythonic,
comprehensive error handling with helpful messages and automatic recovery suggestions.

Key features:
- ValidationError for validation errors
- NeuralNetworkError for neural network-related errors
- ErrorHandler for automatic error handling and recovery suggestions
- Input validation for configuration and parameters
- Type checking and value validation
- Backward compatibility with original error handling

Usage examples:

    from pynlm.enhanced.errors import (
        ValidationError,
        NeuralNetworkError,
        ErrorHandler,
        validate_config,
        validate_brain_parameters
    )

    # Validate configuration
    try:
        config = validate_config({"brain.neuron_count": 1000})
    except ValidationError as e:
        print(f"Configuration error: {e}")

    # Validate brain parameters
    try:
        validate_brain_parameters(brain, steps=100)
    except NeuralNetworkError as e:
        print(f"Brain error: {e}")

    # Handle errors automatically
    try:
        brain.step(100)
    except Exception as e:
        ErrorHandler.handle_brain_error(e, "brain.step")

The error handling and validation module includes:

1. ValidationError - Exception for validation errors
2. NeuralNetworkError - Exception for neural network-related errors
3. ErrorHandler - Automatic error handling and recovery suggestions
4. validate_config - Configuration validation
5. validate_brain_parameters - Brain parameter validation
6. validate_sensory_input - Sensory input validation
7. validate_motor_command - Motor command validation
8. validate_world_parameters - World parameter validation
"""

from typing import Dict, Any, Optional, List, Union
import warnings

# Import enhanced_api for error handling
from ..enhanced_api import ValidationError, NeuralNetworkError

class ErrorHandler:
    """Handle errors and provide helpful messages.
    
    The ErrorHandler class provides automatic error handling with helpful
    messages and recovery suggestions. It handles various types of errors
    including validation errors, neural network errors, and configuration errors.
    
    Example:
        from pynlm.enhanced.errors import ErrorHandler
        
        try:
            brain.step(100)
        except Exception as e:
            ErrorHandler.handle_brain_error(e, "brain.step")
    """
    
    @staticmethod
    def handle_validation_error(error: ValidationError, context: str) -> None:
        """Handle validation errors with helpful messages.
        
        Args:
            error: The validation error
            context: Context where error occurred
        """
        error_msg = str(error)
        
        print(f"Validation Error in {context}:")
        print(f"  {error_msg}")
        print("  Suggested fixes:")
        print("    - Check parameter format (dot notation for configuration)")
        print("    - Ensure parameter values are within expected ranges")
        print("    - Validate required parameters are present")
        
        raise error
    
    @staticmethod
    def handle_neural_network_error(error: NeuralNetworkError, context: str) -> None:
        """Handle neural network errors with helpful messages.
        
        Args:
            error: The neural network error
            context: Context where error occurred
        """
        error_msg = str(error)
        
        print(f"Neural Network Error in {context}:")
        print(f"  {error_msg}")
        print("  Suggested fixes:")
        
        if "initialization" in context.lower():
            print("    - Ensure configuration is valid and all required parameters are set")
            print("    - Check that initialize() is called before step()")
        elif "step" in context.lower():
            print("    - The brain may need a warmup period")
            print("    - Check that sensory input is properly connected")
            print("    - Ensure configuration parameters are appropriate for your task")
        elif "configuration" in context.lower():
            print("    - Check that all configuration values are valid")
            print("    - Ensure configuration files exist and are readable")
            print("    - Validate parameter types and ranges")
        else:
            print("    - Check the error details above")
            print("    - Review the NLM documentation for common issues")
        
        raise error
    
    @staticmethod
    def handle_brain_error(error: Exception, context: str) -> None:
        """Handle brain-related errors with helpful messages.
        
        Args:
            error: The error
            context: Context where error occurred
        """
        if isinstance(error, ValidationError):
            ErrorHandler.handle_validation_error(error, context)
        elif isinstance(error, NeuralNetworkError):
            ErrorHandler.handle_neural_network_error(error, context)
        else:
            print(f"Error in {context}:")
            print(f"  {str(error)}")
            print("  Suggested fixes:")
            print("    - Check the error details above")
            print("    - Review the NLM documentation for common issues")
            print("    - Ensure all required parameters are set")
            
            raise NeuralNetworkError(f"Failed to {context}: {str(error)}")
    
    @staticmethod
    def handle_configuration_error(error: Exception, context: str) -> None:
        """Handle configuration errors with helpful messages.
        
        Args:
            error: The error
            context: Context where error occurred
        """
        print(f"Configuration Error in {context}:")
        print(f"  {str(error)}")
        print("  Suggested fixes:")
        print("    - Check that configuration files are valid JSON/JSONL")
        print("    - Ensure all required parameters are present")
        print("    - Validate parameter types and ranges")
        print("    - Check file permissions and paths")
        
        raise NeuralNetworkError(f"Configuration error in {context}: {str(error)}")
    
    @staticmethod
    def handle_simulation_error(error: Exception, context: str) -> None:
        """Handle simulation errors with helpful messages.
        
        Args:
            error: The error
            context: Context where error occurred
        """
        print(f"Simulation Error in {context}:")
        print(f"  {str(error)}")
        print("  Suggested fixes:")
        print("    - Check that initialize() is called before step()")
        print("    - Ensure simulation parameters are valid")
        print("    - Check that all required components are initialized")
        print("    - Verify that configuration parameters are appropriate")
        
        raise NeuralNetworkError(f"Simulation error in {context}: {str(error)}")

# Configuration validation
def validate_config(config_dict: Dict[str, Any]) -> Dict[str, Any]:
    """Validate a configuration dictionary.
    
    Args:
        config_dict: Dictionary containing configuration parameters
        
    Returns:
        Validated configuration dictionary
        
    Raises:
        ValidationError: If configuration is invalid
    """
    valid_sections = {'brain', 'plasticity', 'neuromod', 'simulation', 'world'}
    
    for key in config_dict:
        if '.' not in key:
            raise ValidationError(f"Configuration key '{key}' must use dot notation (e.g., 'brain.neuron_count')")
        
        section = key.split('.')[0]
        if section not in valid_sections:
            warnings.warn(f"Unknown configuration section '{section}'; valid sections: {valid_sections}")
    
    return config_dict

# Brain parameter validation
def validate_brain_parameters(brain, min_steps: int = 0, max_steps: int = 10000) -> None:
    """Validate brain parameters.
    
    Args:
        brain: Brain object to validate
        min_steps: Minimum steps (default: 0)
        max_steps: Maximum steps (default: 10000)
        
    Raises:
        NeuralNetworkError: If brain parameters are invalid
    """
    # Check brain initialization
    if not hasattr(brain, 'initialize') or brain.initialize is None:
        raise NeuralNetworkError("Brain object does not have initialize method")
    
    # Check brain step method
    if not hasattr(brain, 'step') or brain.step is None:
        raise NeuralNetworkError("Brain object does not have step method")
    
    # Check step count
    if min_steps > max_steps:
        raise ValidationError(f"min_steps ({min_steps}) cannot be greater than max_steps ({max_steps})")
    
    if max_steps <= 0:
        raise ValidationError(f"max_steps ({max_steps}) must be positive")

# Sensory input validation
def validate_sensory_input(input_data: Any) -> None:
    """Validate sensory input.
    
    Args:
        input_data: Sensory input data
        
    Raises:
        NeuralNetworkError: If sensory input is invalid
    """
    if input_data is None:
        raise NeuralNetworkError("Sensory input cannot be None")
    
    # Check if input_data has required attributes
    if not hasattr(input_data, 'getType') or input_data.getType() is None:
        warnings.warn("Sensory input does not have getType method or returns None")

# Motor command validation
def validate_motor_command(motor_command: Any) -> None:
    """Validate motor command.
    
    Args:
        motor_command: Motor command to validate
        
    Raises:
        NeuralNetworkError: If motor command is invalid
    """
    if motor_command is None:
        raise NeuralNetworkError("Motor command cannot be None")
    
    # Check if motor_command has required attributes
    if not hasattr(motor_command, 'getType') or motor_command.getType() is None:
        warnings.warn("Motor command does not have getType method or returns None")

# World parameter validation
def validate_world_parameters(world, width: int = 0, height: int = 0) -> None:
    """Validate world parameters.
    
    Args:
        world: World object to validate
        width: Expected world width (default: 0)
        height: Expected world height (default: 0)
        
    Raises:
        NeuralNetworkError: If world parameters are invalid
    """
    # Check world initialization
    if not hasattr(world, 'configure') or world.configure is None:
        raise NeuralNetworkError("World object does not have configure method")
    
    # Check world reset method
    if not hasattr(world, 'reset') or world.reset is None:
        raise NeuralNetworkError("World object does not have reset method")
    
    # Check dimensions
    if width <= 0 or height <= 0:
        warnings.warn(f"World dimensions (width={width}, height={height}) may be invalid")

# Agent validation
def validate_agent(agent, brain=None, world=None) -> None:
    """Validate agent.
    
    Args:
        agent: Agent object to validate
        brain: Optional brain object
        world: Optional world object
        
    Raises:
        NeuralNetworkError: If agent is invalid
    """
    # Check agent initialization
    if not hasattr(agent, 'initialize') or agent.initialize is None:
        raise NeuralNetworkError("Agent object does not have initialize method")
    
    # Check agent processSensoryInput method
    if not hasattr(agent, 'processSensoryInput') or agent.processSensoryInput is None:
        raise NeuralNetworkError("Agent object does not have processSensoryInput method")
    
    # Check agent decodeMotorCommand method
    if not hasattr(agent, 'decodeMotorCommand') or agent.decodeMotorCommand is None:
        raise NeuralNetworkError("Agent object does not have decodeMotorCommand method")

# Export all public functions
__all__ = [
    'ErrorHandler',
    'ValidationError',
    'NeuralNetworkError',
    'validate_config',
    'validate_brain_parameters',
    'validate_sensory_input',
    'validate_motor_command',
    'validate_world_parameters',
    'validate_agent',
]