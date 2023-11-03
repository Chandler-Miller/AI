#ifndef BEHAVIORNODE_H
#define BEHAVIORNODE_H

class BehaviorNode {
public:
    enum class NodeStatus {
        Running,
        Success,
        Failure,
    };

    BehaviorNode();

    virtual NodeStatus Execute() = 0;

    virtual void Reset();

    NodeStatus GetStatus() const;
    
    bool IsTerminal() const;

protected:
    NodeStatus status;
};

#endif // BEHAVIORNODE_H
