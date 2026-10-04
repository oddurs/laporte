// wav.hpp — a sound file is a forty-four-byte header and some numbers.
//
// Why a WAV at all: every instrument in this office ends in the same place, a
// row of samples somebody wants to listen to or look at, and a WAV is the one
// sink they can all share. A dial tone, a ring, a hum on a long loop: each is
// written here and opened in anything that plays sound. Why by hand: rule 5.
// The format is a header of fixed size followed by the samples, small enough to
// read in one sitting, and a dependency to write it would be larger than the
// thing it replaced.
//
// The container is the RIFF/WAVE format, the "Waveform Audio File Format"
// carried in a Resource Interchange File Format wrapper; Microsoft and IBM
// published it in August 1991 as "Multimedia Programming Interface and Data
// Specifications 1.0" (also described on the Library of Congress format page for
// WAVE). Who drafted which part is unsourced here, and nothing below depends on it.
//
// A WAV has no units. It holds integers from -32768 to 32767 and has no field
// for volts. So the scale is not in the file; it is a parameter of whoever
// writes it, given here as a full-scale quantity from units.hpp, and the writer
// both prints it and returns it. A file without its scale written down beside
// it is a picture of a waveform with the axis torn off. Samples beyond full
// scale are clipped, to +/-32767 so that the range is symmetric; -32768 can
// only arrive as a raw integer. A NaN or infinite sample, or a full scale that is
// not finite and positive, is refused (nothing returned), never turned into a
// plausible-looking number; so are rates of 0 or 2^31 and above, and a payload
// too big for the format's 32-bit size fields.
//
// The sample rate is a parameter, never a constant: the office clock is
// whatever the clock decision says (48 kHz at the time of writing), and nothing
// here assumes it, least of all 8 kHz.
//
// Bytes are written one at a time in little-endian order, not by copying a
// struct, because a struct's padding and a machine's byte order are not ours to
// decide.
//
// Not modelled: every other WAV variant. No stereo, no 8- or 24-bit, no float,
// no extensible format, no extra chunks, no list or cue data. Reading refuses
// anything but exactly what writing produces.

#pragma once

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <optional>
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <laporte/units.hpp>

namespace laporte::wav {

struct Pcm {
    std::uint32_t             rate{};      // samples per second
    std::vector<std::int16_t> samples;
    friend bool operator==(const Pcm&, const Pcm&) = default;
};

// What the integers mean. The file does not know; the caller does.
struct Scale {
    double      full_scale{};  // the quantity that maps to 32767
    std::string unit;          // "V" or "A"
};

inline constexpr double full_code = 32767.0;   // the integer that full scale maps to

namespace detail {

// Private builders: append little-endian bytes to the vector they are given.

inline void put16(std::vector<std::uint8_t>& b, std::uint32_t x) {
    b.push_back(std::uint8_t(x & 0xFF));
    b.push_back(std::uint8_t((x >> 8) & 0xFF));
}
inline void put32(std::vector<std::uint8_t>& b, std::uint32_t x) {
    put16(b, x & 0xFFFF);
    put16(b, x >> 16);
}
inline void puttag(std::vector<std::uint8_t>& b, const char (&t)[5]) {
    for (int i = 0; i < 4; ++i) b.push_back(std::uint8_t(t[i]));
}
inline std::uint32_t get16(std::span<const std::uint8_t> b, std::size_t at) {
    return std::uint32_t(b[at]) | (std::uint32_t(b[at + 1]) << 8);
}
inline std::uint32_t get32(std::span<const std::uint8_t> b, std::size_t at) {
    return get16(b, at) | (get16(b, at + 2) << 16);
}
inline bool tag_is(std::span<const std::uint8_t> b, std::size_t at, const char (&t)[5]) {
    for (int i = 0; i < 4; ++i)
        if (b[at + std::size_t(i)] != std::uint8_t(t[i])) return false;
    return true;
}

template <class Tag> constexpr const char* unit_name() {
    if constexpr (std::is_same_v<Tag, VoltTag>) return "V";
    else if constexpr (std::is_same_v<Tag, AmpereTag>) return "A";
    else static_assert(std::is_same_v<Tag, VoltTag>, "wav: only volts or amperes have a scale here");
}

}  // namespace detail

inline constexpr std::size_t header_bytes = 44;

// The file as bytes: 44-byte header, then the samples.
inline std::optional<std::vector<std::uint8_t>> encode(const Pcm& p) {
    if (p.rate == 0 || p.rate >= (std::uint32_t{1} << 31)) return std::nullopt;
    if (p.samples.size() > (std::size_t{0xFFFFFFFFu} - 36) / 2) return std::nullopt;
    const std::uint32_t data = std::uint32_t(p.samples.size()) * 2;
    std::vector<std::uint8_t> b;
    b.reserve(header_bytes + data);
    detail::puttag(b, "RIFF");  detail::put32(b, 36 + data);
    detail::puttag(b, "WAVE");
    detail::puttag(b, "fmt ");  detail::put32(b, 16);
    detail::put16(b, 1);                 // PCM
    detail::put16(b, 1);                 // mono
    detail::put32(b, p.rate);
    detail::put32(b, p.rate * 2);        // bytes per second
    detail::put16(b, 2);                 // bytes per frame
    detail::put16(b, 16);                // bits per sample
    detail::puttag(b, "data");  detail::put32(b, data);
    for (std::int16_t s : p.samples) detail::put16(b, std::uint16_t(s));
    return b;
}

// Exactly what encode() writes, or nothing.
inline std::optional<Pcm> decode(std::span<const std::uint8_t> b) {
    using detail::get16; using detail::get32; using detail::tag_is;
    if (b.size() < header_bytes) return std::nullopt;
    if (!tag_is(b, 0, "RIFF") || !tag_is(b, 8, "WAVE") || !tag_is(b, 12, "fmt ") ||
        !tag_is(b, 36, "data"))
        return std::nullopt;
    if (get32(b, 16) != 16 || get16(b, 20) != 1 || get16(b, 22) != 1 || get16(b, 34) != 16)
        return std::nullopt;
    const std::uint32_t rate = get32(b, 24);
    if (rate == 0 || get32(b, 28) != rate * 2 || get16(b, 32) != 2) return std::nullopt;
    const std::uint32_t data = get32(b, 40);
    if (data % 2 != 0 || b.size() != header_bytes + std::size_t(data) ||
        get32(b, 4) != 36 + data)
        return std::nullopt;
    Pcm p{rate, {}};
    p.samples.reserve(data / 2);
    for (std::size_t i = 0; i < data / 2; ++i)
        p.samples.push_back(std::int16_t(std::uint16_t(get16(b, header_bytes + 2 * i))));
    return p;
}

namespace detail {
inline bool good_scale(double full) { return std::isfinite(full) && full > 0.0; }
}

// Volts or amperes to integers, with `full` mapping to 32767. Clipped. Nothing if
// `full` is not finite and positive or any sample is not finite.
template <class Tag>
std::optional<std::vector<std::int16_t>> quantise(std::span<const Quantity<Tag>> x,
                                                  Quantity<Tag> full) {
    if (!detail::good_scale(full.v)) return std::nullopt;
    std::vector<std::int16_t> out;
    out.reserve(x.size());
    for (Quantity<Tag> q : x) {
        if (!std::isfinite(q.v)) return std::nullopt;
        double s = std::round(q / full * full_code);
        if (s > full_code) s = full_code;
        if (s < -full_code) s = -full_code;
        out.push_back(std::int16_t(s));
    }
    return out;
}

// The inverse, to within one step: the file's integers back to quantities.
template <class Tag>
std::optional<std::vector<Quantity<Tag>>> dequantise(std::span<const std::int16_t> s,
                                                     Quantity<Tag> full) {
    if (!detail::good_scale(full.v)) return std::nullopt;
    std::vector<Quantity<Tag>> out;
    out.reserve(s.size());
    for (std::int16_t v : s) out.push_back(full * (double(v) / full_code));
    return out;
}

inline bool write_bytes(const std::string& path, std::span<const std::uint8_t> b) {
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(b.data()), std::streamsize(b.size()));
    return bool(f);
}

inline std::optional<std::vector<std::uint8_t>> read_bytes(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return std::nullopt;
    return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(f), {});
}

// Write quantities to `path` at `rate`, `full` mapping to full scale. Prints the
// scale, returns it, and returns nothing if the file could not be written.
template <class Tag>
std::optional<Scale> write_file(const std::string& path, std::uint32_t rate,
                                std::span<const Quantity<Tag>> x, Quantity<Tag> full) {
    auto q = quantise(x, full);
    if (!q) return std::nullopt;
    const auto bytes = encode(Pcm{rate, std::move(*q)});
    if (!bytes || !write_bytes(path, *bytes)) return std::nullopt;
    Scale sc{full.v, detail::unit_name<Tag>()};
    std::printf("wav: %s  %u Hz, %zu samples, full scale = %g %s\n", path.c_str(),
                unsigned(rate), x.size(), sc.full_scale, sc.unit.c_str());
    return sc;
}

inline std::optional<Pcm> read_file(const std::string& path) {
    const auto b = read_bytes(path);
    if (!b) return std::nullopt;
    return decode(*b);
}

}  // namespace laporte::wav
