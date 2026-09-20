#!/usr/bin/env python3
"""
Configuration Management Commands for NLM CLI

Handles configuration creation, editing, validation, backup, and restoration.
"""

import json
import shutil
from pathlib import Path
from typing import Dict, Any, Optional
from datetime import datetime

import click
from rich.console import Console
from rich.table import Table
from rich.prompt import Prompt
from rich.syntax import Syntax

from nlm_cli import NLMCLI

console = Console()

class ConfigCommands:
    """Handle configuration management commands."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle configuration commands."""
        command = args.get('command')
        
        handlers = {
            'create': self.create_config,
            'list': self.list_configs,
            'edit': self.edit_config,
            'backup': self.backup_config,
            'restore': self.restore_config,
            'validate': self.validate_config,
            'show': self.show_config,
            'export': self.export_config,
            'import': self.import_config,
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown configuration command: {command}")
    
    def create_config(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Create a new configuration."""
        name = args.get('name')
        description = args.get('description', '')
        parameters = args.get('parameters', {})
        
        configs = self.nlm_cli.load_config()
        
        if name in configs:
            raise click.ClickException(f"Configuration '{name}' already exists.")
        
        config = {
            'name': name,
            'description': description,
            'created_at': datetime.now().isoformat(),
            'last_modified': datetime.now().isoformat(),
            'parameters': parameters
        }
        
        configs[name] = config
        self.nlm_cli.save_config(configs)
        
        return {
            'success': True,
            'message': f"Configuration '{name}' created successfully.",
            'config': config
        }
    
    def list_configs(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """List configurations."""
        name = args.get('name')
        all_flag = args.get('all', False)
        
        configs = self.nlm_cli.load_config()
        
        if not configs:
            return {
                'success': True,
                'message': 'No configurations found.',
                'configs': []
            }
        
        if name:
            if name in configs:
                return {
                    'success': True,
                    'message': f"Configuration '{name}' found.",
                    'config': configs[name]
                }
            else:
                return {
                    'success': False,
                    'message': f"Configuration '{name}' not found.",
                    'error': 'Configuration not found'
                }
        
        if all_flag:
            table = Table(title="All Configurations")
            table.add_column("Name", style="cyan")
            table.add_column("Description", style="green")
            table.add_column("Created", style="yellow")
            table.add_column("Last Modified", style="magenta")
            
            for config_name, config in configs.items():
                created = config.get('created_at', 'Unknown')[:10]
                modified = config.get('last_modified', 'Unknown')[:10]
                description = config.get('description', 'No description')[:30] + (
                    '...' if len(config.get('description', '')) > 30 else '')
                
                table.add_row(config_name, description, created, modified)
            
            console.print(table)
            return {
                'success': True,
                'message': f"Found {len(configs)} configurations.",
                'configs': list(configs.values())
            }
        
        # Simple list
        console.print(f"{'Name':<20} {'Description':<30} {'Created':<12}")
        console.print('-' * 65)
        for config_name, config in configs.items():
            created = config.get('created_at', 'Unknown')[:10]
            description = config.get('description', 'No description')[:27] + (
                '...' if len(config.get('description', '')) > 27 else '')
            
            console.print(f"{config_name:<20} {description:<30} {created:<12}")
        
        return {
            'success': True,
            'message': f"Found {len(configs)} configurations.",
            'configs': list(configs.values())[:10]  # Limit to first 10 for output
        }
    
    def edit_config(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Edit an existing configuration."""
        name = args.get('name')
        parameters = args.get('parameters')
        
        configs = self.nlm_cli.load_config()
        
        if name not in configs:
            raise click.ClickException(f"Configuration '{name}' not found.")
        
        if parameters is not None:
            if isinstance(parameters, dict):
                configs[name]['parameters'] = parameters
            elif isinstance(parameters, str) and parameters.startswith('{'):
                try:
                    configs[name]['parameters'] = json.loads(parameters)
                except json.JSONDecodeError as e:
                    raise click.ClickException(f"Invalid JSON parameters: {e}")
            else:
                # Try to load from file
                param_path = Path(parameters)
                if param_path.exists():
                    try:
                        with open(param_path, 'r') as f:
                            configs[name]['parameters'] = json.load(f)
                    except Exception as e:
                        raise click.ClickException(f"Failed to load parameters file: {e}")
                else:
                    raise click.ClickException(f"Parameters file not found: {parameters}")
        
        configs[name]['last_modified'] = datetime.now().isoformat()
        self.nlm_cli.save_config(configs)
        
        return {
            'success': True,
            'message': f"Configuration '{name}' updated successfully.",
            'config': configs[name]
        }
    
    def backup_config(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Backup a configuration."""
        name = args.get('name')
        output_path = args.get('output')
        
        configs = self.nlm_cli.load_config()
        
        if name not in configs:
            raise click.ClickException(f"Configuration '{name}' not found.")
        
        backup_data = {
            'configuration': configs[name],
            'backup_timestamp': datetime.now().isoformat(),
            'backup_version': '1.0',
            'source': 'nlm_cli_backup'
        }
        
        output = Path(output_path)
        output.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output, 'w') as f:
            json.dump(backup_data, f, indent=2, default=str)
        
        return {
            'success': True,
            'message': f"Configuration '{name}' backed up to {output}.",
            'backup_path': str(output),
            'backup_info': backup_data
        }
    
    def restore_config(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Restore a configuration from backup."""
        name = args.get('name')
        backup_path = args.get('backup')
        force = args.get('force', False)
        
        if not Path(backup_path).exists():
            raise click.ClickException(f"Backup file not found: {backup_path}")
        
        with open(backup_path, 'r') as f:
            backup_data = json.load(f)
        
        if 'configuration' not in backup_data:
            raise click.ClickException("Invalid backup file format.")
        
        config = backup_data['configuration']
        
        configs = self.nlm_cli.load_config()
        
        if name in configs and not force:
            if not click.confirm(f"Configuration '{name}' already exists. Overwrite?", default=False):
                return {
                    'success': False,
                    'message': "Operation cancelled."
                }
        
        configs[name] = config
        self.nlm_cli.save_config(configs)
        
        return {
            'success': True,
            'message': f"Configuration '{name}' restored from {backup_path}.",
            'config': config
        }
    
    def validate_config(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Validate a configuration."""
        name = args.get('name')
        
        configs = self.nlm_cli.load_config()
        
        if name not in configs:
            raise click.ClickException(f"Configuration '{name}' not found.")
        
        config = configs[name]
        validation_result = validate_inputs(**config.get('parameters', {}))
        
        if validation_result['valid']:
            message = f"Configuration '{name}' is valid."
            if validation_result['warnings']:
                message += f" Warnings: {', '.join(validation_result['warnings'])}"
            
            return {
                'success': True,
                'message': message,
                'valid': True,
                'warnings': validation_result['warnings']
            }
        else:
            message = f"Configuration '{name}' validation failed."
            if validation_result['errors']:
                message += f" Errors: {', '.join(validation_result['errors'])}"
            
            return {
                'success': False,
                'message': message,
                'valid': False,
                'errors': validation_result['errors']
            }
    
    def show_config(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show configuration details."""
        name = args.get('name')
        
        configs = self.nlm_cli.load_config()
        
        if name not in configs:
            raise click.ClickException(f"Configuration '{name}' not found.")
        
        config = configs[name]
        
        console.print(f"Configuration: {config['name']}")
        console.print(f"Description: {config.get('description', 'No description')}")
        console.print(f"Created: {config.get('created_at', 'Unknown')}")
        console.print(f"Last Modified: {config.get('last_modified', 'Unknown')}")
        
        if config.get('parameters'):
            console.print("\nParameters:")
            syntax = Syntax(
                json.dumps(config['parameters'], indent=2),
                'json',
                theme='default'
            )
            console.print(syntax)
        
        return {
            'success': True,
            'message': f"Configuration '{name}' displayed.",
            'config': config
        }
    
    def export_config(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Export configuration to file."""
        name = args.get('name')
        output_path = args.get('output')
        
        configs = self.nlm_cli.load_config()
        
        if name not in configs:
            raise click.ClickException(f"Configuration '{name}' not found.")
        
        export_data = {
            'export_timestamp': datetime.now().isoformat(),
            'export_version': '1.0',
            'configuration': configs[name],
            'format': 'nlm_config'
        }
        
        output = Path(output_path) if output_path else Path(f"{name}_config.json")
        output.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        return {
            'success': True,
            'message': f"Configuration '{name}' exported to {output}.",
            'export_path': str(output),
            'export_data': export_data
        }
    
    def import_config(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Import configuration from file."""
        input_path = args.get('input')
        name = args.get('name')
        force = args.get('force', False)
        
        if not Path(input_path).exists():
            raise click.ClickException(f"Import file not found: {input_path}")
        
        with open(input_path, 'r') as f:
            import_data = json.load(f)
        
        # Extract configuration from import data
        if 'configuration' in import_data:
            config = import_data['configuration']
        elif 'config' in import_data:
            config = import_data['config']
        else:
            config = import_data
        
        config_name = name or config.get('name', 'imported_config')
        
        configs = self.nlm_cli.load_config()
        
        if config_name in configs and not force:
            if not click.confirm(f"Configuration '{config_name}' already exists. Overwrite?", default=False):
                return {
                    'success': False,
                    'message': "Operation cancelled."
                }
        
        config['last_modified'] = datetime.now().isoformat()
        configs[config_name] = config
        self.nlm_cli.save_config(configs)
        
        return {
            'success': True,
            'message': f"Configuration '{config_name}' imported from {input_path}.",
            'config': config
        }