#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Enhanced Python Interface

A brain-inspired spiking neural network simulator with comprehensive
Python API for research, development, and experimentation.

This module provides advanced Python interfaces for NLM with:
- Type hints and comprehensive documentation
- Advanced simulation control and monitoring
- Configuration management and validation
- Machine learning capabilities
- Performance optimization tools
- Comprehensive testing and validation
"""

__version__ = "2.0.0"
__author__ = "NLM Research Team"
__license__ = "MIT"

# Core imports
from pynlm import (
    # Core types and classes
    NLMConfig,
    ConfigManager,
    SimulationStatus,
    EventType,
    
    # Main interfaces
    AdvancedNLM,
    EnhancedBrainWrapper,
    Simulation,
    
    # Convenience functions
    create_brain,
    create_config,
    create_simulation,
    run_experiment,
    train_brain,
    evaluate_brain,
    
    # Utility functions
    get_version,
    setup_logging,
    benchmark_performance,
)

# Re-export commonly used classes and functions for convenience
__all__ = [
    # Configuration classes
    'NLMConfig',
    'ConfigManager',
    
    # Simulation classes
    'Simulation',
    'SimulationStatus',
    'EventType',
    
    # Brain classes
    'AdvancedNLM',
    'EnhancedBrainWrapper',
    
    # Convenience functions
    'create_brain',
    'create_config',
    'create_simulation',
    'run_experiment',
    'train_brain',
    'evaluate_brain',
    
    # Utility functions
    'get_version',
    'setup_logging',
    'benchmark_performance',
    
    # Preset configurations
    'get_default_config',
    'get_minimal_config',
    'get_high_performance_config',
    'get_research_config',
    
    # Context managers
    'nlm_session',
]

# Import preset configurations and context managers
from pynlm import (
    get_default_config,
    get_minimal_config,
    get_high_performance_config,
    get_research_config,
    nlm_session,
)

# Create a default instance for quick usage
_default_brain: Optional[AdvancedNLM] = None


def get_default_brain() -> AdvancedNLM:
    """Get or create default brain instance."""
    global _default_brain
    if _default_brain is None:
        _default_brain = AdvancedNLM()
    return _default_brain


# Quick start examples
EXAMPLES = {
    "basic_usage": """
# Basic usage example
from nlm import AdvancedNLM, create_config

# Create brain with default configuration
brain = AdvancedNLM()

# Run simulation
results = brain.run_experiment("basic_test", 100)
print(f"Completed {results['steps_completed']} steps")
print(f"Final firing rate: {results['final_brain_state']['firing_rate']:.2f} Hz")
""",
    
    "quick_test": """
# Quick test example
from nlm import AdvancedNLM, get_minimal_config

# Create brain with minimal configuration for fast testing
config = get_minimal_config()
brain = AdvancedNLM(config)

# Run quick test
results = brain.run_experiment("quick_test", 50)
print("Quick test completed successfully!")
""",
    
    "training_example": """
# Training example
from nlm import AdvancedNLM

# Create brain
brain = AdvancedNLM()

# Train brain
training_results = brain.train_brain(
    num_episodes=100,
    max_steps_per_episode=200,
)

print(f"Training completed!")
print(f"Best reward: {training_results['best_reward']:.3f}")
print(f"Average reward: {training_results['average_reward']:.3f}")
""",
    
    "evaluation_example": """
# Evaluation example
from nlm import AdvancedNLM

# Create brain
brain = AdvancedNLM()

# Define test scenarios
test_scenarios = [
    {"name": "stability_test", "steps": 100},
    {"name": "learning_test", "steps": 200},
    {"name": "development_test", "steps": 150},
]

# Evaluate
evaluation_results = brain.evaluate_brain(test_scenarios)

print(f"Evaluated {evaluation_results['total_scenarios']} scenarios")
print(f"Average final spike count: {evaluation_results['average_performance']['final_spike_count']:.1f}")
""",
}


# Performance benchmarks
BENCHMARKS = {
    "basic_simulation": {
        "description": "Basic simulation performance",
        "config": {"neuron_count": 100, "steps": 100},
        "expected_steps_per_second": 100,
    },
    
    "medium_simulation": {
        "description": "Medium-scale simulation performance",
        "config": {"neuron_count": 1000, "steps": 500},
        "expected_steps_per_second": 50,
    },
    
    "learning_simulation": {
        "description": "Learning simulation performance",
        "config": {"neuron_count": 500, "steps": 1000, "plasticity_enabled": True},
        "expected_steps_per_second": 25,
    },
}


# Configuration templates
CONFIG_TEMPLATES = {
    "research": {
        "name": "Research Configuration",
        "description": "Configuration optimized for research experiments",
        "config": get_research_config(),
    },
    
    "production": {
        "name": "Production Configuration", 
        "description": "Configuration optimized for production use",
        "config": get_high_performance_config(),
    },
    
    "testing": {
        "name": "Testing Configuration",
        "description": "Configuration optimized for fast testing",
        "config": get_minimal_config(),
    },
}


def show_examples() -> None:
    """Display available examples."""
    print("Available Examples:")
    print("=" * 50)
    
    for example_name, example_code in EXAMPLES.items():
        print(f"\n{example_name.upper().replace('_', ' ')}:")
        print("-" * 30)
        print(example_code.strip())
    
    print("\nTo use an example, import NLM and copy the relevant code.")


def show_benchmarks() -> None:
    """Display available benchmarks."""
    print("Available Benchmarks:")
    print("=" * 50)
    
    for benchmark_name, benchmark_info in BENCHMARKS.items():
        print(f"\n{benchmark_name.replace('_', ' ').title()}:")
        print(f"  Description: {benchmark_info['description']}")
        print(f"  Expected Performance: {benchmark_info['expected_steps_per_second']} steps/second")


def show_config_templates() -> None:
    """Display available configuration templates."""
    print("Available Configuration Templates:")
    print("=" * 50)
    
    for template_name, template_info in CONFIG_TEMPLATES.items():
        print(f"\n{template_info['name']}:")
        print(f"  Description: {template_info['description']}")


# Error handling utilities
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass
class ConfigurationError(NLMError):
    """Configuration-related error."""
    pass
class SimulationError(NLMError):
    """Simulation-related error."""
    pass
class ValidationError(NLMError):
    """Validation-related error."""
    pass


def safe_execute(func: Callable) -> Callable:
    """
    Decorator for safe NLM execution.
    
    Args:
        func: Function to decorate
        
    Returns:
        Decorated function
    """
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            logger = logging.getLogger(__name__)
            logger.error(f"Error in {func.__name__}: {e}")
            raise
    
    return wrapper


def validate_environment() -> Dict[str, Any]:
    """
    Validate NLM environment.
    
    Returns:
        Dictionary with validation results
    """
    validation_results = {
        "version": get_version(),
        "python_version": "3.8+",
        "dependencies": {
            "numpy": True,
            "psutil": True,
        },
        "modules": {
            "pynlm_core": True,
            "pynlm_advanced": True,
        },
        "status": "ready",
    }
    
    try:
        import numpy
        validation_results["dependencies"]["numpy"] = True
    except ImportError:
        validation_results["dependencies"]["numpy"] = False
        validation_results["status"] = "warning"
    
    try:
        import psutil
        validation_results["dependencies"]["psutil"] = True
    except ImportError:
        validation_results["dependencies"]["psutil"] = False
        validation_results["status"] = "warning"
    
    return validation_results


# Module initialization
if __name__ == "__main__":
    print(f"NLM Python Interface v{get_version()}")
    print("=" * 50)
    print("Available commands:")
    print("  show_examples()     - Display example code")
    print("  show_benchmarks()   - Display benchmark information")
    print("  show_config_templates() - Display configuration templates")
    print("  validate_environment() - Validate environment setup")
    
    # Example usage
    print("\nExample usage:")
    print("-" * 30)
    print(EXAMPLES["basic_usage"])