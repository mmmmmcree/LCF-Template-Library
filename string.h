#ifndef STRING_H
#define STRING_H
#include "vector.h"
#include <cstring>

namespace lcf {
    class string : public vector<char> {
        using _Base = vector<char>;
    public:
        string() = default;
        string(const char* c_str) : _Base(c_str, c_str + strlen(c_str)) { }
        string(size_t n) : _Base(n) { }
        string(size_t n, const_reference val) : _Base(n, val) { }
        string(const string& other) : _Base(other) { }
        string(const std::initializer_list<char>& list) : _Base(list) { }
        template<typename ForwarIterator, typename _ = decltype(*std::declval<ForwarIterator>())>
        string(ForwarIterator first, ForwarIterator last) : _Base(first, last) { }
        size_t length() const { return this->size(); }
        void append(const string& str) { this->insert(this->size(), str.begin(), str.end()); }
        void append(const char* c_str) { this->append(string(c_str)); }
    };
    string operator+(const string& lhs, const string& rhs) {
        string temp(lhs);
        temp.append(rhs);
        return temp;
    }
    string operator+(const string& lhs, const char* rhs) {
        string temp(lhs);
        temp.append(rhs);
        return temp;
    }
    string& operator+=(string& lhs, const string& rhs) {
        lhs.append(rhs);
        return lhs;
    }
    string& operator+=(string& lhs, const char* rhs) {
        lhs.append(rhs);
        return lhs;
    }
}

#endif