#include "test_helper.hpp"

TEST_CASE("Uint") {
    auto v = jsoncfg::Uint(2);
    SECTION("Uint::dumps()") {
        CHECK(v.value() == 2);
        CHECK(v == 2);
        CHECK(v.dumps() == "2");
    }

    v = 5;
    SECTION("Uint = int") {
        CHECK(v.value() == 5);
    }

    v.loads("7");
    SECTION("Uint::loads(int)") {
        CHECK(v.value() == 7);
    }

    auto j = jsoncfg::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(int)") {
        CHECK(j.as<jsoncfg::Uint>());
        CHECK(j.as<jsoncfg::Uint>()->value() == 7);
        CHECK(j.as<jsoncfg::Sint>());
        CHECK(j.as<jsoncfg::Sint>()->value() == 7);
    }

    auto jt = j.as<int>();
    jt = 6;
    SECTION("JsonT::=") {
        CHECK(jt->value() == 6);
    }

    j = jsoncfg::Str("123");
    SECTION("Json<Uint> = Str") {
        CHECK(!j.as<jsoncfg::Uint>());
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123");
    }

    SECTION("Json<Str>::to<Uint>() success") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123");
        auto pi = j.to<jsoncfg::Uint>();
        CHECK(pi);
        CHECK(pi->value() == 123);
        CHECK(j.as<jsoncfg::Uint>());
        CHECK(j.as<jsoncfg::Uint>() == pi);
    }

    j = jsoncfg::Uint(123);
    SECTION("Json<Uint>::to<Str>()") {
        CHECK(j.as<jsoncfg::Uint>());
        CHECK(j.as<jsoncfg::Uint>()->value() == 123);
        auto pi = j.to<jsoncfg::Str>();
        CHECK(pi);
        CHECK(pi->value() == std::to_string(123));
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>() == pi);
    }

    SECTION("Json<Uint>::to<Dec>()") {
        CHECK(j.as<jsoncfg::Uint>());
        CHECK(j.as<jsoncfg::Uint>()->value() == 123);
        auto pi = j.to<jsoncfg::Dec>();
        CHECK(pi);
        CHECK(pi->value() == Approx(123).margin(1e-6));
        CHECK(j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Dec>() == pi);
    }

    SECTION("Json<Uint>::to<Bool>() true") {
        CHECK(j.as<jsoncfg::Uint>());
        CHECK(j.as<jsoncfg::Uint>()->value() == 123);
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi->value() == true);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == pi);
    }

    j = jsoncfg::Uint(0);
    SECTION("Json<Uint>::to<Bool>() false") {
        CHECK(j.as<jsoncfg::Uint>());
        CHECK(j.as<jsoncfg::Uint>()->value() == 0);
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi->value() == false);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == pi);
    }

    j = jsoncfg::Str("i7");
    SECTION("Json<Str>::to<Uint>() failed") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "i7");
        CHECK(!j.to<jsoncfg::Uint>());
        CHECK(!j.as<jsoncfg::Str>());
        CHECK(!j.value());
    }
}
