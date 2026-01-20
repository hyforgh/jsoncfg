#include "test_helper.hpp"

TEST_CASE("Int") {
    auto v = jsonho::Int(2);
    SECTION("Int::dumps()") {
        CHECK(v.value() == 2);
        CHECK(v.dumps() == "2");
    }

    v = 5;
    SECTION("Int = int") {
        CHECK(v.value() == 5);
    }

    v.loads("7");
    SECTION("Int::loads(int)") {
        CHECK(v.value() == 7);
    }

    auto j = jsonho::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(int)") {
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>()->value() == 7);
    }

    j = jsonho::Str("123");
    SECTION("Json<Int> = Str") {
        CHECK(!j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "123");
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

    j = jsonho::Int(123);
    SECTION("Json<Int>::to<Str>()") {
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>()->value() == 123);
        auto pi = j.to<jsonho::Str>();
        CHECK(pi);
        CHECK(pi->value() == std::to_string(123));
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>() == pi);
    }

    SECTION("Json<Int>::to<Float>()") {
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>()->value() == 123);
        auto pi = j.to<jsonho::Float>();
        CHECK(pi);
        CHECK(pi->value() == Approx(123).margin(1e-6));
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>() == pi);
    }

    SECTION("Json<Int>::to<Bool>() true") {
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>()->value() == 123);
        auto pi = j.to<jsonho::Bool>();
        CHECK(pi);
        CHECK(pi->value() == true);
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>() == pi);
    }

    j = jsonho::Int(0);
    SECTION("Json<Int>::to<Bool>() false") {
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>()->value() == 0);
        auto pi = j.to<jsonho::Bool>();
        CHECK(pi);
        CHECK(pi->value() == false);
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
