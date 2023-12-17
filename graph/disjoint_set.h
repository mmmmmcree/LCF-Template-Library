#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H
#include "graph.h"

namespace lcf {
    class disjoint_set {
    public:
        using vertex = graph::vertex;
        disjoint_set(size_t size) : parent(size), rank(size) { init(); }
        void init() { for (vertex i = 0; auto& vtx : parent)  { vtx = i++; } }
        void make_union(vertex u, vertex v) {
            auto u_root = find_root(u), v_root = find_root(v);
            if (rank[u_root] < rank[v_root]) { parent[u_root] = v_root; }
            else {
                parent[v_root] = u_root;
                if (rank[v_root] == rank[u_root]) { ++rank[u_root]; }
            }
        }
        bool same_set(vertex u, vertex v) { return find_root(u) == find_root(v); }
        void make_set(vertex vtx) { parent[vtx] = vtx; }
        bool is_root(vertex vtx) const { return parent[vtx] == vtx; }
        vertex find_root(vertex vtx) { return parent[vtx] == vtx ? vtx : parent[vtx] = find_root(parent[vtx]); }
        auto begin() { return parent.begin(); }
        auto begin() const { return parent.begin(); }
        auto end() { return parent.end(); }
        auto end() const { return parent.end(); }
    private:
        std::vector<vertex> parent;
        std::vector<int> rank;
    };
};

#endif // DISJOINT_SET_H