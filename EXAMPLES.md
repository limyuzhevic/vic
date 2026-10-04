# Use the command-line interface to run a simple brain simulation
# This demonstrates the new CLI functionality

# Show help
./nlm_cli_exec --help

# Show version
./nlm_cli_exec --version

# Create a new brain with 500 neurons
./nlm_cli_exec new --neurons 500 --regions 2

# Run a 2000-step simulation
./nlm_cli_exec run --steps 2000

# Get brain status
./nlm_cli_exec status

# Save brain state
./nlm_cli_exec save /tmp/brain_state.bin

# Load brain state
./nlm_cli_exec load /tmp/brain_state.bin

# Export brain data
./nlm_cli_exec export txt /tmp/brain_data.txt

# Interactive mode
./nlm_cli_exec interactive
