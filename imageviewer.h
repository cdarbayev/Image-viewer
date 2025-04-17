#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>

class QLabel;
class QScrollArea;
class QAction;
class QMenu;
class QToolBar;
class QCloseEvent;
class QDir;
class QFileInfo;
class QTimer;

class ImageViewer : public QMainWindow
{
    Q_OBJECT
    QLabel *lblImage;
    QScrollArea *scrollArea;
    QAction *actQuit;
    QMenu *mnuFile;

    QAction *actZoomIn;
    QAction *actZoomOut;
    QAction *actZoomOriginal;
    QAction *actFTW;//fit to window
    QMenu *mnuZoom;
    QMenu *mnuNavigation;
    QToolBar *toolBar;
    QAction *actOpen;
    void configureCentralWidget();
    void configureActions();
    unsigned int w, h;
    QString workDir;
    QAction *actNext;
    QAction *actPrev;
    QAction *actSlide;
    QStringList imageFiles;
    int index;
    QTimer *slideshowTimer;
    double zoomFactor;
    void loadSettings();
    void saveSettings();
    virtual void closeEvent(QCloseEvent *);
    void zoom(double factor);
    void toggleZoomActions(bool state);
    void loadImages(const QString &filePath);
    void showImage(const QString &filePath);
public:
    ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();
public slots:
    void open(); //actOpen
    void zoomIn();
    void zoomOriginal();
    void zoomOut();
    void fitToWindow();
    void nextImage();
    void prevImage();
    void slideshowMode();



};
#endif // IMAGEVIEWER_H
