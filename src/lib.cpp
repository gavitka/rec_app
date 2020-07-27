#include "lib.h"
#include <string.h>
#include <stdio.h>

QString int00(int i)
{
    QString s;
    s = QString::number(i);
    if(s.length() < 2) {
        s = QString("0") + s.right(1);
        return s;
    } else {
        return s;
    }
}

QString format_time(qint64 t)
{
    int sec, min, hour;
    sec = floor(t / 1000);
    min = floor(sec / 60);
    sec = sec % 60;
    hour = floor(min / 60);
    min = min % 60;
    return  QString("%1:%2:%3").arg(int00(hour)).arg(int00(min)).arg(int00(sec));
}

const char* C_string(QString string) {
    QByteArray ba;
    ba = string.toLocal8Bit();
    return ba.constData();
}
