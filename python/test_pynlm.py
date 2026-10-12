#!/usr/bin/env python3
"""
Test suite for NLM Python bindings.

This test suite validates the enhanced Python bindings for the NLM neural simulation framework.
It covers basic functionality, error handling, and utility functions.
"""

import sys
import os
import tempfile
import json
from typing import Dict, Any, List
import time

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
except ImportError as e:
    print(f"Error importing pynlm: {e}")
    print("Make sure the NLM Python bindings are installed.")
    sys.exit(1)

from nlm_utils import (
    BrainConfiguration, 
    create_simple_brain, 
    create_complete_agent,
    SimulationRunner,
    NeuralNetworkVisualizer,
    LearningCurveAnalyzer,
    benchmark_configuration,
    export_brain_state,
    import_brain_state,
    example_basic_neural_network,
    example_complete_agent_simulation,
    example_brain_training
)

class TestBasicTypes:
    """Test basic type definitions and constants."""
    
    def test_neuron_id(self):
        """Test NeuronId type."""
        # Test creation
        neuron_id = pynlm.NeuronId(42)
        assert neuron_id.value == 42
        assert neuron_id.index() == 42
        
        # Test equality
        assert neuron_id == pynlm.NeuronId(42)
        assert neuron_id != pynlm.NeuronId(43)
        
        # Test representation
        repr_str = repr(neuron_id)
        assert "NeuronId" in repr_str
        assert "42" in repr_str
        
        print("✓ NeuronId tests passed")
    
    def test_synapse_id(self):
        """Test SynapseId type."""
        synapse_id = pynlm.SynapseId(100)
        assert synapse_id.value == 100
        assert synapse_id.index() == 100
        print("✓ SynapseId tests passed")
    
    def test_region_id(self):
        """Test RegionId type."""
        region_id = pynlm.RegionId(5)
        assert region_id.value == 5
        assert region_id.index() == 5
        print("✓ RegionId tests passed")
    
    def test_population_id(self):
        """Test PopulationId type."""
        pop_id = pynlm.PopulationId(3)
        assert pop_id.value == 3
        assert pop_id.index() == 3
        print("✓ PopulationId tests passed")
    
    def test_enums(self):
        """Test enumeration types."""
        # NeuronType
        assert pynlm.NeuronType.Excitatory.value == 0
        assert pynlm.NeuronType.Inhibitory.value == 1
        assert pynlm.NeuronType.Sensory.value == 4
        assert pynlm.NeuronType.Motor.value == 5
        assert pynlm.NeuronType.Internal.value == 6
        
        # SynapseType
        assert pynlm.SynapseType.Excitatory.value == 0
        assert pynlm.SynapseType.Inhibitory.value == 1
        assert pynlm.SynapseType.Electrical.value == 3
        
        # DevelopmentalStage
        assert pynlm.DevelopmentalStage.Initial.value == 0
        assert pynlm.DevelopmentalStage.Adult.value == 3
        
        # FiringState
        assert pynlm.FiringState.Resting.value == 0
        assert pynlm.FiringState.Active.value == 1
        assert pynlm.FiringState.Refractory.value == 2
        
        # ActionType
        assert pynlm.ActionType.MoveForward.value == 0
        assert pynlm.ActionType.Look.value == 7
        
        print("✓ Enum tests passed")

class TestConstants:
    """Test constant values."""
    
    def test_invalid_constants(self):
        """Test invalid ID constants."""
        # These should return the maximum uint64_t value
        invalid_neuron = pynlm.INVALID_NEURON_ID
        invalid_synapse = pynlm.INVALID_SYNAPSE_ID
        invalid_region = pynlm.INVALID_REGION_ID
        invalid_population = pynlm.INVALID_POPULATION_ID
        
        assert isinstance(invalid_neuron, pynlm.NeuronId)
        assert isinstance(invalid_synapse, pynlm.SynapseId)
        assert isinstance(invalid_region, pynlm.RegionId)
        assert isinstance(invalid_population, pynlm.PopulationId)
        
        print("✓ Invalid ID constants tests passed")

class TestBrainCreation:
    """Test brain creation and basic functionality."""
    
    def test_create_default_config(self):
        """Test creating default configuration."""
        config = pynlm.createDefaultConfig()
        assert config is not None
        print("✓ Default config creation test passed")
    
    def test_create_brain(self):
        """Test creating brain with configuration."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        assert brain is not None
        print("✓ Brain creation test passed")
    
    def test_simple_brain_creation(self):
        """Test convenience function create_simple_brain."""
        brain = create_simple_brain()
        assert brain is not None
        assert brain.getTotalNeuronCount() > 0
        print("✓ Simple brain creation test passed")
    
    def test_brain_initialization(self):
        """Test brain initialization."""
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        
        # Should initialize successfully
        result = brain.initialize()
        assert result is True
        print("✓ Brain initialization test passed")

class TestUtilityFunctions:
    """Test utility functions and convenience methods."""
    
    def test_simulation_runner(self):
        """Test SimulationRunner class."""
        brain = create_simple_brain()
        runner = SimulationRunner(brain)
        
        assert runner.brain is brain
        assert runner.world is None
        assert runner.agent is None
        
        # Test running steps
        stats = runner.run_steps(10, timestep=0.1, verbose=False)
        assert len(stats) == 10
        assert all('step' in stat for stat in stats)
        
        print("✓ SimulationRunner test passed")
    
    def test_neural_network_visualizer(self):
        """Test NeuralNetworkVisualizer class."""
        brain = create_simple_brain()
        
        # Test connectivity analysis
        connectivity = NeuralNetworkVisualizer.analyze_connectivity(brain)
        assert 'total_synapses' in connectivity
        assert 'total_neurons' in connectivity
        assert 'synapse_density' in connectivity
        
        print("✓ NeuralNetworkVisualizer test passed")
    
    def test_learning_curve_analyzer(self):
        """Test LearningCurveAnalyzer class."""
        analyzer = LearningCurveAnalyzer()
        
        # Add some metrics
        for step in range(5):
            metrics = {
                'step': step,
                'average_firing_rate': 0.5 + step * 0.1,
                'e_i_ratio': 1.0 + step * 0.1,
                'active_neurons': 100 + step * 20
            }
            analyzer.add_metrics(step, metrics)
        
        # Test curve computation
        curve = analyzer.compute_learning_curve('average_firing_rate', window=3)
        assert len(curve) > 0
        
        # Test summary
        summary = analyzer.get_performance_summary()
        assert 'final_step' in summary
        assert 'final_firing_rate' in summary
        assert 'learning_efficiency' in summary
        
        print("✓ LearningCurveAnalyzer test passed")

class TestCompleteAgent:
    """Test complete agent setup."""
    
    def test_create_complete_agent(self):
        """Test create_complete_agent utility."""
        brain, agent, world = create_complete_agent()
        
        assert brain is not None
        assert agent is not None
        assert world is not None
        
        # Test initialization
        agent.initialize(world)
        
        # Test enabling subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        assert agent.isRewardModulationEnabled() is True
        assert agent.isCuriosityEnabled() is True
        assert agent.isDevelopmentEnabled() is True
        
        print("✓ Complete agent creation test passed")

class TestBenchmarking:
    """Test benchmarking functionality."""
    
    def test_benchmark_configuration(self):
        """Test benchmark_configuration function."""
        results = benchmark_configuration('medium', num_steps=50)
        
        assert 'configuration' in results
        assert 'neuron_count' in results
        assert 'steps_per_second' in results
        assert 'performance_score' in results
        
        assert results['configuration'] == 'medium'
        assert results['neuron_count'] == 1000
        assert results['steps_per_second'] > 0
        assert results['performance_score'] > 0
        
        print("✓ Benchmark configuration test passed")

class TestBrainStateExport:
    """Test brain state export/import functionality."""
    
    def test_export_brain_state(self):
        """Test export_brain_state function."""
        brain = create_simple_brain()
        
        # Create temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            # Export state
            export_brain_state(brain, filepath)
            
            # Check file exists and is valid JSON
            with open(filepath, 'r') as f:
                state = json.load(f)
            
            assert 'brain_info' in state
            assert 'region_info' in state
            assert 'timestamp' in state
            
            print("✓ Export brain state test passed")
        
        finally:
            # Clean up
            if os.path.exists(filepath):
                os.unlink(filepath)

class TestErrorHandling:
    """Test error handling and exception management."""
    
    def test_nlm_exception(self):
        """Test NLMException class."""
        # Test exception creation
        try:
            raise pynlm.NLMException("Test error")
        except pynlm.NLMException as e:
            assert str(e) == "Test error"
        
        print("✓ NLMException test passed")

class TestConfiguration:
    """Test configuration utilities."""
    
    def test_brain_configuration(self):
        """Test BrainConfiguration dataclass."""
        config = BrainConfiguration(
            neuron_count=500,
            region_count=3,
            synaptic_density=0.2,
            learning_rate=0.05
        )
        
        assert config.neuron_count == 500
        assert config.region_count == 3
        assert config.synaptic_density == 0.2
        assert config.learning_rate == 0.05
        
        print("✓ BrainConfiguration test passed")

class TestExamplePatterns:
    """Test example pattern functions."""
    
    def test_example_basic_neural_network(self):
        """Test example_basic_neural_network function."""
        # This should run without errors
        stats = example_basic_neural_network()
        
        # Should return a list of statistics
        assert isinstance(stats, list)
        assert len(stats) > 0
        
        print("✓ Example basic neural network test passed")
    
    def test_example_complete_agent_simulation(self):
        """Test example_complete_agent_simulation function."""
        # This should run without errors
        stats, agent = example_complete_agent_simulation()
        
        assert isinstance(stats, list)
        assert agent is not None
        
        print("✓ Example complete agent simulation test passed")
    
    def test_example_brain_training(self):
        """Test example_brain_training function."""
        # This should run without errors
        results = example_brain_training()
        
        assert isinstance(results, dict)
        assert 'minimal' in results
        assert 'small' in results
        assert 'medium' in results
        
        print("✓ Example brain training test passed")

class TestIntegration:
    """Integration tests combining multiple components."""
    
    def test_complete_workflow(self):
        """Test complete workflow from creation to analysis."""
        # 1. Create complete agent
        brain, agent, world = create_complete_agent()
        
        # 2. Initialize agent
        agent.initialize(world)
        
        # 3. Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # 4. Run simulation
        runner = SimulationRunner(brain, world, agent)
        stats = runner.run_steps(100, timestep=0.05, verbose=False)
        
        # 5. Analyze results
        analyzer = LearningCurveAnalyzer()
        for stat in stats:
            analyzer.add_metrics(stat['step'], stat)
        
        summary = analyzer.get_performance_summary()
        
        # 6. Export and import (partial implementation)
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            filepath = f.name
        
        try:
            export_brain_state(brain, filepath)
            import_brain_state(filepath)  # Partial implementation
            
            print("✓ Complete workflow integration test passed")
        
        finally:
            if os.path.exists(filepath):
                os.unlink(filepath)

def run_all_tests():
    """Run all tests and return summary."""
    print("=" * 60)
    print("NLM Python Bindings Test Suite")
    print("=" * 60)
    
    test_classes = [
        TestBasicTypes,
        TestConstants,
        TestBrainCreation,
        TestUtilityFunctions,
        TestCompleteAgent,
        TestBenchmarking,
        TestBrainStateExport,
        TestErrorHandling,
        TestConfiguration,
        TestExamplePatterns,
        TestIntegration
    ]
    
    passed = 0
    failed = 0
    
    for test_class in test_classes:
        print(f"\nRunning {test_class.__name__}...")
        
        test_instance = test_class()
        
        # Get all test methods
        test_methods = [method for method in dir(test_instance) 
                       if method.startswith('test_') and callable(getattr(test_instance, method))]
        
        for method_name in test_methods:
            try:
                method = getattr(test_instance, method_name)
                method()
                passed += 1
            except Exception as e:
                print(f"✗ {method_name} failed: {e}")
                failed += 1
    
    print("\n" + "=" * 60)
    print(f"Test Summary: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return passed, failed

if __name__ == "__main__":
    passed, failed = run_all_tests()
    
    if failed == 0:
        print("\n🎉 All tests passed!")
        sys.exit(0)
    else:
        print(f"\n❌ {failed} tests failed")
        sys.exit(1)
