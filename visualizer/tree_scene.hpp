#pragma once

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QObject>
#include <QTimer>
#include <unordered_map>
#include "../decision_tree/node.hpp"
#include "../data_container/data_container.hpp"

class TreeScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit TreeScene(QObject *parent = nullptr);
    void setRootNode(const Node* root);
    void highlightNode(int nodeId, bool active);
    void clearHighlight();
    
    // Helper to visualize traversal
    void startTraversal(const Node* root, const DataContainer& data);
    void updateNodeVisuals();

signals:
    void traversalStep(int nodeId);
    void traversalFinished(int finalNodeId);

private slots:
    void onStep();

private:
    void drawTree(const Node* node, double x, double y, double hSpacing);
    void updateNodeRecursive(const Node* node);
    
    const Node* rootNode_ = nullptr;
    std::unordered_map<int, QGraphicsItem*> nodeItems_;
    std::unordered_map<int, QGraphicsSimpleTextItem*> textItems_;
    std::unordered_map<int, QGraphicsItem*> edgeItems_; 
    
    struct Walker {
        const Node* currentNode;
        DataContainer data;
        bool finished;
    };
    
    std::vector<Walker> walkers_;
    QTimer* stepTimer_;
    
    // Visual constants
    const double MIN_RADIUS = 15.0;
    const double MAX_RADIUS = 40.0;
    const int MAX_SAMPLES_FOR_SCALE = 150; // Adjust based on dataset size
};
