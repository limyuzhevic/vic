# pynlm - Main module entry point
"""
Enhanced Python bindings for NLM (Neural Learning Machine)

This module provides a comprehensive Python interface to the NLM neural simulation framework
with advanced features for research and development.

Usage:
    import pynlm
    
    # Create brain with advanced configuration
    config = pynlm.create_default_config()
    config.set("brain.neuron_count", 2000)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.create_brain(config)
    for step in range(100):
        brain.step(step)
    
    print(f"Simulation complete! {brain.get_firing_neurons()} neurons firing")
"""

# Import all public API from submodules
from pynlm.core import (
    NLMConfig,
    NLMBrain,
    SimpleWorld,
    AgentBrain,
    PerformanceProfiler,
    PerformanceMetrics,
    SimulationStats,
    LogLevel,
    NeuronType,
    DevelopmentalStage,
    ActionType,
)

from pynlm.simulation import (
    run_simulation,
    run_multiple_simulations,
    create_advanced_brain,
    create_reduced_brain,
)

from pynlm.utils import (
    get_performance_stats,
    reset_performance_stats,
    cleanup_memory,
    set_log_level,
)

from pynlm.benchmark import (
    benchmark_brain,
    benchmark_configuration,
    benchmark_agent,
)

from pynlm.explorer import (
    explore_brain,
    explore_configuration,
    interactive_demo,
)

from pynlm.developer import (
    create_developer_config,
    validate_config,
    optimize_configuration,
)

# Re-export all public API
__all__ = [
    # Core classes
    'NLMConfig',
    'NLMBrain', 
    'SimpleWorld',
    'AgentBrain',
    'PerformanceProfiler',
    'PerformanceMetrics',
    'SimulationStats',
    
    # Enums
    'LogLevel',
    'NeuronType',
    'DevelopmentalStage',
    'ActionType',
    
    # Simulation utilities
    'run_simulation',
    'run_multiple_simulations',
    'create_advanced_brain',
    'create_reduced_brain',
    
    # Utility functions
    'get_performance_stats',
    'reset_performance_stats',
    'cleanup_memory',
    'set_log_level',
    
    # Benchmarking
    'benchmark_brain',
    'benchmark_configuration',
    'benchmark_agent',
    
    # Exploration
    'explore_brain',
    'explore_configuration',
    'interactive_demo',
    
    # Development
    'create_developer_config',
    'validate_config',
    'optimize_configuration',
]

# Global instances and configuration
_global_config = None
_global_logger = None

# Version and metadata
__version__ = "2.0.0"
__author__ = "NLM Team"
__description__ = "Enhanced Python bindings for NLM neural simulation framework"
__license__ = "MIT"
__copyright__ = "Copyright 2024 NLM Team"

# Performance monitoring
_performance_profiler = None

# Module initialization
import sys
import time
from typing import Any, Dict, List, Optional, Callable

def initialize() -> None:
    """Initialize the pynlm module"""
    global _global_config, _global_logger, _performance_profiler
    
    # Initialize global configuration
    _global_config = NLMConfig()
    
    # Initialize performance profiler
    _performance_profiler = PerformanceProfiler()
    
    # Log module initialization
    if _global_logger:
        _global_logger.info(f"NLM Python bindings v{__version__} initialized")
    
    # Register cleanup function
    import atexit
    atexit.register(cleanup)

def cleanup() -> None:
    """Cleanup module resources"""
    global _performance_profiler
    
    if _performance_profiler:
        _performance_profiler.reset()
    
    # Cleanup other resources
    if _global_logger:
        _global_logger.info("NLM Python bindings cleanup completed")

def get_version() -> str:
    """Get NLM Python bindings version"""
    return __version__

def get_info() -> Dict[str, Any]:
    """Get information about NLM installation"""
    import platform
    import numpy as np
    
    return {
        'version': __version__,
        'author': __author__,
        'description': __description__,
        'license': __license__,
        'platform': platform.platform(),
        'python_version': platform.python_version(),
        'numpy_version': np.__version__,
        'build_type': 'scikit-build',
        'features': [
            'enhanced_api',
            'performance_monitoring',
            'batch_processing',
            'advanced_configuration',
            'memory_management',
            'cli_interface',
            'development_tools',
            'benchmarking',
            'exploration',
        ],
        'modules': {
            'core': 'Core neural simulation classes',
            'simulation': 'Simulation utilities',
            'utils': 'Utility functions',
            'benchmark': 'Benchmarking tools',
            'explorer': 'Exploration tools',
            'developer': 'Development tools',
        }
    }

# Command line interface
import argparse
from pathlib import Path

def main() -> None:
    """Main command line interface"""
    parser = argparse.ArgumentParser(
        description=f"NLM Python Bindings v{__version__}",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  pynlm-info                    # Show installation information
  pynlm-run --steps 100         # Run simple simulation
  pynlm-benchmark --neurons 1000  # Run benchmark
  pynlm-explore --mode interactive  # Open interactive explorer
        """
    )
    
    parser.add_argument(
        '--version', '-v',
        action='version',
        version=f"NLM Python Bindings {__version__}"
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Info command
    info_parser = subparsers.add_parser('info', help='Show installation information')
    info_parser.set_defaults(func=lambda args: print_info())
    
    # Run command
    run_parser = subparsers.add_parser('run', help='Run simulation')
    run_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    run_parser.add_argument('--config', help='Configuration file path')
    run_parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    run_parser.add_argument('--world-size', nargs=2, type=int, metavar=('WIDTH', 'HEIGHT'), help='World dimensions')
    run_parser.set_defaults(func=lambda args: run_simulation_command(args))
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Run benchmark tests')
    benchmark_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    benchmark_parser.add_argument('--iterations', type=int, default=10, help='Number of iterations')
    benchmark_parser.add_argument('--regions', type=int, default=2, help='Number of regions')
    benchmark_parser.set_defaults(func=lambda args: benchmark_command(args))
    
    # Explore command
    explore_parser = subparsers.add_parser('explore', help='Open interactive explorer')
    explore_parser.add_argument('--mode', choices=['interactive', 'batch', 'visual'], 
                               default='interactive', help='Exploration mode')
    explore_parser.add_argument('--brain-file', help='Brain file to load')
    explore_parser.set_defaults(func=lambda args: explore_command(args))
    
    # Developer command
    dev_parser = subparsers.add_parser('dev', help='Development tools')
    dev_parser.add_argument('--config', help='Configuration file path')
    dev_parser.add_argument('--validate', action='store_true', help='Validate configuration')
    dev_parser.add_argument('--optimize', action='store_true', help='Optimize configuration')
    dev_parser.set_defaults(func=lambda args: dev_command(args))
    
    # Parse and execute command
    args = parser.parse_args()
    
    if hasattr(args, 'func'):
        args.func(args)
    else:
        parser.print_help()

def print_info() -> None:
    """Print installation information"""
    info = get_info()
    
    print(f"NLM Python Bindings v{info['version']}")
    print(f"Author: {info['author']}")
    print(f"Description: {info['description']}")
    print(f"License: {info['license']}")
    print(f"Platform: {info['platform']}")
    print(f"Python: {info['python_version']}")
    print(f"NumPy: {info['numpy_version']}")
    print(f"Build Type: {info['build_type']}")
    
    print("\nAvailable Modules:")
    for module, description in info['modules'].items():
        print(f"  {module}: {description}")
    
    print("\nKey Features:")
    for feature in info['features']:
        print(f"  ✓ {feature.replace('_', ' ').title()}")
    
    print("\nUsage Examples:")
    print("  import pynlm")
    print("  brain = pynlm.create_brain()")
    print("  brain.step(0)")
    print("  print(f'Active neurons: {brain.get_firing_neurons()}')")

def run_simulation_command(args) -> None:
    """Handle run command"""
    print(f"Running simulation with {args.steps} steps...")
    
    # Create brain
    config = NLMConfig()
    if args.config:
        try:
            config = NLMConfig.load(args.config)
        except Exception as e:
            print(f"Warning: Could not load config: {e}")
    
    brain = create_brain(config)
    
    # Run simulation
    for step in range(args.steps):
        brain.step(step)
        if args.verbose and step % 10 == 0:
            stats = brain.get_stats()
            print(f"Step {step}: {stats.firing_neurons} firing, {stats.total_spikes} spikes")
    
    # Print final statistics
    final_stats = brain.get_stats()
    performance = brain.get_performance()
    
    print(f"\nSimulation Complete!")
    print(f"Total steps: {final_stats.step_count}")
    print(f"Firing neurons: {final_stats.firing_neurons}")
    print(f"Total spikes: {final_stats.total_spikes}")
    print(f"Average firing rate: {final_stats.average_firing_rate:.2f} Hz")
    print(f"Total simulation time: {performance.get_total_simulation_time():.3f}s")
    print(f"Average step time: {performance.get_average_step_time()*1000:.2f}ms")

def benchmark_command(args) -> None:
    """Handle benchmark command"""
    print(f"Running benchmark with {args.neurons} neurons, {args.iterations} iterations...")
    
    # Run benchmark
    results = benchmark_brain(
        neuron_count=args.neurons,
        region_count=args.regions,
        iterations=args.iterations
    )
    
    print(f"\nBenchmark Results:")
    print(f"Total time: {results['total_time']:.3f}s")
    print(f"Steps per second: {results['steps_per_second']:.1f}")
    print(f"Average step time: {results['average_step_time_ms']:.2f}ms")
    print(f"Standard deviation: {results['step_time_std_ms']:.2f}ms")
    print(f"Memory efficiency: {results['memory_efficiency']:.2f} neurons/ms")

def explore_command(args) -> None:
    """Handle explore command"""
    print(f"Opening NLM explorer in {args.mode} mode...")
    
    # Load brain if specified
    brain = None
    if args.brain_file:
        try:
            brain = explore_brain(args.brain_file)
        except Exception as e:
            print(f"Warning: Could not load brain: {e}")
    
    # Open appropriate exploration mode
    if args.mode == 'interactive':
        print("Interactive mode - not implemented in this demo")
        print("Available features:")
        print("  - Brain visualization")
        print("  - Parameter tuning")
        print("  - Performance analysis")
        print("  - Experiment management")
    
    elif args.mode == 'batch':
        print("Batch mode - not implemented in this demo")
        print("Features available in simulation runner")
    
    elif args.mode == 'visual':
        print("Visual mode - requires matplotlib")
        print("Features available in extended package")

def dev_command(args) -> None:
    """Handle developer command"""
    if args.validate:
        print("Validating configuration...")
        if args.config:
            try:
                config = NLMConfig.load(args.config)
                validate_config(config)
                print("Configuration validation successful")
            except Exception as e:
                print(f"Configuration validation failed: {e}")
        else:
            print("No configuration file provided for validation")
    
    elif args.optimize:
        print("Optimizing configuration...")
        config = create_developer_config()
        optimized = optimize_configuration(config)
        print(f"Optimization completed. Neurons: {optimized.get('brain.neuron_count', 'N/A')}")
        print(f"Plasticity: {optimized.get('plasticity.stdp.enable', 'N/A')}")
    
    else:
        print("Development tools available:")
        print("  --validate: Validate configuration file")
        print("  --optimize: Optimize configuration")
        print("  --config: Configuration file path")

# Initialize module
initialize()

# Print welcome message
print(f"Welcome to NLM Python Bindings v{__version__}")
print(f"Type 'pynlm-info' for more information")
print(f"Type 'pynlm --help' for command line options")
