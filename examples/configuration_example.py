import pynlm

print("\n=== Configuration Factory Example ===")

# Test different configuration types
configs = [
    ("Default", pynlm.EnhancedConfig.createDefault()),
    ("Speed", pynlm.EnhancedConfig.createForSpeed()),
    ("Accuracy", pynlm.EnhancedConfig.createForAccuracy()),
    ("Learning", pynlm.EnhancedConfig.createForLearning()),
]

for name, config in configs:
    print(f"\n{name} Configuration:")
    print(f"  Neurons: {config.getNeuronCount()}")
    print(f"  Regions: {config.getRegionCount()}")
    print(f"  Step size: {config.getTimestep()}s")
    print(f"  Random seed: {config.getRandomSeed()}")
