#include <concepts>
#include <functional>
#include <iostream>
#include <my/format/println.hpp>

namespace my {

namespace detail {

template <class T, class R = std::remove_reference_t<T>>
concept ostream_like = requires(R& obj, int val) {
    { obj << val } -> std::convertible_to<R&>;
};

}  // namespace detail

/**
 * @brief Simple utility class. Provide possibility to print into multiple
 * streams in one operator<< call
 *
 * Example:
 *     std::fstream fs("test.txt", std::ios::out);
 *     my::zip_ostream os(std::cout, fs);
 *     os << "Hello World!"; // outputs message both to fstream and cout
 *
 * @tparam Ch char type
 * @tparam Tr char traits type
 */
template <class Ch, class Tr>
class basic_zip_ostream {
   public:
    using stream_type = std::basic_ostream<Ch, Tr>;

    template <class... Args>
    explicit basic_zip_ostream(Args&... streams) {
        _streams.reserve(sizeof...(streams));
        (_streams.emplace_back(streams), ...);
    }

    /**
     * @brief view raw vector of my::printer_base instances
     *
     */
    constexpr auto& data() { return _streams; }
    constexpr const auto& data() const { return _streams; }

    friend auto& operator<<(basic_zip_ostream& os, const auto& obj) {
        for (auto&& stream : os._streams) {
            stream.get() << obj;
        }
        return os;
    }

   private:
    std::vector<my::printer_base<Ch, Tr>> _streams;
};

using zip_ostream = basic_zip_ostream<char, std::char_traits<char>>;
using zip_wostream = basic_zip_ostream<wchar_t, std::char_traits<wchar_t>>;

}  // namespace my