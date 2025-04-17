#include "imageviewer.h"
#include <QLabel>
#include <QScrollArea>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QIcon>
#include <QToolBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QDir>
#include <QStringList>
#include <QTimer>
#include <QFileInfo>


ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent), w(1024), h(768), workDir(QDir::homePath()), index(-1), slideshowTimer(new QTimer(this))
{
    configureCentralWidget();
    loadSettings();
    configureActions();
    connect(slideshowTimer, &QTimer::timeout, this, &ImageViewer::nextImage);
}

void ImageViewer::loadSettings(){
    QSettings settings("FNSPE", "ImageViewer");
    workDir = settings.value("workDir", workDir).toString();
}

void ImageViewer::saveSettings(){
    QSettings settings("FNSPE", "ImageViewer");
    settings.setValue("workDir", workDir);
}

void ImageViewer::closeEvent(QCloseEvent *evt){
    saveSettings();
    if(QMessageBox::question(this, tr("Really quit?"), tr("Do you want to quit the viewer?"))
        == QMessageBox::StandardButton::Yes){
        evt->accept();
    }
    else
    {
        evt->ignore();
    }
}

void ImageViewer::configureCentralWidget(){
    lblImage = new QLabel;
    lblImage->setSizePolicy(QSizePolicy::Ignored,
                            QSizePolicy::Ignored);
    lblImage->setScaledContents(true);
    scrollArea = new QScrollArea;
    scrollArea->setWidget(lblImage);
    this->setCentralWidget(scrollArea);
    this->setWindowTitle(tr("Simple image viewer"));
    this->resize(w,  h);
}

void ImageViewer::configureActions(){
    QIcon iconOpen = QIcon::fromTheme("document-open", QIcon(":/folder.png"));
    actOpen = new QAction(tr("&Open..."));
    actOpen->setStatusTip(tr("Opens image..."));
    actOpen->setToolTip(tr("Opens image"));
    actOpen->setShortcut(QKeySequence::Open);
    actOpen->setIcon(iconOpen);

    QIcon iconQuit = QIcon::fromTheme("application-exit", QIcon(":/quit.png"));
    actQuit = new QAction(tr("&Quit"));
    actQuit->setStatusTip(tr("Closes the viewer", "status tip"));
    actQuit->setToolTip(tr("Closes the viewer", "tool tip"));
    actQuit->setShortcut(QKeySequence::Quit);
    actQuit->setIcon(iconQuit);

    mnuFile = new QMenu(tr("&File"));
    mnuFile->addAction(actOpen);
    mnuFile->addSeparator();
    mnuFile->addAction(actQuit);

    QIcon iconZoomIn = QIcon::fromTheme("zoom-in");
    actZoomIn = new QAction(tr("Zoom &in"));
    actZoomIn->setStatusTip(tr("Zooms image in"));
    actZoomIn->setToolTip(tr("Zooms image in"));
    actZoomIn->setShortcut(QKeySequence::ZoomIn);
    actZoomIn->setIcon(iconZoomIn);

    QIcon iconZoomOriginal = QIcon::fromTheme("zoom-original", QIcon(":/zoom-out.png"));
    actZoomOriginal = new QAction(tr("Original size"));
    actZoomOriginal->setStatusTip(tr("Resets zoom to original size"));
    actZoomOriginal->setToolTip(tr("Resets zoom to original size"));
    actZoomOriginal->setIcon(iconZoomOriginal);

    QIcon iconZoomOut = QIcon::fromTheme("zoom-out");
    actZoomOut = new QAction(tr("Zoom &out"));
    actZoomOut->setStatusTip(tr("Zooms image out"));
    actZoomOut->setToolTip(tr("Zooms image out"));
    actZoomOut->setShortcut(QKeySequence::ZoomOut);
    actZoomOut->setIcon(iconZoomOut);

    QIcon iconFTW = QIcon::fromTheme("zoom-fit-best");
    actFTW = new QAction(tr("Fit to &Window"));
    actFTW->setStatusTip(tr("Fits image to window"));
    actFTW->setToolTip(tr("Fits image to window"));
    actFTW->setIcon(iconFTW);
    actFTW->setCheckable(true);
    actFTW->setChecked(false);
    actFTW->setEnabled(false);

    mnuZoom = new QMenu(tr("&Zoom"));
    mnuZoom->addAction(actZoomIn);
    mnuZoom->addAction(actZoomOriginal);
    mnuZoom->addAction(actZoomOut);
    mnuZoom->addSeparator();
    mnuZoom->addAction(actFTW);

    QIcon iconPrev = QIcon::fromTheme("go-previous");
    actPrev = new QAction(tr("&Previous image"));
    actPrev->setStatusTip(tr("Previous image"));
    actPrev->setToolTip(tr("Previous image"));
    actPrev->setShortcut(QKeySequence::MoveToPreviousChar);
    actPrev->setIcon(iconPrev);


    QIcon iconNext = QIcon::fromTheme("go-next");
    actNext = new QAction(tr("&Next image"));
    actNext->setStatusTip(tr("Next image"));
    actNext->setToolTip(tr("Next image"));
    actNext->setShortcut(QKeySequence::MoveToNextChar);
    actNext->setIcon(iconNext);


    QIcon iconSlide = QIcon::fromTheme("slideshow", QIcon(":/slideshow.png"));
    actSlide = new QAction(tr("&Slideshow mode"));
    actSlide->setStatusTip(tr("Slideshow mode"));
    actSlide->setToolTip(tr("Start slideshow"));
    actSlide->setIcon(iconSlide);
    actSlide->setCheckable(true);

    mnuNavigation = new QMenu(tr("&Navigation"));
    mnuNavigation->addAction(actPrev);
    mnuNavigation->addAction(actNext);
    mnuNavigation->addAction(actSlide);

    this->menuBar()->addMenu(mnuFile);
    this->menuBar()->addMenu(mnuZoom);
    this->menuBar()->addMenu(mnuNavigation);


    toolBar = this->addToolBar(tr("main toolbar"));
    toolBar->addAction(actOpen);
    toolBar->addAction(actQuit);
    toolBar->addSeparator();
    toolBar->addAction(actZoomIn);
    toolBar->addAction(actZoomOriginal);
    toolBar->addAction(actZoomOut);
    toolBar->addAction(actFTW);
    toolBar->addSeparator();
    toolBar->addAction(actPrev);
    toolBar->addAction(actNext);
    toolBar->addAction(actSlide);


    connect(actOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(actQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(actZoomOriginal, SIGNAL(triggered()), this, SLOT(zoomOriginal()));
    connect(actZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(actFTW, SIGNAL(triggered()), this, SLOT(fitToWindow()));
    toggleZoomActions(false);
    connect(actPrev, SIGNAL(triggered()), this, SLOT(prevImage()));
    connect(actNext, SIGNAL(triggered()), this, SLOT(nextImage()));
    connect(actSlide, SIGNAL(triggered()), this, SLOT(slideshowMode()));

    this->statusBar()->showMessage(tr("Ready"), 3000);
}

ImageViewer::~ImageViewer() {}

// void ImageViewer::open(){
//     QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"),
//                                                     workDir,
//                                                     tr("images (*.png *.jpg);;All files (*)"));
//     if(!fileName.isEmpty()){
//         QFileInfo info(fileName);
//         workDir = info.absoluteDir().absolutePath();
//         QImage image(fileName);
//         if(image.isNull()){
//             QMessageBox::warning(this, tr("Error"), tr("Failed to open image %1").arg(fileName));
//             return;
//         }
//         zoomFactor = 1.0;
//         lblImage->setPixmap(QPixmap::fromImage(image));
//         lblImage->adjustSize();
//         this->statusBar()->showMessage(tr("Image was loaded"), 2000);
//         toggleZoomActions(true);
//         actFTW->setEnabled(true);
//     }
// }
void ImageViewer::open(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), workDir, tr("Images (*.png *.jpg);;All files (*)"));
    if(!fileName.isEmpty()){
        loadImages(fileName);
        showImage(fileName);
    }
}

void ImageViewer::loadImages(const QString &filePath){
    QFileInfo fileInfo(filePath);
    workDir = fileInfo.absoluteDir().absolutePath();
    QDir dir(workDir);
    QStringList images = {"*.png", "*.jpg"};
    imageFiles = dir.entryList(images, QDir::Files);
    index = imageFiles.indexOf(fileInfo.fileName());
}

void ImageViewer::showImage(const QString &filePath){
    if(index < 0 || index >= imageFiles.size()){
        QMessageBox::warning(this, tr("Error"), tr("Index is wrong %1"));
        return;
    }
    QImage image(filePath);
    if(image.isNull()){
        QMessageBox::warning(this, tr("Error"), tr("Failed to open image %1").arg(filePath));
        return;
    }
    zoomFactor = 1.0;
    lblImage->setPixmap(QPixmap::fromImage(image));
    lblImage->adjustSize();
    this->statusBar()->showMessage(tr("Image: %1").arg(filePath), 2000);
    toggleZoomActions(true);
    actFTW->setEnabled(true);
}

void ImageViewer::prevImage(){
    if(index > 0){
        --index;
        showImage(QDir(workDir).filePath(imageFiles[index]));
    }
}

void ImageViewer::nextImage(){
    if(index < imageFiles.size() - 1){
        ++index;
        showImage(QDir(workDir).filePath(imageFiles[index]));
    }
}

void ImageViewer::slideshowMode(){
    if(actSlide->isChecked()){
        slideshowTimer->start(2000);
    } else {
        slideshowTimer->stop();
    }
}
void ImageViewer::fitToWindow(){
    bool fit = actFTW->isChecked();
    toggleZoomActions(!fit);
    scrollArea->setWidgetResizable(fit);
    if(!fit){
        lblImage->adjustSize();
        zoomOriginal();
    }

}

void ImageViewer::toggleZoomActions(bool state){
    actZoomIn->setEnabled(state);
    actZoomOut->setEnabled(state);
    actZoomOriginal->setEnabled(state);
    actFTW->setEnabled(state);
}

void ImageViewer::zoomIn(){
    zoom(1.2);
}

void ImageViewer::zoomOriginal(){
    zoomFactor = 1.0;
    zoom(1);
}

void ImageViewer::zoomOut(){
    zoom(0.8);
}

void ImageViewer::zoom(double factor){
    zoomFactor = factor*zoomFactor;
    lblImage->resize(lblImage->pixmap(Qt::ReturnByValue).size()*zoomFactor);
    actZoomIn->setEnabled(zoomFactor < 5);
    actZoomOut->setEnabled(zoomFactor > 0.1);
}
