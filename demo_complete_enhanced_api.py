#!/usr/bin/env python3
"""
Final Comprehensive Demonstration of NLM Enhanced Python API

This script demonstrates all the enhancements made to the NLM Python API,
showcasing the new power user utilities and advanced features.
"""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm

def main():
    print("=" * 70)
    print("NLM Enhanced Python API - Comprehensive Demonstration")
    print("=" * 70)
    print()
    
    print("1. Enhanced Configuration Utilities")
    print("-" * 50)
    
    # Create complex configuration
    config = pynlm.createComplexConfig()
    print(f"Created complex config with {config.getOr('brain.neuron_count', 1000)} neurons")
    
    # Demonstrate type-specific configuration
    config.setInt("brain.neuron_count", 1500)
    config.setFloat("brain.synapse_density", 0.18)
    config.setString("brain.name", "ResearchBrain")
    config.setBool("plasticity.stdp.enable", True)
    
    print(f"Type-specific config:")
    print(f"  Neurons: {config.getOrInt('brain.neuron_count', 1000)}")
    print(f"  Synapse Density: {config.getOrFloat('brain.synapse_density', 0.1):.3f}")
    print(f"  Brain Name: {config.getOrString('brain.name', 'Unknown')}")
    print(f"  STDP Enabled: {config.getOrBool('plasticity.stdp.enable', False)}")
    
    # Export/Import
    exported = config.export()
    print(f"Config exported with {len(exported)} keys")
    
    config2 = pynlm.createDefaultConfig()
    config2.import(exported)
    print(f"Config imported - neuron count: {config2.getOr('brain.neuron_count', 0)}")
    
    # Validation
    errors = config.validate()
    if errors:
        print(f"Validation found {len(errors)} error categories")
    else:
        print("Configuration validation passed")
    
    print()
    print("2. Advanced Brain Features")
    print("-" * 50)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    # Access enhanced brain methods
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    spike_system = brain.getSpikeSystem()
    
    print(f"Enhanced brain methods:")
    print(f"  STDP system: {'Available' if stdp else 'Not available'}")
    print(f"  Hebbian system: {'Available' if hebbian else 'Not available'}")
    print(f"  Structural plasticity: {'Available' if structural else 'Not available'}")
    print(f"  Spike system: {'Available' if spike_system else 'Not available'}")
    
    # Demonstrate plasticity control
    brain.updatePlasticity()
    print("Plasticity update performed")
    
    brain.develop()
    print("Development update performed")
    
    # Development stage
    dev_stage = brain.getDevelopmentalStage()
    print(f"Current developmental stage: {dev_stage}")
    
    print()
    print("3. Complete Agent Setup (NEW)")
    print("-" * 50)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Create complete agent setup
    brain2, agent, world_obj = pynlm.createCompleteSetup(config, world)
    
    print(f"Complete agent setup created:")
    print(f"  Brain: {brain2.getTotalNeuronCount()} neurons")
    print(f"  World: {world_obj.getWidth()}x{world_obj.getHeight()}")
    print(f"  Agent subsystems:")
    print(f"    Reward Modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"    Structural Plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    print(f"    Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    print(f"    Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    
    print()
    print("4. Advanced Simulation with Power User Features")
    print("-" * 50)
    
    # Run enhanced simulation
    print("Running enhanced simulation...")
    
    step = 0
    total_spikes = 0
    curiosity_values = []
    novelty_values = []
    neuromodulation_values = []
    
    for step in range(150):
        # Update world
        world_obj.update(0.1)
        
        # Get comprehensive world sensors
        all_sensors = agent.getWorldSensors(world_obj)
        
        # Process sensory input
        agent.processSensoryInput(world_obj.getSensoryPercept())
        
        # Brain computation
        brain2.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        world_obj.applyMotorCommand(motor_cmd, world_obj.getSimulationTime())
        
        # Apply neuromodulation
        reward = 0.1 if step % 30 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        metrics = agent.getAllMetrics()
        curiosity_values.append(metrics['curiosity'])
        novelty_values.append(metrics['novelty'])
        neuromodulation_values.append(metrics['neuromodulation'])
        
        total_spikes += brain2.getTotalSpikeCount()
        
        if step % 40 == 0:
            print(f"  Step {step}: {brain2.getFiringNeuronCount()} neurons firing, "
                  f"curiosity={metrics['curiosity']:.3f}, "
                  f"novelty={metrics['novelty']:.3f}")
    
    print(f"Simulation complete after {step + 1} steps!")
    
    # Analyze results
    avg_curiosity = sum(curiosity_values) / len(curiosity_values) if curiosity_values else 0
    avg_novelty = sum(novelty_values) / len(novelty_values) if novelty_values else 0
    avg_neuromod = sum(neuromodulation_values) / len(neuromodulation_values) if neuromodulation_values else 0
    
    print(f"Results:")
    print(f"  Average curiosity: {avg_curiosity:.3f}")
    print(f"  Average novelty: {avg_novelty:.3f}")
    print(f"  Average neuromodulation: {avg_neuromod:.3f}")
    print(f"  Total spikes: {total_spikes}")
    
    # Get world info
    world_info = agent.getWorldInfo(world_obj)
    print(f"Final agent position: ({world_info['x']:.1f}, {world_info['y']:.1f})")
    
    print()
    print("5. World Exploration Utilities")
    print("-" * 50)
    
    # Create new world for exploration
    explore_world = pynlm.createSimpleWorld()
    explore_world.configure(width=60, height=60, visionWidth=10, visionHeight=10)
    explore_world.reset()
    
    # Add various objects
    explore_world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, value=1.0))
    explore_world.addObject(pynlm.WorldObject(30, 30, pynlm.WorldObjectType.Hazard, value=0.5))
    explore_world.addObject(pynlm.WorldObject(50, 20, pynlm.WorldObjectType.Resource, value=2.0))
    explore_world.addObject(pynlm.WorldObject(20, 50, pynlm.WorldObjectType.Wall, value=0.0))
    explore_world.addObject(pynlm.WorldObject(40, 40, pynlm.WorldObjectType.Marker, value=0.1))
    
    print(f"Created exploration world with {explore_world.countObjects()} objects")
    
    # World exploration
    resources = explore_world.getObjectsByType(pynlm.WorldObjectType.Resource)
    hazards = explore_world.getObjectsByType(pynlm.WorldObjectType.Hazard)
    walls = explore_world.getObjectsByType(pynlm.WorldObjectType.Wall)
    markers = explore_world.getObjectsByType(pynlm.WorldObjectType.Marker)
    
    print(f"Object counts:")
    print(f"  Resources: {len(resources)}")
    print(f"  Hazards: {len(hazards)}")
    print(f"  Walls: {len(walls)}")
    print(f"  Markers: {len(markers)}")
    
    # Find objects in range
    agent_pos = (25, 25)
    nearby_resources = explore_world.getObjectsInRange(agent_pos[0], agent_pos[1], 15.0)
    print(f"Objects within 15 units: {len(nearby_resources)}")
    
    # Get comprehensive world statistics
    world_stats = explore_world.getWorldStats()
    print(f"World statistics:")
    for key, value in world_stats.items():
        print(f"  {key}: {value}")
    
    # Get all sensory data
    all_sensors = explore_world.getAllSensoryData()
    print(f"Available sensor types: {list(all_sensors.keys())}")
    
    # Get agent state
    agent_state = explore_world.getAgentState()
    print(f"Agent state:")
    for key, value in agent_state.items():
        if key in ['x', 'y']:
            print(f"  Position: ({value:.1f}, {value:.1f})")
        else:
            print(f"  {key}: {value:.3f}")
    
    print()
    print("6. Module-level Utilities")
    print("-" * 50)
    
    # Test configuration validation
    test_config = pynlm.createDefaultConfig()
    test_config.set("brain.neuron_count", 2000)
    
    validation_errors = pynlm.validateConfig(test_config)
    print(f"Configuration validation:")
    print(f"  Error categories: {len(validation_errors)}")
    
    # Test build info
    build_info = pynlm.getBuildInfo()
    print(f"Build information:")
    print(f"  Version: {build_info['version']}")
    print(f"  Enhanced: {build_info['api_enhanced']}")
    print(f"  Features: {build_info['features']}")
    
    print()
    print("7. Serialization Utilities")
    print("-" * 50)
    
    # Test serialization
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.bin', delete=False) as f:
        filepath = f.name
    
    try:
        # Save brain
        save_success = pynlm.serializeBrain(brain2, filepath)
        print(f"Brain save: {'Success' if save_success else 'Failed'}")
        
        # Load brain
        loaded_brain = pynlm.deserializeBrain(filepath)
        print(f"Brain load: {'Success' if loaded_brain else 'Failed'}")
        
        if loaded_brain:
            print(f"Loaded brain has {loaded_brain.getTotalNeuronCount()} neurons")
            
    finally:
        if os.path.exists(filepath):
            os.unlink(filepath)
    
    print()
    print("=" * 70)
    print("DEMONSTRATION COMPLETE")
    print("=" * 70)
    print()
    print("Key Features Demonstrated:")
    print("✓ Enhanced configuration utilities")
    print("✓ Advanced brain access methods")
    print("✓ Complete agent setup utilities")
    print("✓ Enhanced simulation with metrics collection")
    print("✓ World exploration and spatial queries")
    print("✓ Module-level utility functions")
    print("✓ Serialization and data export/import")
    print()
    print("The enhanced NLM Python API provides comprehensive tools for")
    print("power users to build advanced neural simulations and research applications.")
    print("=" * 70)

if __name__ == "__main__":
    main()