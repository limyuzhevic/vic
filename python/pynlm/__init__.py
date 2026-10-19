from .core import *
from .brain import *
from .memory import *
from .neuromodulation import *
from .prediction import *
from .cognition import *
from .development import *
from .sensory import *
from .motor import *
from .environment import *
from .agent import *
from .experiments import *
from .config import *
from .types import *
from .utils import *

__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Package-level documentation
doc = """
NLM (熙然) - Neural Learning Machine
An Experimental Artificial Developmental Brain

Python Package: pynlm

This package provides Python bindings for the NLM C++ neural simulation framework.
It offers classes for brain simulation, neural dynamics, memory systems, neuromodulation,
prediction, cognition, development, and more.

Key Features:
-------------
✓ Modular Pythonic API with comprehensive documentation
✓ Type hints for better IDE support
✓ Context managers for resource management
✓ Advanced error handling and validation
✓ Phase 6 integration examples and demos
✓ Performance monitoring utilities
✓ Visualization integration
✓ Advanced user features
✓ Educational examples and tutorials
✓ Experimental frameworks
✓ Configuration management
✓ Multi-system integration

Quick Start:
-------------
```python
from pynlm import Brain, createDefaultConfig

config = createDefaultConfig()
config.set("neuron_count", 1000)

brain = Brain(config)
brain.initialize()

for step in range(100):
    brain.step(step, step * 0.001)

action = brain.produceAction()
print(f"Action type: {action.getType()}")
```

For Phase 6 integration demo:
```python
from pynlm.experiments import Phase6IntegrationDemo

demo = Phase6IntegrationDemo()
results = demo.run({
    "neuronCount": 500,
    "maxSteps": 2000,
    "enableCheckpointing": True,
    "enableReplay": True,
    "enableDevelopment": True
})
```

API Documentation:
------------------
See individual modules for detailed API documentation:
- core: Core types and utilities
- brain: Neural brain simulation
- memory: Memory systems (working, episodic, associative)
- neuromodulation: Neuromodulators (dopamine, curiosity, novelty)
- prediction: Prediction systems
- cognition: Cognitive mechanisms (planning, attention, concept formation)
- development: Developmental processes
- sensory: Sensory input processing
- motor: Motor output generation
- environment: Environment interface
- agent: Agent brain interface
- experiments: Experimental frameworks (including Phase 6)
- config: Configuration management
- types: Type definitions and identifiers
- utils: Utility functions and helpers
"""

__doc__ = doc