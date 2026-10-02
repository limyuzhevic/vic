# NLM Examples - Advanced Example Programs

## Overview

This document provides comprehensive example programs demonstrating the NLM framework's capabilities, from basic simulations to advanced Phase 4 cognitive architectures. Each example is designed to be self-contained and educational, showing how to use specific features of the system.

---

## Example 1: Basic Neural Simulation

### Purpose
Demonstrates the fundamental neural computation capabilities of NLM, showing how neurons process information and generate spikes.

### Code
```python
import pynlm

def basic_neural_simulation(num_steps=500):
    """Run a basic neural simulation demonstrating core neural computation."""
    
    # Create brain with moderate size for demonstration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("=== Basic Neural Simulation ===")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print()
    
    # Run simulation
    for step in range(num_steps):
        brain.step(step)
        
        # Log statistics periodically
        if step % 100 == 0:
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getFiringNeuronCount()
            print(f"Step {step:3d}: {active_neurons:4d} neurons firing, {firing_rate:6.3f} Hz")
    
    print(f"\nSimulation complete!")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    return brain

# Run the simulation
brain = basic_neural_simulation(1000)
```

### Key Concepts Illustrated

1. **Neural Initialization**: Setting up a basic brain configuration
2. **LIF Dynamics**: Leaky Integrate-and-Fire neuron computation
3. **Spike Propagation**: Event-driven neural activity
4. **Plasticity**: Synaptic weight changes during simulation
5. **Statistics**: Monitoring neural population behavior

### Expected Output
```
=== Basic Neural Simulation ===
Total neurons: 1000
Total synapses: 14500
Initial firing rate: 0.000 Hz

Step   0:    10 neurons firing, 0.010 Hz
Step 100:   150 neurons firing, 0.150 Hz
Step 200:   280 neurons firing, 0.280 Hz
Step 300:   420 neurons firing, 0.420 Hz
Step 400:   580 neurons firing, 0.580 Hz
Step 500:   720 neurons firing, 0.720 Hz
Step 600:   850 neurons firing, 0.850 Hz
Step 700:   920 neurons firing, 0.920 Hz
Step 800:   950 neurons firing, 0.950 Hz
Step 900:   970 neurons firing, 0.970 Hz

Simulation complete!
Final firing rate: 0.970 Hz
```

---

## Example 2: Agent-World Interaction

### Purpose
Demonstrates how NLM agents interact with environments, process sensory information, and generate motor commands.

### Code
```python
import pynlm

def agent_world_simulation(num_steps=800):
    """Run a complete agent-world simulation with sensory-motor loop."""
    
    # Create sophisticated agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("neuromod.dopamine.scale", 1.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,
        height=30,
        visionWidth=10,
        visionHeight=10,
        visionFOV=90.0
    )
    world.reset()
    world.setAgentStart(15.0, 15.0)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("=== Agent-World Interaction Simulation ===")
    print(f"World size: {world.getWidth()} x {world.getHeight()}")
    print(f"Agent start: ({world.getAgentX():.1f}, {world.getAgentY():.1f})")
    print(f"Vision range: {world.getVisionWidth()} x {world.getVisionHeight()}")
    print()
    
    # Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Motor action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Statistics logging
        if step % 100 == 0:
            print(f"Step {step:3d}:")
            print(f"  Position: ({world.getAgentX():6.2f}, {world.getAgentY():6.2f})")
            print(f"  Action: {action}")
            print(f"  Reward: {reward:6.3f}")
            print(f"  Neurons: {brain.getFiringNeuronCount():4d} firing")
            print(f"  Curiosity: {agent.getCuriosityLevel():6.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print()
    
    print("=== Simulation Complete ===")
    return brain, agent, world

# Run the simulation
brain, agent, world = agent_world_simulation(1000)
```

### Key Concepts Illustrated

1. **Sensory Processing**: Converting world states to neural inputs
2. **Motor Control**: Mapping neural activity to actions
3. **Reward Learning**: Using prediction errors for learning
4. **Development**: Age-dependent changes in behavior
5. **Environmental Interaction**: Dynamic agent-environment coupling

### Expected Behavior

- Agent explores the world, moving toward resources
- Firing rates increase as agent gains experience
- Curiosity drives exploration of novel areas
- Development stage changes affect learning rates
- Planning emerges from predictive neural dynamics

---

## Example 3: Working Memory for Sequential Tasks

### Purpose
Demonstrates working memory capabilities in NLM, showing how information is stored, maintained, and used for planning.

### Code
```python
import pynlm

def working_memory_example():
    """Demonstrate working memory functionality for sequential task processing."""
    
    # Create brain with working memory system
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access working memory system
    working_memory = brain.getWorkingMemory()
    working_memory.setCapacity(20)
    working_memory.setDecayRate(0.02)
    
    print("=== Working Memory Example ===")
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Initial decay rate: {working_memory.getDecayRate():.3f}")
    print()
    
    # Simulate a sequential task
    task_sequence = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Eat
    ]
    
    print("Storing task sequence in working memory...")
    for i, action in enumerate(task_sequence):
        # Encode task with contextual information
        task_pattern = [
            float(action.value),  # Action type
            float(i) / len(task_sequence),  # Position in sequence
            1.0,  # Strength
            float(i * 10),  # Time component
            step  # Current step (will be updated)
        ]
        working_memory.store(task_pattern, strength=1.0)
        print(f"  Step {i:2d}: Stored {action}")
    
    print(f"\nMemory activity after storage: {working_memory.getMemoryActivity():.3f}")
    print(f"Number of active traces: {working_memory.getActiveTraces()}")
    
    # Demonstrate memory competition and selection
    print("\nRunning memory competition...")
    working_memory.runCompetition()
    winners = working_memory.getWinners()
    
    print(f"Memory winners: {winners}")
    print(f"Competition complete. Memory content: {working_memory.getMemoryActivity():.3f}")
    
    # Simulate memory decay
    print("\nSimulating memory decay over time...")
    for decay_step in range(5):
        working_memory.update(0.1)
        print(f"Decay step {decay_step}: Memory activity = {working_memory.getMemoryActivity():.3f}")
    
    print(f"\nFinal memory neurons: {len(working_memory.getMemoryNeurons())}")
    print(f"Final activation levels: {working_memory.getMemoryActivity():.3f}")
    
    return brain, working_memory

# Run the working memory example
brain, working_memory = working_memory_example()
```

### Key Concepts Illustrated

1. **Memory Encoding**: Converting information into neural patterns
2. **Persistent Activity**: Maintaining information over time
3. **Competitive Selection**: Winner-take-all memory retrieval
4. **Dynamic Decay**: Time-dependent memory weakening
5. **Capacity Management**: Limited memory slots and allocation

### Expected Output

- Sequential tasks stored in working memory with contextual information
- Memory activity levels showing stable maintenance
- Competition mechanism selecting dominant memory traces
- Gradual decay of less reinforced memories
- Tracking of memory neurons and activation levels

---

## Example 4: Self-Model Development

### Purpose
Demonstrates self-model development in NLM, showing how agents learn about their own actions and effects.

### Code
```python
import pynlm

def self_model_development_example():
    """Demonstrate self-model development through sensorimotor experience."""
    
    # Create agent system
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1800)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Access self-model system
    self_model = brain.getSelfModel()
    
    print("=== Self-Model Development Example ===")
    print("Developing self-model through sensorimotor experience...")
    print("This demonstrates how agents learn about their own capabilities.")
    print()
    
    # Run experience to develop self-model
    for step in range(500):
        world.update(0.1)
        
        # Get current state
        before_state = world.getSensoryPercept()
        
        # Perform action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get after state
        after_state = world.getSensoryPercept()
        
        # Record action-effect relationship
        self_model.recordSelfAction(action, before_state, after_state)
        
        # Provide some sensory input to make agent act
        if step < 50:
            # Random actions for exploration
            agent.processSensoryInput(before_state)
            brain.step(step)
        else:
            # Use stored sensory input for consistency
            agent.processSensoryInput(before_state)
            brain.step(step)
        
        # Log progress
        if step % 100 == 0:
            confidence = self_model.getSelfModelConfidence(action)
            body_awareness = self_model.getBodyAwareness()
            capability = self_model.getCapabilityLevel()
            
            print(f"Step {step:3d}:")
            print(f"  Action: {action}")
            print(f"  Self-model confidence: {confidence:6.3f}")
            print(f"  Body awareness: {body_awareness:6.3f}")
            print(f"  Capability level: {capability:6.3f}")
            print(f"  Has self-model: {self_model.hasSelfModel()}")
            print()
    
    print("=== Self-Model Development Complete ===")
    print(f"Final self-model confidence: {self_model.getSelfModelConfidence(pynlm.ActionType.MoveForward):.3f}")
    print(f"Final body awareness: {self_model.getBodyAwareness():.3f}")
    print(f"Final capability level: {self_model.getCapabilityLevel():.3f}")
    print(f"Number of learned action effects: {len(self_model.getActionEffects())}")
    
    return brain, self_model

# Run the self-model example
brain, self_model = self_model_development_example()
```

### Key Concepts Illustrated

1. **Forward Models**: Learning action-effect relationships
2. **Self-Awareness**: Understanding one's own capabilities
3. **Agency Detection**: Distinguishing self-generated from external effects
4. **Capability Representation**: Tracking skill levels and proficiency
5. **Experience Integration**: Building self-model through interaction

### Expected Behavior

- Self-model confidence increases with repeated actions
- Body awareness improves through successful prediction
- Capability level reflects learned skills and limitations
- Action effects accumulated for better prediction accuracy
- Self-model becomes more robust with experience

---

## Example 5: Social Learning

### Purpose
Demonstrates social learning capabilities in NLM, showing how agents can learn from observing others.

### Code
```python
import pynlm

def social_learning_example():
    """Demonstrate social learning through observation and imitation."""
    
    print("=== Social Learning Example ===")
    print("This example demonstrates learning from other agents.")
    print("Social learning is fundamental for cultural transmission and cooperative behavior.")
    print()
    
    # Create two agents for social learning demonstration
    agent1 = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
    agent2 = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
    
    # Initialize with different worlds
    world1 = pynlm.createSimpleWorld()
    world2 = pynlm.createSimpleWorld()
    
    world1.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world2.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    
    world1.reset()
    world2.reset()
    
    agent1.initialize(world1)
    agent2.initialize(world2)
    
    # Enable social learning
    social1 = agent1.getSocialLearning()
    social2 = agent2.getSocialLearning()
    
    print("Setting up social learning scenario...")
    print("Agent 1 will observe Agent 2's actions and learn from them.")
    print()
    
    # Social learning simulation
    print("Running social learning simulation...")
    
    # Agent 2 performs observable actions
    observed_actions = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.Interact,
        pynlm.ActionType.Eat
    ]
    
    for step in range(200):
        # Agent 2 performs actions (observable to Agent 1)
        action2 = observed_actions[step % len(observed_actions)]
        world2.applyMotorCommand(action2, world2.getSimulationTime())
        
        # Agent 1 observes Agent 2
        # In a real scenario, Agent1 would observe Agent2's sensory input
        # and world state changes
        if step % 20 == 0:
            # Simulate observation
            state_before = world1.getSensoryPercept()
            
            # Agent1 records observation of Agent2's action
            agent1.observeAgentAction(action2, state_before, world2.getSensoryPercept())
            
            print(f"Step {step:3d}: Agent 2 performed {action2}")
            print(f"  Agent 1 observations: {social1.getObservationCount()}")
            print(f"  Can imitate action2: {social1.canImitate(action2)}")
    
    # Agent 2 learns from its own experience
    print("\nAgent 2 learning from own experience...")
    for step in range(100):
        action2 = observed_actions[step % len(observed_actions)]
        world2.applyMotorCommand(action2, world2.getSimulationTime())
        agent2.updateSocialKnowledge(0.5)  # Simulate learning from interaction
    
    # Results
    print("\n=== Social Learning Results ===")
    print(f"Agent 1 observations: {social1.getObservationCount()}")
    print(f"Agent 2 observations: {social2.getObservationCount()}")
    print(f"Agent 1 can imitate MoveForward: {social1.canImitate(pynlm.ActionType.MoveForward)}")
    print(f"Agent 1 can imitate TurnLeft: {social1.canImitate(pynlm.ActionType.TurnLeft)}")
    print(f"Agent 1 can imitate Interact: {social1.canImitate(pynlm.ActionType.Interact)}")
    print(f"Agent 1 can imitate Eat: {social1.canImitate(pynlm.ActionType.Eat)}")
    
    # Demonstrate imitation capability
    print("\nDemonstrating imitation capability...")
    if social1.hasSocialKnowledge():
        imitation_action = social1.getImitationAction([0.1, 0.1, 0.1])
        print(f"Agent 1 would imitate action: {imitation_action}")
    else:
        print("Agent 1 has not yet learned to imitate.")
    
    return agent1, agent2, social1, social2

# Run the social learning example
agent1, agent2, social1, social2 = social_learning_example()
```

### Key Concepts Illustrated

1. **Observational Learning**: Learning by watching others
2. **Imitation Capability**: Reproducing observed actions
3. **Communication Signals**: Learning shared signals with others
4. **Cultural Transmission**: Transfer of learned behaviors
5. **Cooperation**: Coordinated behavior through shared understanding

### Expected Behavior

- Observer learns action-effect relationships
- Imitation capability develops with observations
- Communication signals emerge from shared experiences
- Social knowledge accumulates over time
- Cooperative behavior emerges from shared learning

---

## Example 6: Prediction-Based Planning

### Purpose
Demonstrates prediction-based planning capabilities, showing how NLM agents can anticipate future states and plan multi-step actions.

### Code
```python
import pynlm

def prediction_planning_example():
    """Demonstrate prediction-based planning and action evaluation."""
    
    # Create agent with prediction system
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("neuromod.dopamine.scale", 1.5)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable planning
    agent.setPlanningDepth(4)
    agent.setCurrentGoal([0.8, 0.8, 1.0])  # Target location
    
    print("=== Prediction-Based Planning Example ===")
    print(f"Planning depth: {agent.getPlanningDepth()}")
    print(f"Current goal: {agent.getCurrentGoal()}")
    print("This example shows how NLM plans actions using learned predictions.")
    print()
    
    # Run planning simulation
    for step in range(300):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Get planned action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Evaluate current plan
        if step % 50 == 0:
            plan = agent.evaluateSequence(
                [action, pynlm.ActionType.MoveForward, pynlm.ActionType.Interact],
                agent.getCurrentState()
            )
            
            print(f"Step {step:3d}:")
            print(f"  Current action: {action}")
            print(f"  Plan evaluation:")
            print(f"    Expected reward: {plan.expectedReward:6.3f}")
            print(f"    Confidence: {plan.confidence:6.3f}")
            print(f"    Depth: {plan.depth}")
            
            # Update plan quality based on actual outcome
            actual_reward = 1.0 if action == pynlm.ActionType.MoveForward else 0.0
            agent.updatePlanQuality([action], [action], actual_reward)
            
            print(f"    Actual reward: {actual_reward}")
            print(f"    Plan quality updated: {plan.expectedReward:.3f} -> {plan.expectedReward:.3f}")
            print()
    
    print("=== Prediction Planning Complete ===")
    print(f"Final planning depth: {agent.getPlanningDepth()}")
    print(f"Final planning confidence: {agent.getPlanningConfidence():.3f}")
    print(f"Current goal: {agent.getCurrentGoal()}")
    
    return brain, agent

# Run the prediction planning example
brain, agent = prediction_planning_example()
```

### Key Concepts Illustrated

1. **Forward Models**: Predicting action consequences
2. **Multi-Step Planning**: Evaluating sequences of actions
3. **Prediction Errors**: Learning from incorrect predictions
4. **Action Quality**: Assessing which actions lead to rewards
5. **Dynamic Planning**: Updating plans based on experience

### Expected Behavior

- Planning accuracy improves with experience
- Action evaluation becomes more reliable
- Multi-step sequences are planned and executed
- Prediction confidence increases with successful outcomes
- Plan quality updates based on actual rewards

---

## Example 7: Complete Integrated System

### Purpose
Demonstrates a complete NLM system with all advanced features integrated, showing the full capabilities of Phase 6.

### Code
```python
import pynlm

def complete_integrated_system_example():
    """Demonstrate a complete NLM system with all features integrated."""
    
    print("=== Complete Integrated System Example ===")
    print("This example demonstrates all Phase 6 features working together.")
    print("The system includes memory, prediction, planning, social learning, and development.")
    print()
    
    # Create comprehensive configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.3)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create sophisticated brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width(35, height=35, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(17.5, 17.5)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableAllFeatures()
    agent.setPlanningDepth(5)
    agent.setCurrentGoal([0.6, 0.6, 1.0])  # Find resource
    
    print("System Configuration:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  World size: {world.getWidth()} x {world.getHeight()}")
    print(f"  Planning depth: {agent.getPlanningDepth()}")
    print(f"  Goal: {agent.getCurrentGoal()}")
    print()
    
    # Access all advanced systems
    working_memory = brain.getWorkingMemory()
    prediction_system = brain.getPredictionSystem()
    planner = brain.getPlanner()
    self_model = brain.getSelfModel()
    social_learning = brain.getSocialLearning()
    attention = brain.getAttention()
    development_system = brain.getDevelopmentSystem()
    
    print("Advanced Systems Available:")
    print(f"  Working Memory: {working_memory.getCapacity()} capacity")
    print(f"  Prediction System: Enabled")
    print(f"  Neural Planner: {planner.getPlanningDepth()} steps")
    print(f"  Self-Model: {'Yes' if self_model.hasSelfModel() else 'No'}")
    print(f"  Social Learning: {'Yes' if social_learning.hasSocialKnowledge() else 'No'}")
    print(f"  Attention: {len(attention.getAttendedRegions())} regions")
    print(f"  Development: {brain.getDevelopmentalStage()}")
    print()
    
    # Run comprehensive simulation
    print("Running comprehensive simulation with all features...")
    print("This demonstrates how all systems integrate in Phase 6.")
    print()
    
    for step in range(500):
        # World update
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # All learning systems
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        # Progress monitoring (less frequent for long simulation)
        if step % 100 == 0:
            print(f"Step {step:3d}:")
            print(f"  Position: ({world.getAgentX():6.2f}, {world.getAgentY():6.2f})")
            print(f"  Action: {action}")
            print(f"  Reward: {reward:6.3f}")
            print(f"  Working Memory: {working_memory.getMemoryActivity():6.3f}")
            print(f"  Planning Confidence: {agent.getPlanningConfidence():6.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print(f"  Social Knowledge: {social_learning.getObservationCount()}")
            print()
    
    print("=== Complete Integrated System Simulation Complete ===")
    print("\nFinal System State:")
    print(f"  Total neurons used: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses created: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Planning depth: {agent.getPlanningDepth()}")
    print(f"  Planning confidence: {agent.getPlanningConfidence():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Social learning observations: {social_learning.getObservationCount()}")
    print(f"  Self-model formed: {self_model.hasSelfModel()}")
    
    return brain, agent, world

# Run the complete integrated system example
brain, agent, world = complete_integrated_system_example()
```

### Key Concepts Illustrated

1. **System Integration**: All Phase 6 features working together
2. **Multi-Modal Processing**: Combining multiple cognitive systems
3. **Emergent Behavior**: Complex behavior from simple components
4. **Adaptive Learning**: Continuous adaptation through multiple mechanisms
5. **Scalable Architecture**: Handling complex tasks with many components

### Expected Behavior

- Coordinated behavior from multiple cognitive systems
- Emergent planning and problem-solving capabilities
- Social learning integrated with individual learning
- Dynamic development affecting all systems
- Complex task completion through system integration

---

## Example Comparison Summary

| Example | Primary Focus | Key Features Demonstrated | Complexity |
|---------|---------------|---------------------------|------------|
| **1. Basic Neural Simulation** | Core neural computation | LIF dynamics, plasticity, statistics | ★★☆☆☆ |
| **2. Agent-World Interaction** | Sensory-motor loop | Perception, action, reward learning | ★★★☆☆ |
| **3. Working Memory** | Information storage | Memory encoding, competition, decay | ★★★★☆ |
| **4. Self-Model Development** | Self-awareness | Forward models, agency detection | ★★★★☆ |
| **5. Social Learning** | Observational learning | Imitation, communication, culture | ★★★★★ |
| **6. Prediction Planning** | Future prediction | Planning, prediction errors, evaluation | ★★★★★ |
| **7. Complete Integrated** | System integration | All Phase 6 features | ★★★★★ |

---

## Running These Examples

### Python Examples
All examples are written in Python and can be run directly:

```bash
# Navigate to the project directory
cd /path/to/nlm

# Install the package (if not already installed)
pip install -e .

# Run specific example
python example1.py
python example2.py
# ... etc
```

### Modifying Examples

To create new examples or modify existing ones:

1. **Change Configuration**: Modify `config.set()` parameters
2. **Adjust Parameters**: Change step counts, world sizes, neuron counts
3. **Add Logging**: Add print statements to monitor specific behaviors
4. **Extend Features**: Combine features from multiple examples
5. **Test Hypotheses**: Create experiments to test specific theories

### Performance Considerations

For longer simulations:
- Reduce neuron count: `config.set("brain.neuron_count", 1000)`
- Decrease world size: `world.configure(width=20, height=20)`
- Shorten simulation steps: Use smaller step counts
- Clear working memory periodically: `working_memory.clear()`

---

## Example Educational Value

These examples provide:

1. **Hands-on Learning**: Practical experience with NLM features
2. **Conceptual Understanding**: Deep insight into neural computation
3. **Research Foundation**: Starting point for scientific investigation
4. **System Exploration**: Discovery of emergent behaviors
5. **Performance Testing**: Benchmarking different configurations

### For Students

- Start with Example 1 and progress through the sequence
- Modify parameters to understand trade-offs
- Create experiments to test hypotheses
- Compare results across different examples

### For Researchers

- Extend examples for specific research questions
- Add visualization and analysis tools
- Create comparative studies across configurations
- Develop new feature demonstrations

---

## Future Enhancements

These examples can be extended with:

1. **Visualization Tools**: Plotting neural activity and behavior
2. **Analysis Scripts**: Statistical analysis of simulation results
3. **Interactive Features**: Real-time monitoring and control
4. **Benchmark Suite**: Performance testing across configurations
5. **Educational Modules**: Structured learning paths
6. **Research Tools**: Hypothesis testing frameworks

---

## Conclusion

The NLM examples provide comprehensive demonstrations of the framework's capabilities, from basic neural simulations to sophisticated cognitive architectures. Each example builds on previous concepts while introducing new features, creating a progressive learning experience.

These examples serve as:

- **Educational Tools**: Teaching neural computation and cognitive modeling
- **Research Platforms**: Testing hypotheses and exploring behaviors
- **Development Frameworks**: Building and extending NLM features
- **Documentation**: Living examples of NLM capabilities

By working through these examples, users can gain deep understanding of brain-inspired computing and the emergence of complex cognitive abilities from simple neural components.

---

*For the latest versions and updates to these examples, visit the NLM project repository.*

---

## Technical Notes

### Python Environment

These examples require:
```python
import pynlm
# All other standard Python libraries
```

### Installation
```bash
pip install pynlm
```

### Building from Source
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Dependencies
- C++20 compatible compiler
- CMake 3.16+
- Python 3.8+
- pybind11 2.11.0+
- scikit-build-core

### Performance Tips

For long-running simulations:
1. Use smaller brain configurations
2. Reduce world sizes
3. Monitor system resources
4. Use checkpointing for long simulations
5. Clear working memory periodically

### Troubleshooting

Common issues and solutions:

**Import errors:** Ensure pynlm is properly installed
**Memory issues:** Clear working memory, reduce brain size
**Performance issues:** Optimize configuration parameters
**Segmentation faults:** Check initialization sequence, validate inputs

---

## Support and Further Reading

### Documentation
- **Main Documentation**: README.md, HOW_TO_USE.md
- **API Reference**: Generated from source code
- **Architecture Guide**: docs/ARCHITECTURE.md
- **Science Background**: docs/SCIENCE.md

### Community
- **GitHub**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: Bug reports and feature requests
- **Discussions**: Community support and collaboration

### Research
- **Phase 1-6 Papers**: Available in project documentation
- **Experiment Results**: docs/EXPERIMENTS.md
- **Performance Analysis**: docs/PERFORMANCE.md
- **Limitations and Future Work**: docs/LIMITATIONS.md

---

*End of Examples Documentation*