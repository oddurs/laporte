// wav_roundtrip.hpp — a file written is a file read, bit for bit.
//
// Writes a buffer holding full scale, zero and both negative extremes, reads it
// back and compares every sample. Then it damages the header a byte at a time
// in the places a foreign file would differ and checks each is refused. Measures
// 1 if all of that holds, 0 otherwise.
//
// Not covered: that a player agrees with the header; that was looked at once
// with afinfo, by hand, and is in the item's note.

#pragma once

#include <cstdint>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

#include <laporte/wav.hpp>

#include "verify.hpp"

namespace laporte::checks {

inline double wav_roundtrip_measure() {
    const wav::Pcm in{48000, {0, 1, -1, 32767, -32768, -32767, 12345, -12345, 0}};
    const auto enc = wav::encode(in);
    if (!enc) return 0.0;
    const auto& bytes = *enc;
    if (bytes.size() != 44 + 2 * in.samples.size()) return 0.0;
    const auto out = wav::decode(bytes);
    if (!out || !(*out == in)) return 0.0;
    const auto again = wav::encode(*out);
    if (!again || *again != bytes) return 0.0;

    // Foreign or damaged headers: every field a variant would change.
    const std::size_t at[] = {0, 4, 8, 12, 16, 20, 22, 24, 28, 32, 34, 36, 40};
    for (std::size_t i : at) {
        auto bad = bytes;
        bad[i] ^= 0x01;
        if (wav::decode(bad)) return 0.0;
    }
    auto cut = bytes;   cut.pop_back();
    auto tail = bytes;  tail.push_back(0);
    if (wav::decode(cut) || wav::decode(tail)) return 0.0;
    if (wav::decode(std::vector<std::uint8_t>(10, 0))) return 0.0;

    // Rates the reader refuses, the writer refuses too.
    if (wav::encode(wav::Pcm{0, {1}}) || wav::encode(wav::Pcm{0x80000000u, {1}})) return 0.0;

    // Quantise: extremes, clipping, refusals.
    const std::vector<Volts> v{Volts{4.0}, Volts{-4.0}, Volts{0.0}, Volts{9.0}, Volts{-9.0}};
    const auto q = wav::quantise<VoltTag>(v, Volts{4.0});
    if (!q || *q != std::vector<std::int16_t>{32767, -32767, 0, 32767, -32767}) return 0.0;
    const auto back = wav::dequantise<VoltTag>(*q, Volts{4.0});
    if (!back || (*back)[0] != Volts{4.0} || (*back)[1] != Volts{-4.0} || (*back)[2] != Volts{0.0})
        return 0.0;
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    const std::vector<Volts> bad_nan{Volts{1.0}, Volts{nan}};
    const std::vector<Volts> bad_inf{Volts{inf}};
    if (wav::quantise<VoltTag>(bad_nan, Volts{4.0}) || wav::quantise<VoltTag>(bad_inf, Volts{4.0}))
        return 0.0;
    for (double f : {0.0, -1.0, nan, inf})
        if (wav::quantise<VoltTag>(v, Volts{f}) || wav::dequantise<VoltTag>(*q, Volts{f})) return 0.0;

    // Through a file: written, printed scale returned, read back identical.
    const std::string path =
        (std::filesystem::temp_directory_path() / "laporte_wav_roundtrip.wav").string();
    const std::vector<Amperes> a{Amperes{0.02}, Amperes{-0.02}, Amperes{0.0}, Amperes{0.01}};
    const auto sc = wav::write_file<AmpereTag>(path, 48000, a, Amperes{0.02});
    const auto rd = wav::read_file(path);
    std::filesystem::remove(path);
    if (!sc || sc->full_scale != 0.02 || sc->unit != "A") return 0.0;
    if (!rd || rd->rate != 48000 || rd->samples != *wav::quantise<AmpereTag>(a, Amperes{0.02}))
        return 0.0;
    if (wav::read_file(path)) return 0.0;   // gone, so refused
    return 1.0;
}

inline const bool wav_roundtrip_registered = verify::add({
    .name = "wav.roundtrip", .source = "selftest", .unit = "", .published = 1.0,
    .tolerance = 0.0, .until_item = 0, .measure = wav_roundtrip_measure});

}  // namespace laporte::checks
