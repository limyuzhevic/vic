# Quick Test of Python Bindings Improvements

This file demonstrates the key improvements made to the NLM Python bindings.

## Test 1: Basic Functionality with New Convenience Functions
```python
import pynlm
import json

# Test 1.1: Create brain with optimized configuration (NEW)
print("=== Test 1.1: Configuration Factory ===")
brain = pynlm.createBrain(pynlm.create_research_config())
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Test 1.2: Mathematical utilities (NEW)
print("\n=== Test 1.2: Mathematical Utilities ===")
value = pynlm.clamp(15.5, 0.0, 10.0)
print(f"Clamped 15.5 to [0,10]: {value}")

normalized = pynlm.normalize(5.0, 0.0, 10.0)
print(f"Normalized 5.0 from [0,10] to [0,1]: {normalized}")

interpolated = pynlm.lerp(0.5, 10.0, 20.0)
print(f"Linear interpolation t=0.5 from 10 to 20: {interpolated}")

# Test 1.3: Validation functions (NEW)
print("\n=== Test 1.3: Validation Functions ===")
try:
    pynlm.validate_neuron_count(100)
    print("Neuron count validation passed")
except Exception as e:
    print(f"Validation failed: {e}")

# Test 1.4: Performance monitoring (NEW)
print("\n=== Test 1.4: Performance Monitoring ===")
elapsed_ms = pynlm.profile_brain_step(brain, 100)
print(f"Brain simulation took {elapsed_ms} ms")

# Test 1.5: Metrics export (NEW)
print("\n=== Test 1.5: Metrics Export ===")
metrics = pynlm.export_metrics(brain)
metric_data = json.loads(metrics)
print(f"Brain firing efficiency: {metric_data.get('firing_efficiency', 'N/A'):.2%}")
print(f"Total spike count: {metric_data.get('total_spike_count', 'N/A')}")
```

## Test 2: Advanced AgentBrain Features
```python
import pynlm

# Test 2.1: Create comprehensive system
print("\n=== Test 2.1: Advanced System Creation ===")
config = pynlm.create_production_config()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Test 2.2: Enable all subsystems
print("\n=== Test 2.2: Subsystem Enable/Disable ===")
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
print("All subsystems enabled successfully")

# Test 2.3: Debug statistics (NEW)
print("\n=== Test 2.3: Debug Statistics ===")
stats = agent.get_detailed_statistics()
print(f"Agent neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Agent novelty level: {agent.getNoveltyLevel():.3f}")

# Test 2.4: Phase 6 advanced features (NEW)
print("\n=== Test 2.4: Phase 6 Advanced Features ===")
internal_rates = agent.get_internal_firing_rates()
print(f"Internal firing rates count: {len(internal_rates)}")

thought_trace = agent.get_thought_trace()
print(f"Thought trace: {thought_trace}")

planner_stats = agent.get_planner_counts()
print(f"Planner override count: {planner_stats}")

# Test 2.5: Comprehensive performance report (NEW)
print("\n=== Test 2.5: Performance Report ===")
report = pynlm.generate_performance_report(brain, agent)
report_data = json.loads(report)
print(f"Performance report generated with {len(report_data)} metrics")
print(f"Development progress: {report_data.get('performance', {}).get('development_progress', 'N/A'):.1%}")
```

## Test 3: Configuration Management
```python
import pynlm

# Test 3.1: Create brain from file (NEW)
print("\n=== Test 3.1: Configuration File I/O ===")
try:
    config = pynlm.createConfigFromFile("config.json")
    print("Config loaded from file successfully")
except Exception as e:
    print(f"Expected error (file not found): {e}")

# Test 3.2: Configuration presets (NEW)
print("\n=== Test 3.2: Configuration Presets ===")
research_config = pynlm.createConfigWithPreset("research")
print("Research configuration created")

production_config = pynlm.createConfigWithPreset("production")
print("Production configuration created")

# Test 3.3: Merge configurations (NEW)
print("\n=== Test 3.3: Configuration Merging ===")
config1 = pynlm.create_default_config()
config2 = pynlm.create_research_config()
merged = pynlm.merge_configs([config1, config2])
print("Configurations merged successfully")
```

## Test 4: Simulation Capabilities
```python
import pynlm

# Test 4.1: Create world and run episode (NEW)
print("\n=== Test 4.1: Complete Episode Simulation ===")
config = pynlm.create_default_config()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation
reward = pynlm.simulate_episode(brain, world, agent, 50)
print(f"Episode simulation completed with total reward: {reward:.3f}")

# Test 4.2: Visualization and debugging config (NEW)
print("\n=== Test 4.2: Visualization Configuration ===")
viz_config = pynlm.create_visualization_config()
print("Visualization configuration created")

# Test 4.3: Debug data export/import (NEW)
print("\n=== Test 4.3: Debug Data Export ===")
debug_data = pynlm.export_debug_data(brain, agent)
print(f"Debug data exported ({len(debug_data)} characters)")

# Test 4.4: Performance benchmarking (NEW)
print("\n=== Test 4.4: Performance Benchmarking ===")
benchmark_result = pynlm.benchmark_brain(brain, 100)
elapsed_ms, ops_per_sec = benchmark_result
print(f"Performance: {elapsed_ms} ms elapsed, {ops_per_sec:.0f} ops/sec")
```

## Summary of Improvements

✅ **Error Handling**: Custom exceptions and validation
✅ **Convenience Functions**: Configuration factories and utilities
✅ **Performance Monitoring**: Profiling and benchmarking tools
✅ **Advanced Features**: Phase 5/6 debugging and statistics
✅ **Configuration Management**: File I/O and preset management
✅ **Mathematical Utilities**: Clamping, normalization, interpolation
✅ **Debugging Tools**: Debug data export and comprehensive reporting
✅ **Visualization**: Configuration helpers for advanced users

All improvements maintain backward compatibility while adding powerful new features for enhanced functionality and debugging capabilities.
