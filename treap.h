#pragma once

#include "binary-tree.h"

#include <chrono>
#include <random>

namespace nodes {

template <typename T>
class TreapNode : public DefaultNode<T> {
public:
    inline static std::mt19937_64 rng{
        static_cast<unsigned>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        )
    };

    size_t priority;

    using base_type = DefaultNode<T>;
    using base_type::base_type;

    template <typename... Args>
    TreapNode(TreapNode<T>* left, TreapNode<T>* right, 
              TreapNode<T>* parent, Args&&... args)
            : base_type(left, right, parent, std::forward<Args>(args)...)
            , priority(rng())
    {}
};

}

template <typename T>
class Treap : public BinaryTree<T, nodes::TreapNode<T>,
                        std::allocator<nodes::TreapNode<T>>> {
    using base_type = BinaryTree<T, nodes::TreapNode<T>, std::allocator<nodes::TreapNode<T>>>;
    using base_type::base_type;

    using base_type::sentinel_node;
    using base_type::reset_sentinel;

    using base_type::create_node;
    using base_type::destroy_node;

    using typename base_type::NodeType;

public:
    // Basic functions
    using base_type::find;

    // Helpers
    using base_type::print_by_layer;

    // Iterators
    using typename base_type::iterator;
    using typename base_type::const_iterator;

    using base_type::begin;
    using base_type::end;

    // Fake node connects first, last and parent
    using BaseNode = nodes::BaseNode<nodes::TreapNode<T>>;

    // Modified functions implementation
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        NodeType* new_node = create_node(
            &sentinel_node, &sentinel_node, &sentinel_node,
            std::forward<Args>(args)...
        );
        
        if (find(new_node->value) == end()) {
            destroy_node(new_node);
            return std::make_pair(end(), false);
        }

        auto [lhs, rhs] = split(sentinel_node.parent, new_node->value);
        sentinel_node.parent = merge(lhs, merge(new_node, rhs));

        if (sentinel_node.left == &sentinel_node 
                || new_node->value < sentinel_node.left->as_derived()->value) {
            sentinel_node.left = new_node;
        }
        if (sentinel_node.right == &sentinel_node 
                || new_node->value > sentinel_node.right->as_derived()->value) {
            sentinel_node.right = new_node;
        }

        return std::make_pair(iterator(new_node, &sentinel_node), true);
    }

    std::pair<iterator, bool> insert(const T& value) {
        return emplace(value);
    }

    std::pair<iterator, bool> insert(T&& value) {
        return emplace(std::forward<T>(value));
    }

private:
    std::pair<BaseNode*, BaseNode*> split(BaseNode* ptr, const T& key) {
        if (ptr == &sentinel_node) {
            return std::make_pair(&sentinel_node, &sentinel_node);
        } else if (ptr->as_derived()->value < key) {
            auto [lhs, rhs] = split(ptr->right, key);
            ptr->right = lhs;
            if (lhs != &sentinel_node) {
                lhs->parent = ptr;
            }
            if (rhs != &sentinel_node) {
                rhs->parent = &sentinel_node;
            }
            return std::make_pair(ptr, rhs);
        } else {
            auto [lhs, rhs] = split(ptr->left, key);
            ptr->left = rhs;
            if (lhs != &sentinel_node) {
                lhs->parent = &sentinel_node;
            }
            if (rhs != &sentinel_node) {
                rhs->parent = ptr;
            }
            return std::make_pair(lhs, ptr);
        }
    }

    BaseNode* merge(BaseNode* left_ptr, BaseNode* right_ptr) {
        if (left_ptr == &sentinel_node) {
            return right_ptr;
        } else if (right_ptr == &sentinel_node) {
            return left_ptr;
        } else if (left_ptr->as_derived()->priority > right_ptr->as_derived()->prioriity) {
            left_ptr->right = merge(left_ptr->right, right_ptr);
            left_ptr->right->parent = left_ptr;
            return left_ptr;
        } else {
            right_ptr->left = merge(left_ptr, right_ptr->left);
            right_ptr->left->parent = right_ptr;
            return right_ptr;
        }
    }
};
