#pragma once

#include <cslibs_indexed_storage/backend/options.hpp>
#include <cslibs_indexed_storage/backend/kdtree/kdtree_options.hpp>
#include <cslibs_indexed_storage/interface/data/data_interface.hpp>
#include <limits>

namespace cslibs_indexed_storage
{
namespace backend
{
namespace kdtree
{

template<template<typename> class node_allocator_t_,
        typename data_interface_t_, typename index_interface_t_, typename... options_ts_>
class GenericKDTree
{
public:
    using data_if = data_interface_t_;
    using data_storage_t = typename data_if::storage_type;
    using data_output_t = typename data_if::output_type;

    using index_if = index_interface_t_;
    using index_t = typename index_if::type;

    using split_value_t = option::get_option<option::split_value_type_opt, options_ts_...>;
    static constexpr auto on_duplicate_index_strategy = option::get_option<option::merge_strategy_opt, options_ts_...>::value;

protected:
    struct Node
    {
        inline constexpr bool is_leaf() const { return (left == nullptr) & (right == nullptr); }
        inline constexpr bool branch_left(const index_t& index) const { return index[split_dimension] < split_value; }

        inline Node* split(Node* left, Node* right, const index_t& index)
        {
            {
                split_value_t max_delta = 0;
                for (std::size_t i = 0; i < index_if::dimensions; ++i)
                {
                    auto delta = std::abs(index_if::access(i, this->index) - index_if::access(i, index));
                    if (delta > max_delta)
                    {
                        max_delta = delta;
                        split_dimension = i;
                    }
                }

                split_value = (index_if::access(split_dimension, this->index) + index_if::access(split_dimension, index)) / split_value_t(2);
            }

            this->left = left;
            this->right = right;

            if (branch_left(this->index))
            {
                std::swap(left->data, this->data);
                left->index = this->index;

                right->index = index;
                return right;
            }
            else
            {
                std::swap(right->data, this->data);
                right->index = this->index;

                left->index = index;
                return left;
            }
        }

        inline void clear()
        {
            // left, right are cleared during deallocation
            // split_value, split_dimension are irrelevant
            index = index_t();
            //!\todo data should only be cleared when really desired by user, to avoid unneccessary operations
            data = data_storage_t();
        }

        Node* left = nullptr;
        Node* right = nullptr;
        split_value_t split_value = {};
        std::size_t split_dimension = {};

        index_t index;
        data_storage_t data;
    };

    using node_allocator_t = node_allocator_t_<Node>;

public:
    ~GenericKDTree()
    {
        clear();
    }

    template<typename... Args>
    inline data_output_t& insert(const index_t& index, Args&&... args)
    {
        if (root_ == nullptr)
        {
            root_ = allocator.allocate();
            root_->index = index;
            root_->data = data_if::create(std::forward<Args>(args)...);
            return data_if::expose(root_->data);
        }
        else
        {
            Node* current = root_;
            while (!current->is_leaf())
            {
                if (current->branch_left(index))
                    current = current->left;
                else
                    current = current->right;
            }

            if (current->index != index)
            {
                current = current->split(allocator.allocate(), allocator.allocate(), index);
                current->data = data_if::create(std::forward<Args>(args)...);
            }
            else
                data_if::template merge<on_duplicate_index_strategy>(current->data, std::forward<Args>(args)...);

            return data_if::expose(current->data);
        }
    }

    inline data_output_t* get(const index_t& index)
    {
        if (root_ == nullptr)
            return nullptr;

        Node* current = root_;
        while (!current->is_leaf())
        {
            if (current->branch_left(index))
                current = current->left;
            else
                current = current->right;
        }
        if (current->index != index)
            return nullptr;

        return &data_if::expose(current->data);
    }

    inline const data_output_t* get(const index_t& index) const
    {
        if (root_ == nullptr)
            return nullptr;

        const Node* current = root_;
        while (!current->is_leaf())
        {
            if (current->branch_left(index))
                current = current->left;
            else
                current = current->right;
        }
        if (current->index != index)
            return nullptr;

        return &data_if::expose(current->data);
    }

    template<typename Fn>
    inline void traverse(const Fn& function)
    {
        if (root_)
            traverse(function, root_);
    }

    template<typename Fn>
    inline void traverse(const Fn& function) const
    {
        if (root_)
            traverse(function, root_);
    }

    inline void clear()
    {
        if (root_)
        {
            clear(root_);
            allocator.deallocate(root_);
        }
        allocator.reset();
    }

private:
    template<typename Fn>
    inline void traverse(const Fn& function, Node* node)
    {
        if (node->is_leaf())
            function(node->index, data_if::expose(node->data));
        else
        {
            traverse(function, node->left);
            traverse(function, node->right);
        }
    }

    template<typename Fn>
    inline void traverse(const Fn& function, const Node* node) const
    {
        if (node->is_leaf())
            function(node->index, data_if::expose(node->data));
        else
        {
            traverse(function, node->left);
            traverse(function, node->right);
        }
    }

    inline void clear(Node* node)
    {
        if (!node->is_leaf())
        {
            clear(node->left);
            allocator.deallocate(node->left);
            clear(node->right);
            allocator.deallocate(node->right);
        }
        node->clear();
    }

protected:
    void set_node_allocator(node_allocator_t allocator)
    {
        if (root_ != nullptr)
            throw std::runtime_error("Allocator change not allowed with active data");

        std::swap(this->allocator, allocator);
    }

private:
    node_allocator_t allocator;
    Node* root_ = nullptr;
};

}
}
}