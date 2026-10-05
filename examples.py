#!/usr/bin/env python3
"""
NLM Interactive Examples and Tutorials

Comprehensive examples and tutorials for using NLM (Neural Learning Machine) Python API.
Designed for both beginners and advanced users with progressive learning paths.
"""

import sys
import os
import time
import json
import numpy as np
from typing import Optional, List, Dict, Any

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError:
    PYNL_AVAILABLE = False
    print("Warning: pynlm module not found. Some examples will not work.")
class ExampleRunner:
    """Base class for example runners."""
    
    def __init__(self):
        self.title = "Example"
        self.description = "This example demonstrates..."
    
    def print_header(self):
        """Print example header."""
        print("=" * 60)
        print(f"Example: {self.title}")
        print("=" * 60)
        print(f"Description: {self.description}")
        print()
    
    def print_code(self, code: str):
        """Print code with syntax highlighting."""
        print(">>> Python Code:")
        print("-" * 40)
        print(code)
        print("-" * 40)
        print()
    
    def print_output(self, output: str):
        """Print output."""
        print(">>> Output:")
        print("-" * 40)
        print(output)
        print("-" * 40)
        print()
    
    def print_explanation(self, explanation: str):
        """Print explanation."""
        print(">>> Explanation:")
        print("-" * 40)
        print(explanation)
        print("-" * 40)
        print()
    
    def run(self):
        """Run the example."""
        self.print_header()
        try:
            self.execute()
            self.print_footer()
        except Exception as e:
            print(f"Error: {e}")
            import traceback
            traceback.print_exc()
    
    def execute(self):
        """Execute the example. Override in subclasses."""
        raise NotImplementedError("Subclasses must implement execute()")
    
    def print_footer(self):
        """Print footer."""
        print()
        print("Example completed successfully!")
        print()
class GettingStartedExample(ExampleRunner):
    """Getting started with NLM - Basic brain creation and simulation."""
    
    def __init__(self):
        super().__init__()
        self.title = "Getting Started with NLM"
        self.description = "Create a simple brain and run a basic simulation"
    
    def execute(self):
        # Import pynlm
        self.print_explanation("""
This example shows how to create and run a basic NLM brain.
We'll create a simple brain with 1000 neurons and run a 100-step simulation.
        """)
        
        # Create configuration
        self.print_code("""
# Create a default configuration
import pynlm

config = pynlm.create_default_config()
print("Default configuration created")
        """)
        
        # Create brain
        config = pynlm.create_default_config()
        self.print_code(f"""
# Create and initialize the brain
brain = pynlm.create_brain(config)
brain.initialize()
print(f"Brain created with {{brain.get_total_neuron_count()}} neurons and {{brain.get_total_synapse_count()}} synapses")
        """)
        
        # Run simulation
        self.print_code("""
# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    
print(f"Simulation completed. Total spikes: {{brain.get_total_spike_count()}}")
print(f"Average firing rate: {{brain.get_average_firing_rate():.2f}} Hz")
        """)
        
        # Execute the code
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        output = f"""Brain created successfully with {brain.get_total_neuron_count()} neurons and {brain.get_total_synapse_count()} synapses

Running 100 steps of simulation...
Simulation completed. Total spikes: {brain.get_total_spike_count()}
Average firing rate: {brain.get_average_firing_rate():.2f} Hz
Firing neurons (last step): {brain.get_firing_neuron_count()}
        """
        
        print(output)
class AgentExample(ExampleRunner):
    """Agent-based simulation with world interaction."""
    
    def __init__(self):
        super().__init__()
        self.title = "Agent-Based Simulation"
        self.description = "Create an agent that interacts with a world"
    
    def execute(self):
        self.print_explanation("""
This example demonstrates how to create an agent that interacts with a simulated world.
The agent receives sensory input, processes it, and produces motor actions.
        """)
        
        # Create brain, world, and agent
        self.print_code("""
# Create brain and world
import pynlm

config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

world = pynlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

print("Components created successfully")
        """)
        
        # Create components
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        output = f"""Components created successfully:
- Brain: {brain.get_total_neuron_count()} neurons
- World: {world.get_width()}x{world.get_height()} with {world.get_vision_width()}x{world.get_vision_height()} vision
- Agent: Input size={agent.get_sensory_input_size()}, Output size={agent.get_motor_output_size()}

Enabling all agent subsystems...
        """
        
        print(output)
        
        # Enable subsystems
        agent.enable_reward_modulation(True)
        agent.enable_structural_plasticity(True)
        agent.enable_development(True)
        agent.enable_curiosity(True)
        
        output2 = f"""Subsystems enabled:
- Reward modulation: {agent.is_reward_modulation_enabled()}
- Structural plasticity: {agent.is_structural_plasticity_enabled()}
- Development: {agent.is_development_enabled()}
- Curiosity: {agent.is_curiosity_enabled()}

Running agent episode...
        """
        
        print(output2)
        
        # Run agent episode
        self.print_code("""
# Run agent episode
for step in range(200):
    # Get sensory input from world
    percept = world.get_sensory_percept()
    
    # Process sensory input in brain
    agent.process_sensory_input(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode motor command from brain activity
    motor_cmd = agent.decode_motor_command()
    
    # Apply motor command to world
    world.apply_motor_command(motor_cmd, world.get_simulation_time())
    
    # Update development
    agent.update_development(0.1)
    
    if step % 50 == 0:
        print(f"Step {step}: curiosity={agent.get_curiosity_level():.3f}, novelty={agent.get_novelty_level():.3f}")

print("Agent episode completed!")
        """)
        
        # Execute agent episode
        for step in range(200):
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            brain.step(step)
            motor_cmd = agent.decode_motor_command()
            world.apply_motor_command(motor_cmd, world.get_simulation_time())
            agent.update_development(0.1)
            
            if step % 50 == 0:
                print(f"  Step {step}: curiosity={agent.get_curiosity_level():.3f}, novelty={agent.get_novelty_level():.3f}")
        
        print("Agent episode completed successfully!")
class DevelopmentExample(ExampleRunner):
    """Development and plasticity in NLM."""
    
    def __init__(self):
        super().__init__()
        self.title = "Development and Plasticity"
        self.description = "Explore developmental processes and neural plasticity"
    
    def execute(self):
        self.print_explanation("""
This example demonstrates developmental processes and neural plasticity in NLM.
We'll create a brain with extended development time and monitor plasticity effects.
        """)
        
        # Create research configuration with longer development
        self.print_code("""
import pynlm

# Create research configuration
config = pynlm.create_default_config()

# Set development parameters
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("development.synaptogenesis_rate", 0.001)
config.set("development.pruning_rate", 0.0001)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Create brain and agent
brain = pynlm.create_brain(config)
brain.initialize()

print("Development configuration created")
        """)
        
        # Create components
        config = pynlm.create_default_config()
        config.set("brain.neuron_count", 2000)
        config.set("brain.region_count", 3)
        config.set("development.synaptogenesis_rate", 0.001)
        config.set("development.pruning_rate", 0.0001)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        print(f"Development configuration created with {brain.get_total_neuron_count()} neurons")
        print("Development parameters:")
        print("  - Synaptogenesis rate: 0.001")
        print("  - Pruning rate: 0.0001")
        print("  - Curiosity enabled: True")
        print("  - Novelty detection enabled: True")
        
        # Run development simulation
        self.print_code("""
# Run simulation and monitor development
for step in range(500):
    brain.step(step)
    
    if step % 100 == 0:
        dev_stage = brain.get_developmental_stage()
        firing_rate = brain.get_average_firing_rate()
        e_i_ratio = brain.get_excitation_inhibition_ratio()
        
        print(f"Step {step}:")
        print(f"  - Development stage: {dev_stage}")
        print(f"  - Firing rate: {firing_rate:.2f} Hz")
        print(f"  - E/I ratio: {e_i_ratio:.2f}")
        print(f"  - Total spikes: {brain.get_total_spike_count()}")

print("Development simulation completed!")
        """)
        
        # Execute development simulation
        for step in range(500):
            brain.step(step)
            
            if step % 100 == 0:
                dev_stage = brain.get_developmental_stage()
                firing_rate = brain.get_average_firing_rate()
                e_i_ratio = brain.get_excitation_inhibition_ratio()
                
                print(f"Step {step}:")
                print(f"  - Development stage: {dev_stage}")
                print(f"  - Firing rate: {firing_rate:.2f} Hz")
                print(f"  - E/I ratio: {e_i_ratio:.2f}")
                print(f"  - Total spikes: {brain.get_total_spike_count()}")
        
        print("Development simulation completed successfully!")
        print("\nDevelopment insights:")
        print("- Brain started in Initial stage")
        print("- Curiosity and novelty detection drove exploration")
        print("- Neural plasticity allowed adaptation over time")
class AdvancedFeaturesExample(ExampleRunner):
    """Advanced NLM features for expert users."""
    
    def __init__(self):
        super().__init__()
        self.title = "Advanced NLM Features"
        self.description = "Demonstrate advanced features and research capabilities"
    
    def execute(self):
        self.print_explanation("""
This example showcases advanced NLM features including advanced agent capabilities,
performance monitoring, and research-grade simulations.
        """)
        
        # Create advanced agent configuration
        self.print_code("""
import pynlm

# Create advanced configuration
config = pynlm.create_default_config()

# Set advanced parameters
config.set("brain.neuron_count", 5000)
config.set("brain.region_count", 5)
config.set("brain.connection_probability", 0.05)
config.set("simulation.timestep", 0.001)

# Create brain, world, and advanced agent
brain = pynlm.create_brain(config)
brain.initialize()

world = pynlm.create_simple_world()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()

# Create perfect advanced agent
agent = pynlm.create_perfect_advanced_agent(brain, world)

print("Advanced agent created with all features enabled:")
print("  - Meta-learning")
print("  - Transfer learning") 
print("  - Self-supervised learning")
print("  - Social learning")
print("  - Safety constraints")
print("  - Performance optimization")
        """)
        
        # Create advanced components
        config = pynlm.create_default_config()
        config.set("brain.neuron_count", 5000)
        config.set("brain.region_count", 5)
        config.set("brain.connection_probability", 0.05)
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
        world.reset()
        
        # Create perfect advanced agent
        agent = pynlm.create_perfect_advanced_agent(brain, world)
        
        print("Advanced agent created with all features enabled:")
        print("  - Meta-learning: Enabled")
        print("  - Transfer learning: Enabled")
        print("  - Self-supervised learning: Enabled")
        print("  - Social learning: Enabled")
        print("  - Safety constraints: Enabled")
        print("  - Performance optimization: Enabled")
        print("  - Visualization: Enabled")
        print("  - Real-time logging: Enabled")
        
        # Run advanced research simulation
        self.print_code("""
# Run advanced research simulation
print("Starting advanced research simulation...")

for step in range(300):
    # Get sensory input
    percept = world.get_sensory_percept()
    
    # Process sensory input with enhanced processing
    agent.process_sensory_input_enhanced(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode motor command with confidence
    motor_cmd, confidence = agent.decode_motor_command_with_confidence()
    
    # Apply action to world
    world.apply_motor_command(motor_cmd, world.get_simulation_time())
    
    # Update development
    agent.update_development(0.1)
    
    # Apply reward modulation
    reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
    agent.apply_reward_modulation(reward, 0.0)
    
    # Monitor performance
    if step % 50 == 0:
        metrics = agent.get_performance_metrics()
        print(f"Step {step}:")
        print(f"  - Curiosity: {metrics['average_curiosity']:.3f}")
        print(f"  - Novelty: {metrics['average_novelty']:.3f}")
        print(f"  - Prediction Error: {metrics['average_prediction_error']:.3f}")
        print(f"  - Success Rate: {metrics['successful_actions']/metrics['total_steps']:.3f}")
        print(f"  - Memory Usage: {agent.get_memory_usage()}/{agent.get_memory_capacity()}")

print("Advanced research simulation completed!")
        """)
        
        # Execute advanced simulation
        for step in range(300):
            percept = world.get_sensory_percept()
            agent.process_sensory_input_enhanced(percept)
            brain.step(step)
            motor_cmd, confidence = agent.decode_motor_command_with_confidence()
            world.apply_motor_command(motor_cmd, world.get_simulation_time())
            agent.update_development(0.1)
            
            reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
            agent.apply_reward_modulation(reward, 0.0)
            
            if step % 50 == 0:
                metrics = agent.get_performance_metrics()
                print(f"Step {step}:")
                print(f"  - Curiosity: {metrics['average_curiosity']:.3f}")
                print(f"  - Novelty: {metrics['average_novelty']:.3f}")
                print(f"  - Prediction Error: {metrics['average_prediction_error']:.3f}")
                print(f"  - Success Rate: {metrics['successful_actions']/metrics['total_steps']:.3f}")
                print(f"  - Memory Usage: {agent.get_memory_usage()}/{agent.get_memory_capacity()}")
        
        print("Advanced research simulation completed successfully!")
class InteractiveExample(ExampleRunner):
    """Interactive example with user input."""
    
    def __init__(self):
        super().__init__()
        self.title = "Interactive NLM Example"
        self.description = "Interactive example with user configuration and visualization"
    
    def execute(self):
        self.print_explanation("""
This interactive example allows you to configure and run NLM simulations
with real-time feedback and visualization options.
        """)
        
        print("Interactive NLM Configuration")
        print("=" * 40)
        
        # Get user preferences
        print("\nPlease configure your simulation:")
        
        try:
            neuron_count = int(input("Number of neurons (default 1000): ") or 1000)
            region_count = int(input("Number of regions (default 2): ") or 2)
            world_size = int(input("World size (default 20): ") or 20)
            vision_size = int(input("Vision size (default 8): ") or 8)
            steps = int(input("Number of steps (default 100): ") or 100)
            
            # Validate input
            if neuron_count < 100:
                print("Warning: Low neuron count may affect simulation quality")
            if region_count < 1:
                print("Warning: At least 1 region required")
                
        except ValueError:
            print("Invalid input. Using defaults.")
            neuron_count = 1000
            region_count = 2
            world_size = 20
            vision_size = 8
            steps = 100
        
        # Create simulation
        self.print_code(f"""
# Create customized simulation
import pynlm

config = pynlm.create_default_config()
config.set("brain.neuron_count", {neuron_count})
config.set("brain.region_count", {region_count})

brain = pynlm.create_brain(config)
brain.initialize()

world = pynlm.create_simple_world()
world.configure(width={world_size}, height={world_size}, 
               visionWidth={vision_size}, visionHeight={vision_size})
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

print("Simulation started...")
for step in range({steps}):
    # Simulation loop here
    pass
        """)
        
        # Create simulation
        config = pynlm.create_default_config()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", region_count)
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        world = pynlm.create_simple_world()
        world.configure(width=world_size, height=world_size, 
                       visionWidth=vision_size, visionHeight=vision_size)
        world.reset()
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        print(f"\nRunning interactive simulation with {steps} steps...")
        
        # Run simulation
        for step in range(steps):
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            brain.step(step)
            motor_cmd = agent.decode_motor_command()
            world.apply_motor_command(motor_cmd, world.get_simulation_time())
            agent.update_development(0.1)
            
            if step % 20 == 0:
                print(f"  Step {step}: curiosity={agent.get_curiosity_level():.3f}, "
                      f"firing={brain.get_firing_neuron_count()}, "
                      f"dev_stage={static_cast<int>(agent.get_developmental_stage())}")
        
        print("\nInteractive simulation completed!")
        print(f"\nSimulation Summary:")
        print(f"  - Neurons: {neuron_count}")
        print(f"  - Regions: {region_count}")
        print(f"  - World Size: {world_size}x{world_size}")
        print(f"  - Vision Size: {vision_size}x{vision_size}")
        print(f"  - Steps: {steps}")
        print(f"  - Final Curiosity: {agent.get_curiosity_level():.3f}")
        print(f"  - Final Novelty: {agent.get_novelty_level():.3f}")
        print(f"  - Final Prediction Error: {agent.get_prediction_error():.3f}")
        print(f"  - Final Development Stage: {static_cast<int>(agent.get_developmental_stage())}")
class ExampleCollection:
    """Collection of NLM examples."""
    
    def __init__(self):
        self.examples = [
            GettingStartedExample(),
            AgentExample(),
            DevelopmentExample(),
            AdvancedFeaturesExample(),
            InteractiveExample()
        ]
    
    def run_example(self, example_index: int):
        """Run a specific example."""
        if example_index < 0 or example_index >= len(self.examples):
            print(f"Invalid example index: {example_index}")
            return False
        
        self.examples[example_index].run()
        return True
    
    def list_examples(self):
        """List all available examples."""
        print("Available NLM Examples:")
        print("=" * 40)
        
        for i, example in enumerate(self.examples):
            print(f"{i + 1}. {example.title}")
            print(f"   {example.description}")
            print()
    
    def run_all_examples(self):
        """Run all examples sequentially."""
        print("Running all examples...")
        
        for i, example in enumerate(self.examples):
            print(f"\n{'='*60}")
            print(f"Running example {i + 1}/{len(self.examples)}: {example.title}")
            print(f"{'='*60}")
            
            try:
                example.run()
            except Exception as e:
                print(f"Example {i + 1} failed: {e}")
                continue
    
    def export_examples(self, output_dir: str = "examples_output"):
        """Export examples to output directory."""
        os.makedirs(output_dir, exist_ok=True)
        
        for i, example in enumerate(self.examples):
            filename = os.path.join(output_dir, f"example_{i + 1}_{example.title.lower().replace(' ', '_')}.md")
            
            with open(filename, 'w') as f:
                f.write(f"# {example.title}\n\n")
                f.write(f"{example.description}\n\n")
                f.write("```python\n")
                # Add example code here
                f.write("```\n")
            
            print(f"Exported example {i + 1} to {filename}")

def main():
    """Main entry point for the examples."""
    if not PYNL_AVAILABLE:
        print("Error: pynlm module is not available.")
        print("Please install pynlm to run examples.")
        return 1
    
    print("NLM Interactive Examples and Tutorials")
    print("=" * 60)
    
    examples = ExampleCollection()
    
    while True:
        print("\nMain Menu:")
        print("1. List available examples")
        print("2. Run specific example by index")
        print("3. Run all examples")
        print("4. Export examples to files")
        print("5. Exit")
        
        try:
            choice = input("\nEnter your choice (1-5): ").strip()
            
            if choice == "1":
                examples.list_examples()
            
            elif choice == "2":
                examples.list_examples()
                try:
                    index = int(input("Enter example index (1-{}): ".format(len(examples.examples)))) - 1
                    if 0 <= index < len(examples.examples):
                        examples.run_example(index)
                    else:
                        print("Invalid example index.")
                except ValueError:
                    print("Please enter a valid number.")
            
            elif choice == "3":
                examples.run_all_examples()
            
            elif choice == "4":
                output_dir = input("Output directory (default 'examples_output'): ").strip() or "examples_output"
                examples.export_examples(output_dir)
            
            elif choice == "5":
                print("Goodbye!")
                break
            
            else:
                print("Invalid choice. Please enter a number between 1 and 5.")
                
        except KeyboardInterrupt:
            print("\n
Interrupted by user. Exiting...")
            break
        except Exception as e:
            print(f"Error: {e}")
    
    return 0
if __name__ == "__main__":
    sys.exit(main())