This document provides advanced usage patterns and examples for the NLM Python bindings. It complements the beginner-focused `easy_usage.md` and expands upon `HOW_TO_USE.md` with more sophisticated examples and practical use cases.

# Advanced NLM Usage Guide

## Chapter 1: Direct Neuron and Synapse Manipulation

### 1.1 Fine-Grained Neural Control

```python
import pynlm

# Create a simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get regions and their neurons
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: {region.getName()}")
    
    # Iterate through neurons in each region
    for neuron_id in region.getNeuronIds():
        neuron = brain.getNeuron(neuron_id)
        
        # Monitor and manipulate individual neuron state
        print(f"  Neuron {neuron_id}: "
              f"potential={neuron.getMembranePotential():.1f}mV, "
              f"type={neuron.getType()}")
        
        # Inject external current
        if neuron.getMembranePotential() < -60.0:  # Depolarize if hyperpolarized
            neuron.injectCurrent(10.0)
        
        # Record artificial spike for testing
        if neuron_id.value % 7 == 0:  # Every 7th neuron
            neuron.recordSpike(brain.getRandomGenerator().getCurrentTime())

# Check spike statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### 1.2 Synaptic Connectivity Analysis

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Synaptic connectivity analysis:")

# Analyze excitatory synapses
synapse_count = brain.getTotalSynapseCount()
excitatory_count = 0
inhibitory_count = 0

for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    for synapse_id in region.getSynapseIds():
        synapse = brain.getSynapse(synapse_id)
        
        if synapse.isExcitatory():
            excitatory_count += 1
            # Examine synaptic strength
            weight = synapse.getWeight()
            if abs(weight) > 0.5:
                print(f"Strong excitatory synapse {synapse_id}: weight={weight:.3f}")
        
        elif synapse.isInhibitory():
            inhibitory_count += 1
            # Get presynaptic spike history
            pre_spikes = synapse.getPreSpikeHistory()
            if len(pre_spikes) > 10:
                print(f"Active inhibitory synapse {synapse_id}: "
                      f"presynaptic activity={len(pre_spikes)}")

print(f"Excitatory synapses: {excitatory_count}")
print(f"Inhibitory synapses: {inhibitory_count}")
print(f"Net excitation ratio: {excitatory_count / max(1, inhibitory_count):.2f}")

# Check plasticity flags
print("\nPlasticity status by synapse:")
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    for synapse_id in region.getSynapseIds():
        synapse = brain.getSynapse(synapse_id)
        flags = synapse.getPlasticityFlags()
        if flags.hebbian or flags.stdp or flags.reward_modulated:
            print(f"  Synapse {synapse_id}: Hebbian={flags.hebbian}, "
                  f"STDP={flags.stdp}, Reward={flags.reward_modulated}")
```

## Chapter 2: Plasticity System Configuration

### 2.1 STDP Learning Rule Configuration

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access and configure STDP
stdp = brain.getSTDP()

print("Current STDP configuration:")
print(f"LTP weight: {stdp.getLTPWeight()}")
print(f"LTD weight: {stdp.getLTDWeight()}")
print(f"Time constant: {stdp.getTimeConstant()} ms")

# Configure for specific learning behavior
stdp.configure(ltpWeight=0.02,  # Strong potentiation
               ltdWeight=0.025,  # Stronger depression
               tau=25.0)        # Faster decay

# Create a test synapse for experimentation
test_synapse = brain.createTestSynapse()

# Simulate spike pairs for different timing scenarios
print("\nTesting STDP with different spike timings:")

# Pre before post (potentiation)
stdp_simulator = pynlm.STDPSimulator()
pre_spikes = [0.0]   # Pre before post
post_spikes = [20.0] # 20ms later
pre_spikes = stdp_simulator.simulateSTDP(stdp, pre_spikes, post_spikes)
print(f"Pre→Post (20ms): Δw = {pre_spikes[0]:.4f}")

# Post before pre (depression)
post_spikes = [0.0]   # Post first
pre_spikes = [10.0]   # Pre 10ms later
pre_spikes = stdp_simulator.simulateSTDP(stdp, pre_spikes, post_spikes)
print(f"Post→Pre (10ms): Δw = {pre_spikes[0]:.4f}")

# Simultaneous spikes (minimal change)
pre_spikes = [0.0]
post_spikes = [0.0]
pre_spikes = stdp_simulator.simulateSTDP(stdp, pre_spikes, post_spikes)
print(f"Simultaneous: Δw = {pre_spikes[0]:.4f}")
```

### 2.2 Hebbian Learning Configuration

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access Hebbian plasticity
hebbian = brain.getHebbian()

print("Current Hebbian configuration:")
print(f"Learning rate: {hebbian.getLearningRate()}")
print(f"Max weight: {hebbian.getMaxWeight()}")
print(f"Min weight: {hebbian.getMinWeight()}")

# Configure for different learning scenarios
hebbian.setLearningRate(0.005)   # Conservative learning
hebbian.setMaxWeight(0.5)       # Lower saturation
hebbian.setMinWeight(-0.5)      # Allow depression

# Test Hebbian learning with correlated activity
print("\nSimulating Hebbian learning with correlated activity:")

hebbian_simulator = pynlm.Hebbiansimulator()

# Correlated firing patterns (high coactivity)
correlated_pre = [0.0, 10.0, 20.0]  # Regular spiking
correlated_post = [5.0, 15.0, 25.0]  # Slightly offset
weight_change = hebbian_simulator.calculateWeightChange(hebbian, correlated_pre, correlated_post)
print(f"Correlated spikes: Δw = {weight_change:.4f} (potentiation)")

# Uncorrelated firing (low coactivity)
uncorrelated_pre = [0.0, 100.0, 200.0]  # Sparse firing
uncorrelated_post = [50.0, 150.0, 250.0]  # Random offset
weight_change = hebbian_simulator.calculateWeightChange(hebbian, uncorrelated_pre, uncorrelated_post)
print(f"Uncorrelated spikes: Δw = {weight_change:.4f} (minimal change)")
```

## Chapter 3: Memory System Operations

### 3.1 Working Memory Manipulation

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

working_mem = brain.getWorkingMemory()

print("Working memory operations:")

# Store and retrieve neural states
for i in range(5):
    neuron_id = pynlm.NeuronId(i)
    value = 0.5 + 0.1 * i  # Test values
    
    working_mem.store(neuron_id, value)
    print(f"Stored value {value} for neuron {neuron_id}")

# Retrieve and process stored values
print("\nRetrieving and processing stored values:")
for i in range(5):
    neuron_id = pynlm.NeuronId(i)
    if working_mem.contains(neuron_id):
        retrieved = working_mem.retrieve(neuron_id)
        print(f"Retrieved {retrieved} for neuron {neuron_id}")
        
        # Apply some processing
        processed = retrieved * 0.8 + 0.1  # Simple transformation
        print(f"  Processed value: {processed}")

# Check memory statistics
print(f"\nMemory statistics:")
print(f"Capacity: {working_mem.getCapacity()}")
print(f"Current size: {working_mem.getCurrentSize()}")
print(f"Utilization: {working_mem.getCurrentSize() / working_mem.getCapacity() * 100:.1f}%")

# Decay simulation
print("\nSimulating memory decay:")
for decay_rate in [0.0, 0.1, 0.2]:
    working_mem.decay(decay_rate)
    print(f"After decay rate {decay_rate}: {working_mem.getCurrentSize()} items remaining")
```

### 3.2 Episodic Memory Management

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

episodic_mem = brain.getEpisodicMemory()

print("Episodic memory operations:")

# Create some episodic entries
for episode_id in range(3):
    item = pynlm.EpisodicMemoryItem()
    item.timestamp = episode_id * 100
    
    # Add neural activity patterns
    for i in range(5):
        neuron_id = pynlm.NeuronId(i * 10 + episode_id)
        value = 0.7 - 0.1 * i
        item.neurons.append(neuron_id)
        item.values.append(value)
    
    item.metadata = f"Episode_{episode_id}_test"
    
    episodic_mem.storeEpisode(item)
    print(f"Stored episode {episode_id}: {len(item.neurons)} neural patterns")

# Retrieve and analyze episodes
print("\nRetrieving and analyzing episodes:")
recent_episodes = episodic_mem.getRecentEpisodes(10)

for i, episode in enumerate(recent_episodes):
    print(f"Episode {i}:")
    print(f"  Timestamp: {episode.timestamp}")
    print(f"  Neural pattern size: {len(episode.neurons)}")
    print(f"  Average activation: {sum(episode.values) / len(episode.values):.2f}")
    print(f"  Metadata: {episode.metadata}")

# Memory consolidation simulation
print("\nTesting memory consolidation:")
for threshold in [0.5, 0.7, 0.9]:
    # Make a copy for testing
    test_mem = episodic_mem
    consolidated_before = test_mem.getEpisodeCount()
    test_mem.consolidate(threshold)
    consolidated_after = test_mem.getEpisodeCount()
    print(f"At threshold {threshold}: {consolidated_after} episodes remain "
          f"(removed {consolidated_before - consolidated_after})")
```

### 3.3 Semantic Memory Operations

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

semantic_mem = brain.getSemanticMemory()

print("Semantic memory operations:")

# Store semantic facts (knowledge)
facts_to_store = {
    "temperature_hot": "high_temp_threshold",
    "temperature_cold": "low_temp_threshold", 
    "food_available": "nutrient_source",
    "water_available": "hydration_source",
    "predator_nearby": "threat_detected"
}

for key, value in facts_to_store.items():
    semantic_mem.storeFact(key, value)
    print(f"Stored fact: '{key}' -> '{value}'")

# Retrieve and process facts
print("\nRetrieving semantic facts:")
retrieved_facts = [
    semantic_mem.retrieveFact("temperature_hot"),
    semantic_mem.retrieveFact("food_available"),
    semantic_mem.retrieveFact("predator_nearby")
]

for i, fact in enumerate(retrieved_facts):
    print(f"Fact {i}: '{fact}'")

# Semantic relationship queries
print("\nSemantic relationship queries:")
all_facts = semantic_mem.getAllFacts()
for key, value in all_facts:
    print(f"  {key} ↔ {value}")

# Check if certain concepts exist
required_concepts = ["temperature_hot", "water_available", "unknown_concept"]
for concept in required_concepts:
    exists = semantic_mem.hasFact(concept)
    print(f"  Concept '{concept}' exists: {exists}")
```

## Chapter 4: Neuromodulation System Control

### 4.1 Curiosity System Configuration

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

print("Neuromodulation system status:")

# Configure curiosity for exploration vs exploitation
print("\nCuriosity system configuration:")
print(f"Current level: {curiosity.getLevel():.3f}")
print(f"Novelty weight: {curiosity.getNoveltyWeight():.3f}")
print(f"Prediction error weight: {curiosity.getPredictionErrorWeight():.3f}")

# Configure for different behavioral strategies
curiosity.setNoveltyWeight(0.7)    # Novelty-seeking behavior
curiosity.setPredictionErrorWeight(0.3)

# Simulate curiosity updates
print("\nSimulating curiosity updates:")

novelty_detected = 0.8
prediction_error = 0.6

for step in range(5):
    curiosity.update(novelty_detected, prediction_error, 0.1)
    exploration_drive = curiosity.getExplorationDrive()
    print(f"Step {step}: curiosity={curiosity.getLevel():.3f}, "
          f"exploration_drive={exploration_drive:.3f}")

# Reset curiosity
curiosity.reset()
print(f"After reset: level={curiosity.getLevel():.3f}")
```

### 4.2 Dopamine System Configuration

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

dopamine = brain.getDopamine()

print("Dopamine system configuration:")
print(f"Current level: {dopamine.getLevel():.3f}")
print(f"Plasticity factor: {dopamine.getPlasticityFactor():.3f}")

# Configure reward processing
print("\nReward signaling simulation:")

rewards = [1.0, 0.5, -0.5, 0.0, 1.0]
errors = [0.0, 0.3, 0.7, 0.0, 0.2]

for i, (reward, error) in enumerate(zip(rewards, errors)):
    dopamine.signalReward(reward)
    dopamine.signalRewardPredictionError(error)
    
    print(f"Time {i}: reward={reward}, error={error}, "
          f"level={dopamine.getLevel():.3f}, "
          f"plasticity_factor={dopamine.getPlasticityFactor():.3f}")

# Direct level control for testing
print("\nDirect neuromodulator level control:")
for level in [0.0, 0.25, 0.5, 0.75, 1.0]:
    dopamine.setLevel(level)
    print(f"Set level {level}: plasticity factor={dopamine.getPlasticityFactor():.3f}")
```

## Chapter 5: Development System Control

### 5.1 Developmental Stage Management

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get development system
brain_dev = brain.getDevelopmentSystem()

print("Developmental stage management:")
print(f"Current stage: {brain.getDevelopmentalStage()}")

# Manual stage control
print("\nTesting manual stage transitions:")

for target_stage in [
    pynlm.DevelopmentalStage.Initial,
    pynlm.DevelopmentalStage.CriticalPeriod,
    pynlm.DevelopmentalStage.Maturation,
    pynlm.DevelopmentalStage.Adult,
    pynlm.DevelopmentalStage.Aging
]:
    brain.setDevelopmentalStage(target_stage)
    current_stage = brain.getDevelopmentalStage()
    
    print(f"  Set to {target_stage}: now at {current_stage}")
    
    # Check developmental effects based on stage
    dev_effects = brain_dev.getCurrentEffects()
    print(f"    Developmental effects: {dev_effects}")

# Automatic stage progression
print("\nAutomatic stage progression simulation:")
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Initial)

for step in range(10):
    brain.develop()  # Let the system handle development
    current_stage = brain.getDevelopmentalStage()
    print(f"Step {step}: {current_stage}")
    
    if current_stage == pynlm.DevelopmentalStage.Aging:
        print("  Reached final aging stage")
        break
```

## Chapter 6: Advanced Simulation Patterns

### 6.1 Learning Rate Scheduling

```python
import pynlm

def create_learning_schedule(brain):
    """Create a simulated learning rate schedule over time."""
    
    schedule = {
        "initial": {"stdp": 0.001, "hebbian": 0.005, "novelty": 0.0},
        "exploration": {"stdp": 0.01, "hebbian": 0.02, "novelty": 0.8},
        "consolidation": {"stdp": 0.005, "hebbian": 0.01, "novelty": 0.2},
        "expertise": {"stdp": 0.0005, "hebbian": 0.001, "novelty": 0.1}
    }
    
    return schedule

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

stdp = brain.getSTDP()
hebbian = brain.getHebbian()
curiosity = brain.getCuriosity()

# Implement learning phase transitions
print("Learning phase transitions:")

phases = ["initial", "exploration", "consolidation", "expertise"]
for phase in phases:
    schedule = create_learning_schedule(brain)
    
    # Apply phase-specific settings
    if phase == "initial":
        stdp.configure(ltpWeight=0.001, ltdWeight=0.001, tau=50.0)
        hebbian.setLearningRate(0.005)
        curiosity.setNoveltyWeight(0.0)
        curiosity.setPredictionErrorWeight(0.0)
        curiosity.reset()
    
    elif phase == "exploration":
        stdp.configure(ltpWeight=0.02, ltdWeight=0.02, tau=20.0)
        hebbian.setLearningRate(0.02)
        curiosity.setNoveltyWeight(0.7)
        curiosity.setPredictionErrorWeight(0.3)
        curiosity.update(0.5, 0.5, 1.0)
    
    elif phase == "consolidation":
        stdp.configure(ltpWeight=0.005, ltdWeight=0.005, tau=30.0)
        hebbian.setLearningRate(0.01)
        curiosity.setNoveltyWeight(0.2)
        curiosity.setPredictionErrorWeight(0.1)
        curiosity.update(0.2, 0.3, 1.0)
    
    elif phase == "expertise":
        stdp.configure(ltpWeight=0.0005, ltdWeight=0.0005, tau=100.0)
        hebbian.setLearningRate(0.001)
        curiosity.setNoveltyWeight(0.1)
        curiosity.setPredictionErrorWeight(0.1)
        curiosity.update(0.1, 0.1, 1.0)
    
    # Record phase status
    stdp_config = (stdp.getLTPWeight(), stdp.getLTDWeight(), stdp.getTimeConstant())
    hebbian_config = hebbian.getLearningRate()
    curiosity_config = curiosity.getLevel()
    
    print(f"  {phase.capitalize()} phase:")
    print(f"    STDP: LTP={stdp_config[0]:.4f}, LTD={stdp_config[1]:.4f}, τ={stdp_config[2]:.1f}ms")
    print(f"    Hebbian: learning_rate={hebbian_config:.4f}")
    print(f"    Curiosity: level={curiosity_config:.3f}")
```

### 6.2 Multi-Agent Environment Simulation

```python
import pynlm
import numpy as np

def simulate_multi_agent_environment(world_size=20, num_agents=3, steps=100):
    """Simulate multiple agents exploring and learning in a shared environment."""
    
    # Create shared environment
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, visionWidth=8, visionHeight=8)
    
    # Initialize agents with different configurations
    agents = []
    for i in range(num_agents):
        # Create agent with unique characteristics
        config = pynlm.createDefaultConfig()
        
        # Different agent configurations
        if i == 0:
            # Conservative agent (low curiosity, stable learning)
            config.set("neuromod.curiosity.novelty_weight", 0.2)
            config.set("neuromod.curiosity.prediction_error_weight", 0.1)
            config.set("plasticity.stdp.learning_rate", 0.001)
        
        elif i == 1:
            # Exploratory agent (high curiosity, rapid learning)
            config.set("neuromod.curiosity.novelty_weight", 0.8)
            config.set("neuromod.curiosity.prediction_error_weight", 0.6)
            config.set("plasticity.stdp.learning_rate", 0.01)
        
        else:
            # Balanced agent
            config.set("neuromod.curiosity.novelty_weight", 0.5)
            config.set("neuromod.curiosity.prediction_error_weight", 0.5)
            config.set("plasticity.stdp.learning_rate", 0.005)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        agents.append((agent, brain))
    
    print(f"Multi-agent simulation initialized with {num_agents} agents")
    print(f"World size: {world_size}x{world_size}")
    
    # Simulation loop
    for step in range(steps):
        # Update shared environment
        world.update(0.1)
        
        # Each agent senses and acts
        for i, (agent, brain) in enumerate(agents):
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processing
            brain.step(step)
            
            # Agent decision making
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learning from experience
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Development updates
            agent.updateDevelopment(0.1)
        
        # Print statistics every 20 steps
        if step % 20 == 0:
            print(f"\nStep {step}:")
            for i, (agent, brain) in enumerate(agents):
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                firing_rate = brain.getAverageFiringRate()
                
                print(f"  Agent {i}: curiosity={curiosity:.3f}, "
                      f"novelty={novelty:.3f}, firing_rate={firing_rate:.2f}Hz")
    
    print(f"\nSimulation completed. Final statistics:")
    for i, (agent, brain) in enumerate(agents):
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"  Agent {i}: curiosity={curiosity:.3f}, "
              f"novelty={novelty:.3f}, firing_rate={firing_rate:.2f}Hz")
    
    return agents, world

# Run multi-agent simulation
agents, world = simulate_multi_agent_environment(world_size=30, num_agents=3, steps=200)
```

## Chapter 7: Performance and Debugging

### 7.1 Performance Monitoring

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Performance monitoring:")

# Run performance test
num_steps = 1000
start_time = time.time()

for step in range(num_steps):
    brain.step(step)
    
    if step % 200 == 0:
        # Log performance metrics
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        spike_count = brain.getTotalSpikeCount()
        active_neurons = brain.getActiveNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        
        print(f"Step {step}: neurons={neuron_count}, "
              f"synapses={synapse_count}, spikes={spike_count}, "
              f"active={active_neurons}, firing={firing_neurons}")

end_time = time.time()
elapsed = end_time - start_time
speed = num_steps / elapsed

print(f"\nPerformance results:")
print(f"  Total steps: {num_steps}")
print(f"  Elapsed time: {elapsed:.2f}s")
print(f"  Speed: {speed:.0f} steps/second")

# Check system efficiency
spike_efficiency = brain.getTotalSpikeCount() / max(1, brain.getTotalNeuronCount())
print(f"  Spike efficiency: {spike_efficiency:.2f} spikes/neuron")
```

### 7.2 Debugging and State Inspection

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Debugging and state inspection:")

# Save initial state
brain.save("debug_initial_state.bin")

# Run some simulation steps
for step in range(50):
    brain.step(step)
    
    # Check for anomalies every 10 steps
    if step % 10 == 0:
        print(f"\nDebug step {step}:")
        
        # Check neuron health
        neuron_count = brain.getTotalNeuronCount()
        active_neurons = brain.getActiveNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        
        print(f"  Neurons: {neuron_count}, Active: {active_neurons}, Firing: {firing_neurons}")
        
        # Check for potential issues
        if firing_neurons == 0:
            print("  WARNING: No neurons firing - check sensory input or neuron parameters")
        
        if active_neurons > neuron_count * 0.9:
            print("  WARNING: Very high neuron activity - check thresholds and inputs")
        
        # Check developmental stage
        dev_stage = brain.getDevelopmentalStage()
        print(f"  Development stage: {dev_stage}")
        
        # Check neuromodulation levels
        dopamine = brain.getDopamine()
        curiosity = brain.getCuriosity()
        print(f"  Neuromodulation - Dopamine: {dopamine.getLevel():.3f}, "
              f"Curiosity: {curiosity.getLevel():.3f}")

# Restore from debug state
brain.load("debug_initial_state.bin")
print("\nRestored from debug state")

# Test system recovery
print("Testing system recovery:")
for step in range(20):
    brain.step(step)
    
    neuron_count = brain.getTotalNeuronCount()
    spike_count = brain.getTotalSpikeCount()
    print(f"  Step {step}: {neuron_count} neurons, {spike_count} total spikes")
```

## Chapter 8: Python Integration and Advanced Patterns

### 8.1 Context Manager for Brain Sessions

```python
import pynlm
import time

class BrainSession:
    """Context manager for managing brain simulation sessions."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
    
    def __enter__(self):
        """Enter the session - initialize all components."""
        print("Initializing brain session...")
        
        # Create and initialize brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world and agent
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        
        print("Brain session initialized successfully")
        return self
    
    def run(self, steps):
        """Run simulation steps."""
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Sense and act
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            self.agent.updateDevelopment(0.1)
        
        return self
    
    def get_stats(self):
        """Get session statistics."""
        if not self.brain:
            return {}
        
        return {
            "total_neurons": self.brain.getTotalNeuronCount(),
            "total_synapses": self.brain.getTotalSynapseCount(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "average_firing_rate": self.brain.getAverageFiringRate(),
            "development_stage": self.brain.getDevelopmentalStage().name,
            "curiosity_level": self.agent.getCuriosityLevel(),
            "novelty_level": self.agent.getNoveltyLevel()
        }
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit the session - cleanup."""
        print("Cleaning up brain session...")
        
        # Save final state
        if self.brain:
            self.brain.save("session_final_state.bin")
        
        # Cleanup
        self.brain = None
        self.world = None
        self.agent = None
        
        print("Brain session cleaned up")
        
        # Don't suppress exceptions
        return False

# Use the context manager
with BrainSession() as session:
    print("Running brain session...")
    session.run(100)
    
    stats = session.get_stats()
    print(f"\nSession statistics: {stats}")
```

### 8.2 Neural Network Analysis Utilities

```python
import pynlm
import numpy as np

class NeuralNetworkAnalyzer:
    """Utilities for analyzing neural network structure and function."""
    
    def __init__(self, brain):
        self.brain = brain
    
    def analyze_connectivity(self):
        """Analyze neural connectivity patterns."""
        regions = self.brain.getRegions()
        
        # Count connections between regions
        inter_region_connections = {}
        for region in regions:
            for other_region in regions:
                if region.getId() != other_region.getId():
                    conn_id = (region.getId().value, other_region.getId().value)
                    connections = len(region.getInterRegionConnections(other_region))
                    if connections > 0:
                        inter_region_connections[conn_id] = connections
        
        # Analyze within-region connectivity
        within_region_stats = []
        for region in regions:
            neurons = region.getNeurons()
            if len(neurons) > 1:
                # Calculate average connection density
                total_possible = len(neurons) * (len(neurons) - 1) / 2
                actual_connections = sum(len(n.getOutgoingSynapses()) for n in neurons) / 2
                density = actual_connections / total_possible if total_possible > 0 else 0
                
                within_region_stats.append({
                    "region_id": region.getId().value,
                    "neuron_count": len(neurons),
                    "connection_density": density,
                    "total_connections": int(actual_connections)
                })
        
        return {
            "inter_region_connections": inter_region_connections,
            "within_region_stats": within_region_stats
        }
    
    def analyze_activity_patterns(self, steps=100):
        """Analyze neural activity patterns over time."""
        activity_history = []
        
        for step in range(steps):
            self.brain.step(step)
            
            # Record activity metrics
            neuron_count = self.brain.getTotalNeuronCount()
            firing_neurons = self.brain.getFiringNeuronCount()
            average_firing_rate = self.brain.getAverageFiringRate()
            
            activity_history.append({
                "step": step,
                "firing_ratio": firing_neurons / max(1, neuron_count),
                "average_firing_rate": average_firing_rate
            })
        
        # Analyze patterns
        firing_ratios = [entry["firing_ratio"] for entry in activity_history]
        
        return {
            "activity_history": activity_history,
            "mean_firing_ratio": np.mean(firing_ratios),
            "std_firing_ratio": np.std(firing_ratios),
            "firing_variance": np.var(firing_ratios)
        }
    
    def detect_spike_patterns(self, window_size=50):
        """Detect spike patterns and periodicity."""
        regions = self.brain.getRegions()
        all_spikes = []
        
        for region in regions:
            neurons = region.getNeurons()
            for neuron in neurons:
                spikes = neuron.getSpikeHistory()
                all_spikes.extend(spikes)
        
        if len(all_spikes) < window_size:
            return {"insufficient_data": True}
        
        # Analyze spike timing
        spike_times = sorted(all_spikes)
        intervals = np.diff(spike_times)
        
        # Calculate periodicity
        if len(intervals) > 0:
            mean_interval = np.mean(intervals)
            std_interval = np.std(intervals)
            
            # Detect regular vs irregular firing
            regularity = 1.0 - min(1.0, std_interval / mean_interval) if mean_interval > 0 else 0
            
            return {
                "total_spikes": len(all_spikes),
                "mean_interval_ms": mean_interval,
                "std_interval_ms": std_interval,
                "regularity_score": regularity,
                "firing_regularity": "Regular" if regularity > 0.7 else "Irregular"
            }
        
        return {"insufficient_data": True}

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

analyzer = NeuralNetworkAnalyzer(brain)

print("Neural network analysis:")

# Run analysis
connectivity = analyzer.analyze_connectivity()
activity = analyzer.analyze_activity_patterns(50)
patterns = analyzer.detect_spike_patterns(20)

print("\nConnectivity analysis:")
print(f"Inter-region connections: {len(connectivity['inter_region_connections'])}")
print(f"Regions analyzed: {len(connectivity['within_region_stats'])}")

print("\nActivity pattern analysis:")
print(f"Mean firing ratio: {activity['mean_firing_ratio']:.3f}")
print(f"Firing ratio std: {activity['std_firing_ratio']:.3f}")

print("\nSpike pattern analysis:")
if not patterns.get("insufficient_data"):
    print(f"Regularity score: {patterns['regularity_score']:.3f}")
    print(f"Firing pattern: {patterns['firing_regularity']}")
else:
    print("Insufficient spike data for pattern analysis")
```

## Chapter 9: Advanced Error Handling and Validation

### 9.1 Custom Exception Types

```python
import pynlm

class NLMError(Exception):
    """Base exception for all NLM errors."""
    pass

class NeuronError(NLMError):
    """Exception raised for neuron-related errors."""
    pass

class SynapseError(NLMError):
    """Exception raised for synapse-related errors."""
    pass

class MemoryError(NLMError):
    """Exception raised for memory system errors."""
    pass

class NLMValidator:
    """Validation utilities for NLM operations."""
    
    @staticmethod
    def validate_neuron_operation(brain, neuron_id):
        """Validate neuron operation and raise appropriate exceptions."""
        regions = brain.getRegions()
        
        # Check if neuron exists
        neuron_found = False
        for region in regions:
            neurons = region.getNeurons()
            for neuron in neurons:
                if neuron.getId() == neuron_id:
                    neuron_found = True
                    break
            if neuron_found:
                break
        
        if not neuron_found:
            raise NeuronError(f"Neuron {neuron_id} not found")
        
        # Check if neuron is active
        if neuron.getState().firingState == pynlm.FiringState.Refractory:
            raise NeuronError(f"Neuron {neuron_id} is in refractory state")
        
        return True
    
    @staticmethod
    def validate_synapse_operation(brain, synapse_id):
        """Validate synapse operation and raise appropriate exceptions."""
        regions = brain.getRegions()
        
        # Check if synapse exists
        synapse_found = False
        for region in regions:
            synapses = region.getSynapses()
            for synapse in synapses:
                if synapse.getId() == synapse_id:
                    synapse_found = True
                    break
            if synapse_found:
                break
        
        if not synapse_found:
            raise SynapseError(f"Synapse {synapse_id} not found")
        
        return True
    
    @staticmethod
    def validate_memory_operation(memory, capacity, current_size):
        """Validate memory operation and raise appropriate exceptions."""
        if current_size > capacity:
            raise MemoryError(f"Memory capacity exceeded: {current_size} > {capacity}")
        
        return True

# Example usage with error handling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

validator = NLMValidator()

try:
    # Try to access a non-existent neuron
    fake_neuron_id = pynlm.NeuronId(999999)
    validator.validate_neuron_operation(brain, fake_neuron_id)
    
except NeuronError as e:
    print(f"Caught neuron error: {e}")
except NLMError as e:
    print(f"Caught NLM error: {e}")

# Test memory operation
working_mem = brain.getWorkingMemory()
validator.validate_memory_operation(working_mem, 1000, 500)  # Should pass
validator.validate_memory_operation(working_mem, 100, 150)   # Should raise exception
```

### 9.2 Robust Simulation Wrapper

```python
import pynlm
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class RobustBrainSimulator:
    """Robust brain simulator with comprehensive error handling and recovery."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        self.step_count = 0
        self.error_count = 0
        self.recovery_attempts = 0
    
    def initialize(self):
        """Initialize the simulator with error handling."""
        try:
            logger.info("Initializing brain simulator...")
            
            # Create components
            self.brain = pynlm.createBrain(self.config)
            self.brain.initialize()
            
            self.world = pynlm.createSimpleWorld()
            self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            self.world.reset()
            
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable subsystems with error checking
            if not self.agent.enableRewardModulation(True):
                logger.warning("Reward modulation initialization failed")
            
            if not self.agent.enableCuriosity(True):
                logger.warning("Curiosity initialization failed")
            
            logger.info("Brain simulator initialized successfully")
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize brain simulator: {e}")
            self.cleanup()
            return False
    
    def step(self):
        """Execute one simulation step with robust error handling."""
        try:
            # Update world
            self.world.update(0.1)
            
            # Sense and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain processing
            self.brain.step(self.step_count)
            
            # Agent decision making
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learning and development
            reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            self.step_count += 1
            return True
            
        except Exception as e:
            self.error_count += 1
            logger.warning(f"Step {self.step_count} failed: {e}")
            
            # Attempt recovery
            if self.recover_from_error():
                logger.info(f"Recovered from error on step {self.step_count}")
                return True
            else:
                logger.error(f"Failed to recover from error on step {self.step_count}")
                return False
    
    def recover_from_error(self):
        """Attempt to recover from simulation errors."""
        self.recovery_attempts += 1
        
        try:
            if self.recovery_attempts <= 3:  # Limit recovery attempts
                logger.info("Attempting error recovery...")
                
                # Reset world state
                self.world.reset()
                
                # Reset brain state
                self.brain.reset()
                
                # Re-initialize agent
                self.agent = pynlm.createAgentBrain(self.brain)
                self.agent.initialize(self.world)
                
                # Retry current step
                return True
            
            return False
            
        except Exception as e:
            logger.error(f"Recovery attempt {self.recovery_attempts} failed: {e}")
            return False
    
    def run(self, steps):
        """Run simulation with robust error handling."""
        if not self.initialize():
            logger.error("Failed to initialize simulator")
            return False
        
        successful_steps = 0
        failed_steps = 0
        
        for step in range(steps):
            if self.step():
                successful_steps += 1
            else:
                failed_steps += 1
        
        # Final statistics
        logger.info(f"Simulation completed:")
        logger.info(f"  Steps attempted: {steps}")
        logger.info(f"  Successful steps: {successful_steps}")
        logger.info(f"  Failed steps: {failed_steps}")
        logger.info(f"  Recovery attempts: {self.recovery_attempts}")
        
        return successful_steps / steps > 0.5  # Success if >50% success rate
    
    def get_statistics(self):
        """Get comprehensive simulation statistics."""
        if not self.brain:
            return {}
        
        return {
            "simulation_stats": {
                "total_steps": self.step_count,
                "error_count": self.error_count,
                "success_rate": (self.step_count - self.error_count) / max(1, self.step_count)
            },
            "brain_stats": {
                "neurons": self.brain.getTotalNeuronCount(),
                "synapses": self.brain.getTotalSynapseCount(),
                "spikes": self.brain.getTotalSpikeCount(),
                "active_neurons": self.brain.getActiveNeuronCount(),
                "firing_neurons": self.brain.getFiringNeuronCount(),
                "average_firing_rate": self.brain.getAverageFiringRate(),
                "development_stage": self.brain.getDevelopmentalStage().name
            },
            "neuromodulation_stats": {
                "curiosity_level": self.agent.getCuriosityLevel(),
                "novelty_level": self.agent.getNoveltyLevel(),
                "prediction_error": self.agent.getPredictionError()
            }
        }
    
    def cleanup(self):
        """Clean up resources."""
        try:
            logger.info("Cleaning up simulator resources...")
            
            # Save final state
            if self.brain:
                self.brain.save("simulator_final_state.bin")
            
            # Cleanup
            self.brain = None
            self.world = None
            self.agent = None
            
            logger.info("Simulator resources cleaned up")
            
        except Exception as e:
            logger.error(f"Error during cleanup: {e}")

# Example usage with robust simulator
print("Running robust brain simulation...")

simulator = RobustBrainSimulator()
if simulator.run(500):
    stats = simulator.get_statistics()
    print("\nSimulation completed successfully!")
    print(f"Success rate: {stats['simulation_stats']['success_rate']:.2%}")
    
    # Print key statistics
    brain_stats = stats['brain_stats']
    print(f"Final brain state:")
    print(f"  Neurons: {brain_stats['neurons']}")
    print(f"  Synapses: {brain_stats['synapses']}")
    print(f"  Total spikes: {brain_stats['spikes']}")
    print(f"  Development stage: {brain_stats['development_stage']}")
else:
    print("Simulation failed to meet minimum success criteria")

# Cleanup
simulator.cleanup()
```

## Chapter 10: Performance Optimization Tips

### 10.1 Memory Management Strategies

```python
import pynlm
import gc

def optimize_memory_usage(brain, world, agent):
    """Optimize memory usage for large simulations."""
    
    # Force garbage collection before starting
    gc.collect()
    
    # Create checkpoint system for memory management
    checkpoint_interval = 1000
    
    for step in range(checkpoint_interval):
        # Update world
        world.update(0.1)
        
        # Sense and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Periodic checkpointing for memory management
        if step % checkpoint_interval == 0 and step > 0:
            # Save checkpoint to free memory
            checkpoint_file = f"checkpoint_{step//checkpoint_interval}.bin"
            brain.save(checkpoint_file)
            
            # Force garbage collection
            gc.collect()
            
            print(f"Checkpoint saved: {checkpoint_file}")
    
    return brain, world, agent

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

brain, world, agent = optimize_memory_usage(brain, world, agent)
```

### 10.2 Efficient Data Processing

```python
import pynlm
import numpy as np

def batch_process_neurons(brain, processing_function, neuron_ids=None):
    """Efficiently process multiple neurons in batch."""
    
    if neuron_ids is None:
        # Get all neurons from all regions
        neuron_ids = []
        for region in brain.getRegions():
            neuron_ids.extend([neuron.getId() for neuron in region.getNeurons()])
    
    # Process neurons in batches for efficiency
    batch_size = 100
    results = []
    
    for i in range(0, len(neuron_ids), batch_size):
        batch_ids = neuron_ids[i:i + batch_size]
        batch_results = []
        
        for neuron_id in batch_ids:
            # Find and process neuron
            neuron = brain.getNeuron(neuron_id)
            result = processing_function(neuron)
            batch_results.append(result)
        
        results.extend(batch_results)
    
    return results

def analyze_neural_population(brain):
    """Analyze neural population characteristics efficiently."""
    
    # Get all neurons
    all_neurons = []
    for region in brain.getRegions():
        all_neurons.extend(region.getNeurons())
    
    if not all_neurons:
        return None
    
    # Batch analysis
    potentials = []
    firing_states = []
    neuron_types = []
    
    for neuron in all_neurons:
        potentials.append(neuron.getMembranePotential())
        firing_states.append(neuron.getState().firingState)
        neuron_types.append(neuron.getType())
    
    # Statistical analysis
    analysis = {
        "total_neurons": len(all_neurons),
        "average_potential": np.mean(potentials),
        "std_potential": np.std(potentials),
        "min_potential": np.min(potentials),
        "max_potential": np.max(potentials),
        "potential_range": np.max(potentials) - np.min(potentials),
        
        # Firing state distribution
        firing_state_counts = {}
        for state in firing_states:
            firing_state_counts[state.name] = firing_state_counts.get(state.name, 0) + 1
        
        "firing_state_distribution": firing_state_counts,
        
        # Neuron type distribution
        neuron_type_counts = {}
        for neuron_type in neuron_types:
            neuron_type_counts[neuron_type.name] = neuron_type_counts.get(neuron_type.name, 0) + 1
        
        "neuron_type_distribution": neuron_type_counts,
        
        # Activity statistics
        firing_rates = [neuron.getState().firingRate for neuron in all_neurons]
        "firing_rate_stats": {
            "mean": np.mean(firing_rates),
            "std": np.std(firing_rates),
            "min": np.min(firing_rates),
            "max": np.max(firing_rates)
        }
    }
    
    return analysis

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Analyzing neural population...")
analysis = analyze_neural_population(brain)

if analysis:
    print(f"Total neurons: {analysis['total_neurons']}")
    print(f"Average potential: {analysis['average_potential']:.2f} mV")
    print(f"Firing state distribution: {analysis['firing_state_distribution']}")
    print(f"Neuron type distribution: {analysis['neuron_type_distribution']}")
```

## Chapter 11: Integration with External Systems

### 11.1 Sensor Interface

```python
import pynlm
import numpy as np
from typing import Optional

class SensorInterface:
    """Interface for external sensor data integration."""
    
    def __init__(self, world):
        self.world = world
        self.sensor_data = {}
    
    def add_vision_sensor(self, width, height, channels=3):
        """Add vision sensor data."""
        # Generate synthetic vision data (in real applications, this would come from cameras)
        vision_data = np.random.normal(0.5, 0.1, (height, width, channels)).astype(np.float32)
        
        # Create vision sensory input
        vision = pynlm.Vision(width, height, channels)
        vision.setData(vision_data.flatten().tolist())
        
        self.sensor_data['vision'] = vision
        return vision
    
    def add_audio_sensor(self, sample_rate=44100, num_samples=1024):
        """Add audio sensor data."""
        # Generate synthetic audio data
        audio_data = np.random.normal(0, 0.01, num_samples).astype(np.float32)
        
        # Create audio sensory input
        audio = pynlm.Audio(sample_rate, num_samples)
        audio.setData(audio_data.tolist())
        
        self.sensor_data['audio'] = audio
        return audio
    
    def add_internal_sensor(self, signal_values):
        """Add internal sensor (proprioception, etc.) data."""
        internal = pynlm.InternalSignals()
        
        for i, value in enumerate(signal_values):
            internal.addSignal(value)
        
        self.sensor_data['internal'] = internal
        return internal
    
    def update_world(self):
        """Update world with all sensor data."""
        # Apply sensor data to world
        for sensor_name, sensor_data in self.sensor_data.items():
            if sensor_name == 'vision':
                # Vision affects object detection and navigation
                pass
            elif sensor_name == 'audio':
                # Audio affects threat detection and communication
                pass
            elif sensor_name == 'internal':
                # Internal signals affect motor control and energy
                pass
        
        return True

# Usage example
world = pynlm.createSimpleWorld()

sensor_interface = SensorInterface(world)
sensor_interface.add_vision_sensor(8, 8)  # 8x8 vision
 sensor_interface.add_audio_sensor()
sensor_interface.add_internal_sensor([0.5, 0.3, 0.7])

sensor_interface.update_world()
```

### 11.2 Motor Output Interface

```python
import pynlm
from typing import Dict, Any

class MotorOutputInterface:
    """Interface for external system motor output integration."""
    
    def __init__(self, world):
        self.world = world
        self.motor_data = {}
    
    def add_motor_command(self, action_type, parameters=None):
        """Add motor command from external system."""
        if parameters is None:
            parameters = {}
        
        # Create action based on type
        action = pynlm.Action(pynlm.ActionType(action_type))
        
        if parameters:
            action.setParameters(parameters)
        
        self.motor_data[action_type] = action
        return action
    
    def process_external_commands(self, external_commands: Dict[str, Any]):
        """Process external motor commands."""
        
        for command_name, command_data in external_commands.items():
            if command_name == 'move_forward':
                action = self.add_motor_command('MoveForward', {'distance': command_data['distance']})
            
            elif command_name == 'turn':
                action = self.add_motor_command('TurnLeft' if command_data['direction'] > 0 else 'TurnRight', 
                                              {'angle': abs(command_data['direction'])})
            
            elif command_name == 'interact':
                action = self.add_motor_command('Interact', {'target': command_data['target']})
            
            elif command_name == 'custom':
                action = self.add_motor_command('Custom', {'parameters': command_data})
            
            # Apply motor command to world
            if 'current_time' not in command_data:
                command_data['current_time'] = self.world.getSimulationTime()
            
            self.world.applyMotorCommand(action, command_data['current_time'])
        
        return True
    
    def get_motor_statistics(self):
        """Get motor system statistics."""
        if not hasattr(self, 'motor_data') or not self.motor_data:
            return {}
        
        stats = {
            "total_commands": len(self.motor_data),
            "command_types": {},
            "command_summary": {}
        }
        
        for action_type, action in self.motor_data.items():
            stats["command_types"][action_type] = stats["command_types"].get(action_type, 0) + 1
            
            # Get action parameters
            params = action.getParameters()
            if params:
                stats["command_summary"][action_type] = params
        
        return stats

# Usage example
world = pynlm.createSimpleWorld()

motor_interface = MotorOutputInterface(world)

# Process external commands
external_commands = {
    'move_forward': {'distance': 10.0},
    'turn': {'direction': 45.0},  # 45 degrees
    'interact': {'target': 'object_123'},
    'custom': {'parameters': {'speed': 2.0, 'duration': 5.0}}
}

motor_interface.process_external_commands(external_commands)
stats = motor_interface.get_motor_statistics()
print(f"Motor statistics: {stats}")
```

### 11.3 Database Integration

```python
import pynlm
import json
from datetime import datetime

class DatabaseInterface:
    """Interface for database integration (SQLite, MongoDB, etc.)."""
    
    def __init__(self, db_path=None):
        self.db_path = db_path
        self.simulations = {}
        self.experiments = []
    
    def save_simulation(self, brain, world, agent, simulation_id=None):
        """Save simulation state to database."""
        
        if simulation_id is None:
            simulation_id = f"sim_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        
        # Collect simulation data
        simulation_data = {
            "id": simulation_id,
            "timestamp": datetime.now().isoformat(),
            "brain_state": self._collect_brain_data(brain),
            "world_state": self._collect_world_data(world),
            "agent_state": self._collect_agent_data(agent),
            "statistics": self._calculate_simulation_statistics(brain, world, agent)
        }
        
        self.simulations[simulation_id] = simulation_data
        
        # Save to file (in production, this would connect to actual database)
        filename = f"simulation_{simulation_id}.json"
        with open(filename, 'w') as f:
            json.dump(simulation_data, f, indent=2)
        
        return simulation_id
    
    def load_simulation(self, simulation_id):
        """Load simulation state from database."""
        
        if simulation_id not in self.simulations:
            # Try to load from file
            filename = f"simulation_{simulation_id}.json"
            try:
                with open(filename, 'r') as f:
                    simulation_data = json.load(f)
                self.simulations[simulation_id] = simulation_data
            except FileNotFoundError:
                raise ValueError(f"Simulation {simulation_id} not found")
        
        # Reconstruct objects from saved data
        return self._reconstruct_simulation_objects(self.simulations[simulation_id])
    
    def save_experiment(self, experiment_name, parameters, results):
        """Save experiment data to database."""
        
        experiment = {
            "name": experiment_name,
            "timestamp": datetime.now().isoformat(),
            "parameters": parameters,
            "results": results
        }
        
        self.experiments.append(experiment)
        
        # Save to file
        filename = f"experiment_{experiment_name}_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(filename, 'w') as f:
            json.dump(experiment, f, indent=2)
        
        return True
    
    def query_experiments(self, filter_criteria=None):
        """Query saved experiments with optional filtering."""
        
        if filter_criteria is None:
            return self.experiments
        
        filtered_experiments = []
        for experiment in self.experiments:
            match = True
            for key, value in filter_criteria.items():
                if key not in experiment or experiment[key] != value:
                    match = False
                    break
            
            if match:
                filtered_experiments.append(experiment)
        
        return filtered_experiments

# Usage example
db_interface = DatabaseInterface()

# Save simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

simulation_id = db_interface.save_simulation(brain, world, agent)
print(f"Saved simulation with ID: {simulation_id}")

# Save experiment
experiment_parameters = {
    "learning_rate": 0.01,
    "curiosity_level": 0.5,
    "simulation_steps": 1000
}

experiment_results = {
    "final_firing_rate": brain.getAverageFiringRate(),
    "total_spikes": brain.getTotalSpikeCount(),
    "curiosity_level": agent.getCuriosityLevel()
}

db_interface.save_experiment("Hebbian_Learning_Test", experiment_parameters, experiment_results)

# Query experiments
recent_experiments = db_interface.query_experiments({"timestamp": datetime.now().isoformat()})
print(f"Found {len(recent_experiments)} recent experiments")
```

## Chapter 12: Troubleshooting and Common Issues

### 12.1 Common Error Scenarios

```python
import pynlm

def handle_common_errors(brain, world, agent, step):
    """Handle common error scenarios in simulation."""
    
    errors_handled = 0
    
    # Check for neuron exhaustion
    neuron_count = brain.getTotalNeuronCount()
    firing_neurons = brain.getFiringNeuronCount()
    
    if neuron_count > 0 and firing_neurons == 0:
        print(f"Step {step}: Warning - no neurons firing")
        
        # Try to inject some external input
        try:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
            errors_handled += 1
        except Exception as e:
            print(f"  Failed to inject sensory input: {e}")
    
    # Check for world boundary violations
    body = world.getAgentBody()
    if body.x < -100 or body.x > 100 or body.y < -100 or body.y > 100:
        print(f"Step {step}: Warning - agent near world boundary")
        errors_handled += 1
    
    # Check for memory issues
    try:
        working_mem = brain.getWorkingMemory()
        if working_mem.getCurrentSize() > working_mem.getCapacity() * 0.9:
            print(f"Step {step}: Warning - working memory near capacity")
            # Clear some memory
            working_mem.clear()
            errors_handled += 1
    except Exception as e:
        print(f"  Failed to check memory: {e}")
    
    # Check for neuromodulation issues
    try:
        curiosity = agent.getCuriosityLevel()
        if curiosity > 1.0:
            print(f"Step {step}: Warning - curiosity level too high")
            agent.enableCuriosity(True)  # Reset if disabled
            errors_handled += 1
    except Exception as e:
        print(f"  Failed to check neuromodulation: {e}")
    
    return errors_handled

# Usage with error handling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

print("Running simulation with error handling...")

for step in range(100):
    # Run simulation step
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Handle common errors
    errors = handle_common_errors(brain, world, agent, step)
    
    if errors > 0:
        print(f"  Handled {errors} error(s)")
```

### 12.2 Performance Debugging

```python
import pynlm
import time

def performance_profile(brain, world, agent, steps=100):
    """Profile performance of simulation components."""
    
    print("Performance profiling:")
    
    # Profile different components
    components = {
        "brain_step": 0.0,
        "world_update": 0.0,
        "agent_processing": 0.0,
        "agent_decision": 0.0
    }
    
    # Warm-up run
    for i in range(10):
        brain.step(i)
    
    # Main profiling run
    for step in range(steps):
        # Profile world update
        start = time.time()
        world.update(0.1)
        components["world_update"] += time.time() - start
        
        # Profile agent processing
        start = time.time()
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        components["agent_processing"] += time.time() - start
        
        # Profile brain step
        start = time.time()
        brain.step(step)
        components["brain_step"] += time.time() - start
        
        # Profile agent decision
        start = time.time()
        action = agent.decodeMotorCommand()
        components["agent_decision"] += time.time() - start
    
    # Calculate statistics
    print("\nComponent performance:")
    for component, total_time in components.items():
        avg_time = total_time / steps
        steps_per_second = steps / total_time if total_time > 0 else 0
        print(f"  {component}: {avg_time*1000:.2f}ms/step, {steps_per_second:.0f} steps/sec")
    
    # Memory usage check
    print(f"\nMemory usage:")
    print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  Brain synapses: {brain.getTotalSynapseCount()}")
    print(f"  Agent curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return components

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

performance_profile(brain, world, agent, steps=50)
```

### 12.3 State Validation

```python
import pynlm

def validate_simulation_state(brain, world, agent, step):
    """Validate simulation state at each step."""
    
    issues = []
    
    # Validate brain state
    try:
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        
        if neuron_count <= 0:
            issues.append("Brain has no neurons")
        
        if synapse_count < 0:
            issues.append("Brain has negative synapse count")
        
        # Check for potential numerical instability
        if neuron_count > 0:
            firing_rate = brain.getAverageFiringRate()
            if firing_rate > 1000:  # Unrealistic firing rate
                issues.append(f"Unrealistic firing rate: {firing_rate} Hz")
        
    except Exception as e:
        issues.append(f"Brain validation error: {e}")
    
    # Validate world state
    try:
        body = world.getAgentBody()
        
        # Check agent position
        if abs(body.x) > 1e6 or abs(body.y) > 1e6:  # Numerical overflow
            issues.append("Agent position numerical overflow detected")
        
        # Check agent energy
        if body.energy < -1e6 or body.energy > 1e6:
            issues.append("Agent energy numerical overflow detected")
        
    except Exception as e:
        issues.append(f"World validation error: {e}")
    
    # Validate agent state
    try:
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        if curiosity < -10 or curiosity > 10:
            issues.append(f"Unreasonable curiosity level: {curiosity}")
        
        if novelty < -10 or novelty > 10:
            issues.append(f"Unreasonable novelty level: {novelty}")
        
    except Exception as e:
        issues.append(f"Agent validation error: {e}")
    
    if issues:
        print(f"\nStep {step} validation issues:")
        for issue in issues:
            print(f"  - {issue}")
        
        return False
    else:
        return True

# Usage with validation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

print("Running validated simulation...")

for step in range(50):
    # Run simulation step
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Validate state
    if not validate_simulation_state(brain, world, agent, step):
        print("Simulation state validation failed")
        break
```

## Chapter 13: Advanced Configuration

### 13.1 Custom Configuration Templates

```python
import pynlm

class ConfigurationTemplate:
    """Predefined configuration templates for different simulation types."""
    
    @staticmethod
    def create_conservative_template():
        """Template for conservative, stable simulations."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.learning_rate", 0.005)
        config.set("neuromod.curiosity.novelty_weight", 0.1)
        config.set("neuromod.curiosity.prediction_error_weight", 0.05)
        return config
    
    @staticmethod
    def create_exploratory_template():
        """Template for exploratory, adaptive simulations."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.learning_rate", 0.01)
        config.set("plasticity.hebbian.learning_rate", 0.02)
        config.set("neuromod.curiosity.novelty_weight", 0.8)
        config.set("neuromod.curiosity.prediction_error_weight", 0.6)
        return config
    
    @staticmethod
    def create_reactive_template():
        """Template for reactive, fast simulations."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.learning_rate", 0.005)
        config.set("plasticity.hebbian.learning_rate", 0.01)
        config.set("neuromod.curiosity.novelty_weight", 0.5)
        config.set("neuromod.curiosity.prediction_error_weight", 0.5)
        return config

# Usage
print("Available configuration templates:")

templates = {
    "conservative": ConfigurationTemplate.create_conservative_template(),
    "exploratory": ConfigurationTemplate.create_exploratory_template(),
    "reactive": ConfigurationTemplate.create_reactive_template()
}

for name, config in templates.items():
    print(f"\n{name.capitalize()} template:")
    
    # Create brain with template
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Print key configuration values
    if config.has("brain.neuron_count"):
        print(f"  Neurons: {config.get('brain.neuron_count')}")
    
    if config.has("plasticity.stdp.learning_rate"):
        print(f"  STDP learning rate: {config.get('plasticity.stdp.learning_rate')}")
    
    if config.has("neuromod.curiosity.novelty_weight"):
        print(f"  Curiosity novelty weight: {config.get('neuromod.curiosity.novelty_weight')}")
```

### 13.2 Performance Tuning

```python
import pynlm

def optimize_simulation_performance(brain, world, agent, target_fps=30):
    """Optimize simulation for target performance."""
    
    print(f"Optimizing simulation for {target_fps} FPS...")
    
    # Get current performance
    start_time = time.time()
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    current_fps = 100 / (time.time() - start_time)
    print(f"Current performance: {current_fps:.1f} FPS")
    
    # Optimization recommendations
    if current_fps < target_fps:
        print("Performance below target. Optimization recommendations:")
        
        # Check brain configuration
        neuron_count = brain.getTotalNeuronCount()
        if neuron_count > 5000:
            print(f"  - Reduce neuron count (currently {neuron_count})")
        
        # Check plasticity settings
        stdp = brain.getSTDP()
        if stdp.getTimeConstant() < 10:
            print(f"  - Increase STDP time constant (currently {stdp.getTimeConstant()}ms)")
        
        # Check world size
        width, height = world.getWidth(), world.getHeight()
        if width * height > 400:
            print(f"  - Reduce world size (currently {width}x{height})")
        
        # Check agent subsystems
        if agent.getCuriosityLevel() > 0.5:
            print(f"  - Reduce curiosity level (currently {agent.getCuriosityLevel():.2f})")
        
        return False
    else:
        print(f"Performance meets or exceeds target!")
        return True

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

optimized = optimize_simulation_performance(brain, world, agent, target_fps=30)

if not optimized:
    print("\nApplying performance optimizations...")
    
    # Apply optimizations
    config = brain.getConfig()
    
    # Reduce neuron count for better performance
    if config.has("brain.neuron_count"):
        current_neurons = config.get("brain.neuron_count")
        optimized_neurons = max(100, current_neurons // 2)
        config.set("brain.neuron_count", optimized_neurons)
        print(f"  Reduced neuron count: {current_neurons} -> {optimized_neurons}")
    
    # Increase STDP time constant
    if hasattr(brain, 'getSTDP'):
        stdp = brain.getSTDP()
        current_tau = stdp.getTimeConstant()
        optimized_tau = current_tau * 2
        stdp.setTimeConstant(optimized_tau)
        print(f"  Increased STDP time constant: {current_tau} -> {optimized_tau}ms")
    
    # Re-initialize with optimized settings
    brain.reset()
    print("Performance optimization complete")
```

## Chapter 14: Reference and API Quick Reference

### 14.1 Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Brain` | Central neural processor | `step()`, `receiveSensoryInput()`, `produceAction()`, `getTotalNeuronCount()` |
| `SimpleWorld` | 2D simulation environment | `update()`, `getSensoryPercept()`, `applyMotorCommand()`, `configure()` |
| `AgentBrain` | Agent interface to brain | `processSensoryInput()`, `decodeMotorCommand()`, `applyRewardModulation()`, `updateDevelopment()` |
| `Config` | Configuration management | `loadFromFile()`, `saveToFile()`, `getKeys()`, `set()` |

### 14.2 Data Classes

| Class | Purpose | Key Properties |
|-------|---------|---------------|
| `NeuronId` | Neuron identifier | `value`, `index` |
| `SynapseId` | Synapse identifier | `value`, `index` |
| `RegionId` | Brain region identifier | `value`, `index` |
| `Action` | Motor output | `type`, `parameters`, `getName()` |
| `WorldObject` | Environment object | `x`, `y`, `type`, `radius`, `value` |

### 14.3 Enum Types

| Enum | Values | Description |
|------|--------|-------------|
| `NeuronType` | `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal` | Neuron classification |
| `ActionType` | `MoveForward`, `MoveBackward`, `TurnLeft`, `TurnRight`, `Look`, `Eat`, `Rest`, etc. | Available actions |
| `DevelopmentalStage` | `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging` | Brain development stages |
| `WorldObjectType` | `Empty`, `Resource`, `Hazard`, `Wall`, `Marker` | World object types |

### 14.4 Factory Functions

| Function | Returns | Description |
|----------|---------|-------------|
| `createDefaultConfig()` | `Config` | Default configuration |
| `createBrain(config)` | `Brain` | Create neural brain |
| `createSimpleWorld()` | `SimpleWorld` | Create simulation world |
| `createAgentBrain(brain)` | `AgentBrain` | Create agent interface |

### 14.5 Common Usage Patterns

#### Pattern 1: Basic Simulation
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
```

#### Pattern 2: Agent in World
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Pattern 3: Learning and Development
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and learn
    reward = get_reward_from_world(world)
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

## Chapter 15: Migration Guide

### 15.1 From Basic to Advanced Usage

If you're migrating from `easy_usage.md`, here are the key changes:

#### Basic Usage → Advanced Usage

**Basic (from easy_usage.md):**
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

**Advanced (same functionality with additional control):**
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Add advanced control
stdp = brain.getSTDP()
stdp.configure(ltpWeight=0.01, ltdWeight=0.01, tau=20.0)

# Run with statistics
for i in range(100):
    brain.step(i)
    
    if i % 20 == 0:
        print(f"Step {i}: spikes={brain.getTotalSpikeCount()}, "
              f"firing={brain.getFiringNeuronCount()}")
```

### 15.2 Upgrading from Python Bindings

If you were using older Python bindings with limited APIs, here are the new capabilities:

**Old (limited APIs):**
```python
# You could only access basic statistics
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

**New (full access):**
```python
# Now you have access to all brain systems
brain = pynlm.createBrain(config)
brain.initialize()

# Direct neuron control
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    for neuron_id in region.getNeuronIds():
        neuron = brain.getNeuron(neuron_id)
        neuron.injectCurrent(5.0)  # Direct neuron control

# Configure learning systems
stdp = brain.getSTDP()
stdp.configure(ltpWeight=0.02, ltdWeight=0.02, tau=25.0)

# Access memory systems
working_mem = brain.getWorkingMemory()
episodic_mem = brain.getEpisodicMemory()

# Control neuromodulation
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()

for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        print(f"Step {step}: ")
        print(f"  Spike count: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  STDP weight: {stdp.getLTPWeight()}")
        print(f"  Curiosity: {curiosity.getLevel():.3f}")
```

## Bibliography

- NLM Project Documentation
- Phase 6 Integration Analysis
- Python Bindings Development Guide
- Advanced Neuroscience Simulation Patterns

---

*This document provides comprehensive guidance for advanced NLM usage, including direct neural manipulation, plasticity system configuration, memory operations, neuromodulation control, and performance optimization.*