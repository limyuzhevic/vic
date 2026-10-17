#!/usr/bin/env python3
"""
Documentation Generation for Enhanced NLM Python Bindings

This module provides comprehensive documentation generation for the Enhanced
NLM Python bindings, including API documentation, user guides, examples,
and reference materials for neural simulation research and development.
"""

import os
import re
import json
import inspect
import pydoc
from typing import List, Dict, Tuple, Optional, Any, Union
from pathlib import Path
from datetime import datetime
from abc import ABC, abstractmethod
import warnings

warnings.filterwarnings('ignore')

class DocFormat(Enum):
    """Documentation output formats"""
    RST = "rst"
    MARKDOWN = "markdown"
    HTML = "html"
    JSON = "json"
    TEXT = "text"

class APIStyle(Enum):
    """API documentation styles"""
    NUMPYDOC = "numpydoc"
    GOOGLE = "google"
    EPYDOC = "epydoc"
    RUST = "rust"

@dataclass
class Document:
    """Container for documentation"""
    title: str
    content: str
    format: DocFormat
    metadata: Dict[str, Any]
    sections: List[Dict[str, Any]] = None
    
    def __post_init__(self):
        if self.sections is None:
            self.sections = []

@dataclass
class APIDocumentation:
    """API documentation generator"""
    
    def __init__(self, style: APIStyle = APIStyle.NUMPYDOC):
        self.style = style
        self.modules: Dict[str, Any] = {}
        self.classes: Dict[str, Any] = {}
        self.functions: Dict[str, Any] = {}
        self.constants: Dict[str, Any] = {}
    
    def add_module(self, name: str, module: Any):
        """Add module to API documentation"""
        self.modules[name] = module
    
    def add_class(self, name: str, cls: type):
        """Add class to API documentation"""
        self.classes[name] = cls
    
    def add_function(self, name: str, func: Callable):
        """Add function to API documentation"""
        self.functions[name] = func
    
    def add_constant(self, name: str, value: Any):
        """Add constant to API documentation"""
        self.constants[name] = value
    
    def generate_api_documentation(self, format: DocFormat, output_path: str) -> Document:
        """Generate API documentation"""
        if self.style == APIStyle.NUMPYDOC:
            content = self._generate_numpydoc(format, output_path)
        elif self.style == APIStyle.GOOGLE:
            content = self._generate_google(format, output_path)
        elif self.style == APIStyle.EPYDOC:
            content = self._generate_epydoc(format, output_path)
        elif self.style == APIStyle.RUST:
            content = self._generate_rust(format, output_path)
        else:
            content = self._generate_text(format, output_path)
        
        metadata = {
            'generated_at': datetime.now().isoformat(),
            'style': self.style.value,
            'modules_count': len(self.modules),
            'classes_count': len(self.classes),
            'functions_count': len(self.functions),
            'constants_count': len(self.constants)
        }
        
        return Document(title="NLM Python Bindings API Documentation", content=content, format=format, metadata=metadata)
    
    def _generate_numpydoc(self, format: DocFormat, output_path: str) -> str:
        """Generate NumPy style documentation"""
        if format == DocFormat.RST:
            return self._generate_rst_numpydoc()
        elif format == DocFormat.MARKDOWN:
            return self._generate_markdown_numpydoc()
        elif format == DocFormat.HTML:
            return self._generate_html_numpydoc()
        else:
            return self._generate_text()
    
    def _generate_google(self, format: DocFormat, output_path: str) -> str:
        """Generate Google style documentation"""
        if format == DocFormat.RST:
            return self._generate_rst_google()
        elif format == DocFormat.MARKDOWN:
            return self._generate_markdown_google()
        elif format == DocFormat.HTML:
            return self._generate_html_google()
        else:
            return self._generate_text()
    
    def _generate_epydoc(self, format: DocFormat, output_path: str) -> str:
        """Generate EpyDoc style documentation"""
        if format == DocFormat.RST:
            return self._generate_rst_epydoc()
        elif format == DocFormat.MARKDOWN:
            return self._generate_markdown_epydoc()
        elif format == DocFormat.HTML:
            return self._generate_html_epydoc()
        else:
            return self._generate_text()
    
    def _generate_rust(self, format: DocFormat, output_path: str) -> str:
        """Generate Rust style documentation"""
        if format == DocFormat.RST:
            return self._generate_rst_rust()
        elif format == DocFormat.MARKDOWN:
            return self._generate_markdown_rust()
        elif format == DocFormat.HTML:
            return self._generate_html_rust()
        else:
            return self._generate_text()
    
    def _generate_text(self) -> str:
        """Generate plain text documentation"""
        text = "NLM Python Bindings API Documentation\n"
        text += "=" * 50 + "\n\n"
        
        text += f"Total Modules: {len(self.modules)}\n"
        text += f"Total Classes: {len(self.classes)}\n"
        text += f"Total Functions: {len(self.functions)}\n"
        text += f"Total Constants: {len(self.constants)}\n\n"
        
        # Modules
        text += "Modules:\n"
        for name in sorted(self.modules.keys()):
            text += f"  - {name}\n"
        text += "\n"
        
        # Classes
        text += "Classes:\n"
        for name in sorted(self.classes.keys()):
            cls = self.classes[name]
            docstring = inspect.getdoc(cls)
            text += f"  - {name}: {docstring or 'No documentation'}\n"
        text += "\n"
        
        # Functions
        text += "Functions:\n"
        for name in sorted(self.functions.keys()):
            func = self.functions[name]
            docstring = inspect.getdoc(func)
            text += f"  - {name}: {docstring or 'No documentation'}\n"
        text += "\n"
        
        # Constants
        text += "Constants:\n"
        for name, value in sorted(self.constants.items()):
            text += f"  - {name}: {value}\n"
        
        return text
    
    def _generate_rst_numpydoc(self) -> str:
        """Generate RST NumPy style documentation"""
        rst = "NLM Python Bindings API Documentation\n"
        rst += "=" * 50 + "\n\n"
        
        # Modules section
        rst += "Modules\n"
        rst += "-------\n\n"
        for name in sorted(self.modules.keys()):
            rst += f"{name}\n"
            rst += f"   Description: Enhanced NLM Python bindings module\n\n"
        
        # Classes section
        rst += "Classes\n"
        rst += "-------\n\n"
        for name in sorted(self.classes.keys()):
            cls = self.classes[name]
            docstring = inspect.getdoc(cls)
            rst += f"{name}\n"
            if docstring:
                rst += f"   {docstring}\n"
            rst += "\n"
        
        # Functions section
        rst += "Functions\n"
        rst += "--------\n\n"
        for name in sorted(self.functions.keys()):
            func = self.functions[name]
            docstring = inspect.getdoc(func)
            rst += f"{name}\n"
            if docstring:
                rst += f"   {docstring}\n"
            rst += "\n"
        
        # Constants section
        rst += "Constants\n"
        rst += "---------\n\n"
        for name, value in sorted(self.constants.items()):
            rst += f"{name}: {value}\n"
        
        return rst
    
    def _generate_markdown_numpydoc(self) -> str:
        """Generate Markdown NumPy style documentation"""
        md = "# NLM Python Bindings API Documentation\n\n"
        
        # Modules section
        md += "## Modules\n\n"
        for name in sorted(self.modules.keys()):
            md += f"- **{name}**\n"
            md += f"  Enhanced NLM Python bindings module\n\n"
        
        # Classes section
        md += "## Classes\n\n"
        for name in sorted(self.classes.keys()):
            cls = self.classes[name]
            docstring = inspect.getdoc(cls)
            md += f"### {name}\n"
            if docstring:
                md += f"{docstring}\n"
            md += "\n"
        
        # Functions section
        md += "## Functions\n\n"
        for name in sorted(self.functions.keys()):
            func = self.functions[name]
            docstring = inspect.getdoc(func)
            md += f"### {name}(...)*\n"
            if docstring:
                md += f"{docstring}\n"
            md += "\n"
        
        # Constants section
        md += "## Constants\n\n"
        for name, value in sorted(self.constants.items()):
            md += f"- **{name}**: {value}\n"
        
        return md
    
    def _generate_html_numpydoc(self) -> str:
        """Generate HTML NumPy style documentation"""
        html = """<!DOCTYPE html>
<html>
<head>
    <title>NLM Python Bindings API Documentation</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .module { border: 1px solid #ddd; margin: 10px; padding: 10px; }
        .class { border: 1px solid #ddd; margin: 10px; padding: 10px; }
        .function { border: 1px solid #ddd; margin: 10px; padding: 10px; }
        .constant { border: 1px solid #ddd; margin: 10px; padding: 10px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Python Bindings API Documentation</h1>
    </div>
    
"""
        
        # Modules
        html += '<div class="module">\n'
        html += '<h2>Modules</h2>\n'
        for name in sorted(self.modules.keys()):
            html += f'<h3>{name}</h3>\n'
            html += '<p>Enhanced NLM Python bindings module</p>\n'
        html += '</div>\n\n'
        
        # Classes
        html += '<div class="class">\n'
        html += '<h2>Classes</h2>\n'
        for name in sorted(self.classes.keys()):
            cls = self.classes[name]
            docstring = inspect.getdoc(cls)
            html += f'<h3>{name}</h3>\n'
            if docstring:
                html += f'<p>{docstring}</p>\n'
        html += '</div>\n\n'
        
        # Functions
        html += '<div class="function">\n'
        html += '<h2>Functions</h2>\n'
        for name in sorted(self.functions.keys()):
            func = self.functions[name]
            docstring = inspect.getdoc(func)
            html += f'<h3>{name}(...)</h3>\n'
            if docstring:
                html += f'<p>{docstring}</p>\n'
        html += '</div>\n\n'
        
        # Constants
        html += '<div class="constant">\n'
        html += '<h2>Constants</h2>\n'
        for name, value in sorted(self.constants.items()):
            html += f'<h3>{name}</h3>\n'
            html += f'<p>{value}</p>\n'
        html += '</div>\n\n'
        
        html += '</body>\n</html>'
        return html
    
    def _generate_rst_google(self) -> str:
        """Generate RST Google style documentation"""
        return self._generate_rst_numpydoc()  # Simplified
    
    def _generate_markdown_google(self) -> str:
        """Generate Markdown Google style documentation"""
        return self._generate_markdown_numpydoc()  # Simplified
    
    def _generate_html_google(self) -> str:
        """Generate HTML Google style documentation"""
        return self._generate_html_numpydoc()  # Simplified
    
    def _generate_rst_epydoc(self) -> str:
        """Generate RST EpyDoc style documentation"""
        return self._generate_rst_numpydoc()  # Simplified
    
    def _generate_markdown_epydoc(self) -> str:
        """Generate Markdown EpyDoc style documentation"""
        return self._generate_markdown_numpydoc()  # Simplified
    
    def _generate_html_epydoc(self) -> str:
        """Generate HTML EpyDoc style documentation"""
        return self._generate_html_numpydoc()  # Simplified
    
    def _generate_rst_rust(self) -> str:
        """Generate RST Rust style documentation"""
        return self._generate_rst_numpydoc()  # Simplified
    
    def _generate_markdown_rust(self) -> str:
        """Generate Markdown Rust style documentation"""
        return self._generate_markdown_numpydoc()  # Simplified
    
    def _generate_html_rust(self) -> str:
        """Generate HTML Rust style documentation"""
        return self._generate_html_numpydoc()  # Simplified
class UserGuideGenerator:
    """Generator for user guides"""
    
    def __init__(self):
        self.tutorials: List[Dict[str, Any]] = []
        self.examples: List[Dict[str, Any]] = []
        self.concepts: List[Dict[str, Any]] = []
    
    def add_tutorial(self, title: str, content: str, difficulty: str = "beginner"):
        """Add tutorial"""
        self.tutorials.append({
            'title': title,
            'content': content,
            'difficulty': difficulty,
            'order': len(self.tutorials)
        })
    
    def add_example(self, title: str, code: str, description: str = "",
                   language: str = "python"):
        """Add example"""
        self.examples.append({
            'title': title,
            'code': code,
            'description': description,
            'language': language,
            'order': len(self.examples)
        })
    
    def add_concept(self, title: str, explanation: str, related: List[str] = None):
        """Add concept"""
        self.concepts.append({
            'title': title,
            'explanation': explanation,
            'related': related or [],
            'order': len(self.concepts)
        })
    
    def generate_user_guide(self, format: DocFormat, output_path: str) -> Document:
        """Generate user guide"""
        if format == DocFormat.RST:
            content = self._generate_rst_user_guide()
        elif format == DocFormat.MARKDOWN:
            content = self._generate_markdown_user_guide()
        elif format == DocFormat.HTML:
            content = self._generate_html_user_guide()
        elif format == DocFormat.JSON:
            content = self._generate_json_user_guide()
        else:
            content = self._generate_text_user_guide()
        
        metadata = {
            'generated_at': datetime.now().isoformat(),
            'tutorials_count': len(self.tutorials),
            'examples_count': len(self.examples),
            'concepts_count': len(self.concepts)
        }
        
        return Document(title="NLM Python Bindings User Guide", content=content, format=format, metadata=metadata)
    
    def _generate_rst_user_guide(self) -> str:
        """Generate RST user guide"""
        rst = "NLM Python Bindings User Guide\n"
        rst += "=" * 50 + "\n\n"
        
        # Introduction
        rst += "Introduction\n"
        rst += "------------\n\n"
        rst += "The Enhanced NLM Python bindings provide a comprehensive API for neural simulation "
        rst += "and research. This guide covers the basics of getting started with the library, "
        rst += "as well as advanced topics for experienced users.\n\n"
        
        # Concepts
        rst += "Concepts\n"
        rst += "-------\n\n"
        for concept in self.concepts:
            rst += f"{concept['order'] + 1}. {concept['title']}\n"
            rst += f"   {concept['explanation']}\n\n"
        
        # Tutorials
        rst += "Tutorials\n"
        rst += "--------\n\n"
        for tutorial in self.tutorials:
            rst += f"{tutorial['order'] + 1}. {tutorial['title']} ({tutorial['difficulty']})\n"
            rst += f"   {tutorial['content']}\n\n"
        
        # Examples
        rst += "Examples\n"
        rst += "-------\n\n"
        for example in self.examples:
            rst += f"{example['order'] + 1}. {example['title']}\n"
            rst += f"   {example['description']}\n"
            rst += f"   ```{example['language']}\n"
            rst += f"{example['code']}\n"
            rst += f"   ```\n\n"
        
        return rst
    
    def _generate_markdown_user_guide(self) -> str:
        """Generate Markdown user guide"""
        md = "# NLM Python Bindings User Guide\n\n"
        
        # Introduction
        md += "## Introduction\n\n"
        md += "The Enhanced NLM Python bindings provide a comprehensive API for neural simulation "
        md += "and research. This guide covers the basics of getting started with the library, "
        md += "as well as advanced topics for experienced users.\n\n"
        
        # Concepts
        md += "## Concepts\n\n"
        for concept in self.concepts:
            md += f"### {concept['order'] + 1}. {concept['title']}\n"
            md += f"{concept['explanation']}\n\n"
        
        # Tutorials
        md += "## Tutorials\n\n"
        for tutorial in self.tutorials:
            md += f"### {tutorial['order'] + 1}. {tutorial['title']} ({tutorial['difficulty']})\n"
            md += f"{tutorial['content']}\n\n"
        
        # Examples
        md += "## Examples\n\n"
        for example in self.examples:
            md += f"### {example['order'] + 1}. {example['title']}\n"
            md += f"{example['description']}\n\n"
            md += f"```{example['language']}\n"
            md += f"{example['code']}\n"
            md += f"```\n\n"
        
        return md
    
    def _generate_html_user_guide(self) -> str:
        """Generate HTML user guide"""
        html = """<!DOCTYPE html>
<html>
<head>
    <title>NLM Python Bindings User Guide</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
        .section { margin: 20px 0; }
        .example { background-color: #f8f8f8; border: 1px solid #ddd; padding: 15px; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Python Bindings User Guide</h1>
    </div>
    
"""
        
        # Introduction
        html += '<div class="section">\n'
        html += '<h2>Introduction</h2>\n'
        html += '<p>The Enhanced NLM Python bindings provide a comprehensive API for neural simulation ' \
                'and research. This guide covers the basics of getting started with the library, ' \
                'as well as advanced topics for experienced users.</p>\n'
        html += '</div>\n\n'
        
        # Concepts
        html += '<div class="section">\n'
        html += '<h2>Concepts</h2>\n'
        for concept in self.concepts:
            html += f'<h3>{concept["order"] + 1}. {concept["title"]}</h3>\n'
            html += f'<p>{concept["explanation"]}</p>\n'
        html += '</div>\n\n'
        
        # Tutorials
        html += '<div class="section">\n'
        html += '<h2>Tutorials</h2>\n'
        for tutorial in self.tutorials:
            html += f'<h3>{tutorial["order"] + 1}. {tutorial["title"]} ({tutorial["difficulty"]})</h3>\n'
            html += f'<p>{tutorial["content"]}</p>\n'
        html += '</div>\n\n'
        
        # Examples
        html += '<div class="section">\n'
        html += '<h2>Examples</h2>\n'
        for example in self.examples:
            html += f'<div class="example">\n'
            html += f'<h3>{example["order"] + 1}. {example["title"]}</h3>\n'
            html += f'<p>{example["description"]}</p>\n'
            html += f'<pre><code class="language-{example["language"]}">{example["code"]}</code></pre>\n'
            html += '</div>\n\n'
        html += '</div>\n\n'
        
        html += '</body>\n</html>'
        return html
    
    def _generate_json_user_guide(self) -> str:
        """Generate JSON user guide"""
        data = {
            'title': 'NLM Python Bindings User Guide',
            'generated_at': datetime.now().isoformat(),
            'tutorials': self.tutorials,
            'examples': self.examples,
            'concepts': self.concepts
        }
        
        return json.dumps(data, indent=2)
    
    def _generate_text_user_guide(self) -> str:
        """Generate text user guide"""
        text = "NLM Python Bindings User Guide\n"
        text += "=" * 50 + "\n\n"
        
        # Introduction
        text += "Introduction\n"
        text += "------------\n\n"
        text += "The Enhanced NLM Python bindings provide a comprehensive API for neural simulation "
        text += "and research. This guide covers the basics of getting started with the library, "
        text += "as well as advanced topics for experienced users.\n\n"
        
        # Concepts
        text += "Concepts\n"
        text += "-------\n\n"
        for concept in self.concepts:
            text += f"{concept['order'] + 1}. {concept['title']}\n"
            text += f"   {concept['explanation']}\n\n"
        
        # Tutorials
        text += "Tutorials\n"
        text += "--------\n\n"
        for tutorial in self.tutorials:
            text += f"{tutorial['order'] + 1}. {tutorial['title']} ({tutorial['difficulty']})\n"
            text += f"   {tutorial['content']}\n\n"
        
        # Examples
        text += "Examples\n"
        text += "-------\n\n"
        for example in self.examples:
            text += f"{example['order'] + 1}. {example['title']}\n"
            text += f"   {example['description']}\n"
            text += f"   {example['language']}:\n"
            text += f"   {example['code']}\n\n"
        
        return text
class APIIntrospector:
    """API introspection tools"""
    
    def __init__(self, modules: Dict[str, Any]):
        self.modules = modules
    
    def introspect_module(self, module_name: str) -> Dict[str, Any]:
        """Introspect a module"""
        if module_name not in self.modules:
            return {'error': f'Module {module_name} not found'}
        
        module = self.modules[module_name]
        
        introspection = {
            'module_name': module_name,
            'file': getattr(module, '__file__', 'N/A'),
            'doc': getattr(module, '__doc__', 'N/A'),
            'version': getattr(module, '__version__', 'N/A'),
            'author': getattr(module, '__author__', 'N/A'),
            'license': getattr(module, '__license__', 'N/A'),
            'copyright': getattr(module, '__copyright__', 'N/A'),
            'members': []
        }
        
        # Get module members
        if hasattr(module, '__all__'):
            introspection['members'] = module.__all__
        else:
            introspection['members'] = [name for name in dir(module) 
                                      if not name.startswith('_')]
        
        return introspection
    
    def introspect_class(self, class_name: str, module_name: str) -> Dict[str, Any]:
        """Introspect a class"""
        if module_name not in self.modules:
            return {'error': f'Module {module_name} not found'}
        
        module = self.modules[module_name]
        
        if not hasattr(module, class_name):
            return {'error': f'Class {class_name} not found in module {module_name}'}
        
        cls = getattr(module, class_name)
        
        introspection = {
            'class_name': class_name,
            'module': module_name,
            'doc': inspect.getdoc(cls),
            'methods': [],
            'attributes': [],
            'bases': [base.__name__ for base in inspect.getmro(cls) if base != object],
            'module_file': getattr(module, '__file__', 'N/A')
        }
        
        # Get class members
        for name, obj in inspect.getmembers(cls):
            if inspect.isfunction(obj) or inspect.ismethod(obj):
                introspection['methods'].append({
                    'name': name,
                    'doc': inspect.getdoc(obj),
                    'signature': inspect.signature(obj) if inspect.isfunction(obj) else 'N/A',
                    'is_method': inspect.ismethod(obj)
                })
            elif not name.startswith('_'):
                introspection['attributes'].append({
                    'name': name,
                    'type': type(obj).__name__,
                    'value': repr(obj) if not callable(obj) else 'callable'
                })
        
        return introspection
    
    def introspect_function(self, func_name: str, module_name: str) -> Dict[str, Any]:
        """Introspect a function"""
        if module_name not in self.modules:
            return {'error': f'Module {module_name} not found'}
        
        module = self.modules[module_name]
        
        if not hasattr(module, func_name):
            return {'error': f'Function {func_name} not found in module {module_name}'}
        
        func = getattr(module, func_name)
        
        introspection = {
            'function_name': func_name,
            'module': module_name,
            'doc': inspect.getdoc(func),
            'signature': inspect.signature(func),
            'module_file': getattr(module, '__file__', 'N/A'),
            'is_builtin': inspect.isbuiltin(func),
            'is_callable': callable(func),
            'is_async': inspect.iscoroutinefunction(func)
        }
        
        return introspection
    
    def generate_api_introspection(self, output_path: str) -> Dict[str, Any]:
        """Generate API introspection data"""
        introspection_data = {
            'generated_at': datetime.now().isoformat(),
            'modules': {},
            'classes': {},
            'functions': {},
            'constants': {}
        }
        
        # Introspect modules
        for module_name in self.modules.keys():
            introspection_data['modules'][module_name] = self.introspect_module(module_name)
        
        # In a real implementation, you would introspect classes, functions, and constants
        # from the actual NLM modules
        
        # Write to file
        with open(output_path, 'w') as f:
            json.dump(introspection_data, f, indent=2)
        
        return introspection_data
class DocumentationGenerator:
    """Main documentation generator"""
    
    def __init__(self):
        self.api_docs = APIDocumentation()
        self.user_guide = UserGuideGenerator()
        self.introspector = APIIntrospector({})
    
    def add_module(self, name: str, module: Any):
        """Add module to documentation"""
        self.api_docs.add_module(name, module)
        self.introspector.modules[name] = module
    
    def add_tutorial(self, title: str, content: str, difficulty: str = "beginner"):
        """Add tutorial to user guide"""
        self.user_guide.add_tutorial(title, content, difficulty)
    
    def add_example(self, title: str, code: str, description: str = "",
                   language: str = "python"):
        """Add example to user guide"""
        self.user_guide.add_example(title, code, description, language)
    
    def add_concept(self, title: str, explanation: str, related: List[str] = None):
        """Add concept to user guide"""
        self.user_guide.add_concept(title, explanation, related)
    
    def generate_all_documentation(self, output_dir: str, formats: List[DocFormat]):
        """Generate all documentation"""
        os.makedirs(output_dir, exist_ok=True)
        
        results = {}
        
        # Generate API documentation
        for format in formats:
            api_doc = self.api_docs.generate_api_documentation(format, 
                                                              os.path.join(output_dir, f"api.{format.value}.doc"))
            results['api_' + format.value] = api_doc
        
        # Generate user guide
        for format in formats:
            user_doc = self.user_guide.generate_user_guide(format,
                                                           os.path.join(output_dir, f"user_guide.{format.value}.doc"))
            results['user_guide_' + format.value] = user_doc
        
        # Generate API introspection
        introspect_data = self.introspector.generate_api_introspection(
            os.path.join(output_dir, 'api_introspection.json'))
        results['api_introspection'] = introspect_data
        
        return results

# Convenience functions

def create_api_documentation(style: APIStyle = APIStyle.NUMPYDOC) -> APIDocumentation:
    """Convenience function to create API documentation generator"""
    return APIDocumentation(style)

def create_user_guide_generator() -> UserGuideGenerator:
    """Convenience function to create user guide generator"""
    return UserGuideGenerator()

def create_api_introspector(modules: Dict[str, Any]) -> APIIntrospector:
    """Convenience function to create API introspector"""
    return APIIntrospector(modules)

def create_documentation_generator() -> DocumentationGenerator:
    """Convenience function to create documentation generator"""
    return DocumentationGenerator()

# Export all classes and functions
__all__ = [
    'DocFormat', 'APIStyle', 'Document', 'APIDocumentation',
    'UserGuideGenerator', 'APIIntrospector', 'DocumentationGenerator',
    'create_api_documentation', 'create_user_guide_generator',
    'create_api_introspector', 'create_documentation_generator'
]
