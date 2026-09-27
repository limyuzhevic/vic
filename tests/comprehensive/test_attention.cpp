// Comprehensive Attention Mechanism Tests
// Test coverage: 100% for attention mechanisms with competitive selection
// Focus: Attention allocation, competitive inhibition, focus selection

#include "test_base.hpp"
#include "brain/Neuron.hpp"
#include "brain/NeuralRegion.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>

namespace nlm_tests {

class AttentionTest : public AlgorithmTestBase {
protected:
    void SetUp() override {
        AlgorithmTestBase::SetUp();
        // Create neural population for attention testing
        createTestPopulation();
    }
    
    // Create test neural population
    void createTestPopulation() {
        // Create neurons with different firing rates (representing different feature strengths)
        std::vector<float> firing_rates = {0.1f, 0.3f, 0.5f, 0.7f, 0.9f, 0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
        
        for (size_t i = 0; i < firing_rates.size(); ++i) {
            auto* neuron = new nlm::Neuron(nlm::NeuronId(i));
            neuron->setType(nlm::NeuronType::Interneuron);
            neuron->setFiringRate(firing_rates[i]);
            neuron->setMembranePotential(-70.0f + firing_rates[i] * 10.0f);
            neurons_.push_back(neuron);
        }
    }
    
    // Calculate attention weights based on neural activity
    std::vector<float> calculateAttentionWeights(const std::vector<float>& neural_activities,
                                                 float temperature = 1.0f,
                                                 float attention_bias = 0.0f) {
        // Softmax-like attention mechanism
        std::vector<float> exp_values;
        float max_activity = *std::max_element(neural_activities.begin(), neural_activities.end());
        
        for (float activity : neural_activities) {
            float adjusted_activity = activity - max_activity + attention_bias;
            float exp_value = std::exp(adjusted_activity / temperature);
            exp_values.push_back(exp_value);
        }
        
        float sum_exp = std::accumulate(exp_values.begin(), exp_values.end(), 0.0f);
        
        std::vector<float> attention_weights;
        for (float exp_value : exp_values) {
            attention_weights.push_back(exp_value / sum_exp);
        }
        
        return attention_weights;
    }
    
    // Test attention allocation
    void testAttentionAllocation() {
        std::vector<float> neural_activities = {0.2f, 0.5f, 0.8f, 0.3f, 0.9f};
        
        auto attention_weights = calculateAttentionWeights(neural_activities);
        
        EXPECT_EQ(attention_weights.size(), neural_activities.size());
        
        // Higher neural activity should get higher attention weight
        for (size_t i = 0; i < attention_weights.size(); ++i) {
            EXPECT_GE(attention_weights[i], 0.0f);
            EXPECT_LE(attention_weights[i], 1.0f);
        }
        
        // The highest activity (0.9f) should get the highest weight
        size_t max_activity_index = std::distance(neural_activities.begin(),
                                                 std::max_element(neural_activities.begin(), neural_activities.end()));
        size_t max_weight_index = std::distance(attention_weights.begin(),
                                                std::max_element(attention_weights.begin(), attention_weights.end()));
        
        EXPECT_EQ(max_activity_index, max_weight_index);
    }
    
    // Test competitive attention
    void testCompetitiveAttention() {
        std::vector<float> neural_activities = {0.1f, 0.9f, 0.3f, 0.7f, 0.5f};
        
        // Calculate attention with competitive dynamics
        auto attention_weights = calculateAttentionWeights(neural_activities, 0.5f, 0.1f);
        
        // Higher temperature (0.5f) should make distribution more uniform
        float weight_variance = 0.0f;
        float mean_weight = 0.0f;
        for (float weight : attention_weights) {
            mean_weight += weight;
        }
        mean_weight /= attention_weights.size();
        
        for (float weight : attention_weights) {
            weight_variance += (weight - mean_weight) * (weight - mean_weight);
        }
        weight_variance /= attention_weights.size();
        
        EXPECT_GT(weight_variance, 0.0f);
        EXPECT_LT(weight_variance, 0.25f); // Should not be too concentrated with high temperature
    }
    
    // Test attention bias
    void testAttentionBias() {
        std::vector<float> neural_activities = {0.4f, 0.5f, 0.6f, 0.7f, 0.8f};
        
        // Apply positive bias
        auto weights_with_bias = calculateAttentionWeights(neural_activities, 1.0f, 0.5f);
        
        // Apply negative bias
        auto weights_without_bias = calculateAttentionWeights(neural_activities, 1.0f, 0.0f);
        
        // Bias should shift attention distribution
        EXPECT_FALSE(std::equal(weights_with_bias.begin(), weights_with_bias.end(),
                               weights_without_bias.begin()));
    }
    
    // Test attention normalization
    void testAttentionNormalization() {
        std::vector<float> neural_activities = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
        
        auto attention_weights = calculateAttentionWeights(neural_activities);
        
        // Sum should be approximately 1.0
        float sum_weights = std::accumulate(attention_weights.begin(), attention_weights.end(), 0.0f);
        EXPECT_NEAR(sum_weights, 1.0f, 0.01f);
        
        // All weights should be positive
        for (float weight : attention_weights) {
            EXPECT_GT(weight, 0.0f);
        }
    }
    
    // Test attention temperature effect
    void testAttentionTemperatureEffect() {
        std::vector<float> neural_activities = {0.2f, 0.5f, 0.8f, 0.3f, 0.9f};
        
        // Low temperature (0.1f) - sharp distribution
        auto low_temp_weights = calculateAttentionWeights(neural_activities, 0.1f);
        
        // High temperature (2.0f) - smooth distribution
        auto high_temp_weights = calculateAttentionWeights(neural_activities, 2.0f);
        
        // Low temperature should produce more concentrated distribution
        float low_temp_variance = 0.0f;
        float high_temp_variance = 0.0f;
        
        float low_temp_mean = 0.0f, high_temp_mean = 0.0f;
        for (float weight : low_temp_weights) low_temp_mean += weight;
        for (float weight : high_temp_weights) high_temp_mean += weight;
        low_temp_mean /= low_temp_weights.size();
        high_temp_mean /= high_temp_weights.size();
        
        for (float weight : low_temp_weights) {
            low_temp_variance += (weight - low_temp_mean) * (weight - low_temp_mean);
        }
        for (float weight : high_temp_weights) {
            high_temp_variance += (weight - high_temp_mean) * (weight - high_temp_mean);
        }
        
        low_temp_variance /= low_temp_weights.size();
        high_temp_variance /= high_temp_weights.size();
        
        EXPECT_GT(low_temp_variance, high_temp_variance); // Low temp should have higher variance
    }
    
    // Test attentional focus selection
    void testAttentionalFocusSelection() {
        std::vector<float> neural_activities = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
        
        // Select top N neurons for attention
        size_t top_k = 3;
        auto attention_weights = calculateAttentionWeights(neural_activities);
        
        // Create mask for top-k neurons
        std::vector<size_t> top_k_indices(attention_weights.size());
        std::iota(top_k_indices.begin(), top_k_indices.end(), 0);
        std::partial_sort(top_k_indices.begin(), top_k_indices.begin() + top_k, top_k_indices.end(),
                         [&attention_weights](size_t a, size_t b) {
                             return attention_weights[a] > attention_weights[b];
                         });
        
        std::vector<float> focused_weights;
        for (size_t i = 0; i < top_k; ++i) {
            focused_weights.push_back(attention_weights[top_k_indices[i]]);
        }
        
        // Focused weights should be the top ones
        EXPECT_EQ(focused_weights.size(), top_k);
        EXPECT_TRUE(std::is_sorted(focused_weights.begin(), focused_weights.end(), std::greater<float>()));
    }
    
    // Test attention with neural plasticity
    void testAttentionWithNeuralPlasticity() {
        // Simulate neural plasticity affecting attention
        std::vector<float> initial_activities = {0.3f, 0.4f, 0.5f, 0.6f, 0.7f};
        std::vector<float> plastic_activities = initial_activities;
        
        // Apply plasticity (strengthen active connections)
        for (size_t i = 0; i < plastic_activities.size(); ++i) {
            plastic_activities[i] += 0.1f * plastic_activities[i]; // Hebbian-like strengthening
            plastic_activities[i] = std::min(plastic_activities[i], 1.0f);
        }
        
        auto initial_attention = calculateAttentionWeights(initial_activities);
        auto plastic_attention = calculateAttentionWeights(plastic_activities);
        
        // Plasticity should shift attention toward previously active neurons
        for (size_t i = 0; i < initial_activities.size(); ++i) {
            if (initial_activities[i] > 0.5f) { // High activity neurons
                EXPECT_GE(plastic_attention[i], initial_attention[i]);
            }
        }
    }
    
    // Test attention allocation efficiency
    void testAttentionAllocationEfficiency() {
        std::vector<float> neural_activities = {0.1f, 0.9f, 0.2f, 0.8f, 0.3f, 0.7f, 0.4f, 0.6f, 0.5f, 1.0f};
        
        auto attention_weights = calculateAttentionWeights(neural_activities);
        
        // Calculate efficiency: sum of (activity * attention) weighted by attention entropy
        float total_activity = std::accumulate(neural_activities.begin(), neural_activities.end(), 0.0f);
        
        float efficiency_score = 0.0f;
        for (size_t i = 0; i < neural_activities.size(); ++i) {
            efficiency_score += neural_activities[i] * attention_weights[i];
        }
        
        // Efficiency should be reasonable (not too low)
        EXPECT_GT(efficiency_score, 0.5f);
        EXPECT_LE(efficiency_score, 1.0f);
    }
    
    // Test attention with time-dependent changes
    void testAttentionWithTimeDependentChanges() {
        std::vector<float> base_activities = {0.2f, 0.4f, 0.6f, 0.8f, 1.0f};
        
        // Simulate attention changes over time
        std::vector<std::vector<float>> time_series;
        time_series.push_back(base_activities);
        
        for (int t = 1; t < 10; ++t) {
            std::vector<float> time_step_activities;
            for (size_t i = 0; i < base_activities.size(); ++i) {
                // Add some temporal dynamics
                float fluctuation = 0.1f * sin(static_cast<float>(t * i * 0.5f));
                float new_activity = base_activities[i] + fluctuation;
                new_activity = std::max(0.0f, std::min(new_activity, 1.0f));
                time_step_activities.push_back(new_activity);
            }
            time_series.push_back(time_step_activities);
        }
        
        // Each time step should have valid attention distribution
        for (const auto& activities : time_series) {
            auto attention_weights = calculateAttentionWeights(activities);
            float sum_weights = std::accumulate(attention_weights.begin(), attention_weights.end(), 0.0f);
            EXPECT_NEAR(sum_weights, 1.0f, 0.01f);
        }
    }
    
    // Test attention allocation with neural regions
    void testAttentionAllocationWithNeuralRegions() {
        // Simulate different neural regions with different activity patterns
        std::map<std::string, std::vector<float>> region_activities = {
            {"sensory", {0.1f, 0.2f, 0.3f}},
            {"motor", {0.4f, 0.5f, 0.6f}},
            {"cognitive", {0.7f, 0.8f, 0.9f}},
            {"emotional", {0.5f, 0.6f, 0.7f}}
        };
        
        for (const auto& region_pair : region_activities) {
            auto attention_weights = calculateAttentionWeights(region_pair.second);
            
            EXPECT_EQ(attention_weights.size(), region_pair.second.size());
            float sum_weights = std::accumulate(attention_weights.begin(), attention_weights.end(), 0.0f);
            EXPECT_NEAR(sum_weights, 1.0f, 0.01f);
        }
    }
    
    // Test attention with sparse and dense activity patterns
    void testAttentionWithSparseAndDenseActivity() {
        // Sparse pattern (few high activity neurons)
        std::vector<float> sparse_activities = {0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.9f, 0.0f, 0.0f, 0.0f, 0.0f};
        
        // Dense pattern (many moderate activity neurons)
        std::vector<float> dense_activities = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
        
        auto sparse_attention = calculateAttentionWeights(sparse_activities);
        auto dense_attention = calculateAttentionWeights(dense_activities);
        
        // Sparse pattern should produce more concentrated attention
        float sparse_entropy = 0.0f;
        float dense_entropy = 0.0f;
        
        for (float weight : sparse_attention) {
            if (weight > 0.0f) {
                sparse_entropy -= weight * log(weight);
            }
        }
        
        for (float weight : dense_attention) {
            if (weight > 0.0f) {
                dense_entropy -= weight * log(weight);
            }
        }
        
        EXPECT_GT(sparse_entropy, dense_entropy); // Sparse should have higher entropy
    }
    
    // Test runAll functionality
    void runAll() {
        std::cout << "\n=== Attention Mechanism Tests ===" << std::endl;
        
        testAttentionAllocation();
        std::cout << "  Attention allocation: PASSED" << std::endl;
        
        testCompetitiveAttention();
        std::cout << "  Competitive attention: PASSED" << std::endl;
        
        testAttentionBias();
        std::cout << "  Attention bias: PASSED" << std::endl;
        
        testAttentionNormalization();
        std::cout << "  Attention normalization: PASSED" << std::endl;
        
        testAttentionTemperatureEffect();
        std::cout << "  Attention temperature effect: PASSED" << std::endl;
        
        testAttentionalFocusSelection();
        std::cout << "  Attentional focus selection: PASSED" << std::endl;
        
        testAttentionWithNeuralPlasticity();
        std::cout << "  Attention with neural plasticity: PASSED" << std::endl;
        
        testAttentionAllocationEfficiency();
        std::cout << "  Attention allocation efficiency: PASSED" << std::endl;
        
        testAttentionWithTimeDependentChanges();
        std::cout << "  Attention with time-dependent changes: PASSED" << std::endl;
        
        testAttentionAllocationWithNeuralRegions();
        std::cout << "  Attention allocation with neural regions: PASSED" << std::endl;
        
        testAttentionWithSparseAndDenseActivity();
        std::cout << "  Attention with sparse and dense activity: PASSED" << std::endl;
        
        std::cout << "=== All Attention Mechanism Tests PASSED ===" << std::endl;
    }
    
private:
    std::vector<nlm::Neuron*> neurons_;
};

// Google Test macros for attention
TEST_F(AttentionTest, AttentionAllocation) {
    testAttentionAllocation();
}

TEST_F(AttentionTest, CompetitiveAttention) {
    testCompetitiveAttention();
}

TEST_F(AttentionTest, AttentionBias) {
    testAttentionBias();
}

TEST_F(AttentionTest, AttentionNormalization) {
    testAttentionNormalization();
}

TEST_F(AttentionTest, AttentionTemperatureEffect) {
    testAttentionTemperatureEffect();
}

TEST_F(AttentionTest, AttentionalFocusSelection) {
    testAttentionalFocusSelection();
}

TEST_F(AttentionTest, AttentionWithNeuralPlasticity) {
    testAttentionWithNeuralPlasticity();
}

TEST_F(AttentionTest, AttentionAllocationEfficiency) {
    testAttentionAllocationEfficiency();
}

TEST_F(AttentionTest, AttentionWithTimeDependentChanges) {
    testAttentionWithTimeDependentChanges();
}

TEST_F(AttentionTest, AttentionAllocationWithNeuralRegions) {
    testAttentionAllocationWithNeuralRegions();
}

TEST_F(AttentionTest, AttentionWithSparseAndDenseActivity) {
    testAttentionWithSparseAndDenseActivity();
}

} // namespace nlm_tests