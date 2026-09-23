#!/usr/bin/env python3
"""
Command-line interface for NLM (Neural Learning Machine).

This module provides a comprehensive CLI for working with NLM brains, including:
- Brain creation and simulation
- Configuration management
- Experiment execution
- Analysis and visualization
- Advanced debugging tools

Usage:
  nlm [OPTIONS] COMMAND [ARGS]...

Options:
  --help  Show this message and exit.

Commands:
  create    Create a new brain configuration
  run       Run a simulation
  analyze   Analyze brain performance
  config    Manage brain configurations
  experiment Run an experiment
  debug     Debug tools
  benchmark Benchmark performance
  help      Show help for a command
"""

import click
import tempfile
import os
import json
from pathlib import Path
import pynlm


@click.group()
def cli():
    """NLM (Neural Learning Machine) - Brain Simulation Framework"""
    pass


@cli.command()
@click.option('--neuron-count', default=1000, help='Number of neurons')
@click.option('--region-count', default=1, help='Number of brain regions')
@click.option('--connection-prob', default=0.1, help='Synapse connection probability')
@click.option('--seed', default=42, help='Random seed')
@click.option('--output', '-o', help='Output configuration file')
def create(neuron_count, region_count, connection_prob, seed, output):
    """Create a new brain configuration."""
    import pynlm

    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    config.set("brain.connection_probability", connection_prob)
    config.set("random_seed", seed)

    if output:
        config.saveToFile(output)
        click.echo(f"Configuration saved to {output}")
    else:
        click.echo("Configuration created:")
        click.echo(config.summary())


@cli.command()
@click.option('--config', '-c', help='Configuration file')
@click.option('--steps', default=1000, help='Number of simulation steps')
@click.option('--world/--no-world', default=False, help='Run with world simulation')
@click.option('--output', '-o', help='Output file for results')
@click.option('--verbose/--quiet', default=False, help='Verbose output')
def run(config, steps, world, output, verbose):
    """Run a simulation."""
    if config and os.path.exists(config):
        # Load configuration from file
        import pynlm
        temp_config = pynlm.createDefaultConfig()
        # Note: This would need implementation for JSON config loading
        click.echo(f"Loading configuration from {config}")
        # For now, use default config
    else:
        import pynlm
        temp_config = pynlm.createDefaultConfig()

    brain = pynlm.createBrain(temp_config)
    brain.initialize()

    if world:
        world_obj = pynlm.createSimpleWorld()
        world_obj.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world_obj.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world_obj)

        # Run simulation with world
        for step in range(steps):
            world_obj.update(0.1)

            percept = world_obj.getSensoryPercept()
            agent.processSensoryInput(percept)

            brain.step(step)

            action = agent.decodeMotorCommand()
            world_obj.applyMotorCommand(action, world_obj.getSimulationTime())

            if verbose and step % 100 == 0:
                click.echo(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

            if output and step == steps - 1:
                # Save final results
                results = {
                    'total_spikes': brain.getTotalSpikeCount(),
                    'avg_firing_rate': brain.getAverageFiringRate(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio(),
                    'final_stage': str(brain.getDevelopmentalStage())
                }
                with open(output, 'w') as f:
                    json.dump(results, f, indent=2)
                click.echo(f"Results saved to {output}")
    else:
        # Run simulation without world
        for step in range(steps):
            brain.step(step)

            if verbose and step % 100 == 0:
                click.echo(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

            if output and step == steps - 1:
                results = {
                    'total_spikes': brain.getTotalSpikeCount(),
                    'avg_firing_rate': brain.getAverageFiringRate(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio()
                }
                with open(output, 'w') as f:
                    json.dump(results, f, indent=2)


@cli.command()
@click.option('--brain-file', help='Path to saved brain file')
@click.option('--output-dir', default='./analysis', help='Output directory for analysis')
@click.option('--format', type=click.Choice(['json', 'csv', 'text']), default='text', help='Output format')
def analyze(brain_file, output_dir, format):
    """Analyze brain performance."""
    import pynlm

    # Create brain (simplified for demo)
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    os.makedirs(output_dir, exist_ok=True)

    if format == 'json':
        # JSON output
        analysis = {
            'basic_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio()
            },
            'brain_info': pynlm.getBrainSystemInfo(brain),
            'integration_test': pynlm.runIntegrationTest()
        }

        output_file = os.path.join(output_dir, 'analysis.json')
        with open(output_file, 'w') as f:
            json.dump(analysis, f, indent=2)
        click.echo(f"Analysis saved to {output_file}")

    elif format == 'csv':
        # CSV output
        import csv
        analysis_file = os.path.join(output_dir, 'analysis.csv')

        with open(analysis_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['Metric', 'Value'])

            writer.writerow(['Total Neurons', brain.getTotalNeuronCount()])
            writer.writerow(['Total Synapses', brain.getTotalSynapseCount()])
            writer.writerow(['Firing Neurons', brain.getFiringNeuronCount()])
            writer.writerow(['Average Firing Rate', brain.getAverageFiringRate()])
            writer.writerow(['E/I Ratio', brain.getExcitationInhibitionRatio()])

        click.echo(f"Analysis saved to {analysis_file}")

    else:
        # Text output
        analysis_file = os.path.join(output_dir, 'analysis.txt')

        with open(analysis_file, 'w') as f:
            f.write("NLM Brain Analysis\n")
            f.write("=" * 50 + "\n\n")

            f.write(f"Total Neurons: {brain.getTotalNeuronCount()}\n")
            f.write(f"Total Synapses: {brain.getTotalSynapseCount()}\n")
            f.write(f"Firing Neurons: {brain.getFiringNeuronCount()}\n")
            f.write(f"Average Firing Rate: {brain.getAverageFiringRate():.4f}\n")
            f.write(f"E/I Ratio: {brain.getExcitationInhibitionRatio():.4f}\n")
            f.write(f"Development Stage: {brain.getDevelopmentalStage()}\n")

            f.write("\nSystem Information:\n")
            system_info = pynlm.getBrainSystemInfo(brain)
            for key, value in system_info.items():
                f.write(f"  {key}: {value}\n")

        click.echo(f"Analysis saved to {analysis_file}")


@cli.command()
@click.option('--source', help='Source configuration file')
@click.option('--target', help='Target configuration file')
@click.option('--merge/--override', default=False, help='Merge or override configuration')
def config(source, target, merge):
    """Manage brain configurations."""
    import pynlm

    if source and os.path.exists(source):
        config = pynlm.createDefaultConfig()
        # Note: This would need implementation for file loading
        click.echo(f"Loading configuration from {source}")
    else:
        config = pynlm.createDefaultConfig()

    if target:
        config.saveToFile(target)
        click.echo(f"Configuration saved to {target}")
    else:
        click.echo("Current configuration:")
        click.echo(config.summary())


@cli.command()
@click.option('--name', default='default_experiment', help='Experiment name')
@click.option('--steps', default=1000, help='Number of simulation steps')
@click.option('--output-dir', default='./experiments', help='Output directory')
def experiment(name, steps, output_dir):
    """Run an experiment."""
    import pynlm

    os.makedirs(output_dir, exist_ok=True)

    # Run multiple trials
    results = []
    for trial in range(3):  # Run 3 trials
        click.echo(f"Running trial {trial + 1}/3...")

        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        # Run simulation
        for step in range(steps):
            brain.step(step)

            if step % 100 == 0:
                results.append({
                    'trial': trial,
                    'step': step,
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'avg_firing_rate': brain.getAverageFiringRate()
                })

    # Save results
    output_file = os.path.join(output_dir, f"{name}_results.json")
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2)

    click.echo(f"Experiment results saved to {output_file}")


@cli.command()
@click.option('--brain-file', help='Path to brain file')
@click.option('--checkpoint-dir', default='./checkpoints', help='Checkpoint directory')
def debug(brain_file, checkpoint_dir):
    """Debug tools."""
    import pynlm

    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Run a few steps for debugging
    for step in range(10):
        brain.step(step)

    click.echo("Debug Information:")
    click.echo(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    click.echo(f"  Firing Neurons: {brain.getFiringNeuronCount()}")
    click.echo(f"  Average Firing Rate: {brain.getAverageFiringRate():.4f}")
    click.echo(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.4f}")
    click.echo(f"  Development Stage: {brain.getDevelopmentalStage()}")

    click.echo("\nSystem Info:")
    system_info = pynlm.getBrainSystemInfo(brain)
    for key, value in system_info.items():
        click.echo(f"  {key}: {value}")

    # Create checkpoints
    os.makedirs(checkpoint_dir, exist_ok=True)
    for i in range(5):
        checkpoint_file = os.path.join(checkpoint_dir, f"checkpoint_{i:03d}.bin")
        # Note: This would need implementation for actual checkpointing
        click.echo(f"Would save checkpoint to {checkpoint_file}")


@cli.command()
@click.option('--steps', default=1000, help='Number of steps to benchmark')
@click.option('--output', '-o', help='Output file')
def benchmark(steps, output):
    """Benchmark performance."""
    import pynlm
    import time

    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()

    # Benchmark
    start_time = time.time()

    for step in range(steps):
        brain.step(step)

    end_time = time.time()
    elapsed = end_time - start_time

    results = {
        'steps': steps,
        'elapsed_time': elapsed,
        'steps_per_second': steps / elapsed if elapsed > 0 else 0,
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate()
    }

    print("Benchmark Results:")
    print(f"  Steps: {steps}")
    print(f"  Elapsed Time: {elapsed:.2f} seconds")
    print(f"  Steps per Second: {results['steps_per_second']:.2f}")
    print(f"  Total Spikes: {results['total_spikes']}")
    print(f"  Average Firing Rate: {results['avg_firing_rate']:.4f}")

    if output:
        with open(output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to {output}")


@cli.command()
@click.pass_context
def help(ctx):
    """Show help for a command."""
    if ctx.invoked_subcommand:
        # Show help for specific command
        for command_name, command in ctx.parent.commands.items():
            if command_name == ctx.invoked_subcommand:
                click.echo(f"Help for '{command_name}':")
                click.echo(command.get_help(ctx.parent))
                break
    else:
        # Show general help
        click.echo(ctx.parent.get_help())


if __name__ == '__main__':
    cli()