#ifndef TARJAN_H
#define TARJAN_H
#include "graph.h"
#include <stack>
#include <tr2/dynamic_bitset>

namespace lcf {
    /*
    * strongly connected component: vertices in it are mutually reachable;
    * tree edge: the edge's head is not visited
    * back edge: connect vertex to its ancestor; the edge's head might be scc's root;
    * forward edge: connect vertex to its visited child
    * cross edge: cross from one subtree to another; change tail's trace to earlier one to expand scc
    */
   template <typename Graph>
   struct tarjan_scc {
        using Vertex = typename Graph::vertex;
        using Edge = typename Graph::edge_type;
        tarjan_scc(const Graph& g)
        : graph(g), count(0), timestamp(0), trace(g.size()),
          dfn(g.size(), -1), group(g.size(), graph::nvtx) //* not grouped; not visited
        {
            for (Vertex i = 0, n = g.size(); i < n; ++i)
            { if (dfn[i] == -1) { dfs(i); } }
        }
        void dfs(Vertex cur_vtx) {
            dfn[cur_vtx] = trace[cur_vtx] = timestamp++;
            stack.emplace(cur_vtx);
            for (const auto& edge : graph[cur_vtx]) {
                auto child = edge.head();
                if (dfn[child] == -1) { //* tree edge
                    dfs(child);
                    trace[cur_vtx] = std::min(trace[cur_vtx], trace[child]);
                } else if (group[child] == -1) //* back edge or cross edge or forward edge
                { trace[cur_vtx] = std::min(trace[cur_vtx], trace[child]); }
            }
            if (trace[cur_vtx] != dfn[cur_vtx]) { return; }
            Vertex vtx;
            do {
                vtx = stack.top(); stack.pop();
                group[vtx] = count;
            } while (vtx != cur_vtx); //* at least do once to pop cur_vtx
            ++count;
        }
        const Graph& graph;
        int count, timestamp;
        std::vector<int> trace, dfn; //* trace <= dfn; all vertices'trace in scc point to scc's root
        std::vector<Vertex> group;
        std::stack<Vertex> stack;
   };
}

namespace lcf {
    template <typename Graph> 
    struct tarjan_cut_vertex {
        using Vertex = typename Graph::vertex;
        tarjan_cut_vertex(const Graph& g)
        : graph(g), timestamp(0), dfn(g.size(), -1), trace(g.size()), cut(g.size())
        {
            for (Vertex u = graph.size() - 1; u != -1; --u)
            { if (dfn[u] == -1) { dfs(u, u); } }
        }
        void dfs(Vertex vtx, Vertex root) {
            dfn[vtx] = trace[vtx] = timestamp++;
            for (int branch_cnt = 0; const auto& edge : graph[vtx]) {
                auto child = edge.head();
                if (dfn[child] != -1) {
                    trace[vtx] = std::min(trace[vtx], dfn[child]); //* update with dfn, not trace
                    continue;
                }
                dfs(child, root);
                trace[vtx] = std::min(trace[vtx], trace[child]);
                if (trace[child] == dfn[vtx]) //* blocked by current dfn
                { if (vtx != root or ++branch_cnt > 1) { cut[vtx] = true; } } //* the root is cut only when it has more than one brach
            }
        }
        const Graph& graph;
        int timestamp;
        std::vector<int> dfn, trace;
        std::tr2::dynamic_bitset<> cut;
    };
}

namespace lcf {
    /*
    * compare to cut_vertex, the cur_vtx can't go backward along the reverse edge during the dfs process;
    * otherwise dfn[cur_vtx] <= trace[child] always satisfies, so that bridge can never be found;
    ! However, if the graph has multiple edges, the above situation should be skipped only once;
    * during the dfs process of cut_vertex, the same prohibition can be used as well, but the cut_vertex
    * condition should be altered from (trace[child] == dfn[vtx]) to (trace[child] >= dfn[vtx])
    */
    template <typename Graph, typename Edge = graph::unweighted_edge>
    struct tarjan_bridge {
        using Vertex = typename Graph::vertex;
        tarjan_bridge(const Graph& g)
        : graph(g), dfn(g.size(), -1), trace(g.size())
        {
            for (Vertex u = 0, n = graph.size(); u < n; ++u)
            { if (dfn[u] == -1) { dfs(u, lcf::graph::nvtx); } }
        }
        void dfs(Vertex cur_vtx, Vertex pre_vtx) {
            dfn[cur_vtx] = trace[cur_vtx] = timestamp++;
            int back_edge_cnt = 0;
            for (const auto& edge : graph[cur_vtx]) {
                auto child = edge.head();
                if (child == pre_vtx and ++back_edge_cnt == 1) { continue; }
                if (dfn[child] != -1) { trace[cur_vtx] = std::min(trace[cur_vtx], dfn[child]); continue; }
                dfs(child, cur_vtx);
                trace[cur_vtx] = std::min(trace[cur_vtx], trace[child]);
                if (trace[child] > dfn[cur_vtx])  {
                    if constexpr (Edge::has_value::value) { bridges.emplace_back(cur_vtx, child, edge.value()); }
                    else { bridges.emplace_back(cur_vtx, child); }
                }
            }
        }
        const Graph& graph;
        int timestamp;
        std::vector<int> dfn, trace;
        std::vector<Edge> bridges;
    };
}

namespace lcf {
    template <typename Graph>
    struct tarjan_edcc {
        using Vertex = typename Graph::vertex;
        tarjan_edcc(const Graph& g)
        : graph(g), timestamp(0), dfn(g.size(), -1), trace(g.size()), count(0), group(g.size())
        {
            for (Vertex u = graph.size() - 1; u != -1; --u)
            { if (dfn[u] == -1) { dfs(u, graph::nvtx); } }
        }
        void dfs(Vertex cur_vtx, Vertex pre_vtx) {
            dfn[cur_vtx] = trace[cur_vtx] = timestamp++;
            stack.emplace(cur_vtx);
            int back_edge_cnt = 0;
            for (const auto& edge : graph[cur_vtx]) {
                auto child = edge.head();
                if (child == pre_vtx and ++back_edge_cnt == 1) { continue; } //* only skip one back edge;
                if (dfn[child] != -1) { trace[cur_vtx] = std::min(trace[cur_vtx], dfn[child]); continue; }
                dfs(child, cur_vtx);
                trace[cur_vtx] = std::min(trace[cur_vtx], trace[child]);
            }
            if (trace[cur_vtx] != dfn[cur_vtx]) { return; }
            while (true) {
                auto vtx = stack.top(); stack.pop();
                group[vtx] = count;
                if (vtx == cur_vtx) { break; }
            }
            ++count;
        }
        const Graph& graph;
        int timestamp;
        std::vector<int> dfn, trace;
        std::stack<Vertex> stack;
        int count;
        std::vector<Vertex> group;
    };
}

namespace lcf {
    template <typename Graph>
    struct tarjan_vdcc {
        using Vertex = typename Graph::vertex;
        tarjan_vdcc(const Graph& g)
        : graph(g), timestamp(0), dfn(g.size(), -1), trace(g.size())
        {
            for (Vertex u = graph.size() - 1; u != -1; --u)
            { if (dfn[u] == -1) { dfs(u); } }
        }
        void dfs(Vertex cur_vtx) {
            dfn[cur_vtx] = trace[cur_vtx] = timestamp++;
            stack.emplace(cur_vtx);
            int branch_cnt = 0;
            for (const auto& edge : graph[cur_vtx]) {
                auto child = edge.head();
                if (dfn[child] != -1) { trace[cur_vtx] = std::min(trace[cur_vtx], dfn[child]); continue; }
                dfs(child);
                trace[cur_vtx] = std::min(trace[cur_vtx], trace[child]);
                if (trace[child] != dfn[cur_vtx]) { continue; }
                ++branch_cnt;
                result.emplace_back();
                while (true) {
                    auto vtx = stack.top(); stack.pop();
                    result.back().emplace_back(vtx);
                    if (vtx == child) { break; } //! meet child and break, not meet cur_vtx
                }
                result.back().emplace_back(cur_vtx);
            }
            if (trace[cur_vtx] == dfn[cur_vtx] and branch_cnt == 0) //* single vertex is an independent vdcc
            { result.emplace_back(1, cur_vtx); }
        }
        const Graph& graph;
        int timestamp;
        std::vector<int> dfn, trace;
        std::stack<Vertex> stack;
        std::vector<std::vector<Vertex>> result;
    };
}

namespace lcf {
    template <typename Graph>
    struct unweighted_block_forest {
        using Vertex = typename Graph::vertex;
        unweighted_block_forest(const Graph& g)
        : graph(g), timestamp(0), dfn(g.size(), -1), trace(g.size()), forest(g.size())
        {
            for (Vertex u = graph.size() - 1; u != -1; --u)
            { if (dfn[u] == -1) { dfs(u); } }
        }
        void dfs(Vertex cur_vtx) {
            dfn[cur_vtx] = trace[cur_vtx] = timestamp++;
            stack.emplace(cur_vtx);
            int branch_cnt = 0;
            for (const auto& edge : graph[cur_vtx]) {
                auto child = edge.head();
                if (dfn[child] != -1) { trace[cur_vtx] = std::min(trace[cur_vtx], dfn[child]); continue; }
                dfs(child);
                trace[cur_vtx] = std::min(trace[cur_vtx], trace[child]);
                if (trace[child] != dfn[cur_vtx]) { continue; }
                ++branch_cnt;
                auto new_vtx = forest.emplace_vertex();
                while (true) {
                    auto vtx = stack.top(); stack.pop();
                    forest.emplace_edge(vtx, new_vtx);
                    forest.emplace_edge(new_vtx, vtx);
                    if (vtx == child) { break; }
                }
                forest.emplace_edge(cur_vtx, new_vtx);
                forest.emplace_edge(new_vtx, cur_vtx);
            }
            if (dfn[cur_vtx] == trace[cur_vtx] and branch_cnt == 0) {
                auto new_vtx = forest.emplace_vertex();
                forest.emplace_edge(new_vtx, cur_vtx);
                forest.emplace_edge(cur_vtx, new_vtx);
            }
        }
        const Graph& graph;
        int timestamp;
        std::vector<int> dfn, trace;
        std::stack<Vertex> stack;
        Graph forest;
    };
}

#endif