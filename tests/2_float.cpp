#include "test_helper.hpp"

TEST_CASE("Float") {
    auto v = jsonho::Float(2.5);
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

    auto j = jsonho::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(int)") {
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>()->value() == Approx(7.8).margin(1e-6));
    }

    j = jsonho::Str("123.5");
    SECTION("Json<Float> = Str") {
        CHECK(!j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "123.5");
    }

    SECTION("Json<Str>::to<Float>() success") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "123.5");
        auto pi = j.to<jsonho::Float>();
        CHECK(pi);
        CHECK(pi->value() == Approx(123.5).margin(1e-6));
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>() == pi);
    }

    j = jsonho::Float(123.5);
    SECTION("Json<Float>::to<Str>()") {
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>()->value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsonho::Str>();
        CHECK(pi);
        std::stringstream ss;
        ss << 123.5;
        CHECK(pi->value() == ss.str());
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>() == pi);
    }

    SECTION("Json<Float>::to<Int>()") {
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>()->value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsonho::Int>();
        CHECK(pi);
        CHECK(pi->value() == static_cast<jsonho::Int::value_t>(123.5));
        CHECK(j.as<jsonho::Int>());
        CHECK(j.as<jsonho::Int>() == pi);
    }

    SECTION("Json<Float>::to<Bool>() true") {
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>()->value() == Approx(123.5).margin(1e-6));
        auto pi = j.to<jsonho::Bool>();
        CHECK(pi);
        CHECK(pi->value() == true);
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>() == pi);
    }

    j = jsonho::Float(0.0);
    SECTION("Json<Float>::to<Bool>() false") {
        CHECK(j.as<jsonho::Float>());
        CHECK(j.as<jsonho::Float>()->value() == Approx(0).margin(1e-6));
        auto pi = j.to<jsonho::Bool>();
        CHECK(pi);
        CHECK(pi->value() == false);
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>() == pi);
    }

    j = jsonho::Str("i7.3");
    SECTION("Json<Str>::to<Float>() failed") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "i7.3");
        CHECK(!j.to<jsonho::Float>());
        CHECK(!j.as<jsonho::Str>());
        CHECK(!j.value());
    }
}
