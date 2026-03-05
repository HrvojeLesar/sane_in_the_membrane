#ifndef UI_IMAGE_VIEW
#define UI_IMAGE_VIEW

#include <cstddef>
#include <cstdint>
#include <grpcpp/support/status.h>
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qimage.h>
#include <qlabel.h>
#include <QCheckBox>
#include <qlist.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qtmetamacros.h>
#include <qtransform.h>
#include <qwidget.h>
#include <QScrollArea>
#include <QPushButton>
#include "../Utils/File.hpp"
#include "../Utils/ScannerUtils.hpp"
#include "Image/ImageToolbar.hpp"
#include <SynchronizedAccess.hpp>
#include "Image/ImageHorizontalScroll.hpp"
#include "../Ocr/OcrMyPdfProcess.hpp"

namespace sane_in_the_membrane::ui {
    enum EMovePage : uint8_t {
        LEFT,
        RIGHT
    };

    class CImageItem : public QWidget {
        Q_OBJECT
      public:
        CImageItem(std::shared_ptr<utils::CFile>& file, std::size_t page_number, QWidget* parent);
        ~CImageItem();

        std::shared_ptr<utils::CFile>        file() const;
        const std::shared_ptr<utils::CFile>& file_ref() const;

      public:
        std::size_t get_page_number();
        void        set_page_number(std::size_t page_number);

      signals:
        void sig_remove_requested();
        void sig_reposition();
        void sig_move_page_by(CImageItem* item, EMovePage move_to);

      private slots:
        void sl_remove_me();

      private:
        void set_pixmap();
        void resizeEvent(QResizeEvent* event) override;

      private:
        std::shared_ptr<utils::CFile>   m_file;
        QVBoxLayout* const              m_layout;
        QLabel* const                   m_image_label;
        QPixmap                         m_pixmap;
        QTransform                      m_transform{};
        ui::image::CImageToolbar* const m_toolbar;
        bool                            m_preview_disabled{false};
        std::size_t                     m_page_number;
    };

    class CImageView : public QWidget {
        Q_OBJECT

      private:
        const size_t IMAGE_QUALITY = 85;

      public:
        explicit CImageView(std::string filepath = "", QWidget* parent = nullptr);
        void add_image(std::shared_ptr<utils::CFile>& file);
        void remove_item(CImageItem* item);


      private slots:
        void sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params);
        void sl_save_pdf();

      private:
        QList<CImageItem*> image_items() const;

      private:
        QVBoxLayout* const                   m_main_layout;
        QWidget* const                       m_image_container;
        QHBoxLayout* const                   m_grid;
        image::CImageHorizontalScroll* const m_scroll;
        QPushButton* const                   m_save;
#ifdef OCR
        QCheckBox* const      m_ocr_checkbox;
        ocr::COcrMyPdfProcess m_ocr_processor{};
#endif
    };
}

#endif // !UI_IMAGE_VIEW
