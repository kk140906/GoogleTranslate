#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <qt_windows.h>
#include <QMainWindow>
#include <QTextCodec>
#include <QCursor>
#include <QPlainTextEdit>
#include <QDebug>
#include <QLabel>
#include <QMouseEvent>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QtMath>
#include <QString>
#include <QList>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QObject>
#include <QRegularExpression>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDateTime>
#include <QMessageBox>
#include <waitprogress.h>
#include <QFont>
#include <QFontMetrics>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QScrollBar>
#include <percentratio.h>
#include <QTimer>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SimulateCopy();
    void EmptyData();
    void GetData();
    void ShowWindow();
    void CreateTrayIcon();

    long long CalTokenMidValue(long long a, QString b);
    void CalOringinToken(QString text,QString tkk = "0");
    void UpdateTkk();
    void DeleteAllScrollObject();

signals:
    void UpdateTkkisfinished();
    void CalTokenisfinished();
    void ReadResultisfinished(QString);
    void ReadSynonym_enisfinished();
    void ReadSynonym_zh_CNisfinished();
    void DebugOutput(QString,int handle = _error);


protected:
    bool eventFilter(QObject *object,QEvent *event);

private:
    Ui::MainWindow *ui;
    enum ErrorStyle{_debug,_error,_warn,_info};
    bool errorFlag = false;

    QPoint mwRelativePos;//Mouse And MainWindow Relative Position;
    bool dragFlag=false;


    QString text;//The text is need to translate;
    bool textChangeFlag=false;
    QTimer *editTimer;
    int TextChangedCount;

    int ChineseCount = 0;
    int EnglishCount = 0;

    bool _isRunning = false;

    QSystemTrayIcon *systrayicon;
    QMenu *trayiconmenu;
    QAction *action_showmainwindow;
    QAction *action_exit;
    QAction *action_logcontrl;

    bool DebuglogFlag=false;



    QNetworkAccessManager *managerTKK;
    QNetworkRequest requestTKK;
    QNetworkReply *replyTKK;
    QString NewTkk;
    QString Token;
    QUrl url;
    QNetworkAccessManager *managerResult;
    QNetworkRequest requestResult;
    QNetworkReply *replyResult;
    QString TranslateResult;
    bool managerResultFlag = true;

    QJsonArray Synonym_zh_CN;
    QJsonArray Synonym_en;

    WaitProgress *waitProgess;

    QMessageBox msgBox;



private slots:
    void TrayIconAction(QSystemTrayIcon::ActivationReason);
    void ExitProgram()__attribute__((noreturn));
    void DebugLogConctrl();
    void ReadTkk();
    void TextisChanged();
    void CalToken();
    void WebRequest();
    void ReadResult();
    void ManagerResultisfinished();
    void showSynonym_en();
    void showSynonym_zh_CN();
    void DebugHandle(QString,int handle = _error);
    void TimerEnd();
};

#endif // MAINWINDOW_H
