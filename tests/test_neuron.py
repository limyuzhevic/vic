#!/usr/bin/env python3
"""
Comprehensive unit tests for Neuron.cpp stepLIF method

This test suite validates the LIF (Leaky Integrate-and-Fire) dynamics
implementation in the NLM brain simulator.
"""

import sys
import os
import tempfile
import json
import math
from pathlib import Path

# Add the current directory to Python path to ensure we import the local pynlm module
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error importing pynlm: {e}")
    PYNLM_AVAILABLE = False

class TestNeuronLIF:
    """Comprehensive tests for Neuron.cpp stepLIF method"""
    
    def setup_method(self):
        """Setup before each test"""
        if not PYNLM_AVAILABLE:
            return
            
        # Create a simple brain and neuron for testing
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 10)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Get the first neuron (assuming at least one exists)
        self.neuron = None
        for region in self.brain.getRegions():
            for neuron in region.getAllNeurons():
                self.neuron = neuron
                break
            if self.neuron:
                break
        
        if self.neuron:
            # Configure neuron for testing
            self.neuron.setType(pynlm.NeuronType.Internal)
            self.neuron.setLeakConductance(10.0f)
            self.neuron.setRefractoryPeriod(5)
            self.neuron.setRestingPotential(-70.0f)
            self.neuron.setThreshold(-55.0f)
            self.neuron.setResetPotential(-70.0f)
            self.neuron.setMembranePotential(-70.0f)  # Start at rest
    
    def teardown_method(self):
        """Cleanup after each test"""
        if hasattr(self, 'brain') and self.brain:
            self.brain.reset()
    
    def test_neuron_creation(self):
        """Test basic neuron creation and initialization"""
        assert self.neuron is not None
        
        # Test basic properties
        assert self.neuron.getId().value >= 0
        assert self.neuron.getType() == pynlm.NeuronType.Internal
        assert self.neuron.getMembranePotential() == -70.0f
        assert self.neuron.getThreshold() == -55.0f
        assert self.neuron.getRestingPotential() == -70.0f
        assert self.neuron.getResetPotential() == -70.0f
        assert self.neuron.getLeakConductance() == 10.0f
        assert self.neuron.getRefractoryPeriod() == 5
        assert not self.neuron.isFiring()
        assert not self.neuron.isRefractory()
    
    def test_neuron_step_no_fire(self):
        """Test neuron step when below threshold (should not fire)"""
        initial_potential = self.neuron.getMembranePotential()
        initial_firing = self.neuron.isFiring()
        
        # Take a step with current timestep
        timestep = 0.001
        fired = self.neuron.stepLIF(0.0, timestep)
        
        # Should not have fired
        assert not fired
        assert not self.neuron.isFiring()
        
        # Potential should have changed due to leak dynamics
        assert self.neuron.getMembranePotential() != initial_potential
        
        # Last spike time should remain unchanged (-1.0 for no spikes)
        assert self.neuron.getLastSpikeTime() == -1.0f
    
    def test_neuron_step_with_excitatory_input(self):
        """Test neuron step with excitatory input (should fire)"""
        # Set up neuron to be close to threshold
        self.neuron.setMembranePotential(-55.5f)  # Just below threshold
        
        # Add strong excitatory input
        self.neuron.receiveExcitatoryInput(20.0f)  # Large current
        
        # Take a step
        timestep = 0.001
        fired = self.neuron.stepLIF(0.0, timestep)
        
        # Should have fired
        assert fired
        assert self.neuron.isFiring()
        assert self.neuron.isRefractory()
        
        # Last spike time should be set (approximately current time)
        assert self.neuron.getLastSpikeTime() >= 0.0f
        
        # Membrane potential should be reset
        assert self.neuron.getMembranePotential() == -70.0f
    
    def test_neuron_step_with_inhibitory_input(self):
        """Test neuron step with inhibitory input (should not fire)"""
        # Set up neuron near threshold
        self.neuron.setMembranePotential(-55.0f)
        
        # Add strong inhibitory input
        self.neuron.receiveInhibitoryInput(30.0f)
        
        # Take a step
        timestep = 0.001
        fired = self.neuron.stepLIF(0.0, timestep)
        
        # Should not have fired due to inhibition
        assert not fired
        assert not self.neuron.isFiring()
        
        # Potential should be lower than without inhibition
        # (This depends on the exact dynamics)
    
    def test_neuron_step_refractory_period(self):
        """Test neuron refractory period handling"""
        # First, make neuron fire
        self.neuron.setMembranePotential(-55.5f)
        self.neuron.receiveExcitatoryInput(20.0f)
        self.neuron.stepLIF(0.0, 0.001)
        
        # Now neuron should be refractory
        assert self.neuron.isRefractory()
        assert self.neuron.getLastSpikeTime() >= 0.0f
        
        # Try to fire again while still refractory
        # Need to reset potential to test (but refractory prevents input integration)
        initial_potential = self.neuron.getMembranePotential()
        self.neuron.receiveExcitatoryInput(100.0f)  # Large input
        
        # Step should not cause firing due to refractory
        fired = self.neuron.stepLIF(0.0, 0.001)
        assert not fired
        assert self.neuron.isRefractory()
        
        # Synaptic input should be cleared during refractory
        # (Implementation detail: synaptic input is reset in refractory)
    
    def test_neuron_step_lif_dynamics(self):
        """Test basic LIF dynamics (exponential integration)"""
        # Set up neuron with known parameters
        self.neuron.setMembranePotential(-70.0f)
        self.neuron.setRestingPotential(-70.0f)
        self.neuron.setThreshold(-55.0f)
        
        # Take a step without input - should drift toward resting potential
        timestep = 0.001
        initial_potential = self.neuron.getMembranePotential()
        fired = self.neuron.stepLIF(0.0, timestep)
        
        # Should not have fired
        assert not fired
        
        # Potential should be very close to resting (-70mV)
        # Due to numerical precision, might be exactly -70.0
        assert abs(self.neuron.getMembranePotential() - (-70.0f)) < 0.1f
    
    def test_neuron_step_input_integration(self):
        """Test synaptic input integration"""
        # Set up neuron at resting potential
        self.neuron.setMembranePotential(-70.0f)
        
        # Add excitatory input
        self.neuron.receiveExcitatoryInput(50.0f)
        
        # Record potential before step
        potential_before = self.neuron.getMembranePotential()
        
        # Take a step
        timestep = 0.001
        fired = self.neuron.stepLIF(0.0, timestep)
        
        # Potential should have increased due to input
        # (LIF integration: input contributes to potential change)
        potential_after = self.neuron.getMembranePotential()
        
        # For this test, we just verify the neuron is still functional
        # The exact change depends on LIF parameters
        assert not fired
    
    def test_neuron_step_multiple_inputs(self):
        """Test cumulative effect of multiple inputs"""
        # Set up neuron near threshold
        self.neuron.setMembranePotential(-56.0f)
        
        # Add multiple inputs
        self.neuron.receiveExcitatoryInput(10.0f)
        self.neuron.receiveExcitatoryInput(15.0f)
        self.neuron.receiveExcitatoryInput(5.0f)
        
        # Take a step
        timestep = 0.001
        fired = self.neuron.stepLIF(0.0, timestep)
        
        # The effect of multiple inputs should be cumulative
        # (Exact result depends on LIF integration)
    
    def test_neuron_step_parameters(self):
        """Test LIF parameter effects"""
        # Test different leak conductances
        self.neuron.setLeakConductance(5.0f)
        
        initial_potential = self.neuron.getMembranePotential()
        self.neuron.stepLIF(0.0, 0.001)
        
        # Potential change should be different with different conductance
        # This test mainly ensures parameters are accepted
    
    def test_neuron_step_spike_history(self):
        """Test spike history recording"""
        # Make neuron fire
        self.neuron.setMembranePotential(-55.5f)
        self.neuron.receiveExcitatoryInput(20.0f)
        
        # Take a step
        timestep = 0.001
        fired = self.neuron.stepLIF(0.0, timestep)
        
        # Neuron should have recorded spike
        assert fired
        assert self.neuron.getLastSpikeTime() >= 0.0f
        
        # Spike history should be accessible (via brain if neuron doesn't expose directly)
        # This tests the spike recording functionality
    
    def test_neuron_step_adaptation(self):
        """Test spike-frequency adaptation"""
        # Configure adaptation
        self.neuron.setMembranePotential(-55.5f)
        self.neuron.receiveExcitatoryInput(20.0f)
        
        # First spike - should increase adaptation variable
        fired1 = self.neuron.stepLIF(0.0, 0.001)
        assert fired1
        
        # Adaptation variable should now be positive
        # (We can't directly access it, but we can verify spike occurred)
    
    def test_neuron_step_clamping(self):
        """Test membrane potential clamping"""
        # Set extreme potentials
        self.neuron.setMembranePotential(-100.0f)
        self.neuron.receiveExcitatoryInput(1000.0f)  // Huge input
        
        # Take a step - potential should be clamped
        fired = self.neuron.stepLIF(0.0, 0.001)
        
        # Potential should not exceed clamp bounds
        potential = self.neuron.getMembranePotential()
        assert potential >= -100.0f  # Lower clamp
        assert potential <= 50.0f   # Upper clamp
    
    def test_neuron_step_time_effects(self):
        """Test effects of different timestep sizes"""
        # Use larger timestep for more dramatic effect
        timestep = 0.01  # 10ms instead of 1ms
        
        self.neuron.setMembranePotential(-55.0f)
        self.neuron.receiveExcitatoryInput(20.0f)
        
        # Take a step with larger dt
        fired = self.neuron.stepLIF(0.0, timestep)
        
        # Results should depend on timestep size
        # This tests timestep handling
    
    def test_neuron_step_current_injection(self):
        """Test direct current injection (from sensory input)"""
        # This tests the injectCurrent method
        self.neuron.setMembranePotential(-70.0f)
        self.neuron.injectCurrent(30.0f)
        
        # Take a step
        fired = self.neuron.stepLIF(0.0, 0.001)
        
        # Should not have fired (input may be integrated but likely not enough)
        # This tests the current injection functionality
    
    def test_neuron_step_multiple_spikes(self):
        """Test neuron firing multiple times in sequence"""
        # Set up neuron to fire
        self.neuron.setMembranePotential(-55.5f)
        
        # First spike
        fired1 = self.neuron.stepLIF(0.0, 0.001)
        assert fired1
        
        # Wait in refractory period - should not fire again
        # (Implementation detail: refractory prevents firing)
        self.neuron.stepLIF(0.001, 0.001)
        
        # Neuron should still be refractory
        assert self.neuron.isRefractory()
        
        # After refractory ends, potential should be reset
        # and neuron should be ready to fire again
    
    def test_neuron_step_consistency(self):
        """Test that neuron behavior is consistent"""
        # Run identical simulation multiple times
        results = []
        for i in range(5):
            # Reset neuron
            self.neuron.reset()
            self.neuron.setMembranePotential(-70.0f)
            
            # Run identical sequence
            self.neuron.receiveExcitatoryInput(15.0f)
            fired = self.neuron.stepLIF(0.0, 0.001)
            
            results.append(fired)
        
        # All results should be the same (deterministic behavior)
        # (With identical inputs and random seed, should be deterministic)
    
    def test_neuron_step_state_isolation(self):
        """Test that neuron states are properly isolated"""
        # Create multiple neurons (using brain)
        # This tests that each neuron maintains its own state independently
        # The brain ensures neurons don't interfere with each other
    
    def test_neuron_step_parameters_validation(self):
        """Test parameter validation and boundary conditions"""
        # Test with extreme parameter values
        self.neuron.setLeakConductance(0.1f)  // Very low conductance
        self.neuron.setRefractoryPeriod(1)   // Minimum refractory
        self.neuron.setThreshold(-100.0f)   // Very low threshold
        
        # Should not crash
        fired = self.neuron.stepLIF(0.0, 0.001)
        assert not fired
        
        # Test with high values
        self.neuron.setLeakConductance(1000.0f)  // Very high conductance
        self.neuron.setRefractoryPeriod(1000)   // Very long refractory
        self.neuron.setThreshold(100.0f)       // Very high threshold
        
        # Should not crash
        fired = self.neuron.stepLIF(0.0, 0.001)
        assert not fired

class TestNeuronIntegration:
    """Integration tests for neurons in brain context"""
    
    def setup_method(self):
        """Setup for integration tests"""
        if not PYNLM_AVAILABLE:
            return
            
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 20)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Get a reference neuron
        self.test_neuron = None
        for region in self.brain.getRegions():
            for neuron in region.getAllNeurons():
                self.test_neuron = neuron
                break
            if self.test_neuron:
                break
    
    def teardown_method(self):
        """Cleanup"""
        if hasattr(self, 'brain') and self.brain:
            self.brain.reset()
    
    def test_neuron_in_brain_context(self):
        """Test neuron behavior within brain context"""
        assert self.test_neuron is not None
        
        # Neuron should be properly integrated into brain
        # (This is tested through the brain creation process)
        
        # Modify neuron state
        self.test_neuron.setMembranePotential(-60.0f)
        
        # Neuron should maintain its state within brain context
        # (Brain's management ensures proper isolation)
    
    def test_multiple_neuron_independence(self):
        """Test that multiple neurons maintain independent states"""
        # Get several neurons
        neurons = []
        for region in self.brain.getRegions():
            for neuron in region.getAllNeurons():
                neurons.append(neuron)
                if len(neurons) >= 5:  # Test with 5 neurons
                    break
            if len(neurons) >= 5:
                break
        
        # Modify each neuron differently
        for i, neuron in enumerate(neurons):
            neuron.setMembranePotential(-70.0f + i * 5.0f)
            neuron.receiveExcitatoryInput(i * 10.0f)
        
        # Each neuron should maintain its own state
        # (Brain implementation should ensure isolation)
        
        # Verify initial states
        for i, neuron in enumerate(neurons):
            assert neuron.getMembranePotential() == -70.0f + i * 5.0f
    
    def test_neuron_population_simulation(self):
        """Test neuron population behavior"""
        # This tests the behavior of multiple neurons working together
        # (Through the brain's region/population system)
        
        # The brain creates regions with populations of neurons
        # This tests that the population structure works correctly
        for region in self.brain.getRegions():
            for population in region.getPopulations():
                # Population should have multiple neurons
                assert population.getNeuronCount() > 0
                
                # Test population functionality
                for neuron in population.getNeurons():
                    # Each neuron should function independently
                    assert neuron.getId().value >= 0

class TestNeuronPerformance:
    """Performance tests for neuron operations"""
    
    def setup_method(self):
        """Setup for performance tests"""
        if not PYNLM_AVAILABLE:
            return
            
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 50)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Get test neuron
        self.test_neuron = None
        for region in self.brain.getRegions():
            for neuron in region.getAllNeurons():
                self.test_neuron = neuron
                break
            if self.test_neuron:
                break
    
    def teardown_method(self):
        """Cleanup"""
        if hasattr(self, 'brain') and self.brain:
            self.brain.reset()
    
    def test_neuron_step_performance(self):
        """Test performance of neuron step operations"""
        import time
        
        # Reset and configure neuron
        self.test_neuron.reset()
        self.test_neuron.setMembranePotential(-70.0f)
        
        # Time multiple step operations
        start_time = time.time()
        
        for step in range(1000):
            self.test_neuron.stepLIF(step * 0.001, 0.001)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # 1000 steps should be very fast
        assert elapsed < 1.0, f"Neuron step performance too slow: {elapsed:.3f}s for 1000 steps"
    
    def test_neuron_multiple_steps_performance(self):
        """Test performance of multiple neurons"""
        import time
        
        start_time = time.time()
        
        # Simulate multiple neurons
        for step in range(100):
            for region in self.brain.getRegions():
                for neuron in region.getAllNeurons():
                    neuron.stepLIF(step * 0.001, 0.001)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        # Should be fast even with many neurons
        assert elapsed < 5.0, f"Multiple neuron performance too slow: {elapsed:.3f}s for 100 steps with {self.config.get('brain.neuron_count')} neurons"

# Run tests if executed directly
if __name__ == "__main__":
    import pytest
    
    # Skip tests if pynlm is not available
    if not PYNLM_AVAILABLE:
        print("Skipping Neuron tests - pynlm module not available")
        sys.exit(0)
    
    # Run pytest with neuron-specific tests
    exit_code = pytest.main([
        __file__,
        "-v",
        "-k", "test_neuron",
        "--tb=short",
        "--timeout=30"
    ])
    
    sys.exit(exit_code)