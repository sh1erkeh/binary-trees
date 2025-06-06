#pragma once

#include "binary-tree.h"

template <typename T>
class SplayTree : public BinaryTree<SplayTree<T>, T, nodes::BasicNode<T>> {
    using base_type = BinaryTree<SplayTree<T>, T, nodes::BasicNode<T>>;
    using base_type::base_type;

};

static node* right_rotation(node *y)
{
    node *x = y->left;
    if (x == NULL) return y;

    node *beta = x->right;
    node *parent = y->parent;

    x->parent = parent;
    x->right = y;
    y->parent = x;
    y->left = beta;

    if (beta != NULL)
    {
        beta->parent = y;
    }

    if (parent != NULL)
    {
        if (parent->left == y)
        {
            parent->left = x;
        } else
        {
            parent->right = x;
        }
    }

    return x;
}

static node* left_rotation(node *x)
{
    node *y = x->right;
    if (y == NULL) return x;

    node *beta = y->left;
    node *parent = x->parent;

    y->parent = parent;
    y->left = x;
    x->parent = y;
    x->right = beta;

    if (beta != NULL)
    {
        beta->parent = x;
    }

    if (parent != NULL)
    {
        if (parent->left == x)
        {
            parent->left = y;
        } else {
            parent->right = y;
        }
    }

    return y;
}

static node *zig(node *x)
{
    if (x == NULL) return NULL;
    node *parent = x->parent;
    if (parent == NULL) return x;

    if (parent->left == x)
    {
        return right_rotation(parent);
    } else
    {
        return left_rotation(parent);
    }
}

static node *zigzig(node *x)
{
    if (x == NULL) return NULL;
    node *parent = x->parent;
    if (parent == NULL) return x;
    node *grand = parent->parent;
    if (grand == NULL) return zig(x);

    if (grand->left == parent && parent->left == x)
    {
        grand = right_rotation(grand);
        return right_rotation(grand);
    } else if (grand->right == parent && parent->right == x)
    {
        grand = left_rotation(grand);
        return left_rotation(grand);
    }
    return x;
}

static node *zigzag(node *x)
{
    if (x == NULL) return NULL;
    node *parent = x->parent;
    if (parent == NULL) return x;
    node *grand = parent->parent;
    if (grand == NULL) return zig(x);

    if (grand->left == parent && parent->right == x)
    {
        parent = left_rotation(parent);
        grand->left = parent;
        parent->parent = grand;
        return right_rotation(grand);
    } else if (grand->right == parent && parent->left == x)
    {
        parent = right_rotation(parent);
        grand->right = parent;
        parent->parent = grand;
        return left_rotation(grand);
    }
    return x;
}

static node* splay(node *x)
{
    if (x == NULL) return NULL;

    while (x->parent != NULL)
    {
        node *parent = x->parent;
        node *grand = parent->parent;

        if (grand == NULL)
        {
            x = zig(x);
        } else
        {
            if ((grand->left == parent && parent->left == x) || (grand->right == parent && parent->right == x))
            {
                x = zigzig(x);
            } else
            {
                x = zigzag(x);
            }
        }
    }
    return x;
}

static node* insert_splay_implementation(node *root, int key)
{
    if (root == NULL)
    {
        return create_node(key, 0);
    }

    node *current = root;
    node *parent = NULL;

    while (current != NULL)
    {
        parent = current;
        if (key < current->key)
        {
            current = current->left;
        } else if (key > current->key)
        {
            current = current->right;
        } else
        {
            return splay(current);
        }
    }

    node *new_node = create_node(key, 0);
    new_node->parent = parent;

    if (key < parent->key)
    {
        parent->left = new_node;
    } else
    {
        parent->right = new_node;
    }

    return splay(new_node);
}

void insert_splay(tree_t *tree, int key)
{
    tree->root = insert_splay_implementation(tree->root, key);
}

static node* find_max(node* root)
{
    while (root && root->right)
    {
        root = root->right;
    }
    return root;
}

static node* join(node* left, node* right)
{
    if (left == NULL) return right;
    if (right == NULL) return left;

    node* max_left = find_max(left);
    left = splay(max_left);
    left->right = right;
    right->parent = left;

    return left;
}

static node* erase_splay_implementation(node* root, int key)
{
    if (root == NULL) return NULL;

    node* current = root;
    node* parent = NULL;

    while (current != NULL)
    {
        if (key < current->key)
        {
            parent = current;
            current = current->left;
        } else if (key > current->key)
        {
            parent = current;
            current = current->right;
        } else
        {
            break;
        }
    }

    if (current == NULL)
    {
        if (parent != NULL)
        {
            root = splay(parent);
        }
        return root;
    }

    root = splay(current);

    node* left_subtree = root->left;
    node* right_subtree = root->right;

    if (left_subtree != NULL)
    {
        left_subtree->parent = NULL;
    }
    if (right_subtree != NULL)
    {
        right_subtree->parent = NULL;
    }

    free(root);
    return join(left_subtree, right_subtree);
}

void erase_splay(tree_t* tree, int key)
{
    tree->root = erase_splay_implementation(tree->root, key);
}
