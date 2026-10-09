#!/usr/bin/env python3
"""
Example 2: Advanced Learning Experiment

This example demonstrates an advanced learning experiment with multiple
components including neural plasticity, curiosity-driven exploration,
and comprehensive analysis.

Key features demonstrated:
- Multi-component simulation setup
- Advanced learning configuration
- Real-time monitoring and analysis
- Learning curve tracking and analysis
- Performance benchmarking
"""

import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner,
    ExperimentConfig,
    NLMConfigAnalyzer,
    NLMAnalysisTools,
    create_advanced_brain_config,
    run_advanced_simulation
)
import time
import matplotlib.pyplot as plt
import numpy as np

def example_advanced_learning():
    """Demonstrate advanced learning experiment setup and execution."""
    print("=== Example 2: Advanced Learning Experiment ===\n")
    
    # Create advanced brain configuration
    print("1. Creating advanced brain configuration...")
    brain_config = create_advanced_brain_config(
        neuron_count=2000,
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    # Add experiment-specific configuration
    brain_config.set("brain.synapse_density", 0.15)
    brain_config.set("plasticity.stdp.learning_rate", 0.002)
    brain_config.set("plasticity.hebbian.learning_rate", 0.001)
    brain_config.set("neuromod.curiosity.threshold", 0.3)
    brain_config.set("neuromod.novelty.sensitivity", 0.8)
    
    print(f"   Brain neurons: {brain_config.getOr<int64_t>('brain.neuron_count', 0)}")
    print(f"   Synapse density: {brain_config.getOr<float>('brain.synapse_density', 0.0)}")
    print(f"   STDP enabled: {brain_config.getOr<bool>('plasticity.stdp.enable', False)}")
    print(f"   Curiosity enabled: {brain_config.getOr<bool>('neuromod.curiosity.enable', False)}")
    
    # Create world with obstacles and resources
    print("\n2. Setting up world environment...")
    world_config = {
        'width': 50,
        'height': 50,
        'visionWidth': 15,
        'visionHeight': 15,
        'maxEnergy': 100.0,
        'energyDecayRate': 0.01
    }
    
    world = pynlm.createSimpleWorld()
    world.configure(**world_config)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    # Add some objects to the world
    import pynlm
    # Note: In a real implementation, you would add objects to the world
    # world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, value=1.0))
    
    # Create brain and agent
    print("\n3. Creating brain and agent...")
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    print(f"   ✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Create experiment runner with advanced configuration
    print("\n4. Setting up experiment runner...")
    runner = NLMExperimentRunner(brain, world, agent)
    
    # Add multiple callbacks for comprehensive monitoring
    def learning_monitor(runner, metrics, event_type):
        """Monitor learning progress and alert on key events."""
        learning_rate = metrics.get('brain_firing_rate', 0)
        curiosity = metrics.get('curiosity_level', 0)
        novelty = metrics.get('novelty_level', 0)
        
        if learning_rate > 8.0 and metrics['step'] % 50 == 0:
            print(f"   [Learning Monitor] High firing rate at step {metrics['step']}: {learning_rate:.2f} Hz")
        
        if curiosity > 0.7:
            print(f"   [Learning Monitor] High curiosity at step {metrics['step']}: {curiosity:.2f}")
        
        if novelty > 0.5 and metrics['step'] % 100 == 0:
            print(f"   [Learning Monitor] Novelty peak at step {metrics['step']}: {novelty:.2f}")
    
    def development_monitor(runner, metrics, event_type):
        """Monitor developmental progress."""
        stage = metrics.get('development_stage', 0)
        if stage > 2 and metrics['step'] % 200 == 0:
            print(f"   [Development Monitor] Advanced developmental stage: {stage}")
    
    def exploration_monitor(runner, metrics, event_type):
        """Monitor exploration behavior."""
        if metrics.get('agent_x', 0) > 10 and metrics.get('agent_x', 0) < 40:
            print(f"   [Exploration Monitor] Agent exploring at ({metrics['agent_x']:.1f}, {metrics['agent_y']:.1f})")
    
    # Add all monitors
    runner.add_callback(learning_monitor, "step")
    runner.add_callback(development_monitor, "step")
    runner.add_callback(exploration_monitor, "step")
    
    # Create comprehensive experiment configuration
    experiment_config = ExperimentConfig(
        name="Advanced Learning Experiment",
        description="Complex neural network with curiosity-driven exploration and learning",
        max_steps=2000,
        timestep=0.1,
        random_seed=42,
        verbose=True,
        save_checkpoints=True,
        checkpoint_interval=200,
        metadata={
            'experiment_type': 'advanced_learning',
            'neurons': brain_config.getOr<int64_t>('brain.neuron_count', 0),
            'plasticity': 'STDP+Hebbian',
            'curiosity': True,
            'development': True
        }
    )
    
    print(f"\n5. Running advanced experiment...")
    print(f"   Experiment: {experiment_config.name}")
    print(f"   Description: {experiment_config.description}")
    print(f"   Max steps: {experiment_config.max_steps}")
    print(f"   Timestep: {experiment_config.timestep}s")
    print(f"   Checkpoints: {'Enabled' if experiment_config.save_checkpoints else 'Disabled'}")
    
    # Run the experiment
    start_time = time.time()
    result = runner.run_experiment(experiment_config)
    end_time = time.time()
    
    print(f"\n=== Experiment Results ===")
    print(f"Success: {'✓' if result.success else '✗'}")
    print(f"Total duration: {end_time - start_time:.2f} seconds")
    print(f"Steps completed: {result.steps_completed}")
    print(f"Final firing rate: {result.final_firing_rate:.3f} Hz")
    print(f"Total spikes: {result.total_spikes}")
    print(f"Checkpoints saved: {len(result.checkpoint_files)}")
    
    if result.error:
        print(f"Error: {result.error}")
    
    # Analyze learning metrics
    print(f"\n=== Learning Analysis ===")
    if result.learning_metrics:
        metrics = result.learning_metrics
        print(f"Firing rate trend: {metrics.get('firing_rate_trend', 0):.4f}")
        print(f"Spike activity trend: {metrics.get('spike_activity_trend', 0):.4f}")
        print(f"Stability: {metrics.get('stability', 0):.4f}")
        print(f"Exploration index: {metrics.get('exploration_index', 0):.4f}")
        print(f"Peak activity: {metrics.get('peak_activity', 0):.4f}")
        print(f"Average activity: {metrics.get('average_activity', 0):.4f}")
        print(f"Coherence: {metrics.get('coherence', 0):.4f}")
        
        # Determine learning quality
        trend = metrics.get('firing_rate_trend', 0)
        stability = metrics.get('stability', 0)
        exploration = metrics.get('exploration_index', 0)
        
        learning_quality = "Excellent"
        if trend < 0.1 and stability > 0.5:
            learning_quality = "Good"
        elif trend < 0.2 and stability > 0.3:
            learning_quality = "Fair"
        else:
            learning_quality = "Needs Improvement"
        
        print(f"\nLearning Quality Assessment: {learning_quality}")
    
    return result, runner

def example_configuration_optimization():
    """Demonstrate configuration optimization and comparison."""
    print("\n=== Configuration Optimization Example ===\n")
    
    analyzer = NLMConfigAnalyzer()
    
    # Create different configurations to compare
    print("1. Creating different configurations...")
    
    configs = []
    
    # Configuration 1: Small, simple network
    config1 = create_advanced_brain_config(
        neuron_count=500,
        enable_learning=True,
        enable_development=False,
        enable_curiosity=False
    )
    config1.set("brain.synapse_density", 0.05)
    config1.set("plasticity.stdp.learning_rate", 0.001)
    configs.append(("Small Network", config1))
    
    # Configuration 2: Medium network with learning
    config2 = create_advanced_brain_config(
        neuron_count=1500,
        enable_learning=True,
        enable_development=True,
        enable_curiosity=False
    )
    config2.set("brain.synapse_density", 0.1)
    config2.set("plasticity.stdp.learning_rate", 0.002)
    configs.append(("Medium Network", config2))
    
    # Configuration 3: Large network with all features
    config3 = create_advanced_brain_config(
        neuron_count=3000,
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True
    )
    config3.set("brain.synapse_density", 0.15)
    config3.set("plasticity.stdp.learning_rate", 0.003)
    config3.set("neuromod.curiosity.threshold", 0.2)
    configs.append(("Large Network with Curiosity", config3))
    
    # Validate each configuration
    print("\n2. Validating configurations...")
    validation_results = []
    
    for name, config in configs:
        validation = analyzer.validate_config(config)
        validation_results.append({
            'name': name,
            'validation': validation,
            'config': config
        })
        
        print(f"\n{name}:")
        print(f"  Validation score: {validation['score']}/100")
        print(f"  Valid: {'Yes' if validation['valid'] else 'No'}")
        if validation['suggestions']:
            print(f"  Suggestions: {validation['suggestions']}")
    
    # Compare configurations
    print("\n3. Comparing configurations...")
    config_objects = [result['config'] for result in validation_results]
    comparison = analyzer.compare_configs(config_objects)
    
    print(f"\nPerformance Estimates:")
    for name, estimate in comparison['performance_estimates'].items():
        print(f"  {name}: Score = {estimate['estimated_performance']:.1f}, "
              f"Firing Rate = {estimate['expected_firing_rate']:.2f} Hz")
    
    # Generate recommendations
    print(f"\n4. Recommendations:")
    if comparison['recommendations']:
        for recommendation in comparison['recommendations']:
            print(f"  - {recommendation}")
    
    # Find best configuration
    best_key = max(comparison['performance_estimates'].items(), 
                  key=lambda x: x[1]['estimated_performance'])[0]
    
    print(f"\n5. Best Configuration: {best_key}")
    best_validation = next(r['validation'] for r in validation_results 
                          if str(r['config']) == best_key)
    print(f"   Validation score: {best_validation['score']}/100")
    
    return validation_results, comparison

def example_performance_benchmarking():
    """Demonstrate performance benchmarking."""
    print("\n=== Performance Benchmarking Example ===\n")
    
    analyzer = NLMAnalysisTools()
    
    # Simulate benchmark data from multiple experiments
    print("1. Simulating benchmark data...")
    
    experiments = []
    
    # Generate synthetic experiment data
    for i in range(10):
        duration = np.random.normal(12.0, 3.0)  # Mean 12s, std 3s
        success = np.random.choice([True, False], p=[0.8, 0.2])
        
        experiment = {
            'name': f'Benchmark_Exp_{i+1}',
            'duration': max(0.1, duration),  # Ensure positive duration
            'success': success,
            'config': create_advanced_brain_config(neuron_count=1000)
        }
        
        if success:
            # Add metrics based on experiment characteristics
            neurons = experiment['config'].getOr<int64_t>('brain.neuron_count', 1000)
            experiment['learning_metrics'] = {
                'firing_rate_trend': np.random.uniform(-0.1, 0.1),
                'stability': np.random.uniform(0.3, 0.8),
                'exploration_index': np.random.uniform(0.2, 0.8),
                'peak_activity': np.random.uniform(5.0, 20.0),
                'average_activity': np.random.uniform(3.0, 15.0),
                'coherence': np.random.uniform(0.1, 0.9)
            }
        
        experiments.append(experiment)
    
    # Run batch analysis
    print("2. Running batch analysis...")
    batch_analysis = analyzer.batch_analyze_experiments(experiments)
    
    print(f"\nBenchmark Results:")
    print(f"  Total experiments: {batch_analysis['experiment_count']}")
    print(f"  Successful: {batch_analysis['successful_experiments']}")
    print(f"  Failed: {batch_analysis['failed_experiments']}")
    print(f"  Success rate: {batch_analysis['successful_experiments']/batch_analysis['experiment_count']*100:.1f}%")
    print(f"  Average duration: {batch_analysis['average_duration']:.2f}s")
    print(f"  Duration std: {batch_analysis['duration_std']:.2f}s")
    
    # Performance rankings
    print(f"\nPerformance Rankings:")
    for i, ranked in enumerate(batch_analysis['performance_rankings'][:5]):
        print(f"  {i+1}. {ranked['experiment_name']}: "
              f"Score = {ranked['performance_score']:.1f}")
    
    # Generate recommendations
    if batch_analysis['recommendations']:
        print(f"\nRecommendations:")
        for recommendation in batch_analysis['recommendations']:
            print(f"  - {recommendation}")
    
    return batch_analysis

if __name__ == "__main__":
    print("NLM Advanced Features - Example 2: Advanced Learning Experiment")
    print("=" * 70)
    
    try:
        # Run advanced learning experiment
        result, runner = example_advanced_learning()
        
        # Run configuration optimization example
        config_results, comparison = example_configuration_optimization()
        
        # Run performance benchmarking example
        batch_analysis = example_performance_benchmarking()
        
        print("\n" + "=" * 70)
        print("Example 2 completed successfully!")
        print("Key insights:")
        print("  - Advanced experiments benefit from curiosity-driven exploration")
        print("  - Configuration optimization can significantly improve performance")
        print("  - Batch analysis helps identify successful patterns")
        
    except Exception as e:
        print(f"\nError in example 2: {e}")
        import traceback
        traceback.print_exc()
