#ifndef BIPARTITE_H
#define BIPARTITE_H
#include "graph.h"
#include <tr2/dynamic_bitset>

template <typename Graph>
struct hungarian {
    using Vertex = typename Graph::vertex;
    hungarian(const Graph& g) : graph(g), visited(g.size()), matched_with(g.size(), lcf::graph::nvtx) { }
    size_t maximum_cardinality() {
        size_t max_cnt = 0;
        for (Vertex u = graph.size() - 1; u != lcf::graph::nvtx; --u) {
            visited.reset();
            if (dfs(u)) { ++max_cnt; }
        }
        return max_cnt;
    }
    bool dfs(Vertex vtx) {
        for (const auto& edge : graph[vtx]) {
            auto child = edge.head();
            if (visited[child]) { continue; }
            visited[child] = true;
            if (matched_with[child] == lcf::graph::nvtx or dfs(matched_with[child])) {
                matched_with[child] = vtx;
                return true;
            }
        }
        return false;
    }
    const Graph& graph;
    std::tr2::dynamic_bitset<> visited;
    std::vector<Vertex> matched_with;
};

#endif