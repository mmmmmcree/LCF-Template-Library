#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H
#include "graph.h"
#include "heap.h"
#include <tr2/dynamic_bitset>

namespace lcf {
    template<typename Graph>
    std::vector<typename Graph::edge_type::weight_type>
    bellman_ford(const Graph& graph, graph::vertex source) {
        using Edge = typename Graph::edge_type;
        using Weight = typename Edge::weight_type;
        std::vector<Weight> shortest(graph.size(), Edge::inf); shortest[source] = Weight{};
        for (int i = 0, n = graph.size(); i < n; ++i) {
            bool updated = false;
            for (graph::vertex vtx = graph.size() - 1; vtx != -1; --vtx) {
                if (shortest[vtx] == Edge::inf) { continue; }
                for (const auto& edge : graph[vtx]) {
                    auto child = edge.head();
                    Weight new_distance = shortest[vtx] + edge.value();
                    if (shortest[child] <= new_distance) { continue; }
                    shortest[child] = new_distance;
                    updated = true;
                }
            }
            if (not updated) { return shortest; }
        }
        return std::vector<Weight>{};
    }
}

namespace lcf {
    template<typename Graph>
    std::vector<typename Graph::edge_type::weight_type>
    spfa(const Graph& graph, graph::vertex source) {
        using Edge = typename Graph::edge_type;
        using Weight = typename Edge::weight_type;
        auto vtx_num = graph.size();
        std::vector<Weight> shortest(vtx_num, Edge::inf); shortest[source] = Weight{};
        std::vector<int> edge_count(vtx_num);
        std::queue<graph::vertex> queue; queue.emplace(source);
        std::tr2::dynamic_bitset<> in_queue(vtx_num, false); in_queue[source] = true;
        while (not queue.empty()) {
            graph::vertex vtx = queue.front(); queue.pop();
            in_queue[vtx] = false;
            for (const auto& edge : graph[vtx]) {
                graph::vertex child = edge.head();
                Weight new_distance = shortest[vtx] + edge.value();
                if (shortest[child] <= new_distance) { continue; }
                edge_count[child] = edge_count[vtx] + 1;
                if (edge_count[vtx] == vtx_num) { return std::vector<Weight>{}; }
                shortest[child] = new_distance;
                if (in_queue[child]) { continue; }
                queue.push(child);
                in_queue[child] = true;
            }
        }
        return shortest;
    }
}

namespace lcf {
    template<typename Graph, template <typename...> typename Heap = lcf::std_binary_heap>
    std::vector<typename Graph::edge_type::weight_type>
    dijkstra(const Graph& graph, graph::vertex source) {
        using Edge = typename Graph::edge_type;
        using Weight = typename Edge::weight_type;
        using Pair = std::pair<Weight, graph::vertex>;
        std::vector<Weight> shortest(graph.size(), Edge::inf);
        std::tr2::dynamic_bitset<> processed(graph.size());
        Heap<Pair, std::greater<Pair>> small_heap;
        shortest[source] = Weight{}; small_heap.push(std::make_pair(Weight{}, source));
        while (not small_heap.empty()) {
            auto [_, vtx] = small_heap.top(); small_heap.pop();
            if (processed[vtx]) { continue; }
            else { processed[vtx] = true; }
            for (const auto& edge : graph[vtx]) {
                graph::vertex child = edge.head();
                Weight new_distance = shortest[vtx] + edge.value();
                if (shortest[child] <= new_distance) { continue; }
                shortest[child] = new_distance;
                small_heap.push(std::make_pair(new_distance, child));
            }
        }
        return shortest;
    }
}

namespace lcf {
    //* empty graph[0] should be guaranteed to serve as virtual_vtx;
    template <typename Graph, template <typename...> typename Heap = lcf::pairing_heap>
    struct johnson {
        using Vertex = graph::vertex;
        using Weight = typename Graph::edge_type::weight_type;
        using ShortestPath = decltype(bellman_ford<Graph>);
        johnson(const Graph& g, ShortestPath shortest_path = bellman_ford<Graph>)
        : graph(g) { init(shortest_path); }
        johnson(Graph&& g, ShortestPath shortest_path = bellman_ford<Graph>)
        : graph(std::move(g)) { init(shortest_path); }
        auto operator[](graph::vertex source) {
            auto shortest = dijkstra<Graph, Heap>(graph, source);
            for (Vertex vtx = virtual_vtx + 1, n = graph.size(); vtx < n; ++vtx)
            { shortest[vtx] += h[vtx] - h[source]; }
            return  shortest;
        }
        bool has_negative_circle() const { return h.empty(); }
        void init(ShortestPath shortest_path) {
            for (Vertex i = virtual_vtx + 1, n = graph.size(); i < n; ++i)
            { graph.emplace_edge(virtual_vtx, i, Weight{}); }
            h = shortest_path(graph, virtual_vtx);
            if (h.empty()) { return; }
            for (Vertex vtx = virtual_vtx + 1, n = graph.size(); vtx < n; ++vtx) {
                for (auto& edge : graph[vtx]) {
                    auto child_vertex = edge.head();
                    edge.value() += h[vtx] - h[child_vertex];
                }
            }
        }
        static constexpr graph::vertex virtual_vtx = 0;
        Graph graph;
        std::vector<Weight> h;
    };
}

#endif