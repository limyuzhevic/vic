from setuptools import setup, find_packages
import os

# Read the README for the long description
with open('README.md', 'r', encoding='utf-8') as f:
    long_description = f.read()

# Read the pyproject.toml for version
import tomli
with open('pyproject.toml', 'rb') as f:
    pyproject = tomli.load(f)
    version = pyproject['project']['version']

setup(
    name='nlm',
    version=version,
    description='NLM — 熙然 (serene flow): Neural Learning Machine CLI',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='NLM Authors',
    author_email='',
    url='https://github.com/nlm-project/nlm',
    packages=find_packages(include=['nlm*', 'python*']),
    package_dir={
        '': 'python',
        'nlm': 'nlm',
        'nlm_cli': 'nlm/cli'
    },
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Software Development :: Libraries :: Python Libraries',
    ],
    python_requires='>=3.8',
    install_requires=[
        'pyyaml>=6.0',
    ],
    extras_require={
        'dev': [
            'pytest>=7.0',
            'numpy>=1.20',
        ],
        'docs': [
            'sphinx>=4.0',
            'sphinx-rtd-theme>=1.0',
        ],
    },
    entry_points={
        'console_scripts': [
            'nlm = nlm.cli.nlm_cli:main',
        ],
    },
    keywords=['neural', 'simulation', 'spiking', 'brain', 'learning', 'AI', 'neuroscience'],
    project_urls={
        'Homepage': 'https://github.com/nlm-project/nlm',
        'Documentation': 'https://nlm.readthedocs.io',
        'Repository': 'https://github.com/nlm-project/nlm.git',
        'Issues': 'https://github.com/nlm-project/nlm/issues',
    },
)
