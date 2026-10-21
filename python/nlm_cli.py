#!/usr/bin/env python3
"""
NLM Python Bindings for Command-Line Integration

This module provides Python bindings for the NLM command-line tools,
allowing seamless integration of NLM functionality in Python scripts
and applications.
"""

import subprocess
import json
import sys
import os
import time
from typing import Dict, List, Any, Optional, Union, Callable
from dataclasses import dataclass, asdict
import argparse
import threading

@dataclass
class NLMConfig:
    """Configuration for NLM Python bindings"""
    nlm_path: str = "./nlm"
    tools_dir: str = "tools"
    timeout: Optional[float] = None
    verbose: bool = False

class NLMCommandError(Exception):
    """Exception raised for NLM command errors"""
    pass

class NLMCommandResult:
    """Result from executing an NLM command"""
    def __init__(self, success: bool, stdout: str, stderr: str, exit_code: int, execution_time: float):
        self.success = success
        self.stdout = stdout
        self.stderr = stderr
        self.exit_code = exit_code
        self.execution_time = execution_time
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'success': self.success,
            'stdout': self.stdout,
            'stderr': self.stderr,
            'exit_code': self.exit_code,
            'execution_time': self.execution_time
        }

class NLMCommand:
    """Base class for NLM command wrappers"""
    
    def __init__(self, config: NLMConfig):
        self.config = config
        self.command_name = ""
    
    def execute(self, args: List[str] = None, **kwargs) -> NLMCommandResult:
        """Execute the command with given arguments"""
        cmd_args = [self.config.nlm_path, self.command_name]
        
        if args:
            cmd_args.extend(args)
        
        # Add keyword arguments
        for key, value in kwargs.items():
            if isinstance(value, bool) and value:
                cmd_args.append(f"--{key}")
            else:
                cmd_args.append(f"--{key}={value}")
        
        try:
            start_time = time.time()
            result = subprocess.run(
                cmd_args,
                capture_output=True,
                text=True,
                timeout=self.config.timeout
            )
            execution_time = time.time() - start_time
            
            if self.config.verbose:
                print(f"Executing: {' '.join(cmd_args)}")
            
            return NLMCommandResult(
                success=result.returncode == 0,
                stdout=result.stdout,
                stderr=result.stderr,
                exit_code=result.returncode,
                execution_time=execution_time
            )
        except subprocess.TimeoutExpired as e:
            return NLMCommandResult(
                success=False,
                stdout="",
                stderr=f"Command timed out after {self.config.timeout} seconds",
                exit_code=-1,
                execution_time=self.config.timeout or 0.0
            )
        except Exception as e:
            return NLMCommandResult(
                success=False,
                stdout="",
                stderr=str(e),
                exit_code=-1,
                execution_time=0.0
            )

class NLMConfigTool(NLMCommand):
    """Wrapper for nlm-config command"""
    
    def __init__(self, config: NLMConfig):
        super().__init__(config)
        self.command_name = "nlm-config"
    
    def create_config(self, filename: str, template: str = None) -> NLMCommandResult:
        """Create a new configuration file"""
        args = ["--create", filename]
        if template:
            args.extend(["--template", template])
        return self.execute(args)
    
    def validate_config(self, filename: str) -> NLMCommandResult:
        """Validate an existing configuration file"""
        return self.execute(["--validate", filename])
    
    def list_templates(self) -> NLMCommandResult:
        """List available configuration templates"""
        return self.execute(["--list-templates"])
    
    def set_config_value(self, key: str, value: str) -> NLMCommandResult:
        """Set a configuration value"""
        return self.execute([f"--set={key}={value}"])
    
    def remove_config_key(self, key: str) -> NLMCommandResult:
        """Remove a configuration key"""
        return self.execute(["--remove", key])

class NLMValidateTool(NLMCommand):
    """Wrapper for nlm-validate command"""
    
    def __init__(self, config: NLMConfig):
        super().__init__(config)
        self.command_name = "nlm-validate"
    
    def validate(self, config_file: str = None, brain_tests: bool = False) -> NLMCommandResult:
        """Validate configuration and optionally run brain tests"""
        args = []
        if config_file:
            args.extend(["--config", config_file])
        if brain_tests:
            args.append("--brain-tests")
        return self.execute(args)
    
    def validate_all(self) -> NLMCommandResult:
        """Run all validation tests"""
        return self.execute(["--all"])

class NLMInfoTool(NLMCommand):
    """Wrapper for nlm-info command"""
    
    def __init__(self, config: NLMConfig):
        super().__init__(config)
        self.command_name = "nlm-info"
    
    def get_info(self, config_file: str = None) -> NLMCommandResult:
        """Get system information"""
        args = []
        if config_file:
            args.extend(["--config", config_file])
        return self.execute(args)
    
    def check_installation(self) -> NLMCommandResult:
        """Check if NLM is properly installed"""
        return self.execute(["--validate"])

class NLMBenchmarkTool(NLMCommand):
    """Wrapper for nlm-benchmark command"""
    
    def __init__(self, config: NLMConfig):
        super().__init__(config)
        self.command_name = "nlm-benchmark"
    
    def run_quick_benchmark(self) -> NLMCommandResult:
        """Run quick benchmark"""
        return self.execute(["--quick"])
    
    def run_detailed_benchmark(self, neurons: int = None, steps: int = None) -> NLMCommandResult:
        """Run detailed benchmark"""
        args = ["--detailed"]
        if neurons:
            args.extend(["--neurons", str(neurons)])
        if steps:
            args.extend(["--steps", str(steps)])
        return self.execute(args)

class NLMCheckpointTool(NLMCommand):
    """Wrapper for checkpoint commands"""
    
    def __init__(self, config: NLMConfig):
        super().__init__(config)
        self.command_name = "nlm-save"
    
    def save_checkpoint(self, description: str = None, filename: str = None) -> NLMCommandResult:
        """Save a checkpoint"""
        args = []
        if description:
            args.extend(["--save", description])
        if filename:
            args.extend(["--saveas", description, filename] if description else ["--save", filename])
        return self.execute(args)

class NLMImportTool(NLMCommand):
    """Wrapper for nlm-load command"""
    
    def __init__(self, config: NLMConfig):
        super().__init__(config)
        self.command_name = "nlm-load"
    
    def load_checkpoint(self, filename: str) -> NLMCommandResult:
        """Load a checkpoint"""
        return self.execute(["--load", filename])

class NLMTool:
    """Main NLM tool manager for Python"""
    
    def __init__(self, config_file: str = None):
        self.config = NLMConfig()
        
        if config_file and os.path.exists(config_file):
            self.load_config(config_file)
        
        # Initialize tool wrappers
        self.config_tool = NLMConfigTool(self.config)
        self.validate_tool = NLMValidateTool(self.config)
        self.info_tool = NLMInfoTool(self.config)
        self.benchmark_tool = NLMBenchmarkTool(self.config)
        self.checkpoint_tool = NLMCheckpointTool(self.config)
        self.import_tool = NLMImportTool(self.config)
    
    def load_config(self, config_file: str):
        """Load NLM configuration from file"""
        try:
            with open(config_file, 'r') as f:
                config_data = json.load(f)
            
            if 'nlm_path' in config_data:
                self.config.nlm_path = config_data['nlm_path']
            if 'tools_dir' in config_data:
                self.config.tools_dir = config_data['tools_dir']
            if 'timeout' in config_data:
                self.config.timeout = config_data['timeout']
            if 'verbose' in config_data:
                self.config.verbose = config_data['verbose']
                
        except Exception as e:
            raise NLMCommandError(f"Failed to load config file: {e}")
    
    def save_config(self, config_file: str):
        """Save NLM configuration to file"""
        config_data = {
            'nlm_path': self.config.nlm_path,
            'tools_dir': self.config.tools_dir,
            'timeout': self.config.timeout,
            'verbose': self.config.verbose
        }
        
        with open(config_file, 'w') as f:
            json.dump(config_data, f, indent=2)
    
    def run_config_tool(self, action: str, filename: str = None, **kwargs) -> Dict[str, Any]:
        """Run config tool with given action"""
        if action == "create":
            result = self.config_tool.create_config(filename, kwargs.get('template'))
        elif action == "validate":
            result = self.config_tool.validate_config(filename)
        elif action == "list_templates":
            result = self.config_tool.list_templates()
        elif action == "set":
            result = self.config_tool.set_config_value(kwargs.get('key'), kwargs.get('value'))
        elif action == "remove":
            result = self.config_tool.remove_config_key(filename)
        else:
            raise NLMCommandError(f"Unknown config action: {action}")
        
        return result.to_dict()
    
    def run_validate_tool(self, config_file: str = None, brain_tests: bool = False) -> Dict[str, Any]:
        """Run validation tool"""
        result = self.validate_tool.validate(config_file, brain_tests)
        return result.to_dict()
    
    def run_info_tool(self, config_file: str = None) -> Dict[str, Any]:
        """Run info tool"""
        result = self.info_tool.get_info(config_file)
        return result.to_dict()
    
    def run_benchmark_tool(self, detailed: bool = False, **kwargs) -> Dict[str, Any]:
        """Run benchmark tool"""
        result = self.benchmark_tool.run_detailed_benchmark(
            neurons=kwargs.get('neurons'),
            steps=kwargs.get('steps')
        )
        return result.to_dict()
    
    def run_checkpoint_tool(self, description: str = None, filename: str = None) -> Dict[str, Any]:
        """Run checkpoint tool"""
        result = self.checkpoint_tool.save_checkpoint(description, filename)
        return result.to_dict()
    
    def run_import_tool(self, filename: str) -> Dict[str, Any]:
        """Run import tool"""
        result = self.import_tool.load_checkpoint(filename)
        return result.to_dict()
    
    def quick_run(self, action: str, **kwargs) -> Dict[str, Any]:
        """Quick run of common actions"""
        if action == "create_config":
            return self.run_config_tool("create", kwargs.get('filename'), template=kwargs.get('template'))
        elif action == "validate_config":
            return self.run_validate_tool(kwargs.get('config_file'), kwargs.get('brain_tests', False))
        elif action == "get_info":
            return self.run_info_tool(kwargs.get('config_file'))
        elif action == "run_benchmark":
            return self.run_benchmark_tool(kwargs.get('detailed', False), **kwargs)
        elif action == "save_checkpoint":
            return self.run_checkpoint_tool(kwargs.get('description'), kwargs.get('filename'))
        elif action == "load_checkpoint":
            return self.run_import_tool(kwargs.get('filename'))
        else:
            raise NLMCommandError(f"Unknown quick action: {action}")
    
    def get_tools_info(self) -> Dict[str, Any]:
        """Get information about available NLM tools"""
        info = {
            'nlm_version': '0.1.0',
            'tools': [
                {
                    'name': 'nlm-config',
                    'description': 'Configuration file creation and validation',
                    'category': 'basic'
                },
                {
                    'name': 'nlm-validate',
                    'description': 'Validate configuration files and test brain functionality',
                    'category': 'basic'
                },
                {
                    'name': 'nlm-info',
                    'description': 'Display system information and capabilities',
                    'category': 'basic'
                },
                {
                    'name': 'nlm-benchmark',
                    'description': 'Performance benchmarking suite',
                    'category': 'basic'
                },
                {
                    'name': 'nlm-save',
                    'description': 'Save simulation checkpoints',
                    'category': 'basic'
                },
                {
                    'name': 'nlm-load',
                    'description': 'Load simulation checkpoints',
                    'category': 'basic'
                }
            ]
        }
        return info

# Example usage functions
def example_usage():
    """Demonstrate example usage of NLM Python bindings"""
    print("NLM Python Bindings - Example Usage")
    print("=" * 40)
    
    # Initialize NLM
    nlm = NLMTool()
    
    print("\n1. Create a configuration template:")
    result = nlm.quick_run("create_config", filename="example_config.cfg")
    print(f"   Success: {result['success']}")
    
    print("\n2. Validate configuration:")
    result = nlm.quick_run("validate_config", config_file="example_config.cfg")
    print(f"   Success: {result['success']}")
    
    print("\n3. Get system information:")
    result = nlm.quick_run("get_info")
    print(f"   Success: {result['success']}")
    
    print("\n4. Run quick benchmark:")
    result = nlm.quick_run("run_benchmark")
    print(f"   Success: {result['success']}")
    
    print("\n5. Save checkpoint:")
    result = nlm.quick_run("save_checkpoint", description="Example checkpoint")
    print(f"   Success: {result['success']}")
    
    print("\nExample completed successfully!")

# CLI interface for NLM Python bindings
def main():
    """Main CLI interface for NLM Python bindings"""
    parser = argparse.ArgumentParser(description="NLM Python Bindings for Command-Line Integration")
    parser.add_argument("--config", help="Path to NLM configuration file")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    parser.add_argument("--version", action="store_true", help="Show version information")
    parser.add_argument("--example", action="store_true", help="Run example usage")
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Configuration tools")
    config_subparsers = config_parser.add_subparsers(dest="config_action", help="Config actions")
    
    config_subparsers.add_parser("create", help="Create configuration file")
    config_subparsers.add_parser("validate", help="Validate configuration file")
    config_subparsers.add_parser("list", help="List templates")
    
    # Validate command
    validate_parser = subparsers.add_parser("validate", help="Validation tools")
    validate_parser.add_argument("--config-file", help="Configuration file")
    validate_parser.add_argument("--brain-tests", action="store_true", help="Run brain tests")
    
    # Info command
    info_parser = subparsers.add_parser("info", help="Information tools")
    info_parser.add_argument("--config-file", help="Configuration file")
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser("benchmark", help="Benchmark tools")
    benchmark_parser.add_argument("--detailed", action="store_true", help="Run detailed benchmark")
    benchmark_parser.add_argument("--neurons", type=int, help="Number of neurons")
    benchmark_parser.add_argument("--steps", type=int, help="Number of simulation steps")
    
    # Checkpoint commands
    checkpoint_parser = subparsers.add_parser("checkpoint", help="Checkpoint tools")
    checkpoint_subparsers = checkpoint_parser.add_subparsers(dest="checkpoint_action", help="Checkpoint actions")
    
    checkpoint_subparsers.add_parser("save", help="Save checkpoint")
    checkpoint_subparsers.add_parser("load", help="Load checkpoint")
    
    # Import command
    import_parser = subparsers.add_parser("import", help="Import tools")
    import_parser.add_argument("--file", help="Import file")
    
    # General commands
    subparsers.add_parser("status", help="Check NLM status")
    subparsers.add_parser("help", help="Show help information")
    
    args = parser.parse_args()
    
    if args.version:
        print("NLM Python Bindings v0.1.0")
        return
    
    if args.example:
        example_usage()
        return
    
    if not args.command:
        parser.print_help()
        return
    
    # Initialize NLM with config file if provided
    nlm = NLMTool(args.config if args.config else "nlm_config.json")
    
    try:
        if args.command == "config":
            if args.config_action == "create":
                print("Creating configuration file...")
                result = nlm.config_tool.create_config("example.cfg")
                print(f"Result: {result['success']}")
            elif args.config_action == "validate":
                print("Validating configuration file...")
                result = nlm.config_tool.validate_config("example.cfg")
                print(f"Result: {result['success']}")
            elif args.config_action == "list":
                print("Listing templates...")
                result = nlm.config_tool.list_templates()
                print(f"Result: {result['success']}")
                if result['stdout']:
                    print(result['stdout'])
                    
        elif args.command == "validate":
            print("Running validation...")
            result = nlm.validate_tool.validate(args.config_file, args.brain_tests)
            print(f"Result: {result['success']}")
            if result['stdout']:
                print(result['stdout'])
                
        elif args.command == "info":
            print("Getting system information...")
            result = nlm.info_tool.get_info(args.config_file)
            print(f"Result: {result['success']}")
            if result['stdout']:
                print(result['stdout'])
                
        elif args.command == "benchmark":
            print("Running benchmark...")
            result = nlm.benchmark_tool.run_detailed_benchmark(
                neurons=args.neurons,
                steps=args.steps
            )
            print(f"Result: {result['success']}")
            if result['stdout']:
                print(result['stdout'])
                
        elif args.command == "checkpoint":
            if args.checkpoint_action == "save":
                print("Saving checkpoint...")
                result = nlm.checkpoint_tool.save_checkpoint()
                print(f"Result: {result['success']}")
            elif args.checkpoint_action == "load":
                print("Loading checkpoint...")
                result = nlm.import_tool.load_checkpoint("checkpoint.nckpt")
                print(f"Result: {result['success']}")
                
        elif args.command == "import":
            print("Importing...")
            result = nlm.import_tool.load_checkpoint(args.file)
            print(f"Result: {result['success']}")
            
        elif args.command == "status":
            print("Checking NLM status...")
            result = nlm.info_tool.check_installation()
            print(f"NLM Status: {'OK' if result['success'] else 'Failed'}")
            
        elif args.command == "help":
            print("NLM Python Bindings Help")
            print("Available commands: config, validate, info, benchmark, checkpoint, import, status")
            
    except NLMCommandError as e:
        print(f"Error: {e}")
        return 1
    except Exception as e:
        print(f"Unexpected error: {e}")
        return 1
    
    print("Operation completed successfully!")
    return 0

if __name__ == "__main__":
    sys.exit(main())
