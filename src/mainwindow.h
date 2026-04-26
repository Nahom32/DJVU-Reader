#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QSlider>
#include <QLabel>
#include <QPointer>

class PageRenderer;
class DjvuView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void openFile();
    void closeDocument();
    void zoomIn();
    void zoomOut();
    void zoomActual();
    void fitToWidth();
    void fitToPage();
    void goToPage();
    void firstPage();
    void previousPage();
    void nextPage();
    void lastPage();
    void onPageSliderChanged(int value);
    void updateActions();

private:
    void createActions();
    void createToolbar();
    void createStatusBar();
    void updateWindowTitle();
    void loadDocument(const QString& filePath);

    PageRenderer* m_renderer;
    DjvuView* m_djvuView;

    QAction* m_openAction;
    QAction* m_closeAction;
    QAction* m_exitAction;
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
    QAction* m_zoomActualAction;
    QAction* m_fitWidthAction;
    QAction* m_fitPageAction;
    QAction* m_goToPageAction;
    QAction* m_firstPageAction;
    QAction* m_prevPageAction;
    QAction* m_nextPageAction;
    QAction* m_lastPageAction;

    QToolBar* m_toolbar;
    QSlider* m_pageSlider;
    QLabel* m_pageLabel;
    QLabel* m_zoomLabel;

    QAction* m_firstPageBtn;
    QAction* m_prevPageBtn;
    QAction* m_nextPageBtn;
    QAction* m_lastPageBtn;

    QString m_currentFile;
};

#endif // MAINWINDOW_H