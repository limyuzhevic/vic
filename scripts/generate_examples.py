#!/usr/bin/env python3
"""
NLM Examples Documentation Generator
Generates documentation for example programs and tutorials
"""

import argparse
import re
import os
import sys
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Any, Optional

class ExamplesDocumentationGenerator:
    def __init__(self, examples_dir: str, output_dir: str):
        self.examples_dir = Path(examples_dir)
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Example categories
        self.example_categories = {
            "basic": "Basic Examples",
            "memory": "Memory Systems",
            "neuromodulation": "Neuromodulation",
            "development": "Development",
            "prediction": "Prediction",
            "cognition": "Cognition",
            "integration": "System Integration",
            "advanced": "Advanced Usage",
            "visualization": "Visualization",
            "performance": "Performance Testing"
        }
    
    def generate_examples_documentation(self):
        """Generate examples documentation"""
        print("Generating NLM examples documentation...")
        
        # Categorize examples
        categorized_examples = self.categorize_examples()
        
        # Generate main examples documentation
        self.generate_main_examples_doc(categorized_examples)
        
        # Generate category-specific documentation
        for category, examples in categorized_examples.items():
            if examples:
                self.generate_category_doc(category, examples)
        
        # Generate example index
        self.generate_example_index(categorized_examples)
        
        # Generate code snippets for quick reference
        self.generate_code_snippets(categorized_examples)
        
        print(f"Examples documentation generated in: {self.output_dir}")
    
    def categorize_examples(self) -> Dict[str, List[Path]]:
        """Categorize examples based on their names and content"""
        categories = {category: [] for category in self.example_categories}
        
        for example_file in self.examples_dir.glob("*.cpp"):
            category = self.determine_example_category(example_file)
            categories[category].append(example_file)
        
        return categories
    
    def determine_example_category(self, example_file: Path) -> str:
        """Determine the category of an example based on its name and content"""
        filename = example_file.stemname().lower()
        
        # Category mapping based on example names
        category_mapping = {
            "ex01": "basic",
            "ex02": "memory",
            "ex03": "neuromodulation",
            "ex04": "development",
            "ex05": "prediction",
            "ex06": "cognition",
            "ex07": "integration",
            "ex08": "advanced",
            "ex09": "visualization",
            "ex10": "performance"
        }
        
        # Extract number from filename
        match = re.match(r'ex(\d+)_', filename)
        if match:
            example_num = int(match.group(1))
            if example_num <= len(category_mapping):
                return category_mapping[str(example_num)]
        
        # Fallback categorization based on content
        content = example_file.read_text()
        if "memory" in filename or "Memory" in content:
            return "memory"
        elif "neuromodulation" in filename or "Dopamine" in content:
            return "neuromodulation"
        elif "development" in filename or "development" in content:
            return "development"
        elif "prediction" in filename or "prediction" in content:
            return "prediction"
        elif "cognition" in filename or "concept" in content:
            return "cognition"
        elif "integration" in filename or "integration" in content:
            return "integration"
        elif "visualization" in filename or "visual" in content:
            return "visualization"
        elif "performance" in filename or "benchmark" in content:
            return "performance"
        
        return "basic"
    
    def generate_main_examples_doc(self, categorized_examples: Dict[str, List[Path]]):
        """Generate main examples documentation"""
        content = f"""# NLM Examples Documentation

## Overview

This document provides comprehensive examples demonstrating the use of the NLM (熙然) neural learning machine system. Each example showcases different aspects of the system, from basic functionality to advanced integration and optimization techniques.

The examples are organized into the following categories:

"""
        
        for category, examples in categorized_examples.items():
            if examples:
                content += f"- [{self.example_categories[category]}](#{category})\n"
        
        content += "\n## Quick Start Examples\n\n"
        content += "For those new to NLM, we recommend starting with the basic examples:\n\n"
        
        for example_file in categorized_examples["basic"]:
            content += self.generate_example_section(example_file)
        
        content += "\n## All Examples\n\n"
        
        for category, examples in categorized_examples.items():
            if examples:
                content += f"### {self.example_categories[category]}\n\n"
                for example_file in examples:
                    content += self.generate_example_section(example_file)
                content += "\n"
        
        examples_file = self.output_dir / "EXAMPLES.md"
        examples_file.write_text(content)
    
    def generate_example_section(self, example_file: Path) -> str:
        """Generate documentation for a single example"""
        example_name = example_file.stemname()
        content = example_file.read_text()
        
        # Extract example summary (first few lines of comments)
        summary_lines = []
        for line in content.split('\n'):
            if line.strip().startswith('//') or line.strip().startswith('/*'):
                summary_lines.append(line.strip().lstrip('/ *'))
                if len(summary_lines) >= 5:
                    break
            elif line.strip() and not line.strip().startswith('#'):
                break
        
        summary = '\n'.join(summary_lines)
        
        # Extract key functions and concepts
        key_functions = self.extract_key_functions(content)
        
        # Generate example section
        section = f"""
#### {example_name}

**File:** `{example_file.name}`

**Summary:**
{summary}

"""
        
        if key_functions:
            section += "**Key Features:**\n"
            for func in key_functions[:5]:  # Limit to 5 key functions
                section += f"- `{func}`\n"
            section += "\n"
        
        section += "**Usage:**\n\n"
        section += "```cpp\n"
        
        # Add a snippet of the example code (first 50 lines)
        lines = content.split('\n')
        snippet_lines = []
        line_count = 0
        for line in lines:
            if line_count >= 50:
                break
            snippet_lines.append(line)
            if line.strip().startswith('int main()'):
                # Include until end of main function (approximate)
                pass
            line_count += 1
        
        section += '\n'.join(snippet_lines)
        
        if len(lines) > 50:
            section += "\n// ... (truncated)\n"
        
        section += "```\n\n"
        
        return section
    
    def extract_key_functions(self, content: str) -> List[str]:
        """Extract key functions used in the example"""
        key_functions = []
        
        # Look for function calls
        func_pattern = r'(\w+)\s*\('
        for match in re.finditer(func_pattern, content):
            func_name = match.group(1)
            if func_name not in ['if', 'else', 'for', 'while', 'return', 'std', 'nlm', 'main']:
                key_functions.append(func_name)
        
        # Remove duplicates and limit
        return list(dict.fromkeys(key_functions))[:10]
    
    def generate_category_doc(self, category: str, examples: List[Path]):
        """Generate documentation for a specific category"""
        category_file = self.output_dir / f"{category}.md"
        
        content = f"# {self.example_categories[category]}\n\n"
        content += f"Examples in this category demonstrate {self.example_categories[category].lower()}.\n\n"
        
        for example_file in examples:
            content += self.generate_example_section(example_file)
        
        category_file.write_text(content)
    
    def generate_example_index(self, categorized_examples: Dict[str, List[Path]]):
        """Generate an index of all examples"""
        index_file = self.output_dir / "EXAMPLES_INDEX.md"
        
        content = "# Examples Index\n\n"
        content += "This document provides an index of all NLM examples, organized by category.\n\n"
        
        for category, examples in categorized_examples.items():
            if examples:
                content += f"## {self.example_categories[category]}\n\n"
                content += f"**Number of examples:** {len(examples)}\n\n"
                
                for example_file in examples:
                    example_name = example_file.stemname()
                    content += f"- [`{example_name}`]({category}.md)\n"
                
                content += "\n"
        
        index_file.write_text(content)
    
    def generate_code_snippets(self, categorized_examples: Dict[str, List[Path]]):
        """Generate code snippets for quick reference"""
        snippets_file = self.output_dir / "CODE_SNIPPETS.md"
        
        content = "# NLM Code Snippets\n\n"
        content += "Quick reference for commonly used NLM patterns and examples.\n\n"
        
        # Extract snippets from each example
        for category, examples in categorized_examples.items():
            if examples:
                content += f"## {self.example_categories[category]}\n\n"
                
                for example_file in examples[:2]:  # Limit to 2 examples per category
                    content += f"### {example_file.stemname()}\n\n"
                    
                    # Extract interesting code patterns
                    example_content = example_file.read_text()
                    code_blocks = self.extract_code_blocks(example_content)
                    
                    for i, code_block in enumerate(code_blocks[:3]):  # Limit to 3 code blocks
                        content += f"**Code Block {i + 1}:**\n\n"
                        content += f"```cpp\n{code_block}\n```\n\n"
                
                content += "\n"
        
        snippets_file.write_text(content)
    
    def extract_code_blocks(self, content: str) -> List[str]:
        """Extract code blocks from example content"""
        blocks = []
        
        # Split content into lines
        lines = content.split('\n')
        
        # Look for code blocks (simple heuristic)
        in_code_block = False
        code_block = []
        
        for line in lines:
            if line.strip().startswith('#include') or line.strip().startswith('//') or line.strip().startswith('/*'):
                continue
            
            if line.strip().startswith('```cpp') or (line.strip().startswith('```') and 'cpp' in line):
                in_code_block = not in_code_block
                continue
            
            if in_code_block:
                code_block.append(line)
                
                # Check if block is getting too long
                if len(code_block) > 50:
                    break
        
        if code_block:
            blocks.append('\n'.join(code_block))
        
        return blocks

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate NLM examples documentation")
    parser.add_argument("--examples-dir", default="examples", help="Examples directory")
    parser.add_argument("--output-dir", default="docs", help="Output directory for generated documentation")
    parser.add_argument("--verbose", action="store_true", help="Enable verbose output")
    
    args = parser.parse_args()
    
    generator = ExamplesDocumentationGenerator(args.examples_dir, args.output_dir)
    generator.generate_examples_documentation()
    
    print("Examples documentation generation completed successfully!")
