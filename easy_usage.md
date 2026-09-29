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
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Extended Basic Examples (50 lines)

### Error Handling Examples

```python
import pynlm

def safe_brain_operation():
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(1000):
            brain.step(step)
            
        return brain
    except Exception as e:
        print(f"Error: {e}")
        return None

brain = safe_brain_operation()
if brain:
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

### Configuration Variations

```python
import pynlm

configs = {
    "small": {"brain.neuron_count": 500},
    "medium": {"brain.neuron_count": 1000},
    "large": {"brain.neuron_count": 2000}
}

for name, params in configs.items():
    config = pynlm.createDefaultConfig()
    for key, value in params.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"{name} brain: {brain.getTotalNeuronCount()} neurons")
```

### Progress Monitoring

```python
import pynlm
import time

def monitor_simulation(brain, steps=100):
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 25 == 0:
            elapsed = time.time() - start_time
            print(f"Step {step}/{steps} - "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Time: {elapsed:.2f}s")
    
    total_time = time.time() - start_time
    print(f"Simulation complete in {total_time:.2f}s")
    return total_time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
monitor_simulation(brain)
```

---

## Intermediate Level Examples (150 lines)

### Complete Simulation with Callbacks

```python
import pynlm

class SimulationManager:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.callbacks = {}
    
    def register_callback(self, event_type, callback):
        if event_type not in self.callbacks:
            self.callbacks[event_type] = []
        self.callbacks[event_type].append(callback)
    
    def run_episode(self, num_steps):
        for step in range(num_steps):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            self.trigger_callbacks("step_complete", {
                "step": step,
                "action": action
            })
    
    def trigger_callbacks(self, event_type, data):
        if event_type in self.callbacks:
            for callback in self.callbacks[event_type]:
                callback(data)

def run_callback_simulation():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    sim_manager = SimulationManager(brain, world, agent)
    
    def log_step(data):
        print(f"Step {data['step']} completed with action {data['action']}")
    
    sim_manager.register_callback("step_complete", log_step)
    sim_manager.run_episode(50)
```

### Custom Agent Behaviors

```python
import pynlm
import random

class AdaptiveAgent:
    def __init__(self, brain):
        self.agent = pynlm.createAgentBrain(brain)
        self.exploration_rate = 0.5
    
    def adjust_exploration(self, reward_history):
        if len(reward_history) >= 10:
            avg_reward = sum(reward_history[-10:]) / 10
            if avg_reward < 0.3:
                self.exploration_rate = min(1.0, self.exploration_rate + 0.1)
            elif avg_reward > 0.8:
                self.exploration_rate = max(0.1, self.exploration_rate - 0.1)
    
    def get_action(self, world, brain, step):
        world.update(0.1)
        percept = world.getSensoryPercept()
        self.agent.processSensoryInput(percept)
        brain.step(step)
        
        if self.exploration_rate > 0.7:
            return self._random_action(world)
        else:
            return self.agent.decodeMotorCommand()
    
    def _random_action(self, world):
        actions = list(pynlm.ActionType)
        return random.choice(actions)
```

### Environment Customization

```python
import pynlm

class CustomWorld(pynlm.SimpleWorld):
    def update(self, dt):
        self.weather = "rain" if random.random() < 0.1 else "clear"
        super().update(dt)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = CustomWorld()
world.configure(width=20, height=20)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

---

## 3. Advanced Usage Examples (150 lines)

### Multi-agent Simulations

```python
import pynlm

class MultiAgentSystem:
    def __init__(self, num_agents, world):
        self.agents = []
        self.brains = []
        self.world = world
        
        for i in range(num_agents):
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            
            self.brains.append(brain)
            self.agents.append(agent)
    
    def run_cooperative_simulation(self, num_steps):
        for step in range(num_steps):
            for i, (brain, agent) in enumerate(zip(self.brains, self.agents)):
                percept = self.world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step + i * 0.1)
                action = agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            if step % 50 == 0:
                print(f"Step {step}: Cooperative simulation running")
```

### Complex World Configurations

```python
import pynlm
import random

class ComplexWorld(pynlm.SimpleWorld):
    def setup_environment(self):
        for _ in range(5):
            x = random.uniform(5, self.width-5)
            y = random.uniform(5, self.height-5)
            self.addObject(x, y, pynlm.WorldObjectType.Resource.value, value=2.0)
        
        for _ in range(3):
            x = random.uniform(10, self.width-10)
            y = random.uniform(10, self.height-10)
            self.addObject(x, y, pynlm.WorldObjectType.Hazard.value, value=0.0)
    
    def update(self, dt):
        super().update(dt)
        self.regenerate_resources()
    
    def regenerate_resources(self):
        for obj in self.getObjects():
            if obj.type == pynlm.WorldObjectType.Resource:
                obj.value = min(obj.value + dt * 0.01, 5.0)

world = ComplexWorld()
world.configure(width=25, height=25)
world.reset()

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

---

## 4. Best Practices Section (80 lines)

### Common Pitfalls and Solutions

```python
import pynlm

# Pitfall 1: Not initializing the brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # Required!

# Pitfall 2: Using uninitialized world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()  # Required!

# Pitfall 3: Not enabling learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Pitfall 4: Wrong timestep
for step in range(100):
    world.update(0.1)

# Pitfall 5: Not handling errors
try:
    brain.step(step)
except Exception as e:
    brain.step(step)
```

### Performance Optimization Tips

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)

agent.enableRewardModulation(True)
agent.enableCuriosity(False)
agent.enableStructuralPlasticity(False)
```

---

## 5. API Reference Section (80 lines)

### Main Classes and Their Methods

#### Brain Class

```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step_number)
brain.reset()
brain.getTotalNeuronCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
```

#### Config Class

```python
config = pynlm.createDefaultConfig()
config.has("brain.neuron_count")
config.set("brain.neuron_count", 1000)
```

#### AgentBrain Class

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
agent.decodeMotorCommand()
agent.getCuriosityLevel()
agent.enableRewardModulation(True/False)
```

#### SimpleWorld Class

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()
world.update(timestep)
world.setAgentStart(x, y)
world.addObject(x, y, object_type, value=1.0)
world.isValidPosition(x, y)
```

---

## 6. Troubleshooting Section (50 lines)

### Common Error Scenarios

**"Brain crashes"**

```bash
# 1. Check if you called initialize()
# Missing: brain.initialize()

# 2. Check configuration
# Too many neurons:
# config.set("brain.neuron_count", 5000)
# Try: config.set("brain.neuron_count", 500)
```

**"Agent not responding"**

```python
# 1. Check initialization
agent.initialize(world)

# 2. Check agent subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
```

**"Slow simulation"**

```python
config.set("brain.neuron_count", 500)
```

### Solutions to Specific Problems

**Problem: "Everything is 0"**

```python
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(1000):
    brain.step(i)
```

**Problem: "Agent keeps crashing"**

```python
import traceback

def safe_simulation():
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(500):
            try:
                brain.step(step)
            except:
                brain.step(step)
                
        return brain
    except Exception as e:
        print(f"Simulation failed: {e}")
        return None
```

---

## 7. Integration Examples (40 lines)

### Integrating with External Systems

```python
import pynlm

class ExternalSystemIntegration:
    def __init__(self, api_endpoint=None):
        self.api_endpoint = api_endpoint
        self.integration_data = []
    
    def save_to_external_database(self, brain, world, agent, episode_id):
        data = {
            'episode_id': episode_id,
            'timestamp': time.time(),
            'brain_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount()
            },
            'world_stats': {
                'agent_position': world.getAgentPosition()
            },
            'agent_stats': {
                'curiosity_level': agent.getCuriosityLevel()
            }
        }
        
        self.integration_data.append(data)
        print(f"Data sent to external system")
    
    def export_to_file(self, filename):
        import json
        with open(filename, 'w') as f:
            json.dump(self.integration_data, f, indent=2)
        print(f"Exported {len(self.integration_data)} records")

def example_external_integration():
    integration = ExternalSystemIntegration()
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for episode in range(3):
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        integration.save_to_external_database(brain, world, agent, episode)
    
    integration.export_to_file("simulation_results.json")
    return integration.integration_data
```