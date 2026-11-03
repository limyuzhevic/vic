#!/usr/bin/env python3
"""
Quick Start Guide for Improved NLM Python Bindings

This script provides a quick demonstration of the improved NLM Python API
and shows how to get started with the enhanced features.

To run this guide:
1. Ensure the NLM Python bindings are installed:
   pip install -e .  # or pip install .
2. Run this script: python quick_start.py
"""

import pynlm

def main():
    print("NLM Python Bindings - Quick Start")
    print("=" * 50)
    print()
    
    print("The NLM Python bindings have been significantly improved with:")
    print()
    
    # 1. Enhanced Error Handling
    print("1. ENHANCED ERROR HANDLING")
    print("-" * 30)
    print("The Python bindings now provide better error messages and validation.")
    print("Invalid operations are caught with clear error messages.")
    print()
    
    # 2. Convenience Methods
    print("2. CONVENIENCE METHODS")
    print("-" * 30)
    print("Actions now have helper methods like:")
    
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    print(f"  - action.is_move_action(): {action.is_move_action()}")
    print(f"  - action.is_look_action(): {action.is_look_action()}")
    print()
    
    # 3. Dictionary Configuration
    print("3. DICTIONARY CONFIGURATION")
    print("-" * 30)
    print("Configuration can be created from Python dictionaries:")
    
    config_dict = {
        "brain.neuron_count": 200,
        "brain.connection_probability": 0.1,
        "plasticity.stdp.enable": True,
    }
    
    config = pynlm.create_config_from_dict(config_dict)
    print(f"  - Created config from dict with {config.get_keys()} keys")
    print()
    
    # 4. Enhanced Documentation
    print("4. IMPROVED DOCUMENTATION")
    print("-" * 30)
    print("All methods now have comprehensive docstrings with examples.")
    print()
    
    # 5. Template-Based ID Types
    print("5. CLEANER CODE (Template-Based ID Types)")
    print("-" * 30)
    print("ID types (NeuronId, SynapseId, etc.) are now generated with templates")
    print("to reduce code duplication and improve maintainability.")
    print()
    
    # 6. Advanced Features
    print("6. ADVANCED FEATURES")
    print("-" * 30)
    print("New features include:")
    
    # Text encoding
    pattern = pynlm.encode_text("test")
    print(f"  - Text encoding: 'test' -> pattern with {len(pattern)} neurons")
    
    # Enhanced brain creation
    brain = pynlm.create_brain(config)
    print(f"  - Enhanced brain creation: {brain.get_total_neuron_count()} neurons")
    
    # World simulation
    world = pynlm.create_simple_world()
    print(f"  - World simulation with configurable dimensions")
    print()
    
    # 7. Improved Build Configuration
    print("7. IMPROVED BUILD CONFIGURATION")
    print("-" * 30)
    print("CMakeLists.txt now:")
    print("  - Enables Python bindings by default (recommended)")
    print("  - Provides better installation paths")
    print("  - Supports development and production builds")
    print("  - Better CMake organization and documentation")
    print()
    
    # 8. Comprehensive Examples
    print("8. COMPREHENSIVE EXAMPLES")
    print("-" * 30)
    print("Created comprehensive example files in the examples/ directory:")
    print("  - comprehensive_examples.py: 10 complete example scripts")
    print("  - Each example demonstrates different features")
    print()
    
    # 9. Test Suite
    print("9. COMPREHENSIVE TEST SUITE")
    print("-" * 30)
    print("Created test_improved_bindings.py with:")
    print("  - Unit tests for all major features")
    print("  - Error handling tests")
    print("  - Performance monitoring")
    print("  - Comprehensive validation")
    print()
    
    # 10. Build Instructions
    print("10. UPDATED BUILD INSTRUCTIONS")
    print("-" * 30)
    print("Enhanced README.md provides:")
    print("  - Multiple build options (C++ only, Python only, both)")
    print("  - Development installation instructions")
    print("  - Better documentation and examples")
    print()
    
    # Quick demonstration
    print("QUICK DEMONSTRATION")
    print("=" * 50)
    print()
    
    print("Let's create a simple brain and simulate it:")
    
    # Create and initialize brain
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print(f"1. Brain created: {brain.get_total_neuron_count()} neurons")
    
    # Run a few simulation steps
    for i in range(5):
        brain.step(i)
        print(f"2. Step {i+1}: {brain.get_firing_neuron_count()} neurons firing")
    
    print(f"3. Final statistics:")
    print(f"   - Total spikes: {brain.get_total_spike_count()}")
    print(f"   - Avg firing rate: {brain.get_average_firing_rate():.3f} Hz")
    print(f"   - E/I ratio: {brain.get_excitation_inhibition_ratio():.3f}")
    
    print()
    print("✓ Quick start complete!")
    print()
    
    # Show how to continue learning
    print("NEXT STEPS")
    print("=" * 50)
    print("1. Read examples/comprehensive_examples.py for detailed tutorials")
    print("2. Run test_improved_bindings.py to verify functionality")
    print("3. Check documentation in docs/ for architectural details")
    print("4. Experiment with the Python API to discover new features")
    print()
    
    print("The improved NLM Python bindings make it easier than ever to:")
    print("  - Create neural simulations")
    print("  - Study brain plasticity")
    print("  - Build intelligent agents")
    print("  - Research artificial developmental systems")

if __name__ == "__main__":
    main()
