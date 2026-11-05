#!/usr/bin/env python3
"""
Quick test to verify NLM Python bindings work correctly
"""

import sys
import os

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_imports():
    """Test that all main imports work"""
    print("Testing NLM Python bindings imports...")
    
    try:
        # Test main imports
        from pynlm import (
            Brain, Config, AgentBrain, SimpleWorld,
            Vision, Audio, InternalSignals, SensoryPercept,
            Action, ActionResult, ActionType, Reward,
            NeuronId, SynapseId, RegionId, PopulationId,
            NeuronType, SynapseType, DevelopmentalStage,
            SimulationStep, Timestamp, create_default_config,
            create_brain, create_simple_world, create_agent_brain,
            SimulationLoop, ActionSequence
        )
        
        print("✓ All imports successful")
        return True
        
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False

def test_basic_functionality():
    """Test basic functionality"""
    print("\nTesting basic functionality...")
    
    try:
        from pynlm import create_default_config, create_brain, create_simple_world
        
        # Create components
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        
        # Test basic operations
        print(f"✓ Config created: {type(config).__name__}")
        print(f"✓ Brain created: {type(brain).__name__}")
        print(f"✓ World created: {type(world).__name__}")
        
        # Test type definitions
        from pynlm import NeuronId, SynapseId, RegionId
        
        neuron_id = NeuronId(42)
        synapse_id = SynapseId(100)
        region_id = RegionId(200)
        
        print(f"✓ Type definitions work: NeuronId={neuron_id}, SynapseId={synapse_id}, RegionId={region_id}")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_examples():
    """Test example scripts"""
    print("\nTesting example scripts...")
    
    examples = [
        ("basic_brain", "Basic Brain Simulation Example"),
        ("agent_world", "Agent Navigation Example"),
        ("checkpointing", "Checkpointing Example"),
        ("analysis", "Analysis Example"),
    ]
    
    success_count = 0
    
    for example_name, description in examples:
        print(f"\n  Testing {description}...")
        
        try:
            # Add examples to path
            examples_path = os.path.join(os.path.dirname(__file__), 'examples')
            sys.path.insert(0, examples_path)
            
            # Import the example module
            if example_name == "basic_brain":
                from examples.basic_brain import main as demo_main
                # We won't run the full demo, just test imports
                print(f"    ✓ {description} imports successful")
                success_count += 1
            
            elif example_name == "agent_world":
                from examples.agent_world import main as demo_main
                print(f"    ✓ {description} imports successful")
                success_count += 1
                
            elif example_name == "checkpointing":
                from examples.checkpointing import demonstrate_checkpointing
                print(f"    ✓ {description} imports successful")
                success_count += 1
                
            elif example_name == "analysis":
                from examples.analysis import demonstrate_basic_analysis
                print(f"    ✓ {description} imports successful")
                success_count += 1
                
        except Exception as e:
            print(f"    ✗ {description} failed: {e}")
    
    print(f"\n✓ {success_count}/{len(examples)} example tests passed")
    return success_count == len(examples)

def test_package_structure():
    """Test package structure"""
    print("\nTesting package structure...")
    
    required_files = [
        "pynlm/__init__.py",
        "pynlm/core.py",
        "pynlm/types.py",
        "pynlm/sensory.py",
        "pynlm/motor.py",
        "pynlm/agent.py",
        "pynlm/utils.py",
        "pynlm/exceptions.py"
    ]
    
    success_count = 0
    
    for file_path in required_files:
        full_path = os.path.join(os.path.dirname(__file__), file_path)
        if os.path.exists(full_path):
            print(f"  ✓ {file_path}")
            success_count += 1
        else:
            print(f"  ✗ {file_path} - MISSING")
    
    print(f"\n✓ Package structure: {success_count}/{len(required_files)} files present")
    return success_count == len(required_files)

def main():
    """Main test function"""
    print("=" * 70)
    print("NLM Python Bindings - Quick Test Suite")
    print("=" * 70)
    
    tests = [
        ("Imports", test_imports),
        ("Basic Functionality", test_basic_functionality),
        ("Package Structure", test_package_structure),
        ("Example Scripts", test_examples),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*70}")
        print(f"Running: {test_name} Test")
        print(f"{'='*70}")
        
        try:
            success = test_func()
            results.append((test_name, success))
        except Exception as e:
            print(f"Test failed with exception: {e}")
            import traceback
            traceback.print_exc()
            results.append((test_name, False))
    
    # Summary
    print(f"\n{'='*70}")
    print("TEST SUMMARY")
    print(f"{'='*70}")
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{status:8} {test_name}")
        if success:
            passed += 1
    
    print(f"{'='*70}")
    print(f"Overall: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 ALL TESTS PASSED!")
        return 0
    else:
        print("❌ SOME TESTS FAILED")
        return 1

if __name__ == "__main__":
    sys.exit(main())
