#ifndef NODE_H
#define NODE_H

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <functional>

enum class NodeStatus {
    kSuccess,
    kFail,
    kRunning
};

class BlackBoard {
public:
    void SetValue(const std::string& kKey, const std::string& kValue);
    std::string GetValue(const std::string& kKey) const;
    bool ContainsKey(const std::string& kKey) const;

private:
    std::map<std::string, std::string> data;
};

class BehaviorNode {
public:
    virtual NodeStatus Execute() = 0;
    virtual void Display(int depth = 0) const = 0;
};

class CompositeNode : public BehaviorNode {
protected:
    std::vector<BehaviorNode*> children;

public:
    CompositeNode();
    void AddChild(BehaviorNode* child);
    virtual void Display(int depth = 0) const override;
};

class SequenceNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override;
    virtual void Display(int depth = 0) const override;
};

class SelectorNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override;
    virtual void Display(int depth = 0) const override;
};

class ActionNode : public BehaviorNode {
public:
    using ActionFunction = std::function<NodeStatus()>;

    ActionNode(ActionFunction action);
    ActionNode(ActionFunction action, BlackBoard* blackboard);

    virtual NodeStatus Execute() override;
    virtual void Display(int depth = 0) const override;

private:
    ActionFunction action_;
    BlackBoard* blackboard_;
};

class FallbackNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override;
};

class ParallelNode : public CompositeNode {
public:
    ParallelNode(int successThreshold);

    virtual NodeStatus Execute() override;

private:
    int success_threshold_;
};

#endif NODE_H
