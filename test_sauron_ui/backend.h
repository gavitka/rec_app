#pragma once

#include <QObject>

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString outputText READ outputText WRITE setOutputText NOTIFY outputTextChanged)

public:
    explicit BackEnd(QObject *parent = nullptr) : QObject(parent) {

    }

    QString outputText() {return m_output_text;}
    void setOutputText(QString s);
    
signals:
    void outputTextChanged();

public slots:

    void startRecording();
    void stopRecording();

private:
    QString m_output_text;

    void addOutPutText(QString text) {
        setOutputText(outputText() + text);
    }
};
