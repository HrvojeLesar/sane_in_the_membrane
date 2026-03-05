#ifndef UI_MAIN_WINDOW
#define UI_MAIN_WINDOW

#include <QMainWindow>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <qboxlayout.h>
#include <qevent.h>
#include <qformlayout.h>
#include <qgroupbox.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "Scan/ScanWidget.hpp"
#include "Select/ScannerSelectWidget.hpp"
#include "ImageView.hpp"
#include "Select/ScannerSelectWidget.hpp"
#include "SemaphoreWidget.hpp"
#include <QMessageBox>

namespace sane_in_the_membrane::ui {

    class CMainWindow : public QMainWindow {
        Q_OBJECT

      public:
        CMainWindow();
        ~CMainWindow();

        void closeEvent(QCloseEvent* event) override;

      public slots:
        void sl_documents_changed(std::size_t item_count);
        void sl_document_saved();

      private:
        QWidget* const                              m_central_widget;
        QVBoxLayout* const                          m_main_layout;
        QGroupBox* const                            m_group_box;

        sane_in_the_membrane::ui::CImageView* const m_image_view;
        select::CScannerSelectWidget* const         m_scanner_select_widget;

        CSemaphoreWidget* const                     m_server_status;
        scan::CScanWidget* const                    m_scan_widget;

        QFormLayout* const                          m_scanner_form_layout;
        QGroupBox* const                            m_scanner_section_group;
        QFormLayout* const                          m_images_form_layout;
        QGroupBox* const                            m_images_section_group;
        QFormLayout* const                          m_other_form_layout;
    };
}

#endif // !#ifndef UI_MAIN_WINDOW
