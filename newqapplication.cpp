#include <newqapplication.h>
NewQApplication::NewQApplication(int argc, char **argv) :QApplication(argc,argv)
{
    _isRunning=false;
    QCoreApplication::setApplicationName("localserver");
    QString serverName=QCoreApplication::applicationName();

    QLocalSocket socket;
    socket.connectToServer(serverName);

    if(socket.waitForConnected(500))
    {
        QTextStream stream(&socket);
        QStringList args=QCoreApplication::arguments();

        if(args.count()>1)
        {
            stream<<args.last();
        }else
        {
            stream<<QString();
        }
        stream.flush();
        socket.waitForBytesWritten();
        _isRunning=true;
        return;
    }
    server=new QLocalServer(this);
    connect(server,SIGNAL(newConnection()),this,SLOT(newLocalConnect()));

    if(!server->listen(serverName))
    {
        if(server->serverError()==QAbstractSocket::AddressInUseError)
        {
            QLocalServer::removeServer(serverName);
            server->listen(serverName);
        }
    }
}

NewQApplication::~NewQApplication()
{
    delete server;
}
void NewQApplication::newLocalConnect()
{
    QLocalSocket *socket=server->nextPendingConnection();
    if(!socket)
        return;
    socket->waitForReadyRead(1000);
    QTextStream in(socket);
    QString vl;
    in>>vl;
}
bool NewQApplication::isRunning()
{
  return _isRunning;
}
