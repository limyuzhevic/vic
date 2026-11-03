"""
NLM Command Line Interface - Advanced Features

This module provides advanced command-line tools for working with the NLM neural simulation framework.
"""

import click
from pathlib import Path
from typing import Optional, Dict, Any, List
import json
import time

from pynlm import BrainSimulator, createBrainConfig, createMinimalConfig

# Global verbose flag
VERBOSE = False

class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class NLMArgumentError(NLMError):
    """Exception for invalid arguments"""
    pass

class NLMSimulationError(NLMError):
    """Exception for simulation errors"""
    pass

def validate_simulation_params(steps: int, neurons: int, regions: int) -> None:
    """Validate simulation parameters"""
    if steps <= 0:
        raise NLMArgumentError(f"Steps must be positive, got {steps}")
    if neurons <= 0:
        raise NLMArgumentError(f"Neurons must be positive, got {neurons}")
    if regions <= 0:
        raise NLMArgumentError(f"Regions must be positive, got {regions}")
    if neurons < regions * 10:  # Minimum neurons per region
        raise NLMArgumentError(f"Need at least 10 neurons per region, got {neurons} neurons for {regions} regions")

def format_time(seconds: float) -> str:
    """Format time in human-readable format"""
    if seconds < 60:
        return f"{seconds:.2f}s"
    elif seconds < 3600:
        minutes = seconds / 60
        return f"{minutes:.1f}m"
    else:
        hours = seconds / 3600
        return f"{hours:.1f}h"

def export_stats(stats: Dict[str, Any], output_file: str, format: str = "json"):
    """Export simulation statistics to file"""
    if format.lower() == "json":
        with open(output_file, 'w') as f:
            json.dump(stats, f, indent=2)
    elif format.lower() == "csv":
        import csv
        with open(output_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(stats.keys())
            writer.writerow(stats.values())
    elif format.lower() == "txt":
        with open(output_file, 'w') as f:
            f.write("NLM Simulation Statistics\n")
            f.write("=" * 40 + "\n")
            for key, value in stats.items():
                if isinstance(value, float):
                    f.write(f"{key}: {value:.6f}\n")
                else:
                    f.write(f"{key}: {value}\n")
    else:
        raise NLMArgumentError(f"Unsupported format: {format}. Supported: json, csv, txt")

@click.group()
@click.version_option("0.1.0-enhanced")
@click.option("--verbose", "-v", is_flag=True, help="Enable verbose output")
@click.option("--log-file", "-l", help="Log output to file")
def cli(verbose, log_file):
    """NLM - Neural Learning Machine Simulation Framework"""
    global VERBOSE
    VERBOSE = verbose

    if log_file:
        import logging
        logging.basicConfig(
            filename=log_file,
            level=logging.DEBUG if verbose else logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )

@cli.command()
@click.argument("steps", type=int)
@click.option("--neurons", "-n", default=1000, help="Number of neurons")
@click.option("--regions", "-r", default=1, help="Number of brain regions")
@click.option("--connection-prob", "-p", default=0.1, help="Connection probability (0.0-1.0)")
@click.option("--world", "-w", is_flag=True, help="Run in world environment")
@click.option("--width", "-W", default=20, help="World width")
@click.option("--height", "-H", default=20, help="World height")
@click.option("--vision-width", "-vw", default=8, help="Vision width")
@click.option("--vision-height", "-vh", default=8, help="Vision height")
@click.option("--agent-x", "-ax", default=10.0, help="Agent starting X position")
@click.option("--agent-y", "-ay", default=10.0, help="Agent starting Y position")
@click.option("--reward-mod", "-rm", is_flag=True, help="Enable reward modulation")
@click.option("--structural-plasticity", "-sp", is_flag=True, help="Enable structural plasticity")
@click.option("--development", "-d", is_flag=True, help="Enable development")
@click.option("--curiosity", "-c", is_flag=True, help="Enable curiosity")
@click.option("--output", "-o", help="Output brain state file")
@click.option("--output-format", "--of", type=click.Choice(['bin', 'json']), default='bin', help="Output format")
@click.option("--stats-file", "--sf", help="Export statistics to file")
@click.option("--stats-format", "--sfmt", type=click.Choice(['json', 'csv', 'txt']), default='json', help="Statistics export format")
def sim(steps, neurons, regions, connection_prob, world, width, height, 
        vision_width, vision_height, agent_x, agent_y, reward_mod, structural_plasticity,
        development, curiosity, output, output_format, stats_file, stats_format):
    """Run a NLM simulation with advanced configuration options"""
    if VERBOSE:
        click.echo(f"Starting simulation with {neurons} neurons, {regions} regions for {steps} steps")
        click.echo(f"Connection probability: {connection_prob}")
    
    try:
        # Validate parameters
        validate_simulation_params(steps, neurons, regions)
        
        # Create configuration
        config = createBrainConfig(
            neuronCount=neurons,
            regionCount=regions,
            connectionProb=connection_prob
        )
        
        # Create brain simulator
        brain = BrainSimulator(config)
        
        # Setup world if requested
        if world:
            brain.setupWorld(
                width=width, 
                height=height, 
                visionWidth=vision_width, 
                visionHeight=vision_height
            )
            brain.setAgentStart(agent_x, agent_y)
            
            # Enable learning subsystems
            brain.enableLearning(reward_mod, structural_plasticity, development, curiosity)
        
        # Log start
        if VERBOSE:
            click.echo("Running simulation...")
        
        # Run simulation
        start_time = time.time()
        brain.run(steps, logProgress=VERBOSE)
        elapsed = time.time() - start_time
        
        # Save output if requested
        if output:
            click.echo(f"Saving brain state to {output}")
            success = brain.save(output)
            if not success:
                click.echo(f"Warning: Failed to save brain state to {output}", err=True)
        
        # Get statistics
        stats = brain.getStats()
        stats['simulation_time'] = elapsed
        stats['steps_completed'] = steps
        
        # Print statistics
        click.echo(f"\nSimulation completed in {format_time(elapsed)}:")
        click.echo(f"  Total steps: {steps}")
        click.echo(f"  Total neurons: {stats['neurons_total']}")
        click.echo(f"  Spikes generated: {stats['spikes_total']}")
        click.echo(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
        click.echo(f"  E/I ratio: {stats['excitation_inhibition_ratio']:.3f}")
        
        if world:
            click.echo(f"  Agent position: ({stats['agent_x']:.1f}, {stats['agent_y']:.1f})")
            click.echo(f"  Agent energy: {stats['agent_energy']:.1f}")
            click.echo(f"  Agent health: {stats['agent_health']:.1f}")
            click.echo(f"  Curiosity level: {stats['curiosity_level']:.3f}")
            click.echo(f"  Novelty level: {stats['novelty_level']:.3f}")
        
        # Export statistics if requested
        if stats_file:
            click.echo(f"Exporting statistics to {stats_file}")
            export_stats(stats, stats_file, stats_format)
        
    except NLMError as e:
        click.echo(f"NLM Error: {e}", err=True)
        raise click.Abort()
    except Exception as e:
        click.echo(f"Unexpected error: {e}", err=True)
        if VERBOSE:
            import traceback
            click.echo(traceback.format_exc(), err=True)
        raise click.Abort()

@cli.command()
@click.option("--config-file", "-c", help="Configuration file path")
@click.option("--steps", "-s", default=1000, help="Number of steps")
@click.option("--neurons", "-n", default=1000, help="Number of neurons")
@click.option("--output-dir", "-o", default="./experiments", help="Output directory")
@click.option("--name", "-N", help="Experiment name")
@click.option("--enable-all-learning", "-all", is_flag=True, help="Enable all learning subsystems")
def run_experiment(config_file, steps, neurons, output_dir, name, enable_all_learning):
    """Run a complete experiment with configuration management"""
    click.echo(f"Starting experiment: {name or 'unnamed'}")
    
    # Create output directory
    Path(output_dir).mkdir(parents=True, exist_ok=True)
    
    try:
        # Load configuration or create default
        if config_file:
            click.echo(f"Loading configuration from {config_file}")
            # Note: In a full implementation, this would load from JSON/YAML
            config = createBrainConfig(neuronCount=neurons)
        else:
            config = createMinimalConfig()
        
        # Create brain
        brain = BrainSimulator(config)
        
        # Setup world
        brain.setupWorld(width=30, height=30, visionWidth=12, visionHeight=12)
        
        # Enable learning
        if enable_all_learning:
            brain.enableLearning(True, True, True, True)
        else:
            brain.enableLearning(True, False, False, True)
        
        # Run experiment
        click.echo(f"Running {steps} steps...")
        brain.run(steps, logProgress=VERBOSE)
        
        # Save experiment results
        if name:
            output_file = Path(output_dir) / f"{name}_brain.bin"
            brain.save(str(output_file))
            click.echo(f"Saved brain state to {output_file}")
        
        # Save experiment metadata
        metadata = {
            'name': name or f"experiment_{int(time.time())}",
            'steps': steps,
            'neurons': neurons,
            'world_size': 30,
            'vision_size': 12,
            'enable_all_learning': enable_all_learning,
            'timestamp': time.time()
        }
        
        metadata_file = Path(output_dir) / f"{metadata['name']}_metadata.json"
        import json
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        click.echo(f"Saved experiment metadata to {metadata_file}")
        
        click.echo("Experiment completed successfully!")
        
    except Exception as e:
        click.echo(f"Experiment failed: {e}", err=True)
        if VERBOSE:
            import traceback
            click.echo(traceback.format_exc(), err=True)
        raise click.Abort()

@cli.group()
def checkpoint():
    """Checkpoint management commands"""
    pass

@checkpoint.command("save")
@click.argument("brain_file", type=click.Path(exists=True, dir_okay=False))
@click.option("--output", "-o", help="Output checkpoint file")
@click.option("--description", "-d", help="Checkpoint description")
@click.option("--tags", "-t", help="Tags (comma-separated)")
def save_checkpoint(brain_file, output, description, tags):
    """Save brain state as a checkpoint"""
    click.echo(f"Loading brain from {brain_file}")
    
    # Note: In a full implementation, this would load the brain file
    # For now, we'll create a new brain and save it
    brain = BrainSimulator()
    
    # Create a simple brain for demonstration
    config = createBrainConfig(500)
    brain.initialize(config)
    
    # Save as checkpoint
    output_file = output or f"checkpoint_{int(time.time())}.bin"
    if brain.save(output_file):
        click.echo(f"Saved checkpoint to {output_file}")
        
        # Save metadata
        metadata = {
            'description': description or "Saved checkpoint",
            'tags': tags.split(',') if tags else [],
            'created': time.time(),
            'brain_type': 'BrainSimulator'
        }
        
        metadata_file = output_file.replace('.bin', '_metadata.json')
        import json
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        click.echo(f"Saved metadata to {metadata_file}")
    else:
        click.echo(f"Failed to save checkpoint to {output_file}", err=True)
        raise click.Abort()

@checkpoint.command("load")
@click.argument("checkpoint_file", type=click.Path(exists=True, dir_okay=False))
@click.option("--world", "-w", is_flag=True, help="Setup world environment")
@click.option("--name", "-n", help="Give a name to the brain")
def load_checkpoint(checkpoint_file, world, name):
    """Load a brain state from checkpoint"""
    click.echo(f"Loading checkpoint from {checkpoint_file}")
    
    try:
        brain = BrainSimulator()
        
        if not brain.load(checkpoint_file):
            click.echo(f"Failed to load checkpoint from {checkpoint_file}", err=True)
            raise click.Abort()
        
        click.echo("Checkpoint loaded successfully")
        
        if world:
            brain.setupWorld(width=20, height=20, visionWidth=8, visionHeight=8)
            click.echo("World environment setup")
        
        # Get statistics
        stats = brain.getStats()
        click.echo(f"Brain statistics:")
        click.echo(f"  Neurons: {stats['neurons_total']}")
        click.echo(f"  Spikes: {stats['spikes_total']}")
        click.echo(f"  Firing rate: {stats['average_firing_rate']:.2f} Hz")
        
        if name:
            click.echo(f"Brain named: {name}")
        
    except Exception as e:
        click.echo(f"Failed to load checkpoint: {e}", err=True)
        if VERBOSE:
            import traceback
            click.echo(traceback.format_exc(), err=True)
        raise click.Abort()

@checkpoint.group()
def info():
    """Get information about checkpoints"""
    pass

@info.command("list")
@click.option("--directory", "-d", default=".", help="Directory to search for checkpoints")
@click.option("--format", "-f", type=click.Choice(['brief', 'detailed']), default='brief', help="Output format")
def list_checkpoints(directory, format):
    """List available checkpoints"""
    click.echo(f"Searching for checkpoints in {directory}")
    
    checkpoint_files = []
    for ext in ['.bin', '.chk', '.cp']:
        checkpoint_files.extend(Path(directory).glob(f"*{ext}"))
    
    if not checkpoint_files:
        click.echo("No checkpoint files found.")
        return
    
    if format == 'brief':
        click.echo("Available checkpoints:")
        for cp in sorted(checkpoint_files):
            size = cp.stat().st_size
            click.echo(f"  {cp.name} ({size:,} bytes)")
    else:
        import json
        click.echo("Detailed checkpoint information:")
        for cp in sorted(checkpoint_files):
            try:
                # Try to load metadata
                metadata_file = cp.with_suffix('_metadata.json')
                if metadata_file.exists():
                    with open(metadata_file, 'r') as f:
                        metadata = json.load(f)
                    click.echo(f"\n{cp.name}:")
                    click.echo(f"  Description: {metadata.get('description', 'N/A')}")
                    click.echo(f"  Tags: {', '.join(metadata.get('tags', []))}")
                    click.echo(f"  Created: {time.ctime(metadata.get('created', 0))}")
                    click.echo(f"  Size: {cp.stat().st_size:,} bytes")
            except Exception:
                click.echo(f"\n{cp.name}: (no metadata available)")
                click.echo(f"  Size: {cp.stat().st_size:,} bytes")

if __name__ == "__main__":
    cli()
