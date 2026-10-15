"""
Demonstration tools for NLM Python bindings.

This module provides ready-to-use demonstrations and examples
for the NLM neural simulation framework.
"""

import sys
import time
import argparse
from typing import Dict, Any, List

try:
    import pynlm
    from pynlm import (
        createDefaultConfig, 
        createBrain, 
        createAgentBrain, 
        createSimpleWorld,
        getVersion
    )
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False


def main():
    """Main CLI entry point for demo."""
    parser = argparse.ArgumentParser(
        description="NLM Demo - Ready-to-use demonstrations of NLM",
        prog="nlm-demo"
    )
    
    parser.add_argument(
        "demo", "-d",
        choices=["basic", "agent", "learning", "memory", "development", "prediction", "comprehensive"],
        default="basic",
        help="Demo to run"
    )
    
    parser.add_argument(
        "steps", "-s",
        type=int,
        default=100,
        help="Number of simulation steps"
    )
    
    parser.add_argument(
        "neurons", "-n",
        type=int,
        default=1000,
        help="Number of neurons"
    )
    
    parser.add_argument(
        "width", "-w",
        type=int,
        default=20,
        help="World width (for world demos)"
    )
    
    parser.add_argument(
        "height", "-h",
        type=int,
        default=20,
        help="World height (for world demos)"
    )
    
    parser.add_argument(
        "output", "-o",
        help="Output file for results (JSON)",
        default=None
    )
    
    parser.add_argument(
        "headless", "-H",
        action="store_true",
        help="Run in headless mode"
    )
    
    parser.add_argument(
        "verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "all", "-a",
        action="store_true",
        help="Run all demos"
    )
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: NLM Python bindings not available.")
        print("Install with: pip install pynlm")
        sys.exit(1)
    
    try:
        if args.all:
            run_all_demos(args)
        else:
            run_demo(args)
    except Exception as e:
        print(f"Error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        sys.exit(1)


def run_all_demos(args):
    """Run all available demos."""
    print("=" * 80)
    print("NLM DEMONSTRATION SUITE - Running All Demos")
    print("=" * 80)
    
    demos = [
        ("Basic Brain Simulation", basic_brain_demo, {}),
        ("Agent in Grid World", agent_world_demo, {"width": args.width, "height": args.height}),
        ("Memory Systems", memory_demo, {}),
        ("Plasticity Learning", plasticity_demo, {}),
        ("Development Stages", development_demo, {}),
        ("Prediction System", prediction_demo, {}),
        ("Comprehensive Integration", comprehensive_demo, {}),
    ]
    
    results = {}
    
    for demo_name, demo_func, demo_args in demos:
        print(f"\n{'='*80}")
        print(f"Running: {demo_name}")
        print(f"{'='*80}")
        
        try:
            result = demo_func(args.steps, args.neurons, **demo_args)
            results[demo_name] = result
            print(f"✓ {demo_name} completed successfully")
        except Exception as e:
            print(f"✗ {demo_name} failed: {e}")
            results[demo_name] = {"error": str(e)}
    
    # Summary
    print(f"\n{'='*80}")
    print("DEMONSTRATION SUMMARY")
    print(f"{'='*80}")
    
    for demo_name, result in results.items():
        if "error" in result:
            print(f"✗ {demo_name}: FAILED - {result['error']}")
        else:
            print(f"✓ {demo_name}: SUCCESS")
            if "total_spikes" in result:
                print(f"    Total spikes: {result['total_spikes']}")
            if "firing_rate" in result:
                print(f"    Final firing rate: {result['firing_rate']:.4f}")
            if "simulation_time" in result:
                print(f"    Simulation time: {result['simulation_time']:.2f}s")
    
    # Save combined results if requested
    if args.output:
        save_all_results(results, args.output)


def run_demo(args):
    """Run single demo."""
    demo_functions = {
        "basic": basic_brain_demo,
        "agent": agent_world_demo,
        "learning": plasticity_demo,
        "memory": memory_demo,
        "development": development_demo,
        "prediction": prediction_demo,
        "comprehensive": comprehensive_demo,
    }
    
    demo_func = demo_functions.get(args.demo)
    if not demo_func:
        print(f"Unknown demo: {args.demo}")
        return
    
    # Run demo with appropriate parameters
    if args.demo == "basic":
        demo_func(args.steps, args.neurons)
    elif args.demo == "agent":
        demo_func(args.steps, args.neurons, width=args.width, height=args.height)
    else:
        demo_func(args.steps, args.neurons)


def basic_brain_demo(steps, neurons):
    """Basic brain simulation demonstration."""
    print(f"\nInitializing brain with {neurons} neurons...")
    
    config = createDefaultConfig()
    config.set("neuron_count", neurons)
    config.set("simulation_timestep", 0.001)
    
    brain = createBrain(config)
    brain.initialize()
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons, {brain.getRegionCount()} regions")
    
    # Run simulation
    print(f"\nRunning simulation for {steps} steps...")
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 200 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"\nBasic Brain Demo Results:")
    print(f"  Simulation time: {elapsed:.3f}s")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"  Final stage: {brain.getDevelopmentalStage()}")
    
    return {
        "demo": "basic_brain",
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "simulation_time": elapsed,
        "neurons": neurons,
        "steps": steps
    }


def agent_world_demo(steps, neurons, width, height):
    """Agent in world interaction demonstration."""
    print(f"\nCreating agent in {width}x{height} world...")
    
    # Create brain
    config = createDefaultConfig()
    config.set("neuron_count", neurons)
    brain = createBrain(config)
    brain.initialize()
    
    # Create world
    world = createSimpleWorld()
    world.configure(width=width, height=height, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"  World dimensions: {width}x{height}")
    print(f"  Agent vision: 8x8 pixels")
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    print(f"\nRunning agent simulation for {steps} steps...")
    start_time = time.time()
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get and apply motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"  Step {step}: Time={world.getSimulationTime():.1f}s, "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Reward={agent.getNeuromodulationLevel():.3f}")
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"\nAgent in World Demo Results:")
    print(f"  Simulation time: {elapsed:.3f}s")
    print(f"  World time: {world.getSimulationTime():.2f}s")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    
    return {
        "demo": "agent_world",
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "simulation_time": elapsed,
        "world_time": world.getSimulationTime(),
        "neurons": neurons,
        "steps": steps,
        "neuromodulation": agent.getNeuromodulationLevel(),
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel()
    }


def memory_demo(steps, neurons):
    """Memory systems demonstration."""
    print(f"\nTesting memory systems with {neurons} neurons...")
    
    config = createDefaultConfig()
    config.set("neuron_count", neurons)
    
    brain = createBrain(config)
    brain.initialize()
    
    # Get memory systems
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    associative_mem = brain.getAssociativeMemory()
    
    print(f"  Working memory active: {working_mem is not None}")
    print(f"  Episodic memory active: {episodic_mem is not None}")
    print(f"  Associative memory active: {associative_mem is not None}")
    
    # Run simulation with memory operations
    print(f"\nRunning memory test for {steps} steps...")
    
    for step in range(steps):
        brain.step(step)
        
        # Store patterns in working memory
        if step % 50 == 0 and working_mem:
            working_mem.store(f"pattern_{step}", float(step) * 0.1)
        
        # Store episodes in episodic memory
        if step % 75 == 0 and episodic_mem:
            episodic_mem.store(f"episode_{step}", step * 0.001)
    
    # Show memory statistics
    print(f"\nMemory Demo Results:")
    if working_mem:
        print(f"  Working memory patterns stored: {working_mem.getSize()}")
        if working_mem.getSize() > 0:
            patterns = list(working_mem.getPatterns())
            last_pattern = patterns[-1]
            last_value = working_mem.retrieve(last_pattern)
            print(f"  Last pattern '{last_pattern}': value = {last_value:.3f}")
    
    if episodic_mem:
        print(f"  Episodic memory episodes stored: {episodic_mem.getSize()}")
        print(f"  Time range: {episodic_mem.getTimestampRange()}")
    
    print(f"  Total simulation spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.4f}")
    
    return {
        "demo": "memory",
        "working_memory_size": working_mem.getSize() if working_mem else 0,
        "episodic_memory_size": episodic_mem.getSize() if episodic_mem else 0,
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "neurons": neurons,
        "steps": steps
    }


def plasticity_demo(steps, neurons):
    """Plasticity learning demonstration."""
    print(f"\nDemonstrating plasticity with {neurons} neurons...")
    
    config = createDefaultConfig()
    config.set("neuron_count", neurons)
    
    brain = createBrain(config)
    brain.initialize()
    
    # Enable plasticity systems
    if brain.getSTDP():
        brain.getSTDP()->enable(true)
    if brain.getHebbian():
        brain.getHebbian()->enable(true)
    
    print("  Plasticity systems enabled")
    
    # Run learning simulation
    print(f"\nRunning plasticity learning for {steps} steps...")
    
    for step in range(steps):
        # Inject correlated input patterns to stimulate learning
        if step % 10 == 0:
            brain.injectCurrentToNeurons("Sensory", 40.0)
        
        brain.step(step)
        
        if step % 200 == 0:
            print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes, "
                  f"{brain.getFiringNeuronCount()} firing")
    
    print(f"\nPlasticity Demo Results:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.4f}")
    
    return {
        "demo": "plasticity",
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "exc_inh_ratio": brain.getExcitationInhibitionRatio(),
        "neurons": neurons,
        "steps": steps
    }


def development_demo(steps, neurons):
    """Development stages demonstration."""
    print(f"\nTesting development system with {neurons} neurons...")
    
    config = createDefaultConfig()
    config.set("neuron_count", neurons)
    
    brain = createBrain(config)
    brain.initialize()
    
    dev_system = brain.getDevelopmentSystem()
    
    print(f"  Development system active: {dev_system is not None}")
    
    # Run development simulation
    print(f"\nRunning development simulation for {steps} steps...")
    
    for step in range(steps):
        brain.step(step)
        
        if step % 25 == 0:
            stage = brain.getDevelopmentalStage()
            maturation = dev_system.getMaturationProgress() if dev_system else 0.0
            print(f"  Step {step}: Stage {stage}, Maturation {maturation:.2f}")
    
    print(f"\nDevelopment Demo Results:")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    if dev_system:
        print(f"  Final maturation progress: {dev_system.getMaturationProgress():.2f}")
        print(f"  Mature: {dev_system.isMature()}")
    
    print(f"  Total simulation spikes: {brain.getTotalSpikeCount()}")
    
    return {
        "demo": "development",
        "final_stage": brain.getDevelopmentalStage(),
        "maturation_progress": dev_system.getMaturationProgress() if dev_system else 0.0,
        "is_mature": dev_system.isMature() if dev_system else False,
        "total_spikes": brain.getTotalSpikeCount(),
        "neurons": neurons,
        "steps": steps
    }


def prediction_demo(steps, neurons):
    """Prediction system demonstration."""
    print(f"\nTesting prediction system with {neurons} neurons...")
    
    config = createDefaultConfig()
    config.set("neuron_count", neurons)
    
    brain = createBrain(config)
    brain.initialize()
    
    pred_system = brain.getPredictionSystem()
    
    print(f"  Prediction system active: {pred_system is not None}")
    
    # Run prediction simulation
    print(f"\nRunning prediction simulation for {steps} steps...")
    
    for step in range(steps):
        brain.step(step)
        
        if step % 50 == 0:
            error = pred_system.getPredictionError() if pred_system else 0.0
            confidence = pred_system.getConfidence() if pred_system else 0.0
            print(f"  Step {step}: Error={error:.4f}, Confidence={confidence:.4f}")
    
    print(f"\nPrediction Demo Results:")
    if pred_system:
        print(f"  Final prediction error: {pred_system.getPredictionError():.4f}")
        print(f"  Final confidence: {pred_system.getConfidence():.4f}")
        print(f"  System size: {pred_system.getSize()}")
    
    print(f"  Total simulation spikes: {brain.getTotalSpikeCount()}")
    
    return {
        "demo": "prediction",
        "final_error": pred_system.getPredictionError() if pred_system else 0.0,
        "final_confidence": pred_system.getConfidence() if pred_system else 0.0,
        "system_size": pred_system.getSize() if pred_system else 0,
        "total_spikes": brain.getTotalSpikeCount(),
        "neurons": neurons,
        "steps": steps
    }


def comprehensive_demo(steps, neurons):
    """Comprehensive demonstration of all systems."""
    print(f"\nRunning comprehensive integration demo with {neurons} neurons...")
    
    # Create complete system
    config = createDefaultConfig()
    config.set("neuron_count", neurons)
    
    brain = createBrain(config)
    brain.initialize()
    
    # Create environment and agent
    world = createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Enable brain plasticity
    if brain.getSTDP():
        brain.getSTDP()->enable(true)
    if brain.getHebbian():
        brain.getHebbian()->enable(true)
    
    print("  All systems integrated and enabled")
    
    # Run comprehensive simulation
    print(f"\nRunning comprehensive simulation for {steps} steps...")
    start_time = time.time()
    
    for step in range(steps):
        # World interaction loop
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Motor output
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"  Step {step}: "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Time={world.getSimulationTime():.1f}s, "
                  f"Reward={agent.getNeuromodulationLevel():.3f}")
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"\nComprehensive Demo Results:")
    print(f"  Simulation time: {elapsed:.3f}s")
    print(f"  World time: {world.getSimulationTime():.2f}s")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
    
    # System status summary
    print(f"\nSystem Status:")
    print(f"  ✓ Brain core: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"  ✓ Memory systems: Working={brain.getWorkingMemory() is not None}, "
          f"Episodic={brain.getEpisodicMemory() is not None}, "
          f"Associative={brain.getAssociativeMemory() is not None}")
    print(f"  ✓ Neuromodulation: Reward={agent.getNeuromodulationLevel():.3f}, "
          f"Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
    
    return {
        "demo": "comprehensive",
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "simulation_time": elapsed,
        "world_time": world.getSimulationTime(),
        "final_stage": brain.getDevelopmentalStage(),
        "neuromodulation": agent.getNeuromodulationLevel(),
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel(),
        "neurons": neurons,
        "steps": steps
    }


def save_all_results(results, output_file):
    """Save all demo results to file."""
    import json
    
    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2, default=str)
    
    print(f"\nAll results saved to: {output_file}")


if __name__ == "__main__":
    main()
