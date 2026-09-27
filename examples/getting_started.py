#!/usr/bin/env python3
"""
NLM Getting Started Example
===========================

Complete beginner example demonstrating the NLM Python API
for creating and running a simple neural simulation.

This example shows:
1. Creating a brain with optimal configuration
2. Running a basic simulation
3. Monitoring brain activity
4. Saving and loading brain states
5. Basic data analysis

Prerequisites:
- NLM C++ bindings must be built and installed
- Python 3.8+ with pynlm module
"""

import pynlm as nlm
import json
import time

def main():
    """Run the complete getting started example."""
    print("=" * 60)
    print("NLM Getting Started Example")
    print("=" * 60)
    
    # 1. Create a simple brain
    print("\n1. Creating brain with optimal configuration...")
    brain = nlm.NLMBrain(neuron_count=500, complexity=0.6)
    brain.initialize()
    
    print(f"   ✓ Brain created: {brain.get_total_neurons()} neurons")
    print(f"   ✓ Brain initialized: {brain.get_total_synapses()} synapses")
    
    # 2. Run basic simulation
    print("\n2. Running basic simulation...")
    simulation_results = []
    
    for step in range(50):
        # Run simulation step
        brain.step(step)
        
        # Collect statistics
        firing_rate = brain.get_average_firing_rate()
        active_neurons = brain.get_active_neurons()
        total_spikes = brain.get_total_spikes()
        
        step_result = {
            'step': step,
            'firing_rate': firing_rate,
            'active_neurons': active_neurons,
            'total_spikes': total_spikes,
            'development_stage': str(brain.get_developmental_stage())
        }
        
        simulation_results.append(step_result)
        
        # Print progress every 10 steps
        if step % 10 == 0:
            print(f"   Step {step:2d}: FR={firing_rate:6.3f} Hz, "
                  f"Active={active_neurons:3d}, Spikes={total_spikes}")
    
    print(f"   ✓ Simulation completed: {len(simulation_results)} steps")
    
    # 3. Analyze simulation results
    print("\n3. Analyzing simulation results...")
    
    # Calculate statistics
    avg_firing_rate = sum(r['firing_rate'] for r in simulation_results) / len(simulation_results)
    max_firing_rate = max(r['firing_rate'] for r in simulation_results)
    avg_active = sum(r['active_neurons'] for r in simulation_results) / len(simulation_results)
    total_spikes = simulation_results[-1]['total_spikes']
    
    # Get development stage progression
    development_stages = list(set(r['development_stage'] for r in simulation_results))
    
    print(f"   Average firing rate: {avg_firing_rate:.3f} Hz")
    print(f"   Maximum firing rate: {max_firing_rate:.3f} Hz")
    print(f"   Average active neurons: {avg_active:.1f}")
    print(f"   Total spikes accumulated: {total_spikes}")
    print(f"   Development stages reached: {development_stages}")
    
    # 4. Save brain state
    print("\n4. Saving brain state...")
    brain.save("getting_started_brain.nlm")
    print(f"   ✓ Brain state saved to 'getting_started_brain.nlm'")
    
    # 5. Load brain state and continue simulation
    print("\n5. Loading brain state and continuing simulation...")
    
    # Create new brain instance
    loaded_brain = nlm.NLMBrain(neuron_count=500, complexity=0.6)
    loaded_brain.load("getting_started_brain.nlm")
    
    # Continue simulation from loaded state
    for additional_step in range(10):
        loaded_brain.step(additional_step + 50)
    
    print(f"   ✓ Brain state loaded and simulation continued")
    print(f"   ✓ Final firing rate: {loaded_brain.get_average_firing_rate():.3f} Hz")
    
    # 6. Create summary
    print("\n6. Creating experiment summary...")
    
    summary = {
        'experiment_name': 'Getting Started Example',
        'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
        'brain_configuration': {
            'neuron_count': 500,
            'complexity': 0.6
        },
        'simulation_results': {
            'steps_completed': len(simulation_results),
            'average_firing_rate': avg_firing_rate,
            'max_firing_rate': max_firing_rate,
            'total_spikes': total_spikes,
            'average_active_neurons': avg_active,
            'development_stages': development_stages
        },
        'next_steps': [
            'Experiment with different neuron counts and complexities',
            'Try different input patterns and injection strategies',
            'Implement custom reward functions for learning',
            'Compare with existing NLM examples',
            'Explore advanced training features'
        ]
    }
    
    # Save summary
    with open('getting_started_summary.json', 'w') as f:
        json.dump(summary, f, indent=2)
    
    print(f"   ✓ Experiment summary saved to 'getting_started_summary.json'")
    
    # 7. Final demonstration
    print("\n7. Final demonstration...")
    print(f"   Brain initialized: {brain.get_total_neurons()} neurons")
    print(f"   Simulation completed: {len(simulation_results)} steps")
    print(f"   Maximum firing rate achieved: {max_firing_rate:.3f} Hz")
    print(f"   Development stages: {', '.join(development_stages)}")
    print(f"   Files created:")
    print(f"     - getting_started_brain.nlm (brain state)")
    print(f"     - getting_started_summary.json (analysis summary)")
    
    print("\n" + "=" * 60)
    print("Getting Started Example Completed Successfully!")
    print("=" * 60)
    print("\nNext Steps:")
    print("1. Run 'python -c \"import pynlm as nlm; brain = nlm.NLMBrain(); brain.initialize()\"'")
    print("2. Explore examples/basic_training.py for training examples")
    print("3. Check documentation in PYTHON_API_GUIDE.md for API details")
    print("4. Run 'pip install pynlm[training]' for additional training dependencies")
    
    return summary

if __name__ == '__main__':
    try:
        result = main()
        print(f"\nExample completed successfully!")
        exit(0)
    except Exception as e:
        print(f"\nError running example: {e}")
        print("Please ensure NLM C++ bindings are built and installed.")
        exit(1)
