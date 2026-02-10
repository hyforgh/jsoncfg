#include "test_helper.hpp"

TEST_CASE("Dec") {
    auto v = jsoncfg::Dec(2.5);
    SECTION("Dec::dumps()") {
        CHECK(v.value() == Approx(2.5).margin(1e-6));
        CHECK(v.dumps() == "2.5");
    }

    v = 5.2;
    SECTION("Dec = float") {
        CHECK(v.value() == Approx(5.2).margin(1e-6));
    }

    v.loads("7.8");
    SECTION("Int::loads(float)") {
        CHECK(v.value() == Approx(7.8).margin(1e-6));
    }

    auto j = jsoncfg::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(int)") {
        CHECK(j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Dec>().value() == Approx(7.8).margin(1e-6));
    }

    auto jt = j.as<float>();
    jt = 5.5;
    SECTION("JsonT::=") {
        CHECK(jt.value() == Approx(5.5).margin(1e-6));
    }

    j = jsoncfg::Str("123.5");
    SECTION("Json<Dec> = Str") {
        CHECK(!j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>().value() == "123.5");
    }

    SECTION("Json<Str>::to<Dec>() success") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>().value() == "123.5");
        auto pi = j.to<jsoncfg::Dec>();
        CHECK(pi);
        CHECK(pi.value() == Approx(123.5).margin(1e-6));
        CHECK(j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Dec>() == pi);
    }

    j = jsoncfg::Dec(123.5);
    SECTION("Json<Dec>::to<Str>()") {
        CHECK(j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Dec>().value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsoncfg::Str>();
        CHECK(pi);
        std::stringstream ss;
        ss << 123.5;
        CHECK(pi.value() == ss.str());
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>() == pi);
    }

    SECTION("Json<Dec>::to<Int>()") {
        CHECK(j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Dec>().value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsoncfg::Int>();
        CHECK(pi);
        CHECK(pi.value() == static_cast<jsoncfg::Int::value_t>(123.5));
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>() == pi);
    }

    SECTION("Json<Dec>::to<Bool>() true") {
        CHECK(j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Dec>().value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi.value() == true);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == pi);
    }

    j = jsoncfg::Dec(0.0);
    SECTION("Json<Dec>::to<Bool>() false") {
        CHECK(j.as<jsoncfg::Dec>());
        CHECK(j.as<jsoncfg::Dec>().value() == Approx(0).margin(1e-6));
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi.value() == false);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == pi);
    }

    j = jsoncfg::Str("i7.3");
    SECTION("Json<Str>::to<Dec>() failed") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>().value() == "i7.3");
        CHECK(!j.to<jsoncfg::Dec>());
        CHECK(!j.as<jsoncfg::Str>());
        CHECK(!j.value());
    }
}
