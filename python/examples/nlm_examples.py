#!/usr/bin/env python3
"""
NLM Examples and Tutorials

This module provides examples and tutorials for using the NLM (Neural Learning Machine)
Python bindings, covering basic usage, advanced features, and real-world scenarios.
"""

import sys
from pathlib import Path

# Add the python module to path
sys.path.insert(0, str(Path(__file__).parent))

from pynlm import Brain, Config, Experiment, Phase6Integration, Metrics
from pynlm.logging import Logger, PerformanceLogger


def basic_example():
    """Basic NLM usage example."""
    print("=== Basic NLM Example ===")
    
    # Create a brain with basic configuration
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("connection_probability", 0.15)
    
    brain = Brain(config)
    brain.initialize()
    
    # Run a simple simulation
    for step in range(100):
        brain.step(step, step * 0.001)
    
    # Display results
    print(f"Simulation completed with {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print()


def learning_experiment_example():
    """Learning experiment example."""
    print("=== Learning Experiment Example ===")
    
    # Create brain with learning-specific configuration
    config = Config()
    config.set("neuron_count", 200)
    config.set("region_count", 1)
    config.set("connection_probability", 0.2)
    
    brain = Brain(config)
    brain.initialize()
    
    # Create learning experiment
    experiment = Experiment(brain)
    
    # Run learning simulation
    result = experiment.run(steps=500)
    
    # Display results
    print(f"Learning experiment completed")
    print(f"Total reward: {result.total_reward:.2f}")
    print(f"Average firing rate: {result.avg_firing_rate:.2f}")
    print(f"Learning occurred: {result.learning_occurred}")
    print()


def phase6_integration_example():
    """Phase 6 integration example."""
    print("=== Phase 6 Integration Example ===")
    
    # Create Phase 6 integration
    integration = Phase6Integration()
    
    # Configure for Phase 6 experiment
    config = Phase6Integration.Phase6Config()
    config.neuron_count = 500
    config.max_steps = 2000
    config.enable_checkpointing = True
    config.enable_replay = True
    config.enable_development = True
    
    # Run Phase 6 integration
    result = integration.run(config)
    
    # Display results
    print(f"Phase 6 integration completed")
    print(f"Total reward: {result.total_reward:.2f}")
    print(f"Average firing rate: {result.avg_firing_rate:.2f}")
    print(f"Memory working memory integrated: {result.memory_working_memory_integrated}")
    print(f"Memory episodic memory integrated: {result.memory_episodic_memory_integrated}")
    print(f"Neuromodulation integrated: {result.neuromodulation_integrated}")
    print(f"Prediction integrated: {result.prediction_integrated}")
    print(f"Development integrated: {result.development_integrated}")
    print(f"Checkpointing works: {result.checkpointing_works}")
    print(f"Replay works: {result.replay_works}")
    print()


def visualization_example():
    """Visualization example."""
    print("=== Visualization Example ===")
    
    # Create brain with visualization-enabled configuration
    config = Config()
    config.set("neuron_count", 150)
    config.set("region_count", 1)
    config.set("connection_probability", 0.15)
    
    brain = Brain(config)
    brain.initialize()
    
    # Import visualization module
    try:
        from pynlm_visualization import (
            create_network_visualization,
            create_activity_animation,
            create_results_summary
        )
        
        # Create visualizations
        create_network_visualization(brain, "network_visualization.html")
        create_activity_animation(brain, "activity_animation.gif", steps=50)
        create_results_summary(brain, "results_summary.html")
        
        print("Visualizations created successfully:")
        print("  - network_visualization.html")
        print("  - activity_animation.gif")
        print("  - results_summary.html")
    except ImportError as e:
        print(f"Visualization module not available: {e}")
    
    print()


def logging_example():
    """Logging example."""
    print("=== Logging Example ===")
    
    # Create logger
    logger = Logger()
    logger.set_level("INFO")
    
    # Add handlers
    logger.add_console_handler()
    logger.add_file_handler("nlm_example.log")
    
    # Log simulation events
    logger.info("Starting NLM simulation")
    logger.debug("Debug information about brain state")
    
    # Create brain
    config = Config()
    config.set("neuron_count", 100)
    brain = Brain(config)
    brain.initialize()
    
    logger.info("Brain initialized successfully")
    logger.debug(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(50):
        brain.step(step, step * 0.001)
        if step % 10 == 0:
            logger.info(f"Completed {step} steps")
    
    logger.info("Simulation completed")
    
    # Clean up
    for handler in logger.get_handlers():
        if hasattr(handler, 'close'):
            handler.close()
    
    print("Logging example completed")
    print()


def performance_example():
    """Performance monitoring example."""
    print("=== Performance Monitoring Example ===")
    
    # Create performance logger
    perf_logger = PerformanceLogger()
    
    # Create brain
    config = Config()
    config.set("neuron_count", 100)
    brain = Brain(config)
    brain.initialize()
    
    # Run simulation with performance monitoring
    for step in range(100):
        perf_logger.start_timing("brain_step")
        brain.step(step, step * 0.001)
        perf_logger.end_timing("brain_step")
        
        if step % 20 == 0:
            perf_logger.log_performance("brain_step", step * 0.001)
            perf_logger.log_memory_usage()
    
    # Get performance statistics
    stats = perf_logger.get_performance_stats()
    print(f"Performance statistics:")
    for operation, data in stats.items():
        print(f"  {operation}: {data}")
    
    print()


def configuration_example():
    """Configuration example."""
    print("=== Configuration Example ===")
    
    # Create configuration from file
    try:
        config = Config.load_from_file("configs/phase6_default.cfg")
        print(f"Configuration loaded from file:")
        print(f"  - Neuron count: {config.get('neuron_count')}")
        print(f"  - Region count: {config.get('region_count')}")
        print(f"  - Connection probability: {config.get('connection_probability')}")
        print(f"  - Max steps: {config.get('max_steps')}")
    except Exception as e:
        print(f"Could not load configuration file: {e}")
    
    # Create configuration from template
    try:
        from pynlm import ConfigTemplates
        learning_config = ConfigTemplates.learning_experiment()
        print(f"\nConfiguration from learning experiment template:")
        print(f"  - Neuron count: {learning_config.get('neuron_count')}")
        print(f"  - Connection probability: {learning_config.get('connection_probability')}")
    except Exception as e:
        print(f"Could not load configuration template: {e}")
    
    # Create custom configuration
    config = Config()
    config.set("neuron_count", 300)
    config.set("region_count", 2)
    config.set("connection_probability", 0.25)
    config.set("enable_development", True)
    
    print(f"\nCustom configuration:")
    print(f"  - Neuron count: {config.get('neuron_count')}")
    print(f"  - Region count: {config.get('region_count')}")
    print(f"  - Connection probability: {config.get('connection_probability')}")
    print(f"  - Enable development: {config.get('enable_development')}")
    
    print()


def command_line_example():
    """Command line usage example."""
    print("=== Command Line Usage Example ===")
    print("NLM provides a comprehensive command-line interface:")
    print()
    print("Basic commands:")
    print("  nlm info                    # Show system information")
    print("  nlm create --neurons 500    # Create a brain")
    print("  nlm simulate input.bin      # Run simulation")
    print("  nlm config list             # List configurations")
    print()
    print("Phase 6 integration:")
    print("  nlm phase6-demo --neurons 500 --steps 2000")
    print("  nlm phase6-demo --checkpoint --replay")
    print()
    print("Experiments:")
    print("  nlm experiment --type learning --steps 1000")
    print("  nlm experiment --type plasticity-stdp")
    print("  nlm experiment --type development")
    print()
    print("Comparison:")
    print("  nlm compare config1.json config2.json")
    print()
    print("For more details, run 'nlm --help'")
    print()


def advanced_example():
    """Advanced NLM usage example."""
    print("=== Advanced NLM Example ===")
    
    # Create brain with advanced configuration
    config = Config()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    
    brain = Brain(config)
    brain.initialize()
    
    # Enable advanced features
    brain.set_plasticity_rule("stdp", {"weight": 0.01})
    brain.set_neuromodulator("dopamine", {"learning_rate": 0.01})
    brain.set_neuromodulator("curiosity", {"threshold": 0.5})
    
    # Create advanced experiment
    experiment = Experiment(brain)
    experiment.set_objectives(["learning", "memory", "adaptation"])
    
    # Run advanced simulation
    result = experiment.run(steps=2000)
    
    # Analyze results
    print(f"Advanced experiment completed")
    print(f"Total reward: {result.total_reward:.2f}")
    print(f"Final performance: {result.final_performance:.2f}")
    print(f"Memory storage: {result.memory_episodes_stored}")
    print(f"Learning achieved: {result.learning_occurred}")
    print(f"Performance improvement: {result.performance_improvement:.2%}")
    print()


def tutorial_example():
    """Tutorial example for beginners."""
    print("=== NLM Tutorial for Beginners ===")
    print()
    print("Welcome to NLM (Neural Learning Machine)!")
    print()
    print("What is NLM?")
    print("NLM is an experimental artificial developmental brain that:")
    print("- Creates neural networks with spiking neurons")
    print("- Learns through experience and interaction")
    print("- Develops and adapts over time")
    print("- Integrates multiple cognitive systems")
    print()
    print("Getting Started:")
    print("1. Create a brain:")
    print("   brain = Brain(neuron_count=100)")
    print("   brain.initialize()")
    print()
    print("2. Run simulation:")
    print("   for step in range(100):")
    print("       brain.step(step, step * 0.001)")
    print()
    print("3. Access results:")
    print("   print(f\"Total spikes: {brain.getTotalSpikeCount()}\")")
    print("   print(f\"Average firing rate: {brain.getAverageFiringRate():.2f}\")")
    print()
    print("4. Save brain:")
    print("   brain.save(\"my_brain.bin\")")
    print()
    print("Try the basic example now:")
    try:
        basic_example()
    except Exception as e:
        print(f"Error running example: {e}")
    print()


def main():
    """Run all examples."""
    print("NLM Examples and Tutorials")
    print("=" * 50)
    print()
    
    examples = [
        ("Basic Example", basic_example),
        ("Learning Experiment Example", learning_experiment_example),
        ("Phase 6 Integration Example", phase6_integration_example),
        ("Visualization Example", visualization_example),
        ("Logging Example", logging_example),
        ("Performance Example", performance_example),
        ("Configuration Example", configuration_example),
        ("Command Line Example", command_line_example),
        ("Advanced Example", advanced_example),
        ("Tutorial Example", tutorial_example),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            print()
    
    print("All examples completed!")
    print("\nFor more information, refer to the NLM documentation:")
    print("- Python bindings: python/README.md")
    print("- Command line interface: python/nlm_cli.py")
    print("- Logging system: python/pynlm_logging.py")
    print("- Visualization system: python/pynlm_visualization.py")
    print("- Configuration: python/configs/README.md")
    print("- Testing: python/tests/README.md")


if __name__ == "__main__":
    main()
