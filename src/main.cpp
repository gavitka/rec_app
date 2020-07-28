#include "pch.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QSettings>

#include "backend.h"
#include "kheventfilter.h"
#include "applistmodel.h"
#include "blwindow.h"

BLWindow* wnd;

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
    QCoreApplication::setApplicationName("Time lapse rec");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.addImportPath(":/imports");


    qmlRegisterType<BLWindow>("kh.components", 1, 0, "BLWindow");
    qmlRegisterSingletonType<BackEnd>("kh.components", 1, 0, "BackEnd", &BackEnd::qmlInstance);
    qmlRegisterType<AppListModel>("kh.components", 1, 0, "AppListModel");

    engine.addImageProvider("previewprovider", new PreviewImageProvider);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    auto obj = engine.rootObjects().at(0);
    if( !(wnd = qobject_cast<BLWindow*>(obj) ))
        return -1;

    app.installNativeEventFilter(new KhEventFilter());

    return app.exec();
}
