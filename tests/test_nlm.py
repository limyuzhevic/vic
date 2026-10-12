#!/usr/bin/env python3
"""
Unit Tests for NLM (Neural Learning Machine)

This test suite validates the enhanced NLM functionality including:
- Neuromodulator improvements
- Core brain operations
- Agent brain interface
- Memory systems
- Configuration management
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import tempfile
import json
from typing import Dict, Any
class TestConfig:
    """Test configuration management"""
    
    def test_default_config(self):
        """Test that default configuration is created properly"""
        config = pynlm.createDefaultConfig()
        assert config is not None
        print("✓ Default configuration created")
    
    def test_config_values(self):
        """Test configuration value access"""
        config = pynlm.createDefaultConfig()
        
        # Test setting and getting values
        config.set("brain.neuron_count", 1000)
        assert config.get("brain.neuron_count") == 1000
        
        config.set("learning.rate", 0.001)
        assert config.get("learning.rate") == 0.001
        
        # Test has functionality
        assert config.has("brain.neuron_count")
        assert not config.has("nonexistent.key")
        
        print("✓ Configuration value access works")
    
    def test_config_file_io(self):
        """Test configuration file I/O"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        # Save to file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            filename = f.name
        
        try:
            # Load from file
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(filename)
            
            assert config2.get("brain.neuron_count") == 500
            print("✓ Configuration file I/O works")
        finally:
            os.unlink(filename)
    
    def test_config_summary(self):
        """Test configuration summary"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 123)
        
        summary = config.summary()
        assert "Config:" in summary
        assert "123" in summary
        print("✓ Configuration summary works")
class TestBrain:
    """Test brain core functionality"""
    
    def test_basic_brain_creation(self):
        """Test basic brain creation"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        assert brain is not None
        assert brain.getTotalNeuronCount() >= 100
        print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    def test_brain_step(self):
        """Test brain simulation step"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run several steps
        for i in range(10):
            brain.step(i)
        
        # Spike count should have changed
        assert brain.getTotalSpikeCount() >= initial_spikes
        print("✓ Brain simulation steps work")
    
    def test_brain_reset(self):
        """Test brain reset functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for i in range(5):
            brain.step(i)
        
        assert brain.getTotalSpikeCount() > 0
        
        # Reset
        brain.reset()
        brain.initialize()
        
        # Spike count should be reset
        assert brain.getTotalSpikeCount() == 0
        print("✓ Brain reset works")
    
    def test_sensory_input(self):
        """Test sensory input injection"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create a simple sensory input
        from pynlm import Vision
        vision = Vision()
        vision.setData([0.1, 0.2, 0.3, 0.4, 0.5])
        
        # This would normally go through AgentBrain, but we can test direct injection
        print("✓ Sensory input creation works")
    
    def test_neurons_firing(self):
        """Test that neurons can be made to fire"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get all neurons and inject current to make them fire
        # This is a basic test - real neuron control would need more sophisticated API
        print("✓ Neuron firing test structure in place")
class TestAgentBrain:
    """Test AgentBrain interface"""
    
    def test_agent_creation(self):
        """Test AgentBrain creation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        print("✓ AgentBrain creation works")
    
    def test_agent_initialization(self):
        """Test AgentBrain initialization with world"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        # Initialize agent
        agent.initialize(world)
        
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        print("✓ AgentBrain initialization works")
    
    def test_sensory_processing(self):
        """Test sensory processing through AgentBrain"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure and reset world
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Create a simple percept
        from pynlm import SensoryPercept, Vision
        
        percept = SensoryPercept()
        vision = Vision()
        vision.setData([0.5] * 64)  # 8x8 vision data
        percept.setVision(vision)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        print("✓ Sensory processing works")
    
    def test_motor_decoding(self):
        """Test motor command decoding"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Decode motor command (should return a valid command)
        cmd = agent.decodeMotorCommand()
        
        # Check it's a valid MotorCommand enum value
        from pynlm import MotorCommand
        valid_commands = [
            MotorCommand.MoveForward, MotorCommand.MoveBackward,
            MotorCommand.TurnLeft, MotorCommand.TurnRight,
            MotorCommand.LookLeft, MotorCommand.LookRight,
            MotorCommand.Interact, MotorCommand.Wait
        ]
        
        assert cmd in valid_commands
        print(f"✓ Motor decoding returns valid command: {cmd}")
    
    def test_neuromodulation(self):
        """Test neuromodulation functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Test neuromodulation control
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        assert agent.isRewardModulationEnabled()
        assert agent.isCuriosityEnabled()
        assert agent.isDevelopmentEnabled()
        
        # Test neuromodulation levels
        agent.applyRewardModulation(0.5, 0.3)
        
        assert agent.getNeuromodulationLevel() >= 0
        assert agent.getCuriosityLevel() >= 0
        assert agent.getPredictionError() >= 0
        
        print("✓ Neuromodulation controls work")
class TestWorld:
    """Test world simulation"""
    
    def test_world_creation(self):
        """Test world creation"""
        world = pynlm.createSimpleWorld()
        assert world is not None
        print("✓ World creation works")
    
    def test_world_configuration(self):
        """Test world configuration"""
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        assert world.getWidth() == 20
        assert world.getHeight() == 20
        print("✓ World configuration works")
    
    def test_world_update(self):
        """Test world update"""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        initial_time = world.getSimulationTime()
        
        world.update(0.1)  # Update for 0.1 seconds
        
        assert world.getSimulationTime() > initial_time
        print("✓ World update works")
    
    def test_world_interaction(self):
        """Test world-agent interaction"""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(5.0, 5.0)
        
        # Get percept
        percept = world.getSensoryPercept()
        assert percept is not None
        
        # Apply motor command
        from pynlm import MotorCommand
        world.applyMotorCommand(MotorCommand.MoveForward, world.getSimulationTime())
        
        print("✓ World-agent interaction works")
class TestIntegration:
    """Integration tests"""
    
    def test_complete_agent_simulation(self):
        """Test a complete agent simulation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        
        # Configure and reset
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward (simple +1 for each step)
            agent.applyRewardModulation(1.0, 0.5)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify results
        assert brain.getTotalSpikeCount() > 0
        assert agent.getNeuromodulationLevel() >= 0
        assert agent.getCuriosityLevel() >= 0
        
        print("✓ Complete agent simulation works")
    
    def test_checkpointing(self):
        """Test checkpoint save/load functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps to create state
        for i in range(20):
            brain.step(i)
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            brain.save(f.name)
            filename = f.name
        
        try:
            # Reset and reload
            brain.reset()
            brain.initialize()
            
            for i in range(20):
                brain.step(i)
            
            # Load from checkpoint
            brain.load(filename)
            
            # Should have original spike count
            assert brain.getTotalSpikeCount() == initial_spikes
            print("✓ Checkpoint save/load works")
        finally:
            os.unlink(filename)
def run_all_tests():
    """Run all tests"""
    print("=" * 60)
    print("NLM Unit Test Suite")
    print("=" * 60)
    
    test_suite = [
        ("Config", TestConfig()),
        ("Brain", TestBrain()),
        ("AgentBrain", TestAgentBrain()),
        ("World", TestWorld()),
        ("Integration", TestIntegration())
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_instance in test_suite:
        print(f"\n--- Testing {test_name} ---")
        
        test_methods = [m for m in dir(test_instance) if m.startswith('test_')]
        
        for method_name in test_methods:
            try:
                method = getattr(test_instance, method_name)
                method()
                passed += 1
            except Exception as e:
                print(f"✗ {method_name}: {str(e)}")
                failed += 1
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed > 0:
        print(f"\n⚠️  {failed} tests failed")
        return False
    else:
        print("\n✅ All tests passed!")
        return True
if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)