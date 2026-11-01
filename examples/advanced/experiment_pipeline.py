"""
NLM Advanced Usage Examples - Advanced patterns and integration examples
"""

# examples/advanced/experiment_management.py
"""
Advanced Experiment Management Example

This example demonstrates advanced experiment management and analysis.
"""

import sys
import json
import time
sys.path.insert(0, '.')

try:
    from scripts.experiments import ExperimentRunner, ExperimentConfig
    print("Experiment management tools imported successfully")
except ImportError as e:
    print(f"Failed to import experiment tools: {e}")
    print("Make sure scripts/experiments.py exists")
    sys.exit(1)

def advanced_experiment_management():
    """Example: Advanced experiment management"""
    print("\n=== Advanced Experiment Management ===")
    
    # Create experiment runner
    runner = ExperimentRunner("advanced_experiments")
    
    # Define configuration for different experiment types
    base_config = ExperimentConfig(
        name="test_experiment",
        description="Test experiment for advanced features",
        duration=50,
        steps_per_iteration=5,
        save_checkpoints=True,
        checkpoint_interval=25,
        metrics_to_record=[
            'activity_level', 'plasticity_change', 'memory_utilization',
            'reward_signal', 'novelty_detected', 'energy_consumption',
            'prediction_accuracy', 'attention_focus', 'planning_efficiency'
        ]
    )
    
    # Modify configuration for different experiment scenarios
    config1 = ExperimentConfig(
        name="baseline_experiment",
        description="Baseline experiment without plasticity",
        brain_config={
            'neuron_count': 1000,
            'region_count': 4,
            'plasticity_enabled': False,
            'development_enabled': False,
            'novelty_enabled': False,
            'curiosity_enabled': False
        },
        world_config={
            'width': 200.0,
            'height': 200.0,
            'max_energy': 200.0
        },
        **{k: v for k, v in base_config.__dict__.items() if k not in ['name', 'description', 'brain_config', 'world_config']}
    )
    
    config2 = ExperimentConfig(
        name="plasticity_experiment",
        description="Experiment with enhanced plasticity",
        brain_config={
            'neuron_count': 1000,
            'region_count': 4,
            'plasticity_enabled': True,
            'development_enabled': True,
            'novelty_enabled': True,
            'curiosity_enabled': True,
            'plasticity_rate': 0.8,
            'development_rate': 0.6
        },
        **{k: v for k, v in base_config.__dict__.items() if k not in ['name', 'description', 'brain_config']}
    )
    
    # Create experiments
    experiments = [config1, config2]
    
    for config in experiments:
        print(f"\nCreating experiment: {config.name}")
        if runner.create_experiment(config):
            print(f"  ✓ Created successfully")
        else:
            print(f"  ✗ Failed to create")
    
    # Run experiments in parallel
    print(f"\n=== Running Experiments in Parallel ===")
    experiment_names = [config.name for config in experiments]
    
    results = runner.run_multiple_experiments(experiment_names, parallel=True)
    
    # Analyze results
    print(f"\n=== Experiment Analysis ===")
    for result in results:
        if result.success:
            print(f"\nExperiment: {result.experiment_name}")
            print(f"  Duration: {result.end_time - result.start_time}")
            print(f"  Checkpoints: {len(result.checkpoints)}")
            print(f"  Metrics collected: {len(result.metrics.get('metrics', {}))}")
            print(f"  Performance: {result.performance_stats}")
        else:
            print(f"Experiment {result.experiment_name} failed: {result.error_message}")
    
    # Compare experiments
    print(f"\n=== Experiment Comparison ===")
    comparison = runner.compare_experiments(experiment_names)
    
    for exp_name, exp_data in comparison.items():
        print(f"\n{exp_name}:")
        print(f"  Success: {exp_data['success']}")
        print(f"  Duration: {exp_data['duration']:.2f}s")
        print(f"  Checkpoints: {exp_data['num_checkpoints']}")
    
    # Analyze each experiment individually
    for exp_name in experiment_names:
        print(f"\n=== Detailed Analysis: {exp_name} ===")
        analysis = runner.analyze_results(exp_name, "comprehensive")
        
        print(f"Experiment: {analysis['experiment_name']}")
        print(f"Total duration: {analysis['duration_seconds']:.2f} seconds")
        print(f"Success rate: {analysis['success_rate']}%")
        print(f"Number of checkpoints: {analysis['num_checkpoints']}")
        
        if 'performance' in analysis:
            perf = analysis['performance']
            print(f"Average computation time: {perf.get('avg_computation_time', 0):.4f}s")
            print(f"Total energy consumption: {perf.get('total_energy_consumption', 0):.2f}")
    
    print("\n=== Advanced Experiment Management Complete ===")

if __name__ == "__main__":
    advanced_experiment_management()

# examples/advanced/visualization.py
"""
Advanced Visualization Example

This example demonstrates advanced visualization capabilities.
"""

import sys
sys.path.insert(0, '.')

try:
    from scripts.visualization import BrainVisualizer, SimplePlotter, GraphRenderer
    print("Visualization tools imported successfully")
except ImportError as e:
    print(f"Failed to import visualization tools: {e}")
    print("Make sure scripts/visualization.py exists")
    sys.exit(1)

def advanced_visualization():
    """Example: Advanced visualization techniques"""
    print("\n=== Advanced Visualization Example ===")
    
    # Create visualization tools
    visualizer = BrainVisualizer()
    plotter = SimplePlotter()
    graph_renderer = GraphRenderer()
    
    print("1. Creating comprehensive dashboard...")
    
    # Create sample brain data (simulated)
    class MockBrain:
        def __init__(self):
            self.step_count = 100
        
        def getRegionIds(self):
            return [1, 2, 3, 4]
        
        def getRegion(self, rid):
            return MockRegion(rid)
        
        def getTotalNeuronCount(self):
            return 1000
        
        def getTotalSynapseCount(self):
            return 15000
        
        def getActiveNeuronCount(self):
            return 850
        
        def getFiringNeuronCount(self):
            return 420
        
        def getAverageFiringRate(self):
            return 12.5
        
        def getExcitationInhibitionRatio(self):
            return 2.1
        
        def getTotalSpikeCount(self):
            return 8500
        
        def getDevelopmentalStage(self):
            return 2
    
    class MockRegion:
        def __init__(self, rid):
            self.rid = rid
    
    mock_brain = MockBrain()
    
    # Create comprehensive dashboard
    try:
        dashboard_path = visualizer.create_comprehensive_dashboard(mock_brain, "advanced_dashboard.png")
        print(f"   ✓ Dashboard created: {dashboard_path}")
    except Exception as e:
        print(f"   ✗ Dashboard creation failed: {e}")
    
    print("\n2. Creating activity plots...")
    
    # Create activity plot
    try:
        activity_path = plotter.create_activity_plot(mock_brain, "advanced_activity.png")
        print(f"   ✓ Activity plot created: {activity_path}")
    except Exception as e:
        print(f"   ✗ Activity plot creation failed: {e}")
    
    print("\n3. Creating region comparison...")
    
    # Create region comparison
    try:
        region_path = plotter.create_region_comparison(mock_brain, "region_comparison.png")
        print(f"   ✓ Region comparison created: {region_path}")
    except Exception as e:
        print(f"   ✗ Region comparison creation failed: {e}")
    
    print("\n4. Creating graph visualizations...")
    
    # Create network graph
    try:
        network_path = graph_renderer.render_graph_image(mock_brain, "network_graph.png")
        print(f"   ✓ Network graph created: {network_path}")
    except Exception as e:
        print(f"   ✗ Network graph creation failed: {e}")
    
    # Create activity graph
    try:
        activity_graph_path = graph_renderer.render_graph_image(mock_brain, "activity_graph.png")
        print(f"   ✓ Activity graph created: {activity_graph_path}")
    except Exception as e:
        print(f"   ✗ Activity graph creation failed: {e}")
    
    # Create connections graph
    try:
        connections_path = graph_renderer.render_graph_image(mock_brain, "connections_graph.png")
        print(f"   ✓ Connections graph created: {connections_path}")
    except Exception as e:
        print(f"   ✗ Connections graph creation failed: {e}")
    
    print("\n=== Advanced Visualization Complete ===")

if __name__ == "__main__":
    advanced_visualization()

# examples/advanced/phase6_integration.py
"""
Phase 6 Integration Example

This example demonstrates Phase 6 integration capabilities.
"""

import sys
sys.path.insert(0, '.')

try:
    from scripts.experiments import Phase6Experiment
    print("Phase 6 experiment tools imported successfully")
except ImportError as e:
    print(f"Failed to import Phase 6 tools: {e}")
    print("Make sure scripts/experiments.py exists")
    sys.exit(1)

def phase6_integration_example():
    """Example: Phase 6 integration test"""
    print("\n=== Phase 6 Integration Example ===")
    
    # Create Phase 6 experiment runner
    phase6_runner = Phase6Experiment("phase6_experiments")
    
    print("1. Creating Phase 6 integration experiment...")
    
    # Create integration experiment configuration
    integration_config = {
        'brain_config': {
            'neuron_count': 2000,
            'region_count': 8,
            'excitatory_ratio': 0.85,
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
            'agent_start_x': 200.0,
            'agent_start_y': 200.0,
            'vision_width': 80,
            'vision_height': 80,
            'num_resources': 50,
            'num_hazards': 20
        },
        'duration': 500,
        'steps_per_iteration': 10,
        'save_checkpoints': True,
        'checkpoint_interval': 100,
        'metrics_to_record': [
            'working_memory_usage', 'episodic_memory_recall',
            'associative_memory_strength', 'dopamine_level',
            'curiosity_level', 'novelty_detection_rate',
            'prediction_error', 'planning_confidence',
            'concept_clarity', 'attention_focus',
            'prediction_accuracy', 'action_success_rate'
        ],
        'parallel_runs': 2,
        'max_memory_usage': 4096.0
    }
    
    if phase6_runner.create_integration_experiment(integration_config):
        print("   ✓ Phase 6 integration experiment created successfully")
    else:
        print("   ✗ Failed to create Phase 6 integration experiment")
        return
    
    print("\n2. Running Phase 6 integration test...")
    
    # Run the integration test
    result = phase6_runner.run_integration_test()
    
    if result.success:
        print(f"   ✓ Phase 6 integration test completed successfully!")
        print(f"   Duration: {result.end_time - result.start_time}")
        print(f"   Checkpoints saved: {len(result.checkpoints)}")
        print(f"   Metrics collected: {len(result.metrics.get('metrics', {}))}")
        print(f"   Performance: {result.performance_stats}")
    else:
        print(f"   ✗ Phase 6 integration test failed: {result.error_message}")
        return
    
    print("\n=== Phase 6 Integration Example Complete ===")

if __name__ == "__main__":
    phase6_integration_example()

# examples/advanced/experiment_pipeline.py
"""
Advanced Experiment Pipeline Example

This example demonstrates complex experiment pipelines and workflows.
"""

import sys
import time
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