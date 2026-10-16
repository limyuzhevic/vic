# NLM Docker Development Environment
# Multi-stage Docker setup for building, testing, and deploying NLM

FROM ubuntu:22.04 AS base

LABEL maintainer="NLM Team"
LABEL description="Docker environment for NLM (Neural Learning Machine)"
LABEL version="1.0"

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV NLM_VERSION=1.0.0
ENV NLM_HOME=/opt/nlm
ENV PATH=$NLM_HOME/bin:$PATH
ENV LD_LIBRARY_PATH=$NLM_HOME/lib:$LD_LIBRARY_PATH

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    curl \
    wget \
    sudo \
    unzip \
    zip \
    python3 \
    python3-pip \
    python3-dev \
    libgoogle-glog-dev \
    libgtest-dev \
    libbenchmark-dev \
    libjsoncpp-dev \
    libcurl4-openssl-dev \
    libssl-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
RUN pip3 install --no-cache-dir \
    numpy \
    scipy \
    matplotlib \
    pytest \
    pytest-cov \
    coverage \
    black \
    isort \
    flake8 \
    mypy \
    sphinx \
    breathe \
    plantuml \
    pyyaml

# Create NLM directory structure
RUN mkdir -p $NLM_HOME/{src,tests,build,docs,benchmarks,scripts,configs}
WORKDIR $NLM_HOME

# Copy source code
COPY . $NLM_HOME/src/

FROM base AS development

# Install development tools and IDE
RUN apt-get update && apt-get install -y \
    vim \
    nano \
    tmux \
    htop \
    iotop \
    git-extras \
    && rm -rf /var/lib/apt/lists/*

# Install Rust and cargo (for some tools)
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH=/root/.cargo/bin:$PATH

# Clone and install tools
RUN git clone https://github.com/google/bazel.git && cd bazel && ./tools/ci/build.sh

# Create development scripts
RUN cat > $NLM_HOME/scripts/dev.sh << 'EOF'
#!/bin/bash
# Development environment script for NLM

set -e

function build() {
    local build_type="${1:-Release}"
    mkdir -p build/${build_type}
    cd build/${build_type}
    cmake -DCMAKE_BUILD_TYPE=${build_type} ../..
    cmake --build . --parallel $(nproc)
}

function test() {
    cd build
    ctest --output-on-failure
}

function benchmark() {
    if [ -f ./nlm_perf_benchmark ]; then
        ./nlm_perf_benchmark --benchmark --output=benchmark_results.json
    fi
}

function docs() {
    if [ -f Doxyfile ]; then
        doxygen Doxyfile
    fi
    if [ -f conf.py ]; then
        cd docs && make html
    fi
}

case "${1}" in
    build)
        build "${2}"
        ;;
    test)
        test
        ;;
    benchmark)
        benchmark
        ;;
    docs)
        docs
        ;;
    *)
        echo "Usage: $0 {build|test|benchmark|docs}"
        exit 1
        ;;
esac
EOF

RUN chmod +x $NLM_HOME/scripts/dev.sh

FROM base AS build-env

# Build NLM from source
RUN mkdir -p build && cd build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DNLM_ENABLE_COVERAGE=ON \
        -DNLM_ENABLE_SANITIZERS=OFF \
        -DNLM_BUILD_TESTS=ON \
        -DNLM_BUILD_EXAMPLES=ON \
        -DNLM_BUILD_PYTHON_BINDINGS=ON && \
    cmake --build . --parallel $(nproc) && \
    cmake --build . --target install

# Install runtime dependencies for deployment
RUN apt-get update && apt-get install -y \
    libgoogle-glog0 \
    libjsoncpp25 \
    libcurl4 \
    libssl3 \
    libboost-filesystem1.74.0 \
    libboost-system1.74.0 \
    && rm -rf /var/lib/apt/lists/*

FROM build-env AS production

# Remove build tools and install only runtime dependencies
RUN apt-get update && apt-get install -y \
    libgoogle-glog0 \
    libjsoncpp25 \
    libcurl4 \
    libssl3 \
    libboost-filesystem1.74.0 \
    libboost-system1.74.0 \
    && apt-get remove -y \
        build-essential \
        cmake \
        pkg-config \
        git \
        curl \
        wget \
        sudo \
        unzip \
        zip \
        python3-dev \
        libgtest-dev \
        libbenchmark-dev \
        libjsoncpp-dev \
        libcurl4-openssl-dev \
        libssl-dev \
        libboost-all-dev \
    && apt-get autoremove -y \
    && rm -rf /var/lib/apt/lists/*

# Create production directory structure
RUN mkdir -p $NLM_HOME/{bin,lib,include,config,logs}

# Copy built binaries
COPY --from=build-env $NLM_HOME/bin $NLM_HOME/bin/
COPY --from=build-env $NLM_HOME/lib $NLM_HOME/lib/
COPY --from=build-env $NLM_HOME/include $NLM_HOME/include/

# Create runtime scripts
RUN cat > $NLM_HOME/scripts/run.sh << 'EOF'
#!/bin/bash
# Runtime script for NLM production

set -e

NLM_HOME="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export LD_LIBRARY_PATH=$NLM_HOME/lib:$LD_LIBRARY_PATH

# Default configuration
CONFIG_FILE="${NLM_HOME}/config/default.cfg"
LOG_FILE="${NLM_HOME}/logs/nlm.log"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --config)
            CONFIG_FILE="$2"
            shift 2
            ;;
        --log-file)
            LOG_FILE="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [--config CONFIG] [--log-file LOG] [command]"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Run NLM
exec $NLM_HOME/bin/nlm --config "$CONFIG_FILE"
EOF

RUN chmod +x $NLM_HOME/scripts/run.sh

# Create configuration files
RUN cat > $NLM_HOME/config/default.cfg << 'EOF'
# NLM Production Configuration
# This is the default production configuration for NLM

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = true
log_filename = /var/log/nlm/nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
EOF

# Create performance configuration
RUN cat > $NLM_HOME/config/performance.cfg << 'EOF'
# NLM Performance Configuration
# Optimized settings for high-performance neural simulations

# Performance optimization flags
max_threads = $(nproc)
memory_limit = 4096
chunk_size = 512
batch_size = 1000

# Simulation performance
simulation_threads = $(nproc)
spike_propagation_mode = "async"
plasticity_updates = "batched"
memory_management = "pool"

# Logging performance
log_buffer_size = 65536
log_flush_interval = 1000

# Network performance
tcp_buffer_size = 1024000
udp_buffer_size = 65536
connection_timeout = 30

# Cache performance
cache_size = 1048576
cache_ttl = 3600
EOF

# Create startup script
RUN cat > $NLM_HOME/scripts/start.sh << 'EOF'
#!/bin/bash
# Startup script for NLM production service

set -e

NLM_HOME="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd $NLM_HOME

# Create directories
mkdir -p logs
mkdir -p config

# Install logrotate configuration
cat > /etc/logrotate.d/nlm << EOL
$NLM_HOME/logs/nlm.log {
    daily
    rotate 7
    compress
    delaycompress
    missingok
    notifempty
    create 0644 root root
}
EOL

# Set up systemd service
cat > /etc/systemd/system/nlm.service << EOL
[Unit]
Description=NLM Neural Learning Machine
After=network.target

[Service]
Type=simple
User=nlm
Group=nlm
WorkingDirectory=$NLM_HOME
ExecStart=$NLM_HOME/scripts/run.sh --config $NLM_HOME/config/production.cfg
Restart=on-failure
RestartSec=10
Environment=NLM_HOME=$NLM_HOME

[Install]
WantedBy=multi-user.target
EOL

# Create service user
if ! id nlm >/dev/null 2>&1; then
    useradd --system --no-create-home --shell /bin/false nlm
fi

# Set permissions
chown -R nlm:nlm $NLM_HOME
chmod -R 755 $NLM_HOME/bin
chmod -R 755 $NLM_HOME/scripts

# Enable service
systemctl daemon-reload
systemctl enable nlm

echo "NLM production service configured successfully"
EOF

RUN chmod +x $NLM_HOME/scripts/start.sh

FROM base AS monitoring

# Install monitoring and observability tools
RUN apt-get update && apt-get install -y \
    prometheus \
    node_exporter \
    grafana \
    jq \
    bc \
    sysstat \
    && rm -rf /var/lib/apt/lists/*

# Create monitoring configuration
RUN cat > $NLM_HOME/monitoring/prometheus.yml << 'EOF'
global:
  scrape_interval: 15s

scrape_configs:
  - job_name: 'nlm'
    static_configs:
      - targets: ['localhost:8080']
    metrics_path: /metrics
    scrape_interval: 5s

  - job_name: 'node'
    static_configs:
      - targets: ['localhost:9100']
    scrape_interval: 15s

rule_files:
  - "nlm_rules.yml"

remote_write:
  - url: http://prometheus-remote:8080/api/v1/write
EOF

# Create performance monitoring script
RUN cat > $NLM_HOME/scripts/monitor_perf.sh << 'EOF'
#!/bin/bash
# Performance monitoring script for NLM

set -e

NLM_HOME="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LOG_DIR="$NLM_HOME/logs"
DATA_DIR="$NLM_HOME/benchmarks"

# Create monitoring directories
mkdir -p "$DATA_DIR"

# Monitor CPU usage
echo "=== CPU Usage ==="
ps aux --sort=-%cpu | head -10

# Monitor memory usage
echo "=== Memory Usage ==="
free -h

# Monitor disk usage
echo "=== Disk Usage ==="
df -h

# Monitor network usage
echo "=== Network Usage ==="
ip addr show

# Collect performance data
cat > "$DATA_DIR/perf_$(date +%Y%m%d_%H%M%S).json" << EOL
{
    "timestamp": "$(date -Iseconds)",
    "cpu_usage": $(ps aux --sort=-%cpu | head -1 | awk '{print $3}'),
    "memory_usage": $(free | grep Mem | awk '{printf "%.2f", $3/$2 * 100}'),
    "disk_usage": $(df -h / | tail -1 | awk '{print $5}'),
    "load_average": $(uptime | awk -F'load average:' '{print $2}' | awk '{print $1}')
}
EOL

echo "Performance data collected: $DATA_DIR/perf_$(date +%Y%m%d_%H%M%S).json"
EOF

RUN chmod +x $NLM_HOME/scripts/monitor_perf.sh

FROM base AS testing

# Install comprehensive testing tools
RUN pip3 install --no-cache-dir \
    pytest \
    pytest-cov \
    pytest-xdist \
    pytest-timeout \
    pytest-benchmark \
    pytest-html \
    pytest-json-report \
    pytest-reportportal \
    numpy \
    scipy \
    matplotlib \
    seaborn \
    pandas \
    plotly \
    ipython \
    jupyter \
    && rm -rf /root/.cache/pip

# Install JavaScript testing tools
RUN npm install -g jest jsdom

# Install Go testing tools
RUN curl -LO https://dl.google.com/go/go1.21.0.linux-amd64.tar.gz && \
    tar -C /usr/local -xzf go1.21.0.linux-amd64.tar.gz && \
    rm -f go1.21.0.linux-amd64.tar.gz

ENV PATH=/usr/local/go/bin:$PATH

# Create comprehensive testing framework
RUN cat > $NLM_HOME/scripts/test_all.sh << 'EOF'
#!/bin/bash
# Comprehensive testing script for NLM

set -e

NLM_HOME="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$NLM_HOME/build"
TEST_DIR="$NLM_HOME/tests"
RESULTS_DIR="$NLM_HOME/test-results"

# Create test directories
mkdir -p "$RESULTS_DIR/{unit,integration,performance,coverage,reports}"

# Function to run unit tests
function run_unit_tests() {
    echo "=== Running Unit Tests ==="
    if [ -f "$BUILD_DIR/nlm_test" ]; then
        cd "$BUILD_DIR"
        ./nlm_test --gtest_output=xml:"$RESULTS_DIR/unit/test_results.xml"
        # Generate coverage report
        gcovr --xml --output="$RESULTS_DIR/coverage/coverage.xml"
    fi
}

# Function to run integration tests
function run_integration_tests() {
    echo "=== Running Integration Tests ==="
    if [ -f "$BUILD_DIR/nlm_integration_test" ]; then
        cd "$BUILD_DIR"
        ./nlm_integration_test --gtest_output=xml:"$RESULTS_DIR/integration/integration_results.xml"
    fi
}

# Function to run performance tests
function run_performance_tests() {
    echo "=== Running Performance Tests ==="
    if [ -f "$BUILD_DIR/nlm_perf_benchmark" ]; then
        cd "$BUILD_DIR"
        ./nlm_perf_benchmark --benchmark --output="$RESULTS_DIR/performance/benchmark_results.json"
    fi
}

# Function to generate test reports
function generate_reports() {
    echo "=== Generating Test Reports ==="
    cd "$RESULTS_DIR"
    
    # Generate HTML report
    if command -v pytest-html >/dev/null 2>&1; then
        pytest-html --title "NLM Test Report" unit/integration/performance
    fi
    
    # Generate JSON summary
    python3 << EOL
import json
import os
from datetime import datetime

summary = {
    "timestamp": datetime.now().isoformat(),
    "tests": {},
    "performance": {},
    "coverage": {}
}

# Load test results
if os.path.exists("unit/test_results.xml"):
    import xml.etree.ElementTree as ET
    tree = ET.parse("unit/test_results.xml")
    root = tree.getroot()
    passed = int(root.get("tests", 0)) - int(root.get("failures", 0))
    total = int(root.get("tests", 0))
    summary["tests"]["unit"] = {
        "passed": passed,
        "total": total,
        "success_rate": (passed / total * 100) if total > 0 else 0
    }

# Load performance results
if os.path.exists("performance/benchmark_results.json"):
    with open("performance/benchmark_results.json") as f:
        summary["performance"] = json.load(f)

# Load coverage results
if os.path.exists("coverage/coverage.xml"):
    tree = ET.parse("coverage/coverage.xml")
    root = tree.getroot()
    summary["coverage"] = {
        "line_coverage": float(root.get("line-rate", 0) * 100),
        "branch_coverage": float(root.get("branch-rate", 0) * 100)
    }

# Write summary
with open("reports/test_summary.json", "w") as f:
    json.dump(summary, f, indent=2)

print("Test summary written to reports/test_summary.json")
EOL
}

# Run all tests
run_unit_tests
run_integration_tests
run_performance_tests

# Generate reports
generate_reports

echo "=== All Tests Completed ==="
echo "Results available in $RESULTS_DIR"
EOF

RUN chmod +x $NLM_HOME/scripts/test_all.sh

FROM testing AS final-image

# Install additional tools for final image
RUN pip3 install --no-cache-dir \
    jupyterlab \
    ipykernel \
    && python3 -m ipykernel install --user --display-name "NLM Testing"

# Add testing user
RUN useradd --system --no-create-home --shell /bin/false testing

# Set working directory
WORKDIR $NLM_HOME

# Default command
CMD ["/bin/bash"]
