import pynlm
import numpy as np

def test_learning_and_development():
    """Test learning systems and developmental progression."""
    
    # Create configuration optimized for learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Testing learning and development systems...")
    print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
    
    # Collect developmental data
    developmental_data = []
    curiosity_levels = []
    novelty_levels = []
    neuromodulation_levels = []
    
    for step in range(200):
        brain.step(step)
        
        # Track development
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Track neuromodulation levels
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        neuromodulation_levels.append(agent.getNeuromodulationLevel())
        
        # Track developmental stage
        developmental_data.append({
            'step': step,
            'stage': brain.getDevelopmentalStage(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        })
    
    # Analyze development
    print("\nDevelopment Analysis:")
    print(f"Final developmental stage: {developmental_data[-1]['stage']}")
    print(f"Average curiosity level: {sum(curiosity_levels)/len(curiosity_levels):.3f}")
    print(f"Average novelty level: {sum(novelty_levels)/len(novelty_levels):.3f}")
    print(f"Average neuromodulation level: {sum(neuromodulation_levels)/len(neuromodulation_levels):.3f}")
    
    # Check for developmental progression
    stages = [d['stage'] for d in developmental_data]
    if len(set(stages)) > 1:
        print("✓ Development is progressing through stages")
    else:
        print("⚠ Development may not be progressing through stages")
    
    return developmental_data

if __name__ == "__main__":
    development_results = test_learning_and_development()