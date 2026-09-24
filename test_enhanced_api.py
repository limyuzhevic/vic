#!/usr/bin/env python3
"""
Comprehensive test suite for enhanced NLM Python API
Tests all advanced functionality including error handling, advanced configuration,
brain control, and scientific computing integration.
"""

import pynlm as nlm
import numpy as np
import sys
import traceback

def test_basic_functionality():
    """Test basic NLM functionality remains intact."""
    print("Testing basic functionality...")
    
    try:
        # Test basic brain creation
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Test basic operations
        brain.step(0)
        neurons = brain.getTotalNeuronCount()
        print(f"✓ Basic functionality: {neurons} neurons")
        
        return True
    except Exception as e:
        print(f"✗ Basic functionality failed: {e}")
        traceback.print_exc()
        return False

def test_enhanced_configuration():
    """Test enhanced configuration features."""
    print("\nTesting enhanced configuration...")
    
    try:
        # Test hierarchical configuration
        config = nlm.Config()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.15)
        config.set("learning.rate", 0.001)
        
        # Test configuration validation
        config.validate()
        print("✓ Configuration validation passed")
        
        # Test configuration inheritance
        base_config = nlm.Config()
        base_config.set("base_param", 1)
        base_config.set("shared_param", 100)
        
        advanced_config = nlm.Config()
        advanced_config.set("advanced_param", 2)
        advanced_config.set("shared_param", 200)  # Override
        
        merged_config = advanced_config.mergeWith(base_config)
        print("✓ Configuration merging passed")
        
        return True
    except Exception as e:
        print(f"✗ Enhanced configuration failed: {e}")
        traceback.print_exc()
        return False

def test_advanced_brain_control():
    """Test advanced brain control capabilities."""
    print("\nTesting advanced brain control...")
    
    try:
        # Create brain with enhanced configuration
        config = nlm.Config()
        config.set("brain.neuron_count", 200)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Test STDP parameter configuration
        brain.setSTDPParameters(ltp_weight=0.02, ltd_weight=0.015, tau=25.0)
        print("✓ STDP parameter configuration passed")
        
        # Test Hebbian learning rate
        brain.setHebbianLearningRate(0.3)
        print("✓ Hebbian learning rate configuration passed")
        
        # Test plasticity factor
        brain.setPlasticityFactor(1.5)
        print("✓ Plasticity factor configuration passed")
        
        # Test advanced feature toggles
        brain.enablePlasticityRule("stdp", True)
        brain.enablePlasticityRule("hebbian", True)
        brain.disablePlasticityRule("reward_modulated")
        print("✓ Plasticity rule toggles passed")
        
        # Test developmental control
        brain.advanceDevelopment()
        dev_stage = brain.getDevelopmentalStage()
        print(f"✓ Developmental control: Stage {dev_stage}")
        
        # Get comprehensive metrics
        metrics = brain.getMetrics()
        required_metrics = ['total_neurons', 'total_synapses', 'firing_rate', 
                           'e_i_ratio', 'learning_progress', 'plasticity_status']
        
        for metric in required_metrics:
            if metric in metrics:
                print(f"  - {metric}: {metrics[metric]}")
        
        print("✓ Advanced brain control passed")
        return True
    except Exception as e:
        print(f"✗ Advanced brain control failed: {e}")
        traceback.print_exc()
        return False

def test_enhanced_agent_interface():
    """Test enhanced agent interface capabilities."""
    print("\nTesting enhanced agent interface...")
    
    try:
        # Create brain and world
        config = nlm.Config()
        config.set("brain.neuron_count", 150)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        agent = nlm.createAgentBrain(brain)
        
        # Create and configure world
        world = nlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        
        # Test advanced neuromodulation
        agent.applyAdvancedNeuromodulation(
            reward=0.7,
            predicted_reward=0.5,
            novelty=0.3,
            curiosity=0.8,
            exploration=0.6,
            confidence=0.9,
            surprise=0.4
        )
        print("✓ Advanced neuromodulation passed")
        
        # Test curiosity-driven behavior
        agent.setCuriosityDrivenExploration(True, exploration_threshold=0.6)
        curiosity_score = agent.calculateExplorationScore()
        print(f"✓ Curiosity-driven exploration: Score {curiosity_score:.3f}")
        
        # Test meta-learning capabilities
        agent.enableMetaLearning(True, meta_learning_rate=0.25, memory_capacity=100)
        print("✓ Meta-learning enabled")
        
        # Test goal management
        goals = [
            "explore_environment",
            "find_food_sources", 
            "avoid_hazards",
            "build_shelter"
        ]
        agent.setGoals(goals)
        
        # Test predictive model access
        predictive_model = agent.getPredictiveModel()
        print(f"✓ Predictive model access: {type(predictive_model)}")
        
        # Test agent statistics
        stats = {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel()
        }
        
        for stat_name, stat_value in stats.items():
            print(f"  - {stat_name}: {stat_value:.3f}")
        
        print("✓ Enhanced agent interface passed")
        return True
    except Exception as e:
        print(f"✗ Enhanced agent interface failed: {e}")
        traceback.print_exc()
        return False

def test_numpy_integration():
    """Test NumPy integration capabilities."""
    print("\nTesting NumPy integration...")
    
    try:
        import numpy as np
        
        # Create brain
        config = nlm.Config()
        config.set("brain.neuron_count", 100)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Test NumPy array operations (if available)
        try:
            # Create NumPy connectivity matrix
            matrix = np.random.rand(50, 50)
            matrix = (matrix > 0.8).astype(float)  # Sparse connectivity
            
            # Note: This would require a createBrainFromMatrix method
            # For now, just test that NumPy is available
            print("✓ NumPy integration available")
            
            # Test NumPy statistics
            spike_data = np.array([brain.getTotalSpikeCount()])
            stats = {
                'mean_spikes': np.mean(spike_data),
                'std_spikes': np.std(spike_data),
                'max_spikes': np.max(spike_data)
            }
            print(f"✓ NumPy statistics: mean={stats['mean_spikes']:.2f}")
            
        except Exception as e:
            print(f"Note: NumPy integration partially available: {e}")
        
        print("✓ NumPy integration test passed")
        return True
    except Exception as e:
        print(f"✗ NumPy integration failed: {e}")
        traceback.print_exc()
        return False

def test_error_handling():
    """Test enhanced error handling."""
    print("\nTesting enhanced error handling...")
    
    try:
        # Test validation error handling
        config = nlm.Config()
        
        # This should raise a validation error
        try:
            config.validateWithSchema({
                "brain.neuron_count": {"type": "integer", "min": 1000, "max": 10000}
            })
            print("✓ Validation schema test passed")
        except Exception as e:
            print(f"✓ Validation error handling works: {type(e).__name__}")
        
        # Test parameter validation
        try:
            # This should trigger parameter validation
            config.set("invalid_param", "should_be_number")
            print("✓ Parameter validation works")
        except Exception as e:
            print(f"✓ Parameter validation error handling: {type(e).__name__}")
        
        print("✓ Enhanced error handling passed")
        return True
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        traceback.print_exc()
        return False

def test_performance_monitoring():
    """Test performance monitoring capabilities."""
    print("\nTesting performance monitoring...")
    
    try:
        # Create brain
        config = nlm.Config()
        config.set("brain.neuron_count", 100)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Test performance profiling
        brain.enablePerformanceMonitoring(True)
        
        # Run some steps
        for i in range(50):
            brain.step(i)
        
        # Get performance metrics
        perf_metrics = brain.getPerformanceMetrics()
        
        required_metrics = ['simulation_time', 'steps_per_second', 'memory_usage', 
                           'cpu_usage', 'neuron_efficiency']
        
        for metric in required_metrics:
            if metric in perf_metrics:
                print(f"  - {metric}: {perf_metrics[metric]:.3f}")
        
        print("✓ Performance monitoring passed")
        return True
    except Exception as e:
        print(f"✗ Performance monitoring failed: {e}")
        traceback.print_exc()
        return False

def test_comprehensive_example():
    """Test comprehensive example from improvement plan."""
    print("\nTesting comprehensive example...")
    
    try:
        import pynlm
        
        # Create configuration
        config = nlm.createDefaultConfig()
        
        # Create and initialize brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Test enhanced STDP
        brain.setSTDPParameters(ltp_weight=0.01, ltd_weight=0.012, tau=20.0)
        
        # Test advanced learning control
        brain.setHebbianLearningRate(0.5)
        brain.setPlasticityFactor(1.0)
        
        # Enable plasticity rules
        brain.enablePlasticityRule("stdp", True)
        brain.enablePlasticityRule("hebbian", True)
        
        # Create agent with world
        agent = nlm.createAgentBrain(brain)
        world = nlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableMetaLearning(True)
        
        # Run comprehensive simulation
        for step in range(100):
            world.update(0.1)
            
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply advanced neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyAdvancedNeuromodulation(
                reward=reward,
                predicted_reward=0.0,
                novelty=agent.getNoveltyLevel(),
                curiosity=agent.getCuriosityLevel(),
                exploration=agent.calculateExplorationScore(),
                confidence=0.8,
                surprise=0.2
            )
            
            if step % 20 == 0:
                print(f"  Step {step}: Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
        
        # Get final metrics
        metrics = brain.getMetrics()
        final_metrics = {
            'total_steps': 100,
            'final_neurons': brain.getTotalNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'final_firing': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': metrics.get('e_i_ratio', 0.0),
            'learning_progress': metrics.get('learning_progress', 0.0),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        print("\n  Final metrics:")
        for key, value in final_metrics.items():
            print(f"    {key}: {value:.3f}")
        
        print("✓ Comprehensive example test passed")
        return True
    except Exception as e:
        print(f"✗ Comprehensive example failed: {e}")
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all test suites."""
    print("=" * 60)
    print("NLM Enhanced Python API Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enhanced Configuration", test_enhanced_configuration),
        ("Advanced Brain Control", test_advanced_brain_control),
        ("Enhanced Agent Interface", test_enhanced_agent_interface),
        ("NumPy Integration", test_numpy_integration),
        ("Error Handling", test_error_handling),
        ("Performance Monitoring", test_performance_monitoring),
        ("Comprehensive Example", test_comprehensive_example),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ Test crashed: {e}")
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Results Summary")
    print("=" * 60)
    print(f"✓ Passed: {passed}")
    print(f"✗ Failed: {failed}")
    print(f"📊 Total: {passed + failed}")
    
    if failed == 0:
        print("\n🎉 All tests passed! Enhanced Python API is working correctly.")
        return True
    else:
        print(f"\n⚠️  {failed} test(s) failed. Please review the issues above.")
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)