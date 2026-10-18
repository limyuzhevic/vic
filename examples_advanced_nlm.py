#!/usr/bin/env python3
"""
Comprehensive NLM advanced usage examples
This script demonstrates all the advanced features and improvements made to NLM.
Run with: python3 examples_advanced_nlm.py
"""

import pynlm
import time

print("NLM Advanced Usage Examples")
print("=" * 60)

print("\n1. ADVANCED BRAIN CONFIGURATION")
print("-" * 40)

# Create a custom configuration with advanced settings
config = pynlm.createDefaultConfig()

# Advanced brain configuration
config.set("brain.neuron_count", 2000)  # More neurons for complexity
config.set("brain.synapse_density", 0.15)  # Moderate connectivity
config.set("brain.connection_probability", 0.1)
config.set("brain.initial_weight_mean", 0.6)  # Higher initial weights
config.set("brain.initial_weight_std", 0.15)  # More variation

# Advanced plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)  # Slower learning
config.set("plasticity.stdp.tau_plus", 25.0)  # Longer LTP time constant
config.set("plasticity.stdp.tau_minus", 25.0)  # Longer LTD time constant
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Advanced neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.5)  # Stronger dopamine
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

print("✓ Advanced brain configuration created")
print(f"  Neurons: {config.getOr('brain.neuron_count', 1000)}")
print(f"  Synapse density: {config.getOr('brain.synapse_density', 0.1)}")
print(f"  STDP learning rate: {config.getOr('plasticity.stdp.learning_rate', 0.001)}")

print("\n2. COMPREHENSIVE AGENT SETUP")
print("-" * 40)

# Create brain and world
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(
    width=40,           # Larger world
    height=30,          # Different aspect ratio
    visionWidth=12,     # Better vision resolution
    visionHeight=12     # Higher resolution
)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("✓ Comprehensive agent setup complete")
print(f"  World dimensions: {world.getWidth()} x {world.getHeight()}")
print(f"  Agent brain initialized with world")

print("\n3. ADVANCED AGENT CONFIGURATION")
print("-" * 40)

# Load an advanced preset
agent.loadPreset("balanced")
print("✓ Loaded 'balanced' preset")

# Fine-tune advanced parameters
agent.setExplorationWeight(0.4)
agent.setExploitationWeight(0.6)
agent.setNoveltyThreshold(0.5)
agent.setRiskTolerance(0.5)

agent.setDopamineGain(1.2)
agent.setCuriosityGain(0.9)
agent.setNoveltyGain(1.1)

agent.setDevelopmentRate(1.1)
agent.setMaturationSpeed(1.0)
agent.setLearningRate(0.02)

print("✓ Advanced agent parameters configured")
print(f"  Exploration weight: {agent.getExplorationWeight()}")
print(f"  Exploitation weight: {agent.getExploitationWeight()}")
print(f"  Novelty threshold: {agent.getNoveltyThreshold()}")
print(f"  Risk tolerance: {agent.getRiskTolerance()}")
print(f"  Development rate: {agent.getDevelopmentRate()}")

print("\n4. SUBSYSTEM CONTROL")
print("-" * 40)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("✓ All advanced subsystems enabled")
print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"  Development: {agent.isDevelopmentEnabled()}")
print(f"  Curiosity: {agent.isCuriosityEnabled()}")

print("\n5. DEBUG AND PROFILING")
print("-" * 40)

# Enable debug and profiling
agent.enableDebugMode(True)
agent.enableProfiling(True)

print("✓ Debug and profiling enabled")

# Get detailed debug information
debug_info = agent.getDebugInfo()
print(f"  Debug info available: {len(debug_info)} characters")
print("  Debug info preview:")
for line in debug_info.split('\n')[:10]:  # First 10 lines
    if line.strip():
        print(f"    {line}")

# Get profiling data
profiling_data = agent.getProfilingData()
print(f"  Profiling data points: {len(profiling_data)}")

print("\n6. COMPREHENSIVE SIMULATION")
print("-" * 40)

# Run an advanced simulation with detailed logging
steps = 500
start_time = time.time()

for step in range(steps):
    # Update world
    world.update(0.05)  # Slightly slower timestep
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step, step * config.getOr('simulation_timestep', 0.001))
    
    # Get and apply action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation
    reward = 0.0
    if percept.getInternal():
        reward = percept.getInternal()[0] * 0.1  # Scale reward
    
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.05)
    
    # Record performance
    performance = agent.getPerformanceScore()
    agent.recordPerformance(performance)
    
    # Log progress
    if step % 50 == 0:
        elapsed = time.time() - start_time
        print(f"  Step {step:3d} ({elapsed:6.2f}s) - "
              f"Firing: {brain.getFiringNeuronCount():4d}, "
              f"Spikes: {brain.getTotalSpikeCount():6d}, "
              f"Performance: {performance:6.4f}")

elapsed_time = time.time() - start_time
print(f"\n✓ Comprehensive simulation complete")
print(f"  Total time: {elapsed_time:.2f} seconds")
print(f"  Steps per second: {steps/elapsed_time:.1f}")
print(f"  Final firing rate: {brain.getAverageFiringRate():.4f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")

print("\n7. BRAIN INTROSPECTION")
print("-" * 40)

# Detailed brain analysis
print("  Brain Statistics:")
print(f"    Total neurons: {brain.getTotalNeuronCount()}")
print(f"    Total synapses: {brain.getTotalSynapseCount()}")
print(f"    Active neurons: {brain.getActiveNeuronCount()}")
print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
print(f"    Average firing rate: {brain.getAverageFiringRate():.4f} Hz")
print(f"    Total spikes: {brain.getTotalSpikeCount()}")
print(f"    E/I ratio: {brain.getExcitationInhibitionRatio():.4f}")
print(f"    Developmental stage: {brain.getDevelopmentalStage()}")

# Working memory analysis
if brain.getWorkingMemory():
    wm = brain.getWorkingMemory()
    print(f"    Working memory capacity: {wm.getCapacity()} items")
    print(f"    Working memory usage: {wm.getUsage()}%")

# Episodic memory analysis
if brain.getEpisodicMemory():
    em = brain.getEpisodicMemory()
    print(f"    Episodic memory episodes: {em.getEpisodeCount()}")
    print(f"    Episodic memory retention: {em.getRetentionLevel()}")

print("\n8. RESET AND STATE MANAGEMENT")
print("-" * 40)

print("  Before reset:")
print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
print(f"    Total spikes: {brain.getTotalSpikeCount()}")

# Run some steps to change state
for i in range(20):
    brain.step(steps + i)

print("  After 20 more steps:")
print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
print(f"    Total spikes: {brain.getTotalSpikeCount()}")

# Test reset functionality
print("  Testing reset options...")

# Development reset
agent.resetDevelopment()
print("    Development reset: Complete")

# Neuromodulation reset
agent.resetNeuromodulation()
print("    Neuromodulation reset: Complete")

# Learning reset
agent.resetLearning()
print("    Learning reset: Complete")

# All reset
agent.resetAll()
print("    All reset: Complete")

print("  After full reset:")
print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
print(f"    Total spikes: {brain.getTotalSpikeCount()}")

print("\n9. PERFORMANCE ANALYSIS")
print("-" * 40)

# Get performance history
perf_history = agent.getPerformanceHistory()
if perf_history:
    avg_perf = sum(perf_history) / len(perf_history)
    min_perf = min(perf_history)
    max_perf = max(perf_history)
    
    print(f"  Performance history: {len(perf_history)} entries")
    print(f"  Average performance: {avg_perf:.4f}")
    print(f"  Minimum performance: {min_perf:.4f}")
    print(f"  Maximum performance: {max_perf:.4f}")
    print(f"  Performance variance: {max_perf - min_perf:.4f}")

# Get memory usage
memory_usage = agent.getMemoryUsage()
print(f"  Memory usage: {memory_usage} bytes")

# Get development metrics
print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.4f}")
print(f"  Curiosity level: {agent.getCuriosityLevel():.4f}")
print(f"  Novelty level: {agent.getNoveltyLevel():.4f}")
print(f"  Prediction error: {agent.getPredictionError():.4f}")

print("\n10. CONFIGURATION ANALYSIS")
print("-" * 40)

print("  Current configuration:")
config_summary = config.summary()
for line in config_summary.split('\n'):
    if line.strip():
        print(f"    {line}")

print("\n11. BENCHMARK COMPARISON")
print("-" * 40)

print("  Performance benchmarks:")
print(f"    Average neural computation time: {brain.getAverageFiringRate():.4f} Hz")
print(f"    Firing neuron efficiency: {brain.getFiringNeuronCount() / brain.getTotalNeuronCount() * 100:.1f}%")
print(f"    Spike generation rate: {brain.getTotalSpikeCount() / steps:.1f} spikes/step")

print("\n12. ADVANCED FEATURES DEMONSTRATION")
print("-" * 40)

# Demonstrate advanced features
print("  Advanced features demonstrated:")
print("    ✓ Custom brain configuration with advanced parameters")
print("    ✓ Comprehensive agent setup with fine-tuned settings")
print("    ✓ Advanced agent parameter control (exploration/exploitation)")
print("    ✓ Neuromodulation control (dopamine, curiosity, novelty)")
print("    ✓ Development control (plasticity, maturation)")
print("    ✓ Learning rate control")
print("    ✓ Subsystem isolation and control")
print("    ✓ Debug and profiling capabilities")
print("    ✓ Brain introspection and analysis")
print("    ✓ Comprehensive reset functionality")
print("    ✓ Performance analysis and metrics")
print("    ✓ Configuration management")
print("    ✓ Benchmark comparison")

print("\n" + "=" * 60)
print("ADVANCED NLM USAGE EXAMPLES COMPLETE")
print("=" * 60)
print("\nThis example demonstrates:")
print("• How to create advanced neural configurations")
print("• How to set up sophisticated agents")
print("• How to control advanced agent parameters")
print("• How to enable and manage complex subsystems")
print("• How to debug and profile advanced simulations")
print("• How to analyze brain state and performance")
print("• How to manage simulation state and resets")
print("• How to benchmark and optimize performance")
print("\nThe NLM framework provides comprehensive tools for")
print("creating, simulating, and analyzing advanced neural systems!")
