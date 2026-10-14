#!/usr/bin/env python3
"""
Demonstration of NLM Enhanced Python API for Power Users

This script demonstrates the new advanced features and utilities
added to the NLM Python bindings for power users.
"""

import sys
import os
import time

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm

def demo_basic_enhancements():
    """Demonstrate basic API enhancements"""
    print("1. Basic API Enhancements")
    print("-" * 40)
    
    # Create brain with enhanced config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   Has {brain.getTotalSynapseCount()} synapses")
    
    # Demonstrate enhanced config methods
    config.set("brain.neuron_count", 1500, pynlm.ConfigSource.Runtime)
    config.set("test.value", 3.14159, pynlm.ConfigSource.Runtime)
    
    print(f"   Set neuron count to {config.getOr('brain.neuron_count', 1000)}")
    print(f"   Get float value: {config.getOrFloat('test.value', 0.0)}")
    
    # Demonstrate plasticity access
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    print(f"   Access to STDP: {'Available' if stdp else 'Not available'}")
    print(f"   Access to Hebbian: {'Available' if hebbian else 'Not available'}")
    
    print()

def demo_power_user_utilities():
    """Demonstrate power user utilities"""
    print("2. Power User Utilities")
    print("-" * 40)
    
    # Create complex setup with advanced utilities
    config = pynlm.createComplexConfig()
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionWidth=10)
    world.reset()
    
    print(f"   Created complex config with {config.getOr('brain.neuron_count', 1000)} neurons")
    print(f"   World configured: {world.getWidth()}x{world.getHeight()}")
    
    # Create complete agent setup
    brain, agent, world_obj = pynlm.createCompleteSetup(config, world)
    
    print("   Complete agent setup created with all subsystems enabled:")
    print(f"     - Reward Modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"     - Structural Plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    print(f"     - Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    print(f"     - Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    
    # Demonstrate metrics collection
    metrics = agent.getAllMetrics()
    print("   Agent metrics collected:")
    for key, value in metrics.items():
        print(f"     - {key}: {value}")
    
    # Demonstrate world state access
    world_info = agent.getWorldInfo(world_obj)
    print("   World state (agent):")
    for key, value in world_info.items():
        if key in ['x', 'y']:
            print(f"     - Position: ({value:.1f}, {value:.1f})")
        else:
            print(f"     - {key}: {value:.3f}")
    
    print()

def demo_advanced_configuration():
    """Demonstrate advanced configuration utilities"""
    print("3. Advanced Configuration Utilities")
    print("-" * 40)
    
    # Create config from scratch
    config = pynlm.createDefaultConfig()
    
    # Set multiple values
    config.setInt("brain.neuron_count", 2000)
    config.setFloat("brain.synapse_density", 0.2)
    config.setString("brain.name", "ResearchBrain")
    config.setBool("plasticity.stdp.enable", True)
    config.setFloat("neuromod.curiosity.scale", 2.0)
    
    print("   Created custom configuration:")
    print(f"     - Neuron count: {config.getOrInt('brain.neuron_count', 1000)}")
    print(f"     - Synapse density: {config.getOrFloat('brain.synapse_density', 0.1)}")
    print(f"     - Brain name: {config.getOrString('brain.name', 'Unknown')}")
    print(f"     - STDP enabled: {config.getOrBool('plasticity.stdp.enable', False)}")
    print(f"     - Curiosity scale: {config.getOrFloat('neuromod.curiosity.scale', 1.0)}")
    
    # Demonstrate export/import
    exported = config.export()
    print(f"   Exported config with {len(exported)} keys")
    
    # Create new config and import
    config2 = pynlm.createDefaultConfig()
    config2.import(exported)
    print(f"   Imported config has {config2.getOr('brain.neuron_count', 0)} neurons")
    
    # Demonstrate validation
    errors = config.validate()
    if errors:
        print("   Validation results:")
        for error_type, error_list in errors.items():
            for error in error_list:
                print(f"     - [{error_type}] {error}")
    else:
        print("   Configuration validation passed")
    
    print()

def demo_world_exploration():
    """Demonstrate world exploration utilities"""
    print("4. World Exploration Utilities")
    print("-" * 40)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Add some objects to the world
    world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, value=1.0))
    world.addObject(pynlm.WorldObject(30, 30, pynlm.WorldObjectType.Hazard, value=0.5))
    world.addObject(pynlm.WorldObject(40, 20, pynlm.WorldObjectType.Resource, value=2.0))
    
    print(f"   Created world: {world.getWidth()}x{world.getHeight()}")
    print(f"   Placed 3 objects in world")
    
    # Explore world objects
    resources = world.getObjectsByType(pynlm.WorldObjectType.Resource)
    hazards = world.getObjectsByType(pynlm.WorldObjectType.Hazard)
    
    print(f"   Found {len(resources)} resources")
    print(f"   Found {len(hazards)} hazards")
    
    # Find objects in range
    agent_pos = (25.0, 25.0)
    nearby = world.getObjectsInRange(agent_pos[0], agent_pos[1], 15.0)
    print(f"   Found {len(nearby)} objects within 15 units of agent")
    
    # Get world statistics
    stats = world.getWorldStats()
    print("   World statistics:")
    for key, value in stats.items():
        print(f"     - {key}: {value}")
    
    # Get sensory data
    all_sensors = world.getAllSensoryData()
    print(f"   Available sensor types: {list(all_sensors.keys())}")
    
    print()

def demo_simulation_loop():
    """Demonstrate complete simulation with power user features"""
    print("5. Complete Simulation with Power User Features")
    print("-" * 40)
    
    # Create setup with advanced configuration
    config = pynlm.createComplexConfig()
    world = pynlm.createSimpleWorld()
    world.configure(width=60, height=60, visionWidth=12, visionHeight=12)
    world.reset()
    
    brain, agent, world_obj = pynlm.createCompleteSetup(config, world)
    
    print("   Simulation setup:")
    print(f"     - Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"     - World: {world_obj.getWidth()}x{world_obj.getHeight()}")
    print(f"     - Agent: {agent.getSensoryInputSize()} input, {agent.getMotorOutputSize()} output dimensions")
    
    # Run enhanced simulation
    print("   Running enhanced simulation...")
    
    step = 0
    total_spikes = 0
    curiosity_values = []
    novelty_values = []
    neuromodulation_values = []
    
    for step in range(200):
        # Update world
        world_obj.update(0.1)
        
        # Get world sensors
        all_sensors = agent.getWorldSensors(world_obj)
        
        # Process input
        agent.processSensoryInput(world_obj.getSensoryPercept())
        
        # Brain computation
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        world_obj.applyMotorCommand(motor_cmd, world_obj.getSimulationTime())
        
        # Apply reward and neuromodulation
        reward = 0.1 if step % 20 == 0 else 0.0
        predicted = 0.05
        agent.applyRewardModulation(reward, predicted)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        metrics = agent.getAllMetrics()
        curiosity_values.append(metrics['curiosity'])
        novelty_values.append(metrics['novelty'])
        neuromodulation_values.append(metrics['neuromodulation'])
        
        total_spikes += brain.getTotalSpikeCount()
        
        if step % 40 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity={metrics['curiosity']:.3f}, "
                  f"novelty={metrics['novelty']:.3f}")
    
    print("   Simulation complete!")
    print(f"   Results after {step + 1} steps:")
    print(f"     - Average curiosity: {sum(curiosity_values)/len(curiosity_values):.3f}")
    print(f"     - Average novelty: {sum(novelty_values)/len(novelty_values):.3f}")
    print(f"     - Average neuromodulation: {sum(neuromodulation_values)/len(neuromodulation_values):.3f}")
    print(f"     - Total spikes: {total_spikes}")
    
    # Get final world info
    final_info = agent.getWorldInfo(world_obj)
    print(f"   Final agent position: ({final_info['x']:.1f}, {final_info['y']:.1f})")
    
    print()

def demo_api_overview():
    """Display API overview"""
    print("6. Enhanced Python API Overview")
    print("-" * 40)
    
    print("   New Classes and Methods for Power Users:")
    print("   ")
    print("   Brain Class Enhancements:")
    print("     - getSTDP(), getHebbian(), getStructuralPlasticity() - Access plasticity systems")
    print("     - getSpikeSystem() - Access spike timing system")
    print("     - updatePlasticity(), develop() - Manual control of plasticity and development")
    print("     - getNeuromodulationLevel() - Access neuromodulation state")
    print("     - getTotalPendingSpikeEventCount() - Access pending events")
    print("   ")
    print("   AgentBrain Class Enhancements:")
    print("     - getAllMetrics() - Get all metrics as dictionary")
    print("     - getWorldSensors(world) - Get all sensory data as map")
    print("     - getWorldInfo(world) - Get comprehensive world state")
    print("     - Advanced system activation methods")
    print("   ")
    print("   SimpleWorld Class Enhancements:")
    print("     - getWorldObjects() - Get all objects")
    print("     - getObjectsByType(type) - Filter objects by type")
    print("     - countObjects() - Get object count")
    print("     - getObjectsInRange(x, y, radius) - Spatial queries")
    print("     - getAllSensoryData() - Get all sensory data")
    print("     - getAgentState() - Get agent state as map")
    print("     - exportObjects(), importObjects() - Serialization")
    print("   ")
    print("   Config Class Enhancements:")
    print("     - getOrInt(), getOrFloat(), getOrString(), getOrBool() - Type-specific getters")
    print("     - setInt(), setFloat(), setString(), setBool() - Type-specific setters")
    print("     - export(), import(), merge() - Configuration manipulation")
    print("     - validate() - Configuration validation")
    print("   ")
    print("   Module-level Utilities:")
    print("     - createComplexConfig() - Create advanced configuration")
    print("     - createAgentBrainWithWorld(config, world) - Quick agent setup")
    print("     - createCompleteSetup(config, world) - Complete agent setup")
    print("     - validateConfig(config) - Configuration validation")
    print("     - serializeBrain(), deserializeBrain() - Brain serialization")
    print("     - getBuildInfo() - API information")
    print("   ")
    
    # Build info
    build_info = pynlm.getBuildInfo()
    print(f"   API Version: {build_info['version']}")
    print(f"   Enhanced Features: {build_info['features']}")
    print(f"   Power User Utilities: {build_info['api_enhanced']}")
    
    print()

def main():
    """Run all demonstrations"""
    print("=" * 60)
    print("NLM Enhanced Python API - Power User Demonstration")
    print("=" * 60)
    print()
    
    demo_api_overview()
    demo_basic_enhancements()
    demo_power_user_utilities()
    demo_advanced_configuration()
    demo_world_exploration()
    demo_simulation_loop()
    
    print("=" * 60)
    print("All demonstrations completed successfully!")
    print("The enhanced NLM Python API now provides comprehensive")
    print("tools for power users to build advanced neural simulations.")
    print("=" * 60)

if __name__ == "__main__":
    main()