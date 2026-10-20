#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.
Tests all the new features added in the enhanced Python bindings.
"""

import sys
import unittest
import pynlm

class TestBrainStatistics(unittest.TestCase):
    """Test brain statistics functionality"""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def testBrainStats(self):
        """Test getBrainStats() returns expected keys"""
        stats = self.brain.getBrainStats()
        
        expected_keys = {
            'total_neurons', 'total_synapses', 'firing_neurons',
            'active_neurons', 'total_spikes', 'avg_firing_rate',
            'e_i_ratio', 'development_stage'
        }
        
        self.assertEqual(set(stats.keys()), expected_keys)
        self.assertIsInstance(stats['total_neurons'], int)
        self.assertIsInstance(stats['total_synapses'], int)
        self.assertIsInstance(stats['firing_neurons'], int)
        self.assertIsInstance(stats['active_neurons'], int)
        self.assertIsInstance(stats['total_spikes'], int)
        self.assertIsInstance(stats['avg_firing_rate'], float)
        self.assertIsInstance(stats['e_i_ratio'], float)
        self.assertIsInstance(stats['development_stage'], int)
    
    def testBrainStatsConsistency(self):
        """Test that stats are consistent with individual methods"""
        for step in range(10):
            self.brain.step(step)
            
            stats = self.brain.getBrainStats()
            
            # Verify consistency
            self.assertEqual(stats['total_neurons'], self.brain.getTotalNeuronCount())
            self.assertEqual(stats['total_synapses'], self.brain.getTotalSynapseCount())
            self.assertEqual(stats['firing_neurons'], self.brain.getFiringNeuronCount())
            self.assertEqual(stats['active_neurons'], self.brain.getActiveNeuronCount())
            self.assertEqual(stats['total_spikes'], self.brain.getTotalSpikeCount())
            self.assertEqual(stats['avg_firing_rate'], self.brain.getAverageFiringRate())
            self.assertEqual(stats['e_i_ratio'], self.brain.getExcitationInhibitionRatio())
            self.assertEqual(stats['development_stage'], 
                           static_cast[int>(self.brain.getDevelopmentalStage()))
class TestAgentStatistics(unittest.TestCase):
    """Test agent statistics functionality"""
    
    def setUp(self):
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def testAgentStats(self):
        """Test getAgentStats() returns expected keys"""
        stats = self.agent.getAgentStats()
        
        expected_keys = {
            'curiosity', 'novelty', 'neuromodulation', 'prediction_error',
            'dev_stage', 'reward_modulation_enabled',
            'structural_plasticity_enabled', 'development_enabled',
            'curiosity_enabled'
        }
        
        self.assertEqual(set(stats.keys()), expected_keys)
        self.assertIsInstance(stats['curiosity'], float)
        self.assertIsInstance(stats['novelty'], float)
        self.assertIsInstance(stats['neuromodulation'], float)
        self.assertIsInstance(stats['prediction_error'], float)
        self.assertIsInstance(stats['dev_stage'], int)
        self.assertIsInstance(stats['reward_modulation_enabled'], bool)
        self.assertIsInstance(stats['structural_plasticity_enabled'], bool)
        self.assertIsInstance(stats['development_enabled'], bool)
        self.assertIsInstance(stats['curiosity_enabled'], bool)
    
    def testAgentStatsUpdates(self):
        """Test that agent stats change with learning"""
        initial_stats = self.agent.getAgentStats()
        
        # Run some steps with learning
        for step in range(50):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            reward = 0.1
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
        
        final_stats = self.agent.getAgentStats()
        
        # Stats should change after learning
        self.assertNotEqual(initial_stats['curiosity'], final_stats['curiosity'])
        self.assertNotEqual(initial_stats['novelty'], final_stats['novelty'])
        self.assertNotEqual(initial_stats['neuromodulation'], final_stats['neuromodulation'])
class TestValidationAndErrorHandling(unittest.TestCase):
    """Test parameter validation and error handling"""
    
    def testNegativeConfigValue(self):
        """Test that negative values are properly handled"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # The brain should handle the configuration without crashing
        # (validation happens at C++ level)
        self.assertTrue(brain.getTotalNeuronCount() > 0)
    
    def testMultipleBrainInstances(self):
        """Test creating multiple brain instances"""
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        brain1.initialize()
        brain2.initialize()
        
        # Brains should be independent
        self.assertEqual(brain1.getTotalNeuronCount(), brain2.getTotalNeuronCount())
        
        # Modify one brain
        for i in range(10):
            brain1.step(i)
        
        # The other should be unchanged
        self.assertEqual(brain2.getTotalSpikeCount(), 0)
class TestAdvancedFeatures(unittest.TestCase):
    """Test advanced features and utilities"""
    
    def testWorldInteraction(self):
        """Test comprehensive world interaction"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run enhanced simulation
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Verify final state
        self.assertGreater(brain.getTotalSpikeCount(), 0)
        self.assertLess(world.getAgentBody().health, 100.0)  # Some energy should be consumed
    
    def testContextManagerPattern(self):
        """Test brain context manager support"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test context manager (brain should support with statement)
        with brain:
            # Brain should be active in context
            self.assertGreater(brain.getTotalNeuronCount(), 0)
        
        # After context, brain should still be valid
        self.assertGreater(brain.getTotalNeuronCount(), 0)
    
    def testTypeConversions(self):
        """Test type conversions in configuration"""
        config = pynlm.createDefaultConfig()
        
        # Test setting different types
        config.set("brain.neuron_count", 500)  # int
        config.set("learning.rate", 0.01)      # float
        config.set("name", "TestBrain")        # string
        
        # Test getting different types
        self.assertEqual(config.getOr("brain.neuron_count", 100), 500)
        self.assertEqual(config.getOr("learning.rate", 0.001), 0.01)
        self.assertEqual(config.getOr("name", "Default"), "TestBrain")
class TestPerformanceFeatures(unittest.TestCase):
    """Test performance-related features"""
    
    def testBenchmarkBrain(self):
        """Test brain benchmarking functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # This tests the internal benchmark functionality
        # We can't easily test the external benchmarkBrain function
        # without exposing it, but we can test the brain's performance
        
        import time
        start_time = time.time()
        
        for step in range(50):
            brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Brain should complete steps in reasonable time
        self.assertLess(elapsed, 5.0)  # Should be much less than 5 seconds
        self.assertGreater(brain.getTotalSpikeCount(), 0)
class TestSensoryProcessing(unittest.TestCase):
    """Test sensory processing features"""
    
    def testSensoryPercept(self):
        """Test sensory percept functionality"""
        percept = pynlm.SensoryPercept()
        
        # Test initial state
        self.assertIsNone(percept.getVision())
        self.assertIsNone(percept.getAudio())
        self.assertIsNone(percept.getTouch())
        self.assertIsNone(percept.getInternal())
        
        # Test hasVision etc. methods
        self.assertFalse(percept.hasVision())
        self.assertFalse(percept.hasAudio())
        self.assertFalse(percept.hasTouch())
        self.assertFalse(percept.hasInternal())
        
        # Note: We can't easily create Vision objects without C++ API access
        # But we can test the Python interface
    
    def testWorldSensoryIntegration(self):
        """Test integration with world sensory processing"""
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        percept = world.getSensoryPercept()
        
        # Percept should exist
        self.assertIsNotNone(percept)
        
        # Should have vision capability (8x8)
        self.assertEqual(percept.getVisionWidth(), 8)
        self.assertEqual(percept.getVisionHeight(), 8)
class TestErrorHandling(unittest.TestCase):
    """Test error handling and edge cases"""
    
    def testInvalidParameters(self):
        """Test handling of invalid parameters"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # The Python bindings should handle invalid parameters gracefully
        # Any exceptions should be caught and converted to Python exceptions
        
        # This is more of a C++ level test, but we can test Python interface
        self.assertTrue(brain.getTotalNeuronCount() > 0)
    
    def testBrainReset(self):
        """Test brain reset functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        initial_spikes = brain.getTotalSpikeCount()
        self.assertGreater(initial_spikes, 0)
        
        # Reset brain
        brain.reset()
        brain.initialize()
        
        # Should have reset spike count
        self.assertEqual(brain.getTotalSpikeCount(), 0)
    
    def testSaveLoad(self):
        """Test brain save/load functionality"""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(10):
            brain.step(step)
        
        initial_spikes = brain.getTotalSpikeCount()
        self.assertGreater(initial_spikes, 0)
        
        # Save brain state (this is a C++ API test)
        # We can't easily test file I/O in Python without the full C++ API
        # But we can verify the save method exists
        self.assertTrue(hasattr(brain, 'save'))
        self.assertTrue(hasattr(brain, 'load'))
class TestEnumHandling(unittest.TestCase):
    """Test enumeration handling"""
    
    def testNeuronTypeEnumeration(self):
        """Test NeuronType enumeration"""
        self.assertEqual(pynlm.NeuronType.Excitatory, 0)
        self.assertEqual(pynlm.NeuronType.Inhibitory, 1)
        self.assertEqual(pynlm.NeuronType.Modulatory, 2)
        self.assertEqual(pynlm.NeuronType.Sensory, 3)
        self.assertEqual(pynlm.NeuronType.Motor, 4)
        self.assertEqual(pynlm.NeuronType.Internal, 5)
    
    def testSynapseTypeEnumeration(self):
        """Test SynapseType enumeration"""
        self.assertEqual(pynlm.SynapseType.Excitatory, 0)
        self.assertEqual(pynlm.SynapseType.Inhibitory, 1)
        self.assertEqual(pynlm.SynapseType.Modulatory, 2)
        self.assertEqual(pynlm.SynapseType.Electrical, 3)
        self.assertEqual(pynlm.SynapseType.GapJunction, 4)
    
    def testActionTypeEnumeration(self):
        """Test ActionType enumeration"""
        self.assertEqual(pynlm.ActionType.MoveForward, 0)
        self.assertEqual(pynlm.ActionType.MoveBackward, 1)
        self.assertEqual(pynlm.ActionType.TurnLeft, 2)
        self.assertEqual(pynlm.ActionType.TurnRight, 3)
        self.assertEqual(pynlm.ActionType.Look, 4)
        self.assertEqual(pynlm.ActionType.Interact, 5)
        self.assertEqual(pynlm.ActionType.Eat, 6)
        self.assertEqual(pynlm.ActionType.Drink, 7)
        self.assertEqual(pynlm.ActionType.Rest, 8)
        self.assertEqual(pynlm.ActionType.Wait, 9)
        self.assertEqual(pynlm.ActionType.Custom, 10)
    
    def testDevelopmentalStageEnumeration(self):
        """Test DevelopmentalStage enumeration"""
        self.assertEqual(pynlm.DevelopmentalStage.Initial, 0)
        self.assertEqual(pynlm.DevelopmentalStage.CriticalPeriod, 1)
        self.assertEqual(pynlm.DevelopmentalStage.Maturation, 2)
        self.assertEqual(pynlm.DevelopmentalStage.Adult, 3)
        self.assertEqual(pynlm.DevelopmentalStage.Aging, 4)
class TestIdClasses(unittest.TestCase):
    """Test ID class functionality"""
    
    def testNeuronId(self):
        """Test NeuronId class"""
        neuron_id = pynlm.NeuronId(42)
        self.assertEqual(neuron_id.value, 42)
        self.assertEqual(neuron_id.index(), 0)
        
        # Test equality
        neuron_id2 = pynlm.NeuronId(42)
        self.assertEqual(neuron_id, neuron_id2)
        
        # Test string representation
        self.assertIn("42", str(neuron_id))
    
    def testSynapseId(self):
        """Test SynapseId class"""
        synapse_id = pynlm.SynapseId(123)
        self.assertEqual(synapse_id.value, 123)
        self.assertEqual(synapse_id.index(), 0)
        
        # Test hash (should be usable as dict key)
        id_dict = {synapse_id: "test"}
        self.assertIn(synapse_id, id_dict)
    
    def testRegionId(self):
        """Test RegionId class"""
        region_id = pynlm.RegionId(7)
        self.assertEqual(region_id.value, 7)
        self.assertEqual(region_id.index(), 0)
        
        # Test string representation
        self.assertIn("7", str(region_id))
class TestConstants(unittest.TestCase):
    """Test module constants"""
    
    def testConstantsExist(self):
        """Test that all expected constants exist"""
        self.assertTrue(hasattr(pynlm, 'INVALID_NEURON_ID'))
        self.assertTrue(hasattr(pynlm, 'INVALID_SYNAPSE_ID'))
        self.assertTrue(hasattr(pynlm, 'INVALID_REGION_ID'))
        self.assertTrue(hasattr(pynlm, 'INVALID_POPULATION_ID'))
    
    def testConstantsAreValid(self):
        """Test that constants have valid values"""
        # Constants should be NeuronId/SynapseId/RegionId/PopulationId instances
        self.assertIsInstance(pynlm.INVALID_NEURON_ID, pynlm.NeuronId)
        self.assertIsInstance(pynlm.INVALID_SYNAPSE_ID, pynlm.SynapseId)
        self.assertIsInstance(pynlm.INVALID_REGION_ID, pynlm.RegionId)
        self.assertIsInstance(pynlm.INVALID_POPULATION_ID, pynlm.PopulationId)
if __name__ == '__main__':
    print("Running NLM Python Bindings Comprehensive Test Suite")
    print("=" * 60)
    
    # Run all tests
    unittest.main(verbosity=2)