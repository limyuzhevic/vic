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

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

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

### Pattern 6: Advanced Multi-Agent Learning

```python
# Multi-agent learning system with competing agents
def run_multi_agent_simulation(numAgents=3, numSteps=1500):
    """Run multiple competing agents in the same environment."""
    
    import pynlm
    import random
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=40, 
        height=40, 
        visionWidth=12, 
        visionHeight=12
    )
    world.setAgentStart(20.0, 20.0)
    world.reset()
    
    # Create agents with different configurations
    agents = []
    for i in range(numAgents):
        config = pynlm.createDefaultConfig()
        
        # Vary agent parameters for diversity
        if i == 0:
            # Conservative agent - slow learning, stable behavior
            config.set("brain.neuron_count", 500)
            config.set("brain.connection_probability", 0.08)
        elif i == 1:
            # Aggressive agent - fast learning, risky behavior
            config.set("brain.neuron_count", 1200)
            config.set("brain.connection_probability", 0.15)
        else:
            # Balanced agent - medium settings
            config.set("brain.neuron_count", 800)
            config.set("brain.connection_probability", 0.12)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable different feature combinations for each agent
        if i == 0:
            # Agent 0: Only reward learning (conservative)
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(False)
            agent.enableDevelopment(False)
            agent.enableCuriosity(False)
        elif i == 1:
            # Agent 1: All systems enabled (exploratory)
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
        else:
            # Agent 2: Reward + Curiosity (balanced)
            agent.enableRewardModulation(True)
            agent.enableStructuralPlasticity(False)
            agent.enableDevelopment(True)
            agent.enableCuriosity(True)
        
        agents.append((f"Agent{i+1}", agent, brain))
    
    print(f"Multi-Agent Learning Simulation with {numAgents} agents")
    print(f"Environment: {world.getWidth()}x{world.getHeight()} cells")
    print(f"Vision: {world.getSensoryInputSize() // 4} rays x {world.getSensoryInputSize() // 4} rays")
    print(f"Total steps: {numSteps}")
    print()
    
    # Track agent statistics
    all_stats = []
    
    # Main simulation loop
    for step in range(numSteps):
        # Update world
        world.update(0.1)
        
        # Get world state
        percept = world.getSensoryPercept()
        current_time = world.getSimulationTime()
        
        # Each agent processes sensory input
        for name, agent, brain in agents:
            agent.processSensoryInput(percept)
        
        # Each agent runs its brain
        for name, agent, brain in agents:
            brain.step(step)
        
        # Each agent decides on an action
        actions = []
        for name, agent, brain in agents:
            action = agent.decodeMotorCommand()
            actions.append((name, action))
        
        # Apply agent's action to world (each agent can influence the world)
        for name, agent, brain in agents:
            action = next(a for n, a in actions if n == name)
            world.applyMotorCommand(action, current_time)
        
        # Each agent receives reward and learns
        for name, agent, brain in agents:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            predicted = 0.0  # Simplified - real system would predict
            agent.applyRewardModulation(reward, predicted)
            agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step:4d}:")
            for name, agent, brain in agents:
                print(f"  {name}: "
                      f"Neurons={brain.getFiringNeuronCount():4d}, "
                      f"Spikes={brain.getTotalSpikeCount():5d}, "
                      f"RewardMod={agent.getNeuromodulationLevel():.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Energy={world.getAgentBody().energy:.1f}")
    
    # Collect statistics
    print("\n" + "="*60)
    print("MULTI-AGENT RESULTS")
    print("="*60)
    
    for name, agent, brain in agents:
        stats = {
            'agent': name,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_energy': world.getAgentBody().energy,
            'development_stage': brain.getDevelopmentalStage()
        }
        all_stats.append(stats)
        
        print(f"\n{name}:")
        print(f"  Neurons: {stats['total_neurons']}")
        print(f"  Synapses: {stats['total_synapses']}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
        print(f"  Final neuromodulation: {stats['final_curiosity']:.3f}")
        print(f"  Final curiosity: {stats['final_curiosity']:.3f}")
        print(f"  Final novelty: {stats['final_novelty']:.3f}")
        print(f"  Final energy: {stats['final_energy']:.1f}")
        print(f"  Development stage: {stats['development_stage']}")
    
    # Compare agents
    print(f"\n{'='*60}")
    print("AGENT COMPARISON")
    print(f"{'='*60}")
    
    best_learning = max(all_stats, key=lambda x: x['total_spikes'] / x['total_neurons'])
    most_stable = min(all_stats, key=lambda x: x['final_curiosity'])
    most_exploratory = max(all_stats, key=lambda x: x['final_novelty'])
    
    print(f"Most efficient learner: {best_learning['agent']} "
          f"({best_learning['total_spikes']/best_learning['total_neurons']:.3f} spikes/neuron)")
    print(f"Most stable agent: {most_stable['agent']} "
          f"(curiosity={most_stable['final_curiosity']:.3f})")
    print(f"Most exploratory agent: {most_exploratory['agent']} "
          f"(novelty={most_exploratory['final_novelty']:.3f})")
    
    return agents, world

# Run multi-agent simulation
if __name__ == "__main__":
    agents, world = run_multi_agent_simulation(numAgents=3, numSteps=500)
```

---

```python
# Simulate a complete research project with discovery and learning
import pynlm
import math
import random

class ResearchSimulation:
    """Complete research simulation demonstrating NLM's capabilities."""
    
    def __init__(self):
        self.setup_environment()
        self.run_research_phases()
    
    def setup_environment(self):
        """Initialize the research environment."""
        print("=== NLM Research Simulation ===\n")
        
        # Create a small but capable brain
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)  # Medium size brain
        config.set("brain.connection_probability", 0.08)
        config.set("brain.synapse_density", 0.15)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create world with various resources
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
        self.world.setMaxEnergy(150.0)  # Higher energy for longer experiments
        self.world.reset()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all systems for research
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Statistics tracking
        self.statistics = {
            'steps': 0,
            'resources_found': 0,
            'hazards_avoided': 0,
            'learning_events': 0,
            'development_stages': set()
        }
    
    def run_research_phases(self):
        """Run different research phases demonstrating learning capabilities."""
        print("Phase 1: Exploration Phase")
        self.exploration_phase(500)
        
        print("\nPhase 2: Discovery Phase") 
        self.discovery_phase(500)
        
        print("\nPhase 3: Optimization Phase")
        self.optimization_phase(500)
        
        print("\nPhase 4: Integration Phase")
        self.integration_phase(500)
        
        print("\n=== Research Complete ===")
        self.print_research_summary()
    
    def exploration_phase(self, steps):
        """Initial exploration of the environment."""
        print(f"Exploring {steps} steps...")
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            self.update_statistics(action)
    
    def discovery_phase(self, steps):
        """Discovering patterns and resources."""
        print(f"Discovering patterns in {steps} steps...")
        
        # Clear previous state
        agent.reset()
        world.reset()
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            
            # Use curiosity to explore new areas
            if step % 50 == 0:
                # Occasionally try different actions to explore
                if agent.getCuriosityLevel() > 0.7:
                    action = self.get_exploratory_action()
            
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward for discoveries
            reward = self.calculate_discovery_reward()
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            self.update_statistics(action)
    
    def optimization_phase(self, steps):
        """Optimizing behavior based on learned rewards."""
        print(f"Optimizing behavior in {steps} steps...")
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward prediction for better learning
            predicted_reward = self.predict_future_reward(percPet, action)
            actual_reward = self.calculate_optimization_reward(action)
            agent.applyRewardModulation(actual_reward, predicted_reward)
            
            agent.updateDevelopment(0.1)
            self.update_statistics(action)
    
    def integration_phase(self, steps):
        """Final integration of all learned behaviors."""
        print(f"Integrating all capabilities in {steps} steps...")
        
        # Enable full integration
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percPet)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Combine reward and curiosity for optimal behavior
            reward = self.calculate_integration_reward(action)
            novelty = agent.getNoveltyLevel()
            combined_reward = reward + novelty * 0.1
            
            agent.applyRewardModulation(combined_reward, 0.0)
            agent.updateDevelopment(0.1)
            
            self.update_statistics(action)
    
    def update_statistics(self, action):
        """Update research statistics."""
        self.statistics['steps'] += 1
        
        if action == pynlm.MotorCommand.Interact:
            # Check if we interacted with a resource
            percept = self.world.getSensoryPercept()
            if percept.getInternal() and len(percPet) > 0 and percPet[0] > 0:
                self.statistics['resources_found'] += 1
        
        if self.world.getAgentBody().health < 50:
            self.statistics['hazards_avoided'] += 1
        
        if action != pynlm.MotorCommand.Wait:
            self.statistics['learning_events'] += 1
        
        # Track development stage
        self.statistics['development_stages'].add(
            brain.getDevelopmentalStage()
        )
    
    def get_exploratory_action(self):
        """Get an exploratory action when curiosity is high."""
        actions = [pynlm.MotorCommand.MoveForward, 
                   pynlm.MotorCommand.MoveBackward,
                   pynlm.MotorCommand.TurnLeft,
                   pynlm.MotorCommand.TurnRight,
                   pynlm.MotorCommand.LookLeft,
                   pynlm.MotorCommand.LookRight]
        return random.choice(actions)
    
    def calculate_discovery_reward(self):
        """Calculate reward for discovery."""
        percept = self.world.getSensoryPercept()
        if percept.getInternal() and len(percPet) > 0:
            return percPet[0]
        return 0.0
    
    def predict_future_reward(self, percept, action):
        """Predict future reward (simplified)."""
        return 0.0  # Placeholder for prediction system
    
    def calculate_optimization_reward(self, action):
        """Calculate reward for optimization."""
        # More points for efficient actions
        if action in [pynlm.MotorCommand.MoveForward, pynlm.MotorCommand.MoveBackward]:
            return 1.0
        elif action in [pynlm.MotorCommand.TurnLeft, pynlm.MotorCommand.TurnRight]:
            return 0.5
        elif action == pynlm.MotorCommand.Interact:
            return 2.0  # Resources are valuable
        else:
            return 0.0
    
    def calculate_integration_reward(self, action):
        """Calculate reward for integrated behavior."""
        base_reward = self.calculate_optimization_reward(action)
        novelty_bonus = self.agent.getNoveltyLevel() * 0.5
        curiosity_bonus = self.agent.getCuriosityLevel() * 0.3
        
        return base_reward + novelty_bonus + curiosity_bonus
    
    def print_research_summary(self):
        """Print complete research summary."""
        print(f"\n{'='*50}")
        print("RESEARCH SUMMARY")
        print(f"{'='*50}")
        
        print(f"Total steps: {self.statistics['steps']}")
        print(f"Resources discovered: {self.statistics['resources_found']}")
        print(f"Hazards encountered: {self.statistics['hazards_avoided']}")
        print(f"Learning events: {self.statistics['learning_events']}")
        print(f"Development stages reached: {len(self.statistics['development_stages'])}")
        
        print(f"\n{'='*50}")
        print("KEY FINDINGS")
        print(f"{'='*50}")
        
        if self.statistics['resources_found'] > 5:
            print("✓ Agent successfully discovered and utilized resources")
        else:
            print("⚠ Limited resource discovery - agent may need more development time")
        
        if self.statistics['hazards_avoided'] > 0:
            print("✓ Agent learned to avoid hazards")
        else:
            print("⚠ No hazard avoidance detected")
        
        if self.statistics['development_stages']:
            stages = ", ".join([str(s) for s in self.statistics['development_stages']])
            print(f"✓ Agent progressed through {len(self.statistics['development_stages'])} developmental stages: {stages}")
        
        print(f"\n{'='*50}")
        print("CONCLUSION")
        print(f"{'='*50}")
        
        # Determine overall success
        success_score = (self.statistics['resources_found'] / 10.0 * 0.4 + 
                        self.statistics['hazards_avoided'] / 20.0 * 0.2 + 
                        len(self.statistics['development_stages']) / 5.0 * 0.4)
        
        print(f"Overall learning score: {success_score:.2f}/1.0")
        
        if success_score > 0.8:
            print("✓ Excellent learning performance - agent shows advanced capabilities")
        elif success_score > 0.5:
            print("⚠ Good learning performance - agent is developing well")
        else:
            print("✗ Needs more development - agent still learning basic skills")

# Run the research simulation
if __name__ == "__main__":
    research = ResearchSimulation()
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
