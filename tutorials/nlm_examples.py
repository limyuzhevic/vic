#!/usr/bin/env python3
"""
NLM Usage Examples and Tutorials
===========================

This file contains examples and tutorials for using NLM (Neural Learning Machine)
with its enhanced features and capabilities.
"""

import os
import sys
import subprocess
import json
import time
from pathlib import Path

# Add the src directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

def print_header(title):
    """Print a formatted header."""
    print(f"\n{'=' * 60}")
    print(f"{title}")
    print(f"{'=' * 60}")

def print_example(title, description, command):
    """Print an example with title, description, and command."""
    print(f"\n{title}")
    print(f"{'-' * 40}")
    print(f"Description: {description}")
    print(f"Command: {command}")
    print(f"{'-' * 40}")

def example_1_basic_usage():
    """Example 1: Basic NLM usage."""
    print_header("Example 1: Basic NLM Usage")
    print("This example shows how to run NLM with basic configuration.")
    print("\nPrerequisites:")
    print("- NLM must be built and installed")
    print("- Configuration file: configs/enhanced.cfg")
    
    print("\nCommand:")
    print("> nlm")
    print("\nExpected output:")
    print("- Banner and configuration summary")
    print("- Connectivity test results")
    print("- Plasticity learning experiment")
    print("- STDP verification")
    print("- Final brain status")
    
    print("\nKey features demonstrated:")
    print("- Standard experiment suite")
    print("- Basic neural connectivity testing")
    print("- Plasticity learning demonstration")
    print("- STDP verification")

def example_2_enhanced_logging():
    """Example 2: Enhanced logging usage."""
    print_header("Example 2: Enhanced Logging Usage")
    print("This example shows how to use NLM's enhanced logging features.")
    
    print("\nCommand with enhanced logging:")
    print("> nlm --enhanced-logging --format json")
    
    print("\nFeatures demonstrated:")
    print("- Multi-level logging (TRACE, DEBUG, INFO, WARN, ERROR, CRIT, FATAL)")
    print("- JSON output format for structured logging")
    print("- Asynchronous logging for performance")
    print("- Log search and analysis")
    
    print("\nAdditional commands:")
    print("> nlm --enhanced-logging --format colored")
    print("> nlm --enhanced-logging --format csv")
    print("> nlm --enhanced-logging --format structured")
    print("> nlm --log-file custom.log")
    print("> nlm --async-logging")
    print("> nlm --structured-logging")

def example_3_custom_experiments():
    """Example 3: Custom experiments."""
    print_header("Example 3: Custom Experiments")
    print("This example shows how to run custom experiments.")
    
    print("\nRunning specific experiments:")
    print("> nlm --connectivity")
    print("> nlm --plasticity")
    print("> nlm --stdp")
    
    print("\nCustom experiment mode:")
    print("> nlm --mode custom --steps 2000")
    print("> nlm --mode custom --timestep 0.0005 --neurons 1000")
    
    print("\nExperiment registration:")
    print("To create a custom experiment:")
    print("1. Write experiment function")
    print("2. Register with ExperimentRegistry")
    print("3. Run with --mode custom_experiment_name")
    
    print("\nExample experiment code:")
    print("""
void myCustomExperiment(std::shared_ptr<Brain> brain, const CommandLineParser::Config& config) {
    infoEnhanced("Running custom experiment", "", 0, "myCustomExperiment", "custom");
    
    // Your experiment logic here
    for (int step = 0; step < config.customSteps; ++step) {
        // Your code
    }
}

// Register experiment
ExperimentRegistry::registerExperiment("my_custom", myCustomExperiment);
""")

def example_4_performance_monitoring():
    """Example 4: Performance monitoring."""
    print_header("Example 4: Performance Monitoring")
    print("This example shows performance monitoring and benchmarking.")
    
    print("\nPerformance benchmarking:")
    print("> nlm --performance-monitoring")
    print("> nlm --benchmark-all")
    print("> nlm --benchmark spike_generation")
    
    print("\nBenchmark results export:")
    print("> nlm --export-benchmarks results.csv")
    print("> nlm --export-benchmarks results.json")
    
    print("\nPerformance profiler:")
    print("> nlm --profiler")
    print("> nlm --profiler-functions")
    
    print("\nPerformance metrics:")
    print("- Throughput (operations per second)")
    print("- Latency (time per operation)")
    print("- Memory usage")
    print("- CPU usage")
    print("- Accuracy")
    print("- Efficiency")

def example_5_configuration_management():
    """Example 5: Configuration management."""
    print_header("Example 5: Configuration Management")
    print("This example shows configuration management features.")
    
    print("\nConfiguration files:")
    print("- configs/enhanced.cfg (detailed configuration)")
    print("- configs/default.cfg (basic configuration)")
    print("- Custom configuration files")
    
    print("\nConfiguration validation:")
    print("> python scripts/config_validator.py validate configs/enhanced.cfg")
    print("> python scripts/config_validator.py generate configs/default.cfg")
    
    print("\nConfiguration schema:")
    print("- docs/config_schema_v2.md (detailed schema)")
    print("- Configuration parameters organized by section")
    
    print("\nExample configuration sections:")
    print("[simulation.timing]")
    print("simulation_timestep = 0.001")
    print("max_simulation_steps = 10000")
    print("")
    print("[simulation.topology]")
    print("neuron_count = 500")
    print("region_count = 1")
    print("connection_probability = 0.15")
    print("")
    print("[logging.level]")
    print("log_level = \"INFO\"")
    print("log_to_file = true")
    print("log_filename = \"nlm.log\"")

def example_6_python_integration():
    """Example 6: Python integration."""
    print_header("Example 6: Python Integration")
    print("This example shows Python integration with NLM.")
    
    print("\nPython installation:")
    print("> pip install -e .")
    
    print("\nPython usage example:")
    print("""
import nlm

# Create brain configuration
config = nlm.Config()
config.neuron_count = 1000
config.simulation_timestep = 0.001

# Initialize brain
brain = nlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")

# Create experiment runner
experiment_runner = nlm.ExperimentRunner(brain)
experiment_runner.run_experiment("learning")

# Create analytics module
analytics = nlm.Analytics(brain)
metrics = analytics.get_metrics()
""")
    
    print("\nPython API:")
    print("- nlm.Config() - Configuration object")
    print("- nlm.Brain(config) - Brain instance")
    print("- nlm.ExperimentRunner(brain) - Experiment runner")
    print("- nlm.Analytics(brain) - Analytics module")

def example_7_advanced_features():
    """Example 7: Advanced features."""
    print_header("Example 7: Advanced Features")
    print("This example shows advanced NLM features.")
    
    print("\nAdvanced logging:")
    print("> nlm --enhanced-logging --format detailed")
    print("> nlm --async-logging --structured-logging")
    print("> nlm --performance-monitoring --benchmark-all")
    
    print("\nExperiment extensibility:")
    print("1. Register custom experiment")
    print("2. Add new log levels")
    print("3. Extend output formats")
    print("4. Add performance metrics")
    print("5. Create custom configurations")
    
    print("\nPerformance optimization:")
    print("> nlm --parallel-processing")
    print("> nlm --profiler")
    print("> nlm --benchmark-all")
    print("> nlm --log-file-rotation")
    
    print("\nDevelopment tools:")
    print("> python scripts/config_validator.py")
    print("> make test")
    print("> ctest --output-on-failure")

def create_example_scripts():
    """Create example scripts."""
    print_header("Creating Example Scripts")
    
    # Create a simple example script
    example_script = """
#!/usr/bin/env python3
# Example script for running NLM with custom parameters

import subprocess
import sys
import os

def run_nlm_example():
    """Run NLM with example parameters."""
    # Build command
    cmd = [
        "nlm",
        "--enhanced-logging",
        "--format", "json",
        "--performance-monitoring",
        "--benchmark", "spike_generation",
        "--benchmark", "plasticity_learning"
    ]
    
    print(f"Running: {' '.join(cmd)}")
    
    # Run NLM
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print("Successfully completed NLM run")
        print("Results:", result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error running NLM: {e}")
        print(f"Error output: {e.stderr}")

if __name__ == "__main__":
    run_nlm_example()
"""
    
    with open("example_run.py", "w") as f:
        f.write(example_script)
    
    print("Created example_run.py")
    
    # Create configuration example
    config_example = """
# Configuration example for NLM
# This shows how to create custom configuration

[metadata]
version = "2.0"
phase = "2"
title = "Custom NLM Configuration"

[simulation.timing]
simulation_timestep = 0.001
max_simulation_steps = 50000

[simulation.topology]
neuron_count = 2000
region_count = 3
connection_probability = 0.2

[logging.level]
log_level = "DEBUG"
log_to_file = true
log_filename = "nlm_custom.log"
log_file_rotation = true
log_rotation_size = 5242880  # 5MB

[experimental.performance]
performance_profile = "release"
parallel_processing = true
"""
    
    with open("custom_config.cfg", "w") as f:
        f.write(config_example)
    
    print("Created custom_config.cfg")

def main():
    """Main function."""
    print("NLM Usage Examples and Tutorials")
    print("=" * 60)
    
    examples = [
        ("Basic NLM Usage", example_1_basic_usage),
        ("Enhanced Logging Usage", example_2_enhanced_logging),
        ("Custom Experiments", example_3_custom_experiments),
        ("Performance Monitoring", example_4_performance_monitoring),
        ("Configuration Management", example_5_configuration_management),
        ("Python Integration", example_6_python_integration),
        ("Advanced Features", example_7_advanced_features),
    ]
    
    while True:
        print(f"\n{'=' * 60}")
        print("Available Examples:")
        for i, (title, _) in enumerate(examples, 1):
            print(f"{i}. {title}")
        print(f"{len(examples) + 1}. Create example scripts")
        print(f"{'len(examples) + 1}. Exit")
        
        try:
            choice = input(f"\nSelect an example (1-{len(examples) + 1}): ")
            
            if choice == str(len(examples) + 1):
                print("\nGoodbye!")
                break
            
            choice_idx = int(choice) - 1
            if 0 <= choice_idx < len(examples):
                title, example_func = examples[choice_idx]
                example_func()
            else:
                print("\nInvalid choice. Please try again.")
                
        except ValueError:
            print("\nInvalid input. Please enter a number.")
        except KeyboardInterrupt:
            print("\n\nGoodbye!")
            break
    
    # Create example scripts
    create_example_scripts()

if __name__ == "__main__":
    main()

# Make the script executable
import os
os.chmod("nlm_examples.py", 0o755)
