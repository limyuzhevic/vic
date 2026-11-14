#!/bin/bash
# Make all NLM scripts executable

# Change to the script's directory
script_dir="$(cd "$(dirname "$0")" && pwd)"
cd "$script_dir"

# Make all shell scripts executable
for script in *.sh; do
    if [[ -f "$script" ]]; then
        chmod +x "$script"
        echo "Made $script executable"
    fi
done

echo "All scripts are now executable"
