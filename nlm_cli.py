#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Command Line Interface

Comprehensive CLI tools for managing NLM simulations, brains, experiments, and configurations.
"""

import sys
import os
import json
import time
import click
from pathlib import Path
from typing import Dict, Any, Optional, List, Union
from datetime import datetime

# Import from the enhanced API
try:
    from pynlm.enhanced_api import (
        create_enhanced_brain,
        create_enhanced_world,
        create_enhanced_agent,
        run_enhanced_simulation,
        export_model_to_json,
        import_model_from_json,
        validate_inputs,
        get_version_info,
        create_error_handler,
        NLMError,
        ConfigurationError,
        SimulationError,
        VisualizationError,
        MemoryError,
        ValidationError,
        BrainStats,
        SimulationControls,
        ModelManager,
        VisualizationInterface,
        MemoryManager,
        QueryBuilder,
        SimulationContext,
        BrainContext,
        createBrain,
        createSimpleWorld,
        createAgentBrain,
        createDefaultConfig,
        InvalidNeuronId,
        InvalidSynapseId,
        InvalidRegionId,
        InvalidPopulationId
    )
except ImportError:
    # Fallback if pynlm is not available
    print("Error: pynlm package not found. Please install it with: pip install pynlm")
    sys.exit(1)

# Import command modules
from config_commands import ConfigCommands
from brain_commands import BrainCommands
from simulation_commands import SimulationCommands
from experiment_commands import ExperimentCommands
from analysis_commands import AnalysisCommands
from plugin_commands import PluginCommands
from system_commands import SystemCommands
from cli_help import CliHelp
from cli_examples import CliExamples

class NLMCLI:
    """Main NLM CLI application class."""
    
    def __init__(self):
        self.version_info = get_version_info()
        self.error_handler = create_error_handler()
        self.config_file = self._get_config_path()
        self.context = {
            'brain': None,
            'world': None,
            'agent': None,
            'simulation': None,
            'visualization': None,
            'memory': None,
            'query': None
        }
        self.session_start = time.time()
    
    def _get_config_path(self) -> Path:
        """Get the path to the NLM configuration file."""
        config_dir = Path.home() / '.nlm'
        config_dir.mkdir(exist_ok=True)
        return config_dir / 'config.json'
    
    def load_config(self, config_file: Optional[Path] = None) -> Dict[str, Any]:
        """Load configuration from file."""
        config_path = config_file or self.config_file
        
        if config_path.exists():
            try:
                with open(config_path, 'r') as f:
                    return json.load(f)
            except Exception as e:
                self.error_handler.handle_error(e, f"Failed to load config from {config_path}")
                return {}
        return {}
    
    def save_config(self, config: Dict[str, Any], config_file: Optional[Path] = None):
        """Save configuration to file."""
        config_path = config_file or self.config_file
        
        try:
            config_path.parent.mkdir(parents=True, exist_ok=True)
            with open(config_path, 'w') as f:
                json.dump(config, f, indent=2, default=str)
        except Exception as e:
            self.error_handler.handle_error(e, f"Failed to save config to {config_path}")
    
    def run_command(self, command: str, args: Dict[str, Any] = None):
        """Run a command with given arguments."""
        if args is None:
            args = {}
        
        try:
            # Dispatch to appropriate command handler
            if command == 'config':
                handler = ConfigCommands(self)
                return handler.handle(args)
            elif command == 'brain':
                handler = BrainCommands(self)
                return handler.handle(args)
            elif command == 'simulation':
                handler = SimulationCommands(self)
                return handler.handle(args)
            elif command == 'experiment':
                handler = ExperimentCommands(self)
                return handler.handle(args)
            elif command == 'analysis':
                handler = AnalysisCommands(self)
                return handler.handle(args)
            elif command == 'plugin':
                handler = PluginCommands(self)
                return handler.handle(args)
            elif command == 'system':
                handler = SystemCommands(self)
                return handler.handle(args)
            elif command == 'help':
                handler = CliHelp(self)
                return handler.handle(args)
            elif command == 'examples':
                handler = CliExamples(self)
                return handler.handle(args)
            else:
                raise NLMError(f"Unknown command: {command}")
        except Exception as e:
            self.error_handler.handle_error(e, f"Command {command} failed")
            return {'success': False, 'error': str(e)}
    
    def get_session_info(self) -> Dict[str, Any]:
        """Get information about the current session."""
        return {
            'session_id': f"nlm_{int(self.session_start)}",
            'start_time': datetime.fromtimestamp(self.session_start).isoformat(),
            'version': self.version_info['api_version'],
            'context': self.context
        }

@click.group()
@click.version_option(self.version_info['api_version'])
@click.option('--config', type=click.Path(), help='Path to configuration file')
@click.option('--output', '-o', type=click.Path(), help='Output file path')
@click.option('--format', '-f', type=click.Choice(['text', 'json']), default='text', help='Output format')
@click.option('--quiet', '-q', is_flag=True, help='Suppress output')
@click.option('--verbose', '-v', is_flag=True, help='Verbose output')
@click.pass_context
def cli(ctx, config, output, format, quiet, verbose):
    """
    NLM (Neural Learning Machine) Command Line Interface
    
    A comprehensive CLI for managing NLM simulations, brains, experiments, and configurations.
    
    Features:
    - Configuration Management
    - Brain Management
    - Simulation Control
    - Experiment Management
    - Analysis Tools
    - Plugin Management
    - Error Handling and Logging
    - System Management
    """
    ctx.ensure_object(dict)
    ctx.obj = {
        'config_file': Path(config) if config else None,
        'output_file': Path(output) if output else None,
        'output_format': format,
        'quiet': quiet,
        'verbose': verbose,
        'start_time': time.time()
    }
    
    # Initialize NLM CLI instance
    ctx.obj['nlm_cli'] = NLMCLI()

@cli.command()
@click.option('--name', required=True, help='Name for the configuration')
@click.option('--description', help='Description of the configuration')
@click.option('--parameters', help='JSON file containing parameters or inline JSON')
@click.pass_context
def create_config(ctx, name, description, parameters):
    """Create a new configuration file."""
    nlm_cli = ctx.obj['nlm_cli']
    
    config = {
        'name': name,
        'description': description or '',
        'created_at': datetime.now().isoformat(),
        'parameters': {}
    }
    
    if parameters:
        if parameters.startswith('{') and parameters.endswith('}'):
            # Inline JSON
            try:
                config['parameters'] = json.loads(parameters)
            except json.JSONDecodeError as e:
                raise click.BadParameter(f"Invalid JSON in parameters: {e}")
        else:
            # File path
            param_path = Path(parameters)
            if param_path.exists():
                try:
                    with open(param_path, 'r') as f:
                        config['parameters'] = json.load(f)
                except Exception as e:
                    raise click.BadParameter(f"Failed to load parameters file: {e}")
            else:
                raise click.BadParameter(f"Parameters file not found: {parameters}")
    
    # Save configuration
    config_file = ctx.obj['nlm_cli'].config_file
    all_configs = nlm_cli.load_config() if config_file.exists() else {}
    all_configs[name] = config
    nlm_cli.save_config(all_configs, config_file)
    
    click.echo(f"Configuration '{name}' created successfully!")

@cli.command()
@click.option('--name', help='Name of configuration to list (show all if not specified)')
@click.option('--all', '-a', is_flag=True, help='List all configurations')
@click.pass_context
def list_configs(ctx, name, all):
    """List available configurations."""
    nlm_cli = ctx.obj['nlm_cli']
    configs = nlm_cli.load_config()
    
    if not configs:
        click.echo("No configurations found.")
        return
    
    if name:
        if name in configs:
            config = configs[name]
            click.echo(f"Configuration: {name}")
            click.echo(f"Description: {config.get('description', 'No description')}")
            click.echo(f"Created: {config.get('created_at', 'Unknown')}")
            if 'parameters' in config and config['parameters']:
                click.echo("Parameters:")
                click.echo(json.dumps(config['parameters'], indent=2))
        else:
            click.echo(f"Configuration '{name}' not found.")
    elif all:
        click.echo(f"{'Name':<20} {'Description':<40} {'Created':<20}")
        click.echo('-' * 80)
        for config_name, config in configs.items():
            desc = config.get('description', 'No description')[:37] + ('...' if len(desc) > 37 else '')
            created = config.get('created_at', 'Unknown')[:19]
            click.echo(f"{config_name:<20} {desc:<40} {created:<20}")
    else:
        click.echo("Available configurations:")
        for config_name, config in configs.items():
            click.echo(f"  - {config_name}: {config.get('description', 'No description')}")

@cli.command()
@click.option('--name', required=True, help='Name of configuration to edit')
@click.option('--parameters', help='JSON file containing new parameters or inline JSON')
@click.pass_context
def edit_config(ctx, name, parameters):
    """Edit an existing configuration."""
    nlm_cli = ctx.obj['nlm_cli']
    configs = nlm_cli.load_config()
    
    if name not in configs:
        raise click.ClickException(f"Configuration '{name}' not found.")
    
    if parameters:
        if parameters.startswith('{') and parameters.endswith('}'):
            # Inline JSON
            try:
                configs[name]['parameters'] = json.loads(parameters)
            except json.JSONDecodeError as e:
                raise click.BadParameter(f"Invalid JSON in parameters: {e}")
        else:
            # File path
            param_path = Path(parameters)
            if param_path.exists():
                try:
                    with open(param_path, 'r') as f:
                        configs[name]['parameters'] = json.load(f)
                except Exception as e:
                    raise click.BadParameter(f"Failed to load parameters file: {e}")
            else:
                raise click.BadParameter(f"Parameters file not found: {parameters}")
    
    # Update timestamp
    configs[name]['last_modified'] = datetime.now().isoformat()
    
    # Save configuration
    nlm_cli.save_config(configs, ctx.obj['nlm_cli'].config_file)
    
    click.echo(f"Configuration '{name}' updated successfully!")

@cli.command()
@click.option('--name', required=True, help='Name of configuration to backup')
@click.option('--output', '-o', required=True, help='Output file path for backup')
@click.pass_context
def backup_config(ctx, name, output):
    """Backup a configuration."""
    nlm_cli = ctx.obj['nlm_cli']
    configs = nlm_cli.load_config()
    
    if name not in configs:
        raise click.ClickException(f"Configuration '{name}' not found.")
    
    backup_data = {
        'configuration': configs[name],
        'backup_timestamp': datetime.now().isoformat(),
        'backup_version': '1.0'
    }
    
    try:
        output_path = Path(output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w') as f:
            json.dump(backup_data, f, indent=2, default=str)
        
        click.echo(f"Configuration '{name}' backed up to {output}!")
    except Exception as e:
        raise click.ClickException(f"Failed to backup configuration: {e}")

@cli.command()
@click.option('--name', required=True, help='Name of configuration to restore')
@click.option('--backup', required=True, help='Backup file path')
@click.option('--force', '-f', is_flag=True, help='Overwrite existing configuration')
@click.pass_context
def restore_config(ctx, name, backup, force):
    """Restore a configuration from backup."""
    nlm_cli = ctx.obj['nlm_cli']
    configs = nlm_cli.load_config()
    
    if not Path(backup).exists():
        raise click.ClickException(f"Backup file not found: {backup}")
    
    try:
        with open(backup, 'r') as f:
            backup_data = json.load(f)
        
        if 'configuration' not in backup_data:
            raise click.ClickException("Invalid backup file format.")
        
        config = backup_data['configuration']
        
        if name in configs and not force:
            click.confirm(f"Configuration '{name}' already exists. Overwrite?", abort=True)
        
        configs[name] = config
        nlm_cli.save_config(configs, ctx.obj['nlm_cli'].config_file)
        
        click.echo(f"Configuration '{name}' restored from {backup}!")
    except Exception as e:
        raise click.ClickException(f"Failed to restore configuration: {e}")

@cli.command()
@click.option('--name', required=True, help='Name of configuration to validate')
@click.pass_context
def validate_config(ctx, name):
    """Validate a configuration file."""
    nlm_cli = ctx.obj['nlm_cli']
    configs = nlm_cli.load_config()
    
    if name not in configs:
        raise click.ClickException(f"Configuration '{name}' not found.")
    
    config = configs[name]
    
    # Basic validation
    errors = []
    warnings = []
    
    if 'name' not in config:
        errors.append("Configuration missing 'name' field")
    
    if not isinstance(config.get('name', ''), str):
        errors.append("Configuration name must be a string")
    
    if 'parameters' in config and not isinstance(config['parameters'], dict):
        errors.append("Configuration parameters must be a JSON object")
    
    # Validate parameters if present
    if 'parameters' in config:
        validation_result = validate_inputs(**config['parameters'])
        if not validation_result['valid']:
            errors.extend(validation_result['errors'])
        if validation_result['warnings']:
            warnings.extend(validation_result['warnings'])
    
    if errors:
        click.echo("Configuration validation failed:")
        for error in errors:
            click.echo(f"  ERROR: {error}")
        return False
    else:
        click.echo(f"Configuration '{name}' is valid!")
        if warnings:
            click.echo("Warnings:")
            for warning in warnings:
                click.echo(f"  WARNING: {warning}")
        return True

if __name__ == '__main__':
    cli()