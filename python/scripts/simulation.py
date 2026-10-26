#!/usr/bin/env python3
"""
NLM Simulation Script
=====================

Simple script to run a basic NLM simulation using the enhanced Python bindings.
"""

import sys
import os
import time
import argparse

def main():
    parser = argparse.ArgumentParser(description="Run NLM simulation")
    parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser.add_argument("--output", type=str, default="simulation.log", help="Output log file")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Add the enhanced module path
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
    
    try:
        # Import NLM modules
        import pynlm
        print("NLM Python bindings loaded successfully")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", args.neurons)
        config.set("simulation_timestep", 0.001)
        
        # Create and initialize brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Simulation started with {args.neurons} neurons for {args.steps} steps")
        print(f"Output will be saved to: {args.output}")
        
        # Run simulation
        start_time = time.time()
        
        with open(args.output, 'w') as log_file:
            log_file.write(f"NLM Simulation Log\n")
            log_file.write(f"Started at: {time.ctime()}\n")
            log_file.write(f"Neurons: {args.neurons}\n")
            log_file.write(f"Steps: {args.steps}\n")
            log_file.write("-" * 50 + "\n")
            
            for step in range(args.steps):
                brain.step(step)
                
                # Log statistics
                timestamp = time.time()
                firing_count = brain.getFiringNeuronCount()
                total_spikes = brain.getTotalSpikeCount()
                firing_rate = brain.getAverageFiringRate()
                e_i_ratio = brain.getExcitationInhibitionRatio()
                
                log_entry = f"Step {step:4d}: Firing={firing_count:5d}, TotalSpikes={total_spikes:6d}, "
                           f"FiringRate={firing_rate:.2f}, E/I={e_i_ratio:.2f}, Time={timestamp:.3f}\n"
                
                log_file.write(log_entry)
                
                if args.verbose:
                    print(log_entry.strip())
                
                # Progress indicator
                if (step + 1) % 25 == 0:
                    elapsed = time.time() - start_time
                    print(f"Progress: {step + 1}/{args.steps} steps ({elapsed:.2f}s)")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        print(f"\nSimulation completed in {total_time:.2f} seconds")
        print(f"Final statistics:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        
        return 0
        
    except ImportError as e:
        print(f"Error importing NLM: {e}")
        return 1
    except Exception as e:
        print(f"Error during simulation: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())