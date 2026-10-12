#include "DashboardVisualizer.hpp"
#include <iostream>

namespace nlm {

struct DashboardVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    bool visible;
    int width;
    int height;
    std::string layout;
    size_t gridColumns;
    size_t gridRows;
    int widgetSpacing;
    bool autoRefresh;
    double refreshInterval;
    bool showStatusBar;
    bool minimizeUnseenWidgets;
    std::vector<std::shared_ptr<VisualizationComponent>> widgets;
    
    Impl() : visible(false), width(800), height(600), layout("grid"), gridColumns(2), gridRows(2),
             widgetSpacing(10), autoRefresh(true), refreshInterval(1.0), showStatusBar(true),
             minimizeUnseenWidgets(false) {}
};

DashboardVisualizer::DashboardVisualizer() : pImpl(std::make_unique<Impl>()) {}

bool DashboardVisualizer::initialize(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    pImpl->brain = brain;
    pImpl->visible = true;
    return true;
}

void DashboardVisualizer::update() {
    // Update all dashboard widgets
    for (auto& widget : pImpl->widgets) {
        if (widget) {
            widget->update();
        }
    }
}

void DashboardVisualizer::render() {
    if (!pImpl->visible) return;
    
    std::cout << "Rendering Dashboard Visualization:" << std::endl;
    std::cout << "  Layout: " << pImpl->layout << std::endl;
    std::cout << "  Grid: " << pImpl->gridColumns << " x " << pImpl->gridRows << std::endl;
    std::cout << "  Widget spacing: " << pImpl->widgetSpacing << " pixels" << std::endl;
    std::cout << "  Widgets count: " << pImpl->widgets.size() << std::endl;
    std::cout << "  Auto refresh: " << (pImpl->autoRefresh ? "yes" : "no") << std::endl;
    std::cout << "  Refresh interval: " << pImpl->refreshInterval << " seconds" << std::endl;
    std::cout << "  Status bar shown: " << (pImpl->showStatusBar ? "yes" : "no") << std::endl;
    std::cout << "  Minimize unseen widgets: " << (pImpl->minimizeUnseenWidgets ? "yes" : "no") << std::endl;
}

void DashboardVisualizer::setVisible(bool visible) {
    pImpl->visible = visible;
}

bool DashboardVisualizer::isVisible() const {
    return pImpl->visible;
}

void DashboardVisualizer::setSize(int width, int height) {
    pImpl->width = width;
    pImpl->height = height;
}

void DashboardVisualizer::addWidget(std::shared_ptr<VisualizationComponent> widget) {
    if (widget) {
        pImpl->widgets.push_back(widget);
    }
}

void DashboardVisualizer::removeWidget(std::shared_ptr<VisualizationComponent> widget) {
    for (size_t i = 0; i < pImpl->widgets.size(); ++i) {
        if (pImpl->widgets[i] == widget) {
            pImpl->widgets.erase(pImpl->widgets.begin() + i);
            break;
        }
    }
}

void DashboardVisualizer::clearWidgets() {
    pImpl->widgets.clear();
}

void DashboardVisualizer::setLayout(const std::string& layout) {
    pImpl->layout = layout;
}

void DashboardVisualizer::setGridColumns(size_t columns) {
    pImpl->gridColumns = columns;
}

void DashboardVisualizer::setGridRows(size_t rows) {
    pImpl->gridRows = rows;
}

void DashboardVisualizer::setWidgetSpacing(int spacing) {
    pImpl->widgetSpacing = spacing;
}

void DashboardVisualizer::setAutoRefresh(bool enable) {
    pImpl->autoRefresh = enable;
}

void DashboardVisualizer::setRefreshInterval(double interval) {
    pImpl->refreshInterval = interval;
}

void DashboardVisualizer::setShowStatusBar(bool show) {
    pImpl->showStatusBar = show;
}

void DashboardVisualizer::setMinimizeUnseenWidgets(bool minimize) {
    pImpl->minimizeUnseenWidgets = minimize;
}

} // namespace nlm