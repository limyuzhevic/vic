#!/usr/bin/env python3
"""
Enhanced NLM Python Example - Demonstrating the new Pythonic interface

This file demonstrates the enhanced Python API features and provides
tutorials for getting started with the NLM neural simulation framework.
"""

import sys
import time
import pynlm as nlm

def example_basic_brain():
    """Example 1: Basic brain creation and simulation"""
    print("=== Example 1: Basic Brain ===")
    
    # Create configuration using enhanced dictionary interface
    config = nlm.Config()
    config.update({
        "neuron_count": 500,
        "region_count": 2,
        "random_seed": 42
    })
    
    # Use context manager for automatic lifecycle management
    with nlm.Brain(config) as brain:
        # Run simulation
        results = brain.run_simulation(steps=50)
        
        # Access results
        print(f"Simulation completed {len(results)} steps")
        
        # Get final statistics
        final_stats = results[str(len(results) - 1)]
        print(f"Final firing rate: {final_stats['average_firing_rate']:.2f} Hz")
        print(f"Total spikes: {final_stats['total_spikes']}")
        
    print("Example 1 completed\n")

def example_agent_world():
    """Example 2: Agent interacting with a simulated world"""
    print("=== Example 2: Agent-World Interaction ===")
    
    # Create all components
    brain = nlm.create_brain(nlm.create_config())
    agent = nlm.create_agent_brain(brain)
    world = nlm.create_simple_world()
    
    # Configure world
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    # Enable learning and exploration
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run simulation loop
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process with agent
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply rewards (example: moving is good)
        if action.type in [nlm.ActionType.MOVE_FORWARD, nlm.ActionType.MOVE_LEFT, 
                          nlm.ActionType.MOVE_RIGHT, nlm.ActionType.INTERACT]:
            agent.apply_reward_modulation(reward=0.5, predicted_reward=0.0)
        
        # Log progress
        if step % 20 == 0:
            status = agent.get_agent_status()
            print(f"Step {step}: {action.name}, "
                  f"Neuromodulation={status['neuromodulation_level']:.2f}, "
                  f"Curiosity={status['curiosity_level']:.2f}")
    
    print("Example 2 completed\n")

def example_advanced_configuration():
    """Example 3: Advanced configuration with factory pattern"""
    print("=== Example 3: Advanced Configuration ===")
    
    def create_experiment_config(experiment_type="balanced"):
        """Factory function for different experiment configurations"""
        configs = {
            "simple": {
                "neuron_count": 200,
                "region_count": 1,
                "connection_probability": 0.05,
                "development_enabled": False
            },
            "balanced": {
                "neuron_count": 500,
                "region_count": 2,
                "connection_probability": 0.1,
                "development_enabled": True,
                "reward_modulation_enabled": True
            },
            "complex": {
                "neuron_count": 1000,
                "region_count": 4,
                "connection_probability": 0.15,
                "development_enabled": True,
                "reward_modulation_enabled": True,
                "structural_plasticity_enabled": True,
                "curiosity_enabled": True
            }
        }
        
        config = nlm.Config()
        config.update(configs.get(experiment_type, configs["balanced"]))
        return config
    
    # Create different configurations
    for exp_type in ["simple", "balanced", "complex"]:
        print(f"\nRunning {exp_type} experiment...")
        config = create_experiment_config(exp_type)
        
        with nlm.Brain(config) as brain:
            results = brain.run_simulation(steps=30)
            avg_firing = sum(r['average_firing_rate'] for r in results.values()) / len(results)
            print(f"  Average firing rate: {avg_firing:.2f} Hz")
    
    print("Example 3 completed\n")

def example_neuromodulation():
    """Example 4: Neuromodulation and learning"""
    print("=== Example 4: Neuromodulation and Learning ===")
    
    # Create brain with neuromodulation enabled
    config = nlm.Config()
    config.update({
        "neuron_count": 300,
        "region_count": 2,
        "random_seed": 123
    })
    
    brain = nlm.create_brain(config)
    agent = nlm.create_agent_brain(brain)
    
    # Initialize world and agent
    world = nlm.create_simple_world()
    world.configure(width=15, height=15)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Track learning progress
    neuromodulation_history = []
    curiosity_history = []
    prediction_error_history = []
    
    # Run learning simulation
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward based on action
        reward = 0.0
        if action.type in [nlm.ActionType.MOVE_FORWARD, nlm.ActionType.INTERACT]:
            reward = 1.0
        elif action.type in [nlm.ActionType.MOVE_LEFT, nlm.ActionType.MOVE_RIGHT]:
            reward = 0.5
            
        agent.apply_reward_modulation(reward, predicted_reward=0.0)
        
        # Record neuromodulation state
        status = agent.get_agent_status()
        neuromodulation_history.append(status['neuromodulation_level'])
        curiosity_history.append(status['curiosity_level'])
        prediction_error_history.append(status['prediction_error'])
        
        # Log every 50 steps
        if step % 50 == 0:
            print(f"Step {step}: "
                  f"Dopamine={neuromodulation_history[-1]:.2f}, "
                  f"Curiosity={curiosity_history[-1]:.2f}, "
                  f"Error={prediction_error_history[-1]:.2f}")
    
    # Analyze learning
    final_neuromodulation = neuromodulation_history[-1]
    avg_curiosity = sum(curiosity_history) / len(curiosity_history)
    
    print(f"\nLearning Analysis:")
    print(f"  Final neuromodulation level: {final_neuromodulation:.2f}")
    print(f"  Average curiosity: {avg_curiosity:.2f}")
    print(f"  Peak curiosity: {max(curiosity_history):.2f}")
    print(f"  Neuromodulation stability: {len([x for x in neuromodulation_history if abs(x - final_neuromodulation) < 0.1])}/{len(neuromodulation_history)}")
    
    print("Example 4 completed\n")

def example_error_handling():
    """Example 5: Error handling and recovery"""
    print("=== Example 5: Error Handling and Recovery ===")
    
    def safe_simulation(config, max_steps=1000):
        """Wrapper with error handling and recovery"""
        brain = None
        agent = None
        world = None
        
        try:
            # Initialize components
            brain = nlm.create_brain(config)
            agent = nlm.create_agent_brain(brain)
            world = nlm.create_simple_world()
            
            # Enable features with error checking
            agent.enable_reward_modulation(True)
            
            # Main simulation loop
            for step in range(max_steps):
                # Check for error conditions
                if brain and brain.get_pending_spike_event_count() > 5000:
                    print(f"Warning: High pending spike events at step {step}")
                
                if brain and brain.get_total_spike_count() > 500000:
                    print(f"Warning: High spike count at step {step}")
                
                # Normal simulation step
                world.update(0.1)
                percept = world.get_sensory_percept()
                agent.process_sensory_input(percept)
                brain.step(step)
                
                # Occasionally simulate error and recovery
                if step % 300 == 299:
                    print(f"  Simulating error recovery at step {step}")
                    # This would simulate a reset/recovery
                    if brain:
                        brain.reset()
                    if world:
                        world.reset()
                    
        except Exception as e:
            print(f"Simulation error: {e}")
            # Attempt recovery
            try:
                if brain:
                    brain.reset()
                if world:
                    world.reset()
                print("Recovery successful")
                return True
            except:
                print("Recovery failed")
                return False
                
        return True
    
    # Test error handling
    config = nlm.Config()
    config.update({"neuron_count": 200, "region_count": 1})
    
    print("Running safe simulation with error handling...")
    success = safe_simulation(config)
    
    if success:
        print("Safe simulation completed successfully")
    else:
        print("Safe simulation failed")
    
    print("Example 5 completed\n")

def example_performance_monitoring():
    """Example 6: Performance monitoring and metrics"""
    print("=== Example 6: Performance Monitoring ===")
    
    # Create configuration
    config = nlm.Config()
    config.update({
        "neuron_count": 800,
        "region_count": 3,
        "random_seed": 999
    })
    
    # Create brain with performance monitoring
    with nlm.Brain(config) as brain:
        # Track performance metrics
        performance_data = {
            'steps': [],
            'firing_rates': [],
            'spike_counts': [],
            'memory_usage': [],
            'processing_times': []
        }
        
        print("Running performance monitoring simulation...")
        
        for step in range(100):
            # Measure processing time
            start_time = time.time()
            
            # Run simulation step
            brain.step(step)
            
            end_time = time.time()
            processing_time = end_time - start_time
            
            # Collect metrics
            performance_data['steps'].append(step)
            performance_data['firing_rates'].append(brain.get_average_firing_rate())
            performance_data['spike_counts'].append(brain.get_total_spike_count())
            performance_data['processing_times'].append(processing_time)
            
            # Log progress
            if step % 20 == 0:
                firing_rate = brain.get_average_firing_rate()
                print(f"  Step {step}: {firing_rate:.2f} Hz, "
                      f"Time={processing_time*1000:.1f}ms")
        
        # Performance analysis
        avg_firing_rate = sum(performance_data['firing_rates']) / len(performance_data['firing_rates'])
        avg_processing_time = sum(performance_data['processing_times']) / len(performance_data['processing_times'])
        total_spikes = performance_data['spike_counts'][-1]
        
        print(f"\nPerformance Analysis:")
        print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
        print(f"  Average processing time per step: {avg_processing_time*1000:.1f} ms")
        print(f"  Total spikes processed: {total_spikes}")
        print(f"  Steps per second: {1.0/avg_processing_time:.1f}")
        print(f"  Neurons per second: {total_spikes/avg_processing_time:.0f}")
    
    print("Example 6 completed\n")

def main():
    """Run all examples"""
    print("Enhanced NLM Python Examples")
    print("=" * 50)
    
    try:
        example_basic_brain()
        example_agent_world()
        example_advanced_configuration()
        example_neuromodulation()
        example_error_handling()
        example_performance_monitoring()
        
        print("All examples completed successfully!")
        print("\nEnhanced API features demonstrated:")
        print("  ✓ Dictionary-like configuration interface")
        print("  ✓ Context managers for automatic resource management")
        print("  ✓ Simulation runners with detailed results")
        print("  ✓ Neuromodulation and learning systems")
        print("  ✓ Error handling and recovery")
        print("  ✓ Performance monitoring and metrics")
        print("  ✓ Factory pattern for configuration management")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
