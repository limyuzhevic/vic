#!/usr/bin/env python3
"""
Simple demonstration of the enhanced NLM Python bindings

This is a simplified demonstration that shows the key features
of the enhanced API without requiring the full C++ build process.
It focuses on the Pythonic properties and advanced features that
have been added to make the NLM bindings more Pythonic.
"""

import sys
from pathlib import Path

# Add the pynlm directory to the path
sys.path.insert(0, str(Path(__file__).parent))

# Import the core classes directly from the module
from pynlm.core import Brain, Config, AgentBrain, SimpleWorld
from pynlm.errors import PynlmError, InvalidConfigurationError, NotInitializedError, InvalidInputError
from pynlm.utils import get_version_info, validate_brain, benchmark_brain, compare_brains
from pynlm.types import NeuronType, DevelopmentalStage

def print_demo_header(title):
    """Print a formatted header."""
    print("\n" + "=" * 70)
    print(f" {title}")
    print("=" * 70)

def demonstrate_pythonic_properties():
    """Demonstrate the key Pythonic properties added to the enhanced API."""
    print_demo_header("Enhanced Pythonic Properties")
    
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
    
    # Demonstrate Pythonic properties (the main enhancement)
    print("\nPythonic Properties (Enhanced API Features):")
    print("   brain.spike_count        = (alias for brain.getTotalSpikeCount())")
    print("   brain.synapse_count      = (alias for brain.getTotalSynapseCount())")
    print("   brain.active_neurons     = (alias for brain.getActiveNeuronCount())")
    print("   brain.firing_neurons     = (alias for brain.getFiringNeuronCount())")
    print("   brain.ei_ratio          = (alias for brain.getExcitationInhibitionRatio())")
    print("   brain.region_names      = (convenience for getting region names)")
    print("   agent.reward_modulation_enabled = (property access)")
    print("   agent.curiosity_enabled        = (property access)")
    
    # Show actual property values
    print("\nProperty Values:")
    print(f"   • brain.spike_count: {brain.spike_count}")
    print(f"   • brain.synapse_count: {brain.synapse_count}")
    print(f"   • brain.active_neurons: {brain.active_neurons}")
    print(f"   • brain.firing_neurons: {brain.firing_neurons}")
    print(f"   • brain.ei_ratio: {brain.ei_ratio:.3f}")
    print(f"   • brain.region_names: {brain.region_names}")
    
    # Demonstrate batch operations
    print("\nBatch Operations:")
    print(f"   • brain.step_batch(steps) - Process multiple simulation steps at once")
    print(f"   • agent.process_sensory_batch(percepts) - Process multiple sensory inputs")
    
    # Show summary method
    print("\nEnhanced Summary Method:")
    summary = brain.get_summary()
    print(f"   • brain.get_summary() returns comprehensive brain status")
    print(f"   • Contains {len(summary)} metrics including:")
    for key in list(summary.keys())[:5]:  # Show first 5
        print(f"     - {key}")
    if len(summary) > 5:
        print(f"     - ... and {len(summary) - 5} more metrics")

def demonstrate_agent_enhancements():
    """Demonstrate enhanced AgentBrain features."""
    print_demo_header("Enhanced AgentBrain API")
    
    brain = Brain()
    brain.initialize()
    
    # Create agent brain
    agent_brain = AgentBrain(brain)
    print(f"✓ Created agent brain: {agent_brain}")
    
    # Pythonic properties for AgentBrain
    print("\nPythonic AgentBrain Properties:")
    print(f"   • agent.reward_modulation_enabled = (property access)")
    print(f"   • agent.curiosity_enabled        = (property access)")
    
    # Show actual values
    print(f"   • Current reward_modulation_enabled: {agent_brain.reward_modulation_enabled}")
    print(f"   • Current curiosity_enabled: {agent_brain.curiosity_enabled}")
    
    # Demonstrate agent-specific methods
    print("\nAgentBrain Methods:")
    print(f"   • agent.process_sensory_batch(percepts) - Process multiple inputs")
    print(f"   • agent.get_summary() - Get comprehensive agent brain status")
    print(f"   • agent.enable_reward_modulation(True/False)")
    print(f"   • agent.enable_curiosity(True/False)")

def demonstrate_world_enhancements():
    """Demonstrate enhanced SimpleWorld features."""
    print_demo_header("Enhanced SimpleWorld API")
    
    world = SimpleWorld()
    print(f"✓ Created world: {world}")
    
    # Basic configuration
    world.configure(100, 100, 64, 64)
    print(f"✓ Configured world: {world.get_width()}x{world.get_height()} pixels")
    
    # Enhanced configuration method
    print("\nEnhanced World Configuration:")
    print(f"   • world.configure_advanced(**kwargs) - More flexible configuration")
    print(f"   • Accepts keyword arguments like max_energy, energy_decay_rate, random_seed")
    
    # Demonstrate enhanced file operations
    print("\nEnhanced File Operations:")
    print(f"   • world.save_to_file(filename) - Convenience wrapper for save()")
    print(f"   • world.load_from_file(filename) - Convenience wrapper for load()")

def demonstrate_error_handling():
    """Demonstrate enhanced error handling."""
    print_demo_header("Enhanced Error Handling")
    
    print("Custom Exception Classes:")
    print(f"   • PynlmError - Base exception for all NLM errors")
    print(f"   • InvalidConfigurationError - Configuration-related errors")
    print(f"   • NotInitializedError - State-related errors")
    print(f"   • InvalidInputError - Input validation errors")
    
    print("\nError Handling Features:")
    print(f"   • pynlm.utils.ErrorHandlingContext - Structured error handling")
    print(f"   • pynlm.utils.create_error(error_type, message) - Error factory")
    print(f"   • pynlm.utils.safe_execute(func, *args) - Safe function execution")
    
    # Demonstrate error handling context
    print("\nError Handling Context Example:")
    from pynlm.utils import ErrorHandlingContext
    with ErrorHandlingContext("invalid_input", "brain", log_errors=True) as ctx:
        print(f"   • Created context with error_type='invalid_input', component='brain'")
        print(f"   • Initial last_error: {ctx.last_error}")

def demonstrate_utility_functions():
    """Demonstrate enhanced utility functions."""
    print_demo_header("Enhanced Utility Functions")
    
    # Get version info
    print("Version Information:")
    version_info = get_version_info()
    print(f"   • Version: {version_info['version']}")
    print(f"   • Python API: {version_info['python_api_version']}")
    print(f"   • C++ API: {version_info['cpp_api_version']}")
    
    # Validate brain
    brain = Brain()
    brain.initialize()
    
    print("\nBrain Validation:")
    if validate_brain(brain):
        print(f"   • ✓ Brain is valid and ready for simulation")
    else:
        print(f"   • ✗ Brain validation failed")
    
    # Memory usage
    print("\nMemory Monitoring:")
    from pynlm.utils import get_memory_usage
    memory_info = get_memory_usage()
    print(f"   • RSS: {memory_info['rss_mb']:.2f} MB")
    print(f"   • VMS: {memory_info['vms_mb']:.2f} MB")
    
    # Benchmarking
    print("\nPerformance Benchmarking:")
    print(f"   • pynlm.utils.benchmark_brain(brain, steps=1000)")
    print(f"   • Returns detailed performance metrics")
    
    # Brain comparison
    print("\nBrain Comparison:")
    print(f"   • pynlm.utils.compare_brains(brain1, brain2)")
    print(f"   • Compares brain states and detects differences")

def main():
    """Main demonstration function."""
    print("Enhanced NLM Python Bindings - Demonstration")
    print("=" * 70)
    print("\nThis demonstration shows the enhanced Pythonic API features")
    print("without requiring the full C++ build process.")
    
    # Run all demonstrations
    demonstrate_pythonic_properties()
    demonstrate_agent_enhancements()
    demonstrate_world_enhancements()
    demonstrate_error_handling()
    demonstrate_utility_functions()
    
    # Summary
    print_demo_header("Summary of Enhanced API")
    print("\nKey Improvements Made:")
    print("✓ Pythonic Properties - Intuitive property access")
    print("✓ Batch Operations - Efficient processing of multiple items")
    print("✓ Enhanced Configuration - Flexible world and brain configuration")
    print("✓ Comprehensive Error Handling - Custom exceptions and context managers")
    print("✓ Advanced Utilities - Benchmarking, validation, comparison")
    print("✓ Convenience Methods - Simplified file operations and summaries")
    print("✓ Type Definitions - Enhanced type system with better documentation")
    
    print("\nPythonic Properties Added:")
    print("• brain.spike_count (instead of brain.getTotalSpikeCount())")
    print("• brain.synapse_count (instead of brain.getTotalSynapseCount())")
    print("• brain.active_neurons (instead of brain.getActiveNeuronCount())")
    print("• brain.firing_neurons (instead of brain.getFiringNeuronCount())")
    print("• brain.ei_ratio (instead of brain.getExcitationInhibitionRatio())")
    print("• brain.region_names (convenience method)")
    print("• agent.reward_modulation_enabled (property)")
    print("• agent.curiosity_enabled (property)")
    
    print("\nAdvanced Features Added:")
    print("• brain.step_batch(steps) - Process multiple simulation steps")
    print("• agent.process_sensory_batch(percepts) - Process multiple inputs")
    print("• brain.get_summary() - Get comprehensive brain status")
    print("• world.configure_advanced(**kwargs) - Flexible configuration")
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
