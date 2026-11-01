"""
NLM Advanced Usage Examples - Advanced patterns and integration examples
"""

import sys
import json
import time
from pathlib import Path
sys.path.insert(0, '.')

try:
    from scripts.experiments import ExperimentRunner, ExperimentConfig
    print("Experiment pipeline tools imported successfully")
except ImportError as e:
    print(f"Failed to import experiment pipeline tools: {e}")
    print("Make sure scripts/experiments.py exists")
    sys.exit(1)

def advanced_experiment_pipeline():
    """Example: Advanced experiment pipeline with multiple stages"""
    print("\n=== Advanced Experiment Pipeline ===")
    
    # Create experiment runner
    runner = ExperimentRunner("experiment_pipeline")
    
    # Define experiment pipeline stages
    pipeline_stages = [
        {
            'name': 'baseline_stage',
            'description': 'Baseline experiment without enhancements',
            'brain_config': {
                'neuron_count': 500,
                'region_count': 2,
                'plasticity_enabled': False,
                'development_enabled': False,
                'novelty_enabled': False,
                'curiosity_enabled': False,
                'memory_enabled': False,
                'prediction_enabled': False,
                'cognition_enabled': False
            },
            'world_config': {
                'width': 100.0,
                'height': 100.0,
                'max_energy': 100.0
            },
            'duration': 50,
            'steps_per_iteration': 5
        },
        {
            'name': 'intermediate_stage',
            'description': 'Intermediate experiment with partial enhancements',
            'brain_config': {
                'neuron_count': 1000,
                'region_count': 4,
                'plasticity_enabled': True,
                'development_enabled': True,
                'novelty_enabled': False,
                'curiosity_enabled': False,
                'memory_enabled': True,
                'prediction_enabled': True,
                'cognition_enabled': False
            },
            'world_config': {
                'width': 200.0,
                'height': 200.0,
                'max_energy': 200.0,
                'vision_width': 40,
                'vision_height': 40
            },
            'duration': 100,
            'steps_per_iteration': 10
        },
        {
            'name': 'advanced_stage',
            'description': 'Advanced experiment with all enhancements',
            'brain_config': {
                'neuron_count': 2000,
                'region_count': 8,
                'plasticity_enabled': True,
                'development_enabled': True,
                'novelty_enabled': True,
                'curiosity_enabled': True,
                'memory_enabled': True,
                'prediction_enabled': True,
                'cognition_enabled': True,
                'neural_planner_enabled': True,
                'concept_formation_enabled': True,
                'attentional_selection_enabled': True,
                'self_model_enabled': True,
                'social_learning_enabled': True,
                'continual_learning_enabled': True
            },
            'world_config': {
                'width': 400.0,
                'height': 400.0,
                'max_energy': 400.0,
                'energy_decay_rate': 0.002,
                'vision_width': 80,
                'vision_height': 80,
                'num_resources': 100,
                'num_hazards': 40,
                'agent_start_x': 200.0,
                'agent_start_y': 200.0
            },
            'duration': 200,
            'steps_per_iteration': 15
        }
    ]
    
    # Create experiments
    print("1. Creating experiment pipeline stages...")
    experiment_configs = []
    
    for stage_config in pipeline_stages:
        config = ExperimentConfig(
            name=stage_config['name'],
            description=stage_config['description'],
            brain_config=stage_config['brain_config'],
            world_config=stage_config['world_config'],
            duration=stage_config['duration'],
            steps_per_iteration=stage_config['steps_per_iteration'],
            save_checkpoints=True,
            checkpoint_interval=max(1, stage_config['duration'] // 5),
            metrics_to_record=[
                'activity_level', 'plasticity_change', 'memory_utilization',
                'reward_signal', 'novelty_detected', 'energy_consumption',
                'working_memory_usage', 'episodic_memory_recall',
                'prediction_accuracy', 'planning_confidence', 'concept_clarity'
            ],
            parallel_runs=1,
            max_memory_usage=2048.0
        )
        
        experiment_configs.append(config)
        
        if runner.create_experiment(config):
            print(f"   ✓ Created: {config.name}")
        else:
            print(f"   ✗ Failed to create: {config.name}")
    
    # Create experiment directory structure
    print("\n2. Creating experiment directory structure...")
    pipeline_dir = Path("experiment_pipeline")
    pipeline_dir.mkdir(exist_ok=True)
    
    # Create stage directories
    for stage_config in pipeline_stages:
        stage_dir = pipeline_dir / stage_config['name']
        stage_dir.mkdir(exist_ok=True)
        
        # Create subdirectories for different data types
        (stage_dir / "checkpoints").mkdir(exist_ok=True)
        (stage_dir / "metrics").mkdir(exist_ok=True)
        (stage_dir / "visualizations").mkdir(exist_ok=True)
        
        print(f"   ✓ Created structure for {stage_config['name']}")
    
    # Run experiments in sequence (pipeline approach)
    print("\n3. Running experiment pipeline...")
    
    total_start_time = time.time()
    stage_results = []
    
    for config in experiment_configs:
        print(f"\nExecuting stage: {config.name}")
        stage_start = time.time()
        
        result = runner.run_experiment(config.name)
        stage_results.append(result)
        
        stage_duration = time.time() - stage_start
        print(f"   ✓ Stage completed in {stage_duration:.2f} seconds")
        
        if result.success:
            print(f"   ✓ Success: {len(result.checkpoints)} checkpoints, {len(result.metrics.get('metrics', {}))} metrics")
        else:
            print(f"   ✗ Failed: {result.error_message}")
    
    total_duration = time.time() - total_start_time
    print(f"\nPipeline completed in {total_duration:.2f} seconds")
    
    # Analyze pipeline results
    print("\n4. Analyzing pipeline results...")
    
    # Create pipeline summary
    pipeline_summary = {
        'pipeline_name': 'NLM Experiment Pipeline',
        'total_stages': len(pipeline_stages),
        'successful_stages': sum(1 for r in stage_results if r.success),
        'total_duration': total_duration,
        'stages': []
    }
    
    for i, (config, result) in enumerate(zip(experiment_configs, stage_results)):
        stage_analysis = {
            'stage_number': i + 1,
            'name': config.name,
            'description': config.description,
            'duration': (result.end_time - result.start_time).total_seconds() if result.success else 0,
            'success': result.success,
            'checkpoints': len(result.checkpoints),
            'metrics': len(result.metrics.get('metrics', {})) if result.success else 0,
            'performance': result.performance_stats if result.success else {}
        }
        pipeline_summary['stages'].append(stage_analysis)
    
    # Save pipeline summary
    summary_file = pipeline_dir / "pipeline_summary.json"
    with open(summary_file, 'w') as f:
        json.dump(pipeline_summary, f, indent=2, default=str)
    
    print(f"   ✓ Pipeline summary saved: {summary_file}")
    
    # Compare stages
    print("\n5. Comparing pipeline stages...")
    comparison = runner.compare_experiments([config.name for config in experiment_configs])
    
    print("\nPipeline Performance Comparison:")
    for exp_name, exp_data in comparison.items():
        print(f"\n{exp_name}:")
        print(f"  Success: {exp_data['success']}")
        print(f"  Duration: {exp_data['duration']:.2f}s")
        print(f"  Checkpoints: {exp_data['num_checkpoints']}")
    
    # Generate pipeline visualizations
    print("\n6. Generating pipeline visualizations...")
    
    try:
        from scripts.visualization import BrainVisualizer
        visualizer = BrainVisualizer()
        
        # Create stage comparison visualization
        stage_names = [config.name for config in experiment_configs]
        
        # Create sample data for visualization
        stage_data = []
        for exp_name, exp_data in comparison.items():
            stage_data.append({
                'stage': exp_name,
                'success': exp_data['success'],
                'duration': exp_data['duration'],
                'checkpoints': exp_data['num_checkpoints']
            })
        
        print("   ✓ Pipeline visualization data prepared")
        
    except ImportError:
        print("   ✗ Visualization tools not available")
    
    print("\n=== Advanced Experiment Pipeline Complete ===")

if __name__ == "__main__":
    advanced_experiment_pipeline()