#!/usr/bin/env bash

# Documentation Generation Script
# Generates comprehensive documentation for NLM advanced features

# Usage: ./documentation_generation.sh [options]
# Options:
#   --format FORMAT        Output format (markdown, html, json, latex)
#   --output DIR           Output directory
#   --feature FEATURE      Generate documentation for specific feature
#   --all                  Generate documentation for all features
#   --source-dir DIR       Source directory to extract documentation from
#   --template-file FILE   Template file for documentation generation
#   --exclude-pattern PATTERN   Exclude files matching pattern
#   --include-pattern PATTERN   Include only files matching pattern
#   --verbose              Enable verbose output
#   --help                 Show this help message

cd "$(dirname "$0")"

SCRIPT_DIR="$(pwd)"
PROJECT_ROOT="../../.."
DEFAULT_FORMAT="markdown"
DEFAULT_OUTPUT="../../docs/generated"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log() {
    echo -e "[$(date '+%Y-%m-%d %H:%M:%S')] $*" | tee -a "$LOG_FILE"
}

error_exit() {
    echo -e "${RED}[ERROR] $*${NC}" | tee -a "$LOG_FILE"
    exit 1
}

check_command() {
    if ! command -v $1 &> /dev/null; then
        error_exit "$1 is required but not installed"
    fi
}

generate_markdown_documentation() {
    local source_dir=$1
    local output_dir=$2
    local feature_filter=$3
    
    mkdir -p "$output_dir"
    
    # Find all documentation files
    local doc_files
    if [ -n "$feature_filter" ]; then
        doc_files=$(find "$source_dir" -name "*$feature_filter*" -type f 2>/dev/null)
    else
        doc_files=$(find "$source_dir" -name "*.md" -o -name "*.hpp" -o -name "*.cpp" -o -name "*.h" -o -name "*.c" -type f 2>/dev/null)
    fi
    
    if [ -z "$doc_files" ]; then
        error_exit "No documentation files found in $source_dir"
    fi
    
    # Generate main documentation index
    generate_documentation_index "$output_dir" "$source_dir" "$feature_filter"
    
    # Process each documentation file
    for doc_file in $doc_files; do
        generate_single_document "$doc_file" "$output_dir"
    done
}

generate_single_document() {
    local doc_file=$1
    local output_dir=$2
    
    local filename=$(basename "$doc_file")
    local extension="${filename##*.}"
    local name="${filename%.*}"
    
    # Determine if file is a source code file or documentation
    if [[ "$doc_file" == *.md ]]; then
        # For markdown files, copy as-is
        cp "$doc_file" "$output_dir/${name}.md"
        log "Processed markdown document: $filename -> ${name}.md"
    elif [[ "$doc_file" == *.hpp || "$doc_file" == *.h ]]; then
        # For header files, extract documentation comments and generate markdown
        extract_header_documentation "$doc_file" "$output_dir/${name}.md"
    elif [[ "$doc_file" == *.cpp || "$doc_file" == *.cc || "$doc_file" == *.cxx ]]; then
        # For source files, extract documentation and generate markdown
        extract_source_documentation "$doc_file" "$output_dir/${name}.md"
    else
        # Skip other files
        continue
    fi
}

extract_header_documentation() {
    local header_file=$1
    local output_file=$2
    
    # Extract documentation comments and generate markdown
    python3 << EOF
import re
import sys

header_file = "$header_file"
output_file = "$output_file"

with open(header_file, 'r') as f:
    content = f.read()

# Extract class documentation
class_pattern = r'class\s+(\w+)\s*:\s*//\s*(.*?)(?=\nclass|\nnamespace|\n//|\\Z)'
class_matches = re.findall(class_pattern, content, re.DOTALL)

# Extract struct documentation
struct_pattern = r'struct\s+(\w+)\s*:\s*//\s*(.*?)(?=\nclass|\nstruct|\nnamespace|\n//|\\Z)'
struct_matches = re.findall(struct_pattern, content, re.DOTALL)

# Extract enum documentation
enum_pattern = r'enum\s+class\s+(\w+)\s*:\s*//\s*(.*?)(?=\nenum|\nstruct|\nclass|\nnamespace|\n//|\\Z)'
enum_matches = re.findall(enum_pattern, content, re.DOTALL)

# Generate markdown
markdown = "# " + header_file.split('/')[-1] + "\n\n"
markdown += "Generated documentation from header file.\n\n"

if class_matches:
    markdown += "## Classes\n\n"
    for class_name, class_doc in class_matches:
        markdown += "### " + class_name + "\n\n"
        markdown += class_doc + "\n\n"

if struct_matches:
    markdown += "## Structs\n\n"
    for struct_name, struct_doc in struct_matches:
        markdown += "### " + struct_name + "\n\n"
        markdown += struct_doc + "\n\n"

if enum_matches:
    markdown += "## Enums\n\n"
    for enum_name, enum_doc in enum_matches:
        markdown += "### " + enum_name + "\n\n"
        markdown += enum_doc + "\n\n"

with open(output_file, 'w') as f:
    f.write(markdown)

print(f"Generated documentation for {header_file}")
EOF
}

extract_source_documentation() {
    local source_file=$1
    local output_file=$2
    
    # Extract documentation comments and generate markdown
    python3 << EOF
import re
import sys

header_file = "$header_file"
output_file = "$output_file"

with open(header_file, 'r') as f:
    content = f.read()

# Extract function documentation
function_pattern = r'(\w+\s+\w+\s*\([^)]*\)\s*//\s*(.*?)(?=\n\w+\s+\w+\s*\([^)]*\)\s*//|\n\w+\s*\([^)]*\)\s*//|\nclass|\nstruct|\nnamespace|\n//|\\Z)'
function_matches = re.findall(function_pattern, content, re.DOTALL)

# Extract class member documentation
member_pattern = r'(\w+\s+\w+;|\w+\s+\w+::\w+\s*\(.*\);)\s*//\s*(.*?)(?=\n\w+\s+\w+;|\n\w+\s*\(.*\);|\nclass|\nstruct|\nnamespace|\n//|\\Z)'
member_matches = re.findall(member_pattern, content, re.DOTALL)

# Generate markdown
markdown = "# " + source_file.split('/')[-1] + "\n\n"
markdown += "Generated documentation from source file.\n\n"

if function_matches:
    markdown += "## Functions\n\n"
    for func_signature, func_doc in function_matches:
        markdown += "### " + func_signature + "\n\n"
        markdown += func_doc + "\n\n"

if member_matches:
    markdown += "## Class Members\n\n"
    for member_signature, member_doc in member_matches:
        markdown += "### " + member_signature + "\n\n"
        markdown += member_doc + "\n\n"

with open(output_file, 'w') as f:
    f.write(markdown)

print(f"Generated documentation for {source_file}")
EOF
}

generate_documentation_index() {
    local output_dir=$1
    local source_dir=$2
    local feature_filter=$3
    
    local index_file="$output_dir/INDEX.md"
    
    # Collect all documentation files
    local doc_files=$(find "$source_dir" -name "*.md" -type f 2>/dev/null)
    
    # Generate index
    cat > "$index_file" << EOF
# NLM Advanced Features Documentation

Generated documentation for NLM (Neural Learning Machine) advanced features.

## Overview

This documentation covers the advanced features of the NLM system:

1. **Plugin System** - Dynamic plugin loading and management
2. **Advanced Configuration** - Extended configuration system with templates and inheritance
3. **Performance Profiling** - Comprehensive performance monitoring and optimization
4. **Visualization API** - Advanced visualization and debugging tools
5. **Memory Management API** - Sophisticated memory control and leak detection

## Generated Documentation

The following documentation files have been generated:

EOF
    
    for doc_file in $doc_files; do
        local filename=$(basename "$doc_file")
        local name="${filename%.*}"
        echo "- [$name](./$name.md)" >> "$index_file"
    done
    
    cat >> "$index_file" << EOF

## Documentation Structure

Each feature has its own documentation:

- **Plugin Development**: Documentation for developing and using plugins
- **Advanced Configuration**: Guide to advanced configuration management
- **Performance Profiling**: Guide to performance monitoring and optimization
- **Visualization**: Guide to visualization and debugging capabilities
- **Memory Management**: Guide to memory pool management and leak detection

## Usage

To use this documentation:

1. Navigate to the output directory
2. Read the INDEX.md file for an overview
3. Click on feature links to access specific documentation
4. Use the navigation links at the top of each document

## Generated On

Generated on: $(date)
Generated by: Documentation Generation Script

EOF
}

show_help() {
    cat << EOF
Documentation Generation Script
===============================

Generate comprehensive documentation for NLM advanced features.

USAGE:
  $0 [OPTIONS]

OPTIONS:
  --format FORMAT            Output format (markdown, html, json, latex). Default: markdown
  --output DIR              Output directory. Default: ../../docs/generated
  --feature FEATURE          Generate documentation for specific feature
  --all                     Generate documentation for all features
  --source-dir DIR           Source directory to extract documentation from. Default: ../../src/agent
  --template-file FILE       Template file for documentation generation
  --exclude-pattern PATTERN  Exclude files matching pattern
  --include-pattern PATTERN  Include only files matching pattern
  --verbose                  Enable verbose output
  --help                     Show this help message

FEATURES:
  plugin_system             Generate documentation for plugin system
  advanced_config           Generate documentation for advanced configuration
  advanced_performance       Generate documentation for performance profiling
  visualization_api         Generate documentation for visualization API
  memory_management          Generate documentation for memory management

EXAMPLES:
  $0 --format markdown --output ./docs --feature plugin_system
  $0 --format markdown --output ./docs --feature advanced_config
  $0 --format markdown --output ./docs --all
  $0 --format html --output ./docs --feature advanced_performance

LOG FILES:
  Documentation generation log: documentation_generation.log

The script extracts documentation from source code files and generates comprehensive documentation in the specified format.
EOF
}

# Main execution
main() {
    # Set default options
    local format="$DEFAULT_FORMAT"
    local output_dir="$DEFAULT_OUTPUT"
    local feature_filter=""
    local source_dir="$PROJECT_ROOT/src/agent"
    local verbose=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --format)
                format="$2"
                shift 2
                ;;
            --output)
                output_dir="$2"
                shift 2
                ;;
            --feature)
                feature_filter="$2"
                shift 2
                ;;
            --all)
                feature_filter=""
                shift 1
                ;;
            --source-dir)
                source_dir="$2"
                shift 2
                ;;
            --verbose)
                verbose=true
                shift 1
                ;;
            --help)
                show_help
                exit 0
                ;;
            *)
                echo "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Set up logging
    LOG_FILE="documentation_generation_$(date +%Y%m%d_%H%M%S).log"
    
    echo "Starting documentation generation..."
    echo "Format: $format"
    echo "Output directory: $output_dir"
    echo "Source directory: $source_dir"
    if [ -n "$feature_filter" ]; then
        echo "Feature filter: $feature_filter"
    fi
    echo "Log file: $LOG_FILE"
    
    # Validate format
    case "$format" in
        markdown|html|json|latex)
            echo "Format: $format (valid)"
            ;;
        *)
            echo "Invalid format: $format"
            echo "Supported formats: markdown, html, json, latex"
            exit 1
            ;;
    esac
    
    # Create output directory
    mkdir -p "$output_dir"
    
    # Generate documentation
    case "$format" in
        markdown)
            generate_markdown_documentation "$source_dir" "$output_dir" "$feature_filter"
            ;;
        html)
            echo "HTML generation not yet implemented"
            error_exit "HTML format not yet implemented"
            ;;
        json)
            echo "JSON generation not yet implemented"
            error_exit "JSON format not yet implemented"
            ;;
        latex)
            echo "LaTeX generation not yet implemented"
            error_exit "LaTeX format not yet implemented"
            ;;
    esac
    
    echo "Documentation generation completed successfully!"
    echo "Documentation generated in: $output_dir"
    echo "Main index: $output_dir/INDEX.md"
}

# Run main function
main "$@"