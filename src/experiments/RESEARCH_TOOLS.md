# NLM Research Tools System

## Overview

The NLM Research Tools System provides comprehensive experimental control and analysis capabilities for NLM research. This system enables researchers to conduct controlled experiments, tune parameters, compare conditions, and analyze results systematically.

## System Components

### ResearchTools

The `ResearchTools` class provides:

- **Experimental Condition Management**: Create, run, and manage different experimental conditions
- **Parameter Tuning**: Interactive parameter adjustment and optimization
- **Data Comparison**: Compare results across different experimental conditions
- **Statistical Testing**: Built-in statistical analysis tools
- **Ablation Studies**: Enable/disable specific systems for controlled experiments
- **Data Export/Import**: Save and load experimental data
- **Research Mode**: Specialized mode for research experiments
- **Integration with Metrics**: Connect to both Metrics and ResearchMetrics systems

### Key Features

#### Experimental Condition Management
- **Create Conditions**: Define experimental conditions with specific parameters
- **Run Conditions**: Execute experiments with different parameter sets
- **Condition Comparison**: Compare results across multiple conditions
- **Baseline Management**: Set and manage baseline experimental conditions

#### Parameter Tuning
- **Add Tuning Recipes**: Define parameters to optimize
- **Run Optimization**: Automated parameter tuning
- **Performance Evaluation**: Calculate performance based on research objectives
- **Results Storage**: Save and analyze tuning results

#### Data Comparison
- **Multi-condition Analysis**: Compare metrics across experimental conditions
- **Detailed Comparison Reports**: Generate comprehensive comparison reports
- **Performance Metrics**: Compare performance, learning, memory, etc.
- **Statistical Comparisons**: Statistical analysis of condition differences

#### Statistical Testing
- **Test Management**: Add and run statistical tests
- **Test Results**: Store and analyze statistical test results
- **Significance Testing**: Determine statistical significance
- **Interpretation**: Provide interpretations and recommendations

#### Ablation Studies
- **System Control**: Enable/disable specific brain systems
- **Ablation Configuration**: Define ablative experimental conditions
- **Restoration**: Restore systems after ablation
- **Ablation Tracking**: Track ablation and restoration events

### API Usage

```cpp
// Initialize research tools
ResearchTools tools;

// Set up experimental conditions
ResearchTools::ExperimentalCondition condition1;
condition1.name = "baseline";
condition1.description = "Normal operation without interventions";
condition1.parameters = {{"learning_rate", 0.01}, {"plasticity_rate", 0.1}};
condition1.enabledSystems = {"brain_core", "working_memory"};
condition1.disabledSystems = {}; // Empty = all enabled
condition1.isBaseline = true;

tools.createExperimentalCondition(condition1);

// Create ablation condition
ResearchTools::ExperimentalCondition condition2;
condition2.name = "ablated_memory";
condition2.description = "Working memory system disabled";
condition2.parameters = {{"learning_rate", 0.01}, {"plasticity_rate", 0.1}};
condition2.enabledSystems = {"brain_core"};
condition2.disabledSystems = {"working_memory"};
condition2.isBaseline = false;

tools.createExperimentalCondition(condition2);

// Set baseline
tools.setBaselineCondition("baseline");

// Run experimental condition
bool success = tools.runExperimentalCondition("ablated_memory");

// Compare conditions
std::vector<DataComparisonResult> comparisons = tools.compareAllConditions();

// Add statistical tests
StatisticalTestResult t_test;
t_test.testName = "t-test_memory_performance";
t_test.testType = "independent_t_test";
t_test.statistic = 2.5;
t_test.pValue = 0.02;
t_test.isSignificant = true;
t_test.interpretation = "Memory performance significantly reduced";
t_test.recommendation = "Consider memory system optimization";

tools.addStatisticalTest(t_test);
```

## Integration with Other Systems

### Integration with Metrics and ResearchMetrics
```cpp
// Initialize metrics systems
Metrics metrics;
ResearchMetrics researchMetrics;
ResearchTools tools;

// Set up experimental condition
tools.setBaselineCondition("baseline");

// Integrate with different metrics systems
tools.integrateWithMetrics(metrics);
tools.integrateWithResearchMetrics(researchMetrics);

// Generate reports
std::vector<DataComparisonResult> performanceReport = tools.generatePerformanceReport();
std::vector<DataComparisonResult> learningReport = tools.generateLearningReport();
```

### Integration with Phase 6
```cpp
Phase6IntegratedExperiment experiment;
Phase6Config config;
config.enableVisualization = true;

// Enable research tools
ResearchTools* tools = experiment.getResearchTools();
if (tools) {
    tools->setResearchMode(true);
    tools->enableDebugMode(true);
}

// Run experiment with research tools
auto result = experiment.run(config);

// Access research data
bool exportSuccess = experiment.exportResearchData("./phase6_results");
```

### Integration with Visualization
```cpp
// Generate visualization data
NLMLiveVisualization* visualization = experiment.getVisualization();
if (visualization && visualization->isActive()) {
    bool vizSuccess = experiment.generateVisualizationData("./phase6_viz.json");
}

// Generate statistical visualization
bool statVizSuccess = experiment.generateStatisticalVisualization("./phase6_stats.json");
```

## Performance Considerations

- **Efficient Data Handling**: Optimized for large-scale experimental data
- **Non-blocking Operations**: Tools don't slow down simulation execution
- **Configurable Storage**: Manage memory usage for long experiments
- **Parallel Processing**: Support for concurrent experimental runs

## Data Export/Import

### JSON Export
- Hierarchical structure for all experimental data
- Complete metadata preservation
- Research tools configuration
- Experimental condition definitions

### Condition Export
- Tabular format for experimental conditions
- Parameter definitions
- System enable/disable configurations
- Baseline specifications

## Limitations

- Some advanced statistical tests are simplified for demonstration
- Machine learning integration requires additional libraries
- Real-time parameter tuning may have performance limitations
- Visualization export depends on graphics library availability

## Future Enhancements

- **Automated Experiment Design**: Generate optimal experimental protocols
- **Multi-objective Optimization**: Balance multiple research objectives
- **Real-time Collaboration**: Share experiments across research teams
- **Cloud Integration**: Distribute experimental workloads
- **Advanced Statistics**: Implement more sophisticated statistical tests
- **Machine Learning Integration**: Automated model selection and validation

## Technical Documentation

- Implementation details in `src/experiments/ResearchTools.hpp` and `.cpp`
- Integration examples in `Phase6IntegratedExperiment.hpp` and `.cpp`
- API documentation available in the header files

## Example Research Workflow

### 1. Setup Experimental Conditions
```cpp
// Create baseline condition
ResearchTools::ExperimentalCondition baseline;
baseline.name = "control";
baseline.description = "No interventions";
baseline.parameters = {{/* default parameters */}};
baseline.isBaseline = true;
tools.createExperimentalCondition(baseline);

// Create ablation condition
ResearchTools::ExperimentalCondition ablation;
ablation.name = "no_memory";
ablation.description = "Working memory disabled";
ablation.disabledSystems = {"working_memory"};
tools.createExperimentalCondition(ablation);
```

### 2. Run Experiments
```cpp
// Set and run baseline
tools.setBaselineCondition("control");
// Run experiment and collect metrics

// Set and run ablation
tools.setBaselineCondition("no_memory");
// Run experiment and collect metrics
```

### 3. Analyze Results
```cpp
// Compare conditions
auto comparisons = tools.compareConditions("control", "no_memory");

// Generate statistical tests
auto statistics = tools.runStatisticalTests();

// Create reports
auto performanceReport = tools.generatePerformanceReport();
auto learningReport = tools.generateLearningReport();
```

### 4. Export Results
```cpp
// Export all experimental data
bool success = tools.exportResults("./research_results.json");

// Export specific condition definitions
bool conditionsSuccess = tools.exportConditions("./experimental_conditions.txt");
```

## Research Mode Features

### Enable Research Mode
```cpp
tools.setResearchMode(true);
tools.enableDebugMode(true);
tools.pauseAllSystems(false);
```

### Debug Mode
- Enhanced logging
- Detailed system state information
- Performance metrics
- Experimental parameter validation

### System Control
- **Pause/Resume**: Control experiment execution
- **Parameter Override**: Temporarily modify experimental parameters
- **System Monitoring**: Track system resource usage
- **Event Logging**: Detailed experiment progress tracking

## Statistical Tools

### Available Tests
- **t-tests**: Independent and paired samples
- **ANOVA**: Analysis of variance
- **Correlation tests**: Pearson and Spearman correlation
- **Regression analysis**: Linear and polynomial regression
- **Non-parametric tests**: Mann-Whitney, Kruskal-Wallis

### Test Configuration
```cpp
StatisticalTestResult test;
test.testName = "experiment_1_vs_2";
test.testType = "independent_t_test";
test.isSignificant = true; // Set based on p-value
```

## Visualization Integration

### Data Generation
```cpp
// Generate visualization data for research
bool vizSuccess = tools.generateVisualizationData("./research_viz.json");

// Generate statistical visualizations
bool statsVizSuccess = tools.generateStatisticalVisualization("./research_stats.json");

// Generate comparison visualizations
bool comparisonVizSuccess = tools.generateComparisonVisualization("./research_comparison.json");
```

## Configuration Management

### Set Default Parameters
```cpp
tools.setDefaultParameters();
```

### Load/Save Parameters
```cpp
tools.loadParametersFromFile("./research_config.json");
// ... run experiments ...
tools.saveParametersToFile("./research_config_output.json");
```

## Cleanup and Reset

### Reset Research Tools
```cpp
tools.reset();
```

### Stop All Operations
```cpp
tools.stop();
```

## Error Handling

The ResearchTools system provides comprehensive error handling:

- **File I/O Errors**: Graceful handling of missing or corrupted files
- **Parameter Validation**: Validation of experimental parameters
- **System State Errors**: Detection and recovery from inconsistent system states
- **Resource Management**: Automatic cleanup of resources

## Future Development Plans

### Phase 1: Core Functionality
- [x] Experimental condition management
- [x] Parameter tuning
- [x] Data comparison
- [x] Statistical testing

### Phase 2: Advanced Features
- Automated experimental design
- Multi-objective optimization
- Real-time collaboration

### Phase 3: Integration
- Cloud computing integration
- Machine learning automation
- Advanced visualization dashboards

## Credits

This system was developed as part of the NLM Phase 6 Final Audit implementation, addressing the need for advanced visualization and metrics collection for research analysis.

## License

MIT License

Research Tools System - NLM Phase 6 Implementation
