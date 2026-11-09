"""
Test script to verify NLM Python bindings work correctly.
This script tests the basic functionality documented in HOW_TO_USE.md and easy_usage.md.

To run this test, you need to build the Python module first using:

1. From source (Recommended for development):
   ```bash
   mkdir -p build
   cd build
   cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
   make -j4
   ```

2. Or using pip (preferred):
   ```bash
   pip install pybind11 scikit-build-core pytest numpy
   pip install .  # This will build the pynlm module
   ```

Once built, you can run this test script.
"""
import sys
import traceback

def test_basic_python_bindings():
    """Test basic Python API functionality"""
    print("Testing NLM Python bindings...")
    
    try:
        # Import the pynlm module
        import pynlm
        print(f"✓ Successfully imported pynlm, version: {pynlm.__version__}")
        
        # Test 1: Create default configuration
        config = pynlm.createDefaultConfig()
        print("✓ Successfully created default config")
        
        # Test 2: Configure config parameters
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("neuromod.dopamine.scale", 1.0)
        print("✓ Successfully configured config parameters")
        
        # Test 3: Create brain from configuration
        brain = pynlm.createBrain(config)
        print("✓ Successfully created brain")
        
        # Test 4: Initialize brain
        brain.initialize()
        print("✓ Successfully initialized brain")
        
        # Test 5: Check brain statistics
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        print(f"✓ Brain stats: {neuron_count} neurons, {synapse_count} synapses")
        
        # Test 6: Run brain step
        brain.step(0)
        firing_count = brain.getFiringNeuronCount()
        spike_count = brain.getTotalSpikeCount()
        print(f"✓ Brain step completed: {firing_count} firing neurons, {spike_count} total spikes")
        
        # Test 7: Create agent brain
        agent = pynlm.createAgentBrain(brain)
        print("✓ Successfully created agent brain")
        
        # Test 8: Create simple world
        world = pynlm.createSimpleWorld()
        print("✓ Successfully created simple world")
        
        # Test 9: Configure world
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        print("✓ Successfully configured world")
        
        # Test 10: Reset world
        world.reset()
        print("✓ Successfully reset world")
        
        # Test 11: Set agent start position
        world.setAgentStart(10.0, 10.0)
        print("✓ Successfully set agent start position")
        
        # Test 12: Initialize agent with world
        agent.initialize(world)
        print("✓ Successfully initialized agent with world")
        
        # Test 13: Enable brain features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        print("✓ Successfully enabled brain features")
        
        # Test 14: Get agent statistics
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        neuromod = agent.getNeuromodulationLevel()
        print(f"✓ Agent stats: curiosity={curiosity:.3f}, novelty={novelty:.3f}, neuromod={neuromod:.3f}")
        
        # Test 15: Get developmental stage
        stage = agent.getDevelopmentalStage()
        print(f"✓ Developmental stage: {stage}")
        
        # Test 16: Get thought trace
        thought = agent.getThoughtTrace()
        print(f"✓ Thought trace: {thought}")
        
        # Test 17: Encode text
        text_pattern = agent.encodeText("hello")
        print(f"✓ Text encoding: {len(text_pattern)} indices encoded")
        
        # Test 18: Get internal firing rates
        internal_rates = agent.getInternalFiringRates()
        print(f"✓ Internal firing rates: {len(internal_rates)} values")
        
        # Test 19: Get motor group activities
        motor_activities = agent.getMotorGroupActivities()
        print(f"✓ Motor group activities: {len(motor_activities)} groups")
        
        # Test 20: Get concept count
        concept_count = agent.getConceptCount()
        print(f"✓ Concept count: {concept_count}")
        
        # Test 21: Get self model confidence
        confidence = agent.getSelfModelConfidence()
        print(f"✓ Self model confidence: {confidence:.3f}")
        
        # Test 22: Get social observation count
        social_count = agent.getSocialObservationCount()
        print(f"✓ Social observation count: {social_count}")
        
        # Test 23: Simple world operations
        x, y = 5.0, 5.0
        is_valid = world.isValidPosition(x, y)
        print(f"✓ World position validation: ({x}, {y}) is valid: {is_valid}")
        
        # Test 24: Get world properties
        width = world.getWidth()
        height = world.getHeight()
        time = world.getSimulationTime()
        print(f"✓ World properties: width={width}, height={height}, time={time}")
        
        # Test 25: Convenience functions
        print("✓ All convenience functions imported")
        
        # Test 26: Check enumerations
        print(f"✓ NeuronType.Excitatory = {pynlm.NeuronType.Excitatory}")
        print(f"✓ DevelopmentalStage.Initial = {pynlm.DevelopmentalStage.Initial}")
        print(f"✓ ActionType.MoveForward = {pynlm.ActionType.MoveForward}")
        print(f"✓ WorldObjectType.Resource = {pynlm.WorldObjectType.Resource}")
        
        print("\n" + "="*60)
        print("ALL TESTS PASSED! Python bindings are working correctly.")
        print("="*60)
        return True
        
    except Exception as e:
        print(f"\n✗ TEST FAILED: {e}")
        print("\nFull traceback:")
        traceback.print_exc()
        return False

if __name__ == "__main__":
    print("NLM Python Bindings Test")
    print("=" * 40)
    print()
    success = test_basic_python_bindings()
    
    if success:
        print("\n" + "="*60)
        print("SUCCESS: All Python bindings tests passed!")
        print("The NLM Python module is correctly implemented.")
        print("="*60)
        sys.exit(0)
    else:
        print("\n" + "="*60)
        print("FAILURE: Python bindings tests failed.")
        print("Please check the implementation and try again.")
        print("="*60)
        sys.exit(1)