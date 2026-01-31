#include <iostream>
#include "jsoncfg.hpp"

int main(int argc, char *argv[]) {
    std::cout << "empty dict: " << jsoncfg::Dict().dumps() << std::endl;
    std::cout << "empty list: " << jsoncfg::List().dumps() << std::endl;
    auto root_e = jsoncfg::Dict({
        {"hello", "world"},
        {"foo", "bar"},
        {"list", jsoncfg::List({
            1, 3, 224, 224, "float",
            jsoncfg::Dict({
                {"a", 1},
                {"b", "2"},
            }),
            jsoncfg::List({
                1.5, 6, 8,
            }),
            jsoncfg::List({1, 2}),
        })},
    });
    auto json_s = root_e.dumps();
    std::cout << json_s << std::endl;
    jsoncfg::Json root_d;
    root_d.loads(json_s);
    std::cout << root_d.dumps() << std::endl;
    return 0;
}
