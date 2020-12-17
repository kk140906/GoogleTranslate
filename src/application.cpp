#include "application.h"
App::App(int argc, char **argv) : QApplication(argc,argv) {
  init_();
}

void App::init_() {
  QCoreApplication::setApplicationName(app_name_);
  QLocalSocket socket;
  socket.connectToServer(app_name_);
  if(socket.waitForConnected(timeout_)) {
      running_ = true;
      return;
  }

  server_=std::make_shared<QLocalServer>(new QLocalServer(this));
//  connect(*server_,SIGNAL(),this,SLOT(new_connect_()));

  if(!server_->listen(app_name_)) {
      if(server_->serverError()==QAbstractSocket::AddressInUseError)
      {
          QLocalServer::removeServer(app_name_);
          server_->listen(app_name_);
      }
  }
}



