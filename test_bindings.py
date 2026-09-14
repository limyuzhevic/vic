# Test Python file to verify the bindings

import pynlm

print("Testing NLM Python bindings...")

# Test basic types
print("\nTesting basic types:")
print(f"NeuronId: {pynlm.NeuronId(1)}")
print(f"SynapseId: {pynlm.SynapseId(2)}")

# Test enums
print(f"\nNeuronType.Excitatory: {pynlm.NeuronType.Excitatory}")
print(f"SynapseType.Excitatory: {pynlm.SynapseType.Excitatory}")
print(f"ActionType.MoveForward: {pynlm.ActionType.MoveForward}")

# Test configuration
config = pynlm.Config()
print(f"\nConfig created: {config}")

# Test world
world = pynlm.SimpleWorld()
print(f"World created: {world}")
print(f"World dimensions: {world.getWidth()} x {world.getHeight()}")

# Test sensory input
vision = pynlm.Vision(100, 100)
print(f"\nVision created: {vision.getWidth()} x {vision.getHeight()} x {vision.getChannels()} channels")

# Test action
action = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"\nAction created: {action.getName()}, type: {action.getType()}")

# Test plasticity rules
stdp = pynlm.STDP()
print(f"\nSTDP created: name={stdp.getName()}, LTP={stdp.getLTPWeight()}")

hebbian = pynlm.Hebbian()
print(f"Hebbian created: name={hebbian.getName()}, rate={hebbian.getLearningRate()}")

print("\nAll basic tests passed!")
