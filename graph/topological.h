#ifndef TOPOLOGICAL_H
#define TOPOLOGICAL_H
#include "graph.h"
#include <queue>

namespace lcf {
    template <typename Graph>
    std::vector<int> in_degree(const Graph& graph) {
        std::vector<int> in_degree(graph.size());
        for (graph::vertex i = 0, n = graph.size(); i < n; ++i)
        { for (const auto& edge : graph[i]) { ++in_degree[edge.head()]; } }
        return in_degree;
    }
}

namespace lcf {
    /*
    * if graph[0] is empty, the topological order has [0] at the front.
    * template parameter Queue can be altered to priority_queue,
    * but the circumstance of empty [0] should be considered.
    */
    template <typename Graph, typename Queue = std::queue<graph::vertex>>
    std::vector<graph::vertex> topological_order(const Graph& graph) {
        std::vector<graph::vertex> order; order.reserve(graph.size());
        auto in_degree = lcf::in_degree(graph);
        Queue queue;
        for (graph::vertex i = 0, n = graph.size(); i < n; ++i)
        { if (in_degree[i] == 0) { queue.emplace(i); } }
        while (not queue.empty()) {
            graph::vertex vtx;
            if constexpr (std::is_same_v<Queue, std::queue<graph::vertex>>)
            { vtx = queue.front(); }
            else { vtx = queue.top(); }
            queue.pop();
            for (const auto& edge : graph[vtx]) {
                auto child = edge.head();
                if (--in_degree[child] == 0) { queue.emplace(child); }
            }
            order.emplace_back(vtx);
        }
        if (order.size() < graph.size()) { return std::vector<graph::vertex>{}; }
        return order;
    }
}

#endif