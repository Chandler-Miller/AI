#include <vector>
#include <iostream>
#include <map>
#include <string>

#include <unistd.h>

enum class NodeStatus {
    kSuccess,
    kFail,
    kRunning
};

class BlackBoard {
public:
    void SetValue(const std::string& kKey, const std::string& kValue) 
    {
        data[kKey] = kValue;
    }

    std::string GetValue(const std::string& kKey) const 
    {
        auto it = data.find(kKey);
        if (it != data.end()) {
            return it->second;
        }
        return "No key found matches data";
    }

    bool ContainsKey(const std::string& kKey) const 
    {
        return data.find(kKey) != data.end();
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

    void AddChild(BehaviorNode* child) 
    {
        children.push_back(child);
    }

    virtual void Display(int depth = 0) const override 
    {
        for (int i = 0; i < depth; i++) 
        {
            std::cout << "  "; // Indentation for better visualization
        }

        std::cout << "Composite Node" << std::endl;
        
        for (const auto& kChild : children) 
        {
            kChild->Display(depth + 1);
        }
    }
};

class SequenceNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override 
    {
        for (BehaviorNode* child : children) 
        {
            NodeStatus status = child->Execute();
            if (status == NodeStatus::kFail) 
            {
                return NodeStatus::kFail;
            } else if (status == NodeStatus::kRunning) 
            {
                return NodeStatus::kRunning;
            }
        }

        return NodeStatus::kSuccess;
    }

    virtual void Display(int depth = 0) const override 
    {
        for (int i = 0; i < depth; i++) 
        {
            std::cout << "  ";
        }
        std::cout << "Sequence Node" << std::endl;
        for (const auto& kChild : children) 
        {
            kChild->Display(depth + 1);
        }
    }
};

class SelectorNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override 
    {
        for (BehaviorNode* child : children) 
        {
            NodeStatus status = child->Execute();
            if (status == NodeStatus::kSuccess) 
            {
                return NodeStatus::kSuccess;
            } else if (status == NodeStatus::kRunning) 
            {
                return NodeStatus::kRunning;
            }
        }

        return NodeStatus::kFail;
    }

    virtual void Display(int depth = 0) const override 
    {
        for (int i = 0; i < depth; i++) 
        {
            std::cout << "  ";
        }
        std::cout << "Selector Node" << std::endl;
        for (const auto& kChild : children) 
        {
            kChild->Display(depth + 1);
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

    virtual NodeStatus Execute() override 
    {
        return this->action_();
    }

    virtual void Display(int depth = 0) const override 
    {
        for (int i = 0; i < depth; i++) 
        {
            std::cout << "  ";
        }
        std::cout << "Action Node" << std::endl;
    }
};

class FallbackNode : public CompositeNode {
public:
    virtual NodeStatus Execute() override 
    {
        for (BehaviorNode* child : children) 
        {
            NodeStatus status = child->Execute();
            if (status == NodeStatus::kSuccess) 
            {
                return NodeStatus::kSuccess;
            } else if (status == NodeStatus::kRunning) 
            {
                return NodeStatus::kRunning;
            }
        }
        return NodeStatus::kFail;
    }
};

class ParallelNode : public CompositeNode {
public:
    ParallelNode(int successThreshold) : success_threshold_(successThreshold) {}

    virtual NodeStatus Execute() override 
    {
        int success_count = 0;
        int failure_count = 0;
        int running_count = 0;

        for (BehaviorNode* child : children) {
            NodeStatus status = child->Execute();

            if (status == NodeStatus::kSuccess) 
            {
                success_count++;
            } else if (status == NodeStatus::kFail) 
            {
                failure_count++;
            } else if (status == NodeStatus::kRunning) 
            {
                running_count++;
            }
        }

        if (success_count >= success_threshold_) 
        {
            return NodeStatus::kSuccess;
        } else if (failure_count > children.size() - success_threshold_) 
        {
            return NodeStatus::kFail;
        } else if (running_count > 0) 
        {
            return NodeStatus::kRunning;
        } else 
        {
            return NodeStatus::kFail;
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
        return NodeStatus::kSuccess;
    }));

    patrol_sequence->AddChild(new ActionNode([](){
        std::cout << "Continuing patrol..." << std::endl;
        return NodeStatus::kSuccess;
    }));

    ActionNode* detect_enemy_action = new ActionNode([&blackboard]() {
        std::string enemyDetected = blackboard.GetValue("enemyDetected");

        std::cout << "Detecting enemy..." << std::endl;
        if (enemyDetected == "true") 
        {
            std::cout << "Enemy Found!" << std::endl;
            return NodeStatus::kSuccess;
        } else 
        {
            return NodeStatus::kFail;
        }
    });

    ActionNode* attack_action = new ActionNode([]() {
        std::cout << "Attacking enemy..." << std::endl;
        return NodeStatus::kSuccess;
    });

    root->AddChild(patrol_sequence);
    patrol_sequence->AddChild(detect_enemy_action);
    patrol_sequence->AddChild(attack_action);

    NodeStatus result = root->Execute();

    blackboard.SetValue("enemyDetected", "true");
    sleep(3);
    root->Execute();

    if (result == NodeStatus::kSuccess) 
    {
        std::cout << "Behavior tree succeeded" << std::endl;
    } else if (result == NodeStatus::kFail) 
    {
        std::cout << "Behavior tree failed." << std::endl;
    } else if (result == NodeStatus::kRunning) 
    {
        std::cout << "Behavior tree is running." << std::endl;
    }

    // root->Display();

    
    return 0;
}