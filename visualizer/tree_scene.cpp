#include "tree_scene.hpp"
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPathItem>
#include <QPainterPath>
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
            
            // Pie Chart for Leaves
            if (node->getIsLeaf() && samples > 0) {
                // Remove existing pie slices if any (hacky: we need to track them or just clear children of the circle if we made it a group, 
                // but circle is an Item. We can make slices children of the circle item!)
                qDeleteAll(circle->childItems());
                
                const auto& counts = node->getClassCounts();
                double startAngle = 0.0;
                
                // Color map
                std::unordered_map<std::string, QColor> colors;
                colors["Iris-setosa"] = Qt::red;
                colors["Iris-versicolor"] = Qt::green;
                colors["Iris-virginica"] = Qt::blue;
                
                for (const auto& [label, count] : counts) {
                    if (count == 0) continue;
                    double spanAngle = (double)count / samples * 360.0;
                    
                    QPainterPath path;
                    path.moveTo(r, r); // Center of circle (relative to item rect 0,0 is top-left, so center is r,r)
                    // Actually circle item coords:
                    // The circle item's rect is set to (cx-r, cy-r, 2r, 2r).
                    // Its local coordinates: (0,0) is top-left of the bounding rect? No, 0,0 is the pos.
                    // We haven't setPos on the circle, we setRect. So the circle is drawn at cx-r, cy-r in scene coords.
                    // If we add child items, they are relative to parent.
                    // Let's just draw on top in scene coords? No, children is better.
                    // If circle is at scene 0,0 (default pos) but draws at rect (cx-r...), then children at 0,0 are at scene 0,0.
                    // Better: Set circle pos to (cx, cy) and rect to (-r, -r, 2r, 2r).
                    // But we didn't do that in drawTree.
                    // Let's just use scene coords for slices or use a group.
                    // Simpler: Just add slices to scene and track them? Too complex.
                    // Let's use the child item approach but be careful with coordinates.
                    // Circle item is at (0,0) scene pos (default). It draws at (cx-r, cy-r).
                    // So child item at (cx, cy) would be at center.
                    
                    QPainterPath slicePath;
                    slicePath.moveTo(cx, cy);
                    slicePath.arcTo(cx - r, cy - r, 2 * r, 2 * r, startAngle, spanAngle);
                    slicePath.closeSubpath();
                    
                    QGraphicsPathItem* slice = new QGraphicsPathItem(slicePath, circle); // Parent is circle
                    // Wait, if parent is circle, coordinates are relative to circle.
                    // Circle is at 0,0. So relative = scene.
                    // So (cx, cy) is correct.
                    
                    QColor color = colors.count(label) ? colors[label] : Qt::gray;
                    slice->setBrush(QBrush(color));
                    slice->setPen(Qt::NoPen);
                    slice->setZValue(0.5); // On top of white circle background
                    
                    startAngle += spanAngle;
                }
            } else {
                 qDeleteAll(circle->childItems());
            }
            
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
