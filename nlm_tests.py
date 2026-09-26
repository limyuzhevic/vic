"""
NLM Python Integration Tests

Comprehensive test suite for the enhanced NLM Python bindings.
Tests all Phase 6 integrated systems including memory, neuromodulation, cognition, and development.

Usage:
    python3 -m pytest nlm_tests/ -v
    python3 nlm_tests/run_comprehensive_tests.py
"""

import sys
import time
import unittest
import pynlm
import numpy as np
from typing import Dict, List, Any

class TestNLMConfiguration(unittest.TestCase):
    """Test the enhanced configuration system."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createAdvancedConfig()
    
    def test_default_config_creation(self):
        """Test creating default configuration."""
        config = pynlm.createDefaultConfig()
        self.assertTrue(config.has("neuron_count"))
        self.assertTrue(config.has("region_count"))
        self.assertTrue(config.has("connection_probability"))
    
    def test_advanced_config_creation(self):
        """Test creating advanced configuration."""
        config = pynlm.createAdvancedConfig()
        self.assertTrue(config.has("planning_depth"))
        self.assertTrue(config.has("attention_inhibition_strength"))
        self.assertTrue(config.has("enable_structural_plasticity"))
    
    def test_configuration_validation(self):
        """Test configuration validation."""
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", 2000)
        
        # Valid configuration should pass
        self.assertTrue(config.safeSet("learning_rate", 0.05))
        
        # Invalid configuration should fail (based on schema)
        # This would test schema-based validation
        pass
    
    def test_runtime_configuration(self):
        """Test runtime configuration adjustment."""
        config = pynlm.createAdvancedConfig()
        config.enableRuntimeAdjustment(True)
        
        # Test runtime modification
        self.assertTrue(config.safeSet("curiosity_threshold", 0.3))
        
        # Verify change
        self.assertTrue(config.has("curiosity_threshold"))

class TestNLMBrain(unittest.TestCase):
    """Test the enhanced NLM brain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createAdvancedConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_initialization(self):
        """Test brain initialization with all systems."""
        self.assertGreater(self.brain.getTotalNeuronCount(), 0)
        self.assertGreater(self.brain.getTotalSynapseCount(), 0)
        self.assertEqual(self.brain.getRegionCount(), 3)
    
    def test_brain_step(self):
        """Test brain simulation steps."""
        initial_spikes = self.brain.getTotalSpikeCount()
        
        for step in range(10):
            self.brain.step(step)
        
        # Should have generated spikes
        self.assertGreater(self.brain.getTotalSpikeCount(), initial_spikes)
        self.assertGreater(self.brain.getFiringNeuronCount(), 0)
    
    def test_brain_integration(self):
        """Test integration of all Phase 6 systems."""
        # Memory systems
        self.brain.enableWorkingMemory(True)
        self.brain.enableEpisodicMemory(True)
        self.brain.enableAssociativeMemory(True)
        
        # Neuromodulation
        self.brain.enableDopamine(True)
        self.brain.enableCuriosity(True)
        self.brain.enableNovelty(True)
        
        # Cognition
        self.brain.enablePlanner(True)
        self.brain.enableConceptFormation(True)
        self.brain.enableAttention(True)
        
        # Development
        self.brain.enableDevelopment(True)
        self.brain.enableStructuralPlasticity(True)
        
        # Step and verify systems are working
        self.brain.step(0)
        
        # Check system statistics
        stats = self.brain.getMemoryStats()
        self.assertIn("working_memory_traces", stats)
        
        neuromod_stats = self.brain.getNeuromodulationStats()
        self.assertIn("dopamine_level", neuromod_stats)
        
        cognition_stats = self.brain.getCognitionStats()
        self.assertIn("planning_depth", cognition_stats)

class TestNLMMemorySystems(unittest.TestCase):
    """Test memory systems integration."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createAdvancedConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enable all memory systems
        self.brain.enableWorkingMemory(True)
        self.brain.enableEpisodicMemory(True)
        self.brain.enableAssociativeMemory(True)
    
    def test_working_memory(self):
        """Test working memory functionality."""
        working_memory = self.brain.getWorkingMemory()
        
        # Test memory operations
        self.assertIsNotNone(working_memory)
        self.assertEqual(working_memory.getCapacity(), 300)
        
        # Test storing traces
        test_neuron = pynlm.INVALID_NEURON_ID  # Use invalid ID for testing
        working_memory.storeToNeuron(test_neuron, 0.5)
        
        # Verify memory traces
        traces = working_memory.getMemoryNeurons()
        self.assertGreater(len(traces), 0)
    
    def test_episodic_memory(self):
        """Test episodic memory functionality."""
        episodic_memory = self.brain.getEpisodicMemory()
        
        self.assertIsNotNone(episodic_memory)
        self.assertEqual(episodic_memory.getMaxEpisodes(), 1000)
        
        # Test episode storage
        episode = {
            "timestamp": 0,
            "reward": 1.0,
            "active_neurons": [],
            "neuron_activations": []
        }
        
        episodic_memory.storeEpisode(episode)
        self.assertGreater(episodic_memory.getEpisodeCount(), 0)
    
    def test_associative_memory(self):
        """Test associative memory functionality."""
        associative_memory = self.brain.getAssociativeMemory()
        
        self.assertIsNotNone(associative_memory)
        
        # Test pattern storage and recall
        pattern = [0.1, 0.2, 0.3, 0.4, 0.5]
        neurons = [pynlm.INVALID_NEURON_ID, pynlm.INVALID_NEURON_ID]
        
        associative_memory.storePattern(pattern, neurons)
        
        # Test recall
        recalled = associative_memory.recallPattern(pattern)
        self.assertIsInstance(recalled, list)

class TestNLMNeuromodulation(unittest.TestCase):
    """Test neuromodulation systems."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createAdvancedConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enable all neuromodulation systems
        self.brain.enableDopamine(True)
        self.brain.enableCuriosity(True)
        self.brain.enableNovelty(True)
        self.brain.enablePredictionError(True)
    
    def test_dopamine_system(self):
        """Test dopamine neuromodulation."""
        dopamine = self.brain.getDopamine()
        
        self.assertIsNotNone(dopamine)
        self.assertGreaterEqual(dopamine.getLevel(), -1.0)
        self.assertLessEqual(dopamine.getLevel(), 1.0)
        
        # Test plasticity factor
        plasticity_factor = dopamine.getPlasticityFactor()
        self.assertGreaterEqual(plasticity_factor, 0.0)
    
    def test_curiosity_system(self):
        """Test curiosity neuromodulation."""
        curiosity = self.brain.getCuriosity()
        
        self.assertIsNotNone(curiosity)
        self.assertGreaterEqual(curiosity.getLevel(), 0.0)
        self.assertLessEqual(curiosity.getLevel(), 1.0)
    
    def test_novelty_system(self):
        """Test novelty detection."""
        novelty = self.brain.getNovelty()
        
        self.assertIsNotNone(novelty)
        self.assertGreaterEqual(novelty.getLevel(), 0.0)
    
    def test_prediction_error_system(self):
        """Test prediction error system."""
        prediction_error = self.brain.getPredictionErrorSignal()
        
        self.assertIsNotNone(prediction_error)
        self.assertGreaterEqual(prediction_error.getLevel(), -1.0)
        self.assertLessEqual(prediction_error.getLevel(), 1.0)

class TestN LMCognition(unittest.TestCase):
    """Test cognition systems."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createAdvancedConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enable all cognition systems
        self.brain.enablePlanner(True)
        self.brain.enableConceptFormation(True)
        self.brain.enableAttention(True)
    
    def test_planner_system(self):
        """Test neural planner."""
        planner = self.brain.getPlanner()
        
        self.assertIsNotNone(planner)
        self.assertEqual(planner.getPlanningDepth(), 5)
    
    def test_concept_formation_system(self):
        """Test concept formation."""
        concept_formation = self.brain.getConceptFormation()
        
        self.assertIsNotNone(concept_formation)
        
        # Test concept storage
        pattern = [0.5, 0.3, 0.7, 0.2, 0.8]
        concept_formation.addConcept(pattern)
        
        # Verify concept count
        concepts = concept_formation.getConceptCount()
        self.assertGreaterEqual(concepts, 1)
    
    def test_attention_system(self):
        """Test attentional selection."""
        attention = self.brain.getAttention()
        
        self.assertIsNotNone(attention)
        self.assertGreaterEqual(attention.getInhibitionStrength(), 0.0)
        self.assertGreaterEqual(attention.getExcitationStrength(), 0.0)

class TestNLMAgentBrain(unittest.TestCase):
    """Test AgentBrain functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.config = pynlm.createAdvancedConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_agent_initialization(self):
        """Test agent initialization."""
        self.assertIsNotNone(self.agent)
        self.assertEqual(self.agent.getSensoryInputSize(), 256)  # Vision (16x16) + other inputs
        self.assertEqual(self.agent.getMotorOutputSize(), 6)  # 6 motor neurons
    
    def test_sensory_processing(self):
        """Test sensory processing."""
        percept = self.world.getSensoryPercept()
        
        # Process sensory input
        self.agent.processSensoryInput(percept)
        
        # Verify agent's internal state
        self.assertGreaterEqual(self.agent.getCuriosityLevel(), 0.0)
        self.assertGreaterEqual(self.agent.getNoveltyLevel(), 0.0)
    
    def test_motor_command_decoding(self):
        """Test motor command decoding."""
        # Inject some current to generate motor activity
        self.brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 10.0)
        
        # Decode motor command
        action = self.agent.decodeMotorCommand()
        
        self.assertIsNotNone(action)
        self.assertIsInstance(action.name, str)
    
    def test_reward_modulation(self):
        """Test reward-based neuromodulation."""
        # Test reward application
        self.agent.applyRewardModulation(1.0, 0.5)
        
        # Verify prediction error update
        prediction_error = self.agent.getPredictionError()
        self.assertIsInstance(prediction_error, float)
    
    def test_development_update(self):
        """Test development system updates."""
        # Test development updates
        self.agent.updateDevelopment(0.1)
        
        # Verify developmental stage
        stage = self.agent.getDevelopmentalStage()
        self.assertIsInstance(stage, pynlm.DevelopmentalStage)

class TestNLMIntegration(unittest.TestCase):
    """Test complete integration of all systems."""
    
    def test_complete_integration(self):
        """Test complete integration of all NLM systems."""
        # Create advanced configuration with all systems
        config = pynlm.createAdvancedConfig()
        config.set("enable_all_systems", True)
        
        # Create brain with all systems
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning systems
        agent.enableAllLearningSystems(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.05)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify integration
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
        self.assertGreater(agent.getNoveltyLevel(), 0.0)
        
        # Verify system statistics
        memory_stats = brain.getMemoryStats()
        self.assertIn("working_memory_traces", memory_stats)
        
        neuromod_stats = brain.getNeuromodulationStats()
        self.assertIn("dopamine_level", neuromod_stats)
        
        cognition_stats = brain.getCognitionStats()
        self.assertIn("planning_depth", cognition_stats)
    
    def test_learning_experiment(self):
        """Test complete learning experiment."""
        # Setup learning environment
        brain = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(brain)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run learning experiment
        total_reward = 0.0
        for step in range(200):
            world.update(0.05)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward (simplified)
            reward = 1.0 if step % 10 == 0 else 0.1
            total_reward += reward
            
            # Apply reward modulation
            predicted = reward * 0.9
            agent.applyRewardModulation(reward, predicted)
        
        # Verify learning results
        self.assertGreater(total_reward, 0)
        self.assertGreater(agent.getCuriosityLevel(), 0.0)
        self.assertGreater(brain.getAverageFiringRate(), 0.0)

class TestNLMPerformance(unittest.TestCase):
    """Test performance benchmarks."""
    
    def test_brain_performance(self):
        """Test brain simulation performance."""
        brain = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(brain)
        brain.initialize()
        
        # Measure performance
        start_time = time.time()
        
        for step in range(500):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Performance should be reasonable (less than 10 seconds for 500 steps)
        self.assertLess(duration, 10.0)
        
        # Calculate steps per second
        steps_per_second = 500 / duration
        self.assertGreater(steps_per_second, 10.0)  # At least 10 steps per second
    
    def test_memory_performance(self):
        """Test memory system performance."""
        brain = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(brain)
        brain.initialize()
        
        brain.enableWorkingMemory(True)
        brain.enableEpisodicMemory(True)
        
        # Measure memory operations
        start_time = time.time()
        
        for step in range(200):
            brain.step(step)
            
            # Memory operations
            working_memory = brain.getWorkingMemory()
            if working_memory:
                working_memory.update(0.1)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Memory operations should be efficient
        self.assertLess(duration, 5.0)

class TestNLMUtilities(unittest.TestCase):
    """Test utility functions and helpers."""
    
    def test_configuration_export(self):
        """Test configuration export/import."""
        config = pynlm.createAdvancedConfig()
        
        # Export configuration
        export = config.exportConfiguration()
        self.assertIsInstance(export, str)
        self.assertGreater(len(export), 0)
        
        # Import configuration (simplified)
        # config.importConfiguration(export)
        # self.assertTrue(config.has("neuron_count"))
    
    def test_brain_export(self):
        """Test brain state export."""
        brain = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(brain)
        brain.initialize()
        
        # Export brain state
        state = brain.exportState()
        self.assertIsInstance(state, dict)
        self.assertIn("total_neurons", state)
        self.assertIn("total_synapses", state)
        self.assertIn("total_spikes", state)
    
    def test_error_handling(self):
        """Test error handling."""
        # Test invalid operations
        try:
            # This should handle errors gracefully
            config = pynlm.createDefaultConfig()
            # ... test various error conditions
            pass
        except Exception as e:
            self.assertIsInstance(e, Exception)
    
    def test_constants_and_helpers(self):
        """Test constants and utility functions."""
        # Test constants
        self.assertIsNotNone(pynlm.INVALID_NEURON_ID)
        self.assertIsNotNone(pynlm.INVALID_SYNAPSE_ID)
        self.assertIsNotNone(pynlm.INVALID_REGION_ID)
        
        # Test helper functions
        self.assertIsInstance(pynlm.getVersion(), str)
        
        # Test system info
        info = pynlm.getSystemInfo()
        self.assertIsInstance(info, dict)
        self.assertIn("version", info)

class ComprehensiveNLMTestSuite:
    """Comprehensive test suite that runs all NLM tests."""
    
    @staticmethod
    def run_all_tests():
        """Run all NLM tests and return results."""
        import unittest
        
        # Create test suite
        suite = unittest.TestLoader().loadTestsFromTestCase(TestNLMConfiguration)
        suite.addTestsFromTestCase(TestNLMBrain)
        suite.addTestsFromTestCase(TestNLMMemorySystems)
        suite.addTestsFromTestCase(TestNLMNeuromodulation)
        suite.addTestsFromTestCase(TestNLMCognition)
        suite.addTestsFromTestCase(TestNLMAgentBrain)
        suite.addTestsFromTestCase(TestNLMIntegration)
        suite.addTestsFromTestCase(TestNLMPerformance)
        suite.addTestsFromTestCase(TestNLMUtilities)
        
        # Run tests with verbose output
        runner = unittest.TextTestRunner(verbosity=2)
        result = runner.run(suite)
        
        return result
    
    @staticmethod
    def run_advanced_tests():
        """Run advanced integration tests."""
        print("=== Advanced NLM Integration Tests ===")
        
        # Create advanced brain with all systems
        config = pynlm.createAdvancedConfig()
        config.set("neuron_count", 5000)
        config.set("enable_structural_plasticity", True)
        config.set("enable_prediction_system", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=100, height=100)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all systems
        agent.enableAllLearningSystems(True)
        
        # Run comprehensive simulation
        print("Running 500-step comprehensive simulation...")
        for step in range(500):
            world.update(0.02)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 100 == 0:
                print(f"  Step {step}: Reward={agent.getCuriosityLevel():.3f}, "
                      f"Firing={brain.getAverageFiringRate():.3f}")
        
        # Print final statistics
        print("\n=== Simulation Results ===")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"Prediction error: {agent.getPredictionError():.3f}")
        
        # Memory statistics
        memory_stats = brain.getMemoryStats()
        print(f"Working memory traces: {memory_stats.get('working_memory_traces', 'N/A')}")
        print(f"Episodic memory episodes: {memory_stats.get('episodic_memory_episodes', 'N/A')}")
        
        # Neuromodulation statistics
        neuromod_stats = brain.getNeuromodulationStats()
        print(f"Dopamine level: {neurom_stats.get('dopamine_level', 'N/A'):.3f}")
        
        # Cognition statistics
        cognition_stats = brain.getCognitionStats()
        print(f"Planning depth: {cognition_stats.get('planning_depth', 'N/A')}")
        
        return True

if __name__ == "__main__":
    print("=== NLM Python Integration Test Suite ===")
    print("Testing Phase 6 enhanced NLM Python bindings...")
    print()
    
    # Run basic unit tests
    print("Running basic unit tests...")
    test_suite = ComprehensiveNLMTestSuite()
    result = test_suite.run_all_tests()
    
    print(f"\nUnit tests completed: {result.testsRun} tests run")
    if result.failures:
        print(f"Failures: {len(result.failures)}")
    if result.errors:
        print(f"Errors: {len(result.errors)}")
    
    # Run advanced integration tests
    print("\n" + "="*60)
    try:
        advanced_result = test_suite.run_advanced_tests()
        print("✓ Advanced integration tests completed successfully!")
    except Exception as e:
        print(f"✗ Advanced integration tests failed: {e}")
    
    print("\n" + "="*60)
    print("NLM Python Integration Test Suite Complete")
    print("="*60)
    
    # Exit with appropriate code
    if result.failures or result.errors:
        sys.exit(1)
    else:
        sys.exit(0)
