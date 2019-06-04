#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include "pch.h"
#include "backend.h"

QWindow* windowRef;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<BackEnd>("io.qt.examples.backend", 1, 0, "BackEnd");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    if( !(windowRef = qobject_cast<QWindow*>( engine.rootObjects().at(0)) ))
        return -1;

    return app.exec();
}
