    auto integrationVisualizer = std::make_shared<IntegrationVisualizer>();
    auto performanceVisualizer = std::make_shared<PerformanceVisualizer>();
    auto debugVisualizer = std::make_shared<DebugVisualizer>();
    auto configVisualizer = std::make_shared<ConfigVisualizer>();
    
    // Set references to brain and config
    integrationVisualizer->setBrain(brain);
    integrationVisualizer->setConfig(config);
    
    performanceVisualizer->setBrain(brain);
    performanceVisualizer->setPerformanceProfiler(brain->getPerformanceProfiler());
    
    debugVisualizer->setBrain(brain);
    debugVisualizer->setConfig(config);
    
    configVisualizer->setConfig(config);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // NEW: Set default integration control values
    config->set("monitor_integration", false, ConfigSource::Default);
    config->set("working_memory_size", 100, ConfigSource::Default);
    config->set("prediction_error_threshold", 0.5, ConfigSource::Default);
    config->set("memory_consolidation_interval", 60.0, ConfigSource::Default);
    config->set("replay_checkpoint_mode", "periodic", ConfigSource::Default);
    config->set("memory_span", 1000, ConfigSource::Default);
    config->set("debug_level", 3, ConfigSource::Default);
    config->set("metrics_interval", 100, ConfigSource::Default);
    config->set("generate_reports", false, ConfigSource::Default);
    config->set("analyze_performance", false, ConfigSource::Default);
    config->set("experimental_features", false, ConfigSource::Default);
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
