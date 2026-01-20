#include "test_helper.hpp"

TEST_CASE("Str") {
    auto v = jsonho::Str("hello");
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

    auto j = jsonho::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(Str)") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "7");
    }

    j = jsonho::Int(123);
    SECTION("Json<Str> = Int") {
        CHECK(!j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>()->value() == 123);
    }

    j.to<jsonho::Str>();
    SECTION("Json<Str>::to<Float>() success") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "123");
        auto pi = j.to<jsonho::Float>();
        CHECK(pi);
        CHECK(pi->value() == 123);
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>() == pi);
    }

    SECTION("Json<Str>::to<Int>() success") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "123");
        auto pi = j.to<jsonho::Int>();
        CHECK(pi);
        CHECK(pi->value() == 123);
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>() == pi);
    }

    SECTION("Json<Str>::to<Bool>() success") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "123");
        auto pi = j.to<jsonho::Bool>();
        CHECK(pi);
        CHECK(pi->value() == true);
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>() == pi);
    }

    j = jsonho::Str("i7");
    SECTION("Json<Str>::to<Int>() failed") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "i7");
        CHECK(!j.to<jsonho::Int>());
        CHECK(!j.as<jsonho::Str>());
        CHECK(!j.value());
    }
}
