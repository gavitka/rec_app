#include "pch.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QSettings>

#include "backend.h"
#include "kheventfilter.h"
#include "applistmodel.h"

QWindow* wnd;

int main(int argc, char *argv[])
{
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, L"Time lapse rec.0");
    if (!hMutex) {
        hMutex = CreateMutex(0, 0, L"Time lapse rec.0");
    } else {
        return 0;
    }

    QCoreApplication::setOrganizationName("Gavitka software");
    QCoreApplication::setOrganizationDomain("gavitka.com");
    QCoreApplication::setApplicationName("Diversity recording");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.addImportPath(":/imports");

    qmlRegisterSingletonType<BackEnd>("io.qt.examples.backend", 1, 0, "BackEnd", &BackEnd::qmlInstance);
    qmlRegisterType<AppListModel>("kh.components", 1, 0, "AppListModel");

    engine.addImageProvider("preview", new PreviewImageProvider);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if( !(wnd = qobject_cast<QWindow*>( engine.rootObjects().at(0)) ))
        return -1;

    // This fucks up the debug

    app.installNativeEventFilter(new KhEventFilter());

    return app.exec();
}
