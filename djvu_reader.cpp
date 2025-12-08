// djvu_reader.cpp
#include <iostream>
#include <cstdlib>

// Qt includes
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QImage>

#include <libdjvu/ddjvuapi.h>
#include <libdjvu/miniexp.h>  
#include <QScrollArea>
#include <QWheelEvent>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.djvu> [pageno]\n";
        return 1;
    }

    QApplication app(argc, argv);

    // 1. Create context
    ddjvu_context_t *ctx = ddjvu_context_create("djvu_reader");

    // 2. Open document
    ddjvu_document_t *doc = ddjvu_document_create_by_filename_utf8(ctx, argv[1], TRUE);
    if (!doc) {
        std::cerr << "Cannot open file\n";
        ddjvu_context_release(ctx);
        return 1;
    }

    // 3. Wait until decoding of the whole document is done
    while (!ddjvu_document_decoding_done(doc))
        ddjvu_message_wait(ctx), ddjvu_message_pop(ctx);

    int pageno = (argc >= 3) ? std::atoi(argv[2]) : 0;

    // 4. Create page
    ddjvu_page_t *page = ddjvu_page_create_by_pageno(doc, pageno);
    while (!page || !ddjvu_page_decoding_done(page))
        ddjvu_message_wait(ctx), ddjvu_message_pop(ctx);

    // 5. Get page size (function name changed in newer libdjvulibre)
    int width = 0, height = 0;
    ddjvu_page_get_size(page, &width, &height);   // now works

    // 6. Render setup
    ddjvu_format_t *fmt = ddjvu_format_create(DDJVU_FORMAT_RGB24, 0, nullptr);
    ddjvu_format_set_row_order(fmt, 1);      // top-to-bottom
    ddjvu_format_set_y_direction(fmt, 1);    // top-down

    // ddjvu_rect_t fields are unsigned int → cast needed in C++11+
    ddjvu_rect_t pagerect = {0, 0, static_cast<unsigned>(width), static_cast<unsigned>(height)};
    ddjvu_rect_t renderrect = pagerect;

    // Fixed VLA → use std::vector instead (portable & safe)
    std::vector<char> buffer(width * height * 3);

    // 7. Render the page
    if (!ddjvu_page_render(page,
                           DDJVU_RENDER_COLOR,
                           &pagerect,
                           &renderrect,
                           fmt,
                           static_cast<unsigned>(buffer.size()),
                           buffer.data())) {
        std::cerr << "Rendering failed\n";
    }

    // 8. Convert to QImage/QPixmap and display
    QImage img(reinterpret_cast<const uchar*>(buffer.data()),
               width, height,
               QImage::Format_RGB888);

    QLabel label;
    label.setPixmap(QPixmap::fromImage(img.rgbSwapped()));  // BGR → RGB
    label.resize(width, height);
    label.show();
// After creating label:
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidget(&label);
    scroll->resize(800, 600);
    scroll->show();

    // Cleanup
    ddjvu_format_release(fmt);
    ddjvu_page_release(page);
    ddjvu_document_release(doc);
    ddjvu_context_release(ctx);

    return app.exec();
}
