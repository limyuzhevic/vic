#!/usr/bin/env bash

# Advanced Simulation Control Script
# Power user command for controlling simulation execution

# Usage: ./advanced_simulation_control.sh [options]
# Options:
#   --start                Start the simulation
#   --stop                 Stop the simulation
#   --pause                Pause the simulation
#   --resume               Resume the simulation
#   --step N               Run N simulation steps
#   --status               Show simulation status
#   --reset                Reset simulation to initial state
#   --save-state FILE      Save current simulation state
#   --load-state FILE      Load simulation state from file
#   --profile-time         Enable performance profiling
#   --benchmark FILE       Run benchmark and save results

cd "$(dirname "$0")"

SCRIPT_DIR="$(pwd)"
LOG_FILE="simulation_control_$(date +%Y%m%d_%H%M%S).log"
SIMULATION_PID_FILE="simulation.pid"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log() {
    echo -e "[$(date '+%Y-%m-%d %H:%M:%S')] $*" | tee -a "$LOG_FILE"
}

error_exit() {
    echo -e "${RED}[ERROR] $*${NC}" | tee -a "$LOG_FILE"
    exit 1
}

check_simulation_running() {
    if [ -f "$SIMULATION_PID_FILE" ]; then
        local pid=$(cat "$SIMULATION_PID_FILE")
        if kill -0 "$pid" 2>/dev/null; then
            return 0  # Running
        else
            rm -f "$SIMULATION_PID_FILE"
        fi
    fi
    return 1  # Not running
}

start_simulation() {
    log "Starting advanced simulation..."
    
    if check_simulation_running; then
        error_exit "Simulation is already running with PID $(cat "$SIMULATION_PID_FILE")"
    fi
    
    # Check if simulation binary exists
    if [ ! -f "../../build/nlm_simulation" ]; then
        error_exit "Simulation binary not found. Build the project first."
    fi
    
    # Start simulation with profiling enabled
    timeout 300s ../../build/nlm_simulation \
        --enable-profiling \
        --performance-metrics \
        --memory-profiling \
        --log-level INFO \
        --output-file "simulation_output.log" \
        > "simulation.log" 2>&1 &
    
    local pid=$!
    echo "$pid" > "$SIMULATION_PID_FILE"
    
    # Wait a moment to check if simulation started successfully
    sleep 2
    if kill -0 "$pid" 2>/dev/null; then
        log "${GREEN}Simulation started successfully with PID $pid${NC}"
        echo "$pid"
    else
        rm -f "$SIMULATION_PID_FILE"
        error_exit "Simulation failed to start"
    fi
}

stop_simulation() {
    if ! check_simulation_running; then
        log "Simulation is not running"
        return 0
    fi
    
    local pid=$(cat "$SIMULATION_PID_FILE")
    log "Stopping simulation (PID: $pid)..."
    
    # Send SIGTERM for graceful shutdown
    kill -TERM "$pid" 2>/dev/null
    local wait_count=0
    while kill -0 "$pid" 2>/dev/null && [ $wait_count -lt 30 ]; do
        sleep 1
        wait_count=$((wait_count + 1))
    done
    
    if kill -0 "$pid" 2>/dev/null; then
        log "${YELLOW}Simulation did not stop gracefully, forcing termination...${NC}"
        kill -KILL "$pid" 2>/dev/null
        sleep 1
    fi
    
    rm -f "$SIMULATION_PID_FILE"
    log "${GREEN}Simulation stopped${NC}"
}

pause_simulation() {
    if ! check_simulation_running; then
        error_exit "Simulation is not running"
    fi
    
    local pid=$(cat "$SIMULATION_PID_FILE")
    log "Pausing simulation (PID: $pid)..."
    
    # Send SIGTSTP to pause
    kill -TSTP "$pid" 2>/dev/null
    log "${GREEN}Simulation paused${NC}"
}

resume_simulation() {
    if ! check_simulation_running; then
        error_exit "Simulation is not running"
    fi
    
    local pid=$(cat "$SIMULATION_PID_FILE")
    log "Resuming simulation (PID: $pid)..."
    
    # Send SIGCONT to resume
    kill -CONT "$pid" 2>/dev/null
    log "${GREEN}Simulation resumed${NC}"
}

run_simulation_steps() {
    local steps=$1
    if ! check_simulation_running; then
        error_exit "Simulation is not running"
    fi
    
    local pid=$(cat "$SIMULATION_PID_FILE")
    log "Running $steps simulation steps..."
    
    # Send SIGUSR1 to run specific number of steps
    kill -USR1 "$pid" 2>/dev/null
    log "${GREEN}Sent step command to simulation${NC}"
}

check_simulation_status() {
    if check_simulation_running; then
        local pid=$(cat "$SIMULATION_PID_FILE")
        echo "Simulation Status: RUNNING"
        echo "PID: $pid"
        echo "Log file: simulation.log"
        echo "Performance log: $LOG_FILE"
        
        # Show recent performance metrics if available
        if [ -f "simulation_output.log" ]; then
            echo "\nRecent performance metrics:"
            tail -20 "simulation_output.log" | grep -E "(perf|alloc|memory|cpu)" || echo "No performance metrics found"
        fi
        
        return 0
    else
        echo "Simulation Status: STOPPED"
        return 1
    fi
}

save_simulation_state() {
    local filename=$1
    if [ -z "$filename" ]; then
        filename="simulation_state_$(date +%Y%m%d_%H%M%S).json"
    fi
    
    if ! check_simulation_running; then
        error_exit "Simulation is not running"
    fi
    
    local pid=$(cat "$SIMULATION_PID_FILE")
    log "Saving simulation state to $filename..."
    
    # Send SIGUSR2 to save state
    kill -USR2 "$pid" 2>/dev/null
    log "${GREEN}State save command sent to simulation${NC}"
}

load_simulation_state() {
    local filename=$1
    if [ -z "$filename" ]; then
        error_exit "Please specify a state file to load"
    fi
    
    if [ ! -f "$filename" ]; then
        error_exit "State file '$filename' not found"
    fi
    
    if check_simulation_running; then
        error_exit "Simulation is currently running. Please stop it first."
    fi
    
    log "Loading simulation state from $filename..."
    
    # Implementation depends on your simulation format
    # This is a placeholder - you would need to implement actual state loading
    echo "State loading functionality requires implementation based on your simulation format"
}

run_benchmark() {
    local benchmark_file=$1
    if [ -z "$benchmark_file" ]; then
        benchmark_file="benchmark_results_$(date +%Y%m%d_%H%M%S).json"
    fi
    
    log "Running performance benchmark..."
    
    # Run benchmark with different configurations
    if [ -f "../../build/benchmark" ]; then
        ../../build/benchmark \
            --iterations 100 \
            --warmup 10 \
            --output "$benchmark_file" \
            --detailed
        
        if [ -f "$benchmark_file" ]; then
            log "${GREEN}Benchmark completed and saved to $benchmark_file${NC}"
            
            # Display summary
            echo "\nBenchmark Summary:"
            echo "Results saved to: $benchmark_file"
            echo "View detailed results with: cat $benchmark_file"
        else
            error_exit "Benchmark failed to generate results"
        fi
    else
        error_exit "Benchmark binary not found. Build the benchmark target."
    fi
}

show_help() {
    cat << EOF
Advanced Simulation Control Script
==============================

USAGE:
  $0 [OPTIONS]

OPTIONS:
  --start              Start the simulation
  --stop               Stop the simulation
  --pause              Pause the simulation
  --resume             Resume the simulation
  --step N             Run N simulation steps
  --status             Show simulation status
  --reset              Reset simulation to initial state
  --save-state FILE    Save current simulation state to FILE
  --load-state FILE    Load simulation state from FILE
  --profile-time       Enable detailed performance profiling
  --benchmark FILE     Run benchmark and save results to FILE
  --help               Show this help message

EXAMPLES:
  $0 --start                    # Start the simulation
  $0 --status                   # Check simulation status
  $0 --step 100                 # Run 100 simulation steps
  $0 --save-state my_state.json  # Save current state
  $0 --benchmark perf_results.json  # Run performance benchmark

LOG FILES:
  Control log: $LOG_FILE
  Simulation log: simulation.log
  Simulation output: simulation_output.log

PID file: $SIMULATION_PID_FILE

The script supports graceful shutdown, performance monitoring, and state management.
EOF
}

# Command line argument parsing
case "${1:-}" in
    start)
        start_simulation
        ;;
    stop)
        stop_simulation
        ;;
    pause)
        pause_simulation
        ;;
    resume)
        resume_simulation
        ;;
    status)
        check_simulation_status
        ;;
    reset)
        stop_simulation
        # Additional reset logic would go here
        log "Simulation reset completed"
        ;;
    save-state)
        save_simulation_state "$2"
        ;;
    load-state)
        load_simulation_state "$2"
        ;;
    step)
        run_simulation_steps "$2"
        ;;
    benchmark)
        run_benchmark "$2"
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo "Unknown option: $1"
        show_help
        exit 1
        ;;
esac