This is the main NLM Python interface. It provides Python bindings for the NLM C++ neural simulation framework.

**Key Features:**
- Brain class: Central neural simulation brain with real LIF neuron dynamics
- Config class: Configuration for NLM system with JSON support
- AgentBrain class: Interface connecting NLM brain to world
- SimpleWorld class: Simple 2D world for NLM simulation
- Vision, Audio, and InternalSignals classes: Sensory input types
- MotorCommand and Action classes: Action representations
- WorldObject and AgentBody: State representations
- ActionResult: Results from world interactions

**Basic Usage Example:**

```python
import nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Create a brain with configuration
brain = nlm.createBrain(config)
brain.initialize()

# Create a simple world
world = nlm.createSimpleWorld()
world.configure(16, 16)

# Create agent brain interface
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable neuromodulation for learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Simulation loop
for step in range(100):
    # Get observation from world
    percept = world.observe(agent)
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step)
    
    # Get motor command
    cmd = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyAction(agent, cmd)
    
    # Compute reward
    reward = world.computeReward(agent)
    
    # Apply reward modulation
    agent.applyRewardModulation(reward, 0.0)
    
print("Simulation complete!")
print(f"Total reward: {sum(rewards)/len(rewards):.2f}")
```

**Available Classes:**

1. **Config**: Load and save configuration from files or command line
2. **Brain**: Core neural simulation with LIF neurons, plasticity, and memory
3. **AgentBrain**: Connects brain to world through sensory/motor interfaces
4. **SimpleWorld**: 2D world with resources, hazards, and agent navigation
5. **Vision**: Visual sensory input (256 values = 16x16 pixels)
6. **Audio**: Audio sensory input (sample-based)
7. **InternalSignals**: Internal state signals (energy, health, etc.)
8. **Action**: Motor commands with parameters
9. **WorldObject**: Objects in the world (resources, hazards, walls)
10. **AgentBody**: Agent state (position, orientation, energy, etc.)
11. **ActionResult**: Results from world interactions (reward, success, message)
12. **SensoryPercept**: Complete sensory observation (vision + touch + internal + proprioception)

**Installation:**

```bash
# From the project root:
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python module
pip install .
```

**Testing:**

Run the Phase 6 integration test:
```bash
./nlm_phase6_demo
```

**Performance Features:**
- Event-driven spike processing
- STDP and Hebbian plasticity
- Working memory, episodic memory, and prediction systems
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages affecting plasticity
- Checkpoint save/load for persistent brains
- Replay and consolidation mechanisms

The Python bindings provide a complete interface to the NLM artificial developmental brain system, allowing simulation and experimentation in Python while leveraging the high-performance C++ implementation.

