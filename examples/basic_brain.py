"""Basic brain example demonstrating the Enhanced API."""

import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from pynlm.enhanced import (
    create_enhanced_brain, 
    create_enhanced_config,
    create_enhanced_world,
    create_enhanced_agent,
    ConfigBuilder,
    BrainManager
)

print("Basic Brain Example - Enhanced NLM API")
print("=" * 50)

# Example 1: Using ConfigBuilder
print("\n1. Creating brain with ConfigBuilder:")
builder = ConfigBuilder()
builder.set_brain(neuron_count=500, connection_probability=0.05)
config = builder.create()
print(f"   Created config with {len(builder.to_dict())} parameters")

# Example 2: Using BrainManager
print("\n2. Using BrainManager:")
manager = BrainManager(config)
brain1 = manager.create_brain("Brain1")
brain2 = manager.create_brain("Brain2")
print(f"   Created brains: {manager.list_brains()}")

# Example 3: Basic brain simulation
print("\n3. Simulating brain:")
brain = create_enhanced_brain(name="SimulationBrain")
brain.initialize()
print(f"   Brain initialized with {brain.getTotalNeuronCount()} neurons")

# Example 4: Using iterator interface
print("\n4. Using iterator interface:")
count = 0
for brain_state in brain:
    count += 1
    if count >= 3:  # Just show first 3
        break
print(f"   Iterated through {count} brain states")

# Example 5: Analysis
print("\n5. Analyzing brain:")
brain.step(10)
brain.step(11)
brain.step(12)
analysis = brain.analyze()
print(f"   Analysis: {analysis}")
print(f"   Stability: {analysis.get_stability():.2f}")
print(f"   Total spikes: {analysis.stats.get('total_spikes', 0)}")

# Example 6: Using context manager
print("\n6. Using training context manager:")
with brain.training_context(steps=5, enable_learning=True):
    pass  # Training would happen here
print(f"   Training context completed")

print("\n" + "=" * 50)
print("Basic brain example completed successfully!")
