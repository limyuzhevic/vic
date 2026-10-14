"""
Demonstration of Enhanced NLM Python Bindings

This script demonstrates the new Pythonic API and convenience features
of the enhanced NLM Python bindings.

Run with: python demo_enhanced_bindings.py
"""

import pynlm
import time

def demo_original_api():
    """Demonstrate the original API (still works perfectly)."""
    print("=" * 60)
    print("DEMONSTRATION 1: Original API (Backward Compatible)")
    print("=" * 60)
    
    # Original way of creating a brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print("✓ Created brain with original API")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    
    # Original simulation loop
    for step in range(5):
        brain.step(step)
        print(f"  Step {step}: {brain.getTotalSpikeCount()} spikes")
    
    print("✓ Original API demonstration complete\n")

def demo_pythonic_api():
    """Demonstrate the new Pythonic API."""
    print("=" * 60)
    print("DEMONSTRATION 2: Pythonic Enhanced API")
    print("=" * 60)
    
    # New Pythonic way to create a brain
    brain = pynlm.Brain()
    brain.initialize()
    
    print("✓ Created brain with Pythonic API")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    
    # New convenience methods
    print("\nUsing convenience injection methods:")
    brain.injectToExcitatory(0.5)
    print("  ✓ Injected to excitatory neurons")
    brain.injectToInhibitory(-0.2)
    print("  ✓ Injected to inhibitory neurons")
    brain.injectToSensory(1.0)
    print("  ✓ Injected to sensory neurons")
    
    # New batch operations
    print("\nUsing batch operations:")
    neuron_ids = [brain.getRegion(1).getAllNeurons()[i].getId() 
                  for i in range(min(3, brain.getTotalNeuronCount()))]
    brain.batchInjectCurrent([(id, 0.1) for id in neuron_ids])
    print(f"  ✓ Batch injected to {len(neuron_ids)} neurons")
    
    # New vectorized operations
    brain.injectCurrentVectorized([0.1, 0.2, 0.3], scaling=2.0)
    print("  ✓ Vectorized injection with scaling")
    
    print("✓ Pythonic API demonstration complete\n")

def demo_config_api():
    """Demonstrate the Pythonic Config API."""
    print("=" * 60)
    print("DEMONSTRATION 3: Pythonic Config API")
    print("=" * 60)
    
    # Create config with Pythonic API
    config = pynlm.Config()
    
    # Pythonic getters and setters
    print("Using Pythonic getters and setters:")
    config.setInt("neuron_count", 1000)
    print(f"  ✓ Set neuron_count to {config.getInt('neuron_count')}")
    
    config.setFloat("learning_rate", 0.01)
    print(f"  ✓ Set learning_rate to {config.getFloat('learning_rate')}")
    
    config.setBool("enable_curiosity", True)
    print(f"  ✓ Set enable_curiosity to {config.getBool('enable_curiosity')}")
    
    # Pythonic dictionary-style access
    print("\nUsing Pythonic dictionary-style access:")
    config["max_neurons"] = 2000
    print(f"  ✓ Set max_neurons via config['max_neurons'] = {config['max_neurons']}")
    
    del config["max_neurons"]
    print("  ✓ Deleted max_neurons")
    
    # Batch operations
    print("\nUsing batch operations:")
    config.update({
        "batch_param1": 1.0,
        "batch_param2": 2.0,
        "batch_param3": 3.0
    })
    print("  ✓ Updated multiple parameters at once")
    
    print("✓ Config API demonstration complete\n")

def demo_context_manager():
    """Demonstrate context managers."""
    print("=" * 60)
    print("DEMONSTRATION 4: Context Managers")
    print("=" * 60)
    
    print("Using brain context manager (automatic cleanup):")
    with pynlm.Brain() as brain:
        brain.initialize()
        print("  ✓ Brain initialized inside context")
        brain.step(0)
        print("  ✓ Brain stepped inside context")
        print("  ✓ Brain will be automatically reset when exiting!")
    
    print("✓ Context manager demonstration complete\n")

def demo_agent_brain():
    """Demonstrate enhanced AgentBrain features."""
    print("=" * 60)
    print("DEMONSTRATION 5: Enhanced AgentBrain")
    print("=" * 60)
    
    # Create integrated system
    brain, world, agent = pynlm.createIntegratedSystem()
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("✓ Created integrated system (brain, world, agent)")
    print(f"  Agent has {agent.getSensoryInputSize()} sensory inputs")
    print(f"  Agent has {agent.getMotorOutputSize()} motor outputs")
    
    # Create simple percept
    percept = pynlm.SensoryPercept()
    percept.setVision([0.1, 0.2, 0.3, 0.4])
    
    # Complete learning cycle
    print("\nRunning complete agent learning cycle:")
    action = agent.act(percept, reward=1.0)
    print(f"  ✓ Agent selected action: {action.getName()}")
    
    # Get agent state
    state = agent.getStateDict()
    print(f"  ✓ Agent state: neuromodulation={state['neuromodulation']:.2f}, curiosity={state['curiosity']:.2f}")
    
    print("✓ AgentBrain demonstration complete\n")

def demo_experiment_setup():
    """Demonstrate experiment setup helpers."""
    print("=" * 60)
    print("DEMONSTRATION 6: Experiment Setup Helpers")
    print("=" * 60)
    
    # Create brain for different experiment types
    print("Setting up different experiment types:")
    
    # Learning experiment
    brain1 = pynlm.Brain()
    brain1.setupForExperiment("learning")
    print("  ✓ Learning experiment configured")
    print(f"    - Neurons: {brain1.getConfig().getIntOr('neuron_count', 0)}")
    print(f"    - Learning rate: {brain1.getConfig().getFloatOr('stdp_ltp_weight', 0.0):.3f}")
    
    # Memory experiment  
    brain2 = pynlm.Brain()
    brain2.setupForExperiment("memory")
    print("  ✓ Memory experiment configured")
    print(f"    - Replay interval: {brain2.getConfig().getIntOr('replay_interval', 0)}")
    
    # Cognition experiment
    brain3 = pynlm.Brain()
    brain3.setupForExperiment("cognition")
    print("  ✓ Cognition experiment configured")
    print(f"    - Planning depth: {brain3.getConfig().getIntOr('planning_depth', 0)}")
    
    print("✓ Experiment setup demonstration complete\n")

def demo_episode_recording():
    """Demonstrate episode recording."""
    print("=" * 60)
    print("DEMONSTRATION 7: Episode Recording")
    print("=" * 60)
    
    # Create brain
    brain = pynlm.Brain()
    brain.initialize()
    
    # Create an action
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    
    # Record episode
    print("Recording episode with rich data:")
    episode_data = brain.recordEpisode(
        step=100,
        reward=1.5,
        action=action,
        sensory_input=[0.1, 0.2, 0.3, 0.4]
    )
    
    print(f"  ✓ Episode recorded:")
    print(f"    - Step: {episode_data.step}")
    print(f"    - Reward: {episode_data.reward}")
    print(f"    - Action: {episode_data.action}")
    print(f"    - Active neurons: {len(episode_data.activeNeurons)}")
    
    print("✓ Episode recording demonstration complete\n")

def demo_educational_helpers():
    """Demonstrate educational helper functions."""
    print("=" * 60)
    print("DEMONSTRATION 8: Educational Helper Functions")
    print("=" * 60)
    
    print("Creating tutorial brain for learning:")
    tutorial_brain = pynlm.Brain.createTutorial()
    print(f"  ✓ Tutorial brain: {tutorial_brain.getTotalNeuronCount()} neurons")
    print(f"    - Optimized for neural learning demonstrations")
    
    print("\nCreating benchmark brain for performance testing:")
    benchmark_brain = pynlm.Brain.createBenchmark(neuron_count=500, region_count=2)
    print(f"  ✓ Benchmark brain: {benchmark_brain.getTotalNeuronCount()} neurons")
    print(f"    - Optimized for performance benchmarking")
    
    print("✓ Educational helpers demonstration complete\n")

def demo_complete_system():
    """Demonstrate complete integrated system."""
    print("=" * 60)
    print("DEMONSTRATION 9: Complete Integrated System")
    print("=" * 60)
    
    print("Creating complete integrated system (brain + world + agent):")
    brain, world, agent = pynlm.createIntegratedSystem()
    
    print(f"  ✓ Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"  ✓ World: {world.getWidth()}x{world.getHeight()} meters")
    print(f"  ✓ Agent: {agent.getSensoryInputSize()} inputs, {agent.getMotorOutputSize()} outputs")
    
    # Run quick simulation
    print("\nRunning quick simulation:")
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        action = agent.act(percept, reward=1.0)
        brain.step(step)
        world.applyMotorCommand(action, world.getSimulationTime())
        print(f"  ✓ Step {step}: {brain.getTotalSpikeCount()} spikes")
    
    print("✓ Complete integrated system demonstration complete\n")

def main():
    """Run all demonstrations."""
    print("\n" + "=" * 60)
    print("NLM Python Bindings Enhanced API Demonstration")
    print("=" * 60)
    print("\nThis demonstration showcases the new Pythonic API and")
    print("convenience features of the enhanced NLM Python bindings.")
    print("All original functionality remains fully compatible.\n")
    
    try:
        # Run all demonstrations
        demo_original_api()
        demo_pythonic_api()
        demo_config_api()
        demo_context_manager()
        demo_agent_brain()
        demo_experiment_setup()
        demo_episode_recording()
        demo_educational_helpers()
        demo_complete_system()
        
        print("=" * 60)
        print("ALL DEMONSTRATIONS COMPLETED SUCCESSFULLY!")
        print("=" * 60)
        print("\nKey improvements demonstrated:")
        print("✓ Pythonic API (Brain(), Config() constructors)")
        print("✓ Convenience methods (injectToExcitatory, injectToInhibitory)")
        print("✓ Batch operations (batchInjectCurrent)")
        print("✓ Context managers (with Brain() as brain:)")
        print("✓ Enhanced AgentBrain (completeLearningCycle)")
        print("✓ Experiment setup helpers (setupForExperiment)")
        print("✓ Episode recording and memory management")
        print("✓ Educational helper functions (createTutorial, createBenchmark)")
        print("✓ Complete integrated system (createIntegratedSystem)")
        print("\nOriginal API remains 100% backward compatible!")
        
    except Exception as e:
        print(f"\nERROR: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    exit(main())
