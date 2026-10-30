"""
NLM Visualization and Debugging Example
======================================

This example demonstrates visualization and debugging tools
using the enhanced NLM Python bindings.
"""

import pynlm as nlm
import numpy as np

def brain_visualization_example():
    """Example showing how to visualize brain state"""
    print("\n=== Brain Visualization Example ===\n")
    
    # Create and initialize brain
    config = nlm.create_test_config(neuron_count=200)
    brain = nlm.create_brain(config)
    brain.initialize()
    
    print("Brain Statistics:")
    print(brain.get_brain_status() if hasattr(brain, 'get_brain_status') else brain.logStatus())
    
    # Run simulation and collect statistics
    with nlm.BrainSimulator(brain) as sim:
        print("\nRunning simulation...")
        for step in range(10):
            sim.step(step)
        
        print(f"\nFinal brain statistics:")
        print(sim.get_stats())

def simulation_visualization_example():
    """Example showing simulation visualization"""
    print("\n=== Simulation Visualization Example ===\n")
    
    config = nlm.create_test_config(neuron_count=100)
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # Create enhanced world
    world = nlm.World()
    world.configure(30, 30)
    world.set_agent_start(5, 5)
    
    print("Initial world state:")
    print(world.get_stats())
    
    # Run simulation with world interaction
    with nlm.BrainSimulator(brain) as sim:
        for step in range(10):
            # Get current state
            body = world.get_agent_body()
            print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f})")
            
            # Get sensory input
            # Note: This would need proper brain integration
            # sensory = world.get_percept()
            
            # Update brain
            sim.step(step)
            
            # Apply action (simple forward movement)
            if step > 0 and body.x < 25:
                action = nlm.Action(nlm.ActionType.MoveForward)
                world.apply_action(action, step * 0.1)
    
    print(f"\nFinal world state:")
    print(world.get_stats())
    
    print(f"\nFinal brain statistics:")
    print(sim.get_stats())

def debug_brain_example():
    """Example showing debugging capabilities"""
    print("\n=== Brain Debugging Example ===\n")
    
    config = nlm.create_test_config(neuron_count=150)
    brain = nlm.create_brain(config)
    brain.initialize()
    
    print("Initial brain state:")
    brain.logStatus()
    
    # Monitor brain state over time
    print("\nMonitoring brain state...")
    with nlm.BrainSimulator(brain) as sim:
        for step in range(5):
            sim.step(step)
            print(f"\nStep {step}:")
            print(f"  Neurons: {brain.getTotalNeuronCount()}")
            print(f"  Synapses: {brain.getTotalSynapseCount()}")
            print(f"  Active: {brain.getActiveNeuronCount()}")
            print(f"  Firing: {brain.getFiringNeuronCount()}")
            print(f"  Average rate: {brain.getAverageFiringRate():.3f}")
            print(f"  Spike count: {brain.getTotalSpikeCount()}")
            print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("\nBrain debugging complete.")

def world_visualization_example():
    """Example showing world visualization"""
    print("\n=== World Visualization Example ===\n")
    
    # Create world
    world = nlm.World()
    world.configure(20, 20)
    world.set_agent_start(2, 2)
    
    print("World configuration:")
    print(f"  Dimensions: {world.get_width()}x{world.get_height()}")
    print(f"  Agent start: ({world.get_agent_start_x()}, {world.get_agent_start_y()})")
    print(f"  Max energy: {world.get_max_energy()}")
    print(f"  Energy decay rate: {world.get_energy_decay_rate()}")
    
    # Add some objects
    print("\nAdding objects to world...")
    obj1 = nlm.WorldObject(10, 10, nlm.WorldObjectType.Resource, value=100.0)
    obj2 = nlm.WorldObject(15, 15, nlm.WorldObjectType.Hazard, radius=1.5)
    world.add_object(obj1)
    world.add_object(obj2)
    
    print(f"  Objects added: {len([o for o in [obj1, obj2] if o.active])}")
    
    # Check positions
    print(f"\nPosition checks:")
    print(f"  (10, 10) is valid: {world.is_valid_position(10, 10)}")
    print(f"  (15, 15) is valid: {world.is_valid_position(15, 15)}")
    print(f"  (0, 0) is valid: {world.is_valid_position(0, 0)}")
    
    # Update world
    print("\nUpdating world...")
    for step in range(3):
        world.update(0.1)
        body = world.get_agent_body()
        print(f"  Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
              f"Energy: {body.energy:.1f}")
    
    print(f"\nFinal world state:")
    print(world.get_stats())

def action_debug_example():
    """Example showing action debugging"""
    print("\n=== Action Debugging Example ===\n")
    
    # Create different types of actions
    actions = [
        nlm.Action(nlm.ActionType.MoveForward),
        nlm.Action(nlm.ActionType.TurnLeft, [0.5]),
        nlm.Action(nlm.ActionType.LookUp, [45.0]),
        nlm.Action(nlm.ActionType.Custom, [0.1, 0.2, 0.3, 0.4]),
    ]
    
    print("Action examples:")
    for i, action in enumerate(actions):
        print(f"  Action {i+1}:")
        print(f"    Type: {action.get_type()}")
        print(f"    Name: {action.get_name()}")
        print(f"    Parameters: {action.get_parameters()}")
        print(f"    Repr: {repr(action)}")
        
        # Clone action
        cloned = action.clone()
        print(f"    Cloned successfully: {cloned.get_type() == action.get_type()}")
        print()

def advanced_config_example():
    """Example showing advanced configuration"""
    print("\n=== Advanced Configuration Example ===\n")
    
    # Create configuration using method chaining
    config = (nlm.Config()
              .set_int("neuron_count", 500)
              .set_int("region_count", 3)
              .set_float("connection_probability", 0.15)
              .set_float("stdp_ltp_weight", 0.02)
              .set_float("stdp_ltd_weight", 0.018)
              .set_float("synaptogenesis_rate", 0.0001)
              .set_float("pruning_rate", 0.00001))
    
    print("Advanced configuration:")
    print(f"  Neuron count: {config.get_int('neuron_count')}")
    print(f"  Region count: {config.get_int('region_count')}")
    print(f"  Connection probability: {config.get_float('connection_probability')}")
    print(f"  STDP LTP weight: {config.get_float('stdp_ltp_weight')}")
    print(f"  STDP LTD weight: {config.get_float('stdp_ltd_weight')}")
    print(f"  Synaptogenesis rate: {config.get_float('synaptogenesis_rate')}")
    print(f"  Pruning rate: {config.get_float('pruning_rate')}")
    
    # Show all configuration keys
    print(f"\nAll configuration keys: {config.get_keys()}")
    
    # Save configuration
    print("\nSaving configuration to file...")
    saved = config.save("./test_config.cfg")
    print(f"  Save successful: {saved}")
    
    # Load configuration
    print("\nLoading configuration from file...")
    config2 = nlm.Config("./test_config.cfg")
    print(f"  Loaded neuron count: {config2.get_int('neuron_count')}")
    
    # Clean up
    import os
    if os.path.exists("./test_config.cfg"):
        os.remove("./test_config.cfg")
        print("  Cleaned up test file")

def simulation_comparison_example():
    """Example comparing different simulation settings"""
    print("\n=== Simulation Comparison Example ===\n")
    
    # Create different configurations for comparison
    configs = {
        "Small (50 neurons)": nlm.create_test_config(neuron_count=50),
        "Medium (100 neurons)": nlm.create_test_config(neuron_count=100),
        "Large (200 neurons)": nlm.create_test_config(neuron_count=200),
    }
    
    results = {}
    
    for name, config in configs.items():
        print(f"\nRunning {name}...")
        brain = nlm.create_brain(config)
        brain.initialize()
        
        with nlm.BrainSimulator(brain) as sim:
            # Collect metrics
            start_time = time.time()
            firing_rates = sim.run_steps(50)
            end_time = time.time()
            
            results[name] = {
                'brain': brain,
                'simulator': sim,
                'firing_rates': firing_rates,
                'simulation_time': end_time - start_time,
                'avg_firing_rate': sum(firing_rates) / len(firing_rates),
                'total_spikes': brain.getTotalSpikeCount()
            }
            
            print(f"  Average firing rate: {results[name]['avg_firing_rate']:.3f}")
            print(f"  Total spikes: {results[name]['total_spikes']}")
            print(f"  Simulation time: {results[name]['simulation_time']:.3f}s")
    
    # Compare results
    print(f"\n=== Comparison Results ===")
    print(f"{'Config':<20} {'Neurons':<8} {'Avg Rate':<12} {'Total Spikes':<12} {'Sim Time (s)':<12}")
    print("-" * 70)
    
    for name, result in results.items():
        config = result['brain'].get_config()
        neuron_count = config.getOr<int64_t>("neuron_count", 0)
        print(f"{name:<20} {neuron_count:<8} {result['avg_firing_rate']:<12.3f} "
              f"{result['total_spikes']:<12} {result['simulation_time']:<12.3f}")
    
    # Show correlation between neuron count and firing rate
    print(f"\n=== Correlation Analysis ===")
    neuron_counts = [r['brain'].get_config().getOr<int64_t>("neuron_count", 0) for r in results.values()]
    avg_rates = [r['avg_firing_rate'] for r in results.values()]
    
    if len(neuron_counts) > 1 and len(avg_rates) > 1:
        correlation = np.corrcoef(neuron_counts, avg_rates)[0, 1]
        print(f"Correlation between neuron count and firing rate: {correlation:.3f}")
        
        if abs(correlation) > 0.7:
            print("  Strong correlation detected!")
        elif abs(correlation) > 0.3:
            print("  Moderate correlation detected.")
        else:
            print("  Weak correlation.")

def main():
    """Run all visualization and debugging examples"""
    print("NLM Enhanced Python Bindings - Visualization and Debugging")
    print("=" * 65)
    
    try:
        brain_visualization_example()
        simulation_visualization_example()
        debug_brain_example()
        world_visualization_example()
        action_debug_example()
        advanced_config_example()
        simulation_comparison_example()
        
        print("\n\n" + "=" * 65)
        print("All visualization and debugging examples completed successfully!")
        print("=" * 65)
        
    except Exception as e:
        print(f"\n❌ Error running visualization examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    import time
    main()
