# Python CLI module for NLM
# Provides advanced command-line tools for NLM users

import argparse
import json
import sys
import os
import subprocess
from pathlib import Path
from typing import Dict, List, Optional, Any

# Try to import NLM bindings
try:
    import pynlm as nlm
    PYNL_AVAILABLE = True
except ImportError:
    PYNL_AVAILABLE = False

def create_advanced_config():
    """Create an advanced NLM configuration interactively"""
    print("=== Advanced NLM Configuration Generator ===")
    
    config = {}
    
    # Basic settings
    print("\n1. Basic Settings")
    config['random_seed'] = int(input("Random seed (default: 42): ") or "42")
    config['simulation_timestep'] = float(input("Simulation timestep (default: 0.001): ") or "0.001")
    config['neuron_count'] = int(input("Neuron count (default: 1000): ") or "1000")
    config['region_count'] = int(input("Region count (default: 1): ") or "1")
    config['connection_probability'] = float(input("Connection probability (default: 0.1): ") or "0.1")
    
    # STDP settings
    print("\n2. STDP (Spike-Timing-Dependent Plasticity) Settings")
    config['stdp_ltp_weight'] = float(input("STDP LTP weight (default: 0.01): ") or "0.01")
    config['stdp_ltd_weight'] = float(input("STDP LTD weight (default: 0.012): ") or "0.012")
    config['stdp_tau'] = float(input("STDP tau (default: 20.0): ") or "20.0")
    
    # Structural plasticity
    print("\n3. Structural Plasticity Settings")
    config['synaptogenesis_rate'] = float(input("Synaptogenesis rate (default: 0.0001): ") or "0.0001")
    config['pruning_rate'] = float(input("Pruning rate (default: 0.00001): ") or "0.00001")
    
    # Development
    print("\n4. Development Settings")
    config['development_enabled'] = input("Enable development (y/n, default: y): ").lower() != 'n'
    config['development_start_age'] = float(input("Development start age (default: 60.0): ") or "60.0")
    config['development_maturity_age'] = float(input("Development maturity age (default: 300.0): ") or "300.0")
    
    # Neuromodulation
    print("\n5. Neuromodulation Settings")
    config['dopamine_sensitivity'] = float(input("Dopamine sensitivity (default: 1.0): ") or "1.0")
    config['curiosity_threshold'] = float(input("Curiosity threshold (default: 0.3): ") or "0.3")
    config['novelty_decay_rate'] = float(input("Novelty decay rate (default: 0.99): ") or "0.99")
    
    # Memory
    print("\n6. Memory Settings")
    config['working_memory_capacity'] = int(input("Working memory capacity (default: 1000): ") or "1000")
    config['episodic_memory_capacity'] = int(input("Episodic memory capacity (default: 500): ") or "500")
    
    # Prediction
    print("\n7. Prediction Settings")
    config['prediction_horizon'] = int(input("Prediction horizon (default: 10): ") or "10")
    config['prediction_error_decay'] = float(input("Prediction error decay (default: 0.9): ") or "0.9")
    
    return config

def run_phase6_demo(config_file: Optional[str] = None, neurons: int = 500, steps: int = 2000):
    """Run the Phase 6 integration demo"""
    print("=== NLM Phase 6 Integration Demo ===")
    
    if not PYNL_AVAILABLE:
        print("ERROR: pynlm is not available. Please install NLM Python bindings.")
        return False
    
    # Create config if not provided
    if config_file:
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
        except Exception as e:
            print(f"ERROR: Could not load config file: {e}")
            return False
    else:
        config = create_advanced_config()
    
    # Add Phase 6 specific config
    config['neuron_count'] = neurons
    config['simulation_timestep'] = 0.001
    
    try:
        print(f"Creating NLM brain with {neurons} neurons...")
        brain = nlm.createBrain(nlm.createDefaultConfig())
        brain.initialize()
        
        print(f"Running Phase 6 integration demo for {steps} steps...")
        
        # Run simulation using the advanced experiment runner
        from nlm_advanced import Phase6IntegratedExperiment, Phase6Config
        
        phase6_config = Phase6Config()
        phase6_config.neuronCount = neurons
        phase6_config.maxSteps = steps
        phase6_config.regionCount = 1
        phase6_config.connectionProbability = 0.1f
        phase6_config.enableCheckpointing = True
        phase6_config.enableReplay = True
        phase6_config.enableDevelopment = True
        
        experiment = Phase6IntegratedExperiment()
        result = experiment.run(phase6_config)
        
        print("\n=== Phase 6 Integration Results ===")
        print(f"Total reward: {result.totalReward}")
        print(f"Average firing rate: {result.avgFiringRate}")
        print(f"Memory episodes stored: {result.memoryEpisodesStored}")
        print(f"Dopamine level: {result.dopamineLevel}")
        
        print("\n=== System Integration Status ===")
        print(f"Working Memory: {'✓ INTEGRATED' if result.memoryWorkingMemoryIntegrated else '✗ DISCONNECTED'}")
        print(f"Episodic Memory: {'✓ INTEGRATED' if result.memoryEpisodicMemoryIntegrated else '✗ DISCONNECTED'}")
        print(f"Neuromodulation: {'✓ INTEGRATED' if result.neuromodulationIntegrated else '✗ DISCONNECTED'}")
        print(f"Prediction System: {'✓ INTEGRATED' if result.predictionIntegrated else '✗ DISCONNECTED'}")
        print(f"Development System: {'✓ INTEGRATED' if result.developmentIntegrated else '✗ DISCONNECTED'}")
        print(f"Checkpointing: {'✓ WORKING' if result.checkpointingWorks else '✗ NOT WORKING'}")
        
        return True
        
    except Exception as e:
        print(f"ERROR: Phase 6 demo failed: {e}")
        return False

def run_advanced_simulation(config_file: str, duration: int, visualize: bool = False):
    """Run an advanced simulation with custom configuration"""
    print(f"=== Advanced Simulation (Duration: {duration}s) ===")
    
    try:
        with open(config_file, 'r') as f:
            config = json.load(f)
        
        if not PYNL_AVAILABLE:
            print("ERROR: pynlm is not available.")
            return False
        
        # Create brain from config
        brain_config = nlm.createDefaultConfig()
        # Apply custom config
        for key, value in config.items():
            try:
                brain_config.set(key, value, nlm.ConfigSource.Default)
            except:
                pass  # Some keys might not be supported
        
        brain = nlm.createBrain(brain_config)
        brain.initialize()
        
        print(f"Running advanced simulation for {duration} steps...")
        
        # Collect performance metrics
        total_reward = 0.0
        total_firing_rate = 0.0
        
        for step in range(duration):
            brain.step(step, step * 0.001)
            total_firing_rate += brain.getAverageFiringRate()
            
            if visualize and step % 100 == 0:
                print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.2f}")
        
        avg_firing_rate = total_firing_rate / duration
        
        print(f"\n=== Advanced Simulation Results ===")
        print(f"Average firing rate: {avg_firing_rate:.2f}")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"Development stage: {brain.getDevelopmentalStage()}")
        
        return True
        
    except Exception as e:
        print(f"ERROR: Advanced simulation failed: {e}")
        return False

def debug_mode(config_file: Optional[str] = None):
    """Run NLM in debug mode with enhanced logging"""
    print("=== NLM Debug Mode ===")
    
    try:
        # Enable debug logging
        import logging
        logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        
        if config_file:
            print(f"Loading config from {config_file}")
        
        if not PYNL_AVAILABLE:
            print("ERROR: pynlm is not available.")
            return False
        
        # Create and run brain in debug mode
        brain_config = nlm.createDefaultConfig()
        brain = nlm.createBrain(brain_config)
        brain.initialize()
        
        print("Debug mode enabled. Running 100 steps with detailed logging...")
        
        for step in range(100):
            brain.step(step, step * 0.001)
            
            if step % 10 == 0:
                print(f"Step {step}: Active neurons = {brain.getActiveNeuronCount()}, "
                      f"Firing = {brain.getFiringNeuronCount()}, "
                      f"Firing rate = {brain.getAverageFiringRate():.2f}")
        
        print("\nDebug mode completed successfully.")
        return True
        
    except Exception as e:
        print(f"ERROR: Debug mode failed: {e}")
        return False

def export_configuration(config: Dict, output_file: str):
    """Export configuration to file"""
    try:
        with open(output_file, 'w') as f:
            json.dump(config, f, indent=2)
        print(f"Configuration exported to {output_file}")
        return True
    except Exception as e:
        print(f"ERROR: Could not export configuration: {e}")
        return False

def import_configuration(input_file: str) -> Optional[Dict]:
    """Import configuration from file"""
    try:
        with open(input_file, 'r') as f:
            config = json.load(f)
        print(f"Configuration imported from {input_file}")
        return config
    except Exception as e:
        print(f"ERROR: Could not import configuration: {e}")
        return None

def list_available_models():
    """List available pre-trained models"""
    print("=== Available Pre-trained Models ===")
    
    models = [
        {
            "name": "Phase6_Integration",
            "neurons": 1000,
            "description": "Complete Phase 6 integration demo model",
            "file": "./checkpoints/phase6_integration.bin"
        },
        {
            "name": "Simple_Brain",
            "neurons": 100,
            "description": "Simple brain for testing",
            "file": "./checkpoints/simple_brain.bin"
        },
        {
            "name": "Memory_Focused",
            "neurons": 500,
            "description": "Brain optimized for memory tasks",
            "file": "./checkpoints/memory_focused.bin"
        }
    ]
    
    for i, model in enumerate(models, 1):
        print(f"{i}. {model['name']}")
        print(f"   Neurons: {model['neurons']}")
        print(f"   Description: {model['description']}")
        print(f"   File: {model['file']}")
        print()
    
    return models

def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-command demo                    # Run Phase 6 integration demo
  nlm-command demo --neurons 2000      # Custom neuron count
  nlm-command demo --steps 5000        # Custom simulation steps
  nlm-command config --output config.json    # Generate configuration
  nlm-command debug                    # Run in debug mode
  nlm-command experiment file config.json  # Run experiment with config
  nlm-command models                   # List available models
  nlm-command help                     # Show help

The NLM command line interface provides advanced tools for:
- Phase 6 integration testing
- Custom simulation configuration
- Advanced debugging and monitoring
- Configuration management
- Model management
        ""
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Demo command
    demo_parser = subparsers.add_parser('demo', help='Run Phase 6 integration demo')
    demo_parser.add_argument('--config', '-c', help='Configuration file path')
    demo_parser.add_argument('--neurons', '-n', type=int, default=500, help='Number of neurons (default: 500)')
    demo_parser.add_argument('--steps', '-s', type=int, default=2000, help='Number of simulation steps (default: 2000)')
    
    # Config command
    config_parser = subparsers.add_parser('config', help='Configuration management')
    config_subparsers = config_parser.add_subparsers(dest='config_action', help='Configuration actions')
    
    generate_parser = config_subparsers.add_parser('generate', help='Generate configuration')
    generate_parser.add_argument('--output', '-o', default='config.json', help='Output file path (default: config.json)')
    
    export_parser = config_subparsers.add_parser('export', help='Export configuration')
    export_parser.add_argument('input', help='Input configuration file')
    export_parser.add_argument('output', help='Output file path')
    
    import_parser = config_subparsers.add_parser('import', help='Import configuration')
    import_parser.add_argument('input', help='Input configuration file')
    
    # Debug command
    debug_parser = subparsers.add_parser('debug', help='Run in debug mode')
    debug_parser.add_argument('--config', '-c', help='Configuration file path')
    
    # Experiment command
    experiment_parser = subparsers.add_parser('experiment', help='Run custom experiment')
    experiment_parser.add_argument('file', help='Experiment configuration file')
    experiment_parser.add_argument('--duration', '-d', type=int, required=True, help='Duration in steps')
    experiment_parser.add_argument('--visualize', '-v', action='store_true', help='Enable visualization')
    
    # Models command
    models_parser = subparsers.add_parser('models', help='List available models')
    
    # Help command
    help_parser = subparsers.add_parser('help', help='Show detailed help')
    help_parser.add_argument('topic', nargs='?', choices=['config', 'brain', 'simulation', 'debug'], help='Help topic')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    success = False
    
    if args.command == 'demo':
        success = run_phase6_demo(args.config, args.neurons, args.steps)
    elif args.command == 'config':
        if args.config_action == 'generate':
            config = create_advanced_config()
            success = export_configuration(config, args.output)
        elif args.config_action == 'export':
            config = import_configuration(args.input)
            if config:
                success = export_configuration(config, args.output)
        elif args.config_action == 'import':
            config = import_configuration(args.input)
            success = config is not None
    elif args.command == 'debug':
        success = debug_mode(args.config)
    elif args.command == 'experiment':
        success = run_advanced_simulation(args.file, args.duration, args.visualize)
    elif args.command == 'models':
        list_available_models()
        success = True
    elif args.command == 'help':
        if args.topic:
            help_text = generate_help(args.topic)
            print(help_text)
        else:
            parser.print_help()
        success = True
    
    return 0 if success else 1

def generate_help(topic: str) -> str:
    """Generate help text for a specific topic"""
    help_texts = {
        'config': """
Advanced Configuration Help:
============================

This section explains how to create and use advanced configurations for NLM.

Basic Configuration Parameters:
- random_seed: Random seed for reproducible experiments
- simulation_timestep: Time step size in seconds
- neuron_count: Number of neurons in the brain
- region_count: Number of brain regions
- connection_probability: Probability of synaptic connections

STDP Parameters:
- stdp_ltp_weight: Long-term potentiation weight
- stdp_ltd_weight: Long-term depression weight
- stdp_tau: STDP time constant

Development Parameters:
- development_enabled: Whether development system is active
- development_start_age: Age when development begins
- development_maturity_age: Age when development completes

Neuromodulation Parameters:
- dopamine_sensitivity: Sensitivity to dopamine signals
- curiosity_threshold: Threshold for curiosity activation
- novelty_decay_rate: Rate at which novelty decays

Memory Parameters:
- working_memory_capacity: Capacity of working memory
- episodic_memory_capacity: Capacity of episodic memory

Prediction Parameters:
- prediction_horizon: Number of steps to predict ahead
- prediction_error_decay: Rate at which prediction error decays

Example Configuration:
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "development_enabled": true,
    "dopamine_sensitivity": 1.0,
    "curiosity_threshold": 0.3
}
        """,
        'brain': """
Brain Creation Help:
===================

This section explains how to create and manipulate NLM brains.

Brain Types:
- Standard Brain: Basic NLM brain with core systems
- Advanced Brain: Brain with custom architecture
- ML-Enhanced Brain: Brain with external ML model integration

Brain Operations:
- Create: Create a new brain with configuration
- Initialize: Set up brain for simulation
- Step: Advance simulation by one time step
- Reset: Reset brain to initial state
- Save/Load: Save/restore brain state to/from file

Custom Brain Features:
- Architecture specification: Define neural connectivity patterns
- Population specification: Define neuron populations
- Development control: Control brain development process
- Neuromodulation: Apply neuromodulatory signals
- Plasticity: Control synaptic plasticity

Example Brain Creation:
import pynlm as nlm

# Create configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("random_seed", 42)

# Create brain
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)
        """,
        'simulation': """
Simulation Control Help:
========================

This section explains how to control NLM simulations.

Simulation Types:
- Basic Simulation: Simple forward simulation
- Event-based Simulation: Simulation with callbacks
- Batch Simulation: Multiple simulations in parallel
- Interactive Simulation: Real-time interaction

Simulation Control Functions:
- Step: Advance simulation by one time step
- Reset: Reset simulation to initial state
- Pause/Resume: Control simulation flow
- Speed Control: Adjust simulation speed

Performance Monitoring:
- Real-time metrics: Monitor simulation progress
- Performance profiling: Profile simulation performance
- Resource monitoring: Monitor CPU/memory usage

Event-based Simulation:
- Callbacks: Register functions to be called at specific events
- Event types: Step complete, simulation start, simulation end
- Callback parameters: Event data and context

Example Event-based Simulation:
from nlm_advanced import create_simulation_controller

def on_step(step, time):
    print(f"Step {step}, Time {time}")

def on_start():
    print("Simulation started")

def on_end():
    print("Simulation ended")

controller = create_simulation_controller(on_step, on_start, on_end)
controller.runSimulation(brain, 1000)
        """,
        'debug': """
Debug Mode Help:
===============

This section explains how to use NLM debug mode for troubleshooting.

Debug Mode Features:
- Detailed logging: Enable verbose logging
- Performance profiling: Profile simulation performance
- State inspection: Inspect brain state
- Event tracing: Trace simulation events
- Error reporting: Enhanced error reporting

Debug Commands:
- Enable debug mode: Activate debug logging
- Set log level: Control logging verbosity
- Enable performance monitoring: Monitor resource usage
- Enable event tracing: Trace simulation events
- Capture output: Save debug output to file

Debug Output Analysis:
- Log format: Timestamped log messages
- Error format: Detailed error information
- Performance format: Performance metrics
- State format: Brain state information

Example Debug Mode Usage:
import pynlm as nlm
import logging

# Enable debug logging
logging.basicConfig(level=logging.DEBUG)

# Create and run brain in debug mode
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step, step * 0.001)
    # Debug output will be logged automatically
        """
    }
    
    return help_texts.get(topic, "Help topic not available")

if __name__ == '__main__':
    sys.exit(main())