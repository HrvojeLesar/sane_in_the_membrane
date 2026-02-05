#ifndef UTILS_PDF_PDF
#define UTILS_PDF_PDF

#include <exception>
#include <hpdf.h>
#include <hpdf_doc.h>
#include <hpdf_types.h>
#include <iostream>
#include <memory>
#include <optional>
#include "../ScannerUtils.hpp"

namespace sane_in_the_membrane::utils::pdf {
    class CPdf {
        friend class CPdfBuilder;

      private:
        CPdf(std::unique_ptr<HPDF_Doc> doc) : m_doc(std::move(doc)) {}

      public:
        ~CPdf() {
            HPDF_Free(*m_doc);
        }

        static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data) {
            std::cout << error_no << " detail: " << detail_no << "\n";
        }

        bool add_image(HPDF_Image image) {
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

        bool add_raw_image(const std::string& raw_image_path, sane_in_the_membrane::utils::ScannerParameters& params, sane_in_the_membrane::utils::SANE_Frame frame) {

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

        bool add_jpeg(const std::string& path) {
            HPDF_Image image = HPDF_LoadJpegImageFromFile(*m_doc, path.c_str());

            return add_image(image);
        }

        bool add_png(const std::string& path) {
            HPDF_Image image = HPDF_LoadPngImageFromFile2(*m_doc, path.c_str());

            return add_image(image);
        }

        HPDF_STATUS save(const std::string& path) {
            return HPDF_SaveToFile(*m_doc, path.c_str());
        }

      private:
        std::unique_ptr<HPDF_Doc> m_doc;
    };

    class CPdfBuilder {
      public:
        CPdf build() {
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

        CPdfBuilder* set_error_handler(HPDF_Error_Handler handler) {
            m_error_handler = handler;

            return this;
        }

        CPdfBuilder* set_compression_mode(HPDF_UINT mode) {
            m_compression_mode = mode;

            return this;
        }

        CPdfBuilder* set_user_data(void* user_data) {
            m_user_data = user_data;

            return this;
        }

      private:
        HPDF_Error_Handler       m_error_handler{nullptr};
        std::optional<HPDF_UINT> m_compression_mode{std::nullopt};
        void*                    m_user_data{nullptr};
    };

}

#endif // !UTILS_PDF_PDF
