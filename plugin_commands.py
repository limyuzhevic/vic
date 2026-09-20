#!/usr/bin/env python3
"""
Plugin Management Commands for NLM CLI

Handles plugin listing, installation, configuration, and management.
"""

import json
import time
import subprocess
import sys
from pathlib import Path
from typing import Dict, Any, Optional, List
from datetime import datetime

import click
from rich.console import Console
from rich.table import Table
from rich.syntax import Syntax
from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn

from nlm_cli import NLMCLI

console = Console()

class PluginCommands:
    """Handle plugin management commands."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle plugin commands."""
        command = args.get('command')
        
        handlers = {
            'list': self.list_plugins,
            'install': self.install_plugin,
            'uninstall': self.uninstall_plugin,
            'configure': self.configure_plugin,
            'enable': self.enable_plugin,
            'disable': self.disable_plugin,
            'info': self.get_plugin_info,
            'check': self.check_plugin_status,
            'update': self.update_plugin,
            'search': self.search_plugins,
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown plugin command: {command}")
    
    def list_plugins(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """List available plugins."""
        source = args.get('source', 'local')
        
        plugins = []
        
        # Get local plugins
        local_plugins = self._get_local_plugins()
        
        # Get remote plugins if requested
        remote_plugins = []
        if source in ['remote', 'all']:
            remote_plugins = self._get_remote_plugins()
        
        # Combine plugins
        plugins = local_plugins + remote_plugins if source == 'all' else local_plugins
        
        if not plugins:
            return {
                'success': True,
                'message': 'No plugins found.',
                'plugins': []
            }
        
        if source == 'local':
            table = Table(title="Local Plugins")
            table.add_column("Name", style="cyan")
            table.add_column("Version", style="green")
            table.add_column("Status", style="yellow")
            table.add_column("Description", style="magenta")
            table.add_column("Path", style="blue")
            
            for plugin in plugins:
                status = "Enabled" if plugin.get('enabled', True) else "Disabled"
                table.add_row(
                    plugin['name'],
                    plugin.get('version', 'Unknown'),
                    status,
                    plugin.get('description', 'No description')[:30] + ('...' if len(plugin.get('description', '')) > 30 else ''),
                    plugin.get('path', 'Unknown')
                )
            
            console.print(table)
        else:
            console.print(f"Found {len(plugins)} plugins (including remote).")
        
        return {
            'success': True,
            'message': f"Found {len(plugins)} plugins.",
            'plugins': plugins
        }
    
    def install_plugin(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Install a plugin."""
        name = args.get('name')
        source = args.get('source', 'pypi')
        version = args.get('version')
        
        if not name:
            raise click.ClickException("Plugin name is required for installation.")
        
        # Check if plugin already exists
        local_plugins = self._get_local_plugins()
        for plugin in local_plugins:
            if plugin['name'] == name:
                raise click.ClickException(f"Plugin '{name}' is already installed.")
        
        # Install plugin based on source
        if source == 'pypi':
            installed_path = self._install_from_pypi(name, version)
        elif source == 'local':
            source_path = args.get('source_path')
            if not source_path:
                raise click.ClickException("Source path is required for local installation.")
            installed_path = self._install_from_local(source_path, name)
        elif source == 'git':
            git_url = args.get('git_url')
            if not git_url:
                raise click.ClickException("Git URL is required for git installation.")
            installed_path = self._install_from_git(git_url, name)
        else:
            raise click.ClickException(f"Unsupported plugin source: {source}")
        
        # Load plugin metadata
        plugin_info = self._load_plugin_metadata(installed_path)
        
        # Save plugin metadata
        self._save_plugin_metadata(plugin_info)
        
        return {
            'success': True,
            'message': f"Plugin '{name}' installed successfully at {installed_path}.",
            'plugin_name': name,
            'plugin_info': plugin_info,
            'source': source,
            'installed_path': installed_path
        }
    
    def uninstall_plugin(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Uninstall a plugin."""
        name = args.get('name')
        force = args.get('force', False)
        
        local_plugins = self._get_local_plugins()
        
        target_plugin = None
        for plugin in local_plugins:
            if plugin['name'] == name:
                target_plugin = plugin
                break
        
        if not target_plugin:
            raise click.ClickException(f"Plugin '{name}' not found.")
        
        # Check if plugin is in use
        if not force:
            # Check if plugin is enabled and being used
            enabled_plugins = [p for p in local_plugins if p.get('enabled', True)]
            if len(enabled_plugins) <= 1:
                if not click.confirm(f"Plugin '{name}' is the only enabled plugin. Uninstall anyway?", default=False):
                    return {
                        'success': False,
                        'message': "Operation cancelled."
                    }
        
        # Remove plugin files
        plugin_path = Path(target_plugin['path'])
        if plugin_path.exists():
            import shutil
            shutil.rmtree(plugin_path)
        
        # Remove plugin metadata
        self._remove_plugin_metadata(name)
        
        return {
            'success': True,
            'message': f"Plugin '{name}' uninstalled successfully.",
            'plugin_name': name,
            'removed_path': str(plugin_path) if plugin_path.exists() else None
        }
    
    def configure_plugin(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Configure a plugin."""
        name = args.get('name')
        config_file = args.get('config_file')
        
        local_plugins = self._get_local_plugins()
        
        target_plugin = None
        for plugin in local_plugins:
            if plugin['name'] == name:
                target_plugin = plugin
                break
        
        if not target_plugin:
            raise click.ClickException(f"Plugin '{name}' not found.")
        
        # Load existing configuration
        plugin_config_path = Path(target_plugin['path']) / 'plugin_config.json'
        config = {}
        
        if config_file:
            if not Path(config_file).exists():
                raise click.ClickException(f"Configuration file not found: {config_file}")
            
            with open(config_file, 'r') as f:
                config = json.load(f)
        elif plugin_config_path.exists():
            with open(plugin_config_path, 'r') as f:
                config = json.load(f)
        
        # Show current configuration
        if config:
            console.print(f"Current configuration for '{name}':")
            console.print(json.dumps(config, indent=2))
        else:
            console.print(f"No configuration found for '{name}'.")
        
        # Save configuration
        if config_file:
            # Update from file
            with open(config_file, 'r') as f:
                config = json.load(f)
        else:
            # Interactive configuration
            console.print("Please enter configuration values (leave blank to keep existing):")
            config = self._interactive_configure_plugin(target_plugin)
        
        # Save configuration
        with open(plugin_config_path, 'w') as f:
            json.dump(config, f, indent=2, default=str)
        
        return {
            'success': True,
            'message': f"Plugin '{name}' configured successfully.",
            'plugin_name': name,
            'configuration': config
        }
    
    def enable_plugin(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Enable a plugin."""
        name = args.get('name')
        
        local_plugins = self._get_local_plugins()
        
        target_plugin = None
        for plugin in local_plugins:
            if plugin['name'] == name:
                target_plugin = plugin
                break
        
        if not target_plugin:
            raise click.ClickException(f"Plugin '{name}' not found.")
        
        if target_plugin.get('enabled', True):
            return {
                'success': True,
                'message': f"Plugin '{name}' is already enabled.",
                'plugin_name': name,
                'plugin': target_plugin
            }
        
        # Update plugin metadata
        target_plugin['enabled'] = True
        target_plugin['last_enabled'] = datetime.now().isoformat()
        
        # Save updated metadata
        self._save_plugin_metadata(target_plugin)
        
        return {
            'success': True,
            'message': f"Plugin '{name}' enabled successfully.",
            'plugin_name': name,
            'plugin': target_plugin
        }
    
    def disable_plugin(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Disable a plugin."""
        name = args.get('name')
        
        local_plugins = self._get_local_plugins()
        
        target_plugin = None
        for plugin in local_plugins:
            if plugin['name'] == name:
                target_plugin = plugin
                break
        
        if not target_plugin:
            raise click.ClickException(f"Plugin '{name}' not found.")
        
        if not target_plugin.get('enabled', True):
            return {
                'success': True,
                'message': f"Plugin '{name}' is already disabled.",
                'plugin_name': name,
                'plugin': target_plugin
            }
        
        # Update plugin metadata
        target_plugin['enabled'] = False
        target_plugin['last_disabled'] = datetime.now().isoformat()
        
        # Save updated metadata
        self._save_plugin_metadata(target_plugin)
        
        return {
            'success': True,
            'message': f"Plugin '{name}' disabled successfully.",
            'plugin_name': name,
            'plugin': target_plugin
        }
    
    def get_plugin_info(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get detailed information about a plugin."""
        name = args.get('name')
        
        local_plugins = self._get_local_plugins()
        
        target_plugin = None
        for plugin in local_plugins:
            if plugin['name'] == name:
                target_plugin = plugin
                break
        
        if not target_plugin:
            raise click.ClickException(f"Plugin '{name}' not found.")
        
        # Load detailed plugin information
        detailed_info = self._get_detailed_plugin_info(target_plugin)
        
        console.print(f"Plugin Information: {name}")
        console.print("=" * 50)
        console.print(f"Name: {detailed_info.get('name', 'Unknown')}")
        console.print(f"Version: {detailed_info.get('version', 'Unknown')}")
        console.print(f"Description: {detailed_info.get('description', 'No description')}")
        console.print(f"Author: {detailed_info.get('author', 'Unknown')}")
        console.print(f"License: {detailed_info.get('license', 'Unknown')}")
        console.print(f"Status: {'Enabled' if detailed_info.get('enabled', True) else 'Disabled'}")
        
        if detailed_info.get('dependencies'):
            console.print(f"Dependencies: {', '.join(detailed_info['dependencies'])}")
        
        if detailed_info.get('requirements'):
            console.print(f"Requirements: {detailed_info['requirements']}")
        
        console.print(f"Path: {detailed_info.get('path', 'Unknown')}")
        console.print(f"Last Updated: {detailed_info.get('last_updated', 'Unknown')}")
        
        return {
            'success': True,
            'message': f"Plugin information for '{name}' retrieved.",
            'plugin_name': name,
            'plugin_info': detailed_info
        }
    
    def check_plugin_status(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Check plugin status and dependencies."""
        name = args.get('name')
        
        local_plugins = self._get_local_plugins()
        
        target_plugin = None
        for plugin in local_plugins:
            if plugin['name'] == name:
                target_plugin = plugin
                break
        
        if not target_plugin:
            raise click.ClickException(f"Plugin '{name}' not found.")
        
        # Check plugin status
        status_info = self._check_plugin_status(target_plugin)
        
        console.print(f"Plugin Status Check: {name}")
        console.print("=" * 50)
        
        for check_name, check_result in status_info.items():
            status = check_result.get('status', 'unknown')
            message = check_result.get('message', '')
            
            status_color = "green" if status == "ok" else "yellow" if status == "warning" else "red"
            console.print(f"{check_name}: [{status_color}]{status}[/{status_color}] {message}")
        
        return {
            'success': True,
            'message': f"Plugin status check for '{name}' completed.",
            'plugin_name': name,
            'status_info': status_info
        }
    
    def update_plugin(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Update a plugin."""
        name = args.get('name')
        version = args.get('version')
        source = args.get('source', 'pypi')
        
        local_plugins = self._get_local_plugins()
        
        target_plugin = None
        for plugin in local_plugins:
            if plugin['name'] == name:
                target_plugin = plugin
                break
        
        if not target_plugin:
            raise click.ClickException(f"Plugin '{name}' not found.")
        
        # Update plugin based on source
        if source == 'pypi':
            updated_info = self._update_from_pypi(name, version)
        elif source == 'git':
            git_url = args.get('git_url')
            updated_info = self._update_from_git(git_url, name)
        else:
            raise click.ClickException(f"Unsupported update source: {source}")
        
        return {
            'success': True,
            'message': f"Plugin '{name}' updated successfully.",
            'plugin_name': name,
            'updated_info': updated_info
        }
    
    def search_plugins(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Search for plugins."""
        query = args.get('query')
        source = args.get('source', 'pypi')
        
        if not query:
            raise click.ClickException("Search query is required.")
        
        # Search for plugins
        if source == 'pypi':
            search_results = self._search_pypi(query)
        elif source == 'local':
            search_results = self._search_local(query)
        else:
            raise click.ClickException(f"Unsupported search source: {source}")
        
        # Display results
        console.print(f"Search results for '{query}':")
        console.print("=" * 50)
        
        for plugin in search_results:
            console.print(f"Name: {plugin.get('name', 'Unknown')}")
            console.print(f"Description: {plugin.get('description', 'No description')}")
            console.print(f"Version: {plugin.get('version', 'Unknown')}")
            console.print(f"Author: {plugin.get('author', 'Unknown')}")
            console.print("-" * 30)
        
        return {
            'success': True,
            'message': f"Found {len(search_results)} plugins matching '{query}'.",
            'search_query': query,
            'search_results': search_results
        }
    
    # Plugin management helper methods
    
    def _get_local_plugins(self) -> List[Dict[str, Any]]:
        """Get list of local plugins."""
        plugins = []
        
        # Check for plugins directory
        plugins_dir = Path.home() / '.nlm' / 'plugins'
        
        if not plugins_dir.exists():
            plugins_dir.mkdir(parents=True, exist_ok=True)
        
        # Look for plugin directories or files
        for item in plugins_dir.iterdir():
            if item.is_dir():
                # Directory-based plugin
                plugin_file = item / 'plugin.json'
                if plugin_file.exists():
                    try:
                        with open(plugin_file, 'r') as f:
                            plugin_info = json.load(f)
                            plugin_info['path'] = str(item)
                            plugins.append(plugin_info)
                    except Exception:
                        pass
            elif item.is_file() and item.suffix == '.json':
                # File-based plugin
                try:
                    with open(item, 'r') as f:
                        plugin_info = json.load(f)
                        plugin_info['path'] = str(item)
                        plugins.append(plugin_info)
                except Exception:
                    pass
        
        return plugins
    
    def _get_remote_plugins(self) -> List[Dict[str, Any]]:
        """Get list of remote plugins (placeholder implementation)."""
        # This would typically query a plugin registry or API
        # For now, return empty list
        return []
    
    def _install_from_pypi(self, name: str, version: Optional[str] = None) -> str:
        """Install plugin from PyPI."""
        try:
            # Construct package name
            package_name = f"nlm-plugin-{name}"
            
            # Install using pip
            cmd = [sys.executable, '-m', 'pip', 'install', package_name]
            
            if version:
                cmd.extend(['==', version])
            
            subprocess.run(cmd, check=True, capture_output=True, text=True)
            
            # Return installation path (placeholder)
            install_dir = Path.home() / '.nlm' / 'plugins' / name
            install_dir.mkdir(parents=True, exist_ok=True)
            
            return str(install_dir)
        except subprocess.CalledProcessError as e:
            raise click.ClickException(f"Failed to install plugin from PyPI: {e.stderr}")
    
    def _install_from_local(self, source_path: str, name: Optional[str] = None) -> str:
        """Install plugin from local source."""
        source = Path(source_path)
        
        if not source.exists():
            raise click.ClickException(f"Source path does not exist: {source_path}")
        
        # Determine plugin name
        plugin_name = name or source.stem
        
        # Create target directory
        target_dir = Path.home() / '.nlm' / 'plugins' / plugin_name
        
        # Copy source to target
        import shutil
        if target_dir.exists():
            shutil.rmtree(target_dir)
        
        shutil.copytree(source, target_dir)
        
        return str(target_dir)
    
    def _install_from_git(self, git_url: str, name: Optional[str] = None) -> str:
        """Install plugin from Git repository."""
        # Clone repository
        import git
        
        # Determine plugin name
        plugin_name = name or git_url.split('/')[-1].replace('.git', '')
        
        # Clone
        repo = git.Repo.clone_from(git_url, Path.home() / '.nlm' / 'plugins' / plugin_name)
        repo.git.checkout('main')  # Use main branch if exists
        
        return str(repo.working_dir)
    
    def _load_plugin_metadata(self, plugin_path: str) -> Dict[str, Any]:
        """Load plugin metadata from path."""
        plugin_file = Path(plugin_path) / 'plugin.json'
        
        if plugin_file.exists():
            with open(plugin_file, 'r') as f:
                metadata = json.load(f)
        else:
            # Create default metadata
            metadata = {
                'name': Path(plugin_path).name,
                'version': '1.0.0',
                'description': 'Plugin',
                'author': 'Unknown',
                'license': 'MIT',
                'enabled': True,
                'dependencies': [],
                'requirements': 'python>=3.8',
                'last_updated': datetime.now().isoformat()
            }
        
        return metadata
    
    def _save_plugin_metadata(self, metadata: Dict[str, Any]):
        """Save plugin metadata."""
        plugin_name = metadata.get('name')
        if not plugin_name:
            return
        
        plugin_dir = Path.home() / '.nlm' / 'plugins' / plugin_name
        plugin_file = plugin_dir / 'plugin.json'
        
        plugin_dir.mkdir(parents=True, exist_ok=True)
        
        with open(plugin_file, 'w') as f:
            json.dump(metadata, f, indent=2, default=str)
    
    def _remove_plugin_metadata(self, name: str):
        """Remove plugin metadata."""
        plugin_dir = Path.home() / '.nlm' / 'plugins' / name
        
        if plugin_dir.exists():
            import shutil
            shutil.rmtree(plugin_dir)
    
    def _interactive_configure_plugin(self, plugin: Dict[str, Any]) -> Dict[str, Any]:
        """Interactive configuration of plugin."""
        config = {}
        
        console.print("Enter configuration values (press Enter to skip):")
        
        # Get configuration from user
        try:
            custom_config = click.prompt("Custom configuration (JSON format)", default='{}')
            if custom_config:
                config = json.loads(custom_config)
        except json.JSONDecodeError:
            console.print("Invalid JSON. Using empty configuration.")
        
        return config
    
    def _get_detailed_plugin_info(self, plugin: Dict[str, Any]) -> Dict[str, Any]:
        """Get detailed information about a plugin."""
        detailed_info = plugin.copy()
        
        # Load plugin configuration if exists
        config_file = Path(plugin['path']) / 'plugin_config.json'
        if config_file.exists():
            try:
                with open(config_file, 'r') as f:
                    detailed_info['configuration'] = json.load(f)
            except Exception:
                pass
        
        return detailed_info
    
    def _check_plugin_status(self, plugin: Dict[str, Any]) -> Dict[str, Any]:
        """Check plugin status and dependencies."""
        status = {}
        
        # Check if plugin directory exists
        plugin_path = Path(plugin.get('path', ''))
        status['directory'] = {
            'status': 'ok' if plugin_path.exists() else 'error',
            'message': f"Plugin directory: {'Found' if plugin_path.exists() else 'Not found'}"
        }
        
        # Check if plugin metadata exists
        plugin_file = plugin_path / 'plugin.json'
        status['metadata'] = {
            'status': 'ok' if plugin_file.exists() else 'warning',
            'message': f"Plugin metadata: {'Found' if plugin_file.exists() else 'Not found'}"
        }
        
        # Check if plugin is enabled
        enabled = plugin.get('enabled', True)
        status['enabled'] = {
            'status': 'ok' if enabled else 'warning',
            'message': f"Plugin status: {'Enabled' if enabled else 'Disabled'}"
        }
        
        # Check dependencies
        dependencies = plugin.get('dependencies', [])
        if dependencies:
            status['dependencies'] = {
                'status': 'warning',
                'message': f"Plugin has {len(dependencies)} dependencies that need to be checked"
            }
        else:
            status['dependencies'] = {
                'status': 'ok',
                'message': "No dependencies required"
            }
        
        return status
    
    def _update_from_pypi(self, name: str, version: Optional[str] = None) -> Dict[str, Any]:
        """Update plugin from PyPI."""
        # This would typically check for updates and perform them
        # For now, return a placeholder
        updated_info = {
            'name': name,
            'version': version or 'latest',
            'updated': datetime.now().isoformat(),
            'update_type': 'pypi'
        }
        
        return updated_info
    
    def _update_from_git(self, git_url: str, name: Optional[str] = None) -> Dict[str, Any]:
        """Update plugin from Git repository."""
        # This would typically fetch the latest version and update
        # For now, return a placeholder
        updated_info = {
            'name': name or Path(git_url).name.replace('.git', ''),
            'updated': datetime.now().isoformat(),
            'update_type': 'git'
        }
        
        return updated_info
    
    def _search_pypi(self, query: str) -> List[Dict[str, Any]]:
        """Search for plugins on PyPI (placeholder implementation)."""
        # This would typically query the PyPI API
        # For now, return empty list
        return []
    
    def _search_local(self, query: str) -> List[Dict[str, Any]]:
        """Search for plugins locally."""
        local_plugins = self._get_local_plugins()
        
        # Filter by query
        results = []
        for plugin in local_plugins:
            if query.lower() in plugin.get('name', '').lower() or \
               query.lower() in plugin.get('description', '').lower():
                results.append(plugin)
        
        return results
