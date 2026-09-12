#!/usr/bin/env python3
"""Additional Python examples for NLM (Neural Learning Machine)
This file provides advanced examples and convenience functions for working with NLM.
"""

import pynlm
import json
import os
from pathlib import Path

def create_example_configs():
    """Create example configuration files for different use cases"""
    
    # Create example configs directory
    config_dir = Path("configs")
    config_dir.mkdir(exist_ok=True)
    
    # Basic configuration
    basic_config = {
        "neuron_count": 1000,
        "region_count": 1,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    }
    
    # Learning-focused configuration
    learning_config = {
        "neuron_count": 2000,
        "region_count": 2,
        "simulation_timestep": 0.001,
        "random_seed": 123,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.03,
        "stdp_ltd_weight": 0.02,
        "stdp_tau": 25.0,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002,
        "replay_interval": 50,
        "consolidation_interval": 500,
        "enable_reward_modulation": True,
        "enable_curiosity": True,
        "enable_development": True,
        "enable_structural_plasticity": True
    }
    
    # Save configurations
    with open(config_dir / "basic_config.json", "w") as f:
        json.dump(basic_config, f, indent=2)
    
    with open(config_dir / "learning_config.json", "w") as f:
        json.dump(learning_config, f, indent=2)
    
    print("Created example configurations in 'configs/' directory")

def run_complete_agent_tutorial():
    """Complete tutorial showing all NLM systems working together"""
    print("=== NLM Complete Agent Tutorial ===")
    print("\nThis tutorial demonstrates a full NLM agent with all systems integrated.")
    
    # Create brain with learning configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("\nInitial brain status:")
    brain.logStatus()
    
    print("\nRunning complete agent simulation...")
    
    # Run simulation
    for step in range(100):
        # World updates
        world.update(0.1)
        
        # Agent receives sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and learns
        brain.step(step)
        
        # Agent produces action
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        if step % 10 == 0:  # Check reward every 10 steps
            reward = 1.0 if result.success else -0.5
            predicted = 0.0  # Simple prediction
            agent.applyRewardModulation(reward, predicted)
        
        # Update development periodically
        if step % 50 == 0:
            agent.updateDevelopment(0.1)
        
        if step % 20 == 0:
            print(f"Step {step}: Action={action.getName()}, Reward={result.reward:.2f}, "
                  f"Dopamine={agent.getNeuromodulationLevel():.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}")
    
    print("\nFinal brain status:")
    brain.logStatus()
    
    print("\n=== Integration Results ===")
    print(f"Total steps: 100")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Final neuromodulation levels:")
    print(f"  Dopamine: {agent.getNeuromodulationLevel():.2f}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
    print(f"  Prediction error: {agent.getPredictionError():.2f}")

def run_memory_learning_demo():
    """Demonstrate memory-based learning and retention"""
    print("\n=== Memory Learning Demo ===")
    
    # Create brain with memory configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Episodic memory max episodes: {episodic_memory.getMaxEpisodes()}")
    
    # Simulate learning episodes
    for episode in range(5):
        print(f"\nEpisode {episode + 1}:")
        
        # Reset brain for new episode
        brain.reset()
        brain.initialize()
        
        # Run episode
        for step in range(50):
            brain.step(step)
            
            # The brain's spike activity is automatically stored
            if step % 10 == 0:
                active_traces = working_memory.getActiveTraces()
                episode_count = episodic_memory.getEpisodeCount()
                print(f"  Step {step}: {active_traces} working traces, {episode_count} episodes")
        
        # Show memory content
        print(f"  Memory summary:")
        print(f"    Working memory utilization: {active_traces}/{working_memory.getCapacity()}")
        print(f"    Episodic episodes stored: {episode_count}")
    
    print("\n=== Memory System Performance ===")
    print("Working memory demonstrates pattern capture and maintenance")
    print("Episodic memory shows experience retention and replay")
    print("This enables continual learning and memory-based decision making")

def run_prediction_learning_demo():
    """Demonstrate prediction-based learning"""
    print("\n=== Prediction Learning Demo ===")
    
    print("This demo shows how NLM uses prediction error for learning:")
    print("\nKey prediction concepts:")
    print("1. The brain generates internal models of the world")
    print("2. Prediction errors signal when models are incorrect")
    print("3. Prediction errors drive synaptic changes (learning)")
    print("4. Internal models improve over time through error reduction")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Show prediction system is available
    prediction_system = brain.getPredictionSystem()
    if prediction_system:
        print("\nPrediction system is integrated and functional")
    else:
        print("\nPrediction system would be integrated in Phase 6")
    
    print("\nPrediction error signals provide the basis for:")
    print("- Reward prediction errors (dopamine)")
    print("- Surprise/novelty detection (curiosity)")
    print("- Confidence estimation (attention)")
    print("- Memory consolidation (replay)")

def create_brain_templates():
    """Create template brain configurations for different applications"""
    print("\n=== Brain Template Generator ===")
    
    templates = {
        "simple_brain": {
            "description": "Minimal working brain for basic simulation",
            "neuron_count": 500,
            "region_count": 1,
            "enable_structural_plasticity": False,
            "enable_development": False,
            "learning_rate": "low"
        },
        "learning_brain": {
            "description": "Brain optimized for continual learning",
            "neuron_count": 2000,
            "region_count": 3,
            "enable_structural_plasticity": True,
            "enable_development": True,
            "learning_rate": "medium"
        },
        "expert_brain": {
            "description": "Highly capable brain with advanced learning",
            "neuron_count": 5000,
            "region_count": 5,
            "enable_structural_plasticity": True,
            "enable_development": True,
            "enable_curiosity": True,
            "learning_rate": "high"
        }
    }
    
    print("Available brain templates:")
    for name, details in templates.items():
        print(f"\n{name.replace('_', ' ').title()}:")
        print(f"  {details['description']}")
        print(f"  Neurons: {details['neuron_count']}")
        print(f"  Regions: {details['region_count']}")
        print(f"  Learning: {details['learning_rate']}")
    
    print("\nTo use a template, modify the configuration parameters")
    print("according to your specific application requirements.")

def main():
    """Main function to run all demos and utilities"""
    print("NLM Advanced Python Utilities")
    print("==============================")
    
    # Parse command line arguments
    import argparse
    parser = argparse.ArgumentParser(description="NLM Python utilities")
    parser.add_argument("--config-dir", action="store_true", 
                       help="Create example configuration files")
    parser.add_argument("--tutorial", action="store_true",
                       help="Run complete agent tutorial")
    parser.add_argument("--memory-demo", action="store_true",
                       help="Run memory learning demo")
    parser.add_argument("--prediction-demo", action="store_true",
                       help="Run prediction learning demo")
    parser.add_argument("--templates", action="store_true",
                       help="Show brain templates")
    parser.add_argument("--all", action="store_true",
                       help="Run all demonstrations")
    
    args = parser.parse_args()
    
    if args.config_dir or args.all:
        create_example_configs()
    
    if args.tutorial or args.all:
        run_complete_agent_tutorial()
    
    if args.memory_demo or args.all:
        run_memory_learning_demo()
    
    if args.prediction_demo or args.all:
        run_prediction_learning_demo()
    
    if args.templates or args.all:
        create_brain_templates()
    
    if not any([args.config_dir, args.tutorial, args.memory_demo, 
                args.prediction_demo, args.templates, args.all]):
        print("\nRunning all demonstrations by default...")
        print("\nTo run specific demonstrations, use one of:")
        print("  --config-dir: Create example configuration files")
        print("  --tutorial: Run complete agent tutorial")
        print("  --memory-demo: Run memory learning demo")
        print("  --prediction-demo: Run prediction learning demo")
        print("  --templates: Show brain templates")
        print("  --all: Run all demonstrations")
        print("\nRunning all demonstrations:")
        create_example_configs()
        run_complete_agent_tutorial()
        run_memory_learning_demo()
        run_prediction_learning_demo()
        create_brain_templates()

if __name__ == "__main__":
    main()
