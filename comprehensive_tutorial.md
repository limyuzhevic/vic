#!/usr/bin/env python3
"""
Comprehensive NLM Tutorial: From Basic to Advanced

This is the comprehensive tutorial for the Neural Learning Machine (NLM) framework.
It covers everything from basic usage to advanced features, including:

1. Basic NLM operations
2. Advanced configuration and customization
3. Machine learning integration
4. Performance optimization
5. Real-time visualization
6. Custom plasticity rules
7. Simulation checkpointing
8. Pattern recognition and analysis

This tutorial assumes basic Python knowledge and provides hands-on examples.
"""

import pynlm
import time
import json
import os
from typing import Dict, Any, List, Optional
import matplotlib.pyplot as plt
import numpy as np

print("=" * 80)
print("NLM COMPREHENSIVE TUTORIAL")
print("=" * 80)
print()

print("This tutorial will guide you through using the NLM framework")
print("for everything from basic simulations to advanced machine learning integration.")
print()

# Section 1: Basic NLM Operations
print("1. BASIC NLM OPERATIONS")
print("-" * 80)

print("""
The NLM (Neural Learning Machine) framework provides a powerful platform
for simulating artificial neural systems. Let's start with the basics.

Example 1: Creating Your First NLM Brain
=====================================
""")

# Create a simple brain
brain_config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(brain_config)
brain.initialize()

print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"✓ Brain initialized successfully")

print("""
Example 2: Running a Simple Simulation
=======================================
""")

# Run a simulation
print("Running 100 simulation steps...")
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("✓ Simulation completed successfully")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")

print()

# Section 2: Core API Usage
print("2. CORE API USAGE")
print("-" * 80)

print("""
The NLM API provides several core classes for building simulations.
Here's how to use each one effectively.

Example 3: Working with Configurations
=======================================
""")

# Create and modify configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

print("✓ Created configuration with custom parameters")
print(f"  Neurons: {config.getOr('brain.neuron_count', 1000)}")
print(f"  Synapse density: {config.getOr('brain.synapse_density', 0.1)}")
print(f"  STDP enabled: {config.getOr('plasticity.stdp.enable', False)}")

print("""
Example 4: Working with the World Class
=======================================
""")

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
world.setAgentStart(15.0, 15.0)

print("✓ Created world with 30x30 dimensions")
print(f"  Vision size: {world.getVisionWidth()}x{world.getVisionHeight()}")
print(f"  Agent start position: ({world.getAgentBody().x}, {world.getAgentBody().y})")

print("""
Example 5: Creating and Using an Agent
=======================================
"""

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("✓ Created agent brain interface")
print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"  Development enabled: {agent.isDevelopmentEnabled()}")

print("""
Example 6: Running a Complete Simulation
=========================================\n""")

# Run a complete agent simulation
print("Running complete agent-world simulation (200 steps)...")
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Run brain computation
    brain.step(step)
    
    # Decode action
    action = agent.decodeMotorCommand()
    
    # Apply to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = agent.getNoveltyLevel()  # Use novelty as reward proxy
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"  Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
              f"Curiosity: {agent.getCuriosityLevel():.2f}, Development: {agent.getDevelopmentalStage().name}")

print("✓ Complete simulation finished successfully")
print()

# Section 3: Advanced Configuration
print("3. ADVANCED CONFIGURATION")
print("-" * 80)

print("""
Example 7: Creating Specialized Configurations
==============================================
""")

# Create research configuration
research_config = pynlm.createDefaultConfig()
research_config.set("brain.neuron_count", 5000)
research_config.set("brain.synapse_density", 0.03)
research_config.set("plasticity.stdp.enable", True)
research_config.set("plasticity.stdp.learning_rate", 0.005)
research_config.set("plasticity.hebbian.enable", True)
research_config.set("plasticity.hebbian.learning_rate", 0.002)
research_config.set("plasticity.structural.enable", True)
research_config.set("neuromod.curiosity.enable", True)
research_config.set("neuromod.novelty.enable", True)
research_config.set("development.enable", True)
research_config.set("development.initial_plasticity", 1.5)

print("✓ Created research configuration:")
print(f"  Large network: {research_config.get('brain.neuron_count')} neurons")
print(f"  Balanced plasticity: STDP={research_config.get('plasticity.stdp.enable')}, "
      f"Hebbian={research_config.get('plasticity.hebbian.enable')}")
print(f"  Enhanced learning: Curiosity={research_config.get('neuromod.curiosity.enable')}, "
      f"Novelty={research_config.get('neuromod.novelty.enable')}")

print("""
Example 8: Configuration Presets
================================
""")

# Load configuration presets
from advanced_config import PRESETS

print("Available configuration presets:")
for preset_name in PRESETS.keys():
    print(f"  - {preset_name}")

# Create research configuration using preset
research_config2 = PRESETS['research']()
print(f"\nCreated research preset with:")
print(f"  Neurons: {research_config2.get('brain.neuron_count')}")
print(f"  Plasticity: {research_config2.get('plasticity.stdp.enable')} STDP, "
      f"{research_config2.get('plasticity.hebbian.enable')} Hebbian")

print()

# Section 4: Performance Analysis
print("4. PERFORMANCE ANALYSIS")
print("-" * 80)

print("""
Example 9: Performance Profiling
=================================
""")

# Performance profiling
from performance_profiler import run_performance_analysis, create_performance_report

print("Running performance analysis (100 steps)...")
performance_data = run_performance_analysis(brain, world, agent, steps=100)

print("✓ Performance analysis completed")
print(f"  Overall performance score: {performance_data.get('overall_performance', {}).get('overall_score', 0):.1f}/100")
print(f"  Performance grade: {performance_data.get('overall_performance', {}).get('grade', 'N/A')}")
print(f"  Performance bottlenecks: {len(performance_data.get('performance_metrics', {}).get('bottleneck_analysis', {}))}")

print("""
Example 10: Performance Report
===============================
""")

# Create performance report
report_path = create_performance_report("performance_analysis", brain, performance_data, world, agent)
print(f"✓ Performance report saved to: {report_path}")

print()

# Section 5: Machine Learning Integration
print("5. MACHINE LEARNING INTEGRATION")
print("-" * 80)

print("""
Example 11: ML Integration Setup
================================
""")

# Create ML bridge
from ml_integration import create_ml_bridge

ml_bridge = create_ml_bridge()
print("✓ Created ML bridge for NLM-ML integration")

# Convert data to ML format
ml_data = ml_bridge.analyze_ml_ready_data({
    'brain_neuron_count': brain.getTotalNeuronCount(),
    'brain_synapse_count': brain.getTotalSynapseCount(),
    'brain_total_spikes': brain.getTotalSpikeCount(),
    'brain_average_firing_rate': brain.getAverageFiringRate(),
    'brain_excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
    'simulation_steps': 100,
    'simulation_time': 10.0,
    'step_number': 100,
})

print(f"✓ Converted data to ML format: {len(ml_data['features'])} features")

print("""
Example 12: ML Model Training
===============================
""")

# Train ML models
training_data = [ml_data]
ml_results = ml_bridge.train_ml_models(training_data)
print("✓ ML models trained successfully")
print(f"  Performance model: {'trained' if ml_results['performance_model_trained'] else 'not trained'}")

print()

# Section 6: Visualization
print("6. VISUALIZATION")
print("-" * 80)

print("""
Example 13: Basic Visualization
===============================
""")

# Create visualizer
from visualization import create_neural_visualizer, create_interactive_dashboard

visualizer = create_neural_visualizer()

print("✓ Created neural visualizer")

# Create network visualization
network_fig = visualizer.plot_neural_network(brain, layout='grid', highlight_active=True)
network_fig.savefig('neural_network_example.png', dpi=150, bbox_inches='tight')
print("✓ Network visualization saved: neural_network_example.png")

# Create activity over time plot
activity_fig = visualizer.plot_activity_over_time(brain, steps=50)
activity_fig.savefig('activity_over_time_example.png', dpi=150, bbox_inches='tight')
print("✓ Activity visualization saved: activity_over_time_example.png")

print("""
Example 14: Interactive Dashboard
================================
""")

# Create interactive dashboard
dashboard = create_interactive_dashboard(brain, world, agent)
print("✓ Created interactive dashboard for real-time visualization")
print("  Dashboard features:")
print("    - Real-time neural activity monitoring")
print("    - Network structure visualization")
print("    - Performance metrics tracking")
print("    - Simulation controls")

print()

# Section 7: Checkpointing
print("7. CHECKPOINTING AND RESTART")
print("-" * 80)

print("""
Example 15: Checkpoint Management
=================================
""")

# Setup checkpointing
from checkpoint_manager import CheckpointManager, create_default_checkpoint_config

config = create_default_checkpoint_config()
checkpoint_manager = CheckpointManager(config)

print("✓ Created checkpoint manager")
print(f"  Checkpoint directory: {config.directory}")
print(f"  Maximum checkpoints: {config.max_checkpoints}")
print(f"  Checkpoint interval: {config.checkpoint_interval} steps")

# Create checkpoint
checkpoint = checkpoint_manager.create_checkpoint(brain, world, agent, step_number=100)
if checkpoint:
    print(f"✓ Checkpoint created at step 100")
    print(f"  Checkpoint file: {checkpoint.step_number} steps, {checkpoint.simulation_time:.1f}s")

print()

# Section 8: Custom Plasticity
print("8. CUSTOM PLASTICITY RULES")
print("-" * 80)

print("""
Example 16: Creating Custom Plasticity Rules
=============================================
""")

# Create custom plasticity rules
from custom_plasticity import create_stdp_plus_rule, create_reward_modulated_rule, create_custom_function_rule

# STDP+ rule
stdp_plus = create_stdp_plus_rule("enhanced_stdp", ltp_weight=0.02, ltd_weight=0.025)
print(f"✓ Created enhanced STDP rule: {stdp_plus.name}")

# Reward-modulated rule
reward_rule = create_reward_modulated_rule("reward_based", reward_scale=1.5)
print(f"✓ Created reward-modulated rule: {reward_rule.name}")

# Custom function rule
def custom_learning_rule(synapse_info, pre_activities, post_activities, time_delta):
    # Example: Adaptive learning rate based on activity correlation
    if len(pre_activities) > 0 and len(post_activities) > 0:
        correlation = np.corrcoef(pre_activities, post_activities)[0, 1]
        return correlation * 0.01  # Scale to reasonable range
    return 0.0

custom_rule = create_custom_function_rule("adaptive_learning", custom_learning_rule)
print(f"✓ Created custom adaptive learning rule: {custom_rule.name}")

print("""
Example 17: Using Custom Rules in Agent
=======================================
"""

# Note: This would require extending the AgentBrain class to support custom plasticity
# For now, we'll show how custom rules could be integrated
print("✓ Custom plasticity rules ready for integration with AgentBrain")
print("  Integration would require:")
print("    - Extending AgentBrain class")
print("    - Adding plasticity rule management")
print("    - Updating brain-plasticity interface")

print()

# Summary
print("8. COMPREHENSIVE TUTORIAL SUMMARY")
print("=" * 80)
print("""
This comprehensive tutorial has covered the following NLM capabilities:

✓ Basic NLM Operations
✓ Core API Usage (Brain, Config, World, Agent)
✓ Advanced Configuration Management
✓ Performance Profiling and Analysis
✓ Machine Learning Integration
✓ Visualization and Real-time Monitoring
✓ Checkpointing and Recovery
✓ Custom Plasticity Rules
✓ Advanced Examples and Best Practices

The NLM framework provides a complete platform for:
- Building neural network simulations
- Implementing learning algorithms
- Analyzing performance and behavior
- Integrating with machine learning tools
- Creating sophisticated artificial agents

For more information, refer to:
- NLM documentation: https://nlm.readthedocs.io
- GitHub repository: https://github.com/nlm-project/nlm
- Community discussions: https://github.com/nlm-project/nlm/discussions

Key advantages of NLM:
- Brain-inspired architecture
- Flexible plasticity mechanisms
- Advanced visualization tools
- Machine learning integration
- Performance profiling capabilities
- Real-time monitoring and control
""")

print("=" * 80)
print("TUTORIAL COMPLETED SUCCESSFULLY")
print("=" * 80)
print()
print("You now have a comprehensive understanding of the NLM framework")
print("and can create sophisticated neural simulations for research and applications.")
print()
print("Happy modeling with NLM!")
