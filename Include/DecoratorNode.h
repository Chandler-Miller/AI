#ifndef DECORATORNODE_H
#define DECORATORNODE_H

#include "BehaviorNode.h" // Include the base class header

class DecoratorNode : public BehaviorNode {
public:
    DecoratorNode(BehaviorNode* child);
    
    // Reset the decorator node and its child.
    virtual void Reset() override;

protected:
    BehaviorNode* child;
};

#endif // DECORATORNODE_H
