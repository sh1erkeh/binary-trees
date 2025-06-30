#pragma once

#include <vector>
#include <queue>
#include <iostream>
#include <memory>

namespace nodes {

template <typename DerivedNode>
class BaseNode {
public:
    BaseNode* left;
    BaseNode* right;
    BaseNode* parent;

    DerivedNode* as_derived() noexcept {
        return static_cast<DerivedNode*>(this);
    }

    const DerivedNode* as_derived() const noexcept {
        return static_cast<const DerivedNode*>(this);
    }

    BaseNode()
            : left{this}, right{this}, parent{this}
    {}

    BaseNode(BaseNode* left, BaseNode* right, BaseNode* parent)
            : left{left}, right{right}, parent{parent}
    {}
};

template <typename T>
class DefaultNode : public BaseNode<DefaultNode<T>> {
    using base_type = BaseNode<DefaultNode<T>>;

public:
    T value;

    template <typename... Args>
    DefaultNode(base_type* left, base_type* right,
                base_type* parent, Args&&... args)
            : base_type(left, right, parent)
            , value(std::forward<Args>(args)...)
    {}
};

} // namespace nodes

template <
    typename T,
    typename NodeType,
    typename Allocator
> class BinaryTree {
    template <bool>
    friend class BaseIterator;

protected:
    using BaseNode = nodes::BaseNode<NodeType>;
    using Node = NodeType;
    using node_allocator = typename
        std::allocator_traits<Allocator>::template rebind_alloc<NodeType>;

private:
    template <bool IsConst>
    class BaseIterator {
    public:
        using value_type = T;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        BaseIterator() = default;
        BaseIterator(const BaseIterator&) = default;
        BaseIterator& operator=(const BaseIterator&) = default;

        BaseIterator(BaseNode* node, const BinaryTree& tree) 
                : current(node), tree(tree)
        {}

        bool operator==(const BaseIterator& other) const noexcept {
            return current == other.current;
        }

        bool operator!=(const BaseIterator& other) const noexcept {
            return current != other.current;
        }

        BaseIterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        BaseIterator& operator++() {
            current = tree.find_next(current);
            return *this;
        }

        reference operator*() const {
            return current->as_derived()->value;
        }

        pointer operator->() const {
            return &(current->as_derived()->value);
        }

    private:
        const BinaryTree& tree;
        BaseNode* current;
    };

public:
    using iterator = BaseIterator<false>;
    using const_iterator = BaseIterator<true>;

    BinaryTree()
            : sentinel_node()
    {}

    ~BinaryTree() {
        clear();
    }

    BinaryTree(const BinaryTree& other) 
            : BinaryTree()
    {
        for (const auto& val : other) {
            insert(val);
        }
    }

    BinaryTree(BinaryTree&& other) noexcept
            : sentinel_node{other.sentinel_node}
    {
        other.reset_sentinel();
    }

    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            clear();
            for (const auto& val : other) {
                insert(val);
            }
        }
        return *this;
    }

    BinaryTree& operator=(BinaryTree&& other) noexcept {
        if (this != &other) {
            clear();
            sentinel_node = other.sentinel_node;
            other.reset_sentinel();
        }
        return *this;
    }

    iterator end() noexcept { 
        return iterator(&sentinel_node, *this);
    }

    iterator begin() noexcept {
        return iterator(sentinel_node.left, *this);
    }

    const_iterator end() const noexcept { 
        return const_iterator(&sentinel_node, *this);
    }

    const_iterator begin() const noexcept {
        return const_iterator(sentinel_node.left, *this);
    }

    [[nodiscard]] bool empty() const noexcept {
        return sentinel_node.parent == &sentinel_node;
    }

    iterator find(const T& value) {
        auto [ptr, is_self] = find_helper(value);

        if (is_self) {
            return iterator(ptr, &sentinel_node);
        } else {
            return end();
        }
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        auto [ptr, is_successful] = emplace_helper(std::forward<Args>(args)...);
        if (is_successful) {
            return std::make_pair(iterator(ptr, *this), true);
        } else {
            return std::make_pair(end(), false);
        }
    }

    std::pair<iterator, bool> insert(const T& value) {
        return emplace(value);
    }

    std::pair<iterator, bool> insert(T&& value) {
        return emplace(std::forward<T>(value));
    }

    bool erase(const T& value) {
        auto [ptr, is_self] = find_helper(value);
        if (!is_self) {
            return false;
        }

        if (ptr->left == &sentinel_node 
                && ptr->right == &sentinel_node) {
            if (ptr->parent == &sentinel_node) {
                reset_sentinel();
            } else if (ptr->parent->right == ptr) {
                ptr->parent->right = &sentinel_node;
                if (sentinel_node.right == ptr) {
                    sentinel_node.right = ptr->parent;
                }
            } else {
                ptr->parent->left = &sentinel_node;
                if (sentinel_node.left == ptr) {
                    sentinel_node.left = ptr->parent;
                }
            }
            destroy_node(ptr->as_derived());
        } else if (ptr->left == &sentinel_node
                || ptr->right == &sentinel_node) {
            BaseNode* child = (ptr->left != &sentinel_node) ? ptr->left : ptr->right;
            child->parent = ptr->parent;

            if (ptr->parent == &sentinel_node) {
                sentinel_node.parent = child;
            } else if (ptr->parent->left == ptr) {
                ptr->parent->left = child;
            } else {
                ptr->parent->right = child;
            }
            destroy_node(ptr->as_derived());        
        } else {
            BaseNode* successor = find_next(ptr);
            T successor_value = std::move(successor->as_derived()->value);
            erase(successor_value);
            ptr->as_derived()->value = std::move(successor_value);
        }

        return true;
    }

    void print_by_layer() const noexcept {
        std::queue<BaseNode*> nodes;
        if (sentinel_node.parent != &sentinel_node) {
            nodes.push(sentinel_node.parent);
        }

        while (!nodes.empty()) {
            BaseNode* current = nodes.front();
            nodes.pop();

            std::cout << current->as_derived()->value << ' ';

            if (current->left != &sentinel_node) {
                nodes.push(current->left);
            }
            if (current->right != &sentinel_node) {
                nodes.push(current->right);
            }
        }
        std::cout << '\n';
    }

protected:
    mutable BaseNode sentinel_node;
    [[no_unique_address]] node_allocator alloc;

    std::pair<BaseNode*, bool> find_helper(const T& value) {
        BaseNode* current = sentinel_node.parent;
        BaseNode* parent = &sentinel_node;

        while (current != &sentinel_node) {
            parent = current;
            if (current->as_derived()->value < value) {
                current = current->right;
            } else if (current->as_derived()->value > value) {
                current = current->left;
            } else {
                return std::make_pair(current, true);
            }
        }

        return std::make_pair(parent, false);
    }

    template <typename... Args>
    std::pair<BaseNode*, bool> emplace_helper(Args&&... args) {
        NodeType* new_node = create_node(
                &sentinel_node, &sentinel_node, &sentinel_node, 
                std::forward<Args>(args)...
        );
        auto [ptr, is_self] = find_helper(new_node->value);

        if (ptr == &sentinel_node) {
            sentinel_node.left = sentinel_node.right 
                = sentinel_node.parent = new_node;
        } else if (is_self) {
            destroy_node(new_node);
            return std::make_pair(ptr, false);
        } else if (ptr->as_derived()->value < new_node->value) {
            new_node->parent = ptr;
            ptr->right = new_node;
            if (sentinel_node.right == ptr) {
                sentinel_node.right = new_node;
            }
        } else {
            new_node->parent = ptr;
            ptr->left = new_node;
            if (sentinel_node.left == ptr) {
                sentinel_node.left = new_node;
            }
        }

        return std::make_pair(new_node, true);
    }

    template <typename... Args>
    NodeType* create_node(BaseNode* left, BaseNode* right,
                          BaseNode* parent, Args&&... args) {
        NodeType* new_node =
            std::allocator_traits<node_allocator>::allocate(alloc, 1);
        try {
            std::allocator_traits<node_allocator>::construct(
                alloc, new_node,
                left->as_derived(),
                right->as_derived(),
                parent->as_derived(),
                std::forward<Args>(args)...);
        } catch (...) {
            std::allocator_traits<node_allocator>::deallocate(alloc, new_node, 1);
            throw std::bad_alloc();
        }
        return new_node;
    }

    void destroy_node(NodeType* node) noexcept {
        try {
            std::allocator_traits<node_allocator>::destroy(alloc, node);
        } catch (...) {
            std::terminate();
        }
        std::allocator_traits<node_allocator>::deallocate(alloc, node, 1);
    }

    BaseNode* find_next(BaseNode* node) const noexcept {
        if (node->right != &sentinel_node) {
            node = node->right;
            while (node->left != &sentinel_node) {
                node = node->left;
            }
            return node;
        } else {
            BaseNode* parent = node->parent;
            while (parent != &sentinel_node
                    && node == parent->right) {
                node = parent;
                parent = parent->parent;
            }
            return parent;
        }
    }

    void clear() noexcept {
        if (sentinel_node.parent != &sentinel_node) {
            delete_subtree(sentinel_node.parent);
        }
        reset_sentinel();
    }

    void reset_sentinel() noexcept {
        sentinel_node.left = &sentinel_node;
        sentinel_node.right = &sentinel_node;
        sentinel_node.parent = &sentinel_node;
    }

    void delete_subtree(BaseNode* node) noexcept {
        std::queue<BaseNode*> nodes;
        nodes.push(node);

        while (!nodes.empty()) {
            BaseNode* current = nodes.front();
            nodes.pop();

            if (current != &sentinel_node) {
                nodes.push(current->left);
                nodes.push(current->right);
                destroy_node(current->as_derived());
            }
        }
    }
};

