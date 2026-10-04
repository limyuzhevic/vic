#!/usr/bin/env python3
"""
NLM Python CLI Entry Point
Command-line interface for NLM (Neural Learning Machine)
"""

import sys
import os
import argparse
import json
from pathlib import Path

try:
    import pynlm as nlm
except ImportError as e:
    print(f"Error: Failed to import pynlm: {e}")
    print("Please ensure the pynlm package is properly installed.")
    sys.exit(1)
def run_demo(brain_size=500, world_size=100, steps=1000):
    """Run a complete learning demonstration"""
    print("=" * 60)
    print("NLM Advanced Learning Demonstration")
    print("=" * 60)
    
    # Create configuration
    config = nlm.createDefaultConfig()
    print(f"Configuration created with {brain_size} neurons")
    
    # Create brain
    brain = nlm.createBrain(config)
    brain.initialize()
    print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(world_size, world_size, 10, 10)
    world.setAgentStart(world_size//2, world_size//2)
    world.reset()
    print(f"World created: {world_size}x{world_size} with vision 10x10")
    
    # Create agent brain interface
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced features enabled: Reward modulation, Structural plasticity, Development, Curiosity")
    print()
    
    # Run learning simulation
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step, step * 0.1)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation based on action results
        if step % 10 == 0:
            reward = (world.getAgentBody().energy > 50.0) ? 1.0 : -0.1
            predictedReward = agent.getNeuromodulationLevel()
            agent.applyRewardModulation(reward, predictedReward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step:4d}: {brain.getFiringNeuronCount():4d} firing, "
                  f"{brain.getTotalSpikeCount():5d} spikes, "
                  f"Dev: {int(agent.getDevelopmentalStage()):2d}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    print()
    print("Learning demonstration completed!")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental stage: {int(agent.getDevelopmentalStage())}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    print()
    print("Integration status:", agent.getPredictionError())
    
    return brain, world, agent
def run_cli():
    """Command-line interface for NLM operations"""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) - Advanced Neural Simulation Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-python demo --brain-size 1000 --world-size 200 --steps 500
  nlm-python explore --model-file brain.nlm
  nlm-python config --create my_config.json
  nlm-python benchmark --iterations 10
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run learning demonstration")
    demo_parser.add_argument("--brain-size", type=int, default=500, help="Number of neurons in brain")
    demo_parser.add_argument("--world-size", type=int, default=100, help="Size of square world")
    demo_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    demo_parser.add_argument("--output-file", help="Save brain state to file")
    
    # Config command
    config_parser = subparsers.add_parser("config", help="Manage configurations")
    config_subparsers = config_parser.add_subparsers(dest="config_action")
    
    create_config = config_subparsers.add_parser("create", help="Create new configuration")
    create_config.add_argument("name", help="Configuration name")
    create_config.add_argument("--output", help="Output file path")
    
    # Explore command
    explore_parser = subparsers.add_parser("explore", help="Explore and analyze models")
    explore_parser.add_argument("model-file", help="Path to brain model file")
    explore_parser.add_argument("--stats", action="store_true", help="Show detailed statistics")
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser("benchmark", help="Run performance benchmarks")
    benchmark_parser.add_argument("--iterations", type=int, default=5, help="Number of benchmark iterations")
    benchmark_parser.add_argument("--brain-size", type=int, default=1000, help="Brain size for benchmarking")
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    # Execute commands
    try:
        if args.command == "demo":
            brain, world, agent = run_demo(args.brain_size, args.world_size, args.steps)
            
            if args.output_file:
                brain.save(args.output_file)
                print(f"Brain state saved to: {args.output_file}")
                
        elif args.command == "config":
            if args.config_action == "create":
                config = nlm.createDefaultConfig()
                if args.output:
                    with open(args.output, 'w') as f:
                        # Note: Config doesn't have a JSON serialization method
                        # This is a placeholder for future enhancement
                        f.write("# NLM Configuration\n")
                        f.write(f"# Created: {nlm.getSystemInfo()}\n")
                else:
                    print(config.summary())
                    
        elif args.command == "explore":
            if Path(args.model_file).exists():
                brain = nlm.load(args.model_file)
                print(f"Loaded brain from: {args.model_file}")
                print(f"Brain statistics: {brain.getLearningStatistics()}")
                
                if args.stats:
                    print("\nDetailed Statistics:")
                    stats = brain.getComprehensiveStats()
                    for key, value in stats.items():
                        print(f"  {key}: {value}")
            else:
                print(f"Error: Model file not found: {args.model_file}")
                
        elif args.command == "benchmark":
            print("Running NLM performance benchmark...")
            # Placeholder for benchmark implementation
            print(f"Benchmark configured for {args.iterations} iterations with {args.brain_size} neurons")
            
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
if __name__ == "__main__":
    run_cli()
