#include "BehaviorNode.h"

class DecoratorNode : public BehaviorNode {
public:
    DecoratorNode(BehaviorNode* child) : child(child) {}

    // Reset the decorator node and its child.
    virtual void Reset() override {
        BehaviorNode::Reset();
        child->Reset();
    }

protected:
    BehaviorNode* child;
};
