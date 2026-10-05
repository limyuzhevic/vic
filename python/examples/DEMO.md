#!/usr/bin/env python3
"""
Enhanced NLM Python API Examples - Demonstration and Usage Guide

This file provides a comprehensive demonstration of the Enhanced NLM Python API Examples,
showing how to use the examples and demonstrating the complete capabilities of the API.

## Overview

This demonstration file shows:
1. How to use the examples directory
2. The complete structure and organization
3. Example usage patterns
4. Best practices for learning and development
5. Performance and optimization techniques
"""

import sys
import os
import time
import json

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
from nlm_utils import ConfigBuilder, ConfigValidator, MetricsCollector

def print_header(title):
    """Print a formatted header"""
    print(f"\n{'='*80}")
    print(f"{title}")
    print(f"{'='*80}")

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'-'*80}")
    print(f"{title}")
    print(f"{'-'*80}")

def demonstrate_examples_structure():
    """Demonstrate the examples directory structure"""
    print_section("Enhanced NLM Python API Examples - Structure Demonstration")
    
    print("""
This demonstration shows the complete structure of the Enhanced NLM Python API Examples.

DIRECTORY STRUCTURE:
┌─────────────────────────────────────────────────────────────────────┐
│ python/examples/                                                     │
├─────────────────────────────────────────────────────────────────────┤
│ README.md                 - Main overview and usage instructions      │
│ STRUCTURE.md              - Detailed directory structure              │
│ PROJECT_SUMMARY.md       - Complete project documentation            │
├─────────────────────────────────────────────────────────────────────┤
│ getting_started/                                                      │
│   └─ basic_setup.py     - Getting started tutorial                    │
├─────────────────────────────────────────────────────────────────────┤
│ intermediate/                                                         │
│   └─ configuration_management.py - Intermediate configuration examples│
├─────────────────────────────────────────────────────────────────────┤
│ advanced/                                                            │
│   └─ complex_networks.py       - Complex network architectures         │
│   └─ production_systems.py     - Production-ready systems             │
├─────────────────────────────────────────────────────────────────────┤
│ real_world/                                                          │
│   └─ cognitive_modeling.py       - Cognitive process simulation        │
│   └─ motor_control.py            - Motor control systems                │
│   └─ sensory_processing.py       - Sensory processing                   │
│   └─ biological_simulation.py   - Biological neural systems             │
├─────────────────────────────────────────────────────────────────────┤
│ performance/                                                         │
│   └─ memory_optimization.py      - Memory optimization                 │
│   └─ parallel_processing.py      - Parallel processing                 │
│   └─ benchmarking.py             - Performance benchmarking            │
│   └─ production_optimization.py - Production optimization              │
├─────────────────────────────────────────────────────────────────────┤
│ learning/                                                            │
│   └─ plasticity_learning.py       - Neural plasticity learning           │
│   └─ adaptive_systems.py         - Adaptive system design               │
│   └─ developmental_processes.py - Developmental processes              │
├─────────────────────────────────────────────────────────────────────┤
│ integration/                                                         │
│   └─ jupyter_integration.py      - Jupyter notebook integration        │
│   └─ ml_frameworks_integration.py - ML framework integration             │
│   └─ visualization_tools.py      - Visualization tools integration     │
├─────────────────────────────────────────────────────────────────────┤
│ features/                                                            │
│   └─ configuration_validation.py - Configuration validation              │
│   └─ metrics_collection.py       - Metrics collection                   │
│   └─ export_import.py            - Data export/import                   │
│   └─ template_system.py          - Template system                      │
│   └─ preset_system.py            - Preset system                        │
├─────────────────────────────────────────────────────────────────────┤
│ reference/                                                           │
│   └─ api_reference.py            - API usage patterns                    │
│   └─ migration_examples.py       - Migration examples                   │
│   └─ best_practices.md            - Best practices documentation         │
│   └─ common_patterns.md          - Common patterns documentation          │
│   └─ troubleshooting.md          - Troubleshooting documentation          │
└─────────────────────────────────────────────────────────────────────┘
""")
    
    print("\nKEY FEATURES COVERED:")
    features = [
        ("Configuration Management", "Templates, validation, presets, export/import"),
        ("Neural Network Simulation", "Brain creation, initialization, complex networks"),
        ("Performance Optimization", "Memory, parallel processing, benchmarking"),
        ("Learning and Development", "Plasticity, adaptation, development"),
        ("Integration and Compatibility", "Jupyter, ML frameworks, visualization"),
        ("Real-World Applications", "Cognitive, motor, sensory, biological systems")
    ]
    
    for feature, description in features:
        print(f"   ✓ {feature}:")
        print(f"     {description}")
    
    print(f"\nTOTAL EXAMPLES: {count_python_files('python/examples')}")
    print(f"LEARNING PATHS: 10+")
    print(f"SKILL LEVELS: Beginner → Intermediate → Advanced")

def count_python_files(directory):
    """Count Python files in directory and subdirectories"""
    count = 0
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.py'):
                count += 1
    return count

def demonstrate_basic_example():
    """Demonstrate a basic example"""
    print_section("Basic Example Demonstration - Getting Started")
    
    print("""
This demonstration shows the basic NLM simulation using the enhanced API.

PROBLEM SOLVED:
   Create a simple neural network simulation with basic setup.

SOLUTION APPROACH:
   1. Create configuration using ConfigBuilder
   2. Create brain with configuration
   3. Initialize brain
   4. Run simulation
   5. Collect metrics

CODE IMPLEMENTATION:
""")
    
    # Create basic configuration
    print("\n1. Creating Configuration:")
    config = ConfigBuilder.create_default_config()
    config.set("neuron_count", 500)
    config.set("learning_rate", 0.03)
    print(f"   ✓ Configuration created: {config.getEntryCount()} entries")
    
    # Create brain
    print("\n2. Creating Brain:")
    brain = nlm_py.createBrain(config)
    brain.initialize()
    print(f"   ✓ Brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    print("\n3. Running Simulation:")
    metrics_collector = MetricsCollector()
    
    for step in range(20):
        brain.step(step)
        metrics_collector.collect_metrics(brain, step)
        
        if step % 5 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            print(f"   Step {step}: {firing_rate:.2f} Hz, {active_neurons} active neurons")
    
    # Get results
    print("\n4. Simulation Results:")
    metrics = metrics_collector.get_all_metrics()
    final_firing_rate = brain.getAverageFiringRate()
    final_active_neurons = brain.getActiveNeuronCount()
    
    print(f"   ✓ Final firing rate: {final_firing_rate:.2f} Hz")
    print(f"   ✓ Final active neurons: {final_active_neurons}")
    print(f"   ✓ Total simulation time: {len(metrics)} time points")
    
    # Export metrics
    print("\n5. Exporting Results:")
    try:
        csv_file = "basic_example_results.csv"
        metrics_collector.export_to_csv(csv_file)
        print(f"   ✓ Results exported to {csv_file}")
        
        # Clean up
        os.remove(csv_file)
        print(f"   ✓ Cleaned up {csv_file}")
    except Exception as e:
        print(f"   ⚠ Export failed: {e}")
    
    print(f"\n   ✓ Basic example completed successfully!")

def demonstrate_advanced_example():
    """Demonstrate an advanced example"""
    print_section("Advanced Example Demonstration - Configuration Management")
    
    print("""
This demonstration shows advanced NLM simulation with comprehensive configuration.

PROBLEM SOLVED:
   Create a neural network simulation with advanced configuration features.

SOLUTION APPROACH:
   1. Create advanced configuration with validation
   2. Add validation schemas
   3. Validate configuration
   4. Create brain with advanced features
   5. Run simulation with metrics collection
   6. Export and analyze results

CODE IMPLEMENTATION:
""")
    
    # Create advanced configuration
    print("\n1. Creating Advanced Configuration:")
    
    config = ConfigBuilder.create_training_config()
    config.set("neuron_count", 1500)
    config.set("plasticity_learning_rate", 0.04)
    config.set("simulation_timestep", 0.0005)
    config.set("log_level", "INFO")
    config.set("enable_profiling", True)
    
    print("   ✓ Advanced configuration created with:")
    for key, value in config.config.items():
        print(f"     - {key}: {value}")
    
    # Add validation schema
    print("\n2. Adding Validation Schema:")
    
    schema = ConfigValidator.create_schema(
        key="neuron_count",
        type="int",
        description="Number of neurons in the network",
        min_value=100,
        max_value=10000,
        required=True
    )
    config.add_schema(schema)
    
    print("   ✓ Validation schema added")
    
    # Validate configuration
    print("\n3. Validating Configuration:")
    
    errors = config.validate()
    if errors:
        print(f"   ⚠ Validation errors ({len(errors)}):")
        for error in errors:
            print(f"     - {error}")
    else:
        print("   ✓ Configuration is valid")
    
    # Create brain
    print("\n4. Creating Brain:")
    
    brain = nlm_py.createBrain(config)
    brain.initialize()
    
    print(f"   ✓ Brain created: {brain.getTotalNeuronCount()} neurons")
    print(f"   ✓ Brain has: {brain.getTotalSynapseCount()} synapses")
    
    # Run advanced simulation
    print("\n5. Running Advanced Simulation:")
    
    metrics_collector = MetricsCollector()
    
    for step in range(50):
        brain.step(step)
        metrics_collector.collect_metrics(brain, step)
        
        if step % 10 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            total_neurons = brain.getTotalNeuronCount()
            stability = min(1.0, (firing_rate / 50))  # Stability metric
            
            print(f"   Step {step}: FR: {firing_rate:6.2f} Hz, "
                  f"Active: {active_neurons:4d}, "
                  f"Stability: {stability:.2f}, "
                  f"Efficiency: {active_neurons/total_neurons*100:.1f}%")
    
    # Analyze results
    print("\n6. Analyzing Results:")
    
    metrics = metrics_collector.get_all_metrics()
    if metrics:
        avg_firing_rate = sum(m['firing_rate'] for m in metrics.values()) / len(metrics)
        avg_active_neurons = sum(m['active_neurons'] for m in metrics.values()) / len(metrics)
        total_neurons = metrics[0]['total_neurons']
        
        print(f"   ✓ Average firing rate: {avg_firing_rate:.2f} Hz")
        print(f"   ✓ Average active neurons: {avg_active_neurons:.1f}")
        print(f"   ✓ Network efficiency: {avg_active_neurons/total_neurons*100:.1f}%")
        print(f"   ✓ Performance score: {(avg_firing_rate/50)*100:.1f}/100")
        
        # Export results
        print("\n7. Exporting Results:")
        try:
            json_file = "advanced_example_results.json"
            
            # Create comprehensive results data
            results = {
                "simulation_summary": {
                    "total_neurons": total_neurons,
                    "total_synapses": metrics[0]['total_synapses'],
                    "final_firing_rate": brain.getAverageFiringRate(),
                    "final_active_neurons": brain.getActiveNeuronCount(),
                    "steps_completed": len(metrics)
                },
                "performance_metrics": {
                    "average_firing_rate": avg_firing_rate,
                    "average_active_neurons": avg_active_neurons,
                    "network_efficiency": avg_active_neurons/total_neurons*100,
                    "performance_score": (avg_firing_rate/50)*100
                },
                "simulation_data": metrics
            }
            
            with open(json_file, 'w') as f:
                json.dump(results, f, indent=2)
            
            print(f"   ✓ Results exported to {json_file}")
            
            # Clean up
            os.remove(json_file)
            print(f"   ✓ Cleaned up {json_file}")
            
        except Exception as e:
            print(f"   ⚠ Export failed: {e}")
    
    print(f"\n   ✓ Advanced example completed successfully!")

def demonstrate_integration_example():
    """Demonstrate an integration example"""
    print_section("Integration Example Demonstration - Jupyter Integration")
    
    print("""
This demonstration shows how to integrate NLM with Jupyter notebooks for interactive visualization.

PROBLEM SOLVED:
   Create an interactive Jupyter notebook for neural network simulation.

SOLUTION APPROACH:
   1. Create configuration suitable for Jupyter
   2. Set up simulation with metrics collection
   3. Generate data for visualization
   4. Prepare for interactive analysis

CODE IMPLEMENTATION (for Jupyter):
""")
    
    # Create Jupyter-compatible configuration
    print("\n1. Creating Jupyter-Ready Configuration:")
    
    jupyter_config = ConfigBuilder.create_default_config()
    jupyter_config.set("neuron_count", 800)
    jupyter_config.set("plasticity_learning_rate", 0.025)
    jupyter_config.set("simulation_timestep", 0.0003)
    jupyter_config.set("log_level", "WARNING")  # Reduce logging for Jupyter
    
    print("   ✓ Jupyter configuration created")
    
    # Create Jupyter simulation
    print("\n2. Running Jupyter-Compatible Simulation:")
    
    brain = nlm_py.createBrain(jupyter_config)
    brain.initialize()
    
    metrics_collector = MetricsCollector()
    
    jupyter_data = {
        'step_numbers': [],
        'firing_rates': [],
        'active_neurons': [],
        'total_neurons': [],
        'total_synapses': [],
        'performance_scores': []
    }
    
    for step in range(40):
        brain.step(step)
        metrics_collector.collect_metrics(brain, step)
        
        if step % 5 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            total_neurons = brain.getTotalNeuronCount()
            total_synapses = brain.getTotalSynapseCount()
            performance_score = (firing_rate / 50) * 100 * (active_neurons / total_neurons)
            
            jupyter_data['step_numbers'].append(step)
            jupyter_data['firing_rates'].append(firing_rate)
            jupyter_data['active_neurons'].append(active_neurons)
            jupyter_data['total_neurons'].append(total_neurons)
            jupyter_data['total_synapses'].append(total_synapses)
            jupyter_data['performance_scores'].append(performance_score)
            
            print(f"   Step {step}: FR: {firing_rate:6.2f} Hz, "
                  f"Active: {active_neurons:4d}, "
                  f"Perf: {performance_score:5.1f}/100")
    
    # Generate Jupyter-ready data
    print("\n3. Generating Jupyter-Ready Data:")
    
    jupyter_data_ready = {
        "metadata": {
            "title": "Neural Network Simulation",
            "description": "Interactive neural network simulation with enhanced API",
            "author": "Enhanced NLM Python API",
            "date": time.strftime("%Y-%m-%d %H:%M:%S"),
            "version": "2.0"
        },
        "simulation_data": jupyter_data,
        "configuration": {
            "neuron_count": jupyter_config.config.get("neuron_count"),
            "learning_rate": jupyter_config.config.get("plasticity_learning_rate"),
            "timestep": jupyter_config.config.get("simulation_timestep")
        },
        "analysis_results": {
            "final_firing_rate": brain.getAverageFiringRate(),
            "final_active_neurons": brain.getActiveNeuronCount(),
            "total_steps": len(jupyter_data['step_numbers']),
            "average_performance": sum(jupyter_data['performance_scores']) / len(jupyter_data['performance_scores'])
        }
    }
    
    print("   ✓ Jupyter-ready data generated")
    
    # Export Jupyter data
    print("\n4. Exporting Jupyter Data:")
    
    try:
        jupyter_data_file = "jupyter_simulation_data.json"
        
        with open(jupyter_data_file, 'w') as f:
            json.dump(jupyter_data_ready, f, indent=2)
        
        print(f"   ✓ Jupyter data exported to {jupyter_data_file}")
        
        # Generate Jupyter code snippet
        jupyter_code = """
import json
import matplotlib.pyplot as plt
import numpy as np

# Load Jupyter simulation data
with open('jupyter_simulation_data.json', 'r') as f:
    data = json.load(f)

# Extract data
sim_data = data['simulation_data']
steps = sim_data['step_numbers']
firing_rates = sim_data['firing_rates']
active_neurons = sim_data['active_neurons']
performance_scores = sim_data['performance_scores']

# Create interactive visualizations
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 10))

# Plot 1: Firing rate over time
ax1.plot(steps, firing_rates, 'b-', linewidth=2, marker='o', markersize=4)
ax1.set_xlabel('Simulation Step')
ax1.set_ylabel('Firing Rate (Hz)')
ax1.set_title('Neural Activity Over Time')
ax1.grid(True, alpha=0.3)
ax1.set_ylim(0, max(firing_rates) * 1.1)

# Plot 2: Active neurons over time
ax2.plot(steps, active_neurons, 'r-', linewidth=2, marker='s', markersize=4)
ax2.set_xlabel('Simulation Step')
ax2.set_ylabel('Active Neurons')
ax2.set_title('Active Neurons Over Time')
ax2.grid(True, alpha=0.3)
ax2.set_ylim(0, max(active_neurons) * 1.1)

# Plot 3: Performance score over time
ax3.plot(steps, performance_scores, 'g-', linewidth=2, marker='^', markersize=4)
ax3.set_xlabel('Simulation Step')
ax3.set_ylabel('Performance Score')
ax3.set_title('Simulation Performance Over Time')
ax3.grid(True, alpha=0.3)
ax3.set_ylim(0, max(performance_scores) * 1.1)

plt.tight_layout()
plt.show()

# Interactive widget code for Jupyter
print("Interactive Widgets Code:")
print("""
# In Jupyter notebook:
from ipywidgets import interact, widgets
import matplotlib.pyplot as plt

# Create interactive controls
step_range = widgets.IntSlider(min=0, max=max(steps), value=max(steps)//2, description='Step')
neuron_range = widgets.IntSlider(min=100, max=2000, value=800, description='Neuron Count')
learning_rate = widgets.FloatSlider(min=0.001, max=0.1, value=0.025, description='Learning Rate')

def update_plot(selected_step, neuron_count, lr):
    # Filter data for selected step
    step_data = [d for d in sim_data['step_numbers'] if abs(d - selected_step) < 10]
    fr_data = [d for d in sim_data['firing_rates'] if d in step_data]
    act_data = [d for d in sim_data['active_neurons'] if d in step_data]
    perf_data = [d for d in sim_data['performance_scores'] if d in step_data]
    
    plt.figure(figsize=(15, 5))
    plt.plot(step_data, fr_data, 'b-o', label='Firing Rate')
    plt.plot(step_data, act_data, 'r-s', label='Active Neurons')
    plt.plot(step_data, perf_data, 'g-^', label='Performance Score')
    plt.xlabel('Step')
    plt.ylabel('Value')
    plt.title(f'Neural Network Simulation - Step {selected_step}')
    plt.legend()
    plt.grid(True)
    plt.show()

# Display interactive plot
interact(update_plot, selected_step=step_range, neuron_count=neuron_range, learning_rate=learning_rate)
""")
        
        print("\n   ✓ Jupyter code snippet generated")
        
        # Clean up
        os.remove(jupyter_data_file)
        print(f"   ✓ Cleaned up {jupyter_data_file}")
        
    except Exception as e:
        print(f"   ⚠ Jupyter integration failed: {e}")
    
    print(f"\n   ✓ Jupyter integration example completed successfully!")

def main():
    """Main function to run the demonstration"""
    print_header("Enhanced NLM Python API Examples - Demonstration and Usage Guide")
    print("\nThis demonstration showcases the complete Enhanced NLM Python API Examples,")
    print("demonstrating how to use the examples and highlighting key features.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Show structure
        demonstrate_examples_structure()
        
        # Show basic example
        demonstrate_basic_example()
        
        # Show advanced example
        demonstrate_advanced_example()
        
        # Show integration example
        demonstrate_integration_example()
        
        # Summary
        print_header("Demonstration Summary")
        print("""
You have successfully completed the Enhanced NLM Python API Examples Demonstration!

Key accomplishments:
✓ Demonstrated complete directory structure
✓ Showed basic NLM simulation example
✓ Demonstrated advanced configuration management
✓ Demonstrated Jupyter integration capabilities
✓ Provided comprehensive usage guidance

What Was Demonstrated:
1. ✓ Complete examples directory structure
2. ✓ Basic NLM simulation with configuration
3. ✓ Advanced configuration with validation
4. ✓ Jupyter notebook integration
5. ✓ Data export and visualization

Benefits:
✓ Understanding of all available examples
✓ Practical usage patterns
✓ Integration capabilities
✓ Performance optimization techniques
✓ Real-world application scenarios

Next Steps:
- Explore specific examples in their directories
- Run examples to understand their functionality
- Create custom extensions based on examples
- Integrate examples into your projects
- Share examples with the community

The Enhanced NLM Python API Examples provide a comprehensive foundation for mastering
neural network simulation and analysis using the enhanced API!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()
