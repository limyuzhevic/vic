# Improved NLM Python API

import pynlm

# Create enhanced brain configuration
config = pynlm.createDefaultConfig()

# Configure brain with advanced parameters
config.set("brain.neuron_count", 2000)  # Larger brain
config.set("brain.synapse_density", 0.2)  # More connections
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced agent with multiple subsystems
agent = pynlm.createAgentBrain(brain)

# Create sophisticated world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable advanced learning capabilities
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enablePredictionError(True)       # Use predictive coding

# Enhanced monitoring callback system
monitoring_data = {
    'steps': [],
    'rewards': [],
    'firing_rates': [],
    'curiosity_levels': [],
    'novelty_levels': []
}

def advanced_monitoring(step, world, agent, brain):
    """Enhanced monitoring with performance metrics"""
    percept = world.getSensoryPercept()
    reward = world.computeReward(agent.getBrain().getRegions()[0].get()) if percept.getInternal() else 0.0
    
    monitoring_data['steps'].append(step)
    monitoring_data['rewards'].append(reward)
    monitoring_data['firing_rates'].append(brain.getAverageFiringRate())
    monitoring_data['curiosity_levels'].append(agent.getCuriosityLevel())
    monitoring_data['novelty_levels'].append(agent.getNoveltyLevel())

# Advanced simulation loop with enhanced features
for step in range(2000):
    # Update world with variable timestep
    dt = 0.01 + 0.005 * agent.getCuriosityLevel()  # Adaptive timestep
    world.update(dt)
    
    # Get comprehensive sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input through agent
    agent.processSensoryInput(percept)
    
    # Run brain step with time
    brain.step(step, step * dt)
    
    # Get sophisticated motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply motor command
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply enhanced reward modulation with prediction
    reward = world.computeReward(agent.getBrain().getRegions()[0].get())
    predicted_reward = agent.getPredictionError() if agent.getPredictionError() is not None else 0.0
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Update development at variable rates
    dev_rate = 0.1 * (1.0 + agent.getNoveltyLevel())  # Novelty-dependent development
    agent.updateDevelopment(dev_rate)
    
    # Advanced monitoring
    advanced_monitoring(step, world, agent, brain)
    
    # Enhanced logging with performance insights
    if step % 200 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Avg firing rate: {brain.getAverageFiringRate():.3f} Hz")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Reward: {reward:.3f}")

# Advanced analysis and visualization
print("\n=== Simulation Analysis ===")
print(f"Total steps: {len(monitoring_data['steps'])}")
print(f"Average reward: {sum(monitoring_data['rewards'])/len(monitoring_data['rewards']):.3f}")
print(f"Max firing rate: {max(monitoring_data['firing_rates']):.3f} Hz")
print(f"Average curiosity: {sum(monitoring_data['curiosity_levels'])/len(monitoring_data['curiosity_levels']):.3f}")
print(f"Average novelty: {sum(monitoring_data['novelty_levels'])/len(monitoring_data['novelty_levels']):.3f}")

# Statistical analysis
import statistics as stats
if monitoring_data['rewards']:
    print(f"Reward std deviation: {stats.stdev(monitoring_data['rewards']):.3f}")

# Save advanced configuration
brain.save("advanced_brain_state.bin")
print(f"\nBrain state saved to advanced_brain_state.bin")

print("\n=== Advanced Simulation Complete ===")
