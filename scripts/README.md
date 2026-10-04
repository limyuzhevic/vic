# NLM Linting Scripts README

This directory contains automation scripts for code linting and formatting in the NLM project.

## Overview

The linting scripts provide comprehensive code quality checks for the NLM project, including:

- Code formatting with clang-format
- Static analysis with clang-tidy
- Static analysis with cppcheck
- Build verification
- Automated CI/CD integration

## Scripts

### `run_linting.sh`

**Primary automation script** for running linting and code quality checks.

#### Usage
```bash
# Run quick linting check (default)
./scripts/run_linting.sh check

# Auto-format code
./scripts/run_linting.sh format

# Run full linting suite
./scripts/run_linting.sh full

# Quick feedback mode
./scripts/run_linting.sh quick

# Show help
./scripts/run_linting.sh help

# Show version information
./scripts/run_linting.sh version
```

#### Features

- **Comprehensive logging** with timestamps and progress reporting
- **Automatic dependency checking** and installation guidance
- **Cross-platform compatibility** (Linux, macOS, WSL)
- **Graceful error handling** with detailed error messages
- **Log file generation** for debugging and auditing
- **CMake configuration validation**
- **Build verification**
- **Existing test target execution**

#### Modes

| Mode | Description | Use Case |
|------|-------------|----------|
| `check` | Basic linting checks (formatting only) | Quick validation |
| `format` | Auto-format code | Fixing formatting issues |
| `full` | Complete linting suite | Pre-commit validation |
| `quick` | Minimal checks for feedback | Iterative development |

### `linting.cmake`

CMake script providing linting targets for integration with CMake-based workflows.

#### Targets

- `linting_all` - Run all linting tools (format + static analysis)
- `linting_format` - Check code formatting only
- `linting_tidy` - Run clang-tidy analysis
- `linting_static` - Run cppcheck analysis
- `linting_help` - Display available targets

## Configuration Files

### `.clang-format`

Code formatting configuration based on LLVM coding standards with NLM-specific adaptations.

#### Key Settings
- **Indentation**: 4 spaces, no tabs
- **Line length**: 100 characters
- **Brace style**: Allman (ANSI style for if/else/switch)
- **Include ordering**: Custom categories for NLM headers
- **String handling**: Break string literals for readability

### `.clang-tidy`

Clang-tidy configuration for C++ static analysis.

#### Enabled Checks
- All standard Clang-Tidy checks
- Custom NLM-specific check prefixes
- Configurable check levels for different teams/environments

### `.cppcheck`

Cppcheck configuration for static analysis.

#### Key Settings
- **C++ standard**: C++20
- **Checks enabled**: All standard checks
- **Inconclusive results**: Enabled for research code flexibility
- **Custom defines**: NLM-specific macros

## GitHub Actions Integration

### `.github/workflows/linting.yml`

Automated code quality checks for pull requests and pushes.

#### Workflow Stages

1. **Push to Main/Development**
   - Runs full linting suite
   - Validates code quality before merge

2. **Pull Requests**
   - Runs comprehensive checks
   - Provides detailed feedback
   - Ensures code quality standards

3. **Manual Workflow Dispatch**
   - On-demand linting runs
   - Ad-hoc validation
   - Testing new configurations

#### Jobs

- **`linting`**: Full CI checks (formatting, clang-tidy, cppcheck, build)
- **`formatting`**: Auto-formatting workflow (for fixing issues)

## Contributing

### Development Workflow

1. **Initial Setup**
   ```bash
   ./scripts/run_linting.sh check
   ```

2. **Regular Development**
   ```bash
   ./scripts/run_linting.sh quick    # Quick validation
   ./scripts/run_linting.sh format   # Fix formatting
   ./scripts/run_linting.sh full     # Complete validation
   ```

3. **Code Review**
   - Ensure all linting checks pass
   - Review any new linting warnings
   - Verify CI/CD pipeline passes

### Pre-commit Hooks

For local development, consider using pre-commit hooks:

```yaml
# .pre-commit-config.yaml
repos:
  - repo: local
    hooks:
      - id: nlm-linter
        name: NLM Linter
        entry: scripts/run_linting.sh
        language: script
        files: \.(cpp|hpp)$
        pass_filenames: true
```

## Troubleshooting

### Common Issues

| Issue | Solution |
|-------|----------|
| Linting tools not found | Install dependencies: `sudo apt-get install -y clang-format clang-tidy cppcheck` |
| Code formatting fails | Run auto-format: `./scripts/run_linting.sh format` |
| CI/CD pipeline failing | Run commands locally: `./scripts/run_linting.sh full` |
| Too many warnings | Run formatting first: `./scripts/run_linting.sh format` |
| CMake configuration issues | Rebuild: `rm -rf build && mkdir build && cd build && cmake ..` |

### Error Messages

- **"clang-format not found"**: Install clang-format package
- **"Makefile not found"**: Run CMake configuration
- **"No such file or directory"**: Check script path
- **"Command failed"**: Check logs in output or log file

## Customization

### Local Configuration

Create local configuration files to override defaults:

```bash
# Local .clang-format
cat >> .clang-format << EOF
ColumnLimit: 120
Style: Google
EOF

# Local .clang-tidy
cat >> .clang-tidy << EOF
Checks: "-
    *,\
    -performance-*
    -modernize-*\
"
EOF
```

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `NLM_LINT_VERBOSE` | `false` | Enable verbose logging |
| `NLM_LINT_TIMEOUT` | `300` | Command timeout in seconds |
| `NLM_BUILD_JOBS` | `$(nproc)` | Number of parallel build jobs |

### Team Customization

Create team-specific configuration files:

```bash
# For team-specific overrides
mkdir -p config/linting/
```

## Logging

All scripts generate detailed log files:

### Log File Location

- **Default**: `linting_YYYYmmdd_HHMMSS.log` in project root
- **Environment variable**: `export NLM_LOG_FILE=/path/to/logfile.log`

### Log Content

Each log entry includes:
- Timestamp
- Log level (INFO, WARNING, ERROR, SUCCESS)
- Detailed message
- Command output (when available)

### Viewing Logs

```bash
# View latest log
tail -f linting_*.log

# View specific log
less linting_20240101_120000.log

# Search logs for specific errors
grep -i "ERROR" linting_*.log
```

## Performance Optimization

### Quick Validation

For rapid feedback during development:

```bash
# Only check formatting (fastest)
./scripts/run_linting.sh quick

# Skip expensive checks
./scripts/run_linting.sh check
```

### Full Validation

For complete code quality assurance:

```bash
# Full linting suite (comprehensive but slower)
./scripts/run_linting.sh full

# With increased parallelism
NLM_BUILD_JOBS=8 ./scripts/run_linting.sh full
```

## Integration with Other Tools

### IDE Integration

#### CLion
1. Import project as CMake project
2. Create a run configuration using `scripts/run_linting.sh full`
3. Add to Favorites for quick access

#### VS Code with C/C++ Extension
1. Add `c_cpp_properties.json` for IntelliSense
2. Configure tasks.json for linting:
   ```json
   {
     "label": "NLM Lint",
     "type": "shell",
     "command": "./scripts/run_linting.sh full",
     "problemMatcher": []
   }
   ```

### Docker Integration

Create a Dockerfile for consistent linting environment:

```dockerfile
FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install -y \
    cmake \
    g++ \
    make && \
    apt-get install -y \
    clang-format \
    clang-tidy \
    cppcheck

WORKDIR /app
COPY . /app

CMD ["/bin/bash", "-c", "./scripts/run_linting.sh check"]
```

## Maintenance

### Updating Dependencies

Update linting tools regularly:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y clang-format clang-tidy cppcheck

# macOS with Homebrew
brew update
brew upgrade clang-format clang-tidy cppcheck

# Or use system package managers
# For Windows: choco install clang cppcheck
```

### Configuration Updates

Update configuration files:

```bash
# Backup existing configs
cp .clang-format .clang-format.backup
cp .clang-tidy .clang-tidy.backup
cp .cppcheck .cppcheck.backup

# Test new configurations
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make linting_all
```

### Script Updates

Update the main script when needed:

```bash
# Review script changes
cat scripts/run_linting.sh | grep -n "function\|if\|for"

# Test script syntax
bash -n scripts/run_linting.sh

# Test script help
./scripts/run_linting.sh help
```

## Support

For questions about linting or configuration:

1. **Documentation**: This README and CONTRIBUTING.md
2. **Configuration Files**: `.clang-format`, `.clang-tidy`, `.cppcheck`
3. **Script Help**: `./scripts/run_linting.sh help`
4. **Issues**: Create GitHub Issues for bugs or feature requests
5. **Community**: Check project documentation and community resources

## License

This linting scripts package is part of the NLM project and is covered by the MIT License.

---

*Generated on: $(date '+%Y-%m-%d %H:%M:%S')*
*For more information, see CONTRIBUTING.md*
