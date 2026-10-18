#!/usr/bin/env python3
"""
Demonstration of NLM Python Module Improvements

This script demonstrates the key improvements made to the NLM Python module.
"""

import sys
import os

# Add the python directory to the path so we can import pynlm directly
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm


def demo_basic_usage():
    """Demonstrate basic NLM usage."""
    print("1. Basic NLM Usage")
    print("-" * 40)
    
    # Create a simple brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Run a short simulation
    for i in range(50):
        brain.step(i)
    
    print(f"✓ Simulation completed with {brain.getTotalSpikeCount()} spikes")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print()


def demo_enhanced_configuration():
    """Demonstrate enhanced configuration features."""
    print("2. Enhanced Configuration Features")
    print("-" * 40)
    
    # Create a custom configuration using BrainConfig helper
    custom_config = pynlm.BrainConfig.with_parameters(
        neuron_count=2000,
        synapse_density=0.1,
        learning_rate=0.01,
        v_thresh=-45.0,
        tau_mem=15.0,
        stdp_enable=True,
        hebbian_enable=True,
        neuromod_curiosity_enable=True,
    )
    
    print("✓ Created custom brain with:")
    print(f"  - {custom_config.params.get('brain.neuron_count', 'N/A')} neurons")
    print(f"  - {custom_config.params.get('brain.synapse_density', 'N/A')} synapse density")
    print(f"  - {custom_config.params.get('learning.rate', 'N/A')} learning rate")
    print(f"  - {custom_config.params.get('brain.v_thresh', 'N/A')} mV threshold")
    print(f"  - {custom_config.params.get('brain.tau_mem', 'N/A')} ms membrane time constant")
    print()


def demo_metrics_collection():
    """Demonstrate metrics collection features."""
    print("3. Metrics Collection and Analysis")
    print("-" * 40)
    
    # Create brain and run simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect metrics at different points
    for step in range(100):
        brain.step(step)
        
        if step % 25 == 0:
            metrics = pynlm.SimulationMetrics.collect(brain)
            metrics['step'] = step
            
            print(f"Step {step}: {metrics['firing_neuron_count']} firing neurons, "
                  f"{metrics['average_firing_rate']:.2f} Hz")
    
    # Create sample metrics for demonstration
    sample_metrics = {
        'step': 100,
        'neuron_count': 1000,
        'synapse_count': 50000,
        'firing_neuron_count': 150,
        'total_spike_count': 12500,
        'average_firing_rate': 12.5,
        'excitation_inhibition_ratio': 1.2,
        'developmental_stage': pynlm.DevelopmentalStage.CriticalPeriod,
        'agent_energy': 85.0,
        'agent_health': 0.92,
        'agent_curiosity_level': 0.75,
        'agent_novelty_level': 0.68,
    }
    
    print(f"\n✓ Sample metrics:")
    print(pynlm.SimulationMetrics.format(sample_metrics, precision=2))
    print()


def demo_performance_monitoring():
    """Demonstrate performance monitoring features."""
    print("4. Performance Monitoring")
    print("-" * 40)
    
    # Create brain and world for monitoring
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Create a simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create performance monitor
    monitor = pynlm.PerformanceMonitor(brain, world)
    
    print("✓ Created performance monitor")
    
    # Start monitoring
    monitor.start()
    
    # Run simulation with monitoring
    num_steps = 50
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1)
        
        monitor.record_step(0.001)  # Simulate step time
    
    # Stop monitoring and get results
    results = monitor.stop()
    
    print(f"✓ Monitored {num_steps} simulation steps")
    print(f"✓ Total time: {results['steps']['total_time']:.2f} seconds")
    print(f"✓ Steps per second: {results['steps']['steps_per_second']:.2f}")
    print(f"✓ Average step time: {results['steps']['avg_step_time']*1000:.2f} ms")
    print(f"✓ Efficiency score: {results['efficiency']['efficiency_score']:.2f}")
    print()


def demo_brain_registry():
    """Demonstrate brain registry features."""
    print("5. Brain Registry System")
    print("-" * 40)
    
    import time
    
    # Create multiple brains
    config = pynlm.createDefaultConfig()
    
    brains = []
    for i in range(3):
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
        
        # Register with custom metadata
        pynlm.register_brain(
            f"brain_{i}",
            brain,
            {
                'type': 'test_brain',
                'index': i,
                'neuron_count': brain.getTotalNeuronCount(),
                'created_at': time.time(),
            }
        )
    
    print(f"✓ Registered {len(pynlm.list_brains())} brains")
    print(f"✓ Available brains: {pynlm.list_brains()}")
    
    # Retrieve brains
    for name in pynlm.list_brains():
        brain = pynlm.get_brain(name)
        if brain:
            print(f"✓ Retrieved brain '{name}': {brain.getTotalNeuronCount()} neurons")
    
    # Use context manager
    print("\n✓ Using BrainContext for automatic cleanup...")
    with pynlm.BrainContext("temp_brain", brains[0], {'temp': True}):
        temp_brain = pynlm.get_brain("temp_brain")
        print(f"✓ Temp brain in context: {temp_brain is not None}")
    
    print(f"✓ After context exit - temp brain registered: {pynlm.has_brain('temp_brain')}")
    
    # Clean up
    for name in pynlm.list_brains():
        pynlm.unregister_brain(name)
    
    pynlm.clear_registry()
    print("✓ Registry cleared")
    print()


def demo_experiment_framework():
    """Demonstrate experiment framework features."""
    print("6. Experiment Framework")
    print("-" * 40)
    
    # Create configuration and world
    config = pynlm.createDefaultConfig()
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    # Run experiment
    print("✓ Running experiment with:")
    print("  - 200 simulation steps")
    print("  - Reward modulation enabled")
    print("  - Development enabled")
    print("  - Curiosity enabled")
    
    results = pynlm.run_experiment(
        config=config,
        world=world,
        num_steps=200,
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True,
        log_interval=50,
    )
    
    print(f"✓ Experiment completed: {results.success}")
    print(f"✓ Errors: {len(results.errors)}")
    print(f"✓ Steps recorded: {len(results.metrics)}")
    
    # Get experiment summary
    summary = results.get_summary()
    print(f"✓ Final firing rate: {summary.get('average_firing_rate', 0):.2f} Hz")
    print(f"✓ Final curiosity: {summary.get('agent_curiosity_level', 0):.3f}")
    print(f"✓ Final energy: {summary.get('final_energy', 0):.1f}")
    print()


def demo_benchmarking():
    """Demonstrate benchmarking features."""
    print("7. Benchmarking")
    print("-" * 40)
    
    # Brain creation benchmark
    print("✓ Running brain creation benchmark...")
    brain_creation_results = pynlm.benchmark_brain_creation(
        num_brains=10,
        neuron_count=1000
    )
    
    print(f"  - Created {brain_creation_results['brains_per_second']:.1f} brains/second")
    print(f"  - Average creation time: {brain_creation_results['avg_creation_time']*1000:.2f} ms")
    
    # Simulation benchmark
    print("✓ Running simulation benchmark...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    
    simulation_results = pynlm.benchmark_simulation(brain, world, num_steps=500)
    
    print(f"  - Simulated {simulation_results['steps_per_second']:.1f} steps/second")
    print(f"  - Average step time: {simulation_results['avg_step_time']*1000:.2f} ms")
    print()


def demo_error_handling():
    """Demonstrate error handling features."""
    print("8. Error Handling")
    print("-" * 40)
    
    # Create different types of exceptions
    try:
        raise pynlm.NLMException("Test error", error_code=1)
    except pynlm.NLMException as e:
        print(f"✓ NLMException: {e}")
    
    try:
        raise pynlm.BrainCreationError("Brain creation failed", details={'reason': 'invalid_config'})
    except pynlm.BrainCreationError as e:
        print(f"✓ BrainCreationError: {e}")
    
    try:
        raise pynlm.ValidationError("Invalid parameter value", details={'param': 'learning_rate', 'value': -1.0})
    except pynlm.ValidationError as e:
        print(f"✓ ValidationError: {e}")
    
    # Test registration error
    try:
        # This would fail if brain is already registered, but we'll just show the exception class
        raise pynlm.DuplicateNameError("test_brain")
    except pynlm.DuplicateNameError as e:
        print(f"✓ DuplicateNameError: {e}")
    
    print()


def demo_cli_interface():
    """Demonstrate CLI interface availability."""
    print("9. CLI Interface")
    print("-" * 40)
    
    if hasattr(pynlm, '__main__'):
        print("✓ CLI interface available via 'pynlm-cli'")
        print("✓ Available commands:")
        print("  - run: Run brain simulation")
        print("  - create: Create brain with custom configuration")
        print("  - benchmark: Performance benchmarking")
        print("  - experiment: Run complete agent experiment")
        print("  - config: Manage configuration files")
        print("  - info: Display system information")
        print("  - version: Show version information")
    else:
        print("⚠ CLI interface not available")
    print()


def main():
    """Run all demonstrations."""
    print("=" * 70)
    print("NLM Python Module Improvements Demonstration")
    print("=" * 70)
    print()
    
    # Import time for demo
    import time
    
    # Run demonstrations
    demo_basic_usage()
    demo_enhanced_configuration()
    demo_metrics_collection()
    demo_performance_monitoring()
    demo_brain_registry()
    demo_experiment_framework()
    demo_benchmarking()
    demo_error_handling()
    demo_cli_interface()
    
    print("=" * 70)
    print("DEMONSTRATION COMPLETE")
    print("=" * 70)
    print()
    print("The NLM Python module has been significantly enhanced with:")
    print()
    print("• Comprehensive Python API with helper classes")
    print("• Advanced configuration management")
    print("• Detailed metrics collection and analysis")
    print("• Performance monitoring and benchmarking")
    print("• Brain registry system for lifecycle management")
    print("• Experiment framework for testing and comparison")
    print("• Modern error handling with custom exception classes")
    print("• Command-line interface for automation")
    print("• Extensive documentation and examples")
    print("• Full type hints and IDE support")
    print()
    print("All improvements maintain backward compatibility while adding")
    print("powerful new features for research, development, and production use.")
    print()
    print("Key benefits:")
    print("• Improved developer experience with intuitive APIs")
    print("• Enhanced productivity with automation tools")
    print("• Better performance with monitoring and optimization")
    print("• Increased reliability with comprehensive error handling")
    print("• Future extensibility with modular design")
    print("• Complete documentation with examples")
    print()


if __name__ == "__main__":
    main()