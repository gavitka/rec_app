#pragma once

#include "pch.h"

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QDebug>
#include "stdio.h"
#include "backend.h"

#define WM_KEYSTROKE (WM_USER + 101)

class KhEventFilter : public QAbstractNativeEventFilter
{

public:
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE
    {
        Q_UNUSED(eventType)
        //POINTS ptsEnd;
        MSG* msg = (MSG*)(message);
        if(msg->message == WM_KEYSTROKE) {
            POINT mpt = msg->pt;
            BackEnd::getInstance()->setMouseX(mpt.x);
            BackEnd::getInstance()->setMouseY(mpt.y);
            BackEnd::getInstance()->kick();
        }
        return false;
    }

};
