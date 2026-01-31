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
        auto vec = v.vector();
        CHECK(vec[0]->as<float>()->value() == Approx(1.2).margin(1e-6));
        CHECK(vec[1]->as<int>()->value() == 5);
        CHECK(vec[2]->as<std::string>()->value() == "hello");
        CHECK(vec[3]->as<jsonho::Dict>()->get("world").as<int>()->value() == 8);
        CHECK(vec[3]->as<jsonho::Dict>()->get("json").as<float>()->value() == Approx(9.1).margin(1e-6));
        CHECK(vec[4]->as<jsonho::List>()->vector<int>(0) == std::vector<int> {4, 5, 6});
    }
}
