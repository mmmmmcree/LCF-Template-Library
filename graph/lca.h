#ifndef LCA_H
#define LCA_H
#include "graph.h"

namespace lcf {
    template <typename Graph>
    struct chain_lca {
        using Vertex = lcf::graph::vertex;
        chain_lca(const Graph& _graph, Vertex root)
        : graph(_graph), depth(graph.size()), size(graph.size()),
        parent(graph.size()), top(graph.size()), heavy_child(graph.size(), lcf::graph::nvtx)
        {
            depth[root] = -1;
            init_dfs(root, root);
            chain_dfs(root, root);
            parent[root] = lcf::graph::nvtx;
        }
        Vertex query(Vertex u, Vertex v) {
            while (top[u] != top[v]) {
                if (depth[top[u]] < depth[top[v]]) { std::swap(u, v); }
                u = parent[top[u]];
            }
            return depth[u] < depth[v] ? u : v;
        }
        void init_dfs(Vertex vtx, Vertex pre_vtx) {\
            depth[vtx] = depth[pre_vtx] + 1;
            size[vtx] = 1;
            parent[vtx] = pre_vtx;
            for (const auto& edge : graph[vtx]) {
                Vertex child = edge.head();
                if (child == pre_vtx) { continue; }
                init_dfs(child, vtx);
                size[vtx] += size[child];
                if (size[heavy_child[vtx]] < size[child]) { heavy_child[vtx] = child; }
            }
        }
        void chain_dfs(Vertex vtx, Vertex chain_root) {
            top[vtx] = chain_root;
            if (heavy_child[vtx] == lcf::graph::nvtx) { return; }
            chain_dfs(heavy_child[vtx], chain_root);
            for (const auto& edge : graph[vtx]) {
                Vertex child = edge.head();
                if (child == heavy_child[vtx] or child == parent[vtx]) { continue; }
                chain_dfs(child, child);
            }
        }
        const Graph& graph;
        std::vector<int> depth, size;
        std::vector<Vertex> parent, top, heavy_child;
    };
}

#endif