#include<QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

#include "ntcap.h" // твоя библиотека

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("NTCAP");
    window.resize(400, 300);
    window.show();

    return app.exec();
}

