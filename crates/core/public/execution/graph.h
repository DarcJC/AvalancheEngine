#pragma once

#include "avalanche_core_export.h"
#include "container/exception.hpp"
#include "container/vector.hpp"
#include "container/vector_queue.hpp"
#include "container/shared_ptr.hpp"
#include <cstdint>
#include <concepts>
#include <atomic>

namespace avalanche::core::execution {

    struct cycle_detected final : simple_error {
        AVALANCHE_CORE_INTERNAL explicit cycle_detected();
    };

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
        using node_id_type = int64_t;
        using size_type = size_t;

        static constexpr size_type default_reserved_node_num = 32;

        explicit Graph(size_type reserved_node_num = default_reserved_node_num)
            : m_nodes(reserved_node_num)
            , m_adjacency_to_list(reserved_node_num)
            , m_in_degree(reserved_node_num)
            , m_topological_sorting(reserved_node_num)
        {
            // Create root node
            new_node();
        }

        shared_ptr<node_type> new_node() {
            m_nodes.push_back(make_shared<node_type>(m_increase_id_value++));
            m_adjacency_to_list.emplace_back();
            m_in_degree.emplace_back(0);
            return m_nodes.last_item();
        }

        AVALANCHE_NO_DISCARD bool is_node_exist(node_id_type nid) const {
            return m_nodes.is_valid_index(nid) && m_nodes[nid] && m_nodes[nid]->node_id() == nid;
        }

        void add_edge(const shared_ptr<node_type>& from, const shared_ptr<node_type>& to) {
            AVALANCHE_CHECK(from && to, "Trying to connect invalid nodes");

            const node_id_type nid_from = from->node_id();
            const node_id_type nid_to = to->node_id();

            AVALANCHE_CHECK(!m_adjacency_to_list[nid_from].contains(nid_to), "Edge already exists");

            m_adjacency_to_list[nid_from].push_back(nid_to);
            ++m_in_degree[nid_to];

            // Memory barrier to ensure all modifications are visible to the next operations
            std::atomic_thread_fence(std::memory_order_release);

            if (has_cycle()) {
                m_adjacency_to_list[nid_from].remove_last();
                --m_in_degree[nid_to];
                raise_exception(cycle_detected{});
            }

            // Memory barrier to ensure all read are performed
            std::atomic_thread_fence(std::memory_order_acquire);

            topological_sort();
        }

        shared_ptr<node_type> get_node_from_id(node_id_type nid) const {
            AVALANCHE_CHECK(is_node_exist(nid), "Invalid node id");
            return m_nodes[nid];
        }

        shared_ptr<node_type> default_root_node() const {
            return get_node_from_id(0);
        }

    protected:
        AVALANCHE_NO_DISCARD bool has_cycle() const {
            vector<size_type> in_degree = m_in_degree;
            vector_queue<node_id_type> zero_in_degree_queue{};

            for (int i = 0; i < in_degree.size(); ++i) {
                AVALANCHE_CHECK(in_degree.is_valid_index(i), "Graph::has_cycle(): Internal state is not consistency");
                if (in_degree[i] == 0) {
                    zero_in_degree_queue.emplace_back(i);
                }
            }

            size_type visited_nodes = 0;
            while (!zero_in_degree_queue.queue_is_empty()) {
                const node_id_type nid = zero_in_degree_queue.queue_pop_front();
                visited_nodes++;

                for (const node_id_type next_nid : m_adjacency_to_list[nid]) {
                    in_degree[next_nid]--;
                    if (in_degree[next_nid] == 0) {
                        zero_in_degree_queue.emplace_back(next_nid);
                    }
                }
            }

            return visited_nodes != in_degree.size();
        }

        void topological_sort() {
            vector<size_type> in_degree = m_in_degree;
            vector_queue<node_id_type> zero_in_degree_queue{};
            m_topological_sorting.clear();

            for (size_type i = 0; i < in_degree.size(); ++i) {
                if (in_degree[i] == 0) {
                    zero_in_degree_queue.emplace_back(i);
                }
            }

            while (!zero_in_degree_queue.queue_is_empty()) {
                const node_id_type nid = zero_in_degree_queue.queue_pop_front();
                m_topological_sorting.emplace_back(nid);

                for (const node_id_type next_nid : m_adjacency_to_list[nid]) {
                    in_degree[next_nid]--;
                    if (in_degree[next_nid] == 0) {
                        zero_in_degree_queue.emplace_back(next_nid);
                    }
                }
            }
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
