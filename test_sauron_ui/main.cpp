#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QSettings>

#include "pch.h"
#include "backend.h"
#include "kheventfilter.h"

QWindow* windowRef;

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Gavitka software");
    QCoreApplication::setOrganizationDomain("gavitka.com");
    QCoreApplication::setApplicationName("Diversity recording");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterSingletonType<BackEnd>("io.qt.examples.backend", 1, 0, "BackEnd", &BackEnd::qmlInstance);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    if( !(windowRef = qobject_cast<QWindow*>( engine.rootObjects().at(0)) ))
        return -1;

    // This fucks up the debug

    app.installNativeEventFilter(new KhEventFilter());

    return app.exec();
}
