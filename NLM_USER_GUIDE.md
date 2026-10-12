# NLM User Guide

## Quick Start

This guide provides quick instructions for getting started with the Enhanced NLM (Neural Learning Machine) system.

### Installation

1. **Clone the repository** (if not already done):
   ```bash
   git clone <repository-url>
   cd nlm-enhanced
   ```

2. **Build the system**:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

3. **Run the demo**:
   ```bash
   ./nlm_agent --mode demo --world-width 20 --world-height 20
   ```

### Basic Usage

#### 1. Demonstration Mode
```bash
nlm_agent --mode demo --world-width 20 --world-height 20 --episodes 10 --steps-per-episode 100
```

**What this does:**
- Runs a basic demonstration with all memory systems
- Agent explores a 20x20 world
- Shows neural activity, memory usage, and behavior
- Logs detailed progress information

#### 2. Training Mode
```bash
nlm_agent --mode train --episodes 1000 --steps-per-episode 2000 \
         --enable-curiosity --enable-reward --enable-dev \
         --brain-regions 3 --neurons-per-region 200
```

**What this does:**
- Trains agent with curiosity-driven exploration
- Uses reward modulation for learning
- Enables developmental plasticity
- Creates complex neural networks with multiple regions

#### 3. Evaluation Mode
```bash
nlm_agent --mode eval --load trained_brain.dat --episodes 100
```

**What this does:**
- Tests performance of trained agent
- No learning or adaptation during evaluation
- Measures final competency and skill proficiency

#### 4. Debug Mode
```bash
nlm_agent --mode debug --log-level debug --enable-dev --visualize
```

**What this does:**
- Detailed logging and analysis
- Development system enabled for enhanced plasticity
- Visualization system activated for real-time monitoring
- Focus on debugging and optimization

### Configuration Options

#### Core Parameters
- `--mode`: Operation mode (demo, train, eval, debug)
- `--episodes`: Number of training/evaluation episodes
- `--steps-per-episode`: Steps per episode for agent-environment interaction

#### Environment Parameters
- `--world-width`: Width of the simulated world (meters)
- `--world-height`: Height of the simulated world (meters)
- `--vision-width`: Width of agent's vision sensor
- `--vision-height`: Height of agent's vision sensor

#### Neural Network Parameters
- `--brain-regions`: Number of neural regions in the brain
- `--neurons-per-region`: Number of neurons per region

#### Memory System Parameters
- `--memory-capacity`: Working memory capacity (items)
- `--memory-decay`: Working memory decay rate (0.0-1.0)

#### Learning Features
- `--enable-dev`: Enable developmental plasticity
- `--enable-curiosity`: Enable curiosity-driven exploration
- `--enable-reward`: Enable reward modulation

#### Logging and Monitoring
- `--log-level`: Logging level (trace, debug, info, warn, error, fatal)

### Advanced Usage

#### Using Configuration Files
Save your configuration to a file and load it later:

```bash
# Save configuration
nlm_agent --mode train --episodes 1000 --world-width 30 --world-height 30 \
         --brain-regions 5 --neurons-per-region 400 \
         --enable-curiosity --enable-reward --enable-dev > nlm_config.cfg

# Load configuration
nlm_agent --load-config nlm_config.cfg
```

#### Performance Monitoring
```bash
nlm_agent --mode train --performance-monitoring --metrics-file metrics.json
```

#### Visualization
```bash
nlm_agent --mode debug --visualize --output-format html --output-file dashboard.html
```

#### Integration Testing
```bash
nlm_agent --mode eval --integration-test --test-suite full --verbose
```

### Working with Memory Systems

#### View Memory Statistics
The enhanced NLM system provides detailed memory system statistics:

**Working Memory:**
- Current usage vs capacity
- Access patterns and recency
- Competition metrics

**Episodic Memory:**
- Episode count and recent episodes
- Pattern completion statistics
- Replay activity

**Semantic Memory:**
- Fact count and organization
- Concept formation effectiveness
- Clustering metrics

**Procedural Memory:**
- Skill count and proficiency distribution
- Motor chunk identification
- Consolidation effectiveness

**Associative Memory:**
- Association strength distribution
- Network connectivity
- Higher-order associations

### Visualization

The enhanced NLM system includes comprehensive visualization capabilities:

#### Text Output
```bash
nlm_agent --mode debug --log-level info --visualize-text
```

#### JSON Output
```bash
nlm_agent --mode debug --log-level info --visualize-json > system_data.json
```

#### HTML Dashboard
```bash
nlm_agent --mode debug --visualize-html --output-file dashboard.html
```

#### Real-time Visualization
```bash
nlm_agent --mode train --real-time-viz --viz-update-interval 1000
```

### Error Handling

The enhanced NLM system includes comprehensive error handling:

#### Common Errors and Solutions

**Error:** `Failed to initialize brain!`
**Solution:** Check that you have sufficient system resources and correct configuration parameters.

**Error:** `Invalid configuration value`
**Solution:** Verify configuration parameters are within valid ranges.

**Error:** `Memory allocation failed`
**Solution:** Increase memory limits or reduce system complexity.

#### Getting Help
```bash
nlm_agent --help
```

This displays all available options and usage examples.

### Troubleshooting

#### Performance Issues

**Symptom:** Slow agent behavior
**Possible causes:**
- Large neural networks
- High memory usage
- Complex environment

**Solutions:**
- Reduce brain regions or neurons per region
- Simplify world configuration
- Decrease episode/steps-per-episode counts

#### Memory Issues

**Symptom:** Memory allocation failures
**Possible causes:**
- Insufficient system memory
- Inefficient memory usage

**Solutions:**
- Close other applications
- Monitor memory usage
- Optimize configuration

#### Visualization Issues

**Symptom:** HTML dashboard not loading
**Possible causes:**
- Browser compatibility
- Missing dependencies

**Solutions:**
- Use modern browser (Chrome, Firefox, Safari)
- Check that all dependencies are installed

### Getting Started Tutorial

#### Tutorial 1: Basic Agent Navigation
1. Run a simple demo with a small world:
   ```bash
   nlm_agent --mode demo --world-width 10 --world-height 10 --episodes 5 --steps-per-episode 50
   ```

2. Observe agent movement and exploration
3. Check memory usage and neural activity

#### Tutorial 2: Memory System Exploration
1. Run training with memory visualization:
   ```bash
   nlm_agent --mode train --episodes 10 --steps-per-episode 100 --enable-curiosity \
            --visualize-html --output-file mem_viz.html
   ```

2. Analyze memory system statistics
3. Observe episodic memory storage and semantic memory formation

#### Tutorial 3: Advanced Configuration
1. Create a custom configuration:
   ```bash
   nlm_agent --mode train --episodes 100 --steps-per-episode 500 \
            --world-width 25 --world-height 25 \
            --brain-regions 4 --neurons-per-region 300 \
            --enable-curiosity --enable-reward --enable-dev \
            --log-level info --visualize-text > custom_config.cfg
   ```

2. Use the configuration in future runs:
   ```bash
   nlm_agent --load-config custom_config.cfg
   ```

### Additional Resources

#### Documentation
- **Main Documentation**: `NLM_ENHANCED_DOCUMENTATION.md`
- **API Reference**: Generated from source code headers
- **Examples**: `examples/` directory with sample configurations

#### Online Resources
- **Documentation Website**: Available online
- **Community Forum**: For questions and discussions
- **Issue Tracker**: Bug reports and feature requests

#### Support
- **Email Support**: support@nlm.org
- **GitHub Issues**: Report bugs and request features
- **Discord Server**: Community discussions and real-time support

### License and Credits

#### License
MIT License - See `LICENSE` file for details

#### Credits
- **Lead Developer**: NLM Team
- **Contributors**: Open source community
- **Graphics**: Custom neural visualization system
- **Documentation**: Comprehensive user guides and API references

#### Version
Enhanced NLM System - Version 2.0.0

### Feedback

#### How to Provide Feedback
1. **Bug Reports**: Use GitHub issue tracker
2. **Feature Requests**: Submit via GitHub or email
3. **Documentation Issues**: Report via issue tracker
4. **Performance Concerns**: Email support with details

#### What We Need
- Clear reproduction steps
- System configuration information
- Error messages and logs
- Performance metrics (if available)

### Next Steps

#### Learning Path
1. **Week 1**: Basic usage and simple demonstrations
2. **Week 2**: Advanced configuration and memory systems
3. **Week 3**: Visualization and analysis
4. **Week 4**: Custom development and integration

#### Development Roadmap
1. **Phase 1**: Core system setup and basic functionality
2. **Phase 2**: Memory system enhancements
3. **Phase 3**: Advanced visualization and analysis
4. **Phase 4**: Integration with external systems
5. **Phase 5**: Production deployment and optimization

### Conclusion

The Enhanced NLM system provides a powerful platform for neural network simulation with:

- **Sophisticated memory systems** inspired by biological brains
- **Intuitive command-line interface** for easy configuration
- **Comprehensive visualization** for analysis and debugging
- **Robust error handling** ensuring system reliability
- **Extensive documentation** for easy adoption

Get started today and explore the capabilities of next-generation neural simulation!