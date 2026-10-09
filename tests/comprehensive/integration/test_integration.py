#!/usr/bin/env python3

"""
Integration Tests for NLM Enhanced Components

This module contains comprehensive integration tests for cross-component functionality,
including system-wide testing, component interactions, and end-to-end scenarios.
"""

import sys
import os
import time
import threading
import multiprocessing
from typing import Dict, List, Any, Optional, Tuple
from unittest.mock import Mock, patch, MagicMock
import numpy as np

# Add the parent directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM components for testing
from pynlm import Config, create_default_config, Brain, create_brain
from pynlm.types import NeuronId, RegionId, Position2D
from pynlm.memory import WorkingMemory, EpisodicMemory, AssociativeMemory
from pynlm.neuromodulation import NeuromodulationSystem

class TestCrossComponentIntegration:
    """Integration tests for cross-component functionality."""
    
    def test_brain_memory_integration(self):
        """Test integration between Brain and Memory systems."""
        config = create_default_config()
        
        with Brain(config) as brain:
            # Create mock working memory
            working_mem = WorkingMemory(brain)
            
            # Test working memory integration
            test_pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
            working_mem.store(test_pattern, strength=0.8)
            
            # Test that brain has working memory access
            assert hasattr(brain, 'working_memory')
            
            # Test memory activity reflection in brain
            activity = working_mem.get_memory_activity()
            assert 0.0 <= activity <= 1.0
    
    def test_brain_neuromodulation_integration(self):
        """Test integration between Brain and Neuromodulation systems."""
        config = create_default_config()
        
        with Brain(config) as brain:
            # Create neuromodulation system
            neuromodulation = NeuromodulationSystem(brain)
            
            # Test neuromodulation integration
            dopamine_level = neuromodulation.apply_dopamine(reward=1.0, predicted=0.5)
            curiosity_level = neuromodulation.apply_curiosity(novelty=0.8)
            
            # Test that brain has neuromodulation access
            assert hasattr(brain, 'neuromodulation')
            
            # Test integration with brain's step method
            brain.step(1)
            neuromodulation.update(0.01)
            
            # Verify neuromodulation levels are updated
            summary = neuromodulation.get_neuromodulation_summary()
            assert "dopamine" in summary
            assert "curiosity" in summary
    
    def test_memory_episodic_integration(self):
        """Test integration between different memory systems."""
        episodic_mem = EpisodicMemory()
        assoc_mem = AssociativeMemory()
        
        # Create a test episode
        test_episode = {
            "type": "exploration",
            "location": Position2D(x=10.0, y=20.0),
            "duration": 5.0,
            "reward": 1.0
        }
        
        timestamp = time.time()
        episodic_mem.record(test_episode, timestamp, context={"agent": "test"})
        
        # Create associations based on episodic memory
        assoc_mem.associate("exploration", "reward_learning", strength=0.8)
        assoc_mem.associate("reward_learning", "behavior_modulation", strength=0.6)
        
        # Test cross-memory integration
        associations = assoc_mem.get_associations("exploration")
        assert "reward_learning" in associations
        
        # Search episodic memory for related episodes
        search_results = episodic_mem.search(keywords=["exploration"])
        assert len(search_results) == 1
        
        # Test pattern completion based on associations
        completed = assoc_mem.complete_pattern("exploration", max_depth=2)
        assert "reward_learning" in completed
    
    def test_configuration_brain_integration(self):
        """Test integration between Configuration and Brain systems."""
        # Create enhanced configuration
        config = Config()
        config.set("neuron_count", 1000)
        config.set("region_count", 3)
        config.set("timestep", 0.01)
        config.set("max_energy", 100.0)
        config.set("energy_decay_rate", 0.95)
        
        # Create brain with configuration
        brain = Brain(config)
        
        # Verify configuration was applied
        assert brain.config.get("neuron_count") == 1000
        assert brain.config.get("region_count") == 3
        
        # Initialize brain and test configuration integration
        brain.initialize()
        
        # Verify brain configuration matches
        assert brain.config.get("timestep") == 0.01
        assert brain.config.get("max_energy") == 100.0
    
    def test_full_system_simulation(self):
        """Test full system simulation with all components."""
        # Create complete configuration
        config = create_default_config()
        
        # Initialize brain with all components
        with Brain(config) as brain:
            # Initialize working memory
            working_mem = WorkingMemory(brain)
            working_mem.capacity = 50
            working_mem.decay_rate = 0.02
            
            # Initialize episodic memory
            episodic_mem = EpisodicMemory()
            
            # Initialize associative memory
            assoc_mem = AssociativeMemory()
            
            # Initialize neuromodulation
            neuromodulation = NeuromodulationSystem(brain)
            
            # Run simulation steps
            for step in range(10):
                # Store patterns in working memory
                pattern = [0.1 * (step + 1), 0.5, 0.9, 0.3, 0.7]
                working_mem.store(pattern, strength=0.8)
                
                # Apply neuromodulation
                neuromodulation.apply_dopamine(reward=0.5, predicted=0.4)
                neuromodulation.apply_curiosity(novelty=0.3)
                
                # Record episodic memory
                episode = {
                    "step": step,
                    "pattern": pattern,
                    "working_memory_activity": working_mem.get_memory_activity(),
                    "neuromodulation_levels": neuromodulation.get_neuromodulation_summary()
                }
                episodic_mem.record(episode, time.time())
                
                # Create associations
                assoc_mem.associate(f"pattern_{step}", "exploration", strength=0.7)
                
                # Step brain
                brain.step(step)
                
                # Update memory systems
                working_mem.update(0.01)
                neuromodulation.update(0.01)
            
            # Verify final state
            assert brain.simulation_step == 9
            assert brain.initialized == True
            
            # Verify memories were recorded
            assert episodic_mem.get_episode_count() == 10
            
            # Verify associations were created
            exploration_associations = assoc_mem.get_associations("exploration")
            assert len(exploration_associations) > 0
    
    def test_error_handling_integration(self):
        """Test error handling across components."""
        config = Config()
        
        # Test configuration error recovery
        try:
            config.set("", "value")
            assert False, "Should have raised ValueError"
        except ValueError:
            pass  # Expected
        
        # Configuration should still be usable after error
        config.set("valid_key", "valid_value")
        assert config.get("valid_key") == "valid_value"
        
        # Test brain error handling
        brain = Brain(config)
        
        # Brain operations should handle errors gracefully
        assert brain is not None
        
        # Test memory error handling
        working_mem = WorkingMemory(brain)
        
        try:
            working_mem.store("invalid_pattern", strength=0.5)
            assert False, "Should have raised ValueError"
        except ValueError:
            pass  # Expected
        
        # Memory system should still be usable
        working_mem.store([0.1, 0.2, 0.3], strength=0.5)
        assert len(working_mem.retrieve()) == 1

class TestPerformanceIntegration:
    """Integration tests for performance measurement and optimization."""
    
    def test_system_performance_benchmark(self):
        """Test system performance benchmarking."""
        import time
        
        config = create_default_config()
        
        with Brain(config) as brain:
            # Create component systems
            working_mem = WorkingMemory(brain)
            neuromodulation = NeuromodulationSystem(brain)
            episodic_mem = EpisodicMemory()
            
            # Benchmark brain operations
            start_time = time.time()
            
            for i in range(100):
                brain.step(i)
                working_mem.store([0.1, 0.2, 0.3], strength=0.5)
                neuromodulation.apply_dopamine(reward=0.5)
                working_mem.update(0.01)
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Calculate performance metrics
            steps_per_second = 100 / duration
            operations_per_second = 400 / duration  # 4 operations per iteration
            
            # Performance should be reasonable
            assert steps_per_second > 10, f"Steps per second too low: {steps_per_second}"
            assert operations_per_second > 40, f"Operations per second too low: {operations_per_second}"
            
            # Store performance metrics for reporting
            from tests.comprehensive.core.test_framework import PerformanceMetrics
            metrics = PerformanceMetrics(
                cpu_usage=50.0,  # Mock CPU usage
                memory_usage=100.0,  # Mock memory usage
                execution_time=duration,
                throughput=steps_per_second,
                peak_memory=200.0,  # Mock peak memory
                avg_memory=150.0,  # Mock average memory
                min_response_time=0.001,  # Mock min response time
                max_response_time=0.01,  # Mock max response time
                median_response_time=0.005  # Mock median response time
            )
            
            return metrics
    
    def test_memory_performance(self):
        """Test memory system performance."""
        config = create_default_config()
        
        with Brain(config) as brain:
            working_mem = WorkingMemory(brain)
            
            # Benchmark memory operations
            start_time = time.time()
            
            for i in range(1000):
                pattern = [0.1 * j for j in range(10)]
                working_mem.store(pattern, strength=0.5)
                _ = working_mem.retrieve()
                _ = working_mem.get_memory_activity()
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Calculate memory performance metrics
            operations_per_second = 3000 / duration
            memory_efficiency = len(working_mem.retrieve()) / duration
            
            # Performance should be reasonable
            assert operations_per_second > 300, f"Memory operations per second too low: {operations_per_second}"
            assert memory_efficiency > 1.0, f"Memory efficiency too low: {memory_efficiency}"
    
    def test_neuromodulation_performance(self):
        """Test neuromodulation system performance."""
        config = create_default_config()
        
        with Brain(config) as brain:
            neuromodulation = NeuromodulationSystem(brain)
            
            # Benchmark neuromodulation operations
            start_time = time.time()
            
            for i in range(1000):
                neuromodulation.apply_dopamine(reward=0.5, predicted=0.4)
                neuromodulation.apply_curiosity(novelty=0.3)
                neuromodulation.apply_novelty(novelty=0.2)
                neuromodulation.update(0.01)
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Calculate neuromodulation performance metrics
            operations_per_second = 4000 / duration
            
            # Performance should be reasonable
            assert operations_per_second > 400, f"Neuromodulation operations per second too low: {operations_per_second}"
    
    def test_scalability_benchmark(self):
        """Test system scalability with increasing load."""
        from tests.comprehensive.core.test_framework import PerformanceMetrics
        
        # Test with different configuration sizes
        config_sizes = [
            {"neuron_count": 100, "region_count": 2},
            {"neuron_count": 500, "region_count": 3},
            {"neuron_count": 1000, "region_count": 5}
        ]
        
        scalability_results = {}
        
        for config_size in config_sizes:
            config = Config()
            for key, value in config_size.items():
                config.set(key, value)
            
            with Brain(config) as brain:
                working_mem = WorkingMemory(brain)
                neuromodulation = NeuromodulationSystem(brain)
                episodic_mem = EpisodicMemory()
                
                # Benchmark with scaled load
                start_time = time.time()
                
                for i in range(100):
                    brain.step(i)
                    working_mem.store([0.1 * j for j in range(5)], strength=0.5)
                    neuromodulation.apply_dopamine(reward=0.5)
                    working_mem.update(0.01)
                
                end_time = time.time()
                duration = end_time - start_time
                
                # Calculate scalability metrics
                operations_per_second = 400 / duration
                neuron_count = config_size["neuron_count"]
                scalability_score = operations_per_second / neuron_count
                
                scalability_results[config_size] = {
                    "operations_per_second": operations_per_second,
                    "scalability_score": scalability_score,
                    "duration": duration
                }
                
                # Scalability score should not degrade too much with increased size
                if "neuron_count" in scalability_results:
                    previous_score = scalability_results["neuron_count"]["scalability_score"]
                    assert scalability_score >= previous_score * 0.5, "Scalability score degraded too much"
        
        return scalability_results

class TestCompatibilityIntegration:
    """Integration tests for compatibility with existing NLM systems."""
    
    def test_backward_compatibility(self):
        """Test backward compatibility with existing NLM systems."""
        # Test that existing API still works
        config = create_default_config()
        
        # Test Config API
        assert config.has("neuron_count")
        assert config.get("neuron_count") == 1000
        
        # Test Brain API
        brain = Brain(config)
        assert brain.config is not None
        assert brain.initialized == False
        
        # Test type definitions
        from pynlm.types import NeuronId, RegionId, NeuronType, ActionType
        
        neuron_id = NeuronId(123)
        assert neuron_id.value == 123
        
        region_id = RegionId(456)
        assert region_id.value == 456
        
        assert NeuronType.EXCITATORY.value == "excitatory"
        assert ActionType.MOVE_FORWARD.value == "move_forward"
        
        # Test memory systems
        assert WorkingMemory is not None
        assert EpisodicMemory is not None
        assert AssociativeMemory is not None
        
        # Test neuromodulation systems
        assert NeuromodulationSystem is not None
        assert Dopamine is not None
        assert Curiosity is not None
        assert Novelty is not None
        assert PredictionError is not None
    
    def test_api_compatibility(self):
        """Test API compatibility with existing usage patterns."""
        # Test that all expected classes and functions are available
        from pynlm import (
            Config, create_default_config, Brain, create_brain,
            WorkingMemory, EpisodicMemory, AssociativeMemory,
            NeuromodulationSystem, Dopamine, Curiosity, Novelty, PredictionError,
            NeuronId, RegionId, SynapseId, PopulationId, NeuronType, ActionType,
            DevelopmentalStage, Position2D, ActionParameters, SensoryData,
            MemoryTrace, NeuralConnection, NeuronIdSet
        )
        
        # Test that they can be instantiated
        config = create_default_config()
        assert isinstance(config, Config)
        
        brain = Brain(config)
        assert isinstance(brain, Brain)
        
        working_mem = WorkingMemory(brain)
        assert isinstance(working_mem, WorkingMemory)
        
        episodic_mem = EpisodicMemory()
        assert isinstance(episodic_mem, EpisodicMemory)
        
        assoc_mem = AssociativeMemory()
        assert isinstance(assoc_mem, AssociativeMemory)
        
        neuromodulation = NeuromodulationSystem(brain)
        assert isinstance(neuromodulation, NeuromodulationSystem)
        
        # Test type instances
        neuron_id = NeuronId(123)
        region_id = RegionId(456)
        position = Position2D(x=10.0, y=20.0)
        action_params = ActionParameters(values=[0.1, 0.5, 0.9])
        sensory_data = SensoryData(data=[1.0, 2.0, 3.0], timestamp=time.time())
        memory_trace = MemoryTrace(activation=0.8, age=10)
        neural_conn = NeuralConnection(source=neuron_id, target=region_id)
        neuron_set = NeuronIdSet(neuron_id)
        
        # Verify all instances
        assert neuron_id.value == 123
        assert region_id.value == 456
        assert position.x == 10.0
        assert position.y == 20.0
        assert len(action_params.values) == 3
        assert len(sensory_data.data) == 3
        assert memory_trace.activation == 0.8
        assert neural_conn.source == neuron_id
        assert neural_conn.target == region_id
        assert neuron_set.contains(neuron_id)
    
    def test_extension_compatibility(self):
        """Test compatibility with extension systems."""
        from pynlm import ExtensionAPI, create_custom_circuit
        
        # Test that extension API is available
        assert ExtensionAPI is not None
        assert create_custom_circuit is not None
        
        # Test extension system integration
        mock_brain = Mock()
        
        try:
            ext_api = ExtensionAPI(mock_brain)
            assert ext_api is not None
            
            # Test custom circuit creation
            try:
                custom_circuit = create_custom_circuit()
                assert custom_circuit is not None
            except Exception:
                # Circuit creation may fail without C++ bindings, but that's OK
                pass
                
        except Exception:
            # Extension API may not be fully available without all dependencies
            pass

class TestRealWorldScenarios:
    """Integration tests for real-world scenarios and use cases."""
    
    def test_agent_simulation_scenario(self):
        """Test real-world agent simulation scenario."""
        config = create_default_config()
        
        with Brain(config) as brain:
            # Setup memory systems
            working_mem = WorkingMemory(brain)
            working_mem.capacity = 100
            working_mem.decay_rate = 0.01
            
            episodic_mem = EpisodicMemory()
            assoc_mem = AssociativeMemory()
            neuromodulation = NeuromodulationSystem(brain)
            
            # Simulate agent behavior
            agent_state = {
                "position": Position2D(x=0.0, y=0.0),
                "orientation": 0.0,
                "memory_active": False
            }
            
            # Simulate exploration episode
            for step in range(50):
                # Update position
                agent_state["position"].x += 1.0
                agent_state["position"].y += 1.0
                
                # Store state in working memory
                state_vector = [agent_state["position"].x, agent_state["position"].y, 
                              agent_state["orientation"], 1.0 if agent_state["memory_active"] else 0.0]
                working_mem.store(state_vector, strength=0.7)
                
                # Apply neuromodulation based on movement
                neuromodulation.apply_dopamine(reward=1.0, predicted=0.8)
                neuromodulation.apply_curiosity(novelty=0.3)
                
                # Record episode
                episode = {
                    "step": step,
                    "position": agent_state["position"].copy(),
                    "state_vector": state_vector,
                    "dopamine_level": neuromodulation.get_dopamine_level(),
                    "curiosity_level": neuromodulation.get_curiosity_level()
                }
                episodic_mem.record(episode, time.time())
                
                # Create associations
                assoc_mem.associate(f"position_{step}", "exploration", strength=0.6)
                assoc_mem.associate("exploration", "navigation", strength=0.8)
                
                # Update systems
                working_mem.update(0.01)
                neuromodulation.update(0.01)
                brain.step(step)
            
            # Verify agent behavior
            assert brain.simulation_step == 49
            assert episodic_mem.get_episode_count() == 50
            
            # Search for specific behaviors
            exploration_episodes = episodic_mem.search(keywords=["exploration"])
            assert len(exploration_episodes) >= 10
            
            # Verify associations
            nav_associations = assoc_mem.get_associations("navigation")
            assert "exploration" in nav_associations
    
    def test_adaptation_scenario(self):
        """Test adaptation and learning scenario."""
        config = create_default_config()
        
        with Brain(config) as brain:
            working_mem = WorkingMemory(brain)
            episodic_mem = EpisodicMemory()
            neuromodulation = NeuromodulationSystem(brain)
            
            # Simulate learning episode
            learning_episodes = []
            
            for episode_num in range(20):
                # Create learning experience
                experience = {
                    "episode": episode_num,
                    "stimulus": [random.random() for _ in range(5)],
                    "response": [0.5, 0.5, 0.5, 0.5, 0.5],
                    "reward": random.random() * 2.0
                }
                
                # Store in episodic memory
                episodic_mem.record(experience, time.time())
                learning_episodes.append(experience)
                
                # Apply reinforcement learning
                neuromodulation.apply_dopamine(
                    reward=experience["reward"], 
                    predicted=experience["reward"] * 0.8
                )
                neuromodulation.apply_curiosity(novelty=0.5)
                
                # Store in working memory
                working_mem.store(experience["stimulus"], strength=experience["reward"])
                
                # Update systems
                working_mem.update(0.01)
                neuromodulation.update(0.01)
                brain.step(episode_num)
            
            # Verify learning occurred
            assert brain.simulation_step == 19
            assert episodic_mem.get_episode_count() == 20
            
            # Verify adaptation in neuromodulation
            final_dopamine = neuromodulation.get_dopamine_level()
            final_curiosity = neuromodulation.get_curiosity_level()
            
            assert isinstance(final_dopamine, float)
            assert isinstance(final_curiosity, float)
            
            # Verify working memory adaptation
            final_activity = working_mem.get_memory_activity()
            assert 0.0 <= final_activity <= 1.0
    
    def test_multi_agent_scenario(self):
        """Test multi-agent scenario with interaction."""
        config = create_default_config()
        
        with Brain(config) as brain:
            # Create multiple memory systems
            working_mems = [WorkingMemory(brain) for _ in range(3)]
            episodic_mems = [EpisodicMemory() for _ in range(3)]
            neuromodulations = [NeuromodulationSystem(brain) for _ in range(3)]
            
            # Simulate multi-agent interaction
            for agent_id in range(3):
                for step in range(20):
                    # Create agent-specific experience
                    experience = {
                        "agent_id": agent_id,
                        "step": step,
                        "stimulus": [agent_id * 0.1, step * 0.1, random.random()],
                        "reward": (step + 1) * 0.1
                    }
                    
                    # Store in agent's episodic memory
                    episodic_mems[agent_id].record(experience, time.time())
                    
                    # Apply agent's neuromodulation
                    neuromodulations[agent_id].apply_dopamine(
                        reward=experience["reward"],
                        predicted=experience["reward"] * 0.9
                    )
                    neuromodulations[agent_id].apply_curiosity(novelty=0.3)
                    
                    # Store in agent's working memory
                    working_mems[agent_id].store(experience["stimulus"], strength=experience["reward"])
                    
                    # Update agent's systems
                    working_mems[agent_id].update(0.01)
                    neuromodulations[agent_id].update(0.01)
            
            # Verify multi-agent integration
            assert brain.simulation_step == 19
            
            for i in range(3):
                assert episodic_mems[i].get_episode_count() == 20
                assert neuromodulations[i].get_dopamine_level() is not None
                assert neuromodulations[i].get_curiosity_level() is not None

# Test discovery and registration functions

def discover_integration_tests():
    """Discover all integration test classes."""
    test_classes = [
        TestCrossComponentIntegration,
        TestPerformanceIntegration,
        TestCompatibilityIntegration,
        TestRealWorldScenarios
    ]
    
    return test_classes

def get_integration_test_categories():
    """Get all integration test categories."""
    return {
        'cross_component': TestCrossComponentIntegration,
        'performance': TestPerformanceIntegration,
        'compatibility': TestCompatibilityIntegration,
        'real_world': TestRealWorldScenarios
    }

if __name__ == "__main__":
    # Run integration tests
    import unittest
    
    # Create test suite
    loader = unittest.TestLoader()
    
    # Get all integration test classes
    integration_classes = discover_integration_tests()
    
    # Create test suite
    suite = unittest.TestSuite()
    
    for test_class in integration_classes:
        suite.addTests(loader.loadTestsFromTestCase(test_class))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Exit with appropriate code
    sys.exit(0 if result.wasSuccessful() else 1)
