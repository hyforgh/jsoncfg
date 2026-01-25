/**
 * github: https://github.com/hyforgh/jsonho
 */

/*
MIT License

Copyright (c) 2025-2034 Liu Hongyun

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

#ifndef __JSONHO_HPP__
#define __JSONHO_HPP__

#include <string.h>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <memory>

namespace jsonho {

#define JSONHO_VERSION_MAJOR 1
#define JSONHO_VERSION_MINOR 0
#define JSONHO_VERSION_PATCH 0

enum class Type {
    JSON = 0,
    STR,
    FLOAT,
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
class Primary : public Interface {
public:
    typedef DT value_t;
public:
    virtual ~Primary() {}
    Primary(value_t v) : _value(std::move(v)) {}
    Primary &operator = (value_t v) {
        _value = std::move(v);
        return *this;
    }
    Type type() const override { return JT; }
    static Type TYPE() { return JT; }
    value_t &value() { return _value; }
    const value_t &value() const { return _value; }
protected:
    DT _value;
};

template <typename DT>
struct __dt_2_jt { typedef DT type; };
class Json final : public Primary<Type::JSON, std::shared_ptr<Interface>> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Json() : Primary(nullptr) {}
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
    std::shared_ptr<typename __dt_2_jt<T>::type> as() {
        static_assert(std::is_base_of<Interface, typename __dt_2_jt<T>::type>::value,
            "Json::as<T>() : not supported type");
        return nullptr;
    }
    template <typename T>
    std::shared_ptr<typename __dt_2_jt<T>::type> to();
}; // Json

class Str final : public Primary<Type::STR, std::string> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Str() : Primary("") {}
    Str(value_t v) : Primary(std::move(v)) {}
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

class Float final : public Primary<Type::FLOAT, long double> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Float() : Primary(0) {}
    Float(value_t v) : Primary(v) {}
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
}; // Float

class Int : public Primary<Type::INT, long long> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Int() : Primary(0) {}
    Int(value_t v) : Primary(v) {}
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

class Bool final : public Primary<Type::BOOL, bool> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Bool() : Primary(false) {}
    Bool(value_t v) : Primary(v) {}
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

class Dict final : public Primary<Type::DICT, std::map<std::string, Json>> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    Dict() : Primary({}) {}
    Dict(value_t v) : Primary(std::move(v)) {}
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
    Json &get(const std::string &k, Json default_value = Json()) {
#define DICT_GET()                \
        auto it = _value.find(k); \
        if (it == _value.end()) { \
            return default_value; \
        }                         \
        return it->second;
        DICT_GET()
    }
    template <typename T>
    const Json &get(const std::string &k, Json default_value = Json()) const {
        DICT_GET()
    }
#undef DICT_GET
}; // Dict

class List final : public Primary<Type::LIST, std::list<Json>> {
public:
    using Interface::dumps;
    using Interface::loads;
public:
    List() : Primary({}) {}
    List(value_t v) : Primary(std::move(v)) {}
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
}; // List

#define CASE_JSON_TYPE(JT)                          \
template <> Json::Json(JT v)                        \
    : Primary(std::make_shared<JT>(std::move(v))) { \
}                                                   \
template <>                                              \
Json &Json::operator = (std::remove_const<JT>::type v) { \
    _value = std::make_shared<JT>(std::move(v));         \
    return *this;                                        \
}                                                                    \
template <> std::shared_ptr<typename __dt_2_jt<JT>::type> Json::as<JT>() {                     \
    if (!_value || _value->type() != JT::TYPE()) {                   \
        return nullptr;                                              \
    }                                                                \
    return std::static_pointer_cast<JT>(_value->shared_from_this()); \
}
CASE_JSON_TYPE(Str)
CASE_JSON_TYPE(Float)
CASE_JSON_TYPE(Int)
CASE_JSON_TYPE(Bool)
CASE_JSON_TYPE(Dict)
CASE_JSON_TYPE(List)
#undef CASE_JSON_TYPE

#define CASE_DATA_TYPE(JT, DT)                      \
template <> Json::Json(DT v)                        \
    : Primary(std::make_shared<JT>(std::move(v))) { \
}                                                                    \
template <> struct __dt_2_jt<DT> { typedef JT type; };               \
template <> std::shared_ptr<JT> Json::as<DT>() {                     \
    if (!_value || _value->type() != JT::TYPE()) {                   \
        return nullptr;                                              \
    }                                                                \
    return std::static_pointer_cast<JT>(_value->shared_from_this()); \
}
CASE_DATA_TYPE(Str, std::string)
CASE_DATA_TYPE(Str, const char *)
CASE_DATA_TYPE(Float, long double)
CASE_DATA_TYPE(Float, double)
CASE_DATA_TYPE(Float, float)
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
                CASE_JSON_TYPE(Float);
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
struct __json_to {
    static std::shared_ptr<Tto> cast(const Tfrom &v) {
        static_assert(std::is_base_of<Interface, Tto>::value \
                && std::is_base_of<Interface,  Tfrom>::value,
            "__json_to is only applicable to jonsho types");
        return nullptr;
    }
};
template <typename Tto>
struct __json_to<Tto, Str> {
    static std::shared_ptr<Tto> cast(const Str &v) {
        static_assert(std::is_base_of<Interface, Tto>::value,
            "__json_to is only applicable to jonsho types");
        auto data = std::make_shared<Tto>();
        auto p = v.value().c_str();
        if (data->loads(p) != p) {
            return data;
        }
        return nullptr;
    }
};
template <typename Tfrom>
struct __json_to<Str, Tfrom> {
    static std::shared_ptr<Str> cast(const Tfrom &v) {
        static_assert(std::is_base_of<Interface, Tfrom>::value,
            "__json_to is only applicable to jonsho types");
        return std::make_shared<Str>(v.dumps());
    }
};
template <typename Tfrom>
struct __json_to<Bool, Tfrom> {
    static std::shared_ptr<Bool> cast(const Tfrom &v) {
        static_assert(std::is_base_of<Interface, Tfrom>::value,
            "__json_to is only applicable to jonsho types");
        auto data = std::make_shared<Bool>(true);
        auto s = v.dumps();
        if (s.empty() || s == "false" || s == "{}" || s == "[]") {
            data->value() = false;
        }
        return data;
    }
};
#define CASE_JSON_CAST_NUMBERIC(Tto, Tfrom)            \
template <> struct __json_to<Tto, Tfrom> {             \
    static std::shared_ptr<Tto> cast(const Tfrom &v) { \
        return std::make_shared<Tto>(v.value());       \
    }                                                  \
};
CASE_JSON_CAST_NUMBERIC(Str, Str)
CASE_JSON_CAST_NUMBERIC(Float, Int)
CASE_JSON_CAST_NUMBERIC(Float, Bool)
CASE_JSON_CAST_NUMBERIC(Int, Float)
CASE_JSON_CAST_NUMBERIC(Int, Bool)
CASE_JSON_CAST_NUMBERIC(Bool, Float)
CASE_JSON_CAST_NUMBERIC(Bool, Int)
#undef CASE_JSON_CAST_NUMBERIC
template <>
struct __json_to<Bool, Str> {
    static std::shared_ptr<Bool> cast(const Str &v) {
        bool bv = true;
        auto vs = v.value();
        if (vs.empty() || vs == "null" || vs == "false" || vs == "{}" || vs == "[]") {
            bv = false;
        } else {
            auto f = Float();
            f.loads(vs);
            bv = f.value();
        }
        return std::make_shared<Bool>(bv);
    }
};
template <typename DT> 
std::shared_ptr<typename __dt_2_jt<DT>::type> Json::to() {
    typedef typename __dt_2_jt<DT>::type JT;
    static_assert(std::is_base_of<Interface, JT>::value,
        "unsupported type");
    auto type_to = JT::TYPE();
    if (!_value || _value->type() == type_to) {
        return as<JT>();
    }
    switch (_value->type()) {
    case Type::STR:
        _value = __json_to<JT, Str>::cast(*as<Str>());
        break;
    case Type::FLOAT:
        _value = __json_to<JT, Float>::cast(*as<Float>());
        break;
    case Type::INT:
        _value = __json_to<JT, Int>::cast(*as<Int>());
        break;
    case Type::BOOL:
        _value = __json_to<JT, Bool>::cast(*as<Bool>());
        break;
    case Type::DICT:
        _value = __json_to<JT, Dict>::cast(*as<Dict>());
        break;
    case Type::LIST:
        _value = __json_to<JT, List>::cast(*as<List>());
        break;
    default:
        break;
    }
    return as<JT>();
}

} // jsonho

#endif // __JSONHO_HPP__
