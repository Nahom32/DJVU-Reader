#ifndef DJVUVIEW_H
#define DJVUVIEW_H

#include <QWidget>
#include <QImage>
#include <QScrollArea>
#include <QScrollBar>

class PageRenderer;

class DjvuView : public QScrollArea
{
    Q_OBJECT

public:
    explicit DjvuView(QWidget* parent = nullptr);
    ~DjvuView();

    void setPageRenderer(PageRenderer* renderer);
    void setPage(int pageIndex);
    void setZoom(qreal zoomFactor);
    void fitToWidth();
    void fitToPage();

    int currentPage() const { return m_currentPage; }
    qreal zoomFactor() const { return m_zoomFactor; }

signals:
    void pageChanged(int page);
    void zoomChanged(qreal zoom);

private slots:
    void updateViewport();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void renderCurrentPage();
    void updateScrollBars();

    PageRenderer* m_renderer;
    QWidget* m_viewport;
    QImage m_currentImage;
    int m_currentPage;
    qreal m_zoomFactor;
};

#endif // DJVUVIEW_H