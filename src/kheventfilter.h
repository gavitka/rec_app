#pragma once

#include "pch.h"

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QDebug>
#include "stdio.h"
#include "backend.h"

#include "hooks_dll/mousehook.h"

class KhEventFilter : public QAbstractNativeEventFilter
{

public:
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE
    {
        Q_UNUSED(eventType)
        MSG* msg = (MSG*)(message);
        if(msg->message == WM_KEYSTROKE) {
            // Chrome is sending messages for some reason
            POINT mpt = msg->pt;
            //qDebug() << "eventType" << eventType;
            //qDebug() << "msg->message" << msg->message;
            BackEnd::getInstance()->setMouseX(mpt.x);
            BackEnd::getInstance()->setMouseY(mpt.y);
            BackEnd::getInstance()->kick();
        }
        return false;
    }

};
