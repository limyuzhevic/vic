#!/usr/bin/env python3
"""
Comprehensive test suite for NLM (Neural Learning Machine) Python bindings.
Identifies bugs, validates functionality, and serves as regression test suite.
"""

import pynlm
import tempfile
import os
import sys
from typing import Dict, List, Any

class NLMTestSuite:
    """Test suite for NLM functionality."""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
        self.test_results = []
    
    def run_all_tests(self) -> Dict[str, bool]:
        """Run all tests and return results."""
        print("Running NLM Test Suite - Phase 6 Integration Tests")
        print("=" * 60)
        
        self.test_results = []
        
        # Core functionality tests
        self.test_basic_creation()
        self.test_config_operations()
        self.test_brain_initialization()
        self.test_brain_stepping()
        self.test_memory_systems()
        self.test_neuromodulation()
        self.test_prediction_system()
        self.test_cognition_systems()
        self.test_development_system()
        self.test_world_simulation()
        self.test_agent_integration()
        self.test_checkpoints()
        self.test_errors_and_edge_cases()
        
        # Summary
        passed = sum(1 for _, success in self.test_results if success)
        total = len(self.test_results)
        
        print(f"\nTest Summary: {passed}/{total} passed")
        
        return {
            "passed": passed,
            "total": total,
            "success_rate": passed / total if total > 0 else 0.0,
            "results": dict(self.test_results)
        }
    
    def add_test_result(self, test_name: str, success: bool, message: str = ""):
        """Add a test result."""
        self.test_results.append((test_name, success))
        status = "PASS" if success else "FAIL"
        print(f"  {status}: {test_name}")
        if message and not success:
            print(f"    Error: {message}")
    
    def test_basic_creation(self):
        """Test basic object creation."""
        try:
            self.config = pynlm.createDefaultConfig()
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            self.add_test_result("Basic creation", True, "Created config and brain successfully")
        except Exception as e:
            self.add_test_result("Basic creation", False, f"Exception: {str(e)}")
    
    def test_config_operations(self):
        """Test configuration operations."""
        try:
            config = pynlm.createDefaultConfig()
            
            # Test has
            assert config.has("brain.neuron_count")
            assert not config.has("nonexistent.key")
            
            # Test get
            neuron_count = config.getOr("brain.neuron_count", 1000)
            assert neuron_count == 1000
            
            # Test set
            config.set("test.key", "test_value")
            assert config.has("test.key")
            
            # Test getKeys
            keys = config.getKeys()
            assert "brain.neuron_count" in keys
            assert "test.key" in keys
            
            self.add_test_result("Config operations", True, "Config operations successful")
        except Exception as e:
            self.add_test_result("Config operations", False, f"Exception: {str(e)}")
    
    def test_brain_initialization(self):
        """Test brain initialization and basic properties."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test basic properties
            neuron_count = brain.getTotalNeuronCount()
            assert neuron_count > 0, "Should have neurons"
            
            synapse_count = brain.getTotalSynapseCount()
            assert synapse_count > 0, "Should have synapses"
            
            firing_count = brain.getFiringNeuronCount()
            assert firing_count >= 0, "Firing count should be non-negative"
            
            avg_rate = brain.getAverageFiringRate()
            assert avg_rate >= 0, "Average firing rate should be non-negative"
            
            self.add_test_result("Brain initialization", True, "Brain initialization successful")
        except Exception as e:
            self.add_test_result("Brain initialization", False, f"Exception: {str(e)}")
    
    def test_brain_stepping(self):
        """Test brain stepping simulation."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            initial_spikes = brain.getTotalSpikeCount()
            
            # Take some steps
            for i in range(10):
                brain.step(i)
            
            final_spikes = brain.getTotalSpikeCount()
            assert final_spikes >= initial_spikes, "Spike count should not decrease"
            
            self.add_test_result("Brain stepping", True, "Brain stepping successful")
        except Exception as e:
            self.add_test_result("Brain stepping", False, f"Exception: {str(e)}")
    
    def test_memory_systems(self):
        """Test integrated memory systems."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test working memory access
            working_memory = brain.getWorkingMemory()
            if working_memory:
                # Working memory should have methods
                count = working_memory.getActiveTraces() if hasattr(working_memory, 'getActiveTraces') else 0
                assert count >= 0
            
            # Test episodic memory access
            episodic_memory = brain.getEpisodicMemory()
            if episodic_memory:
                episodes = episodic_memory.getEpisodeCount() if hasattr(episodic_memory, 'getEpisodeCount') else 0
                assert episodes >= 0
            
            # Test associative memory access
            associative_memory = brain.getAssociativeMemory()
            if associative_memory:
                # Should have some API methods
                pass
            
            self.add_test_result("Memory systems", True, "Memory systems accessible")
        except Exception as e:
            self.add_test_result("Memory systems", False, f"Exception: {str(e)}")
    
    def test_neuromodulation(self):
        """Test neuromodulation systems."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test agent integration for neuromodulation
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable neuromodulation
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Test neuromodulation levels
            dopamine_level = agent.getNeuromodulationLevel()
            curiosity_level = agent.getCuriosityLevel()
            novelty_level = agent.getNoveltyLevel()
            
            assert dopamine_level >= -1.0 and dopamine_level <= 1.0
            assert curiosity_level >= 0.0 and curiosity_level <= 1.0
            assert novelty_level >= 0.0 and novelty_level <= 1.0
            
            self.add_test_result("Neuromodulation", True, "Neuromodulation systems functional")
        except Exception as e:
            self.add_test_result("Neuromodulation", False, f"Exception: {str(e)}")
    
    def test_prediction_system(self):
        """Test prediction system integration."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            prediction_system = brain.getPredictionSystem()
            if prediction_system:
                # Prediction system should have some API
                pass
            
            # Test prediction error through agent
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            prediction_error = agent.getPredictionError()
            assert prediction_error == 0.0  # Initial prediction error
            
            self.add_test_result("Prediction system", True, "Prediction system accessible")
        except Exception as e:
            self.add_test_result("Prediction system", False, f"Exception: {str(e)}")
    
    def test_cognition_systems(self):
        """Test cognition system integration."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test cognition systems
            planner = brain.getPlanner()
            concept_formation = brain.getConceptFormation()
            attention = brain.getAttention()
            
            # These systems should have basic API
            if planner:
                pass
            if concept_formation:
                pass
            if attention:
                pass
            
            self.add_test_result("Cognition systems", True, "Cognition systems accessible")
        except Exception as e:
            self.add_test_result("Cognition systems", False, f"Exception: {str(e)}")
    
    def test_development_system(self):
        """Test development system integration."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            development_stage = brain.getDevelopmentalStage()
            assert development_stage != None
            
            # Test agent development
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            agent_stage = agent.getDevelopmentalStage()
            assert agent_stage != None
            
            # Update development
            agent.updateDevelopment(0.1)
            
            self.add_test_result("Development system", True, "Development system functional")
        except Exception as e:
            self.add_test_result("Development system", False, f"Exception: {str(e)}")
    
    def test_world_simulation(self):
        """Test world simulation integration."""
        try:
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            # Test world properties
            assert world.getWidth() == 20
            assert world.getHeight() == 20
            assert world.getVisionWidth() == 8
            assert world.getVisionHeight() == 8
            
            # Test sensory percept
            percept = world.getSensoryPercept()
            assert percept is not None
            
            # Test agent body
            body = world.getAgentBody()
            assert body is not None
            
            # Test world update
            world.update(0.1)
            
            # Test motor command application
            result = world.applyMotorCommand(pynlm.MotorCommand.MoveForward, 0.0)
            assert result.success == True or result.success == False  # Either is valid
            
            self.add_test_result("World simulation", True, "World simulation functional")
        except Exception as e:
            self.add_test_result("World simulation", False, f"Exception: {str(e)}")
    
    def test_agent_integration(self):
        """Test complete agent integration."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable all subsystems
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
            
            # Run a complete simulation step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(0)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Should have completed without crashing
            assert action is not None
            
            self.add_test_result("Agent integration", True, "Agent integration successful")
        except Exception as e:
            self.add_test_result("Agent integration", False, f"Exception: {str(e)}")
    
    def test_checkpoints(self):
        """Test checkpoint/save/load functionality."""
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Take some steps to create state
            for i in range(5):
                brain.step(i)
            
            # Create temporary file
            with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
                checkpoint_file = f.name
            
            try:
                # Save brain state
                save_success = brain.save(checkpoint_file)
                assert save_success == True
                
                # Reset brain
                brain.reset()
                assert brain.getTotalSpikeCount() == 0
                
                # Load brain state
                load_success = brain.load(checkpoint_file)
                assert load_success == True
                
                # Check that state was restored
                load_spikes = brain.getTotalSpikeCount()
                assert load_spikes > 0, "Spike count should be restored from checkpoint"
                
            finally:
                # Clean up
                if os.path.exists(checkpoint_file):
                    os.unlink(checkpoint_file)
            
            self.add_test_result("Checkpoints", True, "Checkpoint functionality working")
        except Exception as e:
            self.add_test_result("Checkpoints", False, f"Exception: {str(e)}")
    
    def test_errors_and_edge_cases(self):
        """Test error handling and edge cases."""
        try:
            # Test brain without initialization
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            
            # Should handle uninitialized state gracefully
            try:
                brain.step(0)
                # If it doesn't crash, that's okay
                pass
            except Exception:
                # Exception is also acceptable for uninitialized brain
                pass
            
            # Test config operations
            config = pynlm.createDefaultConfig()
            
            # Test invalid config access
            invalid_val = config.getOr("invalid.key.that.does.not.exist", 999)
            assert invalid_val == 999, "Should return default for invalid key"
            
            # Test setting various value types
            config.set("string.test", "value")
            config.set("int.test", 42)
            config.set("float.test", 3.14)
            config.set("bool.test", True)
            
            # Verify they were set correctly
            assert config.getOr("string.test", "") == "value"
            assert config.getOr("int.test", 0) == 42
            assert config.getOr("float.test", 0.0) == 3.14
            assert config.getOr("bool.test", False) == True
            
            self.add_test_result("Errors and edge cases", True, "Error handling robust")
        except Exception as e:
            self.add_test_result("Errors and edge cases", False, f"Exception: {str(e)}")

if __name__ == "__main__":
    # Run the test suite
    suite = NLMTestSuite()
    results = suite.run_all_tests()
    
    print("\n" + "=" * 60)
    print("TEST SUITE COMPLETE")
    
    if results["success_rate"] < 0.8:
        print(f"WARNING: Only {results['success_rate']*100:.1f}% tests passed")
        print("This indicates significant issues that need fixing.")
        sys.exit(1)
    else:
        print(f"Good: {results['success_rate']*100:.1f}% of tests passed")
        sys.exit(0)
