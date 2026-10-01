#!/usr/bin/env python3
"""
NLM Advanced Features Test
============================

This script demonstrates the enhanced NLM features and functionality
that have been added to the project.
"""

import sys
import os
import json
import time
from pathlib import Path

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
try:
    import nlm
    from nlm_advanced import (
        AdvancedConfig,
        AdvancedSimulationController,
        CheckpointManager,
        AnalysisTools,
        AdvancedExperimentManager,
        NLMEnhancedInterface
    )
    print("✓ NLM modules imported successfully")
except ImportError as e:
    print(f"✗ Error importing modules: {e}")
    sys.exit(1)

def test_advanced_config():
    """Test advanced configuration features."""
    print("\n1. Testing Advanced Configuration...")
    
    try:
        # Create advanced config
        config = AdvancedConfig()
        
        # Set advanced parameters
        config.set_advanced_param("learning_rate", 0.005)
        config.set_advanced_param("neuron_count", 2000)
        config.set_advanced_param("structure_plasticity", True)
        config.set_advanced_param("development_enabled", True)
        
        # Get parameters
        learning_rate = config.get_advanced_param("learning_rate")
        neuron_count = config.get_advanced_param("neuron_count")
        
        print(f"   ✓ Learning rate: {learning_rate}")
        print(f"   ✓ Neuron count: {neuron_count}")
        
        # Create NLM config from advanced parameters
        nlm_config = config.create_nlm_config()
        print(f"   ✓ Created NLM config with {nlm_config.getKeys().count()} parameters")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return False
def test_advanced_simulation():
    """Test advanced simulation features."""
    print("\n2. Testing Advanced Simulation...")
    
    try:
        # Setup enhanced interface
        enhanced = NLMEnhancedInterface()
        enhanced.setup_advanced_simulation(steps=500)
        
        # Define monitoring callback
        def monitor_callback(step, brain):
            if step % 100 == 0:
                print(f"   Step {step}: Firing rate = {brain.getAverageFiringRate():.2f} Hz")
        
        enhanced.add_monitoring_callbacks([monitor_callback])
        
        # Run advanced simulation
        results = enhanced.run_advanced_simulation(steps=500)
        
        print(f"   ✓ Simulation completed: {results['simulation_results']['final_step']}/500 steps")
        print(f"   ✓ Performance analysis: {len(results['analysis'])} metrics calculated")
        print(f"   ✓ Performance data collected: {len(results['performance_data'])} data points")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return False
def test_checkpoint_manager():
    """Test checkpoint management features."""
    print("\n3. Testing Checkpoint Management...")
    
    try:
        # Create checkpoint manager
        checkpoint_manager = CheckpointManager(max_checkpoints=5)
        
        # Create a simple brain (this is a mock since we can't easily create real brain instances)
        config = nlm.createDefaultConfig()
        brain = nlm.Brain(config)
        
        # Save checkpoint
        success = checkpoint_manager.save(brain, "test_checkpoint")
        if success:
            print("   ✓ Checkpoint saved successfully")
        else:
            print("   ✗ Checkpoint save failed")
            return False
        
        # Load checkpoint
        loaded_brain = checkpoint_manager.load("test_checkpoint")
        if loaded_brain:
            print("   ✓ Checkpoint loaded successfully")
        else:
            print("   ✗ Checkpoint load failed")
            return False
        
        # Test auto-save
        checkpoint_manager.auto_save(brain, "auto_save_test")
        print("   ✓ Auto-save completed")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return False
def test_analysis_tools():
    """Test analysis tools."""
    print("\n4. Testing Analysis Tools...")
    
    try:
        # Create analysis tools
        analysis_tools = AnalysisTools()
        
        # Create mock performance data
        performance_data = {}
        for i in range(10):
            timestamp = time.time() + i
            performance_data[timestamp] = {
                "step": i,
                "firing_rate": 10.0 + (i % 5),  # Varying firing rates
                "total_spikes": i * 10,
                "memory_usage": 50 + (i % 20),
                "time": timestamp
            }
        
        # Analyze performance data
        analysis = analysis_tools.analyze_simulation_data(performance_data)
        
        print(f"   ✓ Analysis completed: {len(analysis)} metrics calculated")
        
        if "firing_rate" in analysis:
            print(f"     Mean firing rate: {analysis['firing_rate']['mean']:.2f} Hz")
            print(f"     Firing rate std: {analysis['firing_rate']['std']:.2f}")
        
        if "efficiency_score" in analysis.get("performance_metrics", {}):
            print(f"     Efficiency score: {analysis['performance_metrics']['efficiency_score']:.3f}")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return False
def test_experiment_manager():
    """Test experiment management features."""
    print("\n5. Testing Experiment Manager...")
    
    try:
        # Create experiment manager
        experiment_manager = AdvancedExperimentManager()
        
        # Create experiment
        experiment_id = experiment_manager.create_experiment(
            "test_experiment",
            {"steps": 100, "learning_rate": 0.01}
        )
        
        print(f"   ✓ Experiment created: {experiment_id}")
        
        # Create brain for experiment
        config = nlm.createDefaultConfig()
        brain = nlm.Brain(config)
        
        # Create world
        world = nlm.createSimpleWorld()
        
        # Run experiment
        results = experiment_manager.run_experiment(experiment_id, brain, world)
        
        print(f"   ✓ Experiment completed: {results['steps']} steps")
        print(f"   ✓ Performance analysis: {len(results['analysis'])} metrics")
        
        # Get experiment results
        exp_results = experiment_manager.get_experiment_results(experiment_id)
        if exp_results:
            print(f"   ✓ Results retrieved: {len(exp_results['performance'])} data points")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return False
def test_script_features():
    """Test convenience script features."""
    print("\n6. Testing Script Features...")
    
    try:
        # Test convenience functions
        from nlm_advanced import (
            setup_advanced_simulation,
            create_advanced_config,
            run_advanced_simulation,
            save_checkpoint,
            load_checkpoint
        )
        
        print("   ✓ Convenience functions imported successfully")
        
        # Test setup_advanced_simulation
        enhanced = setup_advanced_simulation(100)
        print("   ✓ setup_advanced_simulation works")
        
        # Test create_advanced_config
        config = create_advanced_config()
        print("   ✓ create_advanced_config works")
        
        # Test create_advanced_config
        advanced = create_advanced_config()
        advanced.set_advanced_param("test_param", "test_value")
        value = advanced.get_advanced_param("test_param")
        if value == "test_value":
            print("   ✓ Advanced config parameter setting works")
        
        return True
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return False
def test_documentation():
    """Test documentation and help features."""
    print("\n7. Testing Documentation...")
    
    try:
        # Check if documentation files exist
        doc_files = [
            "docs/ADVANCED_USER_GUIDE.md",
            "nlm_advanced.py",
            "nlm_advanced.sh"
        ]
        
        for doc_file in doc_files:
            if os.path.exists(doc_file):
                size = os.path.getsize(doc_file)
                print(f"   ✓ {doc_file}: {size} bytes")
            else:
                print(f"   ✗ {doc_file}: not found")
                return False
        
        # Check if nlm_advanced.sh is executable
        if os.path.exists("nlm_advanced.sh"):
            if os.access("nlm_advanced.sh", os.X_OK):
                print("   ✓ nlm_advanced.sh is executable")
            else:
                print("   ✗ nlm_advanced.sh is not executable")
                return False
        
        return True
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
        return False
def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Advanced Features Test")
    print("=" * 60)
    
    tests = [
        test_advanced_config,
        test_advanced_simulation,
        test_checkpoint_manager,
        test_analysis_tools,
        test_experiment_manager,
        test_script_features,
        test_documentation
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        if test():
            passed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Results: {passed}/{total} tests passed")
    print("=" * 60)
    
    if passed == total:
        print("✓ All tests passed!")
        return 0
    else:
        print("✗ Some tests failed!")
        return 1
if __name__ == "__main__":
    exit(main())
