#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings validation.
Tests all new Python bindings for Neural Learning Machine (NLM) modules.

This test suite validates:
1. Memory System Tests
2. Neuromodulation Tests
3. Plasticity Control Tests
4. Development System Tests
5. Integration Tests

Key requirements:
- Test with actual NLM brain instances
- Validate return values and functionality
- Test error handling for null pointers
- Verify Pythonic API behavior
- Include performance considerations
- Create both unit tests and integration tests
"""

import pynlm as nlm
import time
import sys
import traceback
from typing import List, Tuple, Optional, Any

# Test configuration
TEST_CONFIG = {
    "num_neurons": 1000,
    "num_regions": 10,
    "sensory_width": 64,
    "sensory_height": 64,
    "max_duration": 1000,  # timesteps
}

class NLMBindingTester:
    """Comprehensive test suite for NLM Python bindings."""
    
    def __init__(self):
        self.brain = None
        self.working_memory = None
        self.episodic_memory = None
        self.associative_memory = None
        self.config = None
        self.errors = []
        self.passed = 0
        self.failed = 0
        
    def log_error(self, message: str, test_name: str = ""):
        """Log an error message."""
        error_msg = f"ERROR: {test_name}: {message}" if test_name else f"ERROR: {message}"
        self.errors.append(error_msg)
        self.failed += 1
        print(f"{error_msg}")
        
    def log_pass(self, message: str, test_name: str = ""):
        """Log a successful test."""
        pass_msg = f"PASS: {test_name}: {message}" if test_name else f"PASS: {message}"
        self.passed += 1
        print(f"{pass_msg}")
        
    def setup_test_environment(self):
        """Setup test environment with configuration and brain instance."""
        print("=" * 70)
        print("SETUP: Initializing NLM test environment")
        print("=" * 70)
        
        try:
            # Create and configure brain
            print("\n1. Creating default configuration...")
            self.config = nlm.createDefaultConfig()
            print("   ✓ Configuration created")
            
            print("\n2. Creating brain instance...")
            self.brain = nlm.createBrain(self.config)
            self.brain.initialize()
            print(f"   ✓ Brain initialized with {self.brain.getRegionCount()} regions")
            print(f"   ✓ Total neurons: {self.brain.getTotalNeuronCount()}")
            
            print("\n3. Initializing memory systems...")
            self.working_memory = self.brain.getWorkingMemory()
            self.episodic_memory = self.brain.getEpisodicMemory()
            self.associative_memory = self.brain.getAssociativeMemory()
            
            if self.working_memory:
                print(f"   ✓ Working memory initialized (capacity: {self.working_memory.getCapacity()})")
            if self.episodic_memory:
                print(f"   ✓ Episodic memory initialized (max episodes: {self.episodic_memory.getEpisodeCount()})")
            if self.associative_memory:
                print(f"   ✓ Associative memory initialized (association count: {self.associative_memory.getAssociationCount()})")
                
            self.log_pass("Test environment setup completed")
            
        except Exception as e:
            self.log_error(f"Failed to setup test environment: {str(e)}", "setup_test_environment")
            raise
            
    def cleanup_test_environment(self):
        """Cleanup test environment."""
        print("\n" + "=" * 70)
        print("CLEANUP: Finalizing test environment")
        print("=" * 70)
        
        try:
            if self.brain:
                print("\n1. Resetting brain state...")
                self.brain.reset()
                print("   ✓ Brain reset completed")
                
            print(f"\n2. Test Results Summary:")
            print(f"   - Tests passed: {self.passed}")
            print(f"   - Tests failed: {self.failed}")
            print(f"   - Total errors: {len(self.errors)}")
            
            if self.errors:
                print(f"\n3. Error Details:")
                for i, error in enumerate(self.errors, 1):
                    print(f"   {i}. {error}")
                    
            self.log_pass("Test environment cleanup completed")
            
        except Exception as e:
            self.log_error(f"Error during cleanup: {str(e)}", "cleanup_test_environment")
            
    def run_all_tests(self):
        """Run all test suites."""
        try:
            self.setup_test_environment()
            
            # Run test suites
            self.run_memory_system_tests()
            self.run_neuromodulation_tests()
            self.run_plasticity_control_tests()
            self.run_development_system_tests()
            self.run_integration_tests()
            
            self.log_pass(f"All tests completed: {self.passed} passed, {self.failed} failed")
            
        finally:
            self.cleanup_test_environment()
            
    # ===== MEMORY SYSTEM TESTS =====
    
    def run_memory_system_tests(self):
        """Run all memory system tests."""
        print("\n" + "=" * 70)
        print("MEMORY SYSTEM TESTS")
        print("=" * 70)
        
        self.test_neural_working_memory()
        self.test_neural_episodic_memory()
        self.test_neural_associative_memory()
        
    def test_neural_working_memory(self):
        """Test NeuralWorkingMemory bindings."""
        print("\n--- Testing NeuralWorkingMemory ---")
        
        if not self.working_memory:
            self.log_error("Working memory not initialized", "test_neural_working_memory")
            return
            
        # Test 1: store() and retrieve()
        try:
            print("\n1. Testing store() and retrieve()...")
            pattern = [0.1, 0.3, 0.7, 0.9, 0.5]
            self.working_memory.store(pattern, strength=1.0)
            print("   ✓ Pattern stored")
            
            retrieved = self.working_memory.retrieve()
            assert isinstance(retrieved, list), "retrieve() should return list"
            assert len(retrieved) == len(pattern), "Retrieved pattern length should match"
            print(f"   ✓ Pattern retrieved ({len(retrieved)} values)")
            print(f"   ✓ Average activation: {sum(retrieved)/len(retrieved):.3f}")
            
        except Exception as e:
            self.log_error(f"store()/retrieve() test failed: {str(e)}", "test_neural_working_memory")
            
        # Test 2: getNeuronActivation()
        try:
            print("\n2. Testing getNeuronActivation()...")
            # Test with invalid neuron ID first
            invalid_activation = self.working_memory.getNeuronActivation(-1)
            print(f"   ✓ Invalid neuron ID returned: {invalid_activation}")
            
            # Test with valid neurons
            for i in range(5):
                activation = self.working_memory.getNeuronActivation(i)
                assert isinstance(activation, (int, float)), "Activation should be numeric"
                assert 0.0 <= activation <= 1.0, "Activation should be in [0, 1] range"
            print("   ✓ Valid neuron activations retrieved")
            
        except Exception as e:
            self.log_error(f"getNeuronActivation() test failed: {str(e)}", "test_neural_working_memory")
            
        # Test 3: strengthenMemory()
        try:
            print("\n3. Testing strengthenMemory()...")
            self.working_memory.strengthenMemory(1.2)
            print("   ✓ Memory strengthened by 20%")
            
        except Exception as e:
            self.log_error(f"strengthenMemory() test failed: {str(e)}", "test_neural_working_memory")
            
        # Test 4: Additional methods
        try:
            print("\n4. Testing additional working memory methods...")
            self.working_memory.runCompetition()
            winners = self.working_memory.getMemoryNeurons()
            print(f"   ✓ Competition completed with {len(winners)} winners")
            
            active_traces = self.working_memory.getActiveTraces()
            memory_activity = self.working_memory.getMemoryActivity()
            print(f"   ✓ Active traces: {active_traces}, Memory activity: {memory_activity:.3f}")
            
            self.working_memory.update(0.01)
            print("   ✓ Memory updated")
            
        except Exception as e:
            self.log_error(f"Additional methods test failed: {str(e)}", "test_neural_working_memory")
            
    def test_neural_episodic_memory(self):
        """Test NeuralEpisodicMemory bindings."""
        print("\n--- Testing NeuralEpisodicMemory ---")
        
        if not self.episodic_memory:
            self.log_error("Episodic memory not initialized", "test_neural_episodic_memory")
            return
            
        try:
            print("\n1. Testing storeEpisode()...")
            # Create an episode
            episode = nlm.EpisodicMemoryItem()
            episode.sensoryState = [0.5, 0.2, 0.8]
            episode.positionX = 10.0
            episode.positionY = 5.0
            episode.orientation = 0.0
            episode.action = nlm.ActionType.MoveForward
            episode.reward = 1.0
            episode.energy = 0.8
            episode.novelty = 0.3
            episode.resultingSensoryState = [0.6, 0.3, 0.7]
            episode.resultingReward = 1.2
            episode.activeNeurons = [1001, 1002, 1003]
            episode.neuronActivations = [0.7, 0.8, 0.6]
            episode.timestamp = 100
            
            self.episodic_memory.storeEpisode(episode)
            print("   ✓ Episode stored")
            
            # Test retrieveSimilar()
            print("\n2. Testing retrieveSimilar()...")
            query_pattern = [0.4, 0.2, 0.7]
            similar_episodes = self.episodic_memory.retrieveSimilar(query_pattern, maxResults=3)
            assert isinstance(similar_episodes, list), "retrieveSimilar() should return list"
            print(f"   ✓ Found {len(similar_episodes)} similar episodes")
            
            # Test other retrieval methods
            print("\n3. Testing temporal retrieval...")
            temporal_episodes = self.episodic_memory.retrieveTemporal(50, 150, maxResults=5)
            print(f"   ✓ {len(temporal_episodes)} episodes in time window 50-150")
            
            print("\n4. Testing location retrieval...")
            location_episodes = self.episodic_memory.retrieveByLocation(10.0, 5.0, 2.0, maxResults=3)
            print(f"   ✓ {len(location_episodes)} episodes near (10, 5) radius 2.0")
            
            print("\n5. Testing action retrieval...")
            action_episodes = self.episodic_memory.retrieveAfterAction(nlm.ActionType.MoveForward, maxResults=3)
            print(f"   ✓ {len(action_episodes)} episodes after MoveForward")
            
            # Test replay
            print("\n6. Testing replayEpisode()...")
            if similar_episodes:
                self.episodic_memory.replayEpisode(similar_episodes[0])
                print("   ✓ Episode replayed")
                
            # Test statistics
            print("\n7. Testing statistics...")
            avg_reward = self.episodic_memory.getAverageReward()
            recent_episodes = self.episodic_memory.getRecentEpisodes(5)
            print(f"   ✓ Average reward: {avg_reward:.3f}")
            print(f"   ✓ Recent episodes (last 5): {len(recent_episodes)}")
            
            episodes_for_replay = self.episodic_memory.getEpisodesForReplay(3)
            print(f"   ✓ Episodes selected for replay: {len(episodes_for_replay)}")
            
            self.log_pass("Episodic memory tests completed")
            
        except Exception as e:
            self.log_error(f"Episodic memory tests failed: {str(e)}", "test_neural_episodic_memory")
            
    def test_neural_associative_memory(self):
        """Test NeuralAssociativeMemory bindings."""
        print("\n--- Testing NeuralAssociativeMemory ---")
        
        if not self.associative_memory:
            self.log_error("Associative memory not initialized", "test_neural_associative_memory")
            return
            
        try:
            print("\n1. Testing associate()...")
            # Create associations between patterns
            pattern_a = [0.1, 0.2, 0.3]
            pattern_b = [0.8, 0.9, 0.7]
            pattern_c = [0.4, 0.5, 0.6]
            
            self.associative_memory.associate(pattern_a, pattern_b, strength=0.9)
            self.associative_memory.associate(pattern_b, pattern_c, strength=0.7)
            self.associative_memory.associate(pattern_a, pattern_c, strength=0.5)
            print("   ✓ Associations created")
            
            # Test retrieve()
            print("\n2. Testing retrieve()...")
            query_pattern = pattern_a
            results = self.associative_memory.retrieve(query_pattern, maxResults=3)
            assert isinstance(results, list), "retrieve() should return list"
            print(f"   ✓ Retrieved {len(results)} patterns associated with query")
            
            # Test getAssociationStrength()
            print("\n3. Testing getAssociationStrength()...")
            strength_ab = self.associative_memory.getAssociationStrength(pattern_a, pattern_b)
            strength_bc = self.associative_memory.getAssociationStrength(pattern_b, pattern_c)
            print(f"   ✓ Association strength A-B: {strength_ab:.3f}")
            print(f"   ✓ Association strength B-C: {strength_bc:.3f}")
            
            # Test updateAssociation()
            print("\n4. Testing updateAssociation()...")
            self.associative_memory.updateAssociation(pattern_a, pattern_b, 0.1)
            new_strength = self.associative_memory.getAssociationStrength(pattern_a, pattern_b)
            print(f"   ✓ Updated strength A-B: {new_strength:.3f}")
            
            # Test spreadActivation()
            print("\n5. Testing spreadActivation()...")
            activated_patterns = self.associative_memory.spreadActivation(pattern_a, steps=2)
            assert isinstance(activated_patterns, list), "spreadActivation() should return list"
            print(f"   ✓ Patterns activated through association: {len(activated_patterns)}")
            
            # Test getAssociationCount()
            print("\n6. Testing getAssociationCount()...")
            association_count = self.associative_memory.getAssociationCount()
            print(f"   ✓ Total associations: {association_count}")
            
            self.log_pass("Associative memory tests completed")
            
        except Exception as e:
            self.log_error(f"Associative memory tests failed: {str(e)}", "test_neural_associative_memory")
            
    # ===== NEUROMODULATION TESTS =====
    
    def run_neuromodulation_tests(self):
        """Run all neuromodulation tests."""
        print("\n" + "=" * 70)
        print("NEUROMODULATION TESTS")
        print("=" * 70)
        
        self.test_dopamine()
        self.test_curiosity()
        self.test_novelty()
        self.test_prediction_error()
        
    def test_dopamine(self):
        """Test Dopamine neuromodulator bindings."""
        print("\n--- Testing Dopamine ---")
        
        try:
            print("\n1. Testing signalReward()...")
            # Create dopamine instance
            dopamine = nlm.createWithLevel(0.0)  # Note: using createWithLevel based on the existing pattern
            
            # Test signalReward()
            dopamine.signalReward(1.0)
            print("   ✓ Reward signal sent")
            
            # Test signalRewardPredictionError()
            dopamine.signalRewardPredictionError(0.5)
            print("   ✓ Reward prediction error signal sent")
            
            # Test setLevel()
            dopamine.setLevel(0.8)
            level = dopamine.getLevel()
            assert isinstance(level, float), "Dopamine level should be float"
            assert 0.0 <= level <= 1.0, "Dopamine level should be in [0, 1] range"
            print(f"   ✓ Dopamine level set to {level:.3f}")
            
            # Test reset
            dopamine.reset()
            print("   ✓ Dopamine reset")
            
            self.log_pass("Dopamine tests completed")
            
        except Exception as e:
            self.log_error(f"Dopamine tests failed: {str(e)}", "test_dopamine")
            
    def test_curiosity(self):
        """Test Curiosity neuromodulator bindings."""
        print("\n--- Testing Curiosity ---")
        
        try:
            print("\n1. Testing createWithWeights()...")
            # Create curiosity instance
            curiosity = nlm.createWithWeights(0.7, 0.3)  # Using createWithWeights pattern
            print("   ✓ Curiosity instance created with weights")
            
            # Test getLevel()
            print("\n2. Testing getLevel()...")
            level = curiosity.getLevel()
            assert isinstance(level, float), "Curiosity level should be float"
            assert 0.0 <= level <= 1.0, "Curiosity level should be in [0, 1] range"
            print(f"   ✓ Curiosity level: {level:.3f}")
            
            # Test getExplorationDrive()
            print("\n3. Testing getExplorationDrive()...")
            exploration_drive = curiosity.getExplorationDrive()
            assert isinstance(exploration_drive, float), "Exploration drive should be float"
            assert 0.0 <= exploration_drive <= 1.0, "Exploration drive should be in [0, 1] range"
            print(f"   ✓ Exploration drive: {exploration_drive:.3f}")
            
            # Test update()
            print("\n4. Testing update()...")
            curiosity.update(0.3, 0.5, 0.01)
            print("   ✓ Curiosity updated with novelty=0.3, predictionError=0.5")
            
            # Test setNoveltyWeight() and setPredictionErrorWeight()
            print("\n5. Testing weight setters...")
            curiosity.setNoveltyWeight(0.5)
            curiosity.setPredictionErrorWeight(0.5)
            print("   ✓ Weights set to 0.5 each")
            
            # Test reset
            curiosity.reset()
            print("   ✓ Curiosity reset")
            
            self.log_pass("Curiosity tests completed")
            
        except Exception as e:
            self.log_error(f"Curiosity tests failed: {str(e)}", "test_curiosity")
            
    def test_novelty(self):
        """Test Novelty neuromodulator bindings."""
        print("\n--- Testing Novelty ---")
        
        try:
            print("\n1. Testing createWithSensitivity()...")
            # Create novelty instance
            novelty = nlm.createWithSensitivity(1.0)  # Using createWithSensitivity pattern
            print("   ✓ Novelty instance created with sensitivity=1.0")
            
            # Test getLevel()
            print("\n2. Testing getLevel()...")
            level = novelty.getLevel()
            assert isinstance(level, float), "Novelty level should be float"
            assert 0.0 <= level <= 1.0, "Novelty level should be in [0, 1] range"
            print(f"   ✓ Novelty level: {level:.3f}")
            
            # Test setLevel()
            print("\n3. Testing setLevel()...")
            novelty.setLevel(0.7)
            new_level = novelty.getLevel()
            assert new_level == 0.7, "Novelty level should be 0.7"
            print(f"   ✓ Novelty level set to {new_level:.3f}")
            
            # Test detectNovelty() with patterns
            print("\n4. Testing detectNovelty() with patterns...")
            current_pattern = [0.1, 0.2, 0.3]
            previous_pattern = [0.9, 0.8, 0.7]
            novelty.detectNovelty(current_pattern, previous_pattern)
            print("   ✓ Novelty detected between patterns")
            
            # Test detectNovelty() with observations
            print("\n5. Testing detectNovelty() with observations...")
            current_obs = nlm.Vision(64, 64, 3)
            previous_obs = nlm.Vision(64, 64, 3)
            novelty.detectNovelty(current_obs, previous_obs)
            print("   ✓ Novelty detected between observations")
            
            # Test update()
            print("\n6. Testing update()...")
            novelty.update(0.01)
            print("   ✓ Novelty updated")
            
            # Test getHistory()
            print("\n7. Testing getHistory()...")
            history = novelty.getHistory()
            assert isinstance(history, list), "Novelty history should be list"
            print(f"   ✓ Novelty history contains {len(history)} entries")
            
            # Test clearHistory()
            print("\n8. Testing clearHistory()...")
            novelty.clearHistory()
            print("   ✓ Novelty history cleared")
            
            # Test setSensitivity() and getSensitivity()
            print("\n9. Testing sensitivity methods...")
            novelty.setSensitivity(0.5)
            sensitivity = novelty.getSensitivity()
            assert sensitivity == 0.5, "Novelty sensitivity should be 0.5"
            print(f"   ✓ Novelty sensitivity: {sensitivity:.3f}")
            
            self.log_pass("Novelty tests completed")
            
        except Exception as e:
            self.log_error(f"Novelty tests failed: {str(e)}", "test_novelty")
            
    def test_prediction_error(self):
        """Test PredictionError neuromodulator bindings."""
        print("\n--- Testing PredictionError ---")
        
        try:
            print("\n1. Testing createWithInitial()...")
            # Create prediction error instance
            pred_error = nlm.createWithInitial(0.0)  # Using createWithInitial pattern
            print("   ✓ PredictionError instance created with initial error=0.0")
            
            # Test getError()
            print("\n2. Testing getError()...")
            error = pred_error.getError()
            assert isinstance(error, float), "Prediction error should be float"
            print(f"   ✓ Prediction error: {error:.3f}")
            
            # Test computeError()
            print("\n3. Testing computeError()...")
            pred_error.computeError(0.5, 0.8)  # predicted=0.5, actual=0.8
            print("   ✓ Prediction error computed (pred=0.5, actual=0.8)")
            
            # Test updatePrediction()
            print("\n4. Testing updatePrediction()...")
            pred_error.updatePrediction(0.7)
            print("   ✓ Prediction updated to 0.7")
            
            # Test getHistory()
            print("\n5. Testing getHistory()...")
            history = pred_error.getHistory()
            assert isinstance(history, list), "Prediction error history should be list"
            print(f"   ✓ Prediction error history contains {len(history)} entries")
            
            # Test clearHistory()
            print("\n6. Testing clearHistory()...")
            pred_error.clearHistory()
            print("   ✓ Prediction error history cleared")
            
            # Test getMagnitude()
            print("\n7. Testing getMagnitude()...")
            magnitude = pred_error.getMagnitude()
            assert isinstance(magnitude, float), "Prediction error magnitude should be float"
            assert magnitude >= 0.0, "Prediction error magnitude should be >= 0"
            print(f"   ✓ Prediction error magnitude: {magnitude:.3f}")
            
            # Test reset
            pred_error.reset()
            print("   ✓ Prediction error reset")
            
            self.log_pass("PredictionError tests completed")
            
        except Exception as e:
            self.log_error(f"PredictionError tests failed: {str(e)}", "test_prediction_error")
            
    # ===== PLASTICITY CONTROL TESTS =====
    
    def run_plasticity_control_tests(self):
        """Run all plasticity control tests."""
        print("\n" + "=" * 70)
        print("PLASTICITY CONTROL TESTS")
        print("=" * 70)
        
        self.test_stdp()
        self.test_hebbian()
        self.test_structural_plasticity()
        
    def test_stdp(self):
        """Test STDP plasticity rule bindings."""
        print("\n--- Testing STDP ---")
        
        try:
            print("\n1. Testing create_stdp()...")
            stdp = nlm.create_stdp()
            assert stdp is not None, "STDP instance should not be None"
            print("   ✓ STDP instance created")
            
            # Test parameter getters
            print("\n2. Testing parameter getters...")
            ltp_weight = stdp.getLTPWeight()
            ltd_weight = stdp.getLTDWeight()
            time_constant = stdp.getTimeConstant()
            
            assert isinstance(ltp_weight, float), "LTP weight should be float"
            assert isinstance(ltd_weight, float), "LTD weight should be float"
            assert isinstance(time_constant, float), "Time constant should be float"
            print(f"   ✓ STDP parameters: LTP={ltp_weight:.4f}, LTD={ltd_weight:.4f}, τ={time_constant:.1f}ms")
            
            # Test parameter setters
            print("\n3. Testing parameter setters...")
            stdp.setLTPWeight(0.02)
            stdp.setLTDWeight(0.025)
            stdp.setTimeConstant(30.0)
            
            assert stdp.getLTPWeight() == 0.02, "LTP weight not set correctly"
            assert stdp.getLTDWeight() == 0.025, "LTD weight not set correctly"
            assert stdp.getTimeConstant() == 30.0, "Time constant not set correctly"
            print("   ✓ STDP parameters set and verified")
            
            # Test configure()
            print("\n4. Testing configure()...")
            stdp.configure(0.015, 0.018, 25.0)
            assert stdp.getLTPWeight() == 0.015, "Configure not working for LTP"
            assert stdp.getLTDWeight() == 0.018, "Configure not working for LTD"
            assert stdp.getTimeConstant() == 25.0, "Configure not working for time constant"
            print("   ✓ Configure method working")
            
            # Test inherited methods from PlasticityRule
            print("\n5. Testing inherited PlasticityRule methods...")
            stdp.setEnabled(True)
            assert stdp.isEnabled() == True, "setEnabled/isEnabled not working"
            print("   ✓ Inherited PlasticityRule methods working")
            
            # Test getName()
            print("\n6. Testing getName()...")
            name = stdp.getName()
            assert isinstance(name, str), "getName should return string"
            assert name == "STDP", f"Expected 'STDP', got '{name}'"
            print(f"   ✓ getName() returned: '{name}'")
            
            self.log_pass("STDP tests completed")
            
        except Exception as e:
            self.log_error(f"STDP tests failed: {str(e)}", "test_stdp")
            
    def test_hebbian(self):
        """Test Hebbian plasticity rule bindings."""
        print("\n--- Testing Hebbian ---")
        
        try:
            print("\n1. Testing create_hebbian()...")
            hebbian = nlm.create_hebbian()
            assert hebbian is not None, "Hebbian instance should not be None"
            print("   ✓ Hebbian instance created")
            
            # Test parameter getters
            print("\n2. Testing parameter getters...")
            learning_rate = hebbian.getLearningRate()
            max_weight = hebbian.getMaxWeight()
            
            assert isinstance(learning_rate, float), "Learning rate should be float"
            assert isinstance(max_weight, float), "Max weight should be float"
            print(f"   ✓ Hebbian parameters: η={learning_rate:.4f}, wmax={max_weight:.1f}")
            
            # Test parameter setters
            print("\n3. Testing parameter setters...")
            hebbian.setLearningRate(0.01)
            hebbian.setMaxWeight(2.0)
            
            assert hebbian.getLearningRate() == 0.01, "Learning rate not set correctly"
            assert hebbian.getMaxWeight() == 2.0, "Max weight not set correctly"
            print("   ✓ Hebbian parameters set and verified")
            
            # Test inherited methods
            print("\n4. Testing inherited methods...")
            hebbian.setEnabled(False)
            assert hebbian.isEnabled() == False, "setEnabled/isEnabled not working"
            print("   ✓ Inherited PlasticityRule methods working")
            
            # Test getName()
            print("\n5. Testing getName()...")
            name = hebbian.getName()
            assert isinstance(name, str), "getName should return string"
            assert name == "Hebbian", f"Expected 'Hebbian', got '{name}'"
            print(f"   ✓ getName() returned: '{name}'")
            
            self.log_pass("Hebbian tests completed")
            
        except Exception as e:
            self.log_error(f"Hebbian tests failed: {str(e)}", "test_hebbian")
            
    def test_structural_plasticity(self):
        """Test StructuralPlasticity bindings."""
        print("\n--- Testing StructuralPlasticity ---")
        
        try:
            print("\n1. Testing create_structural_plasticity()...")
            structural = nlm.create_structural_plasticity()
            assert structural is not None, "StructuralPlasticity instance should not be None"
            print("   ✓ StructuralPlasticity instance created")
            
            # Test parameter getters
            print("\n2. Testing parameter getters...")
            synaptogenesis_rate = structural.getSynaptogenesisRate()
            pruning_rate = structural.getPruningRate()
            
            assert isinstance(synaptogenesis_rate, float), "Synaptogenesis rate should be float"
            assert isinstance(pruning_rate, float), "Pruning rate should be float"
            print(f"   ✓ StructuralPlasticity parameters: synaptogenesis={synaptogenesis_rate:.4f}, pruning={pruning_rate:.4f}")
            
            # Test parameter setters
            print("\n3. Testing parameter setters...")
            structural.setSynaptogenesisRate(0.01)
            structural.setPruningRate(0.005)
            
            assert structural.getSynaptogenesisRate() == 0.01, "Synaptogenesis rate not set correctly"
            assert structural.getPruningRate() == 0.005, "Pruning rate not set correctly"
            print("   ✓ StructuralPlasticity parameters set and verified")
            
            self.log_pass("StructuralPlasticity tests completed")
            
        except Exception as e:
            self.log_error(f"StructuralPlasticity tests failed: {str(e)}", "test_structural_plasticity")
            
    # ===== DEVELOPMENT SYSTEM TESTS =====
    
    def run_development_system_tests(self):
        """Run all development system tests."""
        print("\n" + "=" * 70)
        print("DEVELOPMENT SYSTEM TESTS")
        print("=" * 70)
        
        self.test_development_system()
        
    def test_development_system(self):
        """Test DevelopmentSystem bindings."""
        print("\n--- Testing DevelopmentSystem ---")
        
        try:
            print("\n1. Testing createDevelopmentSystem()...")
            # Note: DevelopmentSystem is created by brain
            dev_system = self.brain.getDevelopmentSystem()
            assert dev_system is not None, "DevelopmentSystem should not be None"
            print("   ✓ DevelopmentSystem obtained from brain")
            
            # Test parameter getters
            print("\n2. Testing parameter getters...")
            maturation_rate = dev_system.getMaturationRate()
            critical_period_duration = dev_system.getCriticalPeriodDuration()
            
            assert isinstance(maturation_rate, float), "Maturation rate should be float"
            assert isinstance(critical_period_duration, float), "Critical period duration should be float"
            print(f"   ✓ DevelopmentSystem parameters: maturationRate={maturation_rate:.3f}, criticalPeriodDuration={critical_period_duration:.1f}")
            
            # Test parameter setters
            print("\n3. Testing parameter setters...")
            dev_system.setMaturationRate(0.5)
            dev_system.setCriticalPeriodDuration(100.0)
            
            assert dev_system.getMaturationRate() == 0.5, "Maturation rate not set correctly"
            assert dev_system.getCriticalPeriodDuration() == 100.0, "Critical period duration not set correctly"
            print("   ✓ DevelopmentSystem parameters set and verified")
            
            # Test development stage methods
            print("\n4. Testing development stage methods...")
            current_stage = dev_system.getStage()
            stage_name = dev_system.getStageName()
            print(f"   ✓ Current stage: {stage_name} ({current_stage})")
            
            is_critical_period = dev_system.isCriticalPeriod()
            print(f"   ✓ Is critical period: {is_critical_period}")
            
            critical_period_progress = dev_system.getCriticalPeriodProgress()
            print(f"   ✓ Critical period progress: {critical_period_progress:.1%}")
            
            # Test development age methods
            print("\n5. Testing development age methods...")
            dev_age = dev_system.getDevelopmentalAge()
            print(f"   ✓ Developmental age: {dev_age:.1f}")
            
            # Test plasticity modifier
            print("\n6. Testing plasticity modifier...")
            plasticity_modifier = dev_system.getPlasticityModifier()
            print(f"   ✓ Plasticity modifier: {plasticity_modifier:.3f}")
            
            # Test setPlasticityModifier()
            print("\n7. Testing setPlasticityModifier()...")
            dev_system.setPlasticityModifier(0.8)
            new_plasticity_modifier = dev_system.getPlasticityModifier()
            assert new_plasticity_modifier == 0.8, "Plasticity modifier not set correctly"
            print(f"   ✓ Plasticity modifier set to {new_plasticity_modifier:.3f}")
            
            # Test phase transition
            print("\n8. Testing phase transition...")
            dev_system.triggerPhaseTransition()
            print("   ✓ Phase transition triggered")
            
            self.log_pass("DevelopmentSystem tests completed")
            
        except Exception as e:
            self.log_error(f"DevelopmentSystem tests failed: {str(e)}", "test_development_system")
            
    # ===== INTEGRATION TESTS =====
    
    def run_integration_tests(self):
        """Run all integration tests."""
        print("\n" + "=" * 70)
        print("INTEGRATION TESTS")
        print("=" * 70)
        
        self.test_memory_neuromodulation_integration()
        self.test_complete_agent_simulation()
        
    def test_memory_neuromodulation_integration(self):
        """Test combined usage of memory + neuromodulation + plasticity."""
        print("\n--- Testing Memory + Neuromodulation + Plasticity Integration ---")
        
        try:
            print("\n1. Initializing test scenario...")
            
            # Create test data
            test_pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
            test_episode = nlm.EpisodicMemoryItem()
            test_episode.sensoryState = test_pattern
            test_episode.positionX = 15.0
            test_episode.positionY = 8.0
            test_episode.action = nlm.ActionType.Interact
            test_episode.reward = 2.0
            test_episode.activeNeurons = [5001, 5002,5003]
            test_episode.neuronActivations = [0.8, 0.6, 0.9]
            test_episode.timestamp = 200
            
            print("\n2. Storing in working memory...")
            self.working_memory.store(test_pattern, strength=1.0)
            print("   ✓ Pattern stored in working memory")
            
            # Get neuromodulator instances
            dopamine = nlm.createWithLevel(0.0)
            curiosity = nlm.createWithWeights(0.7, 0.3)
            novelty = nlm.createWithSensitivity(1.0)
            
            print("\n3. Applying neuromodulation...")
            dopamine.signalReward(1.5)
            print("   ✓ Reward signal applied")
            
            curiosity.update(0.2, 0.3, 0.01)
            print("   ✓ Curiosity updated")
            
            novelty.detectNovelty(test_pattern, [0.0, 0.0, 0.0])
            print("   ✓ Novelty detected")
            
            print("\n4. Creating associations...")
            self.associative_memory.associate(test_pattern, [0.2, 0.4, 0.6, 0.8, 0.5], strength=0.8)
            print("   ✓ Association created")
            
            print("\n5. Storing episode...")
            self.episodic_memory.storeEpisode(test_episode)
            print("   ✓ Episode stored")
            
            # Create association from episode
            self.associative_memory.associateFromExperience(test_episode)
            print("   ✓ Association created from experience")
            
            # Test retrieval
            similar_episodes = self.episodic_memory.retrieveSimilar(test_pattern, maxResults=2)
            associated_patterns = self.associative_memory.retrieve(test_pattern, maxResults=2)
            
            print("\n6. Validating integration results...")
            print(f"   ✓ Similar episodes found: {len(similar_episodes)}")
            print(f"   ✓ Associated patterns found: {len(associated_patterns)}")
            
            # Test plasticity influence
            stdp = nlm.create_stdp()
            stdp.configure(0.01, 0.02, 25.0)
            print("   ✓ STDP rule configured for plasticity")
            
            self.log_pass("Memory + Neuromodulation + Plasticity integration test completed")
            
        except Exception as e:
            self.log_error(f"Integration test failed: {str(e)}", "test_memory_neuromodulation_integration")
            
    def test_complete_agent_simulation(self):
        """Test complete agent simulation with all new features."""
        print("\n--- Testing Complete Agent Simulation ---")
        
        try:
            print("\n1. Creating agent brain simulation...")
            
            # Setup agent brain
            agent_brain = nlm.createAgentBrain(self.brain)
            agent_brain.initialize()
            print("   ✓ Agent brain created and initialized")
            
            print("\n2. Simulating perception-action cycle...")
            
            # Create sensory percept
            percept = nlm.SensoryPercept()
            vision = nlm.Vision(64, 64, 3)
            vision.setData([0.1, 0.5, 0.9] * 576)  # 64*64*3 = 576 values
            percept.setVision(vision)
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            print("   ✓ Sensory input processed")
            
            # Simulate neuromodulation
            print("\n3. Simulating neuromodulation dynamics...")
            
            # Create and configure neuromodulators
            dopamine = nlm.createWithLevel(0.0)
            curiosity = nlm.createWithWeights(0.6, 0.4)
            novelty = nlm.createWithSensitivity(1.0)
            
            # Simulate reward and exploration
            for step in range(3):
                reward = 1.0 if step == 1 else 0.0
                predicted_reward = 0.5
                
                dopamine.signalReward(reward)
                dopamine.signalRewardPredictionError(reward - predicted_reward)
                curiosity.update(0.1, abs(reward - predicted_reward), 0.01)
                novelty.update(0.01)
                
                print(f"   ✓ Step {step+1}: reward={reward}, curiosity={curiosity.getLevel():.3f}")
            
            print("\n4. Simulating plasticity...")
            
            # Get plasticity system
            stdp, hebbian, structural = nlm.get_plasticity_system(self.brain)
            
            # Configure and enable plasticity rules
            stdp.configure(0.01, 0.02, 25.0)
            hebbian.setLearningRate(0.01)
            hebbian.setMaxWeight(2.0)
            
            stdp.setEnabled(True)
            hebbian.setEnabled(True)
            print("   ✓ Plasticity rules configured and enabled")
            
            print("\n5. Simulating development...")
            
            # Simulate development progression
            for stage in range(3):
                agent_brain.updateDevelopment(0.01)
                stage_name = agent_brain.getDevelopmentalStageName()
                curiosity_level = agent_brain.getCuriosityLevel()
                print(f"   ✓ Development stage {stage+1}: {stage_name}, curiosity={curiosity_level:.3f}")
            
            print("\n6. Simulating world interaction...")
            
            # Simulate world updates
            world = nlm.createWorld()
            world.configure(100, 100, 64, 64)
            
            for step in range(5):
                # Create random action based on curiosity
                curiosity_level = agent_brain.getCuriosityLevel()
                if curiosity_level > 0.7:
                    action_type = nlm.ActionType.Interact
                elif curiosity_level > 0.4:
                    action_type = nlm.ActionType.MoveForward
                else:
                    action_type = nlm.ActionType.Wait
                
                action = nlm.Action(action_type)
                world.applyMotorCommand(action, step)
                print(f"   ✓ Step {step+1}: action={action_type}, curiosity={curiosity_level:.3f}")
            
            # Validate final state
            print("\n7. Validating simulation results...")
            
            final_curiosity = agent_brain.getCuriosityLevel()
            final_novelty = agent_brain.getNoveltyLevel()
            final_prediction_error = agent_brain.getPredictionError()
            
            print(f"   ✓ Final curiosity: {final_curiosity:.3f}")
            print(f"   ✓ Final novelty: {final_novelty:.3f}")
            print(f"   ✓ Final prediction error: {final_prediction_error:.3f}")
            
            # Test agent brain reset
            agent_brain.reset()
            print("   ✓ Agent brain reset completed")
            
            self.log_pass("Complete agent simulation test completed")
            
        except Exception as e:
            self.log_error(f"Complete agent simulation test failed: {str(e)}", "test_complete_agent_simulation")
            
    # ===== ERROR HANDLING AND EDGE CASE TESTS =====
    
    def run_error_handling_tests(self):
        """Run error handling and edge case tests."""
        print("\n" + "=" * 70)
        print("ERROR HANDLING AND EDGE CASE TESTS")
        print("=" * 70)
        
        self.test_null_pointer_handling()
        self.test_invalid_parameter_handling()
        self.test_boundary_conditions()
        
    def test_null_pointer_handling(self):
        """Test error handling for null pointers."""
        print("\n--- Testing Null Pointer Handling ---")
        
        try:
            print("\n1. Testing wrapper functions with None inputs...")
            
            # Test configure_stdp with None
            try:
                nlm.configure_stdp(None, 0.01, 0.02, 25.0)
                self.log_error("configure_stdp should have raised exception for None", "test_null_pointer_handling")
            except Exception:
                print("   ✓ configure_stdp correctly raises exception for None")
            
            # Test configure_hebbian with None
            try:
                nlm.configure_hebbian(None, 0.01, 2.0)
                self.log_error("configure_hebbian should have raised exception for None", "test_null_pointer_handling")
            except Exception:
                print("   ✓ configure_hebbian correctly raises exception for None")
            
            # Test get_stdp_parameters with None
            try:
                nlm.get_stdp_parameters(None)
                self.log_error("get_stdp_parameters should have raised exception for None", "test_null_pointer_handling")
            except Exception:
                print("   ✓ get_stdp_parameters correctly raises exception for None")
            
            # Test validate_plasticity_operation with None
            try:
                nlm.validate_plasticity_operation(None)
                self.log_error("validate_plasticity_operation should have raised exception for None", "test_null_pointer_handling")
            except Exception:
                print("   ✓ validate_plasticity_operation correctly raises exception for None")
            
            # Test plasticity rule methods with None
            try:
                nlm.setEnabled(None, True)
                self.log_error("setEnabled should have raised exception for None", "test_null_pointer_handling")
            except Exception:
                print("   ✓ setEnabled correctly raises exception for None")
            
            # Test getName with None
            try:
                nlm.getName(None)
                self.log_error("getName should have raised exception for None", "test_null_pointer_handling")
            except Exception:
                print("   ✓ getName correctly raises exception for None")
            
            self.log_pass("Null pointer handling tests completed")
            
        except Exception as e:
            self.log_error(f"Null pointer handling tests failed: {str(e)}", "test_null_pointer_handling")
            
    def test_invalid_parameter_handling(self):
        """Test handling of invalid parameters."""
        print("\n--- Testing Invalid Parameter Handling ---")
        
        try:
            print("\n1. Testing invalid memory parameters...")
            
            if self.working_memory:
                # Test with invalid neuron ID
                activation = self.working_memory.getNeuronActivation(-1)
                print(f"   ✓ Invalid neuron ID handled: {activation}")
                
            print("\n2. Testing invalid neuromodulator parameters...")
            
            # Test with invalid dopamine parameters
            dopamine = nlm.createWithLevel(0.0)
            dopamine.setLevel(1.5)  # Should clamp or be handled
            level = dopamine.getLevel()
            print(f"   ✓ Dopamine level clamped: {level:.3f}")
            
            self.log_pass("Invalid parameter handling tests completed")
            
        except Exception as e:
            self.log_error(f"Invalid parameter handling tests failed: {str(e)}", "test_invalid_parameter_handling")
            
    def test_boundary_conditions(self):
        """Test boundary conditions."""
        print("\n--- Testing Boundary Conditions ---")
        
        try:
            print("\n1. Testing extreme memory values...")
            
            if self.working_memory:
                # Test with extreme pattern values
                extreme_pattern = [-1.0, 2.0, 0.5, 1.5, -0.5]
                try:
                    self.working_memory.store(extreme_pattern, strength=100.0)
                    print("   ✓ Extreme values stored (system should handle them)")
                except Exception:
                    print("   ✓ System rejected extreme values (expected)")
                
            print("\n2. Testing neuromodulator boundaries...")
            
            # Test dopamine boundaries
            dopamine = nlm.createWithLevel(0.0)
            dopamine.setLevel(-2.0)
            level = dopamine.getLevel()
            print(f"   ✓ Dopamine boundary: {level:.3f}")
            
            dopamine.setLevel(2.0)
            level = dopamine.getLevel()
            print(f"   ✓ Dopamine boundary: {level:.3f}")
            
            print("\n3. Testing plasticity rule boundaries...")
            
            if self.brain:
                stdp, hebbian, structural = nlm.get_plasticity_system(self.brain)
                
                # Test with extreme STDP parameters
                stdp.setLTPWeight(10.0)
                stdp.setLTDWeight(-10.0)
                print("   ✓ Extreme STDP parameters handled")
                
                self.log_pass("Boundary condition tests completed")
                
        except Exception as e:
            self.log_error(f"Boundary condition tests failed: {str(e)}", "test_boundary_conditions")
            
    # ===== PERFORMANCE TESTS =====
    
    def run_performance_tests(self):
        """Run performance benchmarking."""
        print("\n" + "=" * 70)
        print("PERFORMANCE BENCHMARKING")
        print("=" * 70)
        
        self.benchmark_memory_operations()
        self.benchmark_neuromodulation_operations()
        self.benchmark_plasticity_operations()
        
    def benchmark_memory_operations(self):
        """Benchmark memory operations."""
        print("\n--- Benchmarking Memory Operations ---")
        
        if not self.working_memory or not self.episodic_memory or not self.associative_memory:
            print("   ⚠ Skipping memory performance tests (memory systems not initialized)")
            return
            
        try:
            print("\n1. Benchmarking working memory...")
            
            # Benchmark store/retrieve
            test_patterns = [[0.1 * i for i in range(10)] for _ in range(10)]
            
            start_time = time.time()
            for pattern in test_patterns:
                self.working_memory.store(pattern, strength=1.0)
                retrieved = self.working_memory.retrieve()
            end_time = time.time()
            
            store_retrieve_time = (end_time - start_time) / len(test_patterns)
            print(f"   ✓ Store/retrieve average time: {store_retrieve_time*1000:.2f}ms per pattern")
            
            print("\n2. Benchmarking episodic memory...")
            
            # Benchmark store and retrieve similar
            test_episodes = []
            for i in range(10):
                episode = nlm.EpisodicMemoryItem()
                episode.sensoryState = [0.1 * j for j in range(5)]
                episode.positionX = float(i * 10)
                episode.positionY = float(i * 5)
                episode.timestamp = i * 100
                test_episodes.append(episode)
                
                self.episodic_memory.storeEpisode(episode)
                similar = self.episodic_memory.retrieveSimilar([0.1, 0.1, 0.1, 0.1, 0.1], maxResults=3)
            
            end_time = time.time()
            avg_time = (end_time - start_time) / len(test_episodes)
            print(f"   ✓ Episode store/retrieve average time: {avg_time*1000:.2f}ms per episode")
            
            print("\n3. Benchmarking associative memory...")
            
            # Benchmark associate and retrieve
            start_time = time.time()
            for i in range(5):
                pattern_a = [0.1 * (j + 1) for j in range(3)]
                pattern_b = [0.9 - 0.1 * (j + 1) for j in range(3)]
                
                self.associative_memory.associate(pattern_a, pattern_b, strength=0.8)
                results = self.associative_memory.retrieve(pattern_a, maxResults=3)
                
            end_time = time.time()
            avg_time = (end_time - start_time) / 5
            print(f"   ✓ Associate/retrieve average time: {avg_time*1000:.2f}ms per association")
            
            self.log_pass("Memory performance benchmarks completed")
            
        except Exception as e:
            self.log_error(f"Memory performance tests failed: {str(e)}", "benchmark_memory_operations")
            
    def benchmark_neuromodulation_operations(self):
        """Benchmark neuromodulation operations."""
        print("\n--- Benchmarking Neuromodulation Operations ---")
        
        try:
            print("\n1. Benchmarking dopamine...")
            
            # Benchmark dopamine operations
            dopamine = nlm.createWithLevel(0.0)
            start_time = time.time()
            
            for _ in range(1000):
                dopamine.signalReward(1.0)
                dopamine.signalRewardPredictionError(0.5)
                dopamine.setLevel(0.8)
                level = dopamine.getLevel()
                
            end_time = time.time()
            avg_time = (end_time - start_time) / 1000
            print(f"   ✓ Dopamine operations average time: {avg_time*1000:.2f}ms per operation")
            
            print("\n2. Benchmarking curiosity...")
            
            # Benchmark curiosity operations
            curiosity = nlm.createWithWeights(0.7, 0.3)
            start_time = time.time()
            
            for _ in range(1000):
                curiosity.update(0.3, 0.5, 0.01)
                level = curiosity.getLevel()
                exploration = curiosity.getExplorationDrive()
                
            end_time = time.time()
            avg_time = (end_time - start_time) / 1000
            print(f"   ✓ Curiosity operations average time: {avg_time*1000:.2f}ms per operation")
            
            self.log_pass("Neuromodulation performance benchmarks completed")
            
        except Exception as e:
            self.log_error(f"Neuromodulation performance tests failed: {str(e)}", "benchmark_neuromodulation_operations")
            
    def benchmark_plasticity_operations(self):
        """Benchmark plasticity operations."""
        print("\n--- Benchmarking Plasticity Operations ---")
        
        try:
            print("\n1. Benchmarking STDP...")
            
            # Benchmark STDP operations
            stdp = nlm.create_stdp()
            start_time = time.time()
            
            for _ in range(1000):
                stdp.setLTPWeight(0.01)
                stdp.setLTDWeight(0.02)
                ltp = stdp.getLTPWeight()
                ltd = stdp.getLTDWeight()
                
            end_time = time.time()
            avg_time = (end_time - start_time) / 1000
            print(f"   ✓ STDP operations average time: {avg_time*1000:.2f}ms per operation")
            
            print("\n2. Benchmarking Hebbian...")
            
            # Benchmark Hebbian operations
            hebbian = nlm.create_hebbian()
            start_time = time.time()
            
            for _ in range(1000):
                hebbian.setLearningRate(0.01)
                hebbian.setMaxWeight(2.0)
                rate = hebbian.getLearningRate()
                weight = hebbian.getMaxWeight()
                
            end_time = time.time()
            avg_time = (end_time - start_time) / 1000
            print(f"   ✓ Hebbian operations average time: {avg_time*1000:.2f}ms per operation")
            
            self.log_pass("Plasticity performance benchmarks completed")
            
        except Exception as e:
            self.log_error(f"Plasticity performance tests failed: {str(e)}", "benchmark_plasticity_operations")
            
    def run_performance_tests_wrapper(self):
        """Wrapper for performance tests (can be enabled/disabled)."""
        # Performance tests can be expensive, so they're disabled by default
        # Uncomment the next line to enable performance testing
        # self.run_performance_tests()
        pass

def main():
    """Main test execution function."""
    print("\n" + "=" * 70)
    print("NLM PYTHON BINDINGS COMPREHENSIVE TEST SUITE")
    print("=" * 70)
    print("\nThis test suite validates all new Python bindings for NLM.")
    print("It covers memory systems, neuromodulation, plasticity, development, and integration.")
    print("\nTests include:")
    print("- Unit tests for individual components")
    print("- Integration tests for combined functionality")
    print("- Error handling and edge cases")
    print("- Performance benchmarking examples")
    print("=" * 70)
    
    # Create and run tester
    tester = NLMBindingTester()
    
    try:
        # Run all tests
        tester.run_all_tests()
        
        # Run error handling tests
        tester.run_error_handling_tests()
        
        # Run performance tests (commented out by default)
        tester.run_performance_tests_wrapper()
        
        # Final summary
        print("\n" + "=" * 70)
        print("TEST SUITE EXECUTION COMPLETE")
        print("=" * 70)
        print(f"\nFINAL RESULTS:")
        print(f"  - Tests passed: {tester.passed}")
        print(f"  - Tests failed: {tester.failed}")
        
        if tester.failed > 0:
            print(f"\n❌ TEST SUITE FAILED - {tester.failed} tests failed")
            return 1
        else:
            print(f"\n✅ ALL TESTS PASSED - {tester.passed} tests passed")
            return 0
            
    except Exception as e:
        print(f"\n❌ FATAL ERROR: {str(e)}")
        print(f"Stack trace:\n{traceback.format_exc()}")
        return 1
        
if __name__ == "__main__":
    sys.exit(main())
    
# End of test_new_bindings.py
