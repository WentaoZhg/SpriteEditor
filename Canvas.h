#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QMouseEvent>
#include <QMessageBox>
#include <QKeyEvent>
#include <QPainter>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QJsonArray>
#include <QByteArray>
#include <QtWidgets>
#include <QKeyEvent>
#include <QPainter>
#include <QBuffer>
#include <QFile>
#include <QDir>

class Canvas : public QWidget
{
    Q_OBJECT

public:
    // @brief
    Canvas(QWidget *parent = nullptr);
    ~Canvas();

    // @brief
    // @param width
    // @param height
    void setSpriteSize(int width, int height);

    // @brief
    // @param &color
    void setColor(const QColor &color);

    // @brief
    // @param erase
    void setEraseMode(bool erase);

    // @brief
    int getSpriteWidth() const;

    // @brief
    int getSpriteHeight() const;

    // @brief
    void addFrame();

    // @brief
    void deleteFrame();

    // @brief
    int getFrameCount() const;

    // @brief
    int getCurrentFrameIndex() const;

    // @brief
    // @param index
    void setCurrentFrame(int index);

    // @brief
    // @param mirror
    void setMirrorMode(bool mirror) { mirrorMode = mirror; }

    //@brief
    void copyPrevious();
protected:
    // @brief
    void paintEvent(QPaintEvent *event) override;

    // @brief
    void mousePressEvent(QMouseEvent *event) override;

    // @brief
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QImage spriteImage;
    QColor currentColor;
    bool eraseMode;
    int spriteWidth;
    int spriteHeight;
    QList<QImage> frames; // List of frames
    int currentFrameIndex;

    // @brief
    QByteArray imageToBase64(const QImage &image);

    // @brief
    QImage base64ToImage(const QString &base64String);

    // @brief
    // @param x
    // @param y
    void drawPixel(int x, int y);

    // @brief
    // @param x
    // @param y
    void erasePixel(int x, int y);

    // @brief
    bool mirrorMode;

    // @brief
    int playBackSpeed = 100;

    QTimer *previewTimer;

    bool previewing = false;

    int canvasXScale = 16;
    int canvasYScale = 16;

public slots:
    // @brief
    void saveProject();

    // @brief
    void loadProject();

    // @brief
    void rotateSprites();

    // @brief
    void previewFrames();

    // @brief
    void setPlayBackSpeed(int speed);

    void togglePreview();

    void newProject();
};

#endif // CANVAS_H
