#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start(3000);

    ui->setupUi(this);

    udpSocket= new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any,10004);//绑定10004端口

    myAudio= new AudioThread();
    connect(myAudio->inputDevice,SIGNAL(readyRead()),this,SLOT(MicroPhoneonReadyRead()));
     //槽函数，当inputDevice收到input写入的音频数据之后,调用onReadyRead函数，发送数据到目标主机上



    myAudio->start();
  //  myAudio->Pause();

    myAudioRec =new AudioRecThread();
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(RecreadyReadSlot()));
    //收到网络数据报就开始往outputDevice写入，进行播放
    myAudioRec->start();
    myAudioRec->Pause();

    ui->comboBox->addItem("无目标");
    ui->comboBox->addItem("广播地址");
    ui->comboBox->addItem("本地回环");
    connect(ui->comboBox,SIGNAL(currentTextChanged(QString)),this,SLOT(AddIPChange()));

    ui->label->setText("本机地址:"+GetIpAddress());
    MyIp=GetIpAddress();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onTimeout()//向局域网中其他设备广播自己地址
{
   QString msg="{\"IP\":"+MyIp+"}";

    udpSocket->writeDatagram(msg.toUtf8(),QHostAddress("255.255.255.255"),10004);


}


QString MainWindow::GetIpAddress()//获取本地IP
{

    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i)
    {
           if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
               ipAddressesList.at(i).toIPv4Address()) {
               strIpAddress = ipAddressesList.at(i).toString();
               break;
           }
     }
     // 如果没有找到，则以本地IP地址为IP
     if (strIpAddress.isEmpty())
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
     return strIpAddress;
}

void MainWindow::MicroPhoneonReadyRead()
{
    //qDebug()<<"It's sending audio!"<<endl;
    video vp;
    memset(&vp,0,sizeof(vp));
    //读取音频
    vp.lens =myAudio->inputDevice->read(vp.audiodata,1024);//读取音频
    QHostAddress destaddr;
    destaddr.setAddress(AddIp);
    //将此结构体发送给目标主机，端口是10004
    udpSocket->writeDatagram((const char*)&vp,sizeof(vp),destaddr,10004);

}

void MainWindow::RecreadyReadSlot()//Udp接受到数据，播放音频
{



    myAudioRec->Resume();
    while(udpSocket->hasPendingDatagrams())
    {
        QHostAddress senderip;
        quint16 senderport;
       // qDebug()<<"audio is being received..."<<endl;
        video vp;
        memset(&vp,0,sizeof(vp));
        udpSocket->readDatagram((char*)&vp,sizeof(vp),&senderip,&senderport);

        myAudioRec->outputDevice->write(vp.audiodata,vp.lens);
    }

    myAudioRec->Pause();
}


void MainWindow::on_pushButton_pressed()//点击对讲按钮
{
    if(ui->pushButton->text() == tr("对讲"))
      {
         // AddIp = ui->lineEdit->text();
          //myAudio->Resume();
          ui->pushButton->setText("停止");
      }
      else
      {
         //myAudio->Pause();
        AddIp = "无";
         ui->pushButton->setText("对讲");

      }

}

void MainWindow::AddIPChange()//COMbox
{

    if(ui->comboBox->currentText()=="广播地址")   {AddIp ="255.255.255.255"; ui->pushButton->setText("停止");}
    else if(ui->comboBox->currentText()=="本地回环")  { AddIp ="127.0.0.1"; ui->pushButton->setText("停止");}
    else {AddIp =ui->comboBox->currentText(); ui->pushButton->setText("停止");}


}

void MainWindow::on_pushButton_2_clicked()
{
    AddIp = ui->lineEdit->text();  ui->pushButton->setText("停止");
}
