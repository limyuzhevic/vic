#!/usr/bin/env python3
"""
NLM Configuration Examples and Testing Scripts

This script provides practical examples of using NLM configuration files
and testing different parameter sets.
"""

import os
import json
import argparse
import subprocess
import sys
from pathlib import Path

def create_example_configs():
    """Create example configuration files for different use cases."""
    
    # Create configs directory
    configs_dir = Path("configs")
    configs_dir.mkdir(exist_ok=True)
    
    # 1. Basic Configuration (for quick testing)
    basic_config = {
        "# Basic NLM Configuration for Testing": "",
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.2,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.008,
        "stdp_tau": 15.0,
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001,
        "development_enabled": True,
        "dopamine_enabled": True,
        "curiosity_enabled": False,
        "novelty_enabled": False
    }
    
    with open(configs_dir / "basic.cfg", 'w') as f:
        f.write("# Basic NLM Configuration for Testing\n")
        f.write("# This configuration is optimized for quick testing and debugging\n\n")
        f.write(f"random_seed = {basic_config['random_seed']}\n")
        f.write(f"simulation_timestep = {basic_config['simulation_timestep']}\n")
        f.write(f"neuron_count = {basic_config['neuron_count']}\n")
        f.write(f"region_count = {basic_config['region_count']}\n")
        f.write(f"connection_probability = {basic_config['connection_probability']}\n")
        f.write(f"stdp_ltp_weight = {basic_config['stdp_ltp_weight']}\n")
        f.write(f"stdp_ltd_weight = {basic_config['stdp_ltd_weight']}\n")
        f.write(f"stdp_tau = {basic_config['stdp_tau']}\n")
        f.write(f"synaptogenesis_rate = {basic_config['synaptogenesis_rate']}\n")
        f.write(f"pruning_rate = {basic_config['pruning_rate']}\n")
        f.write(f"development_enabled = {basic_config['development_enabled']}\n")
        f.write(f"dopamine_enabled = {basic_config['dopamine_enabled']}\n")
        f.write(f"curiosity_enabled = {basic_config['curiosity_enabled']}\n")
        f.write(f"novelty_enabled = {basic_config['novelty_enabled']}\n")
    
    # 2. Research Configuration (for scientific experiments)
    research_config = {
        "# NLM Configuration for Research": "",
        "random_seed": 12345,
        "simulation_timestep": 0.0005,
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.1,
        "stdp": {
            "ltp_weight": 0.025,
            "ltd_weight": 0.02,
            "tau": 25.0,
            "pre_trace_decay": 0.025,
            "post_trace_decay": 0.025
        },
        "structural_plasticity": {
            "synaptogenesis_rate": 0.0005,
            "pruning_rate": 0.00005,
            "max_synapses_per_neuron": 100
        },
        "development": {
            "enabled": True,
            "critical_period_start": 50,
            "critical_period_end": 500,
            "plasticity_decay_rate": 0.0005
        },
        "neuromodulation": {
            "dopamine": {
                "enabled": True,
                "strength": 1.5,
                "prediction_error_weight": 0.7
            },
            "curiosity": {
                "enabled": True,
                "exploration_strength": 0.5
            },
            "novelty": {
                "enabled": True,
                "detection_threshold": 0.3
            }
        }
    }
    
    with open(configs_dir / "research.json", 'w') as f:
        json.dump(research_config, f, indent=2)
    
    # 3. Agent Training Configuration
    agent_config = {
        "# NLM Configuration for Agent Training": "",
        "random_seed": 999,
        "simulation_timestep": 0.001,
        "neuron_count": 1500,
        "region_count": 2,
        "connection_probability": 0.12,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001,
        "development_enabled": True,
        "critical_period_start": 100,
        "critical_period_end": 1000,
        "dopamine_enabled": True,
        "curiosity_enabled": True,
        "novelty_enabled": True,
        "environment": {
            "width": 800,
            "height": 600,
            "vision_width": 160,
            "vision_height": 120,
            "max_energy": 200.0,
            "energy_decay_rate": 0.005
        }
    }
    
    with open(configs_dir / "agent_training.json", 'w') as f:
        json.dump(agent_config, f, indent=2)
    
    # 4. Optimized Production Configuration
    prod_config = {
        "# Optimized NLM Configuration for Production": "",
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    }
    
    with open(configs_dir / "production.cfg", 'w') as f:
        f.write("# Optimized NLM Configuration for Production\n")
        f.write("# This configuration balances performance and learning capability\n\n")
        for key, value in prod_config.items():
            if key.startswith("#"):
                f.write(f"# {value}\n")
            elif isinstance(value, (int, float, bool)):
                f.write(f"{key} = {value}\n")
            else:
                f.write(f"{key} = {value}\n")
    
    print("Created example configuration files in configs/:\n")
    for config_file in configs_dir.iterdir():
        if config_file.is_file():
            print(f"  {config_file.name}")

def test_configuration(config_file, output_file=None):
    """Test a configuration file with NLM."""
    print(f"\nTesting configuration: {config_file}")
    
    # Check if NLM executable exists
    if not os.path.exists("./nlm") and not os.path.exists("./nlm_phase2_demo"):
        print("ERROR: NLM executable not found. Please build the project first.")
        print("Run: mkdir build && cd build && cmake .. && make -j4")
        return False
    
    # Run NLM with the configuration
    cmd = ["./nlm", f"--config={config_file}"]
    
    try:
        print(f"Running: {' '.join(cmd)}")
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode == 0:
            print("SUCCESS: Configuration test passed!")
            if output_file:
                with open(output_file, 'w') as f:
                    f.write(result.stdout)
                print(f"Output saved to: {output_file}")
            return True
        else:
            print(f"FAILED: Configuration test failed with return code {result.returncode}")
            print(f"Error output: {result.stderr}")
            return False
            
    except FileNotFoundError:
        print("ERROR: NLM executable not found")
        return False
    except Exception as e:
        print(f"ERROR: Failed to run configuration test: {e}")
        return False

def compare_configurations(config1, config2, output_diff=None):
    """Compare two configuration files and show differences."""
    print(f"\nComparing configurations: {config1} vs {config2}")
    
    # Load configurations
    config1_data = load_config(config1)
    config2_data = load_config(config2)
    
    # Find differences
    all_keys = set(config1_data.keys()) | set(config2_data.keys())
    differences = []
    
    for key in sorted(all_keys):
        val1 = config1_data.get(key)
        val2 = config2_data.get(key)
        
        if val1 != val2:
            differences.append((key, val1, val2))
    
    # Display differences
    print(f"\nFound {len(differences)} differences:")
    for key, val1, val2 in differences:
        print(f"  {key}:")
        print(f"    {config1}: {val1}")
        print(f"    {config2}: {val2}")
    
    # Save differences if requested
    if output_diff and differences:
        diff_data = {
            "file1": config1,
            "file2": config2,
            "differences": [{"key": k, "value1": v1, "value2": v2} for k, v1, v2 in differences]
        }
        
        with open(output_diff, 'w') as f:
            json.dump(diff_data, f, indent=2)
        
        print(f"\nDifferences saved to: {output_diff}")
    
    return differences

def load_config(filepath):
    """Load configuration from file (supports both JSON and key=value formats)."""
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Try to parse as JSON first
    try:
        return json.loads(content)
    except json.JSONDecodeError:
        # Fall back to key=value format
        config = {}
        for line in content.splitlines():
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                config[key.strip()] = value.strip()
        return config

def run_performance_test(config_file, steps=1000, repetitions=3):
    """Run performance tests with a given configuration."""
    print(f"\nRunning performance tests with: {config_file}")
    print(f"Steps: {steps}, Repetitions: {repetitions}")
    
    times = []
    spike_counts = []
    
    for i in range(repetitions):
        print(f"  Run {i+1}/{repetitions}...")
        cmd = ["./nlm", f"--config={config_file}", f"--steps={steps}"]
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode == 0:
                # Extract timing information from output (simplified)
                lines = result.stdout.split('\n')
                for line in lines:
                    if 'wall clock time' in line.lower():
                        # Extract time value
                        parts = line.split(':')
                        if len(parts) > 1:
                            time_str = parts[-1].strip().replace('s', '')
                            try:
                                times.append(float(time_str))
                            except ValueError:
                                pass
                    elif 'total spikes' in line.lower():
                        # Extract spike count
                        parts = line.split(':')
                        if len(parts) > 1:
                            spike_str = parts[-1].strip()
                            try:
                                spike_counts.append(int(spike_str))
                            except ValueError:
                                pass
            
            # Small delay between runs
            import time
            time.sleep(0.1)
            
        except Exception as e:
            print(f"    ERROR: Run {i+1} failed: {e}")
    
    # Calculate statistics
    if times:
        avg_time = sum(times) / len(times)
        max_time = max(times)
        min_time = min(times)
        
        print(f"\nPerformance Results ({repetitions} runs):")
        print(f"  Average time: {avg_time:.3f}s")
        print(f"  Max time: {max_time:.3f}s")
        print(f"  Min time: {min_time:.3f}s")
        print(f"  Std dev: {((sum((t - avg_time) ** 2 for t in times) / len(times)) ** 0.5):.3f}s")
    
    if spike_counts:
        avg_spikes = sum(spike_counts) / len(spike_counts)
        print(f"\nActivity Results:")
        print(f"  Average spikes: {avg_spikes:.0f}")
        print(f"  Spike range: {min(spike_counts)} - {max(spike_counts)}")
    
    return times, spike_counts

def main():
    parser = argparse.ArgumentParser(
        description="NLM Configuration Examples and Testing Script",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python config_examples.py --create-examples
  python config_examples.py --test-config configs/basic.cfg
  python config_examples.py --compare configs/baseline.cfg configs/optimized.cfg
  python config_examples.py --performance-test configs/agent_training.json
        ""
    )
    
    parser.add_argument(
        "--create-examples",
        action="store_true",
        help="Create example configuration files"
    )
    
    parser.add_argument(
        "--test-config",
        help="Test a configuration file with NLM"
    )
    
    parser.add_argument(
        "--compare",
        nargs=2,
        metavar="CONFIG_FILE",
        help="Compare two configuration files"
    )
    
    parser.add_argument(
        "--performance-test",
        help="Run performance tests with a configuration file"
    )
    
    parser.add_argument(
        "--output-diff",
        help="Output file for configuration comparison differences"
    )
    
    parser.add_argument(
        "--steps",
        type=int,
        default=1000,
        help="Number of simulation steps for performance tests"
    )
    
    parser.add_argument(
        "--repetitions",
        type=int,
        default=3,
        help="Number of repetitions for performance tests"
    )
    
    args = parser.parse_args()
    
    if args.create_examples:
        create_example_configs()
    
    elif args.test_config:
        success = test_configuration(args.test_config)
        sys.exit(0 if success else 1)
    
    elif args.compare:
        compare_configurations(args.compare[0], args.compare[1], args.output_diff)
    
    elif args.performance_test:
        run_performance_test(args.performance_test, args.steps, args.repetitions)
    
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
