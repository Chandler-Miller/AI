#include "node.h"
#include "gtest/gtest.h"

TEST(SequenceNodeTest, Execute) {
    // Create a sequence with two children, the second one always fails
    SequenceNode sequenceNode;
    ActionNode actionNodeSuccess([]() { return NodeStatus::kSuccess; });
    ActionNode actionNodeFail([]() { return NodeStatus::kFail; });

    sequenceNode.AddChild(&actionNodeSuccess);
    sequenceNode.AddChild(&actionNodeFail);

    // The sequence should fail because the second child fails
    EXPECT_EQ(sequenceNode.Execute(), NodeStatus::kFail);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}