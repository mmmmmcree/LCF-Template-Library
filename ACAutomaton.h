#ifndef ACAUTOMATON_H
#define ACAUTOMATON_H
#include "trie.h"
#include <queue>

namespace lcf {
    template<typename TrieType, typename QueryResultType = ac_query_result::Result>
    class ACAutomaton {
        using _Self = ACAutomaton<TrieType>;
    public:
        using trie_type = TrieType;
        using node_type = typename trie_type::node_type;
        using query_result_type = QueryResultType;
        ACAutomaton(const trie_type& trie) : _trie(trie), _next(_trie.size(), 0) { _build(); }
        ACAutomaton(trie_type&& trie) : _trie(std::move(trie)), _next(_trie.size(), 0) { _build(); }
        trie_type&& release() { return std::move(_trie); } // 如果有多次query, 则返还_tree
        query_result_type query(std::string_view str) { return query(str.begin(), str.end()); }
        template<typename ForwardIterator>
        query_result_type query(ForwardIterator first, ForwardIterator last) {
            query_result_type result{};
            for (size_t idx = 0; first != last; ++first) {
                idx = _trie[idx].index_map[node_type::get_key(*first)];
                for (size_t suffix_idx = idx; suffix_idx != 0 and _trie[suffix_idx].queryable(); suffix_idx = _next[suffix_idx])
                { result += _trie[suffix_idx].query_result(); }
            }
            return result;
        }
    private:
        void _build() {
            std::queue<int> queue;
            size_t n = _trie[0].index_map.size();
            for (const auto index : _trie[0].index_map) { if (index != 0) { queue.emplace(index); } }
            while (not queue.empty()) {
                auto cur_idx = queue.front(); queue.pop();
                for (size_t i = 0; i < n; ++i) {
                    auto &child_idx = _trie[cur_idx].index_map[i], next_idx = _trie[_next[cur_idx]].index_map[i];
                    if (child_idx != 0) {
                        _next[child_idx] = next_idx;
                        queue.emplace(child_idx);
                    } else { child_idx = next_idx; }
                }
            }
        }
    private:
        trie_type _trie;
        std::vector<int> _next;
    };
}


namespace ac_query_result {
    //* 重载 operator+= 即可, 与ac_nodes配合使用
    // 洛谷	P3796
    struct Result {
        std::vector<int> counts = std::vector<int>(150, 0);
        void operator+=(int index) { if (index >= 0) { ++counts[index]; }; }
    };
}

namespace ac_nodes {
    // 洛谷	P3796
    struct IndexNode {
        using query_result_type = int;
        using has_prefix_update = std::false_type;
        using has_suffix_update = std::true_type;
        static size_t get_key(char ch) { return ch - 'a'; }
        void clear() { index_map.fill(0), index = -1; }
        void suffix_update() { index = word_index; }
        bool queryable() const { return true; } // 支持重复访问, 则恒为true
        query_result_type query_result() { return index; }
        std::array<int, 26> index_map;
        static inline int word_index = 0;
    private:
        int index = -1;
    };
}

namespace ac_nodes {
    // 对应 result_type 为 int
    struct EndCountNode {
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