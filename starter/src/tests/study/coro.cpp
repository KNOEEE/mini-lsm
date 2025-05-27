#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/futures/Future.h>
#include <folly/executors/GlobalExecutor.h>
#include <folly/init/Init.h>
#include <iostream>

// 定义一个协程函数，返回一个 Task<int>
folly::coro::Task<int> slow() {
    std::cout << "before sleep" << std::endl;
    co_await folly::futures::sleep(std::chrono::seconds{1}); // 暂停协程 1 秒
    std::cout << "after sleep" << std::endl;
    co_return 1; // 返回值 1
}

int main(int argc, char** argv) {
    folly::init(&argc, &argv); // 初始化 Folly
    // 运行协程并等待其完成
    folly::coro::blockingWait(
        slow().scheduleOn(folly::getGlobalCPUExecutor().get()));
    return 0;
}