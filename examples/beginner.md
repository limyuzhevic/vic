# NLM Python Tutorial - Beginner

Welcome to the NLM Python Tutorial! This guide will help you get started with the NLM (Neural Learning Machine) Python bindings.

## Lesson 1: Creating Your First Brain

The NLM library makes it easy to create and simulate neural networks.

### What You'll Learn
- How to create a basic brain
- How to initialize and run simulation steps
- How to get basic statistics about your brain

### Getting Started

First, make sure you have pynlm installed:

```bash
pip install pynlm
```

Now let's create your first brain!

```python
import pynlm

# Create a brain with default settings
brain = pynlm.createBrain()
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

# Check results
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Your First Brain Explained

1. **`pynlm.createBrain()`** - Creates a brain with default configuration
2. **`brain.initialize()`** - Sets up the neural networks
3. **`brain.step(step)`** - Runs one simulation step with the given step number
4. **`brain.getTotalNeuronCount()`** - Returns the total number of neurons
5. **`brain.getTotalSpikeCount()`** - Returns the total number of spikes

### Try It Yourself

Open your terminal and run this example:

```python
import pynlm

print("My First NLM Brain")
print("==================")

# Create and initialize brain
brain = pynlm.createBrain()
brain.initialize()

print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")

# Run a few simulation steps
for i in range(10):
    brain.step(i)

print(f"✓ Ran 10 simulation steps")
print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
```

## Lesson 2: Working with Worlds

Now let's add a world to interact with your brain.

### What You'll Learn
- How to create and configure a world
- How to set up an agent
- How to run a complete agent simulation

### Your First World Simulation

```python
import pynlm

print("Agent in a World")
print("=================")

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
pynlm.configureWorld(world, 20, 20, 8, 8)
world.setAgentStart(1, 1)

# Initialize agent
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("✓ Agent and world created")
print("✓ Agent initialized with world")

# Run simulation
for step in range(50):
    world.update(0.1)  # Update world state
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)  # Brain gets sensory input
    
    brain.step(step)  # Neural processing
    
    action = agent.decodeMotorCommand()  # Get action from brain
    world.applyMotorCommand(action, step * 0.1)  # Apply action
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Key Concepts

- **World**: The environment where the agent lives
- **Percept**: What the agent senses from the world
- **Agent**: The interface between brain and world
- **Action**: What the agent does in response to sensory input

### Try It Yourself

Save this as `my_first_agent.py` and run it:

```python
import pynlm

def run_simulation():
    print("Running Agent Simulation")
    print("========================")
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    pynlm.configureWorld(world, 10, 10, 5, 5)
    world.setAgentStart(1, 1)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Main loop
    for step in range(30):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        print(f"Step {step:2d}: spikes={brain.getTotalSpikeCount():4d}, firing={brain.getFiringNeuronCount():3d}")
    
    print("✓ Simulation complete!")

if __name__ == "__main__":
    run_simulation()
```

## Lesson 3: Using Simplified API

Wave 5 includes a simplified beginner-friendly API that makes it even easier to get started!

### What You'll Learn
- How to use the simplified API functions
- How they compare to the standard API
- When to use which API

### Simplified API Functions

```python
import pynlm

# Simplified versions of common operations
brain = pynlm.beginner_createBrain()  # Instead of: config = pynlm.createDefaultConfig(); brain = pynlm.createBrain(config)

world = pynlm.beginner_createWorld()  # With default settings

total_spikes = pynlm.beginner_runSimulation(brain, steps=1000, verbose=False)
```

### Complete Example Using Simplified API

```python
import pynlm

print("Using Simplified Beginner API")
print("============================")

# Create brain with one line
brain = pynlm.beginner_createBrain()
brain.initialize()

print("✓ Brain created with one line of code!")

# Run simulation with built-in logging
total_spikes = pynlm.beginner_runSimulation(brain, steps=100, verbose=True)

print(f"✓ Simulation complete with {total_spikes} total spikes")
```

### When to Use Simplified vs. Standard API

**Use Simplified API When:**
- You're just getting started
- You want quick results
- You don't need advanced customization
- You're following tutorials

**Use Standard API When:**
- You need custom configuration
- You're doing advanced work
- You want full control
- You're writing production code

## Lesson 4: Error Handling

Good error handling helps you debug your programs.

### What You'll Learn
- How to handle errors gracefully
- What error messages mean
- How to prevent common mistakes

### Common Errors and Solutions

```python
import pynlm

# Good error handling
try:
    world = pynlm.safeCreateWorld(3.0, 3.0, 8, 8)  # Too small
    print("World created successfully")
except ValueError as e:
    print(f"Error: {e}")
    print("Tip: Try larger dimensions like 20x20")
```

### Error Message Examples

1. **World too small**: "World dimensions too small (min 5x5). Try larger dimensions like 20x20."
2. **Vision too large**: "Vision dimensions cannot exceed world dimensions. Reduce vision size or increase world size."
3. **Null brain**: "Brain pointer is null. Create a brain first."

## Lesson 5: Context Managers

Context managers help manage resources automatically.

### What You'll Learn
- How to use context managers
- Why they're useful
- How to create your own

### BrainSimulator Context Manager

```python
import pynlm

print("Using Context Manager")
print("======================")

# Automatic initialization and cleanup
with pynlm.BrainSimulator() as simulator:
    brain = simulator.brain
    brain.initialize()
    
    print("✓ Brain initialized automatically")
    
    # Run simulation
    simulator.run_steps(50)
    
    print("✓ Simulation complete")

print("✓ Brain reset automatically")
```

### Benefits of Context Managers

1. **Automatic cleanup**: No need to remember to reset
2. **Exception safety**: Brain is reset even if errors occur
3. **Cleaner code**: Less boilerplate
4. **Better resource management**: Prevents memory leaks

## Your First Complete Project

Let's put it all together in a complete NLM project!

### Project: "Agent in a World" Simulation

```python
import pynlm

def main():
    print("=== NLM Agent Simulation ===")
    print()
    
    # Create components
    print("1. Creating agent and world...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    pynlm.configureWorld(world, 15, 15, 7, 7)
    world.setAgentStart(1, 1)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    
    # Add some objects to the world
    from pynlm import WorldObject, WorldObjectType
    food = WorldObject(10, 10, WorldObjectType.Resource, 10.0)
    world.addObject(food)
    
    print("✓ All components created and initialized")
    
    # Run simulation
    print("2. Running simulation...")
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        if step % 20 == 0:
            print(f"   Step {step:3d}: {brain.getTotalSpikeCount():5d} spikes, "
                  f"{brain.getFiringNeuronCount():3d} firing")
    
    # Results
    print("3. Final results...")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Agent energy: {world.getAgentBody().energy:.2f}")
    print(f"   Agent health: {world.getAgentBody().health:.2f}")
    
    print()
    print("=== Simulation Complete ===")

if __name__ == "__main__":
    main()
```

### Running Your First Project

1. Save the code as `agent_simulation.py`
2. Run it: `python agent_simulation.py`
3. See the simulation results!

## Tips for Success

### 1. Start Simple
- Begin with the simplest possible code
- Add features one at a time
- Test each component before moving on

### 2. Use the Tools
- Check documentation: `help(pynlm)` in Python
- Look at examples in the `examples/` directory
- Use error messages to debug

### 3. Understand the Concepts
- What is a percept vs. action?
- Why do we need initialization?
- What does step do vs. update?

### 4. Practice Regularly
- Run examples and modify them
- Create your own small projects
- Try different configurations

## Next Steps

### For Beginners
1. Try all three example scripts
2. Modify the examples to see what changes
3. Create your own simple agent
4. Experiment with different world settings

### Advanced Topics (After completing this tutorial)
1. Read the [Advanced Tutorial](advanced.md)
2. Study the [Python API Documentation](docs/python.md)
3. Create complex agent behaviors
4. Implement custom learning rules

## Getting Help

### Documentation
- [Python API Documentation](docs/python.md)
- [Beginner Examples](examples/01_basic_brain.py)
- [Intermediate Examples](examples/02_agent_simulation.py)
- [Advanced Examples](examples/03_advanced_features.py)

### Common Issues

**Issue: "ModuleNotFoundError: No module named 'pynlm'"**
- Solution: `pip install pynlm`

**Issue: Brain creation fails**
- Solution: Check if dependencies are installed
- Use `beginner_createBrain()` for simpler creation

**Issue: World configuration errors**
- Solution: Use `safeCreateWorld()` for better error messages
- Check that dimensions are within limits

**Issue: Simulation doesn't run**
- Solution: Ensure brain is initialized before stepping
- Check that agent is properly set up

### Support Resources
- [NLM Project GitHub](https://github.com/nlm-project/nlm)
- [NLM Documentation](https://nlm.readthedocs.io)
- [PythonBindings Repository](https://github.com/nlm-project/pynlm)

## Conclusion

You've now completed the beginner tutorial! You've learned:

1. ✅ How to create and run a basic brain simulation
2. ✅ How to create and interact with a world
3. ✅ How to create and use an agent
4. ✅ How to use the simplified beginner API
5. ✅ Basic error handling
6. ✅ Context managers for resource management

You're now ready to:
- Explore the advanced examples
- Read the comprehensive API documentation
- Create your own NLM projects
- Contribute to the NLM community

### Happy Learning!

The NLM Python bindings make it easy to create intelligent agents that learn from experience. Whether you're building a simple simulation or a complex learning system, the NLM framework provides the tools you need.

**Remember:** The best way to learn is by doing. Try the examples, modify them, and create something new!

Good luck on your NLM journey!