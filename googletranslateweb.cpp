#include <googletranslateweb.h>
GoogleTranslateWeb::GoogleTranslateWeb(QWidget *parent) : QMainWindow(parent){}

GoogleTranslateWeb::~GoogleTranslateWeb(){}

long long GoogleTranslateWeb::CalTokenMidValue(long long a, QString b)
{
    for (int c = 0; c < b.size() -2; c += 3)
    {
        QChar d = b.at(c+2);
        int e;
        //result 2 must be a  digit char
        e = d >= 'a' ? d.unicode() - 87 : d.digitValue();
        e = '+' == b.at(c+1) ? (a % 0x100000000) >> e : a << e;
        a = '+' == b.at(c) ? (a + e) & 4294967295 : a ^ e;
    }
    return a;
}

QString GoogleTranslateWeb::CalOringinToken(QString text,QString tkk)
{
    if(text.size()>4500) return QString("text too long");
    QList<int> a;
    for (int i=0;i<text.size();i++)
    {
        int TextUnicode=(int)text.at(i).unicode();
        int count = TextUnicode < 0x10000 ? 1 : 2;
        switch (count)
        {
            case 1:
                  a.append(TextUnicode);
                break;
            case 2:
                a.append(qFloor((TextUnicode - 0x10000)/0x400 + 0xD800));
                a.append(qFloor((TextUnicode - 0x10000)%0x400 + 0xDC00));
            default:
                break;
        }
    }
    QString b = tkk != "0" ? tkk : "";
    qDebug() <</* */"tkk" <<b;
    QList<QString>  c= b.split('.');
    long long d = c.size() > 1 ? c.at(0).toLongLong() : 0;
//    qDebug() << "d" <<d;
    QList<int> e;
    for(int j=0;j<text.size();j++)
    {
        int f = a.at(j);
        if (f < 128) e.append(f);
        else
        {
            if (f < 2048) e.append(f >> 6 | 192);
            else
            {
                if ((f & 64512) == 55296 && j+1<text.size() && (a.at(j+1) & 64512) == 56320)
                {
                    j++;
                    f = 65536 + ((1 & 1023) << 10) +(a.at(j) & 1023);
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
//    qDebug() << "e" <<e;
    long long g = d;
    for(int k=0;k<e.size();k++)
    {
        g += e.at(k);
        g = CalTokenMidValue(g,"+-a^+6");
    }
//    qDebug() << "g" <<g;
    g = CalTokenMidValue(g,"+-3^+b+-f");
//    qDebug() << "g" <<g;
    //
    g ^= c.size() > 1 ? c.at(1).toLongLong() : 0;
    if (g < 0) g = (g & 2147483647) + 2147483648;
    g %= 1000000;
    qDebug() <<QString("%1.%2").arg(g).arg(g ^ d);
    return QString("%1.%2").arg(g).arg(g ^ d);
}

void GoogleTranslateWeb::UpdateTkk()
{

    if(!NewTkk.isEmpty())
    {
        QList<QString>  NewTkkList= NewTkk.split('.');
        long long TkkList = NewTkkList.size() > 1 ? NewTkkList.at(0).toLongLong() : 0;
        long long current = floor(QDateTime::currentMSecsSinceEpoch()/3600000.0);
        if(current == TkkList) return;
    }
    managerTKK=new QNetworkAccessManager(this);
    requestTKK->setUrl(QUrl("https://translate.google.com/"));
    replyTKK=managerTKK->get(*requestTKK);
    QObject::connect(replyTKK,SIGNAL(readyRead()),this,SLOT(ReadTkk()));
}

void GoogleTranslateWeb::ReadTkk()
{

    QString reponse=replyTKK->readAll();
    if(!reponse.isEmpty())
    {
        QRegularExpression re_tkk("tkk:\'(.+?)\'");
        QString matched = re_tkk.match(&reponse).captured(1);
        if (!matched.isEmpty())
        {
            NewTkk = matched;
            qDebug() << NewTkk;
            qDebug() << "done";
            managerTKK->deleteLater();
            replyTKK->deleteLater();
        }
    }
}
