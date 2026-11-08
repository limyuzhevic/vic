# Complete Code Examples Demonstrating Python Bindings Improvements

This document provides concrete code examples showing the improvements needed for the NLM Python bindings.

## Example 1: Current API Limitations vs Improved API

### Current API (Limited)

```python
import pynlm

# Can only create brain using factory functions
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Cannot create individual components
# neuron = pynlm.Neuron()  # ❌ AttributeError
try:
    neuron = pynlm.Neuron()  # This will fail
    print("Created neuron")
except AttributeError as e:
    print(f"❌ Cannot create neuron: {e}")

# Cannot access memory systems
# memory = brain.getWorkingMemory()  # ❌ AttributeError
try:
    memory = brain.getWorkingMemory()
    print("✓ Can access working memory")
except AttributeError as e:
    print(f"❌ Cannot access working memory: {e}")

# Poor error handling - unclear what went wrong
result = config.loadFromFile("missing_file.json")
if not result:
    print("❌ Config load failed - but why?")
```

### Improved API (Enhanced)

```python
import pynlm

# Still can create brain the same way (backward compatibility)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Now CAN create individual components!
neuron = pynlm.createNeuron(type=pynlm.NeuronType.Excitatory)
print(f"✓ Created neuron: {neuron}")

synapse = pynlm.createSynapse(neuron, neuron, weight=0.5)
print(f"✓ Created synapse: {synapse}")

region = pynlm.createRegion("cortex", neuron_count=100)
print(f"✓ Created region: {region}")

# CAN access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
print("✓ Can access all memory systems")

# Enhanced error handling with clear messages
config2 = pynlm.createDefaultConfig()
try:
    config2.loadFromFile("/path/to/missing_file.json")
    print("❌ Should have raised exception")
except RuntimeError as e:
    print(f"✓ Clear error message: {e}")

# Convenience methods for common operations
stats = brain.getBrainStats()
print(f"✓ Brain statistics: {stats}")

# Episode runner
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)

pynlm.runEpisode(brain, agent, world, steps=100)
print("✓ Episode completed using convenience method")
```

## Example 2: Advanced Usage Patterns

### Pattern 1: Running Complete Agent Episode

**Current Pattern (Manual Orchestration)**:

```python
import pynlm

def run_agent_episode(brain, agent, world, steps=1000, timestep=0.1):
    """Manual episode runner - tedious to write!"""
    agent.initialize(world)
    
    for step in range(steps):
        # 1. Update world
        world.update(timestep)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress occasionally
        if step % 100 == 0:
            print(f"Step {step}: Firing neurons = {brain.getFiringNeuronCount()}")

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

run_agent_episode(brain, agent, world, steps=500)
```

**Improved Pattern (Convenience Method)**:

```python
import pynlm

# Single line to run complete episode
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Run complete episode with one function call
pynlm.runEpisode(brain, agent, world, steps=500, timestep=0.1, verbose=True)
print("✓ Episode completed successfully")

# With progress reporting
pynlm.runEpisode(brain, agent, world, steps=500, verbose=True)
```

### Pattern 2: Brain Analysis and Debugging

**Current Pattern (Manual Stats Collection)**:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Manually collect stats one by one
total_neurons = brain.getTotalNeuronCount()
total_synapses = brain.getTotalSynapseCount()
active_neurons = brain.getActiveNeuronCount()
firing_neurons = brain.getFiringNeuronCount()
average_firing_rate = brain.getAverageFiringRate()
e_to_i_ratio = brain.getExcitationInhibitionRatio()
total_spikes = brain.getTotalSpikeCount()
development_stage = brain.getDevelopmentalStage()

print(f"Brain Stats:")
print(f"  Total neurons: {total_neurons}")
print(f"  Total synapses: {total_synapses}")
print(f"  Active neurons: {active_neurons}")
print(f"  Firing neurons: {firing_neurons}")
print(f"  Avg firing rate: {average_firing_rate:.3f} Hz")
print(f"  E/I ratio: {e_to_i_ratio:.3f}")
print(f"  Total spikes: {total_spikes}")
print(f"  Development stage: {development_stage}")
```

**Improved Pattern (Single Convenience Method)**:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Single call to get all statistics
stats = brain.getBrainStats()

print(f"Brain Stats (as dictionary):")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Can also access specific stats easily
print(f"\nKey metrics:")
print(f"  Firing rate: {stats['average_firing_rate']:.3f} Hz")
print(f"  Development stage: {stats['development_stage']}")
print(f"  E/I balance: {stats['excitatory_inhibition_ratio']:.3f}")

# Stats are also accessible as named attributes for IDE support
print(f"\nUsing named access:")
print(f"  Total neurons: {stats.total_neurons}")
print(f"  Firing neurons: {stats.firing_neurons}")
print(f"  Development stage: {stats.development_stage}")
```

### Pattern 3: Memory System Access

**Current Pattern (Limited Access)**:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Cannot access memory systems
# working_memory = brain.getWorkingMemory()  # ❌ AttributeError
# episodic_memory = brain.getEpisodicMemory()  # ❌ AttributeError
# associative_memory = brain.getAssociativeMemory()  # ❌ AttributeError

print("❌ Memory systems not accessible")
```

**Improved Pattern (Full Memory System Access)**:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Now can access all memory systems
memory_systems = brain.getMemorySystems()

working_memory = memory_systems["working"]
episodic_memory = memory_systems["episodic"]
associative_memory = memory_systems["associative"]

print("✓ All memory systems accessible:")
print(f"  Working memory: {working_memory}")
print(f"  Episodic memory: {episodic_memory}")
print(f"  Associative memory: {associative_memory}")

# Can now use memory systems
# working_memory.storeToNeuron(neuron_id, value)
# episodic_memory.store(experience)
# associative_memory.associate(pattern1, pattern2)

# And can access brain statistics including memory metrics
stats = brain.getBrainStats()
print(f"✓ Memory-related stats: {stats}")
```

### Pattern 4: Plasticity System Access

**Current Pattern (Limited)**:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Cannot access plasticity systems
# stdp = brain.getSTDP()  # ❌ AttributeError
# hebbian = brain.getHebbian()  # ❌ AttributeError
# structural = brain.getStructuralPlasticity()  # ❌ AttributeError

print("❌ Plasticity systems not accessible")
```

**Improved Pattern (Full Plasticity Access)**:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Can access all plasticity systems
plasticity_systems = brain.getPlasticitySystems()

stdp = plasticity_systems["stdp"]
hebbian = plasticity_systems["hebbian"]
structural = plasticity_systems["structural"]

print("✓ All plasticity systems accessible:")
print(f"  STDP: {stdp}")
print(f"  Hebbian: {hebbian}")
print(f"  Structural: {structural}")

# Can configure plasticity
if stdp:
    stdp.setLearningRate(0.001)
    stdp.setTimeConstant(20.0)

# And access brain statistics
stats = brain.getBrainStats()
print(f"✓ Stats include plasticity info: {stats}")
```

### Pattern 5: Phase 6 Advanced Features

**Current Pattern (Limited Phase 6 Access)**:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Some Phase 6 features work
thought_trace = agent.getThoughtTrace()
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()

# But Phase 6 features not accessible
# self_model = agent.getSelfModel()  # ❌ Not exposed
# social_learning = agent.getSocialLearning()  # ❌ Not exposed
# text_encoding_stats = agent.encodeTextWithStats("hello")  # ❌ Not exposed

print("⚠️  Limited Phase 6 access")
```

**Improved Pattern (Full Phase 6 Access)**:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Can still access basic Phase 6 features
thought_trace = agent.getThoughtTrace()
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()

# Can now access all Phase 6 features
phase6_features = agent.getPhase6Features()

self_model = phase6_features["self_model"]
social_learning = phase6_features["social_learning"]
text_encoding_stats = phase6_features["text_encoding"]
detailed_thought_trace = phase6_features["detailed_trace"]

print("✓ All Phase 6 features accessible:")
print(f"  Self model: {self_model}")
print(f"  Social learning: {social_learning}")
print(f"  Text encoding stats: {text_encoding_stats}")
print(f"  Detailed thought trace: {detailed_thought_trace}")

# Enhanced text encoding
text_stats = pynlm.encodeTextWithStats("hello")
print(f"✓ Text encoding with stats: {text_stats}")
```

## Example 3: Error Handling Improvements

### Pattern: Clear Error Messages

**Current Error Handling**:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Poor error handling - unclear what went wrong
result = config.loadFromFile("/path/to/missing_file.json")
if not result:
    print("❌ Configuration load failed - but why?")  # No context
    # User has no idea what went wrong or how to fix it
```

**Improved Error Handling**:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Clear error messages with helpful information
try:
    config.loadFromFile("/path/to/missing_file.json")
    print("❌ Should have raised exception")
except RuntimeError as e:
    print(f"✓ Clear error message: {e}")
    # Output: "Configuration file not found: /path/to/missing_file.json. "
    #         "Please check the file path and ensure the file exists."

# Clear error handling for other operations
try:
    action_result = world.applyMotorCommand(motor_cmd, current_time)
    if not action_result.success:
        print(f"⚠️ Motor command failed: {action_result.message}")
        # User gets clear information about what went wrong
except RuntimeError as e:
    print(f"✓ Clear error: {e}")
```

## Example 4: Factory Methods Usage

### Creating Neural Components Directly

```python
import pynlm

# Create neurons with specific types
excitory_neuron = pynlm.createNeuron(type=pynlm.NeuronType.Excitatory)
inhibitory_neuron = pynlm.createNeuron(type=pynlm.NeuronType.Inhibitory)
sensory_neuron = pynlm.createNeuron(type=pynlm.NeuronType.Sensory)
motor_neuron = pynlm.createNeuron(type=pynlm.NeuronType.Motor)

print("✓ Created neurons of different types")

# Create synapses between neurons
synapse1 = pynlm.createSynapse(excitory_neuron, inhibitory_neuron, weight=0.5)
synapse2 = pynlm.createSynapse(sensory_neuron, motor_neuron, weight=0.8)

print("✓ Created synapses")

# Create regions with neurons
region1 = pynlm.createRegion("cortex", neuron_count=50)
region2 = pynlm.createRegion("hippocampus", neuron_count=20)

print("✓ Created regions")

# Create populations
population1 = pynlm.createPopulation(type=pynlm.NeuronType.Excitatory, size=10)
population2 = pynlm.createPopulation(type=pynlm.NeuronType.Inhibitory, size=5)

print("✓ Created populations")

# Can now use these components
print("✓ All neural components created successfully")
```

## Example 5: Comprehensive Agent Example

### Complete Agent Episode with Enhanced Features

```python
import pynlm
import time

def run_advanced_agent_episode(steps=1000):
    """Advanced agent episode with all improvements"""
    
    # 1. Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setRandomSeed(42)  # For reproducible experiments
    
    # 3. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Run simulation loop with progress reporting
    print("Starting advanced agent episode...")
    print(f"{'Step':<6} {'Firing':<8} {'Curiosity':<10} {'Novelty':<10} {'Dev Stage':<12} {'Time':<8}")
    print("-" * 70)
    
    start_time = time.time()
    
    for step in range(steps):
        # World update
        world.update(0.1)
        
        # Agent perception
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if world.getSensoryPercept().getInternal():
            reward = world.getSensoryPercept().getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"{step:<6} {brain.getFiringNeuronCount():<8} "
                  f"{agent.getCuriosityLevel():<10.3f} "
                  f"{agent.getNoveltyLevel():<10.3f} "
                  f"{brain.getDevelopmentalStage():<12} "
                  f"{world.getSimulationTime():<8.1f}")
    
    end_time = time.time()
    
    print("-" * 70)
    print(f"Episode completed in {end_time - start_time:.1f} seconds")
    
    # 6. Access enhanced statistics
    stats = brain.getBrainStats()
    memory_systems = brain.getMemorySystems()
    plasticity_systems = brain.getPlasticitySystems()
    phase6_features = agent.getPhase6Features()
    
    print(f"\nFinal Statistics:")
    print(f"  Total neurons: {stats.total_neurons}")
    print(f"  Total synapses: {stats.total_synapses}")
    print(f"  Firing neurons: {stats.firing_neurons}")
    print(f"  Average firing rate: {stats.average_firing_rate:.3f} Hz")
    print(f"  Development stage: {stats.development_stage}")
    
    # 7. Get enhanced thought trace
    detailed_trace = phase6_features["detailed_trace"]
    print(f"\nFinal Thought Trace: {detailed_trace}")
    
    return brain, agent, world, stats

# Run the advanced episode
if __name__ == "__main__":
    print("=== Advanced Agent Episode Example ===")
    brain, agent, world, stats = run_advanced_agent_episode(steps=500)
    print("✓ Advanced episode completed successfully!")
```

## Example 6: Memory System Usage

### Advanced Memory System Operations

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get memory systems
memory_systems = brain.getMemorySystems()

working_memory = memory_systems["working"]
episodic_memory = memory_systems["episodic"]
associative_memory = memory_systems["associative"]

print("=== Memory System Usage Examples ===")

# 1. Working Memory Example
print("\n1. Working Memory:")
print("   Storing patterns...")
for i in range(5):
    pattern = [0.1 * (i + 1), 0.5 - 0.1 * i, 0.9 - 0.05 * i]
    working_memory.storePattern(pattern)

print(f"   Stored {working_memory.getPatternCount()} patterns")
print(f"   Active traces: {working_memory.getActiveTraces()}")

# 2. Episodic Memory Example
print("\n2. Episodic Memory:")
print("   Recording experiences...")
for i in range(3):
    experience = {
        "step": i * 100,
        "reward": 0.8 - 0.1 * i,
        "action": "move_forward",
        "perception": f"vision_{i}",
        "outcome": "success" if i % 2 == 0 else "partial"
    }
    episodic_memory.storeExperience(experience)

print(f"   Recorded {episodic_memory.getExperienceCount()} experiences")
print(f"   Replay capacity: {episodic_memory.getReplayCapacity()}%")

# 3. Associative Memory Example
print("\n3. Associative Memory:")
print("   Learning associations...")
pattern_a = [0.1, 0.8, 0.3]
pattern_b = [0.9, 0.2, 0.7]

associative_memory.associate(pattern_a, pattern_b)

# Recall pattern A
recalled_b = associative_memory.recall(pattern_a)
print(f"   Pattern A: {pattern_a}")
print(f"   Recalled B: {recalled_b}")

# Test similarity
similarity = associative_memory.computeSimilarity(pattern_a, pattern_b)
print(f"   Similarity score: {similarity:.3f}")

# 4. Memory System Statistics
print("\n4. Memory System Statistics:")
stats = brain.getBrainStats()
for key, value in stats.items():
    if "memory" in key.lower():
        print(f"   {key}: {value}")
```

## Example 7: Plasticity System Usage

### Advanced Plasticity Configuration and Management

```python
import pynlm

# Create brain with plasticity
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get plasticity systems
plasticity_systems = brain.getPlasticitySystems()

stdp = plasticity_systems["stdp"]
hebbian = plasticity_systems["hebbian"]
structural = plasticity_systems["structural"]

print("=== Plasticity System Usage Examples ===")

# 1. STDP Configuration
print("\n1. STDP Configuration:")
if stdp:
    print("   Current STDP parameters:")
    print(f"     Learning rate: {stdp.getLearningRate()}")
    print(f"     Time constant: {stdp.getTimeConstant()}")
    print(f"     LTP weight: {stdp.getLTPWeight()}")
    print(f"     LTD weight: {stdp.getLTDWeight()}")
    
    # Modify parameters
    stdp.setLearningRate(0.01)
    stdp.setTimeConstant(10.0)
    print("   Updated learning rate and time constant")

# 2. Hebbian Plasticity
print("\n2. Hebbian Plasticity:")
if hebbian:
    print("   Current Hebbian parameters:")
    print(f"     Learning rate: {hebbian.getLearningRate()}")
    
    # Modify parameters
    hebbian.setLearningRate(0.005)
    print("   Updated learning rate")

# 3. Structural Plasticity
print("\n3. Structural Plasticity:")
if structural:
    print("   Structural plasticity enabled")
    # Can configure structural parameters
    # structural.setMaxConnections(100)
    # structural.setConnectionProbability(0.05)

# 4. Plasticity Statistics
print("\n4. Plasticity Statistics:")
stats = brain.getBrainStats()
for key, value in stats.items():
    if "plasticity" in key.lower() or "plasticity" in str(value).lower():
        print(f"   {key}: {value}")

# 5. Plasticity System Status
print("\n5. Plasticity System Status:")
print(f"   STDP enabled: {bool(stdp) if stdp else 'N/A'}")
print(f"   Hebbian enabled: {bool(hebbian) if hebbian else 'N/A'}")
print(f"   Structural enabled: {bool(structural) if structural else 'N/A'}")
```

## Example 8: Phase 6 Advanced Features

### Complete Phase 6 Feature Access

```python
import pynlm

# Create advanced agent with Phase 6 features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

print("=== Phase 6 Advanced Features Usage ===")

# 1. Basic Phase 6 Features (Already Available)
print("\n1. Basic Phase 6 Features:")
print(f"   Thought trace: {agent.getThoughtTrace()}")
print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"   Prediction error: {agent.getPredictionError():.3f}")
print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")

# 2. Enhanced Phase 6 Features (Newly Available)
print("\n2. Enhanced Phase 6 Features:")

# Get detailed thought trace
detailed_trace = agent.getDetailedThoughtTrace()
print(f"   Detailed trace keys: {list(detailed_trace.keys())}")
print(f"   Percept: {detailed_trace['percept']}")
print(f"   Curiosity: {detailed_trace['curiosity']:.3f}")

# Access Self Model (if available)
self_model = agent.getSelfModel()
if self_model:
    print(f"   Self model confidence: {self_model.getConfidence():.3f}")
    print(f"   Self model consistency: {self_model.getConsistency():.3f}")
else:
    print("   Self model: Not available")

# Access Social Learning (if available)
social_learning = agent.getSocialLearning()
if social_learning:
    print(f"   Social learning episodes: {social_learning.getEpisodeCount()}")
    print(f"   Social bias: {social_learning.getSocialBias():.3f}")
else:
    print("   Social learning: Not available")

# 3. Text Encoding with Statistics
print("\n3. Text Encoding with Statistics:")

# Encode text with basic encoding
basic_encoding = agent.encodeText("hello")
print(f"   Basic encoding for 'hello': {basic_encoding}")

# Encode text with enhanced statistics
enhanced_encoding = pynlm.encodeTextWithStats("hello")
print(f"   Enhanced encoding for 'hello': {enhanced_encoding}")

# Case-insensitive encoding
case_insensitive = agent.encodeText("Hello")
print(f"   Case-insensitive 'Hello': {case_insensitive}")
print(f"   Matches 'hello': {case_insensitive == basic_encoding}")

# Different words encode differently
word1 = agent.encodeText("cat")
word2 = agent.encodeText("dog")
print(f"   'cat' encodes to: {word1}")
print(f"   'dog' encodes to: {word2}")
print(f"   Different words encode differently: {word1 != word2}")

# 4. Concept Formation
print("\n4. Concept Formation:")
concept_formation = brain.getConceptFormation()

# Create test patterns
resource_pattern = [0.8, 0.9, 0.7, 0.6, 0.5]
hazard_pattern = [0.2, 0.3, 0.4, 0.5, 0.6]

resource_concept = concept_formation.encode(resource_pattern)
hazard_concept = concept_formation.encode(hazard_pattern)

# Compute similarity
similarity = concept_formation.computeSimilarity(resource_pattern, hazard_pattern)
print(f"   Resource vs Hazard similarity: {similarity:.3f}")
print(f"   Resource concept ID: {resource_concept}")
print(f"   Hazard concept ID: {hazard_concept}")

# 5. Planning System
print("\n5. Planning System:")
planner = brain.getPlanner()

# Plan action
percept = [0.7, 0.8, 0.6, 0.5]  # Example visual percept
action_plan = planner.planAction(percept)

print(f"   Planned action: {action_plan}")
print(f"   Plan confidence: {planner.getPlanConfidence():.3f}")
print(f"   Plan complexity: {planner.getPlanComplexity()}")

# 6. Self-Model and Social Learning Integration
print("\n6. Self-Model Integration:")
if self_model and social_learning:
    print("   Self-model updated with experience")
    print("   Social learning tracking active")
    print("   All systems integrated successfully")

print("\n✅ All Phase 6 advanced features accessed successfully!")
```

## Example 9: Error Handling and Validation

### Comprehensive Error Handling Examples

```python
import pynlm

print("=== Comprehensive Error Handling Examples ===")

# 1. File I/O Errors
print("\n1. File I/O Error Handling:")
config = pynlm.createDefaultConfig()

try:
    config.loadFromFile("/nonexistent/path/config.json")
    print("❌ Should have raised exception")
except RuntimeError as e:
    print(f"✓ File not found error: {e}")

# 2. Invalid Configuration Errors
print("\n2. Configuration Validation:")
config2 = pynlm.createDefaultConfig()

try:
    config2.set("invalid_parameter", "invalid_value")
    print("❌ Should have raised exception")
except ValueError as e:
    print(f"✓ Invalid parameter error: {e}")

# 3. World Operation Errors
print("\n3. World Operation Errors:")
world = pynlm.createSimpleWorld()

# Try to apply invalid motor command
try:
    world.applyMotorCommand(None, 0.0)  # Invalid command
    print("❌ Should have raised exception")
except RuntimeError as e:
    print(f"✓ Invalid command error: {e}")

# 4. Agent Operation Errors
print("\n4. Agent Operation Errors:")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)

# Try to process input without initialization
try:
    bad_percept = pynlm.Vision(8, 8)
    agent.processSensoryInput(bad_percept)
    print("❌ Should have raised exception")
except RuntimeError as e:
    print(f"✓ Uninitialized agent error: {e}")

# 5. Validation Errors
print("\n5. Input Validation:")

# Validate neuron IDs
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create valid neuron ID
valid_id = pynlm.createNeuron().getId()

# Validate the ID
if brain.validateNeuronId(valid_id):
    print(f"✓ Valid neuron ID: {valid_id}")
else:
    print(f"❌ Invalid neuron ID: {valid_id}")

# Try invalid ID (all zeros typically)
invalid_id = pynlm.NeuronId()  # Default constructor
if not brain.validateNeuronId(invalid_id):
    print(f"✓ Correctly identified invalid ID: {invalid_id}")

print("\n✅ All error handling examples completed successfully!")
```

## Example 10: Performance Monitoring

### Monitoring and Optimizing NLM Performance

```python
import pynlm
import time
import psutil
import os

print("=== Performance Monitoring Examples ===")

# 1. Memory Usage Monitoring
print("\n1. Memory Usage Monitoring:")

process = psutil.Process(os.getpid())

# Get initial memory
initial_memory = process.memory_info().rss / 1024 / 1024  # MB
print(f"   Initial memory usage: {initial_memory:.1f} MB")

# Create and run brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get memory after brain creation
after_brain_memory = process.memory_info().rss / 1024 / 1024  # MB
print(f"   Memory after brain creation: {after_brain_memory:.1f} MB")
print(f"   Memory increase: {after_brain_memory - initial_memory:.1f} MB")

# 2. Performance Benchmarking
print("\n2. Performance Benchmarking:")

def benchmark_operation(operation_name, operation_func, *args, **kwargs):
    """Benchmark an operation and return results"""
    start_time = time.time()
    result = operation_func(*args, **kwargs)
    end_time = time.time()
    
    execution_time = end_time - start_time
    memory_before = process.memory_info().rss / 1024 / 1024
    memory_after = memory_before  # Simplified
    
    print(f"   {operation_name}:")
    print(f"     Execution time: {execution_time:.3f} seconds")
    print(f"     Memory usage: {memory_before:.1f} MB")
    print(f"     Result: {result}")
    
    return execution_time, memory_before, result

# Benchmark brain initialization
time1, mem1, _ = benchmark_operation(
    "Brain initialization", brain.initialize
)

# Benchmark brain steps
start_time = time.time()
for step in range(100):
    brain.step(step)
end_time = time.time()
print(f"   100 brain steps: {end_time - start_time:.3f} seconds")
print(f"   Average per step: {(end_time - start_time) / 100:.3f} seconds")

# 3. Statistics Monitoring
print("\n3. Statistics Monitoring:")

# Get comprehensive statistics
stats = brain.getBrainStats()

print("   Key performance metrics:")
print(f"     Total neurons: {stats.total_neurons:,}")
print(f"     Total synapses: {stats.total_synapses:,}")
print(f"     Active neurons: {stats.active_neurons:,}")
print(f"     Firing neurons: {stats.firing_neurons:,}")
print(f"     Average firing rate: {stats.average_firing_rate:.3f} Hz")
print(f"     Excitatory/Inhibition ratio: {stats.excitatory_inhibition_ratio:.3f}")
print(f"     Total spike count: {stats.total_spike_count:,}")
print(f"     Development stage: {stats.development_stage}")

# 4. Monitoring During Operation
print("\n4. Real-time Monitoring:")

def monitor_during_operation(agent, world, steps=50):
    """Monitor agent during operation"""
    agent.initialize(world)
    
    monitoring_data = {
        'steps': [],
        'firing_counts': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': []
    }
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Collect data
        monitoring_data['steps'].append(step)
        monitoring_data['firing_counts'].append(brain.getFiringNeuronCount())
        monitoring_data['curiosity_levels'].append(agent.getCuriosityLevel())
        monitoring_data['novelty_levels'].append(agent.getNoveltyLevel())
        monitoring_data['development_stages'].append(brain.getDevelopmentalStage())
    
    return monitoring_data

# Create agent and world for monitoring
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)

monitoring_data = monitor_during_operation(agent, world, steps=20)

print("   Monitoring summary:")
print(f"     Total steps: {len(monitoring_data['steps'])}")
print(f"     Average firing neurons: {sum(monitoring_data['firing_counts']) / len(monitoring_data['firing_counts']):.1f}")
print(f"     Average curiosity: {sum(monitoring_data['curiosity_levels']) / len(monitoring_data['curiosity_levels']):.3f}")
print(f"     Average novelty: {sum(monitoring_data['novelty_levels']) / len(monitoring_data['novelty_levels']):.3f}")
print(f"     Development stages observed: {set(monitoring_data['development_stages'])}")

print("\n✅ All performance monitoring examples completed successfully!")
```

## Summary of Improvements Demonstrated

These examples demonstrate all the key improvements to the NLM Python bindings:

### ✅ Usability Improvements
1. **Factory methods** for creating neurons, synapses, regions, populations
2. **Convenience methods** for common operations (episode runner, statistics)
3. **Better error handling** with descriptive exceptions
4. **More Pythonic API** patterns and naming

### ✅ Functionality Gaps Addressed
1. **Memory systems access** (WorkingMemory, EpisodicMemory, AssociativeMemory)
2. **Plasticity systems access** (STDP, Hebbian, StructuralPlasticity)
3. **Phase 6 advanced features** (SelfModel, SocialLearning, enhanced text encoding)
4. **Type safety improvements** with shared_ptr alternatives

### ✅ Code Quality Improvements
1. **Reduced code duplication** through templates
2. **Better error handling** with consistent patterns
3. **Comprehensive documentation** with examples
4. **Improved validation** and type safety

### ✅ Documentation Improvements
1. **Comprehensive docstrings** with examples
2. **Usage examples** for all major features
3. **Error handling documentation**
4. **Performance guidance** and best practices

The improved NLM Python bindings provide a much more user-friendly, feature-complete, and maintainable API while maintaining full backward compatibility with existing code.

```

For implementation details, see:
- ANALYSIS.md - Detailed analysis of issues
- FIX_PLAN.md - Implementation plan
- IMPROVEMENTS_SUMMARY.md - Summary of all improvements
```

All changes maintain backward compatibility while providing significant improvements to usability, functionality, code quality, and documentation.

```