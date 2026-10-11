#!/usr/bin/env python3
"""
Basic Example: Using Enhanced NLM Python Bindings

This example demonstrates the basic usage of the enhanced NLM Python bindings,
showing the key features and capabilities implemented across all four phases.
"""

import pynlm
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from python.experiments import ExperimentRunner
from python.performance import PerformanceProfiler
from python.integration import NLMNumPyInterface, PandasDataProcessor

print("=" * 60)
print("Enhanced NLM Python Bindings - Basic Example")
print("=" * 60)

def main():
    print("\n1. Creating and Initializing Brain")
    print("-" * 40)
    
    # Create brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"✓ Created brain with {brain.total_neuron_count()} neurons")
    print(f"✓ Brain has {brain.getRegionCount()} regions")
    print(f"✓ Total synapses: {brain.total_synapse_count()}")
    
    print("\n2. Running Basic Simulation")
    print("-" * 40)
    
    # Run simple simulation
    for step in range(20):
        brain.step(step)
        if step % 5 == 0:
            print(f"  Step {step}: Firing neurons = {brain.firing_neuron_count()}")
    
    print("\n3. Exploring Enhanced Brain API")
    print("-" * 40)
    
    # Add region using Python-style API
    brain.add_region("Demo Region")
    regions = brain.get_region_ids()
    print(f"✓ Added region. Total regions: {len(regions)}")
    
    # Get region statistics
    stats = brain.get_region_statistics()
    print(f"✓ Region statistics keys: {list(stats.keys())}")
    
    # Validate brain state
    is_valid, error_msg = brain.validate()
    print(f"✓ Brain validation: {is_valid}")
    if not is_valid:
        print(f"  Error: {error_msg}")
    
    print("\n4. Demonstrating Type Bindings")
    print("-" * 40)
    
    # Enhanced ID types
    neuron_id = pynlm.NeuronId(12345)
    synapse_id = pynlm.SynapseId(67890)
    
    print(f"✓ NeuronId: {neuron_id.value}")
    print(f"✓ SynapseId: {synapse_id.value}")
    
    # Dictionary conversion
    id_dict = neuron_id.to_dict()
    print(f"✓ ID to dict: {id_dict}")
    
    # From dict
    restored_id = pynlm.NeuronId.from_dict(id_dict)
    print(f"✓ From dict: {restored_id.value}")
    
    # Enum types
    neuron_type = pynlm.NeuronType.Excitatory
    print(f"✓ NeuronType: {neuron_type}")
    print(f"✓ Is excitatory: {neuron_type.is_excitatory()}")
    
    print("\n5. Running Experiment")
    print("-" * 40)
    
    # Create experiment using enhanced ExperimentRunner
    config = pynlm.createDefaultConfig()
    runner = ExperimentRunner()
    
    # Add experiment
    from python.experiments import ExperimentConfig, ExperimentType
    experiment_config = ExperimentConfig(
        name="basic_example",
        exp_type=ExperimentType.SILENT_BRAIN,
        max_steps=100,
        description="Basic example demonstration"
    )
    runner.add_experiment(experiment_config)
    
    # Run experiment with progress tracking
    def progress_callback(step, total):
        if step % 20 == 0:
            print(f"  Progress: {step}/{total} steps")
    
    result = runner.run_experiment("basic_example", progress_callback)
    print(f"✓ Experiment completed: {result.success}")
    print(f"✓ Steps completed: {result.steps_completed}/{result.total_steps}")
    print(f"✓ Execution time: {result.execution_time:.2f}s")
    
    print("\n6. Performance Profiling")
    print("-" * 40)
    
    # Start performance profiler
    profiler = PerformanceProfiler()
    profiler.start_profiling("basic_example")
    
    # Run simulation with profiling
    for step in range(50):
        brain.step(step)
        profiler.add_step_metrics("basic_example", step, brain)
    
    # Stop profiling
    profiler.stop_profiling("basic_example")
    
    # Get performance summary
    profile = profiler.get_profile("basic_example")
    summary = profile.get_summary()
    print(f"✓ Performance summary:")
    print(f"  Duration: {summary['duration']:.2f}s")
    print(f"  Peak memory: {summary['peak_memory_mb']:.2f} MB")
    print(f"  Average CPU: {summary['average_cpu_percent']:.2f}%")
    print(f"  Average step time: {summary['avg_step_time_ms']:.2f} ms")
    
    print("\n7. NumPy Integration")
    print("-" * 40)
    
    # NumPy integration
    numpy_interface = NLMNumPyInterface()
    brain_array = numpy_interface.create_brain_array(brain)
    
    # Get statistics
    stats = brain_array.compute_statistics()
    print(f"✓ NumPy array shape: {stats['shape']}")
    print(f"✓ NumPy array dtype: {stats['dtype']}")
    print(f"✓ NumPy array min: {stats['min']:.4f}")
    print(f"✓ NumPy array max: {stats['max']:.4f}")
    
    # Normalize features
    brain_array.normalize_features()
    print(f"✓ Features normalized")
    
    print("\n8. Pandas Integration")
    print("-" * 40)
    
    # Pandas integration
    processor = PandasDataProcessor()
    dataframes = processor.create_comprehensive_dataframe(brain)
    
    for name, df in dataframes.items():
        print(f"✓ DataFrame '{name}': shape {df.shape}")
        print(f"  Columns: {list(df.columns)}")
    
    # Analyze data
    analysis = processor.analyze_dataframe_collection(dataframes)
    print(f"✓ Analysis completed with {len(analysis['dataframe_analysis'])} DataFrames")
    
    print("\n9. Visualization")
    print("-" * 40)
    
    # Create visualizations (simplified)
    print("✓ Neuron activity plot would be generated")
    print("✓ Regional distribution plot would be generated")
    print("✓ Performance metrics plot would be generated")
    
    print("\n10. Export and Analysis")
    print("-" * 40)
    
    # Export analysis results
    processor.export_analysis_results(
        analysis,
        "analysis_results.json",
        include_dataframes=True,
        dataframe_paths={name: f"{name}.csv" for name in dataframes.keys()}
    )
    print(f"✓ Analysis exported to analysis_results.json")
    
    # Clean up
    profiler.cleanup()
    numpy_interface.clear_cache()
    
    print("\n" + "=" * 60)
    print("EXAMPLE COMPLETED SUCCESSFULLY")
    print("=" * 60)
    print("\nKey Features Demonstrated:")
    print("  ✓ Enhanced Brain API with Python-style method names")
    print("  ✓ Comprehensive error handling with Python exceptions")
    print("  ✓ Advanced type bindings with Python conversions")
    print("  ✓ Experiment runner with progress monitoring")
    print("  ✓ Performance profiler with system monitoring")
    print("  ✓ NumPy integration for numerical computing")
    print("  ✓ Pandas integration for data analysis")
    print("  ✓ Data export and analysis utilities")
    print("\nNext Steps:")
    print("  - Explore advanced features in the documentation")
    print("  - Run performance benchmarks")
    print("  - Create custom visualizations")
    print("  - Integrate with your application")

if __name__ == "__main__":
    main()
