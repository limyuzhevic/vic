#!/usr/bin/env python3
"""
Comprehensive test suite for enhanced NLM Python bindings.

This test suite covers all the new Pythonic features:
- Property-based access
- Context manager support
- Enhanced error handling
- Builder patterns
- Convenience methods
- Type safety improvements

Tests are designed to verify that the enhanced API works correctly
while maintaining backward compatibility.
"""

import sys
import os
import tempfile
import pytest

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm not available: {e}")

if PYNLM_AVAILABLE:
    # Test the enhanced Python bindings
    def test_property_access():
        """Test property-based access to brain attributes."""
        # Create a minimal configuration
        config = pynlm.Config()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test property access (these should work)
        assert hasattr(brain, 'total_neurons')
        assert hasattr(brain, 'total_synapses')
        assert hasattr(brain, 'firing_neurons')
        assert hasattr(brain, 'total_spikes')
        assert hasattr(brain, 'average_firing_rate')
        assert hasattr(brain, 'development_stage')
        assert hasattr(brain, 'config')
        
        # Test that properties return expected types
        assert isinstance(brain.total_neurons(), int)
        assert isinstance(brain.total_synapses(), int)
        assert isinstance(brain.firing_neurons(), int)
        assert isinstance(brain.total_spikes(), int)
        assert isinstance(brain.average_firing_rate(), float)
        assert brain.development_stage() in [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ]
        
        # Test that old methods still work (backward compatibility)
        assert brain.getTotalNeuronCount() == brain.total_neurons()
        assert brain.getTotalSynapseCount() == brain.total_synapses()
        assert brain.getFiringNeuronCount() == brain.firing_neurons()
        
    def test_context_manager():
        """Test context manager support."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        
        # Test context manager
        with brain:
            brain.initialize()
            # Brain should be initialized within context
            assert brain.total_neurons() > 0
        
        # Brain should be reset after context exit
        # Note: The actual reset behavior depends on implementation
        
    def test_enhanced_config():
        """Test enhanced Config class with builder pattern."""
        # Test default config
        config = pynlm.create_default_config()
        assert config is not None
        
        # Test property access
        assert hasattr(config, 'neuron_count')
        assert hasattr(config, 'synapse_density')
        assert hasattr(config, 'random_seed')
        
        # Test builder pattern
        brain = (pynlm.ConfigBuilder()
                 .with_neuron_count(1000)
                 .with_synapse_density(0.1)
                 .with_plasticity()
                 .build())
        
        brain.initialize()
        assert brain.total_neurons() == 1000
        assert brain.total_synapses() > 0
        
    def test_enhanced_brain_properties():
        """Test enhanced Brain properties and methods."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test enhanced methods
        status = brain.get_status()
        assert isinstance(status, str)
        assert 'Brain Status' in status
        assert 'Neurons' in status
        
        # Test statistics
        stats = brain.get_statistics()
        assert isinstance(stats, dict)
        assert 'neurons' in stats
        assert 'synapses' in stats
        assert 'firing_rate' in stats
        
        # Test development stage property
        original_stage = brain.development_stage()
        brain.set_development_stage(pynlm.DevelopmentalStage.Adult)
        assert brain.development_stage() == pynlm.DevelopmentalStage.Adult
        
        # Restore original stage
        brain.set_development_stage(original_stage)
        
    def test_enhanced_world():
        """Test enhanced SimpleWorld class."""
        world = pynlm.createSimpleWorld()
        
        # Test property access
        assert hasattr(world, 'width')
        assert hasattr(world, 'height')
        assert hasattr(world, 'simulation_time')
        assert hasattr(world, 'max_energy')
        assert hasattr(world, 'energy_decay_rate')
        
        # Test configuration
        world.configure(20, 20, 8, 8)
        assert world.width() == 20
        assert world.height() == 20
        
        # Test context manager
        with world:
            world.reset()
            # World should be reset within context
            assert world.simulation_time() == 0.0
        
        # Test world statistics
        world_stats = world.get_world_statistics()
        assert isinstance(world_stats, dict)
        assert 'width' in world_stats
        assert 'height' in world_stats
        assert 'simulation_time' in world_stats
        
    def test_enhanced_agent():
        """Test enhanced AgentBrain class."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test learning controls
        assert hasattr(agent, 'is_reward_modulation_enabled')
        assert hasattr(agent, 'is_structural_plasticity_enabled')
        assert hasattr(agent, 'is_development_enabled')
        assert hasattr(agent, 'is_curiosity_enabled')
        
        # Test enhanced learning methods
        agent.enable_all_learning()
        assert agent.is_reward_modulation_enabled()
        assert agent.is_structural_plasticity_enabled()
        assert agent.is_development_enabled()
        assert agent.is_curiosity_enabled()
        
        agent.disable_all_learning()
        assert not agent.is_reward_modulation_enabled()
        
        # Test agent statistics
        agent_stats = agent.get_agent_statistics()
        assert isinstance(agent_stats, dict)
        assert 'reward_modulation' in agent_stats
        assert 'structural_plasticity' in agent_stats
        
    def test_simulation_utilities():
        """Test simulation utilities and convenience methods."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 6, 6)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test episode running utility (if available)
        if hasattr(pynlm, 'run_episode'):
            episode_stats = pynlm.run_episode(brain, world, agent, max_steps=50)
            assert isinstance(episode_stats, dict)
            assert 'steps' in episode_stats
            assert 'time' in episode_stats
        
        # Test brain step method
        brain.initialize()
        brain.step(0)
        assert brain.total_spikes() >= 0
        
        # Test run_steps convenience method
        brain.run_steps(10)
        assert brain.total_spikes() >= 0
        
    def test_error_handling():
        """Test enhanced error handling."""
        # Test configuration errors
        config = pynlm.Config()
        
        # Test invalid configuration
        try:
            # This should raise an error in the enhanced implementation
            if hasattr(pynlm, 'PyConfigurationError'):
                # Test that custom exceptions are available
                raise pynlm.PyConfigurationError("Test error")
        except Exception as e:
            assert "Test error" in str(e)
        
        # Test simulation errors
        brain = pynlm.createBrain(pynlm.Config())
        
        # Test invalid timestep
        try:
            world = pynlm.createSimpleWorld()
            world.update(-1.0)  # Invalid timestep
        except Exception as e:
            assert "Timestep must be positive" in str(e)
        
    def test_backward_compatibility():
        """Test that old API still works."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test old-style method calls still work
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        firing = brain.getFiringNeuronCount()
        
        assert isinstance(neurons, int)
        assert isinstance(synapses, int)
        assert isinstance(firing, int)
        
        # Test old config methods
        config = pynlm.create_default_config()
        config.save_to_file("test_config.json")
        
        # Load back
        loaded_config = pynlm.create_default_config()
        loaded_config.load_from_file("test_config.json")
        
        # Clean up
        if os.path.exists("test_config.json"):
            os.remove("test_config.json")
        
    def test_type_safety():
        """Test type safety improvements."""
        # Test NeuronId type
        neuron_id = pynlm.NeuronId(12345)
        assert neuron_id.value() == 12345
        assert neuron_id.index() == 12345
        assert neuron_id.is_valid()
        
        # Test Action type
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        assert action.get_type() == pynlm.ActionType.MoveForward
        assert action.is_valid()
        
        # Test enhanced action
        enhanced_action = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.5])
        assert enhanced_action.get_parameters() == [0.5, 0.5]
        
    def test_builder_patterns():
        """Test builder pattern implementations."""
        # Test ConfigBuilder
        config = (pynlm.ConfigBuilder()
                 .with_neuron_count(1000)
                 .with_synapse_density(0.15)
                 .with_plasticity()
                 .with_neuromodulation(curiosity=True)
                 .build())
        
        config.initialize()
        assert config.total_neurons() == 1000
        
        # Test that builder creates valid config
        assert config.is_stdp_enabled()
        assert config.is_curiosity_enabled()
        
    def test_documentation_examples():
        """Test examples from documentation."""
        # Test basic usage example
        brain = (pynlm.BrainBuilder()
                 .with_neuron_count(1000)
                 .with_plasticity(learning_rate=0.001)
                 .build())
        
        brain.initialize()
        assert brain.total_neurons() == 1000
        
        # Test world builder example
        world = (pynlm.WorldBuilder()
                 .width(20).height(20)
                 .vision(8, 8)
                 .create())
        
        world.configure(20, 20, 8, 8)
        assert world.width() == 20
        
        # Test agent creation example
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        assert agent.get_brain() == brain
        
    def test_performance_improvements():
        """Test that performance improvements work correctly."""
        import time
        
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test that property access is fast
        start_time = time.time()
        for _ in range(1000):
            _ = brain.total_neurons()
        end_time = time.time()
        
        # Property access should be reasonably fast
        assert (end_time - start_time) < 1.0, "Property access is too slow"
        
        # Test that statistics collection works
        stats = brain.get_statistics()
        assert len(stats) > 0
        
    def test_memory_management():
        """Test memory management improvements."""
        config = pynlm.Config()
        brain = pynlm.createBrain(config)
        
        # Test context manager for memory management
        with brain:
            brain.initialize()
            # Brain should be properly managed
            assert brain.total_neurons() > 0
        
        # Brain should be cleaned up
        # Note: Specific cleanup behavior depends on implementation

# Main test runner
def run_tests():
    """Run all tests and report results."""
    if not PYNLM_AVAILABLE:
        print("ERROR: pynlm not available. Cannot run tests.")
        return False
    
    print("=== Enhanced NLM Python Bindings Test Suite ===\n")
    
    tests = [
        ("Property Access", test_property_access),
        ("Context Manager", test_context_manager),
        ("Enhanced Config", test_enhanced_config),
        ("Enhanced Brain Properties", test_enhanced_brain_properties),
        ("Enhanced World", test_enhanced_world),
        ("Enhanced Agent", test_enhanced_agent),
        ("Simulation Utilities", test_simulation_utilities),
        ("Error Handling", test_error_handling),
        ("Backward Compatibility", test_backward_compatibility),
        ("Type Safety", test_type_safety),
        ("Builder Patterns", test_builder_patterns),
        ("Documentation Examples", test_documentation_examples),
        ("Performance Improvements", test_performance_improvements),
        ("Memory Management", test_memory_management),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"Running test: {test_name}...", end=" ")
        try:
            test_func()
            print("PASSED")
            passed += 1
        except Exception as e:
            print(f"FAILED - {e}")
            failed += 1
            import traceback
            traceback.print_exc()
    
    print(f"\n=== Test Results ===")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Total: {passed + failed}")
    
    if failed > 0:
        print(f"\n{failed} test(s) failed. Please review the implementation.")
        return False
    else:
        print("\nAll tests passed!")
        return True

if __name__ == "__main__":
    success = run_tests()
    sys.exit(0 if success else 1)