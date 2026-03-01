/**
 * github: https://github.com/hyforgh/jsoncfg
 */

/*
MIT License

Copyright (c) 2026-2035 Liu Hongyun

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __JSONCFG_HPP__
#define __JSONCFG_HPP__

#include <string.h>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <memory>
#include <vector>

namespace jsoncfg {

#define JSONCFG_VERSION_MAJOR 1
#define JSONCFG_VERSION_MINOR 0
#define JSONCFG_VERSION_PATCH 0

enum class Type {
    JSON = 0,
    STR,
    DEC,
    INT,
    BOOL,
    DICT,
    LIST,
    COUNT
}; // Type

class Interface : public std::enable_shared_from_this<Interface> {
public:
    virtual ~Interface() {}
    virtual Type type() const = 0;
    virtual void dumps(std::ostream &os
            , unsigned indent = 0, bool keep_layout = false) const {
        dumps_depth(os, indent, keep_layout, 0);
    }
    virtual const char *loads(const char *psz) = 0;
    virtual std::string dumps(unsigned indent = 0, bool keep_layout = false) const {
        std::stringstream ss;
        dumps(ss, indent, keep_layout);
        return ss.str();
    }
    virtual size_t loads(const std::string &s) {
        auto p = s.data();
        auto e = loads(p);
        return e - p;
    }
public:
    static const char *skip_blank(const char *p, char *has_newline = nullptr) {
        if (has_newline) {
            *has_newline = false;
        }
        if (!p) {
            return p;
        }
        while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') {
            if (*p == '\n' && has_newline) {
                *has_newline = true;
            }
            ++p;
        }
        return p;
    }
    virtual bool is_head() const = 0;
    virtual void as_head(bool is_head) = 0;
protected:
    friend class Json;
    virtual void dumps_depth(std::ostream &os
            , unsigned indent, bool keep_layout, unsigned depth) const = 0;
    void dumps_head_indent(std::ostream &os, unsigned indent, unsigned depth) const {
        os << "\n";
        for (unsigned i = 0; i < indent * depth; ++i) {
            os << " ";
        }
    }
}; // Interface

template <Type JT, typename DT>
class Value : public Interface {
public:
    enum { TYPE = static_cast<int>(JT) };
    typedef DT value_t;
public:
    virtual ~Value() {}
    Value(value_t v, bool is_head = false)
        : _value(std::move(v)), _is_head(is_head) {}
    Value &operator = (value_t v) {
        _value = std::move(v);
        return *this;
    }
    bool operator == (const value_t &v) const {
        return _value == v;
    }
    bool operator == (const Value &v) const {
        return _value == v._value;
    }
    Type type() const override { return JT; }
    value_t &value() { return _value; }
    const value_t &value() const { return _value; }
    bool is_head() const override { return _is_head; }
    void as_head(bool is_head) override { _is_head = is_head; }
protected:
    DT _value;
    char _is_head;
};
template <Type JT, typename DT>
bool operator == (const DT &v, const Value<JT, DT> &obj) {
    return obj == v;
}

template <typename JT>
class __Agent {
public:
    typedef typename JT::value_t value_t;
public:
    __Agent(std::shared_ptr<JT> v = std::make_shared<JT>()) : _value(std::move(v)) {}
    virtual ~__Agent() {}
    operator bool () const {
        return _value.get() != nullptr;
    }
    JT *operator -> () {
        return _value.get();
    }
    const JT *operator -> () const {
        return _value.get();
    }
    JT &operator * () {
        return *_value.get();
    }
    const JT &operator * () const {
        return *_value.get();
    }
    JT &operator = (value_t v) {
        auto &obj = *_value;
        obj = v;
        return obj;
    }
    std::shared_ptr<JT> agent() {
        return _value;
    }
protected:
    std::shared_ptr<JT> _value;
};

template <typename JT>
class JsonT : public __Agent<JT> {
public:
    using __Agent<JT>::operator =;
public:
    JsonT(std::shared_ptr<JT> v = std::make_shared<JT>())
        : __Agent<JT>(std::move(v)) {}
};

template <typename DT>
struct __dt_2_jt { typedef DT type; };
class Json final : public Value<Type::JSON, std::shared_ptr<Interface>> {
public:
    using Interface::loads;
public:
    Json() : Value(nullptr) {}
    template <typename T>
    Json(T v);
    template <typename T>
    Json &operator = (T v);
    const char *loads(const char *psz) override;
    bool is_head() const override {
        if (_value) {
            return _value->is_head();
        }
        return _is_head;
    }
    void as_head(bool is_head) override {
        if (_value) {
            _value->as_head(is_head);
        }
        _is_head = is_head;
    }
    template <typename T>
    JsonT<typename __dt_2_jt<T>::type> as() {
        return as<T>(_value);
    }
    template <typename T>
    JsonT<typename __dt_2_jt<T>::type> to() {
        auto v = to<T>(_value);
        _value = v;
        return v;
    }
public:
    template <typename T>
    static std::shared_ptr<typename __dt_2_jt<T>::type> as(std::shared_ptr<Interface> v) {
        static_assert(std::is_base_of<Interface, typename __dt_2_jt<T>::type>::value,
            "Json::as<T>() : not supported type");
        return nullptr;
    }
    template <typename T>
    static std::shared_ptr<typename __dt_2_jt<T>::type> to(std::shared_ptr<Interface> v);
protected:
    friend class Dict;
    friend class List;
    void dumps_depth(std::ostream &os
            , unsigned indent, bool keep_layout, unsigned depth) const override {
        if (_value) {
            _value->dumps_depth(os, indent, keep_layout, depth);
        } else {
            os << "null";
        }
    }
}; // Json

class Str final : public Value<Type::STR, std::string> {
public:
    using Interface::loads;
public:
    Str(value_t v = "", bool is_head = false) : Value(std::move(v), is_head) {}
    Str &operator = (const char *v) {
        _value = v;
        return *this;
    }
    const char *loads(const char *psz) override {
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz, &_is_head);
        if (*p != '"') {
            return psz;
        }
        auto e = p + 1;
        while (*e) {
            if (*e == '"' && e[-1] != '\\') {
                _value.resize(e - p, '\0');
                _value.assign(p +  1, e);
                return e + 1;
            }
            ++e;
        }
        return psz;
    }
protected:
    void dumps_depth(std::ostream &os
            , unsigned indent, bool keep_layout, unsigned depth) const override {
        os << "\"" << _value << "\"";
    }
}; // Str

template <>
class JsonT<Str> : public __Agent<Str> {
public:
    using __Agent::operator =;
public:
    JsonT(std::shared_ptr<Str> v = std::make_shared<Str>())
        : __Agent<Str>(std::move(v)) {}
    char &operator [] (unsigned i) {
        return _value->value()[i];
    }
};

class Dec final : public Value<Type::DEC, double> {
public:
    using Interface::loads;
public:
    Dec(value_t v = 0, bool scientific = false, bool is_head = false)
        : Value(v, is_head), _scientific(scientific) {}
    const char *loads(const char *psz) override {
        if (!psz) {
            return psz;
        }
        _scientific = false;
        auto p = skip_blank(psz, &_is_head);
        char *e;
        _value = std::strtold(p, &e);
        if (e != p) {
            auto q = e;
            while (q != p) {
                if (*q == 'e' || *q == 'E') {
                    _scientific = true;
                    break;
                }
                --q;
            }
            return e;
        }
        return psz;
    }
    bool scientific() const { return _scientific; }
    void scientific(bool is) { _scientific = is; }
protected:
    void dumps_depth(std::ostream &os
            , unsigned indent, bool keep_layout, unsigned depth) const override {
        auto os_flags = os.flags();
        if (_scientific) {
            os.flags(os_flags | std::ios_base::scientific);
        }
        os << _value;
        os.flags(os_flags);
    }
private:
    bool _scientific;
}; // Dec

union __int_value_t {
    unsigned long long u;
    long long s;
};
class Int : public Value<Type::INT, __int_value_t> {
public:
    using Interface::loads;
public:
    Int(bool is_head = false)
            : Value({0}, is_head), _is_signed(0), _base(10) {
    }
    Int(unsigned long long v, unsigned base = 10, bool is_head = false)
            : Value({0}, is_head), _is_signed(0) {
        _value.u = v;
        this->base(base);
    }
    Int(long long v, bool is_head = false)
            : Value({0}, is_head), _is_signed(1), _base(10) {
        _value.s = v;
    }
    const char *loads(const char *psz) override {
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz, &_is_head);
        if (*p == '-') {
            _is_signed = 1;
            p = skip_blank(p + 1);
        }
        char *e;
        _base = 10;
        if (*p == '0') {
            ++p;
            if (*p == 'x' || *p == 'X') {
                _base = 16;
            } else if (*p == 'o' || *p == 'O') {
                _base = 8;
            } else if (*p == 'b' || *p == 'B') {
                _base = 2;
            }
            if (_base != 10) {
                ++p;
            }
        }
        _value.u = std::strtoull(p, &e, _base);
        if (e != p) {
            return e;
        }
        if (_is_signed) {
            _value.s = -_value.s;
        }
        return psz;
    }
    bool is_signed() const { return _is_signed; }
    unsigned base() const { return _base; }
    void base(unsigned b) {
        _base = b;
        if (_base != 16 && _base != 8 && _base != 2) {
            _base = 10;
        }
    }
    template <typename Tint>
    Tint &as() {
        return static_cast<Tint &>(*this);
    }
    template <typename Tint>
    const Tint &as() const {
        return static_cast<const Tint &>(*this);
    }
protected:
    void dumps_depth(std::ostream &os
            , unsigned indent, bool keep_layout, unsigned depth) const override {
        unsigned long long v = _value.u;
        if (_is_signed && _value.s < 0) {
            v = -_value.s;
            os << "-";
        }
        switch (_base) {
        case 2: os << "0b"; break;
        case 8: os << "0o"; break;
        case 16: os << "0x"; break;
        default: os << v; return;
        }
        std::vector<int> items; items.reserve(64);
        while (v) {
            items.push_back(v % _base);
            v /= _base;
        }
        for (auto it = items.rbegin(); it != items.rend(); ++it) {
            if (*it > 9) {
                os << (*it - 10 + 'A');
            }
            os << *it;
        }
    }
protected:
    char _is_signed;
    char _base;
}; // Int

class Uint final : public Int {
public:
    typedef unsigned long long value_t;
public:
    Uint(value_t v = 0, unsigned base = 10, bool is_head = false)
        : Int(v, base, is_head) {}
    bool operator == (const value_t &v) const {
        return _value.u == v;
    }
    value_t &value() { return _value.u; }
    const value_t &value() const { return _value.u; }
}; // Uint

class Sint final : public Int {
public:
    typedef long long value_t;
public:
    Sint(value_t v = 0, bool is_head = false)
        : Int(v, is_head) {}
    bool operator == (const value_t &v) const {
        return _value.s == v;
    }
    value_t &value() { return _value.s; }
    const value_t &value() const { return _value.s; }
}; // Sint

class Bool final : public Value<Type::BOOL, bool> {
public:
    using Interface::loads;
public:
    Bool(value_t v = false, bool is_head = false) : Value(v, is_head) {}
    const char *loads(const char *psz) override {
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz, &_is_head);
        if (strncmp(p, "true", 4) == 0) {
            _value = true;
            return p + 4;
        } else if (strncmp(p, "false", 5) == 0) {
            _value = false;
            return p + 5;
        }
        return psz;
    }
protected:
    void dumps_depth(std::ostream &os
            , unsigned indent, bool keep_layout, unsigned depth) const override {
        os << (_value ? "true" : "false");
    }
}; // Bool

template <typename TK, typename TV>
class OrderedDict {
private:
    typedef std::list<std::pair<TK, TV>> list_t;
    typedef std::map<TK, typename list_t::iterator> dict_t;
public:
    typedef typename list_t::iterator  iterator;
    typedef typename list_t::const_iterator const_iterator;
    typedef typename list_t::reverse_iterator  reverse_iterator;
    typedef typename list_t::const_reverse_iterator const_reverse_iterator;
public:
    OrderedDict(std::initializer_list<std::pair<TK, TV>> init) {
        for (auto &it : init) {
            auto itd = _dict.find(it.first);
            if (itd != _dict.end()) {
                itd->second->second = it.second;
                continue;
            }
            _list.emplace_back(it.first, it.second);
            _dict[it.first] = --_list.end();
        }
    }
    TV &operator [] (const TK &key) {
        auto it = _dict.find(key);
        if (it == _dict.end()) {
            _list.emplace_back(key, TV());
            _dict[key] = --_list.end();
            return _list.rbegin()->second;
        }
        return it->second->second;
    }
    iterator find(const TK &key) {
        auto it = _dict.find(key);
        if (it == _dict.end()) {
            return _list.end();
        }
        return it->second;
    }
    const_iterator find(const TK &key) const {
        auto it = _dict.find(key);
        if (it == _dict.end()) {
            return _list.end();
        }
        return it->second;
    }
    void clear() {
        _list.clear();
        _dict.clear();
    }
    size_t size() const { return _list.size(); }
    iterator begin() { return _list.begin(); }
    iterator end() { return _list.end(); }
    const_iterator begin() const { return _list.begin(); }
    const_iterator end() const { return _list.end(); }
    reverse_iterator rbegin() { return _list.rbegin(); }
    reverse_iterator rend() { return _list.rend(); }
    const_reverse_iterator rbegin() const { return _list.rbegin(); }
    const_reverse_iterator rend() const { return _list.rend(); }
private:
    list_t _list;
    dict_t _dict;
};
class Dict final : public Value<Type::DICT, OrderedDict<std::string, Json>> {
public:
    using Interface::loads;
    typedef value_t::iterator iterator;
    typedef value_t::const_iterator const_iterator;
    typedef value_t::reverse_iterator reverse_iterator;
    typedef value_t::const_reverse_iterator const_reverse_iterator;
public:
    Dict(value_t v = {}, unsigned items_per_line = 1, bool is_head = false)
        : Value(std::move(v), is_head), _width(items_per_line) {}
    const char *loads(const char *psz) override {
        _value.clear();
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz, &_is_head);
        if (*p != '{') {
            return psz;
        }
        _width = 0;
        char is_head = false;
        unsigned width = 0;
        p = skip_blank(p + 1, &is_head);
        while (*p) {
            if (*p == '}') {
                break;
            }
            Str k;
            auto e = k.loads(p);
            if (e == p) {
                return psz;
            }
            if (!k.is_head()) {
                k.as_head(is_head);
            }
            p = skip_blank(e);
            if (*p != ':') {
                return psz;
            }
            ++p;
            Json v;
            e = v.loads(p);
            if (e == p) {
                return psz;
            }
            v.as_head(k.is_head());
            ++width;
            if (v.is_head()) {
                if (_width < width) {
                    _width = width;
                }
                width = 1;
            }
            p = skip_blank(e, &is_head);
            if (*p == ',' || *p == '}') {
                _value[k.value()] = std::move(v);
                if (*p == '}') {
                    break;
                }
            } else {
                return psz;
            }
            ++p;
        }
        if (*p == '}') {
            if (_width && _width < width) {
                _width = width;
            }
            return p + 1;
        }
        return psz;
    }
    Json &operator [] (const std::string &k) {
        return _value[k];
    }
    Json &operator [] (const char *k) {
        return _value[k];
    }
    Dict &set(const std::string &k, Json v) {
        _value[k] = std::move(v);
        return *this;
    }
    Json get(const std::string &k, Json default_value = Json()) const {
        auto it = _value.find(k);
        if (it == _value.end()) {
            return default_value;
        }
        return it->second;
    }
    iterator find(const std::string &key) {
        return _value.find(key);
    }
    const_iterator find(const std::string &key) const {
        return _value.find(key);
    }
    void clear() { _value.clear(); }
    size_t size() const { return _value.size(); }
    iterator begin() { return _value.begin(); }
    iterator end() { return _value.end(); }
    const_iterator begin() const { return _value.begin(); }
    const_iterator end() const { return _value.end(); }
    reverse_iterator rbegin() { return _value.rbegin(); }
    reverse_iterator rend() { return _value.rend(); }
    const_reverse_iterator rbegin() const { return _value.rbegin(); }
    const_reverse_iterator rend() const { return _value.rend(); }
protected:
    void dumps_depth(std::ostream &os
            , unsigned indent, bool keep_layout, unsigned depth) const override {
        os << "{";
        unsigned width = 0;
        bool is_first = true;
        for (auto &it : _value) {
            if (!is_first) {
                os << ",";
            }
            if (indent) {
                if (keep_layout) {
                    ++width;
                    if ((is_first && _width) || it.second.is_head() || (_width && width >= _width)) {
                        dumps_head_indent(os, indent, depth + 1);
                        width = 0;
                    }
                } else {
                    dumps_head_indent(os, indent, depth + 1);
                }
            }
            if (!is_first && width) {
                os << " ";
            }
            os << "\"" << it.first << "\":";
            if (indent) {
                os << " ";
            }
            it.second.dumps_depth(os, indent, keep_layout, depth + 1);
            is_first = false;
        }
        if (indent && (!keep_layout || _width)) {
            dumps_head_indent(os, indent, depth);
        }
        os << "}";
    }
protected:
    unsigned _width;
}; // Dict

template <>
class JsonT<Dict> : public __Agent<Dict> {
public:
    typedef value_t::iterator iterator;
    typedef value_t::const_iterator const_iterator;
public:
    JsonT(std::shared_ptr<Dict> v = std::make_shared<Dict>())
        : __Agent<Dict>(std::move(v)) {}
    Json &operator [] (const std::string &k) {
        return (*_value)[k];
    }
    Json &operator [] (const char *k) {
        return (*_value)[k];
    }
};

class List final : public Value<Type::LIST, std::list<Json>> {
public:
    using Interface::loads;
    typedef value_t::iterator iterator;
    typedef value_t::const_iterator const_iterator;
    typedef value_t::reverse_iterator reverse_iterator;
    typedef value_t::const_reverse_iterator const_reverse_iterator;
public:
    List(value_t v = {}, unsigned items_per_line = 1, bool is_head = false)
        : Value(std::move(v), is_head), _width(items_per_line) {}
    const char *loads(const char *psz) override {
        _value.clear();
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz, &_is_head);
        if (*p != '[') {
            return psz;
        }
        _width = 0;
        char is_head = false;
        unsigned width = 0;
        p = skip_blank(p + 1, &is_head);
        while (*p) {
            if (*p == ']') {
                break;
            }
            Json v;
            auto e = v.loads(p);
            if (e == p) {
                return psz;
            }
            if (!v.is_head()) {
                v.as_head(is_head);
            }
            ++width;
            if (v.is_head()) {
                if (_width < width) {
                    _width = width;
                }
                width = 1;
            }
            p = skip_blank(e, &is_head);
            if (*p == ',' || *p == ']') {
                append(std::move(v));
                if (*p == ']') {
                    break;
                }
            } else {
                return psz;
            }
            ++p;
        }
        if (*p == ']') {
            if (_width && _width < width) {
                _width = width;
            }
            return p + 1;
        }
        return psz;
    }
    List &append(Json v) {
        _value.emplace_back(std::move(v));
        return *this;
    }
    List &insert(int i, Json v) {
        _value.emplace(iterator_at(i), std::move(v));
        return *this;
    }
    Json &get(int i) {
        return *iterator_at(i);
    }
    const Json &get(int i) const {
        return *iterator_at(i);
    }
    List &set(int i, Json v) {
        auto it = iterator_at(i);
        *it = std::move(v);
        return *this;
    }
    std::vector<iterator> iterators() {
        std::vector<iterator> vec; vec.reserve(_value.size());
        for (auto it = _value.begin(); it != _value.end(); ++it) {
            vec.emplace_back(it);
        }
        return vec;
    }
    template<typename T>
    std::vector<T> vector(const T &default_value) const {
        std::vector<T> vec; vec.reserve(_value.size());
        for (auto &it : _value) {
            auto v = Json::to<T>(it.value());
            if (v) {
                vec.emplace_back(v->value());
            } else {
                vec.emplace_back(default_value);
            }
        }
        return vec;
    }
    void clear() { _value.clear(); }
    size_t size() const { return _value.size(); }
    iterator begin() { return _value.begin(); }
    iterator end() { return _value.end(); }
    const_iterator begin() const { return _value.begin(); }
    const_iterator end() const { return _value.end(); }
    reverse_iterator rbegin() { return _value.rbegin(); }
    reverse_iterator rend() { return _value.rend(); }
    const_reverse_iterator rbegin() const { return _value.rbegin(); }
    const_reverse_iterator rend() const { return _value.rend(); }
protected:
    void dumps_depth(std::ostream &os
            , unsigned indent, bool keep_layout, unsigned depth) const override {
        os << "[";
        unsigned width = 0;
        bool is_first = true;
        for (auto &it : _value) {
            if (!is_first) {
                os << ",";
            }
            if (indent) {
                if (keep_layout) {
                    ++width;
                    if ((is_first && _width) || it.is_head() || (_width && width >= _width)) {
                        dumps_head_indent(os, indent, depth + 1);
                        width = 0;
                    }
                } else {
                    dumps_head_indent(os, indent, depth + 1);
                }
            }
            if (!is_first && width) {
                os << " ";
            }
            it.dumps_depth(os, indent, keep_layout, depth + 1);
            is_first = false;
        }
        if (indent && (!keep_layout || _width)) {
            dumps_head_indent(os, indent, depth);
        }
        os << "]";
    }
private:
    iterator iterator_at(int i) {
#define ITERATOR_AT()               \
        int n = _value.size();      \
        if (i < 0) {                \
            i += n;                 \
        } else if (i >= n) {        \
            return _value.end();    \
        }                           \
        if (2 * i > n) {            \
            auto it = _value.end(); \
            i = n - i;              \
            while (i--) --it;       \
            return it;              \
        }                           \
        auto it = _value.begin();   \
        while (i--) ++it;           \
        return it;
        ITERATOR_AT();
    }
    const_iterator iterator_at(int i) const {
        ITERATOR_AT();
#undef ITERATOR_AT
    }
protected:
    unsigned _width;
}; // List

template <>
class JsonT<List> : public __Agent<List> {
public:
    typedef value_t::iterator iterator;
    typedef value_t::const_iterator const_iterator;
    typedef value_t::reverse_iterator reverse_iterator;
    typedef value_t::const_reverse_iterator const_reverse_iterator;
public:
    JsonT(std::shared_ptr<List> v = std::make_shared<List>())
        : __Agent<List>(std::move(v)) {}
    Json &operator [] (int i) {
        return _value->get(i);
    }
};

#define CASE_JSON_TYPE(JT)                          \
template <> Json::Json(JT v)                        \
    : Value(std::make_shared<JT>(std::move(v))) { \
}                                                   \
template <> Json::Json(JsonT<typename __dt_2_jt<JT>::type> v) \
    : Value(v.agent()) {                                    \
}                                                             \
template <> Json &Json::operator = (std::remove_const<JT>::type v) { \
    _value = std::make_shared<JT>(std::move(v));                     \
    return *this;                                                    \
}                                                                                            \
template <>                                                                                  \
std::shared_ptr<typename __dt_2_jt<JT>::type> Json::as<JT>(std::shared_ptr<Interface> v) { \
    if (!v || v->type() != static_cast<Type>(JT::TYPE)) {                                    \
        return nullptr;                                                                      \
    }                                                                                        \
    return std::static_pointer_cast<JT>(v->shared_from_this());                              \
}
CASE_JSON_TYPE(Str)
CASE_JSON_TYPE(Dec)
CASE_JSON_TYPE(Uint)
CASE_JSON_TYPE(Sint)
CASE_JSON_TYPE(Bool)
CASE_JSON_TYPE(Dict)
CASE_JSON_TYPE(List)
#undef CASE_JSON_TYPE
template <>
std::shared_ptr<typename __dt_2_jt<Int>::type> Json::as<Int>(
        std::shared_ptr<Interface> v) {
    if (!v || v->type() != static_cast<Type>(Int::TYPE)) {
        return nullptr;
    }
    return std::static_pointer_cast<Int>(v->shared_from_this());
}

#define CASE_DATA_TYPE(JT, DT)                      \
template <> Json::Json(DT v)                        \
    : Value(std::make_shared<JT>(std::move(v))) { \
}                                                                  \
template <> struct __dt_2_jt<DT> { typedef JT type; };             \
template <>                                                        \
std::shared_ptr<JT> Json::as<DT>(std::shared_ptr<Interface> v) { \
    if (!v || v->type() != static_cast<Type>(JT::TYPE)) {          \
        return nullptr;                                            \
    }                                                              \
    return std::static_pointer_cast<JT>(v->shared_from_this());    \
}
CASE_DATA_TYPE(Str, std::string)
CASE_DATA_TYPE(Str, const char *)
CASE_DATA_TYPE(Dec, long double)
CASE_DATA_TYPE(Dec, double)
CASE_DATA_TYPE(Dec, float)
CASE_DATA_TYPE(Uint, unsigned long long)
CASE_DATA_TYPE(Uint, unsigned long)
CASE_DATA_TYPE(Uint, unsigned int)
CASE_DATA_TYPE(Uint, unsigned short)
CASE_DATA_TYPE(Sint, long long)
CASE_DATA_TYPE(Sint, long)
CASE_DATA_TYPE(Sint, int)
CASE_DATA_TYPE(Sint, short)
CASE_DATA_TYPE(Bool, bool)
CASE_DATA_TYPE(Dict, Dict::value_t)
CASE_DATA_TYPE(List, List::value_t)
#undef CASE_JSON_DTYPE

const char *Json::loads(const char *psz) {
#define CASE_JSON_TYPE(JT) {             \
        _value = std::make_shared<JT>(); \
        auto e = _value->loads(psz);     \
        if (e != psz) {                  \
            return e;                    \
        }                                \
    }
    CASE_JSON_TYPE(Dict)
    CASE_JSON_TYPE(List)
    CASE_JSON_TYPE(Bool)
    CASE_JSON_TYPE(Str)
    {
        _value = std::make_shared<Int>();
        auto e = _value->loads(psz);
        if (e != psz) {
            if (*e == '.') {
                CASE_JSON_TYPE(Dec);
            }
            return e;
        }
    }
#undef CASE_JSON_TYPE
    _value = nullptr;
    {
        auto p = skip_blank(psz);
        if (strncmp(p, "null", 4) == 0) {
            return p + 4;
        }
    }
    return psz;
}

template <typename Tto, typename Tfrom>
struct __cast {
    static std::shared_ptr<Tto> cast(const Tfrom &v) {
        static_assert(std::is_base_of<Interface, Tto>::value \
                && std::is_base_of<Interface,  Tfrom>::value,
            "jsoncfg::__cast is only applicable to jonsho types");
        return nullptr;
    }
};
template <typename Tto>
struct __cast<Tto, Str> {
    static std::shared_ptr<Tto> cast(const Str &v) {
        static_assert(std::is_base_of<Interface, Tto>::value,
            "jsoncfg::__cast is only applicable to jonsho types");
        auto data = std::make_shared<Tto>();
        auto p = v.value().c_str();
        if (data->loads(p) != p) {
            return data;
        }
        return nullptr;
    }
};
template <typename Tfrom>
struct __cast<Str, Tfrom> {
    static std::shared_ptr<Str> cast(const Tfrom &v) {
        static_assert(std::is_base_of<Interface, Tfrom>::value,
            "jsoncfg::__cast is only applicable to jonsho types");
        return std::make_shared<Str>(v.dumps());
    }
};
template <typename Tfrom>
struct __cast<Bool, Tfrom> {
    static std::shared_ptr<Bool> cast(const Tfrom &v) {
        static_assert(std::is_base_of<Interface, Tfrom>::value,
            "jsoncfg::__cast is only applicable to jonsho types");
        auto data = std::make_shared<Bool>(true);
        auto s = v.dumps();
        if (s.empty() || s == "false" || s == "{}" || s == "[]") {
            data->value() = false;
        }
        return data;
    }
};
template <>
struct __cast<Bool, Str> {
    static std::shared_ptr<Bool> cast(const Str &v) {
        bool bv = true;
        auto vs = v.value();
        if (vs.empty() || vs == "null" || vs == "false" || vs == "{}" || vs == "[]") {
            bv = false;
        } else {
            auto f = Dec();
            f.loads(vs);
            bv = f.value();
        }
        return std::make_shared<Bool>(bv);
    }
};
template <> struct __cast<Dec, Int> {
    static std::shared_ptr<Dec> cast(const Int &v) {
        if (v.is_signed()) {
            return std::make_shared<Dec>(static_cast<const Sint &>(v).value());
        }
        return std::make_shared<Dec>(static_cast<const Uint &>(v).value());
    }
};
template <> struct __cast<Int, Dec> {
    static std::shared_ptr<Int> cast(const Dec &v) {
        if (v.value() < 0) {
            return std::make_shared<Sint>(v.value());
        }
        return std::make_shared<Uint>(v.value());
    }
};
template <> struct __cast<Uint, Dec> {
    static std::shared_ptr<Uint> cast(const Dec &v) {
        return std::static_pointer_cast<Uint>(__cast<Int, Dec>::cast(v));
    }
};
template <> struct __cast<Sint, Dec> {
    static std::shared_ptr<Sint> cast(const Dec &v) {
        return std::static_pointer_cast<Sint>(__cast<Int, Dec>::cast(v));
    }
};
template <> struct __cast<Bool, Int> {
    static std::shared_ptr<Bool> cast(const Int &v) {
        return std::make_shared<Bool>(static_cast<const Uint &>(v).value());
    }
};
template <> struct __cast<Int, Bool> {
    static std::shared_ptr<Int> cast(const Bool &v) {
        return std::make_shared<Uint>(v.value());
    }
};

#define CASE_JSON_CAST_NUMBERIC(Tto, Tfrom)            \
template <> struct __cast<Tto, Tfrom> {                \
    static std::shared_ptr<Tto> cast(const Tfrom &v) { \
        return std::make_shared<Tto>(v.value());       \
    }                                                  \
};
CASE_JSON_CAST_NUMBERIC(Str, Str)
CASE_JSON_CAST_NUMBERIC(Dec, Uint)
CASE_JSON_CAST_NUMBERIC(Dec, Sint)
CASE_JSON_CAST_NUMBERIC(Dec, Bool)
CASE_JSON_CAST_NUMBERIC(Uint, Bool)
CASE_JSON_CAST_NUMBERIC(Sint, Bool)
CASE_JSON_CAST_NUMBERIC(Bool, Dec)
CASE_JSON_CAST_NUMBERIC(Bool, Uint)
CASE_JSON_CAST_NUMBERIC(Bool, Sint)
#undef CASE_JSON_CAST_NUMBERIC

template <typename DT> 
std::shared_ptr<typename __dt_2_jt<DT>::type> Json::to(std::shared_ptr<Interface> v) {
    typedef typename __dt_2_jt<DT>::type JT;
    static_assert(std::is_base_of<Interface, JT>::value, "unsupported type");
    if (!v || v->type() == static_cast<Type>(JT::TYPE)) {
        return as<JT>(v);
    }
    switch (v->type()) {
    case Type::STR: v = __cast<JT, Str>::cast(*as<Str>(v)); break;
    case Type::DEC: v = __cast<JT, Dec>::cast(*as<Dec>(v)); break;
    case Type::INT: v = __cast<JT, Int>::cast(*as<Int>(v)); break;
    case Type::BOOL: v = __cast<JT, Bool>::cast(*as<Bool>(v)); break;
    case Type::DICT: v = __cast<JT, Dict>::cast(*as<Dict>(v)); break;
    case Type::LIST: v = __cast<JT, List>::cast(*as<List>(v)); break;
    default: break;
    }
    return as<JT>(v);
}

} // jsoncfg

#endif // __JSONCFG_HPP__
