#pragma once

#include <QObject>
#include <QQmlEngine>
#include <thread>

#include"capturethread.h"

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString outputText READ outputText WRITE setOutputText NOTIFY outputTextChanged)
    Q_PROPERTY(bool stopEnabled READ stopEnabled NOTIFY stopEnabledChanged)
    Q_PROPERTY(QString startButtonText READ startButtonText NOTIFY startButtonTextChanged)

public:

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        if(!m_instance) {
            m_instance = new BackEnd();
        }
        return m_instance;
    }

    BackEnd(QObject *parent = nullptr) :
        QObject(parent),
        m_thr(nullptr)
    {
        refreshUI();
    }

    QString outputText() {return m_output_text;}
    void setOutputText(QString s);

    bool stopEnabled();

    QString startButtonText();
    
signals:

    void outputTextChanged();
    void stopEnabledChanged();
    void startButtonTextChanged();

public slots:

    void startRecording();
    void stopRecording();
    void handleResults();
    void refreshUI();

private:

    static QObject* m_instance;
    QString m_output_text;
    CaptureThread* m_thr;

    void addOutPutText(QString text) {
        setOutputText(outputText() + text);
    }

};
