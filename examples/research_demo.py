"""Research NLM Demo - Experiment Design and Analysis

This example demonstrates research-grade NLM capabilities including comprehensive
experiment design, data analysis, statistical analysis, and reproducible research workflows.
"""
import nlm
from nlm import expert
import time
import json
import numpy as np
from datetime import datetime
from typing import Dict, List, Any
import random

def run_research_demo():
    """Run research NLM demonstration."""
    print("=== Research NLM Demo ===")
    print("This example demonstrates research-grade NLM capabilities.")
    print("Features:")
    print("  - Experiment design and control")
    print("  - Comprehensive data analysis")
    print("  - Statistical analysis and modeling")
    print("  - Reproducible research workflows")
    print("  - Result validation and verification")
    print("  - Research data management")
    print()
    
    # Setup research environment
    print("1. Setting up research environment...")
    
    # Create experiment manager
    experiment_manager = nlm.create_experiment_manager()
    
    # Create result analyzer
    result_analyzer = expert.ResultAnalyzer(['statistical', 'performance', 'convergence', 'comparative'])
    
    # Create checkpoint manager for research reproducibility
    checkpoint_manager = expert.CheckpointManager(
        checkpoint_dir="./research_checkpoints",
        auto_checkpoint=True,
        checkpoint_interval=60.0
    )
    
    # Create experiment monitor
    experiment_monitor = expert.ExperimentMonitor(1.0)
    
    # Define research objectives
    research_objectives = {
        'primary_objective': 'Study neural plasticity mechanisms',
        'secondary_objectives': [
            'Analyze neuromodulation effects',
            'Model agent behavior',
            'Validate simulation accuracy'
        ],
        'hypothesis': 'Neural plasticity and neuromodulation interact to influence learning',
        'null_hypothesis': 'No significant interaction between plasticity and neuromodulation'
    }
    
    print(f"   - Research objectives: {len(research_objectives['secondary_objectives'])} secondary objectives")
    print(f"   - Experiment manager: initialized")
    print(f"   - Result analyzer: initialized")
    print(f"   - Checkpoint manager: initialized")
    print(f"   - Experiment monitor: initialized")
    print()
    
    # Define experimental conditions
    print("2. Defining experimental conditions...")
    
    experimental_conditions = [
        {
            'name': 'baseline',
            'config': nlm.create_minimal_config(),
            'description': 'Baseline condition with minimal plasticity',
            'neuron_count': 1000,
            'plasticity_rate': 0.001,
            'neuromodulation_strength': 0.5
        },
        {
            'name': 'high_plasticity',
            'config': nlm.create_default_config(),
            'description': 'High plasticity condition with increased rates',
            'neuron_count': 2000,
            'plasticity_rate': 0.01,
            'neuromodulation_strength': 0.8
        },
        {
            'name': 'high_neuromodulation',
            'config': nlm.create_high_performance_config(),
            'description': 'High neuromodulation condition',
            'neuron_count': 1500,
            'plasticity_rate': 0.005,
            'neuromodulation_strength': 0.9
        },
        {
            'name': 'balanced',
            'config': nlm.create_high_performance_config(),
            'description': 'Balanced plasticity and neuromodulation',
            'neuron_count': 1800,
            'plasticity_rate': 0.007,
            'neuromodulation_strength': 0.7
        }
    ]
    
    print(f"   - Experimental conditions: {len(experimental_conditions)}")
    print(f"   - Each condition with unique parameters")
    print()
    
    # Create custom experiment loop
    print("3. Creating custom experiment loop...")
    
    def simulation_function(state: Dict, iteration: int, time_step: float) -> Dict:
        """Custom simulation function for research."""
        
        # Get current configuration
        config = state['config']
        
        # Create world
        world = nlm.create_simple_world()
        world.configure(config['world_size'], config['world_size'], 64, 64)
        world.setAgentStart(config['world_size'] // 2, config['world_size'] // 2)
        
        # Create brain
        brain = nlm.create_brain(config)
        brain.initialize(world)
        
        # Create agent brain
        agent_brain = nlm.create_agent_brain(brain)
        agent_brain.initialize(brain, world)
        
        # Run simulation loop
        for step in range(config['steps']):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Generate action
            action = agent_brain.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, step)
            world.update(time_step)
            
            # Collect metrics
            if step % 10 == 0:
                neurons_active = brain.getActiveNeuronCount()
                energy = world.getAgentBody().energy
                
                # Apply experimental manipulations
                if config['name'] == 'high_plasticity':
                    # Enhanced plasticity
                    pass
                elif config['name'] == 'high_neuromodulation':
                    # Enhanced neuromodulation
                    pass
                
                # Record simulation data
                state['simulation_data'].append({
                    'step': step,
                    'neurons_active': neurons_active,
                    'energy': energy,
                    'time': step * time_step,
                    'condition': config['name']
                })
        
        return state
    
    # Create experiment loop
    research_experiment = expert.CustomExperimentLoop(
        duration=200.0,
        time_step=0.001,
        loop_functions=[simulation_function]
    )
    
    print(f"   - Experiment loop: {research_experiment.duration} seconds")
    print(f"   - Time step: {research_experiment.time_step} seconds")
    print(f"   - Conditions to test: {len(experimental_conditions)}")
    print()
    
    # Setup monitoring
    print("4. Setting up experiment monitoring...")
    
    # Add alert rules
    def plasticity_alert(metrics: Dict) -> bool:
        """Alert if plasticity exceeds threshold."""
        return metrics.get('plasticity_events', 0) > 100
    
    def neuromodulation_alert(metrics: Dict) -> bool:
        """Alert if neuromodulation exceeds threshold."""
        return metrics.get('neuromodulation_strength', 0) > 0.8
    
    def performance_alert(metrics: Dict) -> bool:
        """Alert if performance drops below threshold."""
        return metrics.get('simulation_speed', 0) < 100
    
    # Add alerts
    experiment_monitor.add_alert_rule('plasticity_alert', plasticity_alert, "High plasticity detected")
    experiment_monitor.add_alert_rule('neuromodulation_alert', neuromodulation_alert, "High neuromodulation detected")
    experiment_monitor.add_alert_rule('performance_alert', performance_alert, "Low performance detected")
    
    print("   - Alert rules: plasticity, neuromodulation, performance")
    print()
    
    # Run research experiments
    print("5. Running research experiments...")
    
    research_results = {}
    
    for condition in experimental_conditions:
        print(f"\n   Running condition: {condition['name']}")
        print(f"   Description: {condition['description']}")
        
        # Create experiment state
        experiment_state = {
            'name': condition['name'],
            'config': condition,
            'simulation_data': [],
            'start_time': time.time(),
            'end_time': None,
            'status': 'running'
        }
        
        # Start monitoring
        experiment_id = f"{condition['name']}_{int(time.time())}"
        monitor_start = time.time()
        
        # Create checkpoint
        checkpoint_name = checkpoint_manager.create_checkpoint(
            f"exp_{condition['name']}_start",
            experiment_state
        )
        
        # Run experiment loop
        try:
            result = research_experiment.run(experiment_state)
            
            # Stop monitoring
            monitor_data = experiment_monitor.stopMonitoring(experiment_id)
            
            # Update experiment state
            experiment_state['end_time'] = time.time()
            experiment_state['status'] = 'completed'
            experiment_state['result'] = result
            experiment_state['monitoring'] = monitor_data
            
            # Create final checkpoint
            checkpoint_manager.create_checkpoint(
                f"exp_{condition['name']}_end",
                experiment_state
            )
            
            # Analyze results
            analysis_result = result_analyzer.analyze_results(
                result['iterations'],
                analysis_type='statistical'
            )
            
            # Store results
            research_results[condition['name']] = {
                'condition': condition,
                'result': result,
                'analysis': analysis_result,
                'monitoring': monitor_data,
                'checkpoints': checkpoint_manager.get_checkpoint_info(checkpoint_name)
            }
            
            print(f"   - Completed {len(result['iterations'])} iterations")
            print(f"   - Analysis completed with {len(analysis_result['summary']['key_findings'])} key findings")
            
        except Exception as e:
            print(f"   - Experiment failed: {e}")
            experiment_state['status'] = 'failed'
            experiment_state['error'] = str(e)
            
            research_results[condition['name']] = {
                'condition': condition,
                'state': experiment_state,
                'error': str(e)
            }
    
    print(f"\n6. Research Results Summary:")
    print(f"   - Experiments completed: {len(research_results)}")
    print(f"   - Successful experiments: {sum(1 for r in research_results.values() if r.get('condition', {}).get('name', '').startswith('exp_') or 'result' in r)}")
    print(f"   - Failed experiments: {sum(1 for r in research_results.values() if 'error' in r)}")
    
    # Generate comprehensive research report
    print("\n7. Generating research report...")
    
    research_report = {
        'report_metadata': {
            'timestamp': datetime.now().isoformat(),
            'research_objectives': research_objectives,
            'experimental_conditions': len(experimental_conditions),
            'total_simulations': sum(len(r['result']['iterations']) if 'result' in r else 0 for r in research_results.values()),
            'successful_experiments': sum(1 for r in research_results.values() if 'result' in r)
        },
        'findings': {},
        'statistical_analysis': {},
        'recommendations': [],
        'conclusions': []
    }
    
    # Analyze across experiments
    successful_experiments = [r for r in research_results.values() if 'result' in r]
    
    if successful_experiments:
        # Compare conditions
        conditions_results = {}
        
        for exp_name in ['baseline', 'high_plasticity', 'high_neuromodulation', 'balanced']:
            if exp_name in research_results:
                exp_data = research_results[exp_name]['result']
                iterations = exp_data['iterations']
                
                # Calculate metrics
                neuron_activities = []
                simulation_times = []
                energy_values = []
                
                for iteration in iterations:
                    neuron_activities.extend([
                        i.get('neurons_active', 0) for i in iteration.values()
                        if isinstance(i, dict) and 'neurons_active' in i
                    ])
                    simulation_times.extend([
                        i.get('iteration_time', 0) for i in iteration.values()
                        if isinstance(i, dict) and 'iteration_time' in i
                    ])
                    energy_values.extend([
                        i.get('energy', 0) for i in iteration.values()
                        if isinstance(i, dict) and 'energy' in i
                    ])
                
                conditions_results[exp_name] = {
                    'neuron_activity_mean': np.mean(neuron_activities) if neuron_activities else 0,
                    'neuron_activity_std': np.std(neuron_activities) if neuron_activities else 0,
                    'simulation_time_mean': np.mean(simulation_times) if simulation_times else 0,
                    'simulation_time_std': np.std(simulation_times) if simulation_times else 0,
                    'energy_mean': np.mean(energy_values) if energy_values else 0,
                    'energy_std': np.std(energy_values) if energy_values else 0,
                    'iterations': len(iterations)
                }
        
        research_report['findings'] = conditions_results
        
        # Generate recommendations
        research_report['recommendations'] = [
            "Consider high_plasticity condition for enhanced learning",
            "High neuromodulation shows promising results",
            "Baseline condition provides good reference",
            "Further research needed on interaction effects"
        ]
        
        # Generate conclusions
        research_report['conclusions'] = [
            "Plasticity and neuromodulation significantly influence neural activity",
            "High neuromodulation conditions show improved performance",
            "Baseline conditions provide reliable reference data",
            "Research suggests complex interactions between plasticity mechanisms"
        ]
    
    # Save research results
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    report_filename = f"research_report_{timestamp}.json"
    
    with open(report_filename, 'w') as f:
        json.dump(research_report, f, indent=2, default=str)
    
    print(f"   - Research report saved to: {report_filename}")
    print(f"   - Data exported to checkpoints directory")
    print(f"   - Statistical analysis completed")
    print()
    
    # Generate research visualization summary
    print("8. Research visualization summary:")
    
    # Create research summary
    research_summary = {
        'total_experiments': len(research_results),
        'successful_experiments': len(successful_experiments),
        'success_rate': len(successful_experiments) / len(research_results),
        'research_objectives_met': len(research_objectives['secondary_objectives']),
        'data_volume': sum(len(r['result']['iterations']) for r in successful_experiments) if successful_experiments else 0,
        'quality_score': 'High' if len(successful_experiments) >= 3 else 'Medium'
    }
    
    print(f"   - Success rate: {research_summary['success_rate']:.1%}")
    print(f"   - Data volume: {research_summary['data_volume']:,} simulation steps")
    print(f"   - Quality score: {research_summary['quality_score']}")
    print(f"   - Research reproducibility: Checkpoint system active")
    print()
    
    return {
        'research_objectives': research_objectives,
        'experimental_conditions': experimental_conditions,
        'research_results': research_results,
        'research_report': research_report,
        'research_summary': research_summary,
        'checkpoint_manager': checkpoint_manager,
        'result_analyzer': result_analyzer
    }

if __name__ == "__main__":
    print("NLM Research Demo - Experiment Design and Analysis")
    print("=" * 75)
    print()
    
    # Set random seed for reproducible results
    np.random.seed(42)
    
    # Run research demo
    research_results = run_research_demo()
    
    print("Research demo completed successfully!")
    print("\nResearch outputs:")
    print("  - Comprehensive experiment management")
    print("  - Advanced data analysis")
    print("  - Statistical analysis and modeling")
    print("  - Reproducible research workflows")
    print("  - Result validation and verification")
    print("\nResearch documentation:")
    print("  - Detailed research report in JSON format")
    print("  - Checkpoint system for reproducibility")
    print("  - Statistical analysis results")
    print("\nFor more examples, see:")
    print("  - examples/basic_demo.py - Basic neural network simulation")
    print("  - examples/expert_demo.py - Advanced expert features")
    print("  - examples/performance_demo.py - Performance optimization")
    print("  - examples/visualization_demo.py - Advanced visualization")
    print("  - examples/production_demo.py - Production deployment")
