#ifndef IMAGE_MAGICK_IMAGE_WRAPPER_HPP
#define IMAGE_MAGICK_IMAGE_WRAPPER_HPP

#include <string>
#include <Magick++/Image.h>

namespace sane_in_the_membrane::utils {

    void write_image(Magick::Image& image, const std::string& path);
    void write_image(const std::string& path, const int width, const int height, const void* data);

}

#endif // !IMAGE_MAGICK_IMAGE_WRAPPER_HPP
