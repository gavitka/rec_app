#pragma once

#include <QObject>

extern "C" {

#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

class QDataStream;

static void encode2(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
    QDataStream &out);

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString outputText READ outputText WRITE setOutputText NOTIFY outputTextChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);

    QString outputText();
    void setOutputText(QString);

    
signals:
    void outputTextChanged();

public slots:

    void startRecording();
    void stopRecording();

private:
    QString m_output_text;
    void addOutPutText(QString text);
};
