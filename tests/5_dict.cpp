#include "test_helper.hpp"

TEST_CASE("Dict") {
    auto v = jsonho::Dict({
        {"bool", false},
        {"dict", jsonho::Dict()},
        {"float", 2.5},
        {"int", 7},
        {"list", jsonho::List()},
        {"str", "hello"},
    });
    SECTION("Dict::dumps()") {
        CHECK(v.dumps() == "{\"bool\":false,\"dict\":{},\"float\":2.5,\"int\":7,\"list\":[],\"str\":\"hello\"}");
    }

    v.loads("{\"bool\":true,\"dict\":{\"a\":97},\"float\":5.2,\"int\":9,\"list\":[99,100],\"str\":\"world\"}");
    SECTION("Dict::loads({})") {
        CHECK(v.get("bool").as<jsonho::Bool>()->value() == true);
        CHECK(v.get("dict").as<jsonho::Dict>()->get("a").as<jsonho::Int>()->value() == 97);
        CHECK(v.get("float").as<jsonho::Float>()->value() == Approx(5.2).margin(1e-6));
        CHECK(v.get("int").as<jsonho::Int>()->value() == 9);
        CHECK(v.get("list").as<jsonho::List>());
        CHECK(v.get("str").as<jsonho::Str>()->value() == "world");
    }

    auto j = jsonho::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(Dict)") {
        auto jd = j.as<jsonho::Dict>();
        CHECK(jd);
        CHECK(jd->get("bool").as<bool>()->value() == true);
        CHECK(jd->get("dict").as<jsonho::Dict>()->get("a").as<int>()->value() == 97);
        CHECK(jd->get("float").as<float>()->value() == Approx(5.2).margin(1e-6));
        CHECK(jd->get("int").as<int>()->value() == 9);
        CHECK(jd->get("list").as<jsonho::List>());
        CHECK(jd->get("str").as<std::string>()->value() == "world");
    }

    std::string s = "{\"name\":\"json\",\"year\":2026}";
    j = jsonho::Str(s);
    SECTION("Json<Dict> = Str") {
        CHECK(!j.as<jsonho::Dict>());
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == s);
    }

    SECTION("Json<Str>::to<Dict>() success") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == s);
        auto p = j.to<jsonho::Dict>();
        CHECK(p);
        CHECK(p->get("name").as<std::string>()->value() == "json");
        CHECK(p->get("year").as<int>()->value() == 2026);
    }

    j = jsonho::Dict({{"addr", 0x80000000}, {"size", 512}, {"mode", "read-only"}});
    SECTION("Json<Dict>::to<Str>()") {
        CHECK(j.as<jsonho::Dict>());
        auto p = j.to<jsonho::Str>();
        CHECK(p);
        CHECK(p->value() == "{\"addr\":2147483648,\"mode\":\"read-only\",\"size\":512}");
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>() == p);
    }

    SECTION("Json<Dict>::to<Bool>() true") {
        CHECK(j.as<jsonho::Dict>());
        auto p = j.to<jsonho::Bool>();
        CHECK(p);
        CHECK(p->value() == true);
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>() == p);
    }

    j = jsonho::Dict();
    SECTION("Json<Dict>::to<Bool>() false") {
        CHECK(j.as<jsonho::Dict>());
        auto p = j.to<jsonho::Bool>();
        CHECK(p);
        CHECK(p->value() == false);
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>() == p);
    }

    j = jsonho::Str("i7");
    SECTION("Json<Str>::to<Dict>() failed") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "i7");
        CHECK(!j.to<jsonho::Dict>());
        CHECK(!j.as<jsonho::Str>());
        CHECK(!j.value());
    }
}
