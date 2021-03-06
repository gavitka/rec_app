#pragma once

#include "pch.h"

#include <QString>
#include <QDebug>

QString int00(int i);

QString format_time(qint64 t);

const char* C_string(QString string);

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg);

BOOL getWindowInfo(HWND hwnd, QString& title, QString& exeName, bool& is64);

QImage captureWindow(HWND hwnd);

QImage captureScreen();

void drawCursor(QPixmap pixmap, int offsetx = 0, int offsety = 0);

