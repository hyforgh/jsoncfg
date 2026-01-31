#include "test_helper.hpp"

TEST_CASE("Bool") {
    auto v = jsoncfg::Bool(true);
    SECTION("Bool::dumps()") {
        CHECK(v.value() == true);
        CHECK(v.dumps() == "true");
    }

    v = false;
    SECTION("Bool = false") {
        CHECK(v.value() == false);
    }

    v = true;
    SECTION("Bool = true") {
        CHECK(v.value() == true);
    }

    v.loads("false");
    SECTION("Bool::loads(false)") {
        CHECK(v.value() == false);
    }

    v.loads("true");
    SECTION("Bool::loads(true)") {
        CHECK(v.value() == true);
    }

    auto j = jsoncfg::Json();
    j.loads("false");
    SECTION("Json::loads(false)") {
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>()->value() == false);
    }

    j.loads(v.dumps());
    SECTION("Json::loads(true)") {
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>()->value() == true);
    }

    j = jsoncfg::Str("123");
    SECTION("Json<Bool> = Str") {
        CHECK(!j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>()->value() == "123");
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

    j = jsoncfg::Bool(true);
    SECTION("Json<Bool>::to<Str>()") {
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>()->value() == true);
        auto pi = j.to<jsoncfg::Str>();
        CHECK(pi);
        CHECK(pi->value() == "true");
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>() == pi);
    }

    SECTION("Json<Bool>::to<Float>()") {
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>()->value() == true);
        auto pi = j.to<jsoncfg::Float>();
        CHECK(pi);
        CHECK(pi->value() == Approx(1).margin(1e-6));
        CHECK(j.as<jsoncfg::Float>());
        CHECK(j.as<jsoncfg::Float>() == pi);
    }

    SECTION("Json<Bool>::to<Int>()") {
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>()->value() == true);
        auto pi = j.to<jsoncfg::Int>();
        CHECK(pi);
        CHECK(pi->value() == 1);
        CHECK(j.as<jsoncfg::Int>());
        CHECK(j.as<jsoncfg::Int>() == pi);
    }
}
