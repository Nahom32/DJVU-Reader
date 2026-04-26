#include "djvuview.h"
#include "pagerenderer.h"

#include <QWheelEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QPainter>
#include <QDebug>

DjvuView::DjvuView(QWidget* parent)
    : QScrollArea(parent)
    , m_renderer(nullptr)
    , m_currentPage(0)
    , m_zoomFactor(1.0)
{
    m_viewport = new QWidget(this);
    m_viewport->setBackgroundRole(QPalette::Dark);
    setWidget(m_viewport);
    setWidgetResizable(false);
    setAlignment(Qt::AlignCenter);
}

DjvuView::~DjvuView()
{
}

void DjvuView::setPageRenderer(PageRenderer* renderer)
{
    m_renderer = renderer;
}

void DjvuView::setPage(int pageIndex)
{
    if (!m_renderer)
        return;

    int pageCount = m_renderer->pageCount();
    if (pageIndex < 0 || pageIndex >= pageCount)
        return;

    m_currentPage = pageIndex;
    renderCurrentPage();
    emit pageChanged(m_currentPage);
}

void DjvuView::setZoom(qreal zoomFactor)
{
    if (zoomFactor < 0.1 || zoomFactor > 4.0)
        return;

    m_zoomFactor = zoomFactor;
    renderCurrentPage();
    emit zoomChanged(m_zoomFactor);
}

void DjvuView::fitToWidth()
{
    if (!m_renderer || m_currentImage.isNull())
        return;

    int viewportWidth = width() - 20;
    if (viewportWidth <= 0)
        viewportWidth = 800;

    qreal newZoom = static_cast<qreal>(viewportWidth) / m_currentImage.width();
    setZoom(newZoom);
}

void DjvuView::fitToPage()
{
    if (!m_renderer || m_currentImage.isNull())
        return;

    int viewportWidth = width() - 20;
    int viewportHeight = height() - 20;

    qreal zoomWidth = static_cast<qreal>(viewportWidth) / m_currentImage.width();
    qreal zoomHeight = static_cast<qreal>(viewportHeight) / m_currentImage.height();

    setZoom(qMin(zoomWidth, zoomHeight));
}

void DjvuView::updateViewport()
{
    renderCurrentPage();
}

void DjvuView::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int delta = event->angleDelta().y();
        qreal step = (delta > 0) ? 1.1 : 0.9;
        setZoom(m_zoomFactor * step);
        event->accept();
    } else {
        QScrollArea::wheelEvent(event);
    }
}

void DjvuView::keyPressEvent(QKeyEvent* event)
{
    int pageCount = m_renderer ? m_renderer->pageCount() : 0;

    switch (event->key()) {
    case Qt::Key_PageUp:
    case Qt::Key_Left:
        if (m_currentPage > 0)
            setPage(m_currentPage - 1);
        break;
    case Qt::Key_PageDown:
    case Qt::Key_Right:
        if (m_currentPage < pageCount - 1)
            setPage(m_currentPage + 1);
        break;
    case Qt::Key_Home:
        if (pageCount > 0)
            setPage(0);
        break;
    case Qt::Key_End:
        if (pageCount > 0)
            setPage(pageCount - 1);
        break;
    default:
        QScrollArea::keyPressEvent(event);
        break;
    }
}

void DjvuView::renderCurrentPage()
{
    if (!m_renderer || m_currentImage.isNull())
        return;

    QSize scaledSize = m_currentImage.size() * m_zoomFactor;
    m_viewport->resize(scaledSize);
    m_viewport->update();
}

void DjvuView::updateScrollBars()
{
}