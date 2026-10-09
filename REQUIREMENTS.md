# NLM Python Module Requirements
# ========================
#
# This file lists the required and optional dependencies for the NLM Python module.
# It can be used with pip and other package managers.
#

# Required dependencies
numpy>=1.20,<2.0
pandas>=1.3.0,<2.0
matplotlib>=3.3.0,<4.0
seaborn>=0.11.0,<1.0

scipy>=1.7.0,<2.0
scikit-learn>=1.0,<2.0
tqdm>=4.60.0,<5.0
joblib>=1.0.0,<2.0

# Development dependencies
dev = [
    pytest>=7.0,<8.0,
    pytest-cov>=3.0,<4.0,
    black>=21.12b0,<22.0,
    isort>=5.10.0,<6.0,
    flake8>=4.0.0,<5.0,
    mypy>=0.910,<1.0,
    sphinx>=4.0.0,<5.0,
    sphinx-rtd-theme>=1.0.0,<2.0,
    myst-parser>=0.15.0,<1.0,
    jupyter>=1.0.0,<2.0,
    ipykernel>=6.0.0,<7.0,
]

# Full ML dependencies
full = [
    tensorflow>=2.8.0,<3.0,
    torch>=1.9.0,<2.0,
    keras>=2.8.0,<3.0,
    xgboost>=1.5.0,<2.0,
    lightgbm>=3.2.0,<4.0,
    catboost>=0.26.0,<1.0,
    hmmlearn>=0.2.5,<1.0,
    statsmodels>=0.13.0,<1.0,
    gym>=0.21.0,<0.22,
    pyglet>=1.0.0,<2.0,
]

# Debug dependencies
debug = [
    line_profiler>=2.1.0,<3.0,
    memory_profiler>=0.60.0,<1.0,
    python-profiler>=1.0.0,<2.0,
]

# Visualization dependencies
visualization = [
    plotly>=5.0.0,<6.0,
    plotly-express>=0.1.0,<2.0,
    kaleido>=0.2.1,<1.0,
    pyvis==0.1.0,
    networkx>=2.6.0,<3.0,
    igraph>=0.10.0,<1.0,
]

# All optional dependencies
all = [
    dev,
    full,
    debug,
    visualization,
]

# Installation notes
# ===================
#
# Basic installation:
#   pip install -e .
#
# Development installation (with all dev tools):
#   pip install -e .[dev]
#
# Full ML installation (with all ML libraries):
#   pip install -e .[full]
#
# Debug installation (with profiling tools):
#   pip install -e .[debug]
#
# Visualization installation (with plotting tools):
#   pip install -e .[visualization]
#
# All optional dependencies:
#   pip install -e .[all]
#
# Minimum requirements installation:
#   pip install numpy pandas matplotlib seaborn scipy scikit-learn
#

# Performance requirements
# ==========================
#
# This package is designed to work efficiently with:
#
# - Modern multi-core processors (uses parallel processing where beneficial)
# - GPUs (for deep learning models)
# - Large memory systems (for complex neural network training)
#
# For optimal performance, ensure your system has:
#
# - CPU: 4+ cores (for parallel processing)
# - RAM: 8GB+ (for large datasets and model training)
# - GPU: Optional (for deep learning acceleration)
# - Storage: 1GB+ (for models and data)
#

# Usage requirements
# ==================
#
# This package is designed for:
#
# - Research and development in computational neuroscience
# - Education and teaching of neural network concepts
# - Production simulation of brain emulation systems
# - Performance testing and benchmarking
# - Data analysis and visualization of neural data
#
# Users should have:
#
# - Basic Python programming knowledge
# - Understanding of neural network concepts
# - Familiarity with scientific computing in Python
# - Access to appropriate computing resources
#

# Troubleshooting
# ===============
#
# Common issues and solutions:
#
# 1. Import errors:
#    - Make sure all dependencies are installed: pip install -e .[all]
#    - Check Python version: python --version
#    - Ensure you're in the correct directory
#
# 2. Performance issues:
#    - Use smaller models or datasets
#    - Reduce batch size
#    - Use CPU-only versions if GPU not available
#
# 3. Memory issues:
#    - Close other applications
#    - Increase virtual memory if needed
#    - Use more efficient data structures
#
# 4. Installation issues:
#    - Use pip install with --verbose flag for more information
#    - Check if you have necessary system dependencies
#
# For more help, visit: https://github.com/nlm-project/nlm
#
