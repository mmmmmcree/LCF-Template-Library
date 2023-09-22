#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H
#include <vector>
#include <array>
#include <cmath>

namespace segment_tree_nodes {
    //*--------------------用于update操作的重载--------------------*//
    namespace tags {
        struct add_tag { static constexpr int init_value = 0; };
        struct mul_tag { static constexpr int init_value = 1; };
    }
    static constexpr tags::add_tag add_tag;
    static constexpr tags::mul_tag mul_tag;
    //*----------------------------------------------------------------*//
    template<typename T>
    class Node {
        using _self = Node<T>;
    public:
        using has_lazy_tag = std::true_type;
        using has_range_size = std::true_type;
        enum {sum, m_size};
        enum {add, mul, t_size};
    public:
        Node() : m_data(), t_data{init_tag_data} { }
        //*--------------------直接修改maintain_data的更新行为--------------------*//
        // 用于push_up, 合并左右子节点信息
        void combine(const _self& lchild, const _self& rchild) { m_data[sum] = lchild.m_data[sum] + rchild.m_data[sum]; }
        // 用于query, 合并某一子节点信息
        void combine(const _self& child) { m_data[sum] += child.m_data[sum]; }
        // 控制原数据赋值给Node的行为
        void assign_value(T value) { m_data[sum] = value; }
        // 用于建树时设置属于该节点控制的区间范围大小
        void assign_range_size(size_t _range_size) { range_size = _range_size; }
        //*------------------------------------------------------------------------------*//
        //*--------------------------实现不同标签的更新行为--------------------------*//
        void update_tag(tags::add_tag, T value) {
            m_data[sum] += value * range_size;
            t_data[add] += value;
        }
        void update_tag(tags::mul_tag, T value) {
            m_data[sum] *= value;
            t_data[mul] *= value;
            t_data[add] *= value;
        }
        //*-------------------------------------------------------------------------------*//
        //*---------在增加不同标签的更新行为后，还要更新标签的传递行为---------*//
        void pass_tag_to(_self& child) {
            // child.update_tag(mul_tag, t_data[mul]);
            // child.update_tag(add_tag, t_data[add]);
            (child.m_data[sum] *= t_data[mul]) += t_data[add] * child.range_size;
            child.t_data[mul] *= t_data[mul];
            (child.t_data[add] *= t_data[mul]) += t_data[add];
        }
        //*-------------------------------------------------------------------------------*//
        void tag_init() { t_data = init_tag_data; }
        bool passable() { return t_data != init_tag_data; }
    public:
        std::array<T, m_size> m_data; // maintain_data, 存储一个或多个区间信息
    private:
        size_t range_size;
        std::array<T, t_size> t_data; // tag_data
        static constexpr std::array<T, t_size> init_tag_data{tags::add_tag::init_value, tags::mul_tag::init_value};
    };
}

namespace lcf {
    /*
    * 查询区间[left, right]
    * 原数据范围区间[first, last)
    */
    template<typename ContainerType, template<typename> typename NodeType = segment_tree_nodes::Node>
    class segment_tree {
    #define lchild_idx ((cur_idx << 1) + 1)
    #define rchild_idx ((cur_idx << 1) + 2)
    public:
        using container_type = ContainerType;
        using value_type = typename container_type::value_type;
        using node_type = NodeType<value_type>;
        using tree_type = std::vector<node_type>;
        using iterator = typename tree_type::iterator;
    public:
        segment_tree(const container_type& data)
        : _data(data), _tree(pow(2, ceil(log2(_data.size())) + 1) - 1) {  _build(0, 0, _data.size()); }
        segment_tree(container_type&& data)
        : _data(std::move(data)), _tree(pow(2, ceil(log2(_data.size())) + 1) - 1) {  _build(0, 0, _data.size()); }
        iterator begin() { return _tree.begin(); }
        iterator end() { return _tree.end(); }
        //*----------------------------有lazy_tag更新----------------------------*//
        template<typename TagType>
        void update(TagType tag, size_t left, size_t right, value_type value) { _update(tag, left, right, value, 0, 0, _data.size()); }
        template<typename TagType>
        void update(TagType tag, size_t pos, value_type value) { _update(tag, pos, pos, value, 0, 0, _data.size()); }
        //*-------------------------------------------------------------------------*//
        //*----------------------------无lazy_tag更新----------------------------*//
        void update(size_t left, size_t right, value_type value) { _update(left, right, value, 0, 0, _data.size()); }
        void update(size_t pos, value_type value) { _update(pos, pos, value, 0, 0, _data.size()); }
        //*-------------------------------------------------------------------------*//
        node_type query(size_t left, size_t right) { return _query(left, right, 0, 0, _data.size()); }
        node_type query(size_t pos) { return _query(pos, pos, 0, 0, _data.size()); }
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
        void _push_down(size_t cur_idx, size_t first, size_t last) {
            if (not _tree[cur_idx].passable()) { return; }
            _tree[cur_idx].pass_tag_to(_tree[lchild_idx]);
            _tree[cur_idx].pass_tag_to(_tree[rchild_idx]);
            _tree[cur_idx].tag_init();
        }
        template<typename TagType>
        void _update(TagType tag, size_t left, size_t right, value_type value, size_t cur_idx, size_t first, size_t last) {
            if (left <= first and right + 1 >= last)
            { _tree[cur_idx].update_tag(tag, value); return; }
            _push_down(cur_idx, first, last);
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
            if (left <= first and right + 1 >= last) { return _tree[cur_idx]; }
            if constexpr (node_type::has_lazy_tag::value == true)
            { _push_down(cur_idx, first, last); }
            node_type result;
            size_t mid = first + (last - first) / 2;
            if (left < mid) { result.combine(_query(left, right, lchild_idx, first, mid)); }
            if (right >= mid) { result.combine(_query(left, right, rchild_idx, mid, last)); }
            return result;
        }
    private:
        container_type _data;
        std::vector<node_type> _tree;
    };
}

namespace segment_tree_nodes {
    template<typename T>
    struct MaxNode {
        using _self = MaxNode<T>;
        using has_lazy_tag = std::false_type;
        using has_range_size = std::false_type;
        enum {max, m_size};
        MaxNode() : m_data{std::numeric_limits<T>::lowest()} { }
        void combine(const _self& lchild, const _self& rchild) { m_data[max] = std::max(lchild.m_data[max], rchild.m_data[max]); }
        void combine(const _self& child) { m_data[max] = std::max(m_data[max], child.m_data[max]); }
        void assign_value(T value) { m_data[max] = value; }
        std::array<T, m_size> m_data;
    };
}

namespace segment_tree_nodes {
    template<typename T>
    struct MaxAndMinNode {
        using _self = MaxAndMinNode<T>;
        using has_lazy_tag = std::false_type;
        using has_range_size = std::false_type;
        enum {max, min, m_size};
        MaxAndMinNode() : m_data{std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()} { }
        void combine(const _self& lchild, const _self& rchild) {
            m_data[max] = std::max(lchild.m_data[max], rchild.m_data[max]); 
            m_data[min] = std::min(lchild.m_data[min], rchild.m_data[min]); 
        }
        void combine(const _self& child) {
            m_data[max] = std::max(m_data[max], child.m_data[max]);
            m_data[min] = std::min(m_data[min], child.m_data[min]);
        }
        void assign_value(T value) { m_data[max] = m_data[min] = value; }
        std::array<T, m_size> m_data;
    };
}

namespace segment_tree_nodes {
    template<typename T>
    struct ProductNode {
        using _self = ProductNode<T>;
        using has_lazy_tag = std::false_type;
        using has_range_size = std::false_type;
        enum {product, m_size};
        ProductNode() : m_data{1} { }
        void combine(const _self& lchild, const _self& rchild)
        { m_data[product] = lchild.m_data[product] * rchild.m_data[product]; }
        void combine(const _self& child) { m_data[product] *= child.m_data[product]; }
        void assign_value(T value) { m_data[product] = value; }
        std::array<T, m_size> m_data;
    };
}

#endif