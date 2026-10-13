import pynlm
import random
import time
import py

def main():
    """Advanced features example demonstrating debugging, visualization, and profiling."""
    print("=== Advanced Features Example ===")
    
    # Create brain with dense template for complex simulation
    print("1. Creating dense brain template...")
    brain = pynlm.create_brain_from_template("dense")
    brain.initialize()
    
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # === DEBUG MONITOR DEMONSTRATION ===
    print("\n2. Creating debug monitor...")
    debug_monitor = pynlm.create_debug_monitor(brain)
    
    print("   Running simulation with debug monitoring...")
    start_time = time.time()
    
    for step in range(150):
        # Generate complex sensory patterns
        vision = pynlm.Vision(80, 60, 3)
        vision_data = [random.uniform(0, 1) for _ in range(80 * 60 * 3)]
        vision.setData(vision_data)
        
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        
        brain.receiveSensoryInput(percept)
        brain.step(step, step * 0.001)
        
        # Log every 25 steps
        if step % 25 == 0:
            debug_monitor.log_step()
            print(f"     Step {step:3d} completed")
    
    end_time = time.time()
    print(f"   Debug monitoring completed in {end_time - start_time:.2f} seconds")
    
    # Display debug summary
    print("\n3. Debug summary:")
    summary = debug_monitor.get_summary()
    for key, value in summary.items():
        if isinstance(value, float):
            print(f"   {key:25s}: {value:10.4f}")
        else:
            print(f"   {key:25s}: {value:10}")
    
    # === VISUALIZER DEMONSTRATION ===
    print("\n4. Creating visualizer...")
    visualizer = pynlm.create_visualizer(brain)
    
    print("   Generating network report...")
    visualizer.print_network_summary()
    visualizer.print_plasticity_summary()
    visualizer.print_development_summary()
    visualizer.print_neuromodulation_summary()
    visualizer.print_memory_summary()
    
    print("\n   Exporting visualization data...")
    visualizer.export_visualization_data()
    
    # === PROFILING DEMONSTRATION ===
    print("\n5. Creating profiler...")
    profiler = pynlm.create_profiler()
    
    print("   Running performance-profiled simulation...")
    
    # Simulate some complex operations with profiling
    profiler.log_event("simulation_start")
    
    for step in range(50):
        # Complex operations that generate profiling data
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0)
        brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 5.0)
        
        brain.step(step, step * 0.001)
        
        if step % 10 == 0:
            profiler.increment_step()
            profiler.log_event(f"processing_step_{step}")
    
    profiler.log_event("simulation_end")
    
    # Get performance report
    print("\n6. Performance analysis:")
    report = profiler.get_profile_report()
    
    print(f"   Total events: {report['total_events']}")
    print(f"   Total steps: {report['total_steps']}")
    print(f"   Total time: {report['total_time_seconds']:.2f} seconds")
    
    if 'events_per_second' in report:
        print(f"   Events per second: {report['events_per_second']:.1f}")
    
    if 'slowest_event' in report:
        print(f"   Slowest event: {report['slowest_event']} "
              f"({report['slowest_event_duration']:.4f}s)")
    
    # Generate detailed performance analysis
    print("\n   Generating detailed performance analysis...")
    profiler.generate_performance_analysis()
    
    # === STATE MANAGEMENT DEMONSTRATION ===
    print("\n7. Creating state saver...")
    state_saver = pynlm.create_state_saver("advanced_example_checkpoint.json")
    
    print("   Demonstrating state save/load...")
    
    # Get current brain state
    current_stats = pynlm.compute_neural_stats(brain)
    print(f"   Current stats keys: {list(current_stats.keys())}")
    
    # Save state
    state_saver.save_state(current_stats)
    
    # Load state
    loaded_state = state_saver.load_state()
    print(f"   Loaded state keys: {list(loaded_state.keys())}")
    
    # Demonstrate auto-save functionality
    print("\n8. Testing auto-save functionality...")
    state_saver.set_auto_save(True, interval=5.0)  # Auto-save every 5 seconds
    
    print("   Auto-save enabled (interval: 5.0 seconds)")
    print("   (In a real application, this would automatically save state)")
    
    # === DATA PROCESSING DEMONSTRATION ===
    print("\n9. Data processing utilities:")
    
    # Serialize and deserialize data
    print("   Testing data serialization...")
    
    # Create test data
    test_data = {
        'neurons': [{'id': 1, 'type': 'excitatory', 'firing': True}],
        'synapses': [{'id': 1, 'weight': 0.5, 'type': 'excitatory'}],
        'metadata': {'simulation_id': pynlm.generate_unique_id()}
    }
    
    # Convert to JSON-serializable format
    json_serializable = pynlm.convert_to_json(test_data)
    print(f"   JSON serialization successful")
    
    # Reverse conversion (basic)
    from_json = pynlm.from_json(json_serializable)
    print(f"   JSON deserialization successful")
    
    # Batch operations
    print("\n10. Batch operations:")
    brains = [brain]  # In real usage, you'd have multiple brains
    print(f"   Batch synchronizing {len(brains)} brain(s)...")
    
    # Parallel-like step execution
    pynlm.batch_synchronize(brains, 0.001)
    
    # Data filtering
    print("   Testing data filtering...")
    sample_data = {'a': 1, 'b': 2, 'c': 3, 'd': 4}
    
    def is_even(value):
        return value % 2 == 0
    
    filtered = pynlm.dict_filter(sample_data, lambda k, v: is_even(v))
    print(f"   Filtered data (even values only): {filtered}")
    
    # === ERROR HANDLING DEMONSTRATION ===
    print("\n11. Error handling demonstration:")
    
    # Safe function execution
    def safe_simulation_function(brain, steps):
        print(f"   Running safe simulation for {steps} steps...")
        for step in range(steps):
            brain.step(step, step * 0.001)
        print(f"   Simulation completed successfully")
        return True
    
    print("   Executing safe function...")
    result = pynlm.safe_execute(safe_simulation_function, brain, 20)
    print(f"   Safe execution result: {result}")
    
    # Error recovery demonstration
    print("\n12. Recovery demonstration:")
    print("   (In real usage, this would handle corrupted or incomplete data)")
    
    # Example of handling invalid inputs
    try:
        invalid_config = pynlm.create_config_with_values({'invalid_key': 'not_a_number'})
        print("   Config created (may not work as expected)")
    except Exception as e:
        print(f"   Error caught: {type(e).__name__}: {e}")
    
    print("\n=== Advanced Features Example Complete ===")
    print("\nSummary of advanced features demonstrated:")
    print("  ✓ Debug monitoring for neural activity tracking")
    print("  ✓ Visualization tools for network analysis")
    print("  ✓ Performance profiling and bottleneck analysis")
    print("  ✓ State management with checkpointing")
    print("  ✓ Data processing and serialization utilities")
    print("  ✓ Safe function execution with error handling")
    print("  ✓ Configuration validation and recovery")

if __name__ == "__main__":
    main()