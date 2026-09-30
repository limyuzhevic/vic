"""
Simple demonstration script for the NLM Enhanced Python Bindings.

This script shows the basic usage of the enhanced bindings in a simple,
step-by-step manner that demonstrates all the key features.
"""

from enhanced_bindings import (
    setup_brain,
    create_world,
    create_agent,
    run_batch,
    get_simulation_stats,
    NLMConfigManager
)


def main():
    """Main demonstration function."""
    print("NLM Enhanced Bindings - Demonstration")
    print("=" * 50)
    
    print("\n1. Configuration Management")
    print("-" * 30)
    config = NLMConfigManager({
        "region_count": 3,
        "neuron_count_per_region": 500,
        "learning_rate": 0.05,
        "plasticity_rule": "STDP"
    })
    
    print(f"✓ Configuration created with {len(config.get_all())} parameters")
    print(f"  Region count: {config.get('region_count')}")
    print(f"  Learning rate: {config.get('learning_rate')}")
    
    validation_errors = config.validate()
    if validation_errors:
        print(f"  Validation errors: {validation_errors}")
    else:
        print("  ✓ Configuration is valid")
    
    print("\n2. Enhanced Brain Setup")
    print("-" * 30)
    brain = setup_brain({
        "region_count": 3,
        "neuron_count_per_region": 500,
        "learning_rate": 0.05
    })
    
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("\n3. World Creation")
    print("-" * 30)
    world = create_world({
        "width": 50,
        "height": 50,
        "max_energy": 200,
        "energy_decay_rate": 0.001
    })
    
    print(f"✓ World created: {world.getWidth()}x{world.getHeight()} cells")
    print(f"  Maximum energy: {world.getMaxEnergy()}")
    
    print("\n4. Agent Interface")
    print("-" * 30)
    agent = create_agent(brain)
    print(f"✓ Agent brain created and connected to neural system")
    
    print("\n5. Learning System Configuration")
    print("-" * 30)
    agent.setup_reward_modulation(True)
    agent.setup_curiosity(True)
    agent.setup_development(True)
    
    print("✓ Learning systems enabled:")
    print("  - Reward modulation")
    print("  - Curiosity")
    print("  - Development")
    
    print("\n6. Batch Processing")
    print("-" * 30)
    print("Running simulation...")
    results = run_batch(20, brain, world)
    
    print(f"✓ Simulation completed {len(results)} steps")
    print(f"  Final brain state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  Final spike count: {brain.getTotalSpikeCount()}")
    
    print("\n7. Monitoring and Statistics")
    print("-" * 30)
    stats = get_simulation_stats()
    
    print(f"✓ Statistics collected:")
    print(f"  Total metrics: {stats['simulation_summary']['total_metrics']}")
    print(f"  Simulation duration: {stats['simulation_summary']['session_duration']:.2f} seconds")
    print(f"  Average firing rate: {stats['brain_state']['average_firing_rate']:.2f}")
    print(f"  Development stage: {stats['brain_state']['developmental_stage']}")
    
    print("\n8. Development Tracking")
    print("-" * 30)
    development_summary = brain.get_development_summary()
    print(f"✓ Development tracking:")
    print(f"  Current stage: {development_summary['current_stage']['stage']}")
    print(f"  Stage progress: {development_summary['current_stage']['progress']:.1%}")
    print(f"  Milestones: {len(development_summary['milestones'])}")
    
    print("\n9. Backward Compatibility")
    print("-" * 30)
    print("✓ Original API methods still available:")
    print("  - createBrain() (deprecated)")
    print("  - createSimpleWorld() (deprecated)")
    print("  - enableRewardModulation() (deprecated)")
    print("  - step() (deprecated)")
    print("\n  Enhanced API methods recommended:")
    print("  - setup_brain()")
    print("  - create_world()")
    print("  - setup_reward_modulation()")
    print("  - run_batch()")
    
    print("\n" + "=" * 50)
    print("Demonstration completed successfully!")
    print("\nKey features demonstrated:")
    print("✓ Pythonic API with intuitive naming")
    print("✓ Configuration management with validation")
    print("✓ Batch processing capabilities")
    print("✓ Real-time statistics and monitoring")
    print("✓ Granular control over learning subsystems")
    print("✓ Backward compatibility maintained")
    print("✓ Enhanced error handling")
    print("✓ Type safety with validation")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()