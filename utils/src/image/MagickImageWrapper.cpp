#include "MagickImageWrapper.hpp"

void sane_in_the_membrane::utils::write_image(Magick::Image& image, const std::string& path) {
    image.write(path);
}

void sane_in_the_membrane::utils::write_image(const std::string& path, const int width, const int height, const void* data) {
    Magick::Image image{};
    image.read(width, height, "RGB", Magick::StorageType::CharPixel, data);

    write_image(image, path);
}
