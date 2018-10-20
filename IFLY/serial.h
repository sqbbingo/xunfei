#ifndef SERIAL_H
#define SERIAL_H

#include <QMainWindow>
#include<QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class Serial;
}

class Serial : public QMainWindow
{
    Q_OBJECT

public:
    explicit Serial(QWidget *parent = 0);
    ~Serial();

private slots:
    void on_OpenSerialButton_clicked();
    void ReadData();
    void on_SendButton_clicked();
private:
    Ui::Serial *ui;
    QSerialPort *serial;
};

#endif // SERIAL_H
