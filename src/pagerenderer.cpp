#include "pagerenderer.h"

PageRenderer::PageRenderer(QObject* parent)
    : QObject(parent)
    , m_context(nullptr)
    , m_document(nullptr)
    , m_pageCount(0)
{
}

PageRenderer::~PageRenderer()
{
    closeDocument();
}

bool PageRenderer::loadDocument(const QString& filePath)
{
    Q_UNUSED(filePath)
    return false;
}

void PageRenderer::closeDocument()
{
    m_document = nullptr;
    m_pageCount = 0;
}

int PageRenderer::pageCount() const
{
    return m_pageCount;
}

QSize PageRenderer::pageSize(int pageIndex) const
{
    Q_UNUSED(pageIndex)
    return QSize();
}

QImage PageRenderer::renderPage(int pageIndex, const QSize& targetSize, qreal dpi) const
{
    Q_UNUSED(pageIndex)
    Q_UNUSED(targetSize)
    Q_UNUSED(dpi)
    return QImage();
}