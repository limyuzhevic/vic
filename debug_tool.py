#!/usr/bin/env python3
"""
NLM Debug Utility - Advanced command-line tool for debugging NLM simulations
"""

import sys
import argparse
import json
from pathlib import Path
import pynlm

def debug_brain(brain, output_file=None):
    """Analyze and debug brain state"""
    print("=== NLM Brain Debug Information ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spike count: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Learned preference: {brain.getLearnedPreference():.3f}")
    
    # Get configuration summary
    config = brain.getConfig()
    print(f"\nConfiguration summary:")
    print(config.summary())
    
    # Save detailed information to file if requested
    if output_file:
        debug_data = {
            "brain_stats": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
                "developmental_stage": str(brain.getDevelopmentalStage()),
                "learned_preference": brain.getLearnedPreference()
            },
            "config_summary": config.summary()
        }
        
        with open(output_file, 'w') as f:
            json.dump(debug_data, f, indent=2)
        print(f"\nDebug information saved to: {output_file}")
    
    return debug_data

def validate_configuration(config_file):
    """Validate a configuration file"""
    print(f"=== Validating Configuration: {config_file} ===")
    
    try:
        config = pynlm.createDefaultConfig()
        if config.loadFromFile(config_file):
            print("✓ Configuration loaded successfully")
            print(f"✓ Contains {len(config.getKeys())} configuration keys")
            print("\nConfiguration keys:")
            for key in config.getKeys():
                print(f"  - {key}")
            
            # Check for required keys
            required_keys = ["neuron_count", "region_count", "connection_probability"]
            missing_keys = []
            for req_key in required_keys:
                if not config.has(req_key):
                    missing_keys.append(req_key)
            
            if missing_keys:
                print(f"\n⚠ WARNING: Missing recommended keys: {', '.join(missing_keys)}")
            else:
                print(f"\n✓ All recommended keys present")
            
            return True
        else:
            print(f"✗ Failed to load configuration from {config_file}")
            return False
            
    except Exception as e:
        print(f"✗ Error validating configuration: {e}")
        return False

def analyze_performance(brain, steps):
    """Analyze performance of a brain simulation"""
    print(f"=== Performance Analysis ({steps} steps) ===")
    
    import time
    
    # Reset brain for clean analysis
    brain.reset()
    brain.initialize()
    
    start_time = time.time()
    
    for i in range(steps):
        brain.step(i)
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print(f"Total simulation time: {total_time:.3f} seconds")
    print(f"Steps per second: {steps/total_time:.1f}")
    print(f"Average time per step: {total_time/steps*1000:.2f} ms")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Average spikes per second: {brain.getTotalSpikeCount()/total_time:.1f}")
    
    # Calculate some additional metrics
    spikes_per_step = brain.getTotalSpikeCount() / steps
    print(f"Average spikes per step: {spikes_per_step:.2f}")
    
    return {
        "total_time": total_time,
        "steps_per_second": steps / total_time,
        "avg_time_per_step_ms": total_time / steps * 1000,
        "total_spikes": brain.getTotalSpikeCount(),
        "avg_spikes_per_second": brain.getTotalSpikeCount() / total_time,
        "avg_spikes_per_step": spikes_per_step
    }

def save_checkpoint(brain, filepath):
    """Save brain state to checkpoint file"""
    print(f"=== Saving Brain Checkpoint: {filepath} ===")
    
    try:
        success = brain.save(filepath)
        if success:
            print("✓ Checkpoint saved successfully")
            return True
        else:
            print("✗ Failed to save checkpoint")
            return False
    except Exception as e:
        print(f"✗ Error saving checkpoint: {e}")
        return False

def load_checkpoint(brain, filepath):
    """Load brain state from checkpoint file"""
    print(f"=== Loading Brain Checkpoint: {filepath} ===")
    
    try:
        success = brain.load(filepath)
        if success:
            print("✓ Checkpoint loaded successfully")
            print(f"Loaded brain state:")
            print(f"  - Neurons: {brain.getTotalNeuronCount()}")
            print(f"  - Synapses: {brain.getTotalSynapseCount()}")
            print(f"  - Spikes: {brain.getTotalSpikeCount()}")
            return True
        else:
            print("✗ Failed to load checkpoint")
            return False
    except Exception as e:
        print(f"✗ Error loading checkpoint: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(
        description="NLM Advanced Debug and Analysis Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s debug --neurons 1000 --steps 1000 output.json
  %(prog)s validate config.json
  %(prog)s performance --steps 500 brain.bin
  %(prog)s checkpoint save brain.bin
  %(prog)s checkpoint load brain.bin
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Debug command
    debug_parser = subparsers.add_parser('debug', help='Debug brain state')
    debug_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons (default: 100)')
    debug_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps (default: 100)')
    debug_parser.add_argument('--output', '-o', help='Output file for debug data (JSON format)')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate configuration file')
    validate_parser.add_argument('config_file', help='Path to configuration file')
    
    # Performance command
    perf_parser = subparsers.add_parser('performance', help='Analyze performance')
    perf_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps (default: 100)')
    perf_parser.add_argument('--brain-file', help='Path to existing brain file for performance testing')
    
    # Checkpoint commands
    checkpoint_parser = subparsers.add_parser('checkpoint', help='Checkpoint operations')
    checkpoint_subparsers = checkpoint_parser.add_subparsers(dest='action', help='Checkpoint actions')
    
    save_parser = checkpoint_subparsers.add_parser('save', help='Save brain to checkpoint')
    save_parser.add_argument('filepath', help='Path to save checkpoint file')
    
    load_parser = checkpoint_subparsers.add_parser('load', help='Load brain from checkpoint')
    load_parser.add_argument('filepath', help='Path to checkpoint file')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        if args.command == 'debug':
            # Create and run debug brain
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", args.neurons)
            config.set("region_count", 1)
            config.set("connection_probability", 0.1f)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation steps
            for i in range(args.steps):
                brain.step(i)
            
            return debug_brain(brain, args.output)
            
        elif args.command == 'validate':
            return 0 if validate_configuration(args.config_file) else 1
            
        elif args.command == 'performance':
            if args.brain_file:
                # Load existing brain for performance testing
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                if brain.load(args.brain_file):
                    return analyze_performance(brain, args.steps)
                else:
                    print(f"Failed to load brain from {args.brain_file}")
                    return 1
            else:
                # Create new brain for performance testing
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                return analyze_performance(brain, args.steps)
                
        elif args.command == 'checkpoint':
            if not args.action:
                checkpoint_parser.print_help()
                return 1
            
            if args.action == 'save':
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Run some steps to generate activity
                for i in range(100):
                    brain.step(i)
                
                return 0 if save_checkpoint(brain, args.filepath) else 1
                
            elif args.action == 'load':
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                return 0 if load_checkpoint(brain, args.filepath) else 1
                
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
