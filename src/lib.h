#pragma once

#include <QString>
#include <QDebug>

QString int00(int i);

QString format_time(qint64 t);

const char* C_string(QString string);

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg);

BOOL getWindowInfo(HWND hwnd, QString& title, QString& exeName, bool& is64);
