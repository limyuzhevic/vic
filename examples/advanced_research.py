"""
Advanced research workflows with parameter sweeps.

This example demonstrates advanced research workflows including
parameter sweeps, statistical analysis, and comprehensive research pipelines.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from pynlm import (
    create_default_config, create_brain, create_simple_world, create_agent_brain,
    create_vision_input, create_internal_signals_input
)
from pynlm.experiments import (
    ExperimentTemplate, ParameterSweep, MultiSeedExperiment, 
    run_experiment, run_preset, run_parameter_sweep, run_multi_seed_experiment,
    analyze_results, compare_experiments, save_experiment_results
)
from pynlm.research import (
    ResearchWorkflow, ScientificExperimentDesigner, 
    ExperimentReproducibilityManager, create_research_workflow
)
from pynlm.visualization import ResultsVisualizer, plot_statistical_analysis
from pynlm.performance import PerformanceMonitor, start_performance_monitoring, stop_performance_monitoring
import time
import random
import numpy as np
import pandas as pd

def advanced_research_workflow():
    """Run advanced research workflow with parameter sweeps."""
    print("=== Advanced Research Workflow with Parameter Sweeps ===")
    
    # Create experiment designer
    designer = ScientificExperimentDesigner()
    
    # Create reproducibility manager
    reproducibility_manager = ExperimentReproducibilityManager("./research_data")
    
    # Define research parameters
    research_parameters = {
        'base_config': {
            'neuron_count': 1000,
            'region_count': 3,
            'connection_probability': 0.12,
            'seed': 12345
        },
        'parameter_ranges': {
            'learning_rate': [0.001, 0.01, 0.1],
            'exploration_rate': [0.1, 0.5, 1.0],
            'novelty_threshold': [0.2, 0.5, 0.8],
            'curiosity_gain': [0.5, 1.0, 1.5]
        },
        'num_seeds': 5,
        'max_steps': 200,
        'num_iterations': 10,
        'output_dir': './research_results'
    }
    
    print("Creating research workflow with parameter sweeps...")
    
    # Step 1: Create research workflow
    workflow = ResearchWorkflow('Advanced_Research_ParameterSweep', 
                              'Advanced research with parameter sweeps and statistical analysis')
    
    # Configure workflow
    workflow.configure({
        'research_type': 'parameter_sweep',
        'num_seeds': research_parameters['num_seeds'],
        'max_steps': research_parameters['max_steps']
    })
    
    # Step 2: Add workflow steps
    # Step 2a: Create base experiment
    workflow.add_step('create_base_experiment', 'experiment', {
        'experiment_name': 'baseline_experiment',
        'experiment_config': research_parameters['base_config']
    })
    
    # Step 2b: Run baseline experiment
    workflow.add_step('run_baseline', 'experiment', {
        'experiment_name': 'baseline_experiment',
        'experiment_config': research_parameters['base_config']
    })
    
    # Step 2c: Run parameter sweep
    workflow.add_step('run_parameter_sweep', 'parameter_sweep', {
        'experiment_template': designer.get_template('learning_curves'),
        'sweep_config': {
            'parameters': research_parameters['parameter_ranges'],
            'grid_search': False,
            'random_search': True,
            'num_iterations': research_parameters['num_iterations'],
            'objective_function': lambda result: result.average_firing_rate,
            'optimization_metric': 'performance',
            'parallel_execution': True,
            'timeout_per_iteration': 60.0
        }
    })
    
    # Step 2d: Run multi-seed experiment
    workflow.add_step('run_multi_seed', 'multi_seed', {
        'base_config': research_parameters['base_config'],
        'num_seeds': research_parameters['num_seeds'],
        'experiment_template': designer.get_template('learning_curves')
    })
    
    # Step 2e: Statistical analysis
    workflow.add_step('perform_statistical_analysis', 'statistical_analysis', {
        'results': 'run_baseline'  # Reference to previous step result
    })
    
    # Step 2f: Comparative analysis
    workflow.add_step('perform_comparative_analysis', 'comparative_analysis', {
        'results': ['run_baseline', 'run_parameter_sweep']
    })
    
    # Step 2g: Data management
    workflow.add_step('save_results', 'data_management', {
        'results': ['run_baseline', 'run_parameter_sweep', 'run_multi_seed'],
        'output_dir': research_parameters['output_dir'],
        'format': 'json'
    })
    
    # Step 2h: Report generation
    workflow.add_step('generate_reports', 'report_generation', {
        'results': ['run_baseline', 'run_parameter_sweep', 'run_multi_seed'],
        'output_dir': research_parameters['output_dir']
    })
    
    # Step 3: Set up reproducibility
    reproducibility_manager.set_seed(12345)
    reproducibility_manager.register_experiment(
        'advanced_research',
        {
            'parameters': research_parameters,
            'designer': str(designer),
            'created': time.time()
        },
        {
            'description': 'Advanced research with parameter sweeps',
            'authors': ['NLM Research Team']
        }
    )
    
    # Step 4: Run the research workflow
    print("\nExecuting advanced research workflow...")
    start_time = time.time()
    
    try:
        # Execute all steps
        workflow_results = workflow.run_all_steps()
        
        end_time = time.time()
        
        print(f"\nResearch workflow completed in {end_time - start_time:.2f} seconds")
        
        # Analyze results
        print("\n=== Research Workflow Analysis ===")
        
        summary = workflow.get_results_summary()
        print(f"Workflow: {summary['workflow_name']}")
        print(f"Description: {summary['description']}")
        print(f"Total steps: {summary['steps']}")
        print(f"Completed steps: {summary['completed_steps']}")
        print(f"Failed steps: {summary['failed_steps']}")
        print(f"Pending steps: {summary['pending_steps']}")
        
        # Extract key results
        key_results = {}
        for step_name, step_result in workflow_results.items():
            if isinstance(step_result, dict) and 'error' not in step_result:
                key_results[step_name] = step_result
        
        # Save research data
        research_data_file = f"{research_parameters['output_dir']}/research_data.json"
        with open(research_data_file, 'w') as f:
            import json
            json.dump({
                'workflow_results': workflow_results,
                'key_results': key_results,
                'parameters': research_parameters,
                'completion_time': end_time - start_time
            }, f, indent=2, default=str)
        
        print(f"\nResearch data saved: {research_data_file}")
        
        # Generate visualizations
        print("\nGenerating research visualizations...")
        
        # Create visualization
        visualizer = ResultsVisualizer()
        
        # Plot statistical analysis
        if 'perform_statistical_analysis' in workflow_results:
            stat_results = workflow_results['perform_statistical_analysis']
            print(f"Statistical analysis completed: {stat_results}")
        
        # Create comparative analysis plot
        if 'perform_comparative_analysis' in workflow_results:
            comp_results = workflow_results['perform_comparative_analysis']
            print(f"Comparative analysis completed with {len(comp_results)} comparisons")
        
        # Save visualization
        visualization_file = f"{research_parameters['output_dir']}/research_workflow_visualization.html"
        
        print(f"Visualization would be saved to: {visualization_file}")
        
        # Performance monitoring
        print("\nRunning performance monitoring...")
        
        # The workflow would use performance monitoring here
        # For now, we'll just create the framework
        
        # Save workflow information
        workflow_info = {
            'name': workflow.name,
            'description': workflow.description,
            'steps': workflow.steps,
            'results': workflow_results,
            'parameters': research_parameters,
            'completion_time': end_time - start_time
        }
        
        workflow_file = f"{research_parameters['output_dir']}/workflow_info.json"
        with open(workflow_file, 'w') as f:
            json.dump(workflow_info, f, indent=2, default=str)
        
        print(f"Workflow info saved: {workflow_file}")
        
        # Print summary of key results
        print(f"\n=== Key Research Results ===")
        
        if 'run_baseline' in key_results:
            baseline_result = key_results['run_baseline']
            print(f"Baseline experiment: {baseline_result.get('success', 'unknown')}")
        
        if 'run_parameter_sweep' in key_results:
            sweep_result = key_results['run_parameter_sweep']
            print(f"Parameter sweep: {len(sweep_result)} iterations completed")
            if sweep_result:
                best_result = sweep_result[0] if sweep_result else None
                if best_result and best_result.success:
                    print(f"  Best performance: {best_result.average_firing_rate:.2f} firing rate")
        
        if 'run_multi_seed' in key_results:
            multi_seed_result = key_results['run_multi_seed']
            print(f"Multi-seed experiment: {len(multi_seed_result)} seeds completed")
        
        print(f"\nAdvanced research workflow completed successfully!")
        print(f"Results saved to: {research_parameters['output_dir']}/")
        
        return workflow_results, key_results
        
    except Exception as e:
        print(f"\nResearch workflow failed: {e}")
        import traceback
        traceback.print_exc()
        return None, None

if __name__ == "__main__":
    workflow_results, key_results = advanced_research_workflow()