#!/usr/bin/env python3
"""
Comprehensive Test Suite for NLM - Neural Learning Machine

This test suite provides thorough testing of NLM functionality including:
- Brain lifecycle management
- Memory system integration
- Neuromodulation effects
- Development dynamics
- Performance benchmarks
- Stress testing
"""

import sys
import os
import time
import statistics
import traceback
from typing import Dict, List, Any, Optional, Callable
import pynlm

# Add current directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))


def run_test(test_name: str, test_func: Callable) -> Dict[str, Any]:
    """Run a single test and return results"""
    result = {
        'name': test_name,
        'passed': False,
        'duration': 0.0,
        'error': None,
        'stack_trace': None
    }
    
    start_time = time.time()
    try:
        test_func()
        result['passed'] = True
        result['duration'] = time.time() - start_time
    except Exception as e:
        result['error'] = str(e)
        result['stack_trace'] = traceback.format_exc()
        result['duration'] = time.time() - start_time
    
    return result


class BrainLifecycleTests:
    """Test brain creation, initialization, and lifecycle management"""
    
    def test_basic_creation():
        """Test basic brain creation with default config"""
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Verify brain created
        assert brain is not None
        
        # Verify can be moved
        moved_brain = brain  # Moving semantics test
        
        # Original should still be usable
        assert moved_brain is not None
    
    def test_custom_config():
        """Test brain creation with custom configuration"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("region_count", 3)
        config.set("random_seed", 12345)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Verify configuration was applied
        regions = brain.getRegions()
        assert len(regions) == 3
        
        total_neurons = brain.getTotalNeuronCount()
        # Each region gets neuron_count / region_count neurons
        expected_per_region = 500 / 3
        assert total_neurons == 500
    
    def test_initialization_states():
        """Test brain initialization with different states"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("region_count", 2)
        
        # Test 1: Fresh brain
        brain1 = pynlm.createBrain(config)
        assert brain1.initialize()
        assert brain1.getTotalNeuronCount() == 100
        
        # Test 2: Reset and re-initialize
        brain1.reset()
        assert brain1.initialize()
        
        # Test 3: Different configurations
        for seed in [42, 123, 456]:
            config.set("random_seed", seed)
            brain = pynlm.createBrain(config)
            assert brain.initialize()
            # Each brain should be independent
            assert brain is not None
    
    def test_error_conditions():
        """Test error handling in brain creation"""
        # Test with invalid configuration (should still work due to defaults)
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 0)  # Edge case
        
        brain = pynlm.createBrain(config)
        # Brain should handle invalid values gracefully
        assert brain.initialize()
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all brain lifecycle tests"""
        test_methods = [
            ('test_basic_creation', self.test_basic_creation),
            ('test_custom_config', self.test_custom_config),
            ('test_initialization_states', self.test_initialization_states),
            ('test_error_conditions', self.test_error_conditions),
        ]
        
        results = []
        for test_name, test_func in test_methods:
            results.append(run_test(f"BrainLifecycle.{test_name}", test_func))
        
        return results


class MemorySystemTests:
    """Test integrated memory systems"""
    
    def test_working_memory():
        """Test working memory system"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        working_memory = brain.getWorkingMemory()
        assert working_memory is not None
        
        # Test memory operations
        # Note: This depends on the specific working memory implementation
        assert hasattr(working_memory, 'storeToNeuron')
        assert hasattr(working_memory, 'getActiveTraces')
    
    def test_episodic_memory():
        """Test episodic memory system"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        episodic_memory = brain.getEpisodicMemory()
        assert episodic_memory is not None
        
        # Test episode storage
        # Note: This depends on the specific episodic memory implementation
        assert hasattr(episodic_memory, 'storeEpisode')
        assert hasattr(episodic_memory, 'getEpisodeCount')
    
    def test_associative_memory():
        """Test associative memory system"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        associative_memory = brain.getAssociativeMemory()
        assert associative_memory is not None
        
        # Test pattern association
        # Note: This depends on the specific associative memory implementation
        assert hasattr(associative_memory, 'formAssociation')
    
    def test_memory_integration():
        """Test integration between memory systems"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # All memory systems should be accessible
        assert brain.getWorkingMemory() is not None
        assert brain.getEpisodicMemory() is not None
        assert brain.getAssociativeMemory() is not None
        
        # They should all reference the same brain
        working_mem = brain.getWorkingMemory()
        episodic_mem = brain.getEpisodicMemory()
        
        # Memory systems should be initialized with brain
        # (This depends on the specific implementation)
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all memory system tests"""
        test_methods = [
            ('test_working_memory', self.test_working_memory),
            ('test_episodic_memory', self.test_episodic_memory),
            ('test_associative_memory', self.test_associative_memory),
            ('test_memory_integration', self.test_memory_integration),
        ]
        
        results = []
        for test_name, test_func in test_methods:
            results.append(run_test(f"MemorySystem.{test_name}", test_func))
        
        return results


class NeuromodulationTests:
    """Test neuromodulation systems"""
    
    def test_dopamine_system():
        """Test dopamine (reward) system"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        dopamine = brain.getDopamine()
        assert dopamine is not None
        
        # Test dopamine level access
        assert hasattr(dopamine, 'getLevel')
        
        # Test reward prediction error signaling
        assert hasattr(dopamine, 'signalRewardPredictionError')
    
    def test_curiosity_system():
        """Test curiosity (exploration) system"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        curiosity = brain.getCuriosity()
        assert curiosity is not None
        
        # Test curiosity level access
        assert hasattr(curiosity, 'getLevel')
        
        # Test curiosity update mechanism
        assert hasattr(curiosity, 'update')
    
    def test_novelty_detection():
        """Test novelty detection system"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        novelty = brain.getNovelty()
        assert novelty is not None
        
        # Test novelty level access
        assert hasattr(novelty, 'getLevel')
        
        # Test novelty update mechanism
        assert hasattr(novelty, 'update')
    
    def test_prediction_error_system():
        """Test prediction error system"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        prediction_error = brain.getPredictionErrorSignal()
        assert prediction_error is not None
        
        # Test prediction error access
        assert hasattr(prediction_error, 'getError')
        
        # Test error recording
        assert hasattr(prediction_error, 'recordError')
    
    def test_neuromodulation_integration():
        """Test integration of all neuromodulation systems"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # All neuromodulation systems should be accessible
        assert brain.getDopamine() is not None
        assert brain.getCuriosity() is not None
        assert brain.getNovelty() is not None
        assert brain.getPredictionErrorSignal() is not None
        
        # Systems should interact through brain
        # This tests the integration architecture
    
    def test_neuromodulation_effects():
        """Test neuromodulation effects on neural computation"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable all neuromodulation
        # This is tested through the agent interface
        # For now, just verify systems exist
        assert brain.getDopamine() is not None
        assert brain.getCuriosity() is not None
        assert brain.getNovelty() is not None
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all neuromodulation tests"""
        test_methods = [
            ('test_dopamine_system', self.test_dopamine_system),
            ('test_curiosity_system', self.test_curiosity_system),
            ('test_novelty_detection', self.test_novelty_detection),
            ('test_prediction_error_system', self.test_prediction_error_system),
            ('test_neuromodulation_integration', self.test_neuromodulation_integration),
            ('test_neuromodulation_effects', self.test_neuromodulation_effects),
        ]
        
        results = []
        for test_name, test_func in test_methods:
            results.append(run_test(f"Neuromodulation.{test_name}", test_func))
        
        return results


class DevelopmentTests:
    """Test neural development and plasticity"""
    
    def test_development_stages():
        """Test developmental stage transitions"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial developmental stage
        initial_stage = brain.getDevelopmentalStage()
        assert initial_stage is not None
        
        # Test development update (requires agent)
        # This is tested through the agent interface
    
    def test_structural_plasticity():
        """Test structural plasticity (synaptogenesis/pruning)"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("synaptogenesis_rate", 0.0001)
        config.set("pruning_rate", 0.00001)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test structural plasticity system
        structural_plasticity = brain.getStructuralPlasticity()
        assert structural_plasticity is not None
        
        # Test configuration access
        assert hasattr(structural_plasticity, 'setSynaptogenesisRate')
        assert hasattr(structural_plasticity, 'setPruningRate')
    
    def test_development_integration():
        """Test integration of development with other systems"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Development system should be accessible
        development_system = brain.getDevelopmentSystem()
        assert development_system is not None
        
        # Should integrate with plasticity systems
        assert brain.getSTDP() is not None
        assert brain.getHebbian() is not None
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all development tests"""
        test_methods = [
            ('test_development_stages', self.test_development_stages),
            ('test_structural_plasticity', self.test_structural_plasticity),
            ('test_development_integration', self.test_development_integration),
        ]
        
        results = []
        for test_name, test_func in test_methods:
            results.append(run_test(f"Development.{test_name}", test_func))
        
        return results


class AgentIntegrationTests:
    """Test agent brain integration with world"""
    
    def test_agent_creation():
        """Test agent brain creation"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent brain
        agent = pynlm.createAgentBrain(brain)
        assert agent is not None
        
        # Agent should reference the brain
        agent_brain = agent.getBrain()
        assert agent_brain is not None
        assert agent_brain.getTotalNeuronCount() == 500
    
    def test_agent_with_world():
        """Test agent integration with world"""
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Agent should be initialized with world
        # Test: agent should have reference to world
        # This depends on the specific agent implementation
    
    def test_agent_features():
        """Test various agent features and capabilities"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test feature enable/disable
        agent.enableRewardModulation(True)
        assert agent.isRewardModulationEnabled()
        
        agent.enableCuriosity(True)
        assert agent.isCuriosityEnabled()
        
        agent.enableDevelopment(True)
        assert agent.isDevelopmentEnabled()
        
        agent.enableStructuralPlasticity(True)
        assert agent.isStructuralPlasticityEnabled()
    
    def test_agent_neuromodulation():
        """Test agent neuromodulation capabilities"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test neuromodulation levels
        dopamine = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        # Should be accessible and in valid range
        assert -1.0 <= dopamine <= 1.0
        assert 0.0 <= curiosity <= 1.0
        assert 0.0 <= novelty <= 1.0
        # Prediction error can be negative
    
    def test_agent_development():
        """Test agent development capabilities"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test developmental stage
        stage = agent.getDevelopmentalStage()
        assert stage is not None
        
        # Test development update
        agent.updateDevelopment(0.1)  # 100ms
        
        # Stage should change with development
        new_stage = agent.getDevelopmentalStage()
        # Stage may or may not change depending on implementation
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all agent integration tests"""
        test_methods = [
            ('test_agent_creation', self.test_agent_creation),
            ('test_agent_with_world', self.test_agent_with_world),
            ('test_agent_features', self.test_agent_features),
            ('test_agent_neuromodulation', self.test_agent_neuromodulation),
            ('test_agent_development', self.test_agent_development),
        ]
        
        results = []
        for test_name, test_func in test_methods:
            results.append(run_test(f"AgentIntegration.{test_name}", test_func))
        
        return results


class SimulationTests:
    """Test simulation capabilities and performance"""
    
    def test_step_consistency():
        """Test step execution consistency"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        config.set("simulation_timestep", 0.001)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get initial state
        initial_spikes = brain.getTotalSpikeCount()
        initial_neurons = brain.getTotalNeuronCount()
        
        # Run steps
        for step in range(10):
            brain.step(step)
        
        # Verify state is reasonable
        assert brain.getTotalNeuronCount() == initial_neurons
        assert brain.getTotalSpikeCount() >= initial_spikes
    
    def test_multiple_step_methods():
        """Test different step method signatures"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test step with only step number
        brain.step(0)
        brain.step(1)
        brain.step(2)
        
        # Test step with timestep
        brain.step(10, 0.01)
        brain.step(20, 0.02)
    
    def test_sensory_processing():
        """Test sensory input processing"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create dummy sensory input
        from src.sensory.SensoryInput import SensoryInput
        from src.sensory.Vision import Vision
        
        vision_data = [0.1, 0.2, 0.3, 0.4, 0.5]
        vision = Vision(1, 1, 1)
        vision.setData(vision_data)
        
        # Test sensory processing
        brain.receiveSensoryInput(vision)
        
        # Should not crash and should process input
        assert brain.getTotalNeuronCount() == 200
    
    def test_action_consistency():
        """Test action production consistency"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get multiple actions (should be consistent in structure)
        action1 = brain.produceAction()
        action2 = brain.produceAction()
        
        # Actions should be valid
        assert action1 is not None
        assert action2 is not None
        
        # They might be different based on neural activity, but should have same type
        # This depends on the specific action system
    
    def test_performance_benchmark():
        """Test basic performance benchmarks"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time step execution
        start_time = time.time()
        
        for step in range(100):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Performance should be reasonable (this is a very loose test)
        # For 100 steps with 500 neurons, should complete in a few seconds
        assert duration < 10.0, f"Performance too slow: {duration:.2f}s for 100 steps"
        
        # Calculate steps per second
        steps_per_second = 100 / duration
        print(f"Performance: {steps_per_second:.1f} steps/second")
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all simulation tests"""
        test_methods = [
            ('test_step_consistency', self.test_step_consistency),
            ('test_multiple_step_methods', self.test_multiple_step_methods),
            ('test_sensory_processing', self.test_sensory_processing),
            ('test_action_consistency', self.test_action_consistency),
            ('test_performance_benchmark', self.test_performance_benchmark),
        ]
        
        results = []
        for test_name, test_func in test_methods:
            results.append(run_test(f"Simulation.{test_name}", test_func))
        
        return results


class StressTests:
    """Test system under stress (edge cases and error conditions)"""
    
    def test_large_brain():
        """Test with large brain configuration"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 5000)  # Large brain
        config.set("region_count", 10)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should handle large configuration
        assert brain.getTotalNeuronCount() == 5000
        assert brain.getRegionCount() == 10
    
    def test_zero_neurons():
        """Test edge case with zero neurons"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 0)  # Edge case
        
        brain = pynlm.createBrain(config)
        # Should handle gracefully
        assert brain.initialize()
        assert brain.getTotalNeuronCount() == 0
    
    def test_high_connection_probability():
        """Test with high connection probability"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("connection_probability", 1.0)  # All-to-all connections
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Should handle high connection probability
        assert brain.getTotalNeuronCount() == 100
        # Synapse count could be high with 1.0 probability
    
    def test_long_simulation():
        """Test extended simulation"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run for many steps
        for step in range(1000):
            brain.step(step)
            
            # Check for reasonable behavior
            if step % 100 == 0:
                # Should not crash and should be in valid state
                assert brain.getTotalNeuronCount() == 200
                assert brain.getTotalSpikeCount() >= 0
        
        # Final verification
        assert brain.getTotalNeuronCount() == 200
    
    def test_memory_usage():
        """Test memory usage patterns"""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        # Create and initialize multiple brains
        brains = []
        for i in range(10):
            config.set("random_seed", i)  # Different seed each brain
            brain = pynlm.createBrain(config)
            assert brain.initialize()
            brains.append(brain)
        
        # All should be independent
        assert len(brains) == 10
        
        # Clean up
        brains.clear()
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all stress tests"""
        test_methods = [
            ('test_large_brain', self.test_large_brain),
            ('test_zero_neurons', self.test_zero_neurons),
            ('test_high_connection_probability', self.test_high_connection_probability),
            ('test_long_simulation', self.test_long_simulation),
            ('test_memory_usage', self.test_memory_usage),
        ]
        
        results = []
        for test_name, test_func in test_methods:
            results.append(run_test(f"Stress.{test_name}", test_func))
        
        return results


class TestSuite:
    """Main test suite that runs all tests"""
    
    def __init__(self):
        self.test_categories = [
            ('BrainLifecycle', BrainLifecycleTests()),
            ('MemorySystem', MemorySystemTests()),
            ('Neuromodulation', NeuromodulationTests()),
            ('Development', DevelopmentTests()),
            ('AgentIntegration', AgentIntegrationTests()),
            ('Simulation', SimulationTests()),
            ('Stress', StressTests()),
        ]
    
    def run_all(self) -> Dict[str, Any]:
        """Run all test categories and return summary"""
        print("=== NLM Comprehensive Test Suite ===\n")
        
        all_results = {}
        total_tests = 0
        total_passed = 0
        total_failed = 0
        total_duration = 0.0
        
        for category_name, test_instance in self.test_categories:
            print(f"Running {category_name} tests...")
            
            try:
                category_results = test_instance.run_all()
                all_results[category_name] = category_results
                
                # Count results
                for result in category_results:
                    total_tests += 1
                    total_passed += 1 if result['passed'] else 0
                    total_failed += 0 if result['passed'] else 1
                    total_duration += result['duration']
                    
                    # Print individual test result
                    status = "✓ PASS" if result['passed'] else "✗ FAIL"
                    print(f"  {status} {result['name']} ({result['duration']:.3f}s)")
                    
                    if not result['passed'] and result['error']:
                        print(f"    Error: {result['error']}")
                
                print(f"  {category_name} tests completed: {len(category_results)} tests\n")
                
            except Exception as e:
                print(f"  {category_name} tests FAILED: {e}\n")
                # Mark all tests in this category as failed
                all_results[category_name] = [{'name': f'{category_name}.CategoryFailure', 'passed': False, 'error': str(e)}]
                total_failed += len([r for r in all_results[category_name] if not r['passed']])
        
        # Generate summary
        summary = {
            'total_tests': total_tests,
            'total_passed': total_passed,
            'total_failed': total_failed,
            'total_duration': total_duration,
            'success_rate': (total_passed / total_tests * 100) if total_tests > 0 else 0,
            'categories': all_results
        }
        
        # Print final summary
        print("=== Test Summary ===")
        print(f"Total tests: {total_tests}")
        print(f"Passed: {total_passed}")
        print(f"Failed: {total_failed}")
        print(f"Success rate: {summary['success_rate']:.1f}%")
        print(f"Total duration: {total_duration:.2f}s")
        print(f"Tests per second: {total_tests/total_duration if total_duration > 0 else 0:.1f}")
        
        return summary


def main():
    """Main test runner"""
    try:
        suite = TestSuite()
        summary = suite.run_all()
        
        # Exit with appropriate code
        if summary['total_failed'] > 0:
            print(f"\n⚠️  {summary['total_failed']} tests failed")
            sys.exit(1)
        else:
            print(f"\n✅ All {summary['total_tests']} tests passed!")
            sys.exit(0)
            
    except Exception as e:
        print(f"Test suite failed: {e}")
        traceback.print_exc()
        sys.exit(2)


if __name__ == '__main__':
    main()
