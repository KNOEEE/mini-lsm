#include <folly/experimental/coro/Generator.h>
#include <folly/init/Init.h>
#include <iostream>
#include <string>

// 定义一个生成器协程
folly::coro::Generator<std::string> generate_items(int count) {
    for (int i = 0; i < count; ++i) {
        co_yield "item " + std::to_string(i);
    }
}

int main(int argc, char** argv) {
    folly::init(&argc, &argv);
    for (auto item : generate_items(3)) {
        std::cout << item << std::endl;
    }
    return 0;
}