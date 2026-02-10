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
    AGENT
}; // Type

class Interface : public std::enable_shared_from_this<Interface> {
public:
    virtual ~Interface() {}
    virtual Type type() const = 0;
    virtual void dumps(std::ostream &os) const = 0;
    virtual const char *loads(const char *psz) = 0;
    virtual std::string dumps() const {
        std::stringstream ss;
        dumps(ss);
        return ss.str();
    }
    virtual size_t loads(const std::string &s) {
        auto p = s.data();
        auto e = loads(p);
        return e - p;
    }
public:
    static const char *skip_blank(const char *p) {
        if (!p) {
            return p;
        }
        while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t') {
            ++p;
        }
        return p;
    }
}; // Interface

template <Type JT, typename DT>
class Value : public Interface {
public:
    enum { TYPE = static_cast<int>(JT) };
    typedef DT value_t;
public:
    virtual ~Value() {}
    Value(value_t v) : _value(std::move(v)) {}
    Value &operator = (value_t v) {
        _value = std::move(v);
        return *this;
    }
    Type type() const override { return JT; }
    value_t &value() { return _value; }
    const value_t &value() const { return _value; }
protected:
    DT _value;
};

template <typename JT>
class Agent {
public:
    typedef typename JT::value_t value_t;
public:
    Agent(std::shared_ptr<JT> v = std::make_shared<JT>()) : _value(std::move(v)) {}
    virtual ~Agent() {}
    operator bool () const {
        return bool(_value);
    }
    JT &operator = (typename JT::value_t v) {
        auto &obj = *_value;
        obj = v;
        return obj;
    }
    value_t &value() {
        return _value->value();
    }
    const value_t &value() const {
        return _value->value();
    }
    std::shared_ptr<JT> agent() {
        return _value;
    }
protected:
    std::shared_ptr<JT> _value;
};

template <typename JT>
class JsonT : public Agent<JT> {
public:
    using Agent<JT>::operator =;
public:
    JsonT(std::shared_ptr<JT> v = std::make_shared<JT>())
        : Agent<JT>(std::move(v)) {}
};

template <typename DT>
struct __dt_2_jt { typedef DT type; };
class Json final : public Value<Type::JSON, std::shared_ptr<Interface>> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Json() : Value(nullptr) {}
    template <typename T>
    Json(T v);
    template <typename T>
    Json &operator = (T v);
    void dumps(std::ostream &os) const override {
        if (_value) {
            _value->dumps(os);
        } else {
            os << "null";
        }
    }
    const char *loads(const char *psz) override;
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
}; // Json

class Str final : public Value<Type::STR, std::string> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Str() : Value("") {}
    Str(value_t v) : Value(std::move(v)) {}
    Str &operator = (const char *v) {
        _value = v;
        return *this;
    }
    void dumps(std::ostream &os) const override {
        os << "\"" << _value << "\"";
    }
    const char *loads(const char *psz) {
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz);
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
}; // Str

class Dec final : public Value<Type::DEC, long double> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Dec() : Value(0) {}
    Dec(value_t v) : Value(v) {}
    void dumps(std::ostream &os) const override {
        os << _value;
    }
    const char *loads(const char *psz) {
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz);
        char *e;
        _value = std::strtold(p, &e);
        if (e != p) {
            return e;
        }
        return psz;
    }
}; // Dec

class Int : public Value<Type::INT, long long> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Int() : Value(0) {}
    Int(value_t v) : Value(v) {}
    void dumps(std::ostream &os) const override {
        os << _value;
    }
    const char *loads(const char *psz) {
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz);
        int base = 10;
        if (*p == '0') {
            ++p;
            if (*p == 'x' || *p == 'X') {
                base = 16;
            } else if (*p == 'o' || *p == 'O') {
                base = 8;
            } else if (*p == 'b' || *p == 'B') {
                base = 2;
            }
            if (base != 10) {
                ++p;
            }
        }
        char *e;
        _value = std::strtoll(p, &e, base);
        if (e != p) {
            return e;
        }
        return psz;
    }
}; // Int

class Bool final : public Value<Type::BOOL, bool> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Bool() : Value(false) {}
    Bool(value_t v) : Value(v) {}
    void dumps(std::ostream &os) const override {
        os << (_value ? "true" : "false");
    }
    const char *loads(const char *psz) {
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz);
        if (strncmp(p, "true", 4) == 0) {
            _value = true;
            return p + 4;
        } else if (strncmp(p, "false", 5) == 0) {
            _value = false;
            return p + 5;
        }
        return psz;
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
    iterator begin() {
        return _list.begin();
    }
    iterator end() {
        return _list.end();
    }
    iterator find(const TK &key) {
        auto it = _dict.find(key);
        if (it == _dict.end()) {
            return _list.end();
        }
        return it->second;
    }
    const_iterator begin() const {
        return _list.begin();
    }
    const_iterator end() const {
        return _list.end();
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
private:
    list_t _list;
    dict_t _dict;
};
class Dict final : public Value<Type::DICT, OrderedDict<std::string, Json>> {
public:
    using Interface::dumps;
    using Interface::loads;
    typedef value_t::iterator iterator;
    typedef value_t::const_iterator const_iterator;
public:
    Dict() : Value({}) {}
    Dict(value_t v) : Value(std::move(v)) {}
    void dumps(std::ostream &os) const override {
        os << "{";
        bool is_first = true;
        for (auto &it : _value) {
            if (is_first) {
                is_first = false;
            } else {
                os << ",";
            }
            os << "\"" << it.first << "\":";
            it.second.dumps(os);
        }
        os << "}";
    }
    const char *loads(const char *psz) {
        _value.clear();
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz);
        if (*p != '{') {
            return psz;
        }
        p = skip_blank(p + 1);
        while (*p) {
            if (*p == '}') {
                return p + 1;
            }
            Str k;
            auto e = k.loads(p);
            if (e == p) {
                return psz;
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
            p = skip_blank(e);
            if (*p == ',' || *p == '}') {
                _value[k.value()] = std::move(v);
                if (*p == '}') {
                    return p + 1;
                }
            } else {
                return psz;
            }
            ++p;
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
    iterator begin() {
        return _value.begin();
    }
    iterator end() {
        return _value.end();
    }
    iterator find(const std::string &key) {
        return _value.find(key);
    }
    const_iterator begin() const {
        return _value.begin();
    }
    const_iterator end() const {
        return _value.end();
    }
    const_iterator find(const std::string &key) const {
        return _value.find(key);
    }
}; // Dict

template <>
class JsonT<Dict> : public Agent<Dict> {
public:
    typedef value_t::iterator iterator;
    typedef value_t::const_iterator const_iterator;
public:
    JsonT(std::shared_ptr<Dict> v = std::make_shared<Dict>())
        : Agent<Dict>(std::move(v)) {}
    Json &operator [] (const std::string &k) {
        return (*_value)[k];
    }
    Json &operator [] (const char *k) {
        return (*_value)[k];
    }
    JsonT &set(const std::string &k, Json v) {
        _value->set(k, std::move(v));
        return *this;
    }
    Json get(const std::string &k, Json default_value = Json()) const {
        return _value->get(k, std::move(default_value));
    }
    iterator begin() {
        return _value->begin();
    }
    iterator end() {
        return _value->end();
    }
    iterator find(const std::string &key) {
        return _value->find(key);
    }
    const_iterator begin() const {
        return _value->begin();
    }
    const_iterator end() const {
        return _value->end();
    }
    const_iterator find(const std::string &key) const {
        return _value->find(key);
    }
};

class List final : public Value<Type::LIST, std::list<Json>> {
public:
    using Interface::dumps;
    using Interface::loads;
    typedef value_t::iterator iterator;
    typedef value_t::const_iterator const_iterator;
    typedef value_t::reverse_iterator reverse_iterator;
    typedef value_t::const_reverse_iterator const_reverse_iterator;
public:
    List() : Value({}) {}
    List(value_t v) : Value(std::move(v)) {}
    void dumps(std::ostream &os) const override {
        os << "[";
        bool is_first = true;
        for (auto &it : _value) {
            if (is_first) {
                is_first = false;
            } else {
                os << ",";
            }
            it.dumps(os);
        }
        os << "]";
    }
    const char *loads(const char *psz) {
        _value.clear();
        if (!psz) {
            return psz;
        }
        auto p = skip_blank(psz);
        if (*p != '[') {
            return psz;
        }
        p = skip_blank(p + 1);
        while (*p) {
            if (*p == ']') {
                return p + 1;
            }
            Json v;
            auto e = v.loads(p);
            if (e == p) {
                return psz;
            }
            p = skip_blank(e);
            if (*p == ',' || *p == ']') {
                append(std::move(v));
                if (*p == ']') {
                    return p + 1;
                }
            } else {
                return psz;
            }
            ++p;
        }
        return psz;
    }
    List &append(Json v) {
        _value.emplace_back(std::move(v));
        return *this;
    }
    List &insert(unsigned i, Json v) {
        _value.emplace(iterator_at(i), std::move(v));
        return *this;
    }
    Json &get(unsigned i) {
        return *iterator_at(i);
    }
    const Json &get(unsigned i) const {
        return *iterator_at(i);
    }
    List &set(unsigned i, Json v) {
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
    size_t size() const {
        return _value.size();
    }
    iterator begin() {
        return _value.begin();
    }
    iterator end() {
        return _value.end();
    }
    const_iterator begin() const {
        return _value.begin();
    }
    const_iterator end() const {
        return _value.end();
    }
    reverse_iterator rbegin() {
        return _value.rbegin();
    }
    reverse_iterator rend() {
        return _value.rend();
    }
    const_reverse_iterator rbegin() const {
        return _value.rbegin();
    }
    const_reverse_iterator rend() const {
        return _value.rend();
    }
private:
    iterator iterator_at(unsigned i) {
#define ITERATOR_AT()               \
        auto n = _value.size();     \
        if (i >= n) {               \
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
    const_iterator iterator_at(unsigned i) const {
        ITERATOR_AT();
#undef ITERATOR_AT
    }
}; // List

template <>
class JsonT<List> : public Agent<List> {
public:
    typedef value_t::iterator iterator;
    typedef value_t::const_iterator const_iterator;
    typedef value_t::reverse_iterator reverse_iterator;
    typedef value_t::const_reverse_iterator const_reverse_iterator;
public:
    JsonT(std::shared_ptr<List> v = std::make_shared<List>())
        : Agent<List>(std::move(v)) {}
    Json &operator [] (unsigned i) {
        return _value->get(i);
    }
    JsonT &append(Json v) {
        _value->append(std::move(v));
        return *this;
    }
    JsonT &insert(unsigned i, Json v) {
        _value->insert(i, std::move(v));
        return *this;
    }
    Json &get(unsigned i) {
        return _value->get(i);
    }
    const Json &get(unsigned i) const {
        return _value->get(i);
    }
    JsonT &set(unsigned i, Json v) {
        _value->set(i, std::move(v));
        return *this;
    }
    std::vector<iterator> iterators() {
        return _value->iterators();
    }
    template<typename T>
    std::vector<T> vector(const T &default_value) const {
        return _value->vector<T>(default_value);
    }
    size_t size() const {
        return _value->size();
    }
    iterator begin() {
        return _value->begin();
    }
    iterator end() {
        return _value->end();
    }
    const_iterator begin() const {
        return _value->begin();
    }
    const_iterator end() const {
        return _value->end();
    }
    reverse_iterator rbegin() {
        return _value->rbegin();
    }
    reverse_iterator rend() {
        return _value->rend();
    }
    const_reverse_iterator rbegin() const {
        return _value->rbegin();
    }
    const_reverse_iterator rend() const {
        return _value->rend();
    }
};

#define CASE_JSON_TYPE(JT)                        \
template <> Json::Json(JT v)                      \
    : Value(std::make_shared<JT>(std::move(v))) { \
}                                                 \
template <> Json::Json(JsonT<typename __dt_2_jt<JT>::type> v) \
    : Value(v.agent()) {                                      \
}                                                             \
template <> Json &Json::operator = (std::remove_const<JT>::type v) { \
    _value = std::make_shared<JT>(std::move(v));                     \
    return *this;                                                    \
}                                                                                          \
template <>                                                                                \
std::shared_ptr<typename __dt_2_jt<JT>::type> Json::as<JT>(std::shared_ptr<Interface> v) { \
    if (!v || v->type() != static_cast<Type>(JT::TYPE)) {                                  \
        return nullptr;                                                                    \
    }                                                                                      \
    return std::static_pointer_cast<JT>(v->shared_from_this());                            \
}
CASE_JSON_TYPE(Str)
CASE_JSON_TYPE(Dec)
CASE_JSON_TYPE(Int)
CASE_JSON_TYPE(Bool)
CASE_JSON_TYPE(Dict)
CASE_JSON_TYPE(List)
#undef CASE_JSON_TYPE

#define CASE_DATA_TYPE(JT, DT)                    \
template <> Json::Json(DT v)                      \
    : Value(std::make_shared<JT>(std::move(v))) { \
}                                                                \
template <> struct __dt_2_jt<DT> { typedef JT type; };           \
template <>                                                      \
std::shared_ptr<JT> Json::as<DT>(std::shared_ptr<Interface> v) { \
    if (!v || v->type() != static_cast<Type>(JT::TYPE)) {        \
        return nullptr;                                          \
    }                                                            \
    return std::static_pointer_cast<JT>(v->shared_from_this());  \
}
CASE_DATA_TYPE(Str, std::string)
CASE_DATA_TYPE(Str, const char *)
CASE_DATA_TYPE(Dec, long double)
CASE_DATA_TYPE(Dec, double)
CASE_DATA_TYPE(Dec, float)
CASE_DATA_TYPE(Int, unsigned long long)
CASE_DATA_TYPE(Int, unsigned long)
CASE_DATA_TYPE(Int, unsigned int)
CASE_DATA_TYPE(Int, unsigned short)
CASE_DATA_TYPE(Int, long long)
CASE_DATA_TYPE(Int, long)
CASE_DATA_TYPE(Int, int)
CASE_DATA_TYPE(Int, short)
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

#define CASE_JSON_CAST_NUMBERIC(Tto, Tfrom)            \
template <> struct __cast<Tto, Tfrom> {                \
    static std::shared_ptr<Tto> cast(const Tfrom &v) { \
        return std::make_shared<Tto>(v.value());       \
    }                                                  \
};
CASE_JSON_CAST_NUMBERIC(Str, Str)
CASE_JSON_CAST_NUMBERIC(Dec, Int)
CASE_JSON_CAST_NUMBERIC(Dec, Bool)
CASE_JSON_CAST_NUMBERIC(Int, Dec)
CASE_JSON_CAST_NUMBERIC(Int, Bool)
CASE_JSON_CAST_NUMBERIC(Bool, Dec)
CASE_JSON_CAST_NUMBERIC(Bool, Int)
#undef CASE_JSON_CAST_NUMBERIC

template <typename DT> 
std::shared_ptr<typename __dt_2_jt<DT>::type> Json::to(std::shared_ptr<Interface> v) {
    typedef typename __dt_2_jt<DT>::type JT;
    static_assert(std::is_base_of<Interface, JT>::value, "unsupported type");
    if (!v || v->type() == static_cast<Type>(JT::TYPE)) {
        return as<JT>(v);
    }
    switch (v->type()) {
    case Type::STR:
        v = __cast<JT, Str>::cast(*as<Str>(v));
        break;
    case Type::DEC:
        v = __cast<JT, Dec>::cast(*as<Dec>(v));
        break;
    case Type::INT:
        v = __cast<JT, Int>::cast(*as<Int>(v));
        break;
    case Type::BOOL:
        v = __cast<JT, Bool>::cast(*as<Bool>(v));
        break;
    case Type::DICT:
        v = __cast<JT, Dict>::cast(*as<Dict>(v));
        break;
    case Type::LIST:
        v = __cast<JT, List>::cast(*as<List>(v));
        break;
    default:
        break;
    }
    return as<JT>(v);
}

} // jsoncfg

#endif // __JSONCFG_HPP__
