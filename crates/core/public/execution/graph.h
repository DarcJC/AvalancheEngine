#pragma once

#include "container/vector.hpp"
#include "container/shared_ptr.hpp"

namespace avalanche::core::execution {

    struct Node {
        vector<weak_ptr<Node>> parents;
        vector<shared_ptr<Node>> children;
    };

    class Graph {
    private:
        shared_ptr<Node> m_root = nullptr;
    };

}
