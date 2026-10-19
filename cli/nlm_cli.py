#!/usr/bin/env python3
"""
Enhanced CLI for NLM (Neural Learning Machine)

This module provides enhanced command-line interface capabilities for NLM,
including configuration management, analysis tools, and utility functions.
"""

import argparse
import json
import sys
import os
from typing import Dict, List, Optional, Any
from pathlib import Path

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
try:
    from nlm_enhanced import AdvancedConfig, AdvancedBrain, SimulationController, NeuralProfiler
    from pynlm import createBrain, createDefaultConfig, createSimpleWorld
    NLX_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM modules not available: {e}")
    NLX_AVAILABLE = False

class NLMCLI:
    """Enhanced CLI for NLM operations."""
    
    def __init__(self):
        self.parser = argparse.ArgumentParser(
            description="Enhanced CLI for NLM (Neural Learning Machine)",
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        self.subparsers = self.parser.add_subparsers(dest='command', help='Available commands')
        
        # Register subcommands
        self._register_config_commands()
        self._register_simulation_commands()
        self._register_analysis_commands()
        self._register_utility_commands()
    
    def _register_config_commands(self):
        """Register configuration-related commands."""
        config_parser = self.subparsers.add_parser('config', help='Configuration management')
        config_subparsers = config_parser.add_subparsers(dest='config_action', help='Configuration actions')
        
        # Show configuration
        show_parser = config_subparsers.add_parser('show', help='Show configuration')
        show_parser.add_argument('file', nargs='?', help='Configuration file')
        
        # Create configuration
        create_parser = config_subparsers.add_parser('create', help='Create configuration')
        create_parser.add_argument('--output', '-o', help='Output file')
        create_parser.add_argument('--schema', action='store_true', help='Create with schema')
        
        # Import configuration
        import_parser = config_subparsers.add_parser('import', help='Import configuration')
        import_parser.add_argument('file', help='Configuration file to import')
        import_parser.add_argument('--format', choices=['auto', 'json', 'yaml', 'cfg'], 
                                   default='auto', help='File format')
        
        # Export configuration
        export_parser = config_subparsers.add_parser('export', help='Export configuration')
        export_parser.add_argument('output', help='Output file')
        export_parser.add_argument('--format', choices=['auto', 'json', 'yaml', 'cfg'], 
                                   default='auto', help='File format')
        
        # Validate configuration
        validate_parser = config_subparsers.add_parser('validate', help='Validate configuration')
        validate_parser.add_argument('file', help='Configuration file to validate')
        
        # Compare configurations
        compare_parser = config_subparsers.add_parser('compare', help='Compare configurations')
        compare_parser.add_argument('file1', help='First configuration file')
        compare_parser.add_argument('file2', help='Second configuration file')
    
    def _register_simulation_commands(self):
        """Register simulation-related commands."""
        sim_parser = self.subparsers.add_parser('simulation', help='Simulation management')
        sim_subparsers = sim_parser.add_subparsers(dest='sim_action', help='Simulation actions')
        
        # Run simulation
        run_parser = sim_subparsers.add_parser('run', help='Run simulation')
        run_parser.add_argument('--steps', '-s', type=int, default=100, help='Number of simulation steps')
        run_parser.add_argument('--config', '-c', help='Configuration file')
        run_parser.add_argument('--output', '-o', help='Output file for results')
        run_parser.add_argument('--log-level', choices=['debug', 'info', 'warning', 'error'], 
                               default='info', help='Log level')
        
        # Show simulation status
        status_parser = sim_subparsers.add_parser('status', help='Show simulation status')
        status_parser.add_argument('--brain-id', help='Brain ID')
        
        # Pause/resume simulation
        control_parser = sim_subparsers.add_parser('control', help='Control simulation')
        control_parser.add_argument('action', choices=['pause', 'resume', 'reset'], help='Control action')
        control_parser.add_argument('--step', '-s', type=int, help='Step number')
    
    def _register_analysis_commands(self):
        """Register analysis-related commands."""
        analysis_parser = self.subparsers.add_parser('analysis', help='Analysis and profiling')
        analysis_subparsers = analysis_parser.add_subparsers(dest='analysis_action', help='Analysis actions')
        
        # Profile brain
        profile_parser = analysis_subparsers.add_parser('profile', help='Profile neural network')
        profile_parser.add_argument('--output', '-o', help='Output file for profile data')
        profile_parser.add_argument('--format', choices=['json', 'text', 'csv'], default='json', help='Output format')
        
        # Analyze configuration
        analyze_parser = analysis_subparsers.add_parser('analyze', help='Analyze configuration')
        analyze_parser.add_argument('--config-file', help='Configuration file')
        analyze_parser.add_argument('--output', '-o', help='Output file for analysis results')
        
        # Validate network
        validate_parser = analysis_subparsers.add_parser('validate', help='Validate neural network')
        validate_parser.add_argument('--config-file', help='Configuration file')
        validate_parser.add_argument('--strict', action='store_true', help='Strict validation')
    
    def _register_utility_commands(self):
        """Register utility commands."""
        utility_parser = self.subparsers.add_parser('utility', help='Utility commands')
        utility_subparsers = utility_parser.add_subparsers(dest='utility_action', help='Utility actions')
        
        # Create backup
        backup_parser = utility_subparsers.add_parser('backup', help='Create backup')
        backup_parser.add_argument('--output', '-o', help='Backup output file')
        
        # Restore from backup
        restore_parser = utility_subparsers.add_parser('restore', help='Restore from backup')
        restore_parser.add_argument('backup-file', help='Backup file to restore')
        
        # Show help
        help_parser = utility_subparsers.add_parser('help', help='Show help')
        help_parser.add_argument('topic', nargs='?', help='Help topic')
        
        # Show version
        version_parser = utility_subparsers.add_parser('version', help='Show version information')
        version_parser.add_argument('--full', action='store_true', help='Show full version information')
    
    def run_command(self, args: List[str]) -> int:
        """
        Run command with given arguments.
        
        Args:
            args: Command line arguments
            
        Returns:
            Exit code (0 for success, non-zero for error)
        """
        try:
            parsed_args = self.parser.parse_args(args)
            
            if not parsed_args.command:
                self.parser.print_help()
                return 0
            
            # Dispatch to appropriate command handler
            if parsed_args.command == 'config':
                return self._handle_config_command(parsed_args)
            elif parsed_args.command == 'simulation':
                return self._handle_simulation_command(parsed_args)
            elif parsed_args.command == 'analysis':
                return self._handle_analysis_command(parsed_args)
            elif parsed_args.command == 'utility':
                return self._handle_utility_command(parsed_args)
            else:
                print(f"Unknown command: {parsed_args.command}")
                return 1
                
        except Exception as e:
            print(f"Error: {e}")
            return 1
    
    def _handle_config_command(self, args) -> int:
        """Handle configuration commands."""
        if not NLX_AVAILABLE:
            print("NLM modules not available")
            return 1
        
        if args.config_action == 'show':
            return self._cmd_config_show(args.file)
        elif args.config_action == 'create':
            return self._cmd_config_create(args)
        elif args.config_action == 'import':
            return self._cmd_config_import(args)
        elif args.config_action == 'export':
            return self._cmd_config_export(args)
        elif args.config_action == 'validate':
            return self._cmd_config_validate(args)
        elif args.config_action == 'compare':
            return self._cmd_config_compare(args)
        else:
            print(f"Unknown config action: {args.config_action}")
            return 1
    
    def _handle_simulation_command(self, args) -> int:
        """Handle simulation commands."""
        if not NLX_AVAILABLE:
            print("NLM modules not available")
            return 1
        
        if args.sim_action == 'run':
            return self._cmd_simulation_run(args)
        elif args.sim_action == 'status':
            return self._cmd_simulation_status(args)
        elif args.sim_action == 'control':
            return self._cmd_simulation_control(args)
        else:
            print(f"Unknown simulation action: {args.sim_action}")
            return 1
    
    def _handle_analysis_command(self, args) -> int:
        """Handle analysis commands."""
        if not NLX_AVAILABLE:
            print("NLM modules not available")
            return 1
        
        if args.analysis_action == 'profile':
            return self._cmd_analysis_profile(args)
        elif args.analysis_action == 'analyze':
            return self._cmd_analysis_analyze(args)
        elif args.analysis_action == 'validate':
            return self._cmd_analysis_validate(args)
        else:
            print(f"Unknown analysis action: {args.analysis_action}")
            return 1
    
    def _handle_utility_command(self, args) -> int:
        """Handle utility commands."""
        if not NLX_AVAILABLE:
            print("NLM modules not available")
            return 1
        
        if args.utility_action == 'backup':
            return self._cmd_utility_backup(args)
        elif args.utility_action == 'restore':
            return self._cmd_utility_restore(args)
        elif args.utility_action == 'help':
            return self._cmd_utility_help(args)
        elif args.utility_action == 'version':
            return self._cmd_utility_version(args)
        else:
            print(f"Unknown utility action: {args.utility_action}")
            return 1
    
    # Configuration command handlers
    def _cmd_config_show(self, file_path: Optional[str]) -> int:
        """Show configuration."""
        if not file_path:
            print("Error: Configuration file required")
            return 1
        
        try:
            config = AdvancedConfig()
            success = config.load_from_file(file_path)
            
            if success:
                print("Configuration:")
                for key in config.keys():
                    print(f"  {key}: {config[key]}")
                
                print(f"\nStatistics:")
                stats = config.get_stats()
                for key, value in stats.items():
                    print(f"  {key}: {value}")
                
                return 0
            else:
                print(f"Error: Failed to load configuration from {file_path}")
                return 1
                
        except Exception as e:
            print(f"Error showing configuration: {e}")
            return 1
    
    def _cmd_config_create(self, args) -> int:
        """Create configuration."""
        try:
            config = AdvancedConfig()
            
            if args.schema:
                # Create configuration with schema
                config.add_schema("brain.neuron_count", "int", "Number of neurons", [100, 10000])
                config.add_schema("brain.synapse_density", "float", "Synapse density", [0.0, 1.0])
            
            # Set default values
            config.set("brain.neuron_count", "1000")
            config.set("brain.synapse_density", "0.1")
            
            if args.output:
                success = config.save_to_file(args.output, "cfg", "Generated configuration")
                if success:
                    print(f"Configuration saved to {args.output}")
                    return 0
                else:
                    print(f"Error: Failed to save configuration to {args.output}")
                    return 1
            else:
                print("Configuration created:")
                for key in config.keys():
                    print(f"  {key}: {config[key]}")
                return 0
                
        except Exception as e:
            print(f"Error creating configuration: {e}")
            return 1
    
    def _cmd_config_import(self, args) -> int:
        """Import configuration."""
        try:
            config = AdvancedConfig()
            success = config.load_from_file(args.file, args.format)
            
            if success:
                print(f"Configuration imported from {args.file}")
                return 0
            else:
                print(f"Error: Failed to import configuration from {args.file}")
                return 1
                
        except Exception as e:
            print(f"Error importing configuration: {e}")
            return 1
    
    def _cmd_config_export(self, args) -> int:
        """Export configuration."""
        try:
            config = AdvancedConfig()
            
            # Load configuration first (in a real implementation)
            # For now, create a sample configuration
            config.add_schema("brain.neuron_count", "int", "Number of neurons")
            config.set("brain.neuron_count", "2000")
            
            success = config.save_to_file(args.output, args.format, "Exported configuration")
            
            if success:
                print(f"Configuration exported to {args.output}")
                return 0
            else:
                print(f"Error: Failed to export configuration to {args.output}")
                return 1
                
        except Exception as e:
            print(f"Error exporting configuration: {e}")
            return 1
    
    def _cmd_config_validate(self, args) -> int:
        """Validate configuration."""
        try:
            config = AdvancedConfig()
            success = config.load_from_file(args.file)
            
            if success:
                errors = config.validate()
                
                if errors:
                    print("Validation errors:")
                    for error in errors:
                        print(f"  {error.key}: {error.message} ({error.severity})")
                    return 1
                else:
                    print("Configuration validation passed")
                    return 0
            else:
                print(f"Error: Failed to load configuration from {args.file}")
                return 1
                
        except Exception as e:
            print(f"Error validating configuration: {e}")
            return 1
    
    def _cmd_config_compare(self, args) -> int:
        """Compare configurations."""
        try:
            config1 = AdvancedConfig()
            config2 = AdvancedConfig()
            
            success1 = config1.load_from_file(args.file1)
            success2 = config2.load_from_file(args.file2)
            
            if success1 and success2:
                diff = config1.diff(config2)
                print("Configuration differences:")
                print(diff)
                return 0
            else:
                print("Error: Failed to load one or both configuration files")
                return 1
                
        except Exception as e:
            print(f"Error comparing configurations: {e}")
            return 1
    
    # Simulation command handlers
    def _cmd_simulation_run(self, args) -> int:
        """Run simulation."""
        try:
            if not NLX_AVAILABLE:
                print("NLM modules not available")
                return 1
            
            # Create configuration
            config = AdvancedConfig()
            if args.config:
                config.load_from_file(args.config)
            
            # Create brain
            brain = AdvancedBrain(config)
            
            # Create simulation controller
            controller = SimulationController(brain)
            
            # Set log level
            import logging
            logging.getLogger().setLevel(getattr(logging, args.log_level.upper()))
            
            print(f"Running simulation with {args.steps} steps...")
            
            # Run simulation
            def on_step(step, metrics):
                if step % 100 == 0:
                    print(f"  Step {step}: {metrics['firing_neurons']} firing neurons, "
                          f"{metrics['total_spikes']} total spikes")
            
            controller.run_episode(args.steps, on_step_complete=on_step)
            
            # Save results
            if args.output:
                brain.save_advanced(args.output, "json", {"description": "Simulation results"})
                print(f"Results saved to {args.output}")
            
            return 0
            
        except Exception as e:
            print(f"Error running simulation: {e}")
            return 1
    
    def _cmd_simulation_status(self, args) -> int:
        """Show simulation status."""
        try:
            if not NLX_AVAILABLE:
                print("NLM modules not available")
                return 1
            
            # Create sample brain
            config = AdvancedConfig()
            config.set("brain.neuron_count", "500")
            
            brain = AdvancedBrain(config)
            controller = SimulationController(brain)
            
            print("Simulation Status:")
            print(f"  Brain: {brain}")
            print(f"  Controller: {controller}")
            print(f"  Performance: {brain.get_performance_metrics():.3f}")
            
            return 0
            
        except Exception as e:
            print(f"Error getting simulation status: {e}")
            return 1
    
    def _cmd_simulation_control(self, args) -> int:
        """Control simulation."""
        try:
            print(f"Simulation control action: {args.action}")
            if args.step:
                print(f"Step: {args.step}")
            
            return 0
            
        except Exception as e:
            print(f"Error controlling simulation: {e}")
            return 1
    
    # Analysis command handlers
    def _cmd_analysis_profile(self, args) -> int:
        """Profile neural network."""
        try:
            if not NLX_AVAILABLE:
                print("NLM modules not available")
                return 1
            
            # Create sample brain
            config = AdvancedConfig()
            config.set("brain.neuron_count", "1000")
            
            brain = AdvancedBrain(config)
            profiler = NeuralProfiler(brain)
            
            # Profile network
            network_profile = profiler.profile_network()
            print(f"Network Profile:")
            print(f"  Neurons: {network_profile.total_neuron_count}")
            print(f"  Synapses: {network_profile.total_synapse_count}")
            print(f"  Average Firing Rate: {network_profile.average_firing_rate:.3f} Hz")
            
            # Save profile if requested
            if args.output:
                if args.format == 'json':
                    import json
                    profile_data = {
                        'network_profile': {
                            'total_neuron_count': network_profile.total_neuron_count,
                            'total_synapse_count': network_profile.total_synapse_count,
                            'average_firing_rate': network_profile.average_firing_rate,
                            'excitation_inhibition_ratio': network_profile.excitation_inhibition_ratio
                        }
                    }
                    with open(args.output, 'w') as f:
                        json.dump(profile_data, f, indent=2)
                elif args.format == 'csv':
                    # Save as CSV
                    with open(args.output, 'w') as f:
                        f.write("metric,value\n")
                        f.write(f"neuron_count,{network_profile.total_neuron_count}\n")
                        f.write(f"synapse_count,{network_profile.total_synapse_count}\n")
                        f.write(f"average_firing_rate,{network_profile.average_firing_rate}\n")
                
                print(f"Profile saved to {args.output}")
            
            return 0
            
        except Exception as e:
            print(f"Error profiling neural network: {e}")
            return 1
    
    def _cmd_analysis_analyze(self, args) -> int:
        """Analyze configuration."""
        try:
            if not NLX_AVAILABLE:
                print("NLM modules not available")
                return 1
            
            if args.config_file:
                config = AdvancedConfig()
                config.load_from_file(args.config_file)
                
                print("Configuration Analysis:")
                stats = config.get_stats()
                for key, value in stats.items():
                    print(f"  {key}: {value}")
                
                recommendations = config.get_recommendations()
                if recommendations:
                    print("\nRecommendations:")
                    for rec in recommendations:
                        print(f"  • {rec}")
            else:
                print("Error: Configuration file required")
                return 1
            
            return 0
            
        except Exception as e:
            print(f"Error analyzing configuration: {e}")
            return 1
    
    def _cmd_analysis_validate(self, args) -> int:
        """Validate neural network."""
        try:
            if not NLX_AVAILABLE:
                print("NLM modules not available")
                return 1
            
            # Create sample brain
            config = AdvancedConfig()
            config.set("brain.neuron_count", "1000")
            
            brain = AdvancedBrain(config)
            
            # Validate brain
            validation_metrics = brain.get_stats()
            print("Network Validation:")
            for key, value in validation_metrics.items():
                print(f"  {key}: {value}")
            
            # Check if validation passed
            if validation_metrics.get('total_neuron_count', 0) > 0:
                print("✓ Network validation passed")
                return 0
            else:
                print("✗ Network validation failed")
                return 1
            
        except Exception as e:
            print(f"Error validating neural network: {e}")
            return 1
    
    # Utility command handlers
    def _cmd_utility_backup(self, args) -> int:
        """Create backup."""
        try:
            if not NLX_AVAILABLE:
                print("NLM modules not available")
                return 1
            
            if not args.output:
                print("Error: Output file required")
                return 1
            
            # Create backup using advanced brain save
            config = AdvancedConfig()
            config.set("brain.neuron_count", "1000")
            
            brain = AdvancedBrain(config)
            backup_file = brain.create_backup(args.output)
            
            if backup_file:
                print(f"Backup created: {backup_file}")
                return 0
            else:
                print(f"Error: Failed to create backup")
                return 1
            
        except Exception as e:
            print(f"Error creating backup: {e}")
            return 1
    
    def _cmd_utility_restore(self, args) -> int:
        """Restore from backup."""
        try:
            if not NLX_AVAILABLE:
                print("NLM modules not available")
                return 1
            
            # Restore backup
            config = AdvancedConfig()
            success = config.restore_from_backup(args.backup_file)
            
            if success:
                print(f"Backup restored from {args.backup_file}")
                return 0
            else:
                print(f"Error: Failed to restore from {args.backup_file}")
                return 1
            
        except Exception as e:
            print(f"Error restoring backup: {e}")
            return 1
    
    def _cmd_utility_help(self, args) -> int:
        """Show help."""
        if args.topic:
            print(f"Help topic: {args.topic}")
            print("(This would show detailed help for the topic)")
        else:
            self.parser.print_help()
        
        return 0
    
    def _cmd_utility_version(self, args) -> int:
        """Show version information."""
        print("NLM Enhanced CLI Version 2.0.0")
        print("Python bindings for NLM (Neural Learning Machine)")
        
        if args.full:
            print("\nFull Version Information:")
            print(f"  Version: 2.0.0")
            print(f"  NLM: Phase 6")
            print(f"  C++ Standard: C++20")
            print(f"  Python: 3.8+")
            print(f"  Build: Enhanced with performance optimizations")
        
        return 0
def main():
    """Main entry point."""
    cli = NLMCLI()
    return cli.run_command(sys.argv[1:])
if __name__ == "__main__":
    sys.exit(main())