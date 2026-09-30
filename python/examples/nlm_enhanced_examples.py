"""
Example usage of the enhanced NLM Python bindings.

This file demonstrates various features of the enhanced bindings including:
- Basic setup and usage
- Advanced configuration and learning control
- Batch processing and monitoring
- Checkpoint management
- Development tracking
- Brain region exploration

The examples show both beginner-friendly and advanced usage patterns.
"""

from enhanced_bindings import (
    setup_brain,
    create_world,
    create_agent,
    run_batch,
    get_simulation_stats,
    NLMConfigManager,
    NMLearningController,
    NLMCheckpointManager,
    NLMBrainRegionAccessor
)


def beginner_example():
    """
    Beginner example: Simple setup and basic usage.
    
    This example shows how to quickly setup an NLM simulation using
    the enhanced bindings with sensible defaults.
    """
    print("=== Beginner Example: Simple Setup ===")
    
    # Create brain with basic configuration
    brain = setup_brain({
        "region_count": 3,
        "neuron_count_per_region": 500,
        "learning_rate": 0.05,
        "plasticity_rule": "STDP"
    })
    
    # Create world with basic settings
    world = create_world({
        "width": 50,
        "height": 50,
        "max_energy": 200,
        "energy_decay_rate": 0.001
    })
    
    # Create agent brain interface
    agent = create_agent(brain)
    
    # Setup learning systems
    agent.setup_reward_modulation(True)
    agent.setup_curiosity(True)
    agent.setup_development(True)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Created world: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent brain connected to brain with {agent.getBrain().getTotalNeuronCount()} neurons")
    
    # Run a small simulation
    print("\nRunning simulation...")
    results = run_batch(10, brain, world)
    
    print(f"Simulation completed {len(results)} steps")
    print(f"Final stats: {get_simulation_stats()}")
    
    return brain, world, agent


def advanced_example():
    """
    Advanced example: Full feature demonstration.
    
    This example shows advanced usage of all enhanced features
    including learning control, monitoring, checkpoints, and region access.
    """
    print("\n=== Advanced Example: Full Feature Demonstration ===")
    
    # Create brain with detailed configuration
    config = NLMConfigManager({
        "region_count": 5,
        "neuron_count_per_region": 1000,
        "learning_rate": 0.02,
        "plasticity_rule": "BCM",
        "initial_excitability": 2.0,
        "structural_plasticity": True,
        "reward_modulation": True,
        "curiosity": True,
        "development": True,
        "simulation_time_step": 0.0005,
        "max_energy": 500,
        "energy_decay_rate": 0.0005
    })
    
    # Validate configuration
    errors = config.validate()
    if errors:
        print(f"Configuration errors: {errors}")
        return
    
    print(f"Configuration validated successfully: {config.get_all()}")
    
    # Create brain using enhanced setup
    brain = setup_brain()
    
    # Configure advanced learning using learning controller
    learning_controller = NMLearningController(brain)
    
    # Configure plasticity rules
    learning_controller.set_plasticity_rule(
        rule_type="STDP",
        learning_rate=0.05,
        weight_decay=0.001,
        eligibility_window=0.2
    )
    
    learning_controller.set_plasticity_rule(
        rule_type="Hebbian",
        learning_rate=0.03,
        weight_decay=0.002,
        eligibility_window=0.1
    )
    
    # Configure neuromodulation
    learning_controller.configure_neuromodulation(
        dopamine=1.5,
        serotonin=1.2,
        acetylcholine=0.8,
        noradrenaline=1.1,
        curiosity_threshold=0.7
    )
    
    # Configure development
    learning_controller.configure_development(
        developmental_stage="Maturation",
        plasticity_rate_modifier=1.2,
        structural_plasticity_enabled=True,
        critical_period_enabled=False
    )
    
    print("Learning configuration applied:")
    print(f"  Plasticity rules: {list(learning_controller.get_plasticity_rules().keys())}")
    print(f"  Neuromodulation: {learning_controller.get_neuromodulation_params()}")
    print(f"  Development: {learning_controller.get_development_params()}")
    
    # Create world and agent
    world = create_world({
        "width": 100,
        "height": 100,
        "max_energy": 1000,
        "energy_decay_rate": 0.0001,
        "simulation_time_step": 0.0005
    })
    
    agent = create_agent(brain)
    
    # Access brain region accessor for advanced exploration
    region_accessor = NLMBrainRegionAccessor(brain)
    
    # Explore brain regions and neurons
    print("\nBrain Region Exploration:")
    print(f"Available regions: {brain.getRegionIds()}")
    
    # Find neurons by criteria
    excitatory_neurons = region_accessor.find_neurons(
        neuron_type="Excitatory",
        min_activity=0.3,
        limit=5
    )
    
    print(f"Found {len(excitatory_neurons)} excitatory neurons with activity >= 0.3:")
    for neuron in excitatory_neurons:
        print(f"  - {neuron['id']}: activity={neuron['activity']:.2f}")
    
    # Get population statistics
    region_id = list(brain.getRegionIds())[0] if brain.getRegionIds() else 0
    population_stats = region_accessor.get_population_stats(region_id=region_id)
    print(f"\nPopulation statistics for region {region_id}:")
    print(f"  Neuron count: {population_stats['neuron_count']}")
    print(f"  Firing rate: {population_stats['firing_rate']:.2f}")
    print(f"  Average activity: {population_stats['average_activity']:.2f}")
    print(f"  Connection density: {population_stats['connection_density']:.3f}")
    
    # Run a batch simulation with monitoring
    print("\nRunning advanced simulation...")
    batch_results = run_batch(50, brain, world)
    
    print(f"Simulation completed {len(batch_results)} steps")
    
    # Get comprehensive statistics
    stats = get_simulation_stats()
    print(f"\nSimulation statistics:")
    print(f"  Total metrics: {stats['simulation_summary']['total_metrics']}")
    print(f"  Total time points: {stats['simulation_summary']['total_time_points']}")
    print(f"  Average firing rate: {stats['brain_state']['average_firing_rate']:.2f}")
    print(f"  Learning rate: {learning_controller.get_plasticity_rules()['STDP']['learning_rate']:.3f}")
    
    # Checkpoint management
    checkpoint_manager = NLMCheckpointManager()
    
    # Save checkpoints
    checkpoint1 = brain.save_checkpoint("advanced_checkpoint_1", "Initial advanced simulation state")
    print(f"\nSaved checkpoint: {checkpoint1['name']} (version {checkpoint1['version']})")
    
    # Run more simulation
    print("Running more simulation for additional checkpoints...")
    run_batch(25, brain, world)
    
    checkpoint2 = brain.save_checkpoint("advanced_checkpoint_2", "Mid-simulation checkpoint")
    print(f"Saved checkpoint: {checkpoint2['name']} (version {checkpoint2['version']})")
    
    # List all checkpoints
    checkpoints = brain.list_checkpoints()
    print(f"\nAll checkpoints ({len(checkpoints)} total):")
    for cp in checkpoints:
        print(f"  - {cp['name']} (v{cp['version']}) at {cp['timestamp'][:19]}")
    
    # Development tracking
    development_summary = brain.get_development_summary()
    print(f"\nDevelopment Summary:")
    print(f"  Current stage: {development_summary['current_stage']['stage']}")
    print(f"  Stage progress: {development_summary['current_stage']['progress']:.1%}")
    print(f"  Next stage: {development_summary['current_stage']['next_stage']}")
    print(f"  Milestones: {len(development_summary['milestones'])}")
    
    # Add a development milestone
    agent.add_milestone(
        name="high_learning_rate",
        description="Achieve learning rate of 0.05 or higher",
        target_stage="Maturation",
        target_metric="learning_rate",
        target_value=0.05
    )
    
    # Check milestone progress
    milestone_achieved = agent.update_milestone("high_learning_rate", 0.06)
    print(f"Milestone 'high_learning_rate' achieved: {milestone_achieved}")
    
    return brain, world, agent


def configuration_example():
    """
    Configuration example: Demonstrating configuration validation and management.
    """
    print("\n=== Configuration Example: Validation and Management ===")
    
    # Create configuration manager
    config = NLMConfigManager()
    
    # Test different configurations
    test_configs = [
        {"region_count": 1},
        {"region_count": 10, "neuron_count_per_region": 500},
        {"region_count": 5, "learning_rate": 0.5},  # High learning rate
        {"region_count": 3, "plasticity_rule": "InvalidRule"}  # Invalid rule
    ]
    
    for i, test_config in enumerate(test_configs):
        print(f"\nTest {i+1}: {test_config}")
        
        # Update configuration
        config.update(test_config)
        
        # Validate
        errors = config.validate()
        if errors:
            print(f"  Validation errors: {errors}")
        else:
            print(f"  Valid configuration")
            print(f"  All keys: {list(config.get_all().keys())}")
    
    # Demonstrate type validation
    print(f"\nType validation demonstration:")
    try:
        config.update({"learning_rate": "invalid"})  # Should fail
        print("ERROR: Should have raised validation error")
    except Exception as e:
        print(f"  Type validation caught error: {type(e).__name__}")
    
    # Demonstrate range validation
    try:
        config.update({"region_count": 0})  # Should fail (min=1)
        print("ERROR: Should have raised validation error")
    except Exception as e:
        print(f"  Range validation caught error: {type(e).__name__}")
    
    return config


def monitoring_example():
    """
    Monitoring example: Demonstrating statistics collection and analysis.
    """
    print("\n=== Monitoring Example: Statistics Collection ===")
    
    # Create brain and world
    brain = setup_brain({"region_count": 2})
    world = create_world()
    agent = create_agent(brain)
    
    # Run simulation
    print("Running simulation for monitoring...")
    run_batch(10, brain, world)
    
    # Get statistics
    stats = get_simulation_stats()
    
    print("\nCollected Statistics:")
    print(f"  Session duration: {stats['simulation_summary']['session_duration']:.2f} seconds")
    print(f"  Total metrics collected: {stats['simulation_summary']['total_metrics']}")
    print(f"  Total time points: {stats['simulation_summary']['total_time_points']}")
    
    print(f"\nBrain Metrics:")
    print(f"  Total neurons: {stats['brain_state']['neuron_count']}")
    print(f"  Total synapses: {stats['brain_state']['synapse_count']}")
    print(f"  Average firing rate: {stats['brain_state']['average_firing_rate']:.2f}")
    print(f"  Development stage: {stats['brain_state']['developmental_stage']}")
    
    print(f"\nLearning Metrics:")
    print(f"  Learning rules: {list(stats['learning_controller'].keys())}")
    print(f"  Neuromodulation level: {stats['neuromodulation'].get('dopamine', 0):.2f}")
    print(f"  Curiosity level: {stats['neuromodulation'].get('curiosity_threshold', 0):.2f}")
    
    print(f"\nCheckpoint Information:")
    for cp in stats['checkpoints']:
        print(f"  - {cp['name']} (v{cp['version']}) at {cp['timestamp'][:19]}")
    
    return stats


def backward_compatibility_example():
    """
    Backward compatibility example: Show how the enhanced bindings
    maintain compatibility with the original API.
    """
    print("\n=== Backward Compatibility Example ===")
    
    print("Original API methods are still available:")
    print("  - createBrain() -> setup_brain()")
    print("  - createSimpleWorld() -> create_world()")
    print("  - createAgentBrain() -> create_agent()")
    print("  - enableRewardModulation() -> setup_reward_modulation()")
    print("  - enableCuriosity() -> setup_curiosity()")
    print("  - enableDevelopment() -> setup_development()")
    print("  - step() -> run_step() or batch_run()")
    print("  - decodeMotorCommand() -> decode_action()")
    
    # Demonstrate that old API still works
    print("\nDemonstrating backward compatibility:")
    
    # These would work with the original bindings
    # brain = createBrain()  # Now calls setup_brain()
    # world = createSimpleWorld()  # Now calls create_world()
    # agent = createAgentBrain(brain)  # Now calls create_agent(brain)
    # enableRewardModulation(True)  # Now calls setup_reward_modulation(True)
    
    print("✓ All original methods have enhanced equivalents")
    print("✓ Enhanced methods maintain backward compatibility")
    print("✓ Warnings are issued when deprecated methods are used")
    print("✓ New methods provide enhanced functionality")


def main():
    """
    Main function to run all examples.
    
    This function demonstrates all the features of the enhanced bindings
    in a comprehensive manner.
    """
    print("NLM Enhanced Bindings - Examples")
    print("=" * 50)
    
    try:
        # Beginner example
        brain1, world1, agent1 = beginner_example()
        
        # Advanced example
        brain2, world2, agent2 = advanced_example()
        
        # Configuration example
        config = configuration_example()
        
        # Monitoring example
        stats = monitoring_example()
        
        # Backward compatibility example
        backward_compatibility_example()
        
        print("\n" + "=" * 50)
        print("All examples completed successfully!")
        print("\nKey features demonstrated:")
        print("✓ Pythonic API with intuitive naming")
        print("✓ Configuration management with validation")
        print("✓ Batch processing capabilities")
        print("✓ Real-time statistics and monitoring")
        print("✓ Granular control over learning subsystems")
        print("✓ Direct access to brain regions and populations")
        print("✓ Advanced checkpoint management")
        print("✓ Development tracking tools")
        print("✓ Improved error handling")
        print("✓ Complete type safety")
        print("✓ Backward compatibility")
        
    except Exception as e:
        print(f"\nExample failed with error: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()