#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase sensoringDB = QSqlDatabase::addDatabase("QSQLITE");
    sensoringDB.setDatabaseName("/home/lauro/Desktop/Pesquisa/cpp/qt/ConnectingDatabase/sensors.db");

    if(!sensoringDB.open()) {
        qDebug() << "NOT Connected to Database!";
    }
    else {
        qDebug() << "Connected to Database...";
    }

    QSqlQuery query;

    query.exec("SELECT * FROM Sensoring");

    qDebug() << "# Leitura da Base de Dados";

    while (query.next()){
        QString kind = query.value(0).toString();
        QString timestamp = query.value(1).toString();
        QString value = query.value(2).toString();
        qDebug() << kind << timestamp << value ;
    }

    QDateTime now = QDateTime::currentDateTime();

    qDebug() << now.toString();

    // Reading Arduino

    qDebug() << "# Iniciando leitura de arduino";

    arduino = new QSerialPort(this);

    // debbugArduino();

    bool arduino_is_available = false;
    QString arduino_uno_port_name;

    // @TODO Transformar numa funçao
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        //  check if the serialport has both a product identifier and a vendor identifier
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
            //  check if the product ID and the vendor ID match those of the arduino uno
            if((serialPortInfo.productIdentifier() == arduino_uno_product_id)
                    && (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id)){
                arduino_is_available = true; //    arduino uno is available on this port
                arduino_uno_port_name = serialPortInfo.portName();
            }
        }
    }

    if(arduino_is_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_uno_port_name);
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud115200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }else{
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }

}

MainWindow::~MainWindow()
{
    if(arduino->isOpen()){
        arduino->close();
    }
    delete ui;
}

void MainWindow::debbugArduino()
{
    qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length() << "\n";
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "Description: " << serialPortInfo.description() << "\n";
        qDebug() << "Has vendor id?: " << serialPortInfo.hasVendorIdentifier() << "\n";
        qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier() << "\n";
        qDebug() << "Has product id?: " << serialPortInfo.hasProductIdentifier() << "\n";
        qDebug() << "Product ID: " << serialPortInfo.productIdentifier() << "\n";
    }
}

void MainWindow::readSerial()
{
    QStringList buffer_split = serialBuffer.split(",");

    if(buffer_split.length() < 3){
        serialData = arduino->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        MainWindow::updateTemperature(serialBuffer);
        serialData.clear();
    }else{
        serialBuffer = "";
        qDebug() << buffer_split << "\n";
        parsed_data = buffer_split[1];
        temperature_value = (9/5.0) * (parsed_data.toDouble()) + 32; // convert to fahrenheit
        qDebug() << "Temperature: " << temperature_value << "\n";
        parsed_data = QString::number(temperature_value, 'g', 4); // format precision of temperature_value to 4 digits or fewer
        MainWindow::updateTemperature(parsed_data);
    }
}

void MainWindow::updateTemperature(QString sensor_reading)
{
    ui->lcdNumber->display(sensor_reading);
    ui->temp_label->setText(sensor_reading);
    qDebug() << sensor_reading ;
}
