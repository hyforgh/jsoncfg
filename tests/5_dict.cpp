#include "test_helper.hpp"

TEST_CASE("Dict") {
    auto v = jsoncfg::Dict({
        {"bool", false},
        {"dict", jsoncfg::Dict()},
        {"float", 2.5},
        {"int", 7},
        {"list", jsoncfg::List()},
        {"str", "hello"},
    });
    SECTION("Dict::dumps()") {
        CHECK(v.dumps() == "{\"bool\":false,\"dict\":{},\"float\":2.5,\"int\":7,\"list\":[],\"str\":\"hello\"}");
    }

    v.loads("{\"bool\":true,\"dict\":{\"a\":97},\"float\":5.2,\"int\":9,\"list\":[99,100],\"str\":\"world\"}");
    SECTION("Dict::loads({})") {
        CHECK(v.get("bool").as<jsoncfg::Bool>().value() == true);
        CHECK(v.get("dict").as<jsoncfg::Dict>().get("a").as<jsoncfg::Int>().value() == 97);
        CHECK(v.get("float").as<jsoncfg::Dec>().value() == Approx(5.2).margin(1e-6));
        CHECK(v.get("int").as<jsoncfg::Int>().value() == 9);
        CHECK(v.get("list").as<jsoncfg::List>());
        CHECK(v.get("str").as<jsoncfg::Str>().value() == "world");
    }

    SECTION("Dict::set()") {
        CHECK(v.set("bool", false).get("bool").as<jsoncfg::Bool>().value() == false);
        CHECK(v.get("dict").as<jsoncfg::Dict>().set("a", 100).get("a").as<jsoncfg::Int>().value() == 100);
        CHECK(v.set("float", 6.0).get("float").as<jsoncfg::Dec>().value() == Approx(6).margin(1e-6));
        CHECK(v.set("int", 10).get("int").as<jsoncfg::Int>().value() == 10);
        CHECK(v.set("list", jsoncfg::List({1, 2, 3})).get("list").as<jsoncfg::List>().vector<int>(0) == std::vector<int> {1, 2, 3});
        CHECK(v.set("str", "json").get("str").as<std::string>().value() == "json");
        v.get("list").as<jsoncfg::List>().get(1).as<int>().value() = 5;
        CHECK(v.dumps() == "{\"bool\":false,\"dict\":{\"a\":100},\"float\":6,\"int\":10,\"list\":[1,5,3],\"str\":\"json\"}");
    }

    auto j = jsoncfg::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(Dict)") {
        auto jd = j.as<jsoncfg::Dict>();
        CHECK(jd);
        CHECK(jd.get("bool").as<bool>().value() == true);
        CHECK(jd.get("dict").as<jsoncfg::Dict>().get("a").as<int>().value() == 97);
        CHECK(jd.get("float").as<float>().value() == Approx(5.2).margin(1e-6));
        CHECK(jd.get("int").as<int>().value() == 9);
        CHECK(jd.get("list").as<jsoncfg::List>());
        CHECK(jd.get("str").as<std::string>().value() == "world");
    }

    std::string s = "{\"name\":\"json\",\"year\":2026}";
    j = jsoncfg::Str(s);
    SECTION("Json<Dict> = Str") {
        CHECK(!j.as<jsoncfg::Dict>());
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>().value() == s);
    }

    SECTION("Json<Str>::to<Dict>() success") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<std::string>().value() == s);
        auto p = j.to<jsoncfg::Dict>();
        CHECK(p);
        CHECK(p.get("name").as<std::string>().value() == "json");
        CHECK(p.get("year").as<int>().value() == 2026);
    }

    j = jsoncfg::Dict({{"addr", 0x80000000}, {"size", 512}, {"mode", "read-only"}});
    SECTION("Json<Dict>::to<Str>()") {
        CHECK(j.as<jsoncfg::Dict>());
        auto p = j.to<jsoncfg::Str>();
        CHECK(p);
        CHECK(p.value() == "{\"addr\":2147483648,\"size\":512,\"mode\":\"read-only\"}");
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<std::string>() == p);
    }

    SECTION("Json<Dict>::to<Bool>() true") {
        CHECK(j.as<jsoncfg::Dict>());
        auto p = j.to<jsoncfg::Bool>();
        CHECK(p);
        CHECK(p.value() == true);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<bool>() == p);
    }

    j = jsoncfg::Dict();
    SECTION("Json<Dict>::to<Bool>() false") {
        CHECK(j.as<jsoncfg::Dict>());
        auto p = j.to<jsoncfg::Bool>();
        CHECK(p);
        CHECK(p.value() == false);
        CHECK(j.as<jsoncfg::Bool>());
        CHECK(j.as<jsoncfg::Bool>() == p);
    }

    j = jsoncfg::Str("i7");
    SECTION("Json<Str>::to<Dict>() failed") {
        CHECK(j.as<jsoncfg::Str>());
        CHECK(j.as<jsoncfg::Str>().value() == "i7");
        CHECK(!j.to<jsoncfg::Dict>());
        CHECK(!j.as<jsoncfg::Str>());
        CHECK(!j.value());
    }
}
