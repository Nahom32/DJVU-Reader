#ifndef PAGERENDERER_H
#define PAGERENDERER_H

#include <QObject>
#include <QImage>
#include <QSize>

class DDJVUContext;
class DDJVUDocument;
class DDJVUPage;

class PageRenderer : public QObject
{
    Q_OBJECT

public:
    explicit PageRenderer(QObject* parent = nullptr);
    ~PageRenderer();

    bool loadDocument(const QString& filePath);
    void closeDocument();

    int pageCount() const;
    QSize pageSize(int pageIndex) const;
    QImage renderPage(int pageIndex, const QSize& targetSize, qreal dpi = 72.0) const;

signals:
    void documentLoaded(bool success);
    void renderCompleted(const QImage& image);

private:
    DDJVUContext* m_context;
    DDJVUDocument* m_document;
    int m_pageCount;
};

#endif // PAGERENDERER_H