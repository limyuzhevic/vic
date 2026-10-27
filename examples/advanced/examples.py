#!/usr/bin/env python3
"""
Advanced NLM Example Scripts

This file contains advanced example scripts for using the NLM (Neural Learning Machine) library.
These examples demonstrate more sophisticated usage patterns and advanced features.

Each example can be run independently:
    python examples/advanced/examples.py <example_name>

Example names: simple, agent, learning, development, checkpoint, visualization, custom_config
"""

import pynlm
import json
import time
import os
from pathlib import Path

class AdvancedNLMExamples:
    """
    Advanced examples demonstrating sophisticated NLM usage patterns.
    """
    
    def __init__(self):
        self.examples_dir = Path(__file__).parent
    
    def create_custom_config(self, config_type="advanced"):
        """
        Create custom configurations for different use cases.
        
        Args:
            config_type: Type of configuration ("simple", "advanced", "debug", "production")
            
        Returns:
            pynlm.Config object
        """
        config = pynlm.createDefaultConfig()
        
        if config_type == "simple":
            # Minimal configuration for quick testing
            config.set("brain.neuron_count", 500)
            config.set("brain.synapse_density", 0.05)
            config.set("plasticity.stdp.enable", True)
            config.set("neuromod.dopamine.scale", 1.0)
            
        elif config_type == "advanced":
            # Advanced configuration with all features
            config.set("brain.neuron_count", 5000)
            config.set("brain.synapse_density", 0.1)
            config.set("brain.connection_probability", 0.05)
            config.set("brain.initial_weight_mean", 0.5)
            config.set("brain.v_thresh", -50.0)
            config.set("brain.v_rest", -70.0)
            config.set("brain.tau_mem", 20.0)
            config.set("brain.tau_ref", 2.0)
            
            # Plasticity settings
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.stdp.learning_rate", 0.001)
            config.set("plasticity.stdp.tau_plus", 20.0)
            config.set("plasticity.stdp.tau_minus", 20.0)
            config.set("plasticity.hebbian.enable", True)
            config.set("plasticity.structural.enable", True)
            
            # Neuromodulation
            config.set("neuromod.dopamine.scale", 1.0)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.novelty.enable", True)
            
            # Memory systems
            config.set("memory.working_capacity", 1000)
            config.set("memory.episodic.max_episodes", 5000)
            
            # Development
            config.set("development.initial_plasticity", 1.0)
            config.set("development.critical_period_length", 300.0)
            
            # Simulation
            config.set("simulation.timestep", 0.001)
            config.set("simulation.max_steps", 10000)
            
        elif config_type == "debug":
            # Configuration optimized for debugging and development
            config.set("brain.neuron_count", 100)
            config.set("brain.synapse_density", 0.02)
            config.set("plasticity.stdp.enable", True)
            config.set("neuromod.dopamine.scale", 0.5)
            config.set("development.enable", True)
            config.set("simulation.timestep", 0.01)  # Larger timestep for faster debugging
            
        elif config_type == "production":
            # Production-optimized configuration
            config.set("brain.neuron_count", 10000)
            config.set("brain.synapse_density", 0.15)
            config.set("plasticity.stdp.enable", True)
            config.set("plasticity.stdp.learning_rate", 0.0005)
            config.set("neuromod.dopamine.scale", 1.5)
            config.set("neuromod.curiosity.enable", True)
            config.set("memory.working_capacity", 5000)
            config.set("memory.episodic.max_episodes", 20000)
            
        return config
    
    def save_config_to_json(self, config, filename):
        """
        Save configuration to JSON file.
        
        Args:
            config: pynlm.Config object
            filename: Output JSON filename
        """
        # Convert config to JSON-serializable format
        config_data = {
            "config_entries": [
                {
                    "key": entry.key,
                    "value": self._convert_config_value(entry.value),
                    "source": str(entry.source),
                    "description": entry.description
                }
                for entry in config.getKeys()
            ]
        }
        
        with open(filename, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        print(f"Configuration saved to {filename}")
    
    def load_config_from_json(self, filename):
        """
        Load configuration from JSON file.
        
        Args:
            filename: Input JSON filename
            
        Returns:
            pynlm.Config object
        """
        config = pynlm.createDefaultConfig()
        
        try:
            with open(filename, 'r') as f:
                config_data = json.load(f)
            
            for entry in config_data.get("config_entries", []):
                key = entry["key"]
                value = self._convert_json_value(entry["value"])
                config.set(key, value)
                
        except Exception as e:
            print(f"Error loading config from {filename}: {e}")
            return None
        
        return config
    
    def _convert_config_value(self, value):
        """Convert pynlm ConfigValue to JSON-serializable format."""
        # This is a helper method that would need to be implemented
        # based on the actual ConfigValue variant types
        return str(value)
    
    def _convert_json_value(self, value):
        """Convert JSON value to pynlm ConfigValue."""
        # This is a helper method that would need to be implemented
        # based on the actual ConfigValue variant types
        return value
    
    def example_simple_brain(self):
        """Example 1: Create a simple brain with basic configuration."""
        print("=== Example 1: Simple Brain ===")
        
        # Create simple configuration
        config = self.create_custom_config("simple")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for i in range(100):
            brain.step(i)
            if i % 25 == 0:
                print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
        
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print("Simple brain example completed!\n")
    
    def example_agent_world(self):
        """Example 2: Complete agent-world interaction with learning."""
        print("=== Example 2: Agent-World Interaction ===")
        
        # Create advanced configuration
        config = self.create_custom_config("advanced")
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run simulation loop
        total_reward = 0.0
        for step in range(500):
            world.update(0.1)
            
            # Get sensory input and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinking
            brain.step(step)
            
            # Agent action selection
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and apply learning
            reward = 1.0 if step % 50 == 0 else 0.0
            total_reward += reward
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}:")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Development stage: {brain.getDevelopmentalStage()}")
                print(f"  Total reward: {total_reward:.1f}")
        
        print("Agent-world interaction example completed!\n")
    
    def example_learning_session(self):
        """Example 3: Learning session with checkpointing and replay."""
        print("=== Example 3: Learning Session with Checkpointing ===")
        
        # Create production-like configuration
        config = self.create_custom_config("production")
        
        # Create checkpoint directory
        checkpoint_dir = "nlm_checkpoints"
        os.makedirs(checkpoint_dir, exist_ok=True)
        config.set("checkpoint_dir", checkpoint_dir)
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable all learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Training loop with periodic checkpoints
        episodes = 5
        steps_per_episode = 200
        
        for episode in range(episodes):
            print(f"\n--- Episode {episode + 1}/{episodes} ---")
            
            # Save checkpoint at start of each episode
            checkpoint_file = f"{checkpoint_dir}/episode_{episode}.bin"
            brain.save(checkpoint_file)
            print(f"Saved checkpoint: {checkpoint_file}")
            
            # Run episode
            total_reward = 0.0
            for step in range(steps_per_episode):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Add variety to rewards
                if step % 75 == 0:
                    reward = 2.0  # Positive reward for finding target
                else:
                    reward = 0.1  # Small exploration reward
                total_reward += reward
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            print(f"Episode {episode + 1} reward: {total_reward:.1f}")
            print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
        
        # Load from final checkpoint and test
        print("\n--- Testing loaded brain ---")
        test_brain = pynlm.createBrain(config)
        if test_brain.load(checkpoint_file):
            print("Successfully loaded brain from checkpoint")
            test_brain.initialize()
            
            # Quick test simulation
            for i in range(50):
                test_brain.step(i)
            
            print(f"Loaded brain firing rate: {test_brain.getAverageFiringRate():.2f}")
        
        print("Learning session example completed!\n")
    
    def example_development_stages(self):
        """Example 4: Development stages and maturation."""
        print("=== Example 4: Development Stages ===")
        
        config = self.create_custom_config("advanced")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print("Development stages over time:")
        
        for stage in range(5):
            # Simulate development
            brain.develop()
            
            # Record development metrics
            dev_stage = brain.getDevelopmentalStage()
            plasticity = brain.getConfig().getOr("plasticity", 1.0)
            neuron_count = brain.getTotalNeuronCount()
            
            print(f"  Development step {stage}:")
            print(f"    Stage: {dev_stage}")
            print(f"    Plasticity modifier: {plasticity:.3f}")
            print(f"    Total neurons: {neuron_count}")
            print(f"    Average firing rate: {brain.getAverageFiringRate():.2f}")
            
            # Run a simulation step to see effects
            for i in range(10):
                brain.step(i)
        
        print("Development stages example completed!\n")
    
    def example_configuration_comparison(self):
        """Example 5: Configuration comparison and optimization."""
        print("=== Example 5: Configuration Comparison ===")
        
        configs = {
            "Simple": self.create_custom_config("simple"),
            "Advanced": self.create_custom_config("advanced"),
            "Debug": self.create_custom_config("debug"),
            "Production": self.create_custom_config("production")
        }
        
        results = {}
        
        for name, config in configs.items():
            print(f"\nTesting {name} configuration:")
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            start_time = time.time()
            for i in range(100):
                brain.step(i)
            elapsed_time = time.time() - start_time
            
            results[name] = {
                "neurons": brain.getTotalNeuronCount(),
                "spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "time": elapsed_time,
                "e_i_ratio": brain.getExcitationInhibitionRatio()
            }
            
            print(f"  Neurons: {results[name]['neurons']}")
            print(f"  Total spikes: {results[name]['spikes']}")
            print(f"  Average firing rate: {results[name]['firing_rate']:.2f}")
            print(f"  E/I ratio: {results[name]['e_i_ratio']:.2f}")
            print(f"  Time elapsed: {results[name]['time']:.3f}s")
        
        # Save comparison results
        comparison_file = "config_comparison.json"
        with open(comparison_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"\nConfiguration comparison saved to {comparison_file}")
        print("Configuration comparison example completed!\n")
    
    def run_all_examples(self):
        """Run all advanced examples."""
        print("Running all advanced NLM examples...\n")
        
        self.example_simple_brain()
        self.example_agent_world()
        self.example_learning_session()
        self.example_development_stages()
        self.example_configuration_comparison()
        
        print("All advanced examples completed successfully!")

def save_advanced_examples():
    """Save advanced example scripts to individual files."""
    examples_dir = Path("examples")
    examples_dir.mkdir(exist_ok=True)
    
    advanced_dir = examples_dir / "advanced"
    advanced_dir.mkdir(exist_ok=True)
    
    # Save main examples file
    with open(advanced_dir / "examples.py", "w") as f:
        f.write(__doc__ + "\n\n" + AdvancedNLMExamples.__doc__ + "\n\n" + 
                "def main():\n" +
                "    examples = AdvancedNLMExamples()\n" +
                "    examples.run_all_examples()\n" +
                "\n" +
                "if __name__ == \"__main__\":\n" +
                "    main()")
    
    # Create individual example files
    individual_examples = [
        ("simple_brain", """
import pynlm

def example_simple_brain():
    \"\"\"Simple brain example\"\"\"
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for i in range(100):
        brain.step(i)
    
    print(f\"Simple brain completed: {brain.getTotalSpikeCount()} spikes\")

if __name__ == \"__main__\":
    example_simple_brain()
"""),
        
        ("agent_world", """
import pynlm

def example_agent_world():
    \"\"\"Agent-world interaction example\"\"\"
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world.reset()
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f\"Agent-world example completed: {brain.getTotalSpikeCount()} spikes, Curiosity: {agent.getCuriosityLevel():.3f}\")

if __name__ == \"__main__\":
    example_agent_world()
"""),
        
        ("learning_session", """
import pynlm
import os

def example_learning_session():
    \"\"\"Learning session with checkpointing example\"\"\"
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Save checkpoint
    os.makedirs("checkpoints", exist_ok=True)
    brain.save("checkpoints/episode_1.bin")
    
    for step in range(300):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.0)
        agent.updateDevelopment(0.1)
    
    print(f\"Learning session completed: {brain.getTotalSpikeCount()} spikes, Development stage: {brain.getDevelopmentalStage()}\")

if __name__ == \"__main__\":
    example_learning_session()
"""),
        
        ("development_stages", """
import pynlm

def example_development_stages():
    \"\"\"Development stages example\"\"\"
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Development stages:")
    for i in range(5):
        brain.develop()
        print(f\"  Step {i}: Stage {brain.getDevelopmentalStage()}, Firing rate: {brain.getAverageFiringRate():.2f}\")
    
    print(f\"Development example completed: {brain.getTotalSpikeCount()} spikes\")

if __name__ == \"__main__\":
    example_development_stages()
"""),
        
        ("configuration_comparison", """
import pynlm
import time

def example_configuration_comparison():
    \"\"\"Configuration comparison example\"\"\"
    configs = {
        \"Simple\": create_config(\"simple\"),
        \"Advanced\": create_config(\"advanced\"),
    }
    
    results = {}
    
    for name, config in configs.items():
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for i in range(50):
            brain.step(i)
        elapsed_time = time.time() - start_time
        
        results[name] = {
            \"spikes\": brain.getTotalSpikeCount(),
            \"firing_rate\": brain.getAverageFiringRate(),
            \"time\": elapsed_time
        }
        
        print(f\"{name}: {results[name]['spikes']} spikes, {results[name]['firing_rate']:.2f} Hz, {results[name]['time']:.3f}s\")
    
    print(f\"Configuration comparison completed\"\"")

def create_config(config_type):
    \"\"\"Helper function to create different configurations\"\"\"
    config = pynlm.createDefaultConfig()
    if config_type == \"simple\":
        config.set(\"brain.neuron_count\", 500)
    elif config_type == \"advanced\":
        config.set(\"brain.neuron_count\", 2000)
        config.set(\"plasticity.stdp.enable\", True)
    return config

if __name__ == \"__main__\":
    example_configuration_comparison()
""")
    }
    
    for filename, content in individual_examples.items():
        with open(advanced_dir / f"{filename}.py", "w") as f:
            f.write(content)
    
    print(f"Advanced examples saved to {advanced_dir}/")

if __name__ == "__main__":
    # Run advanced examples
    examples = AdvancedNLMExamples()
    examples.run_all_examples()
    
    # Save individual example files
    save_advanced_examples()