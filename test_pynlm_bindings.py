#!/usr/bin/env python3
"""
Python tests for NLM bindings

This file contains comprehensive tests for the enhanced Python bindings
including error handling, advanced features, and expert functionality.
"""

import pynlm
import unittest
import tempfile
import os
import sys
from pathlib import Path

# Add the current directory to the Python path for testing
sys.path.insert(0, str(Path(__file__).parent))


class TestPyNLMBasic(unittest.TestCase):
    """Test basic NLM Python bindings functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def tearDown(self):
        """Clean up test fixtures"""
        del self.brain
        del self.config
    
    def test_brain_creation(self):
        """Test basic brain creation"""
        self.assertIsNotNone(self.brain)
        self.assertEqual(self.brain.getTotalNeuronCount(), 1000)
        self.assertEqual(self.brain.getTotalSynapseCount(), 50)
    
    def test_brain_initialization(self):
        """Test brain initialization"""
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
        self.assertTrue(self.brain.getTotalSynapseCount() >= 0)
    
    def test_brain_step(self):
        """Test brain simulation step"""
        initial_neurons = self.brain.getTotalNeuronCount()
        initial_spikes = self.brain.getTotalSpikeCount()
        
        for i in range(10):
            self.brain.step(i)
        
        final_neurons = self.brain.getTotalNeuronCount()
        final_spikes = self.brain.getTotalSpikeCount()
        
        self.assertEqual(initial_neurons, final_neurons)
        self.assertGreaterEqual(final_spikes, initial_spikes)
    
    def test_brain_reset(self):
        """Test brain reset functionality"""
        self.brain.step(0)
        self.brain.reset()
        
        # After reset, basic properties should remain the same
        self.assertEqual(self.brain.getTotalNeuronCount(), 1000)
        self.assertEqual(self.brain.getTotalSynapseCount(), 50)


class TestPyNLMErrorHandling(unittest.TestCase):
    """Test error handling in NLM Python bindings"""
    
    def test_config_error_handling(self):
        """Test configuration error handling"""
        config = pynlm.createDefaultConfig()
        
        # Test valid configuration
        config.set("brain.neuron_count", 500)
        self.assertEqual(config.has("brain.neuron_count"), True)
        
        # Test configuration validation
        with self.assertRaises(pynlm.ConfigError):
            config.set("invalid.key", "invalid")
        
        # Test file operations
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"test": "data"}')
            temp_file = f.name
        
        try:
            config.loadFromFile(temp_file)
            self.assertTrue(config.has("test"))
        finally:
            os.unlink(temp_file)
        
        # Test save functionality
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            
            # Verify file was created and can be loaded
            config2 = pynlm.createDefaultConfig()
            config2.loadFromFile(f.name)
            self.assertEqual(config2.getKeys(), ["test"])
            
            os.unlink(f.name)
    
    def test_brain_error_handling(self):
        """Test brain error handling"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test step with valid parameters
        brain.step(0)
        self.assertTrue(brain.getTotalNeuronCount() > 0)
        
        # Test reset
        brain.reset()
        self.assertEqual(brain.getTotalNeuronCount(), 1000)
        
        # Test state persistence
        brain.step(1)
        # Save and load test would require file operations


class TestPyNLMWorld(unittest.TestCase):
    """Test world-related functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
    
    def tearDown(self):
        """Clean up test fixtures"""
        del self.world
    
    def test_world_creation(self):
        """Test world creation and configuration"""
        self.assertEqual(self.world.getWidth(), 20)
        self.assertEqual(self.world.getHeight(), 20)
        self.assertEqual(self.world.getSimulationTime(), 0.0)
    
    def test_world_update(self):
        """Test world update functionality"""
        initial_time = self.world.getSimulationTime()
        
        for i in range(5):
            self.world.update(0.1)
        
        final_time = self.world.getSimulationTime()
        self.assertGreater(final_time, initial_time)
        self.assertEqual(final_time, initial_time + 0.5)
    
    def test_world_sensory_percept(self):
        """Test sensory percept functionality"""
        percept = self.world.getSensoryPercept()
        self.assertIsNotNone(percept)
        
        # Test vision data
        vision = percept.getVision()
        self.assertIsNotNone(vision)
        self.assertEqual(vision.getWidth(), 8)
        self.assertEqual(vision.getHeight(), 8)
        
        # Test internal signals
        internal = percept.getInternal()
        self.assertIsNotNone(internal)
    
    def test_world_motor_command(self):
        """Test motor command functionality"""
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        initial_energy = self.world.getAgentBody().energy
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        final_energy = self.world.getAgentBody().energy
        self.assertLessEqual(final_energy, initial_energy)


class TestPyNLMAdvancedFeatures(unittest.TestCase):
    """Test advanced features for expert users"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def tearDown(self):
        """Clean up test fixtures"""
        del self.agent
        del self.world
        del self.brain
        del self.config
    
    def test_agent_brain_functionality(self):
        """Test agent brain functionality"""
        # Test sensory processing
        percept = self.world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        
        # Test motor decoding
        action = self.agent.decodeMotorCommand()
        self.assertIsNotNone(action)
        self.assertIsInstance(action.getType(), pynlm.ActionType)
        
        # Test reward modulation
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Test development
        self.agent.updateDevelopment(0.1)
    
    def test_advanced_learning_features(self):
        """Test advanced learning features"""
        # Enable advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Verify features are enabled
        self.assertTrue(self.agent.isRewardModulationEnabled())
        self.assertTrue(self.agent.isStructuralPlasticityEnabled())
        self.assertTrue(self.agent.isDevelopmentEnabled())
        self.assertTrue(self.agent.isCuriosityEnabled())
        
        # Test neuromodulation levels
        neuromod_level = self.agent.getNeuromodulationLevel()
        self.assertIsInstance(neuromod_level, float)
        
        curiosity_level = self.agent.getCuriosityLevel()
        self.assertIsInstance(curiosity_level, float)
        
        novelty_level = self.agent.getNoveltyLevel()
        self.assertIsInstance(novelty_level, float)
        
        prediction_error = self.agent.getPredictionError()
        self.assertIsInstance(prediction_error, float)
    
    def test_complete_agent(self):
        """Test CompleteAgent functionality"""
        try:
            # Try to use CompleteAgent if available
            agent = pynlm.CompleteAgent()
            self.assertIsNotNone(agent)
        except AttributeError:
            # CompleteAgent might not be available in all installations
            self.skipTest("CompleteAgent not available")
    
    def test_checkpoint_manager(self):
        """Test CheckpointManager functionality"""
        try:
            # Try to use CheckpointManager if available
            checkpoint_manager = pynlm.CheckpointManager("test_checkpoints")
            self.assertIsNotNone(checkpoint_manager)
        except AttributeError:
            # CheckpointManager might not be available
            self.skipTest("CheckpointManager not available")
    
    def test_network_analyzer(self):
        """Test NetworkAnalyzer functionality"""
        try:
            # Try to use NetworkAnalyzer if available
            analyzer = pynlm.NetworkAnalyzer(self.brain)
            self.assertIsNotNone(analyzer)
            
            # Test basic network analysis
            connectivity_matrix = analyzer.calculate_connectivity_matrix()
            self.assertIsNotNone(connectivity_matrix)
            
            topology = analyzer.analyze_network_topology()
            self.assertIsNotNone(topology)
        except AttributeError:
            # NetworkAnalyzer might not be available
            self.skipTest("NetworkAnalyzer not available")
    
    def test_learning_curve_analyzer(self):
        """Test LearningCurveAnalyzer functionality"""
        try:
            # Try to use LearningCurveAnalyzer if available
            analyzer = pynlm.LearningCurveAnalyzer()
            self.assertIsNotNone(analyzer)
        except AttributeError:
            # LearningCurveAnalyzer might not be available
            self.skipTest("LearningCurveAnalyzer not available")
    
    def test_development_stage_analyzer(self):
        """Test DevelopmentStageAnalyzer functionality"""
        try:
            # Try to use DevelopmentStageAnalyzer if available
            analyzer = pynlm.DevelopmentStageAnalyzer()
            self.assertIsNotNone(analyzer)
        except AttributeError:
            # DevelopmentStageAnalyzer might not be available
            self.skipTest("DevelopmentStageAnalyzer not available")
    
    def test_brain_profiler(self):
        """Test BrainProfiler functionality"""
        try:
            # Try to use BrainProfiler if available
            profiler = pynlm.BrainProfiler(self.brain)
            self.assertIsNotNone(profiler)
            
            # Test basic profiling
            profiler.start()
            
            for step in range(10):
                self.brain.step(step)
            
            profiler.stop()
            
            # Test performance analysis
            report = profiler.generate_report()
            self.assertIsNotNone(report)
            
            optimization_suggestions = profiler.generate_optimization_report()
            self.assertIsNotNone(optimization_suggestions)
            
        except AttributeError:
            # BrainProfiler might not be available
            self.skipTest("BrainProfiler not available")
    
    def test_memory_monitor(self):
        """Test MemoryMonitor functionality"""
        try:
            # Try to use MemoryMonitor if available
            monitor = pynlm.MemoryMonitor(self.brain)
            self.assertIsNotNone(monitor)
            
            # Test memory monitoring
            monitor.start_monitoring()
            
            for step in range(10):
                self.brain.step(step)
            
            monitor.stop_monitoring()
            
            # Test memory analysis
            peak_memory = monitor.get_peak_memory_usage()
            self.assertIsInstance(peak_memory, (int, float))
            
            current_memory = monitor.get_current_memory_usage()
            self.assertIsInstance(current_memory, (int, float))
            
            optimization_suggestions = monitor.get_memory_optimization_suggestions()
            self.assertIsNotNone(optimization_suggestions)
            
        except AttributeError:
            # MemoryMonitor might not be available
            self.skipTest("MemoryMonitor not available")


class TestPyNLMConfiguration(unittest.TestCase):
    """Test configuration utilities"""
    
    def test_config_validation(self):
        """Test configuration validation"""
        try:
            validator = pynlm.ConfigValidator()
            self.assertIsNotNone(validator)
            
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            
            is_valid, issues, suggestions = validator.validate(config)
            self.assertIsInstance(is_valid, bool)
            self.assertIsInstance(issues, list)
            self.assertIsInstance(suggestions, list)
            
        except AttributeError:
            # ConfigValidator might not be available
            self.skipTest("ConfigValidator not available")


class TestPyNLMExpertUsage(unittest.TestCase):
    """Test expert usage patterns"""
    
    def test_complete_agent_example(self):
        """Test complete agent example from expert guide"""
        try:
            # This test would use the complete agent example from expert_usage_guide.md
            agent = pynlm.CompleteAgent()
            self.assertIsNotNone(agent)
            
            # Test agent with world
            world = agent.get_world()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            # Test agent initialization
            agent.initialize()
            
            # Run simulation
            for step in range(10):
                world.update(0.1)
                agent.run_step()
            
            # Get analytics
            analytics = agent.get_analytics()
            self.assertIsNotNone(analytics)
            
        except AttributeError:
            # CompleteAgent might not be available
            self.skipTest("CompleteAgent not available for expert test")
    
    def test_advanced_analysis(self):
        """Test advanced analysis functions"""
        try:
            # Create brain and run simulation
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            for step in range(50):
                brain.step(step)
            
            # Test advanced analysis
            analysis_results = {
                'basic_statistics': {
                    'total_neurons': brain.getTotalNeuronCount(),
                    'total_synapses': brain.getTotalSynapseCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'developmental_stage': brain.getDevelopmentalStage()
                }
            }
            
            self.assertIsInstance(analysis_results['basic_statistics']['total_neurons'], int)
            self.assertIsInstance(analysis_results['basic_statistics']['firing_rate'], float)
            
        except Exception as e:
            self.fail(f"Advanced analysis test failed: {e}")


class TestPyNLMPerformance(unittest.TestCase):
    """Test performance-related functionality"""
    
    def test_profiler_performance(self):
        """Test profiler performance"""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            profiler = pynlm.BrainProfiler(brain)
            profiler.start()
            
            start_time = time.time()
            
            for step in range(100):
                brain.step(step)
            
            end_time = time.time()
            
            profiler.stop()
            
            # Verify performance metrics
            report = profiler.generate_report()
            self.assertIsNotNone(report)
            
            # Performance should be reasonable (less than 1 second per 100 steps)
            self.assertLess(end_time - start_time, 1.0)
            
        except AttributeError:
            self.skipTest("BrainProfiler not available for performance test")
        except Exception as e:
            self.fail(f"Profiler performance test failed: {e}")


if __name__ == '__main__':
    unittest.main()
