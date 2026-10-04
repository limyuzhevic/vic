#!/usr/bin/env python3

"""
NLM Command Line Interface

A comprehensive command-line interface for NLM (Neural Learning Machine) simulations.
Provides commands for creating, running, visualizing, and managing NLM simulations.
"""

import argparse
import sys
import os
import time
import json
import yaml
import tempfile
from pathlib import Path
from typing import Dict, Any, List, Optional
import subprocess

# Add the parent directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent.parent))

import pynlm
def create_parser() -> argparse.ArgumentParser:
    """Create the main argument parser"""
    parser = argparse.ArgumentParser(
        prog="nlm",
        description="NLM (Neural Learning Machine) - Neural Simulation Framework",
        epilog="For more information, visit: https://github.com/nlm-project/nlm",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "-v", "--version",
        action="version",
        version="NLM 0.1.0"
    )
    
    parser.add_argument(
        "-c", "--config",
        default="configs/default.cfg",
        help="Path to configuration file"
    )
    
    parser.add_argument(
        "-o", "--output",
        help="Output directory for results"
    )
    
    parser.add_argument(
        "-q", "--quiet",
        action="store_true",
        help="Suppress output messages"
    )
    
    parser.add_argument(
        "-f", "--fast",
        action="store_true",
        help="Use fast configuration (smaller simulation)"
    )
    
    parser.add_argument(
        "-s", "--steps",
        type=int,
        default=100,
        help="Number of simulation steps"
    )
    
    subparsers = parser.add_subparsers(
        dest="command",
        title="Commands",
        description="Available commands",
        help="Command to execute"
    )
    
    # Create command
    create_parser = subparsers.add_parser(
        "create",
        aliases=["new"],
        help="Create a new NLM simulation"
    )
    create_parser.add_argument(
        "name",
        help="Name of the simulation"
    )
    create_parser.add_argument(
        "--brain-type",
        choices=["simple", "advanced", "research"],
        default="simple",
        help="Type of brain to create"
    )
    create_parser.add_argument(
        "--world-type",
        choices=["simple", "complex"],
        default="simple",
        help="Type of world to create"
    )
    
    # Run command
    run_parser = subparsers.add_parser(
        "run",
        help="Run an NLM simulation"
    )
    run_parser.add_argument(
        "name",
        help="Name of the simulation to run"
    )
    run_parser.add_argument(
        "--steps",
        type=int,
        help="Number of steps to run (overrides config)"
    )
    run_parser.add_argument(
        "--output",
        help="Output directory for results"
    )
    run_parser.add_argument(
        "--plot",
        action="store_true",
        help="Generate visualization plots"
    )
    run_parser.add_argument(
        "--profile",
        action="store_true",
        help="Profile simulation performance"
    )
    
    # Test command
    test_parser = subparsers.add_parser(
        "test",
        help="Run NLM test suite"
    )
    test_parser.add_argument(
        "--type",
        choices=["quick", "full", "performance"],
        default="quick",
        help="Type of test suite to run"
    )
    test_parser.add_argument(
        "--output",
        help="Output file for test results"
    )
    
    # Config command
    config_parser = subparsers.add_parser(
        "config",
        aliases=["cfg"],
        help="Manage configuration files"
    )
    config_subparsers = config_parser.add_subparsers(
        dest="config_command",
        title="Config Commands",
        help="Config command to execute"
    )
    
    # Config create
    config_create_parser = config_subparsers.add_parser(
        "create",
        help="Create a new configuration"
    )
    config_create_parser.add_argument(
        "name",
        help="Name of configuration"
    )
    config_create_parser.add_argument(
        "--type",
        choices=["json", "yaml", "keyvalue"],
        default="json",
        help="Configuration file format"
    )
    
    # Config list
    config_subparsers.add_parser(
        "list",
        help="List all configurations"
    )
    
    # Config show
    config_show_parser = config_subparsers.add_parser(
        "show",
        help="Show configuration details"
    )
    config_show_parser.add_argument(
        "name",
        help="Name of configuration"
    )
    
    # Config export
    config_export_parser = config_subparsers.add_parser(
        "export",
        help="Export configuration"
    )
    config_export_parser.add_argument(
        "name",
        help="Name of configuration"
    )
    config_export_parser.add_argument(
        "--format",
        choices=["json", "yaml", "keyvalue"],
        default="json",
        help="Export format"
    )
    
    # Visualization command
    viz_parser = subparsers.add_parser(
        "viz",
        aliases=["visualize"],
        help="Create visualizations"
    )
    viz_parser.add_argument(
        "name",
        help="Name of simulation to visualize"
    )
    viz_parser.add_argument(
        "--type",
        choices=["network", "activity", "debug", "all"],
        default="all",
        help="Type of visualization"
    )
    viz_parser.add_argument(
        "--output",
        help="Output directory for visualizations"
    )
    
    # Profile command
    profile_parser = subparsers.add_parser(
        "profile",
        help="Profile simulation performance"
    )
    profile_parser.add_argument(
        "name",
        help="Name of simulation to profile"
    )
    profile_parser.add_argument(
        "--steps",
        type=int,
        default=50,
        help="Number of steps to profile"
    )
    profile_parser.add_argument(
        "--output",
        help="Output file for profiling results"
    )
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser(
        "benchmark",
        help="Run benchmark tests"
    )
    benchmark_parser.add_argument(
        "--steps",
        type=int,
        default=100,
        help="Number of steps for each benchmark"
    )
    benchmark_parser.add_argument(
        "--output",
        help="Output file for benchmark results"
    )
    
    return parser
def handle_create(args):
    """Handle the create command"""
    print(f"Creating NLM simulation '{args.name}'...")
    
    # Create simulation directory
    sim_dir = Path("simulations") / args.name
    sim_dir.mkdir(parents=True, exist_ok=True)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    if args.brain_type == "simple":
        config.setInt("brain.neuron_count", 100)
        config.setInt("brain.region_count", 1)
        config.setDouble("brain.connection_probability", 0.1)
    elif args.brain_type == "advanced":
        config.setInt("brain.neuron_count", 500)
        config.setInt("brain.region_count", 3)
        config.setDouble("brain.connection_probability", 0.15)
    elif args.brain_type == "research":
        config.setInt("brain.neuron_count", 1000)
        config.setInt("brain.region_count", 5)
        config.setDouble("brain.connection_probability", 0.2)
    
    # Create world configuration
    if args.world_type == "simple":
        pass  # Default world configuration
    else:
        config.setInt("world.width", 50)
        config.setInt("world.height", 50)
    
    # Save configuration
    config_path = sim_dir / "config.json"
    config.saveToFile(str(config_path))
    
    # Create simulation script
    script_path = sim_dir / "run_simulation.py"
    with open(script_path, 'w') as f:
        f.write(f"""#!/usr/bin/env python3
import pynlm
import time
import sys

# Load configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("{config_path}")

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
print(f"Starting simulation: {{args.name}}")
start_time = time.time()

for step in range({args.steps}):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.updateDevelopment(0.1)
    
    if step % 20 == 0:
        print(f"Step {{step}}: {{brain.getFiringNeuronCount()}} neurons firing")

elapsed = time.time() - start_time
print(f"Simulation completed in {{elapsed:.2f}} seconds")
print(f"Total spikes: {{brain.getTotalSpikeCount()}}")
print(f"Firing rate: {{brain.getAverageFiringRate():.1f}} Hz")
""")
    
    # Make script executable
    script_path.chmod(0o755)
    
    print(f"Simulation '{args.name}' created in {sim_dir}")
    print(f"Configuration: {config_path}")
    print(f"Run simulation: python {script_path}")
def handle_run(args):
    """Handle the run command"""
    print(f"Running NLM simulation '{args.name}'...")
    
    # Check if simulation exists
    sim_dir = Path("simulations") / args.name
    config_file = sim_dir / "config.json"
    script_file = sim_dir / "run_simulation.py"
    
    if not config_file.exists():
        print(f"Error: Simulation '{args.name}' not found")
        print(f"Run 'nlm create {args.name}' to create a new simulation")
        return 1
    
    if not script_file.exists():
        print(f"Error: Simulation script not found for '{args.name}'")
        return 1
    
    # Run simulation script
    try:
        result = subprocess.run(
            [sys.executable, str(script_file)],
            capture_output=not args.quiet,
            text=True
        )
        
        if result.returncode != 0:
            print(f"Error: Simulation failed with exit code {result.returncode}")
            if not args.quiet:
                print(f"Output: {result.stderr}")
            return 1
        
        # Generate plots if requested
        if args.plot:
            handle_viz({
                'name': args.name,
                'type': 'activity',
                'output': args.output
            })
        
        # Profile if requested
        if args.profile:
            handle_profile({
                'name': args.name,
                'steps': args.steps,
                'output': args.output
            })
        
    except Exception as e:
        print(f"Error running simulation: {e}")
        return 1
    
    return 0
def handle_test(args):
    """Handle the test command"""
    print(f"Running NLM tests ({args.type})...")
    
    if args.type == "quick":
        # Run quick tests
        import importlib.util
        spec = importlib.util.spec_from_file_location("test_suite", "tests/test_suite.py")
        test_suite = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(test_suite)
        
        # Run quick tests
        result = test_suite.run_quick_tests()
        
    elif args.type == "full":
        # Run full test suite
        import importlib.util
        spec = importlib.util.spec_from_file_location("test_suite", "tests/test_suite.py")
        test_suite = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(test_suite)
        
        # Run all tests
        result = test_suite.run_all_tests()
        
    elif args.type == "performance":
        # Run performance benchmark
        import pynlm
        import time
        import json
        
        # Test different configurations
        configs = {
            "small": {"brain.neuron_count": 50, "simulation.timestep": 0.01},
            "medium": {"brain.neuron_count": 100, "simulation.timestep": 0.005},
            "large": {"brain.neuron_count": 200, "simulation.timestep": 0.001},
        }
        
        results = {}
        
        for name, params in configs.items():
            config = pynlm.createDefaultConfig()
            for key, value in params.items():
                config.set(key, value)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for i in range(args.steps):
                brain.step(i)
            elapsed = time.time() - start_time
            
            results[name] = {
                "time": elapsed,
                "steps_per_second": args.steps / elapsed,
                "spikes": brain.getTotalSpikeCount()
            }
        
        # Save results
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
        
        print("Benchmark Results:")
        for name, result in results.items():
            print(f"  {name}:")
            print(f"    Time: {result['time']:.2f}s")
            print(f"    Steps/sec: {result['steps_per_second']:.1f}")
            print(f"    Spikes: {result['spikes']}")
        
        result = type('Result', (), {'testsRun': 1, 'failures': [], 'errors': []})()
    
    return 0
def handle_config(args):
    """Handle the config command"""
    if not args.config_command:
        print("Error: Config subcommand required")
        print("Available commands: create, list, show, export")
        return 1
    
    if args.config_command == "create":
        print(f"Creating configuration '{args.name}'...")
        # Implementation would go here
        pass
    
    elif args.config_command == "list":
        print("Listing configurations...")
        # Implementation would go here
        print("Available configurations:")
        for config_file in Path("configs").glob("*.json"):
            print(f"  {config_file.stem}")
    
    elif args.config_command == "show":
        print(f"Showing configuration '{args.name}'...")
        # Implementation would go here
        config_path = Path("configs") / f"{args.name}.json"
        if config_path.exists():
            print(config_path.read_text())
        else:
            print(f"Configuration '{args.name}' not found")
    
    elif args.config_command == "export":
        print(f"Exporting configuration '{args.name}'...")
        # Implementation would go here
        pass
    
    return 0
def handle_viz(args):
    """Handle the viz command"""
    print(f"Creating visualizations for simulation '{args.name}'...")
    
    # Import visualization module
    try:
        from visualization import create_default_visualization
        
        # Create simulation directory path
        sim_dir = Path("simulations") / args.name
        
        # Check if simulation exists
        if not sim_dir.exists():
            print(f"Error: Simulation '{args.name}' not found")
            return 1
        
        # Load simulation configuration
        config_file = sim_dir / "config.json"
        if not config_file.exists():
            print(f"Error: Configuration file not found for '{args.name}'")
            return 1
        
        # Create brain from config
        config = pynlm.createDefaultConfig()
        config.loadFromFile(str(config_file))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Create visualization
        vis = create_default_visualization(brain)
        
        # Determine output directory
        if args.output:
            output_dir = Path(args.output)
        else:
            output_dir = sim_dir / "visualizations"
        
        output_dir.mkdir(parents=True, exist_ok=True)
        
        # Create visualizations based on type
        if args.type in ["network", "all"]:
            network_file = output_dir / "network_graph.png"
            vis.create_network_graph(str(network_file))
            print(f"Network graph saved: {network_file}")
        
        if args.type in ["activity", "all"]:
            activity_file = output_dir / "activity_plots.png"
            vis.create_activity_plots(str(activity_file))
            print(f"Activity plots saved: {activity_file}")
        
        if args.type in ["debug", "all"]:
            debug_file = output_dir / "debug_dashboard.png"
            vis.create_debug_dashboard(str(debug_file))
            print(f"Debug dashboard saved: {debug_file}")
        
        # Export all visualizations
        summary = vis.export_all_visualizations(str(output_dir))
        print(f"Visualizations exported to: {output_dir}")
        
    except ImportError as e:
        print(f"Error importing visualization module: {e}")
        return 1
    except Exception as e:
        print(f"Error creating visualizations: {e}")
        return 1
    
    return 0
def handle_profile(args):
    """Handle the profile command"""
    print(f"Profiling simulation '{args.name}'...")
    
    # Import profiling module
    try:
        from profiling import PerformanceProfiler
        
        # Create simulation directory path
        sim_dir = Path("simulations") / args.name
        
        # Check if simulation exists
        if not sim_dir.exists():
            print(f"Error: Simulation '{args.name}' not found")
            return 1
        
        # Load simulation configuration
        config_file = sim_dir / "config.json"
        if not config_file.exists():
            print(f"Error: Configuration file not found for '{args.name}'")
            return 1
        
        # Create brain from config
        config = pynlm.createDefaultConfig()
        config.loadFromFile(str(config_file))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create profiler
        profiler = PerformanceProfiler("nlm_profile")
        
        # Start profiling session
        profiler.start_session("simulation")
        
        # Run simulation with profiling
        @profiler.profile_operation("simulation", "brain_step")
        def step_brain(brain, step):
            brain.step(step)
        
        # Run simulation
        for i in range(args.steps):
            step_brain(brain, i)
        
        # End profiling session
        profiler.end_session("simulation")
        
        # Get results
        results = profiler.get_session_results("simulation")
        
        # Save results
        if args.output:
            profiler.export_results(args.output)
            print(f"Profiling results saved: {args.output}")
        else:
            # Print results
            print(f"Simulation duration: {results['total_duration']:.2f} seconds")
            print(f"Number of operations: {results['num_operations']}")
            print(f"Average operation time: {results['avg_operation_time']:.4f} seconds")
        
    except ImportError as e:
        print(f"Error importing profiling module: {e}")
        return 1
    except Exception as e:
        print(f"Error profiling simulation: {e}")
        return 1
    
    return 0
def handle_benchmark(args):
    """Handle the benchmark command"""
    print(f"Running NLM benchmark...")
    
    try:
        from tests.test_suite import run_all_tests
        
        # Run benchmark tests
        result = run_all_tests()
        
        # Save results
        if args.output:
            # Convert result to serializable format
            results = {
                "tests_run": result.testsRun,
                "failures": [(test.methodName, str(fail)) for test, fail in result.failures],
                "errors": [(test.methodName, str(error)) for test, error in result.errors],
                "successful": len(result.success_list)
            }
            
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            
            print(f"Benchmark results saved: {args.output}")
        
    except Exception as e:
        print(f"Error running benchmark: {e}")
        return 1
    
    return 0
def main():
    """Main entry point"""
    parser = create_parser()
    args = parser.parse_args()
    
    # Set up logging
    import logging
    if args.quiet:
        logging.basicConfig(level=logging.WARNING)
    else:
        logging.basicConfig(level=logging.INFO)
    
    # Set up output directory
    if args.output:
        output_dir = Path(args.output)
        output_dir.mkdir(parents=True, exist_ok=True)
    
    # Fast configuration if requested
    if args.fast:
        print("Using fast configuration...")
        # Create a minimal config
        config = pynlm.createDefaultConfig()
        config.setInt("brain.neuron_count", 50)
        config.setInt("brain.region_count", 1)
        # Additional fast settings...
    
    # Handle different commands
    if args.command == "create":
        handle_create(args)
    elif args.command == "run":
        return handle_run(args)
    elif args.command == "test":
        return handle_test(args)
    elif args.command == "config":
        return handle_config(args)
    elif args.command == "viz":
        return handle_viz(args)
    elif args.command == "profile":
        return handle_profile(args)
    elif args.command == "benchmark":
        return handle_benchmark(args)
    else:
        parser.print_help()
        return 1
    
    return 0
if __name__ == "__main__":
    sys.exit(main())