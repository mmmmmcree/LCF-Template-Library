#ifndef MAXIMUM_FLOW
#define MAXIMUM_FLOW
#include "graph.h"
#include <queue>

namespace lcf {
    template <typename ResidualGraph>
    typename ResidualGraph::edge_type::weight_type
    residual_capacity(const ResidualGraph& residual_graph, graph::vertex source) {
        typename ResidualGraph::edge_type::weight_type result = 0;
        for (const auto& edge : residual_graph[source]) { result += edge.value(); }
        return result;
    }
}

namespace lcf {
    template <typename Graph>
    struct edmonds_karp {
        using Vertex = typename Graph::vertex;
        using Edge = typename Graph::edge_type;
        using Weight = typename Edge::weight_type;
        edmonds_karp(const Graph& g, Vertex source, Vertex terminal)
        : residual_graph(g), flow(g.size()), trace(g.size())
        { build_residual_graph(source, terminal); }
        edmonds_karp(Graph&& g, Vertex source, Vertex terminal)
        : residual_graph(std::move(g)), flow(residual_graph.size()), trace(residual_graph.size())
        { build_residual_graph(source, terminal); }
        void build_residual_graph(Vertex source, Vertex terminal)
        { while (bfs(source, terminal)) { update(source, terminal); } }
        bool bfs(Vertex source, Vertex terminal) {
            std::fill(flow.begin(), flow.end(), 0); flow[source] = Edge::inf;
            std::queue<Vertex> queue; queue.push(source);
            while (not queue.empty()) {
                auto vtx = queue.front(); queue.pop();
                for (auto iter = residual_graph.begin(vtx), end = residual_graph.end(); iter != end; ++iter) {
                    auto child = (*iter).head();
                    if (flow[child] or not (*iter).value()) { continue; } //* already accessed or current edge has not capacity left
                    flow[child] = std::min(flow[vtx], (*iter).value());
                    trace[child] = iter;
                    if (child == terminal) { return true; }
                    queue.push(child);
                }
            }
            return false;
        }
        void update(Vertex source, Vertex terminal) {
            for (auto vtx = terminal; vtx != source;) {
                auto& iter = trace[vtx];
                (*iter).value() -= flow[terminal];
                (~iter).value() += flow[terminal];
                vtx = (~iter).head();
            }
        }
        Graph residual_graph;
        std::vector<Weight> flow;
        std::vector<typename Graph::iterator> trace;
    };
}

namespace lcf {
    template <typename Graph>
    struct danic {
        using Vertex = typename Graph::vertex;
        using Edge = typename Graph::edge_type;
        using Weight = typename Edge::weight_type;
        danic(const Graph& g, Vertex source, Vertex terminal)
        : residual_graph(g), depth(g.size()), useful(g.size())
        { build_residual_graph(source, terminal); }
        danic(Graph&& g, Vertex source, Vertex terminal)
        : residual_graph(std::move(g)), depth(residual_graph.size()), useful(residual_graph.size())
        { build_residual_graph(source, terminal); } 
        void build_residual_graph(Vertex source, Vertex terminal)
        { while (bfs(source, terminal)) { dfs_update(source, terminal, Edge::inf); } }
        bool bfs(Vertex source, Vertex terminal) {
            for (Vertex u = residual_graph.size() - 1; u != -1; --u)
            { useful[u] = residual_graph.begin(u); }
            std::fill(depth.begin(), depth.end(), 0); depth[source] = 1;
            std::queue<Vertex> queue; queue.push(source);
            while (not queue.empty()) {
                auto vtx = queue.front(); queue.pop();
                for (const auto& edge : residual_graph[vtx]) {
                    auto child = edge.head();
                    if (depth[child] or not edge.value()) { continue; }
                    depth[child] = depth[vtx] + 1;
                    if (child == terminal) { return true; }
                    queue.push(child);
                }
            }
            return false;
        }
        Weight dfs_update(Vertex vtx, Vertex terminal, Weight rest_flow) {
            if (vtx == terminal) { return rest_flow; }
            Weight total_out_flow = 0;
            for (auto iter = useful[vtx], end = residual_graph.end(); iter != end; ++iter) {
                auto child = (*iter).head();
                useful[vtx] = iter;
                if (depth[child] != depth[vtx] + 1 or not (*iter).value()) { continue; }
                Weight out_flow = dfs_update(child, terminal, std::min(rest_flow, (*iter).value()));
                if (not out_flow) { depth[child] = 0; continue; }
                rest_flow -= out_flow; total_out_flow += out_flow;
                (*iter).value() -= out_flow; (~iter).value() += out_flow;
                if (not rest_flow) { break; }
            }
            return total_out_flow;
        }
        Graph residual_graph;
        std::vector<int> depth;
        std::vector<typename Graph::iterator> useful;
    };
}

namespace lcf {
    template <typename Graph>
    struct isap {
        using Vertex = typename Graph::vertex;
        using Edge = typename Graph::edge_type;
        using Weight = typename Edge::weight_type;
        isap(const Graph& g, Vertex source, Vertex terminal)
        : residual_graph(g), height(g.size()), count(g.size() + 3), useful(g.size())
        { build_residual_graph(source, terminal); }
        isap(Graph&& g, Vertex source, Vertex terminal)
        : residual_graph(std::move(g)), height(residual_graph.size()),
          count(residual_graph.size() + 3), useful(residual_graph.size())
        { build_residual_graph(source, terminal); }
        void build_residual_graph(Vertex source, Vertex terminal) {
            if (not bfs_init(source, terminal)) { return; }
            while (height[source] <= residual_graph.size()) {
                for (Vertex u = residual_graph.size() - 1; u != -1; --u)
                { useful[u] = residual_graph.begin(u); }
                dfs(source, source, terminal, Edge::inf);
            }
        }
        bool bfs_init(Vertex source, Vertex terminal) {
            count[1] = height[terminal] = 1;
            std::queue<Vertex> queue; queue.push(terminal);
            while (not queue.empty()) {
                auto vtx = queue.front(); queue.pop();
                for (auto iter = residual_graph.begin(vtx), end = residual_graph.end(); iter != end; ++iter) {
                    auto child = (*iter).head();
                    if (height[child] or not (~iter).value()) { continue; }
                    height[child] = height[vtx] + 1;
                    ++count[height[child]];
                    queue.push(child);
                }
            }
            return height[source];
        }
        Weight dfs(Vertex vtx, Vertex source, Vertex terminal, Weight rest_flow) {
            if (vtx == terminal) { return rest_flow; }
            Weight total_out_flow = 0;
            for (auto iter = useful[vtx], end = residual_graph.end(); iter != end; ++iter) {
                auto child = (*iter).head();
                useful[vtx] = iter;
                if (height[child] != height[vtx] - 1 or not (*iter).value()) { continue; }
                Weight out_flow = dfs(child, source, terminal, std::min(rest_flow, (*iter).value()));
                if (not out_flow) { continue; }
                rest_flow -= out_flow; total_out_flow += out_flow;
                (*iter).value() -= out_flow; (~iter).value() += out_flow;
                if (not rest_flow) { return total_out_flow; }
            }
            if (not --count[height[vtx]]) { height[source] = residual_graph.size() + 1; }
            ++count[++height[vtx]];
            return total_out_flow;
        }
        Graph residual_graph;
        std::vector<int> height, count;
        std::vector<typename Graph::iterator> useful;
    };
}

#endif