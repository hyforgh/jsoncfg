#include "test_helper.hpp"

TEST_CASE("Str") {
    auto v = jsoncfg::Str("hello");
    SECTION("Int::dumps()") {
        CHECK(v.value() == "hello");
        CHECK(v.dumps() == "\"hello\"");
    }

    v = "c-string";
    SECTION("Str = c-string") {
        CHECK(v.value() == "c-string");
    }

    v = std::string("std::string");
    SECTION("Str = std::string") {
        CHECK(v.value() == "std::string");
    }

    v.loads("\"7\"");
    SECTION("Str::loads(str)") {
        CHECK(v.value() == "7");
    }

    auto j = jsoncfg::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(Str)") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "7");
    }

    j = jsoncfg::Int(123);
    SECTION("Json<Str> = Int") {
        CHECK(!j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>()->value() == 123);
    }

    j.to<jsoncfg::Str>();
    SECTION("Json<Str>::to<Float>() success") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123");
        auto pi = j.to<jsoncfg::Float>();
        CHECK(pi);
        CHECK(pi->value() == 123);
        CHECK(j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Float>() == pi);
    }

    SECTION("Json<Str>::to<Int>() success") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123");
        auto pi = j.to<jsoncfg::Int>();
        CHECK(pi);
        CHECK(pi->value() == 123);
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>() == pi);
    }

    SECTION("Json<Str>::to<Bool>() success") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123");
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi->value() == true);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == pi);
    }

    j = jsoncfg::Str("i7");
    SECTION("Json<Str>::to<Int>() failed") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "i7");
        CHECK(!j.to<jsoncfg::Int>());
        CHECK(!j.as<jsoncfg::Str>());
        CHECK(!j.value());
    }
}
