# NLM Advanced Features Troubleshooting

## Advanced Features Issues

**"Advanced features not working"**
- Check that required dependencies are installed
- Install matplotlib and pandas for visualization:
```bash
pip install matplotlib pandas
```

**"Experiment manager not found"**
- The advanced API is available through the `advanced` module:
```python
import pynlm
# Create simulation manager
sim_manager = pynlm.SimulationManager(brain, world, agent)
# Use experiment runner
experiment_runner = pynlm.ExperimentRunner(config)
```

**"Parameter optimization taking too long"**
- Reduce number of trials or parameter combinations:
```python
# Fewer trials
results = pynlm.ConfigOptimizer.parameter_sweep(
    config, parameter_ranges, num_trials=5
)

# Use grid search for smaller parameter sets
results = pynlm.ConfigOptimizer.grid_search(
    config, parameter_grids, world_size=(15, 15), num_steps=500
)
```

**"Visualization not generating plots"**
- Check matplotlib installation and permissions:
```bash
pip install matplotlib
```

**"Export/import issues with simulation data"**
- Ensure simulation has history before exporting:
```python
# Check if sim_manager has history
if sim_manager.sim_history:
    export_file = pynlm.ExportTools.export_simulation_data(sim_manager)
```

**"Diagnostic tools not providing useful information"**
- Check brain state and enable more detailed logging:
```python
# Basic diagnostics
diagnostics = pynlm.DiagnosticTools.analyze_brain_state(brain)

# Debug specific issues
debug_info = pynlm.DiagnosticTools.debug_sensory_processing(world, agent, brain)
```

**"Advanced API not accessible"**
- Import advanced features directly:
```python
import pynlm
# Create simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use advanced simulation manager
sim_manager = pynlm.SimulationManager(brain, world, agent)

# Access experiment runner
from pynlm import advanced
experiment_runner = advanced.ExperimentRunner(config)
```

---

## Further Reading

### Documentation

* [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
* [SCIENCE.md](docs/SCIENCE.md) - Scientific background
* [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
* [LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations
* [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit

### Examples and Tutorials

* `easy_usage.md` - Beginner-friendly usage guide
* `HOW_TO_USE.md` - Comprehensive technical documentation
* `python/advanced_api.py` - Advanced API features and utilities
* `python/bindings.cpp` - Python bindings implementation

### Advanced API Documentation

The NLM Python API includes advanced functionality through the `advanced` module:

#### Key Classes

* `SimulationManager` - High-level simulation control
* `ExperimentRunner` - Multiple experiment management
* `ConfigOptimizer` - Parameter optimization and sweeps
* `DiagnosticTools` - Brain state analysis and debugging
* `VisualizationTools` - Plotting and visualization
* `ExportTools` - Data export and import

#### Key Functions

* `create_default_simulation()` - Create simulation with recommended settings
* `run_quick_test()` - Run quick validation test
* `analyze_brain_state()` - Analyze brain using diagnostic tools
* `debug_simulation()` - Debug simulation for troubleshooting

#### Advanced Usage Examples

See `python/advanced_api.py` for comprehensive examples of:
- Complex simulation management
- Experiment design and execution
- Parameter optimization
- Visualization and analysis
- Data export and import
- Integration with all NLM features

### Online Resources

* GitHub repository: https://github.com/nlm-project/nlm
* Documentation: https://nlm.readthedocs.io
* Project website: https://nlm.ai

### Getting Help

* **GitHub Issues**: Report bugs and request features
* **Documentation**: Check comprehensive API reference
* **Community**: Join NLM community for discussions
* **Examples**: Run existing examples and test suites

---

## Next Steps

After mastering the NLM Python API:

1. **Advanced Features**: Explore the advanced API in `python/advanced_api.py`
2. **Experiment Design**: Create your own experimental protocols
3. **Parameter Optimization**: Use `ConfigOptimizer` for automatic tuning
4. **Visualization**: Generate plots and analysis with `VisualizationTools`
5. **Integration**: Combine multiple features for complex simulations
6. **Research**: Use NLM for neuroscience research and education
7. **Development**: Contribute to the NLM project

NLM provides a powerful platform for creating artificial developmental brains with advanced learning and adaptive capabilities. Start with the basic examples and gradually explore the more advanced features as you become familiar with the system.

**Happy brain simulation!**