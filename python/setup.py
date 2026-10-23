from setuptools import setup, find_packages
import subprocess
import sys
import os

def get_version():
    """Get version from pyproject.toml"""
    try:
        # Parse version from pyproject.toml
        with open("pyproject.toml", "r") as f:
            content = f.read()
            for line in content.split('\n'):
                if 'version =' in line and '"' in line:
                    version = line.split('"')[1]
                    return version
    except:
        pass
    return "0.1.0"

def check_build_dependencies():
    """Check if build dependencies are available"""
    try:
        import pybind11
        import scikit_build_core
        return True
    except ImportError as e:
        print(f"Missing build dependencies: {e}")
        print("Please install: pip install pybind11 scikit-build-core")
        return False

def main():
    if len(sys.argv) > 1 and sys.argv[1] == 'build-deps':
        # Just check build dependencies
        if not check_build_dependencies():
            sys.exit(1)
        print("Build dependencies satisfied")
        return
    
    # Check build dependencies
    if not check_build_dependencies():
        sys.exit(1)
    
    # Determine build type
    build_type = "Release"
    if "--debug" in sys.argv:
        build_type = "Debug"
        sys.argv.remove("--debug")
    
    setup(
        name="pynlm",
        version=get_version(),
        description="Python bindings for NLM (Neural Learning Machine) - Experimental artificial developmental brain",
        long_description=open("README.md").read() if os.path.exists("README.md") else "",
        long_description_content_type="text/markdown",
        author="NLM Authors",
        url="https://github.com/nlm-project/nlm",
        packages=find_packages(include=["python"]),
        package_dir={"": "python"},
        ext_modules=[],  # Will be populated by scikit-build
        setup_requires=["scikit-build-core>=0.5.0", "pybind11>=2.11.0"],
        install_requires=[],
        python_requires=">=3.8",
        classifiers=[
            "Development Status :: 3 - Alpha",
            "Intended Audience :: Science/Research",
            "License :: OSI Approved :: MIT License",
            "Programming Language :: Python :: 3",
            "Programming Language :: Python :: 3.8",
            "Programming Language :: Python :: 3.9",
            "Programming Language :: Python :: 3.10",
            "Programming Language :: Python :: 3.11",
            "Programming Language :: Python :: 3.12",
            "Programming Language :: C++",
            "Topic :: Scientific/Engineering :: Artificial Intelligence",
        ],
        keywords=["neural", "simulation", "spiking", "brain", "learning", "ai"],
        cmdclass={
            "build_ext": "scikit_build_core.build",
        },
        zip_safe=False,
    )

if __name__ == "__main__":
    main()
