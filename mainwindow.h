#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QFile>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpServer tcpServer;
    QTcpSocket *tcpServerConnection;
    qint64 totalBytes;     // Сохраняем информацию об общем размере
    qint64 bytesReceived;  // Размер полученных данных
    qint64 fileNameSize;   // информация о размере имени файла
    qint64 imageSize; // размер картинки

    QString fileName;      // Сохраняем имя файла
    QFile *localFile;      // локальные файлы
    QByteArray inBlock;    // буфер данных
    QString imageContent;

    QImage image;//образ


   private slots:
       void start();// Мониторинг события
       void acceptConnection();// После подключения клиентом создаем сокет, получаем данные, обрабатываем исключения и закрываем сервер
       void updateServerProgress();// Получение и обработка отображаемого изображения

       // base64 строка для изображения
       QImage getImage(const QString &);
       void on_startButton_clicked();
};
#endif // MAINWINDOW_H
