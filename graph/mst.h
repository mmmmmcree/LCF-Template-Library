#ifndef MST_H
#define MST_H
#include "graph.h"
#include "heap.h"
#include <tr2/dynamic_bitset>

namespace lcf {
    // return [edge_num, mst_weight]
    template <typename Graph, template <typename> typename CmpFunctor = std::less>
    std::pair<size_t, typename Graph::edge_type::weight_type>
    kruskal(const Graph& graph) {
        using Weight = typename Graph::edge_type::weight_type;
        using Edge = graph::weighted_edge<Weight>;
        auto edges = switch_to_edges<Edge>(graph);
        size_t edge_cnt = 0;
        Weight result{};
        disjoint_set set(graph.size());
        std::sort(edges.begin(), edges.end(), CmpFunctor<Edge>{});
        for (const auto& [u, v, w] : edges) {
            if (set.same_set(u, v)) { continue; }
            set.make_union(u, v);
            result += w; ++edge_cnt;
        }
        return std::make_pair(edge_cnt, result);
    }
}

namespace lcf {
    // return [edge_num, mst_weight]
    template <typename Graph, template <typename> typename CmpFunctor = std::less>
    std::pair<size_t, typename Graph::edge_type::weight_type>
    boruvka(const Graph& graph) {
        using Weight = typename Graph::edge_type::weight_type;
        using Edge = graph::weighted_edge<Weight>;
        auto edges = switch_to_edges<Edge>(graph);
        disjoint_set set(graph.size());
        std::vector<int> shortest(graph.size(), -1); //* store the index(value) of the shortest out edge of one set(index);
        std::tr2::dynamic_bitset<> used(edges.size());
        size_t edge_cnt = 0;
        Weight result{};
        auto replaceable = [&edges, cmp = CmpFunctor<Edge>{}](int idx, int shortest_idx) {
            if (shortest_idx == -1) { return true; }
            return cmp(edges[idx], edges[shortest_idx]);
        };
        while (true) {
            bool updated = false;
            for (int idx = -1; const auto& [u, v, w] : edges) {
                if (used[++idx]) { continue; }
                auto u_set = set.find_root(u), v_set = set.find_root(v);
                if (u_set == v_set) { continue; }
                if (replaceable(idx, shortest[u_set])) { shortest[u_set] = idx; }
                if (replaceable(idx, shortest[v_set])) { shortest[v_set] = idx; }
            }
            for (auto idx : shortest) {
                const auto& [u, v, w] = edges[idx];
                if (used[idx] or idx == -1) { continue; }
                set.make_union(u, v);
                result += w; ++edge_cnt;
                used[idx] = updated = true;
            }
            if (not updated) { break; }
            std::fill(shortest.begin(), shortest.end(), -1);
        }
        return std::make_pair(edge_cnt, result);
    }
}

namespace lcf {
    // return [edge_num, mst_weight]
    template <typename Graph,
        template <typename> typename CmpFunctor = std::greater,
        template <typename...> typename Heap = lcf::std_binary_heap>
    std::pair<size_t, typename Graph::edge_type::weight_type>
    prim(const Graph& graph) {
        using Weight = typename Graph::edge_type::weight_type;
        using Pair = std::pair<Weight, graph::vertex>;
        std::tr2::dynamic_bitset<> processed(graph.size());
        Heap<Pair, CmpFunctor<Pair>> heap;
        heap.push(std::make_pair(Weight{}, graph.size() - 1));
        size_t edge_cnt = -1;
        Weight result{};
        while (not heap.empty()) {
            auto [weight, vtx] = heap.top(); heap.pop();
            if (processed[vtx]) { continue; }
            for (const auto& edge : graph[vtx]) {
                auto child = edge.head();
                if (processed[child]) { continue; }
                heap.push(std::make_pair(edge.value(), child));
            }
            result += weight; ++edge_cnt;
            processed[vtx] = true;
        }
        return std::make_pair(edge_cnt, result);
    }
}

#endif