#ifndef SUFFIXAUTOMATON_H
#define SUFFIXAUTOMATON_H
#include <vector>
#include <array>

namespace sam {
    struct TrieNode {
        static size_t get_key(char ch) { return ch - 'a'; }
        static char get_char(size_t key) { return key + 'a'; }
        static constexpr size_t size() { return 26; };
        void clear() { index_map.fill(0); }
        std::array<int, 26> index_map;
    };
    template<typename TrieNodeType>
    struct TreeNodeBase {
        using size_type = int;
        using _Self = TreeNodeBase<TrieNodeType>;
        using trie_node_type = TrieNodeType;
        TreeNodeBase() : trie_node(), next(0), length(0){ }
        trie_node_type trie_node;
        size_type next;
        int length;
    };
    template<typename TrieNodeType = TrieNode>
    struct TreeNode : public TreeNodeBase<TrieNodeType> {
        using _Base = TreeNodeBase<TrieNodeType>;
        using _Self = TreeNode<TrieNodeType>;
        using trie_node_type = typename _Base::trie_node_type;
        TreeNode() : _Base(), endpos_size(1) { }
        TreeNode(const _Self& other) : _Base(other), endpos_size(0) { }
        _Self& operator=(const _Self& other) { endpos_size = 0; _Base::operator=(other); return *this; }
        void combine(const _Self& other) { endpos_size += other.endpos_size; }
        void clear() {
            this->trie_node.clear();
            this->length = this->next = 0;
            endpos_size = 1;
        }
        int endpos_size;
    };
}

namespace lcf {
    template<typename NodeType>
    class SuffixAutomaton {
    public:
        using node_type = NodeType;
        using trie_node_type = typename node_type::trie_node_type;
        using size_type = typename node_type::size_type;
        SuffixAutomaton(size_t str_size = 128) : tree(1), last_index(0), prev_index(0) { tree[0].next = -1; tree.reserve(2 * str_size); }
        SuffixAutomaton(std::string_view str) : tree(1), last_index(0), prev_index(0)
        { tree[0].next = -1; tree.reserve(2 * str.size()); push_back(str); }
        void push_back(std::string_view str) { push_back(str.begin(), str.end()); }
        template<typename ForwardIterator>
        void push_back(ForwardIterator first, ForwardIterator last) { for (; first != last; ++first) { push_back(*first); } }
        void push_back(char ch) {
            size_type fail = prev_index, new_index = create_node(); // fail为跳转下标, 用于链接, new_index为新加入节点的下标
            prev_index = new_index; // 为下一次添加做准备
            auto key = trie_node_type::get_key(ch);
            for (; fail != -1 and tree[fail].trie_node.index_map[key] == 0; fail = tree[fail].next)
            { tree[fail].trie_node.index_map[key] = new_index; }
            if (fail == -1) { return; } // tree[new_index].next默认已经指向根结点
            size_type next_index = tree[fail].trie_node.index_map[key]; // next_index为新加入节点即将链接的下标
            if (tree[next_index].length == tree[fail].length + 1) { tree[new_index].next = next_index; return;} // 合法, 则链接成功
            // 不合法, 则即将链接的这个点需要分裂出一个虚空点
            size_type new_split_index = tree[new_index].next = split_node(next_index); // 新点tree[new_index]链接到虚空点
            tree[new_split_index].length = tree[fail].length + 1; // 虚空点的长度由fail确定
            for (; fail != -1 and tree[fail].trie_node.index_map[key] == next_index; fail = tree[fail].next)
            { tree[fail].trie_node.index_map[key] = new_split_index; } // 链接关系转移
        }
        std::vector<size_type> get_dfs_sorted_indexes() {
            // 根据length排序, 得到母树的dfs序
            std::vector<int> bucket(last_index + 1, 0); // bucket[0]不会被处理到, 也不用被处理到
            for (size_type i = 1; i <= last_index; ++i) { ++bucket[tree[i].length]; }
            for (size_type i = 1; i <= last_index; ++i) { bucket[i] += bucket[i - 1]; }
            std::vector<size_type> sorted(last_index + 1); // sorted[0]不会被处理到, 也不用被处理到
            for (size_type i = 1; i <= last_index; ++i) { sorted[bucket[tree[i].length]--] = i;}
            return sorted;         
        }
        size_t unique_substr_count() const {
            size_t count = 0;
            for (size_t i = 1; i <= last_index; ++i)
            { count += tree[i].length - tree[tree[i].next].length; }
            return count;
        }
        void clear() { 
            for (size_type i = 0; i <= last_index; ++i) { tree[i].clear(); }
            last_index = prev_index = 0;
            tree[0].next = -1;
        }
        inline __attribute__((__always_inline__))
        node_type& operator[](size_t index) { return tree[index]; }
        inline __attribute__((__always_inline__))
        const node_type& operator[](size_t index) const { return tree[index]; }
    private:
        size_type create_node() {
            if (++last_index == tree.size()) { tree.emplace_back(); }
            tree[last_index].length = tree[prev_index].length + 1;
            return last_index;
        }
        size_type split_node(size_type index) {
            if (++last_index == tree.size()) { tree.push_back(tree[index]); }
            else { tree[last_index] = tree[index]; } // 虚空点直接拷贝tree[index]
            tree[index].next = last_index; // tree[index].next改为虚空点下标
            return last_index;
        }
    private:
        std::vector<node_type> tree;
        size_type last_index; // 最后一个节点的下标(包括分裂出来的节点)
        size_type prev_index; // 记录最后一个被添加的叶节点的下标(不包括分裂出来的节点)
    };
}

namespace sam {
    // 洛谷	P3975 弦论
    template<typename TrieNodeType>
    struct UniqueNode : public TreeNodeBase<TrieNodeType> {
        using _Base = TreeNodeBase<TrieNodeType>;
        using _Self = UniqueNode<TrieNodeType>;
        using trie_node_type = typename _Base::trie_node_type;
        UniqueNode() : _Base(), endpos_size(1), total_size(1) { }
        UniqueNode(const _Self& other) : _Base(other), endpos_size(0), total_size(0) { }
        _Self& operator=(const _Self& other) { endpos_size = total_size = 0; _Base::operator=(other); return *this; }
        void combine(const _Self& other) { total_size = (endpos_size += other.endpos_size); }
        int endpos_size; // 当前节点控制的字符串作为后缀出现的次数
        int total_size; // endpos_size + "到当前节点的字符"作为前缀出现的次数 == 包含"到当前节点的字符"的字符串的出现次数
    };
    template<typename TrieNodeType>
    struct GenericNode : public TreeNodeBase<TrieNodeType> {
        using _Base = TreeNodeBase<TrieNodeType>;
        using _Self = GenericNode<TrieNodeType>;
        using trie_node_type = typename _Base::trie_node_type;
        GenericNode() : _Base(), endpos_size(1), total_size(1) { }
        int endpos_size, total_size;
    };
}

#endif