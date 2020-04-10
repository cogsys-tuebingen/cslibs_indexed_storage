#pragma once

#include <cslibs_indexed_storage/backend/options.hpp>
#include <cslibs_indexed_storage/backend/kdtree/kdtree_options.hpp>
#include <cslibs_indexed_storage/backend/tags.hpp>
#include <cslibs_indexed_storage/backend/backend_traits.hpp>
#include <cslibs_indexed_storage/interface/data/data_interface.hpp>
#include <limits>

namespace cslibs_indexed_storage { namespace backend { namespace kdtree
{

template<template<typename> class node_allocator_t_,
        typename data_interface_t_, typename index_interface_t_, typename... options_ts_>
class GenericKDTree
{
public:
    using tag = kdtree_tag;

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
                std::swap(left->data_ptr, this->data_ptr);
                left->index = this->index;

                right->index = index;
                return right;
            }
            else
            {
                std::swap(right->data_ptr, this->data_ptr);
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
            // data_ptr is deallocated outside
        }

        inline std::size_t byte_size() const
        {
            return sizeof(*this)
                   + data_ptr ? data_if::byte_size(*data_ptr) : 0ul;
        }

        Node* left = nullptr;
        Node* right = nullptr;
        split_value_t split_value = {};
        std::size_t split_dimension = {};

        index_t index;
        data_storage_t* data_ptr = nullptr;
    };

    using node_allocator_t = node_allocator_t_<Node>;
    using storage_allocator_t = node_allocator_t_<data_storage_t>;

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
            root_->data_ptr = storage_allocator.allocate();
            *(root_->data_ptr) = data_if::create(std::forward<Args>(args)...);
            ++size_;
            return data_if::expose(*root_->data_ptr);
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
                current->data_ptr = storage_allocator.allocate();
                *(current->data_ptr) = data_if::create(std::forward<Args>(args)...);
                ++size_;
            }
            else
                data_if::template merge<on_duplicate_index_strategy>(*current->data_ptr, std::forward<Args>(args)...);

            return data_if::expose(*current->data_ptr);
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

            if(current == nullptr) {
                return nullptr;
            }
        }
        if (current->index != index)
            return nullptr;

        return &data_if::expose(*current->data_ptr);
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

        return &data_if::expose(*current->data_ptr);
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
        storage_allocator.reset();
        size_ = 0;
    }

    inline std::size_t size() const
    {
        return size_;
    }

    inline std::size_t byte_size() const
    {
        return sizeof(*this) + ((root_ == nullptr) ? 0 : byte_size(0, root_));
    }

private:
    inline std::size_t byte_size(std::size_t bytes, const Node* node) const
    {
        /*if (node->is_leaf())
            return bytes + node->byte_size();
        else
        {
            bytes += node->byte_size();
            bytes += byte_size(0, node->left);
            bytes += byte_size(0, node->right);
            return bytes;
        }*/
        if (!node->is_leaf()) {
            bytes += byte_size(0, node->left);
            bytes += byte_size(0, node->right);
        }
        return bytes + node->byte_size();
    }

    template<typename Fn>
    inline void traverse(const Fn& function, Node* node)
    {
        if (node->is_leaf())
            function(node->index, data_if::expose(*node->data_ptr));
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
            function(node->index, data_if::expose(*node->data_ptr));
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
        storage_allocator.deallocate(node->data_ptr);
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
    storage_allocator_t storage_allocator;
    Node* root_ = nullptr;
    std::size_t size_ = 0;
};

}}}

namespace cslibs_indexed_storage { namespace backend
{

template<>
struct backend_traits<kdtree_tag>
{
    static constexpr bool IsFixedSize = false;
};

}}
