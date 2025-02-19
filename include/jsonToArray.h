#include <Arduino.h>
#include <ArduinoJson.h>
#include <array>

namespace ArduinoJson {
template <typename T, size_t N>
struct Converter<std::array<T, N> > {
  static void toJson(const std::array<T, N>& src, JsonVariant dst) {
    JsonArray array = dst.to<JsonArray>();
    for (T item : src)
      array.add(item);
  }

  static std::array<T, N> fromJson(JsonVariantConst src) {
    std::array<T, N> dst;
    size_t idx = 0;
    for (T item : src.as<JsonArrayConst>())
      dst[idx++] = item;
    return dst;
  }

  static bool checkJson(JsonVariantConst src) {
    JsonArrayConst array = src;
    bool result = array;
    size_t size = 0;
    for (JsonVariantConst item : array) {
      result &= item.is<T>();
      size++;
    }
    return result && size == N;
  }
};
}  // namespace ArduinoJson