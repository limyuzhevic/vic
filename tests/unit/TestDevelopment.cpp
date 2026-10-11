// NLM Unit Testing Framework
// Development Tests

#include "framework/TestFramework.hpp"
#include "brain/DevelopmentManager.hpp"
#include "brain/LearningStage.hpp"
#include <cassert>
#include <cmath>

class TestDevelopment : public TestCase {
public:
    TestDevelopment() : TestCase("Development") {}
    
    void testDevelopmentManager() {
        DevelopmentManager dev_manager;
        
        // Should have initial development stage
        assert(dev_manager.getCurrentStage() == LearningStage::BEGINNER);
        
        // Should be able to advance stages
        dev_manager.advanceStage();
        assert(dev_manager.getCurrentStage() == LearningStage::INTERMEDIATE);
        
        dev_manager.advanceStage();
        assert(dev_manager.getCurrentStage() == LearningStage::ADVANCED);
        
        dev_manager.advanceStage();
        assert(dev_manager.getCurrentStage() == LearningStage::EXPERT);
    }
    
    void testSkillProgression() {
        DevelopmentManager dev_manager;
        
        // Track skill development
        std::vector<double> skill_scores = {0.3, 0.5, 0.7, 0.9, 1.0};
        std::vector<SkillType> skills = {
            SkillType::NEURAL_MODELS,
            SkillType::ALGORITHMS,
            SkillType::OPTIMIZATION,
            SkillType::SYSTEMS,
            SkillType::INTEGRATION
        };
        
        for (size_t i = 0; i < skills.size(); ++i) {
            dev_manager.developSkill(skills[i], skill_scores[i]);
            assert(dev_manager.getSkillLevel(skills[i]) >= skill_scores[i]);
        }
    }
    
    void testAdaptiveLearning() {
        DevelopmentManager dev_manager;
        
        // Test adaptive learning based on performance
        std::vector<double> performance_metrics = {0.5, 0.7, 0.8, 0.9, 0.95};
        
        for (size_t i = 0; i < performance_metrics.size(); ++i) {
            dev_manager.updateLearningProgress(performance_metrics[i]);
            
            // Learning rate should adjust based on performance
            double learning_rate = dev_manager.getOptimalLearningRate();
            assert(learning_rate > 0.0 && learning_rate <= 1.0);
            
            // Performance should improve over time
            double progress = dev_manager.getDevelopmentProgress();
            assert(progress >= i * 0.19); // Should be increasing
        }
    }
    
    void testDevelopmentBenchmarks() {
        DevelopmentManager dev_manager;
        
        // Benchmark development milestones
        double start_time = dev_manager.getDevelopmentStartTime();
        double current_time = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        assert(current_time >= start_time);
        
        // Check if development goals are being met
        std::vector<std::string> milestones = {
            "neuron_modeling",
            "synaptic_dynamics", 
            "brain_networks",
            "cognition",
            "behavior"
        };
        
        for (const auto& milestone : milestones) {
            assert(dev_manager.hasMilestoneBeenAchieved(milestone));
        }
    }
    
    void testCrossDomainIntegration() {
        DevelopmentManager dev_manager;
        
        // Test integration of different neural domains
        std::map<std::string, std::vector<double>> domain_performance = {
            {"neuroscience", {0.8, 0.85, 0.9}},
            {"ai_machine_learning", {0.7, 0.75, 0.8}},
            {"biology", {0.6, 0.65, 0.7}}
        };
        
        for (const auto& domain : domain_performance) {
            dev_manager.integrateDomainPerformance(domain.first, domain.second);
            
            double integrated_performance = dev_manager.getDomainPerformance(domain.first);
            assert(integrated_performance >= domain.second.back());
        }
    }
};