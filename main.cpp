#include <memory>
#include <QApplication>

#include "mainwindow.h"
#include "data_layer.h"
#include "services_layer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
 
    std::shared_ptr<services_layer> sl = std::make_shared<services_layer>();
    std::shared_ptr<data_layer> dl = std::make_shared<data_layer>();
    MainWindow w;

    sl->dataLayer(dl);
    sl->mainWindowUILayer(&w);

    w.servicesLayer(sl);
    dl->servicesLayer(sl);

    sl->deserialize("..\\diary-michzyg\\entries.txt");

    w.show();
    auto r = a.exec();

    sl->serialize("..\\diary-michzyg\\entries.txt");

    return r;
}
