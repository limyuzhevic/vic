# NLM Command Line Interface

This module provides command-line tools for working with the NLM neural simulation framework.

## Overview

The CLI includes tools for:
- Running simulations (simple, learning, agent-world)
- Managing checkpoints (save/load)
- Exploring brain configurations
- Running demos

## Installation

After installing pynlm, the tools will be available:

```bash
# Run the simulation tool
nlm-sim --help

# Run exploration demo
nlm-explore --help

# Run learning demo
nlm-learn --help

# Check the CLI structure
nlm-checkpoint --help
```

## CLI Structure

### nlm/cli.py
Main CLI module with argument parsing and command dispatch.

### nlm/commands/
Directory containing individual command implementations.

### nlm/gui/
Directory containing GUI visualization tools.

### nlm/utils/
Utility functions and helpers.

## Main CLI Module

```python
import click
from pathlib import Path
from typing import Optional, Dict, Any

from pynlm import BrainSimulator, createBrainConfig, runLearningExperiment

@click.group()
@click.version_option("0.1.0-enhanced")
@click.option("--verbose", "-v", is_flag=True, help="Enable verbose output")
def cli(verbose):
    """NLM - Neural Learning Machine Simulation Framework"""
    global VERBOSE
    VERBOSE = verbose

@cli.command()
@click.argument("steps", type=int)
@click.option("--neurons", "-n", default=1000, help="Number of neurons")
@click.option("--regions", "-r", default=1, help="Number of brain regions")
@click.option("--world", "-w", is_flag=True, help="Run in world environment")
@click.option("--learn", "-l", is_flag=True, help="Enable learning subsystems")
@click.option("--output", "-o", help="Output file for brain state")
@click.option("--checkpoint", "-c", help="Load checkpoint from file")
def sim(steps, neurons, regions, world, learn, output, checkpoint):
    """Run a NLM simulation"""
    if VERBOSE:
        click.echo(f"Starting simulation with {neurons} neurons for {steps} steps")
    
    try:
        # Create configuration
        config = createBrainConfig(
            neuronCount=neurons,
            regionCount=regions,
            connectionProb=0.1f
        )
        
        # Load checkpoint if specified
        brain = None
        if checkpoint:
            click.echo(f"Loading checkpoint from {checkpoint}")
            brain = BrainSimulator()
            if not brain.load(checkpoint):
                click.echo(f"Warning: Failed to load checkpoint {checkpoint}")
        
        # Create brain if not loaded from checkpoint
        if brain is None:
            brain = BrainSimulator(config)
            if world:
                brain.setupWorld(width=20, height=20, visionWidth=8, visionHeight=8)
                if learn:
                    brain.enableLearning(True, True, True, True)
        
        # Run simulation
        if VERBOSE:
            click.echo("Running simulation...")
        
        brain.run(steps, logProgress=VERBOSE)
        
        # Save output if specified
        if output:
            click.echo(f"Saving brain state to {output}")
            brain.save(output)
        
        # Print statistics
        stats = brain.getStats()
        click.echo(f"\nSimulation completed:")
        click.echo(f"  Total steps: {steps}")
        click.echo(f"  Total neurons: {stats['neurons_total']}")
        click.echo(f"  Spikes generated: {stats['spikes_total']}")
        click.echo(f"  Final firing rate: {stats['average_firing_rate']:.2f} Hz")
        
        if learn and world:
            click.echo(f"  Curiosity level: {stats['curiosity_level']:.3f}")
            click.echo(f"  Novelty level: {stats['novelty_level']:.3f}")
        
    except Exception as e:
        click.echo(f"Error: {e}", err=True)
        if VERBOSE:
            import traceback
            click.echo(traceback.format_exc(), err=True)
        raise click.Abort()

@cli.command()
@click.option("--width", "-W", default=30, help="World width")
@click.option("--height", "-H", default=30, help="World height")
@click.option("--steps", "-s", default=500, help="Number of steps to run")
@click.option("--neurons", "-n", default=2000, help="Number of neurons")
def explore(width, height, steps, neurons):
    """Run an exploration simulation in a 2D world"""
    click.echo(f"Starting exploration simulation in {width}x{height} world")
    click.echo(f"Using {neurons} neurons for {steps} steps")
    
    try:
        runLearningExperiment(
            steps=steps,
            description=f"Exploration in {width}x{height} world"
        )
        
        click.echo("\nExploration completed!")
        
    except Exception as e:
        click.echo(f"Error: {e}", err=True)
        if VERBOSE:
            import traceback
            click.echo(traceback.format_exc(), err=True)
        raise click.Abort()

@cli.command()
@click.option("--steps", "-s", default=1000, help="Number of learning steps")
@click.option("--neurons", "-n", default=500, help="Number of neurons (smaller for faster learning)")
@click.option("--world-size", "-w", default=15, help="World size (width=height)")
@click.option("--resource-reward", "-R", default=10.0, help="Resource reward value")
@click.option("--hazard-cost", "-H", default=-15.0, help="Hazard cost")
def learn(steps, neurons, world_size, resource_reward, hazard_cost):
    """Run a learning experiment with rewards and punishments"""
    click.echo(f"Starting learning experiment")
    click.echo(f"  World: {world_size}x{world_size}")
    click.echo(f"  Neurons: {neurons}, Steps: {steps}")
    click.echo(f"  Resource reward: +{resource_reward}, Hazard cost: {hazard_cost}")
    
    # For now, run a basic learning experiment
    # In a full implementation, this would use a more sophisticated world
    # with resources and hazards that provide rewards/costs
    
    try:
        # Use the enhanced BrainSimulator for learning
        brain = BrainSimulator(createBrainConfig(neurons))
        
        if world_size > 0:
            brain.setupWorld(width=world_size, height=world_size, 
                           visionWidth=8, visionHeight=8)
            brain.enableLearning(True, True, True, True)
        
        # Run learning steps
        click.echo(f"Running {steps} learning steps...")
        
        for step in range(steps):
            brain.step(step)
            
            if (step + 1) % 200 == 0:
                stats = brain.getStats()
                curiosity = stats.get('curiosity_level', 0.0)
                novelty = stats.get('novelty_level', 0.0)
                click.echo(f"  Step {step + 1}/{steps}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
        
        stats = brain.getStats()
        click.echo(f"\nLearning experiment completed:")
        click.echo(f"  Final curiosity: {stats.get('curiosity_level', 0.0):.3f}")
        click.echo(f"  Final novelty: {stats.get('novelty_level', 0.0):.3f}")
        click.echo(f"  Total spikes: {stats['spikes_total']}")
        
    except Exception as e:
        click.echo(f"Error: {e}", err=True)
        if VERBOSE:
            import traceback
            click.echo(traceback.format_exc(), err=True)
        raise click.Abort()

@cli.command()
@click.argument("input_file", type=click.Path(exists=True, dir_okay=False))
@click.argument("output_file", type=click.Path(dir_okay=False))
def checkpoint(input_file, output_file):
    """Copy or convert checkpoint files"""
    input_path = Path(input_file)
    output_path = Path(output_file)
    
    click.echo(f"Loading checkpoint from {input_path}")
    
    try:
        brain = BrainSimulator()
        
        if not brain.load(str(input_path)):
            click.echo(f"Error: Failed to load checkpoint from {input_path}", err=True)
            raise click.Abort()
        
        click.echo(f"Saving checkpoint to {output_path}")
        brain.save(str(output_path))
        
        click.echo("Checkpoint conversion completed successfully!")
        
    except Exception as e:
        click.echo(f"Error: {e}", err=True)
        if VERBOSE:
            import traceback
            click.echo(traceback.format_exc(), err=True)
        raise click.Abort()

if __name__ == "__main__":
    cli()
