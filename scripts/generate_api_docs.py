#!/usr/bin/env python3
"""
NLM Documentation Generation Script
Generates API documentation from Doxygen comments and source code
"""

import argparse
import json
import re
import os
import sys
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Any, Optional

class APIDocumentationGenerator:
    def __init__(self, source_dir: str, output_dir: str):
        self.source_dir = Path(source_dir)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Parse Doxygen comments from header files
        self.header_files = list(self.source_dir.rglob("*.hpp"))
        self.source_files = list(self.source_dir.rglob("*.cpp"))
        
        # Documentation structure
        self.doc_structure = {
            "overview": "API Overview",
            "namespaces": "Namespaces",
            "classes": "Classes",
            "functions": "Functions",
            "types": "Type Definitions",
            "examples": "Examples",
            "configuration": "Configuration Options"
        }
    
    def generate_api_documentation(self):
        """Generate comprehensive API documentation"""
        print("Generating NLM API documentation...")
        
        # Parse Doxygen comments
        doxygen_data = self.parse_doxygen_comments()
        
        # Generate main API documentation file
        self.generate_main_api_doc(doxygen_data)
        
        # Generate namespace documentation
        self.generate_namespace_docs(doxygen_data)
        
        # Generate class documentation
        self.generate_class_docs(doxygen_data)
        
        # Generate function documentation
        self.generate_function_docs(doxygen_data)
        
        # Generate type definitions
        self.generate_type_docs(doxygen_data)
        
        # Generate examples
        self.generate_examples()
        
        # Generate configuration guide
        self.generate_configuration_guide()
        
        print(f"API documentation generated in: {self.output_dir}")
    
    def parse_doxygen_comments(self) -> Dict[str, Any]:
        """Parse Doxygen comments from header files"""
        doxygen_data = {
            "namespaces": {},
            "classes": {},
            "functions": {},
            "types": {},
            "examples": [],
            "configuration": []
        }
        
        for header_file in self.header_files:
            self.parse_header_file(header_file, doxygen_data)
        
        return doxygen_data
    
    def parse_header_file(self, header_file: Path, doxygen_data: Dict[str, Any]):
        """Parse a single header file for Doxygen comments"""
        try:
            content = header_file.read_text()
            
            # Extract namespace blocks
            namespace_pattern = r'namespace\s+(\w+)\s*{([^}]+)}'
            for match in re.finditer(namespace_pattern, content, re.MULTILINE | re.DOTALL):
                namespace_name = match.group(1)
                namespace_content = match.group(2)
                doxygen_data["namespaces"][namespace_name] = self.parse_namespace_content(namespace_content)
            
            # Extract class definitions
            class_pattern = r'/\*\*\s*(.*?)\s*\*/\s*class\s+(\w+)(?:\s*:\s*public\s+(\w+))?\s*{([^}]+)}'
            for match in re.finditer(class_pattern, content, re.MULTILINE | re.DOTALL):
                class_docs = match.group(1)
                class_name = match.group(2)
                base_class = match.group(3) if match.group(3) else ""
                class_content = match.group(4)
                
                doxygen_data["classes"][class_name] = {
                    "documentation": class_docs,
                    "base_class": base_class,
                    "methods": self.parse_class_methods(class_content),
                    "attributes": self.parse_class_attributes(class_content),
                    "file": str(header_file.relative_to(self.source_dir.parent))
                }
            
            # Extract type definitions
            type_pattern = r'using\s+(\w+)\s*=\s*(.+);'
            for match in re.finditer(type_pattern, content):
                type_name = match.group(1)
                type_def = match.group(2)
                doxygen_data["types"][type_name] = {
                    "definition": type_def,
                    "file": str(header_file.relative_to(self.source_dir.parent))
                }
            
            # Extract function prototypes
            func_pattern = r'/\*\*\s*(.*?)\s*\*/\s*(\w+)\s*\((.*?)\)\s*(?:const)?;'
            for match in re.finditer(func_pattern, content, re.MULTILINE | re.DOTALL):
                func_docs = match.group(1)
                func_name = match.group(2)
                func_params = match.group(3)
                
                doxygen_data["functions"][func_name] = {
                    "documentation": func_docs,
                    "parameters": self.parse_function_params(func_params),
                    "file": str(header_file.relative_to(self.source_dir.parent))
                }
            
        except Exception as e:
            print(f"Warning: Failed to parse {header_file}: {e}")
    
    def parse_namespace_content(self, content: str) -> Dict[str, Any]:
        """Parse namespace content"""
        namespace_data = {
            "classes": {},
            "functions": {},
            "types": {}
        }
        
        # Parse classes in namespace
        class_pattern = r'class\s+(\w+)(?:\s*:\s*public\s+(\w+))?\s*{([^}]+)}'
        for match in re.finditer(class_pattern, content, re.MULTILINE | re.DOTALL):
            class_name = match.group(1)
            base_class = match.group(2) if match.group(2) else ""
            class_content = match.group(3)
            
            namespace_data["classes"][class_name] = {
                "base_class": base_class,
                "methods": self.parse_class_methods(class_content),
                "attributes": self.parse_class_attributes(class_content)
            }
        
        # Parse type definitions
        type_pattern = r'using\s+(\w+)\s*=\s*(.+);'
        for match in re.finditer(type_pattern, content):
            type_name = match.group(1)
            type_def = match.group(2)
            namespace_data["types"][type_name] = type_def
        
        # Parse function definitions
        func_pattern = r'(\w+)\s*\((.*?)\)\s*(?:const)?;'
        for match in re.finditer(func_pattern, content, re.MULTILINE | re.DOTALL):
            func_name = match.group(1)
            func_params = match.group(2)
            
            namespace_data["functions"][func_name] = {
                "parameters": self.parse_function_params(func_params)
            }
        
        return namespace_data
    
    def parse_class_methods(self, class_content: str) -> List[Dict[str, Any]]:
        """Parse methods from class content"""
        methods = []
        
        # Parse method definitions
        method_pattern = r'/\*\*\s*(.*?)\s*\*/\s*(\w+)\s*\((.*?)\)\s*(?:const)?\s*{([^}]+)}'
        for match in re.finditer(method_pattern, class_content, re.MULTILINE | re.DOTALL):
            method_docs = match.group(1)
            method_name = match.group(2)
            method_params = match.group(3)
            method_body = match.group(4)
            
            methods.append({
                "name": method_name,
                "documentation": method_docs,
                "parameters": self.parse_function_params(method_params),
                "body": method_body,
                "has_body": True
            })
        
        return methods
    
    def parse_class_attributes(self, class_content: str) -> List[Dict[str, Any]]:
        """Parse attributes from class content"""
        attributes = []
        
        # Parse member variables
        attr_pattern = r'(\w+)\s+(\w+);\s*/\*\*\s*(.*?)\s*\*/'
        for match in re.finditer(attr_pattern, class_content):
            attr_type = match.group(1)
            attr_name = match.group(2)
            attr_docs = match.group(3)
            
            attributes.append({
                "type": attr_type,
                "name": attr_name,
                "documentation": attr_docs
            })
        
        return attributes
    
    def parse_function_params(self, params: str) -> List[Dict[str, str]]:
        """Parse function parameters"""
        if not params.strip():
            return []
        
        param_pattern = r'(\w+)\s+(\w+)(?:\s*=\s*(.+))?'
        parameters = []
        
        for match in re.finditer(param_pattern, params):
            param_type = match.group(1)
            param_name = match.group(2)
            param_default = match.group(3) if match.group(3) else ""
            
            parameters.append({
                "type": param_type,
                "name": param_name,
                "default": param_default
            })
        
        return parameters
    
    def generate_main_api_doc(self, doxygen_data: Dict[str, Any]):
        """Generate main API documentation file"""
        content = f"""# NLM API Reference

## Overview

This document provides comprehensive API documentation for the NLM (熙然) neural learning machine project. The API is organized into logical modules and follows a consistent C++ design pattern.

Generated automatically from Doxygen comments on {len(self.header_files)} header files.

## Contents

1. [Namespaces](#namespaces)
2. [Classes](#classes)
3. [Functions](#functions)
4. [Type Definitions](#type-definitions)
5. [Configuration Options](#configuration-options)
6. [Examples](#examples)

## Namespace: nlm

The NLM API is contained within the `nlm` namespace. All classes, functions, and types are defined within this namespace.

## Classes

"""
        
        for class_name, class_data in doxygen_data["classes"].items():
            content += self.generate_class_doc(class_name, class_data)
        
        content += "\n## Functions\n\n"
        
        for func_name, func_data in doxygen_data["functions"].items():
            content += self.generate_function_doc(func_name, func_data)
        
        content += "\n## Type Definitions\n\n"
        
        for type_name, type_data in doxygen_data["types"].items():
            content += f"- `{type_name}`: {type_data['definition']}\n"
        
        content += self.generate_examples_section()
        content += self.generate_configuration_section(doxygen_data)
        
        # Write to file
        api_file = self.output_dir / "API_REFERENCE.md"
        api_file.write_text(content)
    
    def generate_class_doc(self, class_name: str, class_data: Dict[str, Any]) -> str:
        """Generate documentation for a single class"""
        doc = class_data.get("documentation", "")
        base_class = class_data.get("base_class", "")
        methods = class_data.get("methods", [])
        attributes = class_data.get("attributes", [])
        file_path = class_data.get("file", "")
        
        content = f"""
### {class_name}

**Base Class:** {base_class if base_class else "Object"}

**File:** {file_path}

**Documentation:**
{doc}

#### Public Methods

"""
        
        for method in methods:
            content += f"- **`{method['name']}({self.format_parameters(method['parameters'])}`**"
            
            if method.get('has_body'):
                content += " *\n"
            else:
                content += "\n"
            
            if method.get('documentation'):
                content += f"  {method['documentation']}\n"
            
            if method['parameters']:
                content += f"  **Parameters:**\n"
                for param in method['parameters']:
                    content += f"    - `{param['type']} {param['name']}`"
                    if param.get('default'):
                        content += f" (default: {param['default']})"
                    content += "\n"
        
        if attributes:
            content += "\n#### Attributes\n\n"
            for attr in attributes:
                content += f"- `{attr['type']} {attr['name']}`"
                if attr.get('documentation'):
                    content += f": {attr['documentation']}"
                content += "\n"
        
        return content + "\n"
    
    def generate_function_doc(self, func_name: str, func_data: Dict[str, Any]) -> str:
        """Generate documentation for a function"""
        doc = func_data.get("documentation", "")
        parameters = func_data.get("parameters", [])
        file_path = func_data.get("file", "")
        
        content = f"""
### {func_name}({self.format_parameters(parameters)})

**File:** {file_path}

**Documentation:**
{doc}

#### Parameters

"""
        
        for param in parameters:
            content += f"- `{param['type']} {param['name']}`"
            if param.get('default'):
                content += f" (default: {param['default']})"
            content += "\n"
        
        return content + "\n"
    
    def generate_namespace_docs(self, doxygen_data: Dict[str, Any]):
        """Generate namespace documentation"""
        namespaces_file = self.output_dir / "NAMESPACES.md"
        
        content = "# NLM Namespaces\n\n"
        content += "The NLM API is organized into the following namespaces:\n\n"
        
        for namespace_name, namespace_data in doxygen_data["namespaces"].items():
            content += f"## {namespace_name}\n\n"
            
            if namespace_data.get("classes"):
                content += "### Classes\n\n"
                for class_name in namespace_data["classes"].keys():
                    content += f"- [`{class_name}`](API_REFERENCE.md#{class_name.lower()})\n"
                content += "\n"
            
            if namespace_data.get("functions"):
                content += "### Functions\n\n"
                for func_name in namespace_data["functions"].keys():
                    content += f"- `{func_name}`()\n"
                content += "\n"
            
            if namespace_data.get("types"):
                content += "### Type Definitions\n\n"
                for type_name in namespace_data["types"].keys():
                    content += f"- `{type_name}`\n"
                content += "\n"
        
        namespaces_file.write_text(content)
    
    def generate_examples(self):
        """Generate examples documentation"""
        examples_file = self.output_dir / "EXAMPLES.md"
        
        content = "# NLM Examples\n\n"
        content += "This document contains examples demonstrating the use of the NLM API.\n\n"
        
        # Check for example files
        examples_dir = self.source_dir.parent / "examples"
        if examples_dir.exists():
            example_files = list(examples_dir.glob("*.cpp"))
            for example_file in sorted(example_files):
                content += f"## {example_file.stemmer(3)}\n\n"
                content += "```cpp\n"
                try:
                    content += example_file.read_text() + "\n"
                except Exception:
                  content += "// Example content not available\n"
                content += "```\n\n"
        
        examples_file.write_text(content)
    
    def generate_configuration_section(self, doxygen_data: Dict[str, Any]):
        """Generate configuration documentation"""
        content = "\n## Configuration Options\n\n"
        content += "The NLM system supports extensive configuration options through the `nlm::Config` class.\n\n"
        
        # Add configuration examples and common settings
        content += "### Core Configuration\n\n"
        content += "```json\n{
    \"brain\": {
        \"maxNeurons\": 1000,
        \"learningRate\": 0.01,
        \"plasticityEnabled\": true,
        \"memory\": {
            \"workingMemory\": {
                \"enabled\": true,
                \"capacity\": 500
            },
            \"episodicMemory\": {
                \"enabled\": true,
                \"maxEpisodes\": 1000
            }
        },
        \"neuromodulation\": {
            \"dopamine\": {
                \"enabled\": true,
                \"baselineLevel\": 0.1
            }
        }
    }
}\n```\n\n"
        
        return content
    
    def format_parameters(self, parameters: List[Dict[str, str]]) -> str:
        """Format function parameters for documentation"""
        if not parameters:
            return ""
        
        param_strings = []
        for param in parameters:
            param_str = f"{param['type']} {param['name']}"
            param_strings.append(param_str)
        
        return ", ".join(param_strings)
    
    def generate_examples(self):
        """Generate example documentation"""
        # This would integrate with the existing examples directory
        pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate NLM API documentation")
    parser.add_argument("--source-dir", default="src", help="Source directory containing header files")
    parser.add_argument("--output-dir", default="docs", help="Output directory for generated documentation")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    generator = APIDocumentationGenerator(args.source_dir, args.output_dir)
    generator.generate_api_documentation()
    
    print("API documentation generation completed successfully!")
