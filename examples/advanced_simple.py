#!/usr/bin/env python3
"""
Quick example demonstrating NLM Enhanced Python API features
Phase 7: Advanced Python API with Runtime Features
"""

import pynlm
import time

print("NLM Enhanced Python API Example")
print("=" * 50)

# 1. Basic Enhanced Brain Creation
print("\n1. Creating Enhanced Brain...")
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.2)

brain = pynlm.createBrain(config)
brain.initialize()
print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")

# 2. Low-Level Neural Component Access
print("\n2. Accessing Low-Level Neural Components...")
neurons = brain.getAllNeurons()
synapses = brain.getAllSynapses()

print(f"   ✓ Retrieved {len(neurons)} neurons and {len(synapses)} synapses")

# Demonstrate neuron manipulation
if neurons:
    neuron = neurons[0]
    original_threshold = neuron.getThreshold()
    neuron.setThreshold(-45.0)
    neuron.setType(pynlm.NeuronType.Modulatory)
    print(f"   ✓ Modified neuron: threshold {original_threshold} -> {neuron.getThreshold()}")
    print(f"   ✓ Neuron type: {neuron.getType()}")

# 3. Memory System Configuration
print("\n3. Configuring Advanced Memory Systems...")
working_memory = pynlm.createWorkingMemory()
working_memory.setCapacity(1000)
print(f"   ✓ Working memory capacity: {working_memory.getCapacity()}")

# Store and retrieve patterns
pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
working_memory.store(pattern)
retrieved = working_memory.retrieve(0)
print(f"   ✓ Stored and retrieved memory pattern")

episodic_memory = pynlm.createEpisodicMemory()
episodic_memory.storeExperience(" learned neural dynamics", 0.9, time.time())
print(f"   ✓ Episodic memory: {episodic_memory.getEpisodeCount()} episode(s)")

# 4. Neuromodulation Setup
print("\n4. Setting Up Neuromodulation Systems...")
reward = pynlm.createReward()
reward.setLearningRate(0.01)
reward.setReward(1.0)
print(f"   ✓ Reward system: rate={reward.getLearningRate()}, value={reward.getValue()}")

curiosity = pynlm.createCuriosity()
curiosity.setLevel(0.7)
curiosity.triggerExploration()
print(f"   ✓ Curiosity system: level={curiosity.getLevel()}, drive={curiosity.getDriveStrength()}")

# 5. Plasticity System Configuration
print("\n5. Configuring Plasticity Systems...")
stdp = pynlm.createSTDP()
stdp.setLearningRate(0.001)
hebbian = pynlm.createHebbian()
hebbian.setLearningRate(0.005)
structural = pynlm.createStructuralPlasticity()

print(f"   ✓ STDP: learning rate={stdp.getLearningRate()}")
print(f"   ✓ Hebbian: learning rate={hebbian.getLearningRate()}")
print(f"   ✓ Structural: synaptogenesis rate={structural.getSynaptogenesisRate()}")

# 6. Development System
print("\n6. Configuring Development System...")
development = pynlm.createDevelopmentSystem()
development.setStage(pynlm.DevelopmentalStage.CriticalPeriod)
development.setAge(5.0)

print(f"   ✓ Development stage: {development.getCurrentStage()}")
print(f"   ✓ Development age: {development.getCurrentAge()}")
print(f"   ✓ Critical period: {development.getCriticalPeriodActive()}")

# 7. Cognition Systems
print("\n7. Setting Up Cognition Systems...")
planner = pynlm.createNeuralPlanner()
planner.setPlanningHorizon(100)
concept = pynlm.createConceptFormation()
concept.setLearningRate(0.02)

print(f"   ✓ Neural planner: horizon={planner.getPlanningHorizon()}")
print(f"   ✓ Concept formation: learning rate={concept.getLearningRate()}")

# 8. Dynamic Command System
print("\n8. Dynamic Command System...")
command_registry = pynlm.createCommandRegistry()
print(f"   ✓ Command registry created with {command_registry.getCommandCount()} built-in commands")

# 9. Performance Monitoring
print("\n9. Performance Monitoring...")
memory_config = pynlm.createMemoryPoolConfig()
memory_config.blockSize = 4096
memory_config.numBlocks = 256
memory_config.enablePreallocation = True

simd_config = pynlm.createSIMDConfig()
simd_config.enableVectorization = True
simd_config.vectorWidth = 512

print(f"   ✓ Memory pools: {memory_config.numBlocks} blocks of {memory_config.blockSize} bytes")
print(f"   ✓ SIMD vectorization: {simd_config.vectorWidth}-bit enabled")

# 10. Integrated Simulation
print("\n10. Running Integrated Simulation...")
# Create world for simulation
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(20):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"   ✓ Simulation completed: {step + 1} steps")
print(f"   ✓ Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"   ✓ Final curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"   ✓ Final developmental stage: {brain.getDevelopmentalStage()}")

print("\n" + "=" * 50)
print("Example completed successfully!")
print("=" * 50)
print("\nKey Features Demonstrated:")
print("• Low-level neural component access")
print("• Advanced memory systems")
print("• Neuromodulation (Reward, Curiosity)")
print("• Plasticity (STDP, Hebbian, Structural)")
print("• Development and maturation")
print("• Cognitive planning and concept formation")
print("• Dynamic command system")
print("• Performance monitoring")
print("• Integrated agent-world simulation")
print("\nThis example showcases the enhanced Python API capabilities of NLM Phase 7.")
