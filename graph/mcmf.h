#ifndef MCMF_H
#define MCMF_H
#include "graph.h"
#include "heap.h"
#include <tr2/dynamic_bitset>
#include <ranges>

namespace lcf {
    template <typename Graph>
    struct danic_mcmf {
        using Vertex = typename Graph::vertex;
        using Weight = typename Graph::edge_type::weight_type;
        using Flow = typename Weight::first_type;
        using Cost = typename Weight::second_type;
        static constexpr Flow inf_flow = std::numeric_limits<Flow>::max() >> 2;
        static constexpr Cost inf_cost = std::numeric_limits<Cost>::max() >> 2;
        danic_mcmf(Graph& g, Vertex s, Vertex t)
        : residual_graph(g), source(s), terminal(t),
        cheapest(g.size()), bitset(g.size()),
        maximum_flow(0), minimum_cost(0)
        { build_residual_graph(); }
        void build_residual_graph() {
            while (spfa()) {
                Flow flow = dfs_update(source, inf_flow);
                maximum_flow += flow;
                minimum_cost += cheapest[terminal] * flow;
            }
        }
        bool spfa() {
            std::ranges::fill(cheapest, inf_cost); cheapest[source] = 0;
            std::queue<Vertex> queue; queue.push(source); bitset.flip(source);
            while (not queue.empty()) {
                Vertex vtx = queue.front(); queue.pop(); bitset[vtx] = false;
                for (const auto& edge : residual_graph[vtx]) {
                    auto child = edge.head();
                    auto [capacity, cost] = edge.value();
                    if (not capacity) { continue; }
                    Cost new_cost = cheapest[vtx] + cost;
                    if (new_cost >= cheapest[child]) { continue; }
                    cheapest[child] = new_cost;
                    if (not bitset[child]) { queue.push(child); bitset.flip(child); }
                }
            }
            return cheapest[terminal] != inf_cost;
        }
        Flow dfs_update(Vertex vtx, Flow rest_flow) {
            if (vtx == terminal) { return rest_flow; }
            bitset.flip(vtx);
            Flow total_out_flow = 0;
            for (auto iter = residual_graph.begin(vtx), end = residual_graph.end(); iter != end; ++iter) {
                auto child = (*iter).head();
                auto& [capacity, cost] = (*iter).value();
                if (bitset[child] or not capacity or cheapest[child] != cheapest[vtx] + cost) { continue; }
                Flow out_flow = dfs_update(child, std::min(rest_flow, capacity));
                if (not out_flow) { continue; }
                auto& [rev_capacity, _] = (~iter).value();
                capacity -= out_flow; rev_capacity += out_flow;
                rest_flow -= out_flow; total_out_flow += out_flow;
                if (rest_flow == 0) { break; }
            }
            bitset.flip(vtx);
            return total_out_flow;
        }
        Graph& residual_graph;
        Vertex source, terminal;
        std::vector<Cost> cheapest;
        std::tr2::dynamic_bitset<> bitset;
        Flow maximum_flow;
        Cost minimum_cost;
    };
}

namespace lcf {
    template <typename Graph>
    struct primal_dual_mcmf {
        using Vertex = typename Graph::vertex;
        using Weight = typename Graph::edge_type::weight_type;
        using Flow = typename Weight::first_type;
        using Cost = typename Weight::second_type;
        static constexpr Flow inf_flow = std::numeric_limits<Flow>::max() >> 2;
        static constexpr Cost inf_cost = std::numeric_limits<Cost>::max() >> 2;
        primal_dual_mcmf(Graph& g, Vertex s, Vertex t)
        : residual_graph(g), source(s), terminal(t),
        cheapest(g.size(), inf_cost), positive_cheapest(g.size()), bitset(g.size()),
        maximum_flow(0), minimum_cost(0)
        { build_residual_graph(); }
        void build_residual_graph() {
            if (not spfa()) { return; }
            while (dijkstra()) {
                for (int i = residual_graph.size() - 1; i != -1; --i) { cheapest[i] += positive_cheapest[i]; }
                bitset.reset(); //* reset for dfs_update
                Flow flow = dfs_update(source, inf_flow);
                maximum_flow += flow;
                minimum_cost += cheapest[terminal] * flow;
            }
        }
        bool spfa() {
            std::queue<Vertex> queue; queue.push(source);
            cheapest[source] = 0; bitset.flip(source); //* true if vertex is in the queue
            while (not queue.empty()) {
                Vertex vtx = queue.front(); queue.pop(); bitset.flip(vtx);
                for (const auto& edge : residual_graph[vtx]) {
                    auto child = edge.head();
                    auto [capacity, cost] = edge.value();
                    if (not capacity) { continue; }
                    Cost new_cost = cheapest[vtx] + cost;
                    if (new_cost >= cheapest[child]) { continue; }
                    cheapest[child] = new_cost;
                    if (not bitset[child]) { queue.push(child); bitset.flip(child); }
                }
            }
            return cheapest[terminal] != inf_cost;
        }
        bool dijkstra() {
            std::ranges::fill(positive_cheapest, inf_cost); positive_cheapest[source] = 0;
            bitset.reset(); //* true if vertex is processed
            using Pair = std::pair<Cost, Vertex>;
            lcf::std_binary_heap<Pair, std::greater<Pair>> heap; heap.push(std::make_pair(0, source));
            while (not heap.empty()) {
                auto [_, vtx] = heap.top(); heap.pop();
                for (const auto& edge : residual_graph[vtx]) {
                    auto child = edge.head();
                    auto [capacity, cost] = edge.value();
                    if (bitset[child] or not capacity) { continue; }
                    Cost new_cost = cost + positive_cheapest[vtx] + cheapest[vtx] - cheapest[child];
                    if (new_cost >= positive_cheapest[child]) { continue; }
                    positive_cheapest[child] = new_cost;
                    heap.push(std::make_pair(new_cost, child));
                }
                bitset.flip(vtx);
            }
            return positive_cheapest[terminal] != inf_cost;
        }
        Flow dfs_update(Vertex vtx, Flow rest_flow) {
            if (vtx == terminal) { return rest_flow; }
            bitset.flip(vtx);
            Flow total_out_flow = 0;
            for (auto iter = residual_graph.begin(vtx), end = residual_graph.end(); iter != end; ++iter) {
                auto child = (*iter).head();
                auto& [capacity, cost] = (*iter).value();
                if (bitset[child] or not capacity or cheapest[child] != cheapest[vtx] + cost) { continue; }
                Flow out_flow = dfs_update(child, std::min(rest_flow, capacity));
                if (not out_flow) { continue; }
                auto& [rev_capacity, _] = (~iter).value();
                capacity -= out_flow; rev_capacity += out_flow;
                rest_flow -= out_flow; total_out_flow += out_flow;
                if (rest_flow == 0) { break; }
            }
            bitset.flip(vtx);
            return total_out_flow;
        }
        Graph& residual_graph;
        Vertex source, terminal;
        std::vector<Cost> cheapest, positive_cheapest;
        std::tr2::dynamic_bitset<> bitset; //* used in spfa, dijkstra and dfs_update for different functions
        Flow maximum_flow;
        Cost minimum_cost;
    };
}

namespace lcf {
    template <typename Graph>
    struct ek_mcmf {
        using Vertex = typename Graph::vertex;
        using Weight = typename Graph::edge_type::weight_type;
        using Flow = typename Weight::first_type;
        using Cost = typename Weight::second_type;
        static constexpr Flow inf_flow = std::numeric_limits<Flow>::max() >> 2;
        static constexpr Cost inf_cost = std::numeric_limits<Cost>::max() >> 2;
        ek_mcmf(Graph& g, Vertex s, Vertex t)
        : residual_graph(g), source(s), terminal(t),
        flow(g.size()), cheapest(g.size()), in_queue(g.size()), trace(g.size()),
        maximum_flow(0), minimum_cost(0)
        { build_residual_graph(); }
        void build_residual_graph() {
            while (spfa()) {
                maximum_flow += flow[terminal];
                minimum_cost += cheapest[terminal] * flow[terminal];
                update();
            }
        }
        bool spfa() {
            std::ranges::fill(flow, 0); flow[source] = inf_flow;
            std::ranges::fill(cheapest, inf_cost); cheapest[source] = 0;
            std::queue<Vertex> queue; queue.push(source); in_queue.flip(source);
            while (not queue.empty()) {
                Vertex vtx = queue.front(); queue.pop(); in_queue[vtx] = false;
                for (auto iter = residual_graph.begin(vtx), end = residual_graph.end(); iter != end; ++iter) {
                    auto child = (*iter).head();
                    auto [capacity, cost] = (*iter).value();
                    if (not capacity) { continue; }
                    Cost new_cost = cheapest[vtx] + cost;
                    if (new_cost >= cheapest[child]) { continue; }
                    cheapest[child] = new_cost;
                    flow[child] = std::min(flow[vtx], capacity);
                    trace[child] = iter;
                    if (not in_queue[child]) { queue.push(child); in_queue.flip(child); }
                }
            }
            return flow[terminal] != 0;
        }
        void update() {
            for (Vertex vtx = terminal; vtx != source;) {
                auto& iter = trace[vtx];
                auto& [capacity, _] = (*iter).value();
                auto& [rev_capacity, __] = (~iter).value();
                capacity -= flow[terminal]; rev_capacity += flow[terminal];
                vtx = (~iter).head();
            }
        }
        Graph& residual_graph;
        Vertex source, terminal;
        std::vector<Flow> flow;
        std::vector<Cost> cheapest;
        std::tr2::dynamic_bitset<> in_queue;
        std::vector<typename Graph::iterator> trace;
        Flow maximum_flow;
        Cost minimum_cost;
    };
}

#endif