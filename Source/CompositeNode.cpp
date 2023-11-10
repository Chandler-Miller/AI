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

// TODO: Make blackboard pointer optional
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

class InputActionNode : public ActionNode {
public:
    InputActionNode(std::function<NodeStatus(std::istream& input, std::ostream& output)> input_action)
        : ActionNode([input_action] {
            return input_action(std::cin, std::cout);
        }) {}
};

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
    ParallelNode(int successThreshold) : success_threshold_(successThreshold) {}

    virtual NodeStatus Execute() override {
        int success_count = 0;
        int failure_count = 0;
        int running_count = 0;

        for (BehaviorNode* child : children) {
            NodeStatus status = child->Execute();

            if (status == NodeStatus::Success) {
                success_count++;
            } else if (status == NodeStatus::Fail) {
                failure_count++;
            } else if (status == NodeStatus::Running) {
                running_count++;
            }
        }

        if (success_count >= success_threshold_) {
            return NodeStatus::Success;
        } else if (failure_count > children.size() - success_threshold_) {
            return NodeStatus::Fail;
        } else if (running_count > 0) {
            return NodeStatus::Running;
        } else {
            return NodeStatus::Fail;
        }
    }

private:
    int success_threshold_;
};

int main() {
    BlackBoard blackboard;
    SelectorNode* root = new SelectorNode;

    SequenceNode* patrol_sequence = new SequenceNode;

    patrol_sequence->AddChild(new ActionNode([](){
        std::cout << "Patrolling..." << std::endl;
        return NodeStatus::Success;
    }));

    patrol_sequence->AddChild(new ActionNode([](){
        std::cout << "Continuing patrol..." << std::endl;
        return NodeStatus::Success;
    }));

    ActionNode* detect_enemy_action = new ActionNode([&blackboard]() {
        std::string enemyDetected = blackboard.GetValue("enemyDetected");

        std::cout << "Detecting enemy..." << std::endl;
        if (enemyDetected == "true") {
            std::cout << "Enemy Found!" << std::endl;
            return NodeStatus::Success;
        } else {
            return NodeStatus::Fail;
        }
    });

    ActionNode* attack_action = new ActionNode([]() {
        std::cout << "Attacking enemy..." << std::endl;
        return NodeStatus::Success;
    });

    root->AddChild(patrol_sequence);
    patrol_sequence->AddChild(detect_enemy_action);
    patrol_sequence->AddChild(attack_action);

    NodeStatus result = root->Execute();

    blackboard.SetValue("enemyDetected", "true");
    sleep(3);
    root->Execute();

    if (result == NodeStatus::Success) {
        std::cout << "Behavior tree succeeded" << std::endl;
    } else if (result == NodeStatus::Fail) {
        std::cout << "Behavior tree failed." << std::endl;
    } else if (result == NodeStatus::Running) {
        std::cout << "Behavior tree is running." << std::endl;
    }

    // root->Display();

    
    return 0;
}