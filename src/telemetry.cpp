#include "telemetry.hpp"

#include <array>        // Sabit boyutlu alan ve değer dizileri.
#include <charconv>     // Locale'den bağımsız sayı ayrıştırma.
#include <cmath>        // std::isfinite.
#include <system_error> // std::errc hata kodları.

namespace f16 {

// Yardımcı fonksiyonları yalnızca bu .cpp dosyasında görünür tutar.
namespace {

// Karakterin ASCII rakam olup olmadığını kontrol eder.
bool is_digit(char c) noexcept {
    return c >= '0' && c <= '9';
}

// NaN/Inf gibi ifadeleri büyük-küçük harften bağımsız karşılaştırır.
// expected parametresi küçük harfli metin olarak verilir.
bool equal_ascii_ignore_case(
    std::string_view value,
    std::string_view expected
) noexcept {
    if (value.size() != expected.size()) {
        return false;
    }

    for (std::size_t i = 0; i < value.size(); ++i) {
        const char c = value[i];

        // ASCII büyük harfi küçük harfe dönüştürür.
        const char lower = c >= 'A' && c <= 'Z'
            ? static_cast<char>(c + ('a' - 'A'))
            : c;

        if (lower != expected[i]) {
            return false;
        }
    }

    return true;
}

// Kabul edilen sayı biçimi: [+-]?[0-9]+(\.[0-9]+)?
// Örnek: 12, -50.25, +0.75.
// Boşluk, virgül, üs gösterimi ve eksik rakamlar kabul edilmez.
bool is_fixed_decimal(std::string_view text) noexcept {
    std::size_t i = 0;

    // Başta isteğe bağlı bir işaret bulunabilir.
    if (!text.empty() &&
        (text.front() == '+' || text.front() == '-')) {
        ++i;
    }

    // Ondalık noktadan önce en az bir rakam gerekir.
    const std::size_t integer_start = i;

    while (i < text.size() && is_digit(text[i])) {
        ++i;
    }

    if (i == integer_start) {
        return false;
    }

    // Nokta varsa sonrasında da en az bir rakam gerekir.
    if (i < text.size() && text[i] == '.') {
        ++i;

        const std::size_t fraction_start = i;

        while (i < text.size() && is_digit(text[i])) {
            ++i;
        }

        if (i == fraction_start) {
            return false;
        }
    }

    // Metnin tamamı tüketilmiş olmalı; sonda başka karakter kalamaz.
    return i == text.size();
}

// Tek bir alanı double'a dönüştürür.
// Başarıda value doldurulur; hatada ilgili kategori döndürülür.
ParseError parse_number(
    std::string_view text,
    double& value
) noexcept {
    if (text.empty()) {
        return ParseError::empty_field;
    }

    // Non-finite ifadeleri tanımak için işaretsiz bir görünüm oluşturur.
    auto unsigned_text = text;

    if (unsigned_text.front() == '+' ||
        unsigned_text.front() == '-') {
        unsigned_text.remove_prefix(1);
    }

    if (equal_ascii_ignore_case(unsigned_text, "nan") ||
        equal_ascii_ignore_case(unsigned_text, "inf") ||
        equal_ascii_ignore_case(unsigned_text, "infinity")) {
        return ParseError::non_finite;
    }

    if (!is_fixed_decimal(text)) {
        return ParseError::invalid_number;
    }

    // from_chars baştaki '+' işaretini kabul etmez.
    // Biçimini doğruladığımız bu işareti görünümden çıkarıyoruz.
    if (text.front() == '+') {
        text.remove_prefix(1);
    }

    const auto parsed = std::from_chars(
        text.data(),
        text.data() + text.size(),
        value,
        std::chars_format::fixed
    );

    // Temsil aralığı dışındaki çok büyük/küçük değerleri reddeder.
    if (parsed.ec == std::errc::result_out_of_range) {
        return ParseError::numeric_range;
    }

    // Dönüşüm başarılı olmalı ve alanın tamamını okumalı.
    if (parsed.ec != std::errc{} ||
        parsed.ptr != text.data() + text.size()) {
        return ParseError::invalid_number;
    }

    // Son güvence: sonuç sonlu bir sayı olmalı.
    return std::isfinite(value)
        ? ParseError::none
        : ParseError::non_finite;
}

// Hata dönüşlerini tek biçimde oluşturur.
// std::nullopt: sonuçta TelemetrySample bulunmadığını belirtir.
ParseResult failure(
    ParseError error,
    std::size_t field_index = 0
) noexcept {
    return {std::nullopt, error, field_index};
}

} // namespace

ParseResult parse_telemetry_v0(
    std::string_view datagram
) noexcept {
    // 1. Datagram boyutunu kontrol et.
    if (datagram.empty()) {
        return failure(ParseError::empty_datagram);
    }

    if (datagram.size() > max_datagram_bytes) {
        return failure(ParseError::datagram_too_large);
    }

    // 2. İzin verilmeyen byte'ları reddet.
    // TAB, CR, LF ve yazdırılabilir ASCII karakterleri geçebilir.
    for (const unsigned char c : datagram) {
        if (c != '\t' && c != '\r' && c != '\n' &&
            (c < 32 || c > 126)) {
            return failure(ParseError::invalid_byte);
        }
    }

    // 3. Satır sonunda LF bulunmalı.
    if (datagram.back() != '\n') {
        return failure(ParseError::missing_line_ending);
    }

    // Son LF'yi yalnızca görünümden çıkarır; kaynak veri değişmez.
    datagram.remove_suffix(1);

    // CRLF biçimi de kabul edilir.
    if (!datagram.empty() && datagram.back() == '\r') {
        datagram.remove_suffix(1);
    }

    // Kalan gövdede başka CR/LF varsa tek kayıt şartı bozulmuştur.
    if (datagram.find_first_of("\r\n") != std::string_view::npos) {
        return failure(ParseError::invalid_framing);
    }

    // 4. Gövdeyi TAB ayraçlarından beş alan görünümüne böl.
    std::array<std::string_view, 5> fields{};
    std::size_t count = 0;
    std::size_t start = 0;

    while (true) {
        // Beş alan doldurulduktan sonra başka alan kabul edilmez.
        if (count == fields.size()) {
            return failure(ParseError::field_count);
        }

        const auto tab = datagram.find('\t', start);

        fields[count++] = tab == std::string_view::npos
            ? datagram.substr(start)
            : datagram.substr(start, tab - start);

        // Başka TAB yoksa son alan da okunmuştur.
        if (tab == std::string_view::npos) {
            break;
        }

        start = tab + 1;
    }

    if (count != fields.size()) {
        return failure(ParseError::field_count);
    }

    // 5. Alanları sırayla sayıya dönüştür.
    std::array<double, 5> values{};

    for (std::size_t i = 0; i < fields.size(); ++i) {
        const auto error = parse_number(fields[i], values[i]);

        if (error != ParseError::none) {
            // Diziler 0'dan başlar; dışarıdaki alan numarası 1'den başlar.
            return failure(error, i + 1);
        }
    }

    // Bütün kontroller başarılı: tam ham kaydı döndür.
    // Sıra, XML ve TelemetrySample alan sırasıyla aynıdır.
    return {
        TelemetrySample{
            values[0],
            values[1],
            values[2],
            values[3],
            values[4]
        },
        ParseError::none,
        0
    };
}

// Hata kategorilerini okunabilir sabit metinlere dönüştürür.
std::string_view parse_error_name(ParseError error) noexcept {
    switch (error) {
    case ParseError::none:
        return "none";
    case ParseError::empty_datagram:
        return "empty_datagram";
    case ParseError::datagram_too_large:
        return "datagram_too_large";
    case ParseError::invalid_byte:
        return "invalid_byte";
    case ParseError::missing_line_ending:
        return "missing_line_ending";
    case ParseError::invalid_framing:
        return "invalid_framing";
    case ParseError::field_count:
        return "field_count";
    case ParseError::empty_field:
        return "empty_field";
    case ParseError::invalid_number:
        return "invalid_number";
    case ParseError::non_finite:
        return "non_finite";
    case ParseError::numeric_range:
        return "numeric_range";
    }

    return "unknown";
}

} // namespace f16