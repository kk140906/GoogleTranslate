#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setWindowFlags(Qt::FramelessWindowHint);
  ui->TitleFrame->installEventFilter(this);
  CreateTrayIcon();
  UpdateTkk();//Create object Firstly Update TKK;
  QObject::connect(this,SIGNAL(DebugOutput(QString,int)),this,SLOT(DebugHandle(QString,int)));
  QObject::connect(ui->TitleMinimizeLabel,SIGNAL(clicked()),this,SLOT(close()));
  QObject::connect(ui->InputContent,SIGNAL(textChanged()),this,SLOT(TextisChanged()));
  QObject::connect(this,SIGNAL(UpdateTkkisfinished()),this,SLOT(CalToken()));
  QObject::connect(this,SIGNAL(CalTokenisfinished()),this,SLOT(WebRequest()));
  QObject::connect(this,SIGNAL(ReadResultisfinished(QString)),ui->ReturnResult,SLOT(setPlainText(QString)));
  QObject::connect(this,SIGNAL(ReadSynonym_zh_CNisfinished()),this,SLOT(showSynonym_zh_CN()));
  QObject::connect(this,SIGNAL(ReadSynonym_enisfinished()),this,SLOT(showSynonym_en()));

  editTimer =new QTimer(this);
  QObject::connect(editTimer,SIGNAL(timeout()),this,SLOT(TimerEnd()));

  waitProgess = new WaitProgress(ui->MainWidget);
  waitProgess->setGeometry(QRect(180,200,25,25));
  waitProgess->setColor(QColor(220,165,25));
  ui->Synonym_enScrollArea->hide();
  ui->Synonym_zh_CNScrollArea->hide();
}

void MainWindow::DebugHandle(QString info,int handle)
{
  if(handle == _error)
    {
      errorFlag = true;
      hide();
      waitProgess->stopAnimation();
      _isRunning = false;
    }
  if(!DebuglogFlag) return;
  QDateTime currentDateTime = QDateTime::currentDateTime();
  QString Date = currentDateTime.toString(QStringView(QString("yyyy-MM-dd")));
  QString Time = currentDateTime.toString(QStringView(QString("yyyy-MM-dd HH:mm:ss")));
  QDir *LogDir = new QDir();
  if(!LogDir->exists("Log"))
    LogDir->mkdir("Log");
  QFile *File = new QFile(LogDir->currentPath()+"/Log/"+Date+".log");
  if(!File->open(QIODevice::Append|QIODevice::Text))
    return;
  QTextStream debugOut(File);
  switch(handle)
    {
    case _debug:
      {
        debugOut<<"Debug:["<<Time<<"] : "<<info<<"\n";
        break;
      }
    case _error:
      {
        debugOut<<"Error:["<<Time<<"] : "<<info<<"\n";
        break;
      }
    case _warn:
      {
        debugOut<<"Warn:["<<Time<<"] : "<<info<<"\n";
        break;
      }
    case _info:
      {
        debugOut<<"Info:["<<Time<<"] : "<<info<<"\n";
        break;
      }
    default:
      break;
    }
  File->close();
}

MainWindow::~MainWindow()
{
  DeleteAllScrollObject();
  delete ui;
}

void MainWindow::SimulateCopy()
{
  //Simulate "CTRL+C"
  keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
  keybd_event('C', 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
  keybd_event('C', 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  return;
}

void MainWindow::GetData()
{
  HGLOBAL HglbClipboard;
  LPSTR lptstr;
  if (!IsClipboardFormatAvailable(CF_TEXT))
    {

      msgBox.warning(this,"GoogleTranslate","Can't get text!\n"
                                            "Please don't translate the contents of this Application,\n"
                                            "and try to keep the selected contnents at the Top-Window!");
      emit DebugOutput("Clipboard is Empty!");
      return;
    }
  if (!OpenClipboard(nullptr))
    {
      emit DebugOutput("Open clipboard is Fail!");
      return;
    }

  HglbClipboard = GetClipboardData(CF_TEXT);
  if (HglbClipboard != nullptr)
    {
      lptstr = static_cast<LPSTR>(GlobalLock(HglbClipboard));
      if (lptstr != nullptr)
        {
          ui->InputContent->setPlainText(QString::fromLocal8Bit(lptstr).replace(QRegularExpression("(?<=[\\w\\s\\-])\r\n")," "));
          GlobalUnlock(HglbClipboard);
        }
    }
  EmptyClipboard();
  CloseClipboard();
  return;
}

void MainWindow::EmptyData()
{
  if (!OpenClipboard(nullptr))
    {
      emit DebugOutput("Open clipboard is Fail!");
      return;
    }
  EmptyClipboard();
  CloseClipboard();
  return;
}

bool MainWindow::eventFilter(QObject *object,QEvent *event)
{
  //Title Colum DragMove
  if(event->type()==QEvent::MouseButtonPress)
    {
      QMouseEvent *mouseEvent=static_cast<QMouseEvent *>(event);
      if(mouseEvent->button() == Qt::LeftButton)
        {
          dragFlag=true;
          mwRelativePos=mouseEvent->globalPos()-pos();
        }
    }
  else if(dragFlag && (event->type() == QEvent::MouseMove))
    {
      QMouseEvent *mouseEvent=static_cast<QMouseEvent *>(event);
      move(mouseEvent->globalPos()-mwRelativePos);
    }
  else if(event->type() == QEvent::MouseButtonRelease)
    dragFlag=false;
  return QObject::eventFilter(object, event);
}

void MainWindow::ShowWindow()
{
  QPoint cursorPos;
  errorFlag = false;
  EmptyData();
  SimulateCopy();
  Sleep(230);
  GetData();
  if(errorFlag) return;
  cursorPos=QCursor::pos();
  move(cursorPos);
  show();
  activateWindow();
}

void MainWindow::CreateTrayIcon()
{
  systrayicon=new QSystemTrayIcon(this);
  systrayicon->setIcon(QIcon(":/Qresource/Google_Translate_Icon.png"));
  systrayicon->show();
  //Create Menu
  trayiconmenu = new QMenu(this);
  action_exit = new QAction(trayiconmenu);
  action_showmainwindow = new QAction(trayiconmenu);
  action_logcontrl =new QAction(trayiconmenu);
  action_showmainwindow->setText("显示主界面");
  action_showmainwindow->setIcon(QIcon(":/Qresource/Google_Translate_Icon.png"));
  action_logcontrl->setText("开启日志");
  action_logcontrl->setIcon(QIcon(":/Qresource/Selected_Icon.png"));
  action_logcontrl->setIconVisibleInMenu(false);
  action_exit->setText("退出程序");
  action_exit->setIcon(QIcon(":/Qresource/Close1_Icon.png"));
  trayiconmenu->addAction(action_showmainwindow);
  trayiconmenu->addAction(action_logcontrl);
  trayiconmenu->addAction(action_exit);
  QObject::connect(action_showmainwindow,SIGNAL(triggered()),this,SLOT(show()));
  QObject::connect(action_exit,SIGNAL(triggered()),this,SLOT(ExitProgram()));
  QObject::connect(action_logcontrl,SIGNAL(triggered()),this,SLOT(DebugLogConctrl()));
  systrayicon->setContextMenu(trayiconmenu);
  trayiconmenu->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 255);\n"
                                            "selection-background-color: rgb(188, 188, 188);"));
  QObject::connect(systrayicon,
                   SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                   this,
                   SLOT(TrayIconAction(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::TrayIconAction(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
      show();
      activateWindow();
      break;
    case QSystemTrayIcon::Context:
      trayiconmenu->show();
      break;
    default:
      break;
    }
}

void MainWindow::ExitProgram()
{

  action_showmainwindow->deleteLater();
  action_exit->deleteLater();
  trayiconmenu->deleteLater();
  systrayicon->hide();
  systrayicon->deleteLater();
  exit(0);
}

void MainWindow::DebugLogConctrl()
{
  if(!DebuglogFlag)
    {
      DebuglogFlag = true;
      action_logcontrl->setIconVisibleInMenu(true);
    }
  else
    {
      DebuglogFlag = false;
      action_logcontrl->setIconVisibleInMenu(false);
    }
  return;
}


long long MainWindow::CalTokenMidValue(long long a, QString b)
{
  for (int c = 0; c < b.size() -2; c += 3)
    {
      QChar d = b.at(c+2);
      long long e;
      //result 2 must be a  digit char
      e = d >= 'a' ? d.unicode() - 87 : d.digitValue();
      e = '+' == b.at(c+1) ? (a % 0x100000000) >> e : a << e;
      a = '+' == b.at(c) ? (a + e) & 4294967295 : a ^ e;
    }
  return a;
}

void MainWindow::CalOringinToken(QString text,QString tkk)
{
  if(text.size()>4500)
    {
      QMessageBox msgBox;
      msgBox.activateWindow();
      msgBox.critical(this,"GoogleTranslate","Tranlsate words too long!");
      emit DebugOutput("Tranlsate words too long");
      return;
    }
  QString b = tkk != "0" ? tkk : "";
  QList<QString>  c= b.split('.');
  long long d = c.size() > 1 ? c.at(0).toLongLong() : 0;
  ChineseCount = 0;
  EnglishCount = 0;
  QList<int> e;
  for(int j=0;j<text.size();j++)
    {
      int f = static_cast<int>(text.at(j).unicode());
      if (f < 128)
        {
          EnglishCount++;
          e.append(f);
        }
      else
        {
          ChineseCount++;
          if (f < 2048) e.append(f >> 6 | 192);
          else
            {
              if ((f & 64512) == 55296 && j+1<text.size() && (static_cast<int>(text.at(j+1).unicode()) & 64512) == 56320)
                {
                  j++;
                  f = 65536 + ((1 & 1023) << 10) +(f & 1023);
                  e.append(f >> 18 | 240);
                  e.append((f >> 12 & 63) |128);
                }
              else
                e.append(f >> 12 | 224);
              e.append((f >> 6 & 63) | 128);
            }
          e.append((f & 63) | 128);
        }
    }
  long long g = d;
  for(int k=0;k<e.size();k++)
    {
      g += e.at(k);
      g = CalTokenMidValue(g,"+-a^+6");
    }
  g = CalTokenMidValue(g,"+-3^+b+-f");
  g ^= c.size() > 1 ? c.at(1).toLongLong() : 0;
  if (g < 0) g = (g & 2147483647) + 2147483648;
  g %= 1000000;
  Token=QString("%1.%2").arg(g).arg(g ^ d);
  emit CalTokenisfinished();
  return;
}

void MainWindow::UpdateTkk()
{

  if(!NewTkk.isEmpty())
    {
      QList<QString>  NewTkkList= NewTkk.split('.');
      long long TkkList = NewTkkList.size() > 1 ? NewTkkList.at(0).toLongLong() : 0;
      long long current = static_cast<long long>(floor(QDateTime::currentMSecsSinceEpoch()/3600000.0));
      if(current == TkkList)
        {
          emit DebugOutput("TKK is up to date",_info);
          emit UpdateTkkisfinished();
          return;
        }
    }
  managerTKK=new QNetworkAccessManager(this);
  if(!managerTKK->networkAccessible())
    {
      QMessageBox msgBox;
      emit DebugOutput("UpdateTKK Network Disconnect Error!");
      msgBox.activateWindow();
      msgBox.critical(this,"GoogleTranslate","Network disconnect!");
      return;
    }
  requestTKK.setUrl(QUrl("https://translate.google.cn/"));
  requestTKK.setRawHeader(QByteArray("user-agent"),
                          QByteArray("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36"));
  replyTKK=managerTKK->get(requestTKK);
  if(replyTKK->error()!=QNetworkReply::NoError)
    {
      QMessageBox msgBox;
      msgBox.activateWindow();
      msgBox.critical(this,"GoogleTranslate","Network Request Error!");
      emit DebugOutput("UpdateTKK Request Error!");
      return;
    }
  QObject::connect(replyTKK,SIGNAL(finished()),this,SLOT(ReadTkk()));
}

void MainWindow::ReadTkk()
{
  QVariant statusCode = replyTKK->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if(statusCode.isValid() && statusCode.toInt() != 200)
    {
      QMessageBox msgBox;
      msgBox.activateWindow();
      msgBox.critical(this,"GoogleTranslate","TKK Network Reply Error!");
      emit DebugOutput("ReadTkk Network Reply Error!");
      return;
    }
  QString response=replyTKK->readAll();
  QRegularExpression re_tkk("tkk:\'([0-9,\\.]+?)\'");
  QRegularExpressionMatch matched = re_tkk.match(response);
  if (matched.hasMatch())
    {
      NewTkk = matched.captured(1);
      emit UpdateTkkisfinished();
      emit DebugOutput("NewTKK:"+NewTkk,_info);
      managerTKK->deleteLater();
      replyTKK->deleteLater();
      return;
    }
  replyTKK->close();
  return;
}



void MainWindow::TextisChanged()
{
  ui->ReturnResult->clear();
  text = ui->InputContent->toPlainText();
  if(text.isEmpty())
    {
      waitProgess->stopAnimation();
      ui->ReturnResult->clear();
      return;
    }
  waitProgess->startAnimation();
  DeleteAllScrollObject();
  if(editTimer->isActive())
    {
     emit DebugOutput("Timer is running!",_warn);
      return;
    }
  editTimer->start(600);
  textChangeFlag = true;
}

void MainWindow::TimerEnd()
{
  editTimer->stop();
    if(_isRunning)
      {
        emit DebugOutput("Last Network operate is running!",_warn);
        return;
      }
    _isRunning = true;
    UpdateTkk();
    return;
}

void MainWindow::CalToken()
{
  //Only UpdateTKK & Text is changed can calculate token
  if(!textChangeFlag) return;
  CalOringinToken(text,NewTkk);
  return;
}

void MainWindow::WebRequest()
{
  if(!textChangeFlag) return;
  textChangeFlag = false;
  QList<QPair<QString, QString> > googlequery;
  googlequery<<qMakePair(QString("client"),QString("webapp"));

  if(ChineseCount >= EnglishCount)
    {
      googlequery<<qMakePair(QString("sl"),QString("zh-CN"));
      googlequery<<qMakePair(QString("tl"),QString("en"));
    }
  else
    {
      googlequery<<qMakePair(QString("sl"),QString("en"));
      googlequery<<qMakePair(QString("tl"),QString("zh-CN"));
    }
  googlequery<<qMakePair(QString("hl"),QString("zh-CN"));
  googlequery<<qMakePair(QString("dt"),QString("at"));
  googlequery<<qMakePair(QString("dt"),QString("bd"));
  googlequery<<qMakePair(QString("dt"),QString("ex"));
  googlequery<<qMakePair(QString("dt"),QString("ld"));
  googlequery<<qMakePair(QString("dt"),QString("md"));
  googlequery<<qMakePair(QString("dt"),QString("qca"));
  googlequery<<qMakePair(QString("dt"),QString("rw"));
  googlequery<<qMakePair(QString("dt"),QString("rm"));
  googlequery<<qMakePair(QString("dt"),QString("ss"));
  googlequery<<qMakePair(QString("dt"),QString("t"));
  googlequery<<qMakePair(QString("otf"),QString("1"));
  googlequery<<qMakePair(QString("swap"),QString("1"));
  googlequery<<qMakePair(QString("pc"),QString("1"));
  googlequery<<qMakePair(QString("ssel"),QString("0"));
  googlequery<<qMakePair(QString("tsel"),QString("0"));
  googlequery<<qMakePair(QString("kc"),QString("0"));
  googlequery<<qMakePair(QString("tk"),Token);
  googlequery<<qMakePair(QString("q"),QString(text.toUtf8().toPercentEncoding()));
  QUrlQuery urlgooglequery;
  urlgooglequery.setQueryItems(googlequery);
  url=QString("https://translate.google.cn/translate_a/single?");
  url.setQuery(urlgooglequery);
  managerResult=new QNetworkAccessManager(this);
  if(managerResult->networkAccessible()==QNetworkAccessManager::NotAccessible)
    {
      QMessageBox msgBox;
      msgBox.activateWindow();
      msgBox.critical(this,"GoogleTranslate","Network disconnect!");

      emit DebugOutput("WebRequest Network Disconnect Error!");
      return;
    }
  requestResult.setUrl(url);
  requestResult.setRawHeader(QByteArray("user-agent"),
                             QByteArray("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36"));

  if(managerResultFlag)
    {
      replyResult=managerResult->get(requestResult);
      if (replyResult->error()!=QNetworkReply::NoError)
        {
          QMessageBox msgBox;
          msgBox.activateWindow();
          msgBox.critical(this,"GoogleTranslate","Network Request Error");

          emit DebugOutput("Translate Request Error!");
          return;
        }
      managerResultFlag =false;
    }
  QObject::connect(managerResult,SIGNAL(finished(QNetworkReply *)),this,SLOT(ManagerResultisfinished()));
  QObject::connect(replyResult,SIGNAL(finished()),this,SLOT(ReadResult()));
}

void MainWindow::ManagerResultisfinished()
{
  managerResultFlag = true;
}

void MainWindow::ReadResult()
{
  QVariant statusCode = replyResult->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if(statusCode.isValid())
    {
      if(statusCode.toInt() != 200)
        {
          QMessageBox msgBox;
          msgBox.activateWindow();
          msgBox.critical(this,"GoogleTranslate","Network Request Error\nResponse StatusCode:\n"+statusCode.toString());
          emit DebugOutput("ReadResult Error!");
          return;
        }
    }
  QByteArray response=replyResult->readAll();
  QJsonDocument jsonDoc =QJsonDocument::fromJson(response);
  QJsonArray jsonArray = jsonDoc.array();
  QJsonArray TranslateResultJsonArray= jsonArray.at(0).toArray();
  TranslateResult="";
  for(int j=0;j<TranslateResultJsonArray.size()-1;j++)
    {
      TranslateResult.append(TranslateResultJsonArray.at(j).toArray().at(0).toString());
    }
  emit ReadResultisfinished(TranslateResult);
  if(!jsonArray.at(1).isNull())
    {
      Synonym_zh_CN = jsonArray.at(1).toArray();
      emit ReadSynonym_zh_CNisfinished();


    }
  if(jsonArray.size()>=12)
    {
      if(!jsonArray.at(11).isNull())
        {
          Synonym_en = jsonArray.at(11).toArray();
          emit ReadSynonym_enisfinished();
        }
    }
  waitProgess->stopAnimation();
  _isRunning = false;
  replyResult->close();
}

void MainWindow::showSynonym_zh_CN()
{
  ui->Synonym_zh_CNScrollArea->show();
  QFont font("Consolas",10);
  QFontMetrics fontSize(font);
  QGridLayout *Synonym_zh_CNLayout = new QGridLayout(ui->Synonym_zh_CNContents);
  Synonym_zh_CNLayout->setObjectName("Synonym_zh_CNLayout");
  Synonym_zh_CNLayout->setColumnMinimumWidth(1,250);
  Synonym_zh_CNLayout->setColumnMinimumWidth(2,50);
//  Synonym_zh_CNLayout->setColumnStretch(0,0);
//  Synonym_zh_CNLayout->setColumnStretch(1,0);
//  Synonym_zh_CNLayout->setColumnStretch(2,0);
  Synonym_zh_CNLayout->setAlignment(Qt::AlignCenter);
  int Synonym_zh_CNRow=0;
  int Synonym_zh_CNCol=0;
  for(int i=0;i<Synonym_zh_CN.size();i++)
    {
      QJsonArray ALlwordPropertryArray = Synonym_zh_CN.at(i).toArray();
      QString wordPropertry = ALlwordPropertryArray.at(0).toString()+":";
      QLabel *labelwordPropertry = new QLabel(ui->Synonym_zh_CNContents);
      labelwordPropertry->setObjectName(wordPropertry);
      labelwordPropertry->setText(wordPropertry);
      Synonym_zh_CNLayout->addWidget(labelwordPropertry,Synonym_zh_CNRow,Synonym_zh_CNCol);
      labelwordPropertry->setAlignment(Qt::AlignCenter);
      labelwordPropertry->setFont(font);
      labelwordPropertry->show();
      Synonym_zh_CNRow += 1;
      QJsonArray wordPropertryArray = ALlwordPropertryArray.at(2).toArray();
      for(int j=0;j<wordPropertryArray.size();j++)
        {
          QJsonArray  zh_CNContentArray= wordPropertryArray.at(j).toArray();
          QString ColContent1 = zh_CNContentArray.at(0).toString();
          int ColContent1Width = fontSize.width(ColContent1);
          if(ColContent1Width > 35)
            {
              int  ColContent1Size= ColContent1.size();
              qreal singleCharWidth = ColContent1Width/ColContent1Size;
              for(int indexColcontent1 = 0;indexColcontent1<ColContent1Size;indexColcontent1++)
                {
                  if(indexColcontent1 * singleCharWidth > 35 )
                    {
                      if(ColContent1.at(indexColcontent1) == QChar(' ') ||
                         ColContent1.at(indexColcontent1) == QChar(';') ||
                         ColContent1.at(indexColcontent1) == QChar(','))
                        {
                          ColContent1.replace(indexColcontent1,1,QChar('\n'));
                          break;
                        }
                    }
                }
            }
          QLabel *labelColContent1 = new QLabel(ui->Synonym_zh_CNContents);
          labelColContent1->setObjectName(ColContent1);
          labelColContent1->setText(ColContent1);
          Synonym_zh_CNLayout->addWidget(labelColContent1,Synonym_zh_CNRow,Synonym_zh_CNCol);
          labelColContent1->setAlignment(Qt::AlignCenter);
          labelColContent1->setFont(font);
          labelColContent1->setStyleSheet(QStringLiteral("color:#e55334"));
          labelColContent1->setTextInteractionFlags(Qt::TextSelectableByMouse);
          labelColContent1->show();
          Synonym_zh_CNCol += 1;
          QJsonArray ColContent2Array = zh_CNContentArray.at(1).toArray();

          QVBoxLayout *ColContent2VLayout = new QVBoxLayout();
          QString ColContent2VLayoutObjName = QString("ColContente2VLayout").append(QString::number(i));
          ColContent2VLayout->setObjectName(ColContent2VLayoutObjName);
          QHBoxLayout *ColContent2HLayout = new QHBoxLayout();
          QString ColContent2HLayoutObjName = QString("ColContente2HLayout").append(QString::number(i));
          ColContent2HLayout->setObjectName(ColContent2HLayoutObjName);
          int FontWidth = 0;
          int FontHeight = fontSize.height()+10;
          int WidthPos_X = 0;
          for(int k=0;k<ColContent2Array.size();k++)
            {
              QString ColContent2 = ColContent2Array.at(k).toString();
              QLabel *labelColContent2 = new QLabel(ui->Synonym_zh_CNContents);
              labelColContent2->setObjectName(ColContent2);
              labelColContent2->setFont(font);
              labelColContent2->setStyleSheet(QStringLiteral("border-radius: 5px;\n"
                                                             "border:1.5px solid;\n"
                                                             "padding : 3px;\n"
                                                             "color:#b0ee1b"));
              labelColContent2->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
              labelColContent2->setText(ColContent2);
              labelColContent2->setMinimumHeight(FontHeight);
              labelColContent2->setTextInteractionFlags(Qt::TextSelectableByMouse);
              FontWidth = fontSize.width(ColContent2)+15; //Word Width;

              if(WidthPos_X + FontWidth > 200)
                {
                  QSpacerItem *ColContent2HLayoutSpacer1= new QSpacerItem(240-WidthPos_X,
                                                                          FontHeight,
                                                                          QSizePolicy::Expanding,
                                                                          QSizePolicy::Minimum);
                  ColContent2HLayout->addItem(ColContent2HLayoutSpacer1);
                  ColContent2VLayout->addLayout(ColContent2HLayout);
                  ColContent2HLayout= new QHBoxLayout();
                  QString ColContent2HLayoutNextObjName = ColContent2HLayoutObjName.append(QString::number(k));
                  ColContent2HLayout->setObjectName(ColContent2HLayoutNextObjName);
                  WidthPos_X = 0;

                }
              ColContent2HLayout->addWidget(labelColContent2);
              WidthPos_X += FontWidth + 10; //Word Interval;
              labelColContent2->show();
            }
          QSpacerItem *ColContent2HLayoutSpacer2= new QSpacerItem(240-WidthPos_X,
                                                                  FontHeight,
                                                                  QSizePolicy::Expanding,
                                                                  QSizePolicy::Minimum);
          ColContent2HLayout->addItem(ColContent2HLayoutSpacer2);
          ColContent2VLayout->addLayout(ColContent2HLayout);
          Synonym_zh_CNLayout->addLayout(ColContent2VLayout,Synonym_zh_CNRow,Synonym_zh_CNCol);
          Synonym_zh_CNCol += 1;
          qreal ColContent3 =static_cast<int>(zh_CNContentArray.at(3).toDouble()*10000)/100.0;
          PercentRatio *labelColContent3 = new PercentRatio(ui->Synonym_zh_CNContents);
          labelColContent3->setObjectName(QString::number(ColContent3));
          if(ColContent3<5)
            ColContent3=5;
          labelColContent3->setRatio(ColContent3);
          Synonym_zh_CNLayout->addWidget(labelColContent3,Synonym_zh_CNRow,Synonym_zh_CNCol);
          labelColContent3->setRectHeight(FontHeight-18);
          labelColContent3->show();
          Synonym_zh_CNRow += 1;
          Synonym_zh_CNCol = 0;
        }
    }
  ui->Synonym_zh_CNContents->adjustSize();
}

void MainWindow::showSynonym_en()
{
  ui->Synonym_enScrollArea->show();
  QFont font("Consolas",10);
  QFontMetrics fontSize(font);
  int FontWidth = 0;
  int FontHeight = fontSize.height()+10;
  int WidthPos_X = 0;
  int HeightPos_Y = 0;
  for(int i =0;i<Synonym_en.size();i++)
    {
      QJsonArray wordPropertryArray = Synonym_en.at(i).toArray();
      QLabel *labelwordPropertry = new QLabel(ui->Synonym_enContents);
      QString wordPropertry = wordPropertryArray.at(0).toString();
      wordPropertry += QString(":");
      labelwordPropertry->setFont(font);
      labelwordPropertry->setObjectName(wordPropertry);
      labelwordPropertry->setGeometry(QRect(WidthPos_X,HeightPos_Y,fontSize.width(wordPropertry),FontHeight));
      labelwordPropertry->setText(wordPropertry);
      labelwordPropertry->show();
      WidthPos_X = 0;
      HeightPos_Y += FontHeight;

      QJsonArray AllenArray= wordPropertryArray.at(1).toArray();
      for(int j=0;j<AllenArray.size();j++)
        {
          QJsonArray enArray = AllenArray.at(j).toArray().at(0).toArray();
          for(int k=0;k<enArray.size();k++)
            {
              QLabel *labelcontent = new QLabel(ui->Synonym_enContents);
              QString enContent = enArray.at(k).toString();
              labelcontent->setObjectName(enContent);
              labelcontent->setText(enContent);
              FontWidth = fontSize.width(enContent)+10;
              if(WidthPos_X + FontWidth > 210)
                {
                  WidthPos_X = 0;
                  HeightPos_Y += FontHeight+12;
                }
              labelcontent->setGeometry(QRect(WidthPos_X,HeightPos_Y,FontWidth,FontHeight));

              labelcontent->setStyleSheet(QStringLiteral("border-radius: 5px;\n"
                                                         "border:1.5px solid;\n"
                                                         "padding : 3px;\n"
                                                         "color:#f4ed1f"));
              labelcontent->setFont(font);
              labelcontent->setTextInteractionFlags(Qt::TextSelectableByMouse);
              labelcontent->setAlignment(Qt::AlignCenter);
              WidthPos_X += (FontWidth+10);
              labelcontent->show();
            }
        }
      WidthPos_X = 0;
      HeightPos_Y += FontHeight;
    }
  ui->Synonym_enContents->adjustSize();
}

void MainWindow::DeleteAllScrollObject()
{
  ui->Synonym_enScrollArea->hide();
  ui->Synonym_zh_CNScrollArea->hide();
  QScrollBar *Synonym_enScrollBar = ui->Synonym_enScrollArea->verticalScrollBar();
  Synonym_enScrollBar->setValue(Synonym_enScrollBar->minimum());
  QScrollBar *Synonym_zh_CNScrollBar = ui->Synonym_zh_CNScrollArea->verticalScrollBar();
  Synonym_zh_CNScrollBar->setValue(Synonym_zh_CNScrollBar->minimum());
  QObjectList ListSynonym_en = ui->Synonym_enContents->children();
  QObjectList ListSynonym_zh_CN = ui->Synonym_zh_CNContents->children();
  if(!ListSynonym_en.isEmpty())
    {
      for(int i=0;i<ListSynonym_en.size();i++)
        {
          delete  ListSynonym_en.at(i);
//          ListSynonym_en.at(i)->deleteLater(); /*Can't delete object immediately*/
        }
    }
  if(!ListSynonym_zh_CN.isEmpty())
    {
      for(int j=0;j<ListSynonym_zh_CN.size();j++)
        {
          delete  ListSynonym_zh_CN.at(j);
//          ListSynonym_zh_CN.at(j)->deleteLater();
        }
    }
  return;
}
