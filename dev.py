import pynlm
from pynlm.advanced_api import AdvancedAPI, SimpleAgent, SimulationRunner

def develop():
    print("=== NLM Development Session ===")
    
    # Create development brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create development agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    
    agent = SimpleAgent(brain, world, "dev_agent")
    agent.initialize(world)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run development simulation
    sim_runner = SimulationRunner(num_steps=500, dt=0.1)
    sim_runner.addAgent(agent)
    
    sim_runner.start()
    for step in range(500):
        sim_runner.step()
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getAverageFiringRate():.3f} Hz")
    
    sim_runner.stop()
    
    print("=== Development Complete ===")

if __name__ == "__main__":
    develop()
