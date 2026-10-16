"""Agent and world examples demonstrating enhanced agent and world interfaces."""

import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from pynlm.enhanced import (
    create_enhanced_brain,
    create_enhanced_agent, 
    create_enhanced_world,
    create_enhanced_config,
    EnhancedAgent,
    EnhancedWorld
)

print("Agent and World Examples - Enhanced NLM API")
print("=" * 50)

# Example 1: Basic agent-world setup
print("\n1. Basic agent-world setup:")
config = create_enhanced_config()
brain = create_enhanced_brain(config, name="AgentBrain")
brain.initialize()

world = create_enhanced_world()
world.configure_with_defaults(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = create_enhanced_agent(brain, name="ExampleAgent")
agent.initialize(world)

print(f"   Created agent: {agent}")
print(f"   Agent sensory input size: {agent.getSensoryInputSize()}")
print(f"   Agent motor output size: {agent.getMotorOutputSize()}")

# Example 2: Running episode
print("\n2. Running episode:")
episode_stats = agent.run_episode(world, max_steps=30)
print(f"   Episode stats: {episode_stats}")

# Example 3: Enhanced agent features
print("\n3. Enhanced agent features:")
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True) 
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print(f"   Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"   Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
print(f"   Development enabled: {agent.isDevelopmentEnabled()}")
print(f"   Curiosity enabled: {agent.isCuriosityEnabled()}")

# Example 4: Neuromodulation stats
print("\n4. Neuromodulation statistics:")
print(f"   Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"   Prediction error: {agent.getPredictionError():.3f}")
print(f"   Developmental stage: {agent.getDevelopmentalStage()}")

# Example 5: Enhanced world features
print("\n5. Enhanced world features:")
print(f"   World width: {world.getWidth()}")
print(f"   World height: {world.getHeight()}")
print(f"   World simulation time: {world.getSimulationTime()}")

# Example 6: World simulation with agent
print("\n6. Running world simulation:")
# Reset agent for simulation
agent.reset()
world.reset_with_history()

# Create additional objects in world
from pynlm import WorldObject, WorldObjectType
obj = WorldObject(5.0, 5.0, WorldObjectType.Resource, value=1.0)
world.add_object_with_history(obj)

print(f"   Added object at (5.0, 5.0)")
print(f"   Total objects in history: {len(world.object_history)}")

# Run simulation
states = world.run_simulation(agent, duration=2.0, timestep=0.1)
print(f"   Ran simulation for {len(states)} time steps")
print(f"   Agent position at end: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")

print("\n" + "=" * 50)
print("Agent and world examples completed successfully!")
