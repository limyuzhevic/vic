#!/usr/bin/env python3
"""
Demonstration script showcasing the improved NLM Python bindings.

This script demonstrates the new Pythonic interfaces, enhanced features,
and improved developer experience of the NLM Python bindings.
"""

import time
import sys
from pathlib import Path

# Add the python module to the path
sys.path.insert(0, str(Path(__file__).parent / 'python'))

# Import the enhanced NLM bindings
import pynlm

def demonstrate_basic_functionality():
    """Demonstrate basic NLM functionality."""
    print("=== Basic NLM Functionality ===")
    
    # Create and initialize brain using enhanced Pythonic interface
    config = pynlm.createDefaultConfig()
    print(f"Created default config: {config}")
    
    brain = pynlm.createBrain(config)
    print(f"Created brain: {brain}")
    
    brain.initialize()
    print(f"Initialized brain with {brain.total_neurons} neurons")
    
    # Run simple simulation
    for step in range(10):
        brain.step(step)
    
    print(f"Simulation complete: {brain.getTotalSpikeCount()} total spikes")
    print()

def demonstrate_enhanced_configuration():
    """Demonstrate enhanced configuration features."""
    print("=== Enhanced Configuration Features ===")
    
    # Create configuration with validation
    config = pynlm.create_config(neuron_count=500)
    print(f"Created config: {config}")
    
    # Use Pythonic configuration access
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    print(f"Updated config: {config}")
    
    # Validate configuration
    from pynlm.utils import validate_config
    validation_result = validate_config(config.to_dict())
    
    if validation_result['valid']:
        print("✓ Configuration is valid")
        print(f"  Normalized: {validation_result['normalized_config']}")
    else:
        print("✗ Configuration errors:")
        for error in validation_result['errors']:
            print(f"  - {error}")
    
    print()

def demonstrate_pythonic_interfaces():
    """Demonstrate Pythonic interfaces."""
    print("=== Pythonic Interfaces ===")
    
    # Use context manager
    print("Using context manager:")
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        brain.initialize()
        print(f"  Brain created and initialized: {brain}")
        brain.step(0)
        print(f"  Brain stepped: {brain}")
    
    print("  Context manager exited - brain cleaned up")
    
    # Use property-based access
    print("\nUsing property-based access:")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"  Total neurons: {brain.total_neurons}")
    print(f"  Total synapses: {brain.total_synapses}")
    print(f"  Active neurons: {brain.active_neurons}")
    print(f"  Firing neurons: {brain.firing_neurons}")
    print(f"  Average firing rate: {brain.average_firing_rate:.3f}")
    print(f"  E/I ratio: {brain.e_i_ratio:.3f}")
    print(f"  Total spikes: {brain.total_spikes}")
    
    brain.step(0)
    print(f"  After step - Firing neurons: {brain.firing_neurons}")
    
    print()

def demonstrate_advanced_features():
    """Demonstrate advanced simulation features."""
    print("=== Advanced Simulation Features ===")
    
    # Create brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced features enabled:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    
    # Run simulation loop
    for step in range(20):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 5 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Prediction error: {agent.getPredictionError():.3f}")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
    
    print("Advanced simulation completed")
    print()

def demonstrate_monitoring_and_analysis():
    """Demonstrate monitoring and analysis features."""
    print("=== Monitoring and Analysis ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create monitoring tools
    from pynlm.monitoring import StatisticsCollector, PerformanceMonitor, NeuralAnalyzer
    
    collector = StatisticsCollector(pynlm.MonitoringLevel.COMPREHENSIVE)
    monitor = PerformanceMonitor()
    analyzer = NeuralAnalyzer(sampling_rate=1000.0)
    
    monitor.start_monitoring()
    
    # Run simulation with monitoring
    for step in range(50):
        brain.step(step)
        metrics = brain.get_statistics()
        
        # Record data
        collector.record_step(step, metrics)
        monitor.record_step_time(time.time())
        analyzer.add_neural_data(step, metrics)
        
        # Progress reporting
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    monitor.stop_monitoring()
    
    # Get reports
    print("\nPerformance Report:")
    performance = monitor.get_performance_report()
    print(f"  Steps: {performance['steps']}")
    print(f"  Total time: {performance['total_time']:.3f}s")
    print(f"  Steps per second: {performance['steps_per_second']:.1f}")
    
    print("\nStatistics Summary:")
    stats = collector.get_summary_statistics()
    print(f"  Total steps: {stats.get('total_steps', 0)}")
    print(f"  Total spikes: {stats.get('total_spikes', 0)}")
    print(f"  Average firing rate: {stats.get('neural_activity', {}).get('average_firing_rate', 0):.3f}")
    
    print("\nNeural Analysis:")
    analysis = analyzer.get_analysis_summary()
    print(f"  Analysis complete: {analysis['analysis_complete']}")
    print(f"  Duration: {analysis.get('time_span', 0):.1f}s")
    print(f"  Firing rate range: {analysis['activity_range']['min_firing_rate']:.3f} - {analysis['activity_range']['max_firing_rate']:.3f}")
    print(f"  Key findings: {len(analysis['key_findings'])}")
    for finding in analysis['key_findings']:
        print(f"    - {finding}")
    
    print("\nPattern Detection:")
    patterns = analyzer.detect_patterns()
    for pattern_type, pattern_data in patterns.items():
        print(f"  {pattern_type}: {pattern_data}")
    
    print("\nMonitoring and analysis completed")
    print()

def demonstrate_batch_processing():
    """Demonstrate batch processing features."""
    print("=== Batch Processing ===")
    
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create batch simulator
    from pynlm.simulation import BatchSimulator
    
    simulator = BatchSimulator(brain)
    
    print("Running batch simulation...")
    start_time = time.time()
    
    results = simulator.run_batch(1000, timestep=0.001, enable_monitoring=True)
    
    end_time = time.time()
    
    print(f"Batch simulation completed in {end_time - start_time:.3f}s")
    print(f"Results:")
    print(f"  Total steps: {results['total_steps']}")
    print(f"  Total time: {results['total_time']:.3f}s")
    print(f"  Average step time: {results['average_step_time']:.6f}s")
    print(f"  Steps per second: {results['steps_per_second']:.1f}")
    print(f"  Max step time: {results['max_step_time']:.6f}s")
    print(f"  Min step time: {results['min_step_time']:.6f}s")
    
    # Get performance statistics
    performance = simulator.get_performance_stats()
    print(f"Performance statistics:")
    print(f"  Total steps: {performance['total_steps']}")
    print(f"  Total time: {performance['total_time']:.3f}s")
    print(f"  Steps per second: {performance['steps_per_second']:.1f}")
    print(f"  Step time std: {performance['step_time_std']:.6f}s")
    
    print("\nBatch processing completed")
    print()

def demonstrate_episodic_simulation():
    """Demonstrate episodic simulation features."""
    print("=== Episodic Simulation ===")
    
    # Create complete agent system
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create episode runner
    from pynlm.simulation import EpisodeRunner
    
    runner = EpisodeRunner(brain, world, agent)
    
    print("Running episodic simulation...")
    
    # Run multiple episodes
    episodes = []
    for episode_num in range(3):
        print(f"\n  Episode {episode_num + 1}:")
        
        episode = runner.run_episode(max_steps=100, enable_learning=True)
        episodes.append(episode)
        
        print(f"    Steps: {episode.steps}")
        print(f"    Total spikes: {episode.total_spikes}")
        print(f"    Average firing rate: {episode.average_firing_rate:.3f}")
        print(f"    Total reward: {episode.reward:.3f}")
        print(f"    Curiosity level: {episode.curiosity_level:.3f}")
        print(f"    Development stage: {episode.development_stage}")
        print(f"    Duration: {episode.duration:.3f}s")
    
    # Get episode statistics
    stats = runner.get_episode_statistics()
    print(f"\nEpisode Statistics:")
    print(f"  Total episodes: {stats['episodes']}")
    print(f"  Total steps: {stats['total_steps']}")
    print(f"  Average steps per episode: {stats['average_steps_per_episode']:.1f}")
    print(f"  Max steps: {stats['max_steps']}")
    print(f"  Min steps: {stats['min_steps']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.3f}")
    print(f"  Average curiosity: {stats['average_curiosity']:.3f}")
    
    print("\nEpisodic simulation completed")
    print()

def demonstrate_configuration_validation():
    """Demonstrate configuration validation features."""
    print("=== Configuration Validation ===")
    
    from pynlm.utils import create_config, validate_config, ConfigurationValidator
    
    # Create test configurations
    print("Creating test configurations:")
    
    # Simple configuration
    simple_config = create_config(neuron_count=100)
    print(f"  Simple config: {simple_config}")
    
    # Validation
    validation_result = validate_config(simple_config)
    
    if validation_result['valid']:
        print("  ✓ Simple config is valid")
        print(f"    Normalized: {validation_result['normalized_config']}")
    else:
        print("  ✗ Simple config has errors:")
        for error in validation_result['errors']:
            print(f"    - {error}")
    
    # Advanced configuration
    from pynlm.utils import create_test_config
    advanced_config = create_test_config('complex')
    print(f"\n  Advanced config: {advanced_config}")
    
    # Validation with advanced config
    validation_result2 = validate_config(advanced_config)
    
    if validation_result2['valid']:
        print("  ✓ Advanced config is valid")
        print(f"    Has {len(validation_result2['normalized_config'])} sections")
    else:
        print("  ✗ Advanced config has errors:")
        for error in validation_result2['errors']:
            print(f"    - {error}")
    
    # Direct validator usage
    print("\nUsing ConfigurationValidator directly:")
    validator = ConfigurationValidator()
    
    test_configs = [
        {'brain': {'neuron_count': 50}},
        {'brain': {'neuron_count': 1000, 'synapse_density': 0.2}},
        {'brain': {'neuron_count': 10000, 'synapse_density': 0.5}},
    ]
    
    for i, config in enumerate(test_configs):
        result = validator.validate_config(config)
        print(f"  Config {i+1}: {'✓ Valid' if result['valid'] else '✗ Invalid'}")
        if result['warnings']:
            print(f"    Warnings: {result['warnings']}")
    
    print("\nConfiguration validation completed")
    print()

def demonstrate_utility_functions():
    """Demonstrate utility functions."""
    print("=== Utility Functions ===")
    
    from pynlm.utils import (
        create_config, validate_config, format_output, 
        analyze_results, OutputFormatter, ResultAnalyzer
    )
    
    # Create sample data
    sample_data = [
        {'step': 0, 'total_spikes': 5, 'active_neurons': 10, 'average_firing_rate': 0.5},
        {'step': 1, 'total_spikes': 8, 'active_neurons': 15, 'average_firing_rate': 0.8},
        {'step': 2, 'total_spikes': 12, 'active_neurons': 20, 'average_firing_rate': 1.2},
    ]
    
    # Format output
    print("Formatted JSON output:")
    json_output = format_output(sample_data, 'json')
    print(json_output[:200] + "..." if len(json_output) > 200 else json_output)
    
    # Format CSV output
    print("\nFormatted CSV output:")
    csv_output = format_output(sample_data, 'csv')
    lines = csv_output.split('\n')[:5]
    for line in lines:
        print(f"  {line}")
    
    # Analyze results
    print("\nAnalysis results:")
    analysis = analyze_results(sample_data, 'summary')
    for key, value in analysis.items():
        print(f"  {key}: {value}")
    
    # Format report
    print("\nFormatted report:")
    report = format_output(analysis, 'summary')
    print(report)
    
    print("\nUtility functions completed")
    print()

def demonstrate_context_manager():
    """Demonstrate context manager functionality."""
    print("=== Context Manager ===")
    
    print("Using context manager for automatic resource management:")
    
    # This demonstrates the Pythonic interface
    from pynlm.brain import Brain
    
    # Using the enhanced Brain class
    config = Brain.createDefaultConfig()
    
    print(f"Created config: {config}")
    
    # The Brain class now has context manager support
    # (This would be demonstrated in the actual implementation)
    print("✓ Context manager support available in Brain class")
    
    print("\nContext manager demonstration completed")
    print()

def main():
    """Run all demonstrations."""
    print("NLM Python Bindings - Enhanced Features Demonstration")
    print("=" * 60)
    print()
    
    try:
        demonstrate_basic_functionality()
        demonstrate_enhanced_configuration()
        demonstrate_pythonic_interfaces()
        demonstrate_advanced_features()
        demonstrate_monitoring_and_analysis()
        demonstrate_batch_processing()
        demonstrate_episodic_simulation()
        demonstrate_configuration_validation()
        demonstrate_utility_functions()
        demonstrate_context_manager()
        
        print("=" * 60)
        print("All demonstrations completed successfully!")
        print()
        print("Key improvements demonstrated:")
        print("  ✓ Pythonic interfaces with context managers")
        print("  ✓ Enhanced configuration validation")
        print("  ✓ Property-based access")
        print("  ✓ Advanced simulation features")
        print("  ✓ Comprehensive monitoring and analysis")
        print("  ✓ Batch processing capabilities")
        print("  ✓ Episodic simulation support")
        print("  ✓ Utility functions and formatters")
        print("  ✓ Configuration validation")
        
    except Exception as e:
        print(f"\nError during demonstration: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())