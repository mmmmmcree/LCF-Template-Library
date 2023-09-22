#ifndef GRAPH_H
#define GRAPH_H
#include <vector>
namespace lcf {
    static constexpr size_t npos = -1;
    struct unweighted_edge {
        using node = size_t;
        unweighted_edge(node _to, node _next)
        : to(_to), next(_next) {}
        node to;
        node next;
    };
    struct weighted_edge {
        using node = size_t;
        using weight_type = long long;
        weighted_edge(node _to, node _next, weight_type w)
        : to(_to), next(_next), weight(w) {}
        node to;
        node next;
        weight_type weight;
    };
    struct query_edge {
        using node = size_t;
        query_edge(node _to, node _next, size_t i)
        : to(_to), next(_next), index(i) {}
        node to;
        node next;
        size_t index;
    };
    template<typename EdgeType>
    struct graph {
        using node = size_t;
        using edge_type = EdgeType;
        graph(size_t nodes_num, size_t edges_num = 128) : heads(nodes_num, npos)
        {
            edges.reserve(edges_num);
        }
        template<typename... Args>
        void add_edge(node from, node to, Args&&... args) noexcept {
            edges.emplace_back(to, heads[from], std::forward<Args>(args)...);
            heads[from] = edges.size() - 1;
        }
    // private:
        std::vector<node> heads;
        std::vector<edge_type> edges;
    };
}
#endif