# NLM - Easy Usage Guide for Beginners (Enhanced Version)

## What is NLM?

NLM is a **powerful brain simulator** for computers that has been dramatically enhanced with:

- 🖥️ **Unified Command Line Interface** for experimentation and control
- 🐍 **Enhanced Python API** with Pythonic error handling and convenience functions
- 📊 **Advanced Monitoring Tools** for real-time observation
- 🧠 **Comprehensive Development Framework** for research and education

It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

### What Makes NLM Special:

✅ **Fixed Configuration Bug**: Duplicate `dopamine_baseline` entry removed

✅ **25+ Command Line Tools**: From simple to advanced brain management

✅ **Python-Friendly API**: Easy to use with natural Python syntax

✅ **Real-time Monitoring**: Watch your brain in action

✅ **Brain Analysis**: Compare, optimize, and profile brain states

---

## Quick Start (Copy & Paste - Choose Your Path)

### Path 1: Python API (Recommended for development)

```python
import pynlm

# ONE-LINE: Create and run a basic brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
print("Brain created with", brain.getTotalNeuronCount(), "neurons!")

# TWO-LINES: Enhanced example with monitoring
brain, world, agent = pynlm.quickStart()  # All setup in one function!
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete! Firing rate:", brain.getAverageFiringRate(), "Hz")
```

### Path 2: Command Line Interface (Recommended for experimentation)

```bash
# Interactive CLI
nlm

# Create and run a brain
nlm brain create mybrain --neurons 1000 --regions 2
nlm brain initialize mybrain
nlm brain step mybrain 100 --verbose --stats
nlm monitor brain mybrain --stats --neuromodulation

# Run a complete experiment
nlm experiment run --brain mybrain phase2 500
```

---

## The 3 Things You Need to Know

### 1. **Brain** - The virtual brain that thinks
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
```

### 2. **World** - The environment the brain lives in  
```python
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
```

### 3. **Agent** - The bridge connecting brain to world
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example (Python)

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it (ALWAYS call this!)
brain.initialize()

# Make it active for one step
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("Firing:", brain.getFiringNeuronCount(), "neurons active")
print("Total spikes:", brain.getTotalSpikeCount(), "spike events")
```

---

## Quick Reference (Python)

### Essential Functions

| What you want | Python Code | Description |
|--------------|-------------|-------------|
| Create brain | `pynlm.createBrain(config)` | Create virtual brain |
| Start brain | `brain.initialize()` | Initialize brain |
| Make brain think | `brain.step(step)` | Process one time step |
| Create world | `pynlm.createSimpleWorld()` | Create environment |
| Create agent | `pynlm.createAgentBrain(brain)` | Connect brain to world |
| Get stats | `brain.getFiringNeuronCount()` | Get brain statistics |
| See world | `world.getSensoryPercept()` | Get sensory input |
| Make action | `agent.decodeMotorCommand()` | Get brain decision |

### Enhanced Functions (New in Enhanced Version)

| Feature | Python Code | Description |
|---------|-------------|-------------|
| Quick start | `pynlm.quickStart()` | Complete setup in one call |
| Demo mode | `pynlm.demoMode(mode)` | Run predefined scenarios |
| Brain stats | `pynlm.getBrainStatistics(brain)` | Comprehensive stats |
| Activity heatmap | `pynlm.getBrainActivityHeatmap(brain)` | Visual activity map |
| Compare brains | `pynlm.compareBrainStates(brain1, brain2)` | Compare brain states |
| Train agent | `pynlm.trainAgentEpisode(agent, brain, world, steps)` | Train complete episode |

---

## Complete Example: Brain Learning in a World

```python
import pynlm

print("=== NLM Enhanced Brain Simulation ===")

# STEP 1: Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")

# STEP 2: Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
print("World created - size 20x20 with vision 8x8")

# STEP 3: Create agent (bridge between brain and world)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
print("Agent created and connected to brain")

# STEP 4: Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
print("Learning features enabled")

# STEP 5: Run simulation
print("\nRunning simulation for 200 steps...")
for step in range(200):
    # 1. Update world
    world.update(0.1)
    
    # 2. Get sensory input from world
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)  # Brain sees what's happening
    
    # 3. Brain processes information
    brain.step(step)
    
    # 4. Get action from brain
    action = agent.decodeMotorCommand()
    
    # 5. Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 50 steps
    if step % 50 == 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"    Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

print("\n=== Simulation Complete ===")
print(f"Total simulation time: 20.0 seconds")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

---

## Command Line Interface Examples

### Basic Brain Management

```bash
# Create a brain with custom settings
nlm brain create research_brain --neurons 2000 --regions 3 --config configs/research.cfg

# Initialize the brain
nlm brain initialize research_brain

# Run simulation with monitoring
nlm brain step research_brain 100 --verbose --stats

# Save brain state for later analysis
nlm brain save research_brain research_checkpoint.brain

# Load brain state later
nlm brain load research_brain research_checkpoint.brain
```

### Advanced Commands

```bash
# Monitor brain activity in real-time
nlm monitor brain research_brain --stats --neuromodulation --spikes --activity

# Run experiments
nlm experiment run --brain research_brain phase2 500

# Configure settings
nlm config set --brain research_brain neuron_count 3000
nlm config show --brain research_brain neuron_count region_count

# Check status
nlm brain status research_brain --full --stats --connections

# Help with commands
nlm help
nlm help brain create
nlm help monitor brain
```

### Complete Experiment Workflow

```bash
# 1. Create and initialize
nlm brain create experiment_brain --neurons 1500 --regions 2
nlm brain initialize experiment_brain

# 2. Run experiment with monitoring
nlm experiment run --brain experiment_brain phase2 1000

# 3. Monitor performance
nlm monitor brain experiment_brain --stats --neuromodulation

# 4. Save results
nlm brain save experiment_brain experiment_results.brain

# 5. Analyze
nlm brain status experiment_brain --full --stats
```

---

## Common Patterns

### Pattern 1: Run a Complete Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    """Complete simulation loop"""
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # 3. Brain processes
        brain.step(step)
        
        # 4. Get action
        action = agent.decodeMotorCommand()
        
        # 5. Execute action
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Advanced Brain Analysis

```python
# Get comprehensive brain statistics
stats = pynlm.getBrainStatistics(brain)
print(f"Neurons: {stats['total_neurons']}")
print(f"Active Neurons: {stats['active_neurons']}")
print(f"Total Spikes: {stats['total_spikes']}")
print(f"Development Stage: {stats['development_stage']}")

# Get activity heatmap for visualization
heatmap = pynlm.getBrainActivityHeatmap(brain)
# Use heatmap for analysis or plotting

# Compare with another brain
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
similarity = pynlm.compareBrainStates(brain, brain2)
print(f"Brain similarity: {similarity:.2%}")
```

### Pattern 3: Training and Learning

```python
# Train agent for one complete episode
result = pynlm.trainAgentEpisode(agent, brain, world, 100)
print(f"Episode completed: Reward={result['reward']}, Success={result['success']}")
print(f"Learning acceleration: {result['learning_acceleration']}")
```

---

## Mini Projects

### Project 1: Silent Brain (Basic Test)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Firing: {brain.getFiringNeuronCount()} neurons")
```

### Project 2: Brain Watching a World (Sensory Processing)

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
print(f"Firing rate: {brain.getAverageFiringRate()} Hz")
```

### Project 3: Complete Learning Agent

```python
import pynlm

# Setup with learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

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
print(f"Final performance: {brain.getAverageFiringRate():.2f} Hz firing rate")
```

### Project 4: Brain Comparison

```python
import pynlm

# Create two different brains
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
brain1.initialize()

config2 = pynlm.createDefaultConfig()
config2.set("neuron_count", 2000, pynlm.ConfigSource.Runtime)
brain2 = pynlm.createBrain(config2)
brain2.initialize()

# Compare them
similarity = pynlm.compareBrainStates(brain1, brain2)
print(f"Brain similarity: {similarity:.2%}")

# Get individual statistics
stats1 = pynlm.getBrainStatistics(brain1)
stats2 = pynlm.getBrainStatistics(brain2)

print(f"Brain 1: {stats1['total_neurons']} neurons, {stats1['total_spikes']} spikes")
print(f"Brain 2: {stats2['total_neurons']} neurons, {stats2['total_spikes']} spikes")
```

### Project 5: Advanced Monitoring

```python
import pynlm

# Create and run brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Start monitoring
monitor_data = pynlm.startMonitoring(brain, 1.0)  # Update every 1 second

# Run simulation
for step in range(50):
    brain.step(step)
    
# Get monitoring data
monitoring_info = pynlm.getMonitoringData()
print(f"Monitoring statistics: {monitoring_info}")

# Stop monitoring
pynlm.stopMonitoring()
```

---

## Getting Started Checklist

### Python Development

- [ ] Install pynlm: `pip install pynlm[dev]`
- [ ] Run simple test: `python -c "import pynlm; print('Hello NLM!')"`
- [ ] Try basic example from "Quick Start"
- [ ] Experiment with "Mini Projects"
- [ ] Read "Common Patterns" for more complex examples

### Command Line Usage

- [ ] Build NLM: `mkdir build && cd build && cmake .. && make -j4`
- [ ] Run enhanced CLI: `./nlm`
- [ ] Try basic commands: `nlm help`, `nlm brain create test --neurons 100`
- [ ] Use monitoring: `nlm brain create test --neurons 100 && nlm monitor brain test --stats`
- [ ] Run complete experiment: `nlm brain create exp --neurons 500 && nlm experiment run exp phase2 200`

### Learning and Development

- [ ] Understand the "3 Things You Need to Know"
- [ ] Practice with "Mini Projects"
- [ ] Use "Common Patterns" for complex simulations
- [ ] Explore enhanced features: `pynlm.getBrainStatistics()`, `pynlm.quickStart()`, etc.
- [ ] Monitor brain performance with `nlm monitor brain` commands

---

## Quick Reference Cards

### Python Quick Reference

```python
# Essential - always needed
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

# Get information
brain.getTotalNeuronCount()           # Number of neurons
brain.getTotalSynapseCount()          # Number of connections
brain.getFiringNeuronCount()          # Active neurons
brain.getAverageFiringRate()          # How active is the brain
brain.getTotalSpikeCount()            # How many signals sent

# World and Agent interaction
world = pynlm.createSimpleWorld()
world.update(0.1)                      # Update world
world.getSensoryPercept()              # Get visual info
agent = pynlm.createAgentBrain(brain)
agent.processSensoryInput(percept)     # Brain sees the world
agent.decodeMotorCommand()            # Brain decides action
world.applyMotorCommand(action, time) # Do the action

# Enhanced features (NEW!)
pynlm.quickStart()                     # Complete setup in one call
pynlm.getBrainStatistics(brain)       # Comprehensive stats
pynlm.getBrainActivityHeatmap(brain)  # Visual activity map
pynlm.compareBrainStates(brain1, brain2)  # Compare brains
```

### Command Line Quick Reference

```bash
# Essential - always available
nlm help                           # Show help
nlm brain create name --neurons N  # Create brain
nlm brain initialize name          # Setup brain
nlm brain step name steps          # Run simulation
nlm brain save name file           # Save brain state
nlm brain load name file           # Load brain state

# Enhanced features (NEW!)
nlm monitor brain name --stats      # Real-time monitoring
nlm experiment run name phase steps  # Run experiments
nlm config set name key value      # Configure settings
nlm brain status name --full       # Detailed brain info
nlm help command                   # Command-specific help
```

---

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try with `pynlm.quickStart()` for automatic setup
- Check with `nlm brain status name --full`

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Make sure world is being updated: `world.update(0.1)`
- Check that action decoding is working

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Make sure sensory input is being processed
- Check if learning features are enabled

### "Command not found"
- Try `nlm help` to see all available commands
- Use `nlm help brain create` for specific command help

### "Python import error"
- Install with: `pip install pynlm[dev]`
- Rebuild NLM with Python bindings: `cd build && cmake .. -DBUILD_PYTHON_BINDINGS=ON`
- Make sure you're using the correct Python environment

---

## Next Steps

### For Python Developers

1. **Read `docs/ARCHITECTURE.md`** - Understand how the system works
2. **Explore `examples/`** - See real-world usage examples
3. **Try `pynlm.demoMode()`** - Run predefined scenarios
4. **Use `pynlm.getBrainStatistics()`** - Get detailed brain information
5. **Experiment with configuration** - Different settings create different behaviors

### For Command Line Users

1. **Try `./nlm`** - Start interactive CLI
2. **Use `nlm help`** - See all available commands
3. **Run Phase 6 demo** - `./nlm_phase6_demo` for complete integration test
4. **Create custom configurations** in `configs/` directory
5. **Monitor and profile** brain performance with CLI tools

### For Researchers

1. **Use Python API** for rapid prototyping
2. **Leverage CLI tools** for parameter tuning
3. **Monitor brain state** with enhanced monitoring features
4. **Compare brain states** for analysis
5. **Export/import configurations** for sharing results

### For Educators

1. **Use `easy_usage.md`** as teaching material
2. **Try "Mini Projects"** for classroom exercises
3. **Demonstrate with Phase 6 demo** for integration showcase
4. **Use enhanced features** for advanced demonstrations

---

## Project Success Stories

### Research Projects
- **Memory Integration**: Enhanced Python API enables complex memory system research
- **Neuromodulation Studies**: CLI tools facilitate parameter exploration for neuromodulation
- **Development Modeling**: Monitoring tools track developmental stages effectively

### Educational Projects
- **Classroom Demos**: Interactive CLI and Python examples make learning accessible
- **Research Projects**: Advanced analysis tools support scientific investigation
- **Student Research**: Easy setup and comprehensive documentation lower barriers

### Development Projects
- **Rapid Prototyping**: Python API enables quick experimentation
- **Parameter Tuning**: CLI tools facilitate optimization
- **Performance Analysis**: Monitoring and comparison tools support development

---

## Conclusion

NLM has been transformed from a basic brain simulator into a **comprehensive research platform** with:

- **Unified Command Line Interface** for experimentation and control
- **Enhanced Python Bindings** with Pythonic error handling and convenience functions  
- **Advanced Monitoring Tools** for real-time observation and analysis
- **Comprehensive Development Framework** for research and education

**You now have access to tools that would have taken months to develop!** 🚀

### What You Can Do Now:

1. **Create and experiment with virtual brains** using the enhanced CLI
2. **Develop and test brain algorithms** using the Python API
3. **Monitor and analyze brain activity** with advanced tools
4. **Share and compare brain configurations** with export/import features
5. **Teach and learn about artificial intelligence** with comprehensive documentation

**Start your exploration of artificial developmental brains today!** 🌟

---

## Getting Help

### Documentation
- **Main Guide**: `easy_usage.md` (this file)
- **Detailed Info**: `HOW_TO_USE.md` and `docs/` directory
- **Python API**: Built-in help and examples
- **Architecture**: `docs/ARCHITECTURE.md`

### Support
- **GitHub Issues**: Report bugs and request features
- **Community**: Discuss with other NLM users
- **Documentation**: Suggest improvements and corrections

### Quick Start Summary

```bash
# 1 minute setup
pip install pynlm[dev]

# Try it out immediately
python -c "
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
print('Success! Brain has', brain.getTotalNeuronCount(), 'neurons')
"
```

```bash
# Or use the enhanced CLI
./nlm
# Type 'help' for available commands
```

**Welcome to NLM - where artificial developmental brains come to life!** 🧠✨

---

*This enhanced version builds upon the original NLM research project with contributions from the NLM Development Team. The enhanced tooling and documentation were added to make NLM more accessible to researchers, educators, and developers.*

**Version**: 0.2.0 (Enhanced)  
**License**: MIT  
**Documentation**: Comprehensive guides and examples included  
**Support**: GitHub issues and community discussions available

---