#include <iostream>
#include "jsoncfg.hpp"

int main(int argc, char *argv[]) {
    std::cout << "empty dict: " << jsoncfg::Dict().dumps() << std::endl;
    std::cout << "empty list: " << jsoncfg::List().dumps() << std::endl;
    auto root_e = jsoncfg::Dict({
        {"hello", "world"},
        {"list", jsoncfg::List({
            1, 3, 224,
            224, "float",
            jsoncfg::Dict({{"a", 1}, {"b", "2"}}, 0, true),
            jsoncfg::List({
                1.5, 6, 8,
            }, -1, true), jsoncfg::List({1, 2}, 0),
            jsoncfg::Dec(31.415926, true, true),
            jsoncfg::Uint(0x8000, 16, true),
            jsoncfg::Uint(0x0C, 2, true),
        }, 3)},
    }, 1);
    std::cout << root_e.dumps() << std::endl;
    std::cout << root_e.dumps(4) << std::endl;
    std::cout << root_e.dumps(4, true) << std::endl;
    jsoncfg::Json root_d;
    root_d.loads(root_e.dumps(4, true));
    std::cout << root_d.dumps() << std::endl;
    root_d.as<jsoncfg::Dict>()["list"].as<jsoncfg::List>()[-1].as<jsoncfg::Int>()->base(10);
    std::cout << root_d.dumps(4, true) << std::endl;
    return 0;
}
