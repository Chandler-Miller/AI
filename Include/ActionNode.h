#ifndef ACTIONNODE_H
#define ACTIONNODE_H

#include "BehaviorNode.h" // Include the base class header

class ActionNode : public BehaviorNode {
public:
    ActionNode();
    
    // Override the Execute method to define the specific behavior.
    virtual NodeStatus Execute() override;

private:
    // Implement the action logic specific to this action node.
    bool PerformAction();
};

#endif // ACTIONNODE_H
