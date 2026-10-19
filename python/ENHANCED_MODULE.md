"""
NLM Enhanced Python Module
==========================

This is the enhanced Python module for NLM (Neural Learning Machine) with advanced features
including configuration management, brain operations, simulation control, neural profiling,
and more.

The enhanced module provides:
- AdvancedConfig: JSON/YAML configuration with validation and schema
- AdvancedBrain: Enhanced brain with advanced neural operations and monitoring
- SimulationController: Complex simulation orchestration and control
- NeuralProfiler: Detailed neural network profiling and analysis
- MemoryManager: Advanced memory management and consolidation
- DevelopmentController: Enhanced developmental control
- NeuromodulationManager: Advanced neuromodulation systems

Installation
============

Install the enhanced module with:

    pip install -e .

This will install both the basic NLM module and the enhanced module.

Basic Usage
===========

Import the enhanced module:

    from nlm_enhanced import AdvancedConfig, AdvancedBrain, SimulationController, NeuralProfiler

Create an enhanced configuration:

    config = AdvancedConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)

Create an enhanced brain:

    brain = AdvancedBrain(config)

Run a simulation:

    controller = SimulationController(brain)
    controller.run_episode(1000)

Profile the network:

    profiler = NeuralProfiler(brain)
    network_profile = profiler.profile_network()
    print(f"Network has {network_profile.total_neuron_count} neurons and {network_profile.total_synapse_count} synapses")

Advanced Usage
==============

### Configuration Management

The AdvancedConfig class provides enhanced configuration management with JSON/YAML support,
validation, and schema definitions:

    # Create configuration with schema
    config = AdvancedConfig()
    config.add_schema("brain.neuron_count", "int", "Number of neurons", [10, 10000])
    config.add_schema("brain.synapse_density", "float", "Synapse density", [0.0, 1.0])
    
    # Validate configuration
    if config.validate_schema():
        print("Configuration is valid")
    
    # Export to JSON
    json_config = config.export_to_json()
    
    # Import from JSON
    config.import_from_json(json_config)

### Brain Operations

The AdvancedBrain class provides enhanced brain operations including batch operations,
learning acceleration, memory consolidation, and advanced monitoring:

    # Batch operations
    brain.batch_inject_current([1, 2, 3, 4, 5], 50.0)
    
    # Pattern injection
    brain.pattern_injection(["Excitatory", "Inhibitory"], 30.0, 5)
    
    # Learning acceleration
    brain.enable_supervised_learning({1: 1.0, 2: 0.8, 3: 0.6}, 0.01)
    brain.enable_unsupervised_learning(0.01)
    brain.enable_reinforcement_learning(1.0)
    
    # Memory consolidation
    brain.consolidate_memory(1.0)
    brain.replay_memory(100, 0.5)
    
    # Performance monitoring
    stats = brain.get_stats()
    print(f"Performance score: {brain.get_performance_metrics():.3f}")
    print(f"Network report:\n{brain.get_benchmark_report()}")

### Simulation Control

The SimulationController class provides complex simulation orchestration with performance
monitoring, error handling, and advanced debugging capabilities:

    # Create simulation controller
    controller = SimulationController(brain)
    
    # Run simulation with monitoring
    def on_step(step, metrics):
        print(f"Step {step}: {metrics['firing_neurons']} firing neurons")
    
    controller.run_episode(1000, on_step_complete=on_step)
    
    # Get performance report
    print(controller.get_performance_report())
    
    # Save checkpoint
    controller.save_checkpoint()

### Neural Profiling

The NeuralProfiler class provides detailed neural network profiling and analysis:

    # Create profiler
    profiler = NeuralProfiler(brain)
    
    # Profile network
    network_profile = profiler.profile_network()
    print(f"Network has {network_profile.total_neuron_count} neurons")
    
    # Profile dynamics
    dynamics_profile = profiler.profile_dynamics()
    
    # Generate visualization data
    viz_data = profiler.generate_visualization_data("json")
    
    # Get validation report
    print(profiler.get_validation_report())

### Advanced Features

#### Memory Management

The MemoryManager class provides advanced memory management and consolidation:

    from nlm_enhanced import MemoryManager
    
    memory_manager = MemoryManager(brain)
    memory_manager.consolidate_working_memory(1.0)
    memory_stats = memory_manager.get_memory_stats()
    print(f"Memory usage: {memory_stats['total_memory_usage']:.2f} MB")

#### Development Control

The DevelopmentController class provides enhanced developmental control:

    from nlm_enhanced import DevelopmentController
    
    dev_controller = DevelopmentController(brain)
    dev_controller.set_developmental_stage("Adult")
    dev_controller.enable_critical_period(0.0, 1.0)
    dev_stats = dev_controller.get_development_stats()
    print(f"Development stage: {dev_stats['current_stage']}")

#### Neuromodulation Management

The NeuromodulationManager class provides advanced neuromodulation systems:

    from nlm_enhanced import NeuromodulationManager
    
    neuromod_manager = NeuromodulationManager(brain)
    neuromod_manager.enable_dopamine(1.0, 0.01)
    neuromod_manager.enable_curiosity(1.0, 0.1)
    neuromod_stats = neuromod_manager.get_neuromodulation_stats()
    print(f"Curiosity level: {neurom_stats['curiosity_level']:.3f}")

### Error Handling

The enhanced module provides comprehensive error handling with error reporting and recovery:

    controller = SimulationController(brain)
    controller.run_episode(100)
    
    if controller.has_errors():
        print("Errors occurred during simulation:")
        print(controller.get_error_report())
        # Handle errors
        for error in controller.get_errors():
            print(f"Error at step {error.step}: {error.error_message}")

### Event System

The enhanced module provides an event system for simulation events:

    def event_handler(data):
        print(f"Event occurred: {data}")
    
    controller.add_event_handler("simulation_complete", event_handler)
    controller.add_event("simulation_complete", {"step": 100, "reason": "normal"})
    controller.process_events()

### Performance Monitoring

The enhanced module provides comprehensive performance monitoring:

    controller = SimulationController(brain)
    
    # Enable performance monitoring
    controller.enable_profiling(True)
    controller.enable_benchmarking(True)
    
    # Run simulation
    controller.run_episode(100)
    
    # Get performance report
    print(controller.get_performance_report())

### File I/O

The enhanced module provides advanced file I/O capabilities:

    # Save brain state
    brain.save_advanced("brain_state.json", "json", {"description": "Brain state after training"})
    
    # Load brain state
    brain.load_advanced("brain_state.json", "json")
    
    # Create snapshot
    snapshot = brain.create_snapshot()
    
    # Restore from snapshot
    brain.restore_from_snapshot(snapshot)

### Configuration Validation

The enhanced module provides comprehensive configuration validation:

    config = AdvancedConfig()
    config.add_schema("brain.neuron_count", "int", [10, 10000])
    config.add_schema("brain.synapse_density", "float", [0.0, 1.0])
    
    # Validate configuration
    errors = config.validate()
    if errors:
        print("Validation errors:")
        for error in errors:
            print(f"  {error.key}: {error.message} ({error.severity})")

### Statistical Analysis

The enhanced module provides statistical analysis capabilities:

    config = AdvancedConfig()
    # Add time series data
    config.add_time_series("brain.neuron_count", 0.0, "1000")
    config.add_time_series("brain.neuron_count", 1.0, "1100")
    config.add_time_series("brain.neuron_count", 2.0, "1200")
    
    # Get correlations
    correlations = config.get_correlations()
    for corr in correlations:
        print(f"Correlation between {corr['key1']} and {corr['key2']}: {corr['correlation']:.3f}")

### Data Export/Import

The enhanced module provides comprehensive data export/import capabilities:

    # Export results
    results = controller.export_results("json")
    
    # Import results
    controller.import_results(results, "json")

### Backward Compatibility

The enhanced module maintains backward compatibility with the basic NLM module:

    from pynlm import createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain
    
    # Basic usage (still works)
    config = createDefaultConfig()
    brain = createBrain(config)
    world = createSimpleWorld()
    agent = createAgentBrain(brain)

### Installation from Source

To install the enhanced module from source:

    git clone https://github.com/nlm-project/nlm.git
    cd nlm
    pip install -e .

### Additional Resources

For more information:

- Basic NLM module: `pynlm` (imported as `import pynlm`)
- Enhanced NLM module: `nlm_enhanced` (imported as `from nlm_enhanced import ...`)
- Documentation: See `README.md` for basic usage
- Advanced documentation: See this file for advanced features
- Examples: See `examples/` directory for example usage

### Examples

See the `examples/` directory for example usage:

- `basic_usage.py`: Basic usage examples
- `advanced_usage.py`: Advanced usage examples
- `simulation_examples.py`: Simulation examples
- `analysis_examples.py`: Analysis examples
- `configuration_examples.py`: Configuration examples

### Support

For support with the enhanced module:

- Issues: Submit issues on GitHub
- Documentation: Check the documentation in this file
- Examples: Run the example scripts
- Support: Contact the NLM development team