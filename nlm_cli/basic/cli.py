"""
Basic CLI interface for NLM.

This class provides the basic command-line interface for the NLM system,
supporting common operations like running simulations, loading configurations,
and checking system status.
"""

import argparse
import logging
import sys
from pathlib import Path
from typing import List

from utils.helpers import setup_logging, load_config

logger = logging.getLogger(__name__)

class BasicCLI:
    """
    Basic CLI interface for NLM.
    
    This class provides the basic command-line interface for the NLM system,
    supporting common operations like running simulations, loading configurations,
    and checking system status.
    """
    
    def __init__(self, config_path: str = "configs/default.cfg"):
        """
        Initialize the Basic CLI.
        
        Args:
            config_path: Path to configuration file
        """
        self.config_path = config_path
        self.setup_logging()
        
    def setup_logging(self):
        """Set up logging for the CLI."""
        setup_logging("INFO", "logs/nlm_basic_cli.log")
    
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
            if parsed_args.command == "run":
                return self.run_simulation_command(parsed_args)
            elif parsed_args.command == "load":
                return self.run_load_command(parsed_args)
            elif parsed_args.command == "config":
                return self.run_config_command(parsed_args)
            elif parsed_args.command == "help":
                parser.print_help()
                return 0
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
            description="NLM - Neural Learning Machine",
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
        
        subparsers = parser.add_subparsers(dest="command", help="Command")
        
        # Run simulation
        run_parser = subparsers.add_parser(
            "run",
            help="Run NLM simulation"
        )
        run_parser.add_argument(
            "steps",
            type=int,
            default=1000,
            help="Number of simulation steps (default: 1000)"
        )
        run_parser.add_argument(
            "--save",
            "-s",
            help="Save checkpoint after completion",
            action="store_true"
        )
        
        # Load configuration
        load_parser = subparsers.add_parser(
            "load",
            help="Load configuration from file"
        )
        load_parser.add_argument("config_file", help="Configuration file path")
        
        # Configuration management
        config_parser = subparsers.add_parser(
            "config",
            help="Configuration management"
        )
        config_parser.add_argument(
            "action",
            choices=["show", "save", "load"],
            help="Configuration action"
        )
        config_parser.add_argument(
            "key",
            nargs="?",
            help="Configuration key"
        )
        config_parser.add_argument(
            "value",
            nargs="?",
            help="Configuration value"
        )
        
        # Help
        subparsers.add_parser("help", help="Show help information")
        
        return parser
    
    def run_simulation_command(self, parsed_args):
        """Run simulation command."""
        try:
            # Load configuration
            config = load_config(self.config_path)
            
            # Create and run simulation
            logger.info(f"Starting simulation with {parsed_args.steps} steps")
            
            # For now, just log the simulation start
            # In a real implementation, this would start the actual NLM simulation
            print(f"NLM Simulation: {parsed_args.steps} steps")
            print(f"Configuration: {self.config_path}")
            print(f"Save checkpoint: {parsed_args.save}")
            
            if parsed_args.save:
                logger.info("Checkpoint will be saved after completion")
            
            logger.info("Simulation completed successfully")
            return 0
            
        except Exception as e:
            logger.error(f"Simulation failed: {e}")
            return 1
    
    def run_load_command(self, parsed_args):
        """Load configuration command."""
        try:
            # Load and display configuration
            config = load_config(parsed_args.config_file)
            
            if config:
                print(f"Loaded configuration from {parsed_args.config_file}")
                print(f"Number of parameters: {len(config)}")
                
                for key, value in config.items():
                    print(f"  {key}: {value} (type: {type(value).__name__})")
                
                return 0
            else:
                print(f"Error: Could not load configuration from {parsed_args.config_file}")
                return 1
                
        except Exception as e:
            logger.error(f"Load configuration failed: {e}")
            return 1
    
    def run_config_command(self, parsed_args):
        """Configuration command."""
        try:
            if parsed_args.action == "show":
                config = load_config(self.config_path)
                print("Current configuration:")
                for key, value in config.items():
                    print(f"  {key}: {value}")
                return 0
                
            elif parsed_args.action == "save":
                # Save configuration
                # In a real implementation, this would save the current state
                print(f"Configuration would be saved to {self.config_path}")
                return 0
                
            elif parsed_args.action == "load":
                # Load configuration
                if parsed_args.key:
                    config = load_config(self.config_path)
                    if parsed_args.key in config:
                        print(f"{parsed_args.key} = {config[parsed_args.key]}")
                        return 0
                    else:
                        print(f"Error: Key '{parsed_args.key}' not found in configuration")
                        return 1
                else:
                    print("Error: Key required for load action")
                    return 1
            else:
                print("Error: Unknown action")
                return 1
                
        except Exception as e:
            logger.error(f"Configuration command failed: {e}")
            return 1
if __name__ == "__main__":
    cli = BasicCLI()
    sys.exit(cli.run())
