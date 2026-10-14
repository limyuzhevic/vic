#!/usr/bin/env python3

"""
Example demonstrating advanced NLM Python bindings usage.
This file contains comprehensive examples for users new to the enhanced bindings.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

def example_1_basic_brain():
    """
    Example 1: Basic Brain Creation and Simulation
    ================================================
    
    This example shows the fundamental operations of creating and running
    a neural brain simulation using the enhanced Python bindings.
    """
    print("=== Example 1: Basic Brain ===")
    
    # Create a brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500, pynlm.ConfigSource.Runtime)
    config.set("region_count", 3, pynlm.ConfigSource.Runtime)
    config.set("connection_probability", 0.15, pynlm.ConfigSource.Runtime)
    
    # Create the brain
    brain = pynlm.createBrain(config)
    
    # Initialize the brain
    print("Initializing brain...")
    brain.initialize()
    
    # Display initial state
    stats = brain.getStatistics()
    print(f"Brain initialized with {stats['total_neurons']} neurons")
    print(f"Number of regions: {stats['total_neurons'] // 166} (approximate)")
    
    # Run a simple simulation
    print("Running simulation for 50 steps...")
    brain.simulate(50, verbose=False)
    
    # Display final statistics
    final_stats = brain.getStatistics()
    print(f"\nSimulation Results:")
    print(f"  - Total spikes: {final_stats['total_spikes']}")
    print(f"  - Average firing rate: {final_stats['average_firing_rate']:.3f} Hz")
    print(f"  - Active neurons: {final_stats['active_neurons']} / {final_stats['total_neurons']}")
    print(f"  - Firing neurons: {final_stats['firing_neurons']} / {final_stats['total_neurons']}")
    
    # Save the brain for later use
    brain.save("basic_brain.json")
    print(f"\nBrain saved to 'basic_brain.json'")
    
    return brain

def example_2_advanced_configuration():
    """
    Example 2: Advanced Configuration Management
    ==============================================
    
    This example demonstrates advanced configuration techniques using
    the enhanced Python bindings.
    """
    print("\n=== Example 2: Advanced Configuration ===")
    
    # Method 1: Create configuration from dictionary
    print("1. Creating configuration from dictionary...")
    config_params = {
        "neuron_count": 1000,
        "region_count": 5,
        "connection_probability": 0.2,
        "simulation.timestep": 0.001,
        "neuron.LIF.v_threshold": -50.0,
        "neuron.LIF.v_reset": -65.0,
        "plasticity.STDP.learning_rate": 0.01,
        "neuromodulation.reward.enabled": True,
        "neuromodulation.curiosity.enabled": True,
        "memory.episodic.capacity": 100,
        "development.plasticity_rate": 0.8,
    }
    
    config = pynlm.createAdvancedConfig(config_params)
    print(f"   Created config with {len(config_params)} parameters")
    
    # Method 2: Update existing configuration
    print("2. Updating configuration...")
    config.updateFromDict({
        "neuron_count": 1500,
        "region_count": 8,
        "enable_prediction": True,
    })
    
    # Method 3: Convert to Python dictionary for inspection
    print("3. Converting to Python dictionary...")
    config_dict = config.to_dict()
    print(f"   Configuration keys: {list(config_dict.keys())}")
    
    # Display specific values
    print("   Configuration values:")
    for key in ["neuron_count", "region_count", "connection_probability"]:
        if key in config_dict:
            print(f"     {key}: {config_dict[key]}")
    
    # Save configuration
    config.saveToFile("advanced_config.json")
    print("   Configuration saved to 'advanced_config.json'")
    
    return config

def example_3_agent_environment():
    """
    Example 3: Agent-Environment Interaction
    ==========================================
    
    This example demonstrates how to create an agent that interacts
    with an environment using the NLM framework.
    """
    print("\n=== Example 3: Agent-Environment Interaction ===")
    
    # Create environment
    print("1. Creating environment...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=200,
        height=200,
        visionWidth=21,
        visionHeight=21
    )
    world.setAgentStart(100, 100)
    print("   Environment configured (200x200 world, 21x21 vision)")
    
    # Create brain and agent
    print("2. Creating brain and agent...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Initialize agent with world
    agent_brain.initialize(world)
    
    # Display agent capabilities
    print(f"   Agent sensory input size: {agent_brain.getSensoryInputSize()}")
    print(f"   Agent motor output size: {agent_brain.getMotorOutputSize()}")
    print(f"   Initial developmental stage: {agent_brain.getDevelopmentalStage().name}")
    
    # Create simulation
    print("3. Running agent-environment simulation...")
    
    # Get initial percept
    initial_percept = world.getSensoryPercept()
    
    # Run a few simulation steps
    for step in range(10):
        # Agent decision making
        action = agent_brain.act(initial_percept)
        
        if action:
            print(f"   Step {step}: Action produced - {action.getName()}")
            
            # Apply action in environment
            result = world.applyMotorCommand(action.getType(), world.getSimulationTime())
            
            # Get reward feedback
            if result.success:
                print(f"     Reward: {result.reward}")
                
                # Train with reward modulation
                agent_brain.applyRewardModulation(result.reward, 0.0)
        else:
            print(f"   Step {step}: No action produced")
        
        # Update environment
        world.update(1)
        
        # Update percept
        initial_percept = world.getSensoryPercept()
    
    # Display final agent state
    print(f"\nFinal agent state:")
    print(f"  - Developmental stage: {agent_brain.getDevelopmentalStage().name}")
    print(f"  - Neuromodulation level: {agent_brain.getNeuromodulationLevel():.3f}")
    print(f"  - Curiosity level: {agent_brain.getCuriosityLevel():.3f}")
    print(f"  - Novelty level: {agent_brain.getNoveltyLevel():.3f}")
    print(f"  - Prediction error: {agent_brain.getPredictionError():.3f}")
    
    return agent_brain, world

def example_4_memory_systems():
    """
    Example 4: Memory Systems Integration
    ======================================
    
    This example demonstrates how to use the various memory systems
    integrated into the NLM framework.
    """
    print("\n=== Example 4: Memory Systems ===")
    
    # Create brain with memory systems enabled
    print("1. Creating brain with memory systems...")
    config = pynlm.createDefaultConfig()
    
    # Enable all memory systems
    config.set("memory.working.enabled", True)
    config.set("memory.episodic.enabled", True)
    config.set("memory.associative.enabled", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access memory systems
    print("2. Accessing memory systems...")
    
    # Working memory
    if hasattr(brain, 'getWorkingMemory'):
        working_memory = brain.getWorkingMemory()
        print(f"   Working memory capacity: {working_memory.getCapacity()}")
    
    # Episodic memory
    if hasattr(brain, 'getEpisodicMemory'):
        episodic_memory = brain.getEpisodicMemory()
        print(f"   Episodic memory capacity: {episodic_memory.getCapacity()}")
        
        # Store an experience
        from datetime import datetime
        timestamp = datetime.now().timestamp()
        
        # This would require creating actual percept/action objects
        # For demonstration, we'll just show the API
        print("   Episodic memory ready for experience storage")
    
    # Associative memory
    if hasattr(brain, 'getAssociativeMemory'):
        associative_memory = brain.getAssociativeMemory()
        print(f"   Associative memory enabled: {associative_memory.isEnabled()}")
    
    # Prediction system
    if hasattr(brain, 'getPredictionSystem'):
        prediction_system = brain.getPredictionSystem()
        print(f"   Prediction system enabled: {prediction_system.isEnabled()}")
        
        current_error = prediction_system.getPredictionError()
        print(f"   Current prediction error: {current_error:.3f}")
    
    print("\n   Memory systems are integrated into the brain loop")
    print("   - Working memory maintains current neural activity")
    print("   - Episodic memory stores experiences over time")
    print("   - Associative memory forms pattern associations")
    print("   - Prediction system anticipates future states")
    
    return brain

def example_5_development_simulation():
    """
    Example 5: Developmental Stage Simulation
    ============================================
    
    This example demonstrates how developmental stages affect
    neural learning and behavior over time.
    """
    print("\n=== Example 5: Developmental Simulation ===")
    
    # Create brain with development enabled
    print("1. Creating brain with development...")
    config = pynlm.createDefaultConfig()
    config.set("development.enabled", True)
    config.set("development.plasticity_rate", 1.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Define developmental stages
    developmental_stages = [
        ("Initial", pynlm.DevelopmentalStage.Initial),
        ("CriticalPeriod", pynlm.DevelopmentalStage.CriticalPeriod),
        ("Maturation", pynlm.DevelopmentalStage.Maturation),
        ("Adult", pynlm.DevelopmentalStage.Adult),
    ]
    
    print("2. Simulating developmental progression...")
    
    for stage_name, stage_enum in developmental_stages:
        print(f"\n   Entering {stage_name} stage...")
        brain.setDevelopmentalStage(stage_enum)
        
        # Run simulation for this stage
        for step in range(20):
            brain.step(step)
            
            # Monitor neural activity
            active_neurons = brain.getActiveNeuronCount()
            firing_rate = brain.getAverageFiringRate()
            
            if step % 5 == 0:  # Print every 5 steps
                print(f"     Step {step}: {active_neurons} active neurons, "
                      f"{firing_rate:.3f} Hz average firing rate")
        
        # Display stage characteristics
        current_stage = brain.getDevelopmentalStage()
        print(f"     Stage {current_stage.name} completed")
    
    print("\n   Developmental characteristics:")
    print("   - Initial: High plasticity, basic learning")
    print("   - CriticalPeriod: Enhanced sensory learning")
    print("   - Maturation: Fine-tuning and specialization")
    print("   - Adult: Stable performance, slower learning")
    
    return brain

def example_6_visualization_and_analysis():
    """
    Example 6: Visualization and Neural Analysis
    ==============================================
    
    This example demonstrates how to visualize and analyze neural
    activity data using the enhanced Python bindings.
    """
    print("\n=== Example 6: Visualization and Analysis ===")
    
    # Create brain and run simulation
    print("1. Creating brain and running simulation...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run extended simulation
    print("2. Running extended simulation (100 steps)...")
    brain.simulate(100)
    
    # Get comprehensive statistics
    print("3. Analyzing neural activity...")
    stats = brain.getStatistics()
    
    print("   Simulation Summary:")
    print(f"   - Total neurons: {stats['total_neurons']}")
    print(f"   - Total synapses: {stats['total_synapses']}")
    print(f"   - Total spikes: {stats['total_spikes']}")
    print(f"   - Average firing rate: {stats['average_firing_rate']:.3f} Hz")
    print(f"   - Active neurons: {stats['active_neurons']} / {stats['total_neurons']}")
    print(f"   - Firing neurons: {stats['firing_neurons']} / {stats['total_neurons']}")
    print(f"   - Excitatory/inhibitory ratio: {stats['excitation_inhibition_ratio']:.3f}")
    
    # Calculate additional metrics
    print("\n   Additional Metrics:")
    active_percentage = (stats['active_neurons'] / stats['total_neurons']) * 100
    firing_percentage = (stats['firing_neurons'] / stats['total_neurons']) * 100
    print(f"   - Activity percentage: {active_percentage:.1f}%")
    print(f"   - Firing percentage: {firing_percentage:.1f}%")
    
    # Save analysis results
    analysis_data = {
        "simulation_parameters": {
            "neuron_count": stats['total_neurons'],
            "steps": 100,
        },
        "results": stats,
        "derived_metrics": {
            "active_percentage": active_percentage,
            "firing_percentage": firing_percentage,
            "efficiency_score": (stats['total_spikes'] / (stats['total_neurons'] * 100)),
        }
    }
    
    with open("neural_analysis.json", "w") as f:
        import json
        json.dump(analysis_data, f, indent=2)
    
    print(f"   Analysis saved to 'neural_analysis.json'")
    
    # Create checkpoint
    print("4. Creating checkpoint...")
    checkpoint_file = brain.createCheckpoint("Example 6 analysis run")
    print(f"   Checkpoint created: {checkpoint_file}")
    
    return brain, analysis_data

def example_7_batch_processing():
    """
    Example 7: Batch Processing for Multiple Simulations
    ================================================
    
    This example demonstrates how to run multiple simulations
    efficiently using batch processing techniques.
    """
    print("\n=== Example 7: Batch Processing ===")
    
    # Define parameter variations
    print("1. Setting up parameter variations...")
    
    base_config = pynlm.createDefaultConfig()
    base_config.set("neuron_count", 500, pynlm.ConfigSource.Runtime)
    
    parameter_sets = [
        {"neuron_count": 100, "connection_probability": 0.1},
        {"neuron_count": 500, "connection_probability": 0.2},
        {"neuron_count": 1000, "connection_probability": 0.15},
        {"neuron_count": 2000, "connection_probability": 0.1},
        {"neuron_count": 1500, "connection_probability": 0.25},
    ]
    
    print("   Parameter sets to test:")
    for i, params in enumerate(parameter_sets):
        print(f"     Set {i+1}: neurons={params['neuron_count']}, "
              f"connectivity={params['connection_probability']}")
    
    # Run batch simulation
    print("2. Running batch simulation...")
    
    batch_results = []
    
    for i, params in enumerate(parameter_sets):
        print(f"   Running simulation {i+1}/{len(parameter_sets)}...")
        
        # Create config for this parameter set
        config = base_config.clone()
        config.updateFromDict(params)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        brain.simulate(50)
        
        # Collect results
        stats = brain.getStatistics()
        stats['parameters'] = params
        stats['simulation_number'] = i + 1
        
        batch_results.append(stats)
        
        # Save individual result
        brain.save(f"batch_result_{i+1}.json")
        
        print(f"     Completed: {stats['total_spikes']} spikes, "
              f"{stats['average_firing_rate']:.3f} Hz average firing rate")
    
    # Analyze batch results
    print("3. Analyzing batch results...")
    
    print("   Batch Summary:")
    for result in batch_results:
        params = result['parameters']
        print(f"     Neurons: {params['neuron_count']:4d}, "
              f"Connectivity: {params['connection_probability']:.2f}, "
              f"Firing Rate: {result['average_firing_rate']:.3f} Hz, "
              f"Total Spikes: {result['total_spikes']}")
    
    # Calculate correlations
    print("\n   Correlations:")
    
    neuron_counts = [r['parameters']['neuron_count'] for r in batch_results]
    firing_rates = [r['average_firing_rate'] for r in batch_results]
    spike_counts = [r['total_spikes'] for r in batch_results]
    
    # Simple correlation analysis
    avg_neurons = sum(neuron_counts) / len(neuron_counts)
    avg_firing = sum(firing_rates) / len(firing_rates)
    avg_spikes = sum(spike_counts) / len(spike_counts)
    
    print(f"   Average neurons: {avg_neurons:.0f}")
    print(f"   Average firing rate: {avg_firing:.3f} Hz")
    print(f"   Average spike count: {avg_spikes:.0f}")
    
    # Find optimal parameters
    best_result = max(batch_results, key=lambda r: r['total_spikes'] / r['parameters']['neuron_count'])
    print(f"\n   Best efficiency result:")
    print(f"     Parameters: {best_result['parameters']}")
    print(f"     Efficiency (spikes/neuron): {best_result['total_spikes'] / best_result['parameters']['neuron_count']:.3f}")
    
    return batch_results

def example_8_checkpoint_management():
    """
    Example 8: Checkpoint Management and Recovery
    ==============================================
    
    This example demonstrates how to use checkpoints for saving
    and recovering brain states during long-running simulations.
    """
    print("\n=== Example 8: Checkpoint Management ===")
    
    # Create and run initial simulation
    print("1. Creating and running initial simulation...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300, pynlm.ConfigSource.Runtime)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run initial simulation
    brain.simulate(30)
    
    # Create initial checkpoint
    print("2. Creating initial checkpoint...")
    initial_checkpoint = brain.createCheckpoint("Start of simulation")
    print(f"   Initial checkpoint: {initial_checkpoint}")
    
    # Continue simulation
    print("3. Continuing simulation...")
    brain.simulate(40)
    
    # Create intermediate checkpoint
    intermediate_checkpoint = brain.createCheckpoint("Mid-simulation checkpoint")
    print(f"   Intermediate checkpoint: {intermediate_checkpoint}")
    
    # Continue further
    print("4. Continuing to final simulation...")
    brain.simulate(30)
    
    # Create final checkpoint
    final_checkpoint = brain.createCheckpoint("End of simulation")
    print(f"   Final checkpoint: {final_checkpoint}")
    
    # Save analysis data
    print("5. Saving analysis data...")
    
    simulation_log = {
        "checkpoints": [initial_checkpoint, intermediate_checkpoint, final_checkpoint],
        "total_steps": 100,
        "final_statistics": brain.getStatistics(),
        "simulation_date": "2024-01-01",  # Current date would go here
    }
    
    with open("simulation_log.json", "w") as f:
        import json
        json.dump(simulation_log, f, indent=2)
    
    print("   Simulation log saved to 'simulation_log.json'")
    
    # Demonstrate recovery
    print("6. Demonstrating recovery from checkpoint...")
    
    # Load from intermediate checkpoint
    recovered_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    recovered_brain.initialize()
    recovered_brain.load(intermediate_checkpoint)
    
    print(f"   Recovered brain has {recovered_brain.getTotalNeuronCount()} neurons")
    print(f"   Last developmental stage: {recovered_brain.getDevelopmentalStage().name}")
    
    # Continue simulation from recovered state
    print("7. Continuing simulation from recovered state...")
    recovered_brain.simulate(20)
    
    final_stats = recovered_brain.getStatistics()
    print(f"   Final statistics from recovered simulation:")
    print(f"     - Total spikes: {final_stats['total_spikes']}")
    print(f"     - Average firing rate: {final_stats['average_firing_rate']:.3f} Hz")
    
    return brain, simulation_log

def main():
    """
    Main function to run all examples sequentially.
    """
    print("NLM Python Bindings - Advanced Examples")
    print("=" * 50)
    
    examples = [
        example_1_basic_brain,
        example_2_advanced_configuration,
        example_3_agent_environment,
        example_4_memory_systems,
        example_5_development_simulation,
        example_6_visualization_and_analysis,
        example_7_batch_processing,
        example_8_checkpoint_management,
    ]
    
    results = []
    
    for i, example in enumerate(examples):
        try:
            result = example()
            results.append((example.__name__, "Success", result))
            print(f"\n✓ {example.__name__} completed successfully")
        except Exception as e:
            results.append((example.__name__, "Failed", str(e)))
            print(f"\n✗ {example.__name__} failed: {e}")
    
    # Summary
    print("\n" + "=" * 50)
    print("EXAMPLE EXECUTION SUMMARY")
    print("=" * 50)
    
    for name, status, result in results:
        print(f"{name}: {status}")
    
    successful = sum(1 for _, status, _ in results if status == "Success")
    total = len(results)
    
    print(f"\nSuccessfully completed: {successful}/{total} examples")
    
    if successful == total:
        print("\n🎉 All examples completed successfully!")
    else:
        print(f"\n⚠️  {total - successful} example(s) failed to complete")
    
    return results

if __name__ == "__main__":
    # Run all examples
    example_results = main()
    
    # Exit with appropriate code
    successful_examples = sum(1 for _, status, _ in example_results if status == "Success")
    total_examples = len(example_results)
    
    if successful_examples == total_examples:
        exit(0)
    else:
        exit(1)