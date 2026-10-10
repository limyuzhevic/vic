# NLM Documentation
# Neural Learning Machine (熙然) - Complete Documentation

**Version**: 0.1.0
**Date**: October 10, 2026

## Overview

This documentation covers the complete NLM (Neural Learning Machine) system, including:

1. **User Guide** - How to use NLM
2. **API Reference** - Complete API documentation
3. **Development Guide** - How to extend NLM
4. **Configuration Management** - Configuration options
5. **Examples and Tutorials** - Practical examples
6. **Troubleshooting** - Common issues and solutions

## Documentation Structure

### User Documentation (`docs/`)

- **NLMUserGuide.md** - Complete user guide with examples
- **NLMApiReference.md** - API documentation (this file)
- **NLMCommandLine.md** - Command-line interface documentation
- **NLMConfiguration.md** - Configuration management guide
- **NLMTroubleshooting.md** - Troubleshooting and debugging

### Source Code Documentation

The following headers include built-in documentation:

- `include/nlm/brain/Brain.hpp` - Main brain class documentation
- `include/nlm/cli/NLMCLI.hpp` - Command-line interface documentation
- `include/nlm/core/Config/Config.hpp` - Configuration system documentation
- `include/nlm/core/Logger/Logger.hpp` - Logging system documentation

### Generated Documentation

Consider generating additional documentation using tools like:

- **Doxygen** - C++ API documentation
- **Sphinx** - Python-based documentation
- **MkDocs** - Static site documentation

### Build Documentation

**Build documentation from source:**

```bash
# Using Doxygen (if configured)
doxygen Doxyfile

# Using Sphinx (for Python parts)
sphinx-build docs/ docs/_build/html/

# Using MkDocs
mkdocs build
```

## Quick Navigation

### For Users

1. **Start Here**: Read `docs/NLMUserGuide.md`
2. **Command Line**: Read `docs/NLMCommandLine.md`
3. **Configuration**: Read `docs/NLMConfiguration.md`
4. **Troubleshoot**: Read `docs/NLMTroubleshooting.md`

### For Developers

1. **API Reference**: Read `docs/NLMApiReference.md`
2. **Development Guide**: Consult source code headers
3. **Examples**: Look at example programs in `examples/`
4. **Testing**: Run `test_runner` for unit tests

### For Researchers

1. **Scientific Background**: Review `docs/NLMUserGuide.md` Section 8
2. **Architecture**: Study `docs/NLMApiReference.md` for system components
3. **Experiments**: Use `Phase6IntegratedExperiment` for validation
4. **Extensions**: Explore extension points in the API

## Documentation Build Process

### Manual Updates

1. **Update source headers**: Modify Doxygen comments in source files
2. **Update markdown files**: Edit documentation in `docs/`
3. **Verify consistency**: Ensure documentation matches implementation

### Automated Generation

1. **Extract from source**: Use tools to extract documentation from source code
2. **Generate HTML**: Convert markdown to HTML for web viewing
3. **Cross-reference**: Link related documentation sections
4. **Update tables of contents**: Automatically update navigation

## Code Comments Policy

### Doxygen Comments

All public classes, methods, and functions should have Doxygen-style comments:

```cpp
/**
 * Brief description of the class or method.
 *
 * Detailed description can go here. Include parameters, return values,
 * exceptions, and any important implementation details.
 *
 * @param parameter_name Description of parameter.
 * @return Description of return value.
 * @throws ExceptionType Description when exception is thrown.
 * @see Related method or class.
 * @note Important notes or caveats.
 * @warning Potential issues or limitations.
 */
class MyClass {
public:
    /**
     * Constructor with parameter.
     *
     * @param value Initialization value.
     */
    MyClass(int value);
    
    /**
     * Performs some operation.
     *
     * @param input Input data.
     * @return Result of operation.
     */
    int doOperation(const std::string& input);
};
```

### Markdown Comments

Documentation in markdown files should follow standard Markdown conventions:

```markdown
# Section Title

## Subsection Title

Description text with **bold**, *italic*, and `code` elements.

- List item 1
- List item 2
    - Nested list item

1. Ordered list item 1
2. Ordered list item 2

```code
Code block with syntax highlighting
```

| Column 1 | Column 2 |
|----------|----------|
| Cell 1   | Cell 2   |
| Cell 3   | Cell 4   |
```

[Link text](url)
> Block quote text
```

### Documentation Quality Standards

1. **Completeness**: Cover all parameters, return values, and exceptions
2. **Accuracy**: Ensure documentation matches implementation
3. **Clarity**: Use clear, concise language
4. **Examples**: Include relevant code examples
5. **Cross-references**: Link related concepts and documentation
6. **Updates**: Keep documentation in sync with code changes

## Documentation Version Control

### Semantic Versioning

- **Major version**: Breaking changes to API or functionality
- **Minor version**: New features or enhancements
- **Patch version**: Bug fixes and documentation updates

### Documentation Updates

Track documentation changes alongside code changes:

```bash
# Update documentation with code changes
git add docs/ source_headers/

# Document important changes
git commit -m "Update API documentation for new features"
```

## Documentation Deployment

### Web Publishing

1. **GitHub Pages**: Deploy to `docs/` branch
2. **ReadTheDocs**: Host documentation on readthedocs.io
3. **GitLab Pages**: Use GitLab's built-in documentation hosting

### Local Viewing

```bash
# Build HTML documentation
doxygen Doxyfile
mkdocs build

# View in browser
xdg-open docs/_build/html/index.html
```

## Documentation Testing

### Cross-Reference Testing

Ensure all documented references are valid:

```bash
# Check for broken links
grep -r "\[.*\](\|#" docs/ --include="*.md" | grep -v "http"

# Verify Doxygen output
doxygen Doxyfile && doxygen -u Doxyfile
```

### Example Testing

Run example programs to verify documentation:

```bash
# Compile and run examples
make examples
./example_program
```

## Documentation Maintenance

### Regular Updates

1. **Review API changes**: Update documentation after major releases
2. **Fix errors**: Correct documentation errors promptly
3. **Add examples**: Include new examples for new features
4. **Update tutorials**: Update existing tutorials with new workflows

### Community Contributions

1. **Pull requests**: Accept documentation contributions
2. **Code reviews**: Review documentation changes
3. **Peer review**: Ensure technical accuracy
4. **User feedback**: Incorporate user feedback

## Future Documentation Improvements

### Enhanced Features

1. **Interactive examples**: Create runnable code examples
2. **API documentation**: Generate from source code with detailed examples
3. **Tutorials**: Create step-by-step tutorials for common tasks
4. **Reference guide**: Create comprehensive API reference

### Technical Documentation

1. **Architecture diagrams**: Visualize system components
2. **Performance guides**: Document performance optimization
3. **Debugging guides**: Document troubleshooting procedures
4. **Extension guides**: Document how to extend NLM

## Contact and Support

### Documentation Issues

Report documentation issues through the project issue tracker.

### Technical Support

For technical questions or support:

1. Check existing documentation
2. Search issue tracker for similar problems
3. Create new issue if problem is not documented
4. Provide detailed reproduction steps

### Documentation Improvements

Contribute to documentation by:

1. Submitting pull requests with documentation updates
2. Reporting errors and suggesting improvements
3. Providing examples and use cases
4. Reviewing documentation for accuracy and completeness

## Legal and Copyright

### Copyright Notice

```text
Copyright (c) 2026 NLM Research Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### License

This documentation is provided under the same terms as the NLM software:
MIT License.

## Version History

### Version 0.1.0
- **Initial release**
- Complete user guide
- API reference documentation
- Command-line interface documentation
- Configuration management guide
- Troubleshooting section

### Version 0.2.0
- **Planned enhancements**
- Interactive examples
- Extended tutorials
- API reference generator
- Architecture documentation

---

*Documentation generated automatically from source code and maintained by the NLM development team.*
