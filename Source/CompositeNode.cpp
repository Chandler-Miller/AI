#include "BehaviorNode.h"
#include <vector>

class CompositeNode : public BehaviorNode {
public:
    CompositeNode() {}

    // Add a child node to the composite node.
    void AddChild(BehaviorNode* child) {
        children.push_back(child);
    }

    // Reset the composite node and its children.
    virtual void Reset() override {
        BehaviorNode::Reset();
        for (BehaviorNode* child : children) {
            child->Reset();
        }
    }

protected:
    std::vector<BehaviorNode*> children;
};
