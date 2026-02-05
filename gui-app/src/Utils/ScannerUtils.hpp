#ifndef UTILS_SCANNER_UTILS
#define UTILS_SCANNER_UTILS

#include <cstdint>
#include <scanner/v1/scanner.pb.h>

namespace sane_in_the_membrane::utils {
    typedef enum {
        SANE_FRAME_GRAY,
        SANE_FRAME_RGB,
        SANE_FRAME_RED,
        SANE_FRAME_GREEN,
        SANE_FRAME_BLUE
    } SANE_Frame;

    struct ScannerParameters {
        ScannerParameters() {}
        ScannerParameters(const scanner::v1::ScanParameters& scan) :
            format(scan.format()), last_frame(scan.last_frame()), bytes_per_line(scan.bytes_per_line()), pixels_per_line(scan.pixels_per_line()), lines(scan.lines()),
            depth(scan.depth()) {}

        int64_t format;
        bool    last_frame;
        int64_t bytes_per_line;
        int64_t pixels_per_line;
        int64_t lines;
        int64_t depth;

        int64_t width() {
            return pixels_per_line;
        }

        int64_t height() {
            return lines;
        }
    };

}

#endif // !UTILS_SCANNER_UTILS
