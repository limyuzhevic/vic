# Utility Classes Example
"""
Example demonstrating utility classes like SimulationRunner, ExperimentRunner, etc.
"""

import pynlm_enhanced as pynlm

def main():
    print("=== Utility Classes Example ===")
    
    # Create simulation components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Example using SimulationRunner
    print("Using SimulationRunner:")
    runner = pynlm.SimulationRunner(world, brain, agent)
    results = runner.run_experiment(steps=50, timestep=0.1)
    
    print(f"Simulation completed: {results['steps_completed']} steps")
    print(f"Average spikes per step: {results['avg_spikes']}")
    
    # Example using ExperimentRunner for parameter sweep
    print("\nUsing ExperimentRunner for parameter sweep:")
    
    def varying_timestep(timestep):
        """Run simulation with different timestep"""
        world.reset()
        brain.reset()
        if agent:
            agent.reset()
        brain.initialize()
        
        total_spikes = 0
        for step in range(20):
            world.update(timestep)
            if agent:
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                agent.decodeMotorCommand()
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            else:
                brain.step(step)
            total_spikes += brain.getTotalSpikeCount()
        
        return total_spikes / 20
    
    experiment = pynlm.ExperimentRunner(
        world_factory=lambda: pynlm.createSimpleWorld(),
        brain_factory=lambda c: pynlm.createBrain(c),
        agent_factory=lambda b: pynlm.createAgentBrain(b)
    )
    
    results = experiment.run_parameter_sweep(
        timestep=[0.05, 0.1, 0.2, 0.5],
        function=varying_timestep
    )
    
    print("Parameter sweep results:")
    for param, result in zip(['timestep'] * len(results), results):
        print(f"  Time step {param['value']}: {param['result']:.2f} avg spikes")
    
    # Example using DataCollector
    print("\nUsing DataCollector for metrics collection:")
    collector = pynlm.DataCollector(world, brain, agent)
    
    # Simulate and collect data
    for step in range(10):
        world.update(0.1)
        if agent:
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
        
        collector.record_step(
            step=step,
            time=world.getSimulationTime(),
            spikes=brain.getTotalSpikeCount(),
            firing_rate=brain.getAverageFiringRate(),
            curiosity=agent.getCuriosityLevel() if agent else 0
        )
    
    # Analyze collected data
    data = collector.get_collected_data()
    print(f"Collected {len(data['steps'])} data points")
    print(f"Average firing rate: {sum(data['firing_rate']) / len(data['firing_rate']):.3f}")
    print(f"Max curiosity: {max(data['curiosity']) if data['curiosity'] else 0:.3f}")

if __name__ == "__main__":
    main()
