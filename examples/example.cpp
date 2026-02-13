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
            }, 0, true),
            jsoncfg::List({
                1.5, 6, 8,
            }, -1, true),
            jsoncfg::List({1, 2}, 0),
        }, 3)},
    }, 1);
    auto json_s = root_e.dumps();
    std::cout << json_s << std::endl;
    jsoncfg::Json root_d;
    root_d.loads(json_s);
    std::cout << root_d.dumps() << std::endl;
    std::cout << root_e.dumps(4) << std::endl;
    std::cout << root_e.dumps(4, true) << std::endl;
    return 0;
}
