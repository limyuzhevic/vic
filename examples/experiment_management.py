"""
NLM (Neural Learning Machine) - Experiment Management Example

This example demonstrates experiment management techniques for setting up,
running, and managing multiple neural learning experiments efficiently.

Author: NLM Research Team
Version: 6.0.0
"""

import pynlm
import json
import time
from typing import List, Dict, Any

def create_experiment_setup(experiment_type: str, parameters: Dict[str, Any] = None):
    """Create a complete experiment setup with brain, world, and agent"""
    if parameters is None:
        parameters = {}
    
    print(f"\nCreating {experiment_type} experiment setup...")
    
    # Initialize experiment setup
    setup = {
        'type': experiment_type,
        'parameters': parameters,
        'timestamp': time.time(),
        'brain': None,
        'world': None,
        'agent': None,
        'config': None
    }
    
    # Create configuration based on experiment type
    if experiment_type == "basic_learning":
        setup['config'] = pynlm.createDefaultConfig()
        setup['config'].set("brain.neuron_count", parameters.get('neuron_count', 1000))
        setup['config'].set("brain.region_count", parameters.get('region_count', 1))
        setup['config'].set("brain.connection_probability", parameters.get('connection_probability', 0.1))
        
        # Enable basic learning features
        setup['enable_reward'] = parameters.get('enable_reward', True)
        setup['enable_curiosity'] = parameters.get('enable_curiosity', False)
        
    elif experiment_type == "structural_plasticity":
        setup['config'] = pynlm.create_advanced_config()
        setup['config'].set("brain.neuron_count", parameters.get('neuron_count', 2000))
        setup['config'].set("brain.region_count", parameters.get('region_count', 2))
        setup['config'].set("brain.connection_probability", parameters.get('connection_probability', 0.05))
        
        # Enable structural plasticity
        setup['enable_structural'] = parameters.get('enable_structural', True)
        setup['enable_development'] = parameters.get('enable_development', True)
        
    elif experiment_type == "advanced_learning":
        setup['config'] = pynlm.create_advanced_config()
        setup['config'].set("brain.neuron_count", parameters.get('neuron_count', 3000))
        setup['config'].set("brain.region_count", parameters.get('region_count', 3))
        setup['config'].set("plasticity.stdp.learning_rate", parameters.get('stdp_rate', 0.002))
        
        # Enable all advanced features
        setup['enable_reward'] = True
        setup['enable_structural'] = True
        setup['enable_curiosity'] = True
        setup['enable_development'] = True
        
    elif experiment_type == "development_study":
        plasticity_rate = parameters.get('plasticity_rate', 1.0)
        novelty_rate = parameters.get('novelty_rate', 1.0)
        
        setup['config'] = pynlm.create_development_config(plasticity_rate, novelty_rate)
        setup['config'].set("brain.neuron_count", parameters.get('neuron_count', 2500))
        setup['config'].set("development.initial_plasticity", parameters.get('initial_plasticity', 1.5))
        
        # Focus on development
        setup['enable_development'] = True
        setup['enable_reward'] = False  # No external rewards in development study
        
    else:
        # Default configuration
        setup['config'] = pynlm.createDefaultConfig()
        setup['config'].set("brain.neuron_count", parameters.get('neuron_count', 1000))
    
    # Create brain
    setup['brain'] = pynlm.createBrain(setup['config'])
    setup['brain'].initialize()
    
    # Create world
    setup['world'] = pynlm.createSimpleWorld()
    setup['world'].configure(
        width=parameters.get('world_width', 20),
        height=parameters.get('world_height', 20),
        visionWidth=parameters.get('vision_width', 8),
        visionHeight=parameters.get('vision_height', 8)
    )
    setup['world'].reset()
    setup['world'].setAgentStart(
        parameters.get('agent_start_x', 10),
        parameters.get('agent_start_y', 10)
    )
    
    # Create agent brain
    setup['agent'] = pynlm.createAgentBrain(setup['brain'])
    setup['agent'].initialize(*setup['world'])
    
    # Enable features based on experiment type
    if setup.get('enable_reward', False):
        setup['agent'].enableRewardModulation(True)
    
    if setup.get('enable_structural', False):
        setup['agent'].enableStructuralPlasticity(True)
    
    if setup.get('enable_curiosity', False):
        setup['agent'].enableCuriosity(True)
    
    if setup.get('enable_development', False):
        setup['agent'].enableDevelopment(True)
    
    print(f"  ✓ {experiment_type} experiment setup created")
    print(f"    - Brain: {setup['brain'].getTotalNeuronCount()} neurons")
    print(f"    - World: {setup['world'].getWidth()}x{setup['world'].getHeight()} cells")
    print(f"    - Agent: Ready for simulation")
    
    return setup

def run_experiment(setup: Dict[str, Any], num_steps: int, timestep: float = 0.1):
    """Run a single experiment and collect results"""
    print(f"\nRunning {setup['type']} experiment for {num_steps} steps...")
    
    episode_start_time = time.time()
    results = {
        'experiment_type': setup['type'],
        'parameters': setup['parameters'],
        'start_time': episode_start_time,
        'end_time': None,
        'duration': None,
        
        # Performance metrics
        'steps_completed': num_steps,
        'final_step': num_steps - 1,
        
        # Neural activity
        'total_spikes': setup['brain'].getTotalSpikeCount(),
        'firing_neurons': setup['brain'].getFiringNeuronCount(),
        'average_firing_rate': setup['brain'].getAverageFiringRate(),
        'excitation_inhibition_ratio': setup['brain'].getExcitationInhibitionRatio(),
        
        # Learning metrics
        'learning_rate': 0.0,  # Will be calculated from progress
        'development_stage': setup['brain'].getDevelopmentalStage(),
        
        # Agent metrics
        'curiosity_level': setup['agent'].getCuriosityLevel() if hasattr(setup['agent'], 'getCuriosityLevel') else 0.0,
        'novelty_level': setup['agent'].getNoveltyLevel() if hasattr(setup['agent'], 'getNoveltyLevel') else 0.0,
        'reward_modulation': setup['agent'].isRewardModulationEnabled() if hasattr(setup['agent'], 'isRewardModulationEnabled') else False,
        
        # Final state
        'final_brain_state': {
            'neuron_count': setup['brain'].getTotalNeuronCount(),
            'region_count': setup['brain'].getRegionCount(),
            'synapse_count': setup['brain'].getTotalSynapseCount()
        },
        
        'end_time': time.time(),
        'duration': time.time() - episode_start_time
    }
    
    # Run simulation steps
    print(f"  Running simulation...")
    for step in range(num_steps):
        # Update world
        setup['world'].update(timestep)
        
        # Get sensory input and process
        percept = setup['world'].getSensoryPercept()
        setup['agent'].processSensoryInput(percept)
        
        # Run brain step
        setup['brain'].step(step)
        
        # Get action and apply
        action = setup['agent'].decodeMotorCommand()
        setup['world'].applyMotorCommand(action, setup['world'].getSimulationTime())
        
        # Apply reward modulation if enabled
        if setup.get('enable_reward', False):
            # Simple reward based on agent position
            reward = 1.0 if setup['world'].getAgentBody().energy > 50 else 0.0
            setup['agent'].applyRewardModulation(reward, 0.0)
        
        # Update development if enabled
        if setup.get('enable_development', False):
            setup['agent'].updateDevelopment(timestep)
        
        # Print progress every 25% completion
        if (step + 1) % (num_steps // 4) == 0:
            progress = (step + 1) / num_steps * 100
            print(f"    Progress: {progress:.0f}% - "
                  f"Learning: {setup['brain'].getAverageFiringRate():.3f} Hz")
    
    # Calculate final learning metrics
    learning_metrics = pynlm.analyze_learning_progress(setup['brain'], min(10, num_steps // 10))
    results['learning_rate'] = learning_metrics.totalLearning
    
    # Calculate performance score
    performance_score = (
        learning_metrics.totalLearning * 0.4 +
        setup['brain'].getAverageFiringRate() / 20.0 * 0.3 +
        min(setup['brain'].getTotalSpikeCount() / 100.0, 1.0) * 0.3
    )
    results['performance_score'] = performance_score
    
    # Categorize performance
    if performance_score > 0.8:
        results['performance_category'] = 'EXCELLENT'
    elif performance_score > 0.6:
        results['performance_category'] = 'GOOD'
    elif performance_score > 0.4:
        results['performance_category'] = 'ACCEPTABLE'
    else:
        results['performance_category'] = 'NEEDS IMPROVEMENT'
    
    print(f"  ✓ Experiment completed in {results['duration']:.1f} seconds")
    print(f"    Performance score: {performance_score:.3f} ({results['performance_category']})")
    
    return results

def run_experiment_batch(experiment_configs: List[Dict[str, Any]]):
    """Run a batch of experiments and collect results"""
    print(f"\nRunning batch of {len(experiment_configs)} experiments...")
    
    all_results = []
    
    for i, config in enumerate(experiment_configs):
        print(f"\n{'='*60}")
        print(f"Experiment {i + 1}/{len(experiment_configs)}: {config['type']}")
        print(f"{'='*60}")
        
        # Create experiment setup
        setup = create_experiment_setup(
            config['type'],
            config.get('parameters', {})
        )
        
        # Run experiment
        results = run_experiment(
            setup,
            config.get('num_steps', 100),
            config.get('timestep', 0.1)
        )
        
        all_results.append(results)
    
    return all_results

def analyze_batch_results(results: List[Dict[str, Any]]):
    """Analyze batch experiment results"""
    print(f"\nBatch Experiment Analysis")
    print("=" * 60)
    
    if not results:
        print("No results to analyze")
        return
    
    # Basic statistics
    experiment_types = {}
    for result in results:
        exp_type = result['experiment_type']
        if exp_type not in experiment_types:
            experiment_types[exp_type] = []
        experiment_types[exp_type].append(result)
    
    print(f"\nExperiments by Type:")
    print("-" * 50)
    
    for exp_type, exp_results in experiment_types.items():
        print(f"{exp_type}:")
        print(f"  Count: {len(exp_results)}")
        print(f"  Avg performance score: {sum(r['performance_score'] for r in exp_results) / len(exp_results):.3f}")
        print(f"  Avg learning rate: {sum(r['learning_rate'] for r in exp_results) / len(exp_results):.3f}")
        print(f"  Avg firing rate: {sum(r['average_firing_rate'] for r in exp_results) / len(exp_results):.3f} Hz")
        print()
    
    # Overall statistics
    print(f"\nOverall Batch Statistics:")
    print("-" * 40)
    
    all_performance = [r['performance_score'] for r in results]
    all_learning = [r['learning_rate'] for r in results]
    all_firing = [r['average_firing_rate'] for r in results]
    
    print(f"Total experiments: {len(results)}")
    print(f"Average performance score: {statistics.mean(all_performance):.3f}")
    print(f"Performance std deviation: {statistics.stdev(all_performance) if len(all_performance) > 1 else 0:.3f}")
    
    print(f"Average learning rate: {statistics.mean(all_learning):.3f}")
    print(f"Learning std deviation: {statistics.stdev(all_learning) if len(all_learning) > 1 else 0:.3f}")
    
    print(f"Average firing rate: {statistics.mean(all_firing):.3f} Hz")
    print(f"Firing rate std deviation: {statistics.stdev(all_firing) if len(all_firing) > 1 else 0:.3f} Hz")
    
    # Performance ranking
    print(f"\nPerformance Ranking:")
    print("-" * 30)
    
    ranked = sorted(results, key=lambda x: x['performance_score'], reverse=True)
    
    for i, result in enumerate(ranked, 1):
        print(f"{i}. {result['experiment_type']:20} Score: {result['performance_score']:6.3f} "
              f"({result['performance_category']})")
    
    # Best and worst performers
    best = ranked[0]
    worst = ranked[-1]
    
    print(f"\nBest Performer: {best['experiment_type']}")
    print(f"  Performance score: {best['performance_score']:.3f}")
    print(f"  Learning rate: {best['learning_rate']:.3f}")
    print(f"  Firing rate: {best['average_firing_rate']:.3f} Hz")
    
    print(f"\nWorst Performer: {worst['experiment_type']}")
    print(f"  Performance score: {worst['performance_score']:.3f}")
    print(f"  Learning rate: {worst['learning_rate']:.3f}")
    print(f"  Firing rate: {worst['average_firing_rate']:.3f} Hz")

import statistics

def main():
    print("NLM Experiment Management Example")
    print("=" * 70)
    print("This example demonstrates experiment management techniques")
    print("for setting up, running, and managing multiple neural learning experiments.")
    
    # Define experiment configurations
    experiment_configs = [
        {
            'type': 'basic_learning',
            'parameters': {
                'neuron_count': 1000,
                'region_count': 1,
                'connection_probability': 0.1,
                'enable_reward': True,
                'enable_curiosity': False,
                'world_width': 10,
                'world_height': 10
            },
            'num_steps': 100,
            'timestep': 0.1
        },
        {
            'type': 'structural_plasticity',
            'parameters': {
                'neuron_count': 2000,
                'region_count': 2,
                'connection_probability': 0.05,
                'enable_structural': True,
                'enable_development': True,
                'world_width': 15,
                'world_height': 15
            },
            'num_steps': 150,
            'timestep': 0.1
        },
        {
            'type': 'advanced_learning',
            'parameters': {
                'neuron_count': 3000,
                'region_count': 3,
                'stdp_rate': 0.002,
                'enable_reward': True,
                'enable_structural': True,
                'enable_curiosity': True,
                'enable_development': True,
                'world_width': 20,
                'world_height': 20
            },
            'num_steps': 200,
            'timestep': 0.1
        },
        {
            'type': 'development_study',
            'parameters': {
                'neuron_count': 2500,
                'plasticity_rate': 1.2,
                'novelty_rate': 1.5,
                'initial_plasticity': 1.8,
                'world_width': 12,
                'world_height': 12
            },
            'num_steps': 180,
            'timestep': 0.1
        }
    ]
    
    # Create single experiment
    print(f"\n{'='*70}")
    print("1. Creating and running single experiment")
    print(f"{'='*70}")
    
    # Create a development study experiment
    single_setup = create_experiment_setup(
        'development_study',
        {
            'neuron_count': 2000,
            'plasticity_rate': 1.0,
            'novelty_rate': 1.2,
            'world_width': 10,
            'world_height': 10
        }
    )
    
    # Run single experiment
    single_results = run_experiment(single_setup, 120)
    
    # Create and run batch experiments
    print(f"\n{'='*70}")
    print("2. Running batch of experiments")
    print(f"{'='*70}")
    
    batch_results = run_experiment_batch(experiment_configs)
    
    # Analyze all results
    all_results = [single_results] + batch_results
    analyze_batch_results(all_results)
    
    # Save results to file
    print(f"\n3. Saving experiment results...")
    print("-" * 30)
    
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    filename = f"experiment_results_{timestamp}.json"
    
    # Convert results to JSON-serializable format
    json_results = []
    for result in all_results:
        json_results.append({
            'experiment_type': result['experiment_type'],
            'parameters': result['parameters'],
            'performance_score': result['performance_score'],
            'learning_rate': result['learning_rate'],
            'average_firing_rate': result['average_firing_rate'],
            'performance_category': result['performance_category'],
            'duration': result['duration']
        })
    
    # Save to file
    with open(filename, 'w') as f:
        json.dump(json_results, f, indent=2)
    
    print(f"  Results saved to: {filename}")
    print(f"  Total experiments: {len(all_results)}")
    print(f"  Total experiments analyzed: {len(json_results)}")
    
    # Generate summary
    print(f"\n4. Experiment Summary")
    print("-" * 50)
    
    total_score = sum(r['performance_score'] for r in all_results)
    avg_score = total_score / len(all_results)
    
    print(f"Total experiments run: {len(all_results)}")
    print(f"Average performance score: {avg_score:.3f}")
    
    excellent = sum(1 for r in all_results if r['performance_category'] == 'EXCELLENT')
    good = sum(1 for r in all_results if r['performance_category'] == 'GOOD')
    acceptable = sum(1 for r in all_results if r['performance_category'] == 'ACCEPTABLE')
    needs_work = sum(1 for r in all_results if r['performance_category'] == 'NEEDS WORK')
    
    print(f"Performance breakdown:")
    print(f"  Excellent: {excellent} ({excellent/len(all_results)*100:.1f}%)")
    print(f"  Good: {good} ({good/len(all_results)*100:.1f}%)")
    print(f"  Acceptable: {acceptable} ({acceptable/len(all_results)*100:.1f}%)")
    print(f"  Needs work: {needs_work} ({needs_work/len(all_results)*100:.1f}%)")
    
    print("\n✅ Experiment management example completed successfully!")
    print("\nKey features demonstrated:")
    print("  • Experiment setup and configuration")
    print("  • Single experiment execution")
    print("  • Batch experiment management")
    print("  • Results analysis and comparison")
    print("  • Performance ranking")
    print("  • Results persistence (JSON save)")

if __name__ == "__main__":
    main()
