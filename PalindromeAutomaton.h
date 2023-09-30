#ifndef PALINDROMEAUTOMATON_H
#define PALINDROMEAUTOMATON_H
#include <vector>
#include <deque>
#include <array>

namespace pam {
    struct TrieNode {
        using value_type = char;
        static size_t hash(value_type val) { return val - 'a'; }
        static value_type unhash(size_t hash_val) { return hash_val + 'a'; }
        static constexpr size_t size() { return 26; };
        std::array<int, 26> index_map;
    };

    template<typename TrieNodeType>
    struct TreeNodeBase {
        using trie_node_type = TrieNodeType;
        using size_type = int;
        TreeNodeBase() = default;
        TreeNodeBase(size_type _next, int _len, int _size) : trie_node(), next(_next), length(_len), endpos_size(_size) { }
        trie_node_type trie_node;
        size_type next;
        int length, endpos_size;
    };
}

namespace lcf {
    template<typename TreeNodeType = pam::TreeNodeBase<pam::TrieNode>>
    class PalindromeAutomaton {
    public:
        using node_type = TreeNodeType;
        using trie_node_type = typename node_type::trie_node_type;
        using value_type = typename trie_node_type::value_type;
        using size_type = typename node_type::size_type;
        PalindromeAutomaton() : tree(2), str(), prev_index()
        {
            tree[0].next = 1;
            tree[1].length = -1;
        }
        void push_back(const value_type& val) {
            str.emplace_back(val);
            auto key = trie_node_type::hash(val);
            const auto fail = get_fail_from_back(prev_index[_back]);
            size_type cur_idx = tree[fail].trie_node.index_map[key];
            if (cur_idx == 0) {
                auto next_fail = get_fail_from_back(tree[fail].next);
                size_type next_idx = tree[next_fail].trie_node.index_map[key];
                tree.emplace_back(next_idx, tree[fail].length + 2, tree[next_idx].endpos_size + 1);
                cur_idx = tree.size() - 1;            
            }
            prev_index[_back] = tree[fail].trie_node.index_map[key] = cur_idx;
            if (tree[cur_idx].length == str.size()) { prev_index[_front] = cur_idx; }
        }
        void push_front(const value_type& val) {
            str.emplace_front(val);
            auto key = trie_node_type::hash(val);
            const auto fail = get_fail_from_front(prev_index[_front]);
            size_type cur_idx = tree[fail].trie_node.index_map[key];
            if (cur_idx == 0) {
                auto next_fail = get_fail_from_front(tree[fail].next);
                size_type next_idx = tree[next_fail].trie_node.index_map[key];
                tree.emplace_back(next_idx, tree[fail].length + 2, tree[next_idx].endpos_size + 1);
                cur_idx = tree.size() - 1;            
            }
            prev_index[_front] = tree[fail].trie_node.index_map[key] = cur_idx;
            if (tree[cur_idx].length == str.size()) { prev_index[_back] = cur_idx; }
        }
        const node_type& back() const { return tree[prev_index[_back]]; }
        const node_type& front() const { return tree[prev_index[_front]]; }
        const size_type back_index() const { return prev_index[_back]; }
        const size_type front_index() const { return prev_index[_front]; }
    private:
        size_type get_fail_from_back(size_type idx) {
            for (size_type back_idx = str.size() - 1, jump_to_idx;
                (jump_to_idx = back_idx - tree[idx].length - 1) < 0 or str[jump_to_idx] != str[back_idx];
                idx = tree[idx].next );
            return idx;
        }
        size_type get_fail_from_front(size_type idx) {
            for (size_type front_idx = 0, size = str.size(), jump_to_idx;
                (jump_to_idx = front_idx + tree[idx].length + 1) >= size or str[jump_to_idx] != str[front_idx];
                idx = tree[idx].next );
            return idx;
        }
    public:
        std::vector<node_type> tree;
    private:
        std::deque<value_type> str;
        enum {_front, _back, _size};
        std::array<size_type, _size> prev_index;
    };
}

#endif