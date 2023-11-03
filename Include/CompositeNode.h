#ifndef COMPOSITENODE_H
#define COMPOSITENODE_H

#include "BehaviorNode.h" // Include the base class header
#include <vector>

class CompositeNode : public BehaviorNode {
public:
    CompositeNode();
    
    // Add a child node to the composite node.
    void AddChild(BehaviorNode* child);
    
    // Reset the composite node and its children.
    virtual void Reset() override;

protected:
    std::vector<BehaviorNode*> children;
};

#endif // COMPOSITENODE_H
