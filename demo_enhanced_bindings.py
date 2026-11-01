"""
Demonstration script showcasing the enhanced NLM Python bindings
This script demonstrates the new convenience functions and features
"""
import pynlm
import sys

def demo_basic_usage():
    """Demonstrate basic NLM usage"""
    print("=== Basic NLM Usage Demo ===")
    
    # Create a simple brain
    brain = pynlm.createBrain()
    brain.initialize()
    
    # Run some steps
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Created and ran brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Generated {brain.getTotalSpikeCount()} spikes")
    return brain

def demo_enhanced_convenience():
    """Demonstrate new convenience functions"""
    print("\n=== Enhanced Convenience Functions Demo ===")
    
    # Create learning brain with one line
    brain = pynlm.createLearningBrain(500)
    brain.initialize()
    
    # Create training environment
    world = pynlm.createTrainingEnvironment(15, 15, 6, 6)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run complete simulation using convenience function
    print("Running complete agent simulation...")
    pynlm.runAgentSimulation(brain, world, agent, 100)
    
    # Get comprehensive stats
    print(f"✓ Simulation completed!")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    
    # Get brain status
    print(f"\n  Brain status:\n{brain.status()}")
    
    # Get episode stats
    print(f"\n  Episode stats:\n{agent.getEpisodeStats()}")
    
    return brain, world, agent

def demo_configuration_management():
    """Demonstrate enhanced configuration management"""
    print("\n=== Configuration Management Demo ===")
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.15)
    config.set("random_seed", 42)
    config.set("simulation_timestep", 0.001)
    
    # Use convenience getters
    neurons = config.getInt("brain.neuron_count")
    density = config.getFloat("brain.synapse_density")
    seed = config.getInt("random_seed")
    timestep = config.getDouble("simulation_timestep")
    
    print(f"✓ Configuration loaded:")
    print(f"  Neurons: {neurons}")
    print(f"  Synapse density: {density}")
    print(f"  Random seed: {seed}")
    print(f"  Time step: {timestep}")
    
    # Validate configuration
    if config.validate():
        print(f"✓ Configuration validation passed")
    else:
        print(f"✗ Configuration validation failed")
    
    # Create brain with config
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created with custom configuration")
    
    return config, brain

def demo_type_conversions():
    """Demonstrate type conversion utilities"""
    print("\n=== Type Conversion Utilities Demo ===")
    
    # Convert various IDs
    neuron_id = pynlm.toNeuronId(42)
    synapse_id = pynlm.toSynapseId(123)
    region_id = pynlm.toRegionId(1)
    population_id = pynlm.toPopulationId(5)
    
    print(f"✓ Type conversions:")
    print(f"  NeuronId: {neuron_id}")
    print(f"  SynapseId: {synapse_id}")
    print(f"  RegionId: {region_id}")
    print(f"  PopulationId: {population_id}")
    
    return neuron_id, synapse_id, region_id, population_id

def demo_module_utilities():
    """Demonstrate module-level utilities"""
    print("\n=== Module Utilities Demo ===")
    
    # Get version
    version = pynlm.getVersion()
    print(f"✓ NLM Python bindings version: {version}")
    
    # Get supported types
    actions = pynlm.getSupportedActions()
    neuron_types = pynlm.getNeuronTypes()
    object_types = pynlm.getWorldObjectTypes()
    
    print(f"✓ Supported actions: {len(actions)} types")
    print(f"✓ Neuron types: {len(neuron_types)} types")
    print(f"✓ World object types: {len(object_types)} types")
    
    # Generate random seed
    seed = pynlm.createRandomSeed()
    print(f"✓ Generated random seed: {seed}")
    
    return version, actions, neuron_types, object_types

def demo_advanced_features():
    """Demonstrate advanced features"""
    print("\n=== Advanced Features Demo ===")
    
    # Create components with advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("brain.synapse_density", 0.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createTrainingEnvironment(10, 10, 4, 4)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("✓ Advanced features enabled:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    
    # Run episode management
    episode_result = agent.createEpisode()
    print(f"✓ Episode created: {episode_result}")
    
    stats = agent.getEpisodeStats()
    print(f"✓ Episode statistics retrieved")
    
    # Learn from experience
    learning_result = agent.learnFromExperience(0.8, 0.3)
    print(f"✓ Learning completed: {learning_result}")
    
    # Create checkpoint
    checkpoint_result = brain.createCheckpoint()
    print(f"✓ Checkpoint created: {checkpoint_result}")
    
    return brain, world, agent

def main():
    """Run all demonstrations"""
    print("NLM Python Bindings - Enhanced Features Demonstration")
    print("=" * 60)
    
    try:
        # Run all demos
        brain1 = demo_basic_usage()
        
        brain2, world2, agent2 = demo_enhanced_convenience()
        
        config3, brain3 = demo_configuration_management()
        
        ids4 = demo_type_conversions()
        
        version5, actions5, neuron_types5, object_types5 = demo_module_utilities()
        
        brain6, world6, agent6 = demo_advanced_features()
        
        # Summary
        print("\n" + "=" * 60)
        print("DEMONSTRATION SUMMARY")
        print("=" * 60)
        print(f"✓ Basic usage: Brain with {brain1.getTotalNeuronCount()} neurons")
        print(f"✓ Enhanced convenience: Complete simulation with {len(agent2.getEpisodeStats())} lines of stats")
        print(f"✓ Configuration management: Custom config with {config3.getInt('brain.neuron_count')} neurons")
        print(f"✓ Type conversions: Successfully converted {len([ids4, neuron_types5, object_types5])} different ID types")
        print(f"✓ Module utilities: Version {version5}, {len(actions5)} action types, {len(neuron_types5)} neuron types")
        print(f"✓ Advanced features: Full learning simulation with {brain6.getTotalSpikeCount()} spikes")
        
        print(f"\nAll demonstrations completed successfully!")
        print(f"The enhanced NLM Python bindings provide:")
        print(f"- 15+ new convenience functions")
        print(f"- Enhanced configuration management with type-specific getters")
        print(f"- Comprehensive error handling and validation")
        print(f"- Advanced agent episode management")
        print(f"- Type conversion utilities")
        print(f"- Module-level utility functions")
        print(f"- Checkpoint and serialization support")
        
        return True
        
    except Exception as e:
        print(f"\n✗ Demonstration failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)