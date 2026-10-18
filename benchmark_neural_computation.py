#!/usr/bin/env python3
"""
Performance benchmarks for NLM
Run with: python3 benchmark_neural_computation.py
"""

import pynlm
import time
import sys

from nlm.Profiler import getProfiler

def benchmark_neural_computation():
    """Benchmark basic neural computation performance."""
    print("=== NLM Neural Computation Benchmark ===")
    
    # Test 1: Small brain (100 neurons)
    print("\n1. Small brain (100 neurons)")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Benchmark initialization
    start = time.time()
    brain.initialize()
    init_time = time.time() - start
    print(f"   Initialization: {init_time:.4f} seconds")
    
    # Benchmark stepping
    steps = 1000
    start = time.time()
    for step in range(steps):
        brain.step(step)
    step_time = time.time() - start
    print(f"   {steps} steps: {step_time:.4f} seconds ({steps/step_time:.1f} steps/sec)")
    
    # Test 2: Medium brain (1000 neurons)
    print("\n2. Medium brain (1000 neurons)")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start = time.time()
    for step in range(steps):
        brain.step(step)
    step_time = time.time() - start
    print(f"   {steps} steps: {step_time:.4f} seconds ({steps/step_time:.1f} steps/sec)")
    
    # Test 3: Large brain (5000 neurons)
    print("\n3. Large brain (5000 neurons)")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 5000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start = time.time()
    for step in range(steps):
        brain.step(step)
    step_time = time.time() - start
    print(f"   {steps} steps: {step_time:.4f} seconds ({steps/step_time:.1f} steps/sec)")
    
    # Test 4: Brain with plasticity
    print("\n4. Brain with plasticity enabled")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent to enable plasticity
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10)
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    start = time.time()
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    step_time = time.time() - start
    print(f"   {steps} steps: {step_time:.4f} seconds ({steps/step_time:.1f} steps/sec)")
    
    print("\n=== Benchmark Complete ===")

def benchmark_plasticity():
    """Benchmark plasticity performance."""
    print("\n=== Plasticity Performance Benchmark ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("simulation_timestep", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all plasticity enabled
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    steps = 2000
    start = time.time()
    for step in range(steps):
        # Vary input to induce plasticity
        for i in range(5):
            brain->injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0f + (i * 5.0f))
        
        brain.step(step, step * config.getOr<double>("simulation_timestep", 0.001))
        
        # Apply reward modulation
        reward = (step % 100) * 0.1f
        agent.applyRewardModulation(reward, reward * 0.8f)
        
        if step % 500 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    total_time = time.time() - start
    print(f"\n   Total time: {total_time:.2f} seconds")
    print(f"   Steps/sec: {steps/total_time:.1f}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

def benchmark_agent_behavior():
    """Benchmark agent behavior in a simulated environment."""
    print("\n=== Agent Behavior Benchmark ===")
    
    # Create a challenging environment
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Test different exploration strategies
    strategies = ["conservative", "balanced", "aggressive"]
    
    for strategy in strategies:
        print(f"\n   Testing {strategy} strategy...")
        
        # Load the preset
        agent.loadPreset(strategy)
        
        # Reset world and agent
        world.reset()
        agent.resetAll()
        
        # Run simulation
        steps = 1000
        start = time.time()
        
        for step in range(steps):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Execute action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = 0.0
            if world.getSensoryPercept().getInternal():
                reward = world.getSensoryPercept().getInternal()[0]
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        elapsed = time.time() - start
        print(f"      Time: {elapsed:.2f} seconds")
        print(f"      Steps/sec: {steps/elapsed:.1f}")
        print(f"      Final exploration weight: {agent.getExplorationWeight():.3f}")
        print(f"      Final exploitation weight: {agent.getExploitationWeight():.3f}")

def main():
    """Run all benchmarks."""
    print("NLM Performance Benchmark Suite")
    print("=" * 50)
    
    # Get profiler instance
    profiler = getProfiler()
    
    # Benchmark neural computation
    profiler.start("neural_computation_benchmark")
    benchmark_neural_computation()
    profiler.stop("neural_computation_benchmark")
    
    # Benchmark plasticity
    profiler.start("plasticity_benchmark")
    benchmark_plasticity()
    profiler.stop("plasticity_benchmark")
    
    # Benchmark agent behavior
    profiler.start("agent_behavior_benchmark")
    benchmark_agent_behavior()
    profiler.stop("agent_behavior_benchmark")
    
    # Generate summary report
    print("\n" + "=" * 50)
    print("BENCHMARK SUMMARY REPORT")
    print("=" * 50)
    
    all_metrics = profiler.getAllMetrics()
    
    for metric in all_metrics:
        print(f"\n{metric.name}:")
        print(f"  CPU Time: {metric.cpuTimeMs:.2f} ms")
        print(f"  Wall Clock: {metric.wallClockTimeMs:.2f} ms")
        if metric.additionalMetrics:
            print(f"  Additional metrics: {metric.additionalMetrics}")
    
    # Generate HTML report
    html_report = profiler.generateHTMLReport()
    
    # Save report to file
    report_file = "benchmark_report.html"
    with open(report_file, "w") as f:
        f.write(html_report)
    
    print(f"\nHTML report saved to: {report_file}")
    
    # Print general profiling statistics
    print(f"\nPeak memory usage: {profiler.getPeakMemoryUsage()} bytes")
    print(f"Average neural computation time: {profiler.getAverageTime('neural_computation_benchmark'):.4f} ms")
    print(f"Neural computation standard deviation: {profiler.getStdDev('neural_computation_benchmark'):.4f} ms")
    
    print("\nBenchmark suite completed successfully!")

if __name__ == "__main__":
    main()
