#include "node.h"

#include <unistd.h>

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