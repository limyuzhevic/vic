#!/usr/bin/env python3
# test_python_bindings.py
# Comprehensive Python bindings test suite

import pytest
import numpy as np
import time
import tracemalloc
from pynlm import Brain, Neuron, WorkingMemory, EpisodicMemory
from pynlm.neuromodulation import Reward, Curiosity
from pynlm.sensory import Vision, Audio

class TestBrainBindings:
    """Test Brain Python bindings."""

    def test_brain_creation(self):
        """Test basic brain creation."""
        brain = Brain()
        assert brain is not None
        assert brain.get_name() == "Brain"

    def test_brain_creation_with_config(self):
        """Test brain creation with configuration."""
        config = {
            'neuron_count': 50,
            'threshold': -50,
            'rest_potential': -65,
            'reset_potential': -65,
            'learning_rate': 0.01,
            'plasticity_strength': 1.0
        }
        brain = Brain(config)
        assert brain is not None

    def test_neuron_creation(self):
        """Test neuron creation."""
        neuron = Neuron()
        assert neuron is not None
        assert neuron.get_type() == "Neuron"

    def test_neuron_creation_with_config(self):
        """Test neuron creation with configuration."""
        config = {
            'threshold': -50,
            'rest_potential': -65,
            'reset_potential': -65,
            'learning_rate': 0.01,
            'plasticity_strength': 1.0,
            'membrane_time_constant': 10.0,
            'refractory_period': 0.5
        }
        neuron = Neuron(config)
        assert neuron is not None

    def test_add_neuron(self):
        """Test adding neurons to brain."""
        brain = Brain()
        neuron = Neuron()
        brain.add_neuron(neuron)
        assert len(brain.neurons) == 1

    def test_multiple_neurons(self):
        """Test adding multiple neurons."""
        brain = Brain()
        neurons = [Neuron() for _ in range(10)]
        for neuron in neurons:
            brain.add_neuron(neuron)
        assert len(brain.neurons) == 10

    def test_update(self):
        """Test brain update."""
        brain = Brain()
        neuron = Neuron()
        brain.add_neuron(neuron)

        spiked = brain.update(0.01)
        assert isinstance(spiked, list)

    def test_state_management(self):
        """Test brain state management."""
        brain = Brain()
        neuron = Neuron()
        brain.add_neuron(neuron)

        # Get state
        state = brain.get_state()
        assert 'neuron_count' in state
        assert 'synapse_count' in state
        assert 'neuron_states' in state
        assert 'state' in state

        # Set state
        brain.set_state(state)
        assert brain.get_state() == state

    def test_save_load(self, tmpdir):
        """Test saving and loading brain state."""
        brain = Brain()
        neuron = Neuron()
        brain.add_neuron(neuron)

        # Save
        filepath = tmpdir / "brain_state.json"
        brain.save(str(filepath))
        assert filepath.exists()

        # Load
        brain2 = Brain()
        brain2.load(str(filepath))

        # Verify loaded state matches saved state
        state1 = brain.get_state()
        state2 = brain2.get_state()
        assert state1 == state2

class TestMemoryBindings:
    """Test memory system Python bindings."""

    def test_working_memory_creation(self):
        """Test working memory creation."""
        memory = WorkingMemory()
        assert memory is not None
        assert not memory.is_active()

    def test_working_memory_with_config(self):
        """Test working memory with configuration."""
        config = {
            'size': 1000,
            'decay_rate': 0.1,
            'persistence_threshold': 0.01
        }
        memory = WorkingMemory(config)
        assert memory is not None

    def test_activate_pattern(self):
        """Test pattern activation in working memory."""
        memory = WorkingMemory({'size': 100})
        pattern = np.random.rand(100)
        memory.activate_pattern(pattern, 0.5)
        assert memory.is_active()

    def test_pattern_activation_strength(self):
        """Test pattern activation with different strengths."""
        memory = WorkingMemory({'size': 100})
        pattern = np.random.rand(100)

        memory.activate_pattern(pattern, 0.1)
        activations1 = memory.get_activations()

        memory.activate_pattern(pattern, 0.9)
        activations2 = memory.get_activations()

        assert np.all(activations2 > activations1)

    def test_update(self):
        """Test working memory update."""
        memory = WorkingMemory({'size': 100})
        pattern = np.random.rand(100)
        memory.activate_pattern(pattern, 1.0)

        memory.update(0.01, 0.1)
        activations = memory.get_activations()
        assert len(activations) == 100

    def test_episodic_memory_creation(self):
        """Test episodic memory creation."""
        memory = EpisodicMemory()
        assert memory is not None
        assert len(memory.get_recent_episodes(10)) == 0

    def test_encode_experience(self):
        """Test experience encoding."""
        memory = EpisodicMemory()
        experience = {
            'step': 0,
            'reward': 1.0,
            'action': 1,
            'next_state': [0.1, 0.2, 0.3]
        }
        memory.encode_experience(experience)
        assert len(memory.current_episode) == 1

    def test_store_episode(self):
        """Test episode storage."""
        memory = EpisodicMemory()
        experience = {'step': 0, 'reward': 1.0}
        memory.encode_experience(experience)
        memory.store_episode()

        episodes = memory.get_recent_episodes(10)
        assert len(episodes) == 1

    def test_store_multiple_episodes(self):
        """Test storing multiple episodes."""
        memory = EpisodicMemory()
        for i in range(5):
            experience = {'step': i, 'reward': i * 0.1}
            memory.encode_experience(experience)
            memory.store_episode()

        episodes = memory.get_recent_episodes(10)
        assert len(episodes) == 5

class TestNeuromodulationBindings:
    """Test neuromodulation Python bindings."""

    def test_reward_creation(self):
        """Test reward creation."""
        reward = Reward()
        assert reward is not None
        assert reward.get_rpe() == 0.0
        assert reward.get_value_estimate() == 0.0

    def test_reward_update(self):
        """Test reward prediction error update."""
        reward = Reward({
            'learning_rate': 0.1,
            'discount_factor': 0.9
        })

        reward.update(reward=1.0, next_value=0.5)
        assert reward.get_rpe() != 0.0
        assert reward.get_value_estimate() != 0.0

    def test_reward_getters(self):
        """Test reward getter methods."""
        reward = Reward()
        assert reward.get_rpe() == 0.0
        assert reward.get_value_estimate() == 0.0

    def test_curiosity_creation(self):
        """Test curiosity creation."""
        curiosity = Curiosity()
        assert curiosity is not None
        assert curiosity.get_curiosity() == 0.0
        assert curiosity.get_intrinsic_reward() == 0.0

    def test_curiosity_update(self):
        """Test curiosity update."""
        curiosity = Curiosity({
            'exploration_rate': 0.5
        })

        curiosity.update(state_change=0.1, action=1)
        assert curiosity.get_curiosity() == 0.1
        assert curiosity.get_intrinsic_reward() == 0.05

    def test_curiosity_getters(self):
        """Test curiosity getter methods."""
        curiosity = Curiosity()
        assert curiosity.get_curiosity() == 0.0
        assert curiosity.get_intrinsic_reward() == 0.0

class TestSensoryBindings:
    """Test sensory system Python bindings."""

    def test_vision_creation(self):
        """Test vision creation."""
        vision = Vision()
        assert vision is not None
        assert vision.get_field_of_view() > 0
        assert vision.get_resolution()[0] > 0

    def test_vision_with_config(self):
        """Test vision with configuration."""
        config = {
            'field_of_view': 180,
            'resolution': (128, 128),
            'sensitivity': 1.0
        }
        vision = Vision(config)
        assert vision is not None

    def test_vision_process_input(self):
        """Test vision input processing."""
        vision = Vision()
        visual_input = np.random.rand(64, 64)
        vision.process_input(visual_input)
        assert np.allclose(vision.get_input(), visual_input)

    def test_vision_get_input(self):
        """Test vision get_input method."""
        vision = Vision()
        input_data = vision.get_input()
        assert isinstance(input_data, np.ndarray)

    def test_audio_creation(self):
        """Test audio creation."""
        audio = Audio()
        assert audio is not None
        assert audio.get_frequency_range()[0] > 0
        assert audio.get_frequency_range()[1] > 0

    def test_audio_with_config(self):
        """Test audio with configuration."""
        config = {
            'frequency_range': (100, 20000),
            'buffer_size': 2048,
            'gain': 0.5
        }
        audio = Audio(config)
        assert audio is not None

    def test_audio_process_input(self):
        """Test audio input processing."""
        audio = Audio()
        audio_input = np.random.rand(1024)
        audio.process_input(audio_input)
        assert np.allclose(audio.get_input(), audio_input * 0.5)

    def test_audio_get_input(self):
        """Test audio get_input method."""
        audio = Audio()
        input_data = audio.get_input()
        assert isinstance(input_data, np.ndarray)

class TestPerformanceBindings:
    """Test performance aspects of Python bindings."""

    @pytest.mark.performance
    def test_creation_performance(self):
        """Test performance of object creation."""
        start_time = time.time()
        brains = [Brain() for _ in range(100)]
        neurons = [Neuron() for _ in range(100)]
        end_time = time.time()

        assert (end_time - start_time) < 1.0

    @pytest.mark.performance
    def test_update_performance(self):
        """Test performance of brain updates."""
        brain = Brain()
        for i in range(10):
            neuron = Neuron()
            brain.add_neuron(neuron)

        start_time = time.time()
        for step in range(1000):
            brain.update(0.01)
        end_time = time.time()

        assert (end_time - start_time) < 5.0

class TestMemoryUsageBindings:
    """Test memory usage of Python bindings."""

    @pytest.mark.memory
    def test_memory_usage(self):
        """Test memory usage of Python bindings."""
        tracemalloc.start()

        # Create objects to test memory usage
        brain = Brain()
        for i in range(50):
            neuron = Neuron()
            brain.add_neuron(neuron)

        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()

        assert peak < 100 * 1024 * 1024  # Should use less than 100 MB

    @pytest.mark.memory
    def test_working_memory_usage(self):
        """Test memory usage of working memory."""
        tracemalloc.start()

        memories = [WorkingMemory() for _ in range(50)]
        patterns = [np.random.rand(100) for _ in range(50)]
        for memory, pattern in zip(memories, patterns):
            memory.activate_pattern(pattern)

        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()

        assert peak < 50 * 1024 * 1024  # Should use less than 50 MB

class TestAdvancedBindings:
    """Test advanced Python bindings features."""

    def test_context_managers(self):
        """Test context manager bindings."""
        from pynlm.core import ManagedResource

        with ManagedResource() as resource:
            assert resource is not None

    def test_factory_functions(self):
        """Test factory function bindings."""
        from pynlm import create_brain, create_neuron, create_memory_system

        brain = create_brain({'neuron_count': 10})
        neuron = create_neuron({'threshold': -50})
        memory = create_memory_system('working')

        assert brain is not None
        assert neuron is not None
        assert memory is not None

    def test_type_safety(self):
        """Test type safety features."""
        from pynlm.core import TypeSafeValue

        # Test type-safe value binding
        value = TypeSafeValue('int', 42)
        assert value.get() == 42
        assert value.get_type() == 'int'

    def test_parameter_validation(self):
        """Test parameter validation."""
        from pynlm.core import validate_parameters

        params = {'threshold': -50, 'rest_potential': -65}
        assert validate_parameters(params)

        invalid_params = {'invalid_param': 123}
        assert not validate_parameters(invalid_params)

def test_error_handling():
    """Test error handling in Python bindings."""
    from pynlm import NLMError

    # Test that expected exceptions are raised
    with pytest.raises(NLMError):
        raise NLMError("Test error")

def test_numpy_interoperability():
    """Test NumPy interoperability."""
    import numpy as np

    # Test NumPy array creation and manipulation
    arr = np.array([1.0, 2.0, 3.0])
    assert arr.sum() == 6.0

    # Test NumPy interoperability with neural data
    from pynlm import Brain, Neuron

    brain = Brain()
    neurons = [Neuron() for _ in range(10)]

    # Use NumPy for array operations
    spike_counts = np.array([neuron.spike_count for neuron in neurons])
    assert spike_counts.sum() == 0

if __name__ == '__main__':
    pytest.main([__file__, '-v'])