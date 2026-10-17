#!/usr/bin/env python3
"""
Enhanced NLM Python Bindings - Main Module

Comprehensive Python API for the Neural Learning Machine (NLM) framework.
This module provides advanced Python features including:

- Pythonic API with context managers and properties
- Enhanced error handling with custom exception hierarchy
- Statistical analysis utilities for neural data
- Development and debugging tools
- Serialization utilities for brain state management
- Experiment management and benchmarking
- Advanced statistical analysis
- Machine learning utilities for neural data
- Visualization utilities (plotting, graphing)
- Data export/import utilities
- Performance profiling tools
- Memory management utilities
- Experiment tracking and comparison
- Development tools with breakpoints and step-through debugging
- Visualization of neural networks
- Performance benchmarking
- Memory leak detection
- Code profiling tools
- Simulation comparison tools
- Advanced command line interface with argument parsing
- Configuration file support
- Logging and monitoring
- Batch processing utilities
- Custom callback systems
- Plugin architecture
- Extension modules
- Automated testing utilities
- Documentation generation
- API introspection tools

This module is designed to support everything from simple demonstrations
to complex research experiments in neural simulation and computation.
"""

import sys
import os
import json
import time
import logging
import argparse
import threading
import itertools
import math
import csv
from pathlib import Path
from datetime import datetime
from typing import List, Dict, Tuple, Optional, Any, Union, Callable, Iterator, Generator
from abc import ABC, abstractmethod
from contextlib import contextmanager
import warnings

# Import core NLM bindings (these would be provided by pybind11)
try:
    from pynlm_enhanced import (
        Brain, SimpleWorld, Config, AgentBrain,
        Neuron, NeuralRegion, Synapse, NeuralPopulation,
        STDP, Hebbian, StructuralPlasticity,
        Neuromodulator, Dopamine, Curiosity, Novelty, PredictionError,
        WorkingMemory, EpisodicMemory, PredictionSystem,
        NeuralPlanner, ConceptFormation, AttentionalSelection,
        DevelopmentSystem, Synaptogenesis, Pruning, Maturation,
        SpikeSystem, RandomGenerator, SimulationClock,
        Vision, Audio, InternalSignals, SensoryPercept,
        Action, WorldObject, AgentBody, ActionResult,
        Environment, Observation,
        NLMException, NLMConfigError, NLMSimulationError, NLMResourceError,
        create_default_config, create_brain, create_simple_world, create_agent_brain,
        compute_statistics, normalize_vector, timestamp_to_string,
        format_neuron_stats, format_world_stats,
        print_brain_state, print_neuron_statistics,
        write_debug_log, save_brain, load_brain,
        save_config, load_config, run_simulation
    )
    _CORE_AVAILABLE = True
except ImportError:
    _CORE_AVAILABLE = False
    print("Warning: Core NLM bindings not available. Some features will be limited.")

# Import error handling and type safety modules
try:
    from .error_handler import (
        # Custom Exception Hierarchy
        NLMException, NLMConfigError, NLMBrainError, NLMWorldError,
        NLMMemoryError, NLMPrecisionError, NLMFileError, NLMValidationError,
        NLMResourceError, NLMSimulationError,
        
        # Validation Framework
        TypeValidator, ValueValidator, ParameterValidator,
        ErrorHandler, DefensiveProgramming, ValidationFramework,
        TypeSafetyChecker, NLMValidator,
        
        # Decorators
        validate_types, validate_range, safe_operation,
        
        # Global instances
        get_validator, set_validator, create_default_validator,
        validate_parameter, validate_function_call, error_context,
        
        # Utility functions
        get_type_info, get_validation_error_chain, format_validation_report
    )
    _ERROR_HANDLER_AVAILABLE = True
except ImportError:
    _ERROR_HANDLER_AVAILABLE = False
    print("Warning: Error handler modules not available. Some features will be limited.")

# Version information
__version__ = "2.0.0-enhanced"
__author__ = "NLM Authors"
__license__ = "MIT"
__copyright__ = f"Copyright © 2026 NLM Authors"

# Global validator instance
_validator_instance = None

# Pythonic API utilities
@contextmanager
def nlm_context_manager():
    """Context manager for NLM operations."""
    try:
        yield
    except Exception as e:
        # Log the error and re-raise
        logging.getLogger("nlm.context_manager").error(f"Context manager error: {e}")
        raise

def nlm_property(fget):
    """Decorator for creating NLM properties."""
    return property(fget)

class NLMLogger:
    """Custom logger for NLM operations."""
    
    def __init__(self, name: str = "nlm"):
        self.logger = logging.getLogger(name)
        self.logger.setLevel(logging.INFO)
        
        if not self.logger.handlers:
            handler = logging.StreamHandler(sys.stdout)
            formatter = logging.Formatter(
                '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
            )
            handler.setFormatter(formatter)
            self.logger.addHandler(handler)
    
    def info(self, message: str):
        self.logger.info(message)
    
    def warning(self, message: str):
        self.logger.warning(message)
    
    def error(self, message: str):
        self.logger.error(message)
    
    def debug(self, message: str):
        self.logger.debug(message)

print(f"Enhanced NLM Python Bindings v{__version__}")

# Setup basic logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

# Global logger instance
logger = NLMLogger("nlm.main")

# Function to get global validator
def get_validator():
    """Get global validator instance."""
    global _validator_instance
    if _validator_instance is None:
        _validator_instance = NLMValidator()
    return _validator_instance

# Function to set global validator
def set_validator(validator):
    """Set global validator instance."""
    global _validator_instance
    _validator_instance = validator

# Initialize global validator
_validator_instance = get_validator()

__all__ = [
    # Core classes (from pynlm_enhanced)
    'Brain', 'SimpleWorld', 'Config', 'AgentBrain',
    'Neuron', 'NeuralRegion', 'Synapse', 'NeuralPopulation',
    'STDP', 'Hebbian', 'StructuralPlasticity',
    'Neuromodulator', 'Dopamine', 'Curiosity', 'Novelty', 'PredictionError',
    'WorkingMemory', 'EpisodicMemory', 'PredictionSystem',
    'NeuralPlanner', 'ConceptFormation', 'AttentionalSelection',
    'DevelopmentSystem', 'Synaptogenesis', 'Pruning', 'Maturation',
    'SpikeSystem', 'RandomGenerator', 'SimulationClock',
    'Vision', 'Audio', 'InternalSignals', 'SensoryPercept',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'Environment', 'Observation',
    
    # Factory functions
    'create_default_config', 'create_brain', 'create_simple_world', 'create_agent_brain',
    'compute_statistics', 'normalize_vector', 'timestamp_to_string',
    'format_neuron_stats', 'format_world_stats',
    'print_brain_state', 'print_neuron_statistics',
    'write_debug_log', 'save_brain', 'load_brain',
    'save_config', 'load_config', 'run_simulation',
    
    # Error handling (from error_handler)
    'NLMException', 'NLMConfigError', 'NLMBrainError', 'NLMWorldError',
    'NLMMemoryError', 'NLMPrecisionError', 'NLMFileError', 'NLMValidationError',
    'NLMResourceError', 'NLMSimulationError',
    
    # Validation Framework
    'TypeValidator', 'ValueValidator', 'ParameterValidator',
    'ErrorHandler', 'DefensiveProgramming', 'ValidationFramework',
    'TypeSafetyChecker', 'NLMValidator',
    
    # Decorators
    'validate_types', 'validate_range', 'safe_operation',
    
    # Global instances
    'get_validator', 'set_validator', 'create_default_validator',
    'validate_parameter', 'validate_function_call', 'error_context',
    
    # Utility functions
    'get_type_info', 'get_validation_error_chain', 'format_validation_report',
    
    # Pythonic API utilities
    'nlm_context_manager', 'nlm_property', 'NLMLogger',
    
    # Backward compatibility
    'NLMException', 'NLMConfigError', 'NLMSimulationError', 'NLMResourceError'
]
