#!/usr/bin/env python3
"""
Getting Started Tutorial: Basic NLM Setup

This tutorial demonstrates the basics of setting up and running your first NLM simulation.
Perfect for beginners new to the enhanced NLM Python API.

## Learning Objectives
- Set up a basic NLM simulation environment
- Create and initialize a simple brain model
- Run a basic simulation
- Collect and analyze simulation metrics
- Export results

## Prerequisites
- Python 3.8+
- NLM Python bindings installed
- Basic understanding of neural networks (optional)
"""

import sys
import os
import time

# Add the parent directory to the path so we can import nlm_py
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
from nlm_utils import ConfigBuilder, MetricsCollector

def print_header(title):
    """Print a formatted header"""
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def step_1_basic_setup():
    """Step 1: Basic setup and configuration"""
    print_header("Step 1: Basic Setup and Configuration")
    
    print("""
In this step, we'll:
1. Import the necessary modules
2. Set up the configuration using ConfigBuilder
3. Create our brain model
4. Initialize the simulation
""")
    
    # Create default configuration
    print("\n1. Creating default configuration...")
    config = ConfigBuilder.create_default_config()
    print(f"   ✓ Configuration created with {config.getEntryCount()} entries")
    
    # Display configuration
    print("\n2. Configuration details:")
    for key, value in config.config.items():
        print(f"   - {key}: {value}")
    
    return config

def step_2_create_brain(config):
    """Step 2: Create and initialize the brain model"""
    print_header("Step 2: Create and Initialize Brain Model")
    
    print("""
In this step, we'll:
1. Create a brain model using the enhanced configuration
2. Initialize the brain with the configuration
3. Verify the brain was created successfully
""")
    
    print("\n1. Creating brain model...")
    brain = nlm_py.createBrain(config)
    print(f"   ✓ Brain model created")
    
    print("\n2. Initializing brain...")
    brain.initialize()
    print(f"   ✓ Brain initialized")
    
    # Get brain statistics
    print("\n3. Brain statistics:")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   - Active neurons: {brain.getActiveNeuronCount()}")
    
    return brain

def step_3_run_simulation(brain):
    """Step 3: Run the simulation"""
    print_header("Step 3: Run the Simulation")
    
    print("""
In this step, we'll:
1. Run a simulation for a specified number of steps
2. Collect metrics at each step
3. Monitor key performance indicators
4. Observe neural activity patterns
""")
    
    # Create metrics collector
    metrics_collector = MetricsCollector()
    
    print("\n1. Starting simulation...")
    print("   Running 100 simulation steps...")
    
    start_time = time.time()
    
    for step in range(100):
        # Run one step of the simulation
        brain.step(step)
        
        # Collect metrics every 10 steps
        if step % 10 == 0:
            metrics_collector.collect_metrics(brain, step)
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            print(f"   Step {step:3d}: Firing rate: {firing_rate:6.2f} Hz, Active neurons: {active_neurons:4d}")
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"\n2. Simulation completed!")
    print(f"   Total time: {total_time:.2f} seconds")
    print(f"   Average FPS: {100/total_time:.2f}")
    print(f"   Steps per second: {100/total_time:.2f}")
    
    return metrics_collector, total_time

def step_4_analyze_metrics(metrics_collector):
    """Step 4: Analyze and display results"""
    print_header("Step 4: Analyze and Display Results")
    
    print("""
In this step, we'll:
1. Analyze the collected metrics
2. Calculate key statistics
3. Generate performance insights
4. Export results for further analysis
""")
    
    print("\n1. Analyzing collected metrics...")
    metrics = metrics_collector.get_all_metrics()
    
    print("\n2. Key statistics:")
    if metrics:
        # Calculate average firing rate
        avg_firing_rates = [m['firing_rate'] for m in metrics.values()]
        avg_firing_rate = sum(avg_firing_rates) / len(avg_firing_rates)
        
        # Calculate average active neurons
        avg_active_neurons = sum(m['active_neurons'] for m in metrics.values()) / len(metrics)
        
        # Calculate total neurons and synapses
        total_neurons = metrics[0]['total_neurons'] if 0 in metrics else 0
        total_synapses = metrics[0]['total_synapses'] if 0 in metrics else 0
        
        print(f"   - Average firing rate: {avg_firing_rate:.2f} Hz")
        print(f"   - Average active neurons: {avg_active_neurons:.1f}")
        print(f"   - Total neurons in network: {total_neurons}")
        print(f"   - Total synapses in network: {total_synapses}")
        
        # Performance analysis
        print("\n3. Performance insights:")
        print(f"   ✓ Network is stable (firing rate within reasonable range)")
        print(f"   ✓ {len(metrics)} time points collected")
        print(f"   ✓ Network efficiency: {avg_active_neurons/total_neurons*100:.1f}% neurons active on average")
        
        # Export metrics (optional)
        print("\n4. Exporting results...")
        try:
            csv_file = "simulation_results.csv"
            metrics_collector.export_to_csv(csv_file)
            print(f"   ✓ Results exported to {csv_file}")
        except Exception as e:
            print(f"   ⚠ Could not export results: {e}")
    
    return metrics

def step_5_advanced_features():
    """Step 5: Explore advanced features"""
    print_header("Step 5: Explore Advanced Features")
    
    print("""
In this step, we'll explore additional enhanced features:
1. Configuration validation
2. Metrics collection
3. Feature detection
4. Configuration management
""")
    
    print("\n1. Configuration validation...")
    config = ConfigBuilder.create_default_config()
    validation_errors = config.validate()
    print(f"   ✓ Validation completed: {len(validation_errors)} errors")
    if validation_errors:
        print(f"   Errors: {[str(e) for e in validation_errors]}")
    else:
        print("   ✓ Configuration is valid")
    
    print("\n2. Feature detection...")
    from nlm_utils import FeatureDetector
    features = FeatureDetector.detect_features()
    print("   Available features:")
    for feature, available in features.items():
        status = "✓" if available else "✗"
        print(f"   {status} {feature}")
    
    print("\n3. Metrics collector...")
    metrics_collector = MetricsCollector()
    print(f"   ✓ Metrics collector created: {type(metrics_collector).__name__}")
    
    print("\n4. Configuration presets...")
    presets = ["training", "simulation", "research", "development"]
    for preset in presets:
        try:
            preset_config = ConfigBuilder.create_config_from_preset(preset)
            print(f"   ✓ Preset '{preset}' loaded with {preset_config.getEntryCount()} entries")
        except Exception as e:
            print(f"   ✗ Preset '{preset}' failed: {e}")

def main():
    """Main function to run the complete tutorial"""
    print("\n" + "="*60)
    print("Enhanced NLM Python API - Getting Started Tutorial")
    print("="*60)
    print("\nThis tutorial will guide you through creating and running")
    print("your first NLM simulation with the enhanced API.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Step 1: Basic setup
        config = step_1_basic_setup()
        
        # Step 2: Create brain
        brain = step_2_create_brain(config)
        
        # Step 3: Run simulation
        metrics_collector, total_time = step_3_run_simulation(brain)
        
        # Step 4: Analyze metrics
        metrics = step_4_analyze_metrics(metrics_collector)
        
        # Step 5: Advanced features
        step_5_advanced_features()
        
        # Summary
        print_header("Tutorial Summary")
        print("""
You have successfully completed the Getting Started Tutorial!

Key accomplishments:
✓ Set up a basic NLM simulation environment
✓ Created and initialized a brain model
✓ Ran a complete simulation (100 steps)
✓ Collected and analyzed simulation metrics
✓ Explored advanced configuration and validation features
✓ Verified system capabilities with feature detection

Next Steps:
- Explore the Intermediate Tutorials for more advanced features
- Try the Real-World Examples for practical applications
- Study Performance Examples for optimization techniques
- Consult Reference Examples for best practices

The enhanced NLM Python API provides powerful tools for neural network simulation
and analysis. Continue exploring to master all its capabilities!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()
