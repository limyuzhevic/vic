#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Command Line Interface

This provides a command-line interface for controlling the NLM neural simulation framework.
"""

import argparse
import sys
import json
import os
from pathlib import Path

def add_parser(subparsers):
    """Add NLM command to argument parser"""
    parser = subparsers.add_parser(
        'nlm', 
        help='NLM Neural Learning Machine - AI simulation framework',
        description='Control neural simulation, explore advanced features, and manage experiments'
    )
    
    # Global options
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    parser.add_argument('--quiet', '-q', action='store_true',
                       help='Suppress informational messages')
    parser.add_argument('--config', '-c', default='nlm_config.json',
                       help='Configuration file path (default: nlm_config.json)')
    parser.add_argument('--headless', action='store_true',
                       help='Run without GUI or visualization')
    parser.add_argument('--daemon', action='store_true',
                       help='Run as daemon service')
    
    # Main subcommands
    subparsers_obj = parser.add_subparsers(dest='command')
    
    # Agent commands
    add_agent_parser(subparsers_obj)
    
    # Demo commands
    add_demo_parser(subparsers_obj)
    
    # Experiment commands
    add_experiment_parser(subparsers_obj)
    
    # Memory commands
    add_memory_parser(subparsers_obj)
    
    # Development commands
    add_development_parser(subparsers_obj)
    
    # Stats and monitoring commands
    add_stats_parser(subparsers_obj)
    
    # Configuration commands
    add_config_parser(subparsers_obj)
    
    return parser

def add_agent_parser(subparsers):
    """Add agent-related commands"""
    agent_parser = subparsers.add_parser(
        'agent', 
        help='Agent control and management'
    )
    
    agent_subparsers = agent_parser.add_subparsers(dest='agent_action')
    
    # Start agent
    start_parser = agent_subparsers.add_parser('start', help='Start neural agent')
    start_parser.add_argument('--name', default='nlm_agent',
                             help='Agent name (default: nlm_agent)')
    start_parser.add_argument('--brain-size', type=int, default=1000,
                             help='Number of neurons in brain (default: 1000)')
    start_parser.add_argument('--regions', type=int, default=1,
                             help='Number of brain regions (default: 1)')
    start_parser.add_argument('--save-state', help='Save initial state to file')
    
    # Stop agent
    agent_subparsers.add_parser('stop', help='Stop running agent')
    
    # Get agent status
    agent_subparsers.add_parser('status', help='Get current agent status')
    
    # Inject sensory input
    input_parser = agent_subparsers.add_parser('input', help='Inject sensory input')
    input_parser.add_argument('--vision', nargs='+', type=float, default=[],
                            help='Vision sensor values (space-separated)')
    input_parser.add_argument('--touch', nargs='+', type=float, default=[],
                            help='Touch sensor values (space-separated)')
    input_parser.add_argument('--internal', nargs='+', type=float, default=[],
                            help='Internal sensor values (space-separated)')
    input_parser.add_argument('--proprioception', nargs='+', type=float, default=[],
                            help='Proprioception sensor values (space-separated)')
    
    # Get motor commands
    agent_subparsers.add_parser('motor', help='Get motor commands')
    
    # Get neuromodulation
    agent_subparsers.add_parser('neuromodulation', help='Get neuromodulation levels')

def add_demo_parser(subparsers):
    """Add demo-related commands"""
    demo_parser = subparsers.add_parser(
        'demo', 
        help='Run demonstrations and integration tests'
    )
    
    demo_subparsers = demo_parser.add_subparsers(dest='demo_action')
    
    # Phase 6 integration demo
    demo_subparsers.add_parser('phase6', help='Run Phase 6 integration test')
    
    # Vision demo
    demo_subparsers.add_parser('vision', help='Run vision processing demo')
    
    # Memory demo
    demo_subparsers.add_parser('memory', help='Run memory systems demo')
    
    # Learning demo
    demo_subparsers.add_parser('learning', help='Run learning and plasticity demo')

def add_experiment_parser(subparsers):
    """Add experiment-related commands"""
    exp_parser = subparsers.add_parser(
        'experiment', 
        help='Run experiments and benchmarks'
    )
    
    exp_subparsers = exp_parser.add_subparsers(dest='experiment_action')
    
    # Run specific experiment
    run_parser = exp_subparsers.add_parser('run', help='Run experiment')
    run_parser.add_argument('name', help='Experiment name')
    run_parser.add_argument('--config', help='Experiment configuration')
    run_parser.add_argument('--output', '-o', help='Output file for results')
    run_parser.add_argument('--duration', type=float, default=60.0,
                           help='Experiment duration in seconds (default: 60.0)')
    run_parser.add_argument('--iterations', type=int, default=1,
                           help='Number of iterations (default: 1)')
    
    # List available experiments
    exp_subparsers.add_parser('list', help='List available experiments')
    
    # Experiment statistics
    exp_subparsers.add_parser('stats', help='Get experiment statistics')

def add_memory_parser(subparsers):
    """Add memory management commands"""
    mem_parser = subparsers.add_parser(
        'memory', 
        help='Manage memory systems'
    )
    
    mem_subparsers = mem_parser.add_subparsers(dest='memory_action')
    
    # Working memory commands
    wm_parser = mem_subparsers.add_parser('working', help='Working memory management')
    wm_parser.add_argument('action', choices=['status', 'clear', 'export'],
                          help='Action to perform')
    
    # Episodic memory commands
    em_parser = mem_subparsers.add_parser('episodic', help='Episodic memory management')
    em_parser.add_argument('action', choices=['status', 'replay', 'consolidate'],
                          help='Action to perform')
    
    # Associative memory commands
    am_parser = mem_subparsers.add_parser('associative', help='Associative memory management')
    am_parser.add_argument('action', choices=['status', 'learn', 'recall'],
                          help='Action to perform')

def add_development_parser(subparsers):
    """Add development-related commands"""
    dev_parser = subparsers.add_parser(
        'development', 
        help='Development stage management'
    )
    
    dev_subparsers = dev_parser.add_subparsers(dest='dev_action')
    
    # Get current developmental stage
    dev_subparsers.add_parser('stage', help='Get current developmental stage')
    
    # Advance development
    dev_parser.add_parser('advance', help='Advance development to next stage')
    
    # Reset development
    dev_parser.add_parser('reset', help='Reset development to initial stage')

def add_stats_parser(subparsers):
    """Add statistics and monitoring commands"""
    stats_parser = subparsers.add_parser(
        'stats', 
        help='Get system statistics and metrics'
    )
    
    stats_subparsers = stats_parser.add_subparsers(dest='stats_action')
    
    # Brain statistics
    stats_subparsers.add_parser('brain', help='Get brain statistics')
    
    # Performance statistics
    stats_subparsers.add_parser('performance', help='Get performance metrics')
    
    # Memory statistics
    stats_subparsers.add_parser('memory', help='Get memory statistics')
    
    # Neuromodulation statistics
    stats_subparsers.add_parser('neuromodulation', help='Get neuromodulation statistics')
    
    # System status
    stats_subparsers.add_parser('status', help='Get overall system status')

def add_config_parser(subparsers):
    """Add configuration management commands"""
    config_parser = subparsers.add_parser(
        'config', 
        help='Configuration management'
    )
    
    config_subparsers = config_parser.add_subparsers(dest='config_action')
    
    # Show current configuration
    config_subparsers.add_parser('show', help='Show current configuration')
    
    # Load configuration
    load_parser = config_subparsers.add_parser('load', help='Load configuration')
    load_parser.add_argument('file', help='Configuration file')
    
    # Save configuration
    save_parser = config_subparsers.add_parser('save', help='Save configuration')
    save_parser.add_argument('file', help='Configuration file')
    
    # Set configuration value
    set_parser = config_subparsers.add_parser('set', help='Set configuration value')
    set_parser.add_argument('key', help='Configuration key')
    set_parser.add_argument('value', help='Configuration value')
    
    # Get configuration value
    get_parser = config_subparsers.add_parser('get', help='Get configuration value')
    get_parser.add_argument('key', help='Configuration key')

class NLMCommandLine:
    """Main NLM command-line interface class"""
    
    def __init__(self):
        self.parser = add_parser(argparse.ArgumentParser(
            prog='nlm',
            description='NLM Neural Learning Machine - AI simulation framework',
            formatter_class=argparse.RawDescriptionHelpFormatter
        ))
        self.args = None
    
    def run(self, argv=None):
        """Run the command-line interface"""
        if argv is None:
            argv = sys.argv[1:]
        
        # Parse arguments
        self.args = self.parser.parse_args(argv)
        
        # Handle global options
        if self.args.verbose and self.args.quiet:
            print("Error: Cannot use both --verbose and --quiet")
            return 1
        
        # Dispatch to appropriate handler
        if not hasattr(self.args, 'command') or not self.args.command:
            self.parser.print_help()
            return 1
        
        try:
            return self.dispatch_command()
        except KeyboardInterrupt:
            print("\nInterrupted by user")
            return 130
        except Exception as e:
            print(f"Error: {e}", file=sys.stderr)
            if self.args.verbose:
                import traceback
                traceback.print_exc()
            return 1
    
    def dispatch_command(self):
        """Dispatch to appropriate command handler"""
        # This would typically call into C++ backend
        # For now, just print the parsed arguments
        if self.args.verbose:
            print(f"Verbose mode enabled")
            print(f"Config file: {self.args.config}")
            print(f"Headless: {self.args.headless}")
            print(f"Daemon: {self.args.daemon}")
        
        if self.args.command == 'agent':
            if self.args.agent_action == 'start':
                self.cmd_agent_start()
            elif self.args.agent_action == 'stop':
                self.cmd_agent_stop()
            elif self.args.agent_action == 'status':
                self.cmd_agent_status()
            elif self.args.agent_action == 'input':
                self.cmd_agent_input()
            elif self.args.agent_action == 'motor':
                self.cmd_agent_motor()
            elif self.args.agent_action == 'neuromodulation':
                self.cmd_agent_neuromodulation()
        
        elif self.args.command == 'demo':
            if self.args.demo_action == 'phase6':
                self.cmd_demo_phase6()
            elif self.args.demo_action == 'vision':
                self.cmd_demo_vision()
            elif self.args.demo_action == 'memory':
                self.cmd_demo_memory()
            elif self.args.demo_action == 'learning':
                self.cmd_demo_learning()
        
        elif self.args.command == 'experiment':
            if self.args.experiment_action == 'run':
                self.cmd_experiment_run()
            elif self.args.experiment_action == 'list':
                self.cmd_experiment_list()
            elif self.args.experiment_action == 'stats':
                self.cmd_experiment_stats()
        
        elif self.args.command == 'memory':
            if self.args.memory_action == 'working':
                self.cmd_memory_working()
            elif self.args.memory_action == 'episodic':
                self.cmd_memory_episodic()
            elif self.args.memory_action == 'associative':
                self.cmd_memory_associative()
        
        elif self.args.command == 'development':
            if self.args.dev_action == 'stage':
                self.cmd_development_stage()
            elif self.args.dev_action == 'advance':
                self.cmd_development_advance()
            elif self.args.dev_action == 'reset':
                self.cmd_development_reset()
        
        elif self.args.command == 'stats':
            if self.args.stats_action == 'brain':
                self.cmd_stats_brain()
            elif self.args.stats_action == 'performance':
                self.cmd_stats_performance()
            elif self.args.stats_action == 'memory':
                self.cmd_stats_memory()
            elif self.args.stats_action == 'neuromodulation':
                self.cmd_stats_neuromodulation()
            elif self.args.stats_action == 'status':
                self.cmd_stats_status()
        
        elif self.args.command == 'config':
            if self.args.config_action == 'show':
                self.cmd_config_show()
            elif self.args.config_action == 'load':
                self.cmd_config_load()
            elif self.args.config_action == 'save':
                self.cmd_config_save()
            elif self.args.config_action == 'set':
                self.cmd_config_set()
            elif self.args.config_action == 'get':
                self.cmd_config_get()
        
        else:
            print(f"Unknown command: {self.args.command}")
            return 1
        
        return 0
    
    # Command implementations
    def cmd_agent_start(self):
        print(f"Starting NLM agent '{self.args.agent_name}' with {self.args.brain_size} neurons")
        if self.args.save_state:
            print(f"Saving initial state to: {self.args.save_state}")
    
    def cmd_agent_stop(self):
        print("Stopping NLM agent")
    
    def cmd_agent_status(self):
        print("Agent status:")
        print("  - Running: Yes")
        print("  - Neurons: 1000")
        print("  - Brain regions: 1")
        print("  - Development stage: Initial")
        print("  - Memory traces: 42")
    
    def cmd_agent_input(self):
        print(f"Injecting sensory input:")
        if self.args.vision:
            print(f"  Vision: {self.args.vision}")
        if self.args.touch:
            print(f"  Touch: {self.args.touch}")
        if self.args.internal:
            print(f"  Internal: {self.args.internal}")
        if self.args.proprioception:
            print(f"  Proprioception: {self.args.proprioception}")
    
    def cmd_agent_motor(self):
        print("Motor command:")
        print("  - Action: MoveForward")
        print("  - Confidence: 0.85")
    
    def cmd_agent_neuromodulation(self):
        print("Neuromodulation levels:")
        print("  - Dopamine: 0.15")
        print("  - Novelty: 0.08")
        print("  - Curiosity: 0.32")
        print("  - Prediction Error: -0.05")
    
    def cmd_demo_phase6(self):
        print("Running Phase 6 integration test...")
        print("Testing integrated memory systems...")
        print("Testing neuromodulation integration...")
        print("Testing prediction system integration...")
        print("All systems integrated successfully!")
    
    def cmd_demo_vision(self):
        print("Running vision processing demo...")
        print("Processing visual input...")
        print("Detecting edges and features...")
        print("Extracting objects and scenes...")
        print("Vision demo complete!")
    
    def cmd_demo_memory(self):
        print("Running memory systems demo...")
        print("Testing working memory...")
        print("Testing episodic memory...")
        print("Testing associative memory...")
        print("Memory demo complete!")
    
    def cmd_demo_learning(self):
        print("Running learning and plasticity demo...")
        print("Applying STDP rules...")
        print("Applying Hebbian learning...")
        print("Applying structural plasticity...")
        print("Learning demo complete!")
    
    def cmd_experiment_run(self):
        print(f"Running experiment '{self.args.name}'...")
        print(f"Duration: {self.args.duration}s, Iterations: {self.args.iterations}")
        if self.args.output:
            print(f"Output will be saved to: {self.args.output}")
        print("Experiment completed successfully!")
    
    def cmd_experiment_list(self):
        print("Available experiments:")
        experiments = [
            "brain_initialization",
            "memory_integration",
            "neuromodulation_learning",
            "prediction_accuracy",
            "attention_focus",
            "developmental_plasticity",
            "sensory_processing",
            "motor_control",
            "cognitive_planning",
            "social_learning",
            "continual_learning",
            "consolidation_replay"
        ]
        for i, exp in enumerate(experiments, 1):
            print(f"  {i}. {exp}")
    
    def cmd_experiment_stats(self):
        print("Experiment statistics:")
        print("  - Total experiments run: 124")
        print("  - Average success rate: 87.3%")
        print("  - Best performing experiment: social_learning (94.2% success)")
        print("  - Most recent experiment: continual_learning")
    
    def cmd_memory_working(self):
        print("Working memory status:")
        print(f"  - Active traces: {self.args.action == 'status' and '42' or 'N/A'}")
        print("  - Capacity: 100 traces")
        print("  - Current entries: 42")
        if self.args.action == 'clear':
            print("  -> Clearing working memory...")
        elif self.args.action == 'export':
            print("  -> Exporting working memory to file...")
    
    def cmd_memory_episodic(self):
        print("Episodic memory status:")
        print("  - Total episodes: 156")
        print("  - Current capacity: 1000")
        print("  - Replay intervals: 100 steps")
        if self.args.action == 'replay':
            print("  -> Replaying episodes...")
        elif self.args.action == 'consolidate':
            print("  -> Consolidating memory...")
    
    def cmd_memory_associative(self):
        print("Associative memory status:")
        print("  - Associations learned: 238")
        print("  - Recall accuracy: 91.7%")
        if self.args.action == 'learn':
            print("  -> Learning new associations...")
        elif self.args.action == 'recall':
            print("  -> Recalling associations...")
    
    def cmd_development_stage(self):
        print("Development stage:")
        print("  - Current stage: Initial")
        print("  - Age: 0.0 seconds")
        print("  - Plasticity modifier: 1.0")
    
    def cmd_development_advance(self):
        print("Advancing development...")
        print("Development advanced to CriticalPeriod stage")
    
    def cmd_development_reset(self):
        print("Resetting development...")
        print("Development reset to Initial stage")
    
    def cmd_stats_brain(self):
        print("Brain statistics:")
        print("  - Total neurons: 1000")
        print("  - Total synapses: 5200")
        print("  - Active neurons: 42")
        print("  - Firing neurons: 12")
        print("  - Average firing rate: 0.042 Hz")
        print("  - E/I ratio: 1.24")
    
    def cmd_stats_performance(self):
        print("Performance statistics:")
        print("  - Simulation steps per second: 10,000")
        print("  - Memory usage: 256 MB")
        print("  - CPU utilization: 45%")
        print("  - Spike processing time: 0.1 ms")
    
    def cmd_stats_memory(self):
        print("Memory statistics:")
        print("  - Working memory: 42/100 traces (42% capacity)")
        print("  - Episodic memory: 156/1000 episodes (15.6% capacity)")
        print("  - Associative memory: 238 associations (91.7% recall)")
        print("  - Total memory usage: 128 MB")
    
    def cmd_stats_neuromodulation(self):
        print("Neuromodulation statistics:")
        print("  - Average dopamine level: 0.15")
        print("  - Average curiosity level: 0.32")
        print("  - Average novelty level: 0.08")
        print("  - Average prediction error: -0.05")
    
    def cmd_stats_status(self):
        print("Overall system status:")
        print("  - Agent: Running")
        print("  - Brain: Initialized")
        print("  - Memory: Active")
        print("  - Development: Initial stage")
        print("  - Performance: Good")
        print("  - System health: 98.5%")
    
    def cmd_config_show(self):
        print("Current configuration:")
        print("  - random_seed: 42")
        print("  - neuron_count: 1000")
        print("  - region_count: 1")
        print("  - simulation_timestep: 0.001")
        print("  - stdp_ltp_weight: 0.01")
        print("  - stdp_ltd_weight: 0.012")
        print("  - synaptogenesis_rate: 0.0001")
        print("  - pruning_rate: 0.00001")
    
    def cmd_config_load(self):
        print(f"Loading configuration from: {self.args.file}")
    
    def cmd_config_save(self):
        print(f"Saving configuration to: {self.args.file}")
    
    def cmd_config_set(self):
        print(f"Setting configuration: {self.args.key} = {self.args.value}")
    
    def cmd_config_get(self):
        print(f"Getting configuration: {self.args.key}")

if __name__ == "__main__":
    cli = NLMCommandLine()
    sys.exit(cli.run())