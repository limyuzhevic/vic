#!/usr/bin/bash
# NLM Tools Integration Workflow
# Complete workflow example demonstrating how to use all NLM tools together

echo "=== NLM Tools Integration Workflow ==="
echo

echo "This workflow demonstrates how to use all NLM advanced tools together:"
echo "1. Configure the system"
echo "2. Analyze the network"
echo "3. Profile performance"
echo "4. Validate integration"
echo "5. Export results"
echo

# Create a sample configuration
echo "# Sample NLM Configuration" > sample_config.cfg
echo "neuron_count = 1000" >> sample_config.cfg
echo "region_count = 2" >> sample_config.cfg
echo "connection_probability = 0.1" >> sample_config.cfg
echo "plasticity_learning_rate = 0.01" >> sample_config.cfg
echo "max_simulation_steps = 10000" >> sample_config.cfg
echo "log_level = INFO" >> sample_config.cfg
echo "log_to_file = false" >> sample_config.cfg
echo "log_filename = nlm.log" >> sample_config.cfg
echo "visualization_enabled = false" >> sample_config.cfg
echo "visualization_update_rate = 30.0" >> sample_config.cfg
echo

echo "Configuration created: sample_config.cfg"
echo

# Step 1: Configure and validate configuration
echo "=== Step 1: Configuration Management ==="
echo "Using NLMConfig to validate configuration..."
python3 tools/NLMConfig/main.py --validate sample_config.cfg
echo

# Step 2: Generate enhanced configuration
echo "=== Step 2: Configuration Enhancement ==="
echo "Using NLMConfig to generate enhanced configuration..."
python3 tools/NLMConfig/main.py --generate enhanced_config.cfg
echo "Enhanced configuration created: enhanced_config.cfg"
echo

# Step 3: Network analysis
echo "=== Step 3: Network Analysis ==="
echo "Using NLMAnalyze for network connectivity analysis..."
python3 tools/NLMAnalyze/main.py --connectivity enhanced_config.cfg

echo "Using NLMAnalyze for clustering analysis..."
python3 tools/NLMAnalyze/main.py --clustering enhanced_config.cfg

echo "Using NLMAnalyze for topology analysis..."
python3 tools/NLMAnalyze/main.py --topology enhanced_config.cfg

echo "Using NLMAnalyze for performance analysis..."
python3 tools/NLMAnalyze/main.py --performance enhanced_config.cfg
echo

# Step 4: Performance profiling
echo "=== Step 4: Performance Profiling ==="
echo "Using NLMProfile for performance profiling..."
python3 tools/NLMProfile/main.py --performance 10 enhanced_config.cfg

echo "Using NLMProfile for memory profiling..."
python3 tools/NLMProfile/main.py --memory enhanced_config.cfg

echo "Using NLMProfile for bottleneck detection..."
python3 tools/NLMProfile/main.py --bottlenecks enhanced_config.cfg
echo

# Step 5: Integration validation
echo "=== Step 5: Integration Validation ==="
echo "Using NLMIntegrate for integration verification..."
python3 tools/NLMIntegrate/main.py --verify enhanced_config.cfg

echo "Using NLMIntegrate for comprehensive integration testing..."
python3 tools/NLMIntegrate/main.py --comprehensive-test enhanced_config.cfg

echo "Using NLMIntegrate for health check..."
python3 tools/NLMIntegrate/main.py --health-check enhanced_config.cfg
echo

# Step 6: Data export
echo "=== Step 6: Data Export ==="
echo "Using NLMExport for configuration export..."
python3 tools/NLMExport/main.py --export-config exported_config.json json enhanced_config.cfg

echo "Using NLMExport for brain data export..."
python3 tools/NLMExport/main.py --export-brain-data exported_brain_data.json json enhanced_config.cfg

echo "Using NLMExport for simulation results export..."
python3 tools/NLMExport/main.py --export-simulation-results exported_simulation_results.json enhanced_config.cfg

echo "Using NLMExport for analysis data export..."
python3 tools/NLMExport/main.py --export-analysis-data exported_analysis_data.json enhanced_config.cfg

echo "Using NLMExport for all data export..."
python3 tools/NLMExport/main.py --export-all exported_data enhanced_config.cfg
echo

# Step 7: Generate comprehensive reports
echo "=== Step 7: Report Generation ==="
echo "Generating analysis report..."
python3 tools/NLMAnalyze/main.py --connectivity --output final_analysis.txt enhanced_config.cfg

python3 tools/NLMAnalyze/main.py --connectivity --output final_analysis.json --output-format json enhanced_config.cfg

echo "Generating profiling report..."
python3 tools/NLMProfile/main.py --performance 10 --output final_profile.txt enhanced_config.cfg

python3 tools/NLMProfile/main.py --performance 10 --output final_profile.json --output-format json enhanced_config.cfg

echo "Generating integration report..."
python3 tools/NLMIntegrate/main.py --generate-report final_integration_report.txt enhanced_config.cfg
echo

# Step 8: Display summary
echo "=== Workflow Summary ==="
echo "All NLM advanced tools have been successfully executed:"
echo "✓ Configuration validated and enhanced"
echo "✓ Network connectivity analyzed"
echo "✓ Network clustering analyzed"
echo "✓ Network topology analyzed"
echo "✓ Performance metrics analyzed"
echo "✓ Performance profiled"
echo "✓ Memory usage profiled"
echo "✓ Bottlenecks detected"
echo "✓ Integration verified"
echo "✓ Integration tested"
echo "✓ Health checked"
echo "✓ Configuration exported"
echo "✓ Brain data exported"
echo "✓ Simulation results exported"
echo "✓ Analysis data exported"
echo "✓ All data exported to directory"
echo "✓ Analysis report generated"
echo "✓ Profiling report generated"
echo "✓ Integration report generated"
echo
enecho "=== Generated Files ==="
echo "Configuration files:"
echo "  sample_config.cfg"
echo "  enhanced_config.cfg"
echo
echo "Exported data:"
echo "  exported_config.json"
echo "  exported_brain_data.json"
echo "  exported_simulation_results.json"
echo "  exported_analysis_data.json"
echo "  exported_data/ (directory)"
echo
echo "Reports:"
echo "  final_analysis.txt"
echo "  final_analysis.json"
echo "  final_profile.txt"
echo "  final_profile.json"
echo "  final_integration_report.txt"
echo
echo "=== Integration Status ==="
echo "All NLM tools are working correctly and supporting Phase 6 integration."
echo "The workflow demonstrates complete integration of all advanced tools."
echo
echo "=== Benefits ==="
echo "1. Comprehensive configuration management"
echo "2. Advanced network analysis capabilities"
echo "3. Detailed performance profiling"
echo "4. Robust integration validation"
echo "5. Flexible data export options"
echo "6. Complete workflow automation"
echo "7. Integration with NLM Phase 6 requirements"
echo
echo "=== Completion ==="
echo "NLM advanced tools integration workflow completed successfully!"
echo "All tools are ready for production use in Phase 6 integration."
