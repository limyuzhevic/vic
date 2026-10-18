# Comprehensive Documentation and Tutorials for NLM
# Complete documentation system for NLM

import os
import json
import yaml
import re
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple, Union
import textwrap
import argparse

# Documentation structure
class DocSection:
    """Represents a section in the documentation."""
    
    def __init__(self, title: str, level: int = 1, content: str = ""):
        self.title = title
        self.level = level
        self.content = content
        self.subsections: List['DocSection'] = []
        self.code_blocks: List[Dict[str, Any]] = []
        self.tables: List[Dict[str, Any]] = []
        self.images: List[Dict[str, Any]] = []
    
    def add_subsection(self, subsection: 'DocSection') -> None:
        """Add a subsection."""
        subsection.level = self.level + 1
        self.subsections.append(subsection)
    
    def add_code_block(self, code: str, language: str = "", title: str = "") -> None:
        """Add a code block."""
        self.code_blocks.append({
            'code': code,
            'language': language,
            'title': title
        })
    
    def add_table(self, headers: List[str], rows: List[List[str]], title: str = "") -> None:
        """Add a table."""
        self.tables.append({
            'headers': headers,
            'rows': rows,
            'title': title
        })
    
    def add_image(self, path: str, alt: str = "", caption: str = "") -> None:
        """Add an image."""
        self.images.append({
            'path': path,
            'alt': alt,
            'caption': caption
        })
    
    def to_markdown(self) -> str:
        """Convert section to markdown."""
        lines = []
        
        # Title
        if self.title:
            lines.append("#" * self.level + " " + self.title)
            lines.append("")  # Empty line
        
        # Content
        if self.content:
            lines.append(self.content)
            lines.append("")  # Empty line
        
        # Code blocks
        for code_block in self.code_blocks:
            if code_block['title']:
                lines.append(f"### {code_block['title']}")
                lines.append("")
            
            if code_block['language']:
                lines.append(f"```{code_block['language']}")
            else:
                lines.append("```")
            
            lines.append(code_block['code'])
            lines.append("```")
            lines.append("")
        
        # Tables
        for table in self.tables:
            if table['title']:
                lines.append(f"### {table['title']}")
                lines.append("")
            
            # Simple table formatting
            lines.append("| " + " | ".join(table['headers']) + " |")
            lines.append("| " + " | ".join(["---" for _ in table['headers']]) + " |")
            
            for row in table['rows']:
                lines.append("| " + " | ".join([str(cell) for cell in row]) + " |")
            
            lines.append("")
        
        # Images
        for image in self.images:
            lines.append(f"![{image['alt']}]({image['path']})")
            if image['caption']:
                lines.append(f"*{image['caption']}*\n")
        
        # Subsections
        for subsection in self.subsections:
            lines.extend(subsection.to_markdown().splitlines())
            lines.append("")  # Empty line between subsections
        
        return "\n".join(lines)
class DocumentationGenerator:
    """
    Comprehensive documentation generator for NLM.
    
    This class provides functionality to generate documentation for all
    aspects of NLM, including API documentation, tutorials, examples,
    and reference materials.
    """
    
    def __init__(self, output_directory: str = "docs"):
        self.output_directory = output_directory
        self.sections: List[DocSection] = []
        self.index = DocSection("NLM Documentation Index", 1)
        self.setup_documentation_structure()
    
    def setup_documentation_structure(self) -> None:
        """Set up the documentation structure."""
        # Main sections
        main_sections = [
            ("Introduction", "introduction.md"),
            ("Getting Started", "getting-started.md"),
            ("Basic Usage", "basic-usage.md"),
            ("Advanced Features", "advanced-features.md"),
            ("Python API", "python-api.md"),
            ("Command Line Interface", "cli.md"),
            ("Configuration Management", "configuration.md"),
            ("Performance and Benchmarking", "performance.md"),
            ("Visualization and Analysis", "visualization.md"),
            ("Error Handling and Validation", "error-handling.md"),
            ("Plugin System", "plugins.md"),
            ("Scripting and Automation", "scripting.md"),
            ("Performance Tuning", "tuning.md"),
            ("Extending NLM", "extending.md"),
            ("Reference", "reference.md"),
            ("Examples", "examples.md"),
            ("Troubleshooting", "troubleshooting.md"),
            ("Contributing", "contributing.md"),
            ("API Reference", "api-reference.md")
        ]
        
        # Create main sections
        for title, filename in main_sections:
            section = DocSection(title)
            self.index.add_subsection(section)
            self.sections.append(section)
    
    def generate_documentation(self) -> None:
        """Generate all documentation."""
        # Create output directory
        os.makedirs(self.output_directory, exist_ok=True)
        
        # Generate main index
        index_file = os.path.join(self.output_directory, "index.md")
        with open(index_file, 'w') as f:
            f.write(self.index.to_markdown())
        
        # Generate each section
        for section in self.sections:
            section_file = os.path.join(self.output_directory, section.title.lower().replace(" ", "-") + ".md")
            with open(section_file, 'w') as f:
                f.write(section.to_markdown())
    
    def add_section_content(self, section_title: str, content: str, 
                           level: int = 1, add_to_index: bool = True) -> None:
        """
        Add content to a section.
        
        Args:
            section_title: Title of section
            content: Content to add
            level: Level of section
            add_to_index: Whether to add section to index
        """
        # Find or create section
        section = None
        for s in self.sections:
            if s.title == section_title:
                section = s
                break
        
        if section is None:
            section = DocSection(section_title, level)
            self.sections.append(section)
            if add_to_index:
                self.index.add_subsection(section)
        
        # Add content (simplified - in practice, you'd parse content and add to section)
        section.content += content + "\n\n"
class NLMDocumentation:
    """
    Complete NLM documentation system.
    
    This class provides comprehensive documentation for NLM, including
    tutorials, API reference, examples, and help materials.
    """
    
    def __init__(self, docs_directory: str = "docs"):
        self.docs_directory = docs_directory
        self.generator = DocumentationGenerator(docs_directory)
        self.api_reference = APIRefDocumentation()
        self.tutorials = TutorialSystem()
        self.examples = ExampleGallery()
        self.help_system = HelpSystem()
    
    def generate_complete_documentation(self) -> None:
        """Generate complete documentation."""
        # Generate main documentation
        self.generator.generate_documentation()
        
        # Generate API reference
        self.api_reference.generate_reference()
        
        # Generate tutorials
        self.tutorials.generate_tutorials()
        
        # Generate examples
        self.examples.generate_examples()
        
        # Generate help system
        self.help_system.generate_help()
        
        # Create documentation index
        self._create_documentation_index()
    
    def _create_documentation_index(self) -> None:
        """Create documentation index."""
        index_path = os.path.join(self.docs_directory, "docs_index.md")
        
        index_content = """# NLM Documentation Index

This document provides an overview of all NLM documentation resources.

## Quick Start

* [Getting Started](getting-started.md) - Introduction to NLM and basic setup
* [Basic Usage](basic-usage.md) - Fundamental NLM operations and commands

## User Guides

* [Python API](python-api.md) - Complete Python API documentation
* [Command Line Interface](cli.md) - CLI reference and examples
* [Configuration Management](configuration.md) - Advanced configuration system
* [Performance and Benchmarking](performance.md) - Performance analysis tools
* [Visualization and Analysis](visualization.md) - Visualization and analysis utilities
* [Error Handling and Validation](error-handling.md) - Error handling system

## Advanced Topics

* [Plugin System](plugins.md) - Plugin architecture and usage
* [Scripting and Automation](scripting.md) - Scripting capabilities
* [Performance Tuning](tuning.md) - Performance optimization techniques
* [Extending NLM](extending.md) - Extending NLM functionality

## Reference

* [API Reference](api-reference.md) - Complete API documentation
* [Examples](examples.md) - Code examples and demonstrations
* [Troubleshooting](troubleshooting.md) - Common issues and solutions
* [Contributing](contributing.md) - Contribution guidelines

## Additional Resources

* [GitHub Repository](https://github.com/nlm-project/nlm) - NLM source code
* [Documentation Wiki](https://nlm.readthedocs.io) - Online documentation
* [Discussion Forum](https://github.com/nlm-project/nlm/discussions) - User discussions
* [Issue Tracker](https://github.com/nlm-project/nlm/issues) - Bug reports and feature requests
"""
        
        with open(index_path, 'w') as f:
            f.write(index_content)
class APIRefDocumentation:
    """
    API reference documentation for NLM.
    
    This class generates comprehensive API reference documentation
    for all NLM classes, functions, and modules.
    """
    
    def __init__(self, output_directory: str = "docs/api-reference"):
        self.output_directory = output_directory
        self.api_classes: Dict[str, Dict[str, Any]] = {}
        self.api_functions: Dict[str, Dict[str, Any]] = {}
        self.api_modules: Dict[str, Dict[str, Any]] = {}
    
    def add_class(self, class_name: str, description: str, methods: List[Dict[str, Any]],
                  attributes: List[Dict[str, Any]] = None, examples: List[str] = None) -> None:
        """
        Add a class to the API reference.
        
        Args:
            class_name: Name of the class
            description: Class description
            methods: List of method definitions
            attributes: List of attribute definitions
            examples: List of example code snippets
        """
        self.api_classes[class_name] = {
            'name': class_name,
            'description': description,
            'methods': methods,
            'attributes': attributes or [],
            'examples': examples or []
        }
    
    def add_function(self, function_name: str, description: str,
                    parameters: List[Dict[str, Any]], return_type: str,
                    examples: List[str] = None) -> None:
        """
        Add a function to the API reference.
        
        Args:
            function_name: Name of the function
            description: Function description
            parameters: List of parameter definitions
            return_type: Return type
            examples: List of example code snippets
        """
        self.api_functions[function_name] = {
            'name': function_name,
            'description': description,
            'parameters': parameters,
            'return_type': return_type,
            'examples': examples or []
        }
    
    def add_module(self, module_name: str, description: str,
                   functions: List[str], classes: List[str]) -> None:
        """
        Add a module to the API reference.
        
        Args:
            module_name: Name of the module
            description: Module description
            functions: List of functions in module
            classes: List of classes in module
        """
        self.api_modules[module_name] = {
            'name': module_name,
            'description': description,
            'functions': functions,
            'classes': classes
        }
    
    def generate_reference(self) -> None:
        """Generate API reference documentation."""
        # Create output directory
        os.makedirs(self.output_directory, exist_ok=True)
        
        # Generate main API reference
        main_api_file = os.path.join(self.output_directory, "index.md")
        self._generate_main_api_reference(main_api_file)
        
        # Generate class documentation
        for class_name, class_info in self.api_classes.items():
            class_file = os.path.join(self.output_directory, f"{class_name.lower()}.md")
            self._generate_class_documentation(class_file, class_info)
        
        # Generate function documentation
        for function_name, function_info in self.api_functions.items():
            function_file = os.path.join(self.output_directory, f"{function_name.lower()}.md")
            self._generate_function_documentation(function_file, function_info)
        
        # Generate module documentation
        for module_name, module_info in self.api_modules.items():
            module_file = os.path.join(self.output_directory, f"{module_name.lower()}.md")
            self._generate_module_documentation(module_file, module_info)
    
    def _generate_main_api_reference(self, output_file: str) -> None:
        """Generate main API reference."""
        content = """# NLM API Reference

This document provides the complete API reference for the NLM project.

## Overview

NLM provides several key modules and classes for working with the neural network system:

### Core Classes

"""
        
        # Add core classes
        for class_name in self.api_classes.keys():
            content += f"* [{class_name}]({class_name.lower()}.md)\n"
        
        content += "\n### Core Functions\n\n"
        
        # Add core functions
        for function_name in self.api_functions.keys():
            content += f"* [{function_name}]({function_name.lower()}.md)\n"
        
        content += "\n### Modules\n\n"
        
        # Add modules
        for module_name in self.api_modules.keys():
            content += f"* [{module_name}]({module_name.lower()}.md)\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
    
    def _generate_class_documentation(self, output_file: str, class_info: Dict[str, Any]) -> None:
        """Generate class documentation."""
        name = class_info['name']
        description = class_info['description']
        methods = class_info['methods']
        attributes = class_info['attributes']
        examples = class_info['examples']
        
        content = f"""# {name}

{description}

## Class Definition

This class provides {description.lower()} functionality within the NLM system.

"""
        
        # Add attributes
        if attributes:
            content += "## Attributes\n\n"
            content += "| Name | Type | Description |\n"
            content += "|------|------|-------------|\n"
            
            for attr in attributes:
                content += f"| {attr['name']} | {attr['type']} | {attr['description']} |\n"
            
            content += "\n"
        
        # Add methods
        if methods:
            content += "## Methods\n\n"
            
            for method in methods:
                method_name = method['name']
                method_desc = method['description']
                parameters = method.get('parameters', [])
                return_type = method.get('return_type', 'void')
                is_static = method.get('static', False)
                
                content += f"### {method_name}({', '.join([f'{p['name']}: {p['type']}' for p in parameters])})\n"
                content += f"{method_desc}\n\n"
                content += f"**Returns:** {return_type}\n\n"
                
                if is_static:
                    content += "**Static:** Yes\n\n"
                
                if parameters:
                    content += "**Parameters:**\n\n"
                    content += "| Name | Type | Description |\n"
                    content += "|------|------|-------------|\n"
                    
                    for param in parameters:
                        content += f"| {param['name']} | {param['type']} | {param['description']} |\n"
                    
                    content += "\n"
        
        # Add examples
        if examples:
            content += "## Examples\n\n"
            
            for i, example in enumerate(examples):
                content += f"### Example {i + 1}\n\n"
                content += f"```python\n{example}\n```\n\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
    
    def _generate_function_documentation(self, output_file: str, function_info: Dict[str, Any]) -> None:
        """Generate function documentation."""
        name = function_info['name']
        description = function_info['description']
        parameters = function_info['parameters']
        return_type = function_info['return_type']
        examples = function_info['examples']
        
        content = f"""# {name}

{description}

## Function Signature

```python
def {name}({', '.join([f'{p['name']}: {p['type']}' for p in parameters])}) -> {return_type}:
```

## Parameters

| Name | Type | Description |
|------|------|-------------|\n"""
        
        for param in parameters:
            content += f"| {param['name']} | {param['type']} | {param['description']} |\n"
        
        content += "\n## Returns\n\n"
        content += f"{return_type}\n\n"
        
        # Add examples
        if examples:
            content += "## Examples\n\n"
            
            for i, example in enumerate(examples):
                content += f"### Example {i + 1}\n\n"
                content += f"```python\n{example}\n```\n\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
    
    def _generate_module_documentation(self, output_file: str, module_info: Dict[str, Any]) -> None:
        """Generate module documentation."""
        name = module_info['name']
        description = module_info['description']
        functions = module_info['functions']
        classes = module_info['classes']
        
        content = f"""# {name}

{description}

## Overview

This module provides {description.lower()} functionality within the NLM system.

## Contents

### Functions

"""
        
        for func in functions:
            content += f"* [{func}]({func.lower()}.md)\n"
        
        content += "\n### Classes\n\n"
        
        for cls in classes:
            content += f"* [{cls}]({cls.lower()}.md)\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
class TutorialSystem:
    """
    Tutorial system for NLM.
    
    This class provides a collection of tutorials for learning NLM,
    from basic concepts to advanced features.
    """
    
    def __init__(self, output_directory: str = "docs/tutorials"):
        self.output_directory = output_directory
        self.tutorials: List[Dict[str, Any]] = []
    
    def add_tutorial(self, title: str, difficulty: str, duration: str,
                    description: str, steps: List[Dict[str, Any]]) -> None:
        """
        Add a tutorial.
        
        Args:
            title: Tutorial title
            difficulty: Difficulty level (Beginner, Intermediate, Advanced)
            duration: Estimated duration
            description: Tutorial description
            steps: List of tutorial steps
        """
        tutorial = {
            'title': title,
            'difficulty': difficulty,
            'duration': duration,
            'description': description,
            'steps': steps
        }
        
        self.tutorials.append(tutorial)
    
    def generate_tutorials(self) -> None:
        """Generate tutorials."""
        # Create output directory
        os.makedirs(self.output_directory, exist_ok=True)
        
        # Generate main tutorial index
        index_file = os.path.join(self.output_directory, "index.md")
        self._generate_tutorial_index(index_file)
        
        # Generate each tutorial
        for i, tutorial in enumerate(self.tutorials):
            tutorial_file = os.path.join(self.output_directory, f"{i + 1:02d}-{tutorial['title'].lower().replace(' ', '-')}.md")
            self._generate_tutorial(tutorial_file, tutorial)
    
    def _generate_tutorial_index(self, output_file: str) -> None:
        """Generate tutorial index."""
        content = """# NLM Tutorials

This document provides tutorials for learning NLM, from basic concepts to advanced features.

## Tutorial Categories

"""
        
        # Categorize tutorials by difficulty
        beginner_tutorials = [t for t in self.tutorials if t['difficulty'] == 'Beginner']
        intermediate_tutorials = [t for t in self.tutorials if t['difficulty'] == 'Intermediate']
        advanced_tutorials = [t for t in self.tutorials if t['difficulty'] == 'Advanced']
        
        if beginner_tutorials:
            content += "## Beginner Tutorials\n\n"
            for tutorial in beginner_tutorials:
                content += f"### {tutorial['title']}\n"
                content += f"{tutorial['description']}\n"
                content += f"- **Duration:** {tutorial['duration']}\n"
                content += f"- **Steps:** {len(tutorial['steps'])}\n\n"
            
            content += "[Read Tutorial](beginner-01-introduction.md)\n\n"
        
        if intermediate_tutorials:
            content += "## Intermediate Tutorials\n\n"
            for tutorial in intermediate_tutorials:
                content += f"### {tutorial['title']}\n"
                content += f"{tutorial['description']}\n"
                content += f"- **Duration:** {tutorial['duration']}\n"
                content += f"- **Steps:** {len(tutorial['steps'])}\n\n"
            
            content += "[Read Tutorial](intermediate-01-basic-usage.md)\n\n"
        
        if advanced_tutorials:
            content += "## Advanced Tutorials\n\n"
            for tutorial in advanced_tutorials:
                content += f"### {tutorial['title']}\n"
                content += f"{tutorial['description']}\n"
                content += f"- **Duration:** {tutorial['duration']}\n"
                content += f"- **Steps:** {len(tutorial['steps'])}\n\n"
            
            content += "[Read Tutorial](advanced-01-plugin-development.md)\n\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
    
    def _generate_tutorial(self, output_file: str, tutorial: Dict[str, Any]) -> None:
        """Generate a tutorial."""
        title = tutorial['title']
        difficulty = tutorial['difficulty']
        duration = tutorial['duration']
        description = tutorial['description']
        steps = tutorial['steps']
        
        content = f"""# {title}

**Difficulty:** {difficulty}  
**Estimated Duration:** {duration}  

{description}

## Tutorial Steps

"""
        
        for i, step in enumerate(steps):
            step_title = step.get('title', f"Step {i + 1}")
            step_content = step.get('content', '')
            step_code = step.get('code', None)
            step_output = step.get('output', None)
            
            content += f"### {step_title}\n\n"
            content += f"{step_content}\n\n"
            
            if step_code:
                content += f"**Code Example:**\n\n"
                content += f"```python\n{step_code}\n```\n\n"
            
            if step_output:
                content += f"**Expected Output:**\n\n"
                content += f"```\n{step_output}\n```\n\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
class ExampleGallery:
    """
    Example gallery for NLM.
    
    This class provides a collection of examples demonstrating NLM functionality.
    """
    
    def __init__(self, output_directory: str = "docs/examples"):
        self.output_directory = output_directory
        self.examples: List[Dict[str, Any]] = []
    
    def add_example(self, title: str, description: str, difficulty: str,
                   code: str, output: Optional[str] = None,
                   tags: List[str] = None) -> None:
        """
        Add an example.
        
        Args:
            title: Example title
            description: Example description
            difficulty: Difficulty level
            code: Example code
            output: Expected output
            tags: Example tags
        """
        example = {
            'title': title,
            'description': description,
            'difficulty': difficulty,
            'code': code,
            'output': output,
            'tags': tags or []
        }
        
        self.examples.append(example)
    
    def generate_examples(self) -> None:
        """Generate examples."""
        # Create output directory
        os.makedirs(self.output_directory, exist_ok=True)
        
        # Generate main examples index
        index_file = os.path.join(self.output_directory, "index.md")
        self._generate_examples_index(index_file)
        
        # Generate each example
        for i, example in enumerate(self.examples):
            example_file = os.path.join(self.output_directory, f"{i + 1:02d}-{example['title'].lower().replace(' ', '-')}.md")
            self._generate_example(example_file, example)
    
    def _generate_examples_index(self, output_file: str) -> None:
        """Generate examples index."""
        content = """# NLM Examples

This document provides examples demonstrating NLM functionality.

## Example Categories

"""
        
        # Categorize examples by difficulty
        beginner_examples = [e for e in self.examples if e['difficulty'] == 'Beginner']
        intermediate_examples = [e for e in self.examples if e['difficulty'] == 'Intermediate']
        advanced_examples = [e for e in self.examples if e['difficulty'] == 'Advanced']
        
        if beginner_examples:
            content += "## Beginner Examples\n\n"
            for example in beginner_examples:
                content += f"### {example['title']}\n"
                content += f"{example['description']}\n"
                if example['tags']:
                    content += f"**Tags:** {', '.join(example['tags'])}\n"
                content += f"[View Example](beginner-01-hello-world.md)\n\n"
        
        if intermediate_examples:
            content += "## Intermediate Examples\n\n"
            for example in intermediate_examples:
                content += f"### {example['title']}\n"
                content += f"{example['description']}\n"
                if example['tags']:
                    content += f"**Tags:** {', '.join(example['tags'])}\n"
                content += f"[View Example](intermediate-01-basic-simulation.md)\n\n"
        
        if advanced_examples:
            content += "## Advanced Examples\n\n"
            for example in advanced_examples:
                content += f"### {example['title']}\n"
                content += f"{example['description']}\n"
                if example['tags']:
                    content += f"**Tags:** {', '.join(example['tags'])}\n"
                content += f"[View Example](advanced-01-custom-plugin.md)\n\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
    
    def _generate_example(self, output_file: str, example: Dict[str, Any]) -> None:
        """Generate an example."""
        title = example['title']
        description = example['description']
        difficulty = example['difficulty']
        code = example['code']
        output = example.get('output')
        tags = example.get('tags', [])
        
        content = f"""# {title}

**Difficulty:** {difficulty}  

{description}

"""
        
        if tags:
            content += f"**Tags:** {', '.join(tags)}\n\n"
        
        content += "## Code Example\n\n"
        content += f"```python\n{code}\n```\n\n"
        
        if output:
            content += "## Expected Output\n\n"
            content += f"```\n{output}\n```\n\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
class HelpSystem:
    """
    Help system for NLM.
n
This class provides comprehensive help documentation for NLM,
including command-line help, GUI help, and context-sensitive help.
    """
    
    def __init__(self, output_directory: str = "docs/help"):
        self.output_directory = output_directory
        self.help_topics: Dict[str, Dict[str, Any]] = {}
    
    def add_help_topic(self, topic_id: str, title: str, content: str,
                      category: str = "general", related_topics: List[str] = None) -> None:
        """
        Add a help topic.
        
        Args:
            topic_id: Topic identifier
            title: Topic title
            content: Topic content
            category: Topic category
            related_topics: List of related topic identifiers
        """
        self.help_topics[topic_id] = {
            'id': topic_id,
            'title': title,
            'content': content,
            'category': category,
            'related_topics': related_topics or [],
            'last_updated': time.time()
        }
    
    def generate_help(self) -> None:
        """Generate help documentation."""
        # Create output directory
        os.makedirs(self.output_directory, exist_ok=True)
        
        # Generate main help index
        index_file = os.path.join(self.output_directory, "index.md")
        self._generate_help_index(index_file)
        
        # Generate each help topic
        for topic_id, topic_info in self.help_topics.items():
            topic_file = os.path.join(self.output_directory, f"{topic_id}.md")
            self._generate_help_topic(topic_file, topic_info)
    
    def _generate_help_index(self, output_file: str) -> None:
        """Generate help index."""
        content = """# NLM Help Documentation

This document provides comprehensive help documentation for NLM.

## Help Categories

"""
        
        # Categorize help topics
        categories = set()
        for topic_info in self.help_topics.values():
            categories.add(topic_info['category'])
        
        for category in sorted(categories):
            content += f"## {category.title()} Help Topics\n\n"
            
            # Add topics from this category
            for topic_id, topic_info in self.help_topics.items():
                if topic_info['category'] == category:
                    content += f"* [{topic_info['title']}]({topic_id}.md)\n"
            
            content += "\n"
        
        with open(output_file, 'w') as f:
            f.write(content)
    
    def _generate_help_topic(self, output_file: str, topic_info: Dict[str, Any]) -> None:
        """Generate a help topic."""
        title = topic_info['title']
        content = topic_info['content']
        
        content_header = f"# {title}\n\n"
        
        with open(output_file, 'w') as f:
            f.write(content_header + content)
# Convenience function for creating documentation

def create_documentation(title: str, content: str, level: int = 1) -> DocSection:
    """
    Create a documentation section.
    
    Args:
        title: Section title
        content: Section content
        level: Section level
        
    Returns:
        Documentation section
    """
    return DocSection(title, level, content)

# Main function for documentation generation
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Generate NLM documentation")
    parser.add_argument('--output', '-o', default='docs',
                       help='Output directory for documentation')
    parser.add_argument('--format', '-f', choices=['markdown', 'html', 'json'],
                       default='markdown', help='Documentation format')
    parser.add_argument('--force', '-f', action='store_true',
                       help='Overwrite existing documentation')
    
    args = parser.parse_args()
    
    # Generate documentation
    documentation = NLMDocumentation(args.output)
    documentation.generate_complete_documentation()
    
    print(f"Documentation generated in {args.output}")
    print("Main index: docs/index.md")
    print("API reference: docs/api-reference/index.md")
    print("Tutorials: docs/tutorials/index.md")
    print("Examples: docs/examples/index.md")
    print("Help: docs/help/index.md")
# Export key classes and functions
__all__ = [
    'DocSection',
    'DocumentationGenerator',
    'NLMDocumentation',
    'APIRefDocumentation',
    'TutorialSystem',
    'ExampleGallery',
    'HelpSystem',
    'create_documentation',
    'main'
]
