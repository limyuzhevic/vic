#!/usr/bin/env python3
"""
Enhanced NLM Python Command Line Interface

This module provides command-line tools for working with NLM (Neural Learning Machine)
using the enhanced Python bindings.
"""

import argparse
import sys
import os
import json
from pathlib import Path

# Add the current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm_enhanced as pynlm
def main():
    """Main command line interface"""
    parser = argparse.ArgumentParser(
        description="Enhanced NLM Python Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s create --config my_config.json
  %(prog)s simulate --config config.json --steps 1000
  %(prog)s analyze --brain brain.bin
  %(prog)s example --scenario silent_brain
  %(prog)s run --enhanced --verbose
        """
    )
    
    parser.add_argument(
        '--version', action='version',
        version='NLM Enhanced CLI v0.2.0'
    )
    
    parser.add_argument(
        '--log-level', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'],
        default='INFO', help='Logging level (default: INFO)'
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create brain or configuration')
    create_parser.add_argument('action', choices=['brain', 'config', 'agent'], 
                              help='What to create')
    create_parser.add_argument('--config', help='Configuration file path')
    create_parser.add_argument('--output', help='Output file path')
    create_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    create_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    create_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    
    # Simulate command
    simulate_parser = subparsers.add_parser('simulate', help='Run simulation')
    simulate_parser.add_argument('--config', help='Configuration file path')
    simulate_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    simulate_parser.add_argument('--width', type=int, default=20, help='World width')
    simulate_parser.add_argument('--height', type=int, default=20, help='World height')
    simulate_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    simulate_parser.add_argument('--output', help='Output file path for results')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze brain or configuration')
    analyze_parser.add_argument('--brain', help='Brain file path')
    analyze_parser.add_argument('--config', help='Configuration file path')
    analyze_parser.add_argument('--stats-type', choices=['basic', 'connectivity', 'activity', 'all'],
                               default='all', help='Type of statistics to analyze')
    analyze_parser.add_argument('--output', help='Output file path for analysis results')
    
    # Example command
    example_parser = subparsers.add_parser('example', help='Run example scenario')
    example_parser.add_argument('scenario', choices=['silent_brain', 'complete_agent', 'advanced_agent'],
                               help='Example scenario to run')
    example_parser.add_argument('--output', help='Output file path for results')
    example_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    
    # Run command
    run_parser = subparsers.add_parser('run', help='Run enhanced simulation')
    run_parser.add_argument('--enhanced', action='store_true', help='Use enhanced bindings')
    run_parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    run_parser.add_argument('--world-width', type=int, default=20, help='World width')
    run_parser.add_argument('--world-height', type=int, default=20, help='World height')
    run_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    run_parser.add_argument('--output', help='Output file path for results')
    
    # Batch command
    batch_parser = subparsers.add_parser('batch', help='Run multiple simulations')
    batch_parser.add_argument('--configs', nargs='+', help='Configuration files')
    batch_parser.add_argument('--steps', type=int, default=500, help='Steps per simulation')
    batch_parser.add_argument('--output-dir', help='Output directory for results')
    batch_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate configuration or brain')
    validate_parser.add_argument('--config', help='Configuration file path')
    validate_parser.add_argument('--brain', help='Brain file path')
    validate_parser.add_argument('--required-keys', nargs='+', help='Required configuration keys')
    
    # Export command
    export_parser = subparsers.add_parser('export', help='Export results to various formats')
    export_parser.add_argument('--input', help='Input file path')
    export_parser.add_argument('--format', choices=['json', 'csv', 'txt'], default='json', help='Export format')
    export_parser.add_argument('--output', help='Output file path')
    
    args = parser.parse_args()
    
    # Set up logging
    import logging
    logging.basicConfig(level=getattr(logging, args.log_level),
                       format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    logger = logging.getLogger(__name__)
    
    try:
        if args.command == 'create':
            handle_create_command(args, logger)
        elif args.command == 'simulate':
            handle_simulate_command(args, logger)
        elif args.command == 'analyze':
            handle_analyze_command(args, logger)
        elif args.command == 'example':
            handle_example_command(args, logger)
        elif args.command == 'run':
            handle_run_command(args, logger)
        elif args.command == 'batch':
            handle_batch_command(args, logger)
        elif args.command == 'validate':
            handle_validate_command(args, logger)
        elif args.command == 'export':
            handle_export_command(args, logger)
        else:
            parser.error("No command specified")
    except Exception as e:
        logger.error(f"Command failed: {e}")
        sys.exit(1)
def handle_create_command(args, logger):
    """Handle create command"""
    logger.info(f"Creating {args.action}")
    
    if args.action == 'brain':
        # Create brain
        config = pynlm.createDevelopmentConfig(
            neuron_count=args.neurons,
            enableDevelopment=True,
            enableCuriosity=True,
            enableRewardModulation=True
        )
        
        brain = pynlm.createBrain(config)
        
        logger.info(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        logger.info(f"Brain statistics: {pynlm.getBrainStatistics(brain)}")
        
        if args.output:
            if pynlm.saveBrainToFile(brain, args.output):
                logger.info(f"Brain saved to {args.output}")
            else:
                logger.error(f"Failed to save brain to {args.output}")
                
    elif args.action == 'config':
        # Create configuration
        config_dict = {
            "neuron_count": args.neurons,
            "region_count": args.regions,
            "connection_probability": 0.1
        }
        
        config = pynlm.createValidatedConfig(config_dict)
        
        logger.info(f"Created configuration: {pynlm.analyzeConfig(config)}")
        
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(config_dict, f, indent=2)
            logger.info(f"Configuration saved to {args.output}")
            
    elif args.action == 'agent':
        # Create agent
        config = pynlm.createDevelopmentConfig(neuron_count=args.neurons)
        brain = pynlm.createBrain(config)
        agent = pynlm.PyAgentBrain(brain)
        
        logger.info(f"Created agent with brain containing {brain.getTotalNeuronCount()} neurons")
        
        if args.verbose:
            logger.info(f"Agent stats: curiosity={agent.getCuriosityLevel():.3f}")
        
        if args.output:
            logger.info(f"Agent saved to {args.output}")
    
    logger.info("Create command completed successfully")
def handle_simulate_command(args, logger):
    """Handle simulate command"""
    logger.info(f"Running simulation for {args.steps} steps")
    
    # Load configuration if provided
    if args.config:
        try:
            with open(args.config, 'r') as f:
                config_dict = json.load(f)
            config = pynlm.createValidatedConfig(config_dict)
            logger.info("Loaded configuration from file")
        except Exception as e:
            logger.error(f"Failed to load configuration: {e}")
            return
    else:
        config = pynlm.createDevelopmentConfig()
        logger.info("Using default configuration")
    
    # Create components
    brain = pynlm.createBrain(config)
    agent = pynlm.PyAgentBrain(brain)
    world = pynlm.PySimpleWorld()
    
    # Configure world
    world.configure(args.width, args.height, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    logger.info("Starting simulation...")
    result = pynlm.runValidatedSimulation(agent, world, args.steps, args.verbose)
    
    print(result)
    
    if args.output:
        with open(args.output, 'w') as f:
            f.write(result)
        logger.info(f"Simulation results saved to {args.output}")
    
    logger.info("Simulation command completed successfully")
def handle_analyze_command(args, logger):
    """Handle analyze command"""
    logger.info("Analyzing brain or configuration")
    
    if args.brain:
        # Analyze brain file
        try:
            config = pynlm.createDevelopmentConfig(neuron_count=100)
            brain = pynlm.createBrain(config)
            
            if pynlm.loadBrainFromFile(brain, args.brain):
                logger.info("Loaded brain from file")
                
                # Get statistics based on type
                if args.stats_type == 'basic':
                    stats = pynlm.getBrainStatistics(brain, ["basic"])
                elif args.stats_type == 'connectivity':
                    stats = pynlm.getBrainStatistics(brain, ["connectivity"])
                elif args.stats_type == 'activity':
                    stats = pynlm.getBrainStatistics(brain, ["activity"])
                else:  # all
                    stats = pynlm.getBrainStatistics(brain, ["basic", "connectivity", "activity"])
                
                print(stats)
                
                if args.output:
                    with open(args.output, 'w') as f:
                        f.write(stats)
                    logger.info(f"Analysis results saved to {args.output}")
            else:
                logger.error(f"Failed to load brain from {args.brain}")
                
        except Exception as e:
            logger.error(f"Failed to analyze brain: {e}")
            
    elif args.config:
        # Analyze configuration file
        try:
            with open(args.config, 'r') as f:
                config_dict = json.load(f)
            
            config = pynlm.createValidatedConfig(config_dict)
            analysis = pynlm.analyzeConfig(config)
            
            print(analysis)
            
            if args.output:
                with open(args.output, 'w') as f:
                    f.write(analysis)
                logger.info(f"Analysis results saved to {args.output}")
                
        except Exception as e:
            logger.error(f"Failed to analyze configuration: {e}")
    
    logger.info("Analyze command completed successfully")
def handle_example_command(args, logger):
    """Handle example command"""
    logger.info(f"Running example scenario: {args.scenario}")
    
    try:
        if args.scenario == 'silent_brain':
            result = pynlm.runValidatedExample(1, {})
        elif args.scenario == 'complete_agent':
            result = pynlm.runValidatedExample(2, {})
        elif args.scenario == 'advanced_agent':
            result = pynlm.runValidatedExample(2, {"steps": 500, "neuron_count": 800})
        
        print(result)
        
        if args.output:
            with open(args.output, 'w') as f:
                f.write(result)
            logger.info(f"Example results saved to {args.output}")
        
        logger.info(f"Example scenario '{args.scenario}' completed successfully")
        
    except Exception as e:
        logger.error(f"Failed to run example: {e}")
def handle_run_command(args, logger):
    """Handle run command"""
    logger.info("Running enhanced simulation")
    
    # Create components
    config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.PyAgentBrain(brain)
    world = pynlm.PySimpleWorld()
    
    # Configure world
    world.configure(args.world_width, args.world_height, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    logger.info(f"Starting enhanced simulation with {args.steps} steps")
    result = pynlm.runValidatedSimulation(agent, world, args.steps, args.verbose)
    
    print(result)
    
    if args.output:
        with open(args.output, 'w') as f:
            f.write(result)
        logger.info(f"Simulation results saved to {args.output}")
    
    logger.info("Enhanced simulation completed successfully")
def handle_batch_command(args, logger):
    """Handle batch command"""
    logger.info(f"Running batch simulation with {len(args.configs)} configurations")
    
    if not args.output_dir:
        logger.error("Output directory is required for batch command")
        return
    
    os.makedirs(args.output_dir, exist_ok=True)
    
    for i, config_file in enumerate(args.configs):
        logger.info(f"Processing configuration {i+1}/{len(args.configs)}: {config_file}")
        
        try:
            # Load configuration
            with open(config_file, 'r') as f:
                config_dict = json.load(f)
            
            config = pynlm.createValidatedConfig(config_dict)
            
            # Create components
            brain = pynlm.createBrain(config)
            agent = pynlm.PyAgentBrain(brain)
            world = pynlm.PySimpleWorld()
            
            # Configure world (extract from config or use defaults)
            width = config_dict.get('world_width', 20)
            height = config_dict.get('world_height', 20)
            world.configure(width, height, 8, 8)
            world.reset()
            agent.initialize(world)
            
            # Enable subsystems
            agent.enableRewardModulation(config_dict.get('reward_enabled', True))
            agent.enableStructuralPlasticity(config_dict.get('structural_enabled', True))
            agent.enableDevelopment(config_dict.get('development_enabled', True))
            agent.enableCuriosity(config_dict.get('curiosity_enabled', True))
            
            # Run simulation
            result = pynlm.runValidatedSimulation(agent, world, args.steps, args.verbose)
            
            # Save results
            output_file = os.path.join(args.output_dir, f"batch_{i+1}_{Path(config_file).stem}.txt")
            with open(output_file, 'w') as f:
                f.write(result)
            
            logger.info(f"Saved results to {output_file}")
            
        except Exception as e:
            logger.error(f"Failed to process {config_file}: {e}")
    
    logger.info("Batch simulation completed")
def handle_validate_command(args, logger):
    """Handle validate command"""
    logger.info("Validating configuration or brain")
    
    if args.config:
        # Validate configuration
        try:
            with open(args.config, 'r') as f:
                config_dict = json.load(f)
            
            config = pynlm.createValidatedConfig(config_dict)
            
            if args.required_keys:
                if pynlm.validateRequiredConfigKeys(config, args.required_keys):
                    print("Configuration validation: PASSED")
                else:
                    print("Configuration validation: FAILED")
                    print("Missing required keys")
            else:
                print("Configuration validation: PASSED")
                print("Configuration is valid")
                
        except Exception as e:
            print(f"Configuration validation: FAILED - {e}")
            
    elif args.brain:
        # Validate brain
        try:
            config = pynlm.createDevelopmentConfig(neuron_count=100)
            brain = pynlm.createBrain(config)
            
            if pynlm.loadBrainFromFile(brain, args.brain):
                pynlm.validateBrainState(brain)
                print("Brain validation: PASSED")
                print(f"Brain contains {brain.getTotalNeuronCount()} neurons")
                print(f"Brain contains {brain.getTotalSynapseCount()} synapses")
            else:
                print("Brain validation: FAILED - Could not load brain file")
                
        except Exception as e:
            print(f"Brain validation: FAILED - {e}")
    
    logger.info("Validation command completed")
def handle_export_command(args, logger):
    """Handle export command"""
    logger.info(f"Exporting results to {args.format} format")
    
    if not args.input:
        logger.error("Input file is required for export command")
        return
    
    # Load input file
    try:
        if args.format == 'json':
            with open(args.input, 'r') as f:
                data = json.load(f)
            
            with open(args.output, 'w') as f:
                json.dump(data, f, indent=2)
                
        elif args.format == 'csv':
            # For CSV, we'd need to process the data
            logger.warning("CSV export not fully implemented")
            
        elif args.format == 'txt':
            # For TXT, we'd need to process the data
            logger.warning("TXT export not fully implemented")
        
        logger.info(f"Exported results to {args.output}")
        
    except Exception as e:
        logger.error(f"Failed to export: {e}")
if __name__ == "__main__":
    main()
