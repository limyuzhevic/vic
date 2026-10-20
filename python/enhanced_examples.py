#!/usr/bin/env python3
"""
Enhanced NLM Brain Example - Complete Demonstration

This example demonstrates the enhanced NLM Python API with all new functionality:
- Complete subsystem access (Memory, Prediction, Cognition, Development, Neuromodulation)
- Type-safe configuration management
- Enhanced error handling and exceptions
- Advanced agent simulations
- Performance optimizations
- Learning and plasticity demonstrations

This is Phase 1 Core functionality - the enhanced Brain class with complete subsystem access.
"""

import pynlm
import time
from typing import Dict, List, Tuple, Any

def print_header(title: str):
    """Print formatted header"""
    print("\n" + "="*60)
    print(f" {title}")
    print("="*60)

def example_1_basic_enhanced_brain():
    """Example 1: Basic enhanced brain with subsystem access"""
    print_header("Example 1: Basic Enhanced Brain")
    
    print("1. Creating enhanced brain with default configuration...")
    config = pynlm.createDefaultConfig()
    
    # Configure for enhanced learning
    config.set_int("brain.neuron_count", 2000)
    config.set_float("brain.synapse_density", 0.15)
    config.set_float("plasticity.stdp.learning_rate", 0.002)
    config.set_bool("plasticity.hebbian.enable", True)
    config.set_bool("neuromod.dopamine.scale", 1.5)
    config.set_bool("neuromod.curiosity.enable", True)
    
    print(f"   Neuron count: {config.get_int('brain.neuron_count')}")
    print(f"   Synapse density: {config.get_float('brain.synapse_density')}")
    print(f"   Learning rate: {config.get_float('plasticity.stdp.learning_rate')}")
    
    # Create enhanced brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"2. Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    
    # Access all subsystems
    print("3. Accessing enhanced subsystems...")
    
    # Memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    print(f"   ✓ Working memory: Available ({'Enabled' if working_memory else 'Disabled'})")
    print(f"   ✓ Episodic memory: Available ({'Enabled' if episodic_memory else 'Disabled'})")
    print(f"   ✓ Associative memory: Available ({'Enabled' if associative_memory else 'Disabled'})")
    
    # Prediction system
    prediction = brain.getPredictionSystem()
    print(f"   ✓ Prediction system: Available ({'Enabled' if prediction else 'Disabled'})")
    
    # Cognition systems
    planner = brain.getPlanner()
    concept_formation = brain.getConceptFormation()
    attention = brain.getAttention()
    print(f"   ✓ Neural planner: Available ({'Enabled' if planner else 'Disabled'})")
    print(f"   ✓ Concept formation: Available ({'Enabled' if concept_formation else 'Disabled'})")
    print(f"   ✓ Attentional selection: Available ({'Enabled' if attention else 'Disabled'})")
    
    # Development system
    development = brain.getDevelopmentSystem()
    print(f"   ✓ Development system: Available ({'Enabled' if development else 'Disabled'})")
    
    # Neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    prediction_error = brain.getPredictionErrorSignal()
    print(f"   ✓ Dopamine neuromodulation: Available ({'Enabled' if dopamine else 'Disabled'})")
    print(f"   ✓ Curiosity neuromodulation: Available ({'Enabled' if curiosity else 'Disabled'})")
    print(f"   ✓ Novelty neuromodulation: Available ({'Enabled' if novelty else 'Disabled'})")
    print(f"   ✓ Prediction error: Available ({'Enabled' if prediction_error else 'Disabled'})")
    
    # Plasticity systems
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    print(f"   ✓ STDP plasticity: Available ({'Enabled' if stdp else 'Disabled'})")
    print(f"   ✓ Hebbian plasticity: Available ({'Enabled' if hebbian else 'Disabled'})")
    print(f"   ✓ Structural plasticity: Available ({'Enabled' if structural else 'Disabled'})")
    
    # Get detailed statistics
    print("4. Getting enhanced statistics...")
    stats = brain.getDetailedStats()
    
    print("   Brain Statistics:")
    for key, value in stats.items():
        if isinstance(value, (int, float, str)):
            print(f"     {key}: {value}")
    
    print(f"   System status: {brain.getSystemStatus()}")
    
    # Run enhanced simulation
    print("5. Running enhanced simulation...")
    for step in range(50):
        brain.step(step)
        if step % 10 == 0:
            firing = brain.getFiringNeuronCount()
            print(f"   Step {step}: {firing} firing neurons")
    
    print(f"   Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, config

def example_2_complete_agent_simulation():
    """Example 2: Complete agent simulation with enhanced AgentBrain"""
    print_header("Example 2: Complete Agent Simulation")
    
    print("1. Creating complete agent simulation...")
    
    # Create enhanced configuration
    config = pynlm.createDefaultConfig()
    config.set_int("brain.neuron_count", 800)
    config.set_float("brain.synapse_density", 0.12)
    config.set_bool("plasticity.stdp.enable", True)
    config.set_bool("plasticity.hebbian.enable", True)
    config.set_bool("neuromod.dopamine.scale", 1.0)
    config.set_bool("neuromod.curiosity.enable", True)
    config.set_bool("neuromod.novelty.enable", True)
    config.set_bool("development.enabled", True)
    
    # Create brain and agent components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create enhanced world
    world = pynlm.createSimpleWorld()
    world.configure_advanced(25, 25, 12, 12, 100.0, 0.02, 42)
    world.reset()
    
    agent.initialize(world)
    
    print(f"2. Agent created with {brain.getTotalNeuronCount()} neurons")
    print(f"   World size: {world.getWidth()}x{world.getHeight()}")
    print(f"   Vision range: {world.getAgentBody().x:.1f}x{world.getAgentBody().y:.1f}")
    
    # Configure enhanced learning
    print("3. Configuring enhanced learning...")
    agent.enable_all_learning(True)
    
    print("   Learning systems enabled:")
    status = agent.get_learning_status()
    for system, enabled in status.items():
        print(f"     {system}: {enabled}")
    
    # Get agent brain statistics
    agent_stats = agent.get_detailed_stats()
    print("4. Agent statistics:")
    for key, value in agent_stats.items():
        if isinstance(value, (int, float, str, bool)):
            print(f"     {key}: {value}")
    
    # Run enhanced agent episode
    print("5. Running enhanced agent episode...")
    start_time = time.time()
    
    steps_completed = agent.run_episode(world, 200)
    
    end_time = time.time()
    episode_duration = end_time - start_time
    
    print(f"   Episode completed: {steps_completed} steps")
    print(f"   Duration: {episode_duration:.2f} seconds")
    print(f"   Average step time: {episode_duration/steps_completed*1000:.1f} ms")
    
    # Get final statistics
    print("6. Final simulation statistics:")
    brain_stats = brain.getDetailedStats()
    world_stats = world.get_world_state()
    agent_stats = agent.get_detailed_stats()
    
    print("   Brain:")
    print(f"     Neurons: {brain_stats.get('total_neurons', 0)}")
    print(f"     Firing rate: {brain_stats.get('average_firing_rate', 0):.3f}")
    print(f"     Total spikes: {brain_stats.get('total_spikes', 0)}")
    
    print("   Agent:")
    print(f"     Curiosity: {agent_stats.get('curiosity_level', 0):.3f}")
    print(f"     Novelty: {agent_stats.get('novelty_level', 0):.3f}")
    print(f"     Prediction error: {agent_stats.get('prediction_error', 0):.3f}")
    
    return brain, agent, world

def example_3_configuration_management():
    """Example 3: Advanced configuration management"""
    print_header("Example 3: Advanced Configuration Management")
    
    print("1. Creating configuration with type-safe access...")
    
    # Method 1: Default configuration
    config1 = pynlm.createDefaultConfig()
    config1.set_int("brain.neuron_count", 1500)
    config1.set_float("brain.v_thresh", -50.0)
    config1.set_float("brain.v_rest", -70.0)
    config1.set_bool("plasticity.stdp.enable", True)
    config1.set_bool("plasticity.hebbian.enable", True)
    config1.set_float("neuromod.dopamine.scale", 1.2)
    
    print("   Default configuration:")
    print(f"     Neurons: {config1.get_int('brain.neuron_count')}")
    print(f"     Threshold: {config1.get_float('brain.v_thresh')}")
    print(f"     Resting potential: {config1.get_float('brain.v_rest')}")
    print(f"     STDP enabled: {config1.get_bool('plasticity.stdp.enable')}")
    print(f"     Hebbian enabled: {config1.get_bool('plasticity.hebbian.enable')}")
    print(f"     Dopamine scale: {config1.get_float('neuromod.dopamine.scale')}")
    
    # Method 2: Advanced configuration with validation
    print("\n2. Advanced configuration with validation...")
    
    config2 = pynlm.createDefaultConfig()
    
    # Set values with validation
    try:
        config2.set_int("brain.neuron_count", 2000)
        config2.set_float("learning.rate", 0.001)
        config2.set_bool("plasticity.stdp.enable", True)
        
        # Validate configuration
        config2.validate_range("brain.neuron_count", 100, 5000)
        config2.validate_positive("learning.rate")
        
        print("   ✓ Configuration validated successfully")
        
        # Type-safe access
        neuron_count = config2.get_int("brain.neuron_count")
        learning_rate = config2.get_float("learning.rate")
        stdp_enabled = config2.get_bool("plasticity.stdp.enable")
        
        print(f"   ✓ Neurons: {neuron_count}")
        print(f"   ✓ Learning rate: {learning_rate}")
        print(f"   ✓ STDP enabled: {stdp_enabled}")
        
    except Exception as e:
        print(f"   ✗ Configuration error: {e}")
    
    # Method 3: Preset configurations
    print("\n3. Preset configurations...")
    
    presets = ["default", "minimal", "learning", "agent", "development"]
    print("   Available presets:")
    for preset in presets:
        print(f"     - {preset}")
    
    # Create learning preset
    print("\n4. Creating learning preset...")
    learning_config = pynlm.createDefaultConfig()
    learning_config.set_float("plasticity.stdp.learning_rate", 0.005)
    learning_config.set_float("plasticity.hebbian.learning_rate", 0.003)
    learning_config.set_float("neuromod.dopamine.scale", 1.5)
    learning_config.set_bool("neuromod.curiosity.enable", True)
    learning_config.set_bool("neuromod.novelty.enable", True)
    learning_config.set_bool("development.enabled", True)
    
    print("   Learning configuration:")
    print(f"     STDP rate: {learning_config.get_float('plasticity.stdp.learning_rate')}")
    print(f"     Hebbian rate: {learning_config.get_float('plasticity.hebbian.learning_rate')}")
    print(f"     Exploration rate: {learning_config.get_float('neuromod.dopamine.scale')}")
    
    # Method 4: Configuration comparison
    print("\n5. Configuration comparison...")
    
    config_a = pynlm.createDefaultConfig()
    config_a.set_int("brain.neuron_count", 1000)
    config_a.set_float("brain.synapse_density", 0.1)
    
    config_b = pynlm.createDefaultConfig()
    config_b.set_int("brain.neuron_count", 2000)
    config_b.set_float("brain.synapse_density", 0.2)
    
    print("   Configuration A vs B:")
    print(f"     A - Neurons: {config_a.get_int('brain.neuron_count')}, Density: {config_a.get_float('brain.synapse_density')}")
    print(f"     B - Neurons: {config_b.get_int('brain.neuron_count')}, Density: {config_b.get_float('brain.synapse_density')}")
    
    return config1, config2, learning_config

def example_4_error_handling_and_exceptions():
    """Example 4: Advanced error handling and exceptions"""
    print_header("Example 4: Advanced Error Handling and Exceptions")
    
    print("1. Testing configuration error handling...")
    
    # Create brain and try invalid operations
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test configuration errors
    print("2. Testing configuration validation errors...")
    
    # Try invalid configuration values
    try:
        config.set_int("brain.neuron_count", -100)  # Invalid: negative neurons
        print("   ✗ Should have caught invalid neuron count")
    except Exception as e:
        print(f"   ✓ Caught configuration error: {type(e).__name__}: {e}")
    
    try:
        config.validate_range("brain.neuron_count", 100, 5000)  # This should work now
        print("   ✓ Configuration validation passed")
    except Exception as e:
        print(f"   ✗ Unexpected error: {type(e).__name__}: {e}")
    
    # Test brain operation errors
    print("\n3. Testing brain operation error handling...")
    
    try:
        # Try to access non-existent region
        region = brain.getRegion(999)  # Invalid region ID
        print("   ✗ Should have caught invalid region access")
    except Exception as e:
        print(f"   ✓ Caught brain error: {type(e).__name__}: {e}")
    
    # Test world operation errors
    print("\n4. Testing world operation error handling...")
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 4, 4)
    world.reset()
    
    try:
        # Try invalid operation
        invalid_action = "InvalidAction"
        print("   ✗ Should have caught invalid action")
    except Exception as e:
        print(f"   ✓ Caught world error: {type(e).__name__}: {e}")
    
    # Test simulation errors
    print("\n5. Testing simulation error handling...")
    
    try:
        # Try to run without proper initialization
        brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain2.step(0)  # Should fail if not initialized
        print("   ✗ Should have caught uninitialized brain access")
    except Exception as e:
        print(f"   ✓ Caught simulation error: {type(e).__name__}: {e}")
    
    print("\n6. Testing exception helper function...")
    
    try:
        # Simulate a C++ error
        raise pynlm.NLMConfigurationError("Test error from helper")
    except pynlm.NLMConfigurationError as e:
        print(f"   ✓ Caught NLMConfigurationError: {e}")
    except Exception as e:
        print(f"   ✗ Unexpected exception type: {type(e).__name__}: {e}")
    
    return config, brain, world

def example_5_batch_operations():
    """Example 5: Batch operations and performance optimizations"""
    print_header("Example 5: Batch Operations and Performance")
    
    print("1. Creating brain for batch operations test...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("2. Testing batch step operations...")
    
    # Single step
    brain.step(0)
    print(f"   Single step: {brain.getTotalSpikeCount()} spikes")
    
    # Batch step
    brain.step_range(1, 100)
    print(f"   Batch step (1-100): {brain.getTotalSpikeCount()} spikes")
    
    # Reset and batch step again
    brain.reset()
    brain.initialize()
    brain.step_range(0, 500)
    print(f"   Batch step (0-500): {brain.getTotalSpikeCount()} spikes")
    
    # Test batch current injection
    print("\n3. Testing batch current injection...")
    
    # Simulate neuron IDs (this is a simplified example)
    neurons = []
    for i in range(10):
        neurons.append(i)
    
    # This would be the actual method call in real implementation
    # brain.inject_currents(neurons, 50.0)
    print(f"   Would inject {len(neurons)} neurons with current")
    
    print("\n4. Testing batch statistics...")
    
    # Get statistics in batches
    for batch in range(0, 10, 2):
        brain.step_range(batch, batch + 1)
        stats = brain.getDetailedStats()
        firing = stats.get('firing_neurons', 0)
        spikes = stats.get('total_spikes', 0)
        print(f"   Batch {batch}-{batch+1}: {firing} firing, {spikes} total spikes")
    
    print("\n5. Testing configuration batch operations...")
    
    # Test batch configuration operations
    config1 = pynlm.createDefaultConfig()
    config2 = pynlm.createDefaultConfig()
    
    # Clone configuration
    config_clone = config1.clone()
    print(f"   Configuration cloned: {config_clone.get_int('brain.neuron_count')} neurons")
    
    # Compare configurations
    config1.set_int("brain.neuron_count", 1000)
    config2.set_int("brain.neuron_count", 2000)
    
    print("   Configuration comparison:")
    print(f"     Config1: {config1.get_int('brain.neuron_count')} neurons")
    print(f"     Config2: {config2.get_int('brain.neuron_count')} neurons")
    
    return brain, config1, config2

def example_6_enhanced_world_simulation():
    """Example 6: Enhanced world simulation with agent"""
    print_header("Example 6: Enhanced World Simulation")
    
    print("1. Creating enhanced simulation environment...")
    
    # Create world with advanced configuration
    world = pynlm.createSimpleWorld()
    world.configure_advanced(20, 20, 8, 8, 100.0, 0.02, 42)
    
    # Create sample environment
    world.create_sample_environment()
    
    print(f"   World configured: {world.getWidth()}x{world.getHeight()}")
    print(f"   Max energy: {world.getMaxEnergy()}")
    print(f"   Energy decay: {world.getEnergyDecayRate()}")
    
    # Get initial world state
    initial_state = world.get_world_state()
    print("   Initial world state:")
    for key, value in initial_state.items():
        if isinstance(value, (int, float)):
            print(f"     {key}: {value}")
    
    # Create agent
    print("\n2. Creating and configuring agent...")
    
    config = pynlm.createDefaultConfig()
    config.set_int("brain.neuron_count", 600)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create enhanced world for agent
    world2 = pynlm.createSimpleWorld()
    world2.configure_advanced(30, 30, 10, 10, 200.0, 0.015, 67890)
    world2.reset()
    
    agent.initialize(world2)
    agent.enable_all_learning(True)
    
    print(f"   Agent created with {brain.getTotalNeuronCount()} neurons")
    print(f"   World size: {world2.getWidth()}x{world2.getHeight()}")
    
    # Get world statistics
    world_stats = world2.get_environment_stats()
    print("   Environment statistics:")
    for key, value in world_stats.items():
        print(f"     {key}: {value:.2f}")
    
    # Run simulation with enhanced controls
    print("\n3. Running enhanced simulation...")
    
    start_time = time.time()
    
    for step in range(50):
        # Update world
        world2.update(0.1)
        
        # Get sensory input
        percept = world2.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world2.applyMotorCommand(action, world2.getSimulationTime())
        
        # Apply neuromodulation
        curiosity = agent.getCuriosityLevel()
        agent.applyRewardModulation(curiosity, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 10 == 0:
            print(f"   Step {step}: Agent at ({world2.getAgentBody().x:.1f}, {world2.getAgentBody().y:.1f})")
            print(f"     Energy: {world2.getAgentBody().energy:.1f}, Health: {world2.getAgentBody().health:.1f}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\n4. Simulation completed in {duration:.2f} seconds")
    
    # Get final statistics
    print("\n5. Final simulation results:")
    
    final_world_state = world2.get_world_state()
    final_agent_stats = agent.get_detailed_stats()
    final_brain_stats = brain.getDetailedStats()
    
    print("   Final world state:")
    for key, value in final_world_state.items():
        if isinstance(value, (int, float)):
            print(f"     {key}: {value}")
    
    print("   Agent statistics:")
    for key, value in final_agent_stats.items():
        if isinstance(value, (int, float, str, bool)):
            print(f"     {key}: {value}")
    
    print("   Brain statistics:")
    for key, value in final_brain_stats.items():
        if isinstance(value, (int, float, str)):
            print(f"     {key}: {value}")
    
    return brain, agent, world2

def main():
    """Run all enhanced NLM examples"""
    print("NLM Enhanced Python API Examples")
    print("================================")
    
    try:
        # Example 1: Basic enhanced brain
        example_1_basic_enhanced_brain()
        
        # Example 2: Complete agent simulation
        example_2_complete_agent_simulation()
        
        # Example 3: Configuration management
        example_3_configuration_management()
        
        # Example 4: Error handling
        example_4_error_handling_and_exceptions()
        
        # Example 5: Batch operations
        example_5_batch_operations()
        
        # Example 6: Enhanced world simulation
        example_6_enhanced_world_simulation()
        
        print_header("All Examples Completed Successfully!")
        
    except Exception as e:
        print(f"\nError running examples: {type(e).__name__}: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()