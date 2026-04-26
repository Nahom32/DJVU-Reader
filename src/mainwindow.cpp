#include "mainwindow.h"
#include "pagerenderer.h"
#include "djvuview.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_renderer(new PageRenderer(this))
    , m_djvuView(new DjvuView(this))
{
    setCentralWidget(m_djvuView);
    m_djvuView->setPageRenderer(m_renderer);

    createActions();
    createToolbar();
    createStatusBar();
    updateActions();

    resize(800, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    m_openAction = new QAction(tr("&Open..."), this);
    m_openAction->setShortcut(QKeySequence::Open);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);

    m_closeAction = new QAction(tr("&Close"), this);
    m_closeAction->setShortcut(QKeySequence::Close);
    m_closeAction->setEnabled(false);
    connect(m_closeAction, &QAction::triggered, this, &MainWindow::closeDocument);

    m_exitAction = new QAction(tr("E&xit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    m_zoomInAction = new QAction(tr("Zoom &In"), this);
    m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
    m_zoomInAction->setEnabled(false);
    connect(m_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

    m_zoomOutAction = new QAction(tr("Zoom &Out"), this);
    m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    m_zoomOutAction->setEnabled(false);
    connect(m_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

    m_zoomActualAction = new QAction(tr("&Actual Size"), this);
    m_zoomActualAction->setShortcut(QKeySequence("Ctrl+1"));
    m_zoomActualAction->setEnabled(false);
    connect(m_zoomActualAction, &QAction::triggered, this, &MainWindow::zoomActual);

    m_fitWidthAction = new QAction(tr("Fit &Width"), this);
    m_fitWidthAction->setShortcut(QKeySequence("Ctrl+9"));
    m_fitWidthAction->setEnabled(false);
    connect(m_fitWidthAction, &QAction::triggered, this, &MainWindow::fitToWidth);

    m_fitPageAction = new QAction(tr("Fit &Page"), this);
    m_fitPageAction->setShortcut(QKeySequence("Ctrl+0"));
    m_fitPageAction->setEnabled(false);
    connect(m_fitPageAction, &QAction::triggered, this, &MainWindow::fitToPage);

    m_goToPageAction = new QAction(tr("&Go to Page..."), this);
    m_goToPageAction->setShortcut(QKeySequence("Ctrl+G"));
    m_goToPageAction->setEnabled(false);
    connect(m_goToPageAction, &QAction::triggered, this, &MainWindow::goToPage);

    m_firstPageAction = new QAction(tr("&First Page"), this);
    m_firstPageAction->setShortcut(QKeySequence::MoveToStartOfDocument);
    m_firstPageAction->setEnabled(false);
    connect(m_firstPageAction, &QAction::triggered, this, &MainWindow::firstPage);

    m_prevPageAction = new QAction(tr("&Previous Page"), this);
    m_prevPageAction->setShortcut(QKeySequence::PreviousChild);
    m_prevPageAction->setEnabled(false);
    connect(m_prevPageAction, &QAction::triggered, this, &MainWindow::previousPage);

    m_nextPageAction = new QAction(tr("&Next Page"), this);
    m_nextPageAction->setShortcut(QKeySequence::NextChild);
    m_nextPageAction->setEnabled(false);
    connect(m_nextPageAction, &QAction::triggered, this, &MainWindow::nextPage);

    m_lastPageAction = new QAction(tr("&Last Page"), this);
    m_lastPageAction->setShortcut(QKeySequence::MoveToEndOfDocument);
    m_lastPageAction->setEnabled(false);
    connect(m_lastPageAction, &QAction::triggered, this, &MainWindow::lastPage);

    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_closeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(m_zoomInAction);
    viewMenu->addAction(m_zoomOutAction);
    viewMenu->addAction(m_zoomActualAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_fitWidthAction);
    viewMenu->addAction(m_fitPageAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_goToPageAction);

    QMenu* pageMenu = menuBar()->addMenu(tr("&Page"));
    pageMenu->addAction(m_firstPageAction);
    pageMenu->addAction(m_prevPageAction);
    pageMenu->addAction(m_nextPageAction);
    pageMenu->addAction(m_lastPageAction);
}

void MainWindow::createToolbar()
{
    m_toolbar = addToolBar(tr("Main Toolbar"));
    m_toolbar->setMovable(false);

    m_toolbar->addAction(m_openAction);
    m_toolbar->addAction(m_closeAction);
    m_toolbar->addSeparator();

    m_toolbar->addAction(m_zoomOutAction);
    m_toolbar->addAction(m_zoomInAction);
    m_toolbar->addSeparator();

    m_toolbar->addAction(m_fitWidthAction);
    m_toolbar->addAction(m_fitPageAction);
    m_toolbar->addSeparator();

    m_firstPageBtn = m_toolbar->addAction(tr("|<"));
    connect(m_firstPageBtn, &QAction::triggered, this, &MainWindow::firstPage);

    m_prevPageBtn = m_toolbar->addAction(tr("<"));
    connect(m_prevPageBtn, &QAction::triggered, this, &MainWindow::previousPage);

    m_pageSlider = new QSlider(Qt::Horizontal, this);
    m_pageSlider->setMinimum(0);
    m_pageSlider->setMaximum(0);
    m_pageSlider->setPageStep(1);
    m_pageSlider->setVisible(false);
    connect(m_pageSlider, &QSlider::valueChanged, this, &MainWindow::onPageSliderChanged);
    m_toolbar->addWidget(m_pageSlider);

    m_pageLabel = new QLabel(tr("0 / 0"), this);
    m_pageLabel->setMinimumWidth(80);
    m_pageLabel->setAlignment(Qt::AlignCenter);
    m_toolbar->addWidget(m_pageLabel);

    m_nextPageBtn = m_toolbar->addAction(tr(">"));
    connect(m_nextPageBtn, &QAction::triggered, this, &MainWindow::nextPage);

    m_lastPageBtn = m_toolbar->addAction(tr(">|"));
    connect(m_lastPageBtn, &QAction::triggered, this, &MainWindow::lastPage);
}

void MainWindow::createStatusBar()
{
    m_zoomLabel = new QLabel(tr("100%"));
    statusBar()->addPermanentWidget(m_zoomLabel);
}

void MainWindow::updateActions()
{
    bool hasDocument = m_renderer->pageCount() > 0;
    int pageCount = m_renderer->pageCount();
    int currentPage = m_djvuView->currentPage();

    m_closeAction->setEnabled(hasDocument);
    m_zoomInAction->setEnabled(hasDocument);
    m_zoomOutAction->setEnabled(hasDocument);
    m_zoomActualAction->setEnabled(hasDocument);
    m_fitWidthAction->setEnabled(hasDocument);
    m_fitPageAction->setEnabled(hasDocument);
    m_goToPageAction->setEnabled(hasDocument);
    m_firstPageBtn->setEnabled(hasDocument && currentPage > 0);
    m_prevPageBtn->setEnabled(hasDocument && currentPage > 0);
    m_nextPageBtn->setEnabled(hasDocument && currentPage < pageCount - 1);
    m_lastPageBtn->setEnabled(hasDocument && currentPage < pageCount - 1);

    m_pageSlider->setVisible(hasDocument);
    if (hasDocument) {
        m_pageSlider->setMaximum(pageCount - 1);
        m_pageSlider->setValue(currentPage);
        m_pageLabel->setText(tr("%1 / %2").arg(currentPage + 1).arg(pageCount));
    } else {
        m_pageLabel->setText(tr("0 / 0"));
    }

    m_zoomLabel->setText(tr("%1%").arg(static_cast<int>(m_djvuView->zoomFactor() * 100)));
}

void MainWindow::updateWindowTitle()
{
    if (m_currentFile.isEmpty()) {
        setWindowTitle(tr("DJVU Reader"));
    } else {
        setWindowTitle(tr("%1 - DJVU Reader").arg(QFileInfo(m_currentFile).fileName()));
    }
}

void MainWindow::loadDocument(const QString& filePath)
{
    if (m_renderer->loadDocument(filePath)) {
        m_currentFile = filePath;
        m_djvuView->setPage(0);
        m_djvuView->fitToWidth();
        updateWindowTitle();
        updateActions();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Failed to load document: %1").arg(filePath));
    }
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open DJVU File"),
        QString(),
        tr("DJVU Files (*.djvu *.djv);;All Files (*.*)"));

    if (!filePath.isEmpty()) {
        loadDocument(filePath);
    }
}

void MainWindow::closeDocument()
{
    m_renderer->closeDocument();
    m_currentFile.clear();
    m_djvuView->setPage(0);
    updateWindowTitle();
    updateActions();
}

void MainWindow::zoomIn()
{
    m_djvuView->setZoom(m_djvuView->zoomFactor() * 1.25);
    updateActions();
}

void MainWindow::zoomOut()
{
    m_djvuView->setZoom(m_djvuView->zoomFactor() / 1.25);
    updateActions();
}

void MainWindow::zoomActual()
{
    m_djvuView->setZoom(1.0);
    updateActions();
}

void MainWindow::fitToWidth()
{
    m_djvuView->fitToWidth();
    updateActions();
}

void MainWindow::fitToPage()
{
    m_djvuView->fitToPage();
    updateActions();
}

void MainWindow::goToPage()
{
    int pageCount = m_renderer->pageCount();
    if (pageCount <= 0)
        return;

    bool ok;
    int page = QInputDialog::getInt(
        this,
        tr("Go to Page"),
        tr("Page number:"),
        m_djvuView->currentPage() + 1,
        1,
        pageCount,
        1,
        &ok);

    if (ok) {
        m_djvuView->setPage(page - 1);
        updateActions();
    }
}

void MainWindow::firstPage()
{
    m_djvuView->setPage(0);
    updateActions();
}

void MainWindow::previousPage()
{
    m_djvuView->setPage(m_djvuView->currentPage() - 1);
    updateActions();
}

void MainWindow::nextPage()
{
    m_djvuView->setPage(m_djvuView->currentPage() + 1);
    updateActions();
}

void MainWindow::lastPage()
{
    m_djvuView->setPage(m_renderer->pageCount() - 1);
    updateActions();
}

void MainWindow::onPageSliderChanged(int value)
{
    m_djvuView->setPage(value);
    updateActions();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMainWindow::closeEvent(event);
}