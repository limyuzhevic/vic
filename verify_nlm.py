#!/usr/bin/env python3
"""
Quick verification script for NLM Python package

This script performs basic verification that the NLM Python package
is working correctly after all the improvements.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_imports():
    """Test that all imports work correctly"""
    print("Testing imports...")
    
    try:
        import nlm
        print("✓ nlm package imported successfully")
        print(f"  Version: {nlm.__version__}")
    except Exception as e:
        print(f"✗ Failed to import nlm: {e}")
        return False
    
    # Test core classes
    core_classes = [
        'Brain', 'Config', 'AgentBrain', 'SimpleWorld',
        'NeuronType', 'ActionType', 'WorldObjectType',
        'DevelopmentalStage', 'FiringState'
    ]
    
    for class_name in core_classes:
        if hasattr(nlm, class_name):
            print(f"✓ {class_name} available")
        else:
            print(f"✗ {class_name} not found")
            return False
    
    return True

def test_basic_functionality():
    """Test basic functionality"""
    print("\nTesting basic functionality...")
    
    try:
        import nlm
    except ImportError:
        print("✗ Could not import nlm")
        return False
    
    # Test configuration
    try:
        config = nlm.Config()
        config.set("random_seed", 42, None)
        config.set("simulation_timestep", 0.001, None)
        config.set("neuron_count", 100, None)
        
        errors = config.validate()
        if errors:
            print(f"✗ Configuration validation failed: {errors}")
            return False
        
        print("✓ Configuration system works")
    except Exception as e:
        print(f"✗ Configuration system failed: {e}")
        return False
    
    # Test brain creation and initialization
    try:
        brain = nlm.Brain()
        brain.initialize()
        print("✓ Brain creation and initialization works")
    except Exception as e:
        print(f"✗ Brain creation failed: {e}")
        return False
    
    # Test simulation
    try:
        for step in range(10):
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0 + step % 50)
            brain.step(step, step * 0.001)
        
        spike_count = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"✓ Simulation completed: {spike_count} spikes, {firing_rate:.2f} Hz firing rate")
    except Exception as e:
        print(f"✗ Simulation failed: {e}")
        return False
    
    return True

def test_high_level_api():
    """Test high-level API"""
    print("\nTesting high-level API...")
    
    try:
        import nlm
    except ImportError:
        print("✗ Could not import nlm")
        return False
    
    # Test quick start
    try:
        simulation = nlm.NLMSimulation().quick_start(
            neurons=50,
            width=200,
            height=200,
            timestep=0.001,
            seed=42
        )
        
        print("✓ High-level simulation creation works")
        
        # Run simulation
        results = simulation.run_simulation(20, steps_per_episode=10, verbose=False)
        
        if results["total_steps"] == 20:
            print("✓ High-level simulation execution works")
        else:
            print(f"✗ Simulation step count incorrect: {results['total_steps']}")
            return False
            
    except Exception as e:
        print(f"✗ High-level API failed: {e}")
        return False
    
    return True

def test_factory_functions():
    """Test factory functions"""
    print("\nTesting factory functions...")
    
    try:
        import nlm
    except ImportError:
        print("✗ Could not import nlm")
        return False
    
    try:
        # Test factory functions
        config = nlm.create_default_config()
        print("✓ create_default_config() works")
        
        brain = nlm.create_brain()
        print("✓ create_brain() works")
        
        world = nlm.create_world(100, 100, 10, 10)
        if world.getWidth() == 100 and world.getHeight() == 100:
            print("✓ create_world() works")
        else:
            print("✗ create_world() failed")
            return False
        
        agent_brain = nlm.create_agent_brain(brain)
        print("✓ create_agent_brain() works")
        
    except Exception as e:
        print(f"✗ Factory functions failed: {e}")
        return False
    
    return True

def test_command_line():
    """Test command-line interface"""
    print("\nTesting command-line interface...")
    
    try:
        import nlm
    except ImportError:
        print("✗ Could not import nlm")
        return False
    
    # Test CLI creation
    try:
        cli = nlm.NLMCLI()
        print("✓ Command-line interface creation works")
    except Exception as e:
        print(f"✗ CLI creation failed: {e}")
        return False
    
    # Note: Full CLI testing requires subprocess calls
    print("✓ Command-line interface structure OK (full testing requires subprocess)")
    
    return True

def test_documentation():
    """Test documentation availability"""
    print("\nTesting documentation...")
    
    # Check if documentation files exist
    doc_files = [
        'NLM.md',
        'INSTALLATION.md',
        'README.md',
        'README_PY.md'
    ]
    
    for doc_file in doc_files:
        if os.path.exists(doc_file):
            print(f"✓ {doc_file} exists")
        else:
            print(f"✗ {doc_file} not found")
            return False
    
    return True

def main():
    """Run all verification tests"""
    print("="*80)
    print("NLM Python Package - Verification Script")
    print("="*80)
    print()
    print("This script verifies that the NLM Python package is working")
    print("correctly after all improvements.")
    print()
    
    # Run all tests
    tests = [
        ("Basic Imports", test_imports),
        ("Basic Functionality", test_basic_functionality),
        ("High-Level API", test_high_level_api),
        ("Factory Functions", test_factory_functions),
        ("Command-Line Interface", test_command_line),
        ("Documentation", test_documentation),
    ]
    
    passed = 0
    failed = 0
    
    for name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {name}")
        print(f"{'='*60}")
        
        try:
            if test_func():
                passed += 1
                print(f"\n✅ {name}: PASSED")
            else:
                failed += 1
                print(f"\n❌ {name}: FAILED")
        except Exception as e:
            print(f"\n❌ {name}: ERROR - {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print(f"\n{'='*80}")
    print(f"VERIFICATION RESULTS: {passed} PASSED, {failed} FAILED")
    print(f"{'='*80}")
    
    if failed == 0:
        print("\n🎉 All verification tests passed!")
        print("\nThe NLM Python package is working correctly with:")
        print("  ✓ Complete package structure and imports")
        print("  ✓ Core neural simulation functionality")
        print("  ✓ High-level Pythonic API")
        print("  ✓ Factory functions for quick creation")
        print("  ✓ Command-line interface")
        print("  ✓ Comprehensive documentation")
        print("\nThe package is ready for use!")
        print("\nNext steps:")
        print("  1. Run: python demo_nlm.py    # See full feature demonstrations")
        print("  2. Run: python -m nlm demo     # Run command-line demo")
        print("  3. Run: pip install -e .       # Install for permanent use")
        print("  4. Run: pytest               # Run comprehensive tests")
        return 0
    else:
        print(f"\n⚠️  {failed} verification test(s) failed.")
        print("Please check the error messages above.")
        print("\nCommon issues:")
        print("  - Python path not set correctly")
        print("  - Missing dependencies")
        print("  - Import errors in package structure")
        print("  - File permission issues")
        return 1

if __name__ == "__main__":
    sys.exit(main())
