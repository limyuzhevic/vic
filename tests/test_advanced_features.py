# Python test file for pynlm advanced features
"""
Test advanced Python bindings and features for NLM.
"""

import pynlm
import numpy as np
import tempfile
import os

def test_basic_setup():
    """Test basic setup with helper functions"""
    print("Testing basic setup...")
    
    # Test Phase 6 demo setup
    config, brain, world, agent = pynlm.createPhase6Demo()
    
    assert config is not None
    assert brain is not None
    assert world is not None
    assert agent is not None
    
    print("  ✓ Phase 6 demo setup")
    
    # Test basic simulation
    spikes, firing, energy = pynlm.runSimulationLoop(brain, world, agent, 10)
    
    assert isinstance(spikes, int)
    assert isinstance(firing, int)
    assert isinstance(energy, float)
    
    print(f"  ✓ Simulation loop: {spikes} spikes, {firing} firing neurons")
    return config, brain, world, agent

def test_advanced_features():
    """Test advanced features and enable all features"""
    print("Testing advanced features...")
    
    config, brain, world, agent = pynlm.createPhase6Demo()
    
    # Enable all advanced features
    result = pynlm.enableAllFeatures(agent)
    assert result == "All features enabled"
    
    # Check that features are enabled
    assert agent.isRewardModulationEnabled()
    assert agent.isStructuralPlasticityEnabled()
    assert agent.isDevelopmentEnabled()
    assert agent.isCuriosityEnabled()
    
    print("  ✓ All features enabled")
    
    # Run simulation with features
    for i in range(5):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("  ✓ Simulation with all features")
    return config, brain, world, agent

def test_config_presets():
    """Test different configuration presets"""
    print("Testing configuration presets...")
    
    # Test development config
    dev_config = pynlm.createDevelopmentConfig()
    assert dev_config.has("neuron_count")
    assert dev_config.getOr("neuron_count", 0) > 0
    
    # Test exploration config
    expl_config = pynlm.createExplorationConfig()
    assert expl_config.has("curiosity_threshold")
    assert expl_config.has("novelty_threshold")
    
    # Test stable config
    stable_config = pynlm.createStableConfig()
    assert stable_config.has("developmental_stage")
    assert stable_config.getOr("plasticity_modulation", 1.0f) < 1.0f
    
    print("  ✓ Development config created")
    print("  ✓ Exploration config created")
    print("  ✓ Stable config created")
    
    return dev_config, expl_config, stable_config

def test_python_api_features():
    """Test Python API features"""
    print("Testing Python API features...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(10, 10, 5, 5)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Test feature getters
    input_size = agent.getSensoryInputSize()
    output_size = agent.getMotorOutputSize()
    assert input_size > 0
    assert output_size > 0
    
    # Test developmental stage
    stage = agent.getDevelopmentalStage()
    assert stage in [0, 1, 2, 3, 4]  # DevelopmentalStage enum values
    
    # Test neuromodulation levels
    neuromod = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    prediction_error = agent.getPredictionError()
    
    assert isinstance(neuromod, float)
    assert isinstance(curiosity, float)
    assert isinstance(novelty, float)
    assert isinstance(prediction_error, float)
    
    print(f"  ✓ API features: input={input_size}, output={output_size}")
    print(f"  ✓ Development stage: {stage}")
    print(f"  ✓ Neuromodulation levels retrieved")
    
    return config, brain, world, agent

def test_simulation_with_saving():
    """Test simulation with brain saving/loading"""
    print("Testing simulation with saving...")
    
    config, brain, world, agent = pynlm.createPhase6Demo()
    
    # Run a simulation
    for i in range(20):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save brain state
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as tmp:
        save_path = tmp.name
    
    try:
        saved = brain.save(save_path)
        assert saved
        print("  ✓ Brain saved")
        
        # Reset brain
        brain.reset()
        
        # Load brain state
        loaded = brain.load(save_path)
        assert loaded
        print("  ✓ Brain loaded")
        
    finally:
        if os.path.exists(save_path):
            os.unlink(save_path)
    
    return config, brain, world, agent

def test_version_and_info():
    """Test version and system info functions"""
    print("Testing version and system info...")
    
    version = pynlm.getVersionInfo()
    info = pynlm.getSystemInfo()
    
    assert isinstance(version, str)
    assert isinstance(info, str)
    assert "Phase 6" in info
    assert "Neural Learning Machine" in info
    
    print(f"  ✓ Version: {version}")
    print(f"  ✓ System info retrieved")
    
    # Test constants
    assert hasattr(pynlm, 'VERSION_MAJOR')
    assert hasattr(pynlm, 'VERSION_MINOR')
    assert hasattr(pynlm, 'VERSION_PATCH')
    
    assert hasattr(pynlm, 'DEFAULT_NEURON_COUNT')
    assert hasattr(pynlm, 'DEFAULT_REGION_COUNT')
    assert hasattr(pynlm, 'DEFAULT_CONNECTION_PROBABILITY')
    
    assert hasattr(pynlm, 'MIN_NEURON_COUNT')
    assert hasattr(pynlm, 'MAX_NEURON_COUNT')
    
    print(f"  ✓ Constants available")
    print(f"  ✓ Version constants: {pynlm.VERSION_MAJOR}.{pynlm.VERSION_MINOR}.{pynlm.VERSION_PATCH}")
    print(f"  ✓ Default neuron count: {pynlm.DEFAULT_NEURON_COUNT}")
    
    return version, info

def test_comprehensive_workflow():
    """Test a comprehensive workflow using all advanced features"""
    print("Testing comprehensive workflow...")
    
    # 1. Create development configuration
    config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # 2. Configure world
    world.configure(15, 15, 8, 8)
    world.reset()
    
    # 3. Initialize with development settings
    agent.initialize(world)
    
    # 4. Enable all features
    pynlm.enableAllFeatures(agent)
    
    # 5. Run exploration simulation
    stats = []
    for step in range(50):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Agent acts
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect stats
        if step % 10 == 0:
            spikes = brain.getTotalSpikeCount()
            firing = brain.getFiringNeuronCount()
            stage = agent.getDevelopmentalStage()
            neuromod = agent.getNeuromodulationLevel()
            curiosity = agent.getCuriosityLevel()
            
            stats.append({
                'step': step,
                'spikes': spikes,
                'firing': firing,
                'stage': stage,
                'neuromod': neuromod,
                'curiosity': curiosity
            })
    
    # 6. Verify results
    assert len(stats) > 0
    final_spikes = stats[-1]['spikes']
    final_stage = stats[-1]['stage']
    
    print(f"  ✓ Comprehensive simulation completed")
    print(f"  ✓ Final spikes: {final_spikes}, final stage: {final_stage}")
    print(f"  ✓ Stats collected: {len(stats)} samples")
    
    # 7. Save final brain state
    with tempfile.NamedTemporaryFile(suffix='.nlm', delete=False) as tmp:
        save_path = tmp.name
    
    try:
        brain.save(save_path)
        print("  ✓ Final brain state saved")
    finally:
        if os.path.exists(save_path):
            os.unlink(save_path)
    
    return config, brain, world, agent, stats

def main():
    """Run all Python tests"""
    print("=" * 60)
    print("NLM Python Bindings Advanced Features Tests")
    print("=" * 60)
    print()
    
    try:
        # Run all test suites
        test_basic_setup()
        print()
        
        test_advanced_features()
        print()
        
        test_config_presets()
        print()
        
        test_python_api_features()
        print()
        
        test_simulation_with_saving()
        print()
        
        test_version_and_info()
        print()
        
        test_comprehensive_workflow()
        print()
        
        print("=" * 60)
        print("✓ ALL TESTS PASSED!")
        print("=" * 60)
        
        return True
        
    except Exception as e:
        print(f"✗ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)
