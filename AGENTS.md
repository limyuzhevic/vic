# NLM Linting and Formatting Configuration

This directory contains linting and formatting configuration files for the NLM project.

## Files in this directory:

### .pre-commit-config.yaml
Pre-commit hooks for linting and formatting checks before commits.

### .ruff.toml
Ruff linting configuration.

### .black.toml
Black code formatting configuration.

### .flake8
Flake8 linting configuration.

### .editorconfig
Editor configuration for consistent formatting across IDEs.

### AGENTS.md
Configuration for Kilo development agents and tools.

## Usage

### Pre-commit hooks

```bash
# Install pre-commit hooks
pre-commit install

# Run all pre-commit hooks
pre-commit run --all-files

# Run specific hooks
pre-commit run ruff --all-files
pre-commit run black --all-files
```

### Manual linting and formatting

```bash
# Format code with black
black python/ src/

# Lint code with ruff
ruff check python/ src/

# Fix automatic issues with ruff
ruff check --fix python/ src/

# Run mypy type checking
mypy python/

# Run flake8 linting
flake8 python/ src/
```

### Configuration references

- **Black**: Code formatter for consistent Python code style
- **Ruff**: Fast linter and formatter written in Rust
- **Flake8**: Linter for Python code style violations
- **EditorConfig**: Editor configuration file format to maintain consistent coding styles

## Integration with CI/CD

These configuration files are integrated into the CI/CD pipeline:

- GitHub Actions: Linting and formatting checks before merge
- GitLab CI: Automated linting and formatting validation
- Pre-commit: Local hooks to ensure code quality before commits

## Customization

You can customize these configurations in the files within this directory:

- `.pre-commit-config.yaml`: Add or remove pre-commit hooks
- `.ruff.toml`: Adjust Ruff linting rules
- `.black.toml`: Customize Black formatting options
- `.editorconfig`: Set editor-specific formatting rules

## Development workflow

The recommended development workflow includes:

1. Make your changes
2. Run formatting: `black python/ src/`
3. Run linting: `ruff check --fix python/ src/`
4. Run type checking: `mypy python/`
5. Run tests
6. Commit your changes

The pre-commit hook will automatically run these checks before committing.

## License

MIT License. See project root for details.