#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void debbugArduino();

    void readSerial();

    void updateTemperature(QString);

private:

    Ui::MainWindow *ui;

    QSerialPort *arduino;

    static const quint16 arduino_uno_vendor_id = 10755;

    static const quint16 arduino_uno_product_id = 67;

    double temperature_value;

    QByteArray serialData;

    QString serialBuffer;

    QString parsed_data;

};

#endif // MAINWINDOW_H
