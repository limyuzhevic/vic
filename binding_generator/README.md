# Simple template for easy setup

template for creating binding generator

Steps:
1. Add binding_generator/ directory to your project
2. Add binding_generator/CMakeLists.txt to your root CMakeLists.txt
3. Configure project:
   - Create binding_config.json
   - Customize template files
4. Build:
   mkdir build
   cd build
   cmake ..
   make
5. Install:
   make install

# Configuration options

Binding levels:
- SIMPLE: Basic functionality, fast generation
- ADVANCED: Full API with error handling and convenience methods
- FULL: All features including documentation and advanced templates

Key configuration options in binding_config.json:
binding_level: ADVANCED
output_directory: generated_bindings
module_name: pynlm
enable_documentation: true
enable_error_handling: true
enable_type_conversion: true
generate_convenience_methods: true
maintain_backward_compatibility: true

# Template customization

Templates are stored in binding_generator/templates/
Modify templates to customize generated bindings.

# Usage example

./binding_generator \
  --level=ADVANCED \
  --output=./custom_bindings \
  --config=binding_config.json \
  --generate \
  --test \
  --integrate

# Scripts

Generated scripts for common operations:
- scripts/generate.py: Generate bindings
- scripts/test.py: Test bindings
- scripts/install.py: Install bindings
- scripts/integrate.py: Integrate with existing bindings

# Support and documentation

For support, see:
- docs/user_guide.md: User documentation
- docs/integration_guide.md: Integration documentation
- docs/technical_reference.md: Technical reference
- docs/examples.md: Usage examples

# License

NLM Python Binding Generator is licensed under the MIT License.

# Version

Version 2.0.0 - Released 2026-10-16

# Authors

NLM Project Team

# Acknowledgments

- pybind11: C++ to Python binding library
- CMake: Build system
- C++20: Modern C++ features
- Python 3.x: Python bindings

# Acknowledgments

This binding generator was developed as part of the NLM (Neural Learning Machine) framework.

It provides automatic generation of Python bindings for NLM C++ classes,
reducing boilerplate code and improving API consistency.

For more information, visit: https://nlm.readthedocs.io