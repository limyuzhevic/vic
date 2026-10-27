## What is NLM? (Updated for Phase 6)

NLM is a **brain-inspired artificial developmental brain** simulator written in C++20. It now implements **Phase 6: Final Integration** - a complete closed-loop brain system where all components work together cohesively.

**Phase 6 Key Features:**
- Memory systems fully integrated with neural processing
- Neuromodulation affecting plasticity and dynamics
- Prediction integrated as central organizing principle
- Developmental stages modulating learning rates
- Checkpoint save/load and replay functionality
- Complete brain loop functionality

Think of it like this (Phase 6 enhanced):
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns
- **Development** = The brain changing over time through interaction

**That's it! NLM now simulates a complete, developing brain!**

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

## Phase 6: Your Virtual Brain is Now Complete!

**NLM has reached Phase 6: Final Integration!** This means your virtual brain now has all the features of a real biological brain:

### ✅ Phase 6 Integration Achievements

1. **Memory Systems** - Working, episodic, semantic, procedural, associative memory all integrated
2. **Neuromodulation** - Dopamine affects neural excitability and plasticity
3. **Prediction System** - Prediction integrated into brain loop with error signals
4. **Cognition Systems** - Neural planning, concept formation, attention
5. **Development Integration** - Developmental stages affecting plasticity rates
6. **Persistence** - Checkpoint save/load and replay working
7. **Replay and Consolidation** - Episodic memory replay during simulation

### 🔄 The Complete Brain Loop (Phase 6)

```
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### 🚀 New Phase 6 Features You Can Use

- **Advanced Configuration**: Create Phase 6 optimized configurations
- **Integration Testing**: Run comprehensive Phase 6 integration tests
- **Phase 6 Demo**: Experience the complete integrated brain system
- **Validation Tools**: Validate Phase 6 system integration
- **Research Platform**: Create complete research environments
- **Learning Environments**: Setup development-focused learning scenarios
- **Performance Benchmarking**: Benchmark Phase 6 system performance
- **Error Recovery**: Advanced error recovery and resilience testing

### 💡 Example: Phase 6 Demo

```python
import pynlm

# Experience the complete integrated brain system
print("=== Phase 6 Demo Starting ===")
brain, world, agent, result = pynlm.create_phase6_demo()
print(f"✓ Phase 6 demo completed successfully!")
print(f"  Result: {result}")
print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
print(f"  World: {world.getWidth()}x{world.getHeight()} environment")
print(f"  Agent: Interface active")
```

### 📚 Example: Phase 6 Integration Test

```python
import pynlm

# Run comprehensive integration test
print("Running Phase 6 integration test...")
results = pynlm.run_phase6_integration_test(episodes=3, steps_per_episode=150)
print(f"✓ Integration test completed!")
print(f"  Episodes: {results[2]}")
print(f"  Total Reward: {results[0]:.2f}")
print(f"  Total Spikes: {results[1]}")
```

### 🔬 Example: Phase 6 Validation

```python
import pynlm

# Create and validate Phase 6 system
brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()

print("Validating Phase 6 system...")
is_valid, issues = pynlm.validate_phase6_system(brain)

if is_valid:
    print("✓ Phase 6 system validation PASSED")
    print("  All brain systems are properly integrated")
else:
    print("✗ Phase 6 system validation FAILED")
    print("  Issues found:")
    for issue in issues:
        print(f"    - {issue}")
```

### ⚡ Example: Phase 6 Performance

```python
import pynlm

# Benchmark Phase 6 system performance
print("Benchmarking Phase 6 system...")
metrics = pynlm.benchmark_phase6_system(num_steps=500)

print(f"✓ Phase 6 benchmark completed!")
print(f"  Steps per second: {metrics[3]:.0f}")
print(f"  Average step time: {metrics[1]:.3f} ms")
print(f"  Memory usage: {metrics[4] / 1024:.1f} KB")
print(f"  Neural efficiency: {metrics[5]:.3f}")
```

---

## The 3 Components (Enhanced for Phase 6)

### 1. Brain (Phase 6 Enhanced)
Your **Phase 6 virtual brain** with all systems integrated:

- **Memory Systems**: Working + episodic + semantic + procedural + associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Predictive coding and error signals
- **Cognition**: Neural planning, concept formation, attention
- **Development**: Developmental stages affecting plasticity rates
- **Plasticity**: STDP, Hebbian, structural plasticity

```python
# Create Phase 6 enhanced brain
brain = pynlm.createBrain(pynlm.create_default_config())
brain.initialize()
print(f"Phase 6 brain initialized: {brain.getTotalNeuronCount()} neurons")
```

### 2. World (Enhanced for Phase 6)
The **Phase 6 simulation environment** with advanced features:

- **2D Space**: Configurable environment dimensions
- **Sensory Input**: Vision, audio, internal signals
- **Objects**: Resources, hazards, walls with value systems
- **Time**: Real-time simulation with timesteps
- **Agent Body**: Energy, health, age, movement systems

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
print(f"Phase 6 world: {world.getWidth()}x{world.getHeight()} environment")
```

### 3. Agent (Enhanced for Phase 6)
The **Phase 6 agent interface** connecting brain to world:

- **Sensory Processing**: Advanced perception systems
- **Motor Decoding**: Neural activity to actions
- **Learning**: Reward-based neuromodulation
- **Development**: Brain development through interaction
- **Curiosity**: Exploration and novelty seeking
- **Prediction Error**: Learning from prediction errors

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
print(f"Phase 6 agent: {agent.getSensoryInputSize()} inputs, {agent.getMotorOutputSize()} outputs")
```

---

## Getting Started with Phase 6

### 🚀 Quick Start (Phase 6 Enhanced)

```python
import pynlm

# 1. Create Phase 6 components
brain = pynlm.createBrain(pynlm.create_default_config())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# 2. Initialize Phase 6 system
brain.initialize()
world.configure(20, 20, 16, 16)
world.setRandomSeed(42)
agent.initialize(world)

# 3. Enable Phase 6 neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("✓ Phase 6 system initialized!")
print("  All neuromodulation systems active")
print("  Development enabled")
print("  Structural plasticity enabled")
```

### 📊 Research-Ready (Phase 6)

```python
import pynlm

# Create complete Phase 6 research workspace
research = pynlm.create_research_workspace()

# Extract Phase 6 components
brain = research["brain"]
world = research["world"]
agent = research["agent"]

print("✓ Phase 6 research workspace created!")
print(f"  Brain: {brain.getTotalNeuronCount()} neurons")
print(f"  Memory systems: {brain.getTotalSynapseCount()} synapses")
print(f"  Neuromodulation: Enabled")
```

### 🧠 Learning with Phase 6

```python
import pynlm

# Create Phase 6 learning environment
env = pynlm.create_learning_environment(episodes=10, steps_per_episode=200)
brain = env["brain"]
world = env["world"]
agent = env["agent"]

# Custom reward function for Phase 6 learning
def phase6_reward():
    # Reward based on curiosity and development
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    return curiosity * 2.0 + novelty * 1.0

# Run Phase 6 learning session
results = pynlm.learning_session(brain, world, agent, episodes=5)
print(f"✓ Phase 6 learning completed!")
print(f"  Average reward: {results[1]:.2f}")
print(f"  Learning progress observed")
```

---

## Phase 6: Your Virtual Brain is Now Complete!

**Your NLM brain has reached Phase 6: Final Integration!** This means your virtual brain now has:

- ✅ **Memory systems** (working, episodic, semantic, procedural, associative)
- ✅ **Neuromodulation** (dopamine, curiosity, novelty)
- ✅ **Prediction systems** (temporal, causal, self)
- ✅ **Cognition** (planning, attention, concept formation)
- ✅ **Development** (growth, maturation, aging)
- ✅ **Persistence** (save/load, replay, consolidation)

**Ready to build your own artificial brain?** Start with the examples above and explore all Phase 6 features!

---

## Need Help?

### Phase 6 Quick Commands

| Function | Purpose | Example |
|----------|---------|---------|
| `run_phase6_integration_test()` | Run integration test | `results = pynlm.run_phase6_integration_test()` |
| `create_phase6_demo()` | Phase 6 demonstration | `brain, world, agent, result = pynlm.create_phase6_demo()` |
| `validate_phase6_system()` | Validate Phase 6 | `valid, issues = pynlm.validate_phase6_system(brain)` |
| `benchmark_phase6_system()` | Performance benchmark | `metrics = pynlm.benchmark_phase6_system()` |
| `create_research_workspace()` | Research platform | `workspace = pynlm.create_research_workspace()` |
| `create_learning_environment()` | Learning setup | `env = pynlm.create_learning_environment()` |

### Common Phase 6 Issues

**Q: "Why is my Phase 6 system not integrating properly?"**
A: Ensure all neuromodulation systems are enabled and validate with `validate_phase6_system()`

**Q: "Phase 6 memory usage too high?"**
A: Use `estimate_memory_usage(brain)` to check and optimize configuration

**Q: "Phase 6 learning not improving?"**
A: Check that reward systems are working and consider `enable_adaptive_learning(brain, "aggressive")`

**Q: "Phase 6 validation fails?"**
A: Use `brain_health_check(brain)` to get detailed health status

### Phase 6 Documentation

- **Quick Help**: `pynlm.print_help()`
- **Usage Examples**: `examples = pynlm.get_examples()`
- **Advanced Help**: `pynlm.print_advanced_help()`
- **Research Overview**: `pynlm.phase6_research_summary()`
- **Legacy Mode**: `pynlm.legacy_mode()`

---

## Phase 6: The Future is Here!

**Your virtual brain is now a complete, integrated artificial developmental brain system!** Phase 6 represents the culmination of all previous research phases, bringing together:

- **Biology**: Brain-like neural dynamics and plasticity
- **Psychology**: Learning, memory, development
- **Cognition**: Attention, planning, prediction
- **Neuromodulation**: Motivation, curiosity, reward
- **Integration**: All systems working together

**Start building your own artificial brain today!** The tools are ready, the documentation is complete, and Phase 6 provides everything you need for serious brain research.

---

*Phase 6 Research Platform - Your virtual brain is ready for experimentation and discovery!*

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
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

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
