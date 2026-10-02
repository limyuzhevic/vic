# NLM Documentation

## Overview

This directory contains documentation for the NLM (Neural Learning Machine) project - an experimental artificial developmental brain simulation system.

## Documentation Structure

### Main Documentation (README.md)
- **Purpose**: Comprehensive user guide, setup instructions, and API reference
- **Content**: Installation, usage examples, configuration, troubleshooting
- **Recommendation**: Read this first for getting started

### Technical Documentation (docs/ directory)
- **ARCHITECTURE.md**: System architecture and component design
- **SCIENCE.md**: Scientific background and biological principles
- **EXPERIMENTS.md**: Experiment descriptions and results
- **ROADMAP.md**: Development roadmap and future plans
- **PHASE6_FINAL_AUDIT.md**: Phase 6 integration audit results
- **PERFORMANCE.md**: Performance characteristics and benchmarks
- **SCALING.md**: Scalability and optimization insights

### Python API Examples

The main README.md contains extensive Python examples. For quick reference:

```python
import pynlm

# Create and initialize a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Key Resources

1. **README.md** - Complete getting started guide and API reference
2. **docs/ARCHITECTURE.md** - Deep dive into system architecture
3. **docs/SCIENCE.md** - Scientific principles and background
4. **docs/EXPERIMENTS.md** - Experiment results and insights
5. **docs/PHASE6_FINAL_AUDIT.md** - Phase 6 integration verification

## Project Status

NLM is currently in **Phase 6: Final Integration**. All major systems have been integrated into a coherent artificial brain architecture that:

- Integrates memory systems with neural processing
- Connects neuromodulation with plasticity and dynamics
- Integrates prediction with learning
- Connects development with plasticity rates
- Implements checkpoint save/load functionality
- Enables replay and consolidation

## Getting Started

1. **Read README.md** for complete setup and usage instructions
2. **Follow the quick start examples** in the README
3. **Explore technical documentation** for deeper understanding
4. **Run demos** to see the integrated system in action

## Support

For questions or issues:
- Check documentation in README.md and docs/
- Refer to experiment results in docs/EXPERIMENTS.md
- Review performance notes in docs/PERFORMANCE.md

This documentation is continuously being updated as the project evolves.