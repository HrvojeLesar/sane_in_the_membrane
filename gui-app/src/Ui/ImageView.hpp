#ifndef UI_IMAGE_VIEW
#define UI_IMAGE_VIEW

#include <cstddef>
#include <cstdint>
#include <grpcpp/support/status.h>
#include <memory>
#include <optional>
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
#include <qtypes.h>
#include <qwidget.h>
#include <QScrollArea>
#include <QPushButton>
#include "Utils/File.hpp"
#include "Utils/ScannerUtils.hpp"
#include "Image/ImageToolbar.hpp"
#include <SynchronizedAccess.hpp>
#include <vector>
#include "Image/ImageHorizontalScroll.hpp"
#include "Ocr/OcrMyPdfProcess.hpp"
#include "Service/SessionService.hpp"

namespace sane_in_the_membrane::ui {
    enum EMoveDirection : uint8_t {
        LEFT,
        RIGHT
    };

    class CImageItemContainer;
    class CImageItemManager;

    class CImageItem : public QWidget {
        Q_OBJECT

        friend class CImageItemContainer;

      public:
        struct SQtMatrix {
            SQtMatrix();
            SQtMatrix(QTransform& transform);

            qreal m11;
            qreal m12;
            qreal m13;
            qreal m21;
            qreal m22;
            qreal m23;
            qreal m31;
            qreal m32;
            qreal m33;
        };

        struct SImageItemSerialized {
            SImageItemSerialized();
            SImageItemSerialized(CImageItem& item);

            SQtMatrix                                  transform_matrix;
            std::size_t                                page_number;
            std::string                                path;

            service::Bytes                             serialize() const;
            static std::optional<SImageItemSerialized> deserialize(const unsigned char* start, const unsigned char* end);
            std::size_t                                size() const;
        };

      public:
        ~CImageItem();

      private:
        CImageItem(std::shared_ptr<utils::CFile>& file, std::size_t page_number, QWidget* parent);
        CImageItem(SImageItemSerialized& serialized_item, QWidget* parent);

      private:
        void setup_layout_and_connections();

      public:
        std::shared_ptr<utils::CFile>        file() const;
        const std::shared_ptr<utils::CFile>& file_ref() const;

      public:
        std::size_t get_page_number();
        void        set_page_number(std::size_t page_number);

      signals:
        void sig_remove_requested(CImageItem* item);
        void sig_reposition();
        void sig_move_page_by(CImageItem* item, EMoveDirection move_to);

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

    class CImageItemManager {
      public:
        ~CImageItemManager();

        // Takes ownership of item
        CImageItem*                     add_item(CImageItem* item);
        void                            remove_item(CImageItem* item);
        const std::vector<CImageItem*>& items() const;

      private:
        std::vector<CImageItem*> m_items;
    };

    class CImageItemContainer : public QHBoxLayout {
        Q_OBJECT

      public:
        CImageItemContainer(QWidget* parent);
        ~CImageItemContainer();

        CImageItem* add_image(std::shared_ptr<utils::CFile>& file, QWidget* parent);
        // Takes ownership of item
        CImageItem*                     add_image(CImageItem* item);
        CImageItem*                     add_image(CImageItem::SImageItemSerialized& serialized_item, QWidget* parent);
        void                            remove_image(CImageItem* item);
        void                            move_image(CImageItem* item, EMoveDirection direction);
        std::size_t                     image_count() const;
        const std::vector<CImageItem*>& items() const;

      private:
        CImageItemManager m_manager;
    };

    class CImageView : public QWidget {
        Q_OBJECT

      private:
        const size_t IMAGE_QUALITY = 85;

      public:
        explicit CImageView(std::string filepath = "", QWidget* parent = nullptr);
        void add_image(std::shared_ptr<utils::CFile>& file);

      signals:
        void sig_document_saved();
        void sig_document_changed(std::size_t item_count);

      private slots:
        void           sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params);
        void           sl_save_pdf();
        void           move_image(CImageItem* item, EMoveDirection direction);
        void           remove_image(CImageItem* item);
        service::Bytes serialize_items();
        void           deserialize_items(service::Bytes& data);

      private:
        QVBoxLayout* const                   m_main_layout;
        QWidget* const                       m_image_container;
        CImageItemContainer* const           m_grid;
        image::CImageHorizontalScroll* const m_scroll;
        QPushButton* const                   m_save;
        service::CSessionService             m_session;
#ifdef OCR
        QCheckBox* const      m_ocr_checkbox;
        ocr::COcrMyPdfProcess m_ocr_processor{};
#endif
    };
}

#endif // !UI_IMAGE_VIEW
