python3 --version
pip3 --version
pip3 list | grep -E "scikit-build-core|pybind11|pynlm" || echo "Required packages not found"

python3 -c "import sys; print(f'Python executable: {sys.executable}')"
python3 -c "import pybind11; print(f'pybind11: {pybind11.__version__}')" 2>&1 || echo "pybind11 not available"

python3 -c "import scikit_build_core; print('scikit_build_core available')" 2>&1 || echo "scikit_build_core not available"

# Try to import pynlm
try:
    python3 -c "import pynlm; print(f'pynlm version: {getattr(pynlm, \"__version__\", \"unknown\")}')"
except ImportError as e:
    echo "pynlm import failed: $e"