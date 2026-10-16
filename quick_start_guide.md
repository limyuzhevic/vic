#!/usr/bin/env python3
"""
NLM Quick Start Guide

A quick start guide for new NLM users, highlighting key features and best practices.
"""

import sys
import os

def print_help():
    """Print the NLM quick start guide."""
    guide = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                 NLM (Neural Learning Machine) Quick Start Guide              ║
╚══════════════════════════════════════════════════════════════════════════════╝

🔧 QUICK SETUP
──────────────────────────────

1. Install NLM with all features:
   pip install pynlm[all-dev]

2. For basic usage (minimal dependencies):
   pip install pynlm

3. For research features:
   pip install pynlm[research]

📚 BASIC USAGE
──────────────────────────────

Create and run a simple brain simulation:

   import pynlm

   # Create brain
   brain = pynlm.create_brain()
   brain.initialize()

   # Run simulation
   for step in range(100):
       brain.step(step)

   # View statistics
   print(brain.get_stats())

🌟 ENHANCED FEATURES
──────────────────────────────

1. Context Manager for controlled execution:

   with pynlm.create_simulation_context() as brain:
       for step in range(100):
           brain.step(step)

2. Configuration validation:

   config = pynlm.create_default_config()
   config.set("brain.neuron_count", 2000)
   config.set("development.enable", True)

   brain = pynlm.create_brain(config)  # Validates configuration

3. Property-based access:

   brain = pynlm.create_brain()
   print(f"Neurons: {brain.get_property('total_neurons')}")
   print(f"Firing rate: {brain.get_property('average_firing_rate')}")

🔬 RESEARCH FEATURES
──────────────────────────────

Run research experiments with monitoring and analysis:

   from research_utils import run_research_simulation

   results = run_research_simulation(
       steps=1000,
       save_interval=100,
       enable_monitoring=True,
       export_data=True,
       save_plots=True
   )

📊 ANALYSIS AND VISUALIZATION
──────────────────────────────

Analyze brain dynamics and generate plots:

   from research_utils import analyze_brain_network

   analysis = analyze_brain_network(brain, steps=500)
   print(f"Network has {analysis['network_topology']['total_neurons']} neurons")

🏃 PERFORMANCE PROFILING
──────────────────────────────

Profile brain performance:

   from research_utils import profile_brain_performance

   profile = profile_brain_performance(brain, steps=100)
   print(f"Steps per second: {profile['performance_summary']['steps_per_second']:.2f}")

📄 CLI TOOLS
──────────────────────────────

Command-line interface for common operations:

   # Create brain
   nlm-cli create-brain --neuron-count 1000

   # Run simulation
   nlm-cli run-simulation 100

   # Run research experiment
   nlm-research run-experiment my_experiment

📖 DOCUMENTATION
──────────────────────────────

For comprehensive documentation:

   • Python API Reference: python/API_REFERENCE.md
   • Research Utilities: research_utils.py (with examples)
   • NLM CLI: nlm_cli.py
   • Brain CLI: scripts/brain_cli.py

🔧 DEVELOPMENT SETUP
──────────────────────────────

For developers contributing to NLM:

   1. Install development dependencies:
      pip install pynlm[dev]

   2. Run tests:
      pytest tests/

   3. Code formatting:
      black .

   4. Linting:
      flake8 .

   5. Type checking:
      mypy .

📝 BUILDING FROM SOURCE
──────────────────────────────

```bash
# Install build dependencies
pip install scikit-build-core pybind11

# Build from source
python -m build

# Or use CMake directly
mkdir build
cd build
cmake .. -DNLM_ENABLE_PYTHON_CLI=ON
make -j4
```

🌍 COMMUNITY AND SUPPORT
──────────────────────────────

• GitHub: https://github.com/nlm-project/nlm
• Documentation: https://nlm.readthedocs.io
• Issues: https://github.com/nlm-project/nlm/issues
• Discussions: https://github.com/nlm-project/nlm/discussions

──────────────────────────────────────────────────────────────────────────────────────────────────────

NLM is an experimental research project exploring computational brain-like systems.
Use it responsibly and contribute to advancing our understanding of neural computation!

╔══════════════════════════════════════════════════════════════════════════════╗
║                          END OF QUICK START GUIDE                          ║
╚══════════════════════════════════════════════════════════════════════════════╝
"""

    print(guide)

if __name__ == "__main__":
    print_help()