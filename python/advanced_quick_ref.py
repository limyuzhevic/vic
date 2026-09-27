# Advanced Features Quick Reference
# 
# This document provides a quick overview of the advanced Python API features
# included in the NLM Phase 6.5 release.

import pynlm
from pynlm_advanced import (
    SimulationPreset, BrainVisualizer, AgentTrainer,
    SimulationProfiler, create_preset_brain,
    create_agent_world_setup, setup_debug_monitor
)

print("=== NLM Advanced Features Quick Reference ===")
print()

# 1. Presets Demo
print("1. PRESETS - Quick Brain Creation")
print("-" * 40)
for preset in SimulationPreset:
    config = create_preset_brain(preset)
    brain = config['brain']
    print(f"{preset.value:20} - {brain.getTotalNeuronCount():4d} neurons")

print()
print("2. SYSTEM SETUP - Complete Agent-World")
print("-" * 40)
system = create_agent_world_setup(vision_width=8, vision_height=8)
print(f"Vision: {system['vision_size']} pixels")
print(f"Neurons: {system['brain'].getTotalNeuronCount()}")
print(f"Motor commands: {system['agent'].getMotorOutputSize()}")

print()
print("3. VISUALIZATION - Brain Activity Analysis")
print("-" * 40)
visualizer = BrainVisualizer(system['brain'])
print("BrainVisualizer created successfully")
print("- collect_activity_data(steps, interval)")
print("- analyze_patterns(activity_data)")

print()
print("4. TRAINING - Agent Training Utilities")
print("-" * 40)
trainer = AgentTrainer(system['brain'], system['world'], system['agent'])
print("AgentTrainer created successfully")
print("- train_with_custom_reward(reward_function, episodes)")
print("- train_with_curriculum(curriculum, base_steps)")

print()
print("5. PROFILING - Performance Analysis")
print("-" * 40)
profiler = SimulationProfiler(system['brain'], system['world'], system['agent'])
print("SimulationProfiler created successfully")
print("- profile_simulation(brain, world, agent, steps)")
print("- generate_performance_report(profile_data)")

print()
print("6. DEBUGGING - Monitoring and Logging")
print("-" * 40)
monitor = setup_debug_monitor(
    brain=system['brain'],
    world=system['world'],
    agent=system['agent'],
    log_level="INFO",
    enable_profiling=True
)
print("Debug monitor setup successful")
print(f"Log level: {monitor['log_level']}")
print(f"Profiling enabled: {monitor['profiling_enabled']}")

print()
print("=== COMMON WORKFLOWS ===")
print()

print("QUICK SIMULATION:")
print("-" * 20)
system = create_agent_world_setup(vision_width=16, vision_height=16)
for step in range(100):
    system['world'].update(0.1)
    percept = system['world'].getSensoryPercept()
    system['agent'].processSensoryInput(percept)
    system['brain'].step(step)
    action = system['agent'].decodeMotorCommand()
    system['world'].applyMotorCommand(action, system['world'].getSimulationTime())

print("✓ Quick simulation completed (100 steps)")

print()
print("ADVANCED ANALYSIS:")
print("-" * 20)
visualizer = BrainVisualizer(system['brain'])
activity_data = visualizer.collect_activity_data(100, 10)
analysis = visualizer.analyze_patterns(activity_data)
print(f"✓ Analysis complete - Firing rate trend: {analysis['firing_rate_stats']['trend']:.4f}")

print()
print("=== ALL ADVANCED FEATURES AVAILABLE ===")
print()
print("Key Classes:")
print("  • BrainVisualizer - Neural activity analysis")
print("  • AgentTrainer - Advanced training utilities")
print("  • SimulationProfiler - Performance monitoring")
print("  • setup_debug_monitor() - Comprehensive debugging")
print()
print("Key Functions:")
print("  • create_preset_brain() - Preset brain creation")
print("  • create_agent_world_setup() - Complete system setup")
print()
print("Enums:")
print("  • SimulationPreset - 7 predefined simulation types")
print()
print("Ready to use all advanced NLM features!")
