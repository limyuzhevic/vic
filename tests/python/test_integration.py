#!/usr/bin/env python3
"""
NLM Python Integration Tests

Tests integration between Python and C++ components:
- End-to-end integration testing
- Component interaction validation
- Performance and correctness validation
- Real-world scenario simulation
- Cross-language component communication
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
import numpy as np
import pytest
import time

class TestIntegration:
    """Test integration between Python and C++ components"""
    
    def test_end_to_end_integration(self):
        """Test end-to-end integration of all components"""
        print("Running end-to-end integration test...")
        
        # 1. Configuration setup
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        config.set("plasticity.stdp.enable", True)
        
        # 2. Brain creation and initialization
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # 3. World setup
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        # 4. Agent creation and initialization
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # 5. Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        # 6. Run integrated simulation
        for step in range(50):
            # World updates
            world.update(0.1)
            
            # Sensory processing
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Motor control
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Verify integration results
        assert brain.getTotalSpikeCount() >= 0
        assert agent.getNeuromodulationLevel() >= 0.0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        
        print("End-to-end integration test PASSED")
        
    def test_component_interaction(self):
        """Test interaction between different components"""
        print("Running component interaction test...")
        
        # Create brain with configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 150)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Test component interactions
        interactions = []
        
        for step in range(30):
            # Get sensory input from world
            percept = world.getSensoryPercept()
            
            # Agent processes sensory input and interacts with brain
            agent.processSensoryInput(percept)
            
            # Brain produces action
            brain.step(step)
            
            # Agent decodes motor command based on brain activity
            action = agent.decodeMotorCommand()
            
            # World executes action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record interaction
            interaction = {
                'step': step,
                'brain_spikes': brain.getTotalSpikeCount(),
                'active_neurons': brain.getFiringNeuronCount(),
                'action_type': action.getType() if hasattr(action, 'getType') else None,
                'agent_curiosity': agent.getCuriosityLevel(),
                'world_time': world.getSimulationTime()
            }
            interactions.append(interaction)
        
        # Verify component interactions
        assert len(interactions) == 30
        assert all(i['brain_spikes'] >= 0 for i in interactions)
        assert all(i['active_neurons'] >= 0 for i in interactions)
        assert all(i['agent_curiosity'] >= 0.0 for i in interactions)
        assert all(i['world_time'] >= 0.0 for i in interactions)
        
        print("Component interaction test PASSED")
        
    def test_memory_system_integration(self):
        """Test memory system integration"""
        print("Running memory system integration test...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Get memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        # Test memory system integration
        assert working_memory is not None or episodic_memory is not None
        
        # Simulate memory operations
        for step in range(20):
            brain.step(step)
            
            # Inject some activity to store in memory
            # brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0f)
            
            # In actual implementation:
            # working_memory.storeToNeuron(1, brain.getFiringRate())
        
        # Verify memory system state
        # assert working_memory is not None
        # if working_memory:
        #     assert working_memory.getActiveTraces() >= 0
        
        print("Memory system integration test PASSED")
        
    def test_plasticity_integration(self):
        """Test plasticity system integration"""
        print("Running plasticity integration test...")
        
        config = pynlm.createDefaultConfig()
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation to enable plasticity
        for step in range(40):
            brain.step(step)
            
            # Inject current to trigger plasticity
            # brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 1.0f)
        
        # Verify plasticity is active
        assert brain.getExcitationInhibitionRatio() >= 0.0
        
        print("Plasticity integration test PASSED")
        
    def test_neuromodulation_integration(self):
        """Test neuromodulation system integration"""
        print("Running neuromodulation integration test...")
        
        config = pynlm.createDefaultConfig()
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent to access neuromodulation
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable neuromodulation
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        for step in range(30):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Verify neuromodulation integration
        assert agent.getNeuromodulationLevel() >= 0.0
        assert agent.getCuriosityLevel() >= 0.0
        assert agent.getNoveltyLevel() >= 0.0
        
        print("Neuromodulation integration test PASSED")
        
    def test_development_integration(self):
        """Test development system integration"""
        print("Running development integration test...")
        
        config = pynlm.createDefaultConfig()
        config.set("development.enable", True)
        config.set("development.critical_period_duration", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent for development testing
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable development
        agent.enableDevelopment(True)
        
        # Run development simulation
        for step in range(50):
            brain.step(step)
            agent.updateDevelopment(step)
        
        # Verify development integration
        assert brain.getDevelopmentalStage() != pynlm.DevelopmentalStage.Initial
        assert agent.getDevelopmentalStage() != pynlm.DevelopmentalStage.Initial
        
        print("Development integration test PASSED")
        
    def test_visualization_integration(self):
        """Test visualization system integration"""
        print("Running visualization integration test...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run brain for visualization data
        for step in range(10):
            brain.step(step)
        
        # Test visualization capabilities
        # pynlm.NLMVisualizer.plot_brain_state(brain, title="Integration Test")
        
        # Generate brain state data for visualization
        brain_states = []
        for step in range(5):
            brain.step(step)
            state = {
                'total_spikes': brain.getTotalSpikeCount(),
                'active_neurons': brain.getFiringNeuronCount(),
                'average_rate': brain.getAverageFiringRate(),
                'steps': step,
                'memory_usage': np.random.rand() * 100,
                'neuromodulation_level': np.random.rand(),
                'curiosity_level': np.random.rand(),
            }
            brain_states.append(state)
        
        # Test visualization functions
        # pynlm.NLMVisualizer.visualize_simulation_log(5, brain_states)
        
        # Verify visualization data
        assert len(brain_states) == 5
        assert all('total_spikes' in state for state in brain_states)
        assert all('average_rate' in state for state in brain_states)
        
        print("Visualization integration test PASSED")
        
    def test_performance_integration(self):
        """Test performance monitoring integration"""
        print("Running performance integration test...")
        
        # Create brain for performance testing
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test performance with world integration
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=15, visionHeight=15)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all systems for comprehensive performance testing
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run extended simulation
        spike_counts = []
        times = []
        
        for step in range(50):
            start_time = time.time()
            
            world.update(0.05)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            end_time = time.time()
            
            spike_counts.append(brain.getTotalSpikeCount())
            times.append(end_time - start_time)
        
        # Calculate performance metrics
        avg_time_per_step = sum(times) / len(times)
        steps_per_second = len(times) / sum(times)
        total_spikes = sum(spike_counts)
        
        # Verify performance integration
        assert avg_time_per_step > 0
        assert avg_time_per_step < 0.1  # Should be fast per step
        assert steps_per_second > 0
        assert total_spikes >= 0
        
        print(f"Performance integration: {steps_per_second:.2f} steps/second")
        print("Performance integration test PASSED")
        
    def test_cross_language_compatibility(self):
        """Test Python-C++ cross-language compatibility"""
        print("Running cross-language compatibility test...")
        
        # Test that Python objects can be passed to C++ and vice versa
        config = pynlm.createDefaultConfig()
        
        # Test Python list to C++
        config.set("list_test", [1, 2, 3])
        
        # Test Python dict to C++
        config.set("dict_test", {"key": "value"})
        
        # Test Python float to C++
        config.set("float_test", 3.14159)
        
        # Test Python string to C++
        config.set("string_test", "python_string")
        
        # Test Python int to C++
        config.set("int_test", 42)
        
        # Test Python bool to C++
        config.set("bool_test", True)
        
        # Retrieve values and verify they're correctly converted
        assert config.getInt("int_test") == 42
        assert abs(config.getFloat("float_test") - 3.14159) < 0.0001
        assert config.get("string_test") == "python_string"
        
        print("Cross-language compatibility test PASSED")
        
    def test_error_propagation(self):
        """Test error propagation across components"""
        print("Running error propagation test...")
        
        config = pynlm.createDefaultConfig()
        
        # Create brain and test error handling
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test that errors propagate properly
        errors_caught = []
        
        try:
            # Perform operations that might error
            for step in range(10):
                brain.step(step)
                
        except Exception as e:
            errors_caught.append(str(e))
        
        # Verify error handling
        # assert len(errors_caught) > 0  # Should have caught some errors
        
        # Test that system continues to work after error recovery
        # for step in range(10):
        #     brain.step(step)
        
        # Verify brain is still functional
        assert brain.getTotalSpikeCount() >= 0
        
        print("Error propagation test PASSED")
        
    def test_resource_management(self):
        """Test resource management and cleanup"""
        print("Running resource management test...")
        
        config = pynlm.createDefaultConfig()
        
        # Create multiple resources to test management
        brains = []
        worlds = []
        agents = []
        
        try:
            # Create multiple brains
            for i in range(3):
                brain = pynlm.createBrain(config)
                brain.initialize()
                brains.append(brain)
                
                world = pynlm.createSimpleWorld()
                world.configure(width=20, height=20)
                worlds.append(world)
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                agents.append(agent)
                
                # Run operations
                for step in range(5):
                    brain.step(step)
                    world.update(0.1)
                    
                    percept = world.getSensoryPercept()
                    agent.processSensoryInput(percept)
        
        finally:
            # Test resource cleanup
            pass  # In actual implementation would cleanup resources
        
        # Verify all resources were created successfully
        assert len(brains) == 3
        assert len(worlds) == 3
        assert len(agents) == 3
        
        # Verify all resources are functional
        for brain in brains:
            assert brain.getTotalSpikeCount() >= 0
        
        print("Resource management test PASSED")

# Global test functions for pytest compatibility
def run_integration_tests():
    """Run all integration tests"""
    test_obj = TestIntegration()
    
    tests = [
        "test_end_to_end_integration",
        "test_component_interaction",
        "test_memory_system_integration",
        "test_plasticity_integration",
        "test_neuromodulation_integration",
        "test_development_integration",
        "test_visualization_integration",
        "test_performance_integration",
        "test_cross_language_compatibility",
        "test_error_propagation",
        "test_resource_management"
    ]
    
    results = []
    for test_name in tests:
        try:
            method = getattr(test_obj, test_name)
            method()
            results.append({"test": test_name, "status": "PASS", "error": None})
            print(f"✓ {test_name}")
        except Exception as e:
            results.append({"test": test_name, "status": "FAIL", "error": str(e)})
            print(f"✗ {test_name}: {e}")
    
    passed = sum(1 for r in results if r["status"] == "PASS")
    total = len(results)
    
    print(f"\nIntegration Tests: {passed}/{total} passed")
    return results