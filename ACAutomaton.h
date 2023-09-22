#ifndef ACAUTOMATON_H
#define ACAUTOMATON_H
#include "trie.h"
#include <queue>

namespace lcf {
    template<typename TrieType>
    class ACAutomaton {
        using _Self = ACAutomaton<TrieType>;
    public:
        using trie_type = TrieType;
        using node_type = typename trie_type::node_type;
        using query_result_type = typename trie_type::query_result_type;
        ACAutomaton(const trie_type& trie) : _trie(trie), _next(_trie.size(), 0) { _build(); }
        ACAutomaton(trie_type&& trie) : _trie(std::move(trie)), _next(_trie.size(), 0) { _build(); }
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

#endif