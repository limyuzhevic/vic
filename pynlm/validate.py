#!/usr/bin/env python3
"""
Simple NLM Validation Test

This script validates the basic functionality of the NLM Python interface
improvements.
"""

import os
import tempfile
import json
import logging
from pathlib import Path
from pynlm import (
    AdvancedNLM, 
    ConfigManager, 
    NLMConfig, 
    get_minimal_config,
    get_default_config,
    get_research_config,
    create_config,
    setup_logging,
    EventType,
    SimulationStatus
)

# Setup logging
setup_logging(level=logging.WARNING)

def main():
    print("=" * 60)
    print("NLM Python Interface Validation Test")
    print("=" * 60)
    
    # Initialize temporary directory
    temp_dir = tempfile.mkdtemp()
    print(f"Test directory: {temp_dir}")
    
    try:
        # Test 1: Basic configuration presets
        print("\n1. Testing configuration presets...")
        
        minimal = get_minimal_config()
        default = get_default_config()
        research = get_research_config()
        
        assert minimal.neuron_count == 100, f"Expected 100 neurons, got {minimal.neuron_count}"
        assert default.neuron_count == 1000, f"Expected 1000 neurons, got {default.neuron_count}"
        assert research.neuron_count == 10000, f"Expected 10000 neurons, got {research.neuron_count}"
        
        print("   ✓ Configuration presets work correctly")
        
        # Test 2: Advanced NLM creation
        print("\n2. Testing AdvancedNLM creation...")
        
        brain = AdvancedNLM()
        assert brain is not None, "AdvancedNLM creation failed"
        assert brain.brain is not None, "Brain initialization failed"
        assert brain.enhanced_brain is not None, "Enhanced brain wrapper failed"
        
        print("   ✓ AdvancedNLM creation works correctly")
        
        # Test 3: Enhanced brain wrapper functionality
        print("\n3. Testing enhanced brain wrapper...")
        
        neuron_count = brain.enhanced_brain.get_neuron_count()
        firing_neuron_count = brain.enhanced_brain.get_firing_neuron_count()
        firing_rate = brain.enhanced_brain.get_average_firing_rate()
        e_i_ratio = brain.enhanced_brain.get_excitation_inhibition_ratio()
        
        assert neuron_count > 0, f"Expected positive neuron count, got {neuron_count}"
        assert firing_neuron_count >= 0, f"Expected non-negative firing neuron count, got {firing_neuron_count}"
        assert firing_rate >= 0, f"Expected non-negative firing rate, got {firing_rate}"
        assert e_i_ratio >= 0, f"Expected non-negative E/I ratio, got {e_i_ratio}"
        
        config_summary = brain.enhanced_brain.get_config_summary()
        assert isinstance(config_summary, dict), f"Expected dict config summary, got {type(config_summary)}"
        assert 'neuron_count' in config_summary, "Missing neuron_count in config summary"
        
        print("   ✓ Enhanced brain wrapper works correctly")
        
        # Test 4: Performance statistics
        print("\n4. Testing performance statistics...")
        
        performance_stats = brain.enhanced_brain.get_performance_stats()
        assert isinstance(performance_stats, dict), f"Expected dict performance stats, got {type(performance_stats)}"
        assert 'steps_per_second' in performance_stats, "Missing steps_per_second in performance stats"
        assert 'neurons_per_second' in performance_stats, "Missing neurons_per_second in performance stats"
        assert 'synapses_per_neuron' in performance_stats, "Missing synapses_per_neuron in performance stats"
        
        print("   ✓ Performance statistics work correctly")
        
        # Test 5: Simulation creation and running
        print("\n5. Testing simulation...")
        
        simulation = brain.create_simulation("validation_test")
        assert simulation is not None, "Simulation creation failed"
        
        # Track events
        event_count = 0
        def count_events(event):
            nonlocal event_count
            event_count += 1
        
        simulation.add_event_listener(EventType.STEP_COMPLETED, count_events)
        
        # Run simulation
        simulation.run(50)
        
        assert event_count > 0, f"Expected events, got {event_count}"
        
        print("   ✓ Simulation works correctly with event monitoring")
        
        # Test 6: Configuration management
        print("\n6. Testing configuration management...")
        
        config_manager = ConfigManager(get_default_config())
        
        # Update configuration
        config_manager.update_config(neuron_count=500)
        
        # Validate
        is_valid = config_manager.validate()
        assert is_valid, "Configuration validation failed"
        
        # Save and load
        config_file = os.path.join(temp_dir, "config.json")
        config_manager.save_config(config_file)
        
        new_config_manager = ConfigManager()
        new_config_manager.load_config(config_file)
        
        assert new_config_manager.get_config().neuron_count == 500, "Loaded config doesn't match saved config"
        
        print("   ✓ Configuration management works correctly")
        
        # Test 7: Learning systems configuration
        print("\n7. Testing learning systems...")
        
        # Enable learning systems
        brain.brain.enableRewardModulation(True)
        brain.brain.enableStructuralPlasticity(True)
        brain.brain.enableDevelopment(True)
        brain.brain.enableCuriosity(True)
        
        # Verify they are enabled
        assert brain.brain.isRewardModulationEnabled(), "Reward modulation should be enabled"
        assert brain.brain.isStructuralPlasticityEnabled(), "Structural plasticity should be enabled"
        assert brain.brain.isDevelopmentEnabled(), "Development should be enabled"
        assert brain.brain.isCuriosityEnabled(), "Curiosity should be enabled"
        
        # Run simulation with learning
        learning_simulation = brain.create_simulation("learning_test")
        learning_simulation.run(100)
        
        print("   ✓ Learning systems work correctly")
        
        # Test 8: Activity analysis
        print("\n8. Testing activity analysis...")
        
        # Get activity patterns
        patterns = brain.enhanced_brain.get_activity_patterns(50)
        assert isinstance(patterns, dict), f"Expected dict patterns, got {type(patterns)}"
        assert 'spike_count' in patterns, "Missing spike_count in patterns"
        assert 'firing_rate' in patterns, "Missing firing_rate in patterns"
        assert 'e_i_ratio' in patterns, "Missing e_i_ratio in patterns"
        assert 'step' in patterns, "Missing step in patterns"
        
        # Find active regions
        active_regions = brain.enhanced_brain.find_active_regions(threshold=0.1)
        assert isinstance(active_regions, list), f"Expected list active regions, got {type(active_regions)}"
        
        # Analyze stability
        stability = brain.enhanced_brain.analyze_stability(50)
        assert isinstance(stability, dict), f"Expected dict stability, got {type(stability)}"
        assert 'stability' in stability, "Missing stability in stability analysis"
        assert 'recommendations' in stability, "Missing recommendations in stability analysis"
        
        print("   ✓ Activity analysis works correctly")
        
        # Test 9: File I/O operations
        print("\n9. Testing file I/O...")
        
        # Save brain
        brain_file = os.path.join(temp_dir, "test_brain.bin")
        brain_save_success = brain.save_brain(brain_file)
        assert brain_save_success, "Brain save operation failed"
        
        # Verify file exists
        assert os.path.exists(brain_file), "Brain file not found after save"
        
        # Load brain
        new_brain = AdvancedNLM()
        brain_load_success = new_brain.load_brain(brain_file)
        assert brain_load_success, "Brain load operation failed"
        
        print("   ✓ File I/O operations work correctly")
        
        # Test 10: Error handling
        print("\n10. Testing error handling...")
        
        # Test invalid configuration
        try:
            invalid_config = NLMConfig(neuron_count=-100)
            invalid_config.validate()
            assert False, "Should have raised ValueError for invalid configuration"
        except ValueError:
            pass  # Expected
        
        print("   ✓ Error handling works correctly")
        
        print("\n" + "=" * 60)
        print("VALIDATION SUCCESSFUL!")
        print("=" * 60)
        print("All NLM Python interface improvements have been validated.")
        print("The enhanced interface is ready for production use.")
        print(f"\nGenerated files in {temp_dir}:")
        for filename in os.listdir(temp_dir):
            filepath = os.path.join(temp_dir, filename)
            if os.path.isfile(filepath):
                size = os.path.getsize(filepath) / 1024  # KB
                print(f"  - {filename} ({size:.1f} KB)")
        
        return True
        
    except Exception as e:
        print(f"\nVALIDATION FAILED!")
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    finally:
        # Cleanup
        import shutil
        shutil.rmtree(temp_dir, ignore_errors=True)

if __name__ == "__main__":
    # Run validation
    success = main()
    
    # Exit with appropriate code
    exit(0 if success else 1)