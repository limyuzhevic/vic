# NLM API Documentation Generator

A comprehensive tool to generate API documentation from NLM source code.

## Overview

This tool analyzes NLM source code and generates comprehensive API documentation including:
- Class hierarchies
- Method signatures and descriptions
- Dependencies between components
- Usage examples
- Configuration parameters

## Features

### 1. Source Code Analysis
- Automatically extracts class definitions
- Identifies method signatures and parameters
- Analyzes dependencies between components
- Detects configuration parameters

### 2. Documentation Generation
- HTML documentation with searchable index
- Markdown format for integration with existing docs
- LaTeX format for academic papers
- JSON format for machine-readable documentation

### 3. Usage Examples
- Automatically generates usage examples
- Includes code snippets for common tasks
- Provides best practices and patterns
- Includes error handling examples

### 4. Interactive Documentation
- Web-based documentation browser
- Search functionality
- Cross-references between components
- Hierarchical navigation

## Installation

```bash
# Clone the NLM repository
./build_docs.sh

# Or install via package manager
./install_docs.sh
```

## Usage

### Command Line Interface

```bash
# Generate all documentation formats
nlm_docs --format all --output ./docs/api

# Generate HTML documentation
nlm_docs --format html --output ./docs/api

# Generate Markdown documentation
nlm_docs --format markdown --output ./docs/api

# Generate JSON documentation
nlm_docs --format json --output ./docs/api

# Analyze specific components
nlm_docs --components brain --output ./docs/api/brain

# Generate usage examples
nlm_docs --examples --output ./docs/api/examples
```

### Python API

```python
from nlm_docs import generate_documentation

# Generate complete API documentation
generate_documentation(
    output_format='html',
    output_path='./docs/api',
    components=['brain', 'memory', 'neuromodulation']
)

# Generate specific documentation for a component
generate_documentation(
    output_format='markdown',
    output_path='./docs/api',
    source_files=['src/brain/Brain.hpp', 'src/brain/Neuron.hpp']
)
```

### Configuration

The documentation generator can be configured via:

**Environment Variables:**
- `NLM_DOCS_SOURCE_DIR`: Source code directory (default: ./src)
- `NLM_DOCS_OUTPUT_DIR`: Output directory (default: ./docs/api)
- `NLM_DOCS_FORMAT`: Output format (html, markdown, json, latex)
- `NLM_DOCS_TEMPLATE`: Template directory

**Configuration File:**
```yaml
# docs/config.yaml
output:
  directory: ./docs/api
  formats:
    - html
    - markdown
    - json
    - latex

source:
  directory: ./src
  include:
    - "**/*.hpp"
    - "**/*.cpp"
  exclude:
    - "*/tests/*"
    - "*/benchmarks/*"

analysis:
  extract:
    classes: true
    methods: true
    parameters: true
    dependencies: true
    examples: true

examples:
  generate_usage: true
  include_tests: true
  complexity_level: "medium"
```

## Generated Documentation Structure

```
./docs/api/
├── index.html
├── classes/
│   ├── brain.html
│   ├── neuron.html
│   ├── synapse.html
│   └── ...
├── modules/
│   ├── core.html
│   ├── memory.html
│   ├── neuromodulation.html
│   └── ...
├── examples/
│   ├── basic_usage.html
│   ├── experiments.html
│   ├── configuration.html
│   └── ...
├── search/
│   ├── index.js
│   └── data.json
├── api.json
├── README.md
└── config.yaml
```

## Available Documentation Components

### Core System
- **Config**: Configuration management
- **Logger**: Logging system
- **Random**: Random number generation
- **Types**: Core type definitions

### Neural Components
- **Brain**: Main brain class
- **Neuron**: Single neuron model
- **Synapse**: Synaptic connections
- **NeuralPopulation**: Population of neurons
- **NeuralRegion**: Brain regions

### Memory Systems
- **WorkingMemory**: Short-term memory
- **EpisodicMemory**: Episodic memory
- **SemanticMemory**: Semantic memory
- **ProceduralMemory**: Procedural memory

### Neuromodulation
- **Neuromodulator**: Neuromodulator signals
- **Reward**: Reward signals
- **Curiosity**: Curiosity signals
- **Novelty**: Novelty detection

### Cognitive Systems
- **Attention**: Attention system
- **Planner**: Planning system
- **ConceptFormation**: Concept formation
- **PredictionSystem**: Prediction system

### Plasticity
- **PlasticityRule**: Plasticity rules
- **STDP**: Spike-timing dependent plasticity
- **Hebbian**: Hebbian learning
- **StructuralPlasticity**: Structural changes

### Development
- **DevelopmentSystem**: Brain development
- **Maturation**: Maturation process
- **Pruning**: Pruning process
- **Synaptogenesis**: Synaptogenesis

### Environment
- **SimpleWorld**: Simple environment
- **Observation**: Observation data
- **Action**: Action representation

### Agent
- **AgentBrain**: Agent brain interface
- **SensoryPercept**: Sensory perception

### Experiments
- **Phase3Experiment**: Phase 3 experiments
- **Phase4Experiment**: Phase 4 experiments
- **Phase5Experiment**: Phase 5 experiments
- **Phase6IntegratedExperiment**: Phase 6 experiments

## Technical Details

### Source Code Analysis

The documentation generator uses a combination of:

1. **Static Analysis**: Parses header files to extract class definitions
2. **AST Analysis**: Uses Clang libTooling for deep analysis
3. **Dependency Tracking**: Analyzes include relationships
4. **Pattern Recognition**: Identifies common code patterns

### Template System

The documentation generator uses Jinja2 templates to generate:

- **HTML Documentation**: Interactive web documentation
- **Markdown Documentation**: GitHub-compatible documentation
- **JSON Documentation**: Machine-readable API specification
- **LaTeX Documentation**: Academic paper format

### Content Generation

For each analyzed class, the generator creates:

- **Class Documentation**: Overview, purpose, and usage
- **Method Documentation**: Parameters, return values, and examples
- **Inheritance Hierarchy**: Class relationships
- **Usage Examples**: Code snippets and best practices
- **Error Handling**: Common errors and solutions
- **Configuration**: Required parameters and defaults

## Advanced Features

### Custom Documentation

Users can provide custom documentation:

```cpp
/**
 * @class CustomClass
 * @brief A custom class for special purposes
 * @author Your Name
 * @date 2024
 * @version 1.0
 * @note This class is used for...
 * @warning This class may cause memory issues
 * @see RelatedClass
 * @ingroup special
 * 
 * Example usage:
 * @code{.cpp}
 * CustomClass obj;
 * obj.doSomething();
 * @endcode
 */
class CustomClass {
    // ...
};
```

### Cross-References

The documentation generator automatically creates:

- **Cross-references** between classes
- **Dependency graphs** of components
- **Usage patterns** and examples
- **Error guides** and troubleshooting

### Search and Navigation

Features include:

- **Full-text search** across documentation
- **Hierarchical navigation** of components
- **Quick reference** for common operations
- **Table of contents** with automatic numbering

## Integration with Existing Documentation

The generated documentation integrates seamlessly with:

- **GitHub**: Markdown files for GitHub Pages
- **ReadTheDocs**: HTML documentation hosting
- **Sphinx**: Python documentation generation
- **Doxygen**: C++ documentation generation

## Customization

### Extending the Documentation Generator

To add custom documentation:

```cpp
class CustomDocumentationGenerator : public DocumentationGenerator {
public:
    void generateCustomDocumentation() override {
        // Add custom documentation generation logic
    }
};
```

### Template Customization

Customize templates in the `templates/` directory:

```bash
cd docs/tools
vim templates/html/class_doc.html
```

### Content Filtering

Filter generated documentation based on:

- Component type (core, brain, memory, etc.)
- Documentation level (basic, detailed, advanced)
- Audience (beginner, intermediate, expert)

## Troubleshooting

### Common Issues

1. **Missing Source Files**
   - Ensure all header files are in the include path
   - Check file permissions
   - Verify build configuration

2. **Generation Failures**
   - Check for syntax errors in source code
   - Verify tool dependencies are installed
   - Try generating documentation for a single component first

3. **Template Errors**
   - Check Jinja2 template syntax
   - Verify template files exist
   - Try using default templates

4. **Performance Issues**
   - Use the `--parallel` flag for large codebases
   - Limit the number of components analyzed
   - Increase the analysis timeout

### Getting Help

For issues and questions:

- **GitHub Issues**: Report bugs and feature requests
- **Documentation**: Check the user guide and API reference
- **Community**: Join the NLM mailing list and forums
- **Support**: Contact the NLM team for professional support

## License

MIT License. See LICENSE file for details.

## Acknowledgments

The NLM documentation generator is based on:

- **Doxygen**: C++ documentation generation
- **Sphinx**: Documentation generator
- **Jinja2**: Template engine
- **Clang**: C++ parser
- **CMake**: Build system

## Future Improvements

Planned features for future releases:

1. **Real-time Documentation**: Live documentation generation
2. **Interactive Examples**: Executable documentation
3. **API Testing**: Documentation-based API testing
4. **Integration**: Integration with IDEs and editors
5. **Analytics**: Documentation usage analytics

## Build Instructions

```bash
# Clone the repository
./clone.sh

# Build the documentation generator
cmake -B build -S .
make -C build nlm_docs

# Install
make -C build install

# Run documentation generation
./build/docs/nlm_docs --help
```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Follow the coding standards
4. Write comprehensive tests
5. Update documentation
6. Submit a pull request

## References

1. NLM Documentation Standards
2. Google C++ Style Guide
3. Doxygen Documentation Guidelines
4. Markdown Specification
5. HTML5 Specification

## Contact

For questions and support:

- **Email**: support@nlm.org
- **GitHub**: https://github.com/nlm-project/nlm/issues
- **Website**: https://nlm.readthedocs.io

---

*Generated by NLM Documentation Generator v1.0.0*
*Copyright © 2026 NLM Authors*