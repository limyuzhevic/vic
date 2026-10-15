"""
Exploration and demonstration tools for NLM Python bindings.

This module provides interactive tools for exploring NLM capabilities
and running demonstrations of the neural simulation framework.
"""

import sys
import os
import time
import argparse
from typing import Dict, Any, List

try:
    import pynlm
    from pynlm import (
        createDefaultConfig, 
        createBrain, 
        createAgentBrain, 
        createSimpleWorld
    )
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False


def main():
    """Main CLI entry point for explore."""
    parser = argparse.ArgumentParser(
        description="NLM Explore - Interactive exploration of NLM capabilities",
        prog="nlm-explore"
    )
    
    parser.add_argument(
        "--mode", "-m",
        choices=["interactive", "demo", "benchmark"],
        default="interactive",
        help="Exploration mode"
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=100,
        help="Number of simulation steps"
    )
    
    parser.add_argument(
        "--neurons", "-n",
        type=int,
        default=1000,
        help="Number of neurons"
    )
    
    parser.add_argument(
        "--headless", "-h",
        action="store_true",
        help="Run in headless mode (no interactive output)"
    )
    
    parser.add_argument(
        "--plot", "-p",
        action="store_true",
        help="Generate plots (requires matplotlib)"
    )
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available.")
        sys.exit(1)
    
    try:
        if args.mode == "interactive":
            interactive_explore(args)
        elif args.mode == "demo":
            run_demo(args)
        elif args.mode == "benchmark":
            run_benchmark(args)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


def interactive_explore(args):
    """Interactive exploration mode."""
    print("=" * 60)
    print("NLM Interactive Exploration Mode")
    print("=" * 60)
    print("\nWelcome to NLM (Neural Learning Machine) exploration!")
    print("\nThis mode allows you to interactively explore brain behavior")
    print("by modifying parameters and observing the results.")
    print("\nAvailable commands:")
    print("  help     - Show this help message")
    print("  quit     - Exit exploration mode")
    print("  brain    - Display brain statistics")
    print("  step N   - Run N simulation steps")
    print("  config   - Display current configuration")
    print("  set      - Set a configuration parameter")
    print("  mem      - Show memory system status")
    print("  plasticity - Show plasticity settings")
    print("  sensory  - Process sensory input")
    print("  motor    - Get motor commands")
    print("\nStarting with a fresh brain...")
    
    # Create and initialize brain
    config = createDefaultConfig()
    config.set("neuron_count", args.neurons)
    brain = createBrain(config)
    brain.initialize()
    
    if not args.headless:
        print(f"\nBrain created with {brain.getTotalNeuronCount()} neurons")
        print(f"Brain has {brain.getRegionCount()} regions")
    
    # Interactive loop
    step_count = 0
    running = True
    
    while running:
        if not args.headless:
            print(f"\n--- Step {step_count} ---")
            print(f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Rate: {brain.getAverageFiringRate():.4f}")
            
            print("\nEnter command: ", end="")
        
        try:
            if args.headless:
                # Auto-run in headless mode
                for i in range(min(args.steps, 10)):
                    brain.step(step_count + i)
                step_count += 10
                if step_count >= args.steps:
                    break
                continue
            
            command = input().strip().lower()
            
            if command == "help":
                show_help()
            elif command == "quit":
                running = False
            elif command == "brain":
                show_brain_stats(brain)
            elif command == "step":
                run_steps(brain, args, step_count)
                step_count += args.steps if args.steps > 0 else 1
            elif command == "config":
                show_config(brain)
            elif command == "mem":
                show_memory_status(brain)
            elif command == "plasticity":
                show_plasticity_status(brain)
            elif command == "sensory":
                process_sensory_input(brain)
            elif command == "motor":
                get_motor_commands(brain)
            elif command.startswith("set "):
                parts = command.split()
                if len(parts) >= 3:
                    key = parts[1]
                    value = parts[2]
                    try:
                        if key in ["neuron_count", "region_count", "seed"]:
                            value = int(value)
                        elif key in ["connection_probability", "plasticity_learning_rate"]:
                            value = float(value)
                        elif key in ["log_to_file"]:
                            value = value.lower() == "true"
                        config.set(key, value)
                        print(f"  Configuration '{key}' set to '{value}'")
                    except ValueError:
                        print(f"  Error: Invalid value for '{key}'")
            else:
                if command:
                    print(f"  Unknown command: '{command}'. Type 'help' for commands.")
                
        except KeyboardInterrupt:
            print("\nInterrupted by user.")
            running = False
        except Exception as e:
            print(f"  Error: {e}")
    
    print("\nExploration session ended.")


def run_demo(args):
    """Run demonstration mode."""
    print("=" * 60)
    print("NLM Demonstration Mode")
    print("=" * 60)
    
    demos = [
        ("Basic Brain", basic_brain_demo),
        ("Agent in World", agent_world_demo),
        ("Memory System", memory_demo),
        ("Plasticity Learning", plasticity_demo),
        ("Development", development_demo),
        ("Prediction System", prediction_demo),
    ]
    
    for demo_name, demo_func in demos:
        print(f"\n{'='*20} {demo_name} {'='*20}")
        try:
            demo_func(args)
        except Exception as e:
            print(f"  Error in {demo_name}: {e}")
        
        if args.headless:
            continue
        
        input("Press Enter to continue to next demo...")


def run_benchmark(args):
    """Run benchmark tests."""
    print("=" * 60)
    print("NLM Benchmark Mode")
    print("=" * 60)
    
    print(f"\nRunning benchmarks with {args.neurons} neurons, {args.steps} steps...")
    
    # Test different brain sizes
    sizes = [100, 500, 1000]
    
    for size in sizes:
        print(f"\nBenchmarking with {size} neurons...")
        
        config = createDefaultConfig()
        config.set("neuron_count", size)
        config.set("region_count", max(1, size // 200))
        
        brain = createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        for step in range(args.steps):
            brain.step(step)
        end_time = time.time()
        
        elapsed = end_time - start_time
        spikes_per_second = brain.getTotalSpikeCount() / elapsed if elapsed > 0 else 0
        
        print(f"  Time: {elapsed:.3f}s")
        print(f"  Spikes: {brain.getTotalSpikeCount()}")
        print(f"  Spikes/sec: {spikes_per_second:.0f}")
        print(f"  Avg firing rate: {brain.getAverageFiringRate():.4f}")


def basic_brain_demo(args):
    """Demonstrate basic brain functionality."""
    print("Creating a simple brain...")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    print(f"  Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"  and {brain.getTotalSynapseCount()} synapses")
    
    print("\nRunning basic simulation...")
    for step in range(args.steps):
        brain.step(step)
        
        if step % 100 == 0 and args.headless:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"\nSimulation completed!")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.4f}")


def agent_world_demo(args):
    """Demonstrate agent in world interaction."""
    print("Creating agent in world...")
    
    # Create brain, world, and agent
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Agent vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    print("\nRunning agent simulation...")
    for step in range(args.steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0 and args.headless:
            print(f"  Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Time: {world.getSimulationTime():.2f}s")
    
    print(f"\nAgent simulation completed!")
    print(f"  Simulation time: {world.getSimulationTime():.2f}s")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.4f}")


def memory_demo(args):
    """Demonstrate memory systems."""
    print("Testing memory systems...")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Get memory systems
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    associative_mem = brain.getAssociativeMemory()
    
    print(f"  Working memory configured: {working_mem is not None}")
    print(f"  Episodic memory configured: {episodic_mem is not None}")
    print(f"  Associative memory configured: {associative_mem is not None}")
    
    # Test with some neural activity
    for step in range(200):
        brain.step(step)
        
        # Store some patterns in working memory
        if step % 50 == 0:
            working_mem.store(f"pattern_{step}", step * 0.1)
    
    # Show memory status
    if working_mem:
        print(f"  Working memory patterns: {working_mem.getSize()}")
        if working_mem.getSize() > 0:
            pattern = list(working_mem.getPatterns())[0]
            value = working_mem.retrieve(pattern)
            print(f"  Sample pattern '{pattern}': value = {value}")
    
    print("\nMemory systems test completed!")


def plasticity_demo(args):
    """Demonstrate plasticity learning."""
    print("Demonstrating plasticity and learning...")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Enable plasticity
    if brain.getSTDP():
        brain.getSTDP()->enable(true)
    if brain.getHebbian():
        brain.getHebbian()->enable(true)
    
    print("  Plasticity systems enabled")
    
    # Run learning simulation
    print("\nRunning learning simulation...")
    for step in range(args.steps):
        # Inject correlated input patterns
        if step % 10 == 0:
            brain.injectCurrentToNeurons("Sensory", 40.0)
        
        brain.step(step)
        
        if step % 200 == 0 and args.headless:
            print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes")
    
    print(f"\nLearning simulation completed!")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.4f}")


def development_demo(args):
    """Demonstrate development system."""
    print("Testing development system...")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    dev_system = brain.getDevelopmentSystem()
    if dev_system:
        print(f"  Development system active")
        print(f"  Initial stage: {brain.getDevelopmentalStage()}")
        
        # Simulate development over time
        for step in range(100):
            brain.step(step)
            
            if step % 25 == 0:
                print(f"  Step {step}: Stage {brain.getDevelopmentalStage()}, "
                      f"Maturation: {dev_system.getMaturationProgress():.2f}")
        
        print(f"  Final stage: {brain.getDevelopmentalStage()}")
        print(f"  Mature: {dev_system.isMature()}")
    else:
        print("  No development system found")


def prediction_demo(args):
    """Demonstrate prediction system."""
    print("Testing prediction system...")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    pred_system = brain.getPredictionSystem()
    if pred_system:
        print(f"  Prediction system active")
        
        # Run simulation to build predictions
        for step in range(150):
            brain.step(step)
            
            if step % 50 == 0 and args.headless:
                print(f"  Step {step}: "
                      f"Prediction error: {pred_system.getPredictionError():.4f}, "
                      f"Confidence: {pred_system.getConfidence():.4f}")
        
        print(f"\nFinal prediction statistics:")
        print(f"  Prediction error: {pred_system.getPredictionError():.4f}")
        print(f"  Confidence: {pred_system.getConfidence():.4f}")
        print(f"  System size: {pred_system.getSize()}")
    else:
        print("  No prediction system found")


def show_help():
    """Show help message."""
    print("\n" + "="*60)
    print("NLM Interactive Exploration Help")
    print("="*60)
    print("\nAvailable Commands:")
    print("  help     - Show this help message")
    print("  quit     - Exit exploration mode")
    print("  brain    - Display brain statistics")
    print("  step N   - Run N simulation steps")
    print("  config   - Display current configuration")
    print("  set      - Set a configuration parameter")
    print("  mem      - Show memory system status")
    print("  plasticity - Show plasticity settings")
    print("  sensory  - Process sensory input")
    print("  motor    - Get motor commands")
    print("\nBrain Statistics Include:")
    print("  - Total neurons, synapses, spikes")
    print("  - Active and firing neuron counts")
    print("  - Average firing rate")
    print("  - Excitation/inhibition ratio")
    print("\nConfiguration Parameters:")
    print("  - neuron_count: Number of neurons")
    print("  - region_count: Number of brain regions")
    print("  - connection_probability: Connection density")
    print("  - plasticity_learning_rate: Learning rate")
    print("  - stdp_ltp_weight: STDP LTP weight")
    print("  - stdp_ltd_weight: STDP LTD weight")


def show_brain_stats(brain):
    """Display brain statistics."""
    print("\n" + "="*60)
    print("BRAIN STATISTICS")
    print("="*60)
    print(f"Total Neurons: {brain.getTotalNeuronCount()}")
    print(f"Total Synapses: {brain.getTotalSynapseCount()}")
    print(f"Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"Active Neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing Neurons: {brain.getFiringNeuronCount()}")
    print(f"Average Firing Rate: {brain.getAverageFiringRate():.4f}")
    print(f"Excitation/Inhibition Ratio: {brain.getExcitationInhibitionRatio():.4f}")
    print(f"Pending Spike Events: {brain.getPendingSpikeEventCount()}")
    print(f"Developmental Stage: {brain.getDevelopmentalStage()}")
    print("="*60)


def show_config(brain):
    """Display configuration."""
    print("\n" + "="*60)
    print("CONFIGURATION")
    print("="*60)
    
    config = brain.getConfig()
    if config:
        keys = config.getKeys()
        for key in sorted(keys):
            try:
                value = config.getOr(key, "N/A")
                print(f"{key}: {value}")
            except:
                print(f"{key}: <unable to retrieve>")
    print("="*60)


def show_memory_status(brain):
    """Display memory system status."""
    print("\n" + "="*60)
    print("MEMORY SYSTEMS")
    print("="*60)
    
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    associative_mem = brain.getAssociativeMemory()
    
    if working_mem:
        print(f"Working Memory: {working_mem.getSize()} patterns")
    if episodic_mem:
        print(f"Episodic Memory: {episodic_mem.getSize()} episodes")
        print(f"  Time range: {episodic_mem.getTimestampRange()}")
    if associative_mem:
        print(f"Associative Memory: {associative_mem.getSize()} associations")
    
    print("="*60)


def show_plasticity_status(brain):
    """Display plasticity status."""
    print("\n" + "="*60)
    print("PLASTICITY SYSTEMS")
    print("="*60)
    
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    
    if stdp:
        print(f"STDP: {'Enabled' if stdp.isEnabled() else 'Disabled'}")
        print(f"  LTP weight: {stdp.getLTPWeight():.4f}")
        print(f"  LTD weight: {stdp.getLTDWeight():.4f}")
    if hebbian:
        print(f"Hebbian: {'Enabled' if hebbian.isEnabled() else 'Disabled'}")
        print(f"  Learning rate: {hebbian.getLearningRate():.4f}")
    if structural:
        print(f"Structural Plasticity: {'Enabled' if structural.isEnabled() else 'Disabled'}")
    
    print("="*60)


def run_steps(brain, args, current_step):
    """Run simulation steps."""
    print(f"\nRunning {args.steps} simulation steps...")
    
    for step in range(current_step, current_step + args.steps):
        brain.step(step)
        
        if args.verbose:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")


def process_sensory_input(brain):
    """Process sensory input."""
    print("\nProcessing sensory input...")
    
    # Create a simple sensory percept
    percept = {
        "vision": [0.1, 0.5, 0.8, 0.3, 0.2],
        "touch": [0.0, 0.2, 0.1, 0.0, 0.3],
        "internal": [0.5, 0.7],
        "proprioception": [0.3, 0.4, 0.2],
        "audio": [0.0, 0.1, 0.05]
    }
    
    # Inject current based on sensory input
    if "vision" in percept:
        for i, intensity in enumerate(percept["vision"]):
            if intensity > 0.1:
                brain.injectCurrent(i, intensity * 100)
    
    print("  Sensory input processed")


def get_motor_commands(brain):
    """Get motor commands."""
    print("\nGenerating motor commands...")
    
    action = brain.produceAction()
    if action:
        print(f"  Motor command generated")
        print(f"  Forward: {action.getForward():.4f}")
        print(f"  Turn Left: {action.getTurnLeft():.4f}")
        print(f"  Turn Right: {action.getTurnRight():.4f}")
        print(f"  Interact: {action.getInteract():.4f}")
        print(f"  Wait: {action.getWait():.4f}")
    else:
        print("  No motor command generated")


if __name__ == "__main__":
    main()
