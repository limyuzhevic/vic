# NLM Intermediate Tutorial: Advanced Features and Integration

This tutorial bridges the gap between the beginner-friendly `easy_usage.md` and the advanced `HOW_TO_USE.md`. It covers intermediate concepts and provides practical examples for using NLM's more sophisticated features.

## Prerequisites

- Basic understanding of NLM concepts from `easy_usage.md`
- Familiarity with the Python API from `HOW_TO_USE.md`
- Interest in exploring advanced neural simulation features

## Module 1: Advanced Brain Configuration

### Custom Brain Setup with Multiple Regions

```python
import pynlm
import time

# Create configuration with custom parameters
config = pynlm.createDefaultConfig()

# Configure multiple brain regions
config.setNeuronCount(3000)
config.setConnectionProbability(0.08)

# Enable advanced plasticity
config.setStdpEnable(True)
config.setStdpLearningRate(0.002)
config.setStdpTauPlus(25.0)
config.setStdpTauMinus(25.0)
config.setHebbianEnable(True)
config.setCuriosityEnable(True)
config.setNoveltyEnable(True)

# Create brain with regions
brain = pynlm.createBrain(config)
brain.initialize()

# Add different types of regions
region_cortex = brain.addRegion("Cortex - Sensory Processing")
region_motor = brain.addRegion("Motor Cortex - Action Planning")
region_memory = brain.addRegion("Memory Hub - Storage and Retrieval")

print("Created brain with regions:")
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"  - {region_id.value}: Unknown region type")

print(f"\nBrain summary:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Active regions: {brain.getRegionCount()}")
print(f"  Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")
```

### Brain Development Simulation

```python
import pynlm

# Create brain with development enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Brain Development Simulation ===")
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Simulate development over time
development_stages = []
for step in range(0, 1200, 60):  # Every minute (60 simulation seconds)
    brain.step(step)
    
    if step % 60 == 0:
        stage = brain.getDevelopmentalStage()
        development_stages.append((step, stage))
        print(f"Step {step:4d}: {stage.value:20s} (age: {step/1000:.1f}s)")

print(f"\nDevelopment completed: {len(development_stages)} stages reached")
print(f"Final developmental stage: {brain.getDevelopmentalStage().value}")
```

## Module 2: Complete Agent-Based Learning

### Task: Resource Collection Agent

This example creates an intelligent agent that learns to collect resources while avoiding hazards.

```python
import pynlm
import random

class ResourceCollectorAgent:
    def __init__(self, width=20, height=20):
        # Initialize brain and agent
        config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create and configure world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=width, height=height, visionWidth=8, visionHeight=8)
        self.world.setMaxEnergy(100.0)
        self.world.setEnergyDecayRate(0.005)
        
        # Configure agent for learning
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Statistics tracking
        self.episode_stats = {
            'steps': 0,
            'resources_collected': 0,
            'hazards_hit': 0,
            'total_reward': 0.0,
            'energy_used': 0.0
        }
    
    def reset_episode(self):
        """Reset world and agent for new episode"""
        self.world.reset()
        self.world.setAgentStart(5.0, 5.0)
        self.agent.reset()
        self.agent.initialize(self.world)
        
        # Initialize statistics
        for key in self.episode_stats:
            self.episode_stats[key] = 0
        
        print("Episode reset - agent ready for new task")
    
    def run_episode(self, max_steps=1000):
        """Run one complete episode of learning and exploration"""
        self.reset_episode()
        
        print(f"Starting episode: Agent learning to collect resources")
        print(f"World: {self.world.getWidth()}x{self.world.getHeight()} cells")
        print(f"Vision: {self.world.getVisionWidth()}x{self.world.getVisionHeight()} pixels")
        print("-" * 50)
        
        for step in range(max_steps):
            # 1. World update
            self.world.update(timestep=0.1)
            self.episode_stats['steps'] += 1
            
            # 2. Get sensory input
            percept = self.world.getSensoryPercept()
            
            # 3. Process sensory input
            self.agent.processSensoryInput(percept)
            
            # 4. Brain thinks
            self.brain.step(step)
            
            # 5. Get action from brain
            motor_cmd = self.agent.decodeMotorCommand()
            
            # 6. Apply action in world
            action_result = self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # 7. Apply reward modulation for learning
            self.agent.applyRewardModulation(action_result.reward, 0.0)
            
            # 8. Update development
            self.agent.updateDevelopment(0.1)
            
            # Track statistics
            if action_result.success:
                if "resource" in action_result.message:
                    self.episode_stats['resources_collected'] += 1
                    print(f"Step {step:4d}: Collected resource! (+{action_result.reward:.1f})")
                elif "hazard" in action_result.message:
                    self.episode_stats['hazards_hit'] += 1
                    print(f"Step {step:4d}: Hit hazard! ({action_result.reward:.1f})")
            else:
                print(f"Step {step:4d}: Action failed - {action_result.message}")
            
            # Print progress every 50 steps
            if step % 50 == 0:
                print(f"Step {step:4d}: "
                      f"Resources: {self.episode_stats['resources_collected']}, "
                      f"Hazards: {self.episode_stats['hazards_hit']}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {self.agent.getNoveltyLevel():.3f}")
            
            # Check for episode termination
            if self.episode_stats['energy_used'] >= 80 or step >= max_steps - 1:
                print(f"\nEpisode ended at step {step + 1}")
                break
        
        # Episode summary
        self._print_episode_summary()
        return self.episode_stats
    
    def _print_episode_summary(self):
        """Print detailed statistics for completed episode"""
        stats = self.episode_stats
        print("\n" + "=" * 60)
        print("EPISODE SUMMARY")
        print("=" * 60)
        print(f"Total steps: {stats['steps']}")
        print(f"Resources collected: {stats['resources_collected']}")
        print(f"Hazards encountered: {stats['hazards_hit']}")
        print(f"Success rate: {stats['resources_collected'] / (stats['resources_collected'] + stats['hazards_hit']) * 100:.1f}%")
        print(f"Development stage reached: {self.brain.getDevelopmentalStage().value}")
        print(f"Final curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"Final novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"Final prediction error: {self.agent.getPredictionError():.3f}")
        print("=" * 60)
    
    def train(self, num_episodes=5, steps_per_episode=500):
        """Train agent over multiple episodes"""
        print(f"Starting training: {num_episodes} episodes, {steps_per_episode} steps each")
        print("=" * 80)
        
        episode_results = []
        
        for episode in range(num_episodes):
            print(f"\n{'='*20} EPISODE {episode + 1}/{num_episodes} {'='*20}")
            result = self.run_episode(steps_per_episode)
            episode_results.append(result)
        
        # Training summary
        self._print_training_summary(episode_results)
        return episode_results
    
    def _print_training_summary(self, episode_results):
        """Print training summary across all episodes"""
        print("\n" + "=" * 80)
        print("TRAINING SUMMARY")
        print("=" * 80)
        
        avg_steps = sum(r['steps'] for r in episode_results) / len(episode_results)
        avg_resources = sum(r['resources_collected'] for r in episode_results) / len(episode_results)
        avg_success = sum(
            (r['resources_collected'] / max(1, r['resources_collected'] + r['hazards_hit']) * 100)
            for r in episode_results
        ) / len(episode_results)
        
        print(f"Average episode length: {avg_steps:.1f} steps")
        print(f"Average resources collected: {avg_resources:.1f}")
        print(f"Average success rate: {avg_success:.1f}%")
        print(f"Final developmental stage: {self.brain.getDevelopmentalStage().value}")
        print(f"Learning efficiency (resources/steps): {avg_resources / avg_steps:.3f}")
        print("=" * 80)

# Run the complete training simulation
if __name__ == "__main__":
    print("Resource Collector Agent - Advanced NLM Learning Tutorial")
    print("=" * 80)
    
    # Create and train agent
    agent = ResourceCollectorAgent(width=30, height=30)
    results = agent.train(num_episodes=3, steps_per_episode=300)
    
    print("\nTutorial completed! Agent learned to navigate and collect resources.")
```