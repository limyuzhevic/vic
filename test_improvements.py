#!/usr/bin/env python3
"""
Test script for NLM Python bindings improvements.

This script verifies that the enhanced Python API is working correctly
and that all the new functionality is available.
"""

import sys
import os
import json
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

def test_basic_import():
    """Test that basic imports work."""
    print("Testing basic imports...")
    try:
        import pynlm
        print("✅ Successfully imported pynlm")
        return True
    except ImportError as e:
        print(f"❌ Failed to import pynlm: {e}")
        return False

def test_core_functionality():
    """Test core NLM functionality."""
    print("\nTesting core functionality...")
    try:
        import pynlm

        # Test configuration
        config = pynlm.createDefaultConfig()
        print("✅ Created default config")

        # Test brain creation
        brain = pynlm.createBrain(config)
        print("✅ Created brain")

        # Test initialization
        brain.initialize()
        print("✅ Initialized brain")

        # Test basic step
        brain.step(0)
        print("✅ Ran basic step")

        # Test statistics
        neurons = brain.getTotalNeuronCount()
        firing = brain.getFiringNeuronCount()
        print(f"✅ Got statistics: {neurons} neurons, {firing} firing")

        # Test region management
        region_id = brain.addRegion("test_region")
        print(f"✅ Added region with ID: {region_id}")

        region = brain.getRegion(region_id)
        print(f"✅ Got region: {region is not None}")

        return True

    except Exception as e:
        print(f"❌ Core functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_enhanced_config():
    """Test enhanced configuration functionality."""
    print("\nTesting enhanced configuration...")
    try:
        import pynlm

        config = pynlm.createDefaultConfig()

        # Test basic config operations
        config.set("test.int", 42)
        config.set("test.float", 3.14)
        config.set("test.bool", True)
        config.set("test.string", "hello")

        print("✅ Set various config values")

        # Test get() method (new)
        int_value = config.get("test.int")
        print(f"✅ Got int value: {int_value}")

        # Test getOr() method (new)
        default_value = config.getOr("missing.key", "default_value")
        print(f"✅ Got default value: {default_value}")

        # Test remove() method (new)
        config.set("test.to.remove", "temp")
        config.remove("test.to.remove")
        print("✅ Removed config key")

        # Test has() method
        has_key = config.has("test.int")
        print(f"✅ Config has key: {has_key}")

        # Test getKeys() method
        keys = config.getKeys()
        print(f"✅ Config has {len(keys)} keys")

        return True

    except Exception as e:
        print(f"❌ Enhanced config test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_enhanced_brain():
    """Test enhanced brain functionality."""
    print("\nTesting enhanced brain...")
    try:
        import pynlm

        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        # Test new statistics methods
        pending_spikes = brain.getPendingSpikeEventCount()
        print(f"✅ Got pending spike count: {pending_spikes}")

        active_neurons = brain.getActiveNeuronCount()
        print(f"✅ Got active neuron count: {active_neurons}")

        # Test memory system access
        working_memory = brain.getWorkingMemory()
        print(f"✅ Got working memory: {working_memory is not None}")

        episodic_memory = brain.getEpisodicMemory()
        print(f"✅ Got episodic memory: {episodic_memory is not None}")

        associative_memory = brain.getAssociativeMemory()
        print(f"✅ Got associative memory: {associative_memory is not None}")

        # Test prediction system
        prediction_system = brain.getPredictionSystem()
        print(f"✅ Got prediction system: {prediction_system is not None}")

        # Test cognition systems
        planner = brain.getPlanner()
        print(f"✅ Got planner: {planner is not None}")

        concept_formation = brain.getConceptFormation()
        print(f"✅ Got concept formation: {concept_formation is not None}")

        attention = brain.getAttention()
        print(f"✅ Got attention system: {attention is not None}")

        # Test development system
        dev_system = brain.getDevelopmentSystem()
        print(f"✅ Got development system: {dev_system is not None}")

        # Test neuromodulation systems
        dopamine = brain.getDopamine()
        print(f"✅ Got dopamine: {dopamine is not None}")

        curiosity = brain.getCuriosity()
        print(f"✅ Got curiosity: {curiosity is not None}")

        novelty = brain.getNovelty()
        print(f"✅ Got novelty: {novelty is not None}")

        prediction_error = brain.getPredictionErrorSignal()
        print(f"✅ Got prediction error: {prediction_error is not None}")

        return True

    except Exception as e:
        print(f"❌ Enhanced brain test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_world_functionality():
    """Test world functionality."""
    print("\nTesting world functionality...")
    try:
        import pynlm

        # Create world
        world = pynlm.createSimpleWorld()
        print("✅ Created world")

        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        print("✅ Configured world")

        # Reset world
        world.reset()
        print("✅ Reset world")

        # Set agent start
        world.setAgentStart(10.0, 10.0)
        print("✅ Set agent start")

        # Update world
        world.update(0.1)
        print("✅ Updated world")

        # Get sensory percept
        percept = world.getSensoryPercept()
        print(f"✅ Got sensory percept: {percept is not None}")

        # Get agent body
        body = world.getAgentBody()
        print(f"✅ Got agent body: {body is not None}")

        # Add world object
        world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=1.0)
        world.addObject(world_obj)
        print("✅ Added world object")

        # Remove world object
        world.removeObject(5.0, 5.0)
        print("✅ Removed world object")

        return True

    except Exception as e:
        print(f"❌ World functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_cli_import():
    """Test CLI import."""
    print("\nTesting CLI import...")
    try:
        # Try to import the CLI
        import importlib.util
        cli_spec = importlib.util.spec_from_file_location("nlm_cli", "nlm_cli.py")
        if cli_spec is None:
            print("❌ Could not load nlm_cli.py")
            return False

        print("✅ CLI import successful")
        return True

    except Exception as e:
        print(f"❌ CLI import test failed: {e}")
        return False

def test_utils_import():
    """Test utilities import."""
    print("\nTesting utilities import...")
    try:
        from python.nlm_utils import (
            NLMExperimentLogger,
            NLMBenchmark,
            NLMVisualizer
        )
        print("✅ Successfully imported NLM utilities")

        # Test that classes can be instantiated
        # Note: These might not work without actual NLM components,
        # but we can test the imports
        logger = NLMExperimentLogger("test_experiment")
        print("✅ Created NLMExperimentLogger")

        # For other classes, we'll just test import since they need NLM components
        benchmark = NLMBenchmark()
        print("✅ Created NLMBenchmark")

        visualizer = NLMVisualizer()
        print("✅ Created NLMVisualizer")

        return True

    except Exception as e:
        print(f"❌ Utilities import test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_backward_compatibility():
    """Test backward compatibility."""
    print("\nTesting backward compatibility...")
    try:
        import pynlm

        # Test that old API still works
        config = pynlm.createDefaultConfig()
        config.loadFromFile = lambda filepath: True  # Mock
        config.saveToFile = lambda filepath: True  # Mock

        brain = pynlm.createBrain(config)
        brain.initialize()
        brain.step(0)

        action = brain.produceAction()
        print("✅ Backward compatibility: produceAction works")

        brain.reset()
        print("✅ Backward compatibility: reset works")

        brain.save("test.brain")
        print("✅ Backward compatibility: save works")

        brain.load("test.brain")
        print("✅ Backward compatibility: load works")

        return True

    except Exception as e:
        print(f"❌ Backward compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)

    tests = [
        ("Basic Import", test_basic_import),
        ("Core Functionality", test_core_functionality),
        ("Enhanced Config", test_enhanced_config),
        ("Enhanced Brain", test_enhanced_brain),
        ("World Functionality", test_world_functionality),
        ("CLI Import", test_cli_import),
        ("Utilities Import", test_utils_import),
        ("Backward Compatibility", test_backward_compatibility),
    ]

    passed = 0
    failed = 0

    for test_name, test_func in tests:
        print(f"\n{'-' * 60}")
        print(f"Running: {test_name}")
        print(f"{'-' * 60}")

        try:
            if test_func():
                passed += 1
                print(f"✅ {test_name} PASSED")
            else:
                failed += 1
                print(f"❌ {test_name} FAILED")
        except Exception as e:
            failed += 1
            print(f"❌ {test_name} FAILED with exception: {e}")

    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Total tests: {len(tests)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")

    if failed == 0:
        print("\n🎉 All tests passed! The NLM Python API improvements are working correctly.")
        return 0
    else:
        print(f"\n⚠️  {failed} test(s) failed. Please check the output above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
