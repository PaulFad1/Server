#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QCompleter"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->portLineEdit->setPlaceholderText(tr("6666"));// Устанавливаем приглашение по умолчанию
    QStringList portWordList;
    portWordList << tr("6666");
    QCompleter* portCompleter = new QCompleter(portWordList, this);
    ui->portLineEdit->setCompleter(portCompleter);

    connect(&tcpServer, SIGNAL(newConnection()),
               this, SLOT(acceptConnection()));


    ui->imageLabel->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start()
{


    if (!tcpServer.listen(QHostAddress::LocalHost, ui->portLineEdit->text().toInt())) {
        close();
        return;
    }

    totalBytes = 0;
    bytesReceived = 0;
    imageSize = 0;
    ui->serverStatusLabel->setText(tr("Мониторинг"));

}

void MainWindow::acceptConnection()
{
    // Получаем сокет ссылки
    tcpServerConnection = tcpServer.nextPendingConnection();

    // Получение данных
    // readyRead () при появлении новой полезной нагрузки сетевых данных в сетевом сокете
    connect(tcpServerConnection, SIGNAL(readyRead()),
            this, SLOT(updateServerProgress()));
    // Обработка исключения
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    ui->serverStatusLabel->setText(tr("Принять соединение"));
    // Закрываем сервер и больше не слушаем
//    tcpServer.close();
}

void MainWindow::updateServerProgress()
{
    QDataStream in(tcpServerConnection);
    in.setVersion(QDataStream::Qt_5_6);

    // Если полученные данные меньше 16 байт, сохраняем структуру заголовка входящего файла
    if (bytesReceived <= sizeof(qint64)*2) {
        if((tcpServerConnection->bytesAvailable() >= sizeof(qint64)*2)
                && (imageSize == 0)) {
            // Получение информации об общем размере данных и информации о размере имени файла
            in >> totalBytes  >> imageSize;
            bytesReceived += sizeof(qint64) * 2;

            if(imageSize == 0){
                  ui->serverStatusLabel->setText(tr("Показанная картинка пуста!"));
            }

        }
        if((tcpServerConnection->bytesAvailable() >= imageSize)
                && (imageSize != 0)) {

            // Получение имени файла и создание файла
            in >> imageContent;

//            qDebug() << imageContent << endl;

            ui->serverStatusLabel->setText(tr("Получает файлы…"));

            QImage imageData = getImage(imageContent);

            QPixmap resImage = QPixmap::fromImage(imageData);
            QPixmap* imgPointer = &resImage;
            imgPointer->scaled(ui->imageLabel->size(), Qt::IgnoreAspectRatio);// Изменим размер изображения по размеру окна
           // imgPointer-> scaled (ui-> imageLabel-> size (), Qt :: KeepAspectRatio); // Устанавливаем размер масштабирования растрового изображения

            ui->imageLabel->setScaledContents(true);// Установить атрибут метки, можно увеличить растровое изображение, чтобы заполнить все доступное пространство.
            ui->imageLabel->setPixmap(*imgPointer);

            bytesReceived += imageSize;


            if(bytesReceived == totalBytes){
                 ui->serverStatusLabel->setText(tr("Файл успешно получен"));
                 totalBytes = 0;
                 bytesReceived = 0;
                 imageSize = 0;
            }

         }
     }
}

QImage MainWindow::getImage(const QString &data)
{
    QByteArray imageData = QByteArray::fromBase64(data.toLatin1());
    QImage image;
    image.loadFromData(imageData);
    return image;
}
void MainWindow::on_startButton_clicked()
{
    if(ui->startButton->text() == tr("Монитор")){
            ui->startButton->setText(tr("Отключить"));
            start();
        }else{
            ui->startButton->setText(tr("Монитор"));
            tcpServer.close();

            tcpServerConnection->disconnectFromHost();
        }
}
