#!/usr/bin/env python3
"""
Comprehensive Examples for NLM Advanced API

This file provides comprehensive examples demonstrating the use of the Advanced API
for the Neural Learning Machine (NLM) framework. Each example covers different
aspects of the advanced functionality.
"""

import sys
import os
import numpy as np

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from pynlm.advanced_api import (
    AdvancedAPI,
    SimpleAgent,
    SimulationRunner,
    createBrainProfiler,
    createNeuronActivityMonitor,
    createNetworkConnectivityAnalyzer,
    createMemorySystemProfiler,
    createNeuromodulationAnalyzer,
    createDevelopmentStageAnalyzer,
    VisualizationHelper,
)

def example_basic_simulation():
    """
    Example 1: Basic Simulation with SimpleAgent
    ==============================================
    
    This example demonstrates the most basic usage of the Advanced API
    to create a simple agent and run a simulation.
    """
    print("=" * 60)
    print("Example 1: Basic Simulation")
    print("=" * 60)
    
    # Create configuration and brain
    print("Creating configuration and brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Create world and agent
    print("Creating world and agent...")
    world = pynlm.createSimpleWorld()
    agent = SimpleAgent(brain, world, "agent_1")
    
    # Initialize the agent
    print("Initializing agent...")
    agent.initialize()
    
    # Create simulation runner
    print("Setting up simulation runner...")
    sim_runner = SimulationRunner(num_steps=100, dt=1.0)
    sim_runner.addAgent(agent)
    
    # Run simulation
    print("Running simulation...")
    sim_runner.run()
    
    # Get and display results
    print("Simulation completed!")
    stats = sim_runner.getStatistics()
    print(f"Average firing rate: {stats['average_firing_rate']:.2f}")
    print(f"Total spike count: {stats['total_spike_count']}")
    
    print("\n")

def example_advanced_brain_analysis():
    """
    Example 2: Advanced Brain Analysis
    ==================================
    
    This example demonstrates advanced brain analysis using various
    debugging and profiling tools.
    """
    print("=" * 60)
    print("Example 2: Advanced Brain Analysis")
    print("=" * 60)
    
    # Create brain
    print("Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    print("Initializing brain...")
    brain.initialize()
    
    # Run some simulation steps
    print("Running simulation steps...")
    for i in range(50):
        brain.step(1.0)
    
    # Create and use brain profiler
    print("Creating brain profiler...")
    profiler = createBrainProfiler(brain)
    profiler.start()
    
    # Run additional steps while profiling
    print("Profiling simulation...")
    for i in range(50):
        brain.step(1.0)
    
    profiler.stop()
    
    # Get profiling results
    print("\nProfile Summary:")
    print(profiler.getProfileSummary())
    
    # Analyze spike timing
    print("\nSpike Timing Analysis:")
    spike_analysis = profiler.analyzeSpikeTiming()
    for key, value in spike_analysis.items():
        print(f"  {key}: {len(value)} data points")
    
    # Analyze network connectivity
    print("\nNetwork Connectivity Analysis:")
    connectivity_analysis = profiler.analyzeNetworkConnectivity()
    for key, value in connectivity_analysis.items():
        print(f"  {key}: {len(value)} x {len(value[0]) if value else 0} matrix")
    
    print("\n")

def example_neuron_activity_monitoring():
    """
    Example 3: Neuron Activity Monitoring
    =====================================
    
    This example demonstrates neuron activity monitoring and analysis.
    """
    print("=" * 60)
    print("Example 3: Neuron Activity Monitoring")
    print("=" * 60)
    
    # Create brain
    print("Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    print("Initializing brain...")
    brain.initialize()
    
    # Create neuron activity monitor
    print("Creating neuron activity monitor...")
    monitor = createNeuronActivityMonitor(brain)
    
    # Start monitoring
    print("Starting neuron activity monitoring...")
    monitor.startMonitoring()
    
    # Run simulation
    print("Running simulation...")
    for i in range(100):
        brain.step(1.0)
    
    # Stop monitoring
    print("Stopping neuron activity monitoring...")
    monitor.stopMonitoring()
    
    # Get activity statistics
    print("\nNeuron Activity Statistics:")
    print(monitor.getActivitySummary())
    
    # Get firing rates
    print("\nFiring Rates:")
    firing_rates = monitor.getFiringRates()
    for neuron_id, rate in list(firing_rates.items())[:5]:
        print(f"  Neuron {neuron_id}: {rate:.2f}")
    
    # Get bursts
    print("\nBursts:")
    bursts = monitor.getBursts()
    for neuron_id, burst_count in list(bursts.items())[:5]:
        print(f"  Neuron {neuron_id}: {burst_count} bursts")
    
    # Compute coherence
    print("\nCoherence:")
    coherence = monitor.computeCoherence()
    for i, coherence_value in enumerate(coherence[:5]):
        print(f"  Neuron {i}: {coherence_value:.3f}")
    
    print("\n")

def example_network_analysis():
    """
    Example 4: Network Analysis
    ==========================
    
    This example demonstrates comprehensive network analysis.
    """
    print("=" * 60)
    print("Example 4: Network Analysis")
    print("=" * 60)
    
    # Create brain
    print("Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    print("Initializing brain...")
    brain.initialize()
    
    # Create network connectivity analyzer
    print("Creating network connectivity analyzer...")
    analyzer = createNetworkConnectivityAnalyzer(brain)
    
    # Run some steps to establish connectivity
    print("Establishing network connectivity...")
    for i in range(50):
        brain.step(1.0)
    
    # Perform network analysis
    print("\nNetwork Analysis Results:")
    
    # Compute adjacency matrix
    print("Computing adjacency matrix...")
    adjacency = analyzer.computeAdjacencyMatrix()
    print(f"  Adjacency matrix shape: {len(adjacency)} x {len(adjacency[0]) if adjacency else 0}")
    
    # Compute clustering coefficient
    print("Computing clustering coefficient...")
    clustering = analyzer.computeClusteringCoefficient()
    print(f"  Clustering coefficient: {clustering:.4f}")
    
    # Compute characteristic path length
    print("Computing characteristic path length...")
    path_length = analyzer.computeCharacteristicPathLength()
    print(f"  Characteristic path length: {path_length:.4f}")
    
    # Compute small worldness
    print("Computing small worldness...")
    small_worldness = analyzer.computeSmallWorldness()
    print(f"  Small worldness: {small_worldness:.4f}")
    
    # Find hub neurons
    print("\nFinding hub neurons...")
    hubs = analyzer.findHubNeurons(threshold=0.5)
    print(f"  Found {len(hubs)} hub neurons")
    if hubs:
        print(f"  Hub neuron IDs: {hubs[:10]}")
    
    # Find communities
    print("\nFinding communities...")
    communities = analyzer.findCommunities()
    print(f"  Found {len(communities)} communities")
    for community_id, members in list(communities.items())[:5]:
        print(f"    Community {community_id}: {len(members)} members")
    
    # Compute centrality measures
    print("\nCentrality Measures:")
    betweenness = analyzer.computeBetweennessCentrality()
    closeness = analyzer.computeClosenessCentrality()
    
    print(f"  Betweenness centrality (top 5): {sorted(betweenness, reverse=True)[:5]}")
    print(f"  Closeness centrality (top 5): {sorted(closeness, reverse=True)[:5]}")
    
    print("\n")

def example_memory_system_profiling():
    """
    Example 5: Memory System Profiling
    ==================================
    
    This example demonstrates memory system profiling and analysis.
    """
    print("=" * 60)
    print("Example 5: Memory System Profiling")
    print("=" * 60)
    
    # Create brain
    print("Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    print("Initializing brain...")
    brain.initialize()
    
    # Create memory system profiler
    print("Creating memory system profiler...")
    profiler = createMemorySystemProfiler(brain)
    
    # Create neuromodulation analyzer
    print("Creating neuromodulation analyzer...")
    neuromod_analyzer = createNeuromodulationAnalyzer(brain)
    
    # Run simulation
    print("Running simulation...")
    for i in range(100):
        brain.step(1.0)
        # Simulate some memory operations
        if i % 10 == 0:
            # Simulate memory storage
            pattern = np.random.randn(100).astype(np.float32)
            brain.getWorkingMemory().store(pattern, 1.0)
    
    # Get memory profiling results
    print("\nMemory System Profile:")
    print(profiler.getMemorySummary())
    
    # Get neuromodulation analysis
    print("\nNeuromodulation Analysis:")
    print(neurom_analyzer.getModulationSummary())
    
    # Get memory usage statistics
    print("\nMemory Usage Statistics:")
    working_memory = profiler.getWorkingMemoryUsage()
    episodic_memory = profiler.getEpisodicMemoryUsage()
    semantic_memory = profiler.getSemanticMemoryUsage()
    procedural_memory = profiler.getProceduralMemoryUsage()
    
    print(f"  Working memory usage: {working_memory[0]*100:.1f}%")
    print(f"  Episodic memory usage: {episodic_memory[0]*100:.1f}%")
    print(f"  Semantic memory usage: {semantic_memory[0]*100:.1f}%")
    print(f"  Procedural memory usage: {procedural_memory[0]*100:.1f}%")
    
    # Get memory access patterns
    print("\nMemory Access Patterns:")
    patterns = profiler.getMemoryAccessPatterns()
    for memory_type, pattern in patterns.items():
        print(f"  {memory_type}: {len(pattern)} data points")
    
    print("\n")

def example_development_analysis():
    """
    Example 6: Development Stage Analysis
    ====================================
    
    This example demonstrates development stage analysis.
    """
    print("=" * 60)
    print("Example 6: Development Stage Analysis")
    print("=" * 60)
    
    # Create brain
    print("Creating brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize brain
    print("Initializing brain...")
    brain.initialize()
    
    # Create development stage analyzer
    print("Creating development stage analyzer...")
    developer = createDevelopmentStageAnalyzer(brain)
    
    # Run simulation
    print("Running simulation...")
    for i in range(200):
        brain.step(1.0)
    
    # Get development analysis
    print("\nDevelopment Stage Analysis:")
    print(developer.getDevelopmentSummary())
    
    # Get current stage
    stage = developer.getCurrentStage()
    progress = developer.getStageProgress()
    age = developer.getDevelopmentalAge()
    
    print(f"\nDevelopment Stage: {stage}")
    print(f"Stage Progress: {progress*100:.1f}%")
    print(f"Developmental Age: {age}")
    
    # Check critical periods
    if developer.isInCriticalPeriod():
        cp_progress = developer.getCriticalPeriodProgress()
        print(f"Critical Period: Active ({cp_progress*100:.1f}% complete)")
    
    # Get plasticity modifiers
    print("\nPlasticity Modifiers:")
    modifiers = developer.getPlasticityModifiers()
    print(f"  Learning rate: {modifiers[0]:.4f}")
    print(f"  Structural plasticity: {modifiers[1]:.4f}")
    print(f"  Synaptic plasticity: {modifiers[2]:.4f}")
    print(f"  Homeostatic plasticity: {modifiers[3]:.4f}")
    
    # Get growth metrics
    print("\nBrain Growth Metrics:")
    growth = developer.getBrainGrowthMetrics()
    print(f"  Neuron count: {int(growth[0])}")
    print(f"  Synapse count: {int(growth[1])}")
    print(f"  Region count: {int(growth[2])}")
    print(f"  Brain volume: {growth[3]:.4f}")
    print(f"  Brain complexity: {growth[4]:.4f}")
    
    print("\n")

def example_experiment_management():
    """
    Example 7: Experiment Management
    =================================
    
    This example demonstrates experiment management and batch execution.
    """
    print("=" * 60)
    print("Example 7: Experiment Management")
    print("=" * 60)
    
    # Create experiment manager
    print("Creating experiment manager...")
    from pynlm.advanced_api import createExperimentManager
    exp_manager = createExperimentManager()
    
    # Configure experiment manager
    exp_manager.configure({
        "results_directory": "./experiments",
        "num_repetitions": 3
    })
    
    # Define experiment setup and evaluation functions
    def setup_agent(agent):
        print(f"Setting up agent...")
        agent.initialize()
    
    def step_agent(agent, dt):
        agent.update(dt)
    
    def evaluate_agent(agent):
        return agent.get_reward()
    
    # Run multiple experiments
    print("\nRunning experiments...")
    for i in range(3):
        print(f"\nExperiment {i + 1}:")
        result = exp_manager.runExperiment(
            name=f"experiment_{i + 1}",
            setup=setup_agent,
            step=step_agent,
            evaluate=evaluate_agent
        )
        print(f"  Performance score: {result.performance_score:.4f}")
        print(f"  Success: {result.success}")
        print(f"  Duration: {result.duration:.2f} seconds")
    
    # Run batch of experiments
    print("\nRunning batch of experiments...")
    batch_results = exp_manager.runBatch(2)
    
    # Get batch statistics
    print("\nBatch Statistics:")
    stats = exp_manager.getGlobalStatistics()
    for key, value in stats.items():
        print(f"  {key}: {value:.4f}")
    
    # Compare experiments
    print("\nExperiment Comparison:")
    comparison = exp_manager.compareExperiments(0, 1)
    for key, value in comparison.items():
        print(f"  {key}: {value:.4f}")
    
    # Export all results
    print("\nExporting results...")
    exp_manager.exportAllResults()
    
    print("\n")

def example_visualization_and_export():
    """
    Example 8: Visualization and Data Export
    ========================================
    
    This example demonstrates visualization and data export capabilities.
    """
    print("=" * 60)
    print("Example 8: Visualization and Data Export")
    print("=" * 60)
    
    # Create visualization helper
    print("Creating visualization helper...")
    viz_helper = VisualizationHelper()
    
    # Generate sample data
    print("Generating sample data...")
    
    # Time series data
    time_steps = 100
    time_series = [i * 0.1 + (i % 10) * 0.5 for i in range(time_steps)]
    
    # Activity data
    activity_data = [[(i + j) * 0.01 for j in range(10)] for i in range(10)]
    
    # Learning curve data
    learning_curve = [1.0, 1.5, 2.0, 2.3, 2.5, 2.7, 2.8, 2.9, 3.0, 2.9]
    
    # Create visualizations
    print("Creating visualizations...")
    
    # Activity heatmap
    viz_helper.plotActivityHeatmap(activity_data, "activity_heatmap.png")
    print("Created activity heatmap")
    
    # Learning curve
    viz_helper.plotLearningCurve(learning_curve, "learning_curve.png")
    print("Created learning curve")
    
    # Time series
    viz_helper.plotTimeSeries([activity_data[0], activity_data[5]], 
                             ["Neuron 0", "Neuron 5"], "time_series.png")
    print("Created time series plot")
    
    # Comparison plot
    viz_helper.plotComparison([activity_data], ["Activity"], "comparison.png")
    print("Created comparison plot")
    
    # Export data
    print("\nExporting data...")
    
    # Export to CSV
    viz_helper.exportToCSV(activity_data, "activity_data.csv")
    print("Exported activity data to CSV")
    
    # Export to JSON
    import json
    viz_helper.exportToJSON({"activity": activity_data}, "activity_data.json")
    print("Exported activity data to JSON")
    
    # Export to MATLAB
    viz_helper.exportToMatlab(activity_data, "activity_data.mat")
    print("Exported activity data to MATLAB format")
    
    # Generate color map
    color_map = viz_helper.generateColorMap("viridis")
    print(f"Generated viridis color map with {len(color_map.split(','))} colors")
    
    # Compute histogram
    histogram = viz_helper.computeHistogram(time_series, bins=20)
    print(f"Computed histogram with {len(histogram)} bins")
    
    print("\n")

def example_comprehensive_simulation():
    """
    Example 9: Comprehensive Simulation with Multiple Components
    ============================================================
    
    This example demonstrates a comprehensive simulation using multiple
    components of the Advanced API.
    """
    print("=" * 60)
    print("Example 9: Comprehensive Simulation")
    print("=" * 60)
    
    # Create all components
    print("Creating all components...")
    
    # Brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Create agent
    agent = SimpleAgent(brain, world, "main_agent")
    
    # Create simulation runner
    sim_runner = SimulationRunner(num_steps=200, dt=1.0)
    sim_runner.addAgent(agent)
    
    # Create brain profiler
    brain_profiler = createBrainProfiler(brain)
    
    # Create neuron activity monitor
    neuron_monitor = createNeuronActivityMonitor(brain)
    
    # Create network analyzer
    network_analyzer = createNetworkConnectivityAnalyzer(brain)
    
    # Create neuromodulation analyzer
    neuromod_analyzer = createNeuromodulationAnalyzer(brain)
    
    # Create development analyzer
    dev_analyzer = createDevelopmentStageAnalyzer(brain)
    
    # Run comprehensive simulation
    print("Running comprehensive simulation...")
    
    # Start monitoring and profiling
    neuron_monitor.startMonitoring()
    brain_profiler.start()
    
    # Run simulation steps
    for step in range(200):
        agent.update(1.0)
        
        # Simulate some memory and neuromodulation operations
        if step % 20 == 0:
            # Store memory
            pattern = np.random.randn(50).astype(np.float32)
            brain.getWorkingMemory().store(pattern, 1.0)
            
            # Simulate reward
            agent.receiveReward(np.random.random() * 2.0)
        
        # Step brain and world
        brain.step(1.0)
        world.update(1.0)
    
    # Stop monitoring and profiling
    neuron_monitor.stopMonitoring()
    brain_profiler.stop()
    
    # Get comprehensive results
    print("\n=== Comprehensive Results ===")
    
    # Agent results
    print(f"Agent: {agent.agent_id}")
    print(f"  Active: {agent.is_active()}")
    print(f"  Reward: {agent.get_reward():.4f}")
    print(f"  Firing rate: {agent.get_firing_rate():.4f}")
    print(f"  Spike count: {agent.get_spike_count()}")
    
    # Brain profiler results
    print(f"\nBrain Profiler:")
    print(brain_profiler.getProfileSummary())
    
    # Neuron activity monitor results
    print(f"\nNeuron Activity Monitor:")
    print(neuron_monitor.getActivitySummary())
    
    # Network analysis results
    print(f"\nNetwork Analysis:")
    print(network_analyzer.getConnectivitySummary())
    
    # Neuromodulation analysis results
    print(f"\nNeuromodulation Analysis:")
    print(neurom_analyzer.getModulationSummary())
    
    # Development analysis results
    print(f"\nDevelopment Analysis:")
    print(dev_analyzer.getDevelopmentSummary())
    
    # Simulation runner statistics
    print(f"\nSimulation Statistics:")
    stats = sim_runner.getStatistics()
    for key, value in stats.items():
        print(f"  {key}: {value:.4f}")
    
    # Save comprehensive results
    print("\nSaving comprehensive results...")
    sim_runner.saveResults("comprehensive_simulation")
    
    # Export visualization
    print("Creating visualization...")
    viz_helper = VisualizationHelper()
    viz_helper.plotLearningCurve(learning_curve, "comprehensive_simulation_learning.png")
    
    print("\n")

def main():
    """Main function to run all examples"""
    print("\n" + "=" * 80)
    print("NLM Advanced API Examples")
    print("=" * 80)
    print("\nThis script demonstrates the use of the NLM Advanced API.")
    print("Each example shows different aspects of the advanced functionality.")
    print("\n")
    
    # Define learning curve for tracking
    global learning_curve
    learning_curve = []
    
    try:
        # Run all examples
        example_basic_simulation()
        example_advanced_brain_analysis()
        example_neuron_activity_monitoring()
        example_network_analysis()
        example_memory_system_profiling()
        example_development_analysis()
        example_experiment_management()
        example_visualization_and_export()
        example_comprehensive_simulation()
        
        print("\n" + "=" * 80)
        print("All examples completed successfully!")
        print("=" * 80)
        
    except Exception as e:
        print(f"\nError running examples: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
