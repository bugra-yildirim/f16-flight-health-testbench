#pragma once

#include <cstddef>     // std::size_t: boyut ve indeks türü.
#include <optional>    // std::optional: değer içerebilen veya boş kalan sonuç.
#include <string_view> // std::string_view: metni kopyalamadan okuma.
namespace f16 {

// inline: sabitin birden fazla kaynak dosyada kullanılmasını sağlar.
inline constexpr std::size_t max_datagram_bytes = 1024; //kabul edilen en büyük datagram boyutu.


struct TelemetrySample {
    double sim_time_s;        // Simülasyon zamanı [s].
    double altitude_msl_ft;   // Simüle MSL irtifa [ft].
    double airspeed_kt;       // Simüle hava hızı [kt].
    double heading_deg;      // Simüle heading [derece].
    double throttle_command; // Gaz komutu [boyutsuz]; itki değildir.
};

// Parser'ın döndürebileceği hata kategorileri. *?
enum class ParseError {
    none,                // Ayrıştırma başarılı.
    empty_datagram,      // Datagram hiç byte içermiyor.
    datagram_too_large,  // Datagram uygulama sınırını aşıyor.
    invalid_byte,        // İzin verilmeyen byte var.
    missing_line_ending, // Beklenen son satır sonu eksik.
    invalid_framing,     // Tek kayıt şartını bozan CR/LF düzeni var.
    field_count,         // Alan sayısı tam beş değil.
    empty_field,         // Beş alandan biri boş.
    invalid_number,      // Alan geçerli ondalık sayı biçiminde değil.
    non_finite,          // NaN veya sonsuzluk gibi sonlu olmayan değer.
    numeric_range        // Sayı double ile temsil edilebilir aralığı aşıyor.
};

// Ayrıştırmanın hem verisini hem hata bilgisini taşır.                              **?!
struct ParseResult {
    std::optional<TelemetrySample> sample;
    ParseError error;         // Başarıda none; hatada ilgili kategori.
    std::size_t field_index;  // Alan hatası: 1–5; genel hata: 0.


    // Sonucu değiştirmeden kayıt bulunup bulunmadığı                                !
    bool ok() const noexcept {
        return sample.has_value();
    }
};



// Bir datagramı doğrulayıp beş alanlı kayda dönüştürür.
// Burada sadece bildirim; uygulaması telemetry.cpp 
ParseResult parse_telemetry_v0(
    std::string_view datagram
) noexcept;


// Hata kategorisini konsol ve kayıtlar için okunabilir isme çevirir.
// Örnek: ParseError::empty_field -> "empty_field".
std::string_view parse_error_name(
    ParseError error
) noexcept;

} // namespace f16