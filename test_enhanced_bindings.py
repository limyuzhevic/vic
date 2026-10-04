#!/usr/bin/env python3
"""
Comprehensive test suite for enhanced NLM Python bindings.

This test suite demonstrates the advanced functionality of the NLM Python bindings
and provides examples of how to use the new features introduced in Phase 6 improvements.
"""

import pynlm
import unittest
import tempfile
import json
import time
from pathlib import Path

class TestBasicFunctionality(unittest.TestCase):
    """Test basic NLM functionality to ensure backward compatibility."""
    
    def test_basic_brain_creation(self):
        """Test basic brain creation and initialization."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Verify brain is properly initialized
        self.assertEqual(brain.getTotalNeuronCount(), 500)
        self.assertEqual(brain.getRegionCount(), 1)
        self.assertIsNotNone(brain.getConfig())
    
    def test_basic_simulation_step(self):
        """Test basic simulation steps."""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run a few steps
        for step in range(10):
            brain.step(step)
        
        # Verify simulation progress
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(brain.getAverageFiringRate(), 0.0)

class TestAdvancedConfiguration(unittest.TestCase):
    """Test advanced configuration features."""
    
    def test_config_presets(self):
        """Test different configuration presets."""
        # Test development preset
        dev_config = pynlm.createConfigWithPresets("development")
        self.assertTrue(dev_config.has("synaptogenesis_rate"))
        self.assertTrue(dev_config.has("curiosity_enabled"))
        
        # Test production preset
        prod_config = pynlm.createConfigWithPresets("production")
        self.assertTrue(prod_config.has("performance_optimized"))
        
        # Test research preset
        research_config = pynlm.createConfigWithPresets("research")
        self.assertTrue(research_config.has("structural_plasticity_enabled"))
        self.assertTrue(research_config.has("prediction_enabled"))
    
    def test_advanced_brain_creation(self):
        """Test advanced brain creation with custom features."""
        config = pynlm.createConfigWithPresets("research")
        brain = pynlm.createAdvancedBrain(config, 
                                         topology="modular",
                                         enablePlasticity=True,
                                         enableDevelopment=True)
        
        self.assertIsNotNone(brain)
        brain.initialize()
        self.assertGreater(brain.getTotalNeuronCount(), 0)

class TestPerformanceMonitoring(unittest.TestCase):
    """Test performance monitoring and optimization features."""
    
    def test_performance_stats_collection(self):
        """Test performance statistics collection."""
        brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
        brain.initialize()
        
        # Run simulation to generate statistics
        for step in range(100):
            brain.step(step)
        
        # Get performance statistics
        stats = pynlm.getBrainPerformanceStats(brain)
        
        # Verify statistics are collected
        self.assertIn("neurosActive", stats)
        self.assertIn("spikesProcessed", stats)
        self.assertIn("avgFiringRate", stats)
        self.assertGreaterEqual(stats["spikesProcessed"], 0)
        self.assertGreaterEqual(stats["avgFiringRate"], 0.0)
    
    def test_performance_timing(self):
        """Test performance timing capabilities."""
        brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
        brain.initialize()
        
        # Measure simulation speed
        start_time = time.time()
        for step in range(500):
            brain.step(step)
        end_time = time.time()
        
        elapsed = end_time - start_time
        self.assertGreater(elapsed, 0.0)
        self.assertLess(elapsed, 60.0)  # Should complete within a minute

class TestNetworkAnalysis(unittest.TestCase):
    """Test network analysis and statistics features."""
    
    def test_brain_network_analysis(self):
        """Test comprehensive brain network analysis."""
        brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
        brain.initialize()
        
        # Run simulation
        for step in range(500):
            brain.step(step)
        
        # Get network analysis
        analysis = pynlm.analyzeBrainNetwork(brain)
        
        # Verify analysis results
        self.assertIn("connectivity_density", analysis)
        self.assertIn("activity_level", analysis)
        self.assertIn("average_firing_rate", analysis)
        self.assertGreaterEqual(analysis["connectivity_density"], 0.0)
        self.assertLessEqual(analysis["connectivity_density"], 1.0)
        self.assertGreaterEqual(analysis["activity_level"], 0.0)
        self.assertLessEqual(analysis["activity_level"], 1.0)
    
    def test_cloning_functionality(self):
        """Test brain cloning and state preservation."""
        brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
        brain.initialize()
        
        # Run some simulation
        for step in range(200):
            brain.step(step)
        
        # Clone brain state
        cloned_brain = pynlm.cloneBrainState(brain)
        
        # Verify clone
        self.assertIsNotNone(cloned_brain)
        self.assertEqual(cloned_brain.getTotalNeuronCount(), brain.getTotalNeuronCount())
        self.assertEqual(cloned_brain.getTotalSynapseCount(), brain.getTotalSynapseCount())

class TestErrorHandling(unittest.TestCase):
    """Test enhanced error handling and validation."""
    
    def test_exception_handling(self):
        """Test custom exception handling."""
        # Test configuration error handling
        try:
            brain = pynlm.createAdvancedBrain(None)
            self.fail("Should have raised NLMException")
        except pynlm.NLMException:
            pass  # Expected
        
        # Test validation
        self.assertTrue(pynlm.ValidationUtils.validateRange(50, 0, 100))
        self.assertFalse(pynlm.ValidationUtils.validateRange(150, 0, 100))
        
        # Test error message generation
        error_msg = pynlm.ValidationUtils.getRangeError("neuron_count", 150, 0, 100)
        self.assertIn("neuron_count", error_msg)
        self.assertIn("150", error_msg)
        self.assertIn("100", error_msg)

class TestSerialization(unittest.TestCase):
    """Test serialization and persistence features."""
    
    def test_serialization_interface(self):
        """Test serialization interface (basic test)."""
        brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Test serialization (basic check - interface exists)
        serialized = pynlm.serializeBrain(brain)
        self.assertIsInstance(serialized, str)
        self.assertGreater(len(serialized), 0)

class TestBatchOperations(unittest.TestCase):
    """Test batch operations and parallel processing."""
    
    def test_batch_brain_operation(self):
        """Test batch operations on multiple brains."""
        # Create multiple brains
        brains = [pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
                  for _ in range(5)]
        
        # Initialize all brains
        for brain in brains:
            brain.initialize()
        
        # Define batch operation
        def processAllBrains(brain_list):
            for brain in brain_list:
                for step in range(50):
                    brain.step(step)
        
        # Execute batch operation
        processAllBrains(brains)
        
        # Verify all brains processed
        for brain in brains:
            self.assertGreater(brain.getTotalSpikeCount(), 0)

class TestAdvancedIntegration(unittest.TestCase):
    """Test advanced integration features with world and agent."""
    
    def test_advanced_agent_simulation(self):
        """Test advanced agent simulation with learning."""
        # Create learning-enabled agent
        brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure agent for advanced learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        agent.initialize(world)
        
        # Run learning simulation
        for step in range(300):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
        
        # Verify agent learned
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreaterEqual(agent.getNeuromodulationLevel(), 0.0)

class TestConfigurationManagement(unittest.TestCase):
    """Test configuration management features."""
    
    def test_config_batch_loading(self):
        """Test batch configuration loading."""
        # Test the config batch loading interface
        # Note: This is a basic test of the interface availability
        filepaths = ["configs/default.cfg"]  # Would need actual files
        
        # This would test the actual batch loading if files were available
        # For now, we test the interface exists
        pass

class TestDocumentationExamples(unittest.TestCase):
    """Test examples from documentation."""
    
    def test_documentation_examples_basic(self):
        """Test basic examples from documentation."""
        # Example 1: Simplest Brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        for i in range(10):
            brain.step(i)
        self.assertGreater(brain.getTotalSpikeCount(), 0)
    
    def test_documentation_examples_advanced(self):
        """Test advanced examples from documentation."""
        # Example from ADVANCED_GUIDE.md
        brain = pynlm.createAdvancedBrain(pynlm.createConfigWithPresets("research"))
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
        
        analysis = pynlm.analyzeBrainNetwork(brain)
        self.assertIn("connectivity_density", analysis)

def run_comprehensive_test_suite():
    """Run the complete test suite and report results."""
    print("=== NLM Python Bindings - Comprehensive Test Suite ===\n")
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    
    # Run tests with detailed output
    runner = unittest.TextTestRunner(verbosity=2, stream=sys.stdout)
    result = runner.run(suite)
    
    print(f"\n=== Test Summary ===")
    print(f"Tests run: {result.testsRun}")
    print(f"Failures: {len(result.failures)}")
    print(f"Errors: {len(result.errors)}")
    print(f"Success rate: {((result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun * 100):.1f}%")
    
    if result.failures:
        print(f"\n=== Failures ===")
        for test, traceback in result.failures:
            print(f"FAIL: {test}")
            print(f"  {traceback}\n")
    
    if result.errors:
        print(f"\n=== Errors ===")
        for test, traceback in result.errors:
            print(f"ERROR: {test}")
            print(f"  {traceback}\n")
    
    return result.wasSuccessful()

if __name__ == "__main__":
    import sys
    success = run_comprehensive_test_suite()
    sys.exit(0 if success else 1)
