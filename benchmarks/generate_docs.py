#!/usr/bin/env python3
"""
NLM Documentation Generator

This script automatically generates documentation from the NLM source code
and maintains consistency across documentation files.
"""

import os
import re
import json
from pathlib import Path
from datetime import datetime

class NLMDocGenerator:
    """Generate NLM documentation from source code and metadata."""
    
    def __init__(self, docs_dir: str = "docs", output_dir: str = "docs"):
        self.docs_dir = Path(docs_dir)
        self.output_dir = Path(output_dir)
        self.docs_dir.mkdir(exist_ok=True)
        self.output_dir.mkdir(exist=True)
    
    def generate_api_reference(self):
        """Generate API reference documentation from header files."""
        api_docs = []
        
        # Find all header files in src directory
        header_files = list(Path("src").rglob("*.hpp"))
        header_files += list(Path("src").rglob("*.h"))
        
        for header_file in header_files:
            class_docs = self._extract_class_documentation(header_file)
            if class_docs:
                api_docs.extend(class_docs)
        
        # Generate API reference
        if api_docs:
            api_content = self._format_api_reference(api_docs)
            api_file = self.output_dir / "API_REFERENCE.md"
            with open(api_file, 'w') as f:
                f.write(api_content)
            
            print(f"Generated API reference: {api_file}")
    
    def _extract_class_documentation(self, header_file: Path) -> List[Dict]:
        """Extract class documentation from header file."""
        classes = []
        
        try:
            content = header_file.read_text()
            
            # Find class definitions
            class_pattern = r'class\s+(\w+)\s*{([^}]+(?:{[^}]*})?)}'
            matches = re.finditer(class_pattern, content, re.DOTALL)
            
            for match in matches:
                class_name = match.group(1)
                class_body = match.group(2)
                
                # Extract public sections
                public_section = self._extract_public_section(class_body)
                
                # Extract documentation comments
                doc_comments = self._extract_doc_comments(header_file, match.start())
                
                classes.append({
                    'name': class_name,
                    'file': str(header_file.relative_to(Path.cwd())),
                    'public_methods': public_section,
                    'documentation': doc_comments,
                    'header_file': header_file.name
                })
        
        except Exception as e:
            print(f"Warning: Failed to process {header_file}: {e}")
        
        return classes
    
    def _extract_public_section(self, class_body: str) -> List[str]:
        """Extract public method signatures from class body."""
        methods = []
        
        # Look for public: section
        public_pattern = r'public:(.*?)(?:private:|protected:|\{)'
        match = re.search(public_pattern, class_body, re.DOTALL)
        
        if match:
            public_code = match.group(1)
            
            # Extract method signatures
            method_pattern = r'(\w+)\s+(\w+)\s*\([^)]*\)\s*(?:const)?\s*;'
            method_matches = re.finditer(method_pattern, public_code)
            
            for method_match in method_matches:
                return_type = method_match.group(1)
                method_name = method_match.group(2)
                
                # Filter out constructor/destructor
                if method_name not in ['~', '~', '()']:
                    methods.append(f"{return_type} {method_name}()")
        
        return methods
    
    def _extract_doc_comments(self, header_file: Path, position: int) -> str:
        """Extract documentation comments preceding a class."""
        try:
            content = header_file.read_text()
            lines = content.split('\n')
            
            # Find the line number of the class
            line_start = content[:position].count('\n')
            
            # Look backwards for documentation comments
            doc_lines = []
            for i in range(line_start - 1, max(-1, line_start - 20), -1):
                line = lines[i].strip()
                if line.startswith('///') or line.startswith('//!'):
                    doc_lines.insert(0, line[3:].strip())
                elif line.startswith('/**') or line.startswith('/*!'):
                    # Multi-line comment
                    multi_line = [line[3:].strip()]
                    # Look for closing */
                    for j in range(i + 1, min(len(lines), i + 20)):
                        multi_line.append(lines[j].strip())
                        if '*/' in lines[j]:
                            break
                    doc_lines.insert(0, '\n'.join(multi_line))
                    break
                elif line and not line.startswith('//'):
                    break
            
            return '\n'.join(doc_lines)
        
        except Exception:
            return ""
    
    def _format_api_reference(self, classes: List[Dict]) -> str:
        """Format API reference into markdown."""
        content = ["# NLM API Reference", "", "Generated automatically from source code.", ""]
        
        # Group classes by header file
        by_file = {}
        for cls in classes:
            file = cls['file']
            if file not in by_file:
                by_file[file] = []
            by_file[file].append(cls)
        
        for file, file_classes in by_file.items():
            content.append(f"## {file}")
            content.append("")
            
            for cls in file_classes:
                content.append(f"### {cls['name']}")
                content.append("")
                
                if cls['documentation']:
                    content.append("```")
                    content.append(cls['documentation'])
                    content.append("```")
                    content.append("")
                
                if cls['public_methods']:
                    content.append("**Public Methods:**")
                    content.append("")
                    for method in cls['public_methods']:
                        content.append(f"- `{method}`")
                    content.append("")
        
        return "\n".join(content)
    
    def generate_usage_examples(self):
        """Generate usage examples from source code."""
        examples = []
        
        # Look for example files
        example_files = list(self.docs_dir.glob("examples_*.md"))
        example_files += list(Path("examples").glob("*.md"))
        
        if example_files:
            for example_file in example_files:
                content = example_file.read_text()
                examples.append(content)
        
        # Generate combined examples file
        if examples:
            combined_content = self._format_examples(examples)
            examples_file = self.output_dir / "USAGE_EXAMPLES.md"
            with open(examples_file, 'w') as f:
                f.write(combined_content)
            
            print(f"Generated usage examples: {examples_file}")
    
    def _format_examples(self, examples: List[str]) -> str:
        """Format examples into a single document."""
        content = ["# NLM Usage Examples", "", "Generated from example files.", "\n---\n\n"]
        
        for i, example in enumerate(examples, 1):
            # Try to extract title from first heading
            lines = example.split('\n')
            title = lines[0].strip('# ').strip() if lines and lines[0].startswith('#') else f"Example {i}"
            
            content.append(f"## {title}")
            content.append("")
            content.append(example)
            content.append("\n---\n\n")
        
        return "\n".join(content)
    
    def generate_configuration_docs(self):
        """Generate configuration documentation."""
        config_docs = []
        
        # Look for configuration documentation files
        config_files = list(self.docs_dir.glob("config*.md"))
        config_files += list(self.docs_dir.glob("CONFIG*.md"))
        
        if config_files:
            for config_file in config_files:
                content = config_file.read_text()
                config_docs.append(content)
        
        # Generate configuration guide
        if config_docs:
            config_content = self._format_configuration_docs(config_docs)
            config_file = self.output_dir / "CONFIGURATION.md"
            with open(config_file, 'w') as f:
                f.write(config_content)
            
            print(f"Generated configuration docs: {config_file}")
    
    def _format_configuration_docs(self, docs: List[str]) -> str:
        """Format configuration documentation."""
        content = ["# NLM Configuration Guide", "", "Generated from configuration documentation.", "\n---\n\n"]
        
        for doc in docs:
            content.append(doc)
            content.append("\n---\n\n")
        
        return "\n".join(content)
    
    def generate_troubleshooting_guide(self):
        """Generate troubleshooting documentation."""
        troubleshooting_docs = []
        
        # Look for troubleshooting documentation
        trouble_files = list(self.docs_dir.glob("*trouble*.md"))
        trouble_files += list(self.docs_dir.glob("*debug*.md"))
        
        if trouble_files:
            for trouble_file in trouble_files:
                content = trouble_file.read_text()
                troubleshooting_docs.append(content)
        
        # Generate troubleshooting guide
        if troubleshooting_docs:
            trouble_content = self._format_troubleshooting(troubleshooting_docs)
            trouble_file = self.output_dir / "TROUBLESHOOTING.md"
            with open(trouble_file, 'w') as f:
                f.write(trouble_content)
            
            print(f"Generated troubleshooting guide: {trouble_file}")
    
    def _format_troubleshooting(self, docs: List[str]) -> str:
        """Format troubleshooting documentation."""
        content = ["# NLM Troubleshooting Guide", "", "Generated from troubleshooting documentation.", "\n---\n\n"]
        
        for doc in docs:
            content.append(doc)
            content.append("\n---\n\n")
        
        return "\n".join(content)
    
    def generate_documentation_index(self):
        """Generate a comprehensive documentation index."""
        index_content = self._create_documentation_index()
        
        index_file = self.output_dir / "INDEX.md"
        with open(index_file, 'w') as f:
            f.write(index_content)
        
        print(f"Generated documentation index: {index_file}")
    
    def _create_documentation_index(self) -> str:
        """Create the documentation index."""
        content = [
            "# NLM Documentation Index",
            "",
            "Generated automatically on " + datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "",
            "This documentation is automatically generated from source code and metadata.",
            "",
            "## Contents",
            "",
            "### Getting Started",
            "- [Getting Started Guide](README.md) - Introduction to NLM and basic usage",
            "- [Installation](INSTALLATION.md) - Building and installing NLM",
            "- [Quick Start](QUICK_START.md) - Minimal working example",
            "",
            "### User Guide",
            "- [API Reference](API_REFERENCE.md) - Complete API documentation",
            "- [Configuration](CONFIGURATION.md) - Configuration options and parameters",
            "- [Usage Examples](USAGE_EXAMPLES.md) - Code examples and demonstrations",
            "",
            "### Advanced Topics",
            "- [Architecture](docs/ARCHITECTURE.md) - System architecture and design",
            "- [Phase 6 Integration](docs/PHASE6_ARCHITECTURE.md) - Complete brain integration",
            "- [Performance](benchmarks/NLM_PERFORMANCE_BENCHMARKS.md) - Performance benchmarks and analysis",
            "- [Experiments](docs/EXPERIMENTS.md) - Experiment descriptions and results",
            "",
            "### Technical Documentation",
            "- [Configuration API](src/core/Config/Config.hpp) - Configuration class documentation",
            "- [Brain Architecture](src/brain/Brain.hpp) - Brain system documentation",
            "- [Memory Systems](src/memory/) - Memory system documentation",
            "- [Neuromodulation](src/neuromodulation/) - Neuromodulation system documentation",
            "",
            "### Research and Science",
            "- [Scientific Background](docs/SCIENCE.md) - Scientific foundations and theory",
            "- [Research Methods](docs/RESEARCH_METHODS.md) - Research methodologies",
            "- [Limitations](docs/LIMITATIONS.md) - System limitations and boundaries",
            "",
            "### Support and Troubleshooting",
            "- [Troubleshooting Guide](TROUBLESHOOTING.md) - Common issues and solutions",
            "- [Performance Troubleshooting](benchmarks/) - Performance analysis tools",
            "- [Example Scripts](examples/) - Complete working examples",
            "",
            "## Generated Files",
            "",
            "This documentation is continuously generated and updated as the codebase evolves.",
            "All generated files are maintained in the `docs/` directory.",
            "",
            "---",
            "\n*Generated by NLMDocGenerator*"]
        
        return "\n".join(content)
    
    def generate_all_documentation(self):
        """Generate all NLM documentation."""
        print("Generating NLM documentation...")
        
        self.generate_api_reference()
        self.generate_usage_examples()
        self.generate_configuration_docs()
        self.generate_troubleshooting_guide()
        self.generate_documentation_index()
        
        print("\nDocumentation generation completed!")


def main():
    """Main function."""
    generator = NLMDocGenerator()
    generator.generate_all_documentation()


if __name__ == "__main__":
    main()
