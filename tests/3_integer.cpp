#include "test_helper.hpp"

TEST_CASE("Int") {
    auto v = jsoncfg::Int(2);
    SECTION("Int::dumps()") {
        CHECK(v.value() == 2);
        CHECK(v == 2);
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

    auto j = jsoncfg::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(int)") {
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>()->value() == 7);
    }

    auto jt = j.as<int>();
    jt = 6;
    SECTION("JsonT::=") {
        CHECK(jt->value() == 6);
    }

    j = jsoncfg::Str("123");
    SECTION("Json<Int> = Str") {
        CHECK(!j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123");
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

    j = jsoncfg::Int(123);
    SECTION("Json<Int>::to<Str>()") {
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>()->value() == 123);
        auto pi = j.to<jsoncfg::Str>();
        CHECK(pi);
        CHECK(pi->value() == std::to_string(123));
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>() == pi);
    }

    SECTION("Json<Int>::to<Dec>()") {
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>()->value() == 123);
        auto pi = j.to<jsoncfg::Dec>();
        CHECK(pi);
        CHECK(pi->value() == Approx(123).margin(1e-6));
        CHECK(j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Dec>() == pi);
    }

    SECTION("Json<Int>::to<Bool>() true") {
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>()->value() == 123);
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi->value() == true);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == pi);
    }

    j = jsoncfg::Int(0);
    SECTION("Json<Int>::to<Bool>() false") {
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>()->value() == 0);
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi->value() == false);
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
