#!/usr/bin/env python3
"""
NLM Command Line Interface

This script provides a command-line interface for NLM (Neural Learning Machine) operations.
It supports various operations including simulation execution, configuration management,
and visualization.

Usage:
    python -m nlm.cli <command> [options]

Commands:
    simulate    Run a simulation with specified configuration
    config      Manage NLM configuration files
    visualize   Generate visualizations from simulation data
    debug       Run debugging and analysis tools
    monitor     Monitor simulation performance
    export      Export simulation data
    help        Show help information

Examples:
    python -m nlm.cli simulate --config my_config.json --steps 1000
    python -m nlm.cli config list
    python -m nlm.cli visualize --input simulation_data.json --output report.png
    python -m nlm.cli debug --log-level DEBUG --output debug.log
"""

import pynlm
import json
import time
import logging
import argparse
from pathlib import Path
import sys
import os
from typing import Dict, Any, Optional
import threading
import queue

class NLMCLI:
    """
    NLM Command Line Interface class.
    """
    
    def __init__(self, output_dir: str = "nlm_output"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        
        # Setup logging
        self.setup_logging()
        
        # Initialize NLM tools
        self.initialize_tools()
        
        self.logger = logging.getLogger('NLM_CLI')
    
    def setup_logging(self):
        """Set up logging configuration."""
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler(self.output_dir / "cli.log"),
                logging.StreamHandler()
            ]
        )
    
    def initialize_tools(self):
        """Initialize NLM tools."""
        try:
            from nlm.visualization import NLMTools
            self.tools = NLMTools(str(self.output_dir))
        except ImportError:
            self.tools = None
            self.logger.warning("Visualization tools not available")
    
    def run_simulation(self, config_file: str = None, steps: int = 100, 
                      world_config: str = None, agent_config: str = None,
                      output_file: str = None):
        """
        Run a simulation with specified configuration.
        
        Args:
            config_file: Path to configuration file
            steps: Number of simulation steps
            world_config: World configuration parameters
            agent_config: Agent configuration parameters
            output_file: Output file for simulation data
        """
        self.logger.info(f"Starting simulation with {steps} steps")
        
        # Load or create configuration
        if config_file and Path(config_file).exists():
            config = pynlm.createDefaultConfig()
            try:
                # Try to load JSON configuration
                with open(config_file, 'r') as f:
                    config_data = json.load(f)
                
                # Set configuration values
                for key, value in config_data.items():
                    if isinstance(value, (int, float, bool, str)):
                        config.set(key, value)
                    elif isinstance(value, list):
                        config.set(key, value)
                
                self.logger.info(f"Loaded configuration from {config_file}")
            except Exception as e:
                self.logger.error(f"Error loading configuration file: {e}")
                return False
        else:
            # Create default configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            config.set("brain.synapse_density", 0.1)
            self.logger.info("Using default configuration")
        
        # Create world configuration
        if world_config:
            try:
                with open(world_config, 'r') as f:
                    world_data = json.load(f)
                self.logger.info(f"Loaded world configuration from {world_config}")
            except Exception as e:
                self.logger.error(f"Error loading world configuration: {e}")
                return False
        else:
            world_data = {}
        
        # Create agent configuration
        if agent_config:
            try:
                with open(agent_config, 'r') as f:
                    agent_data = json.load(f)
                self.logger.info(f"Loaded agent configuration from {agent_config}")
            except Exception as e:
                self.logger.error(f"Error loading agent configuration: {e}")
                return False
        else:
            agent_data = {}
        
        # Create brain and world
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world if world_config is provided
        world = None
        if world_data:
            try:
                world = pynlm.createSimpleWorld()
                if 'width' in world_data:
                    world.configure(
                        width=world_data['width'],
                        height=world_data.get('height', 20),
                        visionWidth=world_data.get('visionWidth', 8),
                        visionHeight=world_data.get('visionHeight', 8)
                    )
                world.reset()
                self.logger.info("World created and configured")
            except Exception as e:
                self.logger.error(f"Error creating world: {e}")
                return False
        
        # Create agent if agent_config is provided
        agent = None
        if agent_data:
            try:
                agent = pynlm.createAgentBrain(brain)
                if world:
                    agent.initialize(world)
                    
                    # Enable learning features based on configuration
                    agent.enableRewardModulation(agent_data.get('enable_reward', True))
                    agent.enableCuriosity(agent_data.get('enable_curiosity', True))
                    agent.enableDevelopment(agent_data.get('enable_development', True))
                self.logger.info("Agent created and configured")
            except Exception as e:
                self.logger.error(f"Error creating agent: {e}")
                return False
        
        # Run simulation
        simulation_data = {
            'config': {
                'neuron_count': config.get("brain.neuron_count", 0).value if config.has("brain.neuron_count") else 0,
                'synapse_density': config.get("brain.synapse_density", 0.0).value if config.has("brain.synapse_density") else 0.0,
            },
            'world_config': world_data,
            'agent_config': agent_data,
            'steps': steps,
            'neuron_activity': [],
            'metrics': []
        }
        
        for step in range(steps):
            try:
                # Update world if it exists
                if world:
                    world.update(0.1)
                    
                    # Get sensory input and process if agent exists
                    if agent:
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                        
                        # Get action and apply if agent exists
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                        
                        # Apply reward modulation
                        reward = agent_data.get('reward', 0.0) if agent_data else 0.0
                        agent.applyRewardModulation(reward, 0.0)
                        
                        # Update development
                        agent.updateDevelopment(0.1)
                
                # Brain thinking
                brain.step(step)
                
                # Record metrics
                step_data = {
                    'step': step,
                    'timestamp': time.time(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'avg_firing_rate': brain.getAverageFiringRate(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio(),
                    'developmental_stage': str(brain.getDevelopmentalStage()),
                }
                
                if agent:
                    step_data['curiosity'] = agent.getCuriosityLevel()
                    step_data['novelty'] = agent.getNoveltyLevel()
                    step_data['neuromodulation'] = agent.getNeuromodulationLevel()
                
                simulation_data['metrics'].append(step_data)
                simulation_data['neuron_activity'].append(step_data['firing_neurons'])
                
                # Log progress every 100 steps
                if (step + 1) % 100 == 0 or step == steps - 1:
                    self.logger.info(f"Step {step + 1}/{steps}: "
                                   f"{brain.getFiringNeuronCount()} firing neurons, "
                                   f"{brain.getTotalSpikeCount()} total spikes")
                
                # Run visualization tools if available
                if self.tools:
                    try:
                        self.tools.run_visualization(brain, step, format='png')
                    except Exception as e:
                        self.logger.warning(f"Visualization failed: {e}")
                
            except Exception as e:
                self.logger.error(f"Error at step {step}: {e}")
                return False
        
        # Save simulation data
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(simulation_data, f, indent=2)
            self.logger.info(f"Simulation data saved to {output_file}")
        
        # Generate final report
        final_report = {
            'simulation_summary': {
                'total_steps': steps,
                'final_firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': str(brain.getDevelopmentalStage()),
                'simulation_duration': time.time() - simulation_data['steps'],
            },
            'neuron_activity': simulation_data['neuron_activity'],
            'metrics': simulation_data['metrics']
        }
        
        # Save final report
        report_file = self.output_dir / f"simulation_report_{int(time.time())}.json"
        with open(report_file, 'w') as f:
            json.dump(final_report, f, indent=2)
        
        self.logger.info(f"Simulation completed. Final report saved to {report_file}")
        return True
    
    def run_config_management(self, action: str, config_file: str = None,
                            config_type: str = None, output_file: str = None):
        """
        Manage NLM configuration files.
        
        Args:
            action: Action to perform (list, create, edit, delete, export, import)
            config_file: Path to configuration file
            config_type: Type of configuration (simple, advanced, debug, production)
            output_file: Output file path
        """
        self.logger.info(f"Running config management action: {action}")
        
        if action == 'list':
            # List available configurations
            configs_dir = Path("configs")
            if configs_dir.exists():
                config_files = list(configs_dir.glob("*.json")) + list(configs_dir.glob("*.yaml")) + list(configs_dir.glob("*.yml"))
                if config_files:
                    print("Available configuration files:")
                    for config_file in config_files:
                        print(f"  {config_file.name}")
                else:
                    print("No configuration files found in configs/ directory")
            else:
                print("configs/ directory does not exist")
        
        elif action == 'create':
            # Create a new configuration file
            if not config_type:
                print("Error: config_type is required for create action")
                return False
            
            # Create configuration based on type
            config = pynlm.createDefaultConfig()
            
            if config_type == "simple":
                config.set("brain.neuron_count", 100)
                config.set("brain.synapse_density", 0.05)
                config.set("plasticity.stdp.enable", True)
            
            elif config_type == "advanced":
                config.set("brain.neuron_count", 1000)
                config.set("brain.synapse_density", 0.1)
                config.set("brain.connection_probability", 0.05)
                config.set("plasticity.stdp.enable", True)
                config.set("plasticity.stdp.learning_rate", 0.001)
                config.set("neuromod.dopamine.scale", 1.0)
                config.set("neuromod.curiosity.enable", True)
                config.set("neuromod.novelty.enable", True)
                config.set("memory.working_capacity", 1000)
                config.set("development.initial_plasticity", 1.0)
            
            elif config_type == "debug":
                config.set("brain.neuron_count", 10)
                config.set("brain.synapse_density", 0.02)
                config.set("plasticity.stdp.enable", True)
                config.set("simulation.timestep", 0.01)
            
            elif config_type == "production":
                config.set("brain.neuron_count", 10000)
                config.set("brain.synapse_density", 0.15)
                config.set("plasticity.stdp.enable", True)
                config.set("plasticity.stdp.learning_rate", 0.0005)
                config.set("neuromod.dopamine.scale", 1.5)
                config.set("neuromod.curiosity.enable", True)
                config.set("memory.working_capacity", 5000)
                config.set("development.initial_plasticity", 0.8)
            
            # Save configuration
            if output_file:
                with open(output_file, 'w') as f:
                    # Convert config to JSON
                    config_data = {}
                    for key in config.getKeys():
                        value = config.get(key)
                        if value:
                            config_data[key] = value.value
                    
                    json.dump(config_data, f, indent=2)
                print(f"Configuration saved to {output_file}")
            else:
                print("No output file specified. Configuration not saved.")
        
        elif action == 'export':
            # Export current configuration to different format
            if not config_file:
                print("Error: config_file is required for export action")
                return False
            
            # Load existing configuration
            config = pynlm.createDefaultConfig()
            try:
                with open(config_file, 'r') as f:
                    config_data = json.load(f)
                
                for key, value in config_data.items():
                    if isinstance(value, (int, float, bool, str)):
                        config.set(key, value)
                    elif isinstance(value, list):
                        config.set(key, value)
                
                # Export to different format if output_file specified
                if output_file:
                    with open(output_file, 'w') as f:
                        # Create export data
                        export_data = {
                            'format': 'nlm_config',
                            'version': '1.0',
                            'created': time.time(),
                            'config': config_data
                        }
                        json.dump(export_data, f, indent=2)
                    print(f"Configuration exported to {output_file}")
                
            except Exception as e:
                print(f"Error exporting configuration: {e}")
                return False
        
        elif action == 'import':
            # Import configuration from different format
            if not config_file:
                print("Error: config_file is required for import action")
                return False
            
            try:
                with open(config_file, 'r') as f:
                    import_data = json.load(f)
                
                if 'config' in import_data:
                    config_data = import_data['config']
                else:
                    config_data = import_data
                
                # Create configuration
                config = pynlm.createDefaultConfig()
                for key, value in config_data.items():
                    if isinstance(value, (int, float, bool, str)):
                        config.set(key, value)
                    elif isinstance(value, list):
                        config.set(key, value)
                
                print(f"Configuration imported from {config_file}")
                
                # Save as default config if output_file specified
                if output_file:
                    with open(output_file, 'w') as f:
                        config_data = {}
                        for key in config.getKeys():
                            value = config.get(key)
                            if value:
                                config_data[key] = value.value
                        json.dump(config_data, f, indent=2)
                    print(f"Configuration saved to {output_file}")
                
            except Exception as e:
                print(f"Error importing configuration: {e}")
                return False
        
        else:
            print(f"Error: Unknown action '{action}'")
            return False
        
        return True
    
    def run_visualization(self, input_file: str = None, output_file: str = None,
                         format_type: str = "png", world_file: str = None,
                         agent_file: str = None):
        """
        Generate visualizations from simulation data.
        
        Args:
            input_file: Path to simulation data file
            output_file: Output file path
            format_type: Output format (png, svg, pdf, json, html, ascii)
            world_file: Path to world data file
            agent_file: Path to agent data file
        """
        self.logger.info(f"Running visualization with input: {input_file}")
        
        if not input_file or not Path(input_file).exists():
            print("Error: Valid input file required for visualization")
            return False
        
        try:
            # Load simulation data
            with open(input_file, 'r') as f:
                simulation_data = json.load(f)
            
            # Get metrics and neuron activity
            metrics = simulation_data.get('metrics', [])
            neuron_activity = simulation_data.get('neuron_activity', [])
            
            if not metrics:
                print("Error: No metrics data found in input file")
                return False
            
            # Generate visualization using tools if available
            if self.tools:
                try:
                    # Create a simple brain for visualization (would need actual data)
                    config = pynlm.createDefaultConfig()
                    brain = pynlm.createBrain(config)
                    brain.initialize()
                    
                    # Use the first few steps for visualization
                    for i in range(min(50, len(metrics))):
                        step_metrics = metrics[i]
                        brain.step(i)
                    
                    output_path = self.tools.run_visualization(brain, len(metrics), format_type)
                    self.logger.info(f"Visualization generated: {output_path}")
                    
                    if output_file:
                        Path(output_path).rename(output_file)
                        self.logger.info(f"Visualization saved to {output_file}")
                    
                    return True
                    
                except Exception as e:
                    self.logger.error(f"Visualization generation failed: {e}")
                    return False
            else:
                print("Visualization tools not available")
                return False
                
        except Exception as e:
            print(f"Error loading simulation data: {e}")
            return False
    
    def run_debug(self, log_level: str = "INFO", output_file: str = None,
                  brain_file: str = None, world_file: str = None,
                  agent_file: str = None):
        """
        Run debugging and analysis tools.
        
        Args:
            log_level: Logging level (ERROR, WARNING, INFO, DEBUG, TRACE)
            output_file: Output log file
            brain_file: Path to brain data file
            world_file: Path to world data file
            agent_file: Path to agent data file
        """
        self.logger.info(f"Running debug analysis with log level: {log_level}")
        
        # Update logging level
        for handler in logging.getLogger().handlers:
            handler.setLevel(getattr(logging, log_level.upper()))
        
        if self.tools:
            try:
                # Load data if files provided
                brain = None
                world = None
                agent = None
                
                if brain_file and Path(brain_file).exists():
                    # Load brain data
                    pass  # Implementation would depend on brain structure
                
                if world_file and Path(world_file).exists():
                    # Load world data
                    pass  # Implementation would depend on world structure
                
                if agent_file and Path(agent_file).exists():
                    # Load agent data
                    pass  # Implementation would depend on agent structure
                
                # Run debug analysis
                if brain and world and agent:
                    state, issues = self.tools.run_debug_analysis(brain, world, agent)
                    print(f"Debug analysis completed. Issues found: {len(issues)}")
                    
                    if issues:
                        print("Issues:")
                        for i, issue in enumerate(issues):
                            print(f"  {i + 1}. {issue}")
                
                # Generate performance report
                report = self.tools.monitor.generate_performance_report()
                print(f"Performance report generated: {report}")
                
                return True
                
            except Exception as e:
                self.logger.error(f"Debug analysis failed: {e}")
                return False
        else:
            print("Debug tools not available")
            return False
    
    def run_monitor(self, brain_file: str = None, interval: float = 1.0,
                    duration: float = 60.0, output_file: str = None):
        """
        Monitor simulation performance.
        
        Args:
            brain_file: Path to brain data file
            interval: Monitoring interval in seconds
            duration: Monitoring duration in seconds
            output_file: Output file for monitoring data
        """
        self.logger.info(f"Starting performance monitoring for {duration}s")
        
        if self.tools:
            try:
                # Load brain if file provided
                brain = None
                if brain_file and Path(brain_file).exists():
                    # Load brain data
                    pass  # Implementation would depend on brain structure
                
                # Start monitoring
                if brain:
                    monitor_thread = self.tools.run_performance_monitoring(brain, interval)
                    
                    # Wait for specified duration
                    time.sleep(duration)
                    
                    # Stop monitoring
                    if monitor_thread and monitor_thread.is_alive():
                        monitor_thread.join(timeout=5)
                
                # Generate performance report
                report_file = self.tools.monitor.generate_performance_report()
                print(f"Performance monitoring completed. Report: {report_file}")
                
                return True
                
            except Exception as e:
                self.logger.error(f"Performance monitoring failed: {e}")
                return False
        else:
            print("Performance monitoring tools not available")
            return False
    
    def run_export(self, brain_file: str = None, world_file: str = None,
                   agent_file: str = None, output_format: str = "json",
                   output_file: str = None):
        """
        Export simulation data.
        
        Args:
            brain_file: Path to brain data file
            world_file: Path to world data file
            agent_file: Path to agent data file
            output_format: Output format (json, csv, xml)
            output_file: Output file path
        """
        self.logger.info(f"Running export with format: {output_format}")
        
        try:
            # Load data
            brain_data = None
            world_data = None
            agent_data = None
            
            if brain_file and Path(brain_file).exists():
                with open(brain_file, 'r') as f:
                    brain_data = json.load(f)
            
            if world_file and Path(world_file).exists():
                with open(world_file, 'r') as f:
                    world_data = json.load(f)
            
            if agent_file and Path(agent_file).exists():
                with open(agent_file, 'r') as f:
                    agent_data = json.load(f)
            
            # Create export data
            export_data = {
                'export_time': time.time(),
                'format': output_format,
                'brain_data': brain_data,
                'world_data': world_data,
                'agent_data': agent_data
            }
            
            # Save export data
            if output_file:
                with open(output_file, 'w') as f:
                    json.dump(export_data, f, indent=2)
                self.logger.info(f"Data exported to {output_file}")
            
            # Also save to default location
            default_file = self.output_dir / f"export_{int(time.time())}.{output_format}"
            with open(default_file, 'w') as f:
                json.dump(export_data, f, indent=2)
            
            self.logger.info(f"Data also saved to {default_file}")
            return True
            
        except Exception as e:
            self.logger.error(f"Export failed: {e}")
            return False
    
    def show_help(self):
        """Show help information."""
        help_text = """
NLM Command Line Interface

Commands:
  simulate    Run a simulation with specified configuration
  config      Manage NLM configuration files
  visualize   Generate visualizations from simulation data
  debug       Run debugging and analysis tools
  monitor     Monitor simulation performance
  export      Export simulation data
  help        Show help information

Usage Examples:
  python -m nlm.cli simulate --config config.json --steps 1000
  python -m nlm.cli config list
  python -m nlm.cli config create --type advanced --output config.json
  python -m nlm.cli visualize --input simulation.json --output report.png
  python -m nlm.cli debug --log-level DEBUG --output debug.log
  python -m nlm.cli monitor --duration 30
  python -m nlm.cli export --output-format json --output export.json

Configuration File Format:
  {
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": true,
    "neuromod.dopamine.scale": 1.0
  }

Options:
  --help            Show help information
  --version         Show version information
  --output-dir      Output directory (default: nlm_output)
  --log-level       Logging level (ERROR, WARNING, INFO, DEBUG)
"""
        print(help_text)

    def show_version(self):
        """Show version information."""
        version_info = {
            'NLM CLI': '1.0.0',
            'NLM Library': '0.1.0 (Phase 6: Final Integration)',
            'Python': f'{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}',
            'Build Date': time.strftime('%Y-%m-%d %H:%M:%S'),
            'Author': 'NLM Development Team'
        }
        
        print("NLM Command Line Interface Version Information:")
        print("=" * 50)
        for key, value in version_info.items():
            print(f"{key}: {value}")
        print("=" * 50)
def main():
    """Main CLI function."""
    cli = NLMCLI()
    
    # Check if any arguments provided
    if len(sys.argv) < 2:
        cli.show_help()
        return
    
    # Parse command
    command = sys.argv[1]
    
    try:
        if command == 'help':
            cli.show_help()
        
        elif command == 'version':
            cli.show_version()
        
        elif command == 'simulate':
            # Parse simulation arguments
            import argparse
            parser = argparse.ArgumentParser(description='Run NLM simulation')
            parser.add_argument('--config', help='Configuration file path')
            parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
            parser.add_argument('--world-config', help='World configuration file path')
            parser.add_argument('--agent-config', help='Agent configuration file path')
            parser.add_argument('--output', help='Output file path')
            parser.add_argument('--output-dir', default='nlm_output', help='Output directory')
            
            args = parser.parse_args(sys.argv[2:])
            
            # Update CLI output directory
            cli.output_dir = Path(args.output_dir)
            cli.output_dir.mkdir(exist_ok=True)
            
            success = cli.run_simulation(
                config_file=args.config,
                steps=args.steps,
                world_config=args.world_config,
                agent_config=args.agent_config,
                output_file=args.output
            )
            
            if not success:
                sys.exit(1)
        
        elif command == 'config':
            # Parse config management arguments
            import argparse
            parser = argparse.ArgumentParser(description='Manage NLM configuration')
            parser.add_argument('action', choices=['list', 'create', 'edit', 'delete', 'export', 'import'])
            parser.add_argument('--config-file', help='Configuration file path')
            parser.add_argument('--type', dest='config_type', help='Configuration type')
            parser.add_argument('--output', help='Output file path')
            
            args = parser.parse_args(sys.argv[2:])
            
            success = cli.run_config_management(
                action=args.action,
                config_file=args.config_file,
                config_type=args.config_type,
                output_file=args.output
            )
            
            if not success:
                sys.exit(1)
        
        elif command == 'visualize':
            # Parse visualization arguments
            import argparse
            parser = argparse.ArgumentParser(description='Generate NLM visualizations')
            parser.add_argument('--input', required=True, help='Input simulation data file')
            parser.add_argument('--output', help='Output file path')
            parser.add_argument('--format', default='png', choices=['png', 'svg', 'pdf', 'json', 'html', 'ascii'], help='Output format')
            parser.add_argument('--world-file', help='World data file')
            parser.add_argument('--agent-file', help='Agent data file')
            
            args = parser.parse_args(sys.argv[2:])
            
            success = cli.run_visualization(
                input_file=args.input,
                output_file=args.output,
                format_type=args.format,
                world_file=args.world_file,
                agent_file=args.agent_file
            )
            
            if not success:
                sys.exit(1)
        
        elif command == 'debug':
            # Parse debug arguments
            import argparse
            parser = argparse.ArgumentParser(description='Run NLM debugging')
            parser.add_argument('--log-level', default='INFO', choices=['ERROR', 'WARNING', 'INFO', 'DEBUG', 'TRACE'], help='Logging level')
            parser.add_argument('--output', help='Output log file')
            parser.add_argument('--brain-file', help='Brain data file')
            parser.add_argument('--world-file', help='World data file')
            parser.add_argument('--agent-file', help='Agent data file')
            
            args = parser.parse_args(sys.argv[2:])
            
            success = cli.run_debug(
                log_level=args.log_level,
                output_file=args.output,
                brain_file=args.brain_file,
                world_file=args.world_file,
                agent_file=args.agent_file
            )
            
            if not success:
                sys.exit(1)
        
        elif command == 'monitor':
            # Parse monitor arguments
            import argparse
            parser = argparse.ArgumentParser(description='Monitor NLM simulation')
            parser.add_argument('--brain-file', help='Brain data file')
            parser.add_argument('--interval', type=float, default=1.0, help='Monitoring interval in seconds')
            parser.add_argument('--duration', type=float, default=60.0, help='Monitoring duration in seconds')
            
            args = parser.parse_args(sys.argv[2:])
            
            success = cli.run_monitor(
                brain_file=args.brain_file,
                interval=args.interval,
                duration=args.duration
            )
            
            if not success:
                sys.exit(1)
        
        elif command == 'export':
            # Parse export arguments
            import argparse
            parser = argparse.ArgumentParser(description='Export NLM data')
            parser.add_argument('--brain-file', help='Brain data file')
            parser.add_argument('--world-file', help='World data file')
            parser.add_argument('--agent-file', help='Agent data file')
            parser.add_argument('--output-format', default='json', choices=['json', 'csv', 'xml'], help='Output format')
            parser.add_argument('--output', help='Output file path')
            
            args = parser.parse_args(sys.argv[2:])
            
            success = cli.run_export(
                brain_file=args.brain_file,
                world_file=args.world_file,
                agent_file=args.agent_file,
                output_format=args.output_format,
                output_file=args.output
            )
            
            if not success:
                sys.exit(1)
        
        else:
            print(f"Unknown command: {command}")
            cli.show_help()
            sys.exit(1)
        
    except KeyboardInterrupt:
        print("\nOperation interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
if __name__ == "__main__":
    main()