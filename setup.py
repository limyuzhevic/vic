#!/usr/bin/env python3
"""
Setup script for pynlm - Python bindings for NLM neural simulation framework.
"""

import os
import sys
import subprocess
from pathlib import Path
from setuptools import setup, find_packages
from setuptools.command.build_py import build_py
from setuptools.command.install import install
class BuildPyExtended(build_py):
    """Extended build_py to handle additional file copying."""
    
    def run(self):
        super().run()
        # Copy additional files
        self._copy_additional_files()
        
    def _copy_additional_files(self):
        """Copy additional configuration and documentation files."""
        # Copy examples
        examples_src = Path(__file__).parent / "examples"
        examples_dst = Path(self.build_lib) / "pynlm" / "examples"
        
        if examples_src.exists():
            for item in examples_src.rglob("*"):
                if item.is_file():
                    rel_path = item.relative_to(examples_src)
                    dst_path = examples_dst / rel_path
                    dst_path.parent.mkdir(parents=True, exist_ok=True)
                    self.copy_file(str(item), str(dst_path))
                    
        # Copy documentation
        docs_src = Path(__file__).parent / "docs"
        docs_dst = Path(self.build_lib) / "pynlm" / "docs"
        
        if docs_src.exists():
            for item in docs_src.rglob("*"):
                if item.is_file():
                    rel_path = item.relative_to(docs_src)
                    dst_path = docs_dst / rel_path
                    dst_path.parent.mkdir(parents=True, exist_ok=True)
                    self.copy_file(str(item), str(dst_path))

def get_version():
    """Get version from pyproject.toml."""
    pyproject_path = Path(__file__).parent / "pyproject.toml"
    if pyproject_path.exists():
        with open(pyproject_path, 'r') as f:
            for line in f:
                if 'version = "' in line:
                    return line.split('version = "')[1].split('"')[0]
    return "0.1.0"
def get_requirements():
    """Get requirements from pyproject.toml."""
    pyproject_path = Path(__file__).parent / "pyproject.toml"
    
    if pyproject_path.exists():
        content = pyproject_path.read_text()
        
        # Extract dev requirements
        if 'dev = [' in content:
            start = content.find('dev = [') + 6
            end = content.find(']', start)
            dev_req = content[start:end]
            return [req.strip().strip('", ') for req in dev_req.split(',') if req.strip()]
            
    return []
def get_long_description():
    """Get long description from README."""
    readme_path = Path(__file__).parent / "README.md"
    
    if readme_path.exists():
        return readme_path.read_text()
    return ""
class InstallExtended(install):
    """Extended install to create symbolic links for examples."""
    
    def run(self):
        super().run()
        self._create_example_links()
        
    def _create_example_links(self):
        """Create symbolic links for example files."""
        examples_src = Path(__file__).parent / "examples"
        examples_dst = self.install_lib + "/pynlm/examples"
        
        if examples_src.exists() and os.path.exists(self.install_lib):
            # Create examples directory
            os.makedirs(examples_dst, exist_ok=True)
            
            # Copy example files
            for item in examples_src.rglob("*.py"):
                rel_path = item.relative_to(examples_src)
                dst_path = os.path.join(examples_dst, rel_path.name)
                
                if os.path.exists(dst_path):
                    os.remove(dst_path)
                    
                os.symlink(str(item), dst_path)
def main():
    """Main setup function."""
    # Get project root
    project_root = Path(__file__).parent
    
    # Read README for long description
    readme_path = project_root / "README.md"
    long_description = get_long_description() if readme_path.exists() else ""
    
    # Get version
    version = get_version()
    
    # Get requirements
    requirements = get_requirements()
    
    # Get packages
    packages = find_packages(include=['pynlm', 'pynlm.*'])
    
    # Setup configuration
    setup(
        name="pynlm",
        version=version,
        description="Python bindings for NLM (Neural Learning Machine) neural simulation framework",
        long_description=long_description,
        long_description_content_type="text/markdown",
        author="NLM Project",
        author_email="nlm-project@example.com",
        url="https://github.com/nlm-project/nlm",
        packages=packages,
        package_dir={"": str(project_root)},
        install_requires=requirements,
        extras_require={
            "dev": requirements,
            "visualization": ["matplotlib", "seaborn"],
            "jupyter": ["matplotlib", "seaborn", "jupyter"],
            "all": requirements + ["matplotlib", "seaborn", "jupyter"]
        },
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
            "Topic :: Scientific/Engineering :: Machine Learning",
            "Topic :: Scientific/Engineering :: Neuroscience"
        ],
        keywords=["neural", "simulation", "spiking", "brain", "learning", "neuroscience"],
        python_requires=">=3.8",
        cmdclass={
            "build_py": BuildPyExtended,
            "install": InstallExtended
        },
        include_package_data=True,
        package_data={
            "pynlm": [
                "*.md",
                "*.txt",
                "*.rst",
                "*.json",
                "*.yaml",
                "*.yml",
                "*.toml",
                "*.cfg",
                "*.ini",
                "*.csv",
                "*.png",
                "*.jpg",
                "*.jpeg",
                "*.gif",
                "*.svg",
                "*.pdf",
                "*.ipynb"
            ]
        },
        project_urls={
            "Documentation": "https://nlm.readthedocs.io",
            "Source": "https://github.com/nlm-project/nlm",
            "Bug Reports": "https://github.com/nlm-project/nlm/issues",
            "Examples": "https://github.com/nlm-project/nlm/tree/main/examples"
        }
    )
if __name__ == "__main__":
    main()
