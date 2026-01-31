#include "test_helper.hpp"

TEST_CASE("List") {
    auto v = jsonho::List({
        1, 1.5, "hello", jsonho::Dict({{"hello", 7}, {"json", 8.5}}), jsonho::List({4, 5, 6}),
    });
    SECTION("List::dumps()") {
        CHECK(v.dumps() == "[1,1.5,\"hello\",{\"hello\":7,\"json\":8.5},[4,5,6]]");
    }

    v.loads("[1.2,5,\"hello\",{\"world\":8,\"json\":9.1},[4,5,6]]");
    SECTION("List::loads({})") {
        auto iters = v.iterators();
        CHECK(iters[0]->as<float>()->value() == Approx(1.2).margin(1e-6));
        CHECK(iters[1]->as<int>()->value() == 5);
        CHECK(iters[2]->as<std::string>()->value() == "hello");
        CHECK(iters[3]->as<jsonho::Dict>()->get("world").as<int>()->value() == 8);
        CHECK(iters[3]->as<jsonho::Dict>()->get("json").as<float>()->value() == Approx(9.1).margin(1e-6));
        CHECK(iters[4]->as<jsonho::List>()->vector<int>(0) == std::vector<int> {4, 5, 6});
        CHECK(v.vector<std::string>("") == std::vector<std::string> {"1.2", "5", "hello", "{\"world\":8,\"json\":9.1}", "[4,5,6]"});
        CHECK(v.vector<int>(-1) == std::vector<int> {1, 5, -1, -1, -1});
        CHECK(v.vector<float>(0) == std::vector<float> {1.2, 5, 0, 0, 0});
    }

    SECTION("List::set()") {
        CHECK(v.set(0, 2.5).get(0).as<float>()->value() == Approx(2.5).margin(1e-6));
        CHECK(v.set(1, 10).get(1).as<int>()->value() == 10);
        CHECK(v.set(2, "json").get(2).as<std::string>()->value() == "json");
        CHECK(v.get(3).as<jsonho::Dict>()->set("world", 16).get("world").as<int>()->value() == 16);
        CHECK(v.get(3).as<jsonho::Dict>()->set("json", 22.3).get("json").as<float>()->value() == Approx(22.3).margin(1e-6));
        CHECK(v.get(4).as<jsonho::List>()->set(1, 10).vector<int>(0) == std::vector<int> {4, 10, 6});
        v.get(4).as<jsonho::List>()->get(2).as<int>()->value() = 12;
        CHECK(v.dumps() == "[2.5,10,\"json\",{\"world\":16,\"json\":22.3},[4,10,12]]");
    }

    SECTION("List::iterators()") {
        auto iters = v.iterators();
        iters[0]->as<float>()->value() = 2.5;
        iters[1]->as<int>()->value() = 10;
        iters[2]->as<std::string>()->value() = "json";
        iters[3]->as<jsonho::Dict>()->get("world").as<int>()->value() = 16;
        iters[3]->as<jsonho::Dict>()->get("json").as<float>()->value() = 22.3;
        iters[4]->as<jsonho::List>()->set(1, 10);
        v.get(4).as<jsonho::List>()->get(2).as<int>()->value() = 12;
        CHECK(v.dumps() == "[2.5,10,\"json\",{\"world\":16,\"json\":22.3},[4,10,12]]");
    }

    auto j = jsonho::Json();
    j.loads(v.dumps());
    SECTION("Json::loads(List)") {
        auto jd = j.as<jsonho::List>();
        CHECK(jd);
        auto iters = jd->iterators();
        CHECK(iters[0]->as<float>()->value() == Approx(1.2).margin(1e-6));
        CHECK(iters[1]->as<int>()->value() == 5);
        CHECK(iters[2]->as<std::string>()->value() == "hello");
        CHECK(iters[3]->as<jsonho::Dict>()->get("world").as<int>()->value() == 8);
        CHECK(iters[3]->as<jsonho::Dict>()->get("json").as<float>()->value() == Approx(9.1).margin(1e-6));
        CHECK(iters[4]->as<jsonho::List>()->vector<int>(0) == std::vector<int> {4, 5, 6});
    }

    std::string s = "[\"year\",2026]";
    j = jsonho::Str(s);
    SECTION("Json<List> = Str") {
        CHECK(!j.as<jsonho::List>());
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == s);
    }

    SECTION("Json<Str>::to<List>() success") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<std::string>()->value() == s);
        auto p = j.to<jsonho::List>();
        CHECK(p);
        CHECK(p->get(0).as<std::string>()->value() == "year");
        CHECK(p->get(1).as<int>()->value() == 2026);
    }

    j = jsonho::List({"addr", 0x80000000, "size", 512, "mode", "read-only"});
    SECTION("Json<List>::to<Str>()") {
        CHECK(j.as<jsonho::List>());
        auto p = j.to<jsonho::Str>();
        CHECK(p);
        CHECK(p->value() == "[\"addr\",2147483648,\"size\",512,\"mode\",\"read-only\"]");
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<std::string>() == p);
    }

    SECTION("Json<List>::to<Bool>() true") {
        CHECK(j.as<jsonho::List>());
        auto p = j.to<jsonho::Bool>();
        CHECK(p);
        CHECK(p->value() == true);
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<bool>() == p);
    }

    j = jsonho::List();
    SECTION("Json<List>::to<Bool>() false") {
        CHECK(j.as<jsonho::List>());
        auto p = j.to<jsonho::Bool>();
        CHECK(p);
        CHECK(p->value() == false);
        CHECK(j.as<jsonho::Bool>());
        CHECK(j.as<jsonho::Bool>() == p);
    }

    j = jsonho::Str("i7");
    SECTION("Json<Str>::to<List>() failed") {
        CHECK(j.as<jsonho::Str>());
        CHECK(j.as<jsonho::Str>()->value() == "i7");
        CHECK(!j.to<jsonho::List>());
        CHECK(!j.as<jsonho::Str>());
        CHECK(!j.value());
    }
}
