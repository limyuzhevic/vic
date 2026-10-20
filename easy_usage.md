# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## NEW! Advanced Brain Systems (Phase 6 Integration)

The NLM framework now includes **Phase 6: Complete Integration** of all brain systems! You can now access:

### 🧠 **Memory Systems**

NLM now has three sophisticated memory systems:

#### 1. NeuralWorkingMemory
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access working memory
working_memory = pynlm.getWorkingMemoryFromBrain(brain)

# Store patterns in working memory
working_memory.store([0.1, 0.2, 0.3, 0.4, 0.5], 0.8)

# Retrieve patterns
pattern = working_memory.retrieve()
print("Retrieved pattern:", pattern)

# Get memory statistics
print("Capacity:", working_memory.getCapacity())
print("Active traces:", working_memory.getActiveTraces())
print("Memory activity:", working_memory.getMemoryActivity())
```

#### 2. NeuralEpisodicMemory
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access episodic memory
episodic_memory = pynlm.getEpisodicMemoryFromBrain(brain)

# Create and store an episode
episode = pynlm.EpisodicMemoryItem()
episode.action = pynlm.ActionType.MoveForward
episode.reward = 0.8
episode.energy = 0.9

episodic_memory.storeEpisode(episode)

# Retrieve recent episodes
recent = episodic_memory.getRecentEpisodes(5)
print(f"Retrieved {len(recent)} recent episodes")

# Get statistics
print("Episode count:", episodic_memory.getEpisodeCount())
print("Average reward:", episodic_memory.getAverageReward())
```

#### 3. NeuralAssociativeMemory
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access associative memory
associative_memory = pynlm.getAssociativeMemoryFromBrain(brain)

# Create associations between patterns
pattern_a = [0.1, 0.2, 0.3, 0.4, 0.5]
pattern_b = [0.6, 0.7, 0.8, 0.9, 1.0]

associative_memory.associate(pattern_a, pattern_b, 0.7)

# Retrieve associated patterns
associated = associative_memory.retrieve(pattern_a, 3)
print(f"Found {len(associated)} associated patterns")

# Get association strength
strength = associative_memory.getAssociationStrength(pattern_a, pattern_b)
print(f"Association strength: {strength}")
```

### 🧠 **Cognition Systems**

NLM now has three advanced cognition systems:

#### 4. NeuralPlanner
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access neural planner
neural_planner = pynlm.getNeuralPlannerFromBrain(brain)

# Set planning goal
neural_planner.setCurrentGoal([0.1, 0.2, 0.3, 0.4, 0.5])

# Make plans
action = neural_planner.planAction([0.2, 0.4, 0.6, 0.8, 1.0])
print(f"Planned action: {action}")

# Get planning statistics
print(f"Planning depth: {neural_planner.getPlanningDepth()}")
print(f"Planning confidence: {neural_planner.getPlanningConfidence()}")
```

#### 5. ConceptFormation
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access concept formation
concept_formation = pynlm.getConceptFormationFromBrain(brain)

# Learn concepts
pattern = [0.5, 0.6, 0.7, 0.8, 0.9]
features = [1.0, 1.2, 1.5, 2.0]
concept_id = concept_formation.learnConcept(pattern, features, 0.8, 100)

print(f"Learned concept ID: {concept_id}")

# Classify new patterns
new_pattern = [0.4, 0.5, 0.6]
classified_id = concept_formation.classify(new_pattern)
print(f"Classified as concept ID: {classified_id}")

print(f"Total concepts learned: {concept_formation.getConceptCount()}")
```

#### 6. AttentionalSelection
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access attentional selection
attention = pynlm.getAttentionalSelectionFromBrain(brain)

# Set attention parameters
attention.setInhibitionStrength(0.5)
attention.setExcitationStrength(1.5)
attention.setCompetitionThreshold(0.3)

# Process competing inputs
competitors = [0.1, 0.5, 0.3, 0.8, 0.2]
attention_value = attention.process_competition(competitors, 0.2)
print(f"Attention value: {attention_value}")

# Focus on a region
attention.focus_on_region("cortex")
print("Focused on cortex region")

print(f"Winners: {attention.get_winners()}")
```

### 🧠 **Development System**

NLM now has a complete development system:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access development system
development = pynlm.getDevelopmentSystemFromBrain(brain)

# Get developmental stage
print(f"Current stage: {development.getStage()}")
print(f"Stage name: {development.getStageName()}")

# Get developmental age
print(f"Developmental age: {development.getDevelopmentalAge()}")
print(f"Plasticity modifier: {development.getPlasticityModifier()}")

# Set development
development.setDevelopmentalAge(10.5)
development.advanceStage()
```

### 🧠 **Neuromodulation Systems**

NLM now has three neuromodulation systems:

#### 7. Dopamine
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access dopamine system
dopamine = pynlm.getDopamineFromBrain(brain)

# Control dopamine
dopamine.setLevel(0.5)
print(f"Dopamine level: {dopamine.getLevel()}")
print(f"Plasticity factor: {dopamine.getPlasticityFactor()}")

# Signal reward and prediction error
dopamine.signalReward(0.8)
dopamine.signalRewardPredictionError(-0.3)
```

#### 8. Novelty
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access novelty system
novelty = pynlm.getNoveltyFromBrain(brain)

# Control novelty
novelty.setLevel(0.3)
print(f"Novelty level: {novelty.getLevel()}")

# Novelty detection happens automatically based on input patterns
```

#### 9. PredictionError
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access prediction error system
prediction_error = pynlm.getPredictionErrorFromBrain(brain)

# Control prediction error
prediction_error.setLevel(-0.5)
print(f"Prediction error level: {prediction_error.getLevel()}")
print(f"Prediction error sign: {prediction_error.getSign()}")

# Update based on prediction
prediction_error.update(0.1, 1.0, 1.2)
```

---

## 🔧 **New Convenience Functions**

### System Factory Functions
```python
import pynlm

# Create individual systems directly
system_config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(system_config)

# Create system-specific configurations
wm_config = pynlm.createWorkingMemoryConfig()
pred_config = pynlm.createPredictionConfig()
cog_config = pynlm.createCognitionConfig()
neuro_config = pynlm.createNeuromodulationConfig()
dev_config = pynlm.createDevelopmentConfig()

# Create system instances directly
working_memory = pynlm.createWorkingMemory()
episodic_memory = pynlm.createEpisodicMemory()
associative_memory = pynlm.createAssociativeMemory()
prediction_system = pynlm.createPredictionSystem()
neural_planner = pynlm.createNeuralPlanner()
concept_formation = pynlm.createConceptFormation()
attentional_selection = pynlm.createAttentionalSelection()
development_system = pynlm.createDevelopmentSystem()
dopamine = pynlm.createDopamine()
novelty = pynlm.createNovelty()
prediction_error = pynlm.createPredictionError()
```

### Type Conversion Helpers
```python
import pynlm

# Convert between types
neuron_id = pynlm.neuron_id_from_index(42)
index = pynlm.neuron_id_to_index(neuron_id)

region_id = pynlm.region_id_from_name("hippocampus")

action_type = pynlm.action_type_from_string("MoveForward")
string_name = pynlm.action_string_from_type(action_type)
```

---

## 🧪 **Integration Example: Complete Brain System**

Here's a complete example using all the new systems:

```python
import pynlm

def run_advanced_simulation(steps=1000):
    """Run a comprehensive simulation using all NLM systems"""
    
    # 1. Create brain with advanced configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # 3. Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 4. Run simulation with system monitoring
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print system status every 100 steps
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Neural Activity: {brain.getFiringNeuronCount()} neurons firing")
            print(f"  Working Memory: {brain.getWorkingMemory().getActiveTraces()} traces")
            print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
    
    print("\nSimulation complete!")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_advanced_simulation(1000)
```

---

## 📊 **Quick Reference - New Methods**

### Memory Systems
| System | Methods | Description |
|--------|---------|-------------|
| NeuralWorkingMemory | `store()`, `retrieve()`, `update()`, `getActiveTraces()` | Short-term storage with persistence |
| NeuralEpisodicMemory | `storeEpisode()`, `retrieveSimilar()`, `getRecentEpisodes()` | Long-term experience storage |
| NeuralAssociativeMemory | `associate()`, `retrieve()`, `getAssociationStrength()` | Pattern association learning |

### Cognition Systems
| System | Methods | Description |
|--------|---------|-------------|
| NeuralPlanner | `planAction()`, `evaluateSequence()`, `setCurrentGoal()` | Action planning and decision making |
| ConceptFormation | `learnConcept()`, `classify()`, `getConceptCount()` | Pattern discovery and categorization |
| AttentionalSelection | `process_competition()`, `focus_on_region()` | Neural attention and focus |

### Development System
| System | Methods | Description |
|--------|---------|-------------|
| DevelopmentSystem | `get_stage()`, `set_stage()`, `advanceStage()`, `getDevelopmentalAge()` | Brain maturation and development |

### Neuromodulation Systems
| System | Methods | Description |
|--------|---------|-------------|
| Dopamine | `getLevel()`, `setLevel()`, `signalReward()` | Reward and reinforcement learning |
| Novelty | `getLevel()`, `setLevel()` | Novelty detection and exploration drive |
| PredictionError | `getLevel()`, `setLevel()`, `update()` | Prediction error signaling |

### Helper Functions
| Function | Description |
|----------|-------------|
| `getWorkingMemoryFromBrain()` | Get working memory from brain |
| `getEpisodicMemoryFromBrain()` | Get episodic memory from brain |
| `getAssociativeMemoryFromBrain()` | Get associative memory from brain |
| `createWorkingMemoryConfig()` | Create working memory configuration |
| `createPredictionConfig()` | Create prediction system configuration |

---

## 🚀 **Getting Started**

1. **Install the package**: `pip install pynlm`
2. **Run the test script**: `python3 test_bindings.py`
3. **Try the examples**: Copy and modify the examples above
4. **Explore further**: Read `HOW_TO_USE.md` for more details

---

## 🆘 **Troubleshooting**

**"My advanced brain systems aren't working"**
- Did you call `brain.initialize()` before accessing the systems?
- Check that all helper functions have proper error handling
- Use the test script to verify all bindings are working

**"Memory systems are showing 0"**
- Store some patterns in the memory systems before retrieving
- Check that memory systems are properly initialized
- Use the convenience methods like `initializeAllSystems()`

**"Cognition systems returning default values"**
- Set goals and targets for planning systems
- Use the `setCurrentGoal()` method for neural planner
- Test systems individually with simple patterns

---

## 📚 **Next Steps**

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for comprehensive documentation
2. **Read `docs/ARCHITECTURE.md`** to understand system integration
3. **Experiment with different configurations** using the convenience functions
4. **Use the test script** `test_bindings.py` to verify your setup
5. **Create your own simulation** using all the new systems


That's it! You're now ready to use the **complete NLM Python bindings** with all Phase 6 integrated systems. The new convenience functions and helper methods make it easy to work with the sophisticated brain systems while maintaining the simplicity of the original API.

The Python bindings now provide access to all the advanced cognitive and neuromodulatory systems that make NLM a truly integrated artificial brain!
