#include "tree_scene.hpp"
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <algorithm>

TreeScene::TreeScene(QObject *parent)
    : QGraphicsScene(parent) {
    stepTimer_ = new QTimer(this);
    connect(stepTimer_, &QTimer::timeout, this, &TreeScene::onStep);
}

void TreeScene::setRootNode(const Node* root) {
    clear();
    nodeItems_.clear();
    edgeItems_.clear();
    textItems_.clear();
    walkers_.clear();
    rootNode_ = root;
    
    if (rootNode_) {
        drawTree(rootNode_, 0, 0, 400); // Initial spacing 400
    }
}

void TreeScene::drawTree(const Node* node, double x, double y, double hSpacing) {
    if (!node) return;

    // Draw edges first so they are behind nodes
    if (node->getLeftChild()) {
        double childX = x - hSpacing;
        double childY = y + 100;
        QGraphicsLineItem* line = addLine(x, y, childX, childY, QPen(Qt::black));
        line->setZValue(-1);
        drawTree(node->getLeftChild(), childX, childY, hSpacing / 2);
    }
    if (node->getRightChild()) {
        double childX = x + hSpacing;
        double childY = y + 100;
        QGraphicsLineItem* line = addLine(x, y, childX, childY, QPen(Qt::black));
        line->setZValue(-1);
        drawTree(node->getRightChild(), childX, childY, hSpacing / 2);
    }

    // Calculate Radius based on samples
    int samples = node->getNumberSamples();
    double t = std::min(1.0, (double)samples / MAX_SAMPLES_FOR_SCALE);
    double r = MIN_RADIUS + t * (MAX_RADIUS - MIN_RADIUS);

    QGraphicsEllipseItem* circle = addEllipse(x - r, y - r, 2 * r, 2 * r);
    circle->setBrush(QBrush(Qt::white));
    circle->setPen(QPen(Qt::black));
    circle->setZValue(1);
    
    nodeItems_[node->getId()] = circle;

    // Text
    QString text;
    if (node->getIsLeaf()) {
        text = QString("Leaf\nImp: %1\nSamples: %2").arg(const_cast<Node*>(node)->getImpurity(), 0, 'f', 2).arg(samples);
    } else {
        text = QString("Feat: %1\nVal: %2\nSamples: %3").arg(node->getFeatureIndex()).arg(node->getClassifierValue(), 0, 'f', 2).arg(samples);
    }
    
    QGraphicsSimpleTextItem* textItem = addSimpleText(text);
    textItem->setPos(x - r, y - r * 2.5);
    textItem->setZValue(2);
    textItems_[node->getId()] = textItem;
}

void TreeScene::updateNodeVisuals() {
    if (rootNode_) {
        updateNodeRecursive(rootNode_);
    }
}

void TreeScene::updateNodeRecursive(const Node* node) {
    if (!node) return;
    
    if (nodeItems_.count(node->getId())) {
        QGraphicsEllipseItem* circle = dynamic_cast<QGraphicsEllipseItem*>(nodeItems_[node->getId()]);
        QGraphicsSimpleTextItem* textItem = textItems_[node->getId()];
        
        if (circle && textItem) {
            // Update Radius
            int samples = node->getNumberSamples();
            double t = std::min(1.0, (double)samples / MAX_SAMPLES_FOR_SCALE);
            double r = MIN_RADIUS + t * (MAX_RADIUS - MIN_RADIUS);
            
            QRectF currentRect = circle->rect();
            double cx = currentRect.center().x();
            double cy = currentRect.center().y();
            
            circle->setRect(cx - r, cy - r, 2 * r, 2 * r);
            
            // Update Text
            QString text;
            if (node->getIsLeaf()) {
                text = QString("Leaf\nImp: %1\nSamples: %2").arg(const_cast<Node*>(node)->getImpurity(), 0, 'f', 2).arg(samples);
            } else {
                text = QString("Feat: %1\nVal: %2\nSamples: %3").arg(node->getFeatureIndex()).arg(node->getClassifierValue(), 0, 'f', 2).arg(samples);
            }
            textItem->setText(text);
            textItem->setPos(cx - r, cy - r * 2.5);
        }
    }
    
    updateNodeRecursive(node->getLeftChild());
    updateNodeRecursive(node->getRightChild());
}

void TreeScene::startTraversal(const Node* root, const DataContainer& data) {
    if (!root) return;
    
    walkers_.push_back({root, data, false});
    
    if (!stepTimer_->isActive()) {
        stepTimer_->start(200); // Faster steps (200ms)
    }
    
    // Highlight root immediately if it's the first one?
    // Actually onStep will handle highlighting
    updateNodeVisuals(); // Ensure visuals are up to date
}

void TreeScene::onStep() {
    if (walkers_.empty()) {
        stepTimer_->stop();
        // Clear highlights
        for (auto& pair : nodeItems_) {
            QGraphicsEllipseItem* item = dynamic_cast<QGraphicsEllipseItem*>(pair.second);
            if (item) item->setBrush(QBrush(Qt::white));
        }
        return;
    }

    // Clear all highlights first
    for (auto& pair : nodeItems_) {
        QGraphicsEllipseItem* item = dynamic_cast<QGraphicsEllipseItem*>(pair.second);
        if (item) item->setBrush(QBrush(Qt::white));
    }

    // Advance walkers
    for (auto it = walkers_.begin(); it != walkers_.end(); ) {
        Walker& w = *it;
        
        if (w.finished) {
            it = walkers_.erase(it);
            continue;
        }

        // Highlight current node
        if (nodeItems_.count(w.currentNode->getId())) {
            QGraphicsEllipseItem* item = dynamic_cast<QGraphicsEllipseItem*>(nodeItems_[w.currentNode->getId()]);
            if (item) item->setBrush(QBrush(Qt::red));
        }

        if (w.currentNode->getIsLeaf()) {
            w.finished = true;
            emit traversalFinished(w.currentNode->getId());
        } else {
            // Move to next
            double input = w.data.getFeatures().at(w.currentNode->getFeatureIndex());
            if (input >= w.currentNode->getClassifierValue()) {
                w.currentNode = w.currentNode->getRightChild();
            } else {
                w.currentNode = w.currentNode->getLeftChild();
            }
            emit traversalStep(w.currentNode->getId());
        }
        ++it;
    }
    
    // Re-highlight current nodes (in case multiple walkers are in same node, last one wins, but all are red)
    for (const auto& w : walkers_) {
        if (!w.finished && nodeItems_.count(w.currentNode->getId())) {
             QGraphicsEllipseItem* item = dynamic_cast<QGraphicsEllipseItem*>(nodeItems_[w.currentNode->getId()]);
             if (item) item->setBrush(QBrush(Qt::red));
        }
    }
}
