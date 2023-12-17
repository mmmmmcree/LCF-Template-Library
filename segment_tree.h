#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H
#include <vector>
#include <array>
#include <cmath>

namespace st { struct Node; }
namespace lcf {
    /*
    * 查询区间[left, right]
    * 原数据范围区间[first, last)
    */
    template<typename T, typename NodeType = st::Node, typename ContainerType = std::vector<T>>
    class segment_tree {
    #define lchild_idx ((cur_idx << 1) + 1)
    #define rchild_idx ((cur_idx << 1) + 2)
    public:
        using container_type = ContainerType;
        using value_type = typename container_type::value_type;
        using node_type = NodeType;
        using tree_type = std::vector<node_type>;
        using iterator = typename tree_type::iterator;
    public:
        segment_tree(size_t n)
        : _data(n, value_type()), _tree(pow(2, ceil(log2(std::size(_data))) + 1) - 1) {  _build(0, 0, std::size(_data)); }
        segment_tree(size_t n, const value_type& val)
        : _data(n, val), _tree(pow(2, ceil(log2(std::size(_data))) + 1) - 1) {  _build(0, 0, std::size(_data)); }
        segment_tree(const container_type& data)
        : _data(data), _tree(pow(2, ceil(log2(std::size(_data))) + 1) - 1) {  _build(0, 0, std::size(_data)); }
        segment_tree(container_type&& data)
        : _data(std::move(data)), _tree(pow(2, ceil(log2(std::size(_data))) + 1) - 1) {  _build(0, 0, std::size(_data)); }
        template<typename DataIterator, typename = decltype(*std::declval<DataIterator>())>
        segment_tree(DataIterator first, DataIterator last)
        : _data(first, last), _tree(pow(2, ceil(log2(std::size(_data))) + 1) - 1) {  _build(0, 0, std::size(_data)); }
        iterator begin() { return _tree.begin(); }
        iterator begin() const { return _tree.begin(); }
        iterator end() { return _tree.end(); }
        iterator end() const { return _tree.end(); }
        node_type& operator[](size_t index) { return _tree[index]; }
        node_type& operator[](size_t index) const { return _tree[index]; }
        //*----------------------------有lazy_tag更新----------------------------*//
        template<typename TagType>
        void update(TagType tag, size_t left, size_t right, value_type value) { _update(tag, left, right, value, 0, 0, std::size(_data)); }
        template<typename TagType>
        void update(TagType tag, size_t pos, value_type value) { _update(tag, pos, pos, value, 0, 0, std::size(_data)); }
        //*-------------------------------------------------------------------------*//
        //*----------------------------无lazy_tag更新----------------------------*//
        void update(size_t left, size_t right, value_type value) { _update(left, right, value, 0, 0, std::size(_data)); }
        void update(size_t pos, value_type value) { _update(pos, pos, value, 0, 0, std::size(_data)); }
        //*-------------------------------------------------------------------------*//
        node_type query(size_t left, size_t right) { return _query(left, right, 0, 0, std::size(_data)); }
        node_type query(size_t pos) { return _query(pos, pos, 0, 0, std::size(_data)); }
    private:
        void _push_up(size_t cur_idx) { _tree[cur_idx].combine(_tree[lchild_idx], _tree[rchild_idx]); }
        void _build(size_t cur_idx, size_t first, size_t last) {
            if constexpr (node_type::has_range_size::value == true) { _tree[cur_idx].assign_range_size(last - first); }
            if (first + 1 == last) { _tree[cur_idx].assign_value(_data[first]); return; }
            size_t mid = first + (last - first) / 2;
            _build(lchild_idx, first, mid);
            _build(rchild_idx, mid, last);
            _push_up(cur_idx);
        }
        void _push_down(size_t cur_idx) {
            if (not _tree[cur_idx].passable()) { return; }
            _tree[cur_idx].pass_tag_to(_tree[lchild_idx]);
            _tree[cur_idx].pass_tag_to(_tree[rchild_idx]);
            _tree[cur_idx].tag_init();
        }
        template<typename TagType>
        void _update(TagType tag, size_t left, size_t right, value_type value, size_t cur_idx, size_t first, size_t last) {
            if (left <= first and right + 1 >= last)
            { _tree[cur_idx].update_tag(tag, value); return; }
            _push_down(cur_idx);
            size_t mid = first + (last - first) / 2;
            if (left < mid) { _update(tag, left, right, value, lchild_idx, first, mid); }
            if (right >= mid) { _update(tag, left, right, value, rchild_idx, mid, last); }
            _push_up(cur_idx);
        }
        void _update(size_t left, size_t right, value_type value, size_t cur_idx, size_t first, size_t last) {
            if (left <= first and right + 1 >= last)
            { _tree[cur_idx].assign_value(value); return; }
            size_t mid = first + (last - first) / 2;
            if (left < mid) { _update(left, right, value, lchild_idx, first, mid); }
            if (right >= mid) { _update(left, right, value, rchild_idx, mid, last); }
            _push_up(cur_idx);
        }
        node_type _query(size_t left, size_t right, size_t cur_idx, size_t first, size_t last) {
            if (left >= last or right < first) { return node_type{}; }
            if (left <= first and right + 1 >= last) { return _tree[cur_idx]; }
            if constexpr (node_type::has_lazy_tag::value == true)
            { _push_down(cur_idx); }
            node_type result;
            size_t mid = first + (last - first) / 2;
            result.combine(_query(left, right, lchild_idx, first, mid), _query(left, right, rchild_idx, mid, last));
            return result;
        }
    private:
        container_type _data;
        std::vector<node_type> _tree;
    };
}

namespace st {
    //*--------------------用于update操作的重载--------------------*//
    namespace tags {
        struct add_tag { static constexpr int init_value = 0; };
        struct mul_tag { static constexpr int init_value = 1; };
        struct rev_tag { static constexpr bool init_value = false; };
        struct set_tag { static constexpr int init_value = -1;};
    }
    static constexpr tags::rev_tag rev_tag;
    static constexpr tags::set_tag set_tag;
    //*----------------------------------------------------------------*//

    class Node {
        using _self = Node;
    public:
        using has_lazy_tag = std::true_type;
        using has_range_size = std::true_type;
        enum {m_size = 2};
        enum {sum, longest, left, right};
        enum {set, rev, t_size};
        using value_type = int;
        using m_type = std::array<value_type, 4>;
    public:
        Node() : m_data(), t_data{init_tag_data} { }
        // 合并左右子节点信息，用于 segment_tree 的 push_up 和 query
        void combine(const _self& lchild, const _self& rchild) {
            for (size_t i = 0; i <= 1; ++i) {
                m_data[i][longest] = std::max(std::max(lchild.m_data[i][longest], rchild.m_data[i][longest]), lchild.m_data[i][right] + rchild.m_data[i][left]);
                m_data[i][sum] = lchild.m_data[i][sum] + rchild.m_data[i][sum];
                m_data[i][left] = lchild.m_data[i ^ 1][sum] != 0 ? lchild.m_data[i][left] : lchild.m_data[i][left] + rchild.m_data[i][left];
                m_data[i][right] = rchild.m_data[i ^ 1][sum] != 0 ? rchild.m_data[i][right] : lchild.m_data[i][right] + rchild.m_data[i][right];
            }
        }
        // 控制原数据赋值给Node的行为
        void assign_value(const value_type& value) {
            m_data[0].fill(value ^ 1);
            m_data[1].fill(value);
        }
        //*--------------------------实现不同标签的更新行为--------------------------*//
        void update_tag(tags::set_tag, const value_type& value) {
            t_data[rev] = false;
            t_data[set] = value;
            m_data[0].fill(range_size);
            m_data[1].fill(0);
            if (value != 0) { std::swap(m_data[0], m_data[1]); }
        }
        void update_tag(tags::rev_tag, const value_type& value = 0) {
            t_data[rev] ^= 1;
            std::swap(m_data[0], m_data[1]);
        }
        //*----在增加不同标签的更新行为后，还要更新使子节点数据正确的标签传递行为----*//
        void pass_tag_to(_self& child) {
            if (t_data[set] != -1) { child.update_tag(st::set_tag, t_data[set]); }
            if (t_data[rev] == true) { child.update_tag(st::rev_tag); }
        }
        //*-------------------------------------------------------------------------------*//
        void tag_init() { t_data = init_tag_data; }
        bool passable() { return t_data != init_tag_data; }
        // 用于建树时设置属于该节点控制的区间范围大小
        void assign_range_size(size_t _range_size) { range_size = _range_size; }
    public:
        std::array<m_type, m_size> m_data;
    private:
        size_t range_size;
        std::array<value_type, t_size> t_data;
        static constexpr std::array<value_type, t_size> init_tag_data{tags::set_tag::init_value, tags::rev_tag::init_value};
    };
}

// namespace st {
//     //*--------------------用于update操作的重载--------------------*//
//     namespace tags {
//         struct add_tag { static constexpr int init_value = 0; };
//         struct mul_tag { static constexpr int init_value = 1; };
//     }
//     static constexpr tags::add_tag add_tag;
//     static constexpr tags::mul_tag mul_tag;
//     //*----------------------------------------------------------------*//
//     template<typename T>
//     class Node {
//         using _self = Node<T>;
//     public:
//         using has_lazy_tag = std::true_type;
//         using has_range_size = std::true_type;
//         enum {sum, m_size};
//         enum {add, mul, t_size};
//     public:
//         Node() : m_data(), t_data{init_tag_data} { }
//         //*--------------------直接修改maintain_data的更新行为--------------------*//
//         // 用于push_up, 合并左右子节点信息
//         void combine(const _self& lchild, const _self& rchild) { m_data[sum] = lchild.m_data[sum] + rchild.m_data[sum]; }
//         // 控制原数据赋值给Node的行为
//         void assign_value(T value) { m_data[sum] = value; }
//         // 用于建树时设置属于该节点控制的区间范围大小
//         void assign_range_size(size_t _range_size) { range_size = _range_size; }
//         //*--------------------------实现不同标签的更新行为--------------------------*//
//         void update_tag(tags::add_tag, T value) {
//             m_data[sum] += value * range_size;
//             t_data[add] += value;
//         }
//         void update_tag(tags::mul_tag, T value) {
//             m_data[sum] *= value;
//             t_data[mul] *= value;
//             t_data[add] *= value;
//         }
//         //*---------在增加不同标签的更新行为后，还要更新标签的传递行为---------*//
//         void pass_tag_to(_self& child) {
//             (child.m_data[sum] *= t_data[mul]) += t_data[add] * child.range_size;
//             child.t_data[mul] *= t_data[mul];
//             (child.t_data[add] *= t_data[mul]) += t_data[add];
//         }
//         //*-------------------------------------------------------------------------------*//
//         void tag_init() { t_data = init_tag_data; }
//         bool passable() { return t_data != init_tag_data; }
//     public:
//         std::array<T, m_size> m_data; // maintain_data, 存储一个或多个区间信息
//     private:
//         size_t range_size;
//         std::array<T, t_size> t_data; // tag_data
//         static constexpr std::array<T, t_size> init_tag_data{tags::add_tag::init_value, tags::mul_tag::init_value};
//     };
// }

#endif

/*
洛谷 P1607
namespace st {
    namespace tags {
        struct add_tag { static constexpr int init_value = 0; };
    }
    static constexpr tags::add_tag add_tag;
    template<typename T>
    class Node {
        using _self = Node<T>;
    public:
        using has_lazy_tag = std::true_type;
        using has_range_size = std::false_type;
        enum {max, m_size};
        enum {add, t_size};
    public:
        Node() : m_data(), t_data{init_tag_data} { }
        void combine(const _self& lchild, const _self& rchild) { m_data[max] = std::max(lchild.m_data[max], rchild.m_data[max]); }
        void combine(const _self& child) { m_data[max] = std::max(m_data[max], child.m_data[max]); }
        void assign_value(const T& value) { m_data[max] = value; }
        void update_tag(tags::add_tag, T value) {
            m_data[max] += value;
            t_data[add] += value;
        }
        void pass_tag_to(_self& child) {
            child.m_data[max] += t_data[add];
            child.t_data[max] += t_data[add];
        }
        void tag_init() { t_data = init_tag_data; }
        bool passable() { return t_data != init_tag_data; }
    public:
        std::array<T, m_size> m_data;
    private:
        std::array<T, t_size> t_data;
        static constexpr std::array<T, t_size> init_tag_data{tags::add_tag::init_value};
    };
}
*/

// 洛谷 P2572
// namespace st {
//     //*--------------------用于update操作的重载--------------------*//
//     namespace tags {
//         struct add_tag { static constexpr int init_value = 0; };
//         struct mul_tag { static constexpr int init_value = 1; };
//         struct rev_tag { static constexpr bool init_value = false; };
//         struct set_tag { static constexpr int init_value = -1;};
//     }
//     static constexpr tags::rev_tag rev_tag;
//     static constexpr tags::set_tag set_tag;
//     //*----------------------------------------------------------------*//
//     template<typename T>
//     class Node {
//         using _self = Node<T>;
//     public:
//         using has_lazy_tag = std::true_type;
//         using has_range_size = std::true_type;
//         enum {sum_zero, longest_zero, left_zero, right_zero, sum_one, longest_one, left_one, right_one, m_size};
//         enum {set, rev, t_size};
//     public:
//         Node() : m_data(), t_data{init_tag_data} { }
//         void combine(const _self& lchild, const _self& rchild) {
//             m_data[longest_one] = std::max(std::max(lchild.m_data[longest_one], rchild.m_data[longest_one]), lchild.m_data[right_one] + rchild.m_data[left_one]);
//             m_data[longest_zero] = std::max(std::max(lchild.m_data[longest_zero], rchild.m_data[longest_zero]), lchild.m_data[right_zero] + rchild.m_data[left_zero]);
//             m_data[sum_one] = lchild.m_data[sum_one] + rchild.m_data[sum_one];
//             m_data[sum_zero] = lchild.m_data[sum_zero] + rchild.m_data[sum_zero];
//             m_data[left_one] = lchild.m_data[sum_zero] != 0 ? lchild.m_data[left_one] : lchild.m_data[left_one] + rchild.m_data[left_one];
//             m_data[right_one] = rchild.m_data[sum_zero] != 0 ? rchild.m_data[right_one] : lchild.m_data[right_one] + rchild.m_data[right_one];
//             m_data[left_zero] = lchild.m_data[sum_one] != 0 ? lchild.m_data[left_zero] : lchild.m_data[left_zero] + rchild.m_data[left_zero];
//             m_data[right_zero] = rchild.m_data[sum_one] != 0 ? rchild.m_data[right_zero] : lchild.m_data[right_zero] + rchild.m_data[right_zero];
//         }
//         void assign_value(const T& value) {
//             std::fill_n(m_data.begin(), 4, value ^ 1);
//             std::fill_n(m_data.begin() + 4, 4, value);
//         }
//         void update_tag(tags::set_tag, const T& value) {
//             t_data[rev] = false;
//             t_data[set] = value;
//             if (value == 0) {
//                 std::fill_n(m_data.begin(), 4, range_size);
//                 std::fill_n(m_data.begin() + 4, 4, 0);
//             }
//             if (value == 1) {
//                 std::fill_n(m_data.begin(), 4, 0);
//                 std::fill_n(m_data.begin() + 4, 4, range_size);
//             }
//         }
//         void update_tag(tags::rev_tag, const T& value = 0) {
//             t_data[rev] ^= 1;
//             for (size_t i = 0; i < 4; ++i) { std::swap(m_data[i], m_data[i + 4]); }
//         }
//         void pass_tag_to(_self& child) {
//             if (t_data[set] != -1) { child.update_tag(st::set_tag, t_data[set]); }
//             if (t_data[rev] == true) { child.update_tag(st::rev_tag); }
//         }
//         //*-------------------------------------------------------------------------------*//
//         void tag_init() { t_data = init_tag_data; }
//         bool passable() { return t_data != init_tag_data; }
//         void assign_range_size(size_t _range_size) { range_size = _range_size; }
//     public:
//         std::array<T, m_size> m_data; // maintain_data, 存储一个或多个区间信息
//     private:
//         size_t range_size;
//         std::array<T, t_size> t_data; // tag_data
//         static constexpr std::array<T, t_size> init_tag_data{tags::set_tag::init_value, tags::rev_tag::init_value};
//     };
// }