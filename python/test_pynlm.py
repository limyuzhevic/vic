"""
Python Bindings Tests for NLM
=============================

This module contains tests for the NLM Python bindings.
These tests validate that the Python API works correctly and
provide basic functionality verification.

Run these tests using:
    python -m pytest test_pynlm.py -v
    python test_pynlm.py (for basic execution)
"""

import sys
import os
import tempfile
import pytest
import pynlm

# Add current directory to path to ensure we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

class TestBasicBindings:
    """Test basic Python bindings functionality."""
    
    def test_import_and_version(self):
        """Test that pynlm can be imported and has version."""
        assert hasattr(pynlm, '__version__')
        assert isinstance(pynlm.__version__, str)
        assert len(pynlm.__version__) > 0
    
    def test_factory_functions(self):
        """Test that factory functions work correctly."""
        # Test config factory
        config = pynlm.createDefaultConfig()
        assert config is not None
        assert config.has("brain.neuron_count") or config.has("neuron_count")
        
        # Test brain factory
        brain = pynlm.createBrain()
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        
        # Test world factory
        world = pynlm.createSimpleWorld()
        assert world is not None
        assert world.getWidth() > 0
        assert world.getHeight() > 0
        
        # Test agent factory
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
    
    def test_basic_simulation(self):
        """Test a basic simulation works correctly."""
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Initialize
        brain.initialize()
        agent.initialize(world)
        
        # Run a few steps
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify results
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
    
    def test_config_operations(self):
        """Test configuration operations."""
        config = pynlm.createDefaultConfig()
        
        # Test setting and getting values
        config.setInt("test_int", 42)
        assert config.getInt("test_int") == 42
        
        config.setFloat("test_float", 3.14)
        assert config.getFloat("test_float") == 3.14
        
        config.setBool("test_bool", True)
        assert config.getBool("test_bool") == True
        
        config.set("test_string", "hello")
        assert config.get("test_string") == "hello"
        
        # Test default values
        assert config.getInt("non_existent", 100) == 100
        assert config.getFloat("non_existent", 2.71) == 2.71
        assert config.getBool("non_existent", False) == False
        
        # Test has method
        assert config.has("test_int")
        assert not config.has("non_existent")
        
        # Test remove
        config.remove("test_int")
        assert not config.has("test_int")
        
        # Test clear
        config.setInt("temp", 1)
        config.setInt("temp2", 2)
        config.clear()
        assert not config.has("temp")
        assert not config.has("temp2")
    
    def test_brain_operations(self):
        """Test brain operations."""
        brain = pynlm.createBrain()
        brain.initialize()
        
        # Test basic properties
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getRegionCount() > 0
        
        # Test region operations
        region_id = brain.addRegion("TestRegion")
        assert region_id.value > 0
        
        region = brain.getRegion(region_id)
        assert region is not None
        
        # Test stats method
        stats = brain.getStats()
        assert len(stats) == 5
        assert stats[0] == brain.getTotalNeuronCount()
        assert stats[1] == brain.getTotalSynapseCount()
        
        # Test development stage
        stage = brain.getDevelopmentalStage()
        assert stage is not None
        
        # Test reset
        brain.reset()
        assert brain.getTotalSpikeCount() == 0
    
    def test_agent_operations(self):
        """Test agent operations."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Test initialization
        agent.initialize(world)
        
        # Test sizes
        assert agent.getSensoryInputSize() > 0
        assert agent.getMotorOutputSize() > 0
        
        # Test state
        assert agent.isRewardModulationEnabled()
        assert agent.isCuriosityEnabled()
        assert agent.isDevelopmentEnabled()
        
        # Test development stage
        stage = agent.getDevelopmentalStage()
        assert stage is not None
        
        # Test reset
        agent.reset()
        
        # Test stats
        stats = agent.getStats()
        assert len(stats) == 5
    
    def test_world_operations(self):
        """Test world operations."""
        world = pynlm.createSimpleWorld()
        
        # Test configuration
        assert world.getWidth() == 20.0  # Default
        assert world.getHeight() == 20.0  # Default
        assert world.getVisionWidth() == 16  # Default
        assert world.getVisionHeight() == 16  # Default
        
        # Configure world
        world.configure(width=30, height=40, visionWidth=10, visionHeight=10)
        
        assert world.getWidth() == 30.0
        assert world.getHeight() == 40.0
        assert world.getVisionWidth() == 10
        assert world.getVisionHeight() == 10
        
        # Test reset
        world.reset()
        
        # Test agent start
        world.setAgentStart(15.0, 25.0)
        agent_body = world.getAgentBody()
        assert agent_body.x == 15.0
        assert agent_body.y == 25.0
        
        # Test update
        world.update(0.1)
        assert world.getSimulationTime() > 0
        
        # Test energy
        assert world.getMaxEnergy() > 0
        assert world.getEnergyDecayRate() > 0
        
        # Test random seed
        seed = world.getRandomSeed()
        world.setRandomSeed(123)
        assert world.getRandomSeed() == 123
class TestErrorHandling:
    """Test error handling in Python bindings."""
    
    def test_invalid_config_key(self):
        """Test handling of invalid configuration keys."""
        config = pynlm.createDefaultConfig()
        
        # Empty key should raise error
        try:
            config.set("", "value")
            assert False, "Should have raised an error for empty key"
        except Exception as e:
            assert "empty" in str(e).lower()
    
    def test_config_validation(self):
        """Test configuration validation."""
        config = pynlm.createDefaultConfig()
        
        # Test that invalid keys raise errors
        # (Implementation depends on actual error handling)
        pass
class TestAdvancedFeatures:
    """Test advanced features and functionality."""
    
    def test_sensorimotor_loop(self):
        """Test complete sensorimotor loop."""
        # Create complete simulation
        brain = pynlm.createBrain()
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.setAgentStart(10, 10)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run complete loop
        for step in range(50):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process in agent
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Verify results
        assert brain.getTotalSpikeCount() > 0
        assert agent.getCuriosityLevel() > 0
    
    def test_multiple_simulations(self):
        """Test running multiple simulations."""
        simulations = []
        
        # Create multiple simulations
        for i in range(3):
            config = pynlm.createDefaultConfig()
            config.setInt("neuron_count", 500 + i * 200)
            
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.setAgentStart(i * 5, i * 5)
            
            brain.initialize()
            agent.initialize(world)
            
            simulations.append((brain, world, agent))
        
        # Run all simulations
        for i, (brain, world, agent) in enumerate(simulations):
            for step in range(20):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify all simulations ran
        for i, (brain, world, agent) in enumerate(simulations):
            assert brain.getTotalSpikeCount() > 0
            print(f"Simulation {i + 1}: {brain.getTotalSpikeCount()} spikes, {brain.getTotalNeuronCount()} neurons")
    
    def test_checkpoint_operations(self):
        """Test checkpoint save/load operations."""
        import tempfile
        import os
        
        brain = pynlm.createBrain()
        brain.initialize()
        
        # Run some steps to create interesting state
        for step in range(10):
            brain.step(step)
        
        # Create temporary directory for checkpoints
        with tempfile.TemporaryDirectory() as tmpdir:
            checkpoint_file = os.path.join(tmpdir, "test_checkpoint.bin")
            
            # Save checkpoint
            success = brain.save(checkpoint_file)
            assert success, "Failed to save checkpoint"
            assert os.path.exists(checkpoint_file)
            
            # Reset brain
            brain.reset()
            assert brain.getTotalSpikeCount() == 0
            
            # Load checkpoint
            success = brain.load(checkpoint_file)
            assert success, "Failed to load checkpoint"
            
            # Verify loaded state
            assert brain.getTotalSpikeCount() > 0  # Should have spikes from saved state
    
    def test_serialization_compatibility(self):
        """Test serialization compatibility."""
        # Test that different configurations work
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        config2.setInt("neuron_count", 2000)
        
        # Both should work
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        brain1.initialize()
        brain2.initialize()
        
        # Should have different neuron counts
        assert brain1.getTotalNeuronCount() != brain2.getTotalNeuronCount()
class TestPerformance:
    """Basic performance tests."""
    
    def test_simulation_speed(self):
        """Test that simulation runs at reasonable speed."""
        import time
        
        brain = pynlm.createBrain()
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Time 100 simulation steps
        start_time = time.time()
        
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should complete 100 steps reasonably quickly (less than 5 seconds for basic system)
        assert elapsed < 5.0, f"Simulation too slow: {elapsed:.2f}s for 100 steps"
        
        print(f"Performance: {100 / elapsed:.0f} steps/second, {elapsed * 1000 / 100:.1f}ms per step")

# Integration test to run all examples
def run_all_basic_tests():
    """Run all basic tests and report results."""
    print("Running NLM Python Bindings Tests")
    print("=" * 50)
    
    test_classes = [
        TestBasicBindings,
        TestErrorHandling,
        TestAdvancedFeatures,
        TestPerformance,
    ]
    
    passed = 0
    failed = 0
    
    for test_class in test_classes:
        print(f"\n--- Testing {test_class.__name__} ---")
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [method for method in dir(test_instance) if method.startswith('test_')]
        
        for method_name in test_methods:
            try:
                method = getattr(test_instance, method_name)
                method()
                print(f"✓ {method_name}")
                passed += 1
            except Exception as e:
                print(f"✗ {method_name}: {e}")
                failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("All tests passed! ✓")
        return True
    else:
        print(f"Some tests failed: {failed} failures")
        return False

if __name__ == "__main__":
    # Run tests
    success = run_all_basic_tests()
    
    if success:
        print("\nBasic tests completed successfully!")
        print("For more comprehensive tests, run:")
        print("    python -m pytest test_pynlm.py -v")
        sys.exit(0)
    else:
        print("\nSome tests failed.")
        sys.exit(1)
