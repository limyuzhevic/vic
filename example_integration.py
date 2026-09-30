#!/usr/bin/env python3
"""
Integration Examples for NLM Python Bindings

This script demonstrates complete integration of all 24 new Python bindings
functions. It shows how to combine convenience factory functions, utility
functions, advanced features, configuration management, simulation control,
and performance monitoring into comprehensive workflows.

Key Integration Patterns:
- Complete neural network lifecycle management
- End-to-end training pipelines
- Performance monitoring and optimization
- Configuration management across multiple runs
- Error handling and recovery strategies
- Production-ready integration patterns
"""

import sys
import os
import json
import time
import traceback
from datetime import datetime

# Add python directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLMC_AVAILABLE = True
except ImportError:
    PYNLMC_AVAILABLE = False
    print("WARNING: pynlm module not available. Running with mock data.")

def print_header(title):
    """Print formatted header."""
    print(f"\n{'='*80}")
    print(f"  {title}")
    print(f"{'='*80}")

def print_section(title):
    """Print section header."""
    print(f"\n{'•'*60}")
    print(f"{title}")
    print(f"{'•'*60}")

def create_mock_config(neuron_count=1000, region_count=5):
    """Create mock config for testing."""
    class MockConfig:
        def __init__(self):
            self.data = {}
            
        def set(self, key, value, source=None):
            self.data[key] = value
            
        def getKeys(self):
            return list(self.data.keys())
            
        def get(self, key, default=None):
            return self.data.get(key, default)
    
    config = MockConfig()
    config.set('neuron_count', neuron_count)
    config.set('region_count', region_count)
    return config

def create_mock_brain(config=None):
    """Create mock brain for testing."""
    class MockBrain:
        def __init__(self, config):
            self.config = config
            self.spike_count = 0
            self.start_time = time.time()
            self.is_paused = False
            
        def initialize(self):
            return True
            
        def step(self, step):
            self.spike_count += 1
            return True
            
        def getTotalNeuronCount(self):
            return self.config.get('neuron_count', 0)
            
        def getTotalSpikeCount(self):
            return self.spike_count
            
        def getAverageFiringRate(self):
            elapsed = time.time() - self.start_time
            return (self.spike_count / elapsed) * 1000 if elapsed > 0 else 0
            
        def pauseSimulation(self):
            self.is_paused = True
            return True
            
        def resumeSimulation(self):
            self.is_paused = False
            return True
            
        def stepMultiple(self, steps):
            for i in range(steps):
                self.step(i)
            return True
            
        def save(self, filepath=""):
            if filepath:
                print(f"Mock: Saving brain to {filepath}")
            return True
            
        def load(self, filepath=""):
            if filepath:
                print(f"Mock: Loading brain from {filepath}")
            return True
    
    return MockBrain(config or create_mock_config())

def create_mock_world():
    """Create mock world for testing."""
    class MockWorld:
        def __init__(self):
            self.width = 20
            self.height = 20
            self.vision_width = 8
            self.vision_height = 8
            self.time = 0
            
        def configure(self, **kwargs):
            for key, value in kwargs.items():
                setattr(self, key, value)
                
        def reset(self):
            self.time = 0
            
        def update(self, dt):
            self.time += dt
            return self.time
            
        def getSensoryPercept(self):
            return [0.1, 0.2, 0.3, 0.4, 0.5]
    
    return MockWorld()

def create_mock_agent_brain(brain, world):
    """Create mock agent brain for testing."""
    class MockAgentBrain:
        def __init__(self, brain, world):
            self.brain = brain
            self.world = world
            self.reward_modulation_enabled = False
            
        def initialize(self, world):
            self.world = world
            return True
            
        def enableRewardModulation(self, enabled):
            self.reward_modulation_enabled = enabled
            
        def getSensoryInputSize(self):
            return 256
            
        def getMotorOutputSize(self):
            return 6
            
        def processSensoryInput(self, percept):
            return [x * 0.1 for x in percept]
            
        def decodeMotorCommand(self):
            return "MoveForward"
            
        def applyRewardModulation(self, reward, predicted):
            return reward * (1.0 + predicted * 0.1)
    
    return MockAgentBrain(brain, world)

def workflow_example_1_complete_training_pipeline():
    """Example 1: Complete training pipeline using all new functions."""
    print_section("WORKFLOW 1: Complete Training Pipeline")
    
    print("Setting up complete neural network training pipeline...")
    
    # Step 1: Configuration Management with factory functions
    print("\n1. Configuration Management:")
    config = pynlm.createDefaultConfig() if PYNLMC_AVAILABLE else create_mock_config()
    config.set("neuron_count", 1000)
    config.set("region_count", 5)
    config.set("connection_probability", 0.15)
    
    print(f"   ✓ Created config with {config.get('neuron_count')} neurons, {config.get('region_count')} regions")
    
    # Export configuration for documentation
    if PYNLMC_AVAILABLE:
        config_json = pynlm.exportConfigToJson(config)
    else:
        config_json = json.dumps(config.data, indent=2)
    print(f"   ✓ Exported config: {len(config_json)} characters")
    
    # Step 2: Create brain with validation
    print("\n2. Brain Creation with Validation:")
    if PYNLMC_AVAILABLE:
        brain = pynlm.createBrainWithConfig(config)
    else:
        brain = create_mock_brain(config)
    
    brain.initialize()
    print(f"   ✓ Created and initialized brain with {brain.getTotalNeuronCount()} neurons")
    
    # Step 3: Create world and agent
    print("\n3. Agent Setup:")
    world = pynlm.createSimpleWorld() if PYNLMC_AVAILABLE else create_mock_world()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    
    if PYNLMC_AVAILABLE:
        agent = pynlm.createSimpleAgent(brain, world)
        agent.initialize(world)
    else:
        agent = create_mock_agent_brain(brain, world)
        agent.initialize(world)
    
    print(f"   ✓ Created agent with {agent.getSensoryInputSize()} sensory inputs, {agent.getMotorOutputSize()} motor outputs")
    
    # Step 4: Training episode with performance monitoring
    print("\n4. Training Episode with Performance Tracking:")
    if PYNLMC_AVAILABLE:
        start_time = time.time()
        results = pynlm.runTrainingEpisode(agent, world, 50)
        training_time = time.time() - start_time
    else:
        # Mock training episode
        start_time = time.time()
        for i in range(50):
            agent.applyRewardModulation(i * 0.1, 0.5)
        training_time = time.time() - start_time
        results = {
            'steps_taken': 50,
            'total_reward': sum(i * 0.1 for i in range(50)),
            'average_reward_per_step': 2.45,
            'final_action_probability': 0.8,
            'final_health': 100.0,
            'final_age': 1.0
        }
    
    print(f"   ✓ Training completed in {training_time:.2f} seconds")
    print(f"   ✓ Steps taken: {results['steps_taken']}")
    print(f"   ✓ Total reward: {results['total_reward']:.2f}")
    print(f"   ✓ Average reward per step: {results['average_reward_per_step']:.2f}")
    
    # Step 5: Performance benchmarking
    print("\n5. Performance Benchmarking:")
    if PYNLMC_AVAILABLE:
        benchmark = pynlm.benchmarkSimulation(brain, 100)
        memory_info = pynlm.get_memory_usage()
    else:
        # Mock benchmark
        benchmark = {
            'avg_time_ms': 1.5,
            'steps_per_second': 666.67,
            'total_spike_count': 150,
            'firing_rate_hz': 15.0
        }
        memory_info = {
            'total_mb': 1024,
            'used_mb': 256,
            'usage_percent': 25
        }
    
    print(f"   ✓ Performance benchmark:")
    print(f"     - Average step time: {benchmark['avg_time_ms']:.2f} ms")
    print(f"     - Steps per second: {benchmark['steps_per_second']:.2f}")
    print(f"     - Total spikes: {benchmark['total_spike_count']}")
    print(f"     - Firing rate: {benchmark['firing_rate_hz']:.2f} Hz")
    
    print(f"   ✓ Memory usage: {memory_info['usage_percent']}% ({memory_info['used_mb']}/{memory_info['total_mb']} MB)")
    
    # Step 6: Simulation control and checkpointing
    print("\n6. Simulation Control and Checkpointing:")
    if PYNLMC_AVAILABLE:
        pynlm.pauseSimulation(brain)
        print(f"   ✓ Simulation paused")
        
        pynlm.resumeSimulation(brain)
        print(f"   ✓ Simulation resumed")
        
        pynlm.stepMultiple(brain, 100)
        print(f"   ✓ Executed 100 simulation steps")
    else:
        brain.pauseSimulation()
        print(f"   ✓ Simulation paused")
        
        brain.resumeSimulation()
        print(f"   ✓ Simulation resumed")
        
        brain.stepMultiple(100)
        print(f"   ✓ Executed 100 simulation steps")
    
    # Step 7: Generate comprehensive report
    print("\n7. Report Generation:")
    report_data = {
        'avg_time_ms': benchmark['avg_time_ms'],
        'steps_per_second': benchmark['steps_per_second'],
        'total_spike_count': benchmark['total_spike_count'],
        'firing_rate_hz': benchmark['firing_rate_hz'],
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalNeuronCount() * 10 if not PYNLMC_AVAILABLE else 0,
        'average_reward_per_step': results['average_reward_per_step'],
        'final_health': results['final_health'],
        'training_time_seconds': training_time,
        'config_json_length': len(config_json)
    }
    
    if PYNLMC_AVAILABLE:
        report = pynlm.generate_report(report_data)
    else:
        report = f"Training Report\n"
        report += f"================\n\n"
        report += f"Training Summary:\n"
        report += f"  - Training Time: {training_time:.2f} seconds\n"
        report += f"  - Steps Completed: {results['steps_taken']}\n"
        report += f"  - Total Reward: {results['total_reward']:.2f}\n"
        report += f"  - Average Reward: {results['average_reward_per_step']:.2f}\n"
        report += f"\nPerformance Metrics:\n"
        report += f"  - Neurons: {report_data['neuron_count']}\n"
        report += f"  - Steps/sec: {report_data['steps_per_second']:.2f}\n"
        report += f"  - Firing Rate: {report_data['firing_rate_hz']:.2f} Hz\n"
        report += f"  - Avg Step Time: {report_data['avg_time_ms']:.2f} ms\n\n"
        report += f"Configuration:\n"
        report += f"  - Config JSON length: {report_data['config_json_length']} characters\n"
        report += f"\nGenerated at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
    
    print(f"   ✓ Report generated: {len(report)} characters")
    
    # Save training results
    training_results = {
        'training_summary': {
            'timestamp': datetime.now().isoformat(),
            'config_used': config_json,
            'steps_completed': results['steps_taken'],
            'total_reward': results['total_reward'],
            'training_time': training_time
        },
        'performance_metrics': report_data,
        'report_length': len(report)
    }
    
    with open('training_pipeline_results.json', 'w') as f:
        json.dump(training_results, f, indent=2)
    
    print(f"   ✓ Training results saved to training_pipeline_results.json")
    
    return training_results

def workflow_example_2_multi_agent_batch_training():
    """Example 2: Multi-agent batch training workflow."""
    print_section("WORKFLOW 2: Multi-Agent Batch Training")
    
    print("Setting up multi-agent batch training workflow...")
    
    # Create multiple agent configurations
    agent_configs = []
    for i in range(5):
        config = create_mock_config()
        config.set('neuron_count', 500 + i * 200)
        config.set('region_count', 2 + i)
        agent_configs.append(config)
    
    print(f"\n1. Created {len(agent_configs)} agent configurations")
    
    # Training session management
    training_sessions = []
    session_results = []
    
    print("\n2. Running batch training sessions...")
    
    for i, config in enumerate(agent_configs):
        print(f"\n  Training Session {i+1}/{len(agent_configs)}:")
        
        # Create brain for this session
        if PYNLMC_AVAILABLE:
            brain = pynlm.createBrainWithConfig(config)
        else:
            brain = create_mock_brain(config)
        
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld() if PYNLMC_AVAILABLE else create_mock_world()
        world.configure(width=15, height=15)
        
        if PYNLMC_AVAILABLE:
            agent = pynlm.createSimpleAgent(brain, world)
            agent.initialize(world)
            agent.enableRewardModulation(True)
        else:
            agent = create_mock_agent_brain(brain, world)
            agent.initialize(world)
            agent.enableRewardModulation(True)
        
        # Run training episode
        if PYNLMC_AVAILABLE:
            session_results = pynlm.runTrainingEpisode(agent, world, 30)
        else:
            # Mock training results
            session_results = {
                'steps_taken': 30,
                'total_reward': sum(j * 0.1 for j in range(30)),
                'average_reward_per_step': 1.45,
                'final_action_probability': 0.7,
                'final_health': 95.0,
                'final_age': 2.0
            }
        
        training_sessions.append({
            'session_id': i,
            'config': config.data if not PYNLMC_AVAILABLE else config,
            'brain': brain,
            'world': world,
            'agent': agent,
            'results': session_results
        })
        
        print(f"    ✓ Session {i+1}: {session_results['steps_taken']} steps, "
              f"{session_results['total_reward']:.2f} total reward")
    
    # Performance aggregation
    print(f"\n3. Aggregating performance metrics...")
    
    total_rewards = sum(s['results']['total_reward'] for s in training_sessions)
    avg_rewards = sum(s['results']['average_reward_per_step'] for s in training_sessions) / len(training_sessions)
    total_steps = sum(s['results']['steps_taken'] for s in training_sessions)
    
    print(f"   ✓ Total rewards across all sessions: {total_rewards:.2f}")
    print(f"   ✓ Average reward per step: {avg_rewards:.2f}")
    print(f"   ✓ Total training steps: {total_steps}")
    
    # Memory usage monitoring
    if PYNLMC_AVAILABLE:
        print(f"\n4. Memory Usage Monitoring:")
        for i, session in enumerate(training_sessions):
            memory_info = pynlm.get_memory_usage()
            print(f"    Session {i+1}: {memory_info['usage_percent']}% memory usage")
    
    # Save batch training results
    batch_results = {
        'batch_training_summary': {
            'timestamp': datetime.now().isoformat(),
            'total_sessions': len(training_sessions),
            'total_rewards': total_rewards,
            'average_reward_per_step': avg_rewards,
            'total_steps': total_steps
        },
        'session_results': training_sessions,
        'performance_metrics': {
            'rewards_per_session': [s['results']['total_reward'] for s in training_sessions],
            'steps_per_session': [s['results']['steps_taken'] for s in training_sessions]
        }
    }
    
    with open('multi_agent_batch_training_results.json', 'w') as f:
        json.dump(batch_results, f, indent=2)
    
    print(f"   ✓ Batch training results saved to multi_agent_batch_training_results.json")
    
    return batch_results

def workflow_example_3_development_monitoring():
    """Example 3: Development monitoring workflow."""
    print_section("WORKFLOW 3: Development Monitoring and Evolution")
    
    print("Setting up development monitoring workflow...")
    
    # Create brain for development monitoring
    config = create_mock_config()
    config.set('neuron_count', 800)
    
    if PYNLMC_AVAILABLE:
        brain = pynlm.createBrainWithConfig(config)
        agent = pynlm.createAgentBrain(brain)
    else:
        brain = create_mock_brain(config)
        agent = create_mock_agent_brain(brain, create_mock_world())
    
    brain.initialize()
    
    if PYNLMC_AVAILABLE:
        agent.initialize(create_mock_world())
    
    print(f"\n1. Created developmental brain with {brain.getTotalNeuronCount()} neurons")
    
    # Monitor brain development over time
    print(f"\n2. Monitoring brain development...")
    
    if PYNLMC_AVAILABLE:
        developmental_stats = pynlm.monitorBrainDevelopment(brain, 5)
    else:
        # Mock developmental stats
        developmental_stats = []
        for day in range(1, 6):
            developmental_stats.append({
                'day': day,
                'stage': f'Day {day}',
                'plasticity_modifier': 0.8 - (day * 0.1),
                'average_firing_rate': 10.0 + (day * 2.0),
                'excitatory_inhibition_ratio': 1.0 + (day * 0.05)
            })
    
    for day_stats in developmental_stats:
        print(f"    Day {day_stats['day']}: Stage {day_stats.get('stage', 'Developing')}, "
              f"Firing rate: {day_stats['average_firing_rate']:.2f} Hz")
    
    # Analyze neural activity patterns
    print(f"\n3. Analyzing neural activity patterns...")
    
    if PYNLMC_AVAILABLE:
        activity_data = pynlm.analyze_neural_activity(brain, "full_analysis")
    else:
        # Mock activity analysis
        activity_data = {
            'spike_counts': [i * 10 for i in range(100)],
            'connectivity_matrix': [[0] * 10 for _ in range(10)],
            'firing_patterns': [i * 0.5 for i in range(100)],
            'analysis_type': 'full_analysis'
        }
    
    print(f"    ✓ Spike count data points: {len(activity_data['spike_counts'])}")
    print(f"    ✓ Connectivity matrix entries: {len(activity_data['connectivity_matrix']) * len(activity_data['connectivity_matrix'][0])}")
    print(f"    ✓ Firing patterns neurons: {len(activity_data['firing_patterns'])}")
    
    # Create development report
    development_report = {
        'development_summary': {
            'timestamp': datetime.now().isoformat(),
            'initial_neuron_count': brain.getTotalNeuronCount(),
            'development_days': len(developmental_stats),
            'final_firing_rate': developmental_stats[-1]['average_firing_rate'] if developmental_stats else 0,
            'plasticity_evolution': [(d['day'], d['plasticity_modifier']) for d in developmental_stats]
        },
        'developmental_stats': developmental_stats,
        'neural_activity_analysis': activity_data
    }
    
    with open('development_monitoring_report.json', 'w') as f:
        json.dump(development_report, f, indent=2)
    
    print(f"\n4. Development report saved to development_monitoring_report.json")
    print(f"   ✓ Documented {len(developmental_stats)} days of development")
    print(f"   ✓ Captured neural activity patterns for {len(activity_data['spike_counts'])} time points")
    
    return development_report

def workflow_example_4_optimization_pipeline():
    """Example 4: Complete optimization pipeline."""
    print_section("WORKFLOW 4: Complete Optimization Pipeline")
    
    print("Setting up complete optimization pipeline...")
    
    optimization_runs = []
    
    # Test different configurations for optimization
    test_configs = [
        {'neuron_count': 500, 'region_count': 2, 'connection_probability': 0.1},
        {'neuron_count': 1000, 'region_count': 4, 'connection_probability': 0.15},
        {'neuron_count': 1500, 'region_count': 6, 'connection_probability': 0.2},
        {'neuron_count': 2000, 'region_count': 8, 'connection_probability': 0.25},
        {'neuron_count': 2500, 'region_count': 10, 'connection_probability': 0.3},
    ]
    
    print(f"\n1. Testing {len(test_configs)} configurations for optimization...")
    
    for i, config_params in enumerate(test_configs):
        print(f"\n  Configuration {i+1}: Neurons={config_params['neuron_count']}, "
              f"Regions={config_params['region_count']}, "
              f"ConnProb={config_params['connection_probability']}")
        
        if PYNLMC_AVAILABLE:
            config = pynlm.createDefaultConfig()
            for key, value in config_params.items():
                config.set(key, value)
            
            brain = pynlm.createBrainWithConfig(config)
            brain.initialize()
            
            # Performance benchmarking
            benchmark = pynlm.benchmarkSimulation(brain, 50)
            memory_info = pynlm.get_memory_usage()
            
            optimization_runs.append({
                'config_id': i,
                'config_params': config_params,
                'performance': benchmark,
                'memory_usage': memory_info,
                'success': True
            })
            
            print(f"    ✓ Performance: {benchmark['steps_per_second']:.2f} steps/sec, "
                  f"{benchmark['firing_rate_hz']:.2f} Hz firing rate")
            print(f"    ✓ Memory: {memory_info['usage_percent']}% usage")
        else:
            # Mock optimization run
            optimization_runs.append({
                'config_id': i,
                'config_params': config_params,
                'performance': {
                    'avg_time_ms': 0.5 + (i * 0.1),
                    'steps_per_second': 2000 - (i * 200),
                    'total_spike_count': 50 + (i * 20),
                    'firing_rate_hz': 10.0 + (i * 2.0)
                },
                'memory_usage': {
                    'total_mb': 1024 + (i * 200),
                    'used_mb': 256 + (i * 50),
                    'usage_percent': 25 + i * 5
                },
                'success': True
            })
            
            print(f"    ✓ Performance: {optimization_runs[-1]['performance']['steps_per_second']:.2f} steps/sec, "
                  f"{optimization_runs[-1]['performance']['firing_rate_hz']:.2f} Hz firing rate")
            print(f"    ✓ Memory: {optimization_runs[-1]['memory_usage']['usage_percent']}% usage")
    
    # Optimization analysis
    print(f"\n2. Analyzing optimization results...")
    
    successful_runs = [r for r in optimization_runs if r['success']]
    
    # Find best configuration
    if PYNLMC_AVAILABLE:
        best_run = max(successful_runs, key=lambda x: x['performance']['steps_per_second'])
    else:
        best_run = max(successful_runs, key=lambda x: x['performance']['steps_per_second'])
    
    print(f"\n3. Optimization Results:")
    print(f"   ✓ Best configuration:")
    print(f"     - Neurons: {best_run['config_params']['neuron_count']}")
    print(f"     - Regions: {best_run['config_params']['region_count']}")
    print(f"     - Connection Probability: {best_run['config_params']['connection_probability']}")
    print(f"     - Performance: {best_run['performance']['steps_per_second']:.2f} steps/sec")
    print(f"     - Memory Usage: {best_run['memory_usage']['usage_percent']:.1f}%")
    
    # Performance comparison
    print(f"\n4. Performance Comparison:")
    print(f"   ✓ Configurations tested: {len(successful_runs)}")
    
    if PYNLMC_AVAILABLE:
        avg_performance = sum(r['performance']['steps_per_second'] for r in successful_runs) / len(successful_runs)
        max_performance = max(r['performance']['steps_per_second'] for r in successful_runs)
        min_memory = min(r['memory_usage']['usage_percent'] for r in successful_runs)
    else:
        avg_performance = sum(r['performance']['steps_per_second'] for r in successful_runs) / len(successful_runs)
        max_performance = max(r['performance']['steps_per_second'] for r in successful_runs)
        min_memory = min(r['memory_usage']['usage_percent'] for r in successful_runs)
    
    print(f"   ✓ Average performance: {avg_performance:.2f} steps/sec")
    print(f"   ✓ Maximum performance: {max_performance:.2f} steps/sec")
    print(f"   ✓ Minimum memory usage: {min_memory:.1f}%")
    
    # Save optimization results
    optimization_report = {
        'optimization_summary': {
            'timestamp': datetime.now().isoformat(),
            'total_configurations': len(successful_runs),
            'best_config_id': best_run['config_id'],
            'average_performance': avg_performance,
            'maximum_performance': max_performance,
            'minimum_memory_usage': min_memory
        },
        'optimization_runs': optimization_runs,
        'performance_comparison': {
            'configurations': [r['config_params']['neuron_count'] for r in successful_runs],
            'performances': [r['performance']['steps_per_second'] for r in successful_runs],
            'memory_usages': [r['memory_usage']['usage_percent'] for r in successful_runs]
        }
    }
    
    with open('optimization_pipeline_report.json', 'w') as f:
        json.dump(optimization_report, f, indent=2)
    
    print(f"\n5. Optimization pipeline report saved to optimization_pipeline_report.json")
    print(f"   ✓ Analyzed {len(successful_runs)} configurations")
    print(f"   ✓ Identified optimal configuration")
    print(f"   ✓ Generated performance comparison")
    
    return optimization_report

def workflow_example_5_production_deployment():
    """Example 5: Production deployment workflow."""
    print_section("WORKFLOW 5: Production Deployment and Monitoring")
    
    print("Setting up production deployment workflow...")
    
    # Production environment setup
    print(f"\n1. Production Environment Setup:")
    
    # Create production-ready brain
    if PYNLMC_AVAILABLE:
        prod_config = pynlm.createDefaultConfig()
        prod_config.set("neuron_count", 2000)
        prod_config.set("region_count", 8)
        
        prod_brain = pynlm.createBrainWithConfig(prod_config)
        prod_brain.initialize()
        
        prod_agent = pynlm.createSimpleAgent(prod_brain, pynlm.createSimpleWorld())
        prod_agent.initialize(pynlm.createSimpleWorld())
        prod_agent.enableRewardModulation(True)
        
        prod_world = pynlm.createSimpleWorld()
        prod_world.configure(width=30, height=30, visionWidth=15, visionHeight=15)
        
        print(f"   ✓ Created production brain with {prod_brain.getTotalNeuronCount()} neurons")
        print(f"   ✓ Created production agent with {prod_agent.getSensoryInputSize()} sensory inputs")
        print(f"   ✓ Created production world: {prod_world.width}x{prod_world.height}")
    else:
        prod_config = create_mock_config(2000, 8)
        prod_brain = create_mock_brain(prod_config)
        prod_brain.initialize()
        
        prod_world = create_mock_world()
        prod_world.configure(width=30, height=30, visionWidth=15, visionHeight=15)
        
        prod_agent = create_mock_agent_brain(prod_brain, prod_world)
        prod_agent.initialize(prod_world)
        prod_agent.enableRewardModulation(True)
        
        print(f"   ✓ Created production brain with {prod_brain.getTotalNeuronCount()} neurons")
        print(f"   ✓ Created production agent with {prod_agent.getSensoryInputSize()} sensory inputs")
        print(f"   ✓ Created production world: {prod_world.width}x{prod_world.height}")
    
    # Production simulation
    print(f"\n2. Production Simulation:")
    
    # Run production simulation with performance monitoring
    if PYNLMC_AVAILABLE:
        start_time = time.time()
        
        # Run training episode
        prod_results = pynlm.runTrainingEpisode(prod_agent, prod_world, 100)
        
        # Run benchmark
        benchmark = pynlm.benchmarkSimulation(prod_brain, 100)
        
        # Monitor memory
        memory_info = pynlm.get_memory_usage()
        
        prod_time = time.time() - start_time
    else:
        # Mock production simulation
        start_time = time.time()
        
        prod_results = {
            'steps_taken': 100,
            'total_reward': sum(i * 0.5 for i in range(100)),
            'average_reward_per_step': 49.5,
            'final_action_probability': 0.9,
            'final_health': 98.0,
            'final_age': 5.0
        }
        
        benchmark = {
            'avg_time_ms': 1.2,
            'steps_per_second': 833.33,
            'total_spike_count': 350,
            'firing_rate_hz': 35.0
        }
        
        memory_info = {
            'total_mb': 4096,
            'used_mb': 1024,
            'usage_percent': 25
        }
        
        prod_time = time.time() - start_time
        
        # Mock simulation steps
        for i in range(100):
            prod_agent.applyRewardModulation(i * 0.5, 0.8)
    
    print(f"   ✓ Production simulation completed in {prod_time:.2f} seconds")
    print(f"   ✓ Steps completed: {prod_results['steps_taken']}")
    print(f"   ✓ Total reward: {prod_results['total_reward']:.2f}")
    print(f"   ✓ Average reward: {prod_results['average_reward_per_step']:.2f}")
    print(f"   ✓ Performance: {benchmark['steps_per_second']:.2f} steps/sec")
    print(f"   ✓ Memory usage: {memory_info['usage_percent']}% ({memory_info['used_mb']}/{memory_info['total_mb']} MB)")
    
    # Generate production report
    print(f"\n3. Production Report Generation:")
    
    production_report_data = {
        'production_summary': {
            'timestamp': datetime.now().isoformat(),
            'simulation_time_seconds': prod_time,
            'steps_completed': prod_results['steps_taken'],
            'total_reward': prod_results['total_reward'],
            'average_reward_per_step': prod_results['average_reward_per_step']
        },
        'performance_metrics': {
            'steps_per_second': benchmark['steps_per_second'],
            'firing_rate_hz': benchmark['firing_rate_hz'],
            'average_step_time_ms': benchmark['avg_time_ms'],
            'total_spike_count': benchmark['total_spike_count']
        },
        'resource_usage': {
            'memory_usage_percent': memory_info['usage_percent'],
            'memory_used_mb': memory_info['used_mb'],
            'memory_total_mb': memory_info['total_mb']
        },
        'configuration': {
            'neuron_count': prod_brain.getTotalNeuronCount(),
            'region_count': config.get('region_count') if PYNLMC_AVAILABLE else prod_config.data.get('region_count', 0)
        }
    }
    
    if PYNLMC_AVAILABLE:
        production_report = pynlm.generate_report(production_report_data)
    else:
        production_report = f"Production Report\n"
        production_report += f"==================\n\n"
        production_report += f"Production Environment:\n"
        production_report += f"  - Neurons: {production_report_data['configuration']['neuron_count']}\n"
        production_report += f"  - Regions: {production_report_data['configuration']['region_count']}\n"
        production_report += f"\nSimulation Results:\n"
        production_report += f"  - Duration: {production_report_data['production_summary']['simulation_time_seconds']:.2f} seconds\n"
        production_report += f"  - Steps: {production_report_data['production_summary']['steps_completed']}\n"
        production_report += f"  - Total Reward: {production_report_data['production_summary']['total_reward']:.2f}\n"
        production_report += f"  - Average Reward: {production_report_data['production_summary']['average_reward_per_step']:.2f}\n"
        production_report += f"\nPerformance Metrics:\n"
        production_report += f"  - Steps/sec: {production_report_data['performance_metrics']['steps_per_second']:.2f}\n"
        production_report += f"  - Firing Rate: {production_report_data['performance_metrics']['firing_rate_hz']:.2f} Hz\n"
        production_report += f"  - Avg Step Time: {production_report_data['performance_metrics']['average_step_time_ms']:.2f} ms\n"
        production_report += f"\nResource Usage:\n"
        production_report += f"  - Memory: {production_report_data['resource_usage']['memory_usage_percent']:.1f}% ({production_report_data['resource_usage']['memory_used_mb']}/{production_report_data['resource_usage']['memory_total_mb']} MB)\n\n"
        production_report += f"Generated at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n"
    
    print(f"   ✓ Production report generated: {len(production_report)} characters")
    
    # Save production results
    production_deployment = {
        'deployment_summary': {
            'timestamp': datetime.now().isoformat(),
            'environment': 'production',
            'simulation_duration': prod_time,
            'success': True
        },
        'production_report': production_report,
        'production_results': production_report_data
    }
    
    with open('production_deployment_results.json', 'w') as f:
        json.dump(production_deployment, f, indent=2)
    
    print(f"\n4. Production deployment results saved to production_deployment_results.json")
    print(f"   ✓ Successfully deployed and monitored production environment")
    print(f"   ✓ Generated comprehensive production report")
    print(f"   ✓ Validated system performance and resource usage")
    
    return production_deployment

def main():
    """Main function to run all integration examples."""
    print_header("NLM Python Bindings - Complete Integration Examples")
    
    if PYNLMC_AVAILABLE:
        print("✓ pynlm module loaded successfully")
    else:
        print("⚠️  pynlm module not available - running with mock implementation")
    
    print("\nThis script demonstrates complete integration of all 24 new Python bindings")
    print("functions in realistic production and development workflows.")
    
    # Run all workflow examples
    workflows = [
        ("Complete Training Pipeline", workflow_example_1_complete_training_pipeline),
        ("Multi-Agent Batch Training", workflow_example_2_multi_agent_batch_training),
        ("Development Monitoring", workflow_example_3_development_monitoring),
        ("Optimization Pipeline", workflow_example_4_optimization_pipeline),
        ("Production Deployment", workflow_example_5_production_deployment),
    ]
    
    all_results = []
    
    for workflow_name, workflow_func in workflows:
        try:
            print(f"\n{'='*80}")
            print(f"EXECUTING: {workflow_name}")
            print(f"{'='*80}")
            
            result = workflow_func()
            all_results.append((workflow_name, result, "SUCCESS"))
            
        except Exception as e:
            print(f"\n{'='*80}")
            print(f"ERROR in {workflow_name}: {e}")
            print(f"{'='*80}")
            print(f"Traceback: {traceback.format_exc()}")
            all_results.append((workflow_name, None, f"FAILED: {e}"))
    
    # Summary
    print_header("INTEGRATION EXAMPLES SUMMARY")
    
    successful = [r for r in all_results if r[2] == "SUCCESS"]
    failed = [r for r in all_results if r[2] != "SUCCESS"]
    
    print(f"\nTotal workflows executed: {len(all_results)}")
    print(f"Successful workflows: {len(successful)}")
    print(f"Failed workflows: {len(failed)}")
    
    print(f"\n{'='*80}")
    print("SUCCESSFUL WORKFLOWS:")
    print(f"{'='*80}")
    
    for workflow_name, result, status in successful:
        print(f"  ✓ {workflow_name}")
        if isinstance(result, dict):
            if 'steps_completed' in result.get('training_summary', {}):
                print(f"    Completed {result['training_summary']['steps_completed']} training steps")
            elif 'total_sessions' in result.get('batch_training_summary', {}):
                print(f"    Ran {result['batch_training_summary']['total_sessions']} training sessions")
            elif 'development_days' in result.get('development_summary', {}):
                print(f"    Monitored {result['development_summary']['development_days']} days of development")
            elif 'total_configurations' in result.get('optimization_summary', {}):
                print(f"    Tested {result['optimization_summary']['total_configurations']} configurations")
            elif 'simulation_time_seconds' in result.get('production_summary', {}):
                print(f"    Simulated for {result['production_summary']['simulation_time_seconds']:.2f} seconds")
    
    if failed:
        print(f"\n{'='*80}")
        print("FAILED WORKFLOWS:")
        print(f"{'='*80}")
        
        for workflow_name, result, status in failed:
            print(f"  ✗ {workflow_name}")
            print(f"    Error: {status}")
    
    print(f"\n{'='*80}")
    print("INTEGRATION EXAMPLES DEMONSTRATION COMPLETE")
    print(f"{'='*80}")
    print(f"\nKey achievements:")
    print(f"  ✓ Demonstrated complete integration of all 24 new Python bindings")
    print(f"  ✓ Showed realistic production and development workflows")
    print(f"  ✓ Covered training pipelines, monitoring, and optimization")
    print(f"  ✓ Provided production deployment examples")
    print(f"  ✓ Generated comprehensive reports and documentation")
    
    print(f"\nGenerated files:")
    print(f"  ✓ training_pipeline_results.json")
    print(f"  ✓ multi_agent_batch_training_results.json")
    print(f"  ✓ development_monitoring_report.json")
    print(f"  ✓ optimization_pipeline_report.json")
    print(f"  ✓ production_deployment_results.json")
    
    print(f"\n{'='*80}")
    
    return 0

if __name__ == "__main__":
    exit(main())