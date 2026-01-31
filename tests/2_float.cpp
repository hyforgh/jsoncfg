#include "test_helper.hpp"

TEST_CASE("Float") {
    auto v = jsoncfg::Float(2.5);
    SECTION("Float::dumps()") {
        CHECK(v.value() == Approx(2.5).margin(1e-6));
        CHECK(v.dumps() == "2.5");
    }

    v = 5.2;
    SECTION("Float = float") {
        CHECK(v.value() == Approx(5.2).margin(1e-6));
    }

    v.loads("7.8");
    SECTION("Int::loads(float)") {
        CHECK(v.value() == Approx(7.8).margin(1e-6));
    }

    auto j = jsoncfg::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(int)") {
        CHECK(j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Float>()->value() == Approx(7.8).margin(1e-6));
    }

    j = jsoncfg::Str("123.5");
    SECTION("Json<Float> = Str") {
        CHECK(!j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123.5");
    }

    SECTION("Json<Str>::to<Float>() success") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123.5");
        auto pi = j.to<jsoncfg::Float>();
        CHECK(pi);
        CHECK(pi->value() == Approx(123.5).margin(1e-6));
        CHECK(j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Float>() == pi);
    }

    j = jsoncfg::Float(123.5);
    SECTION("Json<Float>::to<Str>()") {
        CHECK(j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Float>()->value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsoncfg::Str>();
        CHECK(pi);
        std::stringstream ss;
        ss << 123.5;
        CHECK(pi->value() == ss.str());
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>() == pi);
    }

    SECTION("Json<Float>::to<Int>()") {
        CHECK(j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Float>()->value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsoncfg::Int>();
        CHECK(pi);
        CHECK(pi->value() == static_cast<jsoncfg::Int::value_t>(123.5));
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>() == pi);
    }

    SECTION("Json<Float>::to<Bool>() true") {
        CHECK(j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Float>()->value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi->value() == true);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == pi);
    }

    j = jsoncfg::Float(0.0);
    SECTION("Json<Float>::to<Bool>() false") {
        CHECK(j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Float>()->value() == Approx(0).margin(1e-6));
        auto pi = j.to<jsoncfg::Bool>();
        CHECK(pi);
        CHECK(pi->value() == false);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == pi);
    }

    j = jsoncfg::Str("i7.3");
    SECTION("Json<Str>::to<Float>() failed") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "i7.3");
        CHECK(!j.to<jsoncfg::Float>());
        CHECK(!j.as<jsoncfg::Str>());
        CHECK(!j.value());
    }
}
