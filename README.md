## 简介
jsoncfg (**Json Config**)
这是一个 C++ 版 JSON 格式字符串生成和解析库；此程序库仅包含一个头文件（header-only）。

## 1. 特点
* 支持生成、解析、修改 JSON 格式字符串，接口简洁易用
* 解析并重新序列化 JSON 字符串时，能保持其中数据出现的顺序、维持数据的布局、维持整数和浮点数的表示格式（如十六进制、科学计数法等），对配置文件非常友好
* 整数类型支持十六进制、八进制和二进制表示
* 浮点数类型支持普通格式和科学计数法表示
* 字典和列表可设置全部显示一行、或设置每行显示的数据个数、或将某一项或某几项单行显示，配置非常灵活

## 2. 快速上手
[examples/example.cpp](examples/example.cpp)
```cpp
#include <iostream>
#include "jsoncfg.hpp"

int main(int argc, char *argv[]) {
    std::cout << "empty dict: " << jsoncfg::Dict().dumps() << std::endl;
    std::cout << "empty list: " << jsoncfg::List().dumps() << std::endl;

    // 构造默认 JSON
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

    // 序列化成 1 行字符串没有任何换行和空格
    std::cout << root_e.dumps() << std::endl;
    // 序列化成标准样式（字典和列表的所有子项各自显示一行），缩减大小为 4
    std::cout << root_e.dumps(4) << std::endl;
    // 按标准格式序列化，并保持定制格式，如果有缩进，则缩减大小为 4
    std::cout << root_e.dumps(4, true) << std::endl;

    jsoncfg::Json root_d;
    // 解析任意布局的 json 字符串
    root_d.loads(root_e.dumps(4, true));
    // 序列化成 1 行字符串没有任何换行和空格
    std::cout << root_d.dumps() << std::endl;
    // 将“路径”为 ["list"][-1] 这一项的整数改成十进制显示
    root_d.as<jsoncfg::Dict>()->get("list").as<jsoncfg::List>()[-1].as<jsoncfg::Int>()->base(10);
    // 将“路径”为 ["list"][3] 这一项的整数值由 224 改成 512
    root_d["list"][3].as<jsoncfg::Uint>() = 512;
    std::cout << root_d.dumps(4, true) << std::endl;
    return 0;
}
```
Output:
```log
empty dict: {}
empty list: []
{"hello":"world","list":[1,3,224,224,"float",{"a":1,"b":"2"},[1.5,6,8],[1,2],3.141593e+01,0x8000,0b1100]}
{
    "hello": "world",
    "list": [
        1,
        3,
        224,
        224,
        "float",
        {
            "a": 1,
            "b": "2"
        },
        [
            1.5,
            6,
            8
        ],
        [
            1,
            2
        ],
        3.141593e+01,
        0x8000,
        0b1100
    ]
}
{
    "hello": "world",
    "list": [
        1, 3, 224,
        224, "float",
        {"a": 1, "b": "2"},
        [
            1.5, 6, 8
        ], [1, 2],
        3.141593e+01,
        0x8000,
        0b1100
    ]
}
{"hello":"world","list":[1,3,224,224,"float",{"a":1,"b":"2"},[1.5,6,8],[1,2],3.141593e+01,0x8000,0b1100]}
{
    "hello": "world",
    "list": [
        1, 3, 224, 512,
        "float",
        {"a": 1, "b": "2"},
        [
            1.5, 6, 8
        ], [1, 2],
        3.141593e+01,
        0x8000,
        12
    ]
}
```

## 3. 接口说明
|class|method or arguments|description|
|-----|-------------------|-----------|
|Json |                   |Json 基础对象，可表达所有 JSON 对象
|     |type               |返回当前 JSON 对象的数据类型
|     |loads              |解析 JSON 格式字符串
|     |dumps              |将 JSON 对象序列化成 JSON 格式字符串
|     |value              |返回当前 JSON 对象存储的数据
|     |as_head            |序列化时，遇到当前对象要换行
|     |as`<Str>`          |尝试将 Json 对象解释成 Str 对象
|     |as`<Dec>`          |尝试将 Json 对象解释成 Dec 对象
|     |as`<Int>`          |尝试将 Json 对象解释成 Int 对象
|     |as`<Uint>`         |尝试将 Json 对象解释成 Uint 对象
|     |as`<Sint>`         |尝试将 Json 对象解释成 Sint 对象
|     |as`<Bool>`         |尝试将 Json 对象解释成 Bool 对象
|     |as`<Dict>`         |尝试将 Json 对象解释成 Dict 对象
|     |as`<List>`         |尝试将 Json 对象解释成 List 对象
|     |to`<Str>`          |将 Json 对象转换成 Str 对象
|     |to`<Dec>`          |将 Json 对象转换成 Dec 对象
|     |to`<Int>`          |将 Json 对象转换成 Int 对象
|     |to`<Uint>`         |将 Json 对象转换成 Uint 对象
|     |to`<Sint>`         |将 Json 对象转换成 Sint 对象
|     |to`<Bool>`         |将 Json 对象转换成 Bool 对象
|     |to`<Dict>`         |将 Json 对象转换成 Dict 对象
|     |to`<List>`         |将 Json 对象转换成 List 对象
|     |Json::[string]     |尝试将 Json 对象解释成 Dict 对象，并获取键值为 string 的值的引用。（可作为左值）。（如果转换失败，会引发空指针引用错误）
|     |Json::[index]      |尝试将 Json 对象解释成 List 对象，并获取索引为 index 的值的引用。（可作为左值）。（如果转换失败，引发空指针引用错误）
|Str  |                   |字符串类型的 JSON 对象
|     |value              |返回字符串值的引用（可作为左值）
|Dec  |                   |浮点类型的 JSON 对象
|     |value              |返回浮点值的引用（可作为左值）
|     |scientific         |序列化时，用科学计数法表示当前浮点数
|Int  |                   |整数类型的 JSON 对象
|     |value              |返回整数值的引用（可作为左值）。注意返回值是一个 union 对象
|     |is_signed          |当前整数对象存储的是否速有符号整数
|     |base               |序列化时，使用的整数的进制。目前只支持十进制（默认）、十六进制、八进制和二进制
|     |as`<Uint>`         |将整数强行视为无符号整数
|     |as`<Sint>`         |将整数强行视为有符号整数
|Uint |                   |无符号整数类型的 JSON 对象。继承 Int 类型的所有属性和接口
|     |value              |返回无符号整数值的引用（可作为左值）
|Sint |                   |有符号整数类型的 JSON 对象。继承 Int 类型的所有属性和接口
|     |value              |返回有符号整数值的引用（可作为左值）
|Bool |                   |布尔类型的 JSON 对象
|     |value              |返回 bool 数值的引用（可作为左值）
|Dict |                   |字典类型的 JSON 对象。键值类型是字符（std::string），数据类型是 Json
|     |value              |返回 jsoncfg::OrderedDict 对象的引用（可作为左值）
|     |Dict::[string]     |返回键值为 string 的值的引用。（可作为左值）
|     |get(string)        |返回键值为 string 的值，可提供默认值
|     |set(string, Json)  |添加或修改键值为 string 的值
|     |find(string)       |查找键值为 string 的键值对，返回迭代器对象
|     |clear              |清空字典
|     |size               |返回字典中数据的个数
|     |begin end rbegin...|标准迭代器接口
|     |items_per_line     |设置序列化时每行显示的数据个数：0 包括大括号在内的所有内容显示在一行；-1 所有数据显示在一行，大括号各自单独显示一个； N 每行显示 N 个数据
|List |                   |数组类型的 JSON 对象。元素类型为 Json
|     |value              |返回 std::list<Json> 对象的引用（可作为左值）
|     |append             |在数组尾部追加 JSON 对象
|     |insert             |在指定位置插入 JSON 对象
|     |List::[index]      |返回索引为 index 的值的引用。（可作为左值）
|     |get(index)         |返回键值为 index 的值的引用。（可作为左值）
|     |set(index, Json)   |修改键值为 index 的值
|     |iterators          |将所有元素所在的迭代器放入 std::vector 中返回。方便高效随机访问
|     |vector`<type>`     |将数组中的所有对象转换成 type 数据类型，并放入 std::vector 中返回。可提供默认值，用于填充转换失败的位置
|     |clear              |清空字典
|     |size               |返回字典中数据的个数
|     |begin end rbegin...|标准迭代器接口
|     |items_per_line     |设置序列化时每行显示的数据个数：0 包括大括号在内的所有内容显示在一行；-1 所有数
