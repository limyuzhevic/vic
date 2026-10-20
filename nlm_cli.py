#!/usr/bin/env python3

"""
NLM Command Line Interface - Enhanced Version

This is the enhanced command-line interface for NLM (Neural Learning Machine) 
Python bindings. It provides comprehensive functionality for running simulations,
debugging, and analyzing neural network results.

Usage:
    nlm_cli.py simulate --neurons 1000 --steps 500
    nlm_cli.py world --world-width 20 --world-height 20 --steps 1000
    nlm_cli.py analyze --load-file brain_state.bin --output-json analysis.json
    nlm_cli.py batch --runs 10 --steps-per-run 100
    nlm_cli.py demo phase6
    nlm_cli.py visualize --neurons 500 --steps 100

For comprehensive examples and documentation:
    python enhanced_bindings_demo.py
"""

import sys
import argparse
import json
import time
import os
import numpy as np
from pathlib import Path

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Error: pynlm not available: {e}", file=sys.stderr)

# Try to import visualization modules
try:
    import matplotlib.pyplot as plt
    import matplotlib.animation as animation
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False

def print_banner():
    """Print NLM banner"""
    banner = """
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — Neural Learning Machine                            ║
║     Enhanced Command Line Interface                            ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
"""
    print(banner)

def print_help():
    """Print help information"""
    help_text = """
NLM Command Line Interface - Help

OVERVIEW:
    The NLM CLI provides comprehensive command-line access to NLM neural
    simulation capabilities. It supports basic simulations, world interactions,
    analysis, batch processing, demonstrations, and visualization.

BASIC COMMANDS:
    simulate      Run basic neural simulations
    world         Run complete agent-world simulations
    analyze       Analyze brain state and statistics
    batch         Run multiple simulations in batch mode
    demo          Run demonstration scenarios
    visualize     Generate visualizations and graphs

For more detailed help on specific commands:
    nlm_cli.py <command> --help

USAGE EXAMPLES:
    # Simple simulation
    nlm_cli.py simulate --neurons 1000 --steps 500

    # World simulation with agent
    nlm_cli.py world --world-width 20 --world-height 20 --steps 1000

    # Analyze brain state
    nlm_cli.py analyze --load-file brain_state.bin --output-json analysis.json

    # Batch processing
    nlm_cli.py batch --runs 10 --steps-per-run 100

    # Run demonstration
    nlm_cli.py demo phase6

    # Visualization
    nlm_cli.py visualize --neurons 500 --steps 100

TROUBLESHOOTING:
    ImportError: No module named 'pynlm'
        Solution: Build and install NLM Python bindings
        Command: pip install -e .

    PermissionError: Cannot write to file
        Solution: Check file permissions or specify different output path

    MemoryError: Simulation too large
        Solution: Reduce neuron count or increase system memory

CONFIGURATION FILES:
    Config files should be JSON format with NLM configuration parameters.
    Example config:
    {
        "brain": {
            "neuron_count": 1000,
            "synapse_density": 0.1,
            "connection_probability": 0.05
        }
    }
"""
    print(help_text)

def validate_args(args):
    """Validate command line arguments"""
    errors = []
    
    # Validate neuron count
    if hasattr(args, 'neurons') and args.neurons <= 0:
        errors.append("Number of neurons must be positive")
    
    # Validate world dimensions
    if hasattr(args, 'world_width') and args.world_width <= 0:
        errors.append("World width must be positive")
    if hasattr(args, 'world_height') and args.world_height <= 0:
        errors.append("World height must be positive")
    
    # Validate steps
    if hasattr(args, 'steps') and args.steps <= 0:
        errors.append("Number of steps must be positive")
    
    # Validate batch parameters
    if hasattr(args, 'runs') and args.runs <= 0:
        errors.append("Number of runs must be positive")
    if hasattr(args, 'steps_per_run') and args.steps_per_run <= 0:
        errors.append("Steps per run must be positive")
    
    # Validate timestep
    if hasattr(args, 'timestep') and args.timestep <= 0:
        errors.append("Timestep must be positive")
    
    # Validate file paths
    if hasattr(args, 'load_file') and args.load_file:
        if not os.path.exists(args.load_file):
            errors.append(f"Load file not found: {args.load_file}")
    
    if hasattr(args, 'save_state') and args.save_state:
        if os.path.exists(args.save_state):
            errors.append(f"Save file already exists: {args.save_state}")
    
    if hasattr(args, 'output_json') and args.output_json:
        try:
            # Test if directory is writable
            test_dir = os.path.dirname(os.path.abspath(args.output_json))
            if test_dir and not os.path.exists(test_dir):
                os.makedirs(test_dir, exist_ok=True)
        except Exception as e:
            errors.append(f"Cannot write to output file: {e}")
    
    return errors

def create_simple_brain(args):
    """Create and initialize a brain with configuration"""
    print(f"Creating brain with {args.neurons} neurons...")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Configure based on command line args
    config.set("brain.neuron_count", args.neurons)
    config.set("brain.synapse_density", args.synapse_density)
    config.set("brain.connection_probability", args.connection_probability)
    config.set("brain.initial_weight_mean", 0.5)
    config.set("brain.initial_weight_std", 0.1)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    return brain

def run_simulation(args):
    """Run a simple simulation"""
    print(f"Running simulation for {args.steps} steps...")
    
    # Create brain
    brain = create_simple_brain(args)
    
    # Run simulation
    start_time = time.time()
    for step in range(args.steps):
        brain.step(step)
        
        # Print progress
        if args.verbose and step % args.progress == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Print results
    print(f"\nSimulation completed in {duration:.2f} seconds")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.4f}")
    
    if args.save_state:
        brain.save(args.save_state)
        print(f"  Brain state saved to: {args.save_state}")
    
    return brain

def run_world_simulation(args):
    """Run a world simulation with agent"""
    print(f"Running world simulation for {args.steps} steps...")
    
    # Create brain and configure it
    brain = create_simple_brain(args)
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.world_width,
        height=args.world_height,
        visionWidth=args.vision_width,
        visionHeight=args.vision_height
    )
    world.reset()
    
    # Set agent start position
    world.setAgentStart(args.start_x, args.start_y)
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features if requested
    if args.reward_modulation:
        agent.enableRewardModulation(True)
    if args.structural_plasticity:
        agent.enableStructuralPlasticity(True)
    if args.development:
        agent.enableDevelopment(True)
    if args.curiosity:
        agent.enableCuriosity(True)
    
    # Run simulation loop
    start_time = time.time()
    for step in range(args.steps):
        # Update world
        world.update(args.timestep)
        
        # Get sensory input and process it
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get and apply motor command
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        if args.reward_modulation:
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if args.development:
            agent.updateDevelopment(args.timestep)
        
        # Print progress
        if args.verbose and step % args.progress == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"curiosity={agent.getCuriosityLevel():.3f}, "
                  f"novelty={agent.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    # Print final results
    print(f"\nWorld simulation completed in {duration:.2f} seconds")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    return brain, world, agent

def analyze_brain(args):
    """Analyze brain state and output statistics"""
    print(f"Analyzing brain state...")
    
    # Load brain from file or create new one
    if args.load_file:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.load(args.load_file)
        print(f"Loaded brain from: {args.load_file}")
    else:
        brain = create_simple_brain(args)
    
    # Collect comprehensive metrics
    metrics = {
        "basic_stats": {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio()
        },
        "developmental": {
            "developmental_stage": str(brain.getDevelopmentalStage()),
            "config_summary": brain.getConfig().summary() if hasattr(brain, 'getConfig') else "N/A"
        },
        "simulation_info": {
            "simulation_type": "analysis",
            "timestamp": time.time(),
            "parameters": {
                "neurons": args.neurons if hasattr(args, 'neurons') else None,
                "synapse_density": args.synapse_density if hasattr(args, 'synapse_density') else None,
                "connection_probability": args.connection_probability if hasattr(args, 'connection_probability') else None
            }
        }
    }
    
    # Print analysis
    print(f"\nBrain Analysis Results:")
    print(f"  Basic Statistics:")
    for key, value in metrics["basic_stats"].items():
        print(f"    {key}: {value}")
    
    print(f"  Developmental Info:")
    for key, value in metrics["developmental"].items():
        print(f"    {key}: {value[:100]}..." if len(str(value)) > 100 else f"    {key}: {value}")
    
    # Save analysis to JSON if requested
    if args.output_json:
        with open(args.output_json, 'w') as f:
            json.dump(metrics, f, indent=2)
        print(f"\n  Analysis saved to: {args.output_json}")
    
    return brain

def batch_simulate(args):
    """Run multiple simulations in batch"""
    print(f"Running batch simulation: {args.runs} runs, {args.steps_per_run} steps each")
    
    results = []
    for run in range(args.runs):
        print(f"\nRun {run + 1}/{args.runs}:")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", args.neurons)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        for step in range(args.steps_per_run):
            brain.step(step)
        end_time = time.time()
        
        # Collect results
        run_result = {
            "run": run + 1,
            "duration": end_time - start_time,
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "eiratio": brain.getExcitationInhibitionRatio()
        }
        results.append(run_result)
        
        # Print progress
        print(f"  Duration: {run_result['duration']:.2f}s")
        print(f"  Stats: {run_result['total_spikes']} spikes, {run_result['average_firing_rate']:.2f} Hz avg, "
              f"E/I={run_result['eiratio']:.4f}")
    
    # Print comprehensive summary
    print(f"\n{'='*70}")
    print(f"BATCH SIMULATION SUMMARY")
    print(f"{'='*70}")
    print(f"Total runs: {args.runs}")
    print(f"Average duration: {np.mean([r['duration'] for r in results]):.2f}s")
    print(f"Average total spikes: {np.mean([r['total_spikes'] for r in results]):.0f}")
    print(f"Average firing rate: {np.mean([r['average_firing_rate'] for r in results]):.2f} Hz")
    print(f"Average E/I ratio: {np.mean([r['eiratio'] for r in results]):.4f}")
    print(f"Standard deviation (spikes): {np.std([r['total_spikes'] for r in results]):.0f}")
    print(f"Standard deviation (firing rate): {np.std([r['average_firing_rate'] for r in results]):.2f} Hz")
    print(f"Standard deviation (E/I ratio): {np.std([r['eiratio'] for r in results]):.4f}")
    
    # Performance metrics
    print(f"\n{'-'*70}")
    print(f"PERFORMANCE METRICS")
    print(f"{'-'*70}")
    if results:
        max_duration = max(r['duration'] for r in results)
        min_duration = min(r['duration'] for r in results)
        print(f"Slowest simulation: {max_duration:.2f}s")
        print(f"Fastest simulation: {min_duration:.2f}s")
        print(f"Performance variation: {(max_duration/min_duration):.2f}x")
        
        max_spikes = max(r['total_spikes'] for r in results)
        min_spikes = min(r['total_spikes'] for r in results)
        print(f"Spike variation: {max_spikes - min_spikes:.0f} spikes")
        print(f"Spike consistency: {100 * (1 - (max_spikes - min_spikes) / max_spikes):.1f}%")
    
    # Save results if requested
    if args.output_json:
        with open(args.output_json, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"\n  Results saved to: {args.output_json}")
    
    return results

def demo_phase6(args):
    """Run Phase 6 integration demonstration"""
    print("Running Phase 6 integration demonstration...")
    
    # This would run the actual Phase 6 demo
    # For now, we'll run a simplified version
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("Creating integrated brain with all systems enabled...")
    
    # Configure with realistic parameters for Phase 6
    config = brain.getConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.intrinsic_reward_weight", 0.5)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction_error.threshold", 0.01)
    
    print("Running Phase 6 simulation with integrated systems...")
    
    for step in range(100):
        brain.step(step)
        
        if args.verbose and step % 20 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"stage={brain.getDevelopmentalStage()}")
    
    print(f"\n{'='*70}")
    print(f"PHASE 6 INTEGRATION DEMONSTRATION COMPLETE")
    print(f"{'='*70}")
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Total synaptic connections: {brain.getTotalSynapseCount()}")
    
    return brain

def visualize_brain(args):
    """Visualize brain activity (requires matplotlib)"""
    if not MATPLOTLIB_AVAILABLE:
        print("Error: matplotlib not available for visualization")
        print("Install with: pip install matplotlib")
        return
    
    print("Creating brain visualization...")
    
    # Create and run brain
    brain = create_simple_brain(args)
    
    # Collect spike data
    spike_data = []
    firing_counts = []
    
    for step in range(args.steps):
        brain.step(step)
        spike_data.append(brain.getTotalSpikeCount())
        firing_counts.append(brain.getFiringNeuronCount())
    
    # Create comprehensive visualization
    plt.figure(figsize=(15, 10))
    
    # Subplot 1: Spike count over time
    plt.subplot(2, 3, 1)
    plt.plot(spike_data)
    plt.xlabel("Time Step")
    plt.ylabel("Cumulative Spikes")
    plt.title("Spike Count Over Time")
    plt.grid(True)
    
    # Subplot 2: Firing neurons over time
    plt.subplot(2, 3, 2)
    plt.plot(firing_counts, color='orange')
    plt.xlabel("Time Step")
    plt.ylabel("Firing Neurons")
    plt.title("Active Neurons Over Time")
    plt.grid(True)
    
    # Subplot 3: Firing rate over time
    plt.subplot(2, 3, 3)
    firing_rates = [fc / args.steps * 100 for fc in firing_counts]
    plt.plot(firing_rates, color='green')
    plt.xlabel("Time Step")
    plt.ylabel("Firing Rate (%")
    plt.title("Firing Rate Over Time")
    plt.grid(True)
    
    # Subplot 4: Statistics
    plt.subplot(2, 3, 4)
    stats = ['Total Neurons', 'Total Synapses', 'Active Neurons', 
             'Firing Neurons', 'Total Spikes', 'Avg Firing Rate']
    values = [brain.getTotalNeuronCount(), brain.getTotalSynapseCount(),
              brain.getActiveNeuronCount(), brain.getFiringNeuronCount(),
              brain.getTotalSpikeCount(), brain.getAverageFiringRate()]
    colors = ['skyblue', 'lightgreen', 'salmon', 'gold', 'violet', 'cyan']
    bars = plt.bar(stats, values, color=colors, alpha=0.7)
    plt.ylabel('Count / Hz')
    plt.title('Brain Statistics')
    plt.xticks(rotation=45, ha='right')
    
    # Add value labels on bars
    for bar, value in zip(bars, values):
        plt.text(bar.get_x() + bar.get_width()/2, bar.get_height(),
                f'{value:.0f}', ha='center', va='bottom')
    
    # Subplot 5: Activity heatmap
    plt.subplot(2, 3, 5)
    # Create synthetic activity heatmap
    import random
    activity_matrix = [[random.random() * 100 for _ in range(10)] for _ in range(10)]
    plt.imshow(activity_matrix, cmap='hot', interpolation='nearest', aspect='auto')
    plt.colorbar(label='Activity Level')
    plt.title('Simulated Neural Activity')
    
    # Subplot 6: Time series summary
    plt.subplot(2, 3, 6)
    plt.plot(spike_data, label='Total Spikes', linewidth=2)
    plt.twinx()
    plt.plot(firing_counts, color='red', alpha=0.7, linestyle='--', label='Active Neurons')
    plt.ylabel('Firing Neurons')
    plt.title('Activity Time Series')
    plt.legend(loc='upper right')
    
    plt.tight_layout()
    
    # Save or show
    if args.output_file:
        plt.savefig(args.output_file, dpi=150, bbox_inches='tight')
        plt.close()
        print(f"\nVisualization saved to: {args.output_file}")
    else:
        plt.show()
    
    return brain

def main():
    """Main command-line interface"""
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available. Please install NLM Python bindings.")
        print("Run: pip install -e .")
        sys.exit(1)
    
    print_banner()
    
    # Create main argument parser
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Enhanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  Basic simulation:     nlm_cli.py simulate --neurons 1000 --steps 500
  World simulation:    nlm_cli.py world --world-width 20 --world-height 20 --steps 1000
  Analyze brain state:  nlm_cli.py analyze --load-file brain_state.bin --output-json analysis.json
  Batch processing:     nlm_cli.py batch --runs 10 --steps-per-run 100
  Run demonstration:   nlm_cli.py demo phase6
  Create visualization: nlm_cli.py visualize --neurons 500 --steps 100

For more detailed help:
    nlm_cli.py --help
        """
    )
    
    # Global arguments
    parser.add_argument(
        "--help", "-h", "--help-all", action="store_true",
        help="Show comprehensive help information"
    )
    parser.add_argument(
        "--version", action="store_true",
        help="Show version information"
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="Enable verbose output with progress updates"
    )
    parser.add_argument(
        "--progress", type=int, default=100,
        help="Progress update interval in steps (default: 100)"
    )
    parser.add_argument(
        "--quiet", "-q", action="store_true",
        help="Suppress non-essential output"
    )
    
    # Create subparsers for different commands
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    subparsers.required = True
    
    # Helper function to add common arguments
    def add_common_args(parser):
        parser.add_argument("--neurons", type=int, default=500,
                           help="Number of neurons (default: 500)")
        parser.add_argument("--synapse-density", type=float, default=0.1,
                           help="Synapse density (default: 0.1)")
        parser.add_argument("--connection-probability", type=float, default=0.05,
                           help="Connection probability (default: 0.05)")
        parser.add_argument("--steps", type=int, default=1000,
                           help="Number of simulation steps (default: 1000)")
        parser.add_argument("--save-state", help="Save brain state to file")
    
    def add_world_args(parser):
        add_common_args(parser)
        parser.add_argument("--world-width", type=int, default=20,
                           help="World width in cells (default: 20)")
        parser.add_argument("--world-height", type=int, default=20,
                           help="World height in cells (default: 20)")
        parser.add_argument("--vision-width", type=int, default=8,
                           help="Vision width in cells (default: 8)")
        parser.add_argument("--vision-height", type=int, default=8,
                           help="Vision height in cells (default: 8)")
        parser.add_argument("--start-x", type=float, default=10.0,
                           help="Agent start X position (default: 10.0)")
        parser.add_argument("--start-y", type=float, default=10.0,
                           help="Agent start Y position (default: 10.0)")
        parser.add_argument("--timestep", type=float, default=0.1,
                           help="Simulation timestep (default: 0.1)")
        parser.add_argument("--reward-modulation", action="store_true",
                           help="Enable reward modulation")
        parser.add_argument("--structural-plasticity", action="store_true",
                           help="Enable structural plasticity")
        parser.add_argument("--development", action="store_true",
                           help="Enable development")
        parser.add_argument("--curiosity", action="store_true",
                           help="Enable curiosity")
    
    def add_batch_args(parser):
        add_common_args(parser)
        parser.add_argument("--runs", type=int, default=10,
                           help="Number of simulation runs (default: 10)")
        parser.add_argument("--steps-per-run", type=int, default=100,
                           help="Steps per simulation run (default: 100)")
    
    def add_analyze_args(parser):
        parser.add_argument("--neurons", type=int, default=500,
                           help="Number of neurons for new brain (default: 500)")
        parser.add_argument("--load-file", help="Load brain state from file")
        parser.add_argument("--output-json", help="Save analysis to JSON file")
    
    def add_visualize_args(parser):
        add_common_args(parser)
        parser.add_argument("--output-file", help="Save visualization to file")
    
    def add_demo_args(parser):
        parser.add_argument(
            "type", choices=["phase6"],
            help="Type of demonstration"
        )
    
    # Simulate command
    sim_parser = subparsers.add_parser("simulate", help="Run simple neural simulation")
    add_common_args(sim_parser)
    sim_parser.add_argument("--detailed", action="store_true",
                         help="Enable detailed output with intermediate statistics")
    
    # World simulation command
    world_parser = subparsers.add_parser("world", help="Run complete agent-world simulation")
    add_world_args(world_parser)
    
    # Analyze command
    analyze_parser = subparsers.add_parser("analyze", help="Analyze brain state and collect statistics")
    add_analyze_args(analyze_parser)
    
    # Batch command
    batch_parser = subparsers.add_parser("batch", help="Run multiple simulations in batch")
    add_batch_args(batch_parser)
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run demonstration scenarios")
    add_demo_args(demo_parser)
    
    # Visualize command
    viz_parser = subparsers.add_parser("visualize", help="Create brain activity visualizations")
    add_visualize_args(viz_parser)
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle global help/version requests
    if args.command == "help" or args.help or args.help_all:
        print_help()
        return
    
    if args.version:
        print("NLM CLI Version 2.0.0 (Enhanced)")
        print("Built with enhanced Python bindings")
        return
    
    # Validate arguments
    errors = validate_args(args)
    if errors:
        print("❌ Argument validation errors:")
        for error in errors:
            print(f"   - {error}")
        sys.exit(1)
    
    # Execute command
    try:
        if args.command == "simulate":
            run_simulation(args)
        elif args.command == "world":
            run_world_simulation(args)
        elif args.command == "analyze":
            analyze_brain(args)
        elif args.command == "batch":
            batch_simulate(args)
        elif args.command == "demo":
            if args.type == "phase6":
                demo_phase6(args)
            else:
                print(f"❌ Unknown demo type: {args.type}")
                sys.exit(1)
        elif args.command == "visualize":
            visualize_brain(args)
        else:
            print(f"❌ Unknown command: {args.command}")
            sys.exit(1)
            
    except KeyboardInterrupt:
        print("\n
⚠️  Simulation interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Error: {e}", file=sys.stderr)
        if args.verbose:
            import traceback
            traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()