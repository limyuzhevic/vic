#!/usr/bin/env python3
"""
NLM Examples Generator
Advanced example generator for Neural Learning Machine

This script provides utilities for:
- Generating comprehensive example configurations
- Creating example scripts for different use cases
- Generating documentation and tutorials
- Creating benchmark and test examples
"""

import os
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
import re
from datetime import datetime

class ExamplesGenerator:
    def __init__(self, examples_dir: str = "examples"):
        self.examples_dir = Path(examples_dir)
        self.examples_dir.mkdir(exist_ok=True)
        
        # Example templates
        self.templates = {
            "getting_started": self._get_getting_started_template(),
            "basic_brain": self._get_basic_brain_template(),
            "advanced_agent": self._get_advanced_agent_template(),
            "memory_systems": self._get_memory_systems_template(),
            "neuromodulation": self._get_neuromodulation_template(),
            "development": self._get_development_template(),
            "plasticity": self._get_plasticity_template(),
            "prediction": self._get_prediction_template(),
            "performance_tuning": self._get_performance_tuning_template(),
            "research_study": self._get_research_study_template(),
            "production_use": self._get_production_use_template(),
            "visualization": self._get_visualization_template(),
            "benchmarking": self._get_benchmarking_template(),
        }
    
    def _get_getting_started_template(self) -> str:
        """Get getting started example template."""
        return """#!/usr/bin/env python3
"""
"NLM - Getting Started Example
This example demonstrates the basic usage of Neural Learning Machine (NLM).

The NLM framework provides an experimental artificial developmental brain
that learns from experience through interaction with an environment.

This example shows the simplest possible brain setup and operation.
"""

import pynlm

def main():
    # Create a simple brain with default configuration
    print("🐭 Creating NLM brain...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Initialize the brain
    print("🔧 Initializing brain...")
    brain.initialize()
    
    # Display brain statistics
    print("\n📊 Brain Statistics:")
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    print(f"   Synapses: {brain.getTotalSynapseCount()}")
    print(f"   Regions: {brain.getRegionCount()}")
    
    # Run a simple simulation
    print("\n🧠 Running simulation...")
    for step in range(100):
        brain.step(step)
        
        # Display progress every 20 steps
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Final statistics
    print(f"\n📈 Simulation Results:")
    print(f"   Total steps: 100")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    print("\n✅ Example completed successfully!")

if __name__ == "__main__":
    main()
"""
    
    def _get_basic_brain_template(self) -> str:
        """Get basic brain example template."""
        return """#!/usr/bin/env python3
"""
"NLM - Basic Brain Configuration Example
This example demonstrates how to configure and use a basic NLM brain
with different settings and parameters.
"""

import pynlm

def create_brain_config(description: str, **kwargs):
    """Create a brain with custom configuration."""
    # Start with default configuration
    config = pynlm.createDefaultConfig()
    
    # Apply customizations
    default_configs = {
        "small": {
            "neuron_count": 100,
            "region_count": 2,
            "connection_probability": 0.05,
        },
        "medium": {
            "neuron_count": 1000,
            "region_count": 4,
            "connection_probability": 0.1,
        },
        "large": {
            "neuron_count": 5000,
            "region_count": 8,
            "connection_probability": 0.15,
        },
        "sparse": {
            "neuron_count": 2000,
            "region_count": 6,
            "connection_probability": 0.02,
        },
        "dense": {
            "neuron_count": 3000,
            "region_count": 5,
            "connection_probability": 0.3,
        }
    }
    
    if description in default_configs:
        for key, value in default_configs[description].items():
            config.set(key, value)
    
    # Apply any additional customizations
    for key, value in kwargs.items():
        config.set(key, value)
    
    return config

def run_brain_simulation(config, steps: int = 500):
    """Run simulation with given configuration."""
    print(f"🧠 Running simulation with {config.getOr('neuron_count', 0)} neurons...")
    print(f"   Steps: {steps}")
    print(f"   Region count: {config.getOr('region_count', 0)}")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Log initial state
    print(f"\n📊 Initial state:")
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    print(f"   Synapses: {brain.getTotalSynapseCount()}")
    print(f"   Regions: {brain.getRegionCount()}")
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Log statistics periodically
        if step % 100 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Final results
    print(f"\n📈 Simulation Results:")
    print(f"   Total steps: {steps}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    return brain

def main():
    """Run all brain configuration examples."""
    print("🧠 NLM Brain Configuration Examples")
    print("=" * 50)
    
    examples = [
        ("small_network", 200),
        ("medium_network", 500),
        ("large_network", 300),
        ("sparse_network", 400),
        ("dense_network", 200),
    ]
    
    for example_name, steps in examples:
        print(f"\n{'='*60}")
        print(f"Example: {example_name.replace('_', ' ').title()}")
        print(f"{'='*60}")
        
        try:
            config = create_brain_config(example_name)
            brain = run_brain_simulation(config, steps)
            print(f"✅ {example_name} completed successfully!")
            
        except Exception as e:
            print(f"❌ Error in {example_name}: {e}")

if __name__ == "__main__":
    main()
"""
    
    def _get_advanced_agent_template(self) -> str:
        """Get advanced agent example template."""
        return """#!/usr/bin/env python3
"""
"NLM - Advanced Agent Example
This example demonstrates the complete agent-brain-world interaction
using NLM's advanced capabilities including memory, prediction, and cognition.
"""

import pynlm
import time
import math
from typing import Tuple, List, Dict

class AdvancedAgent:
    """Advanced agent that demonstrates NLM's full capabilities."""
    
    def __init__(self, config_name: str = "advanced"):
        self.config_name = config_name
        self.setup_configuration()
        self.setup_brain()
        self.setup_world()
        self.setup_agent()
    
    def setup_configuration(self):
        """Setup configuration with advanced settings."""
        
        # ===== MEMORY SYSTEM OPTIMIZATION =====
        self.config = pynlm.createDefaultConfig()
        
        # Memory system configuration
        self.config.set("memory_working_capacity", 200)
        self.config.set("memory_episodic_capacity", 200)
        self.config.set("memory_associative_capacity", 200)
        self.config.set("memory_replay_interval", 50)
        self.config.set("memory_consolidation_threshold", 0.8)
        
        # ===== PLASTICITY SYSTEM CONFIGURATION =====
        self.config.set("plasticity_learning_rate", 0.008)
        self.config.set("stdp_ltp_weight", 0.02)
        self.config.set("stdp_ltd_weight", 0.015)
        self.config.set("stdp_time_constant", 20.0)
        self.config.set("plasticity_structural_enable", True)
        self.config.set("synaptogenesis_rate", 0.0002)
        self.config.set("pruning_rate", 0.00002)
        
        # ===== DEVELOPMENT SYSTEM =====
        self.config.set("development_enable", True)
        self.config.set("initial_plasticity_rate", 0.9)
        self.config.set("maturation_time_constant", 200.0)
        self.config.set("critical_period_duration", 800)
        
        # ===== NEUROMODULATION =====
        self.config.set("neuromod_dopamine_baseline", 0.3)
        self.config.set("neuromod_dopamine_scale", 2.0)
        self.config.set("neuromod_curiosity_enable", True)
        self.config.set("neuromod_curiosity_threshold", 0.1)
        self.config.set("neuromod_novelty_enable", True)
        self.config.set("neuromod_novelty_threshold", 0.15)
        
        # ===== PREDICTION SYSTEM =====
        self.config.set("prediction_enable", True)
        self.config.set("prediction_time_constant", 80.0)
        self.config.set("prediction_update_frequency", 10)
        self.config.set("prediction_confidence_threshold", 0.85)
        self.config.set("prediction_error_decay", 0.95)
        
        # ===== COGNITION SYSTEMS =====
        self.config.set("cognition_planner_enable", True)
        self.config.set("cognition_planner_horizon", 15)
        self.config.set("cognition_concept_formation_threshold", 0.9)
        self.config.set("cognition_attention_strength", 0.7)
        
        # ===== PERFORMANCE OPTIMIZATIONS =====
        self.config.set("performance_use_memory_pools", True)
        self.config.set("performance_pool_size_neurons", 10000)
        self.config.set("performance_pool_size_synapses", 50000)
        self.config.set("visualization_enabled", True)
    
    def setup_brain(self):
        """Setup brain with advanced configuration."""
        print("🧠 Setting up advanced brain...")
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        print(f"   Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"   Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"   Development stage: {self.brain.getDevelopmentalStage()}")
    
    def setup_world(self):
        """Setup interactive world for the agent."""
        print("🌍 Setting up world environment...")
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(
            width=30,
            height=30,
            visionWidth=10,
            visionHeight=10
        )
        self.world.reset()
        self.world.setAgentStart(15.0, 15.0)
        
        print(f"   World size: {self.world.getWidth()}x{self.world.getHeight()}")
        print(f"   Vision range: {self.world.getVisionRange()}")
    
    def setup_agent(self):
        """Setup agent brain interface."""
        print("🤖 Setting up agent interface...")
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all advanced subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        self.agent.enablePrediction(True)
        
        print(f"   Agent initialized with world")
    
    def run_episode(self, max_steps: int = 1000) -> Dict:
        """Run a single episode of agent-world interaction."""
        print(f"\n🎬 Running episode (max {max_steps} steps)...")
        
        episode_data = {
            'steps': 0,
            'rewards': [],
            'actions': [],
            'perception_changes': [],
            'learning_events': []
        }
        
        for step in range(max_steps):
            # Update world
            self.world.update(timestep=0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Calculate reward
            internal_state = percept.getInternal() if percept.getInternal() else []
            reward = internal_state[0] if internal_state else 0.0
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Record data
            episode_data['steps'] = step + 1
            episode_data['actions'].append(motor_cmd)
            
            # Log progress
            if step % 100 == 0:
                print(f"   Step {step}:")
                print(f"     Position: ({self.agent.getPosition():.1f}, {self.agent.getPosition():.1f})")
                print(f"     Reward: {reward:.3f}")
                print(f"     Curiosity: {self.agent.getCuriosityLevel():.3f}")
                print(f"     Novelty: {self.agent.getNoveltyLevel():.3f}")
                print(f"     Prediction error: {self.agent.getPredictionError():.3f}")
                print(f"     Dev stage: {self.brain.getDevelopmentalStage()}")
        
        print(f"\n📊 Episode completed!")
        print(f"   Total steps: {episode_data['steps']}")
        print(f"   Total reward: {sum(episode_data['rewards']):.3f}")
        print(f"   Average reward per step: {sum(episode_data['rewards']) / len(episode_data['rewards']):.3f}")
        
        return episode_data
    
    def run_learning_phase(self, episodes: int = 5):
        """Run multiple learning episodes."""
        print(f"\n🧠 Starting learning phase ({episodes} episodes)...")
        
        learning_data = []
        
        for episode in range(episodes):
            print(f"\n{'='*60}")
            print(f"Episode {episode + 1}/{episodes}")
            print(f"{'='*60}")
            
            episode_data = self.run_episode(max_steps=500)
            episode_data['episode'] = episode + 1
            
            learning_data.append(episode_data)
            
            # Brief pause between episodes
            if episode < episodes - 1:
                print(f"\n⏸️  Pausing before next episode...")
                time.sleep(2)
        
        return learning_data

def main():
    """Run advanced agent example."""
    print("🤖 NLM Advanced Agent Example")
    print("This example demonstrates NLM's advanced capabilities")
    print("including memory, prediction, cognition, and neuromodulation.")
    print("=" * 80)
    
    # Create advanced agent
    agent = AdvancedAgent("advanced")
    
    # Run learning episodes
    learning_data = agent.run_learning_phase(episodes=3)
    
    # Summary statistics
    print(f"\n📊 Learning Phase Summary")
    print("=" * 40)
    
    total_steps = sum(d['steps'] for d in learning_data)
    total_rewards = sum(sum(d['rewards']) for d in learning_data)
    total_episodes = len(learning_data)
    
    print(f"Total episodes: {total_episodes}")
    print(f"Total steps: {total_steps}")
    print(f"Total reward: {total_rewards:.3f}")
    print(f"Average reward per step: {total_rewards / total_steps:.3f}")
    
    # Save learning data
    save_data = input(f"\n💾 Save learning data? (y/N): ").strip().lower() == 'y'
    if save_data:
        import json
        data_file = f"learning_data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(data_file, 'w') as f:
            json.dump(learning_data, f, indent=2)
        print(f"✅ Learning data saved to: {data_file}")
    
    print(f"\n✅ Advanced agent example completed successfully!")

if __name__ == "__main__":
    main()
"""
    
    def _get_memory_systems_template(self) -> str:
        """Get memory systems example template."""
        return """#!/usr/bin/env python3
"""
"NLM - Memory Systems Example
This example demonstrates NLM's comprehensive memory systems including
working memory, episodic memory, and associative memory.
"""

import pynlm
import time
from typing import Dict, List, Tuple

class MemorySystemsDemo:
    """Demonstration of NLM's memory systems."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.setup_memory_configuration()
        self.setup_brain()
    
    def setup_memory_configuration(self):
        """Setup configuration optimized for memory systems."""
        # Configure for memory research
        self.config.set("memory_working_capacity", 500)
        self.config.set("memory_episodic_capacity", 1000)
        self.config.set("memory_associative_capacity", 2000)
        self.config.set("memory_replay_interval", 100)
        
        # Configure for plasticity and learning
        self.config.set("plasticity_learning_rate", 0.01)
        self.config.set("stdp_ltp_weight", 0.02)
        self.config.set("stdp_ltd_weight", 0.015)
        self.config.set("plasticity_structural_enable", True)
        
        # Configure for replay and consolidation
        self.config.set("enable_replay", True)
        self.config.set("replay_interval", 100)
        self.config.set("checkpoint_frequency", 500)
        
        # Performance settings for memory-intensive tasks
        self.config.set("performance_use_memory_pools", True)
        self.config.set("performance_pool_size_neurons", 10000)
        self.config.set("performance_pool_size_synapses", 50000)
        
        # Visualization for memory monitoring
        self.config.set("visualization_enabled", True)
        self.config.set("log_memory_stats", True)
    
    def setup_brain(self):
        """Setup brain with memory-optimized configuration."""
        print("🧠 Setting up brain with memory systems...")
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Enable memory subsystems
        print(f"   Memory capacity configured:")
        print(f"     Working memory: {self.config.get('memory_working_capacity', 'N/A')}")
        print(f"     Episodic memory: {self.config.get('memory_episodic_capacity', 'N/A')}")
        print(f"     Associative memory: {self.config.get('memory_associative_capacity', 'N/A')}")
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize()
    
    def demonstrate_working_memory(self):
        """Demonstrate working memory functionality."""
        print("\n🧠 Working Memory Demonstration")
        print("-" * 40)
        
        # Inject pattern to be stored in working memory
        for i in range(50):
            brain_config = pynlm.createDefaultConfig()
            
            patterns = [
                [1.0, 0.0, 0.0, 0.0, 0.0],
                [0.0, 1.0, 0.0, 0.0, 0.0],
                [0.0, 0.0, 1.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 1.0, 0.0],
                [0.0, 0.0, 0.0, 0.0, 1.0],
            ]
            
            pattern = patterns[i % len(patterns)]
            
            for step in range(20):
                self.brain.step(i * 20 + step)
        
        print(f"   Working memory simulated with {self.config.get('memory_working_capacity', 'N/A')} slots")
    
    def demonstrate_episodic_memory(self):
        """Demonstrate episodic memory functionality."""
        print("\n📚 Episodic Memory Demonstration")
        print("-" * 40)
        
        episodes = []
        
        for episode in range(5):
            experience = {
                'episode_id': episode,
                'timestamp': episode * 100,
                'reward': episode * 2.0,
                'actions': [],
                'perception': []
            }
            
            episodes.append(experience)
            
            print(f"   Episode {episode + 1}: Reward {experience['reward']}")
            print(f"     Stored with ID: {experience['episode_id']}")
            print(f"     Timestamp: {experience['timestamp']}")
        
        # Demonstrate replay
        print(f"\n   Replay phase:")
        for episode in episodes:
            print(f"     Replaying Episode {episode['episode_id']}")
            print(f"       Reward: {episode['reward']}")
    
    def demonstrate_associative_memory(self):
        """Demonstrate associative memory functionality."""
        print("\n🔗 Associative Memory Demonstration")
        print("-" * 40)
        
        patterns = {
            'pattern_a': [1.0, 0.5, 0.2, 0.1, 0.0],
            'pattern_b': [0.0, 0.2, 0.8, 0.4, 0.1],
            'pattern_c': [0.1, 0.0, 0.3, 0.7, 0.5],
        }
        
        associations = {
            ('pattern_a', 'pattern_b'): 0.8,
            ('pattern_b', 'pattern_c'): 0.9,
            ('pattern_c', 'pattern_a'): 0.7,
        }
        
        print(f"   Pattern associations configured:")
        for (p1, p2), strength in associations.items():
            print(f"     {p1} ↔ {p2}: {strength}")
        
        print(f"\n   Simulating Hebbian learning...")
        for iteration in range(10):
            learning_strength = 0.01 * (1.0 - iteration * 0.1)
            total_associations = sum(associations.values())
            
            print(f"     Iteration {iteration + 1}: Learning rate {learning_strength:.3f}, "
                  f"Total association strength: {total_associations:.3f}")
    
    def demonstrate_memory_consolidation(self):
        """Demonstrate memory consolidation and replay."""
        print("\n🔄 Memory Consolidation Demonstration")
        print("-" * 40)
        
        consolidation_events = []
        
        for consolidation_step in range(10):
            event = {
                'step': consolidation_step,
                'important_episodes': [],
                'replay_frequency': 0,
                'consolidation_strength': 0.1 * (consolidation_step + 1)
            }
            
            important = [e for e in range(5) if (e + 1) % 2 == 0]
            event['important_episodes'] = important
            event['replay_frequency'] = len(important) * 10
            
            consolidation_events.append(event)
            
            print(f"   Consolidation step {consolidation_step + 1}:")
            print(f"     Important episodes: {event['important_episodes']}")
            print(f"     Replay frequency: {event['replay_frequency']}")
            print(f"     Consolidation strength: {event['consolidation_strength']:.2f}")
        
        print(f"\n   Total consolidation events: {len(consolidation_events)}")
    
    def run_memory_demonstration(self):
        """Run complete memory systems demonstration."""
        print("📚 NLM Memory Systems Demonstration")
        print("This example demonstrates NLM's comprehensive memory capabilities")
        print("including working memory, episodic memory, and associative memory.")
        print("=" * 80)
        
        self.setup_brain()
        
        self.demonstrate_working_memory()
        self.demonstrate_episodic_memory()
        self.demonstrate_associative_memory()
        self.demonstrate_memory_consolidation()
        
        print(f"\n📊 Memory Systems Summary")
        print("=" * 40)
        print(f"Memory capacities configured:")
        print(f"  Working Memory: {self.config.get('memory_working_capacity', 'N/A')} items")
        print(f"  Episodic Memory: {self.config.get('memory_episodic_capacity', 'N/A')} episodes")
        print(f"  Associative Memory: {self.config.get('memory_associative_capacity', 'N/A')} patterns")
        print(f"  Replay Frequency: {self.config.get('memory_replay_interval', 'N/A')} steps")
        
        save_config = input(f"\n💾 Save memory configuration? (y/N): ").strip().lower() == 'y'
        if save_config:
            config_file = f"memory_config_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
            with open(config_file, 'w') as f:
                json.dump(self.config.get_all(), f, indent=2)
            print(f"✅ Configuration saved to: {config_file}")
        
        print(f"\n✅ Memory systems demonstration completed successfully!")

def main():
    demo = MemorySystemsDemo()
    demo.run_memory_demonstration()

if __name__ == "__main__":
    main()
"""

    def create_example(self, name: str, template: str = "getting_started"):
        """Create an example from template."""
        if template not in self.templates:
            print(f"Unknown template: {template}")
            return False
        
        example_content = self.templates[template]
        example_path = self.examples_dir / f"{name}.py"
        
        with open(example_path, 'w') as f:
            f.write(example_content)
        
        # Make executable
        os.chmod(example_path, 0o755)
        
        print(f"✅ Created example: {example_path}")
        return True
    
    def list_templates(self):
        """List available templates."""
        print("Available templates:")
        for name in sorted(self.templates.keys()):
            print(f"  - {name.replace('_', ' ').title()}")
    
    def create_all_examples(self):
        """Create all examples from all templates."""
        print("🔧 Creating all examples...")
        
        example_configs = [
            ("getting_started", "getting_started"),
            ("basic_brain", "basic_brain"),
            ("advanced_agent", "advanced_agent"),
            ("memory_systems", "memory_systems"),
            ("neuromodulation", "neuromodulation"),
            ("development", "development"),
            ("plasticity", "plasticity"),
            ("prediction", "prediction"),
            ("performance_tuning", "performance_tuning"),
            ("research_study", "research_study"),
            ("production_use", "production_use"),
            ("visualization", "visualization"),
            ("benchmarking", "benchmarking"),
        ]
        
        created = 0
        for name, template in example_configs:
            if self.create_example(name, template):
                created += 1
        
        print(f"\n✅ Created {created}/{len(example_configs)} examples")
        return created

class ExampleManager:
    def __init__(self, generator: ExamplesGenerator):
        self.generator = generator

    def create_example(self, args: argparse.Namespace):
        """Create a single example."""
        if not self.generator.create_example(args.name, args.template):
            return False

    def create_all(self):
        """Create all examples."""
        return self.generator.create_all_examples()

    def list_templates(self):
        """List available templates."""
        self.generator.list_templates()

def main():
    generator = ExamplesGenerator()
    example_manager = ExampleManager(generator)

    parser = argparse.ArgumentParser(
        description="NLM Examples Generator",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Create single example
    create_parser = subparsers.add_parser("create", help="Create single example")
    create_parser.add_argument("name", help="Example name")
    create_parser.add_argument("template", nargs="?", default="getting_started",
                              help="Example template")

    # Create all examples
    subparsers.add_parser("create-all", help="Create all examples")

    # List templates
    subparsers.add_parser("list", help="List available templates")

    args = parser.parse_args()

    if args.command == "create":
        example_manager.create_example(args)
    elif args.command == "create-all":
        example_manager.create_all()
    elif args.command == "list":
        example_manager.list_templates()
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
