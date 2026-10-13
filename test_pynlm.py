#!/usr/bin/env python3
"""
Simple test script to verify the NLM Python package functionality.
This script tests basic imports and functionality.
"""

import sys
import os

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def test_imports():
    """Test that all main modules can be imported"""
    print("Testing imports...")
    
    try:
        from pynlm import (
            Brain, createBrain, createDefaultConfig,
            World, createSimpleWorld,
            AgentBrain, createAgentBrain,
            Config, SensoryPercept, WorldObject,
            MotorCommand, WorldObjectType, DevelopmentalStage,
            NeuronType, SynapseType, ActionType,
            NLMError, ConfigurationError, SimulationError,
            ErrorHandler, DebugLogger, PerformanceMonitor,
            get_version, get_build_info, validate_config, print_system_info,
            types
        )
        print("✓ All main modules imported successfully")
        return True
    except ImportError as e:
        print(f"✗ Import failed: {e}")
        return False

def test_basic_functionality():
    """Test basic functionality"""
    print("\nTesting basic functionality...")
    
    try:
        import pynlm
        
        # Test version
        version = pynlm.get_version()
        print(f"✓ Version: {version}")
        
        # Test build info
        build_info = pynlm.get_build_info()
        print(f"✓ Platform: {build_info['platform']}")
        
        # Test default config
        config = pynlm.createDefaultConfig()
        print(f"✓ Default config created: {config.neuron_count} neurons")
        
        # Test brain creation
        brain = pynlm.createBrain(config)
        print("✓ Brain created successfully")
        
        # Test brain initialization
        brain.initialize()
        print("✓ Brain initialized successfully")
        
        # Test brain step
        for step in range(5):
            brain.step(step)
        print("✓ Brain steps completed")
        
        # Test statistics
        firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        print(f"✓ Statistics - Firing rate: {firing_rate:.2f} Hz, Total spikes: {total_spikes}")
        
        return True
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_config():
    """Test configuration system"""
    print("\nTesting configuration system...")
    
    try:
        import pynlm
        
        # Create custom config
        config = pynlm.createDefaultConfig()
        config.neuron_count = 100
        config.region_count = 2
        config.connection_probability = 0.15
        
        print(f"✓ Custom config: {config.neuron_count} neurons, {config.region_count} regions")
        
        # Test validation
        config_dict = {
            'neuron_count': config.neuron_count,
            'simulation_timestep': config.simulation_timestep,
            'connection_probability': config.connection_probability,
        }
        
        errors = pynlm.validate_config(config_dict)
        if errors:
            print(f"✗ Config validation errors: {errors}")
            return False
        else:
            print("✓ Config validation passed")
        
        # Test brain with custom config
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("✓ Brain initialized with custom config")
        
        return True
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        return False

def test_world():
    """Test world functionality"""
    print("\nTesting world functionality...")
    
    try:
        import pynlm
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=50, height=50, vision_width=10, vision_height=10)
        print("✓ World configured")
        
        # Add objects
        resource = pynlm.WorldObject(25, 25, pynlm.WorldObjectType.Resource, 10.0, 2.0)
        world.addObject(resource)
        
        hazard = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Hazard, 5.0, 2.0)
        world.addObject(hazard)
        
        print("✓ World objects added")
        
        # Get agent body
        agent_body = world.getAgentBody()
        print(f"✓ Agent position: ({agent_body.x}, {agent_body.y})")
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        print(f"✓ Sensory percept received: {len(percept.vision_data)} vision cells")
        
        # Test world update
        world.update(0.1)
        print("✓ World updated")
        
        # Test action
        action = pynlm.MotorCommand.MoveForward
        result = world.applyMotorCommand(action, 0.1)
        print(f"✓ Action applied: {action.name}, Reward: {result.reward}, Success: {result.success}")
        
        return True
    except Exception as e:
        print(f"✗ World test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_agent():
    """Test agent brain functionality"""
    print("\nTesting agent brain functionality...")
    
    try:
        import pynlm
        
        # Create brain and world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=60, height=60, vision_width=12, vision_height=12)
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning systems
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        
        print("✓ Agent brain created and initialized")
        print(f"✓ Developmental stage: {agent.get_developmental_stage().name}")
        print(f"✓ Neuromodulation level: {agent.get_neuromodulation_level():.3f}")
        
        # Test sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        print("✓ Sensory input processed")
        
        # Test motor command decoding
        action = agent.decodeMotorCommand()
        print(f"✓ Motor command decoded: {action.name}")
        
        # Test reward modulation
        agent.applyRewardModulation(5.0, 3.0)
        print(f"✓ Reward modulation applied")
        
        # Test development update
        agent.updateDevelopment(0.1)
        print(f"✓ Development updated")
        
        return True
    except Exception as e:
        print(f"✗ Agent test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_utils():
    """Test utility functions"""
    print("\nTesting utility functions...")
    
    try:
        import pynlm
        
        # Test error handler
        try:
            raise ValueError("Test error")
        except Exception as e:
            error_info = pynlm.ErrorHandler.handle_error(e)
            print(f"✓ Error handler processed: {error_info['type']}")
        
        # Test debug logger
        logger = pynlm.DebugLogger()
        logger.info("Test log message")
        entries = logger.get_log_entries()
        print(f"✓ Debug logger: {len(entries)} entries")
        
        # Test performance monitor
        monitor = pynlm.PerformanceMonitor()
        monitor.start_operation("test")
        time.sleep(0.01)
        monitor.end_operation("test")
        duration = monitor.get_average_timing("test")
        print(f"✓ Performance monitor: {duration:.3f}s")
        
        # Test system info
        pynlm.print_system_info()
        print("✓ System info printed")
        
        return True
    except Exception as e:
        print(f"✗ Utility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Package Test Suite")
    print("=" * 60)
    
    tests = [
        ("Module Imports", test_imports),
        ("Basic Functionality", test_basic_functionality),
        ("Configuration System", test_config),
        ("World Simulation", test_world),
        ("Agent Brain", test_agent),
        ("Utility Functions", test_utils),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*60}")
        print(f"Running: {test_name}")
        print(f"{'='*60}")
        
        if test_func():
            passed += 1
            print(f"✓ {test_name}: PASSED")
        else:
            failed += 1
            print(f"✗ {test_name}: FAILED")
    
    print(f"\n{'='*60}")
    print(f"Test Results: {passed} passed, {failed} failed")
    print(f"{'='*60}")
    
    if failed == 0:
        print("✓ All tests passed! The NLM package is working correctly.")
        return 0
    else:
        print(f"✗ {failed} tests failed. Please check the output above.")
        return 1


if __name__ == '__main__':
    sys.exit(main())
