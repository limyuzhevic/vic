import pynlm
import numpy as np
import time

print("=== Enhanced NLM Python Bindings Demo ===")
print("Version:", pynlm.__version__)

# Test 1: Create a simple brain with default config
print("\n1. Creating a brain with default config...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Test 2: Initialize and step the brain
print("\n2. Initializing and stepping the brain...")
brain.initialize()
for i in range(10):
    brain.step(i)
print(f"After 10 steps: {brain.getTotalSpikeCount()} total spikes, {brain.getFiringNeuronCount()} currently firing")

# Test 3: Create a simple world
print("\n3. Creating a simple world...")
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)
print(f"Created world {world.getWidth()}x{world.getHeight()}")

# Test 4: Create an agent brain
print("\n4. Creating agent brain...")
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
print(f"Agent brain initialized")
print(f"Expected sensory input size: {agent.getSensoryInputSize()}")
print(f"Expected motor output size: {agent.getMotorOutputSize()}")

# Test 5: Create and configure sensory inputs
print("\n5. Creating sensory inputs...")
vision = pynlm.createVision(8, 8, 3)
# Create some dummy image data
image_data = np.random.randn(8*8*3).astype(np.float32)
vision.setData(image_data.tolist())
print(f"Created vision input: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")

audio = pynlm.createAudio(44100, 1024)
audio_data = np.random.randn(1024).astype(np.float32)
audio.setData(audio_data.tolist())
print(f"Created audio input: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")

internal_signals = pynlm.createInternalSignals()
for i in range(5):
    internal_signals.addSignal(np.random.randn())
print(f"Created internal signals with {len(internal_signals.getAllSignals())} values")

# Test 6: Create actions
print("\n6. Creating actions...")
move_forward = pynlm.createAction(pynlm.ActionType.MoveForward)
print(f"Created action: {move_forward.getName()} (type: {move_forward.getType()})")

move_with_params = pynlm.createAction(pynlm.ActionType.Custom, [0.5, 1.0, 2.0])
print(f"Created custom action with parameters: {move_with_params.getParameters()}")

# Test 7: Run a simulation loop
print("\n7. Running simulation loop...")
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

for step in range(20):
    world.update(0.1)
    
    # Create a simple percept from the world
    percept = pynlm.SensoryPercept()
    percept.setVision(vision)
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Step the brain
    brain.step(step)
    
    # Get motor command
    action = agent.decodeMotorCommand()
    
    # Apply action to world (simplified)
    # In a real scenario, you'd apply the actual motor command
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = np.random.randn()
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 5 == 0:
        print(f"Step {step}: Firing neurons={brain.getFiringNeuronCount()}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}")

# Test 8: Get brain statistics
print("\n8. Final brain statistics...")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Test 9: Save and load brain state
print("\n9. Testing brain state persistence...")
import tempfile
import os

with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as f:
    brain_path = f.name

brain.save(brain_path)
print(f"Saved brain state to: {brain_path}")

# Create a new brain and load the saved state
brain2 = pynlm.createBrain(config)
brain2.load(brain_path)
print(f"Loaded brain state from: {brain_path}")
print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")

# Clean up
os.unlink(brain_path)

# Test 10: Configuration utilities
print("\n10. Configuration utilities...")
config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 500)
config2.setFloat("brain.synapse_density", 0.15)
config2.setBool("plasticity.stdp.enable", True)
config2.setDouble("neuromod.dopamine.scale", 2.0)

print(f"Config has brain.neuron_count: {config2.has('brain.neuron_count')}")
print(f"Config keys: {config2.getKeys()}")

# NEW FEATURE TESTS

# Test 11: Advanced Configuration Methods
print("\n11. Advanced Configuration Methods...")

# Test setDefaults
print("\n   a) Testing Config.setDefaults...")
config3 = pynlm.createDefaultConfig()
print(f"   Before setDefaults: stdp={config3.getBool('plasticity.stdp.enable', False)}, "
      f"neuron_count={config3.getInt('brain.neuron_count', 0)}")

config3.setDefaults(pynlm.ConfigType.Learning)
print(f"   After Learning setDefaults: stdp={config3.getBool('plasticity.stdp.enable', False)}, "
      f"neuron_count={config3.getInt('brain.neuron_count', 0)}")

# Test getStats
print("\n   b) Testing Config.getStats...")
config_stats = pynlm.createDefaultConfig()
config_stats.setInt("test.key1", 42)
config_stats.setBool("test.key2", True)
stats = config_stats.getStats()
print(f"   Configuration stats: {stats}")

# Test clone
print("\n   c) Testing Config.clone...")
config_original = pynlm.createDefaultConfig()
config_original.setInt("clone.test", 123)
config_clone = config_original.clone()
print(f"   Original has clone.test: {config_original.has('clone.test')}")
print(f"   Clone has clone.test: {config_clone.has('clone.test')}")

# Test 12: Memory System Methods
print("\n12. Memory System Methods...")

# Get working memory
if hasattr(brain, 'getWorkingMemory'):
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"   WorkingMemory active traces: {working_memory.getActiveTraces()}")

# Get episodic memory
if hasattr(brain, 'getSemanticMemory'):
    episodic_memory = brain.getSemanticMemory()  # Note: This is semantic memory, not episodic
    if episodic_memory:
        print(f"   SemanticMemory facts count: {len(episodic_memory.getAllFacts())}")
        print(f"   SemanticMemory concepts: {episodic_memory.getConcepts()}")

# Test 13: Event Handlers and Callbacks
print("\n13. Event Handlers and Callbacks...")

def reward_handler(reward):
    print(f"   Reward callback: Reward received = {reward:.3f}")

def novelty_handler(novelty):
    print(f"   Novelty callback: Novelty level = {novelty:.3f}")

def development_handler(stage):
    print(f"   Development callback: Stage = {stage}")

print("   Adding callbacks (note: callbacks are called immediately during registration)...")
agent.addRewardCallback(reward_handler)
agent.addNoveltyCallback(novelty_handler)
agent.addDevelopmentCallback(development_handler)

# Test 14: Utility Functions
print("\n14. Utility Functions...")

# Test runSimulation
print("\n   a) Testing pynlm.runSimulation...")
# Create fresh brain/world/agent for clean test
brain_test = pynlm.createBrain(config)
brain_test.initialize()
world_test = pynlm.createSimpleWorld()
world_test.configure(10, 10, 4, 4)
world_test.reset()
agent_test = pynlm.createAgentBrain(brain_test)
agent_test.initialize(world_test)

spikes = pynlm.runSimulation(brain_test, world_test, agent_test, 50)
print(f"   Simulation completed, total spikes: {spikes}")

# Test compareBrains
print("\n   b) Testing pynlm.compareBrains...")
brain_compare1 = pynlm.createBrain(config)
brain_compare2 = pynlm.createBrain(config)
for i in range(10):
    brain_compare1.step(i)
    brain_compare2.step(i)

comparison = pynlm.compareBrains(brain_compare1, brain_compare2)
print(f"   Brain comparison: {comparison}")

# Test benchmarkPerformance
print("\n   c) Testing pynlm.benchmarkPerformance...")
config_bench = pynlm.createDefaultConfig()
metrics = pynlm.benchmarkPerformance(config_bench, 100)
print(f"   Performance metrics: {metrics}")

# Test 15: Visualization Methods
print("\n15. Visualization Methods...")

# Test Brain.generateVisualization
print("\n   a) Testing Brain.generateVisualization...")
viz_data = brain.generateVisualization()
print(f"   Visualization data generated: {len(viz_data)} elements")

# Test SimpleWorld.createMap
print("\n   b) Testing SimpleWorld.createMap...")
world_map = world.createMap()
print(f"   World map created: {len(world_map)} rows x {len(world_map[0])} columns")

# Test AgentBrain.createSensoryMap
print("\n   c) Testing AgentBrain.createSensoryMap...")
sensory_map = agent.createSensoryMap()
print(f"   Sensory map created: {len(sensory_map)} rows x {len(sensory_map[0])} columns")

print("\n=== All Advanced Feature Tests Complete ===")