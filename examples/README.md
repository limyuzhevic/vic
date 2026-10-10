# NLM Examples

This directory contains example scripts demonstrating how to use the NLM Python bindings.

## Overview

These examples showcase the capabilities of the NLM (Neural Learning Machine) framework
and provide templates for common usage patterns.

## Examples

### 1. Minimal Example (`minimal_example.py`)

**Description:** The simplest possible NLM usage. Creates a virtual brain and runs it for a few steps.

**Learning Objectives:**
- Basic brain creation and initialization
- Running simulation steps
- Accessing brain statistics

**Usage:**
```bash
python examples/minimal_example.py
```

### 2. Agent Example (`agent_example.py`)

**Description:** Complete agent-world simulation with learning and development.

**Learning Objectives:**
- Setting up agent-world interaction
- Enabling learning subsystems (reward modulation, curiosity, development)
- Running complete simulation loops
- Monitoring agent behavior

**Usage:**
```bash
python examples/agent_example.py
```

### 3. Performance Benchmark (`benchmark.py`)

**Description:** Performance testing and configuration optimization.

**Learning Objectives:**
- Measuring simulation performance
- Comparing different configurations
- Understanding performance trade-offs
- Optimizing for different use cases

**Usage:**
```bash
python examples/benchmark.py
```

## How to Run Examples

### Prerequisites

1. **Build NLM Python bindings:**
   ```bash
   cd /path/to/nlm
   python python/build.py --install
   ```

2. **Install additional dependencies (for benchmark example):**
   ```bash
   pip install matplotlib numpy
   ```

### Running Examples

All examples can be run from the project root:

```bash
cd /path/to/nlm
python examples/minimal_example.py
python examples/agent_example.py
python examples/benchmark.py
```

### Expected Output

Each example will produce output showing:
- Simulation progress and statistics
- Performance metrics (for benchmark)
- Final results and recommendations

## Customizing Examples

### For Your Own Projects

1. **Modify configurations:**
   - Change neuron counts for different scales
   - Adjust synapse densities for different connectivity
   - Enable/disable learning mechanisms as needed

2. **Create variations:**
   - Replace world configurations with different environments
   - Modify learning parameters for specific tasks
   - Add your own monitoring and logging

3. **Integration:**
   - Call these examples as functions from your own code
   - Import NLM components directly
   - Build upon the provided templates

## Learning Path

### Beginner (1-2 weeks)

1. Start with `minimal_example.py`
2. Understand basic brain creation and simulation
3. Try `agent_example.py` for agent-world interaction
4. Experiment with different configurations

### Intermediate (2-4 weeks)

1. Study the benchmark example for performance tuning
2. Create your own variations
3. Add monitoring and visualization
4. Implement custom learning tasks

### Advanced (4+ weeks)

1. Combine multiple examples
2. Create complex agent behaviors
3. Optimize for specific use cases
4. Contribute to the examples collection

## Troubleshooting

### Common Issues

**"Module not found: pynlm"**
- Ensure you've built and installed the Python bindings
- Run: `python python/build.py --install`

**"ImportError: libpynlm.so: cannot open shared library"**
- Set LD_LIBRARY_PATH on Linux: `export LD_LIBRARY_PATH=/path/to/nlm/build:$LD_LIBRARY_PATH`
- Set DYLD_LIBRARY_PATH on macOS: `export DYLD_LIBRARY_PATH=/path/to/nlm/build:$DYLD_LIBRARY_PATH`

**"No module named matplotlib" (benchmark only)**
- Install with: `pip install matplotlib numpy`

### Getting Help

1. **Check documentation:** Look at `python/DOCUMENTATION.md`
2. **Check examples:** Each example has detailed comments
3. **Network debugging:** Use network tools to find issues
4. **Stack traces:** Run with full traceback to see errors

## Contributing Examples

To add your own examples:

1. Create a new Python script in this directory
2. Follow the existing template format
3. Include clear documentation and comments
4. Add to the README.md in this directory
5. Submit a pull request

## Example Template

When creating new examples, follow this template:

```python
#!/usr/bin/env python3

"""
[Short description of the example]

[Explanation of what this example demonstrates]
"""

import pynlm

def main():
    # Your code here
    pass

if __name__ == "__main__":
    main()
```

## Further Reading

- **Main Documentation:** `python/DOCUMENTATION.md`
- **Build Instructions:** `python/build.py` (with --help)
- **API Reference:** Check individual class methods
- **Research Papers:** Available in the NLM documentation

## License

These examples are provided under the MIT License of the NLM project.

## Copyright

Copyright (c) 2025 NLM Project. All rights reserved.