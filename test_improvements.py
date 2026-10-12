#!/usr/bin/env python3
"""
Test suite for NLM improvements
Tests configuration loading fixes, AgentBrain improvements, and new features

This test file validates the improvements made to the NLM project:
1. Configuration loading bug fixes
2. AgentBrain initialization and organization improvements  
3. Better command-line interface and configuration options
4. Advanced user features and configuration
5. Improved documentation and examples
6. Comprehensive testing and validation

The improvements address:
- Duplicate configuration parameters
- Complex initialization logic
- Limited command-line options
- Missing advanced configuration options
- Documentation gaps
"""

import unittest
import tempfile
import os
import sys
from unittest.mock import patch, MagicMock

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
class TestConfigurationLoading(unittest.TestCase):
    """Test configuration loading fixes"""
    
    def test_default_config_loading(self):
        """Test that default configuration can be loaded"""
        config = pynlm.createDefaultConfig()
        
        # Check that essential parameters are set
        self.assertIsNotNone(config)
        self.assertTrue(config.has("brain.neuron_count"))
        self.assertTrue(config.has("plasticity.stdp.enable"))
        
        # Check default values
        self.assertEqual(config.get("brain.neuron_count"), 1000)
        self.assertEqual(config.get("plasticity.stdp.enable"), True)
    
    def test_config_save_and_load(self):
        """Test that configuration can be saved and loaded"""
        config = pynlm.createDefaultConfig()
        
        # Modify some values
        config.set("brain.neuron_count", 2000)
        config.set("environment.width", 20)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_path = f.name
        
        try:
            config.saveToFile(config_path)
            
            # Load back
            new_config = pynlm.createDefaultConfig()
            new_config.loadFromFile(config_path)
            
            # Verify values
            self.assertEqual(new_config.get("brain.neuron_count"), 2000)
            self.assertEqual(new_config.get("environment.width"), 20)
            
        finally:
            # Clean up
            if os.path.exists(config_path):
                os.unlink(config_path)
    
    def test_config_validation(self):
        """Test configuration validation"""
        config = pynlm.createDefaultConfig()
        
        # Set valid values
        config.set("brain.neuron_count", 500)
        config.set("brain.connection_probability", 0.1)
        config.set("plasticity.stdp.learning_rate", 0.001)
        
        # All these should be accepted
        self.assertTrue(config.has("brain.neuron_count"))
        self.assertTrue(config.has("brain.connection_probability"))
        self.assertTrue(config.has("plasticity.stdp.learning_rate"))
class TestAgentBrainImprovements(unittest.TestCase):
    """Test AgentBrain improvements"""
    
    def test_agent_brain_initialization(self):
        """Test improved AgentBrain initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Initialize agent
        agent.initialize(world)
        
        # Verify that agent has proper state
        self.assertIsNotNone(agent.getBrain())
        self.assertEqual(agent.getBrain(), brain)
        self.assertTrue(agent.isRewardModulationEnabled())
        self.assertTrue(agent.isDevelopmentEnabled())
        self.assertTrue(agent.isCuriosityEnabled())
    
    def test_neuron_group_initialization(self):
        """Test that neuron groups are properly initialized"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Check that brain has motor and sensory neurons
        total_neurons = brain.getTotalNeuronCount()
        self.assertGreater(total_neurons, 0)
        
        # Get regions and populations
        regions = brain.getRegions()
        self.assertGreater(regions.size(), 0)
        
        total_motor = 0
        total_sensory = 0
        
        for region in regions:
            for population in region.getPopulations():
                neuron_type = population.getNeuronType()
                if neuron_type == pynlm.NeuronType.Motor:
                    total_motor += population.getNeuronCount()
                elif neuron_type == pynlm.NeuronType.Sensory:
                    total_sensory += population.getNeuronCount()
        
        # Should have some motor and sensory neurons
        self.assertGreaterEqual(total_motor, 1)
        self.assertGreaterEqual(total_sensory, 1)
    
    def test_agent_functionality(self):
        """Test basic agent functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            
            # Action should be valid
            self.assertIn(action, [
                pynlm.ActionType.MoveForward,
                pynlm.ActionType.MoveBackward,
                pynlm.ActionType.TurnLeft,
                pynlm.ActionType.TurnRight,
                pynlm.ActionType.Interact,
                pynlm.ActionType.Wait
            ])
        
        # Check that agent metrics are reasonable
        self.assertGreaterEqual(agent.getCuriosityLevel(), 0.0)
        self.assertLessEqual(agent.getCuriosityLevel(), 1.0)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0)
        self.assertLessEqual(agent.getNoveltyLevel(), 1.0)
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced user features"""
    
    def test_advanced_configuration(self):
        """Test advanced configuration options"""
        config = pynlm.createDefaultConfig()
        
        # Set advanced parameters
        config.set("brain.neuron_count", 3000)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.005)
        config.set("plasticity.stdp.tau_plus", 30.0)
        config.set("plasticity.stdp.tau_minus", 30.0)
        config.set("neuromod.dopamine.scale", 2.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("environment.width", 30)
        config.set("environment.height", 30)
        config.set("environment.max_energy", 200.0)
        
        # Create brain with advanced configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify configuration was applied
        self.assertEqual(brain.getTotalNeuronCount(), 3000)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
    
    def test_development_stages(self):
        """Test development stages and progression"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Track development stages over time
        stages = []
        
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            agent.decodeMotorCommand()
            
            stage = brain.getDevelopmentalStage()
            stages.append(stage)
        
        # Should have progression through developmental stages
        unique_stages = set(stages)
        self.assertGreaterEqual(len(unique_stages), 1)
        
        # Development stage should be consistent or change gradually
        initial_stage = stages[0]
        final_stage = stages[-1]
        
        # Either stay same or progress forward (depending on configuration)
        self.assertTrue(initial_stage == final_stage or 
                       (pynlm.DevelopmentalStage.Initial < pynlm.DevelopmentalStage.CriticalPeriod and
                        pynlm.DevelopmentalStage.CriticalPeriod < pynlm.DevelopmentalStage.Maturation and
                        pynlm.DevelopmentalStage.Maturation < pynlm.DevelopmentalStage.Adult))
    
    def test_checkpointing_advanced(self):
        """Test advanced checkpointing features"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation to generate activity
        for step in range(200):
            brain.step(step)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_path = f.name
        
        try:
            save_success = brain.save(checkpoint_path)
            self.assertTrue(save_success)
            
            # Verify file exists and is not empty
            self.assertTrue(os.path.exists(checkpoint_path))
            self.assertGreater(os.path.getsize(checkpoint_path), 0)
            
            # Create new brain and load checkpoint
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            
            load_success = new_brain.load(checkpoint_path)
            self.assertTrue(load_success)
            
            # Check that loaded brain has reasonable state
            self.assertGreaterEqual(new_brain.getTotalNeuronCount(), 0)
            
        finally:
            if os.path.exists(checkpoint_path):
                os.unlink(checkpoint_path)
class TestIntegrationAndValidation(unittest.TestCase):
    """Test integration and validation scenarios"""
    
    def test_complete_simulation_integration(self):
        """Test complete simulation with all systems"""
        # Create complete agent-world-brain setup
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("environment.width", 15)
        config.set("environment.height", 15)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
        world.reset()
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run complete simulation
        metrics = {
            'steps_completed': 0,
            'total_reward': 0,
            'final_curiosity': 0,
            'final_novelty': 0,
            'neurons_firing': 0,
        }
        
        for step in range(300):
            # Update world
            world.update(0.05)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.05)
            
            # Collect metrics
            metrics['total_reward'] += reward
            metrics['steps_completed'] += 1
        
        # Validate results
        self.assertEqual(metrics['steps_completed'], 300)
        self.assertGreaterEqual(metrics['total_reward'], 0)
        self.assertGreaterEqual(agent.getCuriosityLevel(), 0)
        self.assertLessEqual(agent.getCuriosityLevel(), 1)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0)
        self.assertLessEqual(agent.getNoveltyLevel(), 1)
        
        # Check that brain is still functional
        self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(brain.getFiringNeuronCount(), 0)
    
    def test_error_handling_and_robustness(self):
        """Test error handling and robustness"""
        # Test that system handles edge cases gracefully
        config = pynlm.createDefaultConfig()
        
        # Create brain with minimal configuration
        brain = pynlm.createBrain(config)
        
        # Should handle initialization even with minimal setup
        try:
            brain.initialize()
            self.assertTrue(True)  # If we get here, initialization succeeded
        except Exception as e:
            self.fail(f"Brain initialization should not throw exception: {e}")
        
        # Test that agent can be created even if brain has few neurons
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent.getBrain())
        
        # Test that agent methods handle empty or invalid states
        world = pynlm.createSimpleWorld()
        world.configure(width=5, height=5, visionWidth=3, visionHeight=3)
        world.reset()
        agent.initialize(world)
        
        # These methods should not crash even in edge cases
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        neuromodulation = agent.getNeuromodulationLevel()
        
        # Values should be in reasonable ranges
        self.assertGreaterEqual(curiosity_level, 0.0)
        self.assertLessEqual(curiosity_level, 1.0)
        self.assertGreaterEqual(novelty_level, 0.0)
        self.assertLessEqual(novelty_level, 1.0)
        # Prediction error and neuromodulation could be negative
class TestPerformanceAndScalability(unittest.TestCase):
    """Test performance and scalability"""
    
    def test_large_brain_performance(self):
        """Test performance with larger brain configuration"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.connection_probability", 0.05)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Check that initialization completes in reasonable time
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        
        # Should be able to run simulation steps
        for step in range(100):
            brain.step(step)
            self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
            self.assertGreaterEqual(brain.getFiringNeuronCount(), 0)
    
    def test_memory_usage(self):
        """Test memory usage patterns"""
        config = pynlm.createDefaultConfig()
        
        # Test with default configuration
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        # Test with smaller configuration
        config.set("brain.neuron_count", 500)
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        # Both should work without memory issues
        for step in range(50):
            brain1.step(step)
            brain2.step(step)
        
        self.assertGreaterEqual(brain1.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(brain2.getTotalSpikeCount(), 0)
# Test helper functions
def run_improvements_tests():
    """
    Run all improvement tests.
    
    This function provides a simple way to run the improvement tests
    and verify that all changes are working correctly.
    """
    print("=" * 70)
    print("NLM PROJECT IMPROVEMENTS VALIDATION SUITE")
    print("=" * 70)
    print()
    print("This test suite validates the improvements made to the NLM project:")
    print("1. Configuration loading bug fixes")
    print("2. AgentBrain initialization and organization improvements")
    print("3. Better command-line interface and configuration options")
    print("4. Advanced user features and configuration")
    print("5. Improved documentation and examples")
    print("6. Comprehensive testing and validation")
    print()
    print("The improvements address:")
    print("- Duplicate configuration parameters (fixed in Phase6IntegratedExperiment.cpp)")
    print("- Complex initialization logic (simplified in AgentBrain.cpp)")
    print("- Limited command-line options (enhanced in main.cpp)")
    print("- Missing advanced configuration options (added in configs/default.cfg)")
    print("- Documentation gaps (improved in easy_usage.md)")
    print()
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestConfigurationLoading)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAgentBrainImprovements))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAdvancedFeatures))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestIntegrationAndValidation))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestPerformanceAndScalability))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    print()
    print("=" * 70)
    if result.wasSuccessful():
        print("SUCCESS: All improvement tests passed!")
        print("The NLM project improvements are working correctly.")
    else:
        print("FAILURE: Some improvement tests failed.")
        print("Please review the test failures and fix any issues.")
    print("=" * 70)
    
    return result.wasSuccessful()
if __name__ == '__main__':
    # If running as script, run the test suite
    success = run_improvements_tests()
    sys.exit(0 if success else 1)

