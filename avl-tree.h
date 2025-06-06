#pragma once

#include <stack>
#include <iostream>
#include <memory>

#include "binary-tree.h"

namespace nodes {

template <typename T>
class AVLNode : public DefaultNode<T> {
public:
    size_t height = 1;

    using base_type = DefaultNode<T>;
    using base_type::base_type;

    AVLNode(AVLNode<T>* left, AVLNode<T>* right, 
              AVLNode<T>* parent, const T& value)
        : base_type(left, right, parent, value)
    {}

    AVLNode(AVLNode<T>* left, AVLNode<T>* right, 
              AVLNode<T>* parent, T&& value)
        : base_type(left, right, parent, std::move(value))
    {}
};

} // namespace nodes

template <typename T>
class AVLTree : public BinaryTree<T, nodes::AVLNode<T>, std::allocator<T>> {
public:
    using base_type = BinaryTree<T, nodes::AVLNode<T>, std::allocator<T>>;
    using base_type::base_type;
    
    // Basic functions
    using base_type::find;

    // Helpers
    using base_type::print_by_layer;

    // Iterators
    using typename base_type::iterator;
    using typename base_type::const_iterator;

    using base_type::begin;
    using base_type::end;

    // Fake node connects first, last and root
    using BaseNode = nodes::BaseNode<nodes::AVLNode<T>>;

    // Modified functions
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        auto result = base_type::emplace(std::forward<Args>(args)...);
        if (result.second) {
            auto* node = result.first.raw();
            rebalance(node);
        }
        return result;
    }

    std::pair<iterator, bool> insert(const T& value) {
        return emplace(value);
    }

    std::pair<iterator, bool> insert(T&& value) {
        return emplace(std::forward<T>(value));
    }

private:
    void update_height(BaseNode* node) {
        int left_height = (node->left != &this->sentinel_node) 
            ? node->left->as_derived()->height : 0;
        int right_height = (node->right != &this->sentinel_node) 
            ? node->right->as_derived()->height : 0;
        node->as_derived()->height = 1 + std::max(left_height, right_height);
    }

    int get_balance(BaseNode* node) {
        int left_height = (node->left != &this->sentinel_node) 
            ? node->left->as_derived()->height : 0;
        int right_height = (node->right != &this->sentinel_node) 
            ? node->right->as_derived()->height : 0;
        return left_height - right_height;
    }

    void rebalance(BaseNode* node) {
        while (node != &this->sentinel_node) {
            update_height(node);
            int balance = get_balance(node);

            if (balance > 1) {
                if (get_balance(node->left) >= 0) {
                    rotate_right(node);
                } else {
                    rotate_left(node->left);
                    rotate_right(node);
                }
            } else if (balance < -1) {
                if (get_balance(node->right) <= 0) {
                    rotate_left(node);
                } else {
                    rotate_right(node->right);
                    rotate_left(node);
                }
            }
            node = node->parent;
        }
    }

    void rotate_left(BaseNode* x) {
        BaseNode* y = x->right;
        x->right = y->left;
        
        if (y->left != &this->sentinel_node) {
            y->left->parent = x;
        }
        
        y->parent = x->parent;
        
        if (x->parent == &this->sentinel_node) {
            this->sentinel_node.parent = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
        
        update_height(x);
        update_height(y);
    }

    void rotate_right(BaseNode* y) {
        BaseNode* x = y->left;
        y->left = x->right;
        
        if (x->right != &this->sentinel_node) {
            x->right->parent = y;
        }
        
        x->parent = y->parent;
        
        if (y->parent == &this->sentinel_node) {
            this->sentinel_node.parent = x;
        } else if (y == y->parent->right) {
            y->parent->right = x;
        } else {
            y->parent->left = x;
        }
        
        x->right = y;
        y->parent = x;
        
        update_height(y);
        update_height(x);
    }
};
