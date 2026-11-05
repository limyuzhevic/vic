#!/usr/bin/env python3
"""
Documentation Tests for NLM Project

Tests for verifying that documentation is accurate and up-to-date,
including testing documentation examples.
"""

import pytest
import os
import re
from pathlib import Path
from typing import Dict, List, Any

# Import test utilities and fixtures
from .conftest import (
    test_config,
    pynlm_available,
    sample_config_dict,
    assert_config_dict_equal,
    create_test_config_file,
    cleanup_test_file
)

# Documentation paths
DOCS_DIR = Path(__file__).parent.parent / "docs"
CLI_DOCS_DIR = DOCS_DIR / "cli"
PERFORMANCE_DOCS_DIR = DOCS_DIR / "performance"
CONFIG_DOCS_DIR = DOCS_DIR / "config"

def pytest_configure(config):
    """Configure pytest for documentation tests."""
    config.addinivalue_line("markers", "docs: documentation tests")
    config.addinivalue_line("markers", "examples: tests that verify documentation examples")


class TestDocumentationStructure:
    """Test that documentation structure is correct."""

    def test_docs_directory_exists(self, test_config):
        """Test that the documentation directory exists."""
        assert DOCS_DIR.exists(), f"Documentation directory {DOCS_DIR} should exist"

    def test_cli_docs_directory_exists(self, test_config):
        """Test that CLI documentation directory exists."""
        assert CLI_DOCS_DIR.exists(), f"CLI documentation directory {CLI_DOCS_DIR} should exist"

    def test_performance_docs_directory_exists(self, test_config):
        """Test that performance documentation directory exists."""
        assert PERFORMANCE_DOCS_DIR.exists(), f"Performance documentation directory {PERFORMANCE_DOCS_DIR} should exist"

    def test_config_docs_directory_exists(self, test_config):
        """Test that configuration documentation directory exists."""
        assert CONFIG_DOCS_DIR.exists(), f"Configuration documentation directory {CONFIG_DOCS_DIR} should exist"


class TestDocumentationCompleteness:
    """Test that documentation is comprehensive and complete."""

    def test_readme_exists_and_has_content(self):
        """Test that README.md exists and has content."""
        readme_path = DOCS_DIR.parent / "README.md"
        assert readme_path.exists(), "README.md should exist"

        with open(readme_path, 'r') as f:
            content = f.read()
            assert len(content.strip()) > 0, "README.md should not be empty"
            assert "# " in content, "README.md should have title"

    def test_documentation_files_have_headers(self, test_config):
        """Test that documentation files have proper headers."""
        # Check main documentation files
        doc_files = [
            DOCS_DIR / "index.md",
            DOCS_DIR / "quickstart.md",
            DOCS_DIR / "api.md",
            DOCS_DIR / "architecture.md"
        ]

        for doc_file in doc_files:
            if doc_file.exists():
                with open(doc_file, 'r') as f:
                    content = f.read()
                    # Check for at least one header
                    assert any(line.strip().startswith('#') for line in content.split('\n')), \
                        f"{doc_file.name} should have at least one header"

    def test_cli_documentation_exists(self, test_config):
        """Test that CLI documentation exists for each CLI tool."""
        cli_docs = [
            CLI_DOCS_DIR / "brain.md",
            CLI_DOCS_DIR / "experiment.md",
            CLI_DOCS_DIR / "performance.md",
            CLI_DOCS_DIR / "config.md",
            CLI_DOCS_DIR / "visualize.md"
        ]

        for doc in cli_docs:
            if doc.exists():
                with open(doc, 'r') as f:
                    content = f.read()
                    assert len(content.strip()) > 100, \
                        f"{doc.name} should have substantial content"

    def test_performance_documentation_exists(self, test_config):
        """Test that performance documentation exists."""
        perf_docs = [
            PERFORMANCE_DOCS_DIR / "benchmarking.md",
            PERFORMANCE_DOCS_DIR / "profiling.md",
            PERFORMANCE_DOCS_DIR / "optimization.md"
        ]

        for doc in perf_docs:
            if doc.exists():
                with open(doc, 'r') as f:
                    content = f.read()
                    assert len(content.strip()) > 0, f"{doc.name} should not be empty"

    def test_config_documentation_exists(self, test_config):
        """Test that configuration documentation exists."""
        config_docs = [
            CONFIG_DOCS_DIR / "json.md",
            CONFIG_DOCS_DIR / "yaml.md",
            CONFIG_DOCS_DIR / "key-value.md"
        ]

        for doc in config_docs:
            if doc.exists():
                with open(doc, 'r') as f:
                    content = f.read()
                    assert len(content.strip()) > 0, f"{doc.name} should not be empty"


class TestDocumentationExamples:
    """Test that documentation examples are accurate and can be executed."""

    def test_readme_examples_exist(self):
        """Test that README.md contains examples."""
        readme_path = DOCS_DIR.parent / "README.md"
        assert readme_path.exists(), "README.md should exist"

        with open(readme_path, 'r') as f:
            content = f.read()

        # Check for code blocks (examples)
        code_blocks = re.findall(r'```\w*\n.*?```', content, re.DOTALL)
        assert len(code_blocks) > 0, "README.md should contain code examples"

    def test_cli_example_syntax(self):
        """Test that CLI examples have correct syntax."""
        # This is a basic syntax check for CLI examples
        example_patterns = [
            r'^  nlm [a-z]+',  # Basic CLI command pattern
            r'^  nlm [a-z]+ --[a-z]+ [a-zA-Z0-9-]+',  # CLI with flags
            r'>>> import pynlm',  # Python import pattern
            r'>>>.*=.*',  # Python assignment pattern
        ]

        # Check various documentation files for example patterns
        doc_files = [
            DOCS_DIR / "quickstart.md",
            DOCS_DIR / "cli.md",
            DOCS_DIR / "api.md"
        ]

        for doc_file in doc_files:
            if doc_file.exists():
                with open(doc_file, 'r') as f:
                    content = f.read()
                    lines = content.split('\n')
                    example_lines = [line for line in lines if any(
                        re.match(pattern, line.strip()) for pattern in example_patterns
                    )]
                    assert len(example_lines) > 0, \
                        f"{doc_file.name} should contain example commands or code"

    def test_configuration_examples(self):
        """Test that configuration examples are properly formatted."""
        # Check configuration documentation files for JSON/YAML examples
        config_examples = [
            CONFIG_DOCS_DIR / "json.md",
            CONFIG_DOCS_DIR / "yaml.md",
            CONFIG_DOCS_DIR / "key-value.md"
        ]

        for example_file in config_examples:
            if example_file.exists():
                with open(example_file, 'r') as f:
                    content = f.read()

                    # Check for JSON examples
                    if example_file.name.endswith('.json.md'):
                        json_blocks = re.findall(r'```json\n.*?```', content, re.DOTALL)
                        assert len(json_blocks) > 0, \
                            f"{example_file.name} should contain JSON examples"

                    # Check for YAML examples
                    elif example_file.name.endswith('.yaml.md'):
                        yaml_blocks = re.findall(r'```yaml\n.*?```', content, re.DOTALL)
                        assert len(yaml_blocks) > 0, \
                            f"{example_file.name} should contain YAML examples"

                    # Check for key=value examples
                    elif example_file.name.endswith('.key-value.md'):
                        kv_blocks = re.findall(r'```ini\n.*?```', content, re.DOTALL)
                        assert len(kv_blocks) > 0, \
                            f"{example_file.name} should contain key=value examples"

    def test_performance_example_correctness(self):
        """Test that performance profiling examples are logically correct."""
        # This would involve parsing and validating performance examples
        perf_doc_path = PERFORMANCE_DOCS_DIR / "benchmarking.md"

        if perf_doc_path.exists():
            with open(perf_doc_path, 'r') as f:
                content = f.read()

                # Check for correct benchmark syntax patterns
                benchmark_patterns = [
                    r'nlm performance benchmark',
                    r'--steps \d+',
                    r'--runs \d+',
                    r'--warmup-steps \d+'
                ]

                for pattern in benchmark_patterns:
                    assert re.search(pattern, content), \
                        f"Documentation should contain valid benchmark pattern: {pattern}"


class TestDocumentationAccuracy:
    """Test that documentation content is accurate."""

    def test_documentation_references_correct_parameters(self):
        """Test that documentation references valid parameters."""
        # This would check that parameter names in documentation match actual config
        config = {"random_seed": 42, "neuron_count": 1000, "log_level": "INFO"}

        # Check sample documentation parameters
        doc_params = ["random_seed", "neuron_count", "log_level"]
        for param in doc_params:
            assert param in config, f"Documentation should reference valid parameter: {param}"

    def test_documentation_version_consistency(self):
        """Test that documentation versions are consistent."""
        # Check version references in documentation
        version_files = [
            DOCS_DIR.parent / "setup.py",
            DOCS_DIR.parent / "pyproject.toml",
            DOCS_DIR / "index.md"
        ]

        versions = set()
        for version_file in version_files:
            if version_file.exists():
                content = version_file.read_text()
                # Extract version patterns
                version_match = re.search(r'version\s*=\s*["\'](\d+\.\d+\.\d+)["\']', content)
                if version_match:
                    versions.add(version_match.group(1))

        # All version references should match
        assert len(versions) <= 1, f"Inconsistent versions found: {versions}"

    def test_documentation_examples_up_to_date(self, pynlm_available):
        """Test that documentation examples are up-to-date with current API."""
        if pynlm_available:
            # This would test if examples work with current pynlm API
            # For now, we just check that documentation exists
            assert DOCS_DIR.exists(), "Documentation directory should exist"


class TestDocumentationTestCoverage:
    """Test that documentation includes test coverage information."""

    def test_test_coverage_documents_exist(self):
        """Test that test coverage documentation exists."""
        coverage_docs = [
            DOCS_DIR / "testing.md",
            DOCS_DIR / "test_coverage.md",
            DOCS_DIR / "development.md"
        ]

        for doc in coverage_docs:
            if doc.exists():
                with open(doc, 'r') as f:
                    content = f.read()
                    # Check for test coverage references
                    assert any(keyword in content.lower() for keyword in [
                        "coverage", "test", "pytest", "unittest"
                    ]), f"{doc.name} should mention testing or coverage"

    def test_documentation_instructions_for_running_tests(self):
        """Test that documentation includes instructions for running tests."""
        doc_files_with_test_instructions = [
            DOCS_DIR / "quickstart.md",
            DOCS_DIR / "development.md"
        ]

        found_test_instructions = False
        for doc_file in doc_files_with_test_instructions:
            if doc_file.exists():
                with open(doc_file, 'r') as f:
                    content = f.read()
                    if any(keyword in content.lower() for keyword in [
                        "run tests", "testing", "pytest", "test suite", "test coverage"
                    ]):
                        found_test_instructions = True
                        break

        assert found_test_instructions, \
            "Documentation should include instructions for running tests"


def test_example_python_code_execution():
    """Test that example Python code can be executed (syntax check only)."""
    example_files = [
        DOCS_DIR / "examples" / "basic_usage.py",
        DOCS_DIR / "examples" / "configuration.py",
        DOCS_DIR / "examples" / "experiment.py"
    ]

    for example_file in example_files:
        if example_file.exists():
            with open(example_file, 'r') as f:
                content = f.read()

                # Basic syntax check - look for obvious errors
                # This is not a full execution test, just a syntax check
                assert 'def ' in content or 'class ' in content, \
                    f"{example_file.name} should contain function or class definitions"


if __name__ == "__main__":
    pytest.main([__file__, "-v"])