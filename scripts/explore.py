#!/usr/bin/env python3
"""
NLM Explore Script
Interactive exploration of NLM capabilities and features
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import pynlm

def explore_brain_features():
    """Explore brain features and capabilities"""
    print("\n=== Brain Features Explorer ===")
    
    # Create brain with different configurations
    configs = [
        ("Default", pynlm.createDefaultConfig()),
        ("Performance", pynlm.createPerformanceConfig()),
        ("Research", pynlm.createResearchConfig()),
    ]
    
    for name, config in configs:
        print(f"\n{name} Configuration:")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Regions: {brain.getRegionCount()}")
        print(f"  Developmental Stage: {brain.getDevelopmentalStage()}")
        
        # Quick simulation
        for step in range(20):
            brain.step(step)
        
        print(f"  After 20 steps: {brain.getTotalSpikeCount()} spikes")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Active neurons: {brain.getFiringNeuronCount()}")

def explore_agent_features():
    """Explore agent features and capabilities"""
    print("\n=== Agent Features Explorer ===")
    
    # Create agent with different feature combinations
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=8, height=8, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Explore different agent configurations
    feature_combinations = [
        ("None", []),
        ("Reward Only", ["reward"]),
        ("Curiosity Only", ["curiosity"]),
        ("Development Only", ["development"]),
        ("All Features", ["reward", "curiosity", "development"]),
    ]
    
    for name, features in feature_combinations:
        print(f"\n{name} Agent:")
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for feature in features:
            if feature == "reward":
                agent.enableRewardModulation(True)
            elif feature == "curiosity":
                agent.enableCuriosity(True)
            elif feature == "development":
                agent.enableDevelopment(True)
        
        # Run simulation
        for step in range(15):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"  Total reward: {sum(world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime()).reward for _ in range(15)):.2f}")
        print(f"  Final developmental stage: {agent.getDevelopmentalStage()}")
        print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")

def explore_world_features():
    """Explore world features and capabilities"""
    print("\n=== World Features Explorer ===")
    
    # Create world with different configurations
    world_configs = [
        ("Small", (6, 6, 4, 4)),
        ("Medium", (10, 10, 8, 8)),
        ("Large", (16, 16, 12, 12)),
        ("Extra Large", (20, 20, 16, 16)),
    ]
    
    for name, (width, height, vision_width, vision_height) in world_configs:
        print(f"\n{name} World ({width}x{height}):")
        world = pynlm.createSimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        world.reset()
        
        print(f"  Vision capability: {vision_width}x{vision_height}")
        print(f"  Max energy: {world.getMaxEnergy()}")
        print(f"  Energy decay rate: {world.getEnergyDecayRate()}")
        
        # Add some objects
        world.addObject(pynlm.WorldObject(x=3, y=3, type=pynlm.WorldObjectType.Resource, value=100.0, radius=0.8))
        world.addObject(pynlm.WorldObject(x=7, y=7, type=pynlm.WorldObjectType.Hazard, value=50.0, radius=0.6))
        world.addObject(pynlm.WorldObject(x=15, y=5, type=pynlm.WorldObjectType.Marker, value=1.0, radius=0.3))
        
        # Simulate agent in world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for step in range(10):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"  Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"  Final agent energy: {world.getAgentBody().energy:.1f}")

def explore_configuration_wizard():
    """Interactive configuration wizard"""
    print("\n=== Configuration Wizard ===")
    
    print("\nStep 1: Choose simulation type:")
    print("  1 - Simple brain demo")
    print("  2 - Agent in world")
    print("  3 - Performance testing")
    print("  4 - Research simulation")
    
    choice = input("Enter choice (1-4): ").strip()
    
    if choice == "1":
        print("\nStep 2: Choose brain size:")
        print("  1 - Small (50 neurons)")
        print("  2 - Medium (200 neurons)")
        print("  3 - Large (500 neurons)")
        
        size_choice = input("Enter choice (1-3): ").strip()
        sizes = {"1": 50, "2": 200, "3": 500}
        
        if size_choice in sizes:
            brain = pynlm.createSimpleBrain(sizes[size_choice])
            brain.initialize()
            
            for i in range(25):
                brain.step(i)
            
            print(f"\nCreated brain with {brain.getTotalNeuronCount()} neurons")
            print(f"Generated {brain.getTotalSpikeCount()} spikes")
            print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    elif choice == "2":
        print("\nSetting up agent-world simulation...")
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentWithDefaults(brain, world)
        
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"\nSimulation completed!")
        print(f"Brain stats: {brain.getTotalSpikeCount()} spikes, {brain.getAverageFiringRate():.2f} Hz firing rate")
        print(f"Agent final stage: {agent.getDevelopmentalStage()}")
    
    elif choice == "3":
        print("\nRunning performance test...")
        brain = pynlm.createBrain(pynlm.createPerformanceConfig())
        brain.initialize()
        
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        elapsed = time.time() - start_time
        
        print(f"\nPerformance test completed in {elapsed:.3f} seconds")
        print(f"Speed: {100/elapsed:.1f} steps/second")
        print(f"Performance brain stats: {brain.getTotalSpikeCount()} spikes")
    
    elif choice == "4":
        print("\nSetting up research simulation...")
        brain = pynlm.createBrain(pynlm.createResearchConfig())
        brain.initialize()
        
        print("Research configuration created with:")
        print("  - 5000 neurons")
        print("  - 8 regions")
        print("  - Advanced plasticity (synaptogenesis/pruning)")
        print("  - Custom random seed for reproducibility")
        
        # Show configuration help
        pynlm.printConfigHelp()
    
    else:
        print("\nInvalid choice. Please run again and select 1-4.")

def explore_help():
    """Interactive help system"""
    print("\n=== NLM Explore Help ===")
    
    topics = [
        "1. Basic Usage",
        "2. Configuration",
        "3. Agent Features",
        "4. World Features",
        "5. Performance Tips",
        "6. Troubleshooting",
        "7. Advanced Features",
    ]
    
    print("\nAvailable topics:")
    for topic in topics:
        print(f"  {topic}")
    
    print("\nExample commands:")
    print("  python explore.py --brain-features    # Explore brain capabilities")
    print("  python explore.py --agent-features    # Explore agent capabilities")
    print("  python explore.py --world-features    # Explore world capabilities")
    print("  python explore.py --wizard           # Interactive configuration")

def main():
    import argparse
    import time
    
    parser = argparse.ArgumentParser(description="Interactive NLM exploration tool")
    parser.add_argument("--brain-features", action="store_true", help="Explore brain features")
    parser.add_argument("--agent-features", action="store_true", help="Explore agent features")
    parser.add_argument("--world-features", action="store_true", help="Explore world features")
    parser.add_argument("--wizard", action="store_true", help="Run configuration wizard")
    parser.add_argument("--help", action="store_true", help="Show help information")
    
    args = parser.parse_args()
    
    if args.help:
        explore_help()
        return
    
    if args.brain_features:
        explore_brain_features()
    elif args.agent_features:
        explore_agent_features()
    elif args.world_features:
        explore_world_features()
    elif args.wizard:
        explore_configuration_wizard()
    else:
        print("\n=== NLM Explore ===")
        print("Available modes:")
        print("  --brain-features    Explore brain capabilities")
        print("  --agent-features    Explore agent capabilities")
        print("  --world-features    Explore world capabilities")
        print("  --wizard           Interactive configuration")
        print("  --help             Show help")
        
        print("\nTry exploring the different feature sets to learn about NLM!")

if __name__ == "__main__":
    main()
