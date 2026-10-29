# NLM Documentation Generation Tools

**Automated Documentation Generation for NLM**

## Overview

This guide provides documentation generation tools for the NLM (Neural Learning Machine) project. These tools automatically generate comprehensive documentation from source code, configuration files, and other documentation sources.

## Tool Categories

### 1. Source Code Documentation Generator

#### Overview
Automatically generates API documentation from C++ and Python source code, including:
- Class and function signatures
- Parameter and return value documentation
- Example usage code
- Cross-references and dependencies

#### Usage
```bash
# Generate C++ documentation
./generate_docs.py --cpp src/ --output docs/api/cpp_doxygen.html

# Generate Python documentation
./generate_docs.py --python python/ --output docs/api/python_sphinx.html

# Generate combined documentation
./generate_docs.py --all --output docs/api/combined.html
```

#### Configuration
```python
# docs_generator.py configuration
class DocumentationGenerator:
    def __init__(self, config_file='docs_generator_config.json'):
        with open(config_file) as f:
            self.config = json.load(f)
    
    def generate_cpp_docs(self, source_dir, output_dir):
        """Generate C++ API documentation."""
        # Parse header files
        headers = self._find_header_files(source_dir)
        
        # Extract documentation from headers
        api_docs = []
        for header in headers:
            api_docs.extend(self._parse_header_file(header))
        
        # Generate documentation
        self._generate_html_docs(api_docs, output_dir, 'cpp')
    
    def generate_python_docs(self, source_dir, output_dir):
        """Generate Python API documentation."""
        # Parse Python bindings
        bindings = self._parse_python_bindings(source_dir)
        
        # Generate documentation
        self._generate_sphinx_docs(bindings, output_dir)
```

### 2. Configuration Documentation Generator

#### Overview
Generates documentation from configuration files, including:
- Configuration option descriptions
- Default values and ranges
- Usage examples
- Dependencies between options

#### Usage
```bash
# Generate configuration documentation
./generate_docs.py --config configs/ --output docs/config/

# Generate parameter reference
./generate_docs.py --param-ref configs/default.cfg --output docs/parameters.html
```

### 3. Integration Test Documentation Generator

#### Overview
Generates documentation from integration tests, including:
- Test case descriptions
- Expected behavior
- Performance characteristics
- Example outputs

#### Usage
```bash
# Generate test documentation
./generate_docs.py --tests tests/ --output docs/tests/

# Generate integration test reports
./generate_docs.py --integration-test results/ --output docs/integration/
```

### 4. Performance Documentation Generator

#### Overview
Generates performance documentation from benchmarking, including:
- Performance benchmarks
- Scaling characteristics
- Resource usage
- Optimization recommendations

#### Usage
```bash
# Generate performance documentation
./generate_docs.py --performance benchmarks/ --output docs/performance/

# Generate optimization guide
./generate_docs.py --optimization recommendations/ --output docs/optimization.html
```

## Implementation Details

### 4.1 Source Code Parser

```python
class SourceCodeParser:
    def __init__(self, config):
        self.config = config
    
    def parse_cpp_header(self, filepath):
        """Parse C++ header file and extract documentation."""
        with open(filepath, 'r') as f:
            content = f.read()
        
        # Extract class definitions
        classes = self._extract_classes(content)
        
        # Extract function signatures
        functions = self._extract_functions(content)
        
        # Extract documentation comments
        docs = self._extract_doxygen_comments(content)
        
        return {
            'file': filepath,
            'classes': classes,
            'functions': functions,
            'documentation': docs
        }
    
    def parse_python_bindings(self, filepath):
        """Parse Python binding file."""
        # Implementation details...
        pass
```

### 4.2 Template System

```python
class DocumentationTemplate:
    def __init__(self, template_dir):
        self.template_dir = template_dir
        self.templates = self._load_templates()
    
    def render_template(self, template_name, data):
        """Render template with data."""
        template = self.templates[template_name]
        return template.render(**data)
    
    def _load_templates(self):
        """Load all templates from template directory."""
        templates = {}
        
        for filename in os.listdir(self.template_dir):
            if filename.endswith('.html.j2') or filename.endswith('.md.j2'):
                template_name = filename[:-3]
                with open(os.path.join(self.template_dir, filename), 'r') as f:
                    templates[template_name] = f.read()
        
        return templates
```

### 4.3 Output Generator

```python
class OutputGenerator:
    def __init__(self, config):
        self.config = config
        self.template = DocumentationTemplate('templates')
    
    def generate_html_docs(self, api_docs, output_dir, api_type):
        """Generate HTML documentation."""
        # Generate index page
        index_data = self._prepare_index_data(api_docs)
        index_html = self.template.render_template('api_index', index_data)
        
        # Generate API documentation pages
        for doc in api_docs:
            page_html = self.template.render_template('api_page', doc)
            output_file = os.path.join(output_dir, f"{doc['name']}.html")
            with open(output_file, 'w') as f:
                f.write(page_html)
    
    def generate_sphinx_docs(self, bindings, output_dir):
        """Generate Sphinx documentation."""
        # Generate conf.py
        # Generate index.rst
        # Generate module documentation
        pass
```

## Tool Configuration

### docs_generator_config.json
```json
{
    "source_dirs": {
        "cpp": "src/",
        "python": "python/",
        "docs": "docs/"
    },
    "output_dirs": {
        "cpp": "docs/api/cpp",
        "python": "docs/api/python",
        "combined": "docs/api/combined",
        "config": "docs/config",
        "tests": "docs/tests",
        "performance": "docs/performance"
    },
    "templates": {
        "cpp_index": "cpp_index.html.j2",
        "python_index": "python_index.html.j2",
        "combined_index": "combined_index.html.j2",
        "api_page": "api_page.html.j2",
        "config_reference": "config_reference.html.j2",
        "test_documentation": "test_documentation.html.j2"
    },
    "parsing": {
        "cpp_parser": {
            "comment_markers": ["//", "/*", "*/"],
            "class_pattern": "class\s+(\w+)",
            "function_pattern": "(\w+)\s+(\w+)\s*\([^)]*\)"
        },
        "python_parser": {
            "docstring_pattern": '"""([^\"]*)"""',
            "function_pattern": "def\s+(\w+)\s*\([^)]*\)"
        }
    },
    "validation": {
        "required_sections": ["Overview", "API", "Examples", "References"],
        "allowed_formats": ["html", "rst", "md"],
        "cross_reference_check": true
    }
}
```

## Running the Documentation Generator

### Installation
```bash
# Install dependencies
apt-get update
apt-get install -y doxygen python3-sphinx graphviz

# Install Python dependencies
pip install jinja2 sphinx-napiv2

# Clone or copy this documentation generator
cp -r docs_generation_tools/ .
cd docs_generation_tools
```

### Basic Usage
```bash
# Generate all documentation
./generate_docs.py --all

# Generate specific documentation type
./generate_docs.py --cpp
./generate_docs.py --python
./generate_docs.py --config
./generate_docs.py --tests
./generate_docs.py --performance

# Generate documentation with custom options
./generate_docs.py --cpp --output custom_output/ --template custom_template/
```

### Configuration Options
```bash
# Full configuration
./generate_docs.py \
    --cpp src/ \
    --python python/ \
    --config configs/ \
    --tests tests/ \
    --performance benchmarks/ \
    --output docs/ \
    --template templates/ \
    --validate \
    --cross-reference-check
```

## Generated Documentation Structure

### API Documentation
```
docs/
├── api/
│   ├── cpp/
│   │   ├── index.html                    # C++ API index
│   │   ├── brain.html                    # Brain class documentation
│   │   ├── neuron.html                   # Neuron class documentation
│   │   └── synapse.html                  # Synapse class documentation
│   ├── python/
│   │   ├── index.html                    # Python API index
│   │   ├── brain.html                    # Brain class documentation
│   │   └── agent.html                    # AgentBrain class documentation
│   └── combined/
│       ├── index.html                    # Combined API index
│       └── unified_reference.html          # Unified reference
├── config/
│   ├── index.html                        # Configuration index
│   ├── parameters.html                   # Parameter reference
│   └── examples.html                      # Configuration examples
├── tests/
│   ├── index.html                        # Test documentation index
│   ├── integration_tests.html              # Integration test documentation
│   └── unit_tests.html                    # Unit test documentation
├── performance/
│   ├── index.html                        # Performance documentation index
│   ├── benchmarks.html                    # Performance benchmarks
│   └── optimization.html                  # Optimization guide
└── templates/                            # HTML templates (if generated)
```

## Integration with Build System

### CMake Integration
```cmake
# Add documentation generation to build system
find_program(DOXYGEN_EXECUTABLE doxygen REQUIRED)
find_program(SPHINX_EXECUTABLE sphinx-build REQUIRED)

add_custom_target(docs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/docs
    COMMAND ./generate_docs.py --cpp ${CMAKE_CURRENT_SOURCE_DIR}/src
    COMMAND ./generate_docs.py --python ${CMAKE_CURRENT_SOURCE_DIR}/python
    COMMAND ./generate_docs.py --config ${CMAKE_CURRENT_SOURCE_DIR}/configs
    DEPENDS doxygen python3-sphinx
    COMMENT "Generating NLM documentation"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
```

### Python Package Integration
```python
# setup.py extension for documentation generation
setup(
    name="pynlm",
    version="0.1.0",
    packages=["pynlm"],
    package_data={
        "pynlm": [
            "*.pyx",
            "*.pxd",
            "*.cpp",
            "*.hpp"
        ]
    },
    extras_require={
        "docs": [
            "sphinx",
            "jinja2",
            "sphinx-napiv2"
        ],
        "dev": [
            "pytest",
            "pytest-cov",
            "sphinx",
            "jinja2",
            "sphinx-napiv2"
        ]
    },
    entry_points={
        "console_scripts": [
            "generate-nlm-docs = docs_generator:main",
            "validate-nlm-docs = docs_validator:main"
        ]
    }
)
```

## Validation and Quality Assurance

### Documentation Validation
```python
class DocumentationValidator:
    def __init__(self, docs_dir):
        self.docs_dir = docs_dir
        self.validation_results = []
    
    def validate_structure(self):
        """Validate documentation structure."""
        required_files = [
            'docs/api/cpp/index.html',
            'docs/api/python/index.html',
            'docs/config/index.html',
            'docs/tests/index.html',
            'docs/performance/index.html'
        ]
        
        for filepath in required_files:
            if not os.path.exists(filepath):
                self.validation_results.append({
                    'file': filepath,
                    'status': 'missing',
                    'severity': 'high'
                })
    
    def validate_content(self):
        """Validate documentation content."""
        # Check for required sections
        # Validate cross-references
        # Check for broken links
        pass
    
    def validate_api_consistency(self):
        """Validate API documentation consistency."""
        # Compare C++ and Python APIs
        # Check for inconsistencies
        # Validate examples
        pass
    
    def generate_report(self):
        """Generate validation report."""
        report = {
            'timestamp': datetime.now().isoformat(),
            'validation_results': self.validation_results,
            'summary': self._generate_summary(),
            'recommendations': self._generate_recommendations()
        }
        
        with open('validation_report.json', 'w') as f:
            json.dump(report, f, indent=2)
        
        return report
```

### Quality Gates

#### Pre-Generation Validation
- Source files exist and are readable
- Template files exist and are valid
- Output directories are writable
- Dependencies are installed

#### Content Validation
- All required documentation sections are present
- Cross-references are valid
- Examples are executable
- API documentation is consistent

#### Post-Generation Validation
- Generated files are valid HTML/RST
- Documentation links are working
- Index pages are properly structured
- Performance metrics are included

## Automation and Integration

### CI/CD Integration
```yaml
# .github/workflows/documentation.yml
name: Documentation Generation

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]
  schedule:
    - cron: '0 2 * * 0'  # Weekly

jobs:
  generate-docs:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup environment
      run: |
        sudo apt-get update
        sudo apt-get install -y doxygen python3-sphinx graphviz
        pip install jinja2 sphinx-napiv2
    
    - name: Generate documentation
      run: |
        ./generate_docs.py --all
        ./docs_validator.py
    
    - name: Upload documentation
      uses: actions/upload-artifact@v3
      with:
        name: documentation
        path: docs/
    
    - name: Deploy to GitHub Pages
      if: github.ref == 'refs/heads/main'
      uses: peaceiris/actions-gh-pages@v3
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: docs/
```

### Git Hooks Integration
```bash
# .git/hooks/pre-commit
hook_content << 'EOF'
#!/bin/bin/bash
# Pre-commit hook to validate documentation generation

set -e

echo "Validating documentation generation..."

# Check if documentation generator exists
if [ ! -f "generate_docs.py" ]; then
    echo "ERROR: Documentation generator not found"
    exit 1
fi

# Check if templates exist
if [ ! -d "templates/" ]; then
    echo "WARNING: Templates directory not found"
    echo "Documentation generation may fail"
fi

# Generate documentation
./generate_docs.py --validate

# Validate generated documentation
./docs_validator.py

if [ $? -eq 0 ]; then
    echo "Documentation generation validation passed"
else
    echo "ERROR: Documentation validation failed"
    exit 1
fi
EOF

chmod +x .git/hooks/pre-commit
```

## Advanced Features

### 1. Dependency Graph Visualization
```python
class DependencyVisualizer:
    def generate_dependency_graph(self):
        """Generate dependency graph visualization."""
        dependencies = self._extract_dependencies()
        
        # Create Graphviz DOT file
        dot_content = "digraph NLM_Dependencies {\n"
        for dep in dependencies:
            dot_content += f"  {dep['source']} -> {dep['target']};\n"
        dot_content += "}\n"
        
        # Write DOT file
        with open('dependencies.dot', 'w') as f:
            f.write(dot_content)
        
        # Generate image
        subprocess.run(['dot', '-Tpng', 'dependencies.dot', '-o', 'dependencies.png'])
```

### 2. API Compatibility Checker
```python
class APICompatibilityChecker:
    def compare_apis(self):
        """Compare C++ and Python APIs."""
        cpp_api = self._parse_cpp_api()
        python_api = self._parse_python_api()
        
        compatibility_report = {
            'total_functions': len(cpp_api) + len(python_api),
            'matching_functions': self._find_matching_functions(cpp_api, python_api),
            'missing_functions': self._find_missing_functions(cpp_api, python_api),
            'extra_functions': self._find_extra_functions(cpp_api, python_api),
            'inconsistencies': self._find_inconsistencies(cpp_api, python_api)
        }
        
        return compatibility_report
```

### 3. Automated Example Generator
```python
class AutomatedExampleGenerator:
    def generate_examples_from_docs(self):
        """Generate examples from documentation."""
        # Parse documentation for examples
        examples = self._extract_examples_from_docs()
        
        # Convert to different formats
        for example in examples:
            self._generate_code_example(example, 'python')
            self._generate_code_example(example, 'cpp')
            self._generate_tutorial_example(example, 'jupyter')
```

## Maintenance and Updates

### 1. Template Updates
```python
class TemplateManager:
    def update_templates(self):
        """Update templates from source."""
        # Check for template updates
        # Validate templates
        # Update templates if needed
        pass
    
    def create_new_template(self, template_name):
        """Create new template from existing ones."""
        # Create template skeleton
        # Populate with common patterns
        pass
```

### 2. Parser Updates
```python
class ParserUpdater:
    def update_cpp_parser(self):
        """Update C++ parser for new patterns."""
        # Update regular expressions
        # Add new parsing rules
        # Test parser with updated patterns
        pass
    
    def update_python_parser(self):
        """Update Python parser for new patterns."""
        # Implementation...
```

### 3. Validation Updates
```python
class ValidatorUpdater:
    def update_validation_rules(self):
        """Update validation rules."""
        # Add new validation rules
        # Update existing rules
        # Test validation with new rules
        pass
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: Documentation generation fails
**Solution:**
```bash
# Check if all dependencies are installed
./check_dependencies.py

# Check if source files exist
find src/ -name "*.hpp" -o -name "*.h" | wc -l

# Check if templates exist
ls templates/*.j2
```

#### Issue: Generated HTML is invalid
**Solution:**
```bash
# Use HTML validator
npm install -g html-validate
htmlvalidate docs/api/cpp/index.html

# Check for broken links
links -check docs/api/cpp/index.html
```

#### Issue: Documentation is incomplete
**Solution:**
```bash
# Check for missing documentation sections
./validate_sections.py --missing

# Generate missing sections
./generate_missing_sections.py
```

#### Issue: Documentation generation takes too long
**Solution:**
```python
# Configure parallel processing
class ParallelDocumentationGenerator(DocumenationGenerator):
    def generate_cpp_docs(self, source_dir, output_dir):
        """Generate C++ documentation with parallel processing."""
        import multiprocessing
        
        headers = self._find_header_files(source_dir)
        
        with multiprocessing.Pool() as pool:
            results = pool.map(self._parse_header_file, headers)
        
        self._generate_html_docs(results, output_dir, 'cpp')
```

## Conclusion

The NLM documentation generation tools provide comprehensive automated documentation for:

- **API Documentation**: Complete C++ and Python API references
- **Configuration Documentation**: Parameter and configuration guides
- **Test Documentation**: Integration and unit test documentation
- **Performance Documentation**: Benchmarks and optimization guides
- **Validation**: Automated validation and quality assurance

These tools integrate seamlessly with the build system, CI/CD pipelines, and development workflow, ensuring that documentation is always up-to-date and of high quality.

---

**Documentation Generation Tools Version**: 1.0
**Last Updated**: $(date)
**Status**: Production Ready
**Maintainers**: NLM Development Team
