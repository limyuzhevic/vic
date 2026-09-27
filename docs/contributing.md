# Contribution Guidelines

## Overview

NLM (熙然) welcomes contributions from the global community of developers, researchers, and enthusiasts. This document outlines the contribution process, coding standards, and guidelines for contributing to the NLM project.

## Getting Started

### Before You Begin

1. **Read the Documentation**: Familiarize yourself with the codebase, architecture, and existing documentation
2. **Set Up Development Environment**: Follow the development setup instructions
3. **Understand the Contribution Process**: Review this guide carefully
4. **Create an Issue (Optional)**: If you have a specific idea, create an issue to discuss it first

### Quick Contribution Checklist

```bash
# 1. Fork the repository
# 2. Create a feature branch
git checkout -b feature/your-name-description

# 3. Make your changes
# 4. Commit and push
git add .
git commit -m "feat: Add new feature"
git push origin feature/your-name-description

# 5. Open a pull request
# 6. Respond to review feedback
```

## Contribution Types

### 1. Code Contributions

#### Bug Fixes
- Identify bugs in existing functionality
- Create reproduction cases
- Implement fixes following coding standards
- Add tests to prevent regression

#### New Features
- Design and implement new functionality
- Update documentation and examples
- Add comprehensive tests
- Ensure backward compatibility

#### Documentation Improvements
- Fix errors and typos
- Add missing information
- Improve formatting and structure
- Update examples and code snippets

#### Performance Optimizations
- Profile and identify bottlenecks
- Implement efficiency improvements
- Add performance benchmarks
- Update documentation

### 2. Research Contributions

#### Algorithm Development
- Implement new neural computation algorithms
- Test against existing implementations
- Provide performance benchmarks
- Document mathematical foundations

#### Model Extensions
- Extend existing brain models
- Add new plasticity rules
- Implement novel memory systems

#### Experimental Studies
- Run comparative experiments
- Analyze performance characteristics
- Document findings and insights

### 3. Community Contributions

#### Code Reviews
- Review pull requests
- Provide constructive feedback
- Suggest improvements
- Approve or request changes

#### Support and Maintenance
- Help new contributors
- Answer questions in issues/discussions
- Maintain project infrastructure
- Organize events and workshops

## Contribution Process

### 1. Issue Management

#### Finding Good First Issues
1. **Look for labels**: `good first issue`, `beginner`, `help wanted`
2. **Check difficulty**: Issues with clear requirements and manageable scope
3. **Consider skills**: Match your expertise with issue requirements

#### Opening an Issue
1. **Use GitHub Issues**: Create issues in the main repository
2. **Provide clear information**:
   - Title: Brief, descriptive title
   - Description: Detailed explanation of the problem
   - Steps to reproduce: Clear reproduction steps
   - Expected behavior: What should happen
   - Actual behavior: What currently happens
   - Reproduction environment: OS, NLM version, configuration
   - Potential solutions: Any ideas for fixing the issue

3. **Follow issue templates**: Use provided issue templates when available

#### Issue Labels

**Standard Labels**:
- `bug`: Actual bugs or unexpected behavior
- `enhancement`: New features or improvements
- `documentation`: Documentation issues
- `question`: Questions about usage or development
- `help wanted`: Community contributions needed
- `good first issue`: Suitable for beginners
- `invalid`: Issue doesn't meet requirements
- `wontfix`: Issues not planned to be addressed

**Project-Specific Labels**:
- `phase-1`, `phase-2`, etc.: Related to specific development phases
- `performance`: Performance-related issues
- `integration`: Integration or compatibility issues
- `testing`: Testing or test-related issues

### 2. Development Workflow

#### Fork and Branch

1. **Fork the Repository**: Create your own copy of the NLM repository
2. **Create Feature Branch**: Work in a dedicated branch for your changes
3. **Keep Branches Small**: Each branch should focus on one specific change
4. **Use Descriptive Names**: `feature/name-of-feature`, `bugfix/name-of-bug`, `docs/name-of-doc`

```bash
# Example workflow
git checkout main
git pull origin main
git checkout -b feature/neuromodulation-integration
# Make changes...
git add .
git commit -m "feat: Add neuromodulation integration"
git push origin feature/neuromodulation-integration
```

#### Coding Standards

**Commit Message Format**
```
<type>[<scope>]: <description>

<body>

<footer>
```

**Commit Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test additions or modifications
- `chore`: Maintenance tasks
- `perf`: Performance improvements

**Commit Message Examples**:
```bash
# Feature commit
git commit -m "feat(neuromodulation): Add dopamine and curiosity integration

- Implement dopamine plasticity modulation
- Add curiosity-driven exploration behavior
- Update brain simulation loop to include neuromodulators

Co-authored-by: OpenHands <openhands@all-hands.dev>"

# Bug fix commit
git commit -m "fix(brain): Memory system integration error

- Fix null pointer in Brain::getWorkingMemory()
- Add proper initialization of memory systems
- Add integration tests for memory functionality

Co-authored-by: OpenHands <openhands@all-hands.dev>"

# Documentation commit
git commit -m "docs(user-manual): Update installation instructions

- Add Docker-based installation method
- Update CMake configuration examples
- Fix typos and formatting

Co-authored-by: OpenHands <openhands@all-hands.dev>"
```

#### Pull Request Process

1. **Push Changes**: Push your feature branch to your fork
2. **Create Pull Request**: Open a pull request against the main branch
3. **Fill PR Template**: Use the provided PR template
4. **Request Reviews**: Ask relevant maintainers to review your changes
5. **Address Feedback**: Make requested changes
6. **Merge**: Once approved, merge the pull request

**Pull Request Checklist**:
- [ ] All tests pass
- [ ] Code follows project standards
- [ ] Documentation is updated
- [ ] Examples are working
- [ ] Performance is not degraded
- [ ] Dependencies are updated (if needed)
- [ ] Changelog is updated (if needed)

### 3. Code Review Process

#### Review Criteria

**Technical Quality**:
- Code follows project coding standards
- Proper error handling and validation
- Efficient algorithms and data structures
- Adequate comments and documentation
- Thread safety where needed

**Integration Quality**:
- Proper integration with existing systems
- Compatibility with different configurations
- Correct behavior in edge cases
- Performance characteristics

**Maintainability**:
- Readable and well-structured code
- Meaningful variable and function names
- Appropriate use of abstractions
- Comprehensive test coverage

**Documentation Quality**:
- Clear API documentation
- Usage examples provided
- Reference material is accurate
- Update notes for breaking changes

#### Review Feedback Categories

**Approval**: ✅ Ready to merge
**Changes Requested**: 🔧 Modifications needed
**Clarification Needed**: ❓ More information required
**Reject**: ❌ Not suitable for merging

#### Rework Process

1. **Address Comments**: Make requested changes
2. **Update Tests**: Add or modify tests as needed
3. **Update Documentation**: Update affected documentation
4. **Request Re-review**: Submit updated pull request
5. **Repeat**: Continue until all feedback addressed

## Project Coding Standards

### 1. C++ Coding Standards

#### Naming Conventions

**Namespaces**: `nlm` for all NLM code
```cpp
namespace nlm {
    // NLM code here
}
```

**Class Names**: PascalCase
```cpp
class NeuralPopulation;
class PlasticityRule;
```

**Method Names**: camelCase
```cpp
void setMembranePotential(float potential);
float getMembranePotential() const;
```

**Variable Names**: snake_case
```cpp
float membrane_potential = 0.0f;
int simulation_steps = 1000;
```

**Constant Names**: UPPER_SNAKE_CASE
```cpp
static constexpr float TIME_CONSTANT = 20.0f;
static constexpr size_t MAX_SPIKE_HISTORY = 100;
```

#### Code Structure

**Header Files**: `.hpp` extension, only declarations
**Source Files**: `.cpp` extension, implementations only
**Include Guards**: `#pragma once` or include guards with `NLM_` prefix

**Example**
```cpp
// File: src/brain/Neuron.hpp
#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Neuron {
public:
    explicit Neuron(NeuronId id);
    
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // ... other methods ...
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

```cpp
// File: src/brain/Neuron.cpp
#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Neuron::Impl {
    // Implementation details
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Constructor implementation
}

// ... other method implementations ...

} // namespace nlm
```

#### Error Handling

**Exception Types**: Use specific exceptions for different error types
```cpp
enum class NLMError {
    CONFIGURATION_ERROR,
    MEMORY_ERROR,
    SIMULATION_ERROR,
    NEURAL_ERROR,
    SYSTEM_ERROR
};

class NLMException : public std::runtime_error {
public:
    explicit NLMException(NLMError error, const std::string& message)
        : std::runtime_error(message), error_(error) {}
    
    NLMError error() const { return error_; }
    
private:
    NLMError error_;
};
```

**Exception Usage**:
- Use specific exceptions for specific error types
- Include error context in exception messages
- Use `throw` only for exceptional conditions
- Catch exceptions at appropriate levels

#### Memory Management

**Smart Pointers**: Prefer modern C++ smart pointers
```cpp
// Good: std::unique_ptr for exclusive ownership
std::unique_ptr<RandomGenerator> rng;

// Good: std::shared_ptr for shared ownership
std::shared_ptr<Config> config;

// Good: std::vector for dynamic arrays
std::vector<std::unique_ptr<NeuralRegion>> regions;
```

**Avoid**: Raw pointers and memory leaks
```cpp
// Bad: Raw pointer without ownership
Neuron* neuron = new Neuron(id);
// Missing deletion
```

### 2. Documentation Standards

**Doxygen Comments**: Complete documentation for all public classes and methods
```cpp
/**
 * @class NeuralPopulation
 * @brief Group of neurons with shared properties
 * 
 * NeuralPopulation represents a collection of neurons that share common
 * characteristics such as type, threshold, and connectivity patterns.
 * Populations are fundamental building blocks of brain regions.
 * 
 * @note Populations maintain separate firing statistics and can be
 *       independently controlled and manipulated.
 * 
 * @see NeuralRegion, Neuron, Synapse
 * 
 * @since Phase 2: Neural population implementation
 * @author NLM Development Team
 * 
 * @warning Populations must be properly initialized before use in simulation.
 */
class NeuralPopulation {
public:
    /**
     * @brief Constructs a neural population with specified identifier
     * 
     * @param id Unique identifier for this population
     * @param type Neuron type for all neurons in this population
     * @param threshold Membrane potential threshold for firing
     * @param leakConductance Leak conductance for all neurons
     * 
     * @throws std::invalid_argument if id is zero or type is invalid
     * 
     * @pre Population identifiers must be unique within a region
     * @post Population is initialized with specified properties
     * 
     * @see NeuronType, getId(), getType()
     */
    NeuralPopulation(PopulationId id, NeuronType type,
                   float threshold, float leakConductance);
    
    // ... other methods ...
};
```

### 3. Testing Standards

**Test Structure**: Use testing framework (Google Test, Catch2, etc.)
```cpp
// tests/test_neural_population.cpp

#include "gtest/gtest.h"
#include "brain/NeuralPopulation.hpp"
#include "brain/NeuralRegion.hpp"

namespace nlm {
namespace tests {

TEST(NeuralPopulationTest, Initialization) {
    NeuralPopulation population(1, NeuronType::EXCITATORY, -55.0f, 0.5f);
    
    EXPECT_EQ(population.getId(), 1);
    EXPECT_EQ(population.getType(), NeuronType::EXCITATORY);
    EXPECT_EQ(population.getNeuronCount(), 0);
}

TEST(NeuralPopulationTest, NeuronAddition) {
    NeuralPopulation population(1, NeuronType::INHIBITORY, -60.0f, 0.8f);
    std::shared_ptr<Neuron> neuron = std::make_shared<Neuron>(NeuronId(2));
    
    population.addNeuron(std::move(neuron));
    
    EXPECT_EQ(population.getNeuronCount(), 1);
}

TEST(NeuralPopulationTest, ActivityUpdate) {
    NeuralPopulation population(1, NeuronType::EXCITATORY, -50.0f, 0.5f);
    
    // Add test neuron
    auto neuron = std::make_shared<Neuron>(NeuronId(1));
    neuron->setMembranePotential(-40.0f); // Above threshold
    population.addNeuron(neuron);
    
    // Update population activity
    population.updateActivity(0.001f);
    
    EXPECT_TRUE(neuron->shouldFire());
}

} // namespace tests
} // namespace nlm
```

### 4. Performance Standards

**Performance Considerations**:
- Profile critical paths
- Optimize memory access patterns
- Use vectorization where appropriate
- Minimize synchronization overhead
- Consider algorithmic complexity

**Benchmarking**:
```bash
# Performance benchmarks
cd build
./benchmark --suite=neural_computation --output=perf_results.json

# Memory profiling
valgrind --tool=massif ./nlm_phase6_demo

# CPU profiling
perf record -g ./nlm_phase6_demo
```

## Community Guidelines

### 1. Code of Conduct

**Be Respectful**:
- Treat all contributors with respect
- Use inclusive language
- Listen to diverse perspectives

**Be Collaborative**:
- Help others when you can
- Share knowledge and resources
- Support inclusive decision-making

**Be Constructive**:
- Focus on code and ideas, not people
- Provide specific, actionable feedback
- Encourage diverse approaches

### 2. Communication Guidelines

**Issue Comments**:
- Keep issues focused and clear
- Provide context for decisions
- Reference related issues when relevant
- Use appropriate formatting for readability

**Pull Request Comments**:
- Provide constructive feedback
- Explain reasoning behind suggestions
- Use GitHub's reaction system for quick responses
- Reference related discussions when helpful

**Online Discussions**:
- Maintain professional tone
- Cite evidence for claims
- Respect differing viewpoints
- Focus on technical merits

## Diversity and Inclusion

### 1. Inclusive Language

**Preferred Terms**:
- Use person-first language: "person with disability"
- Use gender-neutral pronouns: "they/them"
- Use technical terms when appropriate: "neuron", "population"

**Avoid**:
- Gendered terms where not relevant
- Assumptions about identity or background
- Exclusionary language

### 2. Accessible Documentation

**Accessibility Considerations**:
- Provide alternative formats for visual content
- Use semantic HTML for web documentation
- Ensure sufficient color contrast
- Provide text descriptions for diagrams

**Resources**:
- [Web Content Accessibility Guidelines (WCAG)](https://www.w3.org/WAI/WCAG21/intro)
- [Microsoft Writing Style Guide](https://docs.microsoft.com/en-us/style-guide/)
- [IBM Accessibility](https://www.ibm.com/accessible/)

## License and Attribution

### 1. License Compliance

**Project License**: MIT License

**Code Contributions**: Must be licensed under the same license
```cpp
/*
 * NLM - 熙然
 * MIT License
 * 
 * Copyright (c) 2026 NLM Development Team
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
```

**Documentation License**: Same as project license

### 2. Attribution Requirements

**For Code Contributions**:
- Include copyright notice in source files
- Reference NLM and contributors
- Maintain existing license headers

**For Documentation Contributions**:
- Use project license for documentation
- Cite sources for technical information
- Acknowledge contributors

### 3. Third-Party Libraries

**Dependency Management**:
- Use approved dependency management tools
- Check licenses of third-party libraries
- Follow license requirements (attribution, share-alike, etc.)
- Keep dependencies up to date

## Project Governance

### 1. Decision Making

**Technical Decisions**:
- Based on technical merit and project requirements
- Consider performance, scalability, maintainability
- Document decisions in code comments and design documents

**Community Decisions**:
- Open discussion and consensus building
- Use GitHub issues and pull requests for formal proposals
- Respect existing project architecture and standards

### 2. Role Responsibilities

**Maintainers**:
- Code review and merge pull requests
- Resolve conflicts and make final decisions
- Coordinate with contributors
- Ensure project quality and standards

**Contributors**:
- Write and maintain code
- Add tests and documentation
- Follow coding standards
- Participate in code reviews

**Community Members**:
- Provide feedback and suggestions
- Help new contributors
- Participate in discussions
- Support project activities

## Getting Help

### 1. For Technical Questions

**Where to Ask**:
- GitHub Discussions
- Stack Overflow (tagged `nlm`)
- Project Slack/ Discord channel
- Technical forums

**What to Include**:
- Clear problem description
- Code snippets and error messages
- Environment information (OS, NLM version)
- Steps to reproduce the issue
- Expected vs. actual behavior

### 2. For Contribution Questions

**Where to Ask**:
- GitHub Issues with `question` label
- Project documentation
- Contributor guidelines
- Project maintainers

**What to Ask**:
- Specific questions about contribution process
- Code style and standards questions
- Development environment setup
- Testing and validation requirements

## Contact Information

### 1. Project Maintainers

**GitHub**: https://github.com/nlm/nlm
**Issues**: https://github.com/nlm/nlm/issues
**Discussions**: https://github.com/nlm/nlm/discussions
**Web**: https://nlm.ai

### 2. Technical Support

**For bugs and issues**:
- Report issues with complete information
- Include reproduction steps
- Provide environment details

**For documentation questions**:
- Check existing documentation first
- Look for examples and tutorials
- Search for similar questions

### 3. Community Resources

**Online Communities**:
- GitHub Discussions
- Project forums and mailing lists
- Social media channels
- Academic conferences and workshops

**Community Events**:
- Virtual hackathons
- Code sprints
- Documentation workshops
- Technical tutorials

## Acknowledgements

### 1. Contributors

We thank all contributors who have helped build and maintain the NLM project:

- **Founding Contributors**: Original developers and researchers
- **Code Contributors**: Individuals who have submitted pull requests
- **Documentation Contributors**: Those who have improved documentation
- **Community Members**: Everyone who has provided feedback and support
- **Test Contributors**: Those who have added tests and test cases

### 2. Open Source Projects

NLM depends on many open source projects:

**Core Libraries**:
- Google Test (testing framework)
- CMake (build system)
- Boost (C++ libraries)
- External dependencies as needed

**Tools and Services**:
- GitHub (repository hosting)
- GitHub Actions (CI/CD)
- Docker (containerization)
- Various open source tools

### 3. Research Community

**Scientific Influences**:
- Computational neuroscience research
- Neural network literature
- Cognitive science findings
- Biological brain research

**Academic Partnerships**:
- Research institutions
- University collaborations
- Government research labs
- Industry research partnerships

## Future Directions

### 1. Project Growth

**Planned Features**:
- Enhanced neuromodulation systems
- Advanced memory consolidation
- Multi-agent architectures
- Real-time visualization tools

**Research Areas**:
- Neural diversity and heterogeneity
- Developmental learning mechanisms
- Embodied cognition
- Neuromorphic hardware integration

### 2. Community Expansion

**Growth Goals**:
- Increase contributor diversity
- Expand geographic reach
- Broaden technical expertise
- Enhance youth engagement

**Initiatives**:
- Outreach programs
- Mentorship programs
- Educational partnerships
- Industry collaborations

### 3. Long-term Vision

**Project Mission**:
- Advance understanding of neural computation
- Enable AI through biologically inspired approaches
- Contribute to cognitive science
- Support neuroscience research

**Impact Goals**:
- Transform artificial intelligence
- Advance computational neuroscience
- Improve human health and wellbeing
- Create accessible educational tools

## Conclusion

The NLM project thrives on the contributions of its global community. By following these contribution guidelines, you help ensure that the project maintains high quality, fosters innovation, and supports the growth of the broader research community.

**Key Principles for Success**:

1. **Respect**: Treat all contributors and their work with respect
2. **Collaboration**: Work together to achieve shared goals
3. **Quality**: Strive for excellence in all contributions
4. **Inclusion**: Welcome contributions from diverse backgrounds and perspectives
5. **Continuous Improvement**: Learn from experience and refine processes

Together, we can build a powerful, innovative, and inclusive computational neuroscience project that advances our understanding of the brain and creates new possibilities for artificial intelligence.

**Ready to contribute?**
- Fork the repository
- Create a feature branch
- Make your changes
- Submit a pull request
- Join our community!

The NLM development team and community welcome your contributions and look forward to working with you to create the next generation of neural simulation technology.

## References

### Contribution Best Practices

1. **Open Source Guide** - https://opensource.guide/
2. **GitHub Documentation** - https://docs.github.com/
3. **Doxygen Documentation** - http://www.doxygen.org/
4. **Google Test Documentation** - https://github.com/google/googletest/

### Diversity and Inclusion

1. **Diversity in Open Source** - https://diversityopensource.org/
2. **Women Who Code** - https://www.womenwhocode.com/
3. **Black Girls Who Code** - https://www.blackgirlswhocode.com/

### Open Source Licenses

1. **MIT License** - https://opensource.org/licenses/MIT
2. **Apache License 2.0** - https://opensource.org/licenses/Apache-2.0
3. **GNU General Public License** - https://www.gnu.org/licenses/gpl.html

## License

Contribution guidelines are licensed under the MIT License.

## Copyright

© 2026 NLM Development Team. All rights reserved.