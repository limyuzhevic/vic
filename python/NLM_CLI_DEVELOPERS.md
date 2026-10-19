# NLM CLI Dependencies

## Required Dependencies

The NLM CLI requires the following Python packages:

```toml
[tool.poetry.dependencies]
python = "^3.8"
numpy = "^1.20"
pandas = "^1.3.0"
pyyaml = "^6.0"
requests = "^2.25.0"
tqdm = "^4.62.0"
rich = "^10.0.0"
click = "^8.0.0"
typer = "^0.7.0"
```

## Optional Dependencies

The NLM CLI also supports optional dependencies for enhanced functionality:

```toml
[tool.poetry.dependencies.nlm-enhanced]
path = "../nlm_enhanced"

[tool.poetry.dependencies.nlm-advanced]
path = "../nlm_advanced"

# Visualization
matplotlib = { version = "^3.5.0", optional = true }
plotly = { version = "^5.10.0", optional = true }
seaborn = { version = "^0.11.0", optional = true }

# Database and storage
psycopg2-binary = { version = "^2.9.0", optional = true }
sqlalchemy = { version = "^1.4.0", optional = true }
h5py = { version = "^3.7.0", optional = true }

# Machine learning
 scikit-learn = { version = "^1.0.0", optional = true }

# Cloud and distributed computing
boto3 = { version = "^1.26.0", optional = true }
redis = { version = "^4.3.0", optional = true }
```

## Development Dependencies

For development and testing:

```toml
[tool.poetry.group.dev.dependencies]
pytest = "^7.0.0"
pytest-asyncio = "^0.19.0"
pytest-cov = "^3.0.0"
mypy = "^0.991"
black = "^22.0.0"
flake8 = "^4.0.0"
ruff = "^0.1.0"
sphinx = "^4.5.0"
sphinx-rtd-theme = "^1.0.0"
pre-commit = "^2.20.0"
```

## Installation

### Basic Installation

```bash
# From source
pip install -e .

# Or with poetry
poetry install
```

### With Optional Dependencies

```bash
# Install with all optional dependencies
pip install -e .[visualization,database,machine-learning,cloud]

# Or with poetry
poetry install --with visualization,database,machine-learning,cloud
```

### Development Installation

```bash
# Install with development dependencies
pip install -e .[dev]

# Or with poetry
poetry install --group dev
```

## Environment Setup

### Environment Variables

The NLM CLI uses the following environment variables:

```bash
# Logging
NLM_LOG_LEVEL=INFO
NLM_LOG_FILE=/var/log/nlm/nlm.log

# Configuration
NLM_CONFIG_DIR=/etc/nlm/config
NLM_CACHE_DIR=/tmp/nlm/cache

# Batch processing
NLM_MAX_WORKERS=4
NLM_QUEUE_SIZE=1000
NLM_TIMEOUT=3600

# Results
NLM_RESULTS_DIR=/var/lib/nlm/results
NLM_BACKUP_DIR=/var/backups/nlm

# Enhanced components
NLM_ENHANCED_PATH=/path/to/enhanced/bindings
NLM_ADVANCED_PATH=/path/to/advanced/utilities
```

### Configuration Files

Configuration files should be placed in the following directories:

```bash
# System-wide configuration
/etc/nlm/config/

# User configuration
~/.config/nlm/

# Project configuration
./nlm_config/

# Workflow definitions
./workflows/
```

### Workspace Organization

Recommended workspace structure:

```bash
workspace/
├── nlm_cli/                    # NLM CLI installation
│   ├── config/                 # Configuration files
│   ├── workflows/              # Workflow definitions
│   ├── experiments/            # Experiment configurations
│   ├── results/                # Results storage
│   ├── logs/                   # Log files
│   └── plugins/                # Custom plugins
├── scripts/                     # CLI scripts
│   ├── nlm_cli.py              # Main CLI
│   ├── nlm_cli_examples.py      # Usage examples
│   └── nlm_cli_utils.py         # Utility functions
├── tests/                      # Test suite
│   ├── test_cli.py             # CLI tests
│   ├── test_batch.py           # Batch tests
│   ├── test_config.py          # Config tests
│   └── test_workflow.py        # Workflow tests
└── docs/                       # Documentation
    ├── README.md               # CLI documentation
    ├── CONFIGURATION.md        # Configuration guide
    ├── COMMANDS.md             # Command reference
    └── API.md                  # API documentation
```

## Advanced Installation

### Building from Source

For advanced users who want to build the NLM CLI from source:

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the enhanced bindings
mkdir build
cd build
cmake ..
make -j4

# Build the NLM CLI
pip install -e ./python

# Verify installation
nlm --version
```

### Docker Installation

The NLM CLI is available as a Docker image:

```bash
# Pull the image
docker pull nlm/nlm-cli:latest

# Run the CLI
docker run -it --rm nlm/nlm-cli:latest --help

# Run with custom configuration
docker run -it --rm -v $(pwd)/config:/config -v $(pwd)/results:/results \
  nlm/nlm-cli:latest batch run my_sweep --config /config/sweep.json
```

### Kubernetes Integration

For Kubernetes deployment:

```yaml
apiVersion: batch/v1
kind: Job
metadata:
  name: nlm-cli-job
  namespace: nlm
spec:
  template:
    spec:
      containers:
      - name: nlm-cli
        image: nlm/nlm-cli:latest
        command: ["nlm", "batch", "run", "my_sweep"]
        env:
        - name: NLM_LOG_LEVEL
          value: "INFO"
        - name: NLM_RESULTS_DIR
          value: "/results"
        volumeMounts:
        - name: results
          mountPath: /results
      volumes:
      - name: results
        persistentVolumeClaim:
          claimName: nlm-results-pvc
      restartPolicy: Never
  backoffLimit: 3
```

## Troubleshooting Installation

### Common Installation Issues

1. **Missing dependencies**

   ```bash
   # Install system dependencies
   sudo apt-get update
   sudo apt-get install -y python3-dev build-essential libhdf5-dev libboost-dev
   ```

2. **Python version conflicts**

   ```bash
   # Check Python version
   python3 --version
   
   # Install Python 3.8 or higher
   sudo apt-get install python3.8 python3.8-venv
   ```

3. **Enhanced bindings not found**

   ```bash
   # Build enhanced bindings
   cd python
   pip install -e .
   ```

4. **Memory issues during installation**

   ```bash
   # Increase swap space
   sudo swapctl -a /swapfile 2g
   
   # Or reduce parallelism
   pip install --no-build-isolation -e .
   ```

### Environment Setup

```bash
# Set up environment
export NLM_LOG_LEVEL=INFO
export NLM_RESULTS_DIR=$HOME/nlm_results
export PYTHONPATH=$PYTHONPATH:$HOME/nlm_bindings

# Create necessary directories
mkdir -p $NLM_RESULTS_DIR
mkdir -p $HOME/.config/nlm
mkdir -p $HOME/nlm_workspaces

# Initialize configuration
nlm config init --workspace $HOME/nlm_workspaces/my_workspace
```

## Performance Optimization

### Performance Monitoring

The NLM CLI includes performance monitoring tools:

```bash
# Monitor system performance
nlm debug status --format json

# Check resource usage
nlm debug performance --format json

# Monitor batch processing performance
nlm batch run my_sweep --monitor --interval 10
```

### Performance Tuning

```bash
# Optimize for CPU-bound tasks
nlm batch run my_sweep --workers $(nproc)

# Optimize for I/O-bound tasks
nlm batch run my_sweep --queue io_bound

# Optimize for memory usage
nlm batch run my_sweep --checkpoint-interval 1000

# Optimize for distributed execution
nlm batch run my_sweep --distributed --master host:port
```

## Migration from Older Versions

### Migrating from NLM CLI v1.x

```bash
# Upgrade to v2.0
pip install --upgrade nlm-cli

# Check for deprecated features
nlm --check-deprecations

# Migrate configuration files
nlm config migrate --from v1 --to v2
```

### Command Line Changes

Some commands and options have changed in v2.0:

1. **`nlm run` → `nlm batch run`**
2. **`nlm config create-template` → `nlm config create`**
3. **`nlm workflow execute` → `nlm workflow run`**
4. **`--ranges <values>` → `--parameter-ranges <json>`**
5. **`--workers <int>` → `--parallel <int>`**

## Testing the Installation

### Basic Functionality Tests

```bash
# Test CLI installation
python nlm_cli.py --version

# Test help system
python nlm_cli.py --help

# Test basic commands
python nlm_cli.py batch run --help
python nlm_cli.py config create --help
python nlm_cli.py workflow run --help

# Test examples
python nlm_cli_examples.py
```

### Advanced Tests

```bash
# Run the full test suite
pytest tests/

# Run specific test modules
pytest tests/test_cli.py
pytest tests/test_batch.py
pytest tests/test_config.py
pytest tests/test_workflow.py

# Run performance tests
pytest tests/performance/

# Run integration tests
pytest tests/integration/
```

### Integration Tests

```bash
# Test with enhanced bindings
python -c "
import sys
sys.path.insert(0, 'python')
from nlm_cli import NLMCli
cli = NLMCli()
cli.initialize_components()
print('Enhanced NLM components initialized successfully')
"

# Test with advanced utilities
python -c "
import sys
sys.path.insert(0, 'python')
from nlm_advanced.batch import SimulationSweeper
print('NLM Advanced batch utilities imported successfully')
"
```

## Maintenance and Updates

### Updating Dependencies

```bash
# Update poetry.lock
poetry update

# Or update pip requirements
pip install --upgrade -r requirements.txt
```

### Updating Documentation

```bash
# Generate documentation
sphinx-build docs/ docs/_build/

# Check for broken links
linkchecker docs/_build/html
```

### Running Pre-commit Hooks

```bash
# Install pre-commit hooks
pre-commit install

# Run pre-commit checks
pre-commit run --all-files
```

## Support and Community

### Getting Help

1. **Documentation**: [NLM CLI Documentation](https://nlm.readthedocs.io/cli)
2. **GitHub Issues**: [GitHub Repository Issues](https://github.com/nlm-project/nlm/issues)
3. **Community Forum**: [NLM Community Forum](https://community.nlm.ai)
4. **Slack Channel**: [#nlm-cli](https://nlm.slack.com/archives/C012345678)

### Contributing

1. **Code Contributions**: Fork the repository and submit pull requests
2. **Documentation Contributions**: Improve documentation and examples
3. **Bug Reports**: Report bugs and issues
4. **Feature Requests**: Request new features and capabilities

### Attribution

This NLM CLI implementation is based on the enhanced NLM Python bindings and advanced Python utilities. Special thanks to the NLM development team for their contributions to the underlying components.
