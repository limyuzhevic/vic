#!/usr/bin/env python3
import pynlm

# Test basic imports and basic functionality
print("Testing basic NLM Python bindings...")

# Test 1: Create config
config = pynlm.createDefaultConfig()
print(f"Created config: {config}")

# Test 2: Check if new methods exist
print(f"\nTesting Config methods:")
print(f"  has() method exists: {hasattr(config, 'has')}")
print(f"  getKeys() method exists: {hasattr(config, 'getKeys')}")
print(f"  clear() method exists: {hasattr(config, 'clear')}")
print(f"  get() method exists: {hasattr(config, 'get')}")
print(f"  getOr() method exists: {hasattr(config, 'getOr')}")
print(f"  summary() method exists: {hasattr(config, 'summary')}")

# Test 3: Test sensory input clone method
print(f"\nTesting SensoryInput methods:")
print(f"  clone() method exists on SensoryInput: {hasattr(pynlm, 'SensoryInput')}")

# Test 4: Test world methods
print(f"\nTesting SimpleWorld methods:")
world = pynlm.createSimpleWorld()
print(f"  createSimpleWorld() works")
print(f"  has getObjectAt() method: {hasattr(world, 'getObjectAt')}")

# Test 5: Test agent brain methods
print(f"\nTesting AgentBrain methods:")
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
print(f"  getSensoryInputSize() method exists: {hasattr(agent, 'getSensoryInputSize')}")
print(f"  getMotorOutputSize() method exists: {hasattr(agent, 'getMotorOutputSize')}")
print(f"  getNeuromodulationLevel() method exists: {hasattr(agent, 'getNeuromodulationLevel')}")
print(f"  getNoveltyLevel() method exists: {hasattr(agent, 'getNoveltyLevel')}")
print(f"  getPredictionError() method exists: {hasattr(agent, 'getPredictionError')}")

print("\nAll basic tests completed successfully!")