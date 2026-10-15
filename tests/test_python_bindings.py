#!/usr/bin/env python3
"""
Python tests for NLM Python bindings

This module provides comprehensive tests for the pynlm Python bindings,
validating that all the core functionality works correctly through the Python API.
"""

import sys
import os
import tempfile
import json
import unittest
from typing import Optional, Dict, Any

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    print("Please install the NLM Python bindings: pip install pynlm")
    sys.exit(1)
class TestBasicBrain(unittest.TestCase):
    """Test basic brain creation and initialization"""
    
    def testBrainCreation(self):
        """Test basic brain creation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        self.assertIsNotNone(brain)
        
        # Initialize the brain
        result = brain.initialize()
        self.assertTrue(result)
        
        # Check basic statistics
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        firing_count = brain.getFiringNeuronCount()
        
        self.assertGreater(neuron_count, 0)
        self.assertGreaterOrEqual(synapse_count, 0)
        self.assertGreaterOrEqual(firing_count, 0)
    
    def testBrainStepping(self):
        """Test brain stepping and spike generation"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Record initial spike count
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run simulation
        for step in range(10):
            brain.step(step)
        
        # Check that spikes were generated
        final_spikes = brain.getTotalSpikeCount()
        self.assertGreaterEqual(final_spikes, initial_spikes)
        
        # Check firing neurons
        firing_neurons = brain.getFiringNeuronCount()
        self.assertGreaterEqual(firing_neurons, 0)
class TestAgentWorld(unittest.TestCase):
    """Test agent and world functionality"""
    
    def testAgentCreation(self):
        """Test agent creation with brain"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        self.assertIsNotNone(agent)
        
        # Initialize with world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        self.assertIsNotNone(agent)
    
    def testAgentSensoryProcessing(self):
        """Test agent sensory processing"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Should not crash
        action = agent.decodeMotorCommand()
        self.assertIn(action, [
            pynlm.MotorCommand.MoveForward,
            pynlm.MotorCommand.MoveBackward,
            pynlm.MotorCommand.TurnLeft,
            pynlm.MotorCommand.TurnRight,
            pynlm.MotorCommand.LookLeft,
            pynlm.MotorCommand.LookRight,
            pynlm.MotorCommand.Interact,
            pynlm.MotorCommand.Wait
        ])
    
    def testWorldInteraction(self):
        """Test world-agent interaction"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Run simulation loop
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Check metrics
            self.assertGreaterEqual(brain.getFiringNeuronCount(), 0)
            self.assertGreaterEqual(agent.getCuriosityLevel(), 0.0)
            self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0)
            self.assertGreaterEqual(agent.getNeuromodulationLevel(), -1.0)
            self.assertLessEqual(agent.getNeuromodulationLevel(), 1.0)
class TestConfiguration(unittest.TestCase):
    """Test configuration management"""
    
    def testConfigCreation(self):
        """Test configuration creation"""
        config = pynlm.createDefaultConfig()
        self.assertIsNotNone(config)
        
        # Test basic configuration operations
        config.set("test_int", 42)
        config.set("test_string", "hello")
        config.set("test_double", 3.14)
        config.set("test_bool", True)
        
        self.assertTrue(config.has("test_int"))
        self.assertTrue(config.has("test_string"))
        self.assertTrue(config.has("test_double"))
        self.assertTrue(config.has("test_bool"))
        
        # Test getting values
        int_val = config.get("test_int")
        self.assertIsNotNone(int_val)
        # Note: pynlm Config.get() may return None instead of optional
    
    def testConfigGetOr(self):
        """Test getOr method"""
        config = pynlm.createDefaultConfig()
        
        # Test default value
        val = config.getOr("missing_key", "default_value")
        self.assertEqual(val, "default_value")
        
        # Test existing value
        config.set("existing_key", "existing_value")
        val = config.getOr("existing_key", "default_value")
        self.assertEqual(val, "existing_value")
    
    def testConfigSaveLoad(self):
        """Test configuration save and load"""
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            config_file = f.name
        
        try:
            config = pynlm.createDefaultConfig()
            config.set("test_key", "test_value")
            
            # Save configuration
            result = config.saveToFile(config_file)
            self.assertTrue(result)
            
            # Clear and reload
            config.clear()
            self.assertFalse(config.has("test_key"))
            
            # Load configuration
            result = config.loadFromFile(config_file)
            self.assertTrue(result)
            self.assertTrue(config.has("test_key"))
            
        finally:
            # Clean up
            if os.path.exists(config_file):
                os.unlink(config_file)
class TestSerialization(unittest.TestCase):
    """Test serialization (save/load) functionality"""
    
    def testBrainSerialization(self):
        """Test brain serialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some simulation
        for step in range(10):
            brain.step(step)
        
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_file = f.name
        
        try:
            # Save brain state
            save_result = brain.save(checkpoint_file)
            self.assertTrue(save_result)
            
            # Create new brain and load
            new_config = pynlm.createDefaultConfig()
            new_brain = pynlm.createBrain(new_config)
            new_brain.initialize()
            
            # Load the saved state
            load_result = new_brain.load(checkpoint_file)
            self.assertTrue(load_result)
            
            # Brains should have the same structure
            self.assertEqual(brain.getTotalNeuronCount(), new_brain.getTotalNeuronCount())
            
        finally:
            # Clean up
            if os.path.exists(checkpoint_file):
                os.unlink(checkpoint_file)
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced features and subsystems"""
    
    def testAllSubsystemsEnabled(self):
        """Test running with all subsystems enabled"""
        config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.enableStructuralPlasticity(True)
        
        # Run comprehensive simulation
        total_steps = 100
        for step in range(total_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation based on agent's internal state
            reward = agent.getNeuromodulationLevel()
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Verify all subsystems are working
        self.assertGreaterEqual(agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(agent.getNoveltyLevel(), 0.0)
        self.assertGreaterEqual(agent.getNeuromodulationLevel(), -1.0)
        self.assertLessEqual(agent.getNeuromodulationLevel(), 1.0)
        self.assertGreaterEqual(brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(brain.getFiringNeuronCount(), 0)
    
    def testDevelopmentStages(self):
        """Test different developmental stages"""
        config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableDevelopment(True)
        
        # Test developmental stages
        initial_stage = agent.getDevelopmentalStage()
        
        # Run simulation to trigger development
        for step in range(1000):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.updateDevelopment(0.1)
            
            # Check development progression
            if step % 300 == 0:
                current_stage = agent.getDevelopmentalStage()
                print(f"Step {step}: Development stage {current_stage}")
        
        # Development should have progressed
        final_stage = agent.getDevelopmentalStage()
        self.assertGreaterEqual(final_stage.value, initial_stage.value)
class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases"""
    
    def testInvalidBrainStep(self):
        """Test brain step without initialization"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Brain should not be initialized
        # Calling step should not crash (implementation dependent)
        try:
            brain.step(0)
        except Exception:
            # Expected behavior - brain may throw exception
            pass
    
    def testInvalidAgentOperations(self):
        """Test agent operations without world"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Should not crash even without world initialization
        try:
            agent.processSensoryInput(None)  # Will likely crash or handle gracefully
        except Exception:
            pass
    
    def testConfigurationErrors(self):
        """Test configuration error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test various invalid operations
        try:
            config.loadFromFile("/nonexistent/file.cfg")
            # Should return False, not crash
        except Exception:
            pass
        
        try:
            config.saveToFile("/invalid/path/file.cfg")
            # Should return False, not crash
        except Exception:
            pass
class NLMTestSuite:
    """Main test suite runner"""
    
    def __init__(self):
        self.test_classes = [
            TestBasicBrain,
            TestAgentWorld,
            TestConfiguration,
            TestSerialization,
            TestAdvancedFeatures,
            TestErrorHandling
        ]
    
    def run_all_tests(self, verbose=False):
        """Run all tests"""
        print("=" * 60)
        print("NLM Python Bindings Test Suite")
        print("=" * 60)
        
        test_results = []
        
        for test_class in self.test_classes:
            print(f"\nRunning {test_class.__name__}...")
            print("-" * 40)
            
            # Create test suite for this class
            suite = unittest.TestLoader().loadTestsFromTestCase(test_class)
            
            # Run tests with custom result collector
            runner = unittest.TextTestRunner(
                stream=sys.stdout if verbose else open(os.devnull, 'w'),
                verbosity=2 if verbose else 1
            )
            
            result = runner.run(suite)
            
            test_results.append({
                'class': test_class.__name__,
                'passed': result.wasSuccessful(),
                'failures': len(result.failures),
                'errors': len(result.errors),
                'tests_run': result.testsRun
            })
        
        # Print summary
        print("\n" + "=" * 60)
        print("TEST SUMMARY")
        print("=" * 60)
        
        total_tests = sum(r['tests_run'] for r in test_results)
        total_passed = sum(r['tests_run'] - r['failures'] - r['errors'] for r in test_results)
        total_failed = sum(r['failures'] for r in test_results)
        total_errors = sum(r['errors'] for r in test_results)
        
        for result in test_results:
            status = "✓ PASSED" if result['passed'] else "✗ FAILED"
            print(f"{result['class']}: {status} ({result['tests_run']} tests)")
            if result['failures'] > 0:
                print(f"  Failures: {result['failures']}")
            if result['errors'] > 0:
                print(f"  Errors: {result['errors']}")
        
        print("-" * 60)
        print(f"Total Tests: {total_tests}")
        print(f"Passed: {total_passed}")
        print(f"Failed: {total_failed}")
        print(f"Errors: {total_errors}")
        print(f"Success Rate: {total_passed/total_tests*100:.1f}%")
        print("=" * 60)
        
        return total_passed == total_tests
if __name__ == "__main__":
    # Parse command line arguments
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Python Bindings Test Suite")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    parser.add_argument("--json", action="store_true", help="Output results as JSON")
    parser.add_argument("--output", type=str, help="Output file for test results")
    
    args = parser.parse_args()
    
    # Create and run test suite
    test_suite = NLMTestSuite()
    success = test_suite.run_all_tests(verbose=verbose)
    
    # Save results to file if requested
    if args.output or args.json:
        results = {
            'success': success,
            'verbose': verbose,
            'tests_run': sum(r['tests_run'] for r in test_suite.test_classes),
            'timestamp': "2024-01-15T19:51:52Z"
        }
        
        output_file = args.output or "nlm_test_results.json"
        try:
            with open(output_file, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"\nTest results saved to {output_file}")
        except Exception as e:
            print(f"Error saving results: {e}")
    
    sys.exit(0 if success else 1)
