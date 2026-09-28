# NLM CI/CD Pipeline Setup

## Overview

This repository contains a comprehensive CI/CD pipeline for the NLM (Neural Learning Machine) project. The setup provides:

- **GitHub Actions**: Automated CI/CD workflows
- **Docker**: Containerized development and production environments
- **Static Analysis**: Code quality gates and validation
- **Testing**: Comprehensive test suites with coverage and performance metrics
- **Deployment**: Multi-stage build and deployment strategies
- **Monitoring**: Production monitoring and observability

## Repository Structure

```
NLM/
├── .github/
│   └── workflows/
│       └── ci-cd.yml                    # GitHub Actions workflow
├── docker/
│   ├── Dockerfile                         # Base Docker image
│   ├── docker-compose.yml                 # Multi-container setup
│   └── production/                        # Production config
├── scripts/
│   ├── quality_check.sh                   # Code quality gates
│   ├── test_runner.sh                     # Test execution
│   ├── deploy.sh                          # Build and deployment
│   ├── monitor.sh                         # Monitoring and observability
│   └── integration_test.sh                # Integration testing
├── config/
│   ├── codecov.yml                        # Codecov configuration
│   ├── sonarcloud.properties              # SonarCloud properties
│   └── clang-tidy.yml                     # Clang-tidy configuration
├── docs/
│   └── CI_CD/                             # CI/CD documentation
├── Makefile                               # Build automation
└── README.md
```

## Quick Start

### 1. Local Development Setup

```bash
# Setup development environment
docker-compose up -d

# Enter development container
docker-compose exec nlm-dev /bin/bash

# Install development dependencies
./scripts/dev_setup.sh
```

### 2. Run Tests

```bash
# Quick test run (unit tests only)
./scripts/test_runner.sh quick

# Full test suite
./scripts/test_runner.sh full
```

### 3. Quality Gates

```bash
# Run quality checks
./scripts/quality_check.sh
```

### 4. Build and Deploy

```bash
# Build and test locally
./scripts/deploy.sh build

# Deploy to staging
./scripts/deploy.sh deploy staging

# Deploy to production
./scripts/deploy.sh deploy production
```

### 5. Monitor

```bash
# Check system status
./scripts/monitor.sh status

# Export monitoring reports
./scripts/monitor.sh export

# Start monitoring stack
./scripts/monitor.sh setup
```

## CI/CD Pipeline

### Workflow Stages

1. **Quality Gates**
   - Code formatting (clang-format)
   - Static analysis (clang-tidy)
   - Code checking (cppcheck)
   - Include validation (iwyu)
   - Python quality checks

2. **Build and Test**
   - Multi-configuration builds (Debug, Release, RelWithDebInfo)
   - C++ unit tests and integration tests
   - Python bindings testing
   - Code coverage generation
   - Automated reporting

3. **Performance Testing**
   - Benchmark execution
   - Load testing
   - Performance metrics collection

4. **Docker Build**
   - Multi-stage Docker image building
   - Container testing and validation

5. **Static Analysis**
   - Security scanning (Semgrep)
   - Python security analysis (Bandit)
   - Script validation (ShellCheck)

6. **Multi-Platform Builds**
   - Cross-platform build testing

7. **Deployment**
   - Production image deployment
   - Rollback procedures
   - Health checks and validation

## GitHub Actions Workflow

### Key Features

- **Parallel Testing**: Multiple build configurations run in parallel
- **Coverage Reporting**: Automatic code coverage tracking
- **Security Scanning**: Integrated security vulnerability detection
- **Artifact Management**: Build artifacts and test reports are preserved
- **Deploy on Merge**: Automatic deployment to production on main branch pushes

### Workflow File: `.github/workflows/ci-cd.yml`

**Triggers:**
- Push to main and develop branches
- Pull requests to main branch
- Manual workflow dispatch

**Environment Variables:**
- `REGISTRY`: Container registry (default: ghcr.io)
- `IMAGE_NAME`: Docker image name
- `BUILD_TYPE`: Build type (default: Release)

**Jobs:**
- `quality-gates`: Code quality validation
- `build-and-test`: Multi-configuration builds and testing
- `performance-tests`: Performance benchmarking
- `docker-build`: Docker image building
- `static-analysis`: Security and code analysis
- `multi-platform-build`: Cross-platform testing
- `deploy`: Production deployment

## Docker Configuration

### Multi-Container Setup

The Docker configuration provides a complete development environment:

| Service | Purpose | Ports |
|---------|---------|-------|
| `nlm-core` | Main NLM application | 8080 |
| `redis` | Caching and session management | 6379 |
| `postgres` | Persistent data storage | 5432 |
| `elasticsearch` | Logging and search | 9200 |
| `kafka` | Message streaming | 9092 |
| `prometheus` | Metrics collection | 9090 |
| `grafana` | Visualization | 3000 |
| `jaeger` | Distributed tracing | 16686 |
| `nginx` | Reverse proxy | 80, 443 |

### Docker Compose Commands

```bash
# Start all services
docker-compose up -d

# Stop all services
docker-compose down

# View logs
docker-compose logs -f

# Execute in container
docker-compose exec nlm-dev bash
```

## Static Analysis and Quality Gates

### Code Formatting (clang-format)

Validates code style consistency:
```bash
find src tests -name "*.cpp" -o -name "*.h" | xargs clang-format --style=file --dry-run
```

### Static Analysis (clang-tidy)

Checks for code quality issues:
```bash
# Configuration
cat > .clang-tidy << EOF
Checks:
  - readability.*
  - performance.*
  - portability.*
  - misc.*
EOF

# Execute
mkdir -p build_tidy && cd build_tidy && cmake -DCMAKE_BUILD_TYPE=Debug ../src && make -j$(nproc) && cd ..
```

### Code Quality (cppcheck)

Detects potential bugs and issues:
```bash
cppcheck --enable=all --std=c++20 --project=src/CMakeLists.txt
```

### Include Validation (include-what-you-use)

Ensures proper header dependencies:
```bash
# Configuration
cat > .iwyu.cfg << EOF
include_paths: ["src/"]
compatibility: { style: google }
EOF

# Execute
find src tests -name "*.cpp" | xargs iwyu --config=.iwyu.cfg
```

## Testing Infrastructure

### Test Categories

1. **Unit Tests**
   - Individual component testing
   - Mock-based testing
   - Isolated testing

2. **Integration Tests**
   - System-wide functionality
   - Component interaction
   - End-to-end testing

3. **Performance Tests**
   - Benchmarking
   - Stress testing
   - Load testing

### Test Execution

```bash
# Run unit tests
./scripts/test_runner.sh quick

# Run full test suite
./scripts/test_runner.sh full

# Run specific test type
./scripts/test_runner.sh integration
```

### Coverage Reporting

Generates comprehensive coverage reports:
- **HTML Reports**: Interactive web-based coverage
- **XML Reports**: Machine-readable format
- **Codecov Integration**: Online coverage tracking

## Build and Deployment

### Build Process

```bash
# Complete build process
./scripts/deploy.sh build

# Build only
./scripts/deploy.sh build --skip-tests

# Deploy with specific environment
./scripts/deploy.sh deploy staging
./scripts/deploy.sh deploy production
```

### Deployment Strategies

1. **Multi-Stage Build**
   - Development → Staging → Production
   - Docker image optimization
   - Version-based rollback

2. **Kubernetes Deployment**
   - Automated rolling updates
   - Health checks
   - Resource management

3. **Docker Swarm**
   - Scalable container orchestration
   - Load balancing
   - Service discovery

## Monitoring and Observability

### Monitoring Stack

The monitoring setup includes:

- **Prometheus**: Metrics collection and storage
- **Grafana**: Visualization and dashboards
- **Jaeger**: Distributed tracing
- **Elasticsearch**: Log aggregation and search

### Key Metrics

- **Application Metrics**: Request rates, response times, error rates
- **System Metrics**: CPU, memory, disk usage
- **Custom Metrics**: Simulation parameters, learning progress
- **Business Metrics**: User engagement, model performance

### Monitoring Commands

```bash
# Check system status
./scripts/monitor.sh status

# Start monitoring stack
./scripts/monitor.sh setup

# Export reports
./scripts/monitor.sh export

# Stop monitoring stack
./scripts/monitor.sh cleanup
```

## Performance Testing

### Benchmark Execution

Run performance benchmarks:
```bash
docker-compose exec nlm-bench /bin/bash
./benchmarks/run_benchmarks.sh
```

### Load Testing

Execute load tests:
```bash
python scripts/load_test.py --duration 300 --rate 100
```

## Configuration Files

### Project Configuration

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | C++ build configuration |
| `pyproject.toml` | Python package configuration |
| `Makefile` | Build automation |

### Tool Configuration

| File | Tool | Purpose |
|------|------|---------|
| `config/clang-tidy.yml` | clang-tidy | Static analysis configuration |
| `config/cppcheck.xml` | cppcheck | Code quality configuration |
| `config/iwyu.cfg` | iwyu | Include validation configuration |

## Best Practices

### Development

1. **Code Quality**
   - Follow consistent coding standards
   - Write comprehensive unit tests
   - Use code formatting tools
   - Validate with static analysis

2. **Testing**
   - Test early and often
   - Maintain test coverage > 80%
   - Use integration tests for system validation
   - Monitor performance benchmarks

3. **Deployment**
   - Use feature flags for gradual rollout
   - Implement automated rollback
   - Monitor system health continuously
   - Track performance metrics

### Security

1. **Static Analysis**
   - Regular security scanning
   - Dependency vulnerability checks
   - Code review processes

2. **Runtime Security**
   - Input validation
   - Authentication and authorization
   - Secure configuration management

### Operations

1. **Monitoring**
   - Set up comprehensive alerts
   - Monitor key performance indicators
   - Track system resource usage
   - Log all important events

2. **Maintenance**
   - Regular system updates
   - Backup and recovery procedures
   - Performance optimization
   - Capacity planning

## Troubleshooting

### Common Issues

1. **Build Failures**
   ```bash
   # Check for missing dependencies
   docker-compose logs
   
   # Check container status
   docker ps
   
   # View build logs
   docker-compose exec nlm-dev ls -la
   ```

2. **Test Failures**
   ```bash
   # Run tests with verbose output
   ./scripts/test_runner.sh full
   
   # Check test configuration
   cat tests/test_config.json
   
   # Verify test environment
   docker-compose exec nlm-dev which python3
   ```

3. **Docker Issues**
   ```bash
   # Check Docker service status
   sudo systemctl status docker
   
   # Verify network connectivity
   docker network ls
   
   # Clean up unused resources
   docker system prune -a
   ```

### Debugging Commands

```bash
# Check container status
docker ps

# View container logs
docker logs <container_name>

# Check network connectivity
docker exec <container_name> ping <host>

# Examine files in container
docker exec <container_name> ls -la <directory>
```

## Future Enhancements

### Planned Features

1. **Advanced CI/CD**
   - Machine learning model optimization
   - Adaptive testing strategies
   - Real-time monitoring dashboards
   - Automated performance tuning

2. **Integration**
   - GitHub Actions integration
   - Slack/Teams notifications
   - External monitoring services
   - Custom metrics collection

3. **Scaling**
   - Horizontal scaling support
   - Load balancing configuration
   - Auto-scaling rules
   - Geographic distribution

## License

This CI/CD setup is provided under the same license as the NLM project (MIT License).

## Acknowledgments

This CI/CD setup was developed to provide a robust, scalable, and maintainable pipeline for the NLM project. It incorporates industry best practices for software development, testing, deployment, and monitoring.

## Contact

For questions or issues with the CI/CD setup, please refer to the project documentation or contact the development team.