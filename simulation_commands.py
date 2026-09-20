#!/usr/bin/env python3
"""
Simulation Control Commands for NLM CLI

Handles simulation control, monitoring, checkpointing, and metrics tracking.
"""

import json
import time
import threading
from pathlib import Path
from typing import Dict, Any, Optional, List
from datetime import datetime

import click
from rich.console import Console
from rich.table import Table
from rich.syntax import Syntax
from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn, TimeRemainingColumn

from nlm_cli import NLMCLI

console = Console()

class SimulationCommands:
    """Handle simulation control commands."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle simulation commands."""
        command = args.get('command')
        
        handlers = {
            'start': self.start_simulation,
            'stop': self.stop_simulation,
            'pause': self.pause_simulation,
            'resume': self.resume_simulation,
            'step': self.step_simulation,
            'status': self.get_simulation_status,
            'metrics': self.get_simulation_metrics,
            'checkpoint': self.create_checkpoint,
            'checkpoint_list': self.list_checkpoints,
            'checkpoint_restore': self.restore_checkpoint,
            'speed': self.set_simulation_speed,
            'step_mode': self.set_step_mode,
            'configure': self.configure_simulation,
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown simulation command: {command}")
    
    def start_simulation(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Start a simulation."""
        simulation_name = args.get('name', 'default_simulation')
        steps = args.get('steps', 100)
        auto_step = args.get('auto_step', False)
        
        brain = self.nlm_cli.context.get('brain')
        if not brain:
            raise click.ClickException("No brain loaded. Create or load a brain first.")
        
        # Check if world and agent are available
        world = self.nlm_cli.context.get('world')
        agent = self.nlm_cli.context.get('agent')
        
        if not world or not agent:
            raise click.ClickException("No world or agent loaded. Create them first.")
        
        try:
            from pynlm.enhanced_api import SimulationControls
            
            # Create simulation controls
            simulation = SimulationControls(brain, world, agent)
            
            # Store in context
            self.nlm_cli.context['simulation'] = simulation
            
            # Run the simulation
            click.echo(f"Starting simulation '{simulation_name}' for {steps} steps...")
            
            if auto_step:
                simulation.run(steps, auto_step=True)
            else:
                with Progress(
                    SpinnerColumn(),
                    TextColumn("[progress.description]{task.description}"),
                    BarColumn(),
                    TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
                    TimeRemainingColumn(),
                    console=console
                ) as progress:
                    task = progress.add_task("[cyan]Running simulation...", total=steps)
                    
                    for step in range(steps):
                        simulation.step(step)
                        progress.update(task, advance=1)
                        time.sleep(0.01)  # Small delay for visualization
            
            # Get final statistics
            from pynlm.enhanced_api import BrainStats
            brain_stats = BrainStats(brain)
            stats = brain_stats.get_summary()
            
            return {
                'success': True,
                'message': f"Simulation '{simulation_name}' completed successfully.",
                'simulation_name': simulation_name,
                'steps_completed': steps,
                'final_stats': stats,
                'simulation': simulation
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to start simulation '{simulation_name}': {e}",
                'error': str(e)
            }
    
    def stop_simulation(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Stop the current simulation."""
        simulation_name = args.get('name', 'current_simulation')
        
        simulation = self.nlm_cli.context.get('simulation')
        if not simulation:
            return {
                'success': True,
                'message': "No simulation is currently running.",
                'simulation_name': simulation_name
            }
        
        # Pause the simulation
        simulation.pause()
        
        return {
            'success': True,
            'message': f"Simulation '{simulation_name}' paused/stopped successfully.",
            'simulation_name': simulation_name,
            'simulation': simulation
        }
    
    def pause_simulation(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Pause the current simulation."""
        simulation_name = args.get('name', 'current_simulation')
        
        simulation = self.nlm_cli.context.get('simulation')
        if not simulation:
            raise click.ClickException("No simulation is currently running.")
        
        if not simulation.is_paused():
            simulation.pause()
            message = f"Simulation '{simulation_name}' paused successfully."
        else:
            message = f"Simulation '{simulation_name}' is already paused."
        
        return {
            'success': True,
            'message': message,
            'simulation_name': simulation_name,
            'simulation': simulation
        }
    
    def resume_simulation(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Resume the current simulation."""
        simulation_name = args.get('name', 'current_simulation')
        
        simulation = self.nlm_cli.context.get('simulation')
        if not simulation:
            raise click.ClickException("No simulation is currently running.")
        
        if simulation.is_paused():
            simulation.resume()
            message = f"Simulation '{simulation_name}' resumed successfully."
        else:
            message = f"Simulation '{simulation_name}' is not paused."
        
        return {
            'success': True,
            'message': message,
            'simulation_name': simulation_name,
            'simulation': simulation
        }
    
    def step_simulation(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Run a single simulation step."""
        simulation_name = args.get('name', 'current_simulation')
        step_number = args.get('step_number', 0)
        
        simulation = self.nlm_cli.context.get('simulation')
        if not simulation:
            raise click.ClickException("No simulation is currently running.")
        
        if not simulation.is_paused():
            click.echo(f"Simulation '{simulation_name}' is not paused. Pausing first...")
            simulation.pause()
        
        step_result = simulation.step(step_number)
        
        return {
            'success': True,
            'message': f"Simulation '{simulation_name}' stepped to step {step_number}.",
            'simulation_name': simulation_name,
            'step_result': step_result,
            'simulation': simulation
        }
    
    def get_simulation_status(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get current simulation status."""
        simulation_name = args.get('name', 'current_simulation')
        
        simulation = self.nlm_cli.context.get('simulation')
        if not simulation:
            return {
                'success': True,
                'message': "No simulation is currently running.",
                'simulation_name': simulation_name,
                'simulation': None
            }
        
        # Get comprehensive status
        status = {
            'simulation_name': simulation_name,
            'is_paused': simulation.is_paused(),
            'step_mode': simulation._step_mode if hasattr(simulation, '_step_mode') else False,
            'time_scale': simulation._time_scale if hasattr(simulation, '_time_scale') else 1.0,
            'has_brain': self.nlm_cli.context.get('brain') is not None,
            'has_world': self.nlm_cli.context.get('world') is not None,
            'has_agent': self.nlm_cli.context.get('agent') is not None,
            'session_time': time.time() - self.nlm_cli.session_start
        }
        
        console.print(f"Simulation Status: {simulation_name}")
        console.print("=" * 50)
        console.print(f"  Is Paused: {status['is_paused']}")
        console.print(f"  Step Mode: {status['step_mode']}")
        console.print(f"  Time Scale: {status['time_scale']}x")
        console.print(f"  Has Brain: {status['has_brain']}")
        console.print(f"  Has World: {status['has_world']}")
        console.print(f"  Has Agent: {status['has_agent']}")
        console.print(f"  Session Age: {status['session_time']:.2f} seconds")
        
        return {
            'success': True,
            'message': f"Simulation '{simulation_name}' status retrieved.",
            'status': status,
            'simulation': simulation
        }
    
    def get_simulation_metrics(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get simulation metrics."""
        simulation_name = args.get('name', 'current_simulation')
        
        simulation = self.nlm_cli.context.get('simulation')
        if not simulation:
            raise click.ClickException("No simulation is currently running.")
        
        brain = self.nlm_cli.context.get('brain')
        if not brain:
            raise click.ClickException("No brain loaded.")
        
        try:
            from pynlm.enhanced_api import BrainStats
            brain_stats = BrainStats(brain)
            stats = brain_stats.get_summary()
            
            # Create metrics table
            table = Table(title="Simulation Metrics")
            table.add_column("Metric", style="cyan")
            table.add_column("Value", style="green")
            
            metrics = [
                ('Total Neurons', stats['total_neurons']),
                ('Total Synapses', stats['total_synapses']),
                ('Active Neurons', stats['active_neurons']),
                ('Firing Neurons', stats['firing_neurons']),
                ('Total Spikes', stats['total_spikes']),
                ('Average Firing Rate', f"{stats['average_firing_rate']:.2f} Hz"),
                ('Excitation/Inhibition Ratio', f"{stats['excitation_inhibition_ratio']:.2f}"),
                ('Development Stage', str(stats['development_stage'])),
                ('Region Count', stats['region_count']),
                ('Simulation Time', f"{simulation._time_scale if hasattr(simulation, '_time_scale') else 1.0}x"),
            ]
            
            for metric_name, value in metrics:
                table.add_row(metric_name, str(value))
            
            console.print(table)
            
            return {
                'success': True,
                'message': f"Simulation metrics for '{simulation_name}' retrieved.",
                'simulation_name': simulation_name,
                'metrics': stats
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to get simulation metrics: {e}",
                'error': str(e)
            }
    
    def create_checkpoint(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Create a simulation checkpoint."""
        checkpoint_name = args.get('name', f"checkpoint_{int(time.time())}")
        filepath = args.get('filepath')
        
        brain = self.nlm_cli.context.get('brain')
        if not brain:
            raise click.ClickException("No brain loaded.")
        
        if not filepath:
            filepath = f"{checkpoint_name}.json"
        
        try:
            from pynlm.enhanced_api import ModelManager
            model_manager = ModelManager(brain)
            
            # Save checkpoint
            brain.save(filepath)
            
            # Add to checkpoints list
            checkpoint_info = {
                'name': checkpoint_name,
                'path': filepath,
                'timestamp': datetime.now().isoformat(),
                'brain_stats': brain.get_stats().get_summary() if hasattr(brain, 'get_stats') else {}
            }
            
            if not hasattr(model_manager, '_checkpoints'):
                model_manager._checkpoints = []
            
            model_manager._checkpoints.append(checkpoint_info)
            
            return {
                'success': True,
                'message': f"Checkpoint '{checkpoint_name}' created successfully at {filepath}.",
                'checkpoint_name': checkpoint_name,
                'filepath': filepath,
                'checkpoint_info': checkpoint_info,
                'model_manager': model_manager
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to create checkpoint '{checkpoint_name}': {e}",
                'error': str(e)
            }
    
    def list_checkpoints(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """List available checkpoints."""
        model_name = args.get('model_name')
        
        model_manager = self.nlm_cli.context.get('model_manager')
        if not model_manager:
            raise click.ClickException("No model manager available.")
        
        checkpoints = model_manager.get_checkpoints() if hasattr(model_manager, 'get_checkpoints') else []
        
        if not checkpoints:
            return {
                'success': True,
                'message': 'No checkpoints available.',
                'checkpoints': []
            }
        
        table = Table(title="Available Checkpoints")
        table.add_column("Name", style="cyan")
        table.add_column("Path", style="green")
        table.add_column("Timestamp", style="yellow")
        table.add_column("Neuron Count", style="magenta")
        
        for checkpoint in checkpoints:
            stats = checkpoint.get('brain_stats', {})
            table.add_row(
                checkpoint.get('name', 'Unnamed'),
                checkpoint.get('path', 'Unknown'),
                checkpoint.get('timestamp', 'Unknown')[:19],
                str(stats.get('total_neurons', 0))
            )
        
        console.print(table)
        
        return {
            'success': True,
            'message': f"Found {len(checkpoints)} checkpoints.",
            'checkpoints': checkpoints
        }
    
    def restore_checkpoint(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Restore a simulation from checkpoint."""
        checkpoint_name = args.get('name')
        model_name = args.get('model_name')
        
        model_manager = self.nlm_cli.context.get('model_manager')
        if not model_manager:
            raise click.ClickException("No model manager available.")
        
        checkpoints = model_manager.get_checkpoints() if hasattr(model_manager, 'get_checkpoints') else []
        
        target_checkpoint = None
        for checkpoint in checkpoints:
            if checkpoint.get('name') == checkpoint_name:
                target_checkpoint = checkpoint
                break
        
        if not target_checkpoint:
            raise click.ClickException(f"Checkpoint '{checkpoint_name}' not found.")
        
        try:
            # Restore brain from checkpoint
            brain = model_manager.load_model(target_checkpoint['path'])
            self.nlm_cli.context['brain'] = brain
            
            # Update model manager
            from pynlm.enhanced_api import ModelManager
            model_manager = ModelManager(brain)
            self.nlm_cli.context['model_manager'] = model_manager
            
            # Create brain stats
            from pynlm.enhanced_api import BrainStats
            self.nlm_cli.context['brain_stats'] = BrainStats(brain)
            
            return {
                'success': True,
                'message': f"Simulation restored from checkpoint '{checkpoint_name}'.",
                'checkpoint_name': checkpoint_name,
                'checkpoint_path': target_checkpoint['path'],
                'brain': brain,
                'model_manager': model_manager
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to restore checkpoint '{checkpoint_name}': {e}",
                'error': str(e)
            }
    
    def set_simulation_speed(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Set simulation speed."""
        speed = args.get('speed')
        
        simulation = self.nlm_cli.context.get('simulation')
        if not simulation:
            raise click.ClickException("No simulation is currently running.")
        
        if hasattr(simulation, '_time_scale'):
            simulation._time_scale = speed
            message = f"Simulation speed set to {speed}x."
        else:
            message = "Cannot set simulation speed: simulation controls not available."
        
        return {
            'success': True,
            'message': message,
            'speed': speed,
            'simulation': simulation
        }
    
    def set_step_mode(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Enable or disable step mode."""
        enabled = args.get('enabled')
        
        simulation = self.nlm_cli.context.get('simulation')
        if not simulation:
            raise click.ClickException("No simulation is currently running.")
        
        if hasattr(simulation, 'set_step_mode'):
            simulation.set_step_mode(enabled)
            mode = "enabled" if enabled else "disabled"
            message = f"Step mode {mode}."
        else:
            message = "Cannot set step mode: simulation controls not available."
        
        return {
            'success': True,
            'message': message,
            'enabled': enabled,
            'simulation': simulation
        }
    
    def configure_simulation(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Configure simulation parameters."""
        parameters = args.get('parameters', {})
        
        # Store configuration
        if 'simulation_config' not in self.nlm_cli.context:
            self.nlm_cli.context['simulation_config'] = {}
        
        self.nlm_cli.context['simulation_config'].update(parameters)
        
        return {
            'success': True,
            'message': "Simulation configuration updated.",
            'parameters': parameters,
            'simulation_config': self.nlm_cli.context['simulation_config']
        }