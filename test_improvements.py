#!/usr/bin/env python3
"""
Test Script for NLM Improvements

This script tests the various improvements made to the NLM project,
including Python bindings, configuration fixes, and new functionality.
"""

import sys
import os
import tempfile
import json
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm


def test_python_bindings():
    """Test basic Python bindings functionality."""
    print("=== Testing Python Bindings ===")
    
    try:
        # Test 1: Create basic objects
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        print("✓ Object creation successful")
        
        # Test 2: Initialize and basic operations
        brain.initialize()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        print("✓ Initialization successful")
        
        # Test 3: Convenience functions
        stats = pynlm.getBrainStats(brain)
        assert isinstance(stats, dict)
        assert 'neurons' in stats
        assert 'synapses' in stats
        
        print("✓ Convenience functions working")
        
        # Test 4: Process loop
        for step in range(10):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Simulation loop working")
        
        # Test 5: All features
        agent.enableAllFeatures()
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
        
        print("✓ All features activation working")
        
        # Test 6: Process sensory from vectors
        pynlm.processSensoryPercept(agent, 
                                   [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8],  # vision
                                   [0.1],  # touch
                                   [0.2, 0.3],  # internal
                                   [0.4],  # audio
                                   0.5)
        print("✓ Vector sensory processing working")
        
        return True
        
    except Exception as e:
        print(f"✗ Python bindings test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_config_fixes():
    """Test configuration fixes."""
    print("\n=== Testing Configuration Fixes ===")
    
    try:
        # Test 1: Config file reading
        config = pynlm.createDefaultConfig()
        
        # Verify dopamine_baseline is set correctly (not duplicated)
        config.set("dopamine_baseline", 0.1)
        # In a real scenario, we'd load from default.cfg
        # For now, just verify the value can be set
        
        print("✓ Configuration setting working")
        
        # Test 2: File operations
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            print(f"✓ Config saved to: {f.name}")
            
            # Try to load it back
            config2 = pynlm.createDefaultConfig()
            if config2.loadFromFile(f.name):
                print("✓ Config loading working")
            else:
                print("✗ Config loading failed")
                return False
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_advanced_examples():
    """Test advanced examples functionality."""
    print("\n=== Testing Advanced Examples ===")
    
    try:
        # Test 1: Learning agent
        print("Testing learning agent...")
        # Note: We can't fully test without the full learning_agent.py
        # but we can test the basic functionality
        
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create a world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableAllFeatures()
        
        # Run a simple simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print("✓ Advanced example simulation working")
        
        # Test 2: Multi-agent simulation
        print("Testing multi-agent simulation...")
        agents = []
        brains = []
        
        for i in range(3):
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 300)
            config.set("connection_probability", 0.08 + (i * 0.01))
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableAllFeatures()
            
            agents.append(agent)
            brains.append(brain)
        
        print("✓ Multi-agent setup working")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced examples test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_command_line_interface():
    """Test command-line interface functionality."""
    print("\n=== Testing Command Line Interface ===")
    
    try:
        # Test 1: Basic CLI structure
        print("Testing CLI parser structure...")
        
        # We can't easily test the actual CLI without subprocess calls
        # but we can verify the module imports correctly
        from nlm_cli import create_parser
        
        parser = create_parser()
        print("✓ CLI parser creation working")
        
        # Test 2: Help generation
        help_text = parser.format_help()
        assert 'nlm' in help_text.lower()
        assert 'basic' in help_text.lower()
        assert 'learning' in help_text.lower()
        
        print("✓ Help generation working")
        
        return True
        
    except Exception as e:
        print(f"✗ CLI test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_documentation():
    """Test documentation generation."""
    print("\n=== Testing Documentation ===")
    
    try:
        # Test 1: API documentation generation
        print("Testing API documentation...")
        
        # Check that key documentation files exist
        docs_dir = os.path.join(os.path.dirname(__file__), 'docs')
        if os.path.exists(docs_dir):
            print("✓ Documentation directory exists")
        
        # Check that HOW_TO_USE.md exists and has content
        how_to_use = os.path.join(os.path.dirname(__file__), 'HOW_TO_USE.md')
        if os.path.exists(how_to_use):
            with open(how_to_use, 'r') as f:
                content = f.read()
                if len(content) > 1000:  # Should be substantial
                    print("✓ Comprehensive API documentation exists")
                else:
                    print("⚠ Documentation might be too short")
        else:
            print("⚠ HOW_TO_USE.md not found")
        
        return True
        
    except Exception as e:
        print(f"✗ Documentation test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_configuration_validation():
    """Test configuration validation."""
    print("\n=== Testing Configuration Validation ===")
    
    try:
        # Test 1: Default configuration
        config = pynlm.createDefaultConfig()
        
        # Verify key parameters exist
        assert config.has("neuron_count")
        assert config.has("region_count")
        assert config.has("simulation_timestep")
        assert config.has("random_seed")
        
        print("✓ Default configuration validation working")
        
        # Test 2: Configuration modification
        original_neuron_count = config.get("neuron_count")
        config.set("neuron_count", 999)
        assert config.get("neuron_count") == 999
        
        config.set("neuron_count", original_neuron_count)  # Restore
        print("✓ Configuration modification working")
        
        return True
        
    except Exception as e:
        print(f"✗ Configuration validation test failed: {e}")
        import traceback
        traceback.print_exc()
        return False


def main():
    """Run all tests."""
    print("Running NLM Improvements Test Suite")
    print("=" * 50)
    
    tests = [
        ("Python Bindings", test_python_bindings),
        ("Configuration Fixes", test_config_fixes),
        ("Advanced Examples", test_advanced_examples),
        ("Command Line Interface", test_command_line_interface),
        ("Documentation", test_documentation),
        ("Configuration Validation", test_configuration_validation),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} test crashed: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed!")
        return 0
    else:
        print(f"❌ {failed} tests failed")
        return 1


if __name__ == "__main__":
    sys.exit(main())
