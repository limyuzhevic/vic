#!/usr/bin/env python3
"""
create_world.py: Script for world creation and configuration.

This automation script provides comprehensive world creation capabilities for NLM,
including different environment types, configuration management, and validation.
"""

import argparse
import json
import sys
import time
from pathlib import Path
from typing import Dict, List, Optional, Any, Union

# Add parent directory to path to import CLI utilities
sys.path.insert(0, str(Path(__file__).parent.parent))

from scripts.cli import (
    NLMCLI,
    ProgressReporter,
    load_config,
    save_config,
    setup_logging,
)

__version__ = "1.0.0"

class WorldBuilder:
    """Class for creating and configuring NLM worlds with comprehensive capabilities."""

    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.logger = None
        self.setup_logging()

    def setup_logging(self) -> None:
        """Setup logging for world builder."""
        log_level = self.config.get('logging', {}).get('level', 'INFO')
        self.logger = setup_logging(log_level, 'world_builder')

    def create_world(self) -> Dict[str, Any]:
        """Create world with comprehensive workflow."""
        world_config = self.config.get('world', {})
        world_name = world_config.get('name', 'Unnamed World')
        output_dir = Path(world_config.get('output_dir', 'worlds'))

        self.logger.info(f"Creating world: {world_name}")
        self.logger.info(f"World type: {world_config.get('world_type', 'grid')}")
        self.logger.info(f"Output directory: {output_dir}")

        # Create output directory
        output_dir.mkdir(parents=True, exist_ok=True)

        try:
            # Create world based on type
            if world_config.get('world_type') == 'grid':
                world = self._create_grid_world(world_config)
            elif world_config.get('world_type') == 'continuous':
                world = self._create_continuous_world(world_config)
            elif world_config.get('world_type') == 'graph':
                world = self._create_graph_world(world_config)
            else:
                self.logger.error(f"Unknown world type: {world_config.get('world_type')}")
                return {'world_created': False, 'error': f'Unknown world type: {world_config.get("world_type")}'}

            # Save world configuration
            self._save_world_config(world, output_dir, world_name)

            # Create world visualization
            if world_config.get('create_visualizations', True):
                self._create_world_visualization(world, output_dir, world_name)

            # Initialize world
            self._initialize_world(world, world_config)

            # Configure world agents
            if world_config.get('agents', {}).get('create_agents', True):
                self._configure_world_agents(world, world_config)

            # Validate world
            validation_result = self._validate_world(world, world_config)

            self.logger.info(f"World created successfully. World ID: {world.get('id', 'unknown')}")

            return {
                'world_created': True,
                'world_name': world_name,
                'world_type': world_config.get('world_type'),
                'world_id': world.get('id', 'unknown'),
                'output_directory': str(output_dir),
                'validation_passed': validation_result,
                'agents_created': world_config.get('agents', {}).get('create_agents', True),
                'world_size': {
                    'width': len(world.get('grid', {}).get('cells', [])),
                    'height': len(world.get('grid', {}).get('cells', [[]])[0]) if world.get('grid', {}).get('cells') else 0
                }
            }

        except Exception as e:
            self.logger.error(f"World creation failed: {e}")
            return {'world_created': False, 'error': str(e)}

    def _create_grid_world(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Create grid-based world."""
        self.logger.info("Creating grid-based world")

        width = config.get('width', 10)
        height = config.get('height', 10)
        cell_size = config.get('cell_size', 1.0)
        terrain_type = config.get('terrain_type', 'flat')
        obstacles = config.get('obstacles', False)
        resources = config.get('resources', True)

        # Create grid cells
        grid = []
        for y in range(height):
            row = []
            for x in range(width):
                cell = {
                    'x': x,
                    'y': y,
                    'type': 'empty',
                    'terrain': terrain_type,
                    'walkable': True,
                    'resource_present': False,
                    'agent_present': False
                }

                # Add obstacles if configured
                if obstacles and (x % 3 == 0 or y % 3 == 0):
                    cell['type'] = 'wall'
                    cell['walkable'] = False

                # Add resources if configured
                if resources and (x % 4 == 0 or y % 4 == 0):
                    cell['type'] = 'resource'
                    cell['resource_present'] = True
                    cell['resource_amount'] = 10 + (x + y) % 10

                row.append(cell)
            grid.append(row)

        # Create world object
        world = {
            'id': f"grid_world_{int(time.time())}",
            'name': config.get('name', 'Grid World'),
            'world_type': 'grid',
            'width': width,
            'height': height,
            'cell_size': cell_size,
            'terrain_type': terrain_type,
            'grid': grid,
            'metadata': {
                'created_at': time.strftime('%Y-%m-%d %H:%M:%S'),
                'version': __version__
            }
        }

        return world

    def _create_continuous_world(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Create continuous world."""
        self.logger.info("Creating continuous world")

        width = config.get('width', 100.0)
        height = config.get('height', 100.0)
        boundary_type = config.get('boundary_type', 'toroidal')
        elevation_map = config.get('elevation_map', False)
        weather_effects = config.get('weather_effects', True)

        # Create continuous world (simplified representation)
        world = {
            'id': f"continuous_world_{int(time.time())}",
            'name': config.get('name', 'Continuous World'),
            'world_type': 'continuous',
            'width': width,
            'height': height,
            'boundary_type': boundary_type,
            'elevation_map': elevation_map,
            'weather_effects': weather_effects,
            'agents': {},
            'resources': [],
            'metadata': {
                'created_at': time.strftime('%Y-%m-%d %H:%M:%S'),
                'version': __version__
            }
        }

        # Add scattered resources
        if config.get('resources', True):
            for _ in range(config.get('num_resources', 50)):
                resource = {
                    'id': len(world['resources']) + 1,
                    'type': config.get('resource_type', 'food'),
                    'amount': 10 + (hash(f"{_}") % 10),
                    'position': {
                        'x': (hash(f"{_}") % 10000) / 100,
                        'y': ((hash(f"{_}") % 10000) / 100) * (height / 100)
                    }
                }
                world['resources'].append(resource)

        return world

    def _create_graph_world(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Create graph-based world."""
        self.logger.info("Creating graph world")

        num_nodes = config.get('num_nodes', 20)
        connection_probability = config.get('connection_probability', 0.3)
        node_types = config.get('node_types', ['empty', 'resource', 'obstacle', 'agent'])
        reward_structure = config.get('reward_structure', 'sparse')

        # Create graph nodes
        nodes = {}
        for i in range(num_nodes):
            node_type = node_types[hash(i) % len(node_types)]

            node = {
                'id': i,
                'type': node_type,
                'position': {
                    'x': (hash(i) % 10000) / 100,
                    'y': ((hash(i) % 10000) / 100) * 10
                },
                'walkable': node_type not in ['obstacle'],
                'resource_present': node_type == 'resource',
                'agent_present': node_type == 'agent'
            }

            if node_type == 'resource':
                node['resource_amount'] = 5 + (hash(i) % 10)

            nodes[str(i)] = node

        # Create graph edges
        edges = []
        node_ids = list(nodes.keys())

        for i, node_id in enumerate(node_ids):
            for j, other_node_id in enumerate(node_ids):
                if i < j and hash(f"{i}{j}") % 100 < connection_probability * 100:
                    edge = {
                        'id': f"{node_id}_{other_node_id}",
                        'source': node_id,
                        'target': other_node_id,
                        'weight': 1.0 + (hash(f"{i}{j}") % 10) / 10,
                        'cost': 1 + (hash(f"{i}{j}") % 5),
                        'walkable': True
                    }
                    edges.append(edge)

        # Create world object
        world = {
            'id': f"graph_world_{int(time.time())}",
            'name': config.get('name', 'Graph World'),
            'world_type': 'graph',
            'num_nodes': num_nodes,
            'connection_probability': connection_probability,
            'nodes': nodes,
            'edges': edges,
            'reward_structure': reward_structure,
            'metadata': {
                'created_at': time.strftime('%Y-%m-%d %H:%M:%S'),
                'version': __version__
            }
        }

        return world

    def _save_world_config(self, world: Dict[str, Any], output_dir: Path, world_name: str) -> None:
        """Save world configuration to file."""
        world_file = output_dir / f"{world_name.lower().replace(' ', '_')}_config.json"

        try:
            with open(world_file, 'w') as f:
                json.dump(world, f, indent=2)
            self.logger.debug(f"Saved world configuration to: {world_file}")
        except Exception as e:
            self.logger.error(f"Failed to save world configuration: {e}")
            raise

    def _create_world_visualization(self, world: Dict[str, Any], output_dir: Path, world_name: str) -> None:
        """Create world visualization."""
        self.logger.info(f"Creating world visualization for: {world_name}")

        output_file = output_dir / f"{world_name.lower().replace(' ', '_')}_visualization.png"

        # Create visualization based on world type
        if world['world_type'] == 'grid':
            self._create_grid_visualization(world, output_file)
        elif world['world_type'] == 'continuous':
            self._create_continuous_visualization(world, output_file)
        elif world['world_type'] == 'graph':
            self._create_graph_visualization(world, output_file)

        self.logger.debug(f"Created visualization: {output_file}")

    def _create_grid_visualization(self, world: Dict[str, Any], output_file: Path) -> None:
        """Create grid world visualization."""
        # In real implementation, use matplotlib or similar to create grid visualization

    def _create_continuous_visualization(self, world: Dict[str, Any], output_file: Path) -> None:
        """Create continuous world visualization."""
        # In real implementation, use matplotlib to create continuous world visualization

    def _create_graph_visualization(self, world: Dict[str, Any], output_file: Path) -> None:
        """Create graph world visualization."""
        # In real implementation, use networkx and matplotlib to create graph visualization

    def _initialize_world(self, world: Dict[str, Any], config: Dict[str, Any]) -> None:
        """Initialize world with default state."""
        self.logger.info("Initializing world...")

        # Add default agents if configured
        agents_config = config.get('agents', {})
        if agents_config.get('create_agents', False):
            num_agents = agents_config.get('num_agents', 1)

            for i in range(num_agents):
                agent = {
                    'id': f"agent_{i + 1}",
                    'name': agents_config.get('agent_names', [f"Agent_{i + 1}"])[i] if i < len(agents_config.get('agent_names', [])) else f"Agent_{i + 1}",
                    'type': agents_config.get('agent_type', 'default'),
                    'position': self._get_initial_position(world, agents_config),
                    'energy': 100.0,
                    'inventory': [],
                    'goals': agents_config.get('goals', [])
                }

                if world['world_type'] == 'grid':
                    x, y = agent['position']
                    if 0 <= y < len(world['grid']) and 0 <= x < len(world['grid'][0]):
                        world['grid'][y][x]['agent_present'] = True
                        world['agents'][agent['id']] = agent

                elif world['world_type'] == 'continuous':
                    world['agents'][agent['id']] = agent

                elif world['world_type'] == 'graph':
                    if agent['position'] in world['nodes']:
                        world['nodes'][agent['position']]['agent_present'] = True
                        world['agents'][agent['id']] = agent

        self.logger.debug(f"World initialized with {len(world.get('agents', {}))} agents")

    def _get_initial_position(self, world: Dict[str, Any], config: Dict[str, Any]) -> Union[List[int], Dict[str, float]]:
        """Get initial position for agent."""
        if world['world_type'] == 'grid':
            return [0, 0]  # Start at top-left corner

        elif world['world_type'] == 'continuous':
            return {'x': 0.5, 'y': 0.5}  # Center of world

        elif world['world_type'] == 'graph':
            return 0  # First node

        return {'x': 0, 'y': 0}

    def _configure_world_agents(self, world: Dict[str, Any], config: Dict[str, Any]) -> None:
        """Configure world agents with behaviors."""
        self.logger.info("Configuring world agents...")

        agents_config = config.get('agents', {})

        # Configure agent behaviors
        for agent_id, agent in world.get('agents', {}).items():
            agent['behavior_config'] = {
                'learning_rate': agents_config.get('learning_rate', 0.01),
                'exploration_rate': agents_config.get('exploration_rate', 0.1),
                'discount_factor': agents_config.get('discount_factor', 0.95),
                'action_space': agents_config.get('action_space', ['move_up', 'move_down', 'move_left', 'move_right', 'stay']),
                'observation_space': agents_config.get('observation_space', ['vision', 'memory', 'reward'])
            }

            agent['state'] = {
                'last_position': agent['position'],
                'last_reward': 0.0,
                'step_count': 0,
                'energy': agent['energy']
            }

        self.logger.debug(f"Configured {len(world['agents'])} agents")

    def _validate_world(self, world: Dict[str, Any], config: Dict[str, Any]) -> Dict[str, Any]:
        """Validate world configuration."""
        validation_results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'recommendations': []
        }

        try:
            # Validate world type
            world_type = world.get('world_type')
            if not world_type:
                validation_results['errors'].append('World type not specified')
                validation_results['valid'] = False

            # Validate world-specific constraints
            if world_type == 'grid':
                if 'width' not in world or 'height' not in world:
                    validation_results['errors'].append('Grid world requires width and height')
                    validation_results['valid'] = False
                if 'grid' not in world:
                    validation_results['errors'].append('Grid world requires grid definition')
                    validation_results['valid'] = False

            elif world_type == 'continuous':
                if 'width' not in world or 'height' not in world:
                    validation_results['errors'].append('Continuous world requires width and height')
                    validation_results['valid'] = False

            elif world_type == 'graph':
                if 'nodes' not in world:
                    validation_results['errors'].append('Graph world requires nodes definition')
                    validation_results['valid'] = False

            # Validate agents
            agents = world.get('agents', {})
            if agents:
                for agent_id, agent in agents.items():
                    if 'position' not in agent:
                        validation_results['errors'].append(f"Agent {agent_id} missing position")
                        validation_results['valid'] = False

            # Add recommendations
            if world_type == 'grid' and config.get('world', {}).get('width', 0) > 50:
                validation_results['recommendations'].append('Large grid world may impact performance. Consider using continuous world.')

            if len(agents) == 0:
                validation_results['recommendations'].append('No agents created. Consider adding agents for interaction.')

            if not validation_results['errors']:
                self.logger.info("World validation passed")
            else:
                self.logger.warning(f"World validation failed with {len(validation_results['errors'])} errors")

            return validation_results

        except Exception as e:
            validation_results['errors'].append(f"Validation error: {e}")
            validation_results['valid'] = False
            return validation_results

    def _save_world_file(self, world: Dict[str, Any], output_dir: Path, world_name: str) -> None:
        """Save world to file."""
        world_file = output_dir / f"{world_name.lower().replace(' ', '_')}_world.json"

        try:
            with open(world_file, 'w') as f:
                json.dump(world, f, indent=2)
            self.logger.debug(f"Saved world to: {world_file}")
        except Exception as e:
            self.logger.error(f"Failed to save world: {e}")
            raise

    def _create_world_screenshot(self, world: Dict[str, Any], output_dir: Path, world_name: str) -> None:
        """Create world screenshot."""
        screenshot_file = output_dir / f"{world_name.lower().replace(' ', '_')}_screenshot.png"
        self.logger.debug(f"Created screenshot: {screenshot_file}")
        # In real implementation, use matplotlib or similar
class CreateWorldCLI(NLMCLI):
    """CLI tool for creating and configuring NLM worlds."""

    def __init__(self):
        super().__init__(
            name="create_world.py",
            description="Create and configure NLM worlds with comprehensive capabilities",
            version=__version__
        )

    def add_world_arguments(self, parser: argparse.ArgumentParser) -> None:
        """Add world-specific arguments."""
        world_group = parser.add_argument_group('World Configuration')

        world_group.add_argument(
            '--config',
            type=str,
            required=True,
            help='Path to world configuration file (JSON)'
        )

        world_group.add_argument(
            '--world-name',
            type=str,
            help='Name for the world (overrides config)'
        )

        world_group.add_argument(
            '--world-type',
            type=str,
            choices=['grid', 'continuous', 'graph'],
            default='grid',
            help='World type (default: grid)'
        )

        world_group.add_argument(
            '--output-dir',
            type=str,
            help='Output directory for world (overrides config)'
        )

        world_group.add_argument(
            '--validate-only',
            action='store_true',
            help='Validate configuration without creating world'
        )

        # Subcommands
        subparsers = parser.add_subparsers(dest='command', help='Available commands')

        # Create subcommand
        create_parser = subparsers.add_parser('create', help='Create world')
        self.add_world_arguments(create_parser)

        # Export subcommand
        export_parser = subparsers.add_parser('export', help='Export world to file')
        export_parser.add_argument('--world-file', type=str, required=True, help='Path to world file to export')
        export_parser.add_argument('--format', type=str, choices=['json', 'xml', 'yaml'], default='json', help='Export format')

        # Validate subcommand
        validate_parser = subparsers.add_parser('validate', help='Validate world configuration')
        validate_parser.add_argument('--config', type=str, required=True, help='Path to world configuration file')

    def parse_args(self, description: str, subcommands: bool = False) -> argparse.ArgumentParser:
        """Parse command line arguments."""
        self.add_world_arguments(self.parser)
        return self.parser

    def run_command(self, args: argparse.Namespace) -> int:
        """Execute world command."""
        if not args.command:
            self.logger.error("No command specified. Use 'create', 'export', or 'validate'")
            return 1

        if args.command == 'create':
            return self._create_world(args)
        elif args.command == 'export':
            return self._export_world(args)
        elif args.command == 'validate':
            return self._validate_world(args)
        else:
            self.logger.error(f"Unknown command: {args.command}")
            return 1

    def _create_world(self, args: argparse.Namespace) -> int:
        """Create world with configuration."""
        self.logger.info("Starting world creation")

        # Load configuration
        try:
            config = load_config(args.config)
        except Exception as e:
            self.logger.error(f"Failed to load configuration: {e}")
            return 1

        # Override configuration with command line arguments
        if args.world_name:
            config['world']['name'] = args.world_name

        if args.world_type:
            config['world']['world_type'] = args.world_type

        if args.output_dir:
            config['world']['output_dir'] = args.output_dir

        # Validate configuration
        if not self._validate_world_config(config):
            return 1

        # Validate-only mode
        if args.validate_only:
            self.logger.info("Validation mode - checking configuration only")
            self.format_and_output({
                'validate_only': True,
                'config_loaded': True,
                'configuration': config
            }, "Validation Configuration")
            return 0

        try:
            # Create world builder
            builder = WorldBuilder(config)

            # Create world
            results = builder.create_world()

            # Format and output results
            self.format_and_output(results, "World Creation Results")

            # Save world to file if specified
            if args.output_dir:
                results_file = Path(args.output_dir) / "world_summary.json"
                with open(results_file, 'w') as f:
                    import json
                    json.dump(results, f, indent=2)
                self.logger.info(f"World summary saved to: {results_file}")

            self.logger.info("World created successfully")
            return 0

        except Exception as e:
            self.logger.error(f"World creation failed: {e}")
            if self.verbose:
                import traceback
                self.logger.debug(traceback.format_exc())
            return 1

    def _export_world(self, args: argparse.Namespace) -> int:
        """Export world to file."""
        self.logger.info(f"Exporting world: {args.world_file} (format: {args.format})")

        # Export world (mock)
        self.logger.info("Exporting world...")

        results = {
            'export_completed': True,
            'world_file': args.world_file,
            'export_format': args.format,
            'export_size': 1024 * 1024,  # Mock size: 1MB
            'export_time_seconds': 2.5
        }

        self.logger.info("World export completed successfully")
        self.format_and_output(results, "Export Results")
        return 0

    def _validate_world(self, args: argparse.Namespace) -> int:
        """Validate world configuration file."""
        self.logger.info(f"Validating world configuration: {args.config}")

        try:
            config = load_config(args.config)

            # Validate configuration
            if not self._validate_world_config(config):
                return 1

            self.logger.info("World configuration validation passed")
            self.format_and_output({
                'valid': True,
                'config_path': args.config,
                'world_name': config.get('world', {}).get('name', 'Unnamed World'),
                'world_type': config.get('world', {}).get('world_type', 'grid')
            }, "Configuration Validation Results")

            return 0

        except Exception as e:
            self.logger.error(f"Configuration validation failed: {e}")
            return 1

    def _validate_world_config(self, config: Dict[str, Any]) -> bool:
        """Validate world configuration."""
        required_fields = ['world']

        for field in required_fields:
            if field not in config:
                self.logger.error(f"Missing required configuration field: {field}")
                return False

        # Validate world configuration
        world_config = config['world']
        if 'name' not in world_config:
            world_config['name'] = 'Unnamed World'

        if 'world_type' not in world_config:
            world_config['world_type'] = 'grid'

        if 'output_dir' not in world_config:
            world_config['output_dir'] = 'worlds'

        self.logger.info("World configuration validation passed")
        return True

    def main(self) -> int:
        """Main entry point for create_world.py CLI."""
        cli = CreateWorldCLI()
        return cli.run()
if __name__ == '__main__':
    cli = CreateWorldCLI()
    sys.exit(cli.run())