#!/usr/bin/env python3
"""
Complete demonstration scripts for NLM Python bindings.

This module contains comprehensive example scripts that demonstrate all aspects
of using the NLM Python API, from basic functionality to advanced scenarios.
"""

import sys
import os
import argparse
from pathlib import Path

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from .core import create_simulation, run_agent_episode, quick_test

def run_all_examples():
    """Run all example scripts."""
    print("Running all NLM examples...")
    print("=" * 60)
    
    # Example 1: Quick test
    print("\n1. Quick Test")
    print("-" * 40)
    try:
        brain = quick_test()
        print("✓ Quick test completed successfully")
    except Exception as e:
        print(f"✗ Quick test failed: {e}")
    
    # Example 2: Basic simulation
    print("\n2. Basic Simulation")
    print("-" * 40)
    try:
        sim = create_simulation()
        brain = sim['brain']
        world = sim['world']
        
        for step in range(20):
            brain.step(step)
        
        print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} spikes")
    except Exception as e:
        print(f"✗ Basic simulation failed: {e}")
    
    # Example 3: Complete agent episode
    print("\n3. Complete Agent Episode")
    print("-" * 40)
    try:
        results = run_agent_episode(steps=50, enable_features=True)
        print(f"✓ Agent episode completed: {results['steps_run']} steps")
        print(f"  Final curiosity: {results['final_stats']['curiosity']:.3f}")
        print(f"  Final firing rate: {results['final_stats']['avg_firing_rate']:.2f} Hz")
    except Exception as e:
        print(f"✗ Agent episode failed: {e}")
    
    # Example 4: Configuration testing
    print("\n4. Configuration Testing")
    print("-" * 40)
    try:
        config_dict = {
            'brain.neuron_count': 300,
            'brain.synapse_density': 0.12,
            'plasticity.stdp.enable': True,
            'plasticity.hebbian.enable': True
        }
        sim = create_simulation(config_dict)
        config = sim['config']
        
        # Verify configuration
        for key, expected_value in config_dict.items():
            if config.has(key):
                print(f"✓ Config {key} = {config.get(key)}")
            else:
                print(f"✗ Config {key} not found")
        
        print(f"✓ Configuration test completed with {len(config_dict)} parameters")
    except Exception as e:
        print(f"✗ Configuration test failed: {e}")
    
    # Example 5: Statistics collection
    print("\n5. Statistics Collection")
    print("-" * 40)
    try:
        sim = create_simulation()
        brain = sim['brain']
        
        # Collect various statistics
        stats = {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        for key, value in stats.items():
            print(f"✓ {key}: {value}")
        
        print("✓ Statistics collection completed")
    except Exception as e:
        print(f"✗ Statistics collection failed: {e}")
    
    print("\n" + "=" * 60)
    print("All examples completed!")

def run_example_scripts():
    """Run the example scripts in the examples directory."""
    examples_dir = Path(__file__).parent / "examples"
    
    if not examples_dir.exists():
        print("Examples directory not found!")
        return
    
    for example_file in examples_dir.glob("*.py"):
        if example_file.name.startswith("__"):
            continue
        
        print(f"\nRunning {example_file.name}...")
        try:
            # Import and run the example script
            spec = importlib.util.spec_from_file_location(
                example_file.stem, example_file
            )
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            
            # Look for a main function
            if hasattr(module, 'main'):
                module.main()
            elif hasattr(module, 'run_example'):
                module.run_example()
            else:
                print(f"  No main or run_example function found in {example_file.name}")
                
        except Exception as e:
            print(f"  Failed to run {example_file.name}: {e}")
            import traceback
            traceback.print_exc()

def main():
    """Main function to run examples."""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings Examples",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        '--mode', choices=['all', 'demo', 'advanced', 'batch'],
        default='all',
        help='Mode of operation (default: all)'
    )
    
    parser.add_argument(
        '--steps', type=int, default=100,
        help='Number of steps for simulation (default: 100)'
    )
    
    parser.add_argument(
        '--output', type=str, default=None,
        help='Output file for results (default: print to console)'
    )
    
    args = parser.parse_args()
    
    print("NLM Python Bindings Examples")
    print("=" * 60)
    
    if args.mode == 'all':
        run_all_examples()
    elif args.mode == 'demo':
        print("\nRunning demonstration mode...")
        sim = create_simulation()
        brain = sim['brain']
        
        for step in range(min(args.steps, 50)):
            brain.step(step)
            if step % 10 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        print("✓ Demonstration completed")
    
    elif args.mode == 'advanced':
        print("\nRunning advanced examples...")
        run_example_scripts()
    
    elif args.mode == 'batch':
        print("\nRunning batch processing...")
        results = []
        
        for i in range(3):
            print(f"\nBatch {i+1}:")
            sim = create_simulation()
            brain = sim['brain']
            
            for step in range(min(args.steps, 30)):
                brain.step(step)
            
            results.append({
                'batch': i + 1,
                'spikes': brain.getTotalSpikeCount(),
                'firing': brain.getFiringNeuronCount(),
                'rate': brain.getAverageFiringRate()
            })
            
            print(f"  Completed with {brain.getTotalSpikeCount()} spikes")
        
        if args.output:
            import json
            with open(args.output, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"\nResults saved to {args.output}")
        
        print(f"\nBatch processing completed: {len(results)} batches")
    
    print("\n" + "=" * 60)
    print("Example execution completed!")

if __name__ == "__main__":
    main()
