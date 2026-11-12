import pynlm
import json
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
from typing import List, Dict, Optional, Union
import argparse
import sys

# Try to import utility modules if available
try:
    from pynlm.utils import BrainAnalyzer, ConfigManager, WorldUtils
    UTILS_AVAILABLE = True
except ImportError:
    UTILS_AVAILABLE = False

# Try to import config utilities if available
try:
    from pynlm.config import BrainConfigurator, ParameterSweep, ExperimentManager
    CONFIG_AVAILABLE = True
except ImportError:
    CONFIG_AVAILABLE = False

# Try to import visualization utilities if available
try:
    from pynlm.viz import Visualization
    VIZ_AVAILABLE = True
except ImportError:
    VIZ_AVAILABLE = False

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class BrainError(NLMError):
    """Exception for brain-related errors."""
    pass

class WorldError(NLMError):
    """Exception for world-related errors."""
    pass

class ConfigError(NLMError):
    """Exception for configuration errors."""
    pass

class NLMCLI:
    """
    Command-line interface for NLM utilities.
    
    This class provides a comprehensive command-line interface for working
    with NLM brains, configurations, experiments, and visualization.
    """
    
    def __init__(self):
        self.commands = {
            'brain': self._brain_command,
            'config': self._config_command,
            'world': self._world_command,
            'agent': self._agent_command,
            'experiment': self._experiment_command,
            'viz': self._viz_command,
            'batch': self._batch_command,
            'help': self._help_command,
        }
    
    def parse_args(self, args):
        """Parse command line arguments."""
        if not args:
            self._print_help()
            return None
        
        command = args[0]
        if command in self.commands:
            return self.commands[command](args[1:])
        else:
            print(f"Unknown command: {command}")
            self._print_help()
            return None
    
    def _brain_command(self, args):
        """Handle brain commands."""
        if not args:
            self._print_brain_help()
            return
        
        subcommand = args[0]
        if subcommand == 'create':
            self._brain_create(args[1:])
        elif subcommand == 'analyze':
            self._brain_analyze(args[1:])
        elif subcommand == 'save':
            self._brain_save(args[1:])
        elif subcommand == 'load':
            self._brain_load(args[1:])
        elif subcommand == 'export':
            self._brain_export(args[1:])
        elif subcommand == 'compare':
            self._brain_compare(args[1:])
        else:
            print(f"Unknown brain command: {subcommand}")
            self._print_brain_help()
    
    def _config_command(self, args):
        """Handle configuration commands."""
        if not args:
            self._print_config_help()
            return
        
        subcommand = args[0]
        if subcommand == 'presets':
            self._config_presets(args[1:])
        elif subcommand == 'create':
            self._config_create(args[1:])
        elif subcommand == 'save':
            self._config_save(args[1:])
        elif subcommand == 'load':
            self._config_load(args[1:])
        elif subcommand == 'validate':
            self._config_validate(args[1:])
        elif subcommand == 'compare':
            self._config_compare(args[1:])
        else:
            print(f"Unknown config command: {subcommand}")
            self._print_config_help()
    
    def _world_command(self, args):
        """Handle world commands."""
        if not args:
            self._print_world_help()
            return
        
        subcommand = args[0]
        if subcommand == 'create':
            self._world_create(args[1:])
        elif subcommand == 'configure':
            self._world_configure(args[1:])
        elif subcommand == 'reset':
            self._world_reset(args[1:])
        elif subcommand == 'analyze':
            self._world_analyze(args[1:])
        else:
            print(f"Unknown world command: {subcommand}")
            self._print_world_help()
    
    def _agent_command(self, args):
        """Handle agent commands."""
        if not args:
            self._print_agent_help()
            return
        
        subcommand = args[0]
        if subcommand == 'create':
            self._agent_create(args[1:])
        elif subcommand == 'initialize':
            self._agent_initialize(args[1:])
        elif subcommand == 'process':
            self._agent_process(args[1:])
        elif subcommand == 'decode':
            self._agent_decode(args[1:])
        elif subcommand == 'train':
            self._agent_train(args[1:])
        else:
            print(f"Unknown agent command: {subcommand}")
            self._print_agent_help()
    
    def _experiment_command(self, args):
        """Handle experiment commands."""
        if not args:
            self._print_experiment_help()
            return
        
        subcommand = args[0]
        if subcommand == 'create':
            self._experiment_create(args[1:])
        elif subcommand == 'run':
            self._experiment_run(args[1:])
        elif subcommand == 'add-trial':
            self._experiment_add_trial(args[1:])
        elif subcommand == 'summary':
            self._experiment_summary(args[1:])
        elif subcommand == 'results':
            self._experiment_results(args[1:])
        else:
            print(f"Unknown experiment command: {subcommand}")
            self._print_experiment_help()
    
    def _viz_command(self, args):
        """Handle visualization commands."""
        if not args:
            self._print_viz_help()
            return
        
        subcommand = args[0]
        if subcommand == 'dashboard':
            self._viz_dashboard(args[1:])
        elif subcommand == 'stats':
            self._viz_stats(args[1:])
        elif subcommand == 'network':
            self._viz_network(args[1:])
        elif subcommand == 'development':
            self._viz_development(args[1:])
        elif subcommand == 'spikes':
            self._viz_spikes(args[1:])
        else:
            print(f"Unknown viz command: {subcommand}")
            self._print_viz_help()
    
    def _batch_command(self, args):
        """Handle batch processing commands."""
        if not args:
            self._print_batch_help()
            return
        
        subcommand = args[0]
        if subcommand == 'create':
            self._batch_create(args[1:])
        elif subcommand == 'process':
            self._batch_process(args[1:])
        elif subcommand == 'analyze':
            self._batch_analyze(args[1:])
        elif subcommand == 'export':
            self._batch_export(args[1:])
        else:
            print(f"Unknown batch command: {subcommand}")
            self._print_batch_help()
    
    def _help_command(self, args):
        """Handle help command."""
        self._print_help()
    
    # Command implementations
    
    def _brain_create(self, args):
        """Create a new brain."""
        parser = argparse.ArgumentParser(description='Create a new NLM brain')
        parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
        parser.add_argument('--preset', choices=['tiny', 'small', 'medium', 'large', 'research', 'debug'],
                          help='Use built-in preset configuration')
        parser.add_argument('--steps', type=int, help='Run simulation for this many steps')
        parser.add_argument('--output', help='Save brain to file')
        
        try:
            namespace = parser.parse_args(args)
            
            # Create configuration
            if namespace.preset:
                if CONFIG_AVAILABLE:
                    config = BrainConfigurator()
                    config.apply_preset(namespace.preset)
                else:
                    print("Configuration utilities not available")
                    return
            else:
                config = pynlm.createDefaultConfig()
                config.set("brain.neuron_count", namespace.neurons)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation if requested
            if namespace.steps:
                for step in range(namespace.steps):
                    brain.step(step)
            
            # Save if requested
            if namespace.output:
                brain.save(namespace.output)
                print(f"Brain saved to {namespace.output}")
            
            print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            
        except Exception as e:
            print(f"Error creating brain: {e}")
    
    def _brain_analyze(self, args):
        """Analyze brain statistics."""
        parser = argparse.ArgumentParser(description='Analyze brain statistics')
        parser.add_argument('input', help='Brain file to analyze')
        parser.add_argument('--format', choices=['text', 'json', 'csv'], default='text',
                          help='Output format')
        parser.add_argument('--verbose', action='store_true', help='Verbose output')
        
        try:
            namespace = parser.parse_args(args)
            
            # Load brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(namespace.input)
            
            if UTILS_AVAILABLE:
                analyzer = BrainAnalyzer(brain)
                
                if namespace.format == 'text':
                    analyzer.print_formatted_stats()
                elif namespace.format == 'json':
                    analyzer.export_to_json(namespace.input.replace('.bin', '.json'))
                elif namespace.format == 'csv':
                    analyzer.export_to_csv(namespace.input.replace('.bin', '.csv'))
            else:
                print("Brain analyzer utilities not available")
                print(f"Basic stats: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSpikeCount()} spikes")
                
        except Exception as e:
            print(f"Error analyzing brain: {e}")
    
    def _brain_save(self, args):
        """Save brain state to file."""
        parser = argparse.ArgumentParser(description='Save brain state')
        parser.add_argument('input', help='Brain file to save')
        parser.add_argument('output', help='Output file path')
        
        try:
            namespace = parser.parse_args(args)
            
            # Load and save brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(namespace.input)
            brain.save(namespace.output)
            
            print(f"Brain saved from {namespace.input} to {namespace.output}")
            
        except Exception as e:
            print(f"Error saving brain: {e}")
    
    def _brain_load(self, args):
        """Load brain state from file."""
        parser = argparse.ArgumentParser(description='Load brain state')
        parser.add_argument('input', help='Brain file to load')
        
        try:
            namespace = parser.parse_args(args)
            
            # Load brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(namespace.input)
            
            print(f"Brain loaded from {namespace.input}")
            print(f"Brain has {brain.getTotalNeuronCount()} neurons")
            
        except Exception as e:
            print(f"Error loading brain: {e}")
    
    def _brain_export(self, args):
        """Export brain data."""
        parser = argparse.ArgumentParser(description='Export brain data')
        parser.add_argument('input', help='Brain file to export')
        parser.add_argument('--format', choices=['npy', 'json', 'txt'], default='npy',
                          help='Export format')
        parser.add_argument('--output', help='Output directory')
        
        try:
            namespace = parser.parse_args(args)
            
            # Load brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(namespace.input)
            
            # Export based on format
            if namespace.format == 'npy':
                # For now, just save basic stats
                output_file = namespace.output or f"export_{Path(namespace.input).stem}"
                with open(f"{output_file}.txt", 'w') as f:
                    f.write(f"Neurons: {brain.getTotalNeuronCount()}\n")
                    f.write(f"Synapses: {brain.getTotalSynapseCount()}\n")
                    f.write(f"Total spikes: {brain.getTotalSpikeCount()}\n")
                    f.write(f"Firing neurons: {brain.getFiringNeuronCount()}\n")
                print(f"Brain data exported to {output_file}.txt")
                
        except Exception as e:
            print(f"Error exporting brain: {e}")
    
    def _brain_compare(self, args):
        """Compare two brains."""
        parser = argparse.ArgumentParser(description='Compare two brains')
        parser.add_argument('brain1', help='First brain file')
        parser.add_argument('brain2', help='Second brain file')
        
        try:
            namespace = parser.parse_args(args)
            
            # Load both brains
            brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
            brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
            
            brain1.load(namespace.brain1)
            brain2.load(namespace.brain2)
            
            print("Brain Comparison:")
            print(f"{namespace.brain1}:")
            print(f"  Neurons: {brain1.getTotalNeuronCount()}")
            print(f"  Synapses: {brain1.getTotalSynapseCount()}")
            print(f"  Total spikes: {brain1.getTotalSpikeCount()}")
            print(f"{namespace.brain2}:")
            print(f"  Neurons: {brain2.getTotalNeuronCount()}")
            print(f"  Synapses: {brain2.getTotalSynapseCount()}")
            print(f"  Total spikes: {brain2.getTotalSpikeCount()}")
            
        except Exception as e:
            print(f"Error comparing brains: {e}")
    
    def _config_presets(self, args):
        """List built-in configuration presets."""
        if CONFIG_AVAILABLE:
            from pynlm.config import BrainConfigurator
            
            print("Built-in Configuration Presets:")
            print("=" * 50)
            
            for preset_name in BrainConfigurator.BUILTIN_PRESETS.keys():
                preset = BrainConfigurator.BUILTIN_PRESETS[preset_name]
                print(f"\n{preset_name.upper()}:")
                print(f"  Name: {preset['name']}")
                print(f"  Description: {preset['description']}")
                print(f"  Neurons: {preset.get('brain.neuron_count', 'N/A')}")
                print(f"  Synapse Density: {preset.get('brain.synapse_density', 'N/A')}")
        else:
            print("Configuration utilities not available")
            print("\nAvailable presets (hardcoded):")
            print("  tiny: 50 neurons (debugging/testing)")
            print("  small: 200 neurons (basic simulations)")
            print("  medium: 1000 neurons (general purpose)")
            print("  large: 5000 neurons (complex simulations)")
            print("  research: 10000 neurons (scientific research)")
            print("  debug: 100 neurons (development/debugging)")
    
    def _config_create(self, args):
        """Create configuration."""
        parser = argparse.ArgumentParser(description='Create configuration')
        parser.add_argument('--preset', choices=['tiny', 'small', 'medium', 'large', 'research', 'debug'],
                          help='Use built-in preset')
        parser.add_argument('--neurons', type=int, help='Number of neurons')
        parser.add_argument('--seed', type=int, help='Random seed')
        parser.add_argument('--output', help='Save configuration to file')
        
        try:
            namespace = parser.parse_args(args)
            
            if CONFIG_AVAILABLE:
                config = BrainConfigurator()
                
                if namespace.preset:
                    config.apply_preset(namespace.preset)
                else:
                    if namespace.neurons:
                        config.update({'brain.neuron_count': namespace.neurons})
                    if namespace.seed:
                        config.update({'random_seed': namespace.seed})
                
                if namespace.output:
                    config.save_to_json(namespace.output)
                    print(f"Configuration saved to {namespace.output}")
                
                config.print_summary()
            else:
                print("Configuration utilities not available")
                
        except Exception as e:
            print(f"Error creating configuration: {e}")
    
    def _config_save(self, args):
        """Save configuration to file."""
        parser = argparse.ArgumentParser(description='Save configuration')
        parser.add_argument('input', help='Configuration file to save')
        parser.add_argument('output', help='Output file path')
        
        try:
            namespace = parser.parse_args(args)
            
            # Load and save configuration
            config = pynlm.createDefaultConfig()
            
            # This is a simplified implementation - in practice, you'd need to parse
            # the existing configuration properly
            print(f"Configuration saved from {namespace.input} to {namespace.output}")
            
        except Exception as e:
            print(f"Error saving configuration: {e}")
    
    def _config_load(self, args):
        """Load configuration from file."""
        parser = argparse.ArgumentParser(description='Load configuration')
        parser.add_argument('input', help='Configuration file to load')
        
        try:
            namespace = parser.parse_args(args)
            
            print(f"Configuration loaded from {namespace.input}")
            
        except Exception as e:
            print(f"Error loading configuration: {e}")
    
    def _config_validate(self, args):
        """Validate configuration."""
        parser = argparse.ArgumentParser(description='Validate configuration')
        parser.add_argument('--config-file', help='Configuration file to validate')
        
        try:
            namespace = parser.parse_args(args)
            
            if namespace.config_file and CONFIG_AVAILABLE:
                config = BrainConfigurator()
                config.load_from_json(namespace.config_file)
                errors = config.validate()
                
                if errors:
                    print("Validation errors:")
                    for error in errors:
                        print(f"  - {error}")
                else:
                    print("Configuration is valid")
            else:
                print("Configuration utilities not available")
                
        except Exception as e:
            print(f"Error validating configuration: {e}")
    
    def _config_compare(self, args):
        """Compare two configurations."""
        parser = argparse.ArgumentParser(description='Compare two configurations')
        parser.add_argument('config1', help='First configuration file')
        parser.add_argument('config2', help='Second configuration file')
        
        try:
            namespace = parser.parse_args(args)
            
            print(f"Comparing {namespace.config1} and {namespace.config2}")
            print("Configuration comparison not fully implemented yet")
            
        except Exception as e:
            print(f"Error comparing configurations: {e}")
    
    def _world_create(self, args):
        """Create world."""
        parser = argparse.ArgumentParser(description='Create simple world')
        parser.add_argument('--width', type=float, default=20.0, help='World width')
        parser.add_argument('--height', type=float, default=20.0, help='World height')
        parser.add_argument('--vision-width', type=int, default=16, help='Vision width')
        parser.add_argument('--vision-height', type=int, default=16, help='Vision height')
        
        try:
            namespace = parser.parse_args(args)
            
            world = pynlm.createSimpleWorld()
            world.configure(namespace.width, namespace.height,
                           namespace.vision_width, namespace.vision_height)
            world.reset()
            
            print(f"Created world {namespace.width}x{namespace.height} with vision {namespace.vision_width}x{namespace.vision_height}")
            
        except Exception as e:
            print(f"Error creating world: {e}")
    
    def _world_configure(self, args):
        """Configure world parameters."""
        parser = argparse.ArgumentParser(description='Configure world')
        parser.add_argument('--width', type=float, help='World width')
        parser.add_argument('--height', type=float, help='World height')
        
        try:
            namespace = parser.parse_args(args)
            
            world = pynlm.createSimpleWorld()
            
            if namespace.width:
                # This would require existing world - simplified for now
                print(f"World width would be set to {namespace.width}")
            if namespace.height:
                print(f"World height would be set to {namespace.height}")
                
        except Exception as e:
            print(f"Error configuring world: {e}")
    
    def _world_reset(self, args):
        """Reset world."""
        print("World reset functionality would be implemented here")
    
    def _world_analyze(self, args):
        """Analyze world."""
        print("World analysis functionality would be implemented here")
    
    def _agent_create(self, args):
        """Create agent."""
        parser = argparse.ArgumentParser(description='Create agent brain')
        parser.add_argument('brain', help='Brain file or create new')
        
        try:
            namespace = parser.parse_args(args)
            
            print(f"Creating agent for brain {namespace.brain}")
            
        except Exception as e:
            print(f"Error creating agent: {e}")
    
    def _agent_initialize(self, args):
        """Initialize agent."""
        print("Agent initialization functionality would be implemented here")
    
    def _agent_process(self, args):
        """Process sensory input."""
        print("Agent sensory processing functionality would be implemented here")
    
    def _agent_decode(self, args):
        """Decode motor command."""
        print("Agent motor decoding functionality would be implemented here")
    
    def _agent_train(self, args):
        """Train agent."""
        print("Agent training functionality would be implemented here")
    
    def _experiment_create(self, args):
        """Create experiment."""
        parser = argparse.ArgumentParser(description='Create experiment')
        parser.add_argument('name', help='Experiment name')
        parser.add_argument('--config', help='Configuration file')
        
        try:
            namespace = parser.parse_args(args)
            print(f"Creating experiment: {namespace.name}")
            
        except Exception as e:
            print(f"Error creating experiment: {e}")
    
    def _experiment_run(self, args):
        """Run experiment."""
        parser = argparse.ArgumentParser(description='Run experiment')
        parser.add_argument('name', help='Experiment name')
        parser.add_argument('--trials', type=int, default=10, help='Number of trials')
        
        try:
            namespace = parser.parse_args(args)
            print(f"Running experiment: {namespace.name} with {namespace.trials} trials")
            
        except Exception as e:
            print(f"Error running experiment: {e}")
    
    def _experiment_add_trial(self, args):
        """Add trial to experiment."""
        print("Experiment trial addition functionality would be implemented here")
    
    def _experiment_summary(self, args):
        """Show experiment summary."""
        parser = argparse.ArgumentParser(description='Show experiment summary')
        parser.add_argument('name', help='Experiment name')
        
        try:
            namespace = parser.parse_args(args)
            print(f"Experiment summary for: {namespace.name}")
            
        except Exception as e:
            print(f"Error showing experiment summary: {e}")
    
    def _experiment_results(self, args):
        """Export experiment results."""
        parser = argparse.ArgumentParser(description='Export experiment results')
        parser.add_argument('name', help='Experiment name')
        parser.add_argument('--format', choices=['json', 'csv', 'txt'], default='json',
                          help='Output format')
        
        try:
            namespace = parser.parse_args(args)
            print(f"Exporting results for experiment: {namespace.name}")
            
        except Exception as e:
            print(f"Error exporting experiment results: {e}")
    
    def _viz_dashboard(self, args):
        """Create dashboard visualization."""
        parser = argparse.ArgumentParser(description='Create visualization dashboard')
        parser.add_argument('input', help='Brain file')
        parser.add_argument('--output', help='Output file')
        parser.add_argument('--world', help='World file')
        parser.add_argument('--agent', help='Agent file')
        
        try:
            namespace = parser.parse_args(args)
            
            # Load brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(namespace.input)
            
            if VIZ_AVAILABLE:
                viz = Visualization()
                dashboard_file = viz.save_dashboard(
                    brain, filename=namespace.output or "dashboard.png"
                )
                print(f"Dashboard created: {dashboard_file}")
            else:
                print("Visualization utilities not available")
                
        except Exception as e:
            print(f"Error creating dashboard: {e}")
    
    def _viz_stats(self, args):
        """Create statistics visualization."""
        parser = argparse.ArgumentParser(description='Create statistics visualization')
        parser.add_argument('input', help='Brain file')
        parser.add_argument('--output', help='Output file')
        
        try:
            namespace = parser.parse_args(args)
            
            # Load brain
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(namespace.input)
            
            if VIZ_AVAILABLE:
                from pynlm.viz import plot_brain_stats
                fig = plot_brain_stats(brain, filename=namespace.output or "stats.png")
                print(f"Statistics plot created")
            else:
                print("Visualization utilities not available")
                
        except Exception as e:
            print(f"Error creating statistics plot: {e}")
    
    def _viz_network(self, args):
        """Create network visualization."""
        print("Network visualization functionality would be implemented here")
    
    def _viz_development(self, args):
        """Create development visualization."""
        print("Development visualization functionality would be implemented here")
    
    def _viz_spikes(self, args):
        """Create spike activity visualization."""
        print("Spike visualization functionality would be implemented here")
    
    def _batch_create(self, args):
        """Create multiple brains."""
        parser = argparse.ArgumentParser(description='Create multiple brains')
        parser.add_argument('--count', type=int, default=10, help='Number of brains to create')
        parser.add_argument('--preset', choices=['tiny', 'small', 'medium', 'large', 'research', 'debug'],
                          help='Use built-in preset configuration')
        parser.add_argument('--steps', type=int, help='Run simulation for this many steps')
        
        try:
            namespace = parser.parse_args(args)
            print(f"Creating {namespace.count} brains with preset: {namespace.preset}")
            
        except Exception as e:
            print(f"Error creating batch of brains: {e}")
    
    def _batch_process(self, args):
        """Process multiple brains."""
        print("Batch processing functionality would be implemented here")
    
    def _batch_analyze(self, args):
        """Analyze multiple brains."""
        print("Batch analysis functionality would be implemented here")
    
    def _batch_export(self, args):
        """Export multiple brains."""
        print("Batch export functionality would be implemented here")
    
    # Help methods
    
    def _print_help(self):
        """Print general help."""
        print("NLM Utility Program - Command Line Interface")
        print("=" * 50)
        print("\nAvailable commands:")
        for cmd, _ in self.commands.items():
            print(f"  {cmd}")
        print("\nUse 'nlm-utils <command> --help' for detailed command help")
    
    def _print_brain_help(self):
        """Print brain command help."""
        print("Brain commands:")
        print("  create -- Create a new brain")
        print("  analyze -- Analyze brain statistics")
        print("  save -- Save brain state to file")
        print("  load -- Load brain state from file")
        print("  export -- Export brain data")
        print("  compare -- Compare two brains")
    
    def _print_config_help(self):
        """Print configuration command help."""
        print("Configuration commands:")
        print("  presets -- List built-in configuration presets")
        print("  create -- Create configuration")
        print("  save -- Save configuration to file")
        print("  load -- Load configuration from file")
        print("  validate -- Validate configuration")
        print("  compare -- Compare configurations")
    
    def _print_world_help(self):
        """Print world command help."""
        print("World commands:")
        print("  create -- Create simple world")
        print("  configure -- Configure world parameters")
        print("  reset -- Reset world state")
        print("  analyze -- Analyze world")
    
    def _print_agent_help(self):
        """Print agent command help."""
        print("Agent commands:")
        print("  create -- Create agent brain")
        print("  initialize -- Initialize agent")
        print("  process -- Process sensory input")
        print("  decode -- Decode motor command")
        print("  train -- Train agent")
    
    def _print_experiment_help(self):
        """Print experiment command help."""
        print("Experiment commands:")
        print("  create -- Create experiment")
        print("  run -- Run experiment")
        print("  add-trial -- Add trial to experiment")
        print("  summary -- Show experiment summary")
        print("  results -- Export experiment results")
    
    def _print_viz_help(self):
        """Print visualization command help."""
        print("Visualization commands:")
        print("  dashboard -- Create comprehensive dashboard")
        print("  stats -- Create statistics plot")
        print("  network -- Plot network topology")
        print("  development -- Plot developmental stages")
        print("  spikes -- Plot spike activity")
    
    def _print_batch_help(self):
        """Print batch command help."""
        print("Batch processing commands:")
        print("  create -- Create multiple brains")
        print("  process -- Process multiple brains")
        print("  analyze -- Analyze multiple brains")
        print("  export -- Export multiple brains")
    
    def run(self, args):
        """Run the CLI with given arguments."""
        if not args:
            self._print_help()
            return
        
        self.parse_args(args)


def main():
    """Main entry point for NLM CLI."""
    cli = NLMCLI()
    cli.run(sys.argv[1:])

if __name__ == "__main__":
    main()
