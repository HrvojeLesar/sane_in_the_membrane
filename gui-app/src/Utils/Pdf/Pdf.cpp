#include "Pdf.hpp"
#include "hpdf_types.h"

#include <exception>
#include <format>
#include <GLogger.hpp>

using namespace sane_in_the_membrane::utils::pdf;

CPdf::CPdf(std::unique_ptr<HPDF_Doc> doc) : m_doc(std::move(doc)) {}

CPdf::~CPdf() {
    HPDF_Free(*m_doc);
}

void CPdf::error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data) {
    log::error("HPDF failed with errno: {} and detail_no: {}", error_no, detail_no);
}

bool CPdf::add_image(HPDF_Image image) {
    HPDF_Page page = HPDF_AddPage(*m_doc);
    if (page == nullptr) {
        return false;
    }

    if (image == nullptr) {
        return false;
    }

    HPDF_REAL img_width  = HPDF_Image_GetWidth(image);
    HPDF_REAL img_height = HPDF_Image_GetHeight(image);

    HPDF_Page_SetWidth(page, img_width);
    HPDF_Page_SetHeight(page, img_height);

    HPDF_Page_DrawImage(page, image, 0, 0, img_width, img_height);

    return true;
}

bool CPdf::add_raw_image(const std::string& raw_image_path, sane_in_the_membrane::utils::ScannerParameters& params, sane_in_the_membrane::utils::SANE_Frame frame) {

    HPDF_ColorSpace color_space = HPDF_ColorSpace::HPDF_CS_CAL_RGB;
    switch (frame) {
        case SANE_FRAME_GRAY: color_space = HPDF_ColorSpace::HPDF_CS_DEVICE_GRAY; break;
        case SANE_FRAME_RGB:
        case SANE_FRAME_RED:
        case SANE_FRAME_GREEN:
        case SANE_FRAME_BLUE: break;
    }

    HPDF_Image image = HPDF_LoadRawImageFromFile(*m_doc, raw_image_path.c_str(), params.width(), params.height(), color_space);

    return add_image(image);
}

bool CPdf::add_jpeg(const std::string& path) {
    HPDF_Image image = HPDF_LoadJpegImageFromFile(*m_doc, path.c_str());

    return add_image(image);
}

bool CPdf::add_png(const std::string& path) {
    HPDF_Image image = HPDF_LoadPngImageFromFile2(*m_doc, path.c_str());

    return add_image(image);
}

HPDF_STATUS CPdf::save(const std::string& path) {
    return HPDF_SaveToFile(*m_doc, path.c_str());
}

CPdf CPdfBuilder::build() {
    HPDF_Doc pdf = HPDF_New(m_error_handler, m_user_data);

    if (pdf == nullptr) {
        std::cerr << "Failed to create pdf object.";
        std::terminate();
    }

    if (m_compression_mode.has_value()) {
        HPDF_SetCompressionMode(pdf, *m_compression_mode);
    }

    return CPdf{std::make_unique<HPDF_Doc>(pdf)};
}

CPdfBuilder* CPdfBuilder::set_error_handler(HPDF_Error_Handler handler) {
    m_error_handler = handler;

    return this;
}

CPdfBuilder* CPdfBuilder::set_compression_mode(HPDF_UINT mode) {
    m_compression_mode = mode;

    return this;
}

CPdfBuilder* CPdfBuilder::set_user_data(void* user_data) {
    m_user_data = user_data;

    return this;
}

CPdf CPdfBuilder::build_default(HPDF_Error_Handler handler) {
    HPDF_Doc pdf = HPDF_New(handler, nullptr);

    if (pdf == nullptr) {
        std::cerr << "Failed to create pdf object.";
        std::terminate();
    }

    HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

    return CPdf{std::make_unique<HPDF_Doc>(pdf)};
}
