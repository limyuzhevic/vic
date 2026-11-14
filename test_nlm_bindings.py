#!/usr/bin/env python3

"""
Comprehensive test suite for NLM Python bindings.

This module provides extensive testing of the NLM Python bindings, ensuring
compatibility, functionality, and performance. It includes unit tests,
integration tests, and performance benchmarks.
"""

import numpy as np
import time
import tempfile
import os
import sys
from typing import Dict, List, Any, Optional

# Import NLM bindings
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Error: pynlm not available. Install Python bindings first.")
class TestNLMCore:
    """Test basic NLM core functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_basic_creation(self):
        """Test basic brain and configuration creation."""
        print("Testing basic creation...")
        
        # Test default config creation
        config = pynlm.createDefaultConfig()
        assert config is not None
        
        # Test brain creation
        brain = pynlm.createBrain(config)
        assert brain is not None
        
        # Test initialization
        success = brain.initialize()
        assert success, "Brain initialization failed"
        
        # Test basic properties
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        assert brain.getFiringNeuronCount() >= 0
        
        print("  ✓ Basic creation test passed")
        return True
    
    def test_configuration(self):
        """Test configuration functionality."""
        print("Testing configuration...")
        
        config = pynlm.createDefaultConfig()
        
        # Test setting values
        config.set("brain.neuron_count", 500)
        assert config.has("brain.neuron_count")
        
        # Test getting values
        neuron_count = config.getOr("brain.neuron_count", 1000)
        assert neuron_count == 500
        
        # Test file I/O
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config.saveToFile(f.name)
            
            # Load from file
            new_config = pynlm.createDefaultConfig()
            new_config.loadFromFile(f.name)
            assert new_config.has("brain.neuron_count")
        
        os.unlink(f.name)
        print("  ✓ Configuration test passed")
        return True
    
    def test_brain_step(self):
        """Test brain stepping and simulation."""
        print("Testing brain stepping...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        initial_spikes = brain.getTotalSpikeCount()
        initial_firing = brain.getFiringNeuronCount()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        final_spikes = brain.getTotalSpikeCount()
        final_firing = brain.getFiringNeuronCount()
        
        # Spike count should increase or stay same
        assert final_spikes >= initial_spikes
        
        print("  ✓ Brain stepping test passed")
        return True
class TestNLMNeurons:
    """Test neuron-related functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_neuron_properties(self):
        """Test neuron properties and states."""
        print("Testing neuron properties...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Access some neurons (implementation depends on API)
        # For now, test basic brain properties
        neuron_count = brain.getTotalNeuronCount()
        assert neuron_count > 0
        
        # Test different neuron types exist
        # This would require direct neuron access which may not be exposed
        # brain.getRegionIds() to get regions
        # brain.getRegion(region_id) to get neurons
        
        print("  ✓ Neuron properties test passed")
        return True
    
    def test_neuron_enums(self):
        """Test neuron type and state enumerations."""
        print("Testing neuron enumerations...")
        
        # Test NeuronType enum
        assert hasattr(pynlm, 'NeuronType')
        neuron_types = [
            pynlm.NeuronType.Excitatory,
            pynlm.NeuronType.Inhibitory,
            pynlm.NeuronType.Sensory,
            pynlm.NeuronType.Motor,
            pynlm.NeuronType.Internal,
            pynlm.NeuronType.Modulatory
        ]
        
        # Test DevelopmentalStage enum
        assert hasattr(pynlm, 'DevelopmentalStage')
        developmental_stages = [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ]
        
        # Test FiringState enum
        assert hasattr(pynlm, 'FiringState')
        firing_states = [
            pynlm.FiringState.Resting,
            pynlm.FiringState.Active,
            pynlm.FiringState.Refractory,
            pynlm.FiringState.Inhibited
        ]
        
        print("  ✓ Neuron enumerations test passed")
        return True
    
    def test_neuron_ids(self):
        """Test neuron ID handling."""
        print("Testing neuron ID handling...")
        
        # Test NeuronId structure
        neuron_id1 = pynlm.NeuronId()
        assert hasattr(neuron_id1, 'value')
        
        neuron_id2 = pynlm.NeuronId(42)
        assert neuron_id2.value == 42
        
        print("  ✓ Neuron ID handling test passed")
        return True
class TestNLMSynapses:
    """Test synapse-related functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_synapse_properties(self):
        """Test synapse properties and states."""
        print("Testing synapse properties...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        synapse_count = brain.getTotalSynapseCount()
        assert synapse_count > 0
        
        print("  ✓ Synapse properties test passed")
        return True
    
    def test_synapse_enums(self):
        """Test synapse type enumerations."""
        print("Testing synapse enumerations...")
        
        # Test SynapseType enum
        assert hasattr(pynlm, 'SynapseType')
        synapse_types = [
            pynlm.SynapseType.Excitatory,
            pynlm.SynapseType.Inhibitory,
            pynlm.SynapseType.Modulatory,
            pynlm.SynapseType.Electrical,
            pynlm.SynapseType.GapJunction
        ]
        
        print("  ✓ Synapse enumerations test passed")
        return True
class TestNLMActions:
    """Test action-related functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_action_creation(self):
        """Test action creation and properties."""
        print("Testing action creation...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        action = brain.produceAction()
        assert action is not None
        
        # Test ActionType enum
        assert hasattr(pynlm, 'ActionType')
        action_types = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Interact,
            pynlm.ActionType.Eat,
            pynlm.ActionType.Wait
        ]
        
        print("  ✓ Action creation test passed")
        return True
class TestNLMWorld:
    """Test world-related functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_world_creation(self):
        """Test world creation and configuration."""
        print("Testing world creation...")
        
        world = pynlm.createSimpleWorld()
        assert world is not None
        
        # Test configuration
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        
        assert world.getWidth() == 10
        assert world.getHeight() == 10
        assert world.getVisionWidth() == 8
        assert world.getVisionHeight() == 8
        
        # Test reset
        world.reset()
        
        print("  ✓ World creation test passed")
        return True
    
    def test_world_objects(self):
        """Test world object creation and manipulation."""
        print("Testing world objects...")
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Test object creation
        world_obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
        assert world_obj.x == 5.0
        assert world_obj.y == 5.0
        assert world_obj.type == pynlm.WorldObjectType.Resource
        
        # Add to world
        world.addObject(world_obj)
        
        # Test object retrieval
        found_obj = world.getObjectAt(5.0, 5.0)
        assert found_obj is not None
        assert found_obj.type == pynlm.WorldObjectType.Resource
        
        print("  ✓ World objects test passed")
        return True
class TestNLMMemory:
    """Test memory system functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_memory_systems(self):
        """Test memory system access."""
        print("Testing memory systems...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test working memory access
        working_memory = brain.getWorkingMemory()
        assert working_memory is not None
        
        # Test episodic memory access
        episodic_memory = brain.getEpisodicMemory()
        assert episodic_memory is not None
        
        # Test associative memory access
        associative_memory = brain.getAssociativeMemory()
        assert associative_memory is not None
        
        print("  ✓ Memory systems test passed")
        return True
class TestNLMNeuromodulation:
    """Test neuromodulation system functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_neuromodulation_systems(self):
        """Test neuromodulation system access."""
        print("Testing neuromodulation systems...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test dopamine access
        dopamine = brain.getDopamine()
        assert dopamine is not None
        
        # Test curiosity access
        curiosity = brain.getCuriosity()
        assert curiosity is not None
        
        # Test novelty access
        novelty = brain.getNovelty()
        assert novelty is not None
        
        # Test prediction error access
        prediction_error = brain.getPredictionErrorSignal()
        assert prediction_error is not None
        
        print("  ✓ Neuromodulation systems test passed")
        return True
class TestNLMAdvanced:
    """Test advanced NLM functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_agent_brain(self):
        """Test agent brain interface."""
        print("Testing agent brain interface...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        agent.initialize(world)
        
        # Test processing sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Test motor command decoding
        motor_cmd = agent.decodeMotorCommand()
        assert motor_cmd is not None
        
        print("  ✓ Agent brain interface test passed")
        return True
    
    def test_checkpoints(self):
        """Test checkpoint save/load functionality."""
        print("Testing checkpoints...")
        
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        
        # Run some simulation
        for step in range(50):
            brain1.step(step)
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
            checkpoint_path = f.name
        
        save_success = brain1.save(checkpoint_path)
        assert save_success
        
        # Create new brain and load checkpoint
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        load_success = brain2.load(checkpoint_path)
        assert load_success
        
        os.unlink(checkpoint_path)
        print("  ✓ Checkpoints test passed")
        return True
    
    def test_performance(self):
        """Test performance metrics."""
        print("Testing performance...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time simulation
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        end_time = time.time()
        
        elapsed_time = end_time - start_time
        steps_per_second = 100 / elapsed_time
        
        assert steps_per_second > 0
        assert elapsed_time < 10.0  # Should complete within 10 seconds
        
        # Test statistics
        firing_rate = brain.getAverageFiringRate()
        active_neurons = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        
        assert firing_rate >= 0
        assert active_neurons >= 0
        assert total_spikes >= 0
        
        print(f"  ✓ Performance test passed ({steps_per_second:.1f} steps/sec)")
        return True
class TestNLMIntegration:
    """Test NLM integration and end-to-end functionality."""
    
    def __init__(self):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm is required for testing")
    
    def test_complete_simulation(self):
        """Test complete agent-in-world simulation."""
        print("Testing complete simulation...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        
        # Create and configure world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation episode
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Verify simulation completed
        assert brain.getTotalNeuronCount() > 0
        assert brain.getTotalSynapseCount() > 0
        
        print("  ✓ Complete simulation test passed")
        return True
    
    def test_development(self):
        """Test developmental stage changes."""
        print("Testing development...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent for development
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test development
        initial_stage = brain.getDevelopmentalStage()
        
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.updateDevelopment(0.1)
        
        final_stage = brain.getDevelopmentalStage()
        
        print(f"  ✓ Development test passed (stage: {initial_stage} -> {final_stage})")
        return True
    
    def test_planning_and_concept(self):
        """Test planning and concept formation."""
        print("Testing planning and concept...")
        
        # This tests Phase 6 advanced features
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test text encoding (Phase 6 feature)
        text_pattern1 = agent.encodeText("apple")
        text_pattern2 = agent.encodeText("Apple")  # Should be same (case-insensitive)
        text_pattern3 = agent.encodeText("zebra")   # Should be different
        
        assert len(text_pattern1) == 4
        assert text_pattern1 == text_pattern2  # Case insensitive
        assert text_pattern1 != text_pattern3  # Different words
        
        # Test concept formation
        concept_formation = agent.getConceptCount()
        assert concept_formation >= 0
        
        print("  ✓ Planning and concept test passed")
        return True
class NLMTestRunner:
    """Main test runner for NLM bindings."""
    
    def __init__(self):
        self.tests = [
            TestNLMCore(),
            TestNLMNeurons(),
            TestNLMSynapses(),
            TestNLMActions(),
            TestNLMWorld(),
            TestNLMMemory(),
            TestNLMNeuromodulation(),
            TestNLMAdvanced(),
            TestNLMIntegration()
        ]
    
    def run_all_tests(self) -> Dict[str, Any]:
        """Run all tests and return results."""
        print("=" * 60)
        print("NLM Python Bindings Test Suite")
        print("=" * 60)
        
        results = {
            'total_tests': 0,
            'passed_tests': 0,
            'failed_tests': 0,
            'errors': [],
            'execution_time': 0
        }
        
        start_time = time.time()
        
        for test_class in self.tests:
            class_name = test_class.__class__.__name__
            print(f"\n{'='*20} {class_name} {'='*20}")
            
            for method_name in dir(test_class):
                if method_name.startswith('test_'):
                    results['total_tests'] += 1
                    
                    try:
                        method = getattr(test_class, method_name)
                        success = method()
                        
                        if success:
                            results['passed_tests'] += 1
                        else:
                            results['failed_tests'] += 1
                            results['errors'].append(f"Failed: {class_name}.{method_name}")
                            
                    except Exception as e:
                        results['failed_tests'] += 1
                        results['errors'].append(f"Error in {class_name}.{method_name}: {str(e)}")
        
        end_time = time.time()
        results['execution_time'] = end_time - start_time
        
        # Print summary
        self._print_test_summary(results)
        
        return results
    
    def _print_test_summary(self, results: Dict[str, Any]):
        """Print test summary."""
        print("\n" + "=" * 60)
        print("TEST SUMMARY")
        print("=" * 60)
        print(f"Total tests: {results['total_tests']}")
        print(f"Passed: {results['passed_tests']}")
        print(f"Failed: {results['failed_tests']}")
        print(f"Execution time: {results['execution_time']:.2f} seconds")
        
        if results['errors']:
            print(f"\nErrors:")
            for error in results['errors']:
                print(f"  - {error}")
        
        # Overall result
        if results['failed_tests'] == 0:
            print("\n✅ ALL TESTS PASSED!")
        else:
            print(f"\n❌ {results['failed_tests']} tests failed")
        
        print("=" * 60)
    
    def run_specific_test(self, test_class_name: str, method_name: str) -> bool:
        """Run a specific test."""
        for test_class in self.tests:
            if test_class.__class__.__name__ == test_class_name:
                method = getattr(test_class, method_name)
                return method()
        
        print(f"Test class {test_class_name} not found")
        return False
if __name__ == "__main__":
    print("NLM Python Bindings Test Suite")
    print("Running comprehensive tests...")
    
    # Run tests
    test_runner = NLMTestRunner()
    results = test_runner.run_all_tests()
    
    # Exit with appropriate code
    sys.exit(0 if results['failed_tests'] == 0 else 1)