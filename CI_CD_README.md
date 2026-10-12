# NLM CI/CD Pipeline Configuration

This repository contains a comprehensive CI/CD pipeline for the NLM (Neural Learning Machine) project, an experimental computational brain project that simulates brain-like systems.

## Overview

The CI/CD pipeline provides:

- **Multi-platform builds** (Linux, macOS, Windows)
- **Comprehensive testing** (unit tests, integration tests, Python bindings)
- **Documentation generation** (Doxygen, Sphinx)
- **Release automation** (version bumping, package creation, releases)
- **Performance benchmarking** (automated, regression detection)
- **Security scanning** (dependency checks, container security)
- **Code quality analysis** (linting, complexity analysis)
- **Pull request validation** (size, complexity, formatting)

## Workflow Files

### 1. `.github/workflows/build.yml`

**Purpose**: Main build workflow that builds NLM for multiple platforms and configurations

**Key Features**:
- Multi-platform support (Linux, macOS, Windows)
- Multiple build configurations (Debug, Release, RelWithDebInfo)
- Cross-compilation testing (Linux → ARM64)
- Both CMake and scikit-build build systems
- Artifact upload for debugging

**When it runs**:
- On push to main/develop branches
- On pull requests
- On workflow dispatch

**Outputs**:
- Build artifacts for each platform and configuration
- Cross-compilation test results
- Package analysis
- Bundle size analysis

### 2. `.github/workflows/test.yml`

**Purpose**: Comprehensive testing pipeline covering unit tests, integration tests, and Python bindings

**Key Features**:
- Unit testing with CTest
- Phase 6 integration testing (complete brain system)
- Python binding testing
- Memory leak detection with Valgrind and sanitizers
- Performance benchmarking
- Test coverage analysis
- Parallel test execution
- Post-validation checks

**Test Matrix**:
- Linux/macOS/Windows
- Debug/Release builds
- Address/Undefined sanitizers (Linux/macOS)

**Outputs**:
- Test results and coverage reports
- Performance benchmarks
- Memory leak analysis
- Parallel test execution results

### 3. `.github/workflows/docs.yml`

**Purpose**: Documentation generation and validation pipeline

**Key Features**:
- Doxygen C++ API documentation
- Sphinx Python API documentation
- Documentation validation and structure checks
- GitHub Pages deployment

**When it runs**:
- On pushes to main/develop branches
- On tags (releases)
- On workflow dispatch

**Outputs**:
- Generated documentation artifacts
- Validation reports
- Deployed documentation on GitHub Pages

### 4. `.github/workflows/release.yml`

**Purpose**: Release automation pipeline

**Key Features**:
- Automatic version bumping
- Package creation (Python wheels, conda packages, source packages)
- Docker image building and pushing
- GitHub release creation
- Documentation deployment
- Changelog generation

**When it runs**:
- On version tags
- On workflow dispatch

**Outputs**:
- Release packages
- GitHub releases
- Docker images
- Updated documentation

### 5. `.github/workflows/benchmark.yml`

**Purpose**: Performance benchmarking and regression detection

**Key Features**:
- Automated performance benchmarking
- Baseline comparison and regression detection
- Historical performance tracking
- Performance trend analysis
- Comprehensive reporting

**Schedule**:
- Daily at 2:00 AM UTC
- On workflow dispatch with custom parameters

**Outputs**:
- Benchmark results
- Regression analysis
- Performance trends
- Comprehensive reports

### 6. `.github/workflows/security.yml`

**Purpose**: Security scanning and vulnerability detection

**Key Features**:
- Dependency security checks (safety, pip-audit)
- Code scanning with CodeQL and Clang Static Analyzer
- Container security analysis with Trivy
- Secrets and sensitive data detection
- License compliance checking
- Comprehensive security reporting

**Schedule**:
- Daily at 4:00 AM UTC
- On push to main/develop branches
- On pull requests

**Outputs**:
- Security vulnerability reports
- Container scan results
- Compliance reports

### 7. `.github/workflows/pr-validation.yml`

**Purpose**: Pull request validation and quality checks

**Key Features**:
- PR title validation (conventional commits)
- PR size and complexity checks
- Code linting validation (ClangFormat, ClangTidy, CPPCheck)
- Code formatting validation
- Dependency validation
- Integration testing
- Docker validation

**When it runs**:
- On pull requests to main branch
- On workflow dispatch

**Outputs**:
- PR validation reports
- Automated comments on PRs
- Quality check results

### 8. `.github/workflows/scan-tools.yml`

**Purpose**: Code quality and complexity analysis

**Key Features**:
- Code quality scanning (ClangTidy, CPPCheck, ClangFormat)
- Documentation quality validation
- Performance profiling and benchmarking
- Code complexity analysis (McCabe, Radon)
- Dependency analysis
- Comprehensive scan reporting

**When it runs**:
- On push to main/develop branches
- On pull requests
- On workflow dispatch

**Outputs**:
- Quality scan reports
- Complexity metrics
- Dependency analysis results
- Scan status reports

## Usage Examples

### Running a specific workflow manually

```bash
# Run build workflow
gh workflow run build.yml

# Run test workflow
gh workflow run test.yml

# Run benchmark with custom parameters
gh workflow run benchmark.yml --field neuron_count 2000 --field steps 10000

# Run security scan
gh workflow run security.yml
```

### Checking workflow status

```bash
# List all workflows
gh workflow list

# View workflow run logs
gh run view --job <job-id>

# Download artifacts from a run
gh run download --name <artifact-name> <run-id>
```

### Local Development

To test the CI/CD setup locally, ensure you have:

1. **GitHub CLI (gh)**: `brew install gh` (macOS) or download from GitHub
2. **Docker** (for container-based workflows)
3. **CMake 3.16+** (for building)
4. **Python 3.11+** (for Python bindings)

To authenticate with GitHub:

```bash
gh auth login
```

### Customizing Workflows

You can customize the workflows by:

1. **Modifying workflow files** in `.github/workflows/`
2. **Adding environment variables** in GitHub repository settings
3. **Setting up secrets** for external services
4. **Adjusting matrix strategies** for different test combinations

## Environment Variables

### Build Environment Variables
- `CXXFLAGS`: Additional C++ compiler flags
- `CMAKE_BUILD_TYPE`: CMake build type (Debug, Release, RelWithDebInfo)
- `SKIP_TESTING`: Skip testing phase (true/false)

### Testing Environment Variables
- `SANITIZER`: Memory sanitizer type (none, address, undefined)

### Security Environment Variables
- `GITHUB_TOKEN`: GitHub authentication token
- `CONDA_TOKEN`: Conda authentication token

### External Services
The pipeline uses several external services:

- **GitHub Actions**: CI/CD platform
- **GitHub Pages**: Documentation hosting
- **Docker Hub**: Container image registry (if configured)
- **Trivy**: Container security scanner
- **CodeQL**: Code analysis platform
- **Bandit**: Python security scanner

## Troubleshooting

### Common Issues

1. **Permission denied**: Ensure GitHub token has appropriate permissions
2. **Build failures**: Check CMake configuration and dependencies
3. **Test failures**: Review test logs and output files
4. **Documentation generation**: Ensure Doxygen and Sphinx are installed

### Debugging Workflows

1. **Enable debug logging**:
   ```bash
   gh run view --job <job-id> -w
   ```

2. **Download artifacts**:
   ```bash
   gh run download --name <artifact-name> <run-id>
   ```

3. **Check workflow logs**:
   - Go to Actions tab in GitHub repository
   - Select the workflow run
   - Review step-by-step execution logs

## Contributing

To contribute to this CI/CD configuration:

1. **Fork the repository**
2. **Create a feature branch**
3. **Modify workflow files** as needed
4. **Test locally** (if possible)
5. **Create a pull request**
6. **Review and merge**

## License

This CI/CD configuration is part of the NLM project and is licensed under the MIT License.

## Contact

For questions about the CI/CD pipeline:
- Open issues in the repository
- Contact the NLM development team
- Check the documentation in the `docs/` directory

---

*This configuration is designed to be robust, comprehensive, fast, reliable, maintainable, secure, and fully automated for the NLM project development workflow.*
