#!/usr/bin/env python3
"""Setup script for NLM Python package"""

import os
import sys
import platform
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from pathlib import Path

# Read version from __init__.py
version = {}
with open("nlm/__init__.py", "r") as f:
    exec(f.read(), version)

class BuildExtension(build_ext):
    """Custom build extension for NLM"""
    
    def run(self):
        # Ensure the NLM C++ core is built first
        if not os.path.exists("build"):
            print("Building NLM C++ core...")
            # Try to run CMake if available
            try:
                import subprocess
                result = subprocess.run([
                    "cmake", "-B", "build", "-S", ".",
                    "-DCMAKE_BUILD_TYPE=Release"
                ], capture_output=True, text=True)
                if result.returncode == 0:
                    subprocess.run(["cmake", "--build", "build", "--config", "Release"], 
                                 capture_output=True)
                else:
                    print(f"CMake failed: {result.stderr}")
            except Exception as e:
                print(f"Warning: Could not build NLM C++ core: {e}")
        
        super().run()
class InstallCommand(install):
    """Custom install command"""
    
    def run(self):
        self.run_command("build_ext")
        super().run()
def read_requirements():
    """Read requirements from requirements.txt"""
    req_file = Path(__file__).parent / "requirements.txt"
    if req_file.exists():
        with open(req_file, "r") as f:
            return [line.strip() for line in f if line.strip() and not line.startswith("#")]
    return []
def read_readme():
    """Read README content"""
    readme_file = Path(__file__).parent / "README.md"
    if readme_file.exists():
        with open(readme_file, "r") as f:
            return f.read()
    return ""
# Package metadata
package_name = "nlm"
package_version = version.get("__version__", "0.1.0")

setup(
    name=package_name,
    version=package_version,
    description="NLM - Neural Learning Machine Python Package",
    long_description=read_readme(),
    long_description_content_type="text/markdown",
    author="NLM Research Team",
    author_email="research@nlm.ai",
    url="https://github.com/nlm-project/nlm",
    license="MIT",
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
        "Topic :: Scientific/Engineering :: Neural Networks",
    ],
    python_requires=">=3.8",
    install_requires=read_requirements(),
    packages=find_packages(include=["nlm", "nlm.*"]),
    package_data={
        "nlm": [
            "*.py",
            "*.md",
            "*.json",
            "configs/*.cfg",
        ],
    },
    include_package_data=True,
    zip_safe=False,
    keywords=["neural", "network", "brain", "ai", "neuroscience", "simulation"],
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm",
        "Issues": "https://github.com/nlm-project/nlm/issues",
    },
    cmdclass={
        "build_ext": BuildExtension,
        "install": InstallCommand,
    },
    entry_points={
        "console_scripts": [
            "nlm=nlm.__main__:main",
            "nlm-explore=nlm.explore:main",
        ],
    },
)
