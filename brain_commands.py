#!/usr/bin/env python3
"""
Brain Management Commands for NLM CLI

Handles brain creation, loading, saving, comparison, and inspection.
"""

import json
import time
from pathlib import Path
from typing import Dict, Any, Optional, List
from datetime import datetime

import click
from rich.console import Console
from rich.table import Table
from rich.syntax import Syntax
from rich.progress import Progress

from nlm_cli import NLMCLI

console = Console()

class BrainCommands:
    """Handle brain management commands."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle brain commands."""
        command = args.get('command')
        
        handlers = {
            'create': self.create_brain,
            'load': self.load_brain,
            'save': self.save_brain,
            'compare': self.compare_brains,
            'inspect': self.inspect_brain,
            'list': self.list_brains,
            'export': self.export_brain,
            'import': self.import_brain,
            'reset': self.reset_brain,
            'stats': self.get_brain_stats,
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown brain command: {command}")
    
    def create_brain(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Create a new brain instance."""
        brain_name = args.get('name')
        config_name = args.get('config')
        parameters = args.get('parameters', {})
        auto_initialize = args.get('auto_initialize', True)
        debug = args.get('debug', False)
        
        # Get configuration if specified
        config = {}
        if config_name:
            configs = self.nlm_cli.load_config()
            if config_name in configs:
                config = configs[config_name].get('parameters', {})
            else:
                raise click.ClickException(f"Configuration '{config_name}' not found.")
        
        # Merge with provided parameters
        config = {**config, **parameters}
        
        try:
            # Create brain using enhanced API
            from pynlm.enhanced_api import create_enhanced_brain
            brain = create_enhanced_brain(config, auto_initialize, debug)
            
            # Store brain in context
            self.nlm_cli.context['brain'] = brain
            
            # Create model manager
            from pynlm.enhanced_api import ModelManager
            self.nlm_cli.context['model_manager'] = ModelManager(brain)
            
            # Create brain stats
            from pynlm.enhanced_api import BrainStats
            self.nlm_cli.context['brain_stats'] = BrainStats(brain)
            
            return {
                'success': True,
                'message': f"Brain '{brain_name}' created successfully.",
                'brain_name': brain_name,
                'brain': brain,
                'stats': self.nlm_cli.context['brain_stats'].get_summary()
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to create brain '{brain_name}': {e}",
                'error': str(e)
            }
    
    def load_brain(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Load a brain model from file."""
        brain_name = args.get('name')
        filepath = args.get('filepath')
        format = args.get('format', 'json')
        
        if not filepath:
            raise click.ClickException("Filepath is required for loading a brain.")
        
        if not Path(filepath).exists():
            raise click.ClickException(f"Brain file not found: {filepath}")
        
        try:
            from pynlm.enhanced_api import import_model_from_json
            brain = import_model_from_json(filepath)
            
            # Store brain in context
            self.nlm_cli.context['brain'] = brain
            
            # Create model manager
            from pynlm.enhanced_api import ModelManager
            self.nlm_cli.context['model_manager'] = ModelManager(brain)
            
            # Create brain stats
            from pynlm.enhanced_api import BrainStats
            self.nlm_cli.context['brain_stats'] = BrainStats(brain)
            
            return {
                'success': True,
                'message': f"Brain '{brain_name}' loaded successfully from {filepath}.",
                'brain_name': brain_name,
                'filepath': filepath,
                'brain': brain,
                'stats': self.nlm_cli.context['brain_stats'].get_summary()
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to load brain '{brain_name}': {e}",
                'error': str(e)
            }
    
    def save_brain(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Save brain model to file."""
        brain_name = args.get('name')
        filepath = args.get('filepath')
        format = args.get('format', 'json')
        compression = args.get('compression', False)
        metadata = args.get('metadata', {})
        
        brain = self.nlm_cli.context.get('brain')
        if not brain:
            raise click.ClickException("No brain loaded. Create or load a brain first.")
        
        if not filepath:
            raise click.ClickException("Filepath is required for saving a brain.")
        
        try:
            from pynlm.enhanced_api import export_model_to_json
            saved_path = export_model_to_json(brain, filepath, metadata)
            
            return {
                'success': True,
                'message': f"Brain '{brain_name}' saved successfully to {saved_path}.",
                'brain_name': brain_name,
                'filepath': saved_path,
                'format': format,
                'compression': compression,
                'metadata': metadata
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to save brain '{brain_name}': {e}",
                'error': str(e)
            }
    
    def compare_brains(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Compare two brain models."""
        brain1_name = args.get('brain1')
        brain2_name = args.get('brain2')
        
        brain1 = self.nlm_cli.context.get('brain')
        brain2 = self.nlm_cli.context.get('brain2')
        
        if not brain1:
            raise click.ClickException("First brain not loaded.")
        if not brain2:
            raise click.ClickException("Second brain not loaded.")
        
        try:
            from pynlm.enhanced_api import ModelManager
            model_manager = ModelManager(brain1)
            
            comparison = model_manager.compare_models(brain1, brain2)
            
            table = Table(title="Brain Comparison")
            table.add_column("Metric", style="cyan")
            table.add_column("Brain 1", style="green")
            table.add_column("Brain 2", style="yellow")
            table.add_column("Difference", style="magenta")
            
            # We need the stats for both brains
            stats1 = brain1.get_stats().get_summary()
            stats2 = brain2.get_stats().get_summary()
            
            metrics = [
                ('Neurons', 'total_neurons', stats1['total_neurons'], stats2['total_neurons']),
                ('Synapses', 'total_synapses', stats1['total_synapses'], stats2['total_synapses']),
                ('Active Neurons', 'active_neurons', stats1['active_neurons'], stats2['active_neurons']),
                ('Firing Neurons', 'firing_neurons', stats1['firing_neurons'], stats2['firing_neurons']),
                ('Total Spikes', 'total_spikes', stats1['total_spikes'], stats2['total_spikes']),
                ('Firing Rate (Hz)', 'average_firing_rate', stats1['average_firing_rate'], stats2['average_firing_rate']),
                ('E/I Ratio', 'excitation_inhibition_ratio', stats1['excitation_inhibition_ratio'], stats2['excitation_inhibition_ratio']),
                ('Development Stage', 'development_stage', stats1['development_stage'], stats2['development_stage']),
                ('Region Count', 'region_count', stats1['region_count'], stats2['region_count']),
            ]
            
            for metric_name, key, value1, value2 in metrics:
                diff = value2 - value1
                table.add_row(metric_name, str(value1), str(value2), str(diff))
            
            table.add_row("Similarity Score", f"{comparison['similarity_score']:.2%}", "-", "-")
            
            console.print(table)
            
            return {
                'success': True,
                'message': f"Comparison between '{brain1_name}' and '{brain2_name}' completed.",
                'comparison': comparison,
                'stats1': stats1,
                'stats2': stats2
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to compare brains: {e}",
                'error': str(e)
            }
    
    def inspect_brain(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Inspect brain properties and statistics."""
        brain_name = args.get('name')
        
        brain = self.nlm_cli.context.get('brain')
        if not brain:
            raise click.ClickException("No brain loaded. Create or load a brain first.")
        
        try:
            # Get comprehensive statistics
            from pynlm.enhanced_api import BrainStats
            brain_stats = BrainStats(brain)
            stats = brain_stats.get_summary()
            
            console.print(f"Brain Inspection: {brain_name}")
            console.print("=" * 50)
            
            # Basic information
            console.print("\nBasic Statistics:")
            console.print(f"  Total Neurons: {stats['total_neurons']}")
            console.print(f"  Total Synapses: {stats['total_synapses']}")
            console.print(f"  Active Neurons: {stats['active_neurons']}")
            console.print(f"  Firing Neurons: {stats['firing_neurons']}")
            console.print(f"  Total Spikes: {stats['total_spikes']}")
            console.print(f"  Average Firing Rate: {stats['average_firing_rate']:.2f} Hz")
            console.print(f"  Excitation/Inhibition Ratio: {stats['excitation_inhibition_ratio']:.2f}")
            
            # Development and regions
            console.print("\nDevelopment & Regions:")
            console.print(f"  Development Stage: {stats['development_stage']}")
            console.print(f"  Region Count: {stats['region_count']}")
            console.print(f"  Region IDs: {[r.value for r in stats['region_ids']]}")
            
            # Timing information
            console.print("\nTiming:")
            console.print(f"  Timestamp: {datetime.fromtimestamp(stats['timestamp']).isoformat()}")
            console.print(f"  Session Age: {self.nlm_cli.session_start} seconds")
            
            return {
                'success': True,
                'message': f"Brain '{brain_name}' inspection completed.",
                'stats': stats
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to inspect brain '{brain_name}': {e}",
                'error': str(e)
            }
    
    def list_brains(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """List available brain files."""
        directory = args.get('directory', '.')
        
        dir_path = Path(directory)
        if not dir_path.exists():
            raise click.ClickException(f"Directory not found: {directory}")
        
        brain_files = []
        for file_path in dir_path.glob('*.json'):
            try:
                with open(file_path, 'r') as f:
                    data = json.load(f)
                    name = data.get('name', file_path.stem)
                    brain_files.append({
                        'name': name,
                        'path': str(file_path),
                        'size': file_path.stat().st_size,
                        'modified': datetime.fromtimestamp(file_path.stat().st_mtime).isoformat()
                    })
            except Exception:
                continue
        
        if not brain_files:
            return {
                'success': True,
                'message': 'No brain files found.',
                'brains': []
            }
        
        table = Table(title="Available Brains")
        table.add_column("Name", style="cyan")
        table.add_column("Path", style="green")
        table.add_column("Size (bytes)", style="yellow")
        table.add_column("Modified", style="magenta")
        
        for brain in brain_files:
            table.add_row(brain['name'], brain['path'], str(brain['size']), brain['modified'][:10])
        
        console.print(table)
        
        return {
            'success': True,
            'message': f"Found {len(brain_files)} brain files.",
            'brains': brain_files
        }
    
    def export_brain(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Export brain to different formats."""
        brain_name = args.get('name')
        output_path = args.get('output')
        format = args.get('format', 'json')
        
        brain = self.nlm_cli.context.get('brain')
        if not brain:
            raise click.ClickException("No brain loaded.")
        
        if not output_path:
            output_path = f"{brain_name}_exported.{format}"
        
        try:
            from pynlm.enhanced_api import ModelManager
            model_manager = ModelManager(brain)
            
            # Determine format from extension if not specified
            if not format:
                format = Path(output_path).suffix.lower()[1:]
            
            # For now, we only support JSON export
            if format == 'json':
                from pynlm.enhanced_api import export_model_to_json
                saved_path = export_model_to_json(brain, output_path)
            else:
                raise click.ClickException(f"Export format '{format}' not yet supported.")
            
            return {
                'success': True,
                'message': f"Brain '{brain_name}' exported to {saved_path} in {format} format.",
                'brain_name': brain_name,
                'output_path': saved_path,
                'format': format
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to export brain '{brain_name}': {e}",
                'error': str(e)
            }
    
    def import_brain(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Import brain from different formats."""
        input_path = args.get('input')
        brain_name = args.get('name')
        format = args.get('format', None)
        
        if not input_path:
            raise click.ClickException("Input file is required for importing a brain.")
        
        if not Path(input_path).exists():
            raise click.ClickException(f"Input file not found: {input_path}")
        
        # Determine format from extension if not specified
        if not format:
            format = Path(input_path).suffix.lower()[1:]
        
        try:
            if format == 'json':
                from pynlm.enhanced_api import import_model_from_json
                brain = import_model_from_json(input_path)
            else:
                raise click.ClickException(f"Import format '{format}' not yet supported.")
            
            # Store brain in context
            self.nlm_cli.context['brain'] = brain
            
            # Create model manager
            from pynlm.enhanced_api import ModelManager
            self.nlm_cli.context['model_manager'] = ModelManager(brain)
            
            # Create brain stats
            from pynlm.enhanced_api import BrainStats
            self.nlm_cli.context['brain_stats'] = BrainStats(brain)
            
            brain_name = brain_name or Path(input_path).stem
            
            return {
                'success': True,
                'message': f"Brain '{brain_name}' imported successfully from {input_path}.",
                'brain_name': brain_name,
                'input_path': input_path,
                'format': format,
                'brain': brain
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to import brain '{brain_name}': {e}",
                'error': str(e)
            }
    
    def reset_brain(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Reset brain state."""
        brain_name = args.get('name')
        
        brain = self.nlm_cli.context.get('brain')
        if not brain:
            raise click.ClickException("No brain loaded.")
        
        try:
            # Reset brain using model manager
            from pynlm.enhanced_api import ModelManager
            model_manager = ModelManager(brain)
            brain.reset()
            
            return {
                'success': True,
                'message': f"Brain '{brain_name}' reset successfully.",
                'brain_name': brain_name
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to reset brain '{brain_name}': {e}",
                'error': str(e)
            }
    
    def get_brain_stats(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get brain statistics."""
        brain_name = args.get('name')
        
        brain = self.nlm_cli.context.get('brain')
        if not brain:
            raise click.ClickException("No brain loaded.")
        
        try:
            from pynlm.enhanced_api import BrainStats
            brain_stats = BrainStats(brain)
            stats = brain_stats.get_summary()
            
            return {
                'success': True,
                'message': f"Brain statistics for '{brain_name}' retrieved successfully.",
                'brain_name': brain_name,
                'stats': stats
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to get brain statistics for '{brain_name}': {e}",
                'error': str(e)
            }