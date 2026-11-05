#!/usr/bin/env python3
"""
Command Line Interface for NLM Python Bindings

Provides command-line access to NLM functionality for scripting and automation.
"""

import argparse
import sys
import os
import time
import statistics
from typing import Optional

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm import (
    create_default_config, create_brain, create_simple_world,
    create_agent_brain, SimulationLoop, format_action,
    parse_action_string, format_duration
)

class NLMTool:
    """Command-line tool for NLM operations"""
    
    def __init__(self):
        self.parser = self._create_parser()
    
    def _create_parser(self) -> argparse.ArgumentParser:
        """Create argument parser"""
        parser = argparse.ArgumentParser(
            description="NLM (Neural Learning Machine) Python Bindings Tool",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  %(prog)s demo --type basic --steps 20
  %(prog)s simulate --world-width 800 --world-height 600 --steps 50
  %(prog)s analyze --input checkpoint.json --output report.json
  %(prog)s benchmark --runs 5 --steps 100
            """
        )
        
        # Global options
        parser.add_argument(
            "--verbose", "-v",
            action="store_true",
            help="Enable verbose output"
        )
        
        # Subcommands
        subparsers = parser.add_subparsers(dest="command", help="Available commands")
        
        # Demo command
        demo_parser = subparsers.add_parser(
            "demo",
            help="Run a demonstration simulation"
        )
        demo_parser.add_argument(
            "--type", "-t",
            choices=["basic", "agent", "advanced"],
            default="basic",
            help="Type of demonstration (default: basic)"
        )
        demo_parser.add_argument(
            "--steps", "-s",
            type=int,
            default=10,
            help="Number of simulation steps (default: 10)"
        )
        demo_parser.add_argument(
            "--output",
            help="Output file for results"
        )
        
        # Simulate command
        simulate_parser = subparsers.add_parser(
            "simulate",
            help="Run a custom simulation"
        )
        simulate_parser.add_argument(
            "--world-width", "-W",
            type=int,
            default=640,
            help="World width (default: 640)"
        )
        simulate_parser.add_argument(
            "--world-height", "-H",
            type=int,
            default=480,
            help="World height (default: 480)"
        )
        simulate_parser.add_argument(
            "--steps", "-s",
            type=int,
            default=20,
            help="Number of simulation steps (default: 20)"
        )
        simulate_parser.add_argument(
            "--agent-mode",
            action="store_true",
            help="Enable agent-based simulation"
        )
        simulate_parser.add_argument(
            "--output",
            help="Output file for simulation results"
        )
        
        # Analyze command
        analyze_parser = subparsers.add_parser(
            "analyze",
            help="Analyze brain state or simulation results"
        )
        analyze_parser.add_argument(
            "--input",
            help="Input file (checkpoint, results, etc.)"
        )
        analyze_parser.add_argument(
            "--output",
            help="Output file for analysis results"
        )
        analyze_parser.add_argument(
            "--format",
            choices=["json", "text", "csv"],
            default="text",
            help="Output format (default: text)"
        )
        
        # Benchmark command
        benchmark_parser = subparsers.add_parser(
            "benchmark",
            help="Benchmark NLM performance"
        )
        benchmark_parser.add_argument(
            "--runs", "-r",
            type=int,
            default=3,
            help="Number of benchmark runs (default: 3)"
        )
        benchmark_parser.add_argument(
            "--steps", "-s",
            type=int,
            default=50,
            help="Steps per benchmark run (default: 50)"
        )
        benchmark_parser.add_argument(
            "--config",
            help="Configuration file for benchmark"
        )
        
        # Version command
        version_parser = subparsers.add_parser(
            "version",
            help="Show version information"
        )
        
        return parser
    
    def run_demo(self, args) -> bool:
        """Run a demonstration"""
        print(f"Running {args.type} demo for {args.steps} steps...")
        
        # Create simulation
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        agent_brain = create_agent_brain(brain, world)
        
        world.configure(640, 480, 320, 240)
        brain.initialize()
        agent_brain.initialize(world)
        
        # Create simulation loop
        loop = SimulationLoop(brain, world, agent_brain)
        
        # Run demonstration
        actions = loop.run_episode(args.steps)
        
        # Display results
        print(f"\nDemo Results:")
        print(f"  Steps completed: {len(actions)}")
        print(f"  Performance: {loop.get_performance_stats()['steps_per_second']:.1f} steps/sec")
        print(f"  Action types used: {len(set(action.getType() for action in actions))}")
        
        # Save results if requested
        if args.output:
            self._save_results(args.output, loop, actions)
        
        return True
    
    def run_simulate(self, args) -> bool:
        """Run a custom simulation"""
        print(f"Running simulation with world {args.world_width}x{args.world_height} for {args.steps} steps...")
        
        # Create simulation
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        agent_brain = create_agent_brain(brain, world)
        
        world.configure(args.world_width, args.world_height, 
                       args.world_width // 2, args.world_height // 2)
        brain.initialize()
        agent_brain.initialize(world)
        
        # Create simulation loop
        loop = SimulationLoop(brain, world, agent_brain)
        
        # Run simulation
        actions = loop.run_episode(args.steps)
        
        # Display results
        print(f"\nSimulation Results:")
        print(f"  Steps completed: {len(actions)}")
        print(f"  Performance: {loop.get_performance_stats()['steps_per_second']:.1f} steps/sec")
        print(f"  Total time: {format_duration(loop.get_performance_stats()['total_time'])}")
        
        # Show action summary
        action_counts = {}
        for action in actions:
            action_name = action.getName()
            action_counts[action_name] = action_counts.get(action_name, 0) + 1
        
        print(f"\n  Action Summary:")
        for action_name, count in sorted(action_counts.items(), key=lambda x: x[1], reverse=True):
            print(f"    {action_name}: {count}")
        
        # Save results if requested
        if args.output:
            self._save_results(args.output, loop, actions)
        
        return True
    
    def run_analyze(self, args) -> bool:
        """Analyze input or generate analysis"""
        print("Running analysis...")
        
        if args.input:
            # Analyze input file
            if args.input.endswith('.json'):
                print(f"  Analyzing checkpoint file: {args.input}")
                # Implementation would load and analyze the checkpoint
                print("  (Checkpoint analysis not implemented in this demo)")
            else:
                print(f"  Unknown input file format: {args.input}")
        
        # Generate analysis report
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        agent_brain = create_agent_brain(brain, world)
        
        world.configure(640, 480, 320, 240)
        brain.initialize()
        agent_brain.initialize(world)
        
        loop = SimulationLoop(brain, world, agent_brain)
        loop.run_episode(10)
        
        stats = loop.get_brain_stats()
        
        # Save analysis if requested
        if args.output:
            if args.format == 'json':
                import json
                with open(args.output, 'w') as f:
                    json.dump(stats, f, indent=2)
                print(f"  Analysis saved to {args.output} (JSON format)")
            elif args.format == 'csv':
                # CSV implementation would go here
                print(f"  CSV format not yet implemented for {args.output}")
            else:
                with open(args.output, 'w') as f:
                    f.write(str(stats))
                print(f"  Analysis saved to {args.output}")
        
        print(f"  Analysis complete with {len(stats)} data points")
        return True
    
    def run_benchmark(self, args) -> bool:
        """Run performance benchmark"""
        print(f"Running benchmark: {args.runs} runs of {args.steps} steps each...")
        
        # Create simulation
        config = create_default_config()
        brain = create_brain(config)
        world = create_simple_world()
        agent_brain = create_agent_brain(brain, world)
        
        world.configure(640, 480, 320, 240)
        brain.initialize()
        agent_brain.initialize(world)
        
        # Run benchmark
        times = []
        for run in range(args.runs):
            print(f"  Run {run + 1}/{args.runs}...", end=" ")
            loop = SimulationLoop(brain, world, agent_brain)
            start_time = time.time()
            loop.run_episode(args.steps)
            end_time = time.time()
            
            duration = end_time - start_time
            times.append(duration)
            print(f"{format_duration(duration)}")
        
        # Calculate statistics
        avg_time = sum(times) / len(times)
        min_time = min(times)
        max_time = max(times)
        std_time = statistics.stdev(times) if len(times) > 1 else 0
        
        # Display results
        print(f"\nBenchmark Results:")
        print(f"  Runs: {args.runs}")
        print(f"  Steps per run: {args.steps}")
        print(f"  Average time: {format_duration(avg_time)}")
        print(f"  Min time: {format_duration(min_time)}")
        print(f"  Max time: {format_duration(max_time)}")
        print(f"  Std dev: {format_duration(std_time)}")
        print(f"  Average steps/sec: {args.steps / avg_time:.1f}")
        print(f"  Min steps/sec: {args.steps / max_time:.1f}")
        print(f"  Max steps/sec: {args.steps / min_time:.1f}")
        
        return True
    
    def run_version(self, args) -> bool:
        """Show version information"""
        from nlm import __version__, __author__
        print(f"NLM Python Bindings v{__version__}")
        print(f"Author: {__author__}")
        print(f"License: MIT")
        return True
    
    def _save_results(self, filepath: str, loop, actions):
        """Save simulation results to file"""
        print(f"  Saving results to {filepath}...")
        
        results = {
            'steps': loop.step_count,
            'actions': [format_action(action) for action in actions],
            'performance': loop.get_performance_stats(),
            'brain_stats': loop.get_brain_stats()
        }
        
        import json
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"  Results saved successfully")
    
    def run(self, argv) -> bool:
        """Run the tool with command line arguments"""
        args = self.parser.parse_args(argv)
        
        if not args.command:
            self.parser.print_help()
            return False
        
        try:
            if args.command == "demo":
                return self.run_demo(args)
            elif args.command == "simulate":
                return self.run_simulate(args)
            elif args.command == "analyze":
                return self.run_analyze(args)
            elif args.command == "benchmark":
                return self.run_benchmark(args)
            elif args.command == "version":
                return self.run_version(args)
            else:
                print(f"Unknown command: {args.command}")
                return False
        except KeyboardInterrupt:
            print("\nInterrupted by user")
            return False
        except Exception as e:
            print(f"Error: {e}")
            import traceback
            traceback.print_exc()
            return False


def main():
    """Main entry point"""
    tool = NLMTool()
    
    # If no arguments provided, show help
    if len(sys.argv) == 1:
        tool.parser.print_help()
        return 1
    
    # Run tool
    success = tool.run(sys.argv[1:])
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
