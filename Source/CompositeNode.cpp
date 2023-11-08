#include <vector>
#include <iostream>
#include <map>
#include <string>

#include <unistd.h>

enum class NodeStatus {
    Success,
    Fail,
    Running
};

class BlackBoard {
public:
    void SetValue(const std::string& key, const std::string& value) {
        data[key] = value;
    }

    std::string GetValue(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return "No key found matches data";
    }

    bool ContainsKey(const std::string& key) const {
        return data.find(key) != data.end();
    }

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
    CompositeNode() {}

    void AddChild(BehaviorNode* child) {
        children.push_back(child);
    }

    virtual void Display(int depth = 0) const override {
        for (int i = 0; i < depth; i++) {
            std::cout << "  "; // Indentation for better visualization
        }

        std::cout << "Composite Node" << std::endl;
        
        for (const auto& child : children) {
            child->Display(depth + 1);
        }
    }
};

class SequenceNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override {
        for (BehaviorNode* child : children) {
            NodeStatus status = child->Execute();
            if (status == NodeStatus::Fail) {
                return NodeStatus::Fail;
            } else if (status == NodeStatus::Running) {
                return NodeStatus::Running;
            }
        }

        return NodeStatus::Success;
    }

    virtual void Display(int depth = 0) const override {
        for (int i = 0; i < depth; i++) {
            std::cout << "  ";
        }
        std::cout << "Sequence Node" << std::endl;
        for (const auto& child : children) {
            child->Display(depth + 1);
        }
    }
};

class SelectorNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override {
        for (BehaviorNode* child : children) {
            NodeStatus status = child->Execute();
            if (status == NodeStatus::Success) {
                return NodeStatus::Success;
            } else if (status == NodeStatus::Running) {
                return NodeStatus::Running;
            }
        }

        return NodeStatus::Fail;
    }

    virtual void Display(int depth = 0) const override {
        for (int i = 0; i < depth; i++) {
            std::cout << "  ";
        }
        std::cout << "Selector Node" << std::endl;
        for (const auto& child : children) {
            child->Display(depth + 1);
        }
    }
};

class ActionNode : public BehaviorNode {
public:
    using ActionFunction = std::function<NodeStatus()>;
private:
    ActionFunction action_;
    BlackBoard* blackboard_;

public:
    ActionNode(ActionFunction action)
        : action_(action), blackboard_(nullptr) {}

    ActionNode(ActionFunction action, BlackBoard* blackboard)
        : action_(action), blackboard_(blackboard) {}

    virtual NodeStatus Execute() override {
        return this->action_();
    }

    virtual void Display(int depth = 0) const override {
        for (int i = 0; i < depth; i++) {
            std::cout << "  ";
        }
        std::cout << "Action Node" << std::endl;
    }
};


// class InputActionNode : public ActionNode {
// public:
//     InputActionNode(std::function<NodeStatus(std::istream& input, std::ostream& output)> inputAction)
//         : ActionNode([inputAction] {
//             return inputAction(std::cin, std::cout);
//         }) {}
// };


class FallbackNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override {
        for (BehaviorNode* child : children) {
            NodeStatus status = child->Execute();
            if (status == NodeStatus::Success) {
                return NodeStatus::Success;
            } else if (status == NodeStatus::Running) {
                return NodeStatus::Running;
            }
        }
        return NodeStatus::Fail;
    }
};

class ParallelNode : public CompositeNode {
public:
    ParallelNode(int successThreshold) : successThreshold_(successThreshold) {}

    virtual NodeStatus Execute() override {
        int successCount = 0;
        int failureCount = 0;
        int runningCount = 0;

        for (BehaviorNode* child : children) {
            NodeStatus status = child->Execute();

            if (status == NodeStatus::Success) {
                successCount++;
            } else if (status == NodeStatus::Fail) {
                failureCount++;
            } else if (status == NodeStatus::Running) {
                runningCount++;
            }
        }

        if (successCount >= successThreshold_) {
            return NodeStatus::Success;
        } else if (failureCount > children.size() - successThreshold_) {
            return NodeStatus::Fail;
        } else if (runningCount > 0) {
            return NodeStatus::Running;
        } else {
            return NodeStatus::Fail;
        }
    }

private:
    int successThreshold_;
};


int main() {
    BlackBoard blackboard;
    SelectorNode* root = new SelectorNode;

    SequenceNode* patrolSequence = new SequenceNode;

    patrolSequence->AddChild(new ActionNode([](){
        std::cout << "Patrolling..." << std::endl;
        return NodeStatus::Success;
    }));

    patrolSequence->AddChild(new ActionNode([](){
        std::cout << "Continuing patrol..." << std::endl;
        return NodeStatus::Success;
    }));

    ActionNode* detectEnemyAction = new ActionNode([&blackboard]() {
        std::string enemyDetected = blackboard.GetValue("enemyDetected");

        std::cout << "Detecting enemy..." << std::endl;
        if (enemyDetected == "true") {
            return NodeStatus::Success;
        } else {
            return NodeStatus::Fail;
        }
    });

    ActionNode* testAction = new ActionNode([]() {
        std::cout << "Testing..." << std::endl;
        return NodeStatus::Success;
    });

    ActionNode* attackAction = new ActionNode([]() {
        std::cout << "Attacking enemy..." << std::endl;
        return NodeStatus::Success;
    });

    root->AddChild(patrolSequence);
    patrolSequence->AddChild(detectEnemyAction);
    patrolSequence->AddChild(attackAction);
    patrolSequence->AddChild(testAction);

    NodeStatus result = root->Execute();

    blackboard.SetValue("enemyDetected", "true");
    sleep(5);
    root->Execute();

    if (result == NodeStatus::Success) {
        std::cout << "Behavior tree succeeded" << std::endl;
    } else if (result == NodeStatus::Fail) {
        std::cout << "Behavior tree failed." << std::endl;
    } else if (result == NodeStatus::Running) {
        std::cout << "Behavior tree is running." << std::endl;
    }

    root->Display();

    
    return 0;
}