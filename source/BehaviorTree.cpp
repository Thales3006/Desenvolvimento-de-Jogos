#include "BehaviorTree.hpp"

// --- SelectorNode --------------------------------------------------------

SelectorNode::SelectorNode(std::vector<BTNodePtr> children)
    : m_children(std::move(children)) {}

BTResult SelectorNode::tick(const BTContext& context) const {
    // Left to right: return the first child that succeeds, carrying its
    // command. If none succeed, the selector fails with no command.
    for (const auto& child : m_children) {
        BTResult result = child->tick(context);
        if (result.success) {
            return result;
        }
    }
    return {false, ""};
}

// --- SequenceNode --------------------------------------------------------

SequenceNode::SequenceNode(std::vector<BTNodePtr> children)
    : m_children(std::move(children)) {}

BTResult SequenceNode::tick(const BTContext& context) const {
    // Left to right: stop at the first failing child. Succeeds only if every
    // child succeeds, carrying the last non-empty command produced on the way.
    std::string command;
    for (const auto& child : m_children) {
        BTResult result = child->tick(context);
        if (!result.success) {
            return {false, ""};
        }
        if (!result.command.empty()) {
            command = result.command;
        }
    }
    return {true, command};
}

// --- ConditionNode -------------------------------------------------------

ConditionNode::ConditionNode(std::string name)
    : m_name(std::move(name)) {}

BTResult ConditionNode::tick(const BTContext& context) const {
    // Succeeds iff the context holds this name with a non-zero value. Carries
    // no command either way.
    for (const auto& fact : context) {
        if (fact.first == m_name) {
            return {fact.second != 0, ""};
        }
    }
    return {false, ""};
}

// --- ActionNode ----------------------------------------------------------

ActionNode::ActionNode(std::string command)
    : m_command(std::move(command)) {}

BTResult ActionNode::tick(const BTContext& /*context*/) const {
    // Always succeeds and carries its command.
    return {true, m_command};
}

// --- BehaviorTree --------------------------------------------------------

BehaviorTree::BehaviorTree(BTNodePtr root)
    : m_root(std::move(root)) {}

std::string BehaviorTree::tick(const BTContext& context) const {
    return m_root->tick(context).command;
}
