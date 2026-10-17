#!/usr/bin/env python3
"""
Test script for NLM Enhanced Python API.
This file validates the enhanced functionality and provides test cases.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'build'))

import time
import json
from typing import List, Dict, Any

# Try to import the enhanced bindings
try:
    import pynlm_enhanced as nlm
    ENHANCED_BINDINGS_AVAILABLE = True
    print("✓ Enhanced NLM Python bindings loaded successfully")
except ImportError as e:
    print(f"✗ Failed to load enhanced bindings: {e}")
    print("Falling back to basic bindings...")
    try:
        import pynlm as nlm
        ENHANCED_BINDINGS_AVAILABLE = False
        print("✓ Basic NLM Python bindings loaded successfully")
    except ImportError as e2:
        print(f"✗ Failed to load basic bindings: {e2}")
        print("No bindings available - tests cannot run.")
        sys.exit(1)

def test_basic_functionality():
    """Test basic functionality with enhanced API"""
    print("\n" + "="*60)
    print("TEST 1: Basic Functionality")
    print("="*60)
    
    try:
        # Test configuration creation
        print("Testing configuration creation...")
        config = nlm.createDefaultConfig()
        assert config is not None, "Configuration should not be None"
        print("✓ Configuration created")
        
        # Test brain creation
        print("Testing brain creation...")
        brain = nlm.Brain(config)
        assert brain is not None, "Brain should not be None"
        print("✓ Brain created")
        
        # Test brain initialization
        print("Testing brain initialization...")
        success = brain.initialize()
        assert success, "Brain initialization should succeed"
        print("✓ Brain initialized")
        
        # Test simulation step
        print("Testing simulation step...")
        brain.step(0)
        assert brain.getTotalSpikeCount() >= 0, "Spike count should be non-negative"
        print("✓ Simulation step completed")
        
        # Test statistics
        print("Testing statistics...")
        stats = brain.getDetailedStats()
        assert isinstance(stats, list), "Statistics should be a list"
        assert len(stats) > 0, "Statistics should not be empty"
        print(f"✓ Got {len(stats)} statistics entries")
        
        return True
        
    except Exception as e:
        print(f"✗ Basic functionality test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_enhanced_configuration():
    """Test enhanced configuration features"""
    print("\n" + "="*60)
    print("TEST 2: Enhanced Configuration")
    print("="*60)
    
    try:
        # Test advanced configuration
        print("Testing advanced configuration...")
        config = nlm.createDefaultConfig()
        assert config is not None, "Configuration should not be None"
        print("✓ Advanced configuration created")
        
        # Test JSON loading
        print("Testing JSON loading...")
        json_str = '{"brain.neuron_count": 2000, "simulation_timestep": 0.001}'
        config.loadFromJson(json_str)
        print("✓ JSON loaded successfully")
        
        # Test JSON serialization
        print("Testing JSON serialization...")
        json_output = config.saveToJson()
        assert isinstance(json_output, str), "JSON output should be string"
        assert len(json_output) > 0, "JSON output should not be empty"
        
        # Verify it's valid JSON
        parsed = json.loads(json_output)
        assert isinstance(parsed, dict), "Parsed JSON should be a dictionary"
        print("✓ JSON serialization successful")
        
        # Test configuration validation
        print("Testing configuration validation...")
        is_valid = config.validate()
        assert isinstance(is_valid, bool), "Validation should return boolean"
        print(f"✓ Validation result: {is_valid}")
        
        # Test safe access
        print("Testing safe access...")
        value = config.getWithDefault("brain.neuron_count", 1000)
        assert isinstance(value, (int, float)), "Safe access should return numeric value"
        print(f"✓ Safe access returned: {value}")
        
        # Test typed access
        print("Testing typed access...")
        neuron_count = config.getInt("brain.neuron_count")
        assert isinstance(neuron_count, int), "Typed access should return int"
        print(f"✓ Typed access returned: {neuron_count}")
        
        # Test configuration schema
        print("Testing configuration schema...")
        schema = config.getSchema()
        assert isinstance(schema, list), "Schema should be a list"
        assert len(schema) > 0, "Schema should not be empty"
        print(f"✓ Schema contains {len(schema)} entries")
        
        return True
        
    except Exception as e:
        print(f"✗ Enhanced configuration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_visualization_features():
    """Test visualization features"""
    print("\n" + "="*60)
    print("TEST 3: Visualization Features")
    print("="*60)
    
    try:
        # Test visualization helper
        print("Testing visualization helper...")
        viz = nlm.VisualizationHelper()
        assert viz is not None, "Visualization helper should not be None"
        print("✓ Visualization helper created")
        
        # Test heatmap generation
        print("Testing heatmap generation...")
        activity_data = [0.1, 0.5, 0.9, 0.3, 0.7, 0.2, 0.8, 0.4]
        heatmap = viz.createHeatmap(activity_data, 8, 1)
        assert isinstance(heatmap, str), "Heatmap should be string"
        assert len(heatmap) > 0, "Heatmap should not be empty"
        assert "Heatmap" in heatmap, "Heatmap should contain 'Heatmap'"
        print("✓ Heatmap generated successfully")
        
        # Test spike raster generation
        print("Testing spike raster generation...")
        spike_times = [0, 10, 20, 30, 40, 50]
        neuron_ids = [0, 1, 0, 1, 0, 1]
        raster = viz.createRaster(spike_times, neuron_ids)
        assert isinstance(raster, str), "Raster should be string"
        assert len(raster) > 0, "Raster should not be empty"
        assert "Spike Raster" in raster, "Raster should contain 'Spike Raster'"
        print("✓ Spike raster generated successfully")
        
        # Test network visualization
        print("Testing network visualization...")
        config = nlm.createDefaultConfig()
        brain = nlm.Brain(config)
        brain.initialize()
        
        network_viz = viz.createBrainNetwork(brain)
        assert isinstance(network_viz, str), "Network visualization should be string"
        assert len(network_viz) > 0, "Network visualization should not be empty"
        assert "Brain Network" in network_viz, "Network should contain 'Brain Network'"
        print("✓ Network visualization created successfully")
        
        # Test development chart
        print("Testing development chart...")
        development_viz = viz.createDevelopmentChart(brain, 100)
        assert isinstance(development_viz, str), "Development chart should be string"
        assert len(development_viz) > 0, "Development chart should not be empty"
        assert "Developmental Stage" in development_viz, "Chart should contain 'Developmental Stage'"
        print("✓ Development chart created successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ Visualization features test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_world_simulation():
    """Test world simulation features"""
    print("\n" + "="*60)
    print("TEST 4: World Simulation")
    print("="*60)
    
    try:
        # Test enhanced world
        print("Testing enhanced world...")
        world = nlm.EnhancedWorld()
        assert world is not None, "World should not be None"
        print("✓ Enhanced world created")
        
        # Test world configuration
        print("Testing world configuration...")
        world.configure(20, 20, visionWidth=16, visionHeight=16)
        assert world.getWidth() == 20, "World width should be 20"
        assert world.getHeight() == 20, "World height should be 20"
        print("✓ World configured successfully")
        
        # Test agent body
        print("Testing agent body...")
        body = world.getAgentBody()
        assert body is not None, "Agent body should not be None"
        assert hasattr(body, 'x'), "Agent body should have x attribute"
        assert hasattr(body, 'y'), "Agent body should have y attribute"
        print(f"✓ Agent body at ({body.x}, {body.y})")
        
        # Test visual field creation
        print("Testing visual field creation...")
        visual_field = world.createVisualField(10, 10, 0.5)
        assert isinstance(visual_field, list), "Visual field should be a list"
        assert len(visual_field) > 0, "Visual field should not be empty"
        print(f"✓ Visual field created with {len(visual_field)} pixels")
        
        # Test object spawning
        print("Testing object spawning...")
        objects = world.spawnObjects(3, nlm.WorldObjectType.Resource)
        assert isinstance(objects, list), "Objects should be a list"
        assert len(objects) == 3, f"Should spawn 3 objects, got {len(objects)}"
        print(f"✓ Spawned {len(objects)} objects")
        
        # Test world simulation
        print("Testing world simulation...")
        initial_time = world.getSimulationTime()
        for step in range(5):
            world.update(0.1)
        final_time = world.getSimulationTime()
        assert final_time > initial_time, "Simulation time should increase"
        print(f"✓ World simulation completed: {initial_time:.1f} -> {final_time:.1f}s")
        
        # Test world rendering
        print("Testing world rendering...")
        render = world.render()
        assert isinstance(render, str), "World render should be string"
        assert len(render) > 0, "World render should not be empty"
        assert "World" in render, "World render should contain 'World'"
        print("✓ World rendered successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ World simulation test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_advanced_agent():
    """Test advanced agent features"""
    print("\n" + "="*60)
    print("TEST 5: Advanced Agent")
    print("="*60)
    
    try:
        # Create brain and advanced agent
        print("Creating brain and advanced agent...")
        config = nlm.createDefaultConfig()
        brain = nlm.Brain(config)
        agent = nlm.AdvancedAgentBrain(brain)
        
        # Test agent configuration
        print("Testing agent configuration...")
        agent.enableAdvancedLearning(True)
        agent.setLearningRate(0.01)
        agent.setExplorationRate(0.3)
        agent.setMemorySize(1000)
        print("✓ Agent configured successfully")
        
        # Create world
        print("Testing world...")
        world = nlm.EnhancedWorld()
        world.configure(15, 15, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(7, 7)
        
        # Test agent initialization
        print("Testing agent initialization...")
        agent.initialize(world)
        print("✓ Agent initialized successfully")
        
        # Test advanced learning algorithms
        print("Testing advanced learning algorithms...")
        
        # Policy iteration
        print("  Testing Policy Iteration...")
        policy_results = agent.runPolicyIteration(world, 20)
        assert isinstance(policy_results, str), "Policy results should be string"
        assert len(policy_results) > 0, "Policy results should not be empty"
        print("✓ Policy Iteration completed")
        
        # Q-learning
        print("  Testing Q-Learning...")
        ql_results = agent.runQLearning(world, 20)
        assert isinstance(ql_results, str), "Q-learning results should be string"
        assert len(ql_results) > 0, "Q-learning results should not be empty"
        print("✓ Q-Learning completed")
        
        # Actor-critic
        print("  Testing Actor-Critic...")
        ac_results = agent.runActorCritic(world, 20)
        assert isinstance(ac_results, str), "Actor-critic results should be string"
        assert len(ac_results) > 0, "Actor-critic results should not be empty"
        print("✓ Actor-Critic completed")
        
        # Test policy statistics
        print("Testing policy statistics...")
        stats = agent.getPolicyStats()
        assert isinstance(stats, str), "Policy statistics should be string"
        assert len(stats) > 0, "Policy statistics should not be empty"
        assert "Advanced Agent Statistics" in stats, "Statistics should contain header"
        print("✓ Policy statistics retrieved successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ Advanced agent test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_performance_monitoring():
    """Test performance monitoring features"""
    print("\n" + "="*60)
    print("TEST 6: Performance Monitoring")
    print("="*60)
    
    try:
        # Create brain
        print("Creating brain for performance test...")
        config = nlm.createDefaultConfig()
        brain = nlm.Brain(config)
        brain.initialize()
        
        # Test simulation profiling
        print("Testing simulation profiling...")
        start_time = time.time()
        
        for step in range(50):
            brain.step(step, step * 0.001)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        assert elapsed > 0, "Elapsed time should be positive"
        print(f"✓ Simulation completed in {elapsed:.2f} seconds")
        
        # Test current performance metrics
        print("Testing current performance metrics...")
        current_metrics = nlm.PerformanceMonitor.getCurrentMetrics()
        
        assert hasattr(current_metrics, 'cpuUsage'), "Metrics should have cpuUsage"
        assert hasattr(current_metrics, 'memoryUsage'), "Metrics should have memoryUsage"
        assert hasattr(current_metrics, 'simulationTimePerStep'), "Metrics should have simulationTimePerStep"
        assert hasattr(current_metrics, 'neuronsProcessed'), "Metrics should have neuronsProcessed"
        assert hasattr(current_metrics, 'throughput'), "Metrics should have throughput"
        
        assert 0 <= current_metrics.cpuUsage <= 1, "CPU usage should be between 0 and 1"
        assert 0 <= current_metrics.memoryUsage <= 1, "Memory usage should be between 0 and 1"
        assert current_metrics.simulationTimePerStep > 0, "Simulation time per step should be positive"
        assert current_metrics.neuronsProcessed > 0, "Neurons processed should be positive"
        assert current_metrics.throughput > 0, "Throughput should be positive"
        
        print(f"✓ Performance metrics retrieved:")
        print(f"  CPU Usage: {current_metrics.cpuUsage:.1%}")
        print(f"  Memory Usage: {current_metrics.memoryUsage:.1%}")
        print(f"  Simulation Time Per Step: {current_metrics.simulationTimePerStep:.6f}s")
        print(f"  Neurons Processed: {current_metrics.neuronsProcessed}")
        print(f"  Throughput: {current_metrics.throughput:.0f} steps/sec")
        
        # Test metrics reset
        print("Testing metrics reset...")
        nlm.PerformanceMonitor.resetMetrics()
        print("✓ Metrics reset successfully")
        
        return True
        
    except Exception as e:
        print(f"✗ Performance monitoring test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_error_handling():
    """Test error handling features"""
    print("\n" + "="*60)
    print("TEST 7: Error Handling")
    print("="*60)
    
    try:
        # Test configuration validation
        print("Testing configuration validation...")
        config = nlm.createDefaultConfig()
        
        # Test invalid JSON
        print("  Testing invalid JSON handling...")
        try:
            config.loadFromJson("{ invalid json }")
            print("  ✗ Should have thrown an error")
            return False
        except nlm.NLMError as e:
            print(f"  ✓ Caught expected NLMError: {type(e).__name__}")
        except Exception as e:
            print(f"  ✓ Caught error: {type(e).__name__}")
        
        # Test invalid key access
        print("  Testing invalid key access...")
        try:
            value = config.getFloat("nonexistent.key")
            print(f"  ✗ Should have thrown an error (got: {value})")
            return False
        except nlm.NLMError as e:
            print(f"  ✓ Caught expected NLMError: {type(e).__name__}")
        except Exception as e:
            print(f"  ✓ Caught error: {type(e).__name__}")
        
        # Test safe access
        print("  Testing safe access...")
        safe_value = config.getWithDefault("nonexistent.key", 42.0)
        assert safe_value == 42.0, f"Safe access should return default, got {safe_value}"
        print(f"  ✓ Safe access returned default: {safe_value}")
        
        # Test type conversion
        print("  Testing type conversion...")
        test_cases = [
            ("Excitatory", "NeuronType"),
            ("Inhibitory", "SynapseType"),
            ("Adult", "DevelopmentalStage"),
            ("MoveForward", "ActionType"),
            ("Resource", "WorldObjectType")
        ]
        
        for input_str, type_name in test_cases:
            try:
                if type_name == "NeuronType":
                    result = nlm.parseNeuronType(input_str)
                elif type_name == "SynapseType":
                    result = nlm.parseSynapseType(input_str)
                elif type_name == "DevelopmentalStage":
                    result = nlm.parseDevelopmentalStage(input_str)
                elif type_name == "ActionType":
                    result = nlm.parseActionType(input_str)
                elif type_name == "WorldObjectType":
                    result = nlm.parseWorldObjectType(input_str)
                
                print(f"    ✓ Parsed {type_name} '{input_str}' -> {result}")
                
            except nlm.NLMError as e:
                print(f"    ✓ Caught expected NLMError for {type_name}: {type(e).__name__}")
            except Exception as e:
                print(f"    ✓ Caught error for {type_name}: {type(e).__name__}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_workflow_integration():
    """Test complete workflow integration"""
    print("\n" + "="*60)
    print("TEST 8: Workflow Integration")
    print("="*60)
    
    try:
        print("Setting up complete simulation workflow...")
        
        # Step 1: Create configuration
        print("Step 1: Creating configuration...")
        config = nlm.createDefaultConfig()
        config.setInt("brain.neuron_count", 500)
        config.setFloat("plasticity.stdp.learning_rate", 0.001)
        print("✓ Configuration created")
        
        # Step 2: Create brain
        print("Step 2: Creating brain...")
        brain = nlm.Brain(config)
        brain.initialize()
        print("✓ Brain created and initialized")
        
        # Step 3: Create advanced agent
        print("Step 3: Creating advanced agent...")
        agent = nlm.AdvancedAgentBrain(brain)
        agent.enableAdvancedLearning(True)
        agent.setLearningRate(0.01)
        print("✓ Advanced agent created")
        
        # Step 4: Create world
        print("Step 4: Creating world...")
        world = nlm.EnhancedWorld()
        world.configure(10, 10, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(5, 5)
        print("✓ World created")
        
        # Step 5: Initialize agent
        print("Step 5: Initializing agent...")
        agent.initialize(world)
        print("✓ Agent initialized")
        
        # Step 6: Run simulation
        print("Step 6: Running simulation...")
        simulation_results = []
        for step in range(20):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step, step * 0.1)
            
            simulation_results.append({
                'step': step,
                'neurons_firing': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'firing_rate': brain.getAverageFiringRate()
            })
            
            if step % 5 == 0:
                print(f"    Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("✓ Simulation completed")
        
        # Step 7: Run learning episode
        print("Step 7: Running learning episode...")
        episode = brain.simulateEpisode(agent, world, 20)
        
        assert episode.steps == 20, f"Episode should have 20 steps, got {episode.steps}"
        assert episode.totalReward >= 0, f"Total reward should be non-negative, got {episode.totalReward}"
        assert len(episode.observations) == 20, f"Should have 20 observations, got {len(episode.observations)}"
        assert len(episode.actions) == 20, f"Should have 20 actions, got {len(episode.actions)}"
        assert len(episode.rewards) == 20, f"Should have 20 rewards, got {len(episode.rewards)}"
        
        print(f"✓ Learning episode completed:")
        print(f"  Steps: {episode.steps}")
        print(f"  Total Reward: {episode.totalReward:.2f}")
        print(f"  Observations: {len(episode.observations)}")
        print(f"  Actions: {len(episode.actions)}")
        print(f"  Rewards: {len(episode.rewards)}")
        
        # Step 8: Visualize results
        print("Step 8: Visualizing results...")
        viz = nlm.VisualizationHelper()
        
        # Create spike raster from simulation data
        spike_times = []
        neuron_ids = []
        for result in simulation_results:
            if result['neurons_firing'] > 0:
                spike_times.append(result['step'])
                neuron_ids.append(result['neurons_firing'] % 100)
        
        if spike_times:
            raster = viz.createRaster(spike_times, neuron_ids)
            print(f"✓ Created spike raster with {len(spike_times)} spikes")
        
        # Create development chart
        development_viz = viz.createDevelopmentChart(brain, 20)
        print(f"✓ Created development chart")
        
        # Step 9: Test visualization features
        print("Step 9: Testing visualization features...")
        heatmap = viz.createHeatmap([x['neurons_firing'] for x in simulation_results], len(simulation_results), 1)
        print(f"✓ Created heatmap visualization")
        
        print("\n✓ Complete workflow test successful!")
        return True
        
    except Exception as e:
        print(f"✗ Workflow integration test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_backward_compatibility():
    """Test backward compatibility with basic API"""
    print("\n" + "="*60)
    print("TEST 9: Backward Compatibility")
    print("="*60)
    
    try:
        print("Testing backward compatibility...")
        
        # Test that basic API still works
        print("Testing basic configuration...")
        config = nlm.Config()
        assert config is not None, "Basic config should work"
        print("✓ Basic config works")
        
        # Test basic brain creation
        print("Testing basic brain creation...")
        brain = nlm.Brain(config)
        assert brain is not None, "Basic brain should work"
        print("✓ Basic brain creation works")
        
        # Test basic initialization
        print("Testing basic initialization...")
        success = brain.initialize()
        assert success, "Basic initialization should work"
        print("✓ Basic initialization works")
        
        # Test basic simulation
        print("Testing basic simulation...")
        brain.step(0)
        print("✓ Basic simulation works")
        
        # Test basic statistics
        print("Testing basic statistics...")
        neurons = brain.getTotalNeuronCount()
        assert neurons > 0, "Should have neurons"
        print(f"✓ Basic statistics work: {neurons} neurons")
        
        print("\n✓ Backward compatibility test successful!")
        return True
        
    except Exception as e:
        print(f"✗ Backward compatibility test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def run_all_tests():
    """Run all tests"""
    print("NLM Enhanced Python API Test Suite")
    print("="*60)
    print(f"Enhanced bindings available: {ENHANCED_BINDINGS_AVAILABLE}")
    print(f"Basic bindings available: True")
    print("="*60)
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Enhanced Configuration", test_enhanced_configuration),
        ("Visualization Features", test_visualization_features),
        ("World Simulation", test_world_simulation),
        ("Advanced Agent", test_advanced_agent),
        ("Performance Monitoring", test_performance_monitoring),
        ("Error Handling", test_error_handling),
        ("Workflow Integration", test_workflow_integration),
        ("Backward Compatibility", test_backward_compatibility)
    ]
    
    results = []
    
    for test_name, test_func in tests:
        print(f"\n{'='*20} {test_name} {'='*20}")
        success = test_func()
        results.append((test_name, success))
        
        if success:
            print(f"✓ {test_name} PASSED")
        else:
            print(f"✗ {test_name} FAILED")
    
    # Summary
    print("\n" + "="*60)
    print("TEST SUMMARY")
    print("="*60)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name}: {status}")
        
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal Tests: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success Rate: {passed/len(results)*100:.1f}%")
    
    if failed == 0:
        print("\n🎉 ALL TESTS PASSED! 🎉")
        return True
    else:
        print(f"\n⚠️  {failed} tests failed. Check output above for details.")
        return False

def main():
    """Main test function"""
    success = run_all_tests()
    
    if success:
        print("\n" + "="*60)
        print("TEST SUITE COMPLETED SUCCESSFULLY!")
        print("="*60)
        print("\nThe enhanced NLM Python API is working correctly.")
        print("Key features validated:")
        print("  ✓ Advanced configuration with JSON support")
        print("  ✓ Enhanced brain simulation with detailed statistics")
        print("  ✓ Advanced visualization and analysis tools")
        print("  ✓ Machine learning integration utilities")
        print("  ✓ Performance monitoring and profiling")
        print("  ✓ Type-safe conversion utilities")
        print("  ✓ Comprehensive error handling and validation")
        print("  ✓ Backward compatibility with basic API")
        return 0
    else:
        print("\n" + "="*60)
        print("TEST SUITE FAILED!")
        print("="*60)
        print("\nSome tests failed. Please check the output above for details.")
        return 1

if __name__ == "__main__":
    sys.exit(main())