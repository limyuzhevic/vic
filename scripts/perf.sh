#!/bin/bash

# NLM Performance Monitoring Script
# Performance monitoring and statistics collection for NLM

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'
BLUE='\033[0;34m'

function print_performance_help() {
    echo "NLM Performance Monitoring Script"
    echo "================================"
    echo ""
    echo "Usage: ./perf.sh [command] [options]"
    echo ""
    echo "Commands:"
    echo "  stats           Show current performance statistics"
    echo "  monitor         Monitor performance in real-time"
    echo "  benchmark       Run performance benchmark"
    echo "  spike-monitor   Monitor spike activity"
    echo "  memory-monitor  Monitor memory usage"
    echo "  help           Show this help"
    echo ""
    echo "Options for monitor command:"
    echo "  --interval N    Monitoring interval in seconds (default: 1)"
    echo "  --duration T    Monitoring duration in seconds (default: 60)"
    echo "  --spikes        Include spike count monitoring"
    echo "  --neurons       Include neuron activity monitoring"
    echo "  --memory        Include memory usage monitoring"
    echo ""
    echo "Examples:"
    echo "  ./perf.sh stats                     # Show current stats"
    echo "  ./perf.sh monitor --interval 5      # Monitor every 5 seconds"
    echo "  ./perf.sh benchmark                 # Run performance benchmark"
    echo "  ./perf.sh spike-monitor            # Monitor spike activity"
}

function show_stats() {
    echo "${YELLOW}=== NLM Performance Statistics ===${NC}"
    
    if [ -f "build/nlm" ] && [ -x "build/nlm" ]; then
        echo "${BLUE}Executable available:${NC}"
        echo "  ./nlm - Phase 2 demo"
        echo "  ./nlm_phase3_demo - Phase 3 demo"
        echo "  ./nlm_phase4_demo - Phase 4 demo"
        echo "  ./nlm_phase5_demo - Phase 5 demo"
        echo "  ./nlm_phase6_demo - Phase 6 integration demo"
        echo "  ./nlm_benchmark - Performance benchmark"
    else
        echo "${RED}Build directory not found. Run './dev.sh build' first.${NC}"
    fi
    
    if [ -f "configs/default.cfg" ]; then
        echo "${BLUE}Configuration:${NC}"
        echo "  Config file: configs/default.cfg"
        echo "  Last modified: $(stat -c %y configs/default.cfg 2>/dev/null || echo 'Unknown')"
    fi
    
    echo "${BLUE}System Information:${NC}"
    echo "  Platform: Linux $(uname -r)"
    echo "  Cores: $(nproc)"
    echo "  Available memory: $(free -h | grep 'Mem:' | awk '{print $4}')"
    
    echo "${BLUE}Development Tools:${NC}"
    if [ -f "scripts/dev.sh" ] && [ -x "scripts/dev.sh" ]; then
        echo "  Development script: scripts/dev.sh"
        echo "  Profile script: scripts/profile.sh"
        echo "  Performance script: scripts/perf.sh"
    else
        echo "  Scripts may not be executable. Run 'chmod +x scripts/*'"
    fi
}

function monitor_performance() {
    local interval=${1:-1}
    local duration=${2:-60}
    local include_spikes=${3:-false}
    local include_neurons=${4:-false}
    local include_memory=${5:-false}
    
    echo "${YELLOW}Starting performance monitoring...${NC}"
    echo "Duration: ${duration}s, Interval: ${interval}s"
    
    if [ "$include_spikes" = "true" ]; then
        echo "${BLUE}Spike monitoring enabled${NC}"
    fi
    if [ "$include_neurons" = "true" ]; then
        echo "${BLUE}Neuron monitoring enabled${NC}"
    fi
    if [ "$include_memory" = "true" ]; then
        echo "${BLUE}Memory monitoring enabled${NC}"
    fi
    
    local start_time=$(date +%s)
    local end_time=$((start_time + duration))
    local current_time=$start_time
    
    while [ $current_time -lt $end_time ]; do
        echo "[${current_time}s] Monitoring data..."
        
        if [ "$include_spikes" = "true" ]; then
            echo "  Spikes: [monitoring...]".
            ${BLUE}CPU: $(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | cut -d'%' -f1)%
            echo "  Memory: $(free -h | grep 'Mem:' | awk '{print $3}')/$(free -h | grep 'Mem:' | awk '{print $2}') (${YELLOW}$(free -h | grep 'Mem:' | awk '{print int($3/$2 * 100)}')%${NC})
        fi
        
        sleep $interval
        current_time=$(date +%s)
        
        if [ $current_time -ge $end_time ]; then
            break
        fi
    done
    
    echo "${GREEN}Monitoring complete.${NC}"
}

function run_benchmark() {
    echo "${YELLOW}Running NLM Performance Benchmark...${NC}"
    
    if [ ! -f "build/nlm_benchmark" ] || [ ! -x "build/nlm_benchmark" ]; then
        echo "${RED}Benchmark executable not found. Run './dev.sh build' first.${NC}"
        return
    fi
    
    echo "${BLUE}Running benchmark...${NC}"
    ./build/nlm_benchmark
}

function monitor_spikes() {
    echo "${YELLOW}Spike Activity Monitor${NC}"
    echo "This monitor tracks neural spike activity during execution."
    echo "Note: This requires a running NLM simulation to provide spike data."
}

function monitor_memory() {
    echo "${YELLOW}Memory Usage Monitor${NC}"
    echo "This monitor tracks memory usage during execution."
    echo "Note: This requires system access to monitor memory usage."
}

function main() {
    if [ $# -eq 0 ]; then
        print_performance_help
        return
    fi
    
    command=$1
    shift
    
    case $command in
        stats)
            show_stats
            ;;
        monitor)
            monitor_performance "$@"
            ;;
        benchmark)
            run_benchmark
            ;;
        spike-monitor)
            monitor_spikes
            ;;
        memory-monitor)
            monitor_memory
            ;;
        help)
            print_performance_help
            ;;
        *)
            echo "Unknown command: $command"
            print_performance_help
            exit 1
            ;;
    esac
}

main "$@"