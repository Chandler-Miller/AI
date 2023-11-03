class BehaviorNode {
public:
    enum class NodeStatus {
        Running,
        Success,
        Failure,
    };

    BehaviorNode() : status(NodeStatus::Running) {}

    // Virtual method to execute the behavior of the node.
    virtual NodeStatus Execute() = 0;

    // Reset the node to its initial state.
    virtual void Reset() {
        status = NodeStatus::Running;
    }

    // Get the current status of the node.
    NodeStatus GetStatus() const {
        return status;
    }

    // Check if the node is in a terminal state (Success or Failure).
    bool IsTerminal() const {
        return status == NodeStatus::Success || status == NodeStatus::Failure;
    }

protected:
    NodeStatus status;
};
