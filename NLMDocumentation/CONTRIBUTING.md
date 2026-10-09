# Documentation Build Configuration

This directory contains configuration files for building and serving the NLM documentation.

## Overview

The NLM documentation system supports multiple formats and build systems:

1. **Markdown to HTML**: Using MkDocs for web documentation
2. **PDF Generation**: Using LaTeX or MkPDF for printable documentation
3. **API Documentation**: Auto-generated from source code using Sphinx
4. **Interactive Examples**: Jupyter notebooks and code playground
5. **Live Preview**: Local server for development

## Directory Structure

```
NLMDocumentation/
├── docs/                    # Markdown source files
├── mkdocs.yml              # MkDocs configuration
├── docs-requirements.txt   # Python dependencies for documentation
├── Makefile                # Build automation
├── .readthedocs.yml       # ReadTheDocs configuration
├── agent/                 # Kilo agent scripts
└── tools/                 # Build and deployment tools
```

## Configuration Files

### mkdocs.yml

MkDocs is used as the primary documentation build system. This configuration provides:

- **Theme and styling**: Modern, responsive documentation theme
- **Navigation structure**: Table of contents and navigation menus
- **Search functionality**: Full-text search for documentation
- **Cross-references**: Internal linking between documentation pages
- **API documentation integration**: Auto-generated API references
- **Live reloading**: Real-time updates during development

### docs-requirements.txt

Python dependencies for building documentation:

```txt
mkdocs>=1.5.0
mkdocs-material>=8.0.0
mkdocs-section-index>=0.3.0
mkdocs-swagger-ui-tag>=0.1.0
pymdown-extensions>=9.0.0
mkdocs-markdownextradata-tag>=0.1.0
markdown-extensions>=3.0.0
beautifulsoup4>=4.12.0
lxml>=4.9.0
pillow>=10.0.0
jinja2>=3.0.0
python-dateutil>=2.9.0
```

### Makefile

Build automation for documentation:

```makefile
# Documentation targets
.PHONY: help build serve build-pdf deploy clean

help:
	@echo "Available targets:"
	@echo "  build   - Build documentation to HTML"
	@echo "  serve   - Serve documentation locally"
	@echo "  build-pdf - Build PDF documentation"
	@echo "  deploy  - Deploy to ReadTheDocs"
	@echo "  clean   - Clean build artifacts"

build:
	mkdocs build --clean

build-pdf:
	mkdocs build --pdf

build-all:
	mkdocs build --with-pdf

serve:
	mkdocs serve

serve-with-pdf:
	mkdocs serve --dev-addr=0.0.0.0:8000

deploy:
	mkdocs gh-deploy

clean:
	rm -rf site/
	rm -rf *.pdf
	rm -rf *.html
	rm -rf *.md.html
	rm -rf docs/api-reference/
```

### .readthedocs.yml

ReadTheDocs configuration for online documentation hosting:

```yaml
name: NLM Documentation
provider: github
repository: nlm-project/nlm

themes:
  - material
  - readthedocs

build:
  image: latest

python:
  version: 3.8
  install:
    - requirements: docs-requirements.txt
    - method: pip
      path: .

extra_requirements:
  - docs

formats:
  - htmlzip
  - pdf
  - epub

single_version: false

use_system_pypi: false

python_install_from_bdist_wheel: true

# Configuration for automatic builds
on:
  # Build documentation on commits
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]
  # Build on tags
  tags:
    include: [v*]

# Configuration for deployment
 deploy:
   - provider: netlify
     config:
       build_command: "mkdocs build"
       publish_dir: "site"
     branches:
       - main
   - provider: amazon
     config:
       bucket_name: "nlm.readthedocs.io"
       region: "us-east-1"
     branches:
       - main
```

## Documentation Build Process

### Phase 1: Development Setup

1. **Install dependencies**
   ```bash
   pip install -r docs-requirements.txt
   ```

2. **Initialize MkDocs**
   ```bash
   mkdocs new NLMDocumentation
   ```

3. **Configure MkDocs**
   Edit `mkdocs.yml` with project-specific settings

### Phase 2: Content Creation

1. **Create documentation structure**
   ```bash
   mkdir -p NLMDocumentation/docs/getting-started
   mkdir -p NLMDocumentation/docs/configuration
   mkdir -p NLMDocumentation/docs/python-api
   mkdir -p NLMDocumentation/docs/tutorials
   mkdir -p NLMDocumentation/docs/architecture
   mkdir -p NLMDocumentation/docs/performance
   mkdir -p NLMDocumentation/docs/examples
   mkdir -p NLMDocumentation/docs/troubleshooting
   ```

2. **Create main documentation files**
   - `index.md`: Home page
   - `getting-started/index.md`: Getting started guide
   - `configuration/index.md`: Configuration reference
   - `python-api/index.md`: Python API reference
   - `tutorials/index.md`: Tutorial series
   - `architecture/index.md`: Architecture documentation
   - `performance/index.md`: Performance guide
   - `examples/index.md`: Code examples
   - `troubleshooting/index.md`: Troubleshooting guide

3. **Add content to individual pages**
   - Use Markdown syntax for documentation
   - Include code examples in fenced code blocks
   - Use images and diagrams where appropriate
   - Add cross-references between pages

### Phase 3: Building and Validation

1. **Build documentation locally**
   ```bash
   cd NLMDocumentation
   mkdocs build --clean
   ```

2. **Validate content**
   ```bash
   # Check for broken links
   mkdocs build --strict
   
   # Validate Markdown syntax
   python -m markdown --lint docs/
   
   # Check for spelling errors
   aspell -c docs/**/*.md
   ```

3. **Test search functionality**
   ```bash
   # Start local server
   mkdocs serve &
   
   # Test search using curl
   curl -X POST "http://localhost:8000/search" -H "Content-Type: application/json" -d '{"query": "brain"}'
   
   # Stop server
   pkill -f "mkdocs serve"
   ```

### Phase 4: Deployment and Distribution

1. **Deploy to ReadTheDocs**
   ```bash
   # Configure ReadTheDocs
   mkdocs gh-deploy
   ```

2. **Publish to GitHub Pages**
   ```bash
   # Configure GitHub Pages
   mkdocs gh-deploy --force
   ```

3. **Create PDF documentation**
   ```bash
   # Build PDF
   mkdocs build --pdf
   
   # Upload to server
   aws s3 sync site/ s3://nlm.readthedocs.io/pdf/
   ```

## Documentation Content Guidelines

### Markdown Style

1. **Headers**: Use level 1 (#) for main titles, level 2 (##) for sections
2. **Lists**: Use unordered lists for bullet points, ordered lists for steps
3. **Code blocks**: Use fenced code blocks with appropriate language specification
4. **Tables**: Use Markdown tables for comparisons and data
5. **Links**: Use relative paths for internal links, absolute URLs for external links
6. **Images**: Use Markdown image syntax with proper alt text

### Example Markdown Structure

```markdown
# Tutorial 1: Silent Brain

## Overview

This tutorial shows you how to create a basic NLM brain simulation.

## Step 1: Create Your First Brain

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

## Step 2: Run the Simulation

```python
for step in range(100):
    brain.step(step)
```

## Common Issues

| Problem | Solution |
|---------|----------|
| Brain doesn't fire | Check threshold values |
| Memory issues | Reduce brain size |
| Configuration errors | Validate parameters |

## Key Concepts

- **Neurons**: Basic processing units
- **Synapses**: Connections between neurons
- **Learning**: Plasticity mechanisms
```

### Documentation Best Practices

1. **Accuracy**: Ensure all content is technically correct
2. **Completeness**: Include necessary details without overwhelming readers
3. **Clarity**: Use simple language and clear explanations
4. **Examples**: Provide practical, working code examples
5. **References**: Link to related topics and additional resources
6. **Maintenance**: Keep documentation up-to-date with code changes

## Advanced Documentation Features

### MkDocs Plugins

The documentation system includes several MkDocs plugins for enhanced functionality:

1. **mkdocs-section-index**: Automatic section indexing
2. **mkdocs-swagger-ui-tag**: API documentation integration
3. **pymdown-extensions**: Advanced Markdown extensions
4. **mkdocs-markdownextradata-tag**: Extra metadata support

### Custom Themes

Custom themes can be created for brand consistency:

```yaml
theme:
  name: custom
  static_templates:
    - 404.html
    - offline.html
  palette:
    primary: indigo
    accent: indigo
  features:
    - navigation.instant
    - navigation.tabs
    - navigation.sections
    - navigation.expand
```

### API Documentation

API documentation is auto-generated from source code comments:

```python
"""
Brain class for neural simulation

This class represents a neural brain with neurons and synapses.

Attributes:
    neuron_count (int): Number of neurons in the brain
    synapse_count (int): Number of synapses in the brain
    firing_rate (float): Average firing rate in Hz

Methods:
    initialize(): Initialize the brain with configuration
    step(step_number): Run a simulation step
    get_total_neuron_count(): Get total neuron count
    get_total_synapse_count(): Get total synapse count
    get_average_firing_rate(): Get average firing rate
"""
```

### Interactive Examples

Interactive examples can be created using Jupyter notebooks:

```bash
# Convert Jupyter notebooks to documentation
jupyter nbconvert --to markdown *.ipynb

# Include in documentation
```markdown
{{ % embed notebook "examples/tutorial1.ipynb" % }}
```
```

## Documentation Maintenance

### Regular Updates

1. **Review and update documentation quarterly**
2. **Check for broken links and outdated content**
3. **Update examples based on new features**
4. **Verify API references against current version**

### Version Control

```bash
# Track documentation changes
git add NLMDocumentation/
git commit -m "Update documentation - [date]"

# Review documentation changes
git log --oneline -- NLMDocumentation/
git diff HEAD~1 NLMDocumentation/
```

## Performance Considerations

### Build Performance

1. **Incremental builds**: Use `mkdocs build --watch` for development
2. **Parallel builds**: Enable parallel processing for large documentation sets
3. **Caching**: Configure browser caching for production
4. **CDN integration**: Use content delivery networks for faster distribution

### Storage Optimization

1. **Compressed archives**: Store documentation in compressed format
2. **Incremental updates**: Only rebuild changed pages
3. **Version control**: Use git for documentation history
4. **External hosting**: Use external services for large documentation sets

## Security Considerations

### Content Security

1. **Input validation**: Validate all user input in documentation build scripts
2. **Output encoding**: Encode all generated HTML properly
3. **Access control**: Restrict access to sensitive documentation
4. **Audit trails**: Log all documentation changes and deployments

### Deployment Security

1. **HTTPS**: Serve documentation over HTTPS
2. **Authentication**: Implement authentication for sensitive content
3. **Authorization**: Control access based on user roles
4. **Monitoring**: Monitor documentation usage and access patterns

## Conclusion

The NLM documentation system provides a comprehensive foundation for creating, building, and distributing high-quality documentation. By following the guidelines and best practices outlined in this configuration, users can:

1. **Create structured, maintainable documentation** using Markdown
2. **Build documentation efficiently** with MkDocs and its plugins
3. **Deploy to multiple platforms** including ReadTheDocs, GitHub Pages, and custom servers
4. **Maintain documentation quality** through automated validation and testing
5. **Ensure security** through proper content handling and access control

The documentation build system is designed to support:

- **Collaborative documentation creation** by multiple authors
- **Automated documentation generation** from source code
- **Flexible output formats** for different use cases
- **Scalable documentation infrastructure** for growing projects
- **Integration with development workflows** for continuous documentation updates

By implementing this comprehensive documentation system, NLM ensures that users have access to high-quality, up-to-date documentation that supports their needs throughout the project lifecycle.

---

*Documentation Build Configuration Version*: 2.0
*Last Updated*: 2026-10-09
*Build Status*: Production Ready

**Key Files**:
- [mkdocs.yml](NLMDocumentation/mkdocs.yml) - MkDocs configuration
- [docs-requirements.txt](NLMDocumentation/docs-requirements.txt) - Documentation dependencies
- [Makefile](NLMDocumentation/Makefile) - Build automation
- [.readthedocs.yml](NLMDocumentation/.readthedocs.yml) - ReadTheDocs configuration

**Build Commands**:
- `make help` - Show available targets
- `make build` - Build documentation to HTML
- `make serve` - Serve documentation locally
- `make build-pdf` - Build PDF documentation
- `make deploy` - Deploy to ReadTheDocs

**Development Commands**:
- `mkdocs serve` - Start local development server
- `mkdocs build --strict` - Build with strict validation
- `mkdocs gh-deploy` - Deploy to GitHub Pages

**Continuous Integration**:
- GitHub Actions automatically build documentation on commits
- Automated validation and testing
- Continuous integration with deployment

**Additional Resources**:
- [MkDocs Documentation](https://www.mkdocs.org/) - Official MkDocs documentation
- [MkDocs Material](https://squidfunk.github.io/mkdocs-material/) - Material theme documentation
- [ReadTheDocs](https://readthedocs.org/) - Online documentation hosting
- [Markdown Extensions](https://Python-Markdown.github.io/extensions/) - Markdown extensions documentation