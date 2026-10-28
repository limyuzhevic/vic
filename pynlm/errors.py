# Error handling and logging for NLM
import time
import logging
import traceback
from typing import Optional, Dict, Any, List
from enum import Enum
from dataclasses import dataclass
from datetime import datetime

# Error classes
class NLMRuntimeError(Exception):
    """Base exception for NLM runtime errors."""
    
    def __init__(self, message: str, error_code: str = "RUNTIME_ERROR", 
                 details: Optional[Dict[str, Any]] = None, cause: Optional[Exception] = None):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}
        self.cause = cause
        self.timestamp = datetime.now()
        self.traceback = traceback.format_exc() if cause else None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert error to dictionary for serialization."""
        return {
            'type': self.__class__.__name__,
            'message': self.message,
            'error_code': self.error_code,
            'details': self.details,
            'timestamp': self.timestamp.isoformat(),
            'cause': str(self.cause) if self.cause else None,
            'traceback': self.traceback
        }

class NLMConfigurationError(NLMRuntimeError):
    """Exception for configuration-related errors."""
    
    def __init__(self, message: str, config_key: Optional[str] = None, 
                 invalid_value: Optional[Any] = None, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, "CONFIGURATION_ERROR", details or {})
        self.config_key = config_key
        self.invalid_value = invalid_value
        self.details.update({
            'config_key': config_key,
            'invalid_value': invalid_value
        })

class NLMSimulationError(NLMRuntimeError):
    """Exception for simulation-related errors."""
    
    def __init__(self, message: str, simulation_step: Optional[int] = None, 
                 brain_state: Optional[Dict[str, Any]] = None, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, "SIMULATION_ERROR", details or {})
        self.simulation_step = simulation_step
        self.brain_state = brain_state
        self.details.update({
            'simulation_step': simulation_step,
            'brain_state': brain_state
        })

class NLMValidationError(NLMRuntimeError):
    """Exception for validation-related errors."""
    
    def __init__(self, message: str, validation_rules: Optional[List[str]] = None, 
                 failed_values: Optional[Dict[str, Any]] = None, details: Optional[Dict[str, Any]] = None):
        super().__init__(message, "VALIDATION_ERROR", details or {})
        self.validation_rules = validation_rules or []
        self.failed_values = failed_values or {}
        self.details.update({
            'validation_rules': validation_rules,
            'failed_values': failed_values
        })

# Logging system
class NLMLogger:
    """Enhanced logging system for NLM."""
    
    def __init__(self, name: str = "NLM", log_level: str = "INFO", 
                 enable_file_logging: bool = False, log_file: str = "nlm.log"):
        self.name = name
        self.log_level = getattr(logging, log_level.upper())
        self.enable_file_logging = enable_file_logging
        self.log_file = log_file
        
        # Create logger
        self.logger = logging.getLogger(name)
        self.logger.setLevel(self.log_level)
        
        # Remove existing handlers
        for handler in self.logger.handlers[:]:
            self.logger.removeHandler(handler)
        
        # Create formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(self.log_level)
        console_handler.setFormatter(formatter)
        self.logger.addHandler(console_handler)
        
        # File handler (if enabled)
        if self.enable_file_logging:
            file_handler = logging.FileHandler(log_file)
            file_handler.setLevel(self.log_level)
            file_handler.setFormatter(formatter)
            self.logger.addHandler(file_handler)
    
    def debug(self, message: str, *args, **kwargs) -> None:
        """Log debug message."""
        self.logger.debug(message, *args, **kwargs)
    
    def info(self, message: str, *args, **kwargs) -> None:
        """Log info message."""
        self.logger.info(message, *args, **kwargs)
    
    def warning(self, message: str, *args, **kwargs) -> None:
        """Log warning message."""
        self.logger.warning(message, *args, **kwargs)
    
    def error(self, message: str, *args, **kwargs) -> None:
        """Log error message."""
        self.logger.error(message, *args, **kwargs)
    
    def critical(self, message: str, *args, **kwargs) -> None:
        """Log critical message."""
        self.logger.critical(message, *args, **kwargs)
    
    def exception(self, message: str, *args, **kwargs) -> None:
        """Log exception with traceback."""
        self.logger.exception(message, *args, **kwargs)
    
    def log_simulation_step(self, step: int, brain_stats: Dict[str, Any], 
                           world_stats: Dict[str, Any], action: Any) -> None:
        """Log simulation step with comprehensive statistics."""
        self.info(f"Simulation Step {step}:")
        self.debug(f"  Brain Stats: {brain_stats}")
        self.debug(f"  World Stats: {world_stats}")
        self.debug(f"  Action: {action}")
    
    def log_performance_metrics(self, metrics: Dict[str, Any]) -> None:
        """Log performance metrics."""
        self.info("Performance Metrics:")
        for key, value in metrics.items():
            if isinstance(value, float):
                self.debug(f"  {key}: {value:.4f}")
            else:
                self.debug(f"  {key}: {value}")

class ErrorHandler:
    """Error handling utilities for NLM."""
    
    def __init__(self, logger: Optional[NLMLogger] = None):
        self.logger = logger or NLMLogger()
        self.error_count = 0
        self.warning_count = 0
    
    def validate_brain_config(self, config: Dict[str, Any]) -> List[NLMConfigurationError]:
        """Validate brain configuration and return list of errors."""
        errors = []
        
        # Check neuron count
        neuron_count = config.get("brain.neuron_count", 0)
        if not isinstance(neuron_count, (int, float)):
            errors.append(NLMConfigurationError(
                f"Invalid neuron count type: {type(neuron_count)}",
                config_key="brain.neuron_count",
                invalid_value=neuron_count
            ))
        elif neuron_count <= 0:
            errors.append(NLMConfigurationError(
                f"Neuron count must be positive, got: {neuron_count}",
                config_key="brain.neuron_count",
                invalid_value=neuron_count
            ))
        elif neuron_count > 100000:
            errors.append(NLMConfigurationError(
                f"Neuron count very large ({neuron_count}), may cause performance issues",
                config_key="brain.neuron_count",
                invalid_value=neuron_count
            ))
        
        # Check synapse density
        synapse_density = config.get("brain.synapse_density", 0.0)
        if not isinstance(synapse_density, (int, float)):
            errors.append(NLMConfigurationError(
                f"Invalid synapse density type: {type(synapse_density)}",
                config_key="brain.synapse_density",
                invalid_value=synapse_density
            ))
        elif synapse_density < 0 or synapse_density > 1:
            errors.append(NLMConfigurationError(
                f"Synapse density should be between 0 and 1, got: {synapse_density}",
                config_key="brain.synapse_density",
                invalid_value=synapse_density
            ))
        
        # Check simulation timestep
        timestep = config.get("simulation_timestep", 0.0)
        if not isinstance(timestep, (int, float)):
            errors.append(NLMConfigurationError(
                f"Invalid simulation timestep type: {type(timestep)}",
                config_key="simulation_timestep",
                invalid_value=timestep
            ))
        elif timestep <= 0 or timestep > 0.1:
            errors.append(NLMConfigurationError(
                f"Simulation timestep should be positive and <= 0.1s, got: {timestep}",
                config_key="simulation_timestep",
                invalid_value=timestep
            ))
        
        # Check plasticity parameters
        if config.get("plasticity.stdp.enable", False):
            learning_rate = config.get("plasticity.stdp.learning_rate", 0.0)
            if not isinstance(learning_rate, (int, float)):
                errors.append(NLMConfigurationError(
                    f"Invalid STDP learning rate type: {type(learning_rate)}",
                    config_key="plasticity.stdp.learning_rate",
                    invalid_value=learning_rate
                ))
            elif learning_rate < 0 or learning_rate > 1:
                errors.append(NLMConfigurationError(
                    f"STDP learning rate should be between 0 and 1, got: {learning_rate}",
                    config_key="plasticity.stdp.learning_rate",
                    invalid_value=learning_rate
                ))
        
        # Log validation results
        if errors:
            self.logger.error(f"Configuration validation failed with {len(errors)} errors")
            for error in errors:
                self.logger.error(f"  - {error.message}")
        else:
            self.logger.info("Configuration validation passed")
        
        return errors
    
    def validate_simulation_step(self, step: int, brain_state: Dict[str, Any], 
                                 world_state: Dict[str, Any]) -> List[NLMValidationError]:
        """Validate simulation step and return list of errors."""
        errors = []
        
        # Check brain state
        if brain_state.get("total_neuron_count", 0) <= 0:
            errors.append(NLMValidationError(
                f"Invalid brain state at step {step}: zero or negative neuron count",
                validation_rules=["neuron_count_positive"],
                failed_values={"total_neuron_count": brain_state.get("total_neuron_count", 0)}
            ))
        
        if brain_state.get("total_synapse_count", 0) < 0:
            errors.append(NLMValidationError(
                f"Invalid brain state at step {step}: negative synapse count",
                validation_rules=["synapse_count_non_negative"],
                failed_values={"total_synapse_count": brain_state.get("total_synapse_count", 0)}
            ))
        
        # Check world state
        if world_state.get("agent_energy", 0) < 0:
            errors.append(NLMValidationError(
                f"Invalid world state at step {step}: negative agent energy",
                validation_rules=["energy_non_negative"],
                failed_values={"agent_energy": world_state.get("agent_energy", 0)}
            ))
        
        if world_state.get("agent_health", 0) < 0 or world_state.get("agent_health", 0) > 100:
            errors.append(NLMValidationError(
                f"Invalid world state at step {step}: agent health out of range (0-100)",
                validation_rules=["health_range_valid"],
                failed_values={"agent_health": world_state.get("agent_health", 0)}
            ))
        
        # Log validation results
        if errors:
            self.logger.warning(f"Simulation step validation failed at step {step} with {len(errors)} errors")
            for error in errors:
                self.logger.warning(f"  - {error.message}")
        
        return errors
    
    def handle_simulation_error(self, error: Exception, context: Optional[Dict[str, Any]] = None) -> NLMSimulationError:
        """Handle simulation error and convert to NLMSimulationError."""
        self.error_count += 1
        
        # Convert to NLMSimulationError if not already
        if not isinstance(error, NLMSimulationError):
            error_message = str(error)
            
            # Create detailed error information
            details = {
                'error_type': type(error).__name__,
                'timestamp': datetime.now().isoformat(),
                'context': context or {}
            }
            
            simulation_error = NLMSimulationError(
                error_message,
                brain_state=context.get('brain_state') if context else None,
                details=details
            )
            
            # Log the error
            self.logger.error(f"Simulation error at step {context.get('step', 'unknown') if context else 'unknown'}: {error_message}")
            if context:
                self.logger.debug(f"Error context: {context}")
            
            return simulation_error
        
        return error

class ProgressTracker:
    """Track and log simulation progress."""
    
    def __init__(self, total_steps: int, logger: Optional[NLMLogger] = None):
        self.total_steps = total_steps
        self.current_step = 0
        self.start_time = time.time()
        self.logger = logger or NLMLogger()
        self.last_update_time = self.start_time
        self.update_interval = max(1, total_steps // 10)  # Update every 10% of total steps
    
    def update(self, step: int, message: str = "") -> None:
        """Update progress tracking."""
        self.current_step = step
        
        # Check if we should update display
        if step % self.update_interval == 0 or step == self.total_steps:
            elapsed = time.time() - self.start_time
            if step > 0:
                steps_per_second = step / elapsed
                remaining_steps = self.total_steps - step
                eta = remaining_steps / max(steps_per_second, 0.001)
                
                progress_percent = (step / self.total_steps) * 100
                
                self.logger.info(
                    f"Progress: {progress_percent:.1f}% ({step}/{self.total_steps}) "
                    f"- {steps_per_second:.1f} steps/sec - ETA: {eta:.1f}s {message}"
                )
            else:
                self.logger.info(f"Progress: {step}/{self.total_steps} {message}")
    
    def finish(self) -> None:
        """Mark progress as finished."""
        elapsed = time.time() - self.start_time
        steps_per_second = self.current_step / max(elapsed, 0.001)
        
        self.logger.info(
            f"Simulation completed: {self.current_step} steps in {elapsed:.2f}s "
            f"({steps_per_second:.1f} steps/sec)"
        )

# Global logger and error handler instances
_global_logger: Optional[NLMLogger] = None
_global_error_handler: Optional[ErrorHandler] = None

def get_logger(name: str = "NLM", **kwargs) -> NLMLogger:
    """Get or create a logger instance."""
    global _global_logger
    if _global_logger is None:
        _global_logger = NLMLogger(name, **kwargs)
    return _global_logger

def get_error_handler(logger: Optional[NLMLogger] = None) -> ErrorHandler:
    """Get or create an error handler instance."""
    global _global_error_handler
    if _global_error_handler is None:
        _global_error_handler = ErrorHandler(logger)
    return _global_error_handler

# Utility functions for common error handling

def safe_simulation_step(func):
    """Decorator to add error handling to simulation step functions."""
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            logger = get_logger()
            error_handler = get_error_handler(logger)
            
            simulation_error = error_handler.handle_simulation_error(e, {
                'step': kwargs.get('step', args[0] if args else None) if len(args) > 0 else None
            })
            
            logger.exception(f"Simulation step failed: {simulation_error}")
            raise
    
    return wrapper

def validate_and_log_config(config: Dict[str, Any]) -> List[NLMConfigurationError]:
    """Validate configuration and log warnings."""
    logger = get_logger()
    error_handler = get_error_handler(logger)
    
    errors = error_handler.validate_brain_config(config)
    
    if errors:
        logger.error(f"Configuration validation completed with {len(errors)} errors")
        for error in errors:
            logger.error(f"  ERROR: {error.message}")
            if error.config_key:
                logger.error(f"    Key: {error.config_key}")
            if error.invalid_value:
                logger.error(f"    Value: {error.invalid_value}")
    
    return errors

# Context manager for error handling
class NLMErrorContext:
    """Context manager for handling errors in specific operations."""
    
    def __init__(self, operation_name: str, logger: Optional[NLMLogger] = None,
                 error_handler: Optional[ErrorHandler] = None):
        self.operation_name = operation_name
        self.logger = logger or get_logger()
        self.error_handler = error_handler or get_error_handler(self.logger)
        self.start_time = time.time()
    
    def __enter__(self):
        self.logger.info(f"Starting {self.operation_name}...")
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        elapsed = time.time() - self.start_time
        
        if exc_type is not None:
            self.logger.error(f"{self.operation_name} failed after {elapsed:.2f}s: {exc_val}")
            
            # Handle the error
            simulation_error = self.error_handler.handle_simulation_error(
                exc_val, {'operation': self.operation_name}
            )
            
            # Log additional context
            self.logger.debug(f"Error traceback:\n{exc_tb}")
            
            return False  # Propagate the exception
        else:
            self.logger.info(f"{self.operation_name} completed successfully in {elapsed:.2f}s")
            return True
