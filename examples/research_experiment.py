"""
Research Experiment Example
===========================

This example demonstrates how to set up systematic research experiments with
NLM. It shows how to use configuration presets, run multiple episodes,
collect data, and analyze results - all essential for reproducible research.

## What This Example Shows

1. Using configuration presets for reproducible experiments
2. Setting up experiment management system
3. Running multiple episodes with data collection
4. Analyzing and aggregating experimental results
5. Validating experimental setup and performance

## When to Use This Example

- You're conducting research with NLM
- You need reproducible experimental setups
- You want to run parameter sweeps or hyperparameter optimization
- You need to collect and analyze experimental data
- You're writing research papers or presentations
"""

import pynlm
from advanced_api import AdvancedBrain, InteractiveAgent, NLMExperiment, NLMConfigurator, validate_brain_state, benchmark_brain
import numpy as np

def main():
    """Run the research experiment example"""
    print("=== Research Experiment Example ===")
    print()
    
    # 1. Set up experimental configuration
    print("Step 1: Setting up experimental configuration...")
    print("   Using 'research' preset for standardized setup...")
    
    config = NLMConfigurator.create_config('research')
    brain = AdvancedBrain(config)
    
    print(f"   Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"   Plasticity enabled: {config.get('plasticity.stdp.enable')}")
    print(f"   Curiosity enabled: {config.get('neuromod.curiosity.enable')}")
    
    # 2. Create experimental environment
    print("Step 2: Creating experimental environment...")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Initialize agent
    agent = InteractiveAgent(brain, world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    print(f"   Environment size: {world.getWidth()}x{world.getHeight()}")
    print(f"   Agent starting position: (15.0, 15.0)")
    
    # 3. Set up experiment management
    print("Step 3: Setting up experiment management...")
    
    experiment = NLMExperiment(brain, world, agent)
    
    print("   Experiment configured:")
    print("     - Episodes: 10")
    print("     - Steps per episode: 100")
    print("     - Data collection: Automatic")
    print("     - Result aggregation: Enabled")
    
    # 4. Run experimental trials
    print("Step 4: Running experimental trials...")
    print()
    
    results = experiment.run_experiment(
        episodes=10,
        steps_per_episode=100
    )
    
    # 5. Analyze and report results
    print("Step 5: Analyzing and reporting results...")
    print()
    print("Experimental Results Summary:")
    print(f"  Total episodes: {results['total_episodes']}")
    print(f"  Average neurons: {results['average_neurons']:.1f}")
    print(f"  Average firing rate: {results['average_firing_rate']:.4f}")
    print(f"  Total synaptic activity: {results['total_spikes']}")
    print()
    
    # 6. Experimental insights and recommendations
    print("Step 6: Experimental insights...")
    
    # Validate experimental setup
    issues = validate_brain_state(brain)
    if issues:
        print("  Validation warnings:")
        for issue in issues:
            print(f"    - {issue}")
    else:
        print("  Setup validation: PASSED")
    
    # Performance benchmark
    benchmark = benchmark_brain(brain, 500)
    print(f"  Performance benchmark: {benchmark['steps_per_second']:.1f} steps/sec")
    
    print()
    print("=== Research Experiment Complete ===")
    print()
    print("For reproducible research, save these configuration details:")
    print(f"  Configuration preset: 'research'")
    print(f"  Neuron count: {brain.getTotalNeuronCount()}")
    print(f"  Total episodes: 10")
    print(f"  Steps per episode: 100")
    print()
    print("Experiment data available in experiment.results for detailed analysis")

if __name__ == "__main__":
    main()
