# NLM Advanced Usage Examples

## Advanced Agent Training

### Example 1: Curiosity-Driven Learning
```python
import pynlm
import time

print("=== Curiosity-Driven Learning Example ===\n")

# Create a brain with curiosity enabled
config = pynlm.create_preset_config("advanced")
brain = pynlm.createBrain(config)
brain.initialize()

# Create world with obstacles
world = pynlm.create_world_with_obstacles(30, 30, 15)
world.configure(30, 30, 16, 16)

# Create agent with monitoring
agent = pynlm.create_agent_with_monitoring(brain, world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Create progress tracker and learning rate scheduler
progress_tracker = pynlm.create_progress_tracker(500, verbose=True)
learning_scheduler = pynlm.create_learning_rate_scheduler(
    "cosine", 0.01, 0.001, 500
)

# Training loop with monitoring
print("Starting curiosity-driven learning...")
start_time = time.time()

for episode in range(20):
    # Run episode with exploration
    episode_history = pynlm.run_episode(agent, world, 100, enable_exploration=True)
    
    # Analyze episode results
    rewards = [h[0] for h in episode_history]
    curiosity_levels = [h[1] for h in episode_history]
    
    avg_reward = sum(rewards) / len(rewards) if rewards else 0
    avg_curiosity = sum(curiosity_levels) / len(curiosity_levels) if curiosity_levels else 0
    
    print(f"Episode {episode + 1}: Avg Reward = {avg_reward:.2f}, "
          f"Avg Curiosity = {avg_curiosity:.2f}")
    
    # Update progress
    progress_tracker.update((episode + 1) * 100)
    
    # Save checkpoint periodically
    if (episode + 1) % 5 == 0:
        checkpoint_name = f"curiosity_episode_{episode + 1}"
        pynlm.save_experiment_session(
            checkpoint_name, brain, agent, world,
            {"episode": episode + 1, "avg_reward": avg_reward, "avg_curiosity": avg_curiosity}
        )
        print(f"  Saved checkpoint: {checkpoint_name}")

# Load latest checkpoint for continued training
print("\nLoading latest checkpoint for continued training...")
pynlm.load_experiment_session("curiosity_episode_15")

# Save final model for deployment
pynlm.save_experiment_session(
    "final_curiosity_model", brain, agent, world,
    {"training_type": "curiosity_driven", "episodes": 20}
)

end_time = time.time()
print(f"\nTraining completed in {end_time - start_time:.1f} seconds!")
print("Final model saved as 'final_curiosity_model'")
```

### Example 2: Neural Network Evolution
```python
import pynlm

print("=== Neural Network Evolution Example ===\n")

# Create population of brains
population_size = 10
brains = []
agents = []
worlds = []

print("Creating initial population...")
for i in range(population_size):
    # Create diverse initial configurations
    if i == 0:
        config = pynlm.create_preset_config("minimal")
    elif i < 5:
        config = pynlm.create_preset_config("standard")
    else:
        config = pynlm.create_preset_config("advanced")
    
    # Add small random variations
    config.set("neuron_count", config.get<int64_t>("neuron_count") * (1 + (i % 3 - 1) * 0.2))
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable evolution features
    agent.enableStructuralPlasticity(True)
    agent.enableCuriosity(True)
    
    brains.append(brain)
    agents.append(agent)
    worlds.append(world)

print(f"Created population of {population_size} brains")

# Evolution loop
generation = 0
while generation < 5:
    print(f"\n=== Generation {generation + 1} ===")
    
    generation_results = []
    
    # Evaluate each brain in its environment
    for i in range(population_size):
        # Reset world and agent
        worlds[i].reset()
        
        # Run simulation
        episode_history = pynlm.run_episode(agents[i], worlds[i], 50)
        
        # Calculate fitness
        rewards = [h[0] for h in episode_history]
        curiosity_levels = [h[1] for h in episode_history]
        
        fitness = {
            "avg_reward": sum(rewards) / len(rewards) if rewards else 0,
            "max_reward": max(rewards) if rewards else 0,
            "avg_curiosity": sum(curiosity_levels) / len(curiosity_levels) if curiosity_levels else 0,
            "exploration_score": sum(1 for c in curiosity_levels if c > 0.7) / len(curiosity_levels) if curiosity_levels else 0
        }
        
        generation_results.append((i, fitness))
    
    # Select top performers for reproduction
    generation_results.sort(key=lambda x: x[1]["avg_reward"], reverse=True)
    top_performers = generation_results[:3]
    
    print("Top performers:")
    for i, fitness in top_performers:
        print(f"  Brain {i}: Avg Reward = {fitness['avg_reward']:.2f}, "
              f"Exploration = {fitness['exploration_score']:.2f}")
    
    # Create next generation
    print("\nCreating next generation...")
    new_brains = []
    new_agents = []
    new_worlds = []
    
    for i in range(population_size):
        if i < 3:  # Elite individuals (clones)
            parent_idx = top_performers[i][0]
        else:  # Offspring with mutations
            parent_idx = generation_results[i][0]
        
        # Clone parent brain with mutations
        parent_brain = brains[parent_idx]
        new_config = pynlm.createDefaultConfig()
        
        # Copy configuration with mutations
        new_config.set("neuron_count", parent_brain.getConfig().get<int64_t>("neuron_count") * 1.1)
        new_config.set("region_count", parent_brain.getConfig().get<int64_t>("region_count"))
        new_config.set("connection_probability", parent_brain.getConfig().get<double>("connection_probability") * 1.05)
        
        # Add structural mutations
        if i % 4 == 0:
            new_config.set("enable_structural_plasticity", True)
        
        new_brain = pynlm.createBrain(new_config)
        new_brain->initialize()
        
        # Apply developmental mutations
        if i % 3 == 0:
            new_brain->setDevelopmentalStage(nlm::DevelopmentalStage::Maturation)
        
        new_agents.append(pynlm.createAgentBrain(new_brain))
        new_worlds.append(pynlm.createSimpleWorld())
        new_worlds[-1]->configure(20, 20, 8, 8)
    
    brains = new_brains
    agents = new_agents
    worlds = new_worlds
    
    generation += 1

print(f"\nEvolution completed over {generation} generations!")
print("Top performing brain preserved as 'evolved_model'")
pynlm.save_experiment_session(
    "evolved_model", brains[0], agents[0], worlds[0],
    {"generation": generation, "type": "evolutionary"}
)
```

### Example 3: Reinforcement Learning with Memory
```python
import pynlm

print("=== Reinforcement Learning with Memory Example ===\n")

# Create brain with advanced memory systems
config = pynlm.create_preset_config("advanced")
config.set("enable_working_memory", True)
config.set("enable_episodic_memory", True)
config.set("enable_associative_memory", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create complex world
world = pynlm.create_world_with_obstacles(40, 40, 20)
world.configure(40, 40, 16, 16)

# Create agent with memory integration
agent = pynlm.create_agent_with_monitoring(brain, world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Training loop with memory replay
print("Starting reinforcement learning with memory...")

# Store experiences for replay
experiences = []
learning_rate_scheduler = pynlm.create_learning_rate_scheduler("linear", 0.01, 0.001, 1000)

for episode in range(50):
    worlds[0].reset()
    
    episode_experiences = []
    
    for step in range(200):
        # Get current state
        percept = world.getSensoryPercept()
        
        # Process through memory systems
        context = brain.getCurrentContext()
        
        # Get action using current policy
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        
        # Execute action
        if action:
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Store experience
            experience = {
                "state": percept,
                "action": action,
                "reward": result.reward,
                "next_state": world.getSensoryPercept(),
                "done": False,
                "curiosity": agent.getCuriosityLevel()
            }
            episode_experiences.append(experience)
            
            # Apply reward modulation with learning rate
            learning_rate = learning_rate_scheduler.get_rate(step)
            agent.applyRewardModulation(result.reward, agent.getPredictionError())
            
            # Store in episodic memory
            brain.storeMemory(f"episode_{episode}_step_{step}", {
                "action": action,
                "reward": result.reward,
                "curiosity": agent.getCuriosityLevel()
            })
    
    experiences.extend(episode_experiences)
    
    # Memory replay for consolidation
    if episode > 10 and episode % 10 == 0:
        print(f"Performing memory replay for episode {episode}...")
        brain.replayMemories(episode // 10, 0.7)
        
        # Consolidate important memories
        important_memories = brain.getImportantMemories()
        brain.consolidateMemories(important_memories, 0.8)
    
    # Retrieve and use past experiences
    if episode > 0:
        recent_experiences = experiences[-100:]  # Last 100 experiences
        if recent_experiences:
            # Use retrieved experiences to guide action selection
            retrieved_patterns = brain.retrieveSimilarExperiences(recent_experiences[-1]["state"])
            
            # Adjust action based on retrieved patterns
            if retrieved_patterns:
                pattern_reward = sum(e["reward"] for e in retrieved_patterns) / len(retrieved_patterns)
                # Bias action selection towards experienced rewards
                if pattern_reward > 0.5:
                    print(f"  Learning from past experience: positive reward pattern detected")
    
    # Display progress
    if episode % 10 == 0:
        current_stats = pynlm.get_detailed_stats(brain, agent, world)
        print(f"Episode {episode}: "
              f"Neurons: {current_stats['brain']['total_neurons']}, "
              f"Firing: {current_stats['brain']['firing_neurons']}, "
              f"Curiosity: {current_stats['agent']['curiosity_level']:.2f}, "
              f"Memory Usage: {brain.getEpisodicMemoryCount()}")

# Export learned behaviors
print("\nExporting learned behaviors...")
learned_data = {
    "experiences": experiences,
    "final_stats": pynlm.get_detailed_stats(brain, agent, world),
    "behavior_patterns": brain.getBehavioralPatterns()
}

pynlm.export_to_csv({
    "episode": [e["reward"] for e in experiences],
    "curiosity": [e["curiosity"] for e in experiences],
    "step": list(range(len(experiences)))
}, "rl_learning_data.csv")

print("RL training completed! Learned behaviors saved to 'rl_learning_data.csv'")
```

### Example 4: Multi-Agent Cooperation
```python
import pynlm

print("=== Multi-Agent Cooperation Example ===\n")

# Create multiple cooperative agents
num_agents = 4
agent_brains = []
agent_interfaces = []
shared_world = pynlm.create_world_with_obstacles(50, 50, 25)
shared_world.configure(50, 50, 20, 20)

print(f"Creating team of {num_agents} cooperative agents...")

for i in range(num_agents):
    # Create brain for each agent
    config = pynlm.create_preset_config("standard")
    config.set("neuron_count", 150 - i * 20)  # Different sizes
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(shared_world)
    
    # Enable cooperation features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Agents can share sensory information
    agent.setSensoryShare(True)
    
    agent_brains.append(brain)
    agent_interfaces.append(agent)

print("Team initialized. Starting cooperative training...")

# Team training loop
def team_coordination_step():
    """Coordinate actions among all agents"""
    # Each agent perceives the shared environment
    team_percepts = []
    for agent in agent_interfaces:
        percept = shared_world.getSensoryPercept()
        agent.processSensoryInput(percept)
        team_percepts.append(percept)
    
    # Agents can communicate through shared world state
    shared_state = {
        "agent_positions": [shared_world.getAgentBody().x for _ in agent_interfaces],
        "world_objects": shared_world.getObjects(),
        "team_rewards": []
    }
    
    # Each agent makes decisions based on team context
    team_actions = []
    for i, agent in enumerate(agent_interfaces):
        # Add team context to agent's decision making
        team_context = {
            "teammate_positions": [shared_world.getAgentBody().x for j, _ in enumerate(agent_interfaces) if j != i],
            "shared_rewards": shared_state["team_rewards"]
        }
        
        action = agent.decodeMotorCommand(team_context)
        team_actions.append(action)
    
    # Apply coordinated actions
    for i, action in enumerate(team_actions):
        if action:
            result = shared_world.applyMotorCommand(action, shared_world.getSimulationTime())
            shared_state["team_rewards"].append(result.reward)
            
            # Share rewards among agents (cooperation bonus)
            reward_share = result.reward / num_agents
            for j in range(num_agents):
                if j != i:
                    agent_interfaces[j].applyRewardModulation(reward_share, 0.1)
    
    return team_actions, shared_state

# Training loop
for episode in range(30):
    shared_world.reset()
    
    # Reset all agents
    for agent in agent_interfaces:
        agent.reset()
    
    # Coordinated training
    print(f"Episode {episode + 1}: ", end="")
    for step in range(100):
        actions, state = team_coordination_step()
        
        # Monitor team performance
        if step % 25 == 0:
            curiosity_avg = sum(a.getCuriosityLevel() for a in agent_interfaces) / num_agents
            reward_avg = sum(state["team_rewards"]) / len(state["team_rewards"]) if state["team_rewards"] else 0
            print(f"Step {step}: Curiosity={curiosity_avg:.2f}, "
                  f"Team Reward={reward_avg:.2f} | ", end="")
    
    print("Done")

# Analyze team coordination
print("\nAnalyzing team coordination...")
final_stats = {}
for i, agent in enumerate(agent_interfaces):
    stats = pynlm.get_detailed_stats(agent_brains[i], agent, shared_world)
    final_stats[f"agent_{i}"] = {
        "neurons": stats["brain"]["total_neurons"],
        "firing": stats["brain"]["firing_neurons"],
        "curiosity": stats["agent"]["curiosity_level"],
        "cooperation": agent.getCooperationLevel()
    }

print("Final team performance:")
for agent_id, stats in final_stats.items():
    print(f"  {agent_id}: Neurons={stats['neurons']}, "
          f"Firing={stats['firing']}, "
          f"Curiosity={stats['curiosity']:.2f}, "
          f"Cooperation={stats['cooperation']:.2f}")

# Save team model
print("\nSaving cooperative team model...")
pynlm.save_experiment_session(
    "cooperative_team_model", agent_brains[0], agent_interfaces[0], shared_world,
    {"team_size": num_agents, "episodes": 30, "type": "cooperative"}
)

print("Multi-agent cooperation training completed!")
```

### Example 5: Developmental Learning System
```python
import pynlm

print("=== Developmental Learning System Example ===\n")

# Create brain with complex developmental system
config = pynlm.create_preset_config("advanced")
config.set("enable_development", True)
config.set("critical_period_length", 100)
config.set("maturation_rate", 0.03)
config.set("enable_structural_plasticity", True)

brain = pynlm.createBrain(config)
brain.initialize()

print("Initial developmental state:")
initial_stats = pynlm.get_detailed_stats(brain, None, None)
print(f"  Neurons: {initial_stats['brain']['total_neurons']}")
print(f"  Developmental Stage: {initial_stats['brain']['developmental_stage']}")
print(f"  Plasticity Level: {initial_stats['brain']['plasticity_level']:.2f}")
print(f"  Excitability Level: {initial_stats['brain']['excitability_level']:.2f}")
print(f"  Learning Rate: {initial_stats['brain']['learning_rate']:.2f}")

# Developmental phases
phases = [
    {"name": "Infancy", "steps": 50, "stage": "CriticalPeriod", "plasticity_target": 1.2},
    {"name": "Childhood", "steps": 100, "stage": "Maturation", "plasticity_target": 0.8},
    {"name": "Adulthood", "steps": 150, "stage": "Adult", "plasticity_target": 0.5},
    {"name": "Aging", "steps": 100, "stage": "Aging", "plasticity_target": 0.3}
]

print("\nStarting developmental progression...")

for phase_idx, phase in enumerate(phases):
    print(f"\n=== {phase['name']} (Phase {phase_idx + 1}/{len(phases)}) ===")
    print(f"Target developmental stage: {phase['stage']}")
    print(f"Target plasticity: {phase['plasticity_target']}")
    
    # Change developmental stage
    for step in range(phase['steps']):
        # Set developmental stage based on phase
        if phase['stage'] == "CriticalPeriod":
            brain.setDevelopmentalStage(nlm::DevelopmentalStage::CriticalPeriod)
        elif phase['stage'] == "Maturation":
            brain.setDevelopmentalStage(nlm::DevelopmentalStage::Maturation)
        elif phase['stage'] == "Adult":
            brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult)
        elif phase['stage'] == "Aging":
            brain.setDevelopmentalStage(nlm::DevelopmentalStage::Aging)
        
        # Simulate brain development
        brain.step(step + phase_idx * 1000)
        
        # Monitor development
        current_plasticity = brain.getPlasticityLevel()
        current_excitability = brain.getExcitabilityLevel()
        learning_rate = brain.getLearningRate()
        
        # Show progress every 20 steps
        if step % 20 == 0:
            print(f"  Step {step}: Plasticity={current_plasticity:.2f}, "
                  f"Excitability={current_excitability:.2f}, "
                  f"Learning Rate={learning_rate:.2f}")
            
            # Check if we reached targets
            if abs(current_plasticity - phase['plasticity_target']) < 0.05:
                print(f"  Reached plasticity target!")
                break

print("\nFinal developmental state:")
final_stats = pynlm.get_detailed_stats(brain, None, None)
print(f"  Developmental Stage: {final_stats['brain']['developmental_stage']}")
print(f"  Plasticity Level: {final_stats['brain']['plasticity_level']:.2f}")
print(f"  Excitability Level: {final_stats['brain']['excitability_level']:.2f}")
print(f"  Learning Rate: {final_stats['brain']['learning_rate']:.2f}")
print(f"  Total Spikes: {final_stats['brain']['total_spikes']}")
print(f"  Average Firing Rate: {final_stats['brain']['average_firing_rate']:.2f} Hz")

# Test learned behaviors
print("\nTesting learned behaviors...")
test_config = pynlm.create_preset_config("minimal")
test_brain = pynlm.createBrain(test_config)
test_brain.initialize()

# Test pattern from development
test_patterns = [
    ("simple_pattern", 20),
    ("complex_pattern", 50),
    ("novel_pattern", 30)
]

for pattern_name, steps in test_patterns:
    print(f"  Testing {pattern_name}...")
    for step in range(steps):
        test_brain.step(step)
    
    pattern_stats = pynlm.get_detailed_stats(test_brain, None, None)
    print(f"    Result: Firing rate = {pattern_stats['brain']['average_firing_rate']:.2f} Hz")

# Save developmental model
print("\nSaving developmental model...")
pynlm.save_experiment_session(
    "developmental_model", brain, None, None,
    {"type": "developmental", "stages": len(phases), "total_steps": sum(p['steps'] for p in phases)}
)

print("Developmental learning completed! Model saved as 'developmental_model'")
```

## Advanced Python Utilities

### Example: Custom Action Types
```python
import pynlm

# Define custom action types
class CustomActionType:
    EXPLORATION = "explore"
    NAVIGATION = "navigate"
    INTERACTION = "interact"
    LEARNING = "learn"
    REST = "rest"

# Register custom actions
pynlm.registerCustomAction(CustomActionType.EXPLORATION, "Expand search area", {"radius": 5.0})
pynlm.registerCustomAction(CustomActionType.NAVIGATION, "Move to target", {"target_x": 10.0, "target_y": 10.0})
pynlm.registerCustomAction(CustomActionType.INTERACTION, "Interact with object", {"object_type": "door"})

# Use custom actions in simulation
brain = pynlm.createBrain(pynlm.create_preset_config("standard"))
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Create and use custom action
custom_action = pynlm.createCustomAction(CustomActionType.EXPLORATION, {"radius": 8.0})
world.applyCustomAction(custom_action, world.getAgentBody())
```

### Example: Data Export and Analysis
```python
import pynlm
import json

# Create comprehensive simulation
config = pynlm.create_preset_config("advanced")
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation and collect data
simulation_data = pynlm.collect_time_series(brain, agent, world, 500)

# Analyze patterns
patterns = pynlm.analyze_activity_patterns(simulation_data, "all")
print("Activity Pattern Analysis:")
for pattern_type, analysis in patterns.items():
    print(f"  {pattern_type}: {analysis}")

# Export data in multiple formats
# CSV format
pynlm.export_to_csv(simulation_data, "simulation_data.csv")

# JSON format (custom implementation needed)
json_data = {
    "simulation_parameters": {
        "neuron_count": brain.getTotalNeuronCount(),
        "duration": 500
    },
    "final_statistics": pynlm.get_detailed_stats(brain, agent, world),
    "activity_patterns": patterns
}

with open("simulation_results.json", "w") as f:
    json.dump(json_data, f, indent=2, default=str)

# Create visualization
pynlm.create_time_series_plot(simulation_data, "NLM Simulation Results", "simulation_plot.html")

print("\nData export completed!")
print("- CSV data: 'simulation_data.csv'")
print("- JSON results: 'simulation_results.json'")
print("- Plot: 'simulation_plot.html'")
```

## Performance Optimization

### Example: Batch Processing
```python
import pynlm
import time
import threading
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

print("=== Batch Processing Example ===\n")

# Create multiple brains for batch processing
num_simulations = 5
simulation_configs = []

for i in range(num_simulations):
    config = pynlm.create_preset_config("standard")
    # Vary configurations for diversity
    config.set("neuron_count", 100 + i * 50)
    config.set("region_count", 2 + i)
    simulation_configs.append(config)

print(f"Starting batch processing with {num_simulations} simulations...")

# Method 1: Thread-based batch processing (I/O bound)
def run_single_simulation_thread(config, simulation_id):
    print(f"  Simulation {simulation_id} starting (thread)...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        if action:
            world.applyMotorCommand(action, world.getSimulationTime())
    
    stats = pynlm.get_detailed_stats(brain, agent, world)
    print(f"  Simulation {simulation_id} completed. Firing rate: {stats['brain']['average_firing_rate']:.2f} Hz")
    return simulation_id, stats

# Run thread-based batch
start_time = time.time()
with ThreadPoolExecutor(max_workers=3) as executor:
    futures = []
    for i, config in enumerate(simulation_configs):
        future = executor.submit(run_single_simulation_thread, config, i)
        futures.append(future)
    
    results = []
    for future in futures:
        results.append(future.result())

thread_time = time.time() - start_time
print(f"\nThread-based batch completed in {thread_time:.1f} seconds")

# Method 2: Parallel processing (CPU-bound simulation)
def run_single_simulation_process(config, simulation_id):
    # Similar to thread version but for CPU-bound tasks
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(100):
        brain.step(step)
    
    stats = pynlm.get_detailed_stats(brain, agent, world)
    return simulation_id, stats

# Run process-based batch
start_time = time.time()
with ProcessPoolExecutor(max_workers=2) as executor:
    futures = []
    for i, config in enumerate(simulation_configs):
        future = executor.submit(run_single_simulation_process, config, i)
        futures.append(future)
    
    process_results = []
    for future in futures:
        process_results.append(future.result())

process_time = time.time() - start_time
print(f"Process-based batch completed in {process_time:.1f} seconds")

# Compare results
print("\nBatch processing comparison:")
print(f"  Thread-based: {thread_time:.1f}s")
print(f"  Process-based: {process_time:.1f}s")

# Method 3: Asynchronous processing
import asyncio
import pynlm.asyncio as async_pynlm

async def async_simulation_batch(configs):
    print(f"Running {len(configs)} simulations asynchronously...")
    
    tasks = []
    for i, config in enumerate(configs):
        task = asyncio.create_task(async_pynlm.run_simulation_async(config, i))
        tasks.append(task)
    
    results = await asyncio.gather(*tasks)
    return results

# Run async batch
start_time = time.time()
loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)
try:
    async_results = loop.run_until_complete(async_simulation_batch(simulation_configs))
finally:
    loop.close()

async_time = time.time() - start_time
print(f"\nAsync batch completed in {async_time:.1f} seconds")

print(f"\nBatch processing comparison:")
print(f"  Thread-based: {thread_time:.1f}s")
print(f"  Process-based: {process_time:.1f}s")
print(f"  Async-based: {async_time:.1f}s")

# Choose the best method for different scenarios
print("\nRecommendations:")
print("  - For I/O operations: Thread-based processing")
print("  - For CPU-intensive tasks: Process-based processing")
print("  - For network/API operations: Async processing")
```

### Example: Real-time Monitoring
```python
import pynlm
import time
import threading
from datetime import datetime

print("=== Real-time Monitoring Example ===\n")

# Create monitoring dashboard
def create_monitoring_dashboard():
    dashboard = {
        "start_time": datetime.now(),
        "metrics": {
            "firing_rate": [],
            "curiosity_level": [],
            "simulation_time": [],
            "neuron_count": []
        },
        "alerts": [],
        "performance": {}
    }
    return dashboard

# Real-time monitoring callback
def monitoring_callback(step, brain, agent, world, dashboard):
    timestamp = datetime.now()
    
    # Collect metrics
    dashboard["metrics"]["firing_rate"].append(brain.getAverageFiringRate())
    if agent:
        dashboard["metrics"]["curiosity_level"].append(agent.getCuriosityLevel())
    if world:
        dashboard["metrics"]["simulation_time"].append(world.getSimulationTime())
    
    dashboard["metrics"]["neuron_count"].append(brain.getTotalNeuronCount())
    
    # Check for alerts
    if brain.getAverageFiringRate() > 100.0:  # High firing rate alert
        dashboard["alerts"].append({
            "time": timestamp,
            "type": "high_firing_rate",
            "value": brain.getAverageFiringRate(),
            "message": "Neuronal firing rate is extremely high"
        })
    
    if agent and agent.getCuriosityLevel() > 0.8:  # High curiosity alert
        dashboard["alerts"].append({
            "time": timestamp,
            "type": "high_curiosity",
            "value": agent.getCuriosityLevel(),
            "message": "Agent is showing extremely high curiosity"
        })
    
    # Update performance metrics
    dashboard["performance"]["current_step"] = step
    dashboard["performance"]["total_neurons"] = brain.getTotalNeuronCount()
    dashboard["performance"]["firing_efficiency"] = brain.getFiringNeuronCount() / max(1, brain.getTotalNeuronCount())

# Create simulation with real-time monitoring
config = pynlm.create_preset_config("standard")
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Start monitoring thread
dashboard = create_monitoring_dashboard()
monitoring_active = True

monitoring_thread = threading.Thread(
    target=lambda: [monitoring_callback(i, brain, agent, world, dashboard) 
                   for i in range(500)],
    daemon=True
)

monitoring_thread.start()

print("Starting real-time monitoring...")

# Run simulation with monitoring
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    if action:
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Display real-time metrics
    if step % 10 == 0:
        current_firing = brain.getAverageFiringRate()
        current_curiosity = agent.getCuriosityLevel()
        current_time = world.getSimulationTime()
        
        print(f"Step {step}: Firing={current_firing:.1f} Hz, "
              f"Curiosity={current_curiosity:.2f}, "
              f"Time={current_time:.1f}s")

monitoring_active = False
monitoring_thread.join()

# Display monitoring summary
print("\n=== Monitoring Summary ===")
print(f"Total monitoring duration: {len(dashboard['metrics']['firing_rate'])} steps")
print(f"Alerts triggered: {len(dashboard['alerts'])}")

if dashboard['alerts']:
    print("\nAlerts:")
    for alert in dashboard['alerts'][-5:]:  # Last 5 alerts
        print(f"  {alert['time']}: {alert['type']} - {alert['message']}")

# Save monitoring data
pynlm.export_to_csv(dashboard["metrics"], "monitoring_data.csv")

# Generate summary report
print("\nGenerating monitoring report...")
monitoring_report = {
    "simulation_parameters": {
        "neuron_count": dashboard["metrics"]["neuron_count"][0],
        "duration": len(dashboard["metrics"]["firing_rate"])
    },
    "performance_summary": dashboard["performance"],
    "alert_summary": {
        "total_alerts": len(dashboard["alerts"]),
        "alert_types": list(set(alert["type"] for alert in dashboard["alerts"]))
    },
    "metric_statistics": {
        "firing_rate": {
            "mean": sum(dashboard["metrics"]["firing_rate"]) / len(dashboard["metrics"]["firing_rate"]),
            "min": min(dashboard["metrics"]["firing_rate"]),
            "max": max(dashboard["metrics"]["firing_rate"])
        },
        "curiosity_level": {
            "mean": sum(dashboard["metrics"]["curiosity_level"]) / len(dashboard["metrics"]["curiosity_level"]),
            "min": min(dashboard["metrics"]["curiosity_level"]),
            "max": max(dashboard["metrics"]["curiosity_level"])
        }
    }
}

import json
with open("monitoring_report.json", "w") as f:
    json.dump(monitoring_report, f, indent=2, default=str)

print("Monitoring completed!")
print("- Raw metrics: 'monitoring_data.csv'")
print("- Analysis report: 'monitoring_report.json'")
```

## Configuration Presets

### Complete Preset Reference

The following presets are available for different use cases:

```python
# Minimal configuration (fast, simple)
config = pynlm.create_preset_config("minimal")
# Neurons: 50, Regions: 1, Connection probability: 0.05

# Standard configuration (balanced)
config = pynlm.create_preset_config("standard")
# Neurons: 200, Regions: 3, Connection probability: 0.1

# Advanced configuration (complex, learning-focused)
config = pynlm.create_preset_config("advanced")
# Neurons: 500, Regions: 5, Connection probability: 0.15
# Includes: working_memory, episodic_memory, curiosity, development
```

### Custom Configuration Builder
```python
import pynlm

def create_custom_config(base_preset="standard", **customizations):
    """Create custom configuration with preset base and additional customizations"""
    config = pynlm.create_preset_config(base_preset)
    
    # Apply customizations
    for key, value in customizations.items():
        if key == "neuron_count":
            config.set("neuron_count", value)
        elif key == "region_count":
            config.set("region_count", value)
        elif key == "connection_probability":
            config.set("connection_probability", value)
        elif key == "enable_features":
            for feature, enabled in value.items():
                config.set(f"enable_{feature}", enabled)
    
    return config

# Example usage
custom_config = create_custom_config(
    base_preset="standard",
    neuron_count=300,
    region_count=4,
    enable_features={"working_memory": True, "curiosity": True, "development": False}
)
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Brain not responding**
```python
# Problem: Brain step() not changing state
# Solution: Ensure brain is initialized
brain.initialize()

# Or use safe simulation step
result = pynlm.safe_simulation_step(brain, agent, world, step)
if not result["success"]:
    print("Simulation failed:", result["errors"])
```

**Issue 2: Agent not making decisions**
```python
# Problem: agent.decodeMotorCommand() returns None
# Solution: Check if agent is initialized
if not agent.is_initialized():
    agent.initialize(world)

# Or check agent configuration
if not agent.isRewardModulationEnabled():
    agent.enableRewardModulation(True)

# Add simple default action
if action is None:
    action = pynlm.createDefaultAction()
```

**Issue 3: Memory systems not working**
```python
# Problem: Memory features not being used
# Solution: Enable memory systems
config = pynlm.create_preset_config("advanced")
brain = pynlm.createBrain(config)

# Or check configuration
if not config.get("enable_working_memory", False):
    config.set("enable_working_memory", True)
```

**Issue 4: Performance issues**
```python
# Problem: Simulation too slow
# Solutions:
1. Reduce neuron count: config.set("neuron_count", 100)
2. Use simpler world: world = pynlm.createSimpleWorld()
3. Batch processing: Use threading/process pools
4. Reduce simulation steps: for step in range(50) instead of 500
```

**Issue 5: Visualization not working**
```python
# Problem: VisualizationInterface not functional
# Solution: Check if visualization is supported
viz = pynlm.createVisualization()
if viz.isActive():
    viz.initialize()
    # Configure visualization
    viz.setUpdateRate(10.0)
else:
    print("Visualization not available (Phase 2 placeholder)")
```

### Debugging Tools

```python
import pynlm

def debug_simulation(brain, agent, world):
    """Debug simulation state and identify issues"""
    print("=== Debug Information ===")
    
    # Check brain state
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSynapseCount()} synapses")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental stage: {brain.getDevelopmentalStage().value}")
    
    # Check agent state
    if agent:
        print(f"Agent: Curiosity={agent.getCuriosityLevel():.2f}, "
              f"Novelty={agent.getNoveltyLevel():.2f}")
        print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
        print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
    
    # Check world state
    if world:
        print(f"World: {world.getWidth()}x{world.getHeight()} pixels")
        print(f"  Simulation time: {world.getSimulationTime():.2f}")
        agent_body = world.getAgentBody()
        print(f"  Agent position: ({agent_body.x:.1f}, {agent_body.y:.1f})")
    
    # Validate configuration
    validation = pynlm.validate_configuration(brain.getConfig())
    if not validation["valid"]:
        print("Configuration errors:")
        for error in validation["errors"]:
            print(f"  - {error}")
    
    # Check for simulation warnings
    simulation_warnings = pynlm.validate_simulation_setup(brain, agent, world)
    if simulation_warnings["warnings"]:
        print("Simulation warnings:")
        for warning in simulation_warnings["warnings"]:
            print(f"  - {warning}")
    
    return {
        "valid": simulation_warnings["valid"],
        "warnings": simulation_warnings["warnings"],
        "errors": simulation_warnings["errors"]
    }

# Use in your simulation
debug_info = debug_simulation(brain, agent, world)
if not debug_info["valid"]:
    print("Fix the issues before proceeding!")
```

## Migration Guide

### From Basic to Advanced API

**Basic API (Easy Usage)**:
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

**Advanced API (with features)**:
```python
import pynlm

# Create brain with preset
config = pynlm.create_preset_config("standard")
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced features
brain.enableWorkingMemory(True)
brain.enableCuriosity(True)

# Run simulation with monitoring
for i in range(100):
    brain.step(i)
    if i % 10 == 0:
        stats = pynlm.get_detailed_stats(brain, None, None)
        print(f"Step {i}: {stats['brain']['average_firing_rate']:.2f} Hz")
```

### From Simple to Complex Environments

**Simple world**:
```python
world = pynlm.createSimpleWorld()
world.configure(10, 10, 8, 8)
```

**Complex world with obstacles**:
```python
world = pynlm.create_world_with_obstacles(30, 30, 15)
```

### From Manual to Automated Training

**Manual training**:
```python
for episode in range(10):
    world.reset()
    for step in range(100):
        # Manual step-by-step
        world.update(0.1)
        # ... manual actions
```

**Automated training with monitoring**:
```python
# Automated training with progress tracking
progress = pynlm.create_progress_tracker(1000)
results = pynlm.run_safe_simulation(brain, agent, world, 1000, verbose=True)
```

## Next Steps

### Continue Learning

1. **Deep Dive**: Read `HOW_TO_USE.md` for comprehensive technical documentation
2. **Architecture**: Study `docs/ARCHITECTURE.md` to understand system design
3. **Science**: Review `docs/SCIENCE.md` for scientific background
4. **Experiments**: Explore `docs/EXPERIMENTS.md` for experimental setups

### Advanced Development

1. **Performance**: Optimize simulation with SIMD and parallel processing
2. **Extensions**: Add new neuron types, plasticity rules, or cognitive functions
3. **Integration**: Connect with external sensors, actuators, or real-world systems
4. **Research**: Create novel learning algorithms and brain architectures

### Community and Contributions

1. **Documentation**: Contribute examples, tutorials, and best practices
2. **Testing**: Add unit tests for new features and edge cases
3. **Performance**: Benchmark and optimize different configurations
4. **Education**: Create educational materials and tutorials

### Research Applications

1. **Neuroscience**: Model brain development and learning
2. **AI**: Create cognitive architectures without backpropagation
3. **Robotics**: Implement embodied cognition and adaptive behavior
4. **Medicine**: Study neurological disorders and brain injuries

## Technical Notes

### Version Information

```python
# Check NLM version
nlm_version = pynlm.get_version()
print(f"NLM Version: {nlm_version}")

# Check Python bindings version
bindings_version = pynlm.get_bindings_version()
print(f"Python Bindings Version: {bindings_version}")
```

### Dependencies

The Python bindings require:
- C++20 compatible compiler
- pybind11 >= 2.11.0
- scikit-build-core for installation
- Optional: matplotlib for plotting, numpy for array operations

### Performance Considerations

```python
# For large simulations, consider:
# 1. Batch processing for multiple instances
# 2. Reducing simulation timestep for speed
# 3. Using simpler world configurations
# 4. Disabling expensive features when not needed
# 5. Monitoring memory usage with checkpointing
```

### Compatibility

- **Python 3.8+**: Full support for Python 3.8 and later
- **Platform**: Linux, macOS, Windows (with appropriate compilers)
- **Compiler**: C++20 standard compatible compilers (GCC 10+, Clang 12+, MSVC 2019+)

## License

MIT License - See LICENSE file for details.

## Copyright

(c) 2024-2025 NLM Research Team

All rights reserved. Commercial use requires licensing agreement.