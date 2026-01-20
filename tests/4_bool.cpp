#include "test_helper.hpp"

TEST_CASE("Bool") {
    auto v = jsonho::Bool(true);
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

    auto j = jsonho::Json();
    j.loads("false");
    SECTION("Json::loads(false)") {
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>()->value() == false);
    }

    j.loads(v.dumps());
    SECTION("Json::loads(true)") {
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>()->value() == true);
    }

    j = jsonho::Str("123");
    SECTION("Json<Bool> = Str") {
        CHECK(!j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "123");
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

    j = jsonho::Bool(true);
    SECTION("Json<Bool>::to<Str>()") {
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>()->value() == true);
        auto pi = j.to<jsonho::Str>();
        CHECK(pi);
        CHECK(pi->value() == "true");
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>() == pi);
    }

    SECTION("Json<Bool>::to<Float>()") {
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>()->value() == true);
        auto pi = j.to<jsonho::Float>();
        CHECK(pi);
        CHECK(pi->value() == Approx(1).margin(1e-6));
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>() == pi);
    }

    SECTION("Json<Bool>::to<Int>()") {
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>()->value() == true);
        auto pi = j.to<jsonho::Int>();
        CHECK(pi);
        CHECK(pi->value() == 1);
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>() == pi);
    }
}
