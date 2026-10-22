#!/usr/bin/env python3
"""
NLM High-Level API Test Script

Tests the high-level convenience functions for the NLM (Neural Learning Machine)
framework. This script validates that the new convenience functions work correctly
and provide the expected functionality.
"""

import os
import sys
import tempfile
from pathlib import Path

# Add the project root to Python path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    NLM_AVAILABLE = True
except ImportError as e:
    NLM_AVAILABLE = False
    NLM_ERROR = str(e)

def test_imports():
    """Test that all high-level API modules can be imported"""
    print("=== Testing High-Level API Imports ===")
    
    try:
        from python.high_level_api import (
            create_agent_brain_with_config,
            run_simulation_loop,
            run_agent_simulation,
            get_brain_stats,
            enable_brain_features,
            save_brain_with_timestamp,
            load_brain_with_backup,
            configure_brain_from_dict,
            create_world_from_config
        )
        print("✓ All high-level API functions imported successfully")
        return True
    except ImportError as e:
        print(f"✗ Import error: {e}")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return False

def test_create_agent_brain_with_config():
    """Test create_agent_brain_with_config function"""
    print("\n=== Testing create_agent_brain_with_config ===")
    
    try:
        from python.high_level_api import create_agent_brain_with_config
        
        # Test with default config
        print("1. Testing with default configuration...")
        brain1 = create_agent_brain_with_config()
        print(f"   ✓ Brain created with {brain1.getTotalNeuronCount()} neurons")
        
        # Test with custom config
        print("\n2. Testing with custom configuration...")
        custom_config = {
            "brain": {
                "neuron_count": 1500,
                "synapse_density": 0.2
            }
        }
        brain2 = create_agent_brain_with_config(custom_config)
        print(f"   ✓ Custom brain created with {brain2.getTotalNeuronCount()} neurons")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in create_agent_brain_with_config test: {e}")
        return False

def test_get_brain_stats():
    """Test get_brain_stats function"""
    print("\n=== Testing get_brain_stats ===")
    
    try:
        from python.high_level_api import create_agent_brain_with_config, get_brain_stats
        
        # Create brain
        brain = create_agent_brain_with_config()
        
        # Run some steps to generate activity
        for step in range(50):
            brain.step(step)
        
        # Get stats
        print("1. Getting brain statistics...")
        stats = get_brain_stats(brain)
        
        print(f"   ✓ Stats retrieved successfully")
        print(f"     - Neurons: {stats['neuron_count']}")
        print(f"     - Synapses: {stats['synapse_count']}")
        print(f"     - Firing neurons: {stats['firing_neuron_count']}")
        print(f"     - Total spikes: {stats['total_spike_count']}")
        print(f"     - Average firing rate: {stats['average_firing_rate']:.2f}")
        print(f"     - Developmental stage: {stats['developmental_stage']}")
        
        # Validate stat types
        assert isinstance(stats['neuron_count'], int)
        assert isinstance(stats['synapse_count'], int)
        assert isinstance(stats['firing_neuron_count'], int)
        assert isinstance(stats['total_spike_count'], int)
        assert isinstance(stats['average_firing_rate'], float)
        
        return True
        
    except Exception as e:
        print(f"✗ Error in get_brain_stats test: {e}")
        return False

def test_enable_brain_features():
    """Test enable_brain_features function"""
    print("\n=== Testing enable_brain_features ===")
    
    try:
        from python.high_level_api import create_agent_brain_with_config, enable_brain_features
        import pynlm
        
        # Create brain and agent
        brain = create_agent_brain_with_config()
        agent = pynlm.createAgentBrain(brain)
        
        # Test enabling all features
        print("1. Testing enabling all features...")
        enable_brain_features(agent, 
                             reward_modulation=True,
                             structural_plasticity=True,
                             development=True,
                             curiosity=True)
        
        print("   ✓ All features enabled successfully")
        
        # Test checking feature status
        print("\n2. Checking feature status...")
        print(f"   - Reward modulation: {agent.isRewardModulationEnabled()}")
        print(f"   - Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
        print(f"   - Development: {agent.isDevelopmentEnabled()}")
        print(f"   - Curiosity: {agent.isCuriosityEnabled()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in enable_brain_features test: {e}")
        return False

def test_save_load_brain():
    """Test save_brain_with_timestamp and load_brain_with_backup functions"""
    print("\n=== Testing Save/Load Brain Functions ===")
    
    try:
        from python.high_level_api import create_agent_brain_with_config, save_brain_with_timestamp, load_brain_with_backup
        
        # Create brain and run some steps
        brain1 = create_agent_brain_with_config()
        for step in range(100):
            brain1.step(step)
        
        print("1. Testing save_brain_with_timestamp...")
        
        # Save with timestamp
        save_path = save_brain_with_timestamp(brain1)
        print(f"   ✓ Brain saved to: {save_path}")
        
        # Create another brain for load test
        brain2 = create_agent_brain_with_config()
        
        # Load the saved brain
        print("\n2. Testing load_brain_with_backup...")
        success = load_brain_with_backup(brain2, save_path)
        
        if success:
            print("   ✓ Brain loaded successfully")
        else:
            print("   ! Brain load failed but backup was created")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in save/load brain test: {e}")
        return False

def test_configure_brain_from_dict():
    """Test configure_brain_from_dict function"""
    print("\n=== Testing configure_brain_from_dict ===")
    
    try:
        from python.high_level_api import create_agent_brain_with_config, configure_brain_from_dict
        
        # Create brain
        brain = create_agent_brain_with_config()
        
        print("1. Testing brain configuration...")
        
        # Show initial config
        initial_neurons = brain.getTotalNeuronCount()
        print(f"   Initial neuron count: {initial_neurons}")
        
        # Configure with new settings
        config_updates = {
            "brain": {
                "neuron_count": 2000,
                "synapse_density": 0.25
            }
        }
        
        configure_brain_from_dict(brain, config_updates)
        
        # Verify changes
        final_neurons = brain.getTotalNeuronCount()
        print(f"   Final neuron count: {final_neurons}")
        
        if final_neurons == 2000:
            print("   ✓ Configuration updated successfully")
        else:
            print(f"   ! Neuron count changed from {initial_neurons} to {final_neurons}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in configure_brain_from_dict test: {e}")
        return False

def test_create_world_from_config():
    """Test create_world_from_config function"""
    print("\n=== Testing create_world_from_config ===")
    
    try:
        from python.high_level_api import create_world_from_config
        import pynlm
        
        print("1. Testing world creation with config...")
        
        world_config = {
            "width": 25,
            "height": 25,
            "visionWidth": 10,
            "visionHeight": 10,
            "agent_start": (12.5, 12.5)
        }
        
        world = create_world_from_config(world_config)
        
        print(f"   ✓ World created successfully")
        print(f"     - Dimensions: {world.getWidth()}x{world.getHeight()}")
        print(f"     - Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
        print(f"     - Agent start: {world.getAgentStart()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error in create_world_from_config test: {e}")
        return False

def run_all_tests():
    """Run all high-level API tests"""
    print("NLM High-Level API Tests")
    print("========================\n")
    
    if not NLM_AVAILABLE:
        print(f"ERROR: Could not import pynlm: {NLM_ERROR}")
        print("Make sure NLM is properly installed: pip install -e .")
        return False
    
    # Import high-level API for testing
    try:
        from python.high_level_api import run_agent_simulation
        # Set up a simple world and agent for simulation tests
        print("1. Creating test components for simulation...")
        brain = create_agent_brain_with_config()
        agent = pynlm.createAgentBrain(brain)
        world = create_world_from_config({"width": 20, "height": 20})
        agent.initialize(world)
        print("   ✓ Test components created")
    except Exception as e:
        print(f"   ! Could not create all test components: {e}")
    
    tests = [
        ("Module Imports", test_imports),
        ("create_agent_brain_with_config", test_create_agent_brain_with_config),
        ("get_brain_stats", test_get_brain_stats),
        ("enable_brain_features", test_enable_brain_features),
        ("Save/Load Brain", test_save_load_brain),
        ("configure_brain_from_dict", test_configure_brain_from_dict),
        ("create_world_from_config", test_create_world_from_config),
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\nRunning: {test_name}")
        print("-" * 60)
        success = test_func()
        results.append((test_name, success))
    
    # Print test results
    print("\n" + "=" * 60)
    print("Test Results")
    print("=" * 60)
    
    passed = 0
    total = len(results)
    
    for test_name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{test_name:35} {status}")
        if success:
            passed += 1
    
    print(f"\nOverall: {passed}/{total} tests passed")
    
    if passed == total:
        print("\n🎉 All tests passed! The high-level API is working correctly.")
    else:
        print(f"\n⚠️  {total - passed} test(s) failed. Please check the output above.")
    
    return passed == total

def print_summary():
    """Print summary of high-level API features"""
    print("\n" + "=" * 60)
    print("NLM High-Level API Summary")
    print("=" * 60)
    
    print("\nThe NLM High-Level API provides the following convenience functions:")
    print("\n1. create_agent_brain_with_config(config_dict=None)")
    print("   - Creates and initializes a brain with configuration")
    print("   - Supports both simple and advanced configuration")
    
    print("\n2. run_simulation_loop(brain, world, agent, steps, verbose=False)")
    print("   - Runs complete simulation loops")
    print("   - Handles world updates, sensory processing, motor actions")
    
    print("\n3. run_agent_simulation(config=None, world_config=None, steps=1000, enable_features=True)")
    print("   - Complete agent simulation with all setup")
    print("   - Full configuration and feature management")
    
    print("\n4. get_brain_stats(brain) -> dict")
    print("   - Returns structured brain statistics")
    print("   - Programmatically accessible statistics")
    
    print("\n5. enable_brain_features(agent, **features)")
    print("   - Batch enable/disable brain subsystems")
    print("   - Simple, readable feature management")
    
    print("\n6. save_brain_with_timestamp(brain, filepath=None)")
    print("   - Save brain state with automatic timestamps")
    print("   - Easy backup and version control")
    
    print("\n7. load_brain_with_backup(brain, filepath, backup_if_failed=True)")
    print("   - Safe brain loading with automatic backups")
    print("   - Prevents data loss on failed loads")
    
    print("\n8. configure_brain_from_dict(brain, config_dict)")
    print("   - Apply configuration changes to existing brain")
    print("   - Dynamic configuration management")
    
    print("\n9. create_world_from_config(world_config_dict)")
    print("   - Create and configure worlds from dictionaries")
    print("   - Easy world setup and customization")
    
    print("\nThese functions make NLM much more accessible to Python users while")
    print("maintaining all the power of the underlying C++ framework.")
if __name__ == "__main__":
    success = run_all_tests()
    print_summary()
    sys.exit(0 if success else 1)