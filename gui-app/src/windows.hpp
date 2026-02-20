#ifdef WINDOWS_QT_PLUGINS // Should be set only when statically linking on windows

#ifdef _WIN32
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

#endif // !WINDOWS_QT_PLUGINS
