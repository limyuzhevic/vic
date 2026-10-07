#!/usr/bin/env python3
"""
Example Runner Script
=====================

This script provides a command-line interface for running NLM examples.
It supports:
- Running individual examples by name
- Running examples by category (basic, intermediate, advanced)
- Running all examples
- Comparing example outputs
- Exporting examples for documentation

Usage:
    python3 example_runner.py --list-categories
    python3 example_runner.py run basic/basic_example.py
    python3 example_runner.py run-category basic
    python3 example_runner.py run-all
    python3 example_runner.py compare basic/intermediate
"""

import argparse
import os
import sys
import importlib
import json
import time
from pathlib import Path

# Add examples directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def list_available_examples():
    """List all available examples with their paths."""
    examples = []
    
    # Walk through examples directory
    examples_dir = Path(__file__).parent
    
    for example_path in examples_dir.rglob("*.py"):
        # Skip __pycache__ and example_runner.py itself
        if "__pycache__" in str(example_path) or example_path.name == "example_runner.py":
            continue
            
        # Get relative path
        rel_path = example_path.relative_to(examples_dir)
        
        # Skip example_runner.py
        if rel_path.name == "example_runner.py":
            continue
            
        # Get example category from path
        parts = str(rel_path).split(os.sep)
        category = parts[0] if len(parts) > 0 else "unknown"
        
        examples.append({
            "path": str(rel_path),
            "category": category,
            "name": example_path.stem,
            "file": example_path.name
        })
    
    return examples

def list_categories():
    """List available example categories."""
    examples = list_available_examples()
    categories = set(ex["category"] for ex in examples)
    
    print("Available Categories:")
    print("=" * 50)
    for category in sorted(categories):
        category_examples = [ex for ex in examples if ex["category"] == category]
        print(f"\n{category.upper()} ({len(category_examples)} examples):")
        for ex in category_examples:
            print(f"  - {ex['name']}.py")

def run_example(example_path, verbose=False):
    """Run a specific example."""
    print(f"\nRunning example: {example_path}")
    print("=" * 50)
    
    try:
        # Import the module
        module_name = example_path.replace(".py", "").replace("/", ".")
        spec = importlib.util.spec_from_file_location(module_name, example_path)
        
        if spec is None:
            print(f"❌ Failed to load example: {example_path}")
            return False
        
        module = importlib.util.module_from_spec(spec)
        
        # Execute the module
        start_time = time.time()
        spec.loader.exec_module(module)
        end_time = time.time()
        
        execution_time = end_time - start_time
        
        print(f"\n✅ Example completed successfully!")
        print(f"   Execution time: {execution_time:.2f} seconds")
        
        return True
        
    except Exception as e:
        print(f"\n❌ Error running example: {e}")
        import traceback
        traceback.print_exc()
        return False

def run_category(category, verbose=False):
    """Run all examples in a category."""
    print(f"\nRunning all examples in category: {category}")
    print("=" * 50)
    
    examples = list_available_examples()
    category_examples = [ex for ex in examples if ex["category"] == category]
    
    if not category_examples:
        print(f"❌ No examples found in category: {category}")
        return False
    
    results = []
    for example in category_examples:
        print(f"\n[{len(results) + 1}/{len(category_examples)}] Running {example['name']}...")
        success = run_example(example["path"], verbose)
        results.append({
            "name": example["name"],
            "category": example["category"],
            "success": success,
            "path": example["path"]
        })
    
    # Summary
    successful = sum(1 for r in results if r["success"])
    total = len(results)
    
    print(f"\n" + "=" * 50)
    print(f"CATEGORY SUMMARY: {category}")
    print(f"Total examples: {total}")
    print(f"Successful: {successful}")
    print(f"Failed: {total - successful}")
    
    return successful == total

def run_all(verbose=False):
    """Run all available examples."""
    print("\nRunning all available examples")
    print("=" * 50)
    
    examples = list_available_examples()
    
    if not examples:
        print("❌ No examples found!")
        return False
    
    results = []
    for i, example in enumerate(examples):
        print(f"\n[{i+1}/{len(examples)}] Running {example['name']}...")
        success = run_example(example["path"], verbose)
        results.append({
            "name": example["name"],
            "category": example["category"],
            "success": success,
            "path": example["path"]
        })
    
    # Summary by category
    categories = {}
    for result in results:
        category = result["category"]
        if category not in categories:
            categories[category] = {"total": 0, "success": 0}
        categories[category]["total"] += 1
        if result["success"]:
            categories[category]["success"] += 1
    
    print(f"\n" + "=" * 50)
    print("OVERALL SUMMARY")
    print("=" * 50)
    
    total_success = sum(1 for r in results if r["success"])
    total_failed = len(results) - total_success
    
    print(f"Total examples: {len(results)}")
    print(f"Successful: {total_success}")
    print(f"Failed: {total_failed}")
    
    print(f"\nBy Category:")
    for category, stats in sorted(categories.items()):
        success_rate = (stats["success"] / stats["total"]) * 100
        print(f"  {category}: {stats['success']}/{stats['total']} ({success_rate:.1f}%)")
    
    return total_failed == 0

def export_examples(output_format="json"):
    """Export example metadata."""
    print(f"\nExporting example metadata as {output_format.upper()}...")
    
    examples = list_available_examples()
    
    if output_format == "json":
        export_data = {
            "examples": examples,
            "metadata": {
                "total_examples": len(examples),
                "categories": list(set(ex["category"] for ex in examples)),
                "export_timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
            }
        }
        
        filename = "examples_metadata.json"
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        print(f"✅ Metadata exported to: {filename}")
        
    elif output_format == "csv":
        filename = "examples_metadata.csv"
        with open(filename, 'w') as f:
            f.write("Category,Name,Path\n")
            for example in examples:
                f.write(f"{example['category']},{example['name']},{example['path']}\n")
        
        print(f"✅ Metadata exported to: {filename}")
    
    return True

def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description="NLM Example Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # List categories command
    parser_list = subparsers.add_parser(
        "list-categories",
        help="List available example categories"
    )
    
    # Run example command
    parser_run = subparsers.add_parser(
        "run",
        help="Run a specific example"
    )
    parser_run.add_argument(
        "example_path",
        help="Path to the example file (e.g., basic/basic_example.py)"
    )
    parser_run.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    # Run category command
    parser_run_cat = subparsers.add_parser(
        "run-category",
        help="Run all examples in a category"
    )
    parser_run_cat.add_argument(
        "category",
        help="Category name (basic, intermediate, advanced)"
    )
    parser_run_cat.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    # Run all command
    parser_run_all = subparsers.add_parser(
        "run-all",
        help="Run all available examples"
    )
    parser_run_all.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    # Export command
    parser_export = subparsers.add_parser(
        "export",
        help="Export example metadata"
    )
    parser_export.add_argument(
        "format",
        choices=["json", "csv"],
        default="json",
        help="Export format (default: json)"
    )
    
    # Compare command
    parser_compare = subparsers.add_parser(
        "compare",
        help="Compare examples (placeholder)"
    )
    parser_compare.add_argument(
        "categories",
        nargs="+",
        help="Categories to compare"
    )
    
    args = parser.parse_args()
    
    if args.command == "list-categories":
        list_categories()
    
    elif args.command == "run":
        run_example(args.example_path, args.verbose)
    
    elif args.command == "run-category":
        run_category(args.category, args.verbose)
    
    elif args.command == "run-all":
        run_all(args.verbose)
    
    elif args.command == "export":
        export_examples(args.format)
    
    elif args.command == "compare":
        print("Compare functionality is not yet implemented")
        return False
    
    else:
        parser.print_help()
        return False
    
    return True

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)