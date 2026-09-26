# NLM Visualization CLI Tool

This directory contains the command-line visualization tool for the Neural Learning Model (NLM) system.

## Overview

The NLM Visualization CLI Tool provides command-line visualization capabilities for the Neural Learning Model (NLM) system. It allows users to generate visualizations, analyze data, and export results via the command line.

## Files

### cli_visualization_tool.py
Main CLI tool script.

## Usage

### Initialize the tool

```bash
python cli_visualization_tool.py init
```

### Generate visualization data

```bash
python cli_visualization_tool.py visualize output.json [--format json]
```

Options:
- `--format FORMAT`: Output format (default: json)
- `--update-frequency FREQ`: Update frequency in Hz (default: 60)
- `--theme THEME`: Theme (default: default)
- `--quality QUALITY`: Quality (default: high)

### Analyze performance data

```bash
python cli_visualization_tool.py analyze performance perf_data.json perf_report.txt
```

### Analyze learning data

```bash
python cli_visualization_tool.py analyze learning learn_data.json learn_report.txt
```

### Analyze network data

```bash
python cli_visualization_tool.py analyze network network_data.json network_report.txt
```

### Analyze system data

```bash
python cli_visualization_tool.py analyze system system_data.json system_report.txt
```

### Create a visualization template

```bash
python cli_visualization_tool.py create template "My Template" "A custom template" template_data.json my_template.json
```

### Load a visualization template

```bash
python cli_visualization_tool.py load template my_template.json
```

### Save a visualization template

```bash
python cli_visualization_tool.py save template my_template.json saved_template.json
```

### List templates

```bash
python cli_visualization_tool.py list templates
```

### Help

```bash
python cli_visualization_tool.py help
```

## Dependencies

- Python 3.7 or higher
- NLM visualization and analysis modules

## Build Instructions

1. Clone this repository
2. Navigate to the cli_visualization directory
3. Ensure the Python path is set correctly
4. Run the CLI tool with the appropriate command

## Configuration

The CLI visualization tool can be configured by modifying the configuration files in the configs directory.

## Examples

See the examples directory for example usage of the CLI visualization tool.

## Support

For support with the CLI visualization tool, please visit the NLM documentation or contact support.
