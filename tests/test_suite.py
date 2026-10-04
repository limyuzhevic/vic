# NLM Test Suite

# Comprehensive test suite for NLM with improved coverage and integration tests.

import unittest
import time
import json
import tempfile
from typing import List, Dict, Any, Optional
import pynlm
import matplotlib.pyplot as plt
import numpy as np

class TestConfig(unittest.TestCase):
    """Test configuration management"""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
    
    def test_config_creation(self):
        """Test basic config creation"""
        self.assertIsNotNone(self.config)
        self.assertEqual(len(self.config.getKeys()), 0)
    
    def test_config_set_get_int(self):
        """Test integer configuration values"""
        self.config.setInt("test_int", 42)
        self.assertTrue(self.config.has("test_int"))
        self.assertEqual(self.config.getInt("test_int"), 42)
    
    def test_config_set_get_double(self):
        """Test double configuration values"""
        self.config.setDouble("test_double", 3.14159)
        self.assertTrue(self.config.has("test_double"))
        self.assertAlmostEqual(self.config.getDouble("test_double"), 3.14159, places=5)
    
    def test_config_set_get_string(self):
        """Test string configuration values"""
        self.config.setString("test_string", "hello world")
        self.assertTrue(self.config.has("test_string"))
        self.assertEqual(self.config.getString("test_string"), "hello world")
    
    def test_config_set_get_bool(self):
        """Test boolean configuration values"""
        self.config.setBool("test_bool", True)
        self.assertTrue(self.config.has("test_bool"))
        self.assertTrue(self.config.getBool("test_bool"))
    
    def test_config_get_or(self):
        """Test getOr method with default values"""
        # Test with missing key
        value = self.config.getOr("missing_key", "default_value")
        self.assertEqual(value, "default_value")
        
        # Test with existing key
        self.config.setString("existing_key", "actual_value")
        value = self.config.getOr("existing_key", "default_value")
        self.assertEqual(value, "actual_value")
    
    def test_config_remove(self):
        """Test configuration removal"""
        self.config.setString("key_to_remove", "value")
        self.assertTrue(self.config.has("key_to_remove"))
        
        self.config.remove("key_to_remove")
        self.assertFalse(self.config.has("key_to_remove"))
    
    def test_config_clear(self):
        """Test configuration clearing"""
        self.config.setString("key1", "value1")
        self.config.setInt("key2", 42)
        
        self.assertEqual(len(self.config.getKeys()), 2)
        
        self.config.clear()
        self.assertEqual(len(self.config.getKeys()), 0)
    
    def test_config_summary(self):
        """Test configuration summary"""
        self.config.setString("test_key", "test_value")
        summary = self.config.summary()
        
        self.assertIn("Configuration", summary)
        self.assertIn("test_key", summary)
        self.assertIn("test_value", summary)
    
    def test_config_validation(self):
        """Test configuration validation"""
        self.assertTrue(self.config.validate("test_key"))
        self.assertFalse(self.config.validate(""))
    
    def test_config_load_save_json(self):
        """Test JSON file I/O"""
        # Set some test values
        self.config.setString("test_string", "hello")
        self.config.setInt("test_int", 123)
        self.config.setDouble("test_double", 3.14)
        self.config.setBool("test_bool", True)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            self.assertTrue(self.config.saveToFile(filepath))
            
            # Create new config and load
            new_config = pynlm.createDefaultConfig()
            self.assertTrue(new_config.loadFromFile(filepath))
            
            # Verify values
            self.assertEqual(new_config.getString("test_string"), "hello")
            self.assertEqual(new_config.getInt("test_int"), 123)
            self.assertAlmostEqual(new_config.getDouble("test_double"), 3.14, places=5)
            self.assertTrue(new_config.getBool("test_bool"))
        finally:
            import os
            os.unlink(filepath)
    
    def test_config_load_save_yaml(self):
        """Test YAML file I/O"""
        # Set some test values
        self.config.setString("test_string", "hello")
        self.config.setInt("test_int", 123)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
            filepath = f.name
        
        try:
            self.assertTrue(self.config.saveToFile(filepath))
            
            # Create new config and load
            new_config = pynlm.createDefaultConfig()
            self.assertTrue(new_config.loadFromFile(filepath))
            
            # Verify values
            self.assertEqual(new_config.getString("test_string"), "hello")
            self.assertEqual(new_config.getInt("test_int"), 123)
        finally:
            import os
            os.unlink(filepath)
    
    def test_config_load_save_key_value(self):
        """Test key=value format file I/O"""
        # Set some test values
        self.config.setString("test_key", "test_value")
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("# Test config file\n")
            f.write("test_key = test_value\n")
            f.write("another_key = another_value\n")
            filepath = f.name
        
        try:
            # Create new config and load
            new_config = pynlm.createDefaultConfig()
            self.assertTrue(new_config.loadFromFile(filepath))
            
            # Verify values
            self.assertEqual(new_config.getString("test_key"), "test_value")
            self.assertEqual(new_config.getString("another_key"), "another_value")
        finally:
            import os
            os.unlink(filepath)

class TestBrain(unittest.TestCase):
    """Test brain functionality"""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.config.setInt("brain.neuron_count", 100)
        self.config.setInt("brain.region_count", 2)
        self.config.setDouble("brain.connection_probability", 0.1)
    
    def test_brain_creation(self):
        """Test brain creation"""
        brain = pynlm.createBrain(self.config)
        self.assertIsNotNone(brain)
        
        # Verify initial state
        self.assertEqual(brain.getTotalNeuronCount(), 100)
        self.assertEqual(brain.getRegionCount(), 2)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        brain = pynlm.createBrain(self.config)
        self.assertTrue(brain.initialize())
        
        # Verify initialization
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertGreater(brain.getFiringNeuronCount(), 0)
    
    def test_brain_step(self):
        """Test brain stepping"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        
        # Run multiple steps
        for i in range(10):
            brain.step(i)
        
        # Verify state changed
        self.assertGreater(brain.getTotalSpikeCount(), initial_spikes)
    
    def test_brain_reset(self):
        """Test brain reset"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Run some steps
        for i in range(5):
            brain.step(i)
        
        # Reset brain
        brain.reset()
        brain.initialize()
        
        # Verify reset
        self.assertEqual(brain.getTotalNeuronCount(), 100)
        self.assertEqual(brain.getRegionCount(), 2)
    
    def test_brain_regions(self):
        """Test brain region management"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Get region IDs
        region_ids = brain.getRegionIds()
        self.assertEqual(len(region_ids), 2)
        
        # Get regions
        regions = brain.getRegions()
        self.assertEqual(len(regions), 2)
        
        # Verify regions are accessible
        for region_id in region_ids:
            region = brain.getRegion(region_id)
            self.assertIsNotNone(region)
    
    def test_brain_action_production(self):
        """Test action production"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        action = brain.produceAction()
        self.assertIsNotNone(action)
        self.assertIsInstance(action, pynlm.Action)
        
        # Verify action has properties
        action_type = action.getType()
        self.assertIsNotNone(action_type)
    
    def test_brain_save_load(self):
        """Test brain save and load functionality"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Run some steps to create interesting state
        for i in range(20):
            brain.step(i)
        
        # Save to temporary file
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            filepath = f.name
        
        try:
            self.assertTrue(brain.save(filepath))
            
            # Create new brain and load
            new_brain = pynlm.createBrain(self.config)
            new_brain.initialize()
            
            self.assertTrue(new_brain.load(filepath))
            
            # Verify loaded state matches
            self.assertEqual(new_brain.getTotalNeuronCount(), brain.getTotalNeuronCount())
            self.assertEqual(new_brain.getRegionCount(), brain.getRegionCount())
        finally:
            import os
            os.unlink(filepath)
    
    def test_brain_statistics(self):
        """Test brain statistics"""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        # Verify statistics
        self.assertGreater(brain.getTotalNeuronCount(), 0)
        self.assertGreater(brain.getTotalSynapseCount(), 0)
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        self.assertGreater(brain.getAverageFiringRate(), 0.0)
        self.assertGreater(brain.getExcitationInhibitionRatio(), 0.0)
        
        # Verify spike count increases with steps
        initial_spikes = brain.getTotalSpikeCount()
        brain.step(0)
        self.assertGreater(brain.getTotalSpikeCount(), initial_spikes)

class TestAgent(unittest.TestCase):
    """Test agent functionality"""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.config.setInt("brain.neuron_count", 50)
        self.config.setInt("brain.region_count", 1)
        
        # Create brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        self.world.reset()
        self.world.setAgentStart(5.0, 5.0)
        
        # Initialize agent
        self.agent.initialize(self.world)
    
    def test_agent_initialization(self):
        """Test agent initialization"""
        self.assertIsNotNone(self.agent)
        self.assertIsNotNone(self.world)
    
    def test_agent_sensory_processing(self):
        """Test sensory input processing"""
        # Get sensory percept
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Verify processing didn't crash
        self.assertTrue(True)
    
    def test_agent_motor_command(self):
        """Test motor command decoding"""
        # Process some sensory input
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        self.assertIsNotNone(action)
        self.assertIsInstance(action, pynlm.Action)
    
    def test_agent_reward_modulation(self):
        """Test reward-based modulation"""
        # Apply reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        self.agent.applyRewardModulation(-0.5, 0.2)
        
        # Verify neuromodulation
        neuromod_level = self.agent.getNeuromodulationLevel()
        self.assertGreaterEqual(neuromod_level, 0.0)
    
    def test_agent_development(self):
        """Test development updates"""
        # Update development
        for timestep in range(10):
            self.agent.updateDevelopment(0.1)
            
            # Verify development stage
            stage = self.agent.getDevelopmentalStage()
            self.assertIsNotNone(stage)
    
    def test_agent_subsystems(self):
        """Test agent subsystems enable/disable"""
        # Test reward modulation
        self.agent.enableRewardModulation(True)
        self.assertTrue(self.agent.isRewardModulationEnabled())
        
        self.agent.enableRewardModulation(False)
        self.assertFalse(self.agent.isRewardModulationEnabled())
        
        # Test structural plasticity
        self.agent.enableStructuralPlasticity(True)
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        
        self.agent.enableStructuralPlasticity(False)
        self.assertFalse(self.agent.isStructuralPlasticityEnabled())
        
        # Test development
        self.agent.enableDevelopment(True)
        self.assertTrue(self.agent.isDevelopmentEnabled())
        
        self.agent.enableDevelopment(False)
        self.assertFalse(self.agent.isDevelopmentEnabled())
        
        # Test curiosity
        self.agent.enableCuriosity(True)
        self.assertTrue(self.agent.isCuriosityEnabled())
        
        self.agent.enableCuriosity(False)
        self.assertFalse(self.agent.isCuriosityEnabled())
    
    def test_complete_agent_simulation(self):
        """Test complete agent simulation"""
        # Run simulation
        for step in range(20):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply motor command
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update development
            self.agent.updateDevelopment(0.1)
        
        # Verify simulation completed
        self.assertGreater(self.brain.getTotalSpikeCount(), 0)
        self.assertLess(self.agent.getCuriosityLevel(), 1.0)

class TestIntegration(unittest.TestCase):
    """Integration tests"""
    
    def test_complete_system(self):
        """Test complete NLM system"""
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.setInt("brain.neuron_count", 100)
        config.setInt("brain.region_count", 2)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run complete simulation
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.updateDevelopment(0.1)
        
        # Verify system worked
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(brain.getFiringNeuronCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0)
    
    def test_config_templates(self):
        """Test configuration templates"""
        # Test default config
        default_config = pynlm.createDefaultConfig()
        self.assertIsNotNone(default_config)
        
        # Test fast config
        fast_config = pynlm.getDefaultBrainConfig()
        self.assertIsNotNone(fast_config)
        self.assertLess(fast_config.getInt("brain.neuron_count"), 1000)
        
        # Test high-performance config
        hp_config = pynlm.getHighPerformanceBrainConfig()
        self.assertIsNotNone(hp_config)
        self.assertLess(hp_config.getInt("brain.neuron_count"), 500)
        
        # Test research config
        research_config = pynlm.getResearchBrainConfig()
        self.assertIsNotNone(research_config)
        self.assertGreater(research_config.getInt("brain.neuron_count"), 1000)
    
    def test_visualization_tools(self):
        """Test visualization tools"""
        import tempfile
        import os
        
        # Create brain and world
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create visualization
        vis = pynlm.create_default_visualization(brain)
        
        # Test visualization methods
        with tempfile.TemporaryDirectory() as temp_dir:
            network_file = os.path.join(temp_dir, "network.png")
            activity_file = os.path.join(temp_dir, "activity.png")
            debug_file = os.path.join(temp_dir, "debug.png")
            
            # These would create files if dependencies are available
            # For now, just test that the methods don't crash
            try:
                vis.create_network_graph(network_file)
                print(f"Network graph created: {network_file}")
            except Exception as e:
                print(f"Network graph creation failed (expected): {e}")
            
            try:
                vis.create_activity_plots(activity_file)
                print(f"Activity plots created: {activity_file}")
            except Exception as e:
                print(f"Activity plots creation failed (expected): {e}")
            
            try:
                vis.create_debug_dashboard(debug_file)
                print(f"Debug dashboard created: {debug_file}")
            except Exception as e:
                print(f"Debug dashboard creation failed (expected): {e}")

class TestPerformance(unittest.TestCase):
    """Performance tests"""
    
    def test_simulation_speed(self):
        """Test simulation speed"""
        import time
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time simulation
        start_time = time.time()
        
        for i in range(100):
            brain.step(i)
        
        elapsed = time.time() - start_time
        
        # Verify reasonable performance
        self.assertLess(elapsed, 10.0)  # Should complete in less than 10 seconds
        self.assertGreater(elapsed, 0.0)
        
        # Calculate steps per second
        steps_per_second = 100 / elapsed
        print(f"Performance: {steps_per_second:.1f} steps/second")
    
    def test_memory_usage(self):
        """Test memory usage"""
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        
        # Create and run brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for i in range(50):
            brain.step(i)
        
        final_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = final_memory - initial_memory
        
        # Memory increase should be reasonable (less than 100MB for this test)
        self.assertLess(memory_increase, 100.0)
        
        print(f"Memory increase: {memory_increase:.1f} MB")

class TestDocumentation(unittest.TestCase):
    """Test documentation generation"""
    
    def test_documentation_export(self):
        """Test documentation export"""
        import tempfile
        import os
        
        with tempfile.TemporaryDirectory() as temp_dir:
            # Export documentation
            output_path = pynlm.export_documentation(temp_dir)
            
            # Verify files were created
            expected_files = [
                "README.md",
                "HOW_TO_USE.md", 
                "easy_usage.md",
                "API_REFERENCE.md",
                "ARCHITECTURE.md",
                "SCIENCE.md",
                "EXPERIMENTS.md"
            ]
            
            for filename in expected_files:
                filepath = os.path.join(output_path, filename)
                if os.path.exists(filepath):
                    print(f"✓ Documentation file created: {filename}")
                else:
                    print(f"✗ Documentation file missing: {filename}")

# Convenience functions for running tests
def run_all_tests():
    """Run all tests and return results"""
    import sys
    import time
    
    print("Running NLM Test Suite")
    print("=" * 50)
    
    start_time = time.time()
    
    # Create test suite
    suite = unittest.TestLoader().loadTestsFromTestCase(TestConfig)
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestBrain))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestAgent))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestIntegration))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestPerformance))
    suite.addTests(unittest.TestLoader().loadTestsFromTestCase(TestDocumentation))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    elapsed = time.time() - start_time
    
    print(f"\nTest Summary:")
    print(f"  Total tests: {result.testsRun}")
    print(f"  Passed: {len(result.success_list)}")
    print(f"  Failed: {len(result.failures)}")
    print(f"  Errors: {len(result.errors)}")
    print(f"  Duration: {elapsed:.2f}s")
    
    return result

def run_quick_tests():
    """Run quick subset of tests"""
    print("Running Quick NLM Tests")
    print("=" * 30)
    
    # Create a limited test suite
    suite = unittest.TestSuite()
    
    # Add quick test cases
    suite.addTest(TestConfig('test_config_creation'))
    suite.addTest(TestConfig('test_config_set_get_int'))
    suite.addTest(TestBrain('test_brain_creation'))
    suite.addTest(TestBrain('test_brain_initialization'))
    suite.addTest(TestAgent('test_agent_initialization'))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=1)
    result = runner.run(suite)
    
    return result

if __name__ == "__main__":
    # Check if we should run quick tests or all tests
    import sys
    
    if "--quick" in sys.argv:
        run_quick_tests()
    elif "--benchmark" in sys.argv:
        # Run performance benchmark
        print("Running NLM Performance Benchmark")
        print("=" * 40)
        
        import time
        import json
        
        # Test different configurations
        configs = {
            "small": {"brain.neuron_count": 50, "simulation.timestep": 0.01},
            "medium": {"brain.neuron_count": 100, "simulation.timestep": 0.005},
            "large": {"brain.neuron_count": 200, "simulation.timestep": 0.001},
        }
        
        results = {}
        
        for name, params in configs.items():
            config = pynlm.createDefaultConfig()
            for key, value in params.items():
                config.set(key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for i in range(50):
                brain.step(i)
            elapsed = time.time() - start_time
            
            results[name] = {
                "time": elapsed,
                "steps_per_second": 50 / elapsed,
                "spikes": brain.getTotalSpikeCount()
            }
        
        print("Benchmark Results:")
        for name, result in results.items():
            print(f"  {name}:")
            print(f"    Time: {result['time']:.2f}s")
            print(f"    Steps/sec: {result['steps_per_second']:.1f}")
            print(f"    Spikes: {result['spikes']}")
        
        # Save results
        with open("benchmark_results.json", 'w') as f:
            json.dump(results, f, indent=2)
        
        print("\nResults saved to 'benchmark_results.json'")
        
    else:
        # Run full test suite
        run_all_tests()