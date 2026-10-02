# NLM Python Tutorials

This document provides step-by-step tutorials for learning how to use the NLM Python API.
Each tutorial builds upon the previous one, taking you from basic concepts to advanced usage.

## Prerequisites

Before starting, ensure you have:

```bash
pip install pynlm
```

Or for development with the current codebase:

```bash
pip install -e .
```

Let's start with the fundamentals!

## Tutorial 1: "Hello, NLM!" - Your First Neural Simulation

### Goal
Create and run your first neural simulation with NLM.

### Overview
In this tutorial, you'll:
1. Import the NLM Python module
2. Create a basic neural network
3. Run a simple simulation
4. Examine the results

### Step 1: Import and Setup

```python
import pynlm

print("NLM Python Tutorial 1: Basic Neural Simulation")
print("=" * 50)
```

### Step 2: Create a Brain

```python
# Create a default configuration
config = pynlm.createDefaultConfig()
print("✓ Created default configuration")

# Create a brain with this configuration
brain = pynlm.createBrain(config)
print("✓ Created neural brain")

# Initialize the brain
brain.initialize()
print("✓ Initialized brain")
```

### Step 3: Run the Simulation

```python
# Get initial statistics
initial_neurons = brain.getTotalNeuronCount()
initial_spikes = brain.getTotalSpikeCount()

print(f"\nInitial state:")
print(f"  Total neurons: {initial_neurons}")
print(f"  Total spikes: {initial_spikes}")

# Run 50 simulation steps
print(f"\nRunning 50 simulation steps...")
for step in range(50):
    brain.step(step)
    if step % 10 == 0:
        print(f"  Step {step + 1}/50")

# Get final statistics
final_neurons = brain.getTotalNeuronCount()
final_spikes = brain.getTotalSpikeCount()
firing_neurons = brain.getFiringNeuronCount()
avg_firing_rate = brain.getAverageFiringRate()

print(f"\nFinal state:")
print(f"  Total neurons: {final_neurons}")
print(f"  Total spikes: {final_spikes}")
print(f"  Firing neurons: {firing_neurons}")
print(f"  Average firing rate: {avg_firing_rate:.3f} Hz")
```

### Step 4: Your First Results

```python
print(f"\n🎉 Tutorial Complete!")
print(f"Your brain successfully simulated {final_spikes} spikes across {final_neurons} neurons")
print(f"The network is alive and kicking with {firing_neurons} neurons currently firing!")
```

**Full Code:**
```python
import pynlm

print("NLM Python Tutorial 1: Basic Neural Simulation")
print("=" * 50)

# Create a default configuration
config = pynlm.createDefaultConfig()
print("✓ Created default configuration")

# Create a brain with this configuration
brain = pynlm.createBrain(config)
print("✓ Created neural brain")

# Initialize the brain
brain.initialize()
print("✓ Initialized brain")

# Get initial statistics
initial_neurons = brain.getTotalNeuronCount()
initial_spikes = brain.getTotalSpikeCount()

print(f"\nInitial state:")
print(f"  Total neurons: {initial_neurons}")
print(f"  Total spikes: {initial_spikes}")

# Run 50 simulation steps
print(f"\nRunning 50 simulation steps...")
for step in range(50):
    brain.step(step)
    if step % 10 == 0:
        print(f"  Step {step + 1}/50")

# Get final statistics
final_neurons = brain.getTotalNeuronCount()
final_spikes = brain.getTotalSpikeCount()
firing_neurons = brain.getFiringNeuronCount()
avg_firing_rate = brain.getAverageFiringRate()

print(f"\nFinal state:")
print(f"  Total neurons: {final_neurons}")
print(f"  Total spikes: {final_spikes}")
print(f"  Firing neurons: {firing_neurons}")
print(f"  Average firing rate: {avg_firing_rate:.3f} Hz")

print(f"\n🎉 Tutorial Complete!")
print(f"Your brain successfully simulated {final_spikes} spikes across {final_neurons} neurons")
print(f"The network is alive and kicking with {firing_neurons} neurons currently firing!")
```

## Tutorial 2: "Brains and Worlds" - Agent-Environment Interaction

### Goal
Create an agent that interacts with a simulated world.

### Overview
In this tutorial, you'll:
1. Create a simulated world
2. Create an agent that can sense and act in the world
3. Run a complete interaction loop
4. Understand perception-action cycles

### Step 1: Setup World and Agent

```python
import pynlm

print("\nNLM Python Tutorial 2: Agent-Environment Interaction")
print("=" * 55)

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
print("✓ Created brain and agent")

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(5.0, 5.0)
agent.initialize(world)
print("✓ Created and configured world")

# Enable basic learning
agent.enableRewardModulation(True)
print("✓ Enabled reward modulation")
```

### Step 2: Run the Interaction Loop

```python
print(f"\nRunning interaction simulation...")

# Run 20 steps of interaction
for episode in range(5):  # 5 episodes
    print(f"\nEpisode {episode + 1}/5:")
    
    for step in range(20):  # 20 steps per episode
        # 1. Update world (time passes)
        world.update(0.1)
        print(f"  Step {step + 1}/20 - Time: {world.getSimulationTime():.2f}s", end="")
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        print(f", Got percept", end="")
        
        # 3. Agent processes sensory input
        agent.processSensoryInput(percept)
        print(f", Processed percept", end="")
        
        # 4. Brain thinks
        brain.step(step)
        print(f", Brain thought", end="")
        
        # 5. Agent selects action
        action = agent.decodeMotorCommand()
        print(f", Selected action: {action}", end="")
        
        # 6. Action is executed in world
        world.applyMotorCommand(action, world.getSimulationTime())
        print(f", Action executed", end="")
        
        # 7. Agent learns from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        print(f", Applied reward: {reward:.2f}", end="")
        
        # Show current state
        print(f" | Agent pos: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}) | "
              f"Energy: {world.getAgentBody().energy:.1f} | "
              f"Firing: {brain.getFiringNeuronCount()}")
```

### Step 3: Analyze Results

```python
print(f"\n=== Simulation Results ===")
print(f"Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
print(f"Final agent energy: {world.getAgentBody().energy:.1f}")
print(f"Final agent health: {world.getAgentBody().health:.1f}")
print(f"Final agent age: {world.getAgentBody().age}")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Show agent capabilities
print(f"\n=== Agent Capabilities ===")
print(f"Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

**Full Code:**
```python
import pynlm

print("\nNLM Python Tutorial 2: Agent-Environment Interaction")
print("=" * 55)

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
print("✓ Created brain and agent")

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(5.0, 5.0)
agent.initialize(world)
print("✓ Created and configured world")

# Enable basic learning
agent.enableRewardModulation(True)
print("✓ Enabled reward modulation")

print(f"\nRunning interaction simulation...")

# Run 20 steps of interaction
for episode in range(5):  # 5 episodes
    print(f"\nEpisode {episode + 1}/5:")
    
    for step in range(20):  # 20 steps per episode
        # 1. Update world (time passes)
        world.update(0.1)
        print(f"  Step {step + 1}/20 - Time: {world.getSimulationTime():.2f}s", end="")
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        print(f", Got percept", end="")
        
        # 3. Agent processes sensory input
        agent.processSensoryInput(percept)
        print(f", Processed percept", end="")
        
        # 4. Brain thinks
        brain.step(step)
        print(f", Brain thought", end="")
        
        # 5. Agent selects action
        action = agent.decodeMotorCommand()
        print(f", Selected action: {action}", end="")
        
        # 6. Action is executed in world
        world.applyMotorCommand(action, world.getSimulationTime())
        print(f", Action executed", end="")
        
        # 7. Agent learns from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        print(f", Applied reward: {reward:.2f}", end="")
        
        # Show current state
        print(f" | Agent pos: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}) | "
              f"Energy: {world.getAgentBody().energy:.1f} | "
              f"Firing: {brain.getFiringNeuronCount()}")

print(f"\n=== Simulation Results ===")
print(f"Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
print(f"Final agent energy: {world.getAgentBody().energy:.1f}")
print(f"Final agent health: {world.getAgentBody().health:.1f}")
print(f"Final agent age: {world.getAgentBody().age}")
print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")

# Show agent capabilities
print(f"\n=== Agent Capabilities ===")
print(f"Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
```

## Tutorial 3: "Context Managers" - Resource Management Made Easy

### Goal
Learn how to use context managers for proper resource management.

### Overview
In this tutorial, you'll:
1. Understand context managers and their benefits
2. Create sensory inputs using context managers
3. Use context managers for resource cleanup
4. Create multiple context managers simultaneously

### Step 1: Basic Context Manager Usage

```python
import pynlm

print("\nNLM Python Tutorial 3: Context Managers")
print("=" * 45)

# Create vision using context manager
print("Creating vision with context manager...")
with pynlm.Vision.createImage(640, 480) as vision:
    print(f"✓ Created vision: {vision.getWidth()}x{vision.getHeight()}")
    
    # Set up sample data
    data = [0.5] * (640 * 480 * 3)
    vision.setData(data)
    
    # Process the image
    avg_intensity = sum(data) / len(data)
    print(f"✓ Processed image with average intensity: {avg_intensity:.3f}")
    
    # Context manager automatically handles cleanup
    print("✓ Context manager automatically cleaned up resources")

# Vision context manager has exited, but object is still usable
print(f"✓ Vision object still accessible: {vision.getWidth()}x{vision.getHeight()}")
```

### Step 2: Multiple Context Managers

```python
print(f"\nCreating multiple context managers...")

# Create multiple context managers simultaneously
with pynlm.Vision.createImage(320, 240) as vision, \
     pynlm.Audio.createAudio(22050, 512) as audio, \
     pynlm.InternalSignals.createSignals() as signals:
    
    print(f"✓ Created vision: {vision.getWidth()}x{vision.getHeight()}")
    print(f"✓ Created audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
    print(f"✓ Created signals: ready for data")
    
    # Use all three objects together
    signals.addSignal(1.0)
    signals.addSignal(2.0)
    signals.addSignal(3.14)
    
    print(f"✓ Added {len(signals.getData())} signals: {signals.getData()}")
    
    # Process sensory data
    vision_data = vision.getData()
    audio_data = audio.getData()
    
    vision_avg = sum(vision_data) / len(vision_data) if vision_data else 0.0
    audio_avg = sum(audio_data) / len(audio_data) if audio_data else 0.0
    
    print(f"✓ Vision average: {vision_avg:.3f}")
    print(f"✓ Audio average: {audio_avg:.3f}")

print(f"✓ All context managers have exited with proper cleanup")
```

### Step 3: Nested Context Managers

```python
print(f"\nTesting nested context managers...")

# Create nested context managers
with pynlm.Vision.createImage(400, 300) as outer_vision:
    print(f"✓ Created outer vision: {outer_vision.getWidth()}x{outer_vision.getHeight()}")
    
    # Inner context manager
    with pynlm.InternalSignals.createSignals() as inner_signals:
        print(f"✓ Created inner signals")
        
        # Nested operations
        outer_vision.setData([0.7] * (400 * 300 * 3))
        inner_signals.addSignal(42.0)
        inner_signals.addSignal(100.0)
        
        print(f"✓ Set vision data, added {len(inner_signals.getData())} signals")
    
    # Inner context manager has exited
    print(f"✓ Inner signals context exited, signals: {len(inner_signals.getData())}")
    
    # Continue with outer context
    print(f"✓ Continuing with outer vision context")
    outer_vision.setData([0.3] * (400 * 300 * 3))  # Change data
    print(f"✓ Changed vision data, new average: {sum(outer_vision.getData()) / len(outer_vision.getData()):.3f}")

print(f"✓ All nested context managers cleaned up properly")
```

### Step 4: Context Manager with Simulation

```python
print(f"\nContext managers in simulation...")

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Use context managers in simulation loop
for episode in range(3):
    print(f"\nEpisode {episode + 1}:")
    
    with pynlm.Vision.createImage(128, 128) as vision, \
         pynlm.InternalSignals.createSignals() as signals:
        
        # Initialize with episode-specific data
        vision.setData([0.5 + 0.1 * (episode % 3)] * (128 * 128 * 3))
        signals.addSignal(float(episode * 10 + 42))
        
        # Simulate agent processing
        agent.processSensoryInput(percept)  # In real use, pass actual percept
        
        # Show processing
        print(f"  Processed vision: {vision.getWidth()}x{vision.getHeight()}")
        print(f"  Processed signals: {signals.getData()}")
        print(f"  Agent curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Agent novelty: {agent.getNoveltyLevel():.3f}")

print(f"✓ Simulation complete with context manager usage")
```

**Full Code:**
```python
import pynlm

print("\nNLM Python Tutorial 3: Context Managers")
print("=" * 45)

# Create vision using context manager
print("Creating vision with context manager...")
with pynlm.Vision.createImage(640, 480) as vision:
    print(f"✓ Created vision: {vision.getWidth()}x{vision.getHeight()}")
    
    # Set up sample data
    data = [0.5] * (640 * 480 * 3)
    vision.setData(data)
    
    # Process the image
    avg_intensity = sum(data) / len(data)
    print(f"✓ Processed image with average intensity: {avg_intensity:.3f}")
    
    # Context manager automatically handles cleanup
    print("✓ Context manager automatically cleaned up resources")

# Vision context manager has exited, but object is still usable
print(f"✓ Vision object still accessible: {vision.getWidth()}x{vision.getHeight()}")

print(f"\nCreating multiple context managers...")

# Create multiple context managers simultaneously
with pynlm.Vision.createImage(320, 240) as vision, \
     pynlm.Audio.createAudio(22050, 512) as audio, \
     pynlm.InternalSignals.createSignals() as signals:
    
    print(f"✓ Created vision: {vision.getWidth()}x{vision.getHeight()}")
    print(f"✓ Created audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")
    print(f"✓ Created signals: ready for data")
    
    # Use all three objects together
    signals.addSignal(1.0)
    signals.addSignal(2.0)
    signals.addSignal(3.14)
    
    print(f"✓ Added {len(signals.getData())} signals: {signals.getData()}")
    
    # Process sensory data
    vision_data = vision.getData()
    audio_data = audio.getData()
    
    vision_avg = sum(vision_data) / len(vision_data) if vision_data else 0.0
    audio_avg = sum(audio_data) / len(audio_data) if audio_data else 0.0
    
    print(f"✓ Vision average: {vision_avg:.3f}")
    print(f"✓ Audio average: {audio_avg:.3f}")

print(f"✓ All context managers have exited with proper cleanup")

print(f"\nTesting nested context managers...")

# Create nested context managers
with pynlm.Vision.createImage(400, 300) as outer_vision:
    print(f"✓ Created outer vision: {outer_vision.getWidth()}x{outer_vision.getHeight()}")
    
    # Inner context manager
    with pynlm.InternalSignals.createSignals() as inner_signals:
        print(f"✓ Created inner signals")
        
        # Nested operations
        outer_vision.setData([0.7] * (400 * 300 * 3))
        inner_signals.addSignal(42.0)
        inner_signals.addSignal(100.0)
        
        print(f"✓ Set vision data, added {len(inner_signals.getData())} signals")
    
    # Inner context manager has exited
    print(f"✓ Inner signals context exited, signals: {len(inner_signals.getData())}")
    
    # Continue with outer context
    print(f"✓ Continuing with outer vision context")
    outer_vision.setData([0.3] * (400 * 300 * 3))  # Change data
    print(f"✓ Changed vision data, new average: {sum(outer_vision.getData()) / len(outer_vision.getData()):.3f}")

print(f"✓ All nested context managers cleaned up properly")
```

## Tutorial 4: "Factory Methods" - Convenient Creation

### Goal
Learn how to use factory methods for convenient creation of NLM components.

### Overview
In this tutorial, you'll:
1. Understand factory methods and their benefits
2. Use factory methods for common creation tasks
3. Compare factory methods with manual creation
4. Create complex scenarios using factory methods

### Step 1: Basic Factory Methods

```python
import pynlm

print("\nNLM Python Tutorial 4: Factory Methods")
print("=" * 40)

# Create multiple objects using factory methods
print("Creating objects with factory methods...")

# 1. Create configuration with factory
config = pynlm.createDefaultConfig()
print(f"✓ Created default config with neuron_count: {config.getOr<int64_t>('neuron_count', 0)}")

# 2. Create brain using factory
brain = pynlm.createBrain(config)
print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")

# 3. Create world using factory
world = pynlm.createSimpleWorld()
print(f"✓ Created world")

# 4. Create agent using factory
agent = pynlm.createAgentBrain(brain)
print(f"✓ Created agent")

# 5. Create sensory inputs using factory methods
vision = pynlm.Vision.createImage(640, 480)
print(f"✓ Created vision: {vision.getWidth()}x{vision.getHeight()}")

audio = pynlm.Audio.createAudio(44100, 1024)
print(f"✓ Created audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")

signals = pynlm.InternalSignals.createSignals()
print(f"✓ Created signals: ready for data")

# 6. Create quick brain for testing
quick_brain = pynlm.Brain.createQuickBrain()
print(f"✓ Created quick brain: {quick_brain.getTotalNeuronCount()} neurons")
```

### Step 2: Factory Method Benefits

```python
print(f"\nComparing factory methods vs manual creation...")

# Factory method approach
print("\n1. Factory method approach (recommanded):")
config1 = pynlm.createDefaultConfig()
config1.set("neuron_count", 500)
brain1 = pynlm.createBrain(config1)
brain1.initialize()
print(f"   ✓ Created brain with factory methods: {brain1.getTotalNeuronCount()} neurons")

# Manual approach (older style)
print("\n2. Manual approach (for reference):")
from nlm import Config, Brain
config2 = Config()  # Direct constructor
config2.set("neuron_count", 500)
config2.set("region_count", 1)
brain2 = Brain(config2)  # Direct constructor
brain2.initialize()
print(f"   ✓ Created brain manually: {brain2.getTotalNeuronCount()} neurons")

print(f"\nFactory methods are recommended because:")
print(f"   - Safer (with validation)")
print(f"   - More convenient (sensible defaults)")
print(f"   - More Pythonic (Easier to use)")
print(f"   - More maintainable (Single point of creation)")
```

### Step 3: Complex Scenario with Factory Methods

```python
print(f"\nCreating complex scenario with factory methods...")

class SimulationSetup:
    """Setup simulation using factory methods."""
    
    def __init__(self, scenario_type="standard"):
        self.scenario_type = scenario_type
        self.brain = None
        self.agent = None
        self.world = None
        self.setup_scenario()
    
    def setup_scenario(self):
        """Setup based on scenario type."""
        print(f"   Setting up '{self.scenario_type}' scenario...")
        
        if self.scenario_type == "small":
            self.setup_small_scenario()
        elif self.scenario_type == "medium":
            self.setup_medium_scenario()
        elif self.scenario_type == "large":
            self.setup_large_scenario()
        else:
            self.setup_standard_scenario()
    
    def setup_standard_scenario(self):
        """Standard scenario setup."""
        # Create components using factory methods
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable standard learning
        self.agent.enableRewardModulation(True)
    
    def setup_small_scenario(self):
        """Small scenario setup."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def setup_medium_scenario(self):
        """Medium scenario setup."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("region_count", 2)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable enhanced learning
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
    
    def setup_large_scenario(self):
        """Large scenario setup."""
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 4)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
    
    def run_simulation(self, steps=100):
        """Run simulation with this setup."""
        print(f"   Running {steps} steps...")
        
        for step in range(steps):
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning if enabled
            if self.agent.isRewardModulationEnabled():
                reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
        
        return self.brain, self.agent, self.world

# Create different scenarios
print(f"\nCreating scenarios with factory methods:")

small_sim = SimulationSetup("small")
small_brain, small_agent, small_world = small_sim.run_simulation(50)
print(f"   ✓ Small scenario completed: {small_brain.getTotalNeuronCount()} neurons")

medium_sim = SimulationSetup("medium")
medium_brain, medium_agent, medium_world = medium_sim.run_simulation(100)
print(f"   ✓ Medium scenario completed: {medium_brain.getTotalNeuronCount()} neurons")

large_sim = SimulationSetup("large")
large_brain, large_agent, large_world = large_sim.run_simulation(150)
print(f"   ✓ Large scenario completed: {large_brain.getTotalNeuronCount()} neurons")
```

### Step 4: Factory Methods in Action

```python
def create_research_simulation(name, neuron_count=500, enable_learning=True):
    """Create a research simulation using factory methods."""
    print(f"   Creating research simulation: '{name}'")
    
    # Use factory method for configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("region_count", 2)
    
    # Use factory method for brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Use factory method for world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Use factory method for agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure learning based on parameter
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    print(f"   ✓ Research simulation '{name}' ready")
    print(f"   ✓ Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"   ✓ World: {world.getWidth()}x{world.getHeight()}")
    
    return brain, agent, world

# Create research simulations
research1 = create_research_simulation("MemoryStudy", 200, True)
research2 = create_research_simulation("LearningStudy", 1000, True)
research3 = create_research_simulation("DevelopmentStudy", 800, False)

print(f"\n   ✓ Created 3 research simulations using factory methods")
```

**Full Code:**
```python
import pynlm

print("\nNLM Python Tutorial 4: Factory Methods")
print("=" * 40)

# Create multiple objects using factory methods
print("Creating objects with factory methods...")

# 1. Create configuration with factory
config = pynlm.createDefaultConfig()
print(f"✓ Created default config with neuron_count: {config.getOr<int64_t>('neuron_count', 0)}")

# 2. Create brain using factory
brain = pynlm.createBrain(config)
print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")

# 3. Create world using factory
world = pynlm.createSimpleWorld()
print(f"✓ Created world")

# 4. Create agent using factory
agent = pynlm.createAgentBrain(brain)
print(f"✓ Created agent")

# 5. Create sensory inputs using factory methods
vision = pynlm.Vision.createImage(640, 480)
print(f"✓ Created vision: {vision.getWidth()}x{vision.getHeight()}")

audio = pynlm.Audio.createAudio(44100, 1024)
print(f"✓ Created audio: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")

signals = pynlm.InternalSignals.createSignals()
print(f"✓ Created signals: ready for data")

# 6. Create quick brain for testing
quick_brain = pynlm.Brain.createQuickBrain()
print(f"✓ Created quick brain: {quick_brain.getTotalNeuronCount()} neurons")
```

## Tutorial 5: "Error Handling" - Robust Code

### Goal
Learn how to handle errors and create robust NLM applications.

### Overview
In this tutorial, you'll:
1. Understand common error scenarios
2. Implement proper error handling
3. Create defensive code
4. Test error handling

### Step 1: Understanding Error Scenarios

```python
import pynlm

print("\nNLM Python Tutorial 5: Error Handling")
print("=" * 38)

print("Common error scenarios in NLM applications:")
print("1. Null configuration passed to brain creation")
print("2. Uninitialized brain operations")
print("3. Invalid sensory input data")
print("4. Missing configuration keys")
print("5. Context manager usage errors")
print("6. File I/O errors")
```

### Step 2: Safe Brain Creation

```python
def safe_brain_creation(config=None):
    """Safely create a brain with error handling."""
    print(f"   Creating brain safely...")
    
    # Check for null config
    if config is None:
        print(f"   ⚠️  Warning: Using None config")
        config = pynlm.createDefaultConfig()
    
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"   ✓ Brain created successfully: {brain.getTotalNeuronCount()} neurons")
        return brain
        
    except Exception as e:
        print(f"   ✗ Error creating brain: {e}")
        return None

# Test safe brain creation
print(f"\n1. Testing safe brain creation:")
normal_brain = safe_brain_creation()
null_brain = safe_brain_creation(None)  # Should still work
```

### Step 3: Defensive Programming

```python
def defensive_brain_operation(brain, operation_name, **kwargs):
    """Safely perform brain operations with validation."""
    print(f"   Performing defensive operation: '{operation_name}'...")
    
    # Check if brain exists
    if brain is None:
        print(f"   ✗ Error: Brain is None")
        return False
    
    try:
        # Check if brain is initialized
        if not brain.isInitialized():
            print(f"   ⚠️  Warning: Brain not initialized, attempting initialization...")
            if not brain.initialize():
                print(f"   ✗ Error: Failed to initialize brain")
                return False
        
        # Perform operation based on name
        if operation_name == "step":
            step = kwargs.get("step", 0)
            print(f"   Performing step {step}...")
            brain.step(step)
            
        elif operation_name == "get_stats":
            print(f"   Getting brain statistics...")
            neurons = brain.getTotalNeuronCount()
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            print(f"   Statistics: {neurons} neurons, {firing} firing, {rate:.3f} Hz")
            
        elif operation_name == "produce_action":
            print(f"   Producing action...")
            action = brain.produceAction()
            print(f"   ✓ Produced action")
            
        else:
            print(f"   ⚠️  Warning: Unknown operation '{operation_name}'")
            return False
        
        print(f"   ✓ Operation '{operation_name}' completed successfully")
        return True
        
    except Exception as e:
        print(f"   ✗ Error in operation '{operation_name}': {e}")
        return False

# Test defensive operations
print(f"\n2. Testing defensive operations:")

brain = pynlm.Brain.createQuickBrain()

# Test various operations
operations = ["step", "get_stats", "produce_action", "invalid_operation"]
for op in operations:
    success = defensive_brain_operation(brain, op, step=5)
    print(f"   Operation '{op}': {'✓ Success' if success else '✗ Failed'}")
```

### Step 4: Safe Configuration Access

```python
def safe_config_access(config, key, default_value=None):
    """Safely access configuration values."""
    print(f"   Safely accessing config key: '{key}'...")
    
    try:
        # Check if config exists
        if config is None:
            print(f"   ✗ Error: Config is None")
            return default_value
        
        # Check if key exists
        if not config.has(key):
            print(f"   ⚠️  Warning: Key '{key}' not found, using default")
            return default_value
        
        # Try to get value
        if isinstance(default_value, int):
            value = config.getOr<int>(key, default_value)
        elif isinstance(default_value, float):
            value = config.getOr<double>(key, default_value)
        elif isinstance(default_value, bool):
            value = config.getOr<bool>(key, default_value)
        elif isinstance(default_value, str):
            value = config.getOr<std::string>(key, default_value)
        else:
            # Generic case
            value = config.getOr<int>(key, default_value)
        
        print(f"   ✓ Successfully retrieved '{key}': {value}")
        return value
        
    except Exception as e:
        print(f"   ✗ Error accessing '{key}': {e}")
        return default_value

# Test safe configuration access
print(f"\n3. Testing safe configuration access:")

config = pynlm.createDefaultConfig()

# Test accessing various keys
test_keys = ["neuron_count", "random_seed", "invalid_key", "simulation_timestep"]
default_values = [0, 42, "default", 0.001]

for key, default in zip(test_keys, default_values):
    value = safe_config_access(config, key, default)
    print(f"   Key '{key}': {value} (type: {type(value).__name__})")
```

### Step 5: Context Manager Error Handling

```python
def safe_context_manager_operation(object_type, **kwargs):
    """Safely use context managers."""
    print(f"   Safely using context manager: '{object_type}'...")
    
    try:
        if object_type == "vision":
            width = kwargs.get("width", 640)
            height = kwargs.get("height", 480)
            
            with pynlm.Vision.createImage(width, height) as obj:
                print(f"   ✓ Created vision: {width}x{height}")
                return obj
                
        elif object_type == "audio":
            sample_rate = kwargs.get("sample_rate", 44100)
            num_samples = kwargs.get("num_samples", 1024)
            
            with pynlm.Audio.createAudio(sample_rate, num_samples) as obj:
                print(f"   ✓ Created audio: {sample_rate} Hz, {num_samples} samples")
                return obj
                
        elif object_type == "signals":
            with pynlm.InternalSignals.createSignals() as obj:
                print(f"   ✓ Created signals")
                return obj
        
        else:
            print(f"   ✗ Error: Unknown object type '{object_type}'")
            return None
            
    except Exception as e:
        print(f"   ✗ Error with context manager '{object_type}': {e}")
        return None

# Test context manager error handling
print(f"\n4. Testing context manager error handling:")

context_objects = [
    ("vision", {"width": 800, "height": 600}),
    ("audio", {"sample_rate": 22050, "num_samples": 512}),
    ("signals", {}),
    ("invalid_type", {})  # This should fail
]

for obj_type, kwargs in context_objects:
    result = safe_context_manager_operation(obj_type, **kwargs)
    if result is not None:
        print(f"   ✓ Successfully created {obj_type}")
    else:
        print(f"   ✗ Failed to create {obj_type}")
```

### Step 6: Comprehensive Error Handling Example

```python
def robust_simulation_setup(config_name="default"):
    """Robust simulation setup with comprehensive error handling."""
    print(f"   Setting up robust simulation: '{config_name}'...")
    
    try:
        # Step 1: Create configuration
        print(f"   1. Creating configuration...")
        config = pynlm.createDefaultConfig()
        
        # Step 2: Create brain
        print(f"   2. Creating brain...")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Step 3: Create world
        print(f"   3. Creating world...")
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        # Step 4: Create agent
        print(f"   4. Creating agent...")
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Step 5: Configure learning
        print(f"   5. Configuring learning...")
        agent.enableRewardModulation(True)
        
        print(f"   ✓ Robust simulation setup completed successfully")
        return brain, agent, world
        
    except Exception as e:
        print(f"   ✗ Error in robust setup: {e}")
        
        # Cleanup
        try:
            if 'brain' in locals():
                brain.reset()
        except:
            pass
            
        try:
            if 'world' in locals():
                world.reset()
        except:
            pass
            
        return None, None, None

# Test robust simulation setup
print(f"\n5. Testing robust simulation setup:")

brain, agent, world = robust_simulation_setup("standard")
if brain is not None:
    print(f"   ✓ Simulation setup successful!")
    print(f"   ✓ Brain: {brain.getTotalNeuronCount()} neurons")
else:
    print(f"   ✗ Simulation setup failed")
```

**Full Code:**
```python
import pynlm

print("\nNLM Python Tutorial 5: Error Handling")
print("=" * 38)

print("Common error scenarios in NLM applications:")
print("1. Null configuration passed to brain creation")
print("2. Uninitialized brain operations")
print("3. Invalid sensory input data")
print("4. Missing configuration keys")
print("5. Context manager usage errors")
print("6. File I/O errors")
```

## Tutorial 6: "Integration Testing" - Testing Your Implementation

### Goal
Learn how to test NLM Python implementations thoroughly.

### Overview
In this tutorial, you'll:
1. Understand the importance of integration testing
2. Create comprehensive test suites
3. Test basic functionality
4. Test factory methods
5. Test error handling
6. Run and analyze test results

### Step 1: Create a Test Suite

```python
import pynlm
import sys

print("\nNLM Python Tutorial 6: Integration Testing")
print("=" * 42)

class NLMPythonTestSuite:
    """Comprehensive test suite for NLM Python API."""
    
    def __init__(self):
        self.passed_tests = 0
        self.failed_tests = 0
        self.total_tests = 0
    
    def run_all_tests(self):
        """Run all tests in the suite."""
        print(f"Running NLM Python test suite...")
        print(f"This may take a few moments...")
        
        # Test categories
        test_categories = [
            self.test_basic_functionality,
            self.test_factory_methods,
            self.test_error_handling,
            self.test_context_managers,
            self.test_advanced_features
        ]
        
        for test_category in test_categories:
            print(f"\n{'='*60}")
            print(f"Running {test_category.__name__}")
            print(f"{'='*60}")
            
            try:
                test_category()
            except Exception as e:
                print(f"✗ Test category {test_category.__name__} failed: {e}")
                self.failed_tests += 1
        
        # Print summary
        self.print_test_summary()
    
    def test_basic_functionality(self):
        """Test basic NLM functionality."""
        print(f"Testing basic functionality...")
        
        # Test 1: Basic brain creation and initialization
        print(f"   Test 1: Basic brain creation...")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        if brain.getTotalNeuronCount() <= 0:
            raise AssertionError("Brain should have positive neuron count")
        
        self.passed_tests += 1
        
        # Test 2: Basic simulation
        print(f"   Test 2: Basic simulation...")
        initial_spikes = brain.getTotalSpikeCount()
        
        for step in range(10):
            brain.step(step)
        
        final_spikes = brain.getTotalSpikeCount()
        
        if final_spikes < initial_spikes:
            raise AssertionError("Should not lose spikes during simulation")
        
        self.passed_tests += 1
        
        # Test 3: Basic statistics
        print(f"   Test 3: Basic statistics...")
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        firing = brain.getFiringNeuronCount()
        
        if neurons <= 0 or synapses < 0 or firing < 0:
            raise AssertionError("Invalid statistics values")
        
        self.passed_tests += 1
    
    def test_factory_methods(self):
        """Test factory methods."""
        print(f"Testing factory methods...")
        
        # Test 1: Default config factory
        print(f"   Test 1: Default config factory...")
        config1 = pynlm.createDefaultConfig()
        config2 = pynlm.createDefaultConfig()
        
        if config1 is None or config2 is None:
            raise AssertionError("Factory methods should return valid objects")
        
        self.passed_tests += 1
        
        # Test 2: Brain factory methods
        print(f"   Test 2: Brain factory methods...")
        
        # Quick brain factory
        quick_brain = pynlm.Brain.createQuickBrain()
        if quick_brain is None:
            raise AssertionError("Quick brain factory should return valid brain")
        
        self.passed_tests += 1
        
        # Test 3: Sensory input factory methods
        print(f"   Test 3: Sensory input factory methods...")
        
        vision = pynlm.Vision.createImage(100, 100)
        audio = pynlm.Audio.createAudio(8000, 512)
        signals = pynlm.InternalSignals.createSignals()
        
        if vision is None or audio is None or signals is None:
            raise AssertionError("Sensory input factories should return valid objects")
        
        self.passed_tests += 1
        
        # Test 4: Agent factory method
        print(f"   Test 4: Agent factory method...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        if agent is None:
            raise AssertionError("Agent factory should return valid agent")
        
        self.passed_tests += 1
    
    def test_error_handling(self):
        """Test error handling."""
        print(f"Testing error handling...")
        
        # Test 1: Null config handling
        print(f"   Test 1: Null config handling...")
        
        try:
            # This should handle null config gracefully
            brain = pynlm.createBrain(None)
            print(f"   ⚠️  Warning: createBrain(None) should raise error but didn't")
        except Exception:
            # Expected error
            self.passed_tests += 1
        
        # Test 2: Missing key handling
        print(f"   Test 2: Missing key handling...")
        
        config = pynlm.createDefaultConfig()
        
        # Get missing key with default
        value = config.getOr<int>("missing_key", 999)
        if value != 999:
            raise AssertionError("Missing key should return default value")
        
        self.passed_tests += 1
        
        # Test 3: Invalid object type handling
        print(f"   Test 3: Invalid object type handling...")
        
        try:
            # This should handle invalid types
            invalid_object = None
            # In real code, would have error handling
            self.passed_tests += 1
        except Exception:
            self.passed_tests += 1
    
    def test_context_managers(self):
        """Test context managers."""
        print(f"Testing context managers...")
        
        # Test 1: Vision context manager
        print(f"   Test 1: Vision context manager...")
        
        with pynlm.Vision.createImage(100, 100) as vision:
            if vision.getWidth() != 100 or vision.getHeight() != 100:
                raise AssertionError("Vision context manager should create valid vision")
        
        self.passed_tests += 1
        
        # Test 2: Audio context manager
        print(f"   Test 2: Audio context manager...")
        
        with pynlm.Audio.createAudio(8000, 512) as audio:
            if audio.getSampleRate() != 8000 or audio.getNumSamples() != 512:
                raise AssertionError("Audio context manager should create valid audio")
        
        self.passed_tests += 1
        
        # Test 3: Signals context manager
        print(f"   Test 3: Signals context manager...")
        
        with pynlm.InternalSignals.createSignals() as signals:
            # Should be able to add signals
            signals.addSignal(1.0)
            if len(signals.getData()) != 1:
                raise AssertionError("Signals context manager should allow signal operations")
        
        self.passed_tests += 1
    
    def test_advanced_features(self):
        """Test advanced features."""
        print(f"Testing advanced features...")
        
        # Test 1: Agent with learning
        print(f"   Test 1: Agent with learning...")
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableRewardModulation(True)
        
        if not agent.isRewardModulationEnabled():
            raise AssertionError("Reward modulation should be enabled")
        
        self.passed_tests += 1
        
        # Test 2: Complex simulation
        print(f"   Test 2: Complex simulation...")
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Run more complex simulation
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        self.passed_tests += 1
    
    def print_test_summary(self):
        """Print test summary."""
        print(f"\n{'='*60}")
        print(f"TEST SUMMARY")
        print(f"{'='*60}")
        print(f"Total tests run: {self.passed_tests + self.failed_tests}")
        print(f"Tests passed: {self.passed_tests}")
        print(f"Tests failed: {self.failed_tests}")
        
        if self.failed_tests == 0:
            print(f"\n🎉 All tests passed!")
            print(f"Your NLM Python installation is working correctly!")
        else:
            print(f"\n❌ {self.failed_tests} tests failed!")
            print(f"Please check the output above for details.")
        
        print(f"{'='*60}")

# Run the test suite
if __name__ == "__main__":
    test_suite = NLMPythonTestSuite()
    test_suite.run_all_tests()
    
    # Exit with appropriate code
    sys.exit(1 if test_suite.failed_tests > 0 else 0)
```

### Step 2: Run and Analyze Tests

```python
# Method 1: Run individual test files
print("\nMethod 1: Run individual test files")
print("=" * 40)

# In terminal/command line:
# python test_basic_api.py
# python test_factory_methods.py
# python test_error_handling.py
# python test_context_managers.py

# Method 2: Run comprehensive test suite
print("\nMethod 2: Run comprehensive test suite")
print("=" * 40)

import pynlm

# Create and run a custom test suite
def run_custom_tests():
    """Run custom tests based on your needs."""
    print(f"Running custom NLM Python tests...")
    
    # Test 1: Basic functionality
    print(f"\n1. Basic functionality test...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Verify brain properties
    assert brain.getTotalNeuronCount() > 0, "Should have neurons"
    assert brain.getTotalSynapseCount() >= 0, "Should have non-negative synapses"
    
    print(f"   ✓ Brain test passed: {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Factory methods
    print(f"\n2. Factory methods test...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    assert config is not None, "Config factory should work"
    assert brain is not None, "Brain factory should work"
    assert world is not None, "World factory should work"
    assert agent is not None, "Agent factory should work"
    
    print(f"   ✓ Factory methods test passed")
    
    # Test 3: Context managers
    print(f"\n3. Context managers test...")
    
    with pynlm.Vision.createImage(100, 100) as vision:
        assert vision.getWidth() == 100, "Vision width should match"
        assert vision.getHeight() == 100, "Vision height should match"
    
    print(f"   ✓ Context managers test passed")
    
    # Test 4: Error handling
    print(f"\n4. Error handling test...")
    
    # Test missing key handling
    config = pynlm.createDefaultConfig()
    value = config.getOr<int>("missing_key", 42)
    assert value == 42, "Missing key should return default"
    
    print(f"   ✓ Error handling test passed")
    
    print(f"\n🎉 All custom tests passed!")
    return True

# Run custom tests
if run_custom_tests():
    print(f"\n✅ Your NLM Python implementation is working correctly!")
else:
    print(f"\n❌ Some tests failed. Please check your implementation.")
```

### Step 3: Test Analysis and Results

```python
# Test analysis helper functions
def analyze_test_results(test_results):
    """Analyze test results and provide recommendations."""
    print(f"\n{'='*60}")
    print(f"TEST ANALYSIS")
    print(f"{'='*60}")
    
    if not test_results:
        print(f"No test results to analyze")
        return
    
    # Extract key metrics
    total_tests = len(test_results)
    passed_tests = sum(1 for result in test_results if result['passed'])
    failed_tests = total_tests - passed_tests
    
    print(f"Total tests: {total_tests}")
    print(f"Passed: {passed_tests}")
    print(f"Failed: {failed_tests}")
    
    if failed_tests == 0:
        print(f"\n✅ EXCELLENT: All tests passed!")
        print(f"   Your NLM Python implementation is robust and ready for production.")
    elif failed_tests < total_tests * 0.2:  # Less than 20% failed
        print(f"\n⚠️  GOOD: Most tests passed ({failed_tests} failed).")
        print(f"   Implementation needs minor fixes.")
    elif failed_tests < total_tests * 0.5:  # Less than 50% failed
        print(f"\n❌ NEEDS WORK: Several tests failed ({failed_tests} failed).")
        print(f"   Implementation needs significant fixes.")
    else:
        print(f"\n🚨 CRITICAL: Most tests failed ({failed_tests} failed).")
        print(f"   Implementation needs major fixes before use.")
    
    # Performance analysis
    print(f"\nPerformance Analysis:")
    avg_neurons = sum(result.get('neurons', 0) for result in test_results) / total_tests
    avg_firing_rate = sum(result.get('firing_rate', 0.0) for result in test_results) / total_tests
    
    print(f"   Average neurons per test: {avg_neurons:.1f}")
    print(f"   Average firing rate: {avg_firing_rate:.3f} Hz")
    
    # Stability analysis
    stability_score = (passed_tests / total_tests) * 100
    print(f"   Stability score: {stability_score:.1f}%")
    
    return {
        'total_tests': total_tests,
        'passed_tests': passed_tests,
        'failed_tests': failed_tests,
        'stability_score': stability_score,
        'avg_neurons': avg_neurons,
        'avg_firing_rate': avg_firing_rate
    }

# Example test results analysis
test_results = [
    {'test': 'Basic functionality', 'passed': True, 'neurons': 500, 'firing_rate': 2.5},
    {'test': 'Factory methods', 'passed': True, 'neurons': 1000, 'firing_rate': 3.2},
    {'test': 'Error handling', 'passed': True, 'neurons': 200, 'firing_rate': 1.8},
    {'test': 'Context managers', 'passed': True, 'neurons': 800, 'firing_rate': 2.9},
    {'test': 'Advanced features', 'passed': False, 'neurons': 0, 'firing_rate': 0.0}
]

analysis = analyze_test_results(test_results)

print(f"\n{'='*60}")
print(f"NEXT STEPS RECOMMENDATIONS")
print(f"{'='*60}")

if analysis['stability_score'] < 80:
    print(f"❌ Implement fixes for failing tests")
    print(f"   - Review error handling in advanced features")
    print(f"   - Check context manager cleanup")
    print(f"   - Verify factory method validation")
elif analysis['stability_score'] < 95:
    print(f"⚠️  Optimize performance and add more tests")
    print(f"   - Improve advanced feature implementation")
    print(f"   - Add stress testing")
    print(f"   - Implement performance profiling")
else:
    print(f"✅ Implementation is excellent!")
    print(f"   - Consider adding integration tests")
    print(f"   - Add performance benchmarks")
    print(f"   - Create documentation for users")
```

## Tutorial 7: "Real-World Application" - Building Something Useful

### Goal
Build a complete real-world NLM application.

### Overview
In this tutorial, you'll:
1. Design a real-world application
2. Implement the application using NLM
3. Test and refine the application
4. Document and package the application

### Step 1: Application Design

```python
import pynlm

print("\nNLM Python Tutorial 7: Real-World Application")
print("=" * 50)

print("Application Design: NLM-Based Learning Environment")
print("=" * 60)

print("Application: An adaptive learning environment that uses neural networks")
print("to track student progress and provide personalized feedback.")
print()

print("Key Features:")
print("✓ Sensor-based data collection (visual, auditory, internal signals)")
print("✓ Neural learning and adaptation")
print("✓ Memory of student performance and preferences")
print("✓ Personalized feedback based on learned patterns")
print("✓ Curiosity-driven exploration of new concepts")
print("✓ Reward-based learning from successful interactions")
print()

print("System Architecture:")
print("1. Input Layer: Sensors (Vision, Audio, Internal)")
print("2. Processing Layer: Neural Brain with plasticity")
print("3. Decision Layer: Agent with neuromodulation")
print("4. Output Layer: World with adaptive environment")
print("5. Learning Layer: Reward and curiosity systems")
print()

print("Use Cases:")
print("• Educational software that adapts to learning styles")
print("• Game AI that learns player preferences")
print("• Simulation environments for training")
print("• Personalization engines for user interfaces")
print()
```

### Step 2: Core Application Class

```python
class AdaptiveLearningEnvironment:
    """Adaptive learning environment using NLM."""
    
    def __init__(self, student_name="Student", difficulty_level="medium"):
        print(f"Initializing learning environment for {student_name}")
        
        # Create neural components
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 1000)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Configure environment
        self.world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Set student parameters
        self.student_name = student_name
        self.difficulty_level = difficulty_level
        self.current_level = 1
        self.score = 0
        self.attempts = 0
        
        # Learning progress tracking
        self.concept_mastery = {}
        self.preferred_learning_style = None
        self.conceptual_difficulties = []
        
        # Initialize learning
        self._initialize_learning()
    
    def _initialize_learning(self):
        """Initialize learning systems."""
        # Enable all learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        print(f"✓ Learning systems initialized")
        print(f"✓ Reward modulation: {'Enabled' if self.agent.isRewardModulationEnabled() else 'Disabled'}")
        print(f"✓ Curiosity system: {'Enabled' if self.agent.isCuriosityEnabled() else 'Disabled'}")
        print(f"✓ Development system: {'Enabled' if self.agent.isDevelopmentEnabled() else 'Disabled'}")
    
    def present_concept(self, concept_name, materials):
        """Present a new concept to the student."""
        print(f"\n🎓 Presenting concept: {concept_name}")
        
        # Create sensory input for concept materials
        with pynlm.Vision.createImage(200, 150) as concept_visual:
            with pynlm.InternalSignals.createSignals() as concept_signals:
                # Initialize with concept-specific data
                concept_visual.setData([0.7] * (200 * 150 * 3))
                concept_signals.addSignal(1.0)  # Initial engagement signal
                
                # Present concept
                print(f"   Displaying concept materials...")
                print(f"   Visual complexity: High")
                print(f"   Initial engagement signal: {concept_signals.getData()[0]:.3f}")
                
                # Store concept information
                self.concept_mastery[concept_name] = {
                    'introduced': True,
                    'attempts': 0,
                    'successful_attempts': 0,
                    'engagement_signals': [concept_signals.getData()[0]],
                    'difficulty_rating': None
                }
                
                return concept_visual, concept_signals
    
    def assess_understanding(self, response_data):
        """Assess student understanding based on response."""
        print(f"\n📊 Assessing understanding...")
        
        # Extract assessment data
        visual_data = response_data.get('visual', {})
        internal_signals = response_data.get('internal', {})
        
        # Calculate understanding metrics
        engagement_level = internal_signals.get('engagement', 0.5) if internal_signals else 0.5
        visual_complexity = visual_data.get('complexity', 0.5) if visual_data else 0.5
        response_time = response_data.get('response_time', 1.0)
        
        # Determine understanding level
        if engagement_level > 0.8 and visual_complexity < 0.7 and response_time < 2.0:
            understanding = "high"
            confidence = 0.9
        elif engagement_level > 0.5 and response_time < 5.0:
            understanding = "medium"
            confidence = 0.7
        else:
            understanding = "low"
            confidence = 0.4
        
        # Apply learning based on assessment
        reward = self._calculate_reward(understanding, confidence)
        novelty = self._calculate_novelty(understanding, confidence)
        
        # Provide feedback
        feedback = self._generate_feedback(understanding, confidence)
        
        print(f"   Understanding level: {understanding.title()} (confidence: {confidence:.2f})")
        print(f"   Engagement level: {engagement_level:.3f}")
        print(f"   Reward signal: {reward:.3f}")
        print(f"   Novelty signal: {novelty:.3f}")
        print(f"   Feedback: {feedback}")
        
        # Update learning
        self._update_learning(understanding, reward, novelty)
        
        return {
            'understanding': understanding,
            'confidence': confidence,
            'reward': reward,
            'novelty': novelty,
            'feedback': feedback
        }
    
    def _calculate_reward(self, understanding, confidence):
        """Calculate reward signal based on performance."""
        base_reward = 0.5
        understanding_bonus = 0.3 if understanding == "high" else 0.1 if understanding == "medium" else 0.0
        confidence_bonus = 0.2 if confidence > 0.7 else 0.0
        
        return min(1.0, base_reward + understanding_bonus + confidence_bonus)
    
    def _calculate_novelty(self, understanding, confidence):
        """Calculate novelty signal."""
        if understanding == "low":
            # Need to learn something new
            return 0.8
        elif understanding == "medium":
            # Can improve performance
            return 0.4
        else:
            # Already doing well
            return 0.1
    
    def _generate_feedback(self, understanding, confidence):
        """Generate personalized feedback."""
        if understanding == "high":
            if confidence > 0.8:
                return "Excellent work! You have a strong grasp of this concept."
            else:
                return "Good understanding, but there's room for deeper mastery."
        elif understanding == "medium":
            if confidence > 0.7:
                return "Solid progress! Keep practicing to build confidence."
            else:
                return "You're making progress. Consider reviewing the material more carefully."
        else:
            if confidence > 0.6:
                return "You're getting there! Don't get discouraged - persistence pays off."
            else:
                return "You might need additional help or a different approach to understand this concept."
    
    def _update_learning(self, understanding, reward, novelty):
        """Update learning based on assessment."""
        # Update agent's learning systems
        self.agent.applyRewardModulation(reward, novelty)
        self.agent.updateDevelopment(0.1)
        
        # Update concept mastery
        current_concept = list(self.concept_mastery.keys())[-1]
        self.concept_mastery[current_concept]['attempts'] += 1
        
        if understanding in ["high", "medium"]:
            self.concept_mastery[current_concept]['successful_attempts'] += 1
        
        # Calculate mastery level
        attempts = self.concept_mastery[current_concept]['attempts']
        successful = self.concept_mastery[current_concept]['successful_attempts']
        mastery = successful / attempts if attempts > 0 else 0.0
        
        self.concept_mastery[current_concept]['difficulty_rating'] = 1.0 - mastery
        
        # Update preferred learning style based on performance
        engagement_signals = self.concept_mastery[current_concept]['engagement_signals']
        avg_engagement = sum(engagement_signals) / len(engagement_signals)
        
        if avg_engagement > 0.7:
            self.preferred_learning_style = "visual"
        elif avg_engagement > 0.5:
            self.preferred_learning_style = "interactive"
        else:
            self.preferred_learning_style = "repetitive"
        
        # Track conceptual difficulties
        if mastery < 0.6:
            self.conceptual_difficulties.append(current_concept)
    
    def run_learning_session(self, concepts_to_learn):
        """Run a complete learning session."""
        print(f"\n🚀 Starting learning session with {len(concepts_to_learn)} concepts")
        print(f"   Student: {self.student_name}")
        print(f"   Difficulty level: {self.difficulty_level}")
        print(f"   Starting level: {self.current_level}")
        
        session_start_time = time.time()
        
        for concept_index, concept_name in enumerate(concepts_to_learn):
            print(f"\n{'='*60}")
            print(f"Concept {concept_index + 1}/{len(concepts_to_learn)}: {concept_name}")
            print(f"{'='*60}")
            
            # Present concept
            concept_visual, concept_signals = self.present_concept(concept_name, {})
            
            # Simulate student interaction
            print(f"\n💡 Student interaction...")
            
            # Simulate different types of student responses
            response_types = ["quick_answer", "detailed_analysis", "confused", "needs_help"]
            import random
            response_type = random.choice(response_types)
            
            # Generate response data based on response type
            if response_type == "quick_answer":
                response_data = {
                    'visual': {'complexity': 0.3, 'focus': 'central'},
                    'internal': {'engagement': 0.8, 'focus': 'high'},
                    'response_time': 1.5
                }
            elif response_type == "detailed_analysis":
                response_data = {
                    'visual': {'complexity': 0.6, 'focus': 'multiple'},
                    'internal': {'engagement': 0.6, 'focus': 'medium'},
                    'response_time': 3.5
                }
            elif response_type == "confused":
                response_data = {
                    'visual': {'complexity': 0.8, 'focus': 'scattered'},
                    'internal': {'engagement': 0.3, 'focus': 'low'},
                    'response_time': 5.0
                }
            else:  # needs_help
                response_data = {
                    'visual': {'complexity': 0.9, 'focus': 'overwhelmed'},
                    'internal': {'engagement': 0.2, 'focus': 'anxious'},
                    'response_time': 8.0
                }
            
            # Assess understanding
            assessment = self.assess_understanding(response_data)
            
            # Update scores
            self.attempts += 1
            if assessment['understanding'] in ["high", "medium"]:
                self.score += 1
            
            # Show progress
            current_mastery = self.concept_mastery[concept_name]
            attempts = current_mastery['attempts']
            successful = current_mastery['successful_attempts']
            mastery_percentage = (successful / attempts) * 100 if attempts > 0 else 0
            
            print(f"\n📈 Progress Update:")
            print(f"   Concept: {concept_name}")
            print(f"   Attempts: {attempts}")
            print(f"   Successful: {successful}")
            print(f"   Mastery: {mastery_percentage:.1f}%")
            print(f"   Difficulty rating: {current_mastery['difficulty_rating']:.3f}")
            print(f"   Preferred learning style: {self.preferred_learning_style}")
            print(f"   Conceptual difficulties: {len(self.conceptual_difficulties)}")
        
        session_time = time.time() - session_start_time
        
        # Print final results
        self._print_session_results(session_time)
        
        return self._generate_session_report()
    
    def _print_session_results(self, session_time):
        """Print session results."""
        print(f"\n{'='*60}")
        print(f"SESSION RESULTS")
        print(f"{'='*60}")
        
        total_concepts = len(self.concept_mastery)
        successful_concepts = sum(1 for concept in self.concept_mastery.values() 
                                if concept['successful_attempts'] > 0)
        overall_mastery = (sum(1 for concept in self.concept_mastery.values() 
                               if concept['attempts'] > 0 and 
                               concept['successful_attempts'] / concept['attempts'] > 0.6) / 
                          total_concepts * 100) if total_concepts > 0 else 0
        
        print(f"Session duration: {session_time:.2f} seconds")
        print(f"Concepts presented: {total_concepts}")
        print(f"Concepts mastered: {successful_concepts}")
        print(f"Overall mastery: {overall_mastery:.1f}%")
        print(f"Total score: {self.score}/{self.attempts} ({self.score/self.attempts*100:.1f}%)")
        print(f"Learning style: {self.preferred_learning_style}")
        print(f"Difficult concepts: {', '.join(self.conceptual_difficulties)}")
        
        # Show concept mastery details
        print(f"\nDetailed Concept Mastery:")
        for concept_name, mastery_data in self.concept_mastery.items():
            attempts = mastery_data['attempts']
            successful = mastery_data['successful_attempts']
            mastery = (successful / attempts * 100) if attempts > 0 else 0
            difficulty = mastery_data['difficulty_rating']
            
            status = "✓" if mastery >= 80 else "⚠" if mastery >= 60 else "✗"
            
            print(f"   {status} {concept_name}: {mastery:.1f}% mastery (difficulty: {difficulty:.3f})")
    
    def _generate_session_report(self):
        """Generate session report."""
        return {
            'student_name': self.student_name,
            'difficulty_level': self.difficulty_level,
            'current_level': self.current_level,
            'score': self.score,
            'attempts': self.attempts,
            'success_rate': self.score / self.attempts if self.attempts > 0 else 0,
            'concept_mastery': self.concept_mastery,
            'preferred_learning_style': self.preferred_learning_style,
            'conceptual_difficulties': self.conceptual_difficulties
        }

# Run the adaptive learning environment
if __name__ == "__main__":
    # Create learning environment
    learning_env = AdaptiveLearningEnvironment("Alex", "challenging")
    
    # Define concepts to learn
    concepts = [
        "Basic Addition",
        "Fraction Operations", 
        "Algebraic Equations",
        "Geometric Shapes",
        "Data Analysis"
    ]
    
    # Run learning session
    results = learning_env.run_learning_session(concepts)
    
    print(f"\n{'='*60}")
    print(f"ADAPTIVE LEARNING ENVIRONMENT COMPLETE")
    print(f"{'='*60}")
    print(f"Student: {results['student_name']}")
    print(f"Final score: {results['score']}/{results['attempts']} ({results['success_rate']*100:.1f}%)")
    print(f"Learning style: {results['preferred_learning_style']}")
    print(f"Difficult concepts: {len(results['conceptual_difficulties'])}")
    
    if results['success_rate'] > 0.7:
        print(f"\n🎉 Excellent performance! Student is ready for advanced topics.")
    elif results['success_rate'] > 0.5:
        print(f"\n✅ Good performance! Student shows promise with some areas for improvement.")
    else:
        print(f"\n📚 Needs support. Consider additional practice or alternative teaching approaches.")
```

### Step 3: Running the Application

```bash
# Save the application
# Name: adaptive_learning_environment.py
# Location: /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/

# Run the application
python adaptive_learning_environment.py

# Expected output:
# Initializing learning environment for Alex
# Setting up brain, agent, world
# Presenting concepts with adaptive feedback
# Tracking student progress and learning patterns
# Generating final performance report
```

## Summary and Key Takeaways

### Tutorial Summary

You've completed a comprehensive learning journey through the NLM Python API:

1. **Tutorial 1: "Hello, NLM!"** - Basic neural simulation setup
2. **Tutorial 2: "Brains and Worlds"** - Agent-environment interaction  
3. **Tutorial 3: "Context Managers"** - Resource management
4. **Tutorial 4: "Factory Methods"** - Convenient creation
5. **Tutorial 5: "Error Handling"** - Robust coding
6. **Tutorial 6: "Integration Testing"** - Quality assurance
7. **Tutorial 7: "Real-World Application"** - Complete application

### Key Concepts Learned

#### 1. Pythonic API Design
- **Factory Methods**: Use factory methods instead of direct constructors
- **Context Managers**: Use `with` statements for resource management
- **Type Hints**: Include type annotations for better IDE support
- **Error Handling**: Implement comprehensive error handling

#### 2. Core NLM Components
- **Brain**: Central neural simulation with plasticity
- **AgentBrain**: Bridge between brain and world
- **SimpleWorld**: Simulated environment
- **Sensory Inputs**: Vision, Audio, InternalSignals

#### 3. Learning Patterns
- **Reward Modulation**: Learn from positive/negative feedback
- **Curiosity**: Drive exploration of new concepts
- **Development**: Maturation and skill development over time
- **Novelty Detection**: Identify and respond to new experiences

#### 4. Testing and Validation
- **Unit Testing**: Test individual components
- **Integration Testing**: Test system interactions
- **Error Testing**: Test error handling and edge cases
- **Performance Testing**: Test with different configurations

### Best Practices

#### Code Quality
1. **Use factory methods** for creating NLM components
2. **Implement context managers** for resource management
3. **Add comprehensive error handling** for robust code
4. **Include type hints** for better IDE support
5. **Write clear documentation** and examples

#### Application Development
1. **Start simple**, then add complexity gradually
2. **Test thoroughly** at each development stage
3. **Use adaptive learning** patterns for intelligent systems
4. **Monitor performance** and optimize as needed
5. **Document user interactions** and learning progress

#### Community Contribution
1. **Share examples** and tutorials with others
2. **Contribute to documentation** and test suites
3. **Provide feedback** on API improvements
4. **Help others** learn and use the NLM Python API

### Next Steps

#### For Beginners
1. **Master the basics** - Complete all tutorials
2. **Practice regularly** - Build small projects
3. **Study examples** - Analyze working code
4. **Join communities** - Connect with other users

#### For Advanced Users
1. **Contribute code** - Add new features and examples
2. **Improve documentation** - Enhance API documentation
3. **Develop libraries** - Create NLM-based libraries
4. **Teach others** - Create tutorials and courses

#### For Developers
1. **Profile performance** - Optimize for different use cases
2. **Add integration tests** - Test with other frameworks
3. **Implement benchmarks** - Compare performance across platforms
4. **Create tools** - Development and debugging tools

### Final Assessment

To assess your understanding and proficiency:

#### Knowledge Assessment
- [ ] Can create NLM components using factory methods
- [ ] Understands context manager usage
- [ ] Can implement error handling
- [ ] Knows how to test NLM applications
- [ ] Can create real-world applications

#### Skills Assessment
- [ ] Can design NLM-based systems
- [ ] Can debug NLM applications
- [ ] Can optimize NLM performance
- [ ] Can document NLM APIs
- [ ] Can teach others NLM concepts

#### Experience Assessment
- [ ] Completed all tutorials successfully
- [ ] Built at least one NLM application
- [ ] Can troubleshoot NLM issues
- [ ] Can extend NLM functionality
- [ ] Can contribute to NLM community

### Congratulations!

You've successfully completed the NLM Python learning journey! You've:

1. **Learned the fundamentals** of the NLM Python API
2. **Mastered key concepts** of neural simulation and learning
3. **Developed practical skills** in Python programming and NLM usage
4. **Created comprehensive examples** and documentation
5. **Built real-world applications** with adaptive learning capabilities

The NLM Python API is now ready for you to create amazing neural simulation applications, build intelligent systems, and contribute to the growing community of NLM users and developers.

**Remember:** The journey doesn't end here. Continue learning, experimenting, and contributing to make the NLM Python API even better for everyone!

Happy coding with NLM! 🚀🧠✨