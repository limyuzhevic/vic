"""
Example Python script demonstrating NLM Python bindings usage
This script shows various ways to use the NLM Python bindings.
"""

import pynlm
import time

def main():
    print("=== NLM Python Bindings Examples ===\n")
    
    print("Example 1: Basic Brain Creation")
    print("-" * 40)
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    print("\nExample 2: Running Simulation Steps")
    print("-" * 40)
    for i in range(10):
        brain.step(i)
        firing = brain.getFiringNeuronCount()
        print(f"Step {i}: {firing} firing neurons")
    
    print("\nExample 3: Agent System Integration")
    print("-" * 40)
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Run agent simulation
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Step {step}:")
        print(f"  Action: {action}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("\nExample 4: Configuration Management")
    print("-" * 40)
    custom_config = pynlm.createDefaultConfig()
    custom_config.set("brain.neuron_count", 1000)
    custom_config.set("brain.synapse_density", 0.1)
    custom_config.set("plasticity.stdp.learning_rate", 0.001)
    
    print("Custom configuration set:")
    print(f"  Neuron count: {custom_config.getOr('brain.neuron_count', 0)}")
    print(f"  Synapse density: {custom_config.getOr('brain.synapse_density', 0.0)}")
    print(f"  STDP learning rate: {custom_config.getOr('plasticity.stdp.learning_rate', 0.0)}")
    
    print("\nExample 5: Learning Subsystem Control")
    print("-" * 40)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Enabled learning subsystems:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    
    print("\nExample 6: Brain Analysis")
    print("-" * 40)
    connectivity = brain.analyzeConnectivity()
    print("Brain connectivity analysis:")
    print(f"  Region count: {connectivity['region_count']}")
    print(f"  Total synapses: {connectivity['total_synapses']}")
    print(f"  Connection density: {connectivity['connection_density']:.3f}")
    
    metrics = brain.computeNetworkMetrics()
    print("Network metrics:")
    print(f"  Total neurons: {metrics['total_neurons']}")
    print(f"  Total synapses: {metrics['total_synapses']}")
    print(f"  Average firing rate: {metrics['average_firing_rate']:.3f}")
    
    print("\nExample 7: Performance Benchmark")
    print("-" * 40)
    from pynlm import benchmark
    benchmark_results = benchmark(custom_config, 50)
    print(f"Benchmark results:")
    print(f"  Steps per second: {benchmark_results['steps_per_second']:.1f}")
    print(f"  Neurons: {benchmark_results['neurons']}")
    print(f"  Synapses: {benchmark_results['synapses']}")
    print(f"  Elapsed time: {benchmark_results['elapsed_time']:.3f} seconds")
    
    print("\n" + "=" * 40)
    print("All examples completed successfully!")
    print("=" * 40)

if __name__ == "__main__":
    main()