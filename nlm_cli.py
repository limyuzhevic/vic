#!/usr/bin/env python3
"""NLM Command Line Interface

Simple command-line interface for common NLM operations.
Useful for quick experiments, scripting, and automation.
"""

import sys
import argparse
import time
import json
import os
from pathlib import Path

# Add src to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    import pynlm
except ImportError:
    print("Error: pynlm not found. Please install NLM first:")
    print("  pip install pynlm")
    sys.exit(1)

def create_brain(config_file=None, neurons=500, regions=1):
    """Create a new brain with specified configuration"""
    config = pynlm.createDefaultConfig()
    
    if config_file and os.path.exists(config_file):
        try:
            config.loadFromFile(config_file)
            print(f"Loaded configuration from: {config_file}")
        except Exception as e:
            print(f"Warning: Could not load config file: {e}")
    
    # Override with command line parameters
    config.set("neuron_count", neurons)
    config.set("region_count", regions)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain, config

def run_simple_simulation(brain, world, agent, steps):
    """Run a simple simulation with optional progress reporting"""
    print(f"Running simulation for {steps} steps...")
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Progress reporting every 25% or every 100 steps, whichever comes first
        if step % max(1, steps // 4) == 0 or step % 100 == 0:
            print(f"  Step {step + 1}/{steps}: "
                  f"{brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} spikes")
    
    print("Simulation completed!")

def export_brain_stats(brain, output_file=None):
    """Export brain statistics to file or stdout"""
    stats = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio(),
        "development_stage": brain.getDevelopmentalStage().name,
        "region_count": brain.getRegionCount(),
    }
    
    if output_file:
        with open(output_file, 'w') as f:
            json.dump(stats, f, indent=2)
        print(f"Brain statistics saved to: {output_file}")
    else:
        print("Brain Statistics:")
        for key, value in stats.items():
            print(f"  {key.replace('_', ' ').title()}: {value}")
    
    return stats

def create_demo_agent(config_file=None):
    """Create a demo agent with typical configuration"""
    brain, _ = create_brain(config_file, neurons=1000, regions=2)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable typical learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    return brain, world, agent

def main():
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli create --neurons 500 --regions 1
  nlm-cli demo --steps 1000
  nlm-cli export stats.json
  nlm-cli benchmark --neurons 100 --steps 500
  nlm-cli batch test --config configs/default.cfg

For detailed usage, see the NLM documentation.
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create a new brain')
    create_parser.add_argument('--neurons', type=int, default=500, help='Number of neurons')
    create_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    create_parser.add_argument('--config', help='Configuration file path')
    create_parser.add_argument('--output', help='Save brain to file')
    
    # Demo command
    demo_parser = subparsers.add_parser('demo', help='Run demo simulation')
    demo_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    demo_parser.add_argument('--neurons', type=int, default=500, help='Number of neurons')
    demo_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    demo_parser.add_argument('--config', help='Configuration file path')
    
    # Export command
    export_parser = subparsers.add_parser('export', help='Export brain statistics')
    export_parser.add_argument('action', choices=['stats', 'brain', 'config'], help='Export action')
    export_parser.add_argument('--output', help='Output file (default: stdout or auto-named file)')
    
    # Benchmark command
    bench_parser = subparsers.add_parser('benchmark', help='Benchmark performance')
    bench_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons')
    bench_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    bench_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    bench_parser.add_argument('--output', help='Save benchmark results to file')
    
    # Batch command
    batch_parser = subparsers.add_parser('batch', help='Batch operations')
    batch_subparsers = batch_parser.add_subparsers(dest='batch_command', help='Batch operations')
    
    test_parser = batch_subparsers.add_parser('test', help='Run batch tests')
    test_parser.add_argument('--config', help='Configuration file path')
    test_parser.add_argument('--steps', type=int, default=50, help='Steps per test')
    test_parser.add_argument('--neurons', type=int, default=100, help='Neurons per test')
    test_parser.add_argument('--count', type=int, default=3, help='Number of tests to run')
    
    # Interactive mode
    interactive_parser = subparsers.add_parser('interactive', help='Start interactive mode')
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    try:
        if args.command == 'create':
            print("Creating brain...")
            brain, config = create_brain(args.config, args.neurons, args.regions)
            
            print(f"Brain created successfully!")
            print(f"  - Neurons: {brain.getTotalNeuronCount()}")
            print(f"  - Regions: {brain.getRegionCount()}")
            print(f"  - Synapses: {brain.getTotalSynapseCount()}")
            
            if args.output:
                brain.save(args.output)
                print(f"Brain saved to: {args.output}")
            
        elif args.command == 'demo':
            print("Creating demo brain...")
            brain, world, agent = create_demo_agent(args.config)
            
            run_simple_simulation(brain, world, agent, args.steps)
            
            # Show final statistics
            print("\n=== Demo Results ===")
            export_brain_stats(brain)
            
        elif args.command == 'export':
            if args.action == 'stats':
                # Create a simple brain for export demo
                brain, _, _ = create_brain()
                export_brain_stats(brain, args.output)
                
        elif args.command == 'benchmark':
            print("Running benchmark...")
            print(f"Configuration: {args.neurons} neurons, {args.regions} regions, {args.steps} steps")
            
            results = []
            
            for i in range(3):  # Run 3 times for averaging
                print(f"  Run {i + 1}/3...")
                brain, world, agent = create_demo_agent()
                
                start_time = time.time()
                for step in range(args.steps):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                end_time = time.time()
                
                steps_per_sec = args.steps / (end_time - start_time)
                results.append({
                    'run': i + 1,
                    'time': end_time - start_time,
                    'steps_per_second': steps_per_sec,
                    'neurons_firing': brain.getFiringNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_rate': brain.getAverageFiringRate()
                })
                
                print(f"    Time: {results[-1]['time']:.2f}s, "
                      f"Speed: {results[-1]['steps_per_second']:.1f} steps/s")
            
            # Calculate averages
            avg_time = sum(r['time'] for r in results) / len(results)
            avg_speed = sum(r['steps_per_second'] for r in results) / len(results)
            avg_firing_rate = sum(r['firing_rate'] for r in results) / len(results)
            
            print(f"\n=== Benchmark Results ===")
            print(f"Average time: {avg_time:.2f}s")
            print(f"Average speed: {avg_speed:.1f} steps/second")
            print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump({
                        'config': {
                            'neurons': args.neurons,
                            'regions': args.regions,
                            'steps': args.steps
                        },
                        'results': results,
                        'averages': {
                            'time': avg_time,
                            'speed': avg_speed,
                            'firing_rate': avg_firing_rate
                        }
                    }, f, indent=2)
                print(f"Benchmark results saved to: {args.output}")
            
        elif args.command == 'batch':
            if args.batch_command == 'test':
                print(f"Running {args.count} batch tests...")
                
                for i in range(args.count):
                    print(f"\nTest {i + 1}/{args.count}:")
                    brain, world, agent = create_demo_agent(args.config)
                    
                    run_simple_simulation(brain, world, agent, args.steps)
                    
                    stats = export_brain_stats(brain)
                    
                    print(f"  ✓ Test {i + 1} completed")
                
                print(f"\n✓ All {args.count} batch tests completed successfully!")
            
        elif args.command == 'interactive':
            print("Starting interactive mode...")
            print("Available commands:")
            print("  create <neurons> [regions]  - Create a new brain")
            print("  demo <steps>                 - Run demo simulation")
            print("  export stats                 - Show brain statistics")
            print("  benchmark <steps>            - Run performance benchmark")
            print("  help                         - Show help")
            print("  exit                         - Exit interactive mode")
            print()
            
            # Simple interactive loop
            while True:
                try:
                    user_input = input("nlm-cli> ").strip()
                    if not user_input:
                        continue
                    
                    parts = user_input.split()
                    cmd = parts[0]
                    
                    if cmd == 'help':
                        print("Available commands:")
                        print("  create <neurons> [regions]  - Create a new brain")
                        print("  demo <steps>                 - Run demo simulation")
                        print("  export stats                 - Show brain statistics")
                        print("  benchmark <steps>            - Run performance benchmark")
                        print("  help                         - Show help")
                        print("  exit                         - Exit interactive mode")
                    
                    elif cmd == 'create':
                        neurons = int(parts[1]) if len(parts) > 1 else 500
                        regions = int(parts[2]) if len(parts) > 2 else 1
                        brain, _ = create_brain(None, neurons, regions)
                        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
                    
                    elif cmd == 'demo':
                        steps = int(parts[1]) if len(parts) > 1 else 100
                        brain, world, agent = create_demo_agent()
                        run_simple_simulation(brain, world, agent, steps)
                        export_brain_stats(brain)
                    
                    elif cmd == 'export':
                        if len(parts) > 1 and parts[1] == 'stats':
                            brain, _, _ = create_brain()
                            export_brain_stats(brain)
                        else:
                            print("Usage: export stats")
                    
                    elif cmd == 'benchmark':
                        steps = int(parts[1]) if len(parts) > 1 else 100
                        print(f"Running benchmark with {steps} steps...")
                        brain, world, agent = create_demo_agent()
                        
                        start_time = time.time()
                        for step in range(steps):
                            world.update(0.1)
                            agent.processSensoryInput(world.getSensoryPercept())
                            brain.step(step)
                            action = agent.decodeMotorCommand()
                            world.applyMotorCommand(action, world.getSimulationTime())
                        end_time = time.time()
                        
                        steps_per_sec = steps / (end_time - start_time)
                        print(f"Performance: {steps_per_sec:.1f} steps/second")
                    
                    elif cmd == 'exit':
                        print("Exiting interactive mode.")
                        break
                    
                    else:
                        print(f"Unknown command: {cmd}")
                        print("Type 'help' for available commands")
                
                except KeyboardInterrupt:
                    print("\nInterrupted. Exiting interactive mode.")
                    break
                
                except Exception as e:
                    print(f"Error: {e}")
    
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
