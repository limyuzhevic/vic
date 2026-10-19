"""
Advanced CLI interface for NLM development users.

This class provides command-line interfaces for all advanced NLM commands
organized into functional categories.
"""

import argparse
import json
import logging
import subprocess
import sys
import time
from pathlib import Path
from typing import Dict, List, Any, Optional, Union
import yaml

from .api import AdvancedAPI
from ..utils.helpers import setup_logging, load_config, save_config

logger = logging.getLogger(__name__)

class AdvancedCLI:
    """
    Advanced CLI interface for NLM development users.
    
    This class provides comprehensive command-line interfaces for:
    1. Configuration Management Commands
    2. Checkpoint Management Commands
    3. Brain State Inspection Commands
    4. Development System Commands
    5. Memory System Commands
    6. Simulation Control Commands
    7. Debugging and Monitoring Commands
    8. World Management Commands
    """
    
    def __init__(self, config_path: Optional[str] = None):
        """
        Initialize the Advanced CLI.
        
        Args:
            config_path: Path to configuration file
        """
        self.config_path = config_path or "configs/default.cfg"
        self.api = AdvancedAPI(self.config_path)
        self.setup_logging()
        
    def setup_logging(self):
        """Set up logging for the CLI."""
        log_dir = Path("logs")
        log_dir.mkdir(exist_ok=True)
        
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler(log_dir / "nlm_advanced_cli.log"),
                logging.StreamHandler()
            ]
        )
    
    def run(self, args: List[str] = None) -> int:
        """
        Run the CLI with the given arguments.
        
        Args:
            args: Command line arguments
            
        Returns:
            Exit code (0 for success, non-zero for error)
        """
        parser = self.create_argument_parser()
        parsed_args = parser.parse_args(args)
        
        try:
            if parsed_args.command == "config":
                return self.run_config_command(parsed_args)
            elif parsed_args.command == "checkpoint":
                return self.run_checkpoint_command(parsed_args)
            elif parsed_args.command == "brain":
                return self.run_brain_command(parsed_args)
            elif parsed_args.command == "development":
                return self.run_development_command(parsed_args)
            elif parsed_args.command == "memory":
                return self.run_memory_command(parsed_args)
            elif parsed_args.command == "simulation":
                return self.run_simulation_command(parsed_args)
            elif parsed_args.command == "debug":
                return self.run_debug_command(parsed_args)
            elif parsed_args.command == "world":
                return self.run_world_command(parsed_args)
            elif parsed_args.command == "run":
                return self.run_generic_command(parsed_args)
            else:
                parser.print_help()
                return 1
                
        except KeyboardInterrupt:
            logger.info("Operation cancelled by user")
            return 130
        except Exception as e:
            logger.error(f"Error: {e}")
            return 1
    
    def create_argument_parser(self) -> argparse.ArgumentParser:
        """Create the argument parser for the CLI."""
        parser = argparse.ArgumentParser(
            description="Advanced NLM CLI for Development Users",
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        
        parser.add_argument(
            "--config",
            "-c",
            help="Path to configuration file",
            default=self.config_path
        )
        
        parser.add_argument(
            "--log-level",
            "-l",
            choices=["DEBUG", "INFO", "WARNING", "ERROR"],
            help="Log level",
            default="INFO"
        )
        
        parser.add_argument(
            "--verbose",
            "-v",
            action="store_true",
            help="Enable verbose output"
        )
        
        subparsers = parser.add_subparsers(dest="command", help="Command category")
        
        # Configuration commands
        self.add_config_commands(subparsers)
        
        # Checkpoint commands
        self.add_checkpoint_commands(subparsers)
        
        # Brain state commands
        self.add_brain_commands(subparsers)
        
        # Development commands
        self.add_development_commands(subparsers)
        
        # Memory commands
        self.add_memory_commands(subparsers)
        
        # Simulation commands
        self.add_simulation_commands(subparsers)
        
        # Debug commands
        self.add_debug_commands(subparsers)
        
        # World commands
        self.add_world_commands(subparsers)
        
        # Generic run command
        run_parser = subparsers.add_parser(
            "run",
            help="Run a specific advanced command"
        )
        run_parser.add_argument("category", help="Command category")
        run_parser.add_argument("command", help="Command name")
        run_parser.add_argument("args", nargs="*", help="Command arguments")
        
        return parser
    
    def add_config_commands(self, subparsers):
        """Add configuration management commands."""
        config_parser = subparsers.add_parser(
            "config",
            help="Configuration management commands"
        )
        config_subparsers = config_parser.add_subparsers(
            dest="config_command",
            help="Configuration sub-command"
        )
        
        config_subparsers.add_parser("list", help="List all configuration parameters")
        
        get_parser = config_subparsers.add_parser("get", help="Get individual parameter")
        get_parser.add_argument("key", help="Parameter key")
        
        set_parser = config_subparsers.add_parser("set", help="Set individual parameter")
        set_parser.add_argument("key", help="Parameter key")
        set_parser.add_argument("value", help="Parameter value")
        
        load_parser = config_subparsers.add_parser("load", help="Load configuration from file")
        load_parser.add_argument("source", help="Source file path")
        load_parser.add_argument("--target", help="Target file path")
        
        save_parser = config_subparsers.add_parser("save", help="Save configuration to file")
        save_parser.add_argument("target", help="Target file path")
        save_parser.add_argument("--source", help="Source file path")
        
        config_subparsers.add_parser("validate", help="Validate configuration")
        
        template_parser = config_subparsers.add_parser("template", help="Create configuration template")
        template_parser.add_argument("path", help="Template file path")
    
    def add_checkpoint_commands(self, subparsers):
        """Add checkpoint management commands."""
        checkpoint_parser = subparsers.add_parser(
            "checkpoint",
            help="Checkpoint management commands"
        )
        checkpoint_subparsers = checkpoint_parser.add_subparsers(
            dest="checkpoint_command",
            help="Checkpoint sub-command"
        )
        
        checkpoint_subparsers.add_parser("list", help="List available checkpoints")
        
        create_parser = checkpoint_subparsers.add_parser("create", help="Create manual checkpoint")
        create_parser.add_argument("name", help="Checkpoint name")
        create_parser.add_argument("--config", help="Configuration file path")
        
        restore_parser = checkpoint_subparsers.add_parser("restore", help="Restore from checkpoint")
        restore_parser.add_argument("name", help="Checkpoint name")
        restore_parser.add_argument("--config", help="Configuration file path")
        
        delete_parser = checkpoint_subparsers.add_parser("delete", help="Delete checkpoint")
        delete_parser.add_argument("name", help="Checkpoint name")
        
        checkpoint_subparsers.add_parser("stats", help="Get checkpoint statistics")
    
    def add_brain_commands(self, subparsers):
        """Add brain state inspection commands."""
        brain_parser = subparsers.add_parser(
            "brain",
            help="Brain state inspection commands"
        )
        brain_subparsers = brain_parser.add_subparsers(
            dest="brain_command",
            help="Brain sub-command"
        )
        
        dump_parser = brain_subparsers.add_parser("dump", help="Detailed brain state dump")
        dump_parser.add_argument("--output", "-o", help="Output file path")
        
        brain_subparsers.add_parser("memory-stats", help="Memory system statistics")
        brain_subparsers.add_parser("neuromodulation", help="Neuromodulation system status")
        brain_subparsers.add_parser("development", help="Development system status")
        brain_subparsers.add_parser("prediction", help="Prediction system status")
    
    def add_development_commands(self, subparsers):
        """Add development system commands."""
        development_parser = subparsers.add_parser(
            "development",
            help="Development system commands"
        )
        development_subparsers = development_parser.add_subparsers(
            dest="development_command",
            help="Development sub-command"
        )
        
        development_subparsers.add_parser("advance", help="Advance development stage")
        
        plasticity_parser = development_subparsers.add_parser(
            "plasticity",
            help="Modify plasticity rates"
        )
        plasticity_parser.add_argument(
            "--rates",
            nargs="+",
            help="Plasticity rates (format: key=value)"
        )
        
        event_parser = development_subparsers.add_parser(
            "event",
            help="Trigger specific development event"
        )
        event_parser.add_argument("type", help="Event type")
        
        goals_parser = development_subparsers.add_parser(
            "goals",
            help="Set developmental goals"
        )
        goals_parser.add_argument("goals", nargs="+", help="Developmental goals")
    
    def add_memory_commands(self, subparsers):
        """Add memory system commands."""
        memory_parser = subparsers.add_parser(
            "memory",
            help="Memory system commands"
        )
        memory_subparsers = memory_parser.add_subparsers(
            dest="memory_command",
            help="Memory sub-command"
        )
        
        inspect_parser = memory_subparsers.add_parser(
            "inspect",
            help="Inspect memory contents"
        )
        inspect_parser.add_argument("type", help="Memory type (working, episodic)")
        
        search_parser = memory_subparsers.add_parser(
            "search",
            help="Search memories by criteria"
        )
        search_parser.add_argument(
            "--criteria",
            nargs="+",
            help="Search criteria (format: key=value)"
        )
        
        memory_subparsers.add_parser("consolidate", help="Consolidate memories")
        
        replay_parser = memory_subparsers.add_parser(
            "replay",
            help="Replay specific memory"
        )
        replay_parser.add_argument("id", help="Memory ID")
    
    def add_simulation_commands(self, subparsers):
        """Add simulation control commands."""
        simulation_parser = subparsers.add_parser(
            "simulation",
            help="Simulation control commands"
        )
        simulation_subparsers = simulation_parser.add_subparsers(
            dest="simulation_command",
            help="Simulation sub-command"
        )
        
        speed_parser = simulation_subparsers.add_parser(
            "speed",
            help="Control simulation speed"
        )
        speed_parser.add_argument("mode", help="Speed mode (slow, normal, fast)")
        speed_parser.add_argument(
            "--factor",
            type=float,
            help="Speed factor",
            default=1.0
        )
        
        simulation_subparsers.add_parser("pause", help="Pause simulation")
        simulation_subparsers.add_parser("resume", help="Resume simulation")
        
        jump_parser = simulation_subparsers.add_parser(
            "jump",
            help="Jump to specific step"
        )
        jump_parser.add_argument("step", type=int, help="Step number")
        
        record_parser = simulation_subparsers.add_parser(
            "record",
            help="Record simulation data"
        )
        record_parser.add_argument("output", help="Output file path")
        
        replay_parser = simulation_subparsers.add_parser(
            "replay",
            help="Replay recorded simulation"
        )
        replay_parser.add_argument("input", help="Input file path")
    
    def add_debug_commands(self, subparsers):
        """Add debugging and monitoring commands."""
        debug_parser = subparsers.add_parser(
            "debug",
            help="Debugging and monitoring commands"
        )
        debug_subparsers = debug_parser.add_subparsers(
            dest="debug_command",
            help="Debug sub-command"
        )
        
        debug_subparsers.add_parser("monitor", help="Real-time system monitoring")
        debug_subparsers.add_parser("profile", help="Performance profiling")
        debug_subparsers.add_parser("memory", help="Memory usage tracking")
        debug_subparsers.add_parser("validate", help="Checkpoint validation")
        debug_subparsers.add_parser("network", help="Network analysis")
    
    def add_world_commands(self, subparsers):
        """Add world management commands."""
        world_parser = subparsers.add_parser(
            "world",
            help="World management commands"
        )
        world_subparsers = world_parser.add_subparsers(
            dest="world_command",
            help="World sub-command"
        )
        
        add_parser = world_subparsers.add_parser(
            "add",
            help="Add world object"
        )
        add_parser.add_argument("type", help="Object type")
        add_parser.add_argument("x", type=float, help="X coordinate")
        add_parser.add_argument("y", type=float, help="Y coordinate")
        add_parser.add_argument(
            "--properties",
            nargs="+",
            help="Object properties (format: key=value)"
        )
        
        modify_parser = world_subparsers.add_parser(
            "modify",
            help="Modify world parameters"
        )
        modify_parser.add_argument(
            "--parameters",
            nargs="+",
            help="World parameters (format: key=value)"
        )
        
        start_parser = world_subparsers.add_parser(
            "start",
            help="Set agent start position"
        )
        start_parser.add_argument("x", type=float, help="X coordinate")
        start_parser.add_argument("y", type=float, help="Y coordinate")
        
        world_subparsers.add_parser("validate", help="World validation")
    
    def run_config_command(self, parsed_args):
        """Run configuration management commands."""
        if parsed_args.config_command == "list":
            print(self.api.list_config_params())
            return 0
        elif parsed_args.config_command == "get":
            print(self.api.get_config_param(parsed_args.key))
            return 0
        elif parsed_args.config_command == "set":
            print(self.api.set_config_param(parsed_args.key, parsed_args.value))
            return 0
        elif parsed_args.config_command == "load":
            print(self.api.load_config_file(parsed_args.source, parsed_args.target))
            return 0
        elif parsed_args.config_command == "save":
            print(self.api.save_config_file(parsed_args.target, parsed_args.source))
            return 0
        elif parsed_args.config_command == "validate":
            print(self.api.validate_config())
            return 0
        elif parsed_args.config_command == "template":
            print(self.api.create_config_template(parsed_args.path))
            return 0
        else:
            print("Error: Unknown config command")
            return 1
    
    def run_checkpoint_command(self, parsed_args):
        """Run checkpoint management commands."""
        if parsed_args.checkpoint_command == "list":
            print(self.api.list_checkpoints())
            return 0
        elif parsed_args.checkpoint_command == "create":
            print(self.api.create_checkpoint(parsed_args.name, parsed_args.config))
            return 0
        elif parsed_args.checkpoint_command == "restore":
            print(self.api.restore_checkpoint(parsed_args.name, parsed_args.config))
            return 0
        elif parsed_args.checkpoint_command == "delete":
            print(self.api.delete_checkpoint(parsed_args.name))
            return 0
        elif parsed_args.checkpoint_command == "stats":
            print(self.api.checkpoint_statistics())
            return 0
        else:
            print("Error: Unknown checkpoint command")
            return 1
    
    def run_brain_command(self, parsed_args):
        """Run brain state inspection commands."""
        if parsed_args.brain_command == "dump":
            print(self.api.brain_state_dump(parsed_args.output))
            return 0
        elif parsed_args.brain_command == "memory-stats":
            print(self.api.memory_system_statistics())
            return 0
        elif parsed_args.brain_command == "neuromodulation":
            print(self.api.neuromodulation_system_status())
            return 0
        elif parsed_args.brain_command == "development":
            print(self.api.development_system_status())
            return 0
        elif parsed_args.brain_command == "prediction":
            print(self.api.prediction_system_status())
            return 0
        else:
            print("Error: Unknown brain command")
            return 1
    
    def run_development_command(self, parsed_args):
        """Run development system commands."""
        if parsed_args.development_command == "advance":
            print(self.api.advance_development_stage())
            return 0
        elif parsed_args.development_command == "plasticity":
            rates = {}
            if parsed_args.rates:
                for rate in parsed_args.rates:
                    if "=" in rate:
                        k, v = rate.split("=", 1)
                        try:
                            rates[k] = float(v)
                        except ValueError:
                            rates[k] = v
            print(self.api.modify_plasticity_rates(rates))
            return 0
        elif parsed_args.development_command == "event":
            print(self.api.trigger_development_event(parsed_args.type))
            return 0
        elif parsed_args.development_command == "goals":
            print(self.api.set_developmental_goals(parsed_args.goals))
            return 0
        else:
            print("Error: Unknown development command")
            return 1
    
    def run_memory_command(self, parsed_args):
        """Run memory system commands."""
        if parsed_args.memory_command == "inspect":
            print(self.api.inspect_memory_contents(parsed_args.type))
            return 0
        elif parsed_args.memory_command == "search":
            criteria = {}
            if parsed_args.criteria:
                for criterion in parsed_args.criteria:
                    if "=" in criterion:
                        k, v = criterion.split("=", 1)
                        try:
                            criteria[k] = float(v) if "." in v else int(v) if v.isdigit() else v
                        except ValueError:
                            criteria[k] = v
            print(self.api.search_memories(criteria))
            return 0
        elif parsed_args.memory_command == "consolidate":
            print(self.api.consolidate_memories())
            return 0
        elif parsed_args.memory_command == "replay":
            print(self.api.replay_memory(parsed_args.id))
            return 0
        else:
            print("Error: Unknown memory command")
            return 1
    
    def run_simulation_command(self, parsed_args):
        """Run simulation control commands."""
        if parsed_args.simulation_command == "speed":
            print(self.api.control_simulation_speed(parsed_args.mode, parsed_args.factor))
            return 0
        elif parsed_args.simulation_command == "pause":
            print(self.api.pause_simulation())
            return 0
        elif parsed_args.simulation_command == "resume":
            print(self.api.resume_simulation())
            return 0
        elif parsed_args.simulation_command == "jump":
            print(self.api.jump_to_step(parsed_args.step))
            return 0
        elif parsed_args.simulation_command == "record":
            print(self.api.record_simulation(parsed_args.output))
            return 0
        elif parsed_args.simulation_command == "replay":
            print(self.api.replay_simulation(parsed_args.input))
            return 0
        else:
            print("Error: Unknown simulation command")
            return 1
    
    def run_debug_command(self, parsed_args):
        """Run debugging and monitoring commands."""
        if parsed_args.debug_command == "monitor":
            print(self.api.real_time_monitoring())
            return 0
        elif parsed_args.debug_command == "profile":
            print(self.api.performance_profiling())
            return 0
        elif parsed_args.debug_command == "memory":
            print(self.api.memory_usage_tracking())
            return 0
        elif parsed_args.debug_command == "validate":
            print(self.api.checkpoint_validation())
            return 0
        elif parsed_args.debug_command == "network":
            print(self.api.network_analysis())
            return 0
        else:
            print("Error: Unknown debug command")
            return 1
    
    def run_world_command(self, parsed_args):
        """Run world management commands."""
        if parsed_args.world_command == "add":
            properties = {}
            if parsed_args.properties:
                for prop in parsed_args.properties:
                    if "=" in prop:
                        k, v = prop.split("=", 1)
                        try:
                            properties[k] = float(v) if "." in v else int(v) if v.isdigit() else v
                        except ValueError:
                            properties[k] = v
            print(self.api.add_world_object(parsed_args.type, parsed_args.x, parsed_args.y, properties))
            return 0
        elif parsed_args.world_command == "modify":
            parameters = {}
            if parsed_args.parameters:
                for param in parsed_args.parameters:
                    if "=" in param:
                        k, v = param.split("=", 1)
                        try:
                            parameters[k] = float(v) if "." in v else int(v) if v.isdigit() else v
                        except ValueError:
                            parameters[k] = v
            print(self.api.modify_world_parameters(parameters))
            return 0
        elif parsed_args.world_command == "start":
            print(self.api.set_agent_start_position(parsed_args.x, parsed_args.y))
            return 0
        elif parsed_args.world_command == "validate":
            print(self.api.world_validation())
            return 0
        else:
            print("Error: Unknown world command")
            return 1
    
    def run_generic_command(self, parsed_args):
        """Run a generic advanced command."""
        print(self.api.run_advanced_command(
            parsed_args.category,
            parsed_args.command,
            parsed_args.args
        ))
        return 0
if __name__ == "__main__":
    cli = AdvancedCLI()
    sys.exit(cli.run())
