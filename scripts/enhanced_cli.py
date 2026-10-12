#!/usr/bin/env python3
"""Enhanced CLI helper for NLM with advanced features"""

import sys
import os
from pathlib import Path

class NLMCliHelper:
    """Advanced CLI helper for NLM with configuration management"""
    
    @staticmethod
    def print_help():
        """Print enhanced help information"""
        help_text = """
NLM - Neural Learning Machine (Enhanced Edition)

USAGE:
  nlm [OPTIONS] [EXECUTION_OPTIONS]
  nlm --config <file> [OPTIONS] [EXECUTION_OPTIONS]

CONFIGURATION OPTIONS:
  --help                          Show this help message
  --version                       Show version information
  --config <file>                 Configuration file (default: configs/default.cfg)
  --config-format <format>        Configuration file format (cfg, json, auto)
  --list-configs                  List available configuration files
  --create-template               Create a new configuration template file
  --validate <file>               Validate configuration file
  --set <key>=<value>             Set configuration value for this run
  --get <key>                     Get configuration value from config
  --save-config <file>             Save current configuration to file
  --backup-config <file>          Create backup of current configuration

EXECUTION OPTIONS:
  --phase <number>                Run specific phase (1-6)
  --demo                          Run demo mode
  --quick                         Quick test mode (faster, less verbose)
  --benchmark                     Run performance benchmarks
  --plot                          Enable plotting output
  --output <dir>                  Set output directory
  --log-level <level>             Set logging level (DEBUG, INFO, WARNING, ERROR)
  --log-file <file>               Set log file
  --threads <n>                   Number of threads for parallel processing
  --cuda                          Enable CUDA acceleration if available

PRESET MODES:
  --preset <name>                 Use preset configuration (debug, release, demo, research)
  --debug-preset                  Use debug configuration (small scale, verbose output)
  --release-preset                Use release configuration (optimized, minimal output)
  --demo-preset                   Use demo configuration (medium scale, interesting results)
  --research-preset               Use research configuration (large scale, comprehensive metrics)

ADVANCED OPTIONS:
  --dry-run                        Validate configuration without running
  --skip-validation                Skip configuration validation
  --force                          Override warnings and prompts
  --export <file>                 Export configuration to file
  --import <file>                 Import configuration from file
  --copy <source> <dest>           Copy configuration between files
  --diff <file1> <file2>           Compare two configuration files

PERFORMANCE OPTIONS:
  --optimized                      Use optimized build mode
  --fast                          Fast mode (reduced precision, fewer steps)
  --accurate                       Accurate mode (full precision, more steps)
  --quality <level>               Quality mode (low, medium, high, maximum)

CONFIGURATION FORMATS:
  .cfg                           Simple key=value format
  .json                          JSON format (recommended for complex configurations)
  .yaml/.yml                     YAML format (human-readable)

ENVIRONMENTAL OPTIONS:
  --seed <number>                 Random seed for reproducible results
  --steps <number>                Number of simulation steps
  --timestep <seconds>            Simulation timestep
  --network-size <neurons>        Number of neurons in network
  --regions <n>                   Number of brain regions
  --connections <percentage>      Connection probability (0.0-1.0)

OUTPUT OPTIONS:
  --quiet                         Minimal output (results only)
  --verbose                       Verbose output (all diagnostic information)
  --progress                       Show progress indicators
  --save-results <file>            Save simulation results to file
  --generate-report                Generate comprehensive analysis report

EXAMPLES:
  # Basic execution with default config
  nlm

  # Run with custom configuration
  nlm --config my_config.json --demo

  # Use debug preset with specific settings
  nlm --preset debug --set neuron_count=100 --set max_simulation_steps=1000

  # Create and use a template
  nlm --create-template my_template.json
  nlm --config my_template.json --quick

  # Run benchmark with specific settings
  nlm --benchmark --preset research --steps 5000 --network-size 5000

  # Export configuration for sharing
  nlm --get all > shared_config.cfg
  nlm --export shared_config.json

  # Compare configurations
  nlm --config original.cfg --config-format cfg --diff modified.cfg

PRESETS:
  debug: Small network, verbose logging, quick execution
  release: Optimized performance, minimal logging, production-ready
  demo: Medium scale, interesting behaviors, educational examples
  research: Large scale, comprehensive metrics, scientific validation

For more information, visit: https://github.com/nlm-project/nlm
  """
        print(help_text)
    
    @staticmethod
    def parse_cli_args(argv: list) -> dict:
        """Parse command line arguments with enhanced options"""
        args = {
            'config_file': 'configs/default.cfg',
            'config_format': 'auto',
            'phase': None,
            'mode': 'auto',  # auto, demo, quick, benchmark, plot
            'preset': None,
            'output_dir': '.',
            'log_level': 'INFO',
            'log_file': None,
            'threads': None,
            'cuda': False,
            'quality': 'medium',
            'verbose': False,
            'quiet': False,
            'progress': False,
            'dry_run': False,
            'skip_validation': False,
            'force': False,
            'set_values': {},
            'custom_options': {}
        }
        
        i = 1
        while i < len(argv):
            arg = argv[i]
            
            if arg == '--help':
                NLMCliHelper.print_help()
                sys.exit(0)
            elif arg == '--version':
                print("NLM Version 2.0.0 (Enhanced Edition)")
                print("Phase 6 Final Integration with Enhanced Features")
                sys.exit(0)
            elif arg == '--list-configs':
                NLMCliHelper.list_config_files()
                sys.exit(0)
            elif arg == '--create-template':
                NLMCliHelper.create_template()
                sys.exit(0)
            elif arg == '--verbose':
                args['verbose'] = True
                args['quiet'] = False
            elif arg == '--quiet':
                args['quiet'] = True
                args['verbose'] = False
            elif arg == '--progress':
                args['progress'] = True
            elif arg.startswith('--config='):
                args['config_file'] = arg.split('=', 1)[1]
            elif arg == '--config':
                i += 1
                if i < len(argv):
                    args['config_file'] = argv[i]
            elif arg.startswith('--config-format='):
                args['config_format'] = arg.split('=', 1)[1]
            elif arg == '--config-format':
                i += 1
                if i < len(argv):
                    args['config_format'] = argv[i]
            elif arg.startswith('--set='):
                key_value = arg.split('=', 1)[1]
                if '=' in key_value:
                    key, value = key_value.split('=', 1)
                    args['set_values'][key] = value
                else:
                    args['set_values'][key_value] = True
            elif arg == '--set':
                i += 1
                if i < len(argv):
                    if '=' in argv[i]:
                        key, value = argv[i].split('=', 1)
                        args['set_values'][key] = value
                    else:
                        args['set_values'][argv[i]] = True
            elif arg == '--preset':
                i += 1
                if i < len(argv):
                    args['preset'] = argv[i]
            elif arg.startswith('--preset='):
                args['preset'] = arg.split('=', 1)[1]
            elif arg == '--phase':
                i += 1
                if i < len(argv):
                    try:
                        args['phase'] = int(argv[i])
                    except ValueError:
                        print(f"Error: Phase must be an integer, got '{argv[i]}'")
                        sys.exit(1)
            elif arg.startswith('--phase='):
                try:
                    args['phase'] = int(arg.split('=', 1)[1])
                except ValueError:
                    print(f"Error: Phase must be an integer, got '{arg.split('=', 1)[1]}'")
                    sys.exit(1)
            elif arg in ['--demo', '--quick', '--benchmark', '--plot', '--optimized', 
                        '--fast', '--accurate', '--dry-run', '--skip-validation', '--force']:
                args[arg.lstrip('--')] = True
            elif arg == '--seed':
                i += 1
                if i < len(argv):
                    try:
                        args['set_values']['random_seed'] = int(argv[i])
                    except ValueError:
                        print(f"Error: Seed must be an integer, got '{argv[i]}'")
                        sys.exit(1)
            elif arg.startswith('--seed='):
                try:
                    args['set_values']['random_seed'] = int(arg.split('=', 1)[1])
                except ValueError:
                    print(f"Error: Seed must be an integer, got '{arg.split('=', 1)[1]}'")
                    sys.exit(1)
            elif arg == '--steps':
                i += 1
                if i < len(argv):
                    try:
                        args['set_values']['max_simulation_steps'] = int(argv[i])
                    except ValueError:
                        print(f"Error: Steps must be an integer, got '{argv[i]}'")
                        sys.exit(1)
            elif arg.startswith('--steps='):
                try:
                    args['set_values']['max_simulation_steps'] = int(arg.split('=', 1)[1])
                except ValueError:
                    print(f"Error: Steps must be an integer, got '{arg.split('=', 1)[1]}'")
                    sys.exit(1)
            elif arg == '--timestep':
                i += 1
                if i < len(argv):
                    try:
                        args['set_values']['simulation_timestep'] = float(argv[i])
                    except ValueError:
                        print(f"Error: Timestep must be a number, got '{argv[i]}'")
                        sys.exit(1)
            elif arg.startswith('--timestep='):
                try:
                    args['set_values']['simulation_timestep'] = float(arg.split('=', 1)[1])
                except ValueError:
                    print(f"Error: Timestep must be a number, got '{arg.split('=', 1)[1]}'")
                    sys.exit(1)
            elif arg == '--network-size':
                i += 1
                if i < len(argv):
                    try:
                        args['set_values']['neuron_count'] = int(argv[i])
                    except ValueError:
                        print(f"Error: Network size must be an integer, got '{argv[i]}'")
                        sys.exit(1)
            elif arg.startswith('--network-size='):
                try:
                    args['set_values']['neuron_count'] = int(arg.split('=', 1)[1])
                except ValueError:
                    print(f"Error: Network size must be an integer, got '{arg.split('=', 1)[1]}'")
                    sys.exit(1)
            elif arg == '--regions':
                i += 1
                if i < len(argv):
                    try:
                        args['set_values']['region_count'] = int(argv[i])
                    except ValueError:
                        print(f"Error: Regions must be an integer, got '{argv[i]}'")
                        sys.exit(1)
            elif arg.startswith('--regions='):
                try:
                    args['set_values']['region_count'] = int(arg.split('=', 1)[1])
                except ValueError:
                    print(f"Error: Regions must be an integer, got '{arg.split('=', 1)[1]}'")
                    sys.exit(1)
            elif arg == '--connections':
                i += 1
                if i < len(argv):
                    try:
                        value = float(argv[i])
                        if not (0.0 <= value <= 1.0):
                            raise ValueError("Connection probability must be between 0.0 and 1.0")
                        args['set_values']['connection_probability'] = value
                    except ValueError as e:
                        print(f"Error: {e}")
                        sys.exit(1)
            elif arg.startswith('--connections='):
                try:
                    value = float(arg.split('=', 1)[1])
                    if not (0.0 <= value <= 1.0):
                        raise ValueError("Connection probability must be between 0.0 and 1.0")
                    args['set_values']['connection_probability'] = value
                except ValueError as e:
                    print(f"Error: {e}")
                    sys.exit(1)
            elif arg == '--output':
                i += 1
                if i < len(argv):
                    args['output_dir'] = argv[i]
            elif arg.startswith('--output='):
                args['output_dir'] = arg.split('=', 1)[1]
            elif arg == '--log-level':
                i += 1
                if i < len(argv):
                    args['log_level'] = argv[i].upper()
            elif arg.startswith('--log-level='):
                args['log_level'] = arg.split('=', 1)[1].upper()
            elif arg == '--log-file':
                i += 1
                if i < len(argv):
                    args['log_file'] = argv[i]
            elif arg.startswith('--log-file='):
                args['log_file'] = arg.split('=', 1)[1]
            elif arg == '--threads':
                i += 1
                if i < len(argv):
                    try:
                        args['threads'] = int(argv[i])
                    except ValueError:
                        print(f"Error: Threads must be an integer, got '{argv[i]}'")
                        sys.exit(1)
            elif arg.startswith('--threads='):
                try:
                    args['threads'] = int(arg.split('=', 1)[1])
                except ValueError:
                    print(f"Error: Threads must be an integer, got '{arg.split('=', 1)[1]}'")
                    sys.exit(1)
            elif arg == '--quality':
                i += 1
                if i < len(argv):
                    args['quality'] = argv[i].lower()
            elif arg.startswith('--quality='):
                args['quality'] = arg.split('=', 1)[1].lower()
            elif arg in ['--cuda', '--dry-run', '--skip-validation', '--force']:
                args[arg.lstrip('--')] = True
            elif arg.startswith('--'):
                args['custom_options'][arg] = True
            
            i += 1
        
        # Handle preset-specific options
        if args['preset']:
            NLMCliHelper.apply_preset(args, args['preset'])
        
        return args
    
    @staticmethod
    def apply_preset(args: dict, preset: str):
        """Apply preset configuration"""
        presets = {
            'debug': {
                'set_values': {
                    'random_seed': 42,
                    'neuron_count': 100,
                    'region_count': 1,
                    'max_simulation_steps': 1000,
                    'connection_probability': 0.05,
                    'log_level': 'DEBUG',
                    'visualization_enabled': False
                },
                'quality': 'low',
                'mode': 'quick'
            },
            'release': {
                'set_values': {
                    'random_seed': 12345,
                    'neuron_count': 5000,
                    'region_count': 2,
                    'max_simulation_steps': 5000,
                    'connection_probability': 0.1,
                    'log_level': 'INFO',
                    'visualization_enabled': False
                },
                'quality': 'high',
                'mode': 'auto'
            },
            'demo': {
                'set_values': {
                    'random_seed': 999,
                    'neuron_count': 1000,
                    'region_count': 2,
                    'max_simulation_steps': 2000,
                    'connection_probability': 0.15,
                    'log_level': 'INFO',
                    'visualization_enabled': True
                },
                'quality': 'medium',
                'mode': 'demo'
            },
            'research': {
                'set_values': {
                    'random_seed': 2024,
                    'neuron_count': 10000,
                    'region_count': 4,
                    'max_simulation_steps': 20000,
                    'connection_probability': 0.08,
                    'log_level': 'INFO',
                    'visualization_enabled': False
                },
                'quality': 'maximum',
                'mode': 'auto'
            }
        }
        
        if preset in presets:
            print(f"Applying '{preset}' preset configuration...")
            preset_config = presets[preset]
            
            # Merge preset configuration
            args['set_values'].update(preset_config['set_values'])
            args['quality'] = preset_config['quality']
            args['mode'] = preset_config['mode']
            
            # Override with preset-specific defaults
            for key, value in preset_config['set_values'].items():
                args['set_values'][key] = value
        else:
            print(f"Warning: Unknown preset '{preset}'. Available presets: {list(presets.keys())}")
    
    @staticmethod
    def create_template():
        """Create a new configuration template file"""
        template_content = """# NLM Configuration Template
# Copy this file and customize for your specific needs

# Core Simulation Parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity Parameters
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development Parameters
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation Parameters
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Environment Configuration
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging Configuration
log_level = INFO
log_to_file = false
log_filename = nlm_simulation.log

# Simulation Limits
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization Configuration
visualization_enabled = false
visualization_update_rate = 30.0
"""
        
        template_path = Path("configs/nlm_config_template.cfg")
        
        # Create configs directory if it doesn't exist
        template_path.parent.mkdir(exist_ok=True)
        
        with open(template_path, 'w') as f:
            f.write(template_content)
        
        print(f"Created configuration template at: {template_path}")
        print("Customize the values in this file for your simulation needs.")
    
    @staticmethod
    def list_config_files():
        """List available configuration files"""
        config_dir = Path("configs")
        if not config_dir.exists():
            print("No configs directory found")
            return
        
        print("Available configuration files:")
        for config_file in sorted(config_dir.glob("*")):
            if config_file.is_file():
                size = config_file.stat().st_size
                modified = config_file.stat().st_mtime
                print(f"  {config_file.name:30} ({size} bytes)")
    
    @staticmethod
    def format_value(value: any) -> str:
        """Format configuration value for display"""
        if isinstance(value, bool):
            return "true" if value else "false"
        elif isinstance(value, str):
            return f'"{value}"'
        else:
            return str(value)
"""

# Command line interface for enhanced CLI helper
if __name__ == "__main__":
    cli = NLMCliHelper()
    
    if len(sys.argv) > 1:
        # Parse command line arguments
        args = cli.parse_cli_args(sys.argv)
        
        # Print parsed arguments if requested
        if '--verbose' in sys.argv or '--debug' in sys.argv:
            print("Parsed command line arguments:")
            for key, value in args.items():
                if key not in ['set_values', 'custom_options']:
                    print(f"  {key}: {value}")
            
            if args['set_values']:
                print("Set values:")
                for key, value in args['set_values'].items():
                    print(f"  {key}: {cli.format_value(value)}")
        
        if args['custom_options']:
            print("Custom options:")
            for key, value in args['custom_options'].items():
                print(f"  {key}: {value}")
    else:
        # No arguments, print help
        cli.print_help()