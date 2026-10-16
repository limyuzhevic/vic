#!/usr/bin/env python3
"""
NLM Explorer Script

This script provides exploration capabilities for NLM simulations,
including interactive analysis, visualization, and parameter exploration.
"""

import sys
import os
import argparse
import json
import time
import numpy as np

# Add the current directory to the path so we can import nlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import nlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Error: NLM Python bindings not found.")
    print("Please build the project first:")
    print("  mkdir build")
    print("  cd build")
    print("  cmake ..")
    print("  make pynlm")
    sys.exit(1)
def print_welcome():
    """Print welcome message."""
    print("=" * 70)
    print("NLM Explorer - Interactive NLM Simulation Analysis")
    print("=" * 70)
    print()
    print("This tool provides interactive exploration of NLM simulations,")
    print("including visualization, parameter analysis, and scenario comparison.")
    print()
def explore_basic_properties():
    """Explore basic properties of NLM simulations."""
    print("1. Basic Properties Explorer")
    print("-" * 50)
    
    # Create a range of brain configurations
    configurations = [
        {"neurons": 500, "steps": 500, "name": "Small"},
        {"neurons": 1000, "steps": 1000, "name": "Medium"},
        {"neurons": 2000, "steps": 2000, "name": "Large"},
        {"neurons": 5000, "steps": 1000, "name": "Many Neurons"},
    ]
    
    results = {}
    
    for config in configurations:
        print(f"\nExploring {config['name']} configuration...")
        
        # Create configuration
        brain_config = nlm.create_demo_config()
        brain_config.set_int("brain.neuron_count", config["neurons"])
        brain_config.set_int("brain.simulation_steps", config["steps"])
        
        # Create brain and world
        brain = nlm.createBrain(brain_config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Run simulation
        nlm.run_simulation(brain, world, config["steps"])
        
        # Collect statistics
        stats = brain.get_statistics()
        
        results[config["name"]] = {
            "parameters": config,
            "statistics": stats,
            "performance": brain.get_statistics(),
        }
        
        print(f"  - Neurons: {stats.get('total_neurons', 0)}")
        print(f"  - Synapses: {stats.get('total_synapses', 0)}")
        print(f"  - Total Spikes: {stats.get('total_spikes', 0)}")
        print(f"  - Average Firing Rate: {stats.get('avg_firing_rate', 0):.2f} Hz")
    
    return results
def explore_parameter_sensitivity():
    """Explore sensitivity to different parameters."""
    print("\n2. Parameter Sensitivity Explorer")
    print("-" * 50)
    
    # Define parameter ranges to explore
    parameter_ranges = {
        "plasticity.learning_rate": [0.01, 0.05, 0.1, 0.2],
        "brain.connection_probability": [0.01, 0.05, 0.1, 0.2],
        "brain.development_rate": [1, 3, 5],
    }
    
    all_results = []
    
    for param_name, param_values in parameter_ranges.items():
        print(f"\nExploring sensitivity to {param_name}...")
        
        param_results = []
        
        for value in param_values:
            print(f"  Testing {param_name} = {value}...")
            
            # Create configuration
            config = nlm.create_demo_config()
            
            # Set parameter based on its type
            if "plasticity" in param_name:
                config.set_float(param_name, value)
            elif "connection" in param_name:
                config.set_float(param_name, value)
            elif "development" in param_name:
                config.set_int(param_name, int(value))
            
            # Create brain and world
            brain = nlm.createBrain(config)
            brain.initialize()
            
            world = nlm.create_simple_demo_world()
            
            # Run simulation
            nlm.run_simulation(brain, world, config.get_int("brain.simulation_steps", 1000))
            
            # Collect statistics
            stats = brain.get_statistics()
            
            param_result = {
                "parameter": param_name,
                "value": value,
                "statistics": stats,
                "timestamp": time.time(),
            }
            
            param_results.append(param_result)
            
            print(f"    - Spikes: {stats.get('total_spikes', 0)}")
            print(f"    - Firing rate: {stats.get('avg_firing_rate', 0):.2f} Hz")
        
        all_results.extend(param_results)
    
    return all_results
def explore_time_dynamics():
    """Explore temporal dynamics of simulations."""
    print("\n3. Time Dynamics Explorer")
    print("-" * 50)
    
    # Create different brain configurations
    configurations = [
        {"neurons": 1000, "steps": 1000, "name": "Medium"},
        {"neurons": 1000, "steps": 5000, "name": "Long"},
        {"neurons": 1000, "steps": 10000, "name": "Extended"},
    ]
    
    time_series_data = {}
    
    for config in configurations:
        print(f"\nExploring time dynamics for {config['name']} simulation...")
        
        # Create configuration
        brain_config = nlm.create_demo_config()
        brain_config.set_int("brain.neuron_count", config["neurons"])
        brain_config.set_int("brain.simulation_steps", config["steps"])
        
        # Create brain and world
        brain = nlm.createBrain(brain_config)
        brain.initialize()
        
        world = nlm.create_simple_demo_world()
        
        # Collect time series data
        time_series = {
            "step": [],
            "total_spikes": [],
            "active_neurons": [],
            "firing_neurons": [],
            "firing_rate": [],
            "e_i_ratio": [],
        }
        
        # Run simulation and collect data
        for step in range(config["steps"]):
            # Record current state
            time_series["step"].append(step)
            time_series["total_spikes"].append(brain.getTotalSpikeCount())
            time_series["active_neurons"].append(brain.getActiveNeuronCount())
            time_series["firing_neurons"].append(brain.getFiringNeuronCount())
            time_series["firing_rate"].append(brain.getAverageFiringRate())
            time_series["e_i_ratio"].append(brain.getExcitationInhibitionRatio())
            
            # Advance simulation
            world.update(0.01)
            brain.step(step, step * 0.001)
            
            # Record every 100 steps for long simulations
            if step % 100 == 0 and step > 0:
                print(f"  Step {step}/{config['steps']}")
        
        time_series_data[config["name"]] = time_series
        
        print(f"  Collected {len(time_series['step'])} data points")
        
        # Calculate summary statistics
        print(f"  Time series summary:")
        print(f"    - Total spikes: {time_series['total_spikes'][-1]}")
        print(f"    - Final firing rate: {time_series['firing_rate'][-1]:.2f} Hz")
        print(f"    - Final E/I ratio: {time_series['e_i_ratio'][-1]:.4f}")
        print(f"    - Max active neurons: {max(time_series['active_neurons'])}")
    
    return time_series_data
def explore_neural_network_properties():
    """Explore neural network properties and statistics."""
    print("\n4. Neural Network Properties Explorer")
    print("-" * 50)
    
    # Create different network sizes
    network_sizes = [
        {"neurons": 100, "name": "Small"},
        {"neurons": 500, "name": "Medium"},
        {"neurons": 1000, "name": "Large"},
        {"neurons": 2000, "name": "Extra Large"},
    ]
    
    network_properties = {}
    
    for size in network_sizes:
        print(f"\nExploring {size['name']} neural network...")
        
        # Create configuration
        config = nlm.create_demo_config()
        config.set_int("brain.neuron_count", size["neurons"])
        
        # Create brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Get various neural properties
        properties = {
            "basic_stats": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "active_neurons": brain.getActiveNeuronCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
            },
            "region_info": [],
            "developmental_stage": brain.getDevelopmentalStage(),
        }
        
        # Get region information
        region_count = brain.getRegionCount()
        properties["region_count"] = region_count
        
        for i in range(region_count):
            region_id = brain.getRegionIds()[i]
            region = brain.getRegion(region_id)
            
            region_info = {
                "id": str(region_id),
                "neuron_count": region.getNeuronCount(),
                "synapse_count": region.getSynapseCount(),
                "active_neurons": region.getActiveNeuronCount(),
            }
            
            properties["region_info"].append(region_info)
        
        # Get spike system statistics
        spike_system = brain.getSpikeSystem()
        if spike_system:
            properties["spike_system"] = {
                "pending_events": spike_system.getPendingSpikeEventCount(),
                "most_active_neurons": spike_system.getMostActiveNeurons(5),
            }
        
        network_properties[size["name"]] = properties
        
        print(f"  - Network size: {size['neurons']} neurons")
        print(f"  - Total synapses: {properties['basic_stats']['total_synapses']}")
        print(f"  - Synapse density: {properties['basic_stats']['total_synapses'] / size['neurons']:.2f} synapses/neuron")
        print(f"  - Regions: {region_count}")
        print(f"  - Firing rate: {properties['basic_stats']['average_firing_rate']:.2f} Hz")
        print(f"  - E/I ratio: {properties['basic_stats']['excitation_inhibition_ratio']:.4f}")
    
    return network_properties
def create_visualization_data(data, output_file):
    """Create visualization data from exploration results."""
    print(f"\nCreating visualization data in {output_file}...")
    
    # Prepare data for visualization
    visualization_data = {
        "type": "nlm_exploration_results",
        "timestamp": time.time(),
        "data": data,
        "format": "json",
    }
    
    # Save to file
    with open(output_file, 'w') as f:
        json.dump(visualization_data, f, indent=2, default=str)
    
    print(f"  Visualization data saved to {output_file}")
    return visualization_data
def create_analysis_report(exploration_results, report_file):
    """Create analysis report from exploration results."""
    print(f"\nCreating analysis report in {report_file}...")
    
    # Generate summary statistics
    report = {
        "title": "NLM Exploration Analysis Report",
        "timestamp": time.time(),
        "summary": {
            "total_simulations": len(exploration_results),
            "date": time.strftime("%Y-%m-%d %H:%M:%S"),
            "python_version": sys.version,
        },
        "findings": [],
    }
    
    # Analyze each exploration result
    for i, result in enumerate(exploration_results):
        finding = {
            "id": i,
            "type": list(result.keys())[0] if isinstance(result, dict) else "unknown",
            "data_points": len(result.get(list(result.keys())[0], [])) if isinstance(result, dict) else 0,
        }
        
        report["findings"].append(finding)
        
        print(f"  Finding {i + 1}: {finding['type']} with {finding['data_points']} data points")
    
    # Save report
    with open(report_file, 'w') as f:
        json.dump(report, f, indent=2, default=str)
    
    print(f"  Analysis report saved to {report_file}")
    return report
def interactive_menu():
    """Display interactive menu for exploration."""
    print("\n" + "=" * 70)
    print("NLM Explorer Interactive Menu")
    print("=" * 70)
    print()
    print("Available exploration options:")
    print("  1. Basic Properties Explorer")
    print("  2. Parameter Sensitivity Explorer")
    print("  3. Time Dynamics Explorer")
    print("  4. Neural Network Properties Explorer")
    print("  5. Run All Explorations")
    print("  6. Custom Exploration")
    print("  7. Save Results")
    print("  8. Generate Report")
    print("  9. Exit")
    print()
    
    try:
        choice = input("Enter your choice (1-9): ").strip()
        
        if choice == "1":
            results = explore_basic_properties()
            return results
        elif choice == "2":
            results = explore_parameter_sensitivity()
            return results
        elif choice == "3":
            results = explore_time_dynamics()
            return results
        elif choice == "4":
            results = explore_neural_network_properties()
            return results
        elif choice == "5":
            # Run all explorations
            print("Running all explorations...")
            
            results1 = explore_basic_properties()
            results2 = explore_parameter_sensitivity()
            results3 = explore_time_dynamics()
            results4 = explore_neural_network_properties()
            
            all_results = {
                "basic_properties": results1,
                "parameter_sensitivity": results2,
                "time_dynamics": results3,
                "neural_network_properties": results4,
            }
            
            return all_results
        elif choice == "6":
            # Custom exploration would go here
            print("Custom exploration not yet implemented")
            return {}
        elif choice == "7":
            # Save results would go here
            print("Save functionality not yet implemented")
            return {}
        elif choice == "8":
            # Generate report would go here
            print("Report generation not yet implemented")
            return {}
        elif choice == "9":
            print("Exiting...")
            return None
        else:
            print(f"Invalid choice: {choice}")
            return None
            
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return None
    except Exception as e:
        print(f"Error during exploration: {e}")
        return None
def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM Explorer - Interactive NLM Simulation Analysis",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python explore.py                    # Interactive exploration
  python explore.py --basic-only       # Basic properties only
  python explore.py --sensitivity-only # Parameter sensitivity only
  python explore.py --output results.json # Save results to file
  python explore.py --report report.json  # Generate analysis report
        """
    )
    
    parser.add_argument(
        "--basic-only", "-b",
        action="store_true",
        help="Run only basic properties exploration",
    )
    
    parser.add_argument(
        "--sensitivity-only", "-s",
        action="store_true",
        help="Run only parameter sensitivity exploration",
    )
    
    parser.add_argument(
        "--timedynamics-only", "-t",
        action="store_true",
        help="Run only time dynamics exploration",
    )
    
    parser.add_argument(
        "--network-only", "-n",
        action="store_true",
        help="Run only neural network properties exploration",
    )
    
    parser.add_argument(
        "--output", "-o",
        type=str,
        help="Output file for exploration results (JSON format)",
    )
    
    parser.add_argument(
        "--report", "-r",
        type=str,
        help="Report file for analysis results (JSON format)",
    )
    
    parser.add_argument(
        "--interactive", "-i",
        action="store_true",
        help="Use interactive menu for exploration selection",
    )
    
    parser.add_argument(
        "--quick", "-q",
        action="store_true",
        help="Run quick exploration with limited data",
    )
    
    parser.add_argument(
        "--verbose", "-V",
        action="store_true",
        help="Enable verbose output",
    )
    
    args = parser.parse_args()
    
    # Print welcome message
    print_welcome()
    
    # Validate NLM availability
    if not NLM_AVAILABLE:
        print("Error: NLM Python bindings not available.")
        sys.exit(1)
    
    try:
        all_results = {}
        
        # Run explorations based on arguments
        if args.interactive:
            print("\nStarting interactive exploration...")
            results = interactive_menu()
            
            if results is not None:
                all_results["interactive"] = results
        else:
            # Run specific explorations based on arguments
            if args.basic_only:
                if args.quick:
                    print("Running quick basic exploration...")
                    configs = [
                        {"neurons": 500, "steps": 500, "name": "Small"},
                        {"neurons": 1000, "steps": 1000, "name": "Medium"},
                    ]
                else:
                    configs = [
                        {"neurons": 100, "steps": 100, "name": "Small"},
                        {"neurons": 500, "steps": 500, "name": "Medium"},
                        {"neurons": 1000, "steps": 1000, "name": "Large"},
                        {"neurons": 2000, "steps": 2000, "name": "Extra Large"},
                    ]
                
                results = explore_basic_properties()
                all_results["basic_properties"] = results
                
            if args.sensitivity_only:
                results = explore_parameter_sensitivity()
                all_results["parameter_sensitivity"] = results
            
            if args.timedynamics_only:
                results = explore_time_dynamics()
                all_results["time_dynamics"] = results
            
            if args.network_only:
                results = explore_neural_network_properties()
                all_results["neural_network_properties"] = results
            
            # If no specific options, run all explorations
            if not any([args.basic_only, args.sensitivity_only, args.timedynamics_only, args.network_only]):
                print("\nRunning all explorations...")
                
                all_results["basic_properties"] = explore_basic_properties()
                all_results["parameter_sensitivity"] = explore_parameter_sensitivity()
                all_results["time_dynamics"] = explore_time_dynamics()
                all_results["neural_network_properties"] = explore_neural_network_properties()
        
        # Save results if requested
        if args.output and all_results:
            create_visualization_data(all_results, args.output)
        
        # Generate report if requested
        if args.report and all_results:
            create_analysis_report(all_results, args.report)
        
        # Print summary
        print("\n" + "=" * 70)
        print("Exploration completed!")
        print("=" * 70)
        
        if all_results:
            print(f"\nExploration summary:")
            print(f"  Total exploration types: {len(all_results)}")
            
            for exploration_type, results in all_results.items():
                print(f"  {exploration_type}: {len(results) if isinstance(results, list) else 'completed'}")
            
            if args.output:
                print(f"\nResults saved to: {args.output}")
            
            if args.report:
                print(f"Analysis report generated: {args.report}")
        
        print("\nExploration completed successfully!")
        
    except Exception as e:
        print(f"\nError during exploration: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
if __name__ == "__main__":
    main()
