/*
 * The practice part of a webinar with an example
 * of how to use redis-plus-plus (https://github.com/sewenew/redis-plus-plus).
 */

#include <sw/redis++/redis++.h>

#include <iostream>
#include <unordered_map>
#include <unordered_set>

namespace redis = sw::redis;

namespace {
template <typename Source>
typename Source::value_type Join(const Source &source) {
  typename Source::value_type result;
  for (const auto &item : source) {
    if (!result.empty()) {
      result += ", ";
    }
    result += item;
  }
  return result;
}
}  // namespace

int main(const int argc, const char *argv[]) {
  // Create connection to Redis server.
  auto redis = redis::Redis("tcp://127.0.0.1:6379");

  std::cout << "Connected." << std::endl;

  //////////////////////////////////////////////////////////////////////////////
  {
    std::cout << std::endl << "String put-get test:" << std::endl << std::endl;

    // Put value.
    redis.set("key", "val");

    // Get value.
    const auto &val = redis.get("key");

    if (val) {
      std::cout << *val << std::endl;
    } else {
      std::cout << "NO DATA FOUND!" << std::endl;
    }

    std::cout << std::endl
              << std::endl
              << "press any key to contine..." << std::endl;
    getchar();
  }
  //////////////////////////////////////////////////////////////////////////////
  {
    std::cout << std::endl << "Vector put-get test:" << std::endl << std::endl;

    // Push vector from variable.
    {
      const std::vector<std::string> vec = {"a", "b", "c"};
      redis.rpush("list", vec.begin(), vec.end());
    }

    // ...or push from std::initializer_list.
    redis.rpush("list", {"xxx", "q", "w", "r", "y"});

    // Getting list-data from Redis:
    {
      std::vector<std::string> result;
      redis.lrange("list", 0, -1, std::back_inserter(result));

      std::cout << Join(result) << std::endl;
    }

    std::cout << std::endl
              << std::endl
              << "press any key to contine..." << std::endl;
    getchar();
  }

  //////////////////////////////////////////////////////////////////////////////

  {
    std::cout << std::endl
              << "Redis could sort your data:" << std::endl
              << std::endl;

    redis.zadd("sorted_set", "m1", 2.1);

    {
      const std::unordered_map<std::string, double> scores = {{"m2", 4.3},
                                                              {"m3", 1.5}};
      redis.zadd("sorted_set", scores.begin(), scores.end());
    }

    std::vector<std::pair<std::string, double>> result;
    redis.zrangebyscore("sorted_set",
                        redis::UnboundedInterval<double>{},  // (-inf, +inf)
                        std::back_inserter(result));

    for (const auto &record : result) {
      std::cout << record.first << ": " << record.second << std::endl;
    }

    std::cout << std::endl
              << std::endl
              << "press any key to contine..." << std::endl;
    getchar();
  }

  //////////////////////////////////////////////////////////////////////////////

  {
    std::cout << std::endl << "Redis has scripts:" << std::endl << std::endl;

    // Script returns a single element.
    auto num = redis.eval<long long>("return 1", {}, {});

    std::cout << "num: " << num << std::endl;

    // Script returns an array of elements.
    std::vector<std::string> nums;
    redis.eval("return {ARGV[1], ARGV[2]}", {}, {"1", "2"},
               std::back_inserter(nums));

    std::cout << "nums: " << Join(nums) << std::endl;

    std::cout << std::endl
              << std::endl
              << "press any key to contine..." << std::endl;
    getchar();
  }

  //////////////////////////////////////////////////////////////////////////////

  {
    std::cout << std::endl << "Time to live (TTL):" << std::endl << std::endl;

    // mset with TTL
    auto mset_with_ttl_script = R"(
        local len = #KEYS
        if (len == 0 or len + 1 ~= #ARGV) then return 0 end
        local ttl = tonumber(ARGV[len + 1])
        if (not ttl or ttl <= 0) then return 0 end
        for i = 1, len do redis.call("SET", KEYS[i], ARGV[i], "EX", ttl) end
        return 1
    )";

    // Set multiple key-value pairs with TTL of 10 seconds.
    auto keys = {"key1", "key2", "key3"};
    std::vector<std::string> args = {"val1", "val2", "val3", "5"};
    redis.eval<long long>(mset_with_ttl_script, keys.begin(), keys.end(),
                          args.begin(), args.end());

    const auto testKeys = [&redis] {
      std::cout << "key1: " << redis.get("key1").value_or("NO VALUE")
                << std::endl;
      std::cout << "key2: " << redis.get("key2").value_or("NO VALUE")
                << std::endl;
      std::cout << "key3: " << redis.get("key3").value_or("NO VALUE")
                << std::endl;
    };

    testKeys();
    std::cout << std::endl
              << std::endl
              << "WAIT 6 seconds and press any key..." << std::endl;
    getchar();
    testKeys();

    std::cout << std::endl
              << std::endl
              << "press any key to contine..." << std::endl;
    getchar();
  }

  //////////////////////////////////////////////////////////////////////////////

  std::cout << "Completed!" << std::endl;
  return 0;
}
