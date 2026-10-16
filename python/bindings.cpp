// Visualization and debugging tools
    m.def("visualizeBrain", [](const std::shared_ptr<Brain>& brain) {
        std::ostringstream oss;
        oss << "=== Brain Visualization ===" << std::endl;
        oss << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        oss << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        oss << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        oss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        oss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        oss << "Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        oss << "Excitatory/Inhibitory Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        oss << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        return oss.str();
    }, py::arg("brain"), "Generate a human-readable visualization of brain state");
    
    m.def("visualizeWorld", [](const std::shared_ptr<SimpleWorld>& world) {
        std::ostringstream oss;
        oss << "=== World Visualization ===" << std::endl;
        oss << "Width: " << world->getWidth() << std::endl;
        oss << "Height: " << world->getHeight() << std::endl;
        oss << "Simulation Time: " << world->getSimulationTime() << std::endl;
        oss << "Max Energy: " << world->getMaxEnergy() << std::endl;
        oss << "Energy Decay Rate: " << world->getEnergyDecayRate() << std::endl;
        oss << "Random Seed: " << world->getRandomSeed() << std::endl;
        oss << "Agent Position: (" << world->getAgentBody().x << ", " << world->getAgentBody().y 
            << "), Orientation: " << world->getAgentBody().orientation << std::endl;
        oss << "Agent Energy: " << world->getAgentBody().energy << std::endl;
        oss << "Agent Health: " << world->getAgentBody().health << std::endl;
        return oss.str();
    }, py::arg("world"), "Generate a human-readable visualization of world state");
    
    m.def("visualizeAgent", [](const std::shared_ptr<AgentBrain>& agent) {
        std::ostringstream oss;
        oss << "=== Agent Visualization ===" << std::endl;
        oss << "Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
        oss << "Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
        oss << "Novelty Level: " << agent->getNoveltyLevel() << std::endl;
        oss << "Prediction Error: " << agent->getPredictionError() << std::endl;
        oss << "Developmental Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
        oss << "Reward Modulation: " << (agent->isRewardModulationEnabled() ? "Enabled" : "Disabled") << std::endl;
        oss << "Structural Plasticity: " << (agent->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
        oss << "Development: " << (agent->isDevelopmentEnabled() ? "Enabled" : "Disabled") << std::endl;
        oss << "Curiosity: " << (agent->isCuriosityEnabled() ? "Enabled" : "Disabled") << std::endl;
        oss << "Brain Healthy: " << (agent->isHealthy() ? "Yes" : "No") << std::endl;
        return oss.str();
    }, py::arg("agent"), "Generate a human-readable visualization of agent state");
    
    m.def("debugStep", [](Brain& brain, SimulationStep step, Timestamp time) {
        std::ostringstream oss;
        oss << "=== Debug Step Information ===" << std::endl;
        oss << "Step: " << step << ", Time: " << time << std::endl;
        oss << "Before step - Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        oss << "Before step - Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        
        brain.step(step, time);
        
        oss << "After step - Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        oss << "After step - Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        oss << "After step - Active neurons: " << brain.getActiveNeuronCount() << std::endl;
        return oss.str();
    }, py::arg("brain"), py::arg("step"), py::arg("time"), "Debug a single simulation step and return detailed information");
    
    m.def("debugAgentStep", [](AgentBrain& agent, SimpleWorld& world, size_t step) {
        std::ostringstream oss;
        oss << "=== Debug Agent Step ===" << std::endl;
        oss << "Step: " << step << std::endl;
        
        world.update(0.1);
        auto percept = world.getSensoryPercept();
        oss << "Sensory Input - Vision: " << percept.getVisionWidth() << "x" << percept.getVisionHeight() << std::endl;
        oss << "Sensory Input - Internal signals: " << percept.getInternal().size() << std::endl;
        
        agent.processSensoryInput(percept);
        agent.getBrain()->step(step);
        auto action = agent.decodeMotorCommand();
        oss << "Decoded Action Type: " << static_cast<int>(action.getType()) << std::endl;
        
        world.applyMotorCommand(action, world.getSimulationTime());
        oss << "Agent Stats - Neuromodulation: " << agent.getNeuromodulationLevel() << std::endl;
        oss << "Agent Stats - Curiosity: " << agent.getCuriosityLevel() << std::endl;
        
        return oss.str();
    }, py::arg("agent"), py::arg("world"), py::arg("step"), "Debug a single agent step and return detailed information");
    
    m.def("profileSimulation", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict results;
        results["total_steps"] = steps;
        results["total_time_ms"] = duration.count();
        results["time_per_step_ms"] = duration.count() / static_cast<double>(steps);
        results["steps_per_second"] = steps / (duration.count() / 1000.0);
        results["final_firing_rate"] = brain->getAverageFiringRate();
        results["total_spikes"] = brain->getTotalSpikeCount();
        
        return results;
    }, py::arg("brain"), py::arg("steps"), "Profile simulation performance and return metrics");
    
    m.def("profileAgentEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                                 size_t max_steps) {
        world->reset();
        agent->initialize(*world);
        
        auto start = std::chrono::high_resolution_clock::now();
        float total_reward = 0.0f;
        size_t steps_completed = 0;
        
        for (size_t step = 0; step < max_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            agent->getBrain()->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto internal = percept.getInternal();
            float reward = !internal.empty() ? internal[0] : 0.0f;
            agent->applyRewardModulation(reward, 0.0f);
            agent->updateDevelopment(0.1f);
            
            total_reward += reward;
            steps_completed++;
            if (total_reward >= 100.0f) break; // Stop if reward goal reached
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict results;
        results["steps_completed"] = steps_completed;
        results["total_reward"] = total_reward;
        results["total_time_ms"] = duration.count();
        results["time_per_step_ms"] = duration.count() / static_cast<double>(steps_completed);
        results["steps_per_second"] = steps_completed / (duration.count() / 1000.0);
        results["final_curiosity"] = agent->getCuriosityLevel();
        results["final_novelty"] = agent->getNoveltyLevel();
        
        return results;
    }, py::arg("agent"), py::arg("world"), py::arg("max_steps"), "Profile agent episode and return performance metrics");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm