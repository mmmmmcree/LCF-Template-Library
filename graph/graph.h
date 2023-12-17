#ifndef GRAPH_H
#define GRAPH_H
#include <vector>

namespace lcf {
    namespace graph {
        using vertex = int;
        static constexpr vertex nvtx = -1;
    }
}

namespace lcf {
    namespace cfs {
        template<typename Edge>
        struct graph {
            using edge_type = Edge;
            using vertex = lcf::graph::vertex;
            graph(size_t vertex_num, size_t edge_num = 128) : map(vertex_num, lcf::graph::nvtx)
            { edges.reserve(edge_num); }
            graph(graph&& other) = default;
            vertex emplace_vertex() { map.emplace_back(lcf::graph::nvtx); return map.size() - 1; }
            template<typename... Args>
            void emplace_edge(vertex tail, vertex head, Args&&... args) {
                edges.emplace_back(head, map[tail], std::forward<Args>(args)...);
                map[tail] = edges.size() - 1;
            }
            size_t size() const { return map.size(); }
            template <typename Pointer>
            struct _iterator {
                using reference = std::conditional_t<std::is_const_v<std::remove_pointer_t<Pointer>>, const edge_type&, edge_type&>;
                _iterator(Pointer p = nullptr, vertex v = lcf::graph::nvtx) : graph_pointer(p), vtx(v) { }
                reference operator*() const { return graph_pointer->edges[vtx]; }
                reference operator~() const { return graph_pointer->edges[vtx ^ 1]; }
                void operator++() { vtx = graph_pointer->edges[vtx]._next; }
                bool operator!=(const _iterator& rhs) const { return vtx != rhs.vtx; }
                Pointer graph_pointer;
                vertex vtx;
            };
            template <typename Iterator>
            struct iterator_wrapper {
                iterator_wrapper(Iterator b, Iterator e) : _begin(b), _end(e) { }
                Iterator begin() const { return _begin; }
                Iterator end() const { return  _end; }
                Iterator _begin, _end;
            };
            using iterator = _iterator<graph*>;
            using const_iterator = _iterator<const graph*>;
            iterator begin(vertex vtx) { return iterator(this, map[vtx]); }
            const_iterator begin(vertex vtx) const { return const_iterator(this, map[vtx]); }
            iterator end() { return iterator{}; }
            const_iterator end() const { return const_iterator{}; }
            auto operator[](vertex vtx) { return iterator_wrapper(begin(vtx), end()); }
            auto operator[](vertex vtx) const { return iterator_wrapper(begin(vtx), end()); }
        private:
            std::vector<vertex> map;
            std::vector<edge_type> edges;
        };
        struct unweighted_edge {
            using vertex = lcf::graph::vertex;
            template<typename> friend class graph;
            unweighted_edge(vertex head, vertex next): _head(head), _next(next) { }
            vertex head() const { return _head; }
        protected:
            vertex _head, _next;
        };
        template<typename Weight = long long>
        struct weighted_edge : unweighted_edge {
            using weight_type = Weight;
            template<typename... Args>
            weighted_edge(vertex head, vertex next, Args&&... args)
            : unweighted_edge(head, next), _weight(std::forward<Args>(args)...) { }
            weight_type& value() { return _weight; }
            const weight_type& value() const { return _weight; }
            weight_type _weight;
            static constexpr weight_type inf = std::numeric_limits<weight_type>::max() >> 2;
        };
    }
}

namespace lcf {
    namespace al {
        template <typename Edge>
        class graph {
        public:
            using edge_type = Edge;
            using vertex = lcf::graph::vertex;
            using adjacent_list_type = std::vector<std::vector<edge_type>>;
            using reference = typename adjacent_list_type::reference;
            using const_reference = typename adjacent_list_type::const_reference;
            graph(size_t vertex_num) : adjacent_list(vertex_num) { }
            template <typename... Args>
            void emplace_edge(vertex tail, vertex head, Args&&... args)
            { adjacent_list[tail].emplace_back(head, std::forward<Args>(args)...); }
            vertex emplace_vertex() { adjacent_list.emplace_back(); return adjacent_list.size() - 1; }
            size_t size() const { return adjacent_list.size(); }
            reference& operator[](vertex tail) { return adjacent_list[tail]; }
            const_reference operator[](vertex tail) const { return adjacent_list[tail]; }
        private:
            adjacent_list_type adjacent_list;
        };
        struct unweighted_edge {
            using vertex = lcf::graph::vertex;
            unweighted_edge(vertex head): _head(head) { }
            vertex head() const { return _head; }
        protected:
            vertex _head;
        };
        template <typename Weight = long long>
        struct weighted_edge : unweighted_edge {
            using weight_type = Weight;
            template <typename... Args>
            weighted_edge(vertex head, Args&&... args)
            : unweighted_edge(head), _weight(std::forward<Args>(args)...) { }
            weight_type& value() { return _weight; }
            const weight_type& value() const { return _weight; }
            weight_type _weight;
            static constexpr weight_type inf = std::numeric_limits<weight_type>::max() >> 1;
        };
    }
}

namespace lcf {
    namespace am {
        template <typename T>
        class graph : private std::vector<T> {
            using _Base = std::vector<T>;
            using _Self = graph<T>;
        public:
            using value_type = typename _Base::value_type;
            using reference = typename _Base::reference;
            using const_reference = typename _Base::const_reference;
            using size_type = typename _Base::size_type;
            using iterator = typename _Base::iterator;
            graph(size_type size, const_reference value = value_type{})
            : _Base(size * size, value), _size(size) { }
            size_type dimension() const { return _size; }
            iterator begin(size_t row) { return _Base::begin() + row * _size; }
            inline __attribute__((__always_inline__))
            reference operator()(size_type row_idx, size_type col_idx) noexcept
            { return this->_M_impl._M_start[row_idx * _size + col_idx]; }
            inline __attribute__((__always_inline__))
            const_reference operator()(size_type row_idx, size_type col_idx) const noexcept
            { return this->_M_impl._M_start[row_idx * _size + col_idx]; }
        private:  
            size_type _size;
        };
    }
}

namespace lcf {
    namespace graph {
        //* undirected edge
        struct unweighted_edge {
            unweighted_edge() = default;
            unweighted_edge(graph::vertex u, graph::vertex v) : _u(u), _v(v) { }
            using has_value = std::false_type;
            graph::vertex _u, _v;
        };
        template <typename T>
        struct weighted_edge {
            weighted_edge() = default;
            weighted_edge(graph::vertex u, graph::vertex v, const T& w) : _u(u), _v(v), _w(w) { }
            using has_value = std::true_type;
            friend auto operator<=>(const weighted_edge& lhs, const weighted_edge& rhs)
            { return lhs._w <=> rhs._w; }
            graph::vertex _u, _v;
            T _w;
        };
    }
    template <typename Edge, typename Graph>
    std::vector<Edge> switch_to_edges(const Graph& graph) {
        std::vector<Edge> edges; edges.reserve(graph.size());
        for (graph::vertex u = 0, n = graph.size(); u < n; ++u) {
            for (const auto& edge : graph[u]) {
                if constexpr (Edge::has_value::value) { edges.emplace_back(u, edge.head(), edge.value()); }
                else { edges.emplace_back(u, edge.head()); }
            }
        }
        return edges;
    }
}

#endif