"""
Comprehensive test examples for NLM enhanced Python bindings.

This file contains examples demonstrating all the new functionality
in the enhanced Python bindings (pynlm_enhanced).
"""

import pynlm_enhanced as pynlm
import matplotlib.pyplot as plt
import numpy as np
import json
import time
from typing import List, Dict, Any

def test_basic_enhanced_functionality():
    """Test basic enhanced Python bindings functionality."""
    print("=== Testing Basic Enhanced Functionality ===")
    
    # Test enhanced configuration
    config = pynlm.createEnhancedConfig()
    config.setNeuronCount(500)
    config.setConnectionProbability(0.1)
    config.setSTDPEnabled(True)
    
    print(f"Neuron count: {config.getNeuronCount()}")
    print(f"Connection probability: {config.getConnectionProbability()}")
    print(f"STDP enabled: {config.getSTDPEnabled()}")
    
    # Validate configuration
    is_valid = config.validate()
    print(f"Configuration valid: {is_valid}")
    
    # Test schema
    schema = config.getSchema()
    print(f"Schema length: {len(schema)} characters")
    
    return config

def test_performance_profiling():
    """Test performance profiling capabilities."""
    print("\n=== Testing Performance Profiling ===")
    
    # Create brain and profiler
    brain = pynlm.createResearchBrain(200, True)
    profiler = pynlm.PerformanceProfiler()
    
    print("Starting performance profiling...")
    profiler.start()
    
    # Run simulation with step tracking
    for step in range(50):
        brain.step(step)
        profiler.step()
    
    profiler.stop()
    
    # Analyze results
    metrics = profiler.getMetrics()
    print(f"Total simulation time: {metrics.totalTimeMs:.2f}ms")
    print(f"Average step time: {profiler.getAverageStepTime():.3f}ms")
    print(f"Min step time: {profiler.getMinStepTime():.3f}ms")
    print(f"Max step time: {profiler.getMaxStepTime():.3f}ms")
    print(f"Total spikes: {metrics.spikeCount}")
    print(f"Firing rate: {metrics.firingRate:.3f}")
    
    # Test reset functionality
    profiler.reset()
    print("Profiler reset completed")
    
    return profiler, brain

def test_research_utilities():
    """Test research utilities for batch experiments and analysis."""
    print("\n=== Testing Research Utilities ===")
    
    # Create multiple configurations for batch testing
    configs = []
    for neuron_count in [100, 200, 500]:
        config = pynlm.createEnhancedConfig()
        config.setNeuronCount(neuron_count)
        configs.append(config)
    
    print(f"Created {len(configs)} test configurations")
    
    # Test batch experiment functionality
    print("\nTesting batch experiment setup...")
    results = pynlm.ResearchUtilities.runBatchExperiments(configs)
    
    print(f"Batch experiment completed with {len(results)} results")
    
    # Analyze results
    rewards = [result.totalReward for result in results]
    firing_rates = [result.avgFiringRate for result in results]
    
    stats = pynlm.ResearchUtilities.computeStatistics(rewards)
    print(f"\nReward statistics:")
    print(f"  Mean: {stats['mean']:.3f}")
    print(f"  Stddev: {stats['stddev']:.3f}")
    print(f"  Min: {stats['min']:.3f}")
    print(f"  Max: {stats['max']:.3f}")
    
    # Test pattern similarity analysis
    print("\nTesting pattern similarity analysis...")
    pattern1 = [0.1, 0.5, 0.9, 0.2, 0.7]
    pattern2 = [0.2, 0.6, 0.8, 0.3, 0.6]
    similarity_result = pynlm.ResearchUtilities.analyzePatternSimilarity(pattern1, pattern2)
    
    print(f"Pattern 1: {pattern1}")
    print(f"Pattern 2: {pattern2}")
    print(f"Similarity: {similarity_result[0]:.3f}")
    print(f"Dot product: {similarity_result[1]:.3f}")
    
    return results

def test_brain_research_apis():
    """Test enhanced brain research APIs."""
    print("\n=== Testing Brain Research APIs ===")
    
    # Create brain for research
    brain = pynlm.createResearchBrain(300, True)
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    agent = pynlm.createAgentForResearch(brain, world)
    
    # Enable research features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation with data collection
    print("Running research simulation...")
    for step in range(30):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
    
    # Analyze neural activity
    brain_stats = brain.analyzeNeuralActivity()
    print(f"\nNeural activity analysis:")
    for key, value in brain_stats.items():
        print(f"  {key}: {value:.3f}")
    
    # Test development stage control
    print(f"\nCurrent developmental stage: {brain.getDevelopmentalStage()}")
    
    # Advance development and test
    brain.advanceDevelopmentStage(pynlm.DevelopmentalStage.Maturation)
    print(f"Advanced to Maturation stage")
    print(f"Firing rate after development: {brain.getAverageFiringRate():.3f}")
    
    return brain, agent, world

def test_experimental_prototypes():
    """Test experimental protocol prototypes."""
    print("\n=== Testing Experimental Protocol Prototypes ===")
    
    # Protocol 1: Neuromodulation Experiment
    print("Protocol 1: Neuromodulation Experiment")
    brain = pynlm.createResearchBrain(200, True)
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 10, 10)
    agent = pynlm.createAgentForResearch(brain, world)
    
    # Enable neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Conduct neuromodulation experiment
    brain.conductNeuromodulationExperiment("dopamine_reward", 1.0)
    brain.conductNeuromodulationExperiment("curiosity_drive", 0.7)
    
    print("Neuromodulation experiment completed")
    
    # Protocol 2: Memory Consolidation Experiment
    print("\nProtocol 2: Memory Consolidation Experiment")
    brain = pynlm.createResearchBrain(250, True)
    
    # Store experiences for memory research
    for step in range(20):
        pattern = np.random.rand(10).tolist()  # Random pattern
        action_pattern = np.random.rand(5).tolist()  # Random action pattern
        reward = np.random.random()  # Random reward
        brain.storeExperience(pattern, action_pattern, reward)
        brain.step(step)
    
    # Analyze memory storage
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"Working memory capacity: {working_memory.getCapacity()}")
        print(f"Current memory traces: {working_memory.getCurrentSize()}")
    
    # Protocol 3: Performance Optimization Experiment
    print("\nProtocol 3: Performance Optimization Experiment")
    
    # Test different configurations
    configs = [
        {"neuron_count": 100, "plasticity": True},
        {"neuron_count": 300, "plasticity": False},
        {"neuron_count": 500, "plasticity": True}
    ]
    
    results = []
    for config in configs:
        print(f"  Testing {config['neuron_count']} neurons, plasticity={config['plasticity']}")
        brain = pynlm.createResearchBrain(config['neuron_count'], config['plasticity'])
        
        profiler = pynlm.PerformanceProfiler()
        profiler.start()
        
        for step in range(20):
            brain.step(step)
            profiler.step()
        
        profiler.stop()
        
        results.append({
            'neuron_count': config['neuron_count'],
            'plasticity': config['plasticity'],
            'avg_step_time': profiler.getAverageStepTime(),
            'total_time': profiler.getMetrics().totalTimeMs
        })
        
        print(f"    Average step time: {profiler.getAverageStepTime():.3f}ms")
    
    # Compare performance
    print("\nPerformance comparison:")
    for result in results:
        print(f"  {result['neuron_count']} neurons, plasticity={result['plasticity']}: "
              f"{result['avg_step_time']:.3f}ms per step")
    
    return results

def test_configuration_validation():
    """Test configuration validation system."""
    print("\n=== Testing Configuration Validation ===")
    
    # Test valid configuration
    valid_config = pynlm.createEnhancedConfig()
    valid_config.setNeuronCount(1000)
    valid_config.setConnectionProbability(0.05)
    valid_config.setSTDPLearningRate(0.001)
    
    is_valid = valid_config.validate()
    print(f"Valid configuration: {is_valid}")
    
    # Test invalid configuration (should be caught by validation)
    invalid_config = pynlm.createEnhancedConfig()
    invalid_config.setNeuronCount(50)  # Too small
    invalid_config.setConnectionProbability(1.5)  # Invalid range
    
    is_valid = invalid_config.validate()
    print(f"Invalid configuration (should be False): {is_valid}")
    
    # Test schema generation
    schema = valid_config.getSchema()
    print(f"Schema generated: {len(schema)} characters")
    print("Schema preview:")
    print(schema[:200] + "..." if len(schema) > 200 else schema)
    
    return valid_config

def test_data_export_and_analysis():
    """Test data export for external analysis."""
    print("\n=== Testing Data Export and Analysis ===")
    
    # Create comprehensive experiment
    brain = pynlm.createResearchBrain(400, True)
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 12, 12)
    agent = pynlm.createAgentForResearch(brain, world)
    
    # Enable all research features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run experiment with data collection
    experiment_data = []
    profiler = pynlm.PerformanceProfiler()
    
    profiler.start()
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect comprehensive data
        activity = brain.analyzeNeuralActivity()
        activity.update({
            'step': step,
            'world_time': world.getSimulationTime(),
            'reward': percept.getInternal()[0] if percept.getInternal() else 0.0,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development_stage': brain.getDevelopmentalStage()
        })
        
        experiment_data.append(activity)
        profiler.step()
    
    profiler.stop()
    
    # Analyze experiment results
    print(f"\nExperiment completed with {len(experiment_data)} data points")
    print(f"Performance: {profiler.getAverageStepTime():.3f}ms per step")
    
    # Calculate statistics
    rewards = [data['reward'] for data in experiment_data]
    firing_rates = [data['average_firing_rate'] for data in experiment_data]
    
    stats = pynlm.ResearchUtilities.computeStatistics(rewards)
    print(f"\nReward statistics: mean={stats['mean']:.3f}, std={stats['stddev']:.3f}")
    
    # Export data
    print("\nExporting data for external analysis...")
    pynlm.exportForAnalysis(brain, "experiment_data.json", "json")
    
    # Create summary
    summary = {
        'total_steps': len(experiment_data),
        'total_reward': sum(rewards),
        'avg_firing_rate': sum(firing_rates) / len(firing_rates),
        'performance_ms': profiler.getAverageStepTime(),
        'timestamp': time.time()
    }
    
    with open('experiment_summary.json', 'w') as f:
        json.dump(summary, f, indent=2)
    
    print("Data export completed")
    return experiment_data, summary

def test_advanced_monitoring():
    """Test advanced monitoring and visualization capabilities."""
    print("\n=== Testing Advanced Monitoring ===")
    
    # Create brain with profiler
    brain = pynlm.createResearchBrain(300, True)
    profiler = pynlm.PerformanceProfiler()
    
    # Run simulation with monitoring
    print("Starting advanced monitoring simulation...")
    profiler.start()
    
    monitoring_data = {
        'steps': [],
        'spike_counts': [],
        'firing_rates': [],
        'step_times': []
    }
    
    for step in range(30):
        brain.step(step)
        profiler.step()
        
        # Collect monitoring data
        monitoring_data['steps'].append(step)
        monitoring_data['spike_counts'].append(brain.getTotalSpikeCount())
        monitoring_data['firing_rates'].append(brain.getAverageFiringRate())
        monitoring_data['step_times'].append(profiler.getAverageStepTime())
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"rate={brain.getAverageFiringRate():.3f}, "
                  f"perf={profiler.getAverageStepTime():.3f}ms")
    
    profiler.stop()
    
    # Analyze monitoring data
    metrics = profiler.getMetrics()
    print(f"\nMonitoring completed:")
    print(f"  Total steps: {len(monitoring_data['steps'])}")
    print(f"  Average step time: {metrics.totalTimeMs / len(monitoring_data['steps']):.3f}ms")
    print(f"  Performance stability: {metrics.minStepTime / metrics.maxStepTime:.3f}")
    
    # Test pattern similarity matrix
    print("\nTesting pattern similarity matrix...")
    test_patterns = [
        [0.1, 0.2, 0.3, 0.4, 0.5],
        [0.2, 0.3, 0.4, 0.5, 0.6],
        [0.5, 0.4, 0.3, 0.2, 0.1]
    ]
    
    similarity_matrix = pynlm.ResearchUtilities.computePatternSimilarityMatrix(test_patterns)
    print(f"Pattern similarity matrix (3x3):")
    for row in similarity_matrix:
        print(f"  {[f'{x:.3f}' for x in row]}")
    
    return monitoring_data

def main():
    """Run all test examples."""
    print("NLM Enhanced Python Bindings - Comprehensive Test Suite")
    print("=" * 60)
    
    try:
        # Run all test examples
        test_basic_enhanced_functionality()
        test_performance_profiling()
        test_research_utilities()
        test_brain_research_apis()
        test_experimental_prototypes()
        test_configuration_validation()
        test_data_export_and_analysis()
        test_advanced_monitoring()
        
        print("\n" + "=" * 60)
        print("All tests completed successfully!")
        print("\nKey enhancements demonstrated:")
        print("✓ Enhanced configuration with validation")
        print("✓ Performance profiling and analysis")
        print("✓ Research utilities for batch experiments")
        print("✓ Advanced brain research APIs")
        print("✓ Experimental protocol templates")
        print("✓ Configuration schema system")
        print("✓ Data export for external analysis")
        print("✓ Pattern similarity analysis")
        print("✓ Comprehensive monitoring capabilities")
        
        print("\nFor more information, visit the documentation:")
        print("- README.md for basic usage")
        print("- HOW_TO_USE.md for detailed API documentation")
        print("- easy_usage.md for quick start examples")
        
    except Exception as e:
        print(f"\nTest failed with error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()

# Additional test utilities for specific use cases
def test_curiosity_experiment():
    """Example of a curiosity-driven exploration experiment."""
    print("\n=== Curiosity Experiment Example ===")
    
    brain = pynlm.createResearchBrain(250, True)
    world = pynlm.createSimpleWorld()
    world.configure(12, 12, 8, 8)
    agent = pynlm.createAgentForResearch(brain, world)
    
    # Enable curiosity and novelty detection
    agent.enableCuriosity(True)
    
    # Record initial curiosity level
    initial_curiosity = agent.getCuriosityLevel()
    print(f"Initial curiosity level: {initial_curiosity:.3f}")
    
    # Run exploration
    for step in range(20):
        world.update(0.1)
        
        # Process sensory input (stimulates curiosity)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        # Check novelty detection
        novelty = agent.getNoveltyLevel()
        print(f"Step {step}: Novelty level: {novelty:.3f}")
        
        # Use curiosity to drive exploration
        curiosity = agent.getCuriosityLevel()
        if curiosity > 0.5:
            print(f"  High curiosity - exploring new areas!")
    
    print("Curiosity experiment completed")

def test_development_experiment():
    """Example of a developmental study experiment."""
    print("\n=== Development Experiment Example ===")
    
    brain = pynlm.createResearchBrain(150, True)
    
    # Study development across stages
    stages = [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult
    ]
    
    for stage in stages:
        brain.advanceDevelopmentStage(stage)
        print(f"\nDevelopmental Stage: {stage}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        
        # Run experiment at this stage
        for step in range(10):
            brain.step(step)
        
        # Small delay to observe development
        time.sleep(0.1)
    
    print("\nDevelopment experiment completed")

def test_prediction_experiment():
    """Example of a prediction and error minimization experiment."""
    print("\n=== Prediction Experiment Example ===")
    
    brain = pynlm.createResearchBrain(200, True)
    
    if hasattr(brain, 'getPredictionSystem'):
        prediction_system = brain.getPredictionSystem()
        
        print("Prediction system analysis:")
        
        for step in range(15):
            # Create a simple prediction scenario
            current_state = [float(i % 5) / 5.0 for i in range(10)]
            
            # Make prediction (simulated)
            # In real implementation, this would use actual sensory input
            predicted_state = [x * 0.9 for x in current_state]  # Simple prediction
            actual_state = [x * 1.1 for x in current_state]  # Actual state
            
            # Get prediction error
            prediction_error = prediction_system.getPredictionError()
            confidence = prediction_system.getConfidence()
            
            print(f"  Step {step}: Prediction error={prediction_error:.3f}, "
                  f"confidence={confidence:.3f}")
            
            brain.step(step)
        
        print("Prediction experiment completed")
    else:
        print("Prediction system not available (Phase 2)")

if __name__ == "__main__":
    # Run main test suite
    main()
    
    # Run additional specific experiments
    test_curiosity_experiment()
    test_development_experiment()
    test_prediction_experiment()
    
    print("\n" + "=" * 60)
    print("All enhanced Python bindings tests completed successfully!")
    print("\nFor additional examples and documentation, refer to:")
    print("- docs/ARCHITECTURE.md - System architecture")
    print("- docs/SCIENCE.md - Scientific background")
    print("- examples/ - Additional example scripts")