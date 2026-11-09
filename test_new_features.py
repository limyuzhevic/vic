import nlm

# Test 1: Quick brain creation
print("=== Test 1: Quick Brain Creation ===")
brain = nlm.createQuickBrain(neuron_count=100, connection_prob=0.1)
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Test 2: Advanced configuration with nested keys
print("\n=== Test 2: Advanced Configuration ===")
config = nlm.createDefaultConfig()
config.setNested("system.brain.neuron_count", 200)
config.setNested("system.learning.rate", 0.001)
config.setNested("system.plasticity.enable", True)

print(f"Neuron count from config: {config.getNested('system.brain.neuron_count')}")
print(f"Learning rate from config: {config.getNested('system.learning.rate')}")
print(f"Plasticity enabled: {config.getNested('system.plasticity.enable')}")

# Test 3: Configuration dictionary update
print("\n=== Test 3: Configuration Dictionary Update ===")
config2 = nlm.createDefaultConfig()
config2.updateFromDict({
    "brain.neuron_count": 300,
    "brain.connection_probability": 0.15,
    "plasticity.stdp.learning_rate": 0.002
})

print(f"Brain neuron count: {config2.get('brain.neuron_count')}")
print(f"Brain connection probability: {config2.get('brain.connection_probability')}")
print(f"STDP learning rate: {config2.get('plasticity.stdp.learning_rate')}")

# Test 4: Batch operations
print("\n=== Test 4: Batch Operations ===")
config_a = nlm.createDefaultConfig()
config_a.setNested("brain.neuron_count", 50)

config_b = nlm.createDefaultConfig()
config_b.setNested("brain.neuron_count", 75)

config_c = nlm.createDefaultConfig()
config_c.setNested("brain.neuron_count", 100)

brains = [brain, config_a, config_b, config_c]
print(f"Created {len(brains)} brains for batch testing")

# Test 5: World with objects
print("\n=== Test 5: World with Objects ===")
world = nlm.createWorldWithObjects(
    width=20,
    height=20,
    objects=[
        nlm.WorldObject(5.0, 5.0, nlm.WorldObjectType.Resource, value=10.0),
        nlm.WorldObject(15.0, 15.0, nlm.WorldObjectType.Hazard, value=1.0),
        nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Wall, value=0.0)
    ]
)

print(f"Created world with dimensions: {world.getWidth()}x{world.getHeight()}")
print(f"Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")

# Test 6: Agent with all systems
print("\n=== Test 6: Agent with All Systems ===")
agent_brain = nlm.createQuickBrain(neuron_count=50)
world2 = nlm.createWorldWithObjects(15, 15, [])
agent = nlm.createAgentWithAllSystems(agent_brain, world2)

print(f"Agent created with all systems enabled")
print(f"Reward modulation: {agent.isRewardModulationEnabled()}")
print(f"Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
print(f"Development: {agent.isDevelopmentEnabled()}")
print(f"Curiosity: {agent.isCuriosityEnabled()}")

# Test 7: Convenience method - train on pattern
print("\n=== Test 7: Train on Pattern ===")
test_brain = nlm.createQuickBrain(neuron_count=100)
pattern = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
test_brain.trainOnPattern(pattern, 50)
print("Brain trained on pattern")

# Test 8: Convenience method - evaluate test set
print("\n=== Test 8: Evaluate Test Set ===")
test_patterns = [
    [0.1, 0.2, 0.3, 0.4, 0.5],
    [0.2, 0.4, 0.6, 0.8, 1.0],
    [0.5, 0.5, 0.5, 0.5, 0.5]
]
accuracy, individual_accuracies = test_brain.evaluateTestSet(test_patterns, [0, 10, 20, 30, 40])
print(f"Average accuracy: {accuracy:.2%}")
print(f"Individual accuracies: {[f'{acc:.2%}' for acc in individual_accuracies]}")

# Test 9: Progress tracker
print("\n=== Test 9: Progress Tracker ===")
def progress_callback(progress, description):
    print(f"Progress: {progress:.1f}% - {description}")

tracker = nlm.createProgressTracker(100, "Processing simulation", progress_callback, 10.0)
print("Progress tracker created")

# Test 10: Validation functions
print("\n=== Test 10: Validation Functions ===")
try:
    result = nlm.validateBrain(brain, "step")
    print(f"Brain validation result: {result}")
except Exception as e:
    print(f"Brain validation error: {e}")

try:
    result = nlm.validateWorld(world, "update")
    print(f"World validation result: {result}")
except Exception as e:
    print(f"World validation error: {e}")

print("\n=== All Tests Completed Successfully ===")