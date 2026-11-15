#!/usr/bin/env python3
"""
NLM Error Handler and Validator

This script provides comprehensive error handling and validation for NLM operations.
"""

import sys
import os
from pathlib import Path

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install NLM Python bindings.")
    sys.exit(1)


def validate_config(config):
    """Validate configuration parameters."""
    errors = []
    warnings = []
    
    # Check basic configuration
    if config.has("brain.neuron_count"):
        neuron_count = config.get("brain.neuron_count")
        if not isinstance(neuron_count, (int, float)):
            errors.append(f"Invalid neuron count type: {type(neuron_count)}")
        elif neuron_count <= 0:
            errors.append(f"Neuron count must be positive, got: {neuron_count}")
        elif neuron_count > 100000:
            warnings.append(f"Large neuron count ({neuron_count}) may impact performance")
    
    # Check plasticity settings
    if config.has("plasticity.stdp.enable"):
        stdp_enable = config.get("plasticity.stdp.enable")
        if not isinstance(stdp_enable, bool):
            errors.append(f"STDP enable must be boolean, got: {type(stdp_enable)}")
    
    # Check connection probability
    if config.has("brain.connection_probability"):
        conn_prob = config.get("brain.connection_probability")
        if not isinstance(conn_prob, (int, float)):
            errors.append(f"Connection probability must be numeric, got: {type(conn_prob)}")
        elif conn_prob < 0 or conn_prob > 1:
            errors.append(f"Connection probability must be between 0 and 1, got: {conn_prob}")
    
    # Check time constants
    if config.has("plasticity.stdp.tau_plus"):
        tau_plus = config.get("plasticity.stdp.tau_plus")
        if not isinstance(tau_plus, (int, float)) or tau_plus <= 0:
            errors.append(f"STDP tau_plus must be positive number, got: {tau_plus}")
    
    return errors, warnings


def safe_initialize_brain(config):
    """Safely initialize a brain with error handling."""
    print("Initializing brain...")
    
    try:
        brain = pynlm.createBrain(config)
        
        # Validate configuration before initialization
        errors, warnings = validate_config(config)
        
        if warnings:
            print("Warnings:")
            for warning in warnings:
                print(f"  - {warning}")
        
        if errors:
            print("Configuration errors:")
            for error in errors:
                print(f"  - {error}")
            return None
        
        # Initialize brain
        success = brain.initialize()
        
        if success:
            print("✓ Brain initialized successfully")
            print(f"  - Neurons: {brain.getTotalNeuronCount()}")
            print(f"  - Regions: {brain.getRegionCount()}")
            print(f"  - Synapses: {brain.getTotalSynapseCount()}")
            return brain
        else:
            print("✗ Brain initialization failed")
            return None
            
    except Exception as e:
        print(f"✗ Brain initialization error: {e}")
        import traceback
        traceback.print_exc()
        return None


def safe_simulation(brain, steps, validate_after=False):
    """Safely run brain simulation with error handling."""
    print(f"Running simulation for {steps} steps...")
    
    if not brain:
        print("Error: Brain not initialized")
        return False
    
    try:
        for step in range(steps):
            try:
                brain.step(step)
                
                # Log progress every 10% of simulation
                if (step + 1) % (steps // 10) == 0 or step == 0:
                    progress = (step + 1) / steps * 100
                    print(f"  Progress: {progress:.1f}% - "
                          f"Firing: {brain.getFiringNeuronCount()}, "
                          f"Spikes: {brain.getTotalSpikeCount()}")
                
            except Exception as e:
                print(f"Error at step {step}: {e}")
                return False
        
        if validate_after:
            # Validate brain state after simulation
            validate_brain_state(brain)
        
        print(f"✓ Simulation completed successfully")
        print(f"  - Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Simulation error: {e}")
        import traceback
        traceback.print_exc()
        return False


def validate_brain_state(brain):
    """Validate brain state after operations."""
    print("\n=== Brain State Validation ===")
    
    errors = []
    warnings = []
    
    # Check neuron count consistency
    if brain.getTotalNeuronCount() <= 0:
        errors.append("Invalid neuron count")
    elif brain.getTotalNeuronCount() > 1000000:
        warnings.append("Very large neuron count may impact performance")
    
    # Check synapse count
    synapse_count = brain.getTotalSynapseCount()
    if synapse_count < 0:
        errors.append("Invalid synapse count")
    
    # Check firing rate
    firing_rate = brain.getAverageFiringRate()
    if firing_rate < 0:
        errors.append("Invalid firing rate")
    elif firing_rate > 1000:
        warnings.append("Very high firing rate may indicate neural hyperactivity")
    
    # Check E/I ratio
    ei_ratio = brain.getExcitationInhibitionRatio()
    if ei_ratio < 0 or ei_ratio > 100:
        warnings.append(f"Unusual E/I ratio: {ei_ratio}")
    
    # Check spike count
    spike_count = brain.getTotalSpikeCount()
    if spike_count < 0:
        errors.append("Invalid spike count")
    
    if errors:
        print("✗ Validation errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    else:
        print("✓ Brain state validation passed")
    
    if warnings:
        print("Warnings:")
        for warning in warnings:
            print(f"  - {warning}")
    
    return True


def safe_save_brain(brain, filepath, validate=True):
    """Safely save brain state with error handling."""
    print(f"Saving brain to {filepath}...")
    
    if not brain:
        print("Error: No brain to save")
        return False
    
    try:
        # Validate brain state before saving
        if validate:
            if not validate_brain_state(brain):
                print("Brain state validation failed, skipping save")
                return False
        
        # Save brain state
        brain.save(filepath)
        
        print(f"✓ Brain saved successfully to {filepath}")
        
        # Verify file exists and is readable
        if os.path.exists(filepath):
            file_size = os.path.getsize(filepath)
            print(f"  - File size: {file_size} bytes")
            return True
        else:
            print(f"✗ File not created: {filepath}")
            return False
            
    except Exception as e:
        print(f"✗ Save error: {e}")
        import traceback
        traceback.print_exc()
        return False


def safe_load_brain(config, filepath, validate_after=True):
    """Safely load brain state with error handling."""
    print(f"Loading brain from {filepath}...")
    
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Load brain state
        brain.load(filepath)
        
        print(f"✓ Brain loaded successfully from {filepath}")
        
        # Validate brain state after loading
        if validate_after:
            if not validate_brain_state(brain):
                print("✗ Brain state validation failed after loading")
                return None
        
        print(f"  - Neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Synapses: {brain.getTotalSynapseCount()}")
        print(f"  - Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        return brain
        
    except Exception as e:
        print(f"✗ Load error: {e}")
        import traceback
        traceback.print_exc()
        return None


def validate_all(args):
    """Comprehensive validation of NLM components."""
    print("=== NLM System Validation ===")
    
    config = pynlm.createDefaultConfig()
    
    # Validate configuration
    print("\n1. Validating configuration...")
    errors, warnings = validate_config(config)
    
    if warnings:
        print("Warnings:")
        for warning in warnings:
            print(f"  - {warning}")
    
    if errors:
        print("Configuration validation failed!")
        return False
    
    print("✓ Configuration validation passed")
    
    # Test brain creation and initialization
    print("\n2. Testing brain creation and initialization...")
    brain = safe_initialize_brain(config)
    
    if not brain:
        print("Brain creation/initialization failed!")
        return False
    
    # Test brain simulation
    print("\n3. Testing brain simulation...")
    if not safe_simulation(brain, 10):
        print("Brain simulation test failed!")
        return False
    
    # Test brain save/load
    print("\n4. Testing brain save/load...")
    import tempfile
    import shutil
    
    # Create temporary directory
    temp_dir = tempfile.mkdtemp()
    test_file = os.path.join(temp_dir, "test_brain.bin")
    
    try:
        # Save brain
        if not safe_save_brain(brain, test_file, validate=True):
            print("Brain save test failed!")
            return False
        
        # Load brain (create new config)
        config2 = pynlm.createDefaultConfig()
        loaded_brain = safe_load_brain(config2, test_file, validate_after=True)
        
        if not loaded_brain:
            print("Brain load test failed!")
            return False
        
        print("✓ Brain save/load test passed")
        
    finally:
        # Clean up temporary directory
        shutil.rmtree(temp_dir, ignore_errors=True)
    
    print("\n=== Validation Summary ===")
    print("✓ All NLM system validation tests passed!")
    print("  - Configuration validation: PASS")
    print("  - Brain creation: PASS")
    print("  - Brain initialization: PASS")
    print("  - Brain simulation: PASS")
    print("  - Brain save/load: PASS")
    
    return True


def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Error Handler and Validator",
        epilog="Comprehensive error handling and validation for NLM operations."
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # validate command
    parser_validate = subparsers.add_parser('validate', 
                                          help='Comprehensive validation of NLM system')
    parser_validate.set_defaults(func=validate_all)
    
    # safe-brain command
    parser_safe = subparsers.add_parser('safe-brain', 
                                      help='Create brain with safe initialization')
    parser_safe.set_defaults(func=lambda args: None)  # Will handle in main
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        sys.exit(1)
    
    try:
        if args.command == 'validate':
            args.func(args)
        else:
            print("Error: Unknown command")
            sys.exit(1)
    
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == '__main__':
    main()