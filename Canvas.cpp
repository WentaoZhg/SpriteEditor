#include "Canvas.h"

Canvas::Canvas(QWidget *parent)
    : QWidget(parent),
    eraseMode(false),
    spriteWidth(512),
    spriteHeight(512),
    currentFrameIndex(0),
    previewTimer(new QTimer(this))
{
    QImage initialFrame(spriteWidth, spriteHeight, QImage::Format_ARGB32);
    initialFrame.fill(Qt::transparent);
    frames.append(initialFrame);
    spriteImage = initialFrame;
    currentColor = Qt::black;
    mirrorMode = false;

    connect(previewTimer, &QTimer::timeout, this, [this]() {
        if (!frames.isEmpty()) {
            currentFrameIndex = (currentFrameIndex + 1) % frames.size();
            update();
        }
    });
}

Canvas::~Canvas() {}

void Canvas::setSpriteSize(int width, int height)
{
    spriteWidth = width * 512/width;
    spriteHeight = height * 512/height;
    canvasXScale = 512/width;
    canvasYScale = 512/width;
    spriteImage = QImage(spriteWidth, spriteHeight, QImage::Format_ARGB32);
    spriteImage.fill(Qt::transparent);
    update();
}

int Canvas::getSpriteWidth() const
{
    return (spriteWidth/canvasXScale);
}

int Canvas::getSpriteHeight() const
{
    return (spriteHeight/canvasYScale);
}

void Canvas::addFrame()
{
    QImage newFrame(spriteWidth, spriteHeight, QImage::Format_ARGB32);
    newFrame.fill(Qt::transparent); // Initialize with transparent pixels
    frames.append(newFrame);
    currentFrameIndex = frames.size() - 1; // Set to the new frame
    update();
}

void Canvas::deleteFrame()
{
    if (frames.size() > 1) { // Ensure at least one frame remains
        frames.removeLast();
        currentFrameIndex = qMin(currentFrameIndex, frames.size() - 1); // Adjust current frame index
        update();
    }
    else
    {
        frames.removeLast();
        currentFrameIndex = qMin(currentFrameIndex, frames.size() - 1);
        addFrame();
        update();
    }
}

int Canvas::getFrameCount() const
{
    return frames.size();
}

int Canvas::getCurrentFrameIndex() const
{
    return currentFrameIndex;
}

void Canvas::setCurrentFrame(int index)
{
    if (index >= 0 && index < frames.size()) {
        currentFrameIndex = index;
        update(); // Redraw to display the new frame
    }
}

void Canvas::saveProject()
{
    const QString& filename =
        QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath(), "Text Files (*.txt);;All Files (*)");
    QJsonArray imageArray;

    for (const QImage &image : frames) {
        imageArray.append(QString::fromLatin1(imageToBase64(image)));
    }

    // Adds array of paths to JSON object.
    QJsonObject jsonObject;
    jsonObject["images"] = imageArray;

    // Creates and saves JSON document.
    QJsonDocument jsonDoc(jsonObject);
    QFile project(filename);
    if (project.open(QIODevice::WriteOnly)) {
        project.write(jsonDoc.toJson());
        project.close();
    }
}

void Canvas::loadProject()
{
    const QString& filename =
        QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath(), "Text Files (*.txt);;All Files (*)");
    frames.clear();

    QFile project(filename);
    if (project.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = project.readAll();
        project.close();

        // Parse the JSON data
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
        if (jsonDoc.isObject()) {
            QJsonObject jsonObject = jsonDoc.object();
            if (jsonObject.contains("images") && jsonObject["images"].isArray()) {
                QJsonArray jsonArray = jsonObject["images"].toArray();

                // Loop through the base64 strings and convert them to QImage
                for (const QJsonValue &value : jsonArray) {
                    QString base64String = value.toString();
                    QImage image = base64ToImage(base64String);
                    if (!image.isNull()) {
                        frames.append(image);
                    }
                }
            }
        }
    }
}

void Canvas::newProject()
{
    int result = QMessageBox::question(this, "Question", "Would you like to save your current project?",
                    QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes)
    {
        saveProject();
        for (int i = 0; i <= frames.size() + 1; i++)
            deleteFrame();
    }
    else
    {
        for (int i = 0; i <= frames.size() + 1; i++)
            deleteFrame();
    }
}

void Canvas::rotateSprites()
{
    for(QImage &frame : frames) {
        QTransform transform;
        transform.rotate(90); // rotate 90 degrees
        frame = frame.transformed(transform); // apply rotation
    }
    update();
}

void Canvas::copyPrevious()
{
    if (currentFrameIndex > 0 && !frames.isEmpty()) {
        QImage previousFrame = frames[currentFrameIndex - 1];
        frames[currentFrameIndex] = previousFrame.copy(); // Copy the previous frame into the current frame
        update();
    }
}
void Canvas::previewFrames()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, timer]() {
        if (frames.isEmpty()) {
            timer->stop();
            return;
        }
        currentFrameIndex = (currentFrameIndex + 1) % frames.size();
        update();
    });
    timer->start(playBackSpeed);
}

void Canvas::setPlayBackSpeed(int speed) {
    playBackSpeed = 550 - speed;
    if (previewing) {
        previewTimer->start(playBackSpeed); // Update the timer interval if preview is active
    }
}

void Canvas::togglePreview() {
    previewing = !previewing;
    if (previewing) {
        previewTimer->start(playBackSpeed); // Start preview with current speed
    } else {
        previewTimer->stop(); // Stop preview
    }
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!frames.isEmpty()) {
        painter.drawImage(0, 0, frames[currentFrameIndex]);
    }
    QWidget::paintEvent(event);
}

void Canvas::setColor(const QColor &color)
{
    currentColor = color;
}

void Canvas::setEraseMode(bool erase)
{
    eraseMode = erase;
}

void Canvas::drawPixel(int x, int y)
{
    if (x >= 0 && x + canvasXScale <= spriteWidth && y >= 0 && y + canvasYScale <= spriteHeight) {
        if (eraseMode) {
            erasePixel(x, y);
        } else {
            QPainter painter(&frames[currentFrameIndex]);
            painter.setPen(Qt::NoPen);
            painter.setBrush(currentColor);
            painter.drawRect(x, y, canvasXScale, canvasYScale);

            if (mirrorMode) {
                int mirroredX = spriteWidth - x - canvasXScale;
                int mirroredY = spriteHeight - y - canvasYScale;

                painter.drawRect(mirroredX, y, canvasXScale, canvasYScale);
                painter.drawRect(x, mirroredY, canvasXScale, canvasYScale);
                painter.drawRect(mirroredX, mirroredY, canvasXScale, canvasYScale);
            }
        }
        update();
    }
}

void Canvas::erasePixel(int x, int y)
{
    if (x >= 0 && x < spriteWidth && y >= 0 && y < spriteHeight) {
        QPainter painter(&frames[currentFrameIndex]);
        painter.setCompositionMode(QPainter::CompositionMode_Clear); // Set the composition mode to clear
        painter.fillRect(x, y, canvasXScale, canvasYScale, Qt::transparent);
        update();
    }
}
void Canvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int x = (event->pos().x() / canvasXScale) * canvasXScale;
        int y = (event->pos().y() / canvasYScale) * canvasYScale;

        drawPixel(x, y);
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int x = (event->pos().x() / canvasXScale) * canvasXScale;
        int y = (event->pos().y() / canvasYScale) * canvasYScale;

        drawPixel(x, y);
    }
}

// Helper methods to convert images to base 64 and vice versa, in order to save and load.
QByteArray Canvas::imageToBase64(const QImage &image) {
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG"); // or another suitable image format
    return byteArray.toBase64();
}

QImage Canvas::base64ToImage(const QString &base64String) {
    QByteArray byteArray = QByteArray::fromBase64(base64String.toUtf8());
    QImage image;
    image.loadFromData(byteArray);
    return image;
}
