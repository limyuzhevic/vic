#!/usr/bin/env python3
"""
Enhanced NLM Example Scripts
=============================

This file contains comprehensive example scripts demonstrating the enhanced
NLM Python API. These examples cover various use cases from basic to advanced.

For more detailed documentation, see EnhancedAPIDocumentation.md
"""

import pynlm

def example_basic_usage():
    """Example 1: Basic Enhanced API Usage"""
    print("=== Example 1: Basic Enhanced API Usage ===")
    
    # Create a brain using preset configuration
    brain = pynlm.create_brain_for_task("simple")
    print(f"Created brain with {brain.neuron_count} neurons")
    
    # Initialize with convenience method
    brain._initialize_default()
    print("✓ Brain initialized successfully")
    
    # Run simulation with automatic statistics
    stats = brain.run_simulation(500, verbose=True)
    print(f"Simulation completed:")
    print(f"  - Total steps: {len(stats[0])}")
    print(f"  - Total spikes: {stats[1]}")
    print(f"  - Average firing rate: {stats[2]:.2f} Hz")
    
    # Check brain health
    health = brain.check_health()
    if health.isHealthy:
        print("✓ Brain health check passed")
    else:
        print(f"⚠ Health issue: {health.statusMessage}")
    
    print()

def example_complete_agent():
    """Example 2: Complete Agent Setup"""
    print("=== Example 2: Complete Agent Setup ===")
    
    # Create a complete agent setup
    brain, world, agent = pynlm.setup_complete_agent("learning", "adaptive")
    
    print(f"Setup complete:")
    print(f"  - Brain neurons: {brain.neuron_count}")
    print(f"  - World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  - Agent sensory size: {agent.getSensoryInputSize()}")
    print(f"  - Agent motor size: {agent.getMotorOutputSize()}")
    
    # Run a complete simulation
    print("\nRunning agent simulation...")
    results = pynlm.run_agent_simulation(brain, world, agent, 300, verbose=False)
    
    print(f"Simulation results:")
    print(f"  - Average firing rate: {results[2]:.2f} Hz")
    print(f"  - Total spikes: {results[1]}")
    
    # Analyze agent behavior
    agent_status = agent.get_status_report()
    print(f"\nAgent status: {agent_status.statusMessage}")
    print(f"  - Curiosity level: {agent_status.curiosityLevel:.3f}")
    print(f"  - Novelty level: {agent_status.noveltyLevel:.3f}")
    print(f"  - Reward modulation: {'Enabled' if agent_status.rewardModEnabled else 'Disabled'}")
    
    print()

def example_behavior_comparison():
    """Example 3: Behavior Mode Comparison"""
    print("=== Example 3: Behavior Mode Comparison ===")
    
    task_type = "learning"
    behaviors = ["exploratory", "conservative", "adaptive"]
    
    results = {}
    
    for behavior in behaviors:
        print(f"\nTesting behavior mode: {behavior}")
        
        # Create setup for this behavior
        brain = pynlm.create_brain_for_task(task_type)
        brain._initialize_default()
        
        world = pynlm.create_simple_world_with_settings(15, 15, 6, 6, 80.0)
        world.setAgentStart(7.5, 7.5)
        
        agent = pynlm.create_agent_for_environment(brain, behavior)
        
        # Run simulation
        sim_results = pynlm.run_agent_simulation(
            brain, world, agent, 200, verbose=False
        )
        
        # Get performance metrics
        metrics = pynlm.profile_brain_performance(brain)
        
        results[behavior] = {
            'avg_firing': sim_results[2],
            'total_spikes': sim_results[1],
            'stability': metrics.stabilityClassification,
            'confidence': metrics.confidence,
            'avg_firing_rate': metrics.avgFiringRate,
            'coefficient_of_variation': metrics.coefficientOfVariation
        }
        
        print(f"  - Average firing rate: {sim_results[2]:.2f} Hz")
        print(f"  - Total spikes: {sim_results[1]}")
        print(f"  - Stability: {metrics.stabilityClassification}")
        print(f"  - Confidence: {metrics.confidence * 100:.1f}%")
    
    # Comparison summary
    print(f"\n=== Comparison Summary ===")
    best_stability = max(results.items(), key=lambda x: x[1]['confidence'])
    print(f"Most stable: {best_stability[0]} (confidence: {best_stability[1]['confidence'] * 100:.1f}%)")
    
    highest_firing = max(results.items(), key=lambda x: x[1]['avg_firing_rate'])
    print(f"Highest activity: {highest_firing[0]} (firing rate: {highest_firing[1]['avg_firing_rate']:.2f} Hz)")
    
    return results

def example_development_tracking():
    """Example 4: Development Tracking"""
    print("=== Example 4: Development Tracking ===")
    
    # Create brain with development enabled
    brain = pynlm.create_brain_for_task("complex")
    brain._initialize_default()
    
    print("Tracking brain development over time...")
    
    # Track development
    development_data = track_development_over_time(brain, 50)
    
    print(f"Development completed:")
    print(f"  - Final developmental stage: {development_data['final_stage']}")
    print(f"  - Average firing rate: {development_data['firing_rates'][-1]:.2f} Hz")
    print(f"  - Stage progression: {development_data['developmental_stages']}")
    
    # Check for development anomalies
    stage_changes = sum(1 for i in range(1, len(development_data['developmental_stages'])) 
                       if development_data['developmental_stages'][i] != 
                          development_data['developmental_stages'][i-1])
    
    if stage_changes > 0:
        print(f"✓ Development occurred over {stage_changes} stage changes")
    else:
        print(f"ℹ No development stage changes detected")
    
    print()

def example_error_analysis():
    """Example 5: Error Analysis and Debugging"""
    print("=== Example 5: Error Analysis and Debugging ===")
    
    # Test 1: Normal brain
    print("Test 1: Normal brain analysis")
    brain1 = pynlm.create_brain_for_task("simple")
    brain1._initialize_default()
    
    error_analysis1 = pynlm.analyze_brain_errors(brain1)
    print(f"  - Primary issue: {error_analysis1.primaryIssue}")
    print(f"  - Recommendations: {error_analysis1.recommendations}")
    print(f"  - Confidence: {error_analysis1.confidence * 100:.1f}%")
    
    # Test 2: Brain with potential issues
    print("\nTest 2: Brain with potential performance issues")
    brain2 = pynlm.create_brain_for_task("complex")
    brain2._initialize_default()
    
    # Simulate high activity to test error detection
    for i in range(100):
        brain2.step(i)
    
    error_analysis2 = pynlm.analyze_brain_errors(brain2)
    print(f"  - Primary issue: {error_analysis2.primaryIssue}")
    print(f"  - Recommendations: {error_analysis2.recommendations}")
    print(f"  - Confidence: {error_analysis2.confidence * 100:.1f}%")
    
    print()

def example_agent_lifecycle():
    """Example 6: Complete Agent Lifecycle"""
    print("=== Example 6: Complete Agent Lifecycle ===")
    
    # Phase 1: Setup
    print("Phase 1: Agent setup")
    brain, world, agent = pynlm.setup_complete_agent("learning", "exploratory")
    
    agent_setup_status = agent.get_status_report()
    print(f"  - Agent setup: {agent_setup_status.statusMessage}")
    print(f"  - Systems enabled: {agent_setup_status.rewardModEnabled}, "
          f"{agent_setup_status.structuralPlasticEnabled}, "
          f"{agent_setup_status.developmentEnabled}, "
          f"{agent_setup_status.curiosityEnabled}")
    
    # Phase 2: Training
    print("\nPhase 2: Agent training/simulation")
    training_results = pynlm.run_agent_simulation(
        brain, world, agent, 400, verbose=True
    )
    
    print(f"  - Training completed: {training_results[0]} steps")
    print(f"  - Performance: {training_results[2]:.2f} avg firing rate")
    
    # Phase 3: Analysis
    print("\nPhase 3: Performance analysis")
    performance_metrics = pynlm.profile_brain_performance(brain)
    
    print(f"  - Stability classification: {performance_metrics.stabilityClassification}")
    print(f"  - Coefficient of variation: {performance_metrics.coefficientOfVariation:.3f}")
    print(f"  - Total spikes: {performance_metrics.totalSpikes}")
    
    # Phase 4: Reset and reuse
    print("\nPhase 4: Reset and reuse")
    agent.reset_complete()
    print("  - Agent reset complete")
    
    # Re-initialize with different behavior
    agent2 = pynlm.create_agent_for_environment(brain, "conservative")
    print("  - Agent re-initialized with conservative behavior")
    
    print()

def example_batch_operations():
    """Example 7: Batch Operations"""
    print("=== Example 7: Batch Operations ===")
    
    brain = pynlm.create_brain_for_task("complex")
    brain._initialize_default()
    
    # Example 1: Batch current injection
    print("Batch current injection example")
    import random
    
    # Generate random neuron IDs and currents
    neurons = list(range(1, min(brain.neuron_count, 50) + 1))
    injections = []
    
    for _ in range(10):
        neuron_id = random.choice(neurons)
        # Create a NeuronId object
        neuron_id_obj = pynlm.NeuronId(neuron_id)
        current = random.uniform(10.0, 50.0)
        injections.append((neuron_id_obj, current))
    
    print(f"  - Injecting current to {len(injections)} neurons")
    brain.inject_current_batch(injections)
    print(f"  - Firing neurons after injection: {brain.getFiringNeuronCount()}")
    
    # Example 2: Multiple simulation steps
    print("\nBatch simulation steps example")
    start_step = 100
    num_steps = 50
    
    print(f"  - Running {num_steps} simulation steps starting at {start_step}")
    fired_count = brain.batch_step(start_step, num_steps)
    print(f"  - Total neurons fired: {fired_count}")
    print(f"  - Final firing count: {brain.getFiringNeuronCount()}")
    
    # Example 3: Batch configuration
    print("\nBatch configuration example")
    config = pynlm.createDefaultConfig()
    settings = [
        ("neuron_count", "2000"),
        ("region_count", "3"),
        ("connection_probability", "0.08"),
        ("random_seed", "123")
    ]
    
    applied = config.batch_set(settings)
    print(f"  - Applied {applied} out of {len(settings)} settings")
    
    print()

def track_development_over_time(brain, time_steps):
    """
    Helper function to track brain development over time.
    This is extracted from the main examples for reuse.
    """
    developmental_stages = []
    firing_rates = []
    neuromodulation_levels = []
    
    for step in range(time_steps):
        # Record current state
        developmental_stages.append(brain.getDevelopmentalStage())
        firing_rates.append(brain.getAverageFiringRate())
        neuromodulation_levels.append(brain.getExcitationInhibitionRatio())
        
        # Advance development
        brain.develop()
        
        # Simulate neural activity
        brain.step(step)
    
    return {
        'developmental_stages': developmental_stages,
        'firing_rates': firing_rates,
        'neuromodulation_levels': neuromodulation_levels,
        'final_stage': developmental_stages[-1],
        'stage_changes': sum(1 for i in range(1, len(developmental_stages)) 
                           if developmental_stages[i] != developmental_stages[i-1]),
        'avg_firing_rate': sum(firing_rates) / len(firing_rates) if firing_rates else 0
    }

def main():
    """Run all examples"""
    print("Enhanced NLM Python API Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_basic_usage,
        example_complete_agent,
        example_behavior_comparison,
        example_development_tracking,
        example_error_analysis,
        example_agent_lifecycle,
        example_batch_operations,
    ]
    
    for example in examples:
        try:
            example()
        except Exception as e:
            print(f"Error in {example.__name__}: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("All examples completed!")
    print("\nFor more information, see EnhancedAPIDocumentation.md")

if __name__ == "__main__":
    main()