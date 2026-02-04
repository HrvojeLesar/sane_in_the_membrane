#ifndef UI_IMAGE_VIEW
#define UI_IMAGE_VIEW

#include <grpcpp/support/status.h>
#include <memory>
#include <qboxlayout.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qimage.h>
#include <qlabel.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <vector>
#include <QScrollArea>
#include <QPushButton>
#include "../Utils/File.hpp"
#include "../Utils/ScannerUtils.hpp"

namespace sane_in_the_membrane::ui {
    class CImageItem : public QWidget {
        Q_OBJECT
      public:
        CImageItem(std::shared_ptr<utils::CFile> file, QWidget* parent = nullptr);
        ~CImageItem();

      signals:
        void sig_remove_requested();

      private slots:
        void sl_remove_me();

      private:
        void set_pixmap();
        void resizeEvent(QResizeEvent* event) override;

      private:
        std::shared_ptr<utils::CFile> m_file;
        QVBoxLayout* const            m_layout;
        QLabel* const                 m_image_label;
        QPixmap                       m_pixmap;
        QPushButton* const            m_close_button;
    };

    class CImageView : public QWidget {
        Q_OBJECT
      public:
        explicit CImageView(std::string filepath = "", QWidget* parent = nullptr);
        void add_image(std::shared_ptr<utils::CFile> file);

      private slots:
        void sl_sig_done(const std::shared_ptr<grpc::Status> status, std::shared_ptr<utils::CFile> file, std::shared_ptr<utils::ScannerParameters> params);

      private:
        QVBoxLayout* const       m_main_layout;
        QWidget* const           m_image_container;
        QHBoxLayout* const       m_grid;
        QScrollArea* const       m_scroll;

        std::vector<CImageItem*> m_items{};
    };
}

#endif // !UI_IMAGE_VIEW
