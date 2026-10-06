#!/usr/bin/env python3
"""
Python bindings tests for Brain class
Tests mirror the C++ test_brain.cpp functionality
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

import pytest
from pynlm import create_agent_simulation, get_default_config

class TestBrain:
    """Test Brain class functionality"""
    
    def test_brain_creation(self):
        """Test Brain creation"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 1
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Verify brain was created correctly
        assert brain.neuron_count() == 100
        assert brain.get_region_count() == 1
        assert brain.synapse_count() > 0
        
        print("✓ Brain creation test passed")
    
    def test_brain_initialization(self):
        """Test Brain initialization"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 2
        config_dict["connection_probability"] = 0.1
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Brain should be initialized by create_agent_simulation
        assert brain.neuron_count() == 100
        assert brain.get_region_count() == 2
        assert brain.get_total_synapse_count() > 0
        
        print("✓ Brain initialization test passed")
    
    def test_brain_step(self):
        """Test Brain step execution"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 1
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Run several steps
        for step in range(10):
            brain.step(step)
        
        # Verify brain is still in valid state
        assert brain.neuron_count() == 100
        assert brain.get_region_count() == 1
        
        print("✓ Brain step test passed")
    
    def test_brain_reset(self):
        """Test Brain reset functionality"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 50
        config_dict["region_count"] = 1
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Run a step
        brain.step(0)
        
        # Reset the brain
        brain.reset()
        
        # Verify reset worked
        assert brain.neuron_count() == 50
        assert brain.get_region_count() == 1
        
        print("✓ Brain reset test passed")
    
    def test_brain_regions(self):
        """Test Brain region functionality"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 3
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Get region IDs
        region_ids = brain.get_region_ids()
        assert len(region_ids) == 3
        
        # Get each region
        for region_id in region_ids:
            region = brain.get_region(region_id)
            assert region is not None
            assert region.get_total_neuron_count() > 0
        
        print("✓ Brain regions test passed")
    
    def test_brain_action_production(self):
        """Test action production"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 1
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Produce an action
        action = brain.produce_action()
        assert action is not None
        
        # Verify action has expected properties
        assert hasattr(action, 'get_type')
        assert hasattr(action, 'get_parameters')
        
        print("✓ Brain action production test passed")
    
    def test_brain_status(self):
        """Test brain status and statistics"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 1
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Verify statistics are accessible
        assert brain.neuron_count() > 0
        assert brain.synapse_count() >= 0
        assert brain.active_neuron_count() >= 0
        assert brain.firing_neuron_count() >= 0
        assert brain.average_firing_rate() >= 0.0
        assert brain.total_spike_count() >= 0
        assert brain.pending_spike_count() >= 0
        assert brain.get_region_count() == 1
        
        print("✓ Brain status test passed")
    
    def test_agent_brain_interface(self):
        """Test AgentBrain interface"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Verify agent brain interface
        assert agent_brain is not None
        assert agent_brain.get_brain() is brain
        
        # Test sensory processing
        percept = world.get_sensory_percept()
        agent_brain.process_sensory_input(percept)
        
        # Test motor command decoding
        action = agent_brain.decode_motor_command()
        assert action is not None
        
        print("✓ Agent brain interface test passed")
    
    def test_context_manager(self):
        """Test context manager support"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 50
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Test context manager (brain should reset on exit)
        with brain.context_manager():
            brain.step(0)
            # Inside context, brain should be active
            assert brain.neuron_count() == 50
        
        # After context, brain should be reset
        # Note: This behavior depends on implementation
        
        print("✓ Context manager test passed")
    
    def test_batch_operations(self):
        """Test batch operations"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Test batch step
        steps = list(range(5))
        brain.batch_step(steps)
        
        # Test batch inject current
        from pynlm import NeuronType
        brain.batch_inject_current(NeuronType.Excitatory, 5.0)
        
        # Verify brain is still functional
        assert brain.neuron_count() == 100
        
        print("✓ Batch operations test passed")
    
    def test_developmental_stage(self):
        """Test developmental stage functionality"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Get current developmental stage
        stage = brain.get_developmental_stage()
        assert stage is not None
        
        # Set developmental stage
        brain.set_developmental_stage(stage)  # Should not crash
        
        print("✓ Developmental stage test passed")
    
    def test_config_modification(self):
        """Test modifying brain configuration"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 100
        config_dict["region_count"] = 2
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Modify configuration
        brain.set_developmental_stage("Adult")
        
        # Verify configuration was updated
        assert brain.get_developmental_stage() == "Adult"
        
        print("✓ Configuration modification test passed")
    
    def test_error_handling(self):
        """Test error handling"""
        config_dict = get_default_config()
        config_dict["neuron_count"] = 10  # Small brain for testing
        
        world, brain, agent_brain = create_agent_simulation(config_dict)
        
        # Test that brain operations don't crash
        brain.step(0)
        brain.reset()
        
        # Produce action
        action = brain.produce_action()
        assert action is not None
        
        print("✓ Error handling test passed")

if __name__ == "__main__":
    # Run tests manually if script is executed directly
    import sys
    pytest.main([__file__, "-v"])
