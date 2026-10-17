#!/usr/bin/env python3
"""
NLM CLI Tools - Common utilities and base classes for NLM CLI tools.

This module provides shared functionality for:
- Configuration file handling (JSON)
- Logging setup and configuration
- Progress reporting
- Error handling and validation
- Output formatting (console, JSON, CSV)
- Argument parsing utilities
"""

import argparse
import json
import logging
import sys
import time
from datetime import datetime
from enum import Enum
from pathlib import Path
from typing import Any, Dict, List, Optional, Union, TextIO
import csv

__version__ = "1.0.0"
__all__ = [
    'OutputFormat',
    'NLMCLI',
    'load_config',
    'save_config',
    'setup_logging',
    'ProgressReporter',
    'validate_args',
    'format_output',
]
class OutputFormat(Enum):
    """Output format enumeration."""
    CONSOLE = "console"
    JSON = "json"
    CSV = "csv"
    TABLE = "table"
class NLMCLI:
    """Base class for NLM CLI tools providing common functionality."""

    def __init__(self, name: str, description: str, version: str = __version__):
        self.name = name
        self.description = description
        self.version = version
        self.parser = None
        self.args = None
        self.logger = None
        self.output_format = OutputFormat.CONSOLE
        self.output_file = None
        self.verbose = False

    def add_common_arguments(self, parser: argparse.ArgumentParser) -> None:
        """Add common arguments to parser."""
        parser.add_argument(
            '--help',
            action='store_true',
            help='Show this help message and exit'
        )
        parser.add_argument(
            '--version',
            action='store_true',
            help='Show version information and exit'
        )
        parser.add_argument(
            '--verbose',
            action='store_true',
            help='Enable verbose output'
        )
        parser.add_argument(
            '--config',
            type=str,
            help='Path to JSON configuration file'
        )
        parser.add_argument(
            '--output-format',
            type=str,
            choices=[fmt.value for fmt in OutputFormat],
            default=OutputFormat.CONSOLE.value,
            help='Output format (console, json, csv, table)'
        )
        parser.add_argument(
            '--output-file',
            type=str,
            help='Output file path'
        )

    def parse_args(self, description: str, subcommands: bool = False) -> argparse.ArgumentParser:
        """Parse command line arguments."""
        parser = argparse.ArgumentParser(
            description=description,
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog=self._get_epilog()
        )

        self.add_common_arguments(parser)

        if subcommands:
            subparsers = parser.add_subparsers(dest='command', help='Available commands')
            return parser, subparsers

        return parser

    def _get_epilog(self) -> str:
        """Get epilog text for help message."""
        return f"""
Configuration File Support:
  - Use --config <file.json> to load configuration
  - Configuration can be overridden by command line arguments
  - Default configuration is provided

Output Formats:
  - console: Human-readable output to terminal
  - json: Structured JSON output
  - csv: Comma-separated values for data analysis
  - table: Formatted table output

Examples:
  nlm-train --help                          Show help information
  nlm-train --version                       Show version information
  nlm-train --config train_config.json       Load configuration from file
  nlm-train --output-format json             Output results in JSON format
  nlm-train --output-file results.csv        Save results to CSV file
"""

    def initialize(self, args: argparse.Namespace) -> None:
        """Initialize CLI tool with parsed arguments."""
        self.args = args
        self.verbose = args.verbose

        # Setup logging
        self.logger = setup_logging(
            level='DEBUG' if self.verbose else 'INFO',
            logger_name=self.name
        )

        # Setup output format
        self.output_format = OutputFormat(args.output_format)
        if args.output_file:
            self.output_file = Path(args.output_file)
            self.logger.debug(f"Output will be written to: {self.output_file}")

        # Load configuration if specified
        if args.config:
            config_path = Path(args.config)
            if config_path.exists():
                self.logger.info(f"Loading configuration from: {config_path}")
                load_config(str(config_path))
            else:
                self.logger.warning(f"Configuration file not found: {config_path}")

    def log_args(self, args: argparse.Namespace) -> None:
        """Log parsed arguments for debugging."""
        if self.verbose:
            self.logger.debug(f"Parsed arguments: {args}")

    def format_and_output(self, data: Any, title: Optional[str] = None) -> None:
        """Format and output data according to selected format."""
        formatted = format_output(data, self.output_format, title)

        if self.output_file:
            try:
                self.output_file.parent.mkdir(parents=True, exist_ok=True)
                with open(self.output_file, 'w') as f:
                    f.write(formatted)
                self.logger.info(f"Output written to: {self.output_file}")
            except Exception as e:
                self.logger.error(f"Failed to write output file: {e}")
                raise
        else:
            print(formatted)

    def run_command(self, args: argparse.Namespace) -> int:
        """Main command execution method - override in subclasses."""
        raise NotImplementedError("Subclasses must implement run_command")

    def run(self) -> int:
        """Main CLI entry point."""
        try:
            parser, subparsers = self.parse_args(self.description, subcommands=True)
            self.initialize(parser.parse_args())

            if self.args.version:
                self._print_version()
                return 0

            if self.args.help:
                parser.print_help()
                return 0

            self.log_args(self.args)
            return self.run_command(self.args)

        except KeyboardInterrupt:
            self.logger.info("Operation interrupted by user")
            return 130
        except Exception as e:
            self.logger.error(f"Error: {e}")
            if self.verbose:
                import traceback
                self.logger.debug(traceback.format_exc())
            return 1

    def _print_version(self) -> None:
        """Print version information."""
        print(f"{self.name} v{self.version}")
        print(f"Python bindings: pynlm (Phase 3 Advanced Features)")
        print(f"License: MIT")
class ProgressReporter:
    """Progress reporting utilities for long-running operations."""

    def __init__(self, description: str, total: int, logger: Optional[logging.Logger] = None):
        self.description = description
        self.total = total
        self.logger = logger or logging.getLogger(__name__)
        self.start_time = time.time()
        self.last_report_time = self.start_time
        self.current = 0

    def update(self, increment: int = 1) -> None:
        """Update progress."""
        self.current += increment
        elapsed = time.time() - self.start_time

        # Report every 5% or every 10 items, whichever comes first
        if (self.current % max(1, self.total // 20) == 0 or
                self.current % 10 == 0 or
                self.current == self.total):

            rate = self.current / elapsed if elapsed > 0 else 0
            eta = (self.total - self.current) / rate if rate > 0 else 0

            progress_pct = (self.current / self.total) * 100

            msg = f"Progress: {progress_pct:.1f}% ({self.current}/{self.total})"
            if eta > 0:
                msg += f" - ETA: {eta:.1f}s"

            self.logger.info(msg)

    def finish(self) -> None:
        """Report completion."""
        elapsed = time.time() - self.start_time
        self.logger.info(f"Completed {self.description} in {elapsed:.2f}s")
def load_config(config_path: str) -> Dict[str, Any]:
    """Load configuration from JSON file."""
    try:
        with open(config_path, 'r') as f:
            return json.load(f)
    except json.JSONDecodeError as e:
        raise ValueError(f"Invalid JSON in configuration file: {e}")
    except Exception as e:
        raise ValueError(f"Failed to load configuration file: {e}")
def save_config(config: Dict[str, Any], config_path: str) -> None:
    """Save configuration to JSON file."""
    try:
        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)
    except Exception as e:
        raise ValueError(f"Failed to save configuration file: {e}")
def setup_logging(level: str = 'INFO', logger_name: str = 'nlm_cli') -> logging.Logger:
    """Setup logging with appropriate formatting."""
    logger = logging.getLogger(logger_name)
    logger.setLevel(getattr(logging, level.upper()))

    # Clear existing handlers
    logger.handlers.clear()

    # Create console handler
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(getattr(logging, level.upper()))

    # Create formatter
    formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )
    console_handler.setFormatter(formatter)

    # Add handler to logger
    logger.addHandler(console_handler)

    return logger
def format_output(data: Any, format_type: OutputFormat, title: Optional[str] = None) -> str:
    """Format data according to specified format."""
    if format_type == OutputFormat.CONSOLE:
        return _format_console(data, title)
    elif format_type == OutputFormat.JSON:
        return _format_json(data, title)
    elif format_type == OutputFormat.CSV:
        return _format_csv(data, title)
    elif format_type == OutputFormat.TABLE:
        return _format_table(data, title)
    else:
        return str(data)
def _format_console(data: Any, title: Optional[str] = None) -> str:
    """Format data for console output."""
    lines = []
    if title:
        lines.append(title)
        lines.append("=" * len(title))
        lines.append("")

    if isinstance(data, dict):
        for key, value in data.items():
            lines.append(f"{key}: {value}")
    elif isinstance(data, list):
        for i, item in enumerate(data):
            lines.append(f"{i + 1}. {item}")
    else:
        lines.append(str(data))

    return "\n".join(lines)
def _format_json(data: Any, title: Optional[str] = None) -> str:
    """Format data for JSON output."""
    output = {}
    if title:
        output['title'] = title

    if isinstance(data, (dict, list)):
        output['data'] = data
    else:
        output['result'] = str(data)

    return json.dumps(output, indent=2)
def _format_csv(data: Any, title: Optional[str] = None) -> str:
    """Format data for CSV output."""
    import io

    output = io.StringIO()
    writer = csv.writer(output)

    if title:
        writer.writerow([title])
        writer.writerow([])

    if isinstance(data, dict):
        writer.writerow(['Key', 'Value'])
        for key, value in data.items():
            writer.writerow([key, str(value)])
    elif isinstance(data, list):
        if data and isinstance(data[0], dict):
            keys = data[0].keys()
            writer.writerow(keys)
            for row in data:
                writer.writerow([row.get(key, '') for key in keys])
        else:
            writer.writerow(['Item', 'Value'])
            for i, item in enumerate(data):
                writer.writerow([f"Item_{i + 1}", str(item)])
    else:
        writer.writerow(['Result', str(data)])

    return output.getvalue()
def _format_table(data: Any, title: Optional[str] = None) -> str:
    """Format data for table output."""
    import tabulate

    if not tabulate:
        return _format_console(data, title)

    if title:
        output = [title, "=" * len(title), ""]
    else:
        output = []

    if isinstance(data, dict):
        output.append(tabulate.tabulate([list(data.items())], headers=['Key', 'Value'], tablefmt='grid'))
    elif isinstance(data, list):
        if data and isinstance(data[0], dict):
            output.append(tabulate.tabulate(data, headers='keys', tablefmt='grid'))
        else:
            output.append(tabulate.tabulate([[item] for item in data], headers=['Value'], tablefmt='grid'))
    else:
        output.append(tabulate.tabulate([[str(data)]], headers=['Result'], tablefmt='grid'))

    return "\n".join(output)
def validate_args(args: argparse.Namespace, required_fields: List[str]) -> bool:
    """Validate parsed arguments."""
    errors = []

    for field in required_fields:
        if hasattr(args, field):
            value = getattr(args, field)
            if value is None or (isinstance(value, str) and not value.strip()):
                errors.append(f"Required argument --{field} is missing")

    if errors:
        print("Validation errors:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        return False

    return True

# Common configuration schemas
DEFAULT_TRAIN_CONFIG = {
    "training": {
        "epochs": 100,
        "learning_rate": 0.001,
        "batch_size": 32,
        "validation_split": 0.2,
        "early_stopping_patience": 10
    },
    "model": {
        "hidden_layers": [128, 64, 32],
        "activation": "relu",
        "dropout": 0.2,
        "optimizer": "adam"
    },
    "logging": {
        "log_interval": 10,
        "save_interval": 100,
        "visualization_interval": 500
    },
    "output": {
        "save_model": True,
        "export_formats": ["h5", "json"],
        "results_dir": "results"
    }
}

DEFAULT_SIMULATION_CONFIG = {
    "simulation": {
        "timestep": 0.01,
        "max_steps": 1000,
        "time_limit": 0.0,
        "headless": False,
        "seed": 42
    },
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "plasticity_enabled": True
    },
    "world": {
        "environment_type": "grid",
        "width": 10,
        "height": 10,
        "vision_range": 5
    }
}

DEFAULT_ANALYSIS_CONFIG = {
    "analysis": {
        "metrics": ["accuracy", "loss", "f1_score"],
        "statistical_tests": ["t_test", "anova"],
        "confidence_interval": 0.95,
        "cross_validation_folds": 5
    },
    "visualization": {
        "plot_types": ["line", "bar", "heatmap"],
        "export_formats": ["png", "pdf", "html"],
        "style": "seaborn"
    }
}