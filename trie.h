#ifndef TRIE_H
#define TRIE_H
#include <string>
#include <vector>
#include <array>

namespace lcf {
    template<typename NodeType>
    class trie {
    public:
        using node_type = NodeType;
        using reference = node_type&;
        using const_reference = const node_type&;
        using query_result_type = typename node_type::query_result_type;
    public:
        trie(size_t n) : _tree(1), last_index(0) { _tree.reserve(n); }
        trie(const trie&) = default;
        trie(trie&&) = default;
        inline __attribute__((__always_inline__))
        reference operator[](size_t index) { return _tree[index]; }
        inline __attribute__((__always_inline__))
        const_reference operator[](size_t index) const { return _tree[index]; }
        size_t size() const { return _tree.size(); }
        //*--------------------------------------------------------------------------*//
        void insert(std::string_view str) { insert(str.begin(), str.end()); }
        query_result_type query(std::string_view str) { return query(str.begin(), str.end()); }
        void clear() {
            for (size_t i = 0; i <= last_index; ++i) {  _tree[i].clear(); }
            last_index = 0;
        }
        template<typename ForwardIterator>
        void insert(ForwardIterator first, ForwardIterator last) {
            size_t cur_index = 0;
            for (; first != last; ++first) {
                int key = node_type::get_key(*first);
                if (_tree[cur_index].index_map[key] == 0) {
                    _tree[cur_index].index_map[key] = ++last_index;
                    if (last_index == _tree.size()) { _tree.emplace_back(); }
                }
                cur_index = _tree[cur_index].index_map[key];
                if constexpr (node_type::has_prefix_update::value == true)
                { _tree[cur_index].prefix_update(); }
            }
            if constexpr (node_type::has_suffix_update::value == true)
            { _tree[cur_index].suffix_update(); }
        }
        template<typename ForwardIterator>    
        query_result_type query(ForwardIterator first, ForwardIterator last) {
            size_t cur_index = 0;
            for (; first != last; ++first) {
                int key = node_type::get_key(*first);
                if (_tree[cur_index].index_map[key] == 0) { return 0; }
                cur_index = _tree[cur_index].index_map[key];
            }
            return _tree[cur_index].query_result();
        }
    private:
        std::vector<node_type> _tree;
        size_t last_index = 0;
    };
}

namespace trie_nodes {
    struct Node {
        using query_result_type = int;
        static size_t get_key(char ch) {
            if (isupper(ch)) { return ch - 'A'; }
            else if (islower(ch)) { return ch - 'a' + 26; }
            return ch - '0' + 52;
        }
        void clear() { index_map.fill(0), count = 0; }
        void update() { ++count; }
        query_result_type query_result() { return count; }
        std::array<int, 62> index_map;
    private:
        int count = 0;
    };
    
    struct ACNode {
        using query_result_type = int;
        using has_prefix_update = std::false_type;
        using has_suffix_update = std::true_type;
        static size_t get_key(char ch) { return ch - 'a'; }
        void clear() { index_map.fill(0), end_count = 0; }
        void suffix_update() { ++end_count; }
        bool queryable() const { return end_count >= 0; }
        query_result_type query_result() { int tmp = end_count; end_count = -1; return tmp; }
        std::array<int, 26> index_map;
    private:
        int end_count = 0;
    };
}

#endif