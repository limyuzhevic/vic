"""Quick Start: NLM Python Bindings

This is the simplest example for getting started with NLM.
Copy and paste this code to create your first virtual brain!
"""

import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("It has fired", brain.getTotalSpikeCount(), "times so far.")
print("\nYour first brain is working! 🎉")