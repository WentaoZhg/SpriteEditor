#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canvas = new Canvas(ui->stackedWidget);
    ui->stackedWidget->addWidget(canvas);
    ui->stackedWidget->setCurrentWidget(canvas);

    QToolBar *toolbar = addToolBar("Tools");
    addToolBar(Qt::RightToolBarArea, toolbar); // Place toolbar on the right side
    // Adds file and edit menu to the menu bar.
    QMenu *editMenu = menuBar()->addMenu("Edit");
    QMenu *fileMenu = menuBar()->addMenu("File");

    // Adds options to the edit and file menu
    QAction *setColorAction = new QAction("Set Drawing Color", this);
    QAction *setSizeAction = new QAction("Set Sprite Size", this);

    QAction *addFrameAction = new QAction("Add Frame", this);
    QAction *deleteFrameAction = new QAction("Delete Frame", this);

    QAction *nextFrameAction = new QAction("Next Frame", this);
    QAction *prevFrameAction = new QAction("Previous Frame", this);

    QAction *saveAction = new QAction("Save", this);
    QAction *loadAction = new QAction("Load", this);
    QAction *newAction = new QAction("New", this);

    QAction *copyPreviousAction = new QAction("Copy Previous", this); // New action for copying the previous frame

    // Initializes rotate, preview, and mirror actions for the toolbar.
    QAction *toggleMirrorAction = new QAction("Toggle Mirror", this);
    QAction *rotateAction = new QAction("Rotate", this);
    QAction *previewAction = new QAction("Preview", this);
    QAction *toggleEraseAction = new QAction("Toggle Erase", this);
    // Adds menu and toolbar to the UI.

    editMenu->addAction(setSizeAction);
    editMenu->addAction(nextFrameAction);
    editMenu->addAction(prevFrameAction);

    fileMenu->addAction(saveAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(newAction);
    // Drawing tools
    toolbar->addWidget(new QLabel("Drawing Tools"));
    toolbar->addAction(toggleEraseAction);
    toolbar->addAction(setColorAction);
    toolbar->addSeparator();
    // Sprite features
    toolbar->addWidget(new QLabel("Features"));
    toolbar->addAction(toggleMirrorAction);
    toolbar->addAction(rotateAction);
    toolbar->addSeparator();
    // Frame management
    toolbar->addWidget(new QLabel("Frame Management"));
    toolbar->addAction(addFrameAction);
    toolbar->addAction(deleteFrameAction);
    toolbar->addAction(copyPreviousAction);
    toolbar->addSeparator();
    // Playback controls
    toolbar->addWidget(new QLabel("Playback Controls"));
    toolbar->addAction(previewAction);
    toolbar->addAction(prevFrameAction);
    toolbar->addAction(nextFrameAction);
    toolbar->addSeparator();


    // Adds color picker, mirror tool, and rotate tool functionality, respectively.
    connect(setColorAction, &QAction::triggered, this, [=]() {
        QColor color = QColorDialog::getColor(Qt::black, this);
        if (color.isValid()) {
            canvas->setColor(color);
        }
    });

    toggleEraseAction->setCheckable(true);
    connect(toggleEraseAction, &QAction::triggered, this, [=](bool checked) {
        canvas->setEraseMode(checked);
    });

    toggleMirrorAction->setCheckable(true);

    connect(toggleMirrorAction, &QAction::triggered, this, [=](bool checked) {
        canvas->setMirrorMode(checked);
    });

    connect(rotateAction, &QAction::triggered, canvas, &Canvas::rotateSprites);

    canvas->setSpriteSize(32, 32);

    connect(setSizeAction, &QAction::triggered, this, [=]() {
        bool ok;
        int width = QInputDialog::getInt(this, "Set Sprite Width",
                                         "Width:", canvas->getSpriteWidth(), 8, 512, 8, &ok);
        if (ok) {
            int height = QInputDialog::getInt(this, "Set Sprite Height",
                                              "Height:", canvas->getSpriteHeight(), 8, 512, 8, &ok);
            if (ok) {
                canvas->setSpriteSize(width, height);
            }
        }
    });

    connect(addFrameAction, &QAction::triggered, this, [=]() {
        canvas->addFrame();
    });

    connect(deleteFrameAction, &QAction::triggered, this, [=]() {
        canvas->deleteFrame();
    });

    connect(copyPreviousAction, &QAction::triggered, this, [=]() {
        if (canvas->getCurrentFrameIndex() > 0) {
            canvas->copyPrevious();
        }
    });

    connect(nextFrameAction, &QAction::triggered, this, [=]() {
        int nextIndex = canvas->getCurrentFrameIndex() + 1;
        if (nextIndex < canvas->getFrameCount()) {
            canvas->setCurrentFrame(nextIndex);
        }
    });

    connect(prevFrameAction, &QAction::triggered, this, [=]() {
        int prevIndex = canvas->getCurrentFrameIndex() - 1;
        if (prevIndex >= 0) {
            canvas->setCurrentFrame(prevIndex);
        }
    });

    QSlider *speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(50, 500);
    speedSlider->setValue(100);
    toolbar->addWidget(new QLabel("Speed:"));
    toolbar->addWidget(speedSlider);

    connect(speedSlider, &QSlider::valueChanged, canvas, &Canvas::setPlayBackSpeed);
    connect(saveAction, &QAction::triggered, canvas, &Canvas::saveProject);
    connect(loadAction, &QAction::triggered, canvas, &Canvas::loadProject);
    connect(previewAction, &QAction::triggered, canvas, &Canvas::togglePreview);
    connect(newAction, &QAction::triggered, canvas, &Canvas::newProject);
}

MainWindow::~MainWindow()
{
    delete ui;
}
