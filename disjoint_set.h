#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H
#include <vector>
// namespace lcf {
//     template< typename WeightType,
//                     typename WeightUpdateMethod,
//                     typename ParentsSequence = std::vector<size_t>,
//                     typename WeightSequence = std::vector<WeightType>
//     > class disjoint_set {
//     public:
//         using iterator = typename WeightSequence::iterator;
//         using value_type = WeightType;
//         using Node = size_t;
//     public:
//         disjoint_set() = default;
//         disjoint_set(size_t n) : parents(n), path_weights(n, 0), sets_count(n) {this->init();}
//         disjoint_set(const disjoint_set&) = default;
//     public:
//         Node& operator[](size_t n) {return parents[n];}
//     public:
//         iterator begin() {return path_weights.begin();}
//         iterator end() {return path_weights.end();}
//     public:
//         void init() {
//             sets_count = path_weights.size();
//             for (Node node = 0, end_node = parents.size(); node != end_node; ++node)
//             {parents[node] = node;}
//         }
//         void make_set(Node node) {parents[node] = node;}
//         Node find_set_root(Node node) {
//             while (node != parents[node])
//             {node = parents[node];}
//             return node;
//         }
//         Node compress(Node node) {
//             if (node != parents[node]) {
//                 Node parent_node = parents[node];
//                 parents[node] = compress(parent_node);
//                 path_weights[node] = WeightUpdateMethod()(path_weights[node], path_weights[parent_node]);
//             }
//             return parents[node];
//         }
//         void make_union(Node lower_node, Node higher_node, const WeightType& difference = WeightType()) {
//             Node lower_set_root = compress(lower_node);
//             Node higher_set_root = compress(higher_node);
//             if (lower_set_root == higher_set_root) {return;}
//             --sets_count;
//             parents[lower_set_root] = higher_set_root;
//             path_weights[lower_set_root] = WeightUpdateMethod()(path_weights[lower_node], path_weights[higher_node], difference);
//         }
//         bool is_same_set(Node node_1, Node node_2) {return find_set_root(node_1) == find_set_root(node_2);}
//         size_t count() {return sets_count;}
//     private:
//         ParentsSequence parents; // use index as child node, value as parent node
//         WeightSequence path_weights; // store nodes' path weights
//         size_t sets_count;
//     };
    
//     namespace methods {
//         template<typename WeightType>
//         struct AddMethod {
//             WeightType operator()(const WeightType& lhs, const WeightType& rhs) const
//             {return lhs + rhs;}
//             WeightType operator()(const WeightType& lhs, const WeightType& rhs, const WeightType& difference) const
//             {return rhs + difference - lhs;}
//         };
//     }
// }
// No path weights
namespace lcf {
    class disjoint_set {
        using Node = size_t;
        using ParentsSequence = std::vector<size_t>;
    public:
        disjoint_set(size_t n) : parents(n), sets_count(n) {this->init();}
        void init() {
            sets_count = parents.size();
            for (Node node = 0, end_node = parents.size(); node != end_node; ++node)
            {parents[node] = node;}
        }
        void make_set(Node node) {parents[node] = node;}
        Node find_set_root(Node node) {
            while (node != parents[node])
            {node = parents[node];}
            return node;
        }
        Node compress(Node node) {
            if (node != parents[node])
            {parents[node] = compress(parents[node]);}
            return parents[node];
        }
        void make_union(Node lower_node, Node higher_node) {
            Node lower_set_root = compress(lower_node);
            Node higher_set_root = compress(higher_node);
            if (lower_set_root == higher_set_root) {return;}
            --sets_count;
            parents[lower_set_root] = higher_set_root;
        }
        bool is_same_set(Node node_1, Node node_2) {return find_set_root(node_1) == find_set_root(node_2);}
        size_t count() {return sets_count;}
    private:
        ParentsSequence parents;
        size_t sets_count;
    };
}
#endif // DISJOINT_SET_H