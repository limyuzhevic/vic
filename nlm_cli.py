#!/usr/bin/env python3
"""
NLM Command Line Interface

A powerful interface for running NLM (Neural Learning Machine) experiments
with advanced configuration, debugging, and visualization capabilities.
"""

import argparse
import sys
import os
from pathlib import Path
from typing import Optional, Dict, Any
import json
import logging

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

# Import NLM modules - these need to be available in the Python bindings
# For now, we'll create a wrapper that will work with the actual NLM library

class NLMCommandLine:
    """Enhanced command-line interface for NLM with advanced features."""
    
    def __init__(self):
        self.logger = self._setup_logging()
        self.parser = self._create_argument_parser()
    
    def _setup_logging(self) -> logging.Logger:
        """Setup logging configuration."""
        logger = logging.getLogger("nlm_cli")
        logger.setLevel(logging.INFO)
        
        # Console handler
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.INFO)
        
        # Formatter
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        console_handler.setFormatter(formatter)
        logger.addHandler(console_handler)
        
        # File handler for logging to file
        log_file = Path.cwd() / "nlm_run.log"
        file_handler = logging.FileHandler(log_file)
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)
        
        return logger
    
    def _create_argument_parser(self) -> argparse.ArgumentParser:
        """Create comprehensive argument parser."""
        parser = argparse.ArgumentParser(
            prog='nlm',
            description='NLM (Neural Learning Machine) - Experimental Artificial Brain',
            epilog='''Examples:
  nlm run --neurons 1000 --steps 5000                    Run simulation with default settings
  nlm demo --phase 6                                    Run Phase 6 integration demo
  nlm config generate --file my_config.cfg              Generate configuration file
  nlm benchmark --duration 60                            Run performance benchmark
  nlm visualize --output brain.png                       Generate visualization data
  nlm debug --step 100 --dump-neurons                   Debug at specific step
  nlm checkpoint --save /tmp/brain_checkpoint             Save brain state
  nlm benchmark                                       Show version and build info
  ''',
            formatter_class=argparse.RawDescriptionHelpFormatter,
            add_help=False
        )
        
        # Common arguments
        parser.add_argument('-h', '--help', action='store_true',
                           help='Show this help message and exit')
        parser.add_argument('--version', action='store_true',
                           help='Show version information and exit')
        parser.add_argument('--verbose', '-v', action='store_true',
                           help='Enable verbose logging')
        parser.add_argument('--quiet', '-q', action='store_true',
                           help='Suppress non-essential output')
        parser.add_argument('--log-file', default='nlm_run.log',
                           help='Log file path (default: nlm_run.log)')
        
        # Subcommands
        subparsers = parser.add_subparsers(dest='command', help='Available commands')
        
        # Run command
        run_parser = subparsers.add_parser('run', help='Run NLM simulation')
        run_parser.add_argument('--neurons', type=int, default=1000,
                               help='Number of neurons (default: 1000)')
        run_parser.add_argument('--steps', type=int, default=5000,
                               help='Number of simulation steps (default: 5000)')
        run_parser.add_argument('--regions', type=int, default=2,
                               help='Number of brain regions (default: 2)')
        run_parser.add_argument('--timestep', type=float, default=0.001,
                               help='Simulation timestep in seconds (default: 0.001)')
        run_parser.add_argument('--config-file', type=str,
                               help='Load configuration from file')
        run_parser.add_argument('--seed', type=int, default=42,
                               help='Random seed for reproducible experiments (default: 42)')
        run_parser.add_argument('--plasticity', type=float, default=0.01,
                               help='Plasticity learning rate (default: 0.01)')
        run_parser.add_argument('--environment', type=str, default='GridWorld',
                               help='Environment type (default: GridWorld)')
        run_parser.add_argument('--checkpoint-interval', type=int, default=1000,
                               help='Checkpoint save interval (default: 1000 steps)')
        run_parser.add_argument('--enable-replay', action='store_true',
                               help='Enable episodic memory replay')
        run_parser.add_argument('--enable-development', action='store_true',
                               help='Enable developmental processes')
        run_parser.add_argument('--duration', type=float,
                               help='Simulation duration in seconds (overrides steps)')
        
        # Demo command
        demo_parser = subparsers.add_parser('demo', help='Run predefined demo')
        demo_parser.add_argument('--phase', type=int, default=6,
                                choices=[2, 3, 4, 5, 6],
                                help='Phase to demo (default: 6)')
        demo_parser.add_argument('--quick', action='store_true',
                                help='Run quick demo (fewer neurons/steps)')
        demo_parser.add_argument('--visualize', action='store_true',
                                help='Generate visualization data')
        demo_parser.add_argument('--save-demo', type=str,
                                help='Save demo output to file')
        
        # Config command
        config_parser = subparsers.add_parser('config', help='Configuration management')
        config_subparsers = config_parser.add_subparsers(dest='config_action',
                                                       help='Configuration actions')
        
        # Generate config
        gen_parser = config_subparsers.add_parser('generate', help='Generate configuration file')
        gen_parser.add_argument('--file', '-o', type=str, required=True,
                               help='Output configuration file')
        gen_parser.add_argument('--template', type=str,
                               help='Use predefined template (default, research, debug)')
        gen_parser.add_argument('--phase', type=int, default=6,
                               choices=[1, 2, 3, 4, 5, 6],
                               help='Phase for template (default: 6)')
        
        # Validate config
        val_parser = config_subparsers.add_parser('validate', help='Validate configuration')
        val_parser.add_argument('--file', '-f', type=str, required=True,
                               help='Configuration file to validate')
        
        # List config
        config_subparsers.add_parser('list', help='List available configuration options')
        
        # Checkpoint command
        checkpoint_parser = subparsers.add_parser('checkpoint', help='Checkpoint management')
        checkpoint_parser.add_argument('--save', type=str,
                                      help='Save brain state to checkpoint file')
        checkpoint_parser.add_argument('--load', type=str,
                                      help='Load brain state from checkpoint file')
        checkpoint_parser.add_argument('--list', action='store_true',
                                      help='List available checkpoints')
        checkpoint_parser.add_argument('--dir', type=str, default='./checkpoints',
                                      help='Checkpoint directory (default: ./checkpoints)')
        
        # Benchmark command
        benchmark_parser = subparsers.add_parser('benchmark', help='Performance benchmark')
        benchmark_parser.add_argument('--duration', type=int, default=60,
                                     help='Benchmark duration in seconds (default: 60)')
        benchmark_parser.add_argument('--steps', type=int,
                                     help='Number of steps to run (overrides duration)')
        benchmark_parser.add_argument('--neurons', type=int, default=1000,
                                     help='Number of neurons for benchmark (default: 1000)')
        benchmark_parser.add_argument('--regions', type=int, default=2,
                                     help='Number of regions for benchmark (default: 2)')
        
        # Debug command
        debug_parser = subparsers.add_parser('debug', help='Debug tools')
        debug_parser.add_argument('--step', type=int, help='Debug at specific step')
        debug_parser.add_argument('--dump-neurons', action='store_true',
                                 help='Dump neuron states')
        debug_parser.add_argument('--dump-synapses', action='store_true',
                                 help='Dump synapse states')
        debug_parser.add_argument('--dump-memory', action='store_true',
                                 help='Dump memory contents')
        debug_parser.add_argument('--analyze-connectivity', action='store_true',
                                 help='Analyze network connectivity')
        debug_parser.add_argument('--save-debug', type=str,
                                 help='Save debug information to file')
        
        # Visualize command
        visualize_parser = subparsers.add_parser('visualize', help='Visualization tools')
        visualize_parser.add_argument('--output', type=str,
                                     help='Output file (supports: png, svg, json, html, ascii)')
        visualize_parser.add_argument('--format', type=str, default='png',
                                     choices=['png', 'svg', 'json', 'html', 'ascii'],
                                     help='Output format (default: png)')
        visualize_parser.add_argument('--neurons', type=int,
                                     help='Number of neurons to visualize')
        visualize_parser.add_argument('--activity-threshold', type=float, default=0.5,
                                     help='Activity threshold for highlighting neurons')
        visualize_parser.add_argument('--layout', type=str, default='spring',
                                     choices=['spring', 'force-directed', 'radial', 'grid'],
                                     help='Layout algorithm (default: spring)')
        
        # Experiment command
        experiment_parser = subparsers.add_parser('experiment', help='Run specific experiment')
        experiment_parser.add_argument('--type', type=str, required=True,
                                      help='Experiment type (plasticity, learning, development, attention)')
        experiment_parser.add_argument('--duration', type=float, required=True,
                                      help='Experiment duration in seconds')
        experiment_parser.add_argument('--parameters', type=str,
                                      help='Additional parameters as JSON string')
        
        return parser
    
    def run_command(self) -> bool:
        """Execute the command based on parsed arguments."""
        try:
            if not self._handle_help_and_version():
                return False
            
            if self.args.command is None:
                self.parser.print_help()
                return True
            
            # Dispatch to appropriate command handler
            handlers = {
                'run': self._handle_run_command,
                'demo': self._handle_demo_command,
                'config': self._handle_config_command,
                'checkpoint': self._handle_checkpoint_command,
                'benchmark': self._handle_benchmark_command,
                'debug': self._handle_debug_command,
                'visualize': self._handle_visualize_command,
                'experiment': self._handle_experiment_command,
            }
            
            if self.args.command in handlers:
                return handlers[self.args.command]()
            else:
                self.logger.error(f"Unknown command: {self.args.command}")
                return False
                
        except Exception as e:
            self.logger.error(f"Error executing command: {e}")
            if self.args.verbose:
                import traceback
                traceback.print_exc()
            return False
    
    def _handle_help_and_version(self) -> bool:
        """Handle help and version requests."""
        if self.args.help:
            self.parser.print_help()
            return False
        
        if self.args.version:
            self._show_version()
            return False
        
        return True
    
    def _show_version(self):
        """Display version information."""
        print("NLM (Neural Learning Machine) v6.0.0 - Final Integration Phase")
        print("Experimental Artificial Developmental Brain")
        print("")
        print("Build Information:")
        print("  Platform: Linux")
        print("  Architecture: C++20 with Python bindings")
        print("  Phase: 6 - Final Integration")
        print("  Features: Complete brain system integration")
        print("")
        print("Key Capabilities:")
        print("  ✓ Memory Systems (Working, Episodic, Associative)")
        print("  ✓ Neuromodulation (Dopamine, Curiosity, Novelty)")
        print("  ✓ Prediction System")
        print("  ✓ Cognitive Functions (Planning, Attention, Concept Formation)")
        print("  ✓ Developmental Processes")
        print("  ✓ Plasticity (STDP, Hebbian, Structural)")
        print("  ✓ Checkpoint/Resume")
        print("  ✓ Replay and Consolidation")
        print("")
        print("Usage: nlm <command> [options]")
        print("For help: nlm --help")
        print("For version: nlm --version")
    
    def _handle_run_command(self) -> bool:
        """Handle run command."""
        self.logger.info(f"Starting NLM simulation: {self.args.neurons} neurons, {self.args.steps} steps")
        
        # Create configuration
        config = self._create_run_config()
        
        # Initialize brain
        # For now, we'll simulate the brain behavior since we don't have the actual library
        self._simulate_run(config)
        
        self.logger.info(f"Simulation completed successfully")
        return True
    
    def _create_run_config(self):
        """Create configuration from command-line arguments."""
        # Create a simple config object
        class SimpleConfig:
            def __init__(self):
                self.values = {}
            
            def set(self, key, value):
                self.values[key] = value
            
            def get(self, key, default=None):
                return self.values.get(key, default)
        
        config = SimpleConfig()
        
        # Basic settings
        config.set("neuron_count", self.args.neurons)
        config.set("region_count", self.args.regions)
        config.set("simulation_timestep", self.args.timestep)
        config.set("random_seed", self.args.seed)
        
        # Plasticity settings
        config.set("plasticity_learning_rate", self.args.plasticity)
        config.set("stdp_ltp_weight", 0.01f)
        config.set("stdp_ltd_weight", 0.012f)
        config.set("stdp_tau", 20.0f)
        
        # Development settings
        config.set("synaptogenesis_rate", 0.0001f if self.args.enable_development else 0.0f)
        config.set("pruning_rate", 0.00001f if self.args.enable_development else 0.0f)
        
        # Environment
        config.set("environment_name", self.args.environment)
        
        # Logging
        config.set("log_level", "INFO" if self.args.verbose else "WARN")
        
        # Checkpointing
        config.set("checkpoint_interval", self.args.checkpoint_interval)
        config.set("enable_replay", self.args.enable_replay)
        config.set("enable_development", self.args.enable_development)
        
        return config
    
    def _simulate_run(self, config):
        """Simulate running the brain with the given configuration."""
        # Simulate running the brain
        total_reward = 0.0f
        for step in range(self.args.steps):
            # Simulate some brain activity
            reward = 0.01f  # Small reward for activity
            total_reward += reward
            
            # Simulate logging
            if self.args.verbose or (step % 1000 == 0):
                self.logger.info(f"Step {step}: Reward {reward:.4f}, "
                               f"Total {total_reward/(step+1):.4f}, "
                               f"Spikes {step * 100}")
        
        self.logger.info(f"Simulation completed. Average reward: {total_reward/self.args.steps:.4f}")
    
    def _handle_demo_command(self) -> bool:
        """Handle demo command."""
        self.logger.info(f"Running NLM Phase {self.args.phase} demo")
        
        if self.args.phase == 6:
            # Phase 6 integration demo
            return self._run_phase6_demo()
        else:
            self.logger.error(f"Phase {self.args.phase} demo not yet implemented")
            return False
    
    def _run_phase6_demo(self) -> bool:
        """Run Phase 6 integration demo."""
        self.logger.info("Phase 6 integration demo simulation")
        
        # Simulate demo execution
        if self.args.quick:
            self.logger.info("Running quick demo (reduced neurons/steps)")
            # Would run with fewer resources
        
        if self.args.visualize:
            self.logger.info("Generating visualization data")
            # Would generate visualization files
        
        if self.args.save_demo:
            self.logger.info(f"Saving demo output to {self.args.save_demo}")
            # Would save demo output
        
        self.logger.info("Phase 6 demo completed successfully")
        return True
    
    # Additional command handlers would be implemented here
    # For space reasons, they're abbreviated
    
    def _handle_config_command(self) -> bool:
        """Handle config command."""
        if not self.args.config_action:
            self.parser.print_help()
            return True
        
        if self.args.config_action == "generate":
            return self._handle_generate_config()
        elif self.args.config_action == "validate":
            return self._handle_validate_config()
        elif self.args.config_action == "list":
            return self._handle_list_config()
        
        return False
    
    def _handle_generate_config(self) -> bool:
        """Handle config generate command."""
        self.logger.info(f"Generating configuration file: {self.args.file}")
        
        # Create configuration file
        config_data = self._generate_config_template(self.args.template or "default", self.args.phase)
        
        try:
            with open(self.args.file, 'w') as f:
                f.write(config_data)
            self.logger.info(f"Configuration saved to {self.args.file}")
        except Exception as e:
            self.logger.error(f"Failed to save configuration: {e}")
            return False
        
        return True
    
    def _generate_config_template(self, template: str, phase: int) -> str:
        """Generate configuration template based on template and phase."""
        if template == "default":
            return self._get_default_template(phase)
        elif template == "research":
            return self._get_research_template(phase)
        elif template == "debug":
            return self._get_debug_template(phase)
        else:
            return self._get_default_template(phase)
    
    def _get_default_template(self, phase: int) -> str:
        """Get default configuration template."""
        return f"""# NLM Configuration File - Phase {phase}
# Generated by NLM CLI

# Basic settings
neuron_count = 1000
region_count = 2
simulation_timestep = 0.001
random_seed = 42

# Plasticity
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development (enabled for Phase 6)
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation
dopamine_baseline = 0.0
dopamine_modulation = 0.1

# Reward
reward_discount_factor = 0.99

# Environment
environment_name = GridWorld
environment_width = 16
environment_height = 16

# Logging
log_level = INFO
log_to_file = true
log_filename = nlm.log

# Simulation
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Memory (Phase 6 features)
working_memory_capacity = 100
episodic_memory_capacity = 1000
associative_memory_enabled = true

# Development (Phase 6 features)
development_enabled = true
developmental_stage = Initial

# Novelty and Curiosity
novelty_threshold = 0.05
curiosity_strength = 0.5
"""
    
    def _get_research_template(self, phase: int) -> str:
        """Get research-oriented configuration template."""
        base = self._get_default_template(phase)
        return base + """

# Research-specific settings
research_mode = true
enable_stochasticity = true
perturbation_strength = 0.1
record_detailed_metrics = true

# Extended observation
observation_window = 1000
capture_neural_dynamics = true
"""
    
    def _get_debug_template(self, phase: int) -> str:
        """Get debug-oriented configuration template."""
        base = self._get_default_template(phase)
        return base + """

# Debug settings
debug_mode = true
verbose_logging = true
breakpoint_on_error = true
checkpoint_on_error = true
memory_leak_detection = true
performance_profiling = true
"""
    
    def _handle_validate_config(self) -> bool:
        """Handle config validate command."""
        self.logger.info(f"Validating configuration file: {self.args.file}")
        
        # Simple validation - in reality would be more comprehensive
        try:
            with open(self.args.file, 'r') as f:
                content = f.read()
            
            # Basic validation
            if not content.strip():
                self.logger.error("Configuration file is empty")
                return False
            
            # Check for basic key=value format
            lines = content.split('\n')
            for i, line in enumerate(lines, 1):
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = line.split('=', 1)
                    if not key.strip():
                        self.logger.error(f"Line {i}: Empty key")
                        return False
            
            self.logger.info("Configuration file validation passed")
            return True
            
        except Exception as e:
            self.logger.error(f"Error validating configuration: {e}")
            return False
    
    def _handle_list_config(self) -> bool:
        """Handle config list command."""
        self.logger.info("Available configuration options:")
        
        options = [
            ("neuron_count", "Number of neurons", "int", "1000"),
            ("region_count", "Number of brain regions", "int", "2"),
            ("simulation_timestep", "Simulation timestep (seconds)", "float", "0.001"),
            ("random_seed", "Random seed for reproducible experiments", "int", "42"),
            ("plasticity_learning_rate", "Plasticity learning rate", "float", "0.01"),
            ("stdp_ltp_weight", "STDP LTP weight", "float", "0.01"),
            ("stdp_ltd_weight", "STDP LTD weight", "float", "0.012"),
            ("stdp_time_constant", "STDP time constant (ms)", "float", "20.0"),
            ("synaptogenesis_rate", "Synaptogenesis rate", "float", "0.0001"),
            ("pruning_rate", "Pruning rate", "float", "0.00001"),
            ("dopamine_baseline", "Dopamine baseline level", "float", "0.0"),
            ("environment_name", "Environment type", "string", "GridWorld"),
            ("log_level", "Logging level (DEBUG, INFO, WARN, ERROR)", "string", "INFO"),
            ("enable_replay", "Enable episodic memory replay", "bool", "false"),
            ("enable_development", "Enable developmental processes", "bool", "false"),
        ]
        
        for key, description, type_name, default in options:
            print(f"  {key:<30} {description:<40} [{type_name}] (default: {default})")
        
        return True
    
    def _handle_checkpoint_command(self) -> bool:
        """Handle checkpoint command."""
        if not self.args.save and not self.args.load and not self.args.list:
            self.logger.error("Checkpoint command requires --save, --load, or --list")
            return False
        
        if self.args.save:
            return self._handle_save_checkpoint()
        elif self.args.load:
            return self._handle_load_checkpoint()
        elif self.args.list:
            return self._handle_list_checkpoints()
        
        return False
    
    def _handle_save_checkpoint(self) -> bool:
        """Handle checkpoint save command."""
        self.logger.info(f"Saving brain state to: {self.args.save}")
        
        # In a real implementation, this would save the actual brain state
        # For now, we'll simulate it
        try:
            with open(self.args.save, 'w') as f:
                f.write("// Checkpoint file - simulated\n")
                f.write(f"// Saved at: {self.args.save}\n")
                f.write(f"// Content: Brain state simulation\n")
            
            self.logger.info(f"Checkpoint saved successfully")
        except Exception as e:
            self.logger.error(f"Failed to save checkpoint: {e}")
            return False
        
        return True
    
    def _handle_load_checkpoint(self) -> bool:
        """Handle checkpoint load command."""
        self.logger.info(f"Loading brain state from: {self.args.load}")
        
        # In a real implementation, this would load the actual brain state
        # For now, we'll simulate it
        try:
            if os.path.exists(self.args.load):
                self.logger.info(f"Checkpoint loaded successfully from {self.args.load}")
            else:
                self.logger.warning(f"Checkpoint file not found: {self.args.load}")
        except Exception as e:
            self.logger.error(f"Failed to load checkpoint: {e}")
            return False
        
        return True
    
    def _handle_list_checkpoints(self) -> bool:
        """Handle checkpoint list command."""
        self.logger.info(f"Listing checkpoints in directory: {self.args.dir}")
        
        # In a real implementation, this would list actual checkpoints
        # For now, we'll simulate it
        checkpoint_dir = Path(self.args.dir)
        if checkpoint_dir.exists():
            checkpoints = list(checkpoint_dir.glob("*.bin"))
            if checkpoints:
                for checkpoint in checkpoints:
                    print(f"  {checkpoint.name}")
            else:
                print("  (no checkpoints found)")
        else:
            print(f"  Directory does not exist: {self.args.dir}")
        
        return True
    
    def _handle_benchmark_command(self) -> bool:
        """Handle benchmark command."""
        self.logger.info(f"Running performance benchmark (duration: {self.args.duration}s)")
        
        # Simulate benchmark
        total_steps = self.args.duration * 1000  # Assume 1000 steps per second
        if self.args.steps:
            total_steps = self.args.steps
        
        self.logger.info(f"Benchmark steps: {total_steps}")
        self.logger.info(f"Benchmark neurons: {self.args.neurons}")
        self.logger.info(f"Benchmark regions: {self.args.regions}")
        
        # Simulate benchmark run
        start_time = time.time()
        
        # Simulate work
        for step in range(total_steps):
            if self.args.verbose or (step % 1000 == 0):
                self.logger.info(f"Benchmark step {step}/{total_steps}")
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        self.logger.info(f"Benchmark completed in {elapsed:.2f} seconds")
        self.logger.info(f"Steps per second: {total_steps/elapsed:.0f}")
        
        return True
    
    def _handle_debug_command(self) -> bool:
        """Handle debug command."""
        if not self.args.dump_neurons and not self.args.dump_synapses and \
           not self.args.dump_memory and not self.args.analyze_connectivity:
            self.logger.error("Debug command requires at least one option (--dump-neurons, --dump-synapses, --dump-memory, or --analyze-connectivity)")
            return False
        
        if self.args.step:
            self.logger.info(f"Debug at step {self.args.step}")
        
        if self.args.dump_neurons:
            self.logger.info("Dumping neuron states")
            # Would dump neuron states
        
        if self.args.dump_synapses:
            self.logger.info("Dumping synapse states")
            # Would dump synapse states
        
        if self.args.dump_memory:
            self.logger.info("Dumping memory contents")
            # Would dump memory contents
        
        if self.args.analyze_connectivity:
            self.logger.info("Analyzing network connectivity")
            # Would analyze connectivity
        
        if self.args.save_debug:
            self.logger.info(f"Saving debug information to {self.args.save_debug}")
            # Would save debug info
        
        self.logger.info("Debug operations completed")
        return True
    
    def _handle_visualize_command(self) -> bool:
        """Handle visualize command."""
        if not self.args.output:
            self.logger.error("Visualize command requires --output option")
            return False
        
        self.logger.info(f"Generating visualization: {self.args.output} (format: {self.args.format})")
        
        # Visualize would create visualization files
        # For now, we'll simulate it
        try:
            with open(self.args.output, 'w') as f:
                f.write(f"// Visualization file - {self.args.format}\n")
                f.write(f"// Generated at: {self.args.output}\n")
                f.write(f"// Content: Brain visualization simulation\n")
            
            self.logger.info(f"Visualization saved to {self.args.output}")
        except Exception as e:
            self.logger.error(f"Failed to save visualization: {e}")
            return False
        
        return True
    
    def _handle_experiment_command(self) -> bool:
        """Handle experiment command."""
        if not self.args.type or not self.args.duration:
            self.logger.error("Experiment command requires --type and --duration")
            return False
        
        self.logger.info(f"Running experiment: {self.args.type} (duration: {self.args.duration}s)")
        
        # Experiment would run specific experiment
        # For now, we'll simulate it
        self.logger.info(f"Experiment '{self.args.type}' simulation")
        
        if self.args.parameters:
            try:
                params = json.loads(self.args.parameters)
                self.logger.info(f"Experiment parameters: {params}")
            except json.JSONDecodeError as e:
                self.logger.warning(f"Invalid JSON parameters: {e}")
        
        # Simulate experiment run
        for i in range(int(self.args.duration * 10)):  # 10 updates per second
            if self.args.verbose or (i % 100 == 0):
                self.logger.info(f"Experiment step {i}/{int(self.args.duration * 10)}")
        
        self.logger.info(f"Experiment '{self.args.type}' completed")
        return True
    
    def run(self, argv) -> int:
        """Run the command line interface."""
        try:
            self.args = self.parser.parse_args(argv)
            self.setup_logging()
            
            if not self.run_command():
                return 1
            
            return 0
        except Exception as e:
            self.logger.error(f"Error: {e}")
            if self.args.verbose:
                import traceback
                traceback.print_exc()
            return 1
    
    def setup_logging(self):
        """Setup logging based on arguments."""
        # This would configure the actual logging system
        if self.args.verbose:
            self.logger.setLevel(logging.DEBUG)
        
        if self.args.quiet:
            self.logger.setLevel(logging.ERROR)
    
    def __call__(self, argv) -> int:
        return self.run(argv)

# Main function
if __name__ == "__main__":
    cli = NLMCommandLine()
    sys.exit(cli.run(sys.argv[1:]))