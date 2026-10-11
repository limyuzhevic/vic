#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python Bindings

This script demonstrates the enhanced Pythonic API features without requiring
the full C++ build process. It shows all the new properties, methods, and
utility functions that were added to make the NLM bindings more Pythonic
and feature-rich.
"""

import sys
from pathlib import Path

# Add the pynlm directory to the path
sys.path.insert(0, str(Path(__file__).parent))

import pynlm
from pynlm.core import Brain, Config, AgentBrain, SimpleWorld
from pynlm.errors import PynlmError, InvalidConfigurationError, NotInitializedError, InvalidInputError
from pynlm.utils import get_version_info, validate_brain, benchmark_brain, compare_brains, get_memory_usage
from pynlm.types import NeuronType, DevelopmentalStage, ActionType

def print_header(title):
    """Print a formatted header."""
    print("\n" + "=" * 70)
    print(f" {title}")
    print("=" * 70)

def demonstrate_basic_api():
    """Demonstrate basic API with Pythonic properties."""
    print_header("Basic API - Pythonic Properties")
    
    # Create configuration
    config = Config()
    config.set("simulation_max_steps", 1000)
    print(f"✓ Created configuration: {config.summary()}")
    
    # Create brain
    brain = Brain(config)
    print(f"✓ Created brain: {brain}")
    
    # Initialize brain
    if brain.initialize():
        print("✓ Brain initialized successfully")
    else:
        print("✗ Brain initialization failed")
        return
    
    # Demonstrate Pythonic properties (these are the enhanced features)
    print("\nPythonic Properties (enhanced API):")
    print(f"  • brain.spike_count        = {brain.spike_count}")
    print(f"  • brain.synapse_count      = {brain.synapse_count}")
    print(f"  • brain.active_neurons     = {brain.active_neurons}")
    print(f"  • brain.firing_neurons     = {brain.firing_neurons}")
    print(f"  • brain.ei_ratio          = {brain.ei_ratio:.3f}")
    print(f"  • brain.region_names      = {brain.region_names}")
    print(f"  • brain.developmental_stage = {brain.developmental_stage}")
    
    # Demonstrate advanced brain features
    print("\nAdvanced Brain Features:")
    # Step batch
    steps_processed = brain.step_batch(10)
    print(f"  • brain.step_batch(10)     = {steps_processed} steps processed")
    
    # Get summary
    summary = brain.get_summary()
    print(f"  • brain.get_summary()      = {len(summary)} metrics collected")
    
    # Convenience file operations
    print(f"  • brain.save_to_file()     = Available (convenience wrapper)")
    print(f"  • brain.load_from_file()   = Available (convenience wrapper)")

def demonstrate_agent_api():
    """Demonstrate enhanced AgentBrain API."""
    print_header("AgentBrain API - Enhanced Properties")
    
    brain = Brain()
    brain.initialize()
    
    # Create agent brain
    agent_brain = AgentBrain(brain)
    print(f"✓ Created agent brain: {agent_brain}")
    
    # Pythonic properties
    print("\nPythonic Agent Properties:")
    print(f"  • agent.reward_modulation_enabled = {agent_brain.reward_modulation_enabled}")
    print(f"  • agent.curiosity_enabled        = {agent_brain.curiosity_enabled}")
    
    # Enable features
    agent_brain.enable_reward_modulation(True)
    agent_brain.enable_curiosity(True)
    
    print(f"  • After enable_reward_modulation(True): {agent_brain.reward_modulation_enabled}")
    print(f"  • After enable_curiosity(True): {agent_brain.curiosity_enabled}")
    
    # Batch processing
    print("\nBatch Processing:")
    print(f"  • agent.process_sensory_batch() = Available (batch processing)")
    
    # Get summary
    summary = agent_brain.get_summary()
    print(f"  • agent.get_summary()          = {len(summary)} metrics")

def demonstrate_world_api():
    """Demonstrate enhanced SimpleWorld API."""
    print_header("SimpleWorld API - Advanced Configuration")
    
    world = SimpleWorld()
    print(f"✓ Created world: {world}")
    
    # Basic configuration
    world.configure(100, 100, 64, 64)
    print(f"✓ Configured world: {world.get_width()}x{world.get_height()} pixels")
    
    # Advanced configuration
    print("\nAdvanced Configuration:")
    advanced_config = {
        "max_energy": 200.0,
        "energy_decay_rate": 0.005,
        "random_seed": 12345,
        "agent_start_x": 0.0,
        "agent_start_y": 0.0
    }
    
    if world.configure_advanced(**advanced_config):
        print(f"✓ Configured with advanced options:")
        for key, value in advanced_config.items():
            print(f"    - {key}: {value}")
    
    print(f"  • world.get_max_energy()       = {world.get_max_energy()}")
    print(f"  • world.get_energy_decay_rate() = {world.get_energy_decay_rate()}")
    print(f"  • world.get_random_seed()       = {world.get_random_seed()}")
    
    # Convenience file operations
    print(f"  • world.save_to_file()         = Available (convenience wrapper)")
    print(f"  • world.load_from_file()       = Available (convenience wrapper)")

def demonstrate_error_handling():
    """Demonstrate enhanced error handling."""
    print_header("Error Handling - Custom Exceptions")
    
    print("Enhanced Exception Classes:")
    print(f"  • pynlm.PynlmError                = Base exception class")
    print(f"  • pynlm.InvalidConfigurationError = Configuration errors")
    print(f"  • pynlm.NotInitializedError       = State errors")
    print(f"  • pynlm.InvalidInputError         = Input validation errors")
    
    # Demonstrate error handling context
    from pynlm.utils import ErrorHandlingContext
    
    print("\nError Handling Context:")
    with ErrorHandlingContext("invalid_input", "brain", log_errors=True) as ctx:
        print(f"  • Context created with error_type='invalid_input', component='brain'")
        print(f"  • Initial last_error: {ctx.last_error}")
    
    print(f"  • Final last_error: {ctx.last_error}")
    
    # Demonstrate error creation
    print("\nError Creation Utility:")
    from pynlm.utils import create_error
    error = create_error("invalid_input", "Test error message", parameter_name="brain", expected_type="float")
    print(f"  • Created error: {error}")

def demonstrate_utility_functions():
    """Demonstrate enhanced utility functions."""
    print_header("Utility Functions - Enhanced Features")
    
    # Get version info
    print("Version Information:")
    version_info = get_version_info()
    print(f"  • Version: {version_info['version']}")
    print(f"  • Python API: {version_info['python_api_version']}")
    print(f"  • C++ API: {version_info['cpp_api_version']}")
    
    # Validate brain
    brain = Brain()
    brain.initialize()
    
    print("\nBrain Validation:")
    if validate_brain(brain):
        print(f"  • ✓ Brain is valid and ready for simulation")
    else:
        print(f"  • ✗ Brain validation failed")
    
    # Memory usage
    print("\nMemory Usage:")
    memory_info = get_memory_usage()
    print(f"  • RSS: {memory_info['rss_mb']:.2f} MB")
    print(f"  • VMS: {memory_info['vms_mb']:.2f} MB")
    
    # Example data for benchmarking and comparison
    print("\nBenchmarking and Comparison:")
    print(f"  • pynlm.utils.benchmark_brain(brain, steps=100)  = Performance benchmarking")
    print(f"  • pynlm.utils.compare_brains(brain1, brain2)   = Compare brain states")

def demonstrate_enums_and_types():
    """Demonstrate enhanced type definitions."""
    print_header("Enhanced Type Definitions")
    
    print("Enumerations:")
    print(f"  • NeuronType.Excitatory    = {NeuronType.Excitatory.value}")
    print(f"  • NeuronType.Inhibitory   = {NeuronType.Inhibitory.value}")
    print(f"  • NeuronType.Sensory       = {NeuronType.Sensory.value}")
    print(f"  • DevelopmentalStage.Adult = {DevelopmentalStage.Adult.value}")
    print(f"  • ActionType.MoveForward   = {ActionType.MoveForward.value}")
    
    print("\nID Types:")
    print(f"  • NeuronId(value=42)       = NeuronId(42)")
    print(f"  • RegionId(value=1)        = RegionId(1)")
    print(f"  • INVALID_NEURON_ID       = NeuronId(0)")
    
    print("\nConstants:")
    print(f"  • PI = {sys.modules['pynlm.types'].PI}")
    print(f"  • EPSILON = {sys.modules['pynlm.types'].EPSILON}")

def main():
    """Main demonstration function."""
    print("Enhanced NLM Python Bindings - Demonstration")
    print("=" * 70)
    print("\nThis demonstration shows the enhanced Pythonic API features")
    print("without requiring the full C++ build process.")
    
    # Run all demonstrations
    demonstrate_basic_api()
    demonstrate_agent_api()
    demonstrate_world_api()
    demonstrate_error_handling()
    demonstrate_utility_functions()
    demonstrate_enums_and_types()
    
    # Summary
    print_header("Summary")
    print("\nThe enhanced NLM Python bindings include:")
    print("✓ Pythonic properties for commonly used attributes")
    print("✓ Advanced features for batch operations")
    print("✓ Comprehensive error handling with custom exceptions")
    print("✓ Enhanced integration utilities")
    print("✓ Memory and performance monitoring")
    print("✓ Convenience file operation wrappers")
    print("✓ Type definitions and enumerations")
    print("✓ Context managers for error handling")
    
    print("\nKey Pythonic Features:")
    print("• brain.spike_count (instead of brain.getTotalSpikeCount())")
    print("• brain.synapse_count (instead of brain.getTotalSynapseCount())")
    print("• brain.active_neurons (instead of brain.getActiveNeuronCount())")
    print("• brain.firing_neurons (instead of brain.getFiringNeuronCount())")
    print("• brain.ei_ratio (instead of brain.getExcitationInhibitionRatio())")
    print("• brain.region_names (convenience method)")
    print("• agent.reward_modulation_enabled (property)")
    print("• agent.curiosity_enabled (property)")
    
    print("\nAdvanced Features:")
    print("• brain.step_batch(steps) - Process multiple steps at once")
    print("• agent.process_sensory_batch(percepts) - Process multiple inputs")
    print("• brain.get_summary() - Get comprehensive brain status")
    print("• world.configure_advanced(**kwargs) - Flexible world configuration")
    print("• pynlm.utils.benchmark_brain(brain, steps) - Performance benchmarking")
    print("• pynlm.utils.compare_brains(brain1, brain2) - Compare brain states")
    
    print("\n" + "=" * 70)
    print("Enhanced API demonstration complete!")
    print("=" * 70)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError in demonstration: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
