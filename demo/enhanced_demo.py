#!/usr/bin/env python3
"""
Demonstration of Enhanced NLM Python Bindings

This script demonstrates the key improvements made to the NLM Python bindings,
showing the new error handling, validation, and utility functions.
"""

import sys
import os

# Add current directory to path to import nlm_utils
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM Python bindings
try:
    import pynlm
    print("✓ NLM Python bindings imported successfully")
except ImportError as e:
    print(f"✗ Failed to import NLM Python bindings: {e}")
    sys.exit(1)

# Try to import advanced utilities
try:
    from nlm_utils.nlm_advanced import NLMExperiment, NLMPythonAPI
    ADVANCED_AVAILABLE = True
    print("✓ Advanced utilities imported successfully")
except ImportError as e:
    ADVANCED_AVAILABLE = False
    print(f"⚠ Advanced utilities not available: {e}")

def demonstrate_basic_functionality():
    """Demonstrate basic NLM functionality with enhanced bindings."""
    print("\n" + "="*60)
    print("DEMONSTRATION 1: Basic NLM Functionality")
    print("="*60)
    
    # Create configuration
    print("Creating configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    print(f"✓ Created config with {config.getOr('brain.neuron_count', 0)} neurons")
    
    # Create brain
    print("Creating brain...")
    brain = pynlm.createBrain(config)
    print("✓ Brain created")
    
    # Initialize brain
    print("Initializing brain...")
    brain.initialize()
    print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    print("Running simulation...")
    for step in range(10):
        brain.step(step)
    
    print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} total spikes")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Active neurons: {brain.getFiringNeuronCount()}")
    
    return brain

def demonstrate_error_handling():
    """Demonstrate enhanced error handling and validation."""
    print("\n" + "="*60)
    print("DEMONSTRATION 2: Enhanced Error Handling")
    print("="*60)
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Demonstrate validation
    print("Testing brain validation...")
    if hasattr(brain, 'isInitialized'):
        print(f"  - Brain initialized: {brain.isInitialized()}")
    
    if hasattr(brain, 'validate'):
        try:
            is_valid = brain.validate()
            print(f"  - Brain validation: {'✓ Valid' if is_valid else '✗ Invalid'}")
        except Exception as e:
            print(f"  - Validation error (expected): {type(e).__name__}")
    
    # Demonstrate type-safe getters
    print("\nTesting type-safe getters...")
    if hasattr(brain, 'getTotalNeuronCount'):
        neurons = brain.getTotalNeuronCount()
        print(f"  - Total neurons: {neurons}")
    
    if hasattr(brain, 'getAverageFiringRate'):
        firing_rate = brain.getAverageFiringRate()
        print(f"  - Average firing rate: {firing_rate:.2f} Hz")
    
    # Demonstrate configuration validation
    print("\nTesting configuration validation...")
    test_config = pynlm.createDefaultConfig()
    if hasattr(test_config, 'validate'):
        try:
            is_valid = test_config.validate()
            print(f"  - Config validation: {'✓ Valid' if is_valid else '✗ Invalid'}")
        except Exception as e:
            print(f"  - Config validation error (expected): {type(e).__name__}")
    
    return brain

def demonstrate_advanced_features():
    """Demonstrate advanced features if available."""
    print("\n" + "="*60)
    print("DEMONSTRATION 3: Advanced Features")
    print("="*60)
    
    if not ADVANCED_AVAILABLE:
        print("Advanced utilities not available. Skipping advanced features demonstration.")
        return
    
    # Create experiment with advanced utilities
    print("Creating advanced experiment...")
    experiment = NLMExperiment()
    
    # Setup experiment
    print("Setting up experiment environment...")
    experiment.setup_experiment(width=15, height=15, enable_all_subsystems=True)
    
    # Run experiment
    print("Running experiment...")
    results = experiment.run_episode(50, reward_strategy='adaptive', enable_logging=True)
    
    print(f"\nAdvanced experiment results:")
    print(f"  - Experiment ID: {experiment.experiment_id}")
    print(f"  - Total steps: {experiment.step_count}")
    print(f"  - Episode results:")
    for key, value in results.items():
        if key not in ['episode_id', 'step_logs']:
            if isinstance(value, float):
                print(f"    {key}: {value:.3f}")
            else:
                print(f"    {key}: {value}")
    
    # Create checkpoint
    print("\nCreating checkpoint...")
    checkpoint_file = experiment.create_checkpoint()
    print(f"  Checkpoint saved to: {checkpoint_file}")
    
    # Get performance metrics
    print("\nGetting performance metrics...")
    metrics = experiment.get_performance_metrics()
    for key, value in metrics.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    
    return experiment

def demonstrate_validation():
    """Demonstrate installation validation."""
    print("\n" + "="*60)
    print("DEMONSTRATION 4: Installation Validation")
    print("="*60)
    
    print("Validating NLM installation...")
    validation = NLMPythonAPI.validate_installation()
    
    print(f"\nValidation Results:")
    print(f"  Module imported: {validation['module_imported']}")
    print(f"  Errors found: {len(validation['errors'])}")
    print(f"  Warnings: {len(validation['warnings'])}")
    
    if validation['errors']:
        print(f"\nErrors:")
        for error in validation['errors']:
            print(f"  - {error}")
    
    if validation['warnings']:
        print(f"\nWarnings:")
        for warning in validation['warnings']:
            print(f"  - {warning}")
    
    if not validation['errors']:
        print("\n✓ Installation validation passed!")
    else:
        print("\n⚠ Installation validation has issues.")
    
    return validation

def main():
    """
    Run all demonstrations.
    """
    print("=" * 80)
    print("ENHANCED NLM PYTHON BINDINGS DEMONSTRATION")
    print("=" * 80)
    print(f"Python version: {sys.version}")
    print(f"NLM Python bindings version: {pynlm.__version__ if hasattr(pynlm, '__version__') else 'N/A'}")
    
    # Run demonstrations
    try:
        brain1 = demonstrate_basic_functionality()
    except Exception as e:
        print(f"✗ Basic functionality demonstration failed: {e}")
        brain1 = None
    
    try:
        brain2 = demonstrate_error_handling()
    except Exception as e:
        print(f"✗ Error handling demonstration failed: {e}")
        brain2 = None
    
    try:
        experiment = demonstrate_advanced_features()
    except Exception as e:
        print(f"✗ Advanced features demonstration failed: {e}")
        experiment = None
    
    try:
        validation = demonstrate_validation()
    except Exception as e:
        print(f"✗ Validation demonstration failed: {e}")
        validation = None
    
    # Summary
    print("\n" + "=" * 80)
    print("DEMONSTRATION SUMMARY")
    print("=" * 80)
    
    completed = 0
    total = 4
    
    if brain1:
        print("✓ Basic functionality demonstration completed")
        completed += 1
    
    if brain2:
        print("✓ Error handling demonstration completed")
        completed += 1
    
    if experiment:
        print("✓ Advanced features demonstration completed")
        completed += 1
    
    if validation:
        print("✓ Validation demonstration completed")
        completed += 1
    
    print(f"\nCompleted demonstrations: {completed}/{total}")
    
    if completed == total:
        print("\n🎉 All demonstrations completed successfully!")
        print("\nThe enhanced NLM Python bindings provide:")
        print("  • Improved error handling and validation")
        print("  • Type-safe getter methods")
        print("  • Advanced experiment management")
        print("  • Performance profiling tools")
        print("  • Comprehensive visualization utilities")
        print("  • Installation validation")
        print("  • Reproducible research capabilities")
    else:
        print(f"\n⚠ {total - completed} demonstrations failed.")
        print("Please check the output above for details.")
    
    print("\nFor more information, see:")
    print("  • README.md - Installation and usage")
    print("  • NLM_PYTHON_INSTALL.md - Comprehensive installation guide")
    print("  • examples/advanced_examples.py - Advanced example scripts")

if __name__ == "__main__":
    main()
