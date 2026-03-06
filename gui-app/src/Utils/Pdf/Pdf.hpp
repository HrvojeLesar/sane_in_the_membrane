#ifndef UTILS_PDF_PDF
#define UTILS_PDF_PDF

#include <hpdf.h>
#include <hpdf_doc.h>
#include <hpdf_types.h>
#include <memory>
#include <optional>
#include "../ScannerUtils.hpp"

namespace sane_in_the_membrane::utils::pdf {
    struct SPdfMatrix {
        HPDF_REAL a;
        HPDF_REAL b;
        HPDF_REAL c;
        HPDF_REAL d;
        HPDF_REAL x;
        HPDF_REAL y;
    };

    class CPdf {
        friend class CPdfBuilder;

      private:
        CPdf(std::unique_ptr<HPDF_Doc> doc);

      public:
        ~CPdf();

        static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data);

        bool        add_image(HPDF_Image image, SPdfMatrix* matrix);
        bool        add_raw_image(const std::string& raw_image_path, sane_in_the_membrane::utils::ScannerParameters& params, sane_in_the_membrane::utils::SANE_Frame frame);
        bool        add_jpeg(const std::string& path, SPdfMatrix* transform_matrix = nullptr);
        bool        add_png(const std::string& path, SPdfMatrix* transform_matrix = nullptr);

        template <typename Iterator>
        bool add_jpegs(Iterator begin, Iterator end) {
            for (auto it = begin; it != end; ++it) {
                auto pair = *it;
                if (!add_jpeg(pair.first, &pair.second))
                    return false;
            }

            return true;
        }

        HPDF_STATUS save(const std::string& path);

      private:
        std::unique_ptr<HPDF_Doc> m_doc;
    };

    class CPdfBuilder {
      public:
        CPdf         build();
        CPdfBuilder* set_error_handler(HPDF_Error_Handler handler);
        CPdfBuilder* set_compression_mode(HPDF_UINT mode);
        CPdfBuilder* set_user_data(void* user_data);
        static CPdf  build_default(HPDF_Error_Handler handler = nullptr);

      private:
        HPDF_Error_Handler       m_error_handler{nullptr};
        std::optional<HPDF_UINT> m_compression_mode{std::nullopt};
        void*                    m_user_data{nullptr};
    };

}

#endif // !UTILS_PDF_PDF
