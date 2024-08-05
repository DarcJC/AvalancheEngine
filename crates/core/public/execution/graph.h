#pragma once

#include "container/vector.hpp"
#include "container/vector_queue.hpp"
#include "container/shared_ptr.hpp"
#include <cstdint>
#include <concepts>
#include <atomic>

namespace avalanche::core::execution {

    template <typename T>
    class Node : public enable_shared_from_this<T> {
    public:
        using node_id_type = int64_t;

        Node() = default;
        explicit Node(node_id_type nid) : m_node_id(nid) {}

        AVALANCHE_NO_DISCARD node_id_type node_id() const {
            return m_node_id;
        }

    private:
        node_id_type m_node_id = -1;

        template <typename NodeType>
        friend class Graph;
    };

    template <typename NodeType>
    class Graph {
    public:
        using node_type = NodeType;
        using node_id_type = node_type::node_id_type;
        using size_type = size_t;

        static constexpr size_type default_reserved_node_num = 32;

        explicit Graph(size_type reserved_node_num = default_reserved_node_num)
            : m_nodes(reserved_node_num)
            , m_adjacency_to_list(reserved_node_num)
            , m_in_degree(reserved_node_num)
            , m_topological_sorting(reserved_node_num)
        {
            // Create root node
            m_nodes.push_back(new_node());
        }

        weak_ptr<node_type> new_node() {
            m_nodes.push_back(make_shared<node_type>(m_increase_id_value++));
            m_adjacency_to_list.emplace_back();
            return m_nodes.last_item();
        }

        void add_edge(const weak_ptr<node_type>& from_node, const weak_ptr<node_type>& to_node) {
            shared_ptr<node_type> from = from_node.lock();
            shared_ptr<node_type> to = to_node.lock();

            AVALANCHE_CHECK(from && to, "Trying to connect invalid nodes");

            AVALANCHE_TODO();
        }

    protected:
        bool has_cycle() {
            vector<size_type> in_degree = m_in_degree;
            AVALANCHE_TODO();
            return false;
        }

    private:
        std::atomic<node_id_type> m_increase_id_value = 0;

        // Store nodes
        vector<shared_ptr<node_type>> m_nodes;
        // Store out edges of each node
        vector<vector<node_id_type>> m_adjacency_to_list;
        // Store in-degree of each node
        vector<size_type> m_in_degree;
        // Store topological sorting
        vector<node_id_type> m_topological_sorting;
    };

}
