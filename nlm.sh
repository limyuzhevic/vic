#!/usr/bin/env bash
# Helper script for common NLM operations
# Advanced commands for power users

# Phase 6 integration demo
n6() {
    echo "=== Running NLM Phase 6 Integration Demo ==="
    ./nlm_phase6_demo
}

# Phase 5 brain-personality demo  
p5() {
    echo "=== Running NLM Phase 5 Brain-Personality Demo ==="
    ./nlm_phase5_demo
}

# Phase 4 cognition demo
p4() {
    echo "=== Running NLM Phase 4 Cognition Demo ==="
    ./nlm_phase4_demo 50
}

# Phase 3 world interaction demo
p3() {
    echo "=== Running NLM Phase 3 World Interaction Demo ==="
    ./nlm_phase3_demo 5 200
}

# Run benchmark
benchmark() {
    echo "=== Running NLM Benchmark ==="
    ./nlm_benchmark
}

# Run text/demo phase
p6text() {
    echo "=== Running NLM Phase 6 Text/Demo Demo ==="
    ./nlm_phase6_text_demo
}

# Build all targets
clean_build() {
    echo "=== Cleaning and rebuilding NLM ==="
    rm -rf build
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    cd ..
}

# Python bindings
python_build() {
    echo "=== Building Python bindings ==="
    cd build
    cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
    make -j$(nproc)
    cd ..
}

# Run all demos
run_all() {
    echo "=== Running all NLM demos ==="
    p3
    p4
    p5
    n6
    p6text
}

# Show help
help() {
    echo "NLM (Neural Learning Machine) - Available Commands:"
    echo ""
    echo "  p3      - Run Phase 3 world interaction demo"
    echo "  p4      - Run Phase 4 cognition demo"  
    echo "  p5      - Run Phase 5 brain-personality demo"
    echo "  n6      - Run Phase 6 integration demo"
    echo "  p6text  - Run Phase 6 text/teaching demo"
    echo "  benchmark - Run NLM benchmark"
    echo "  clean_build - Clean and rebuild all targets"
    echo "  python_build - Build Python bindings"
    echo "  run_all  - Run all demos"
    echo ""
    echo "  help     - Show this help message"
}

# Main command handling
case "$1" in
    "")
        help
        ;;
    "help"|*)
        help
        ;;
    "p3")
        p3 "$2" "$3"
        ;;
    "p4")
        p4 "$2"
        ;;
    "p5")
        p5
        ;;
    "n6")
        n6
        ;;
    "p6text")
        p6text
        ;;
    "benchmark")
        benchmark
        ;;
    "clean_build")
        clean_build
        ;;
    "python_build")
        python_build
        ;;
    "run_all")
        run_all
        ;;
    *)
        echo "Unknown command: $1"
        help
        ;;
esac