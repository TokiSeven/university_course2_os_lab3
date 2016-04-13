#include <stdio.h>
#include <QDir>
#include <QString>
#include <QDateTime>

bool checkArg(int argc, char *argv[], char arg, int num = 2);
QString getDir(int argc, char *argv[]);
QString getInfoAboutFile(const QFileInfo &fl, bool l);

int main(int argc, char *argv[])
{
    //create container for all files
    QFileInfoList content;

    //get directory from func getDir(..)
    //but before it can be file, and i check it by the 'smt'
    QFileInfo smt(getDir(argc, argv));
    QDir dir(getDir(argc, argv));

    if (smt.isDir())
    {
        //set content by directory from 'dir' (only directories and files)
        if (checkArg(argc, argv, 'd'))
            content = dir.entryInfoList(QStringList(QString("*")), QDir::Dirs | QDir::Files);
        else
            content = dir.entryInfoList(QStringList(QString("*")), QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    }
    else
    {
        //if smt is not a directory
        content.append(smt);
    }

    //go to all files
    for(int i = 0; i < content.size(); i++)
    {
        bool canWrite = true;

        QString str = getInfoAboutFile(content[i], checkArg(argc, argv, 'l'));

        //check our argument on '-d'
        if (checkArg(argc, argv, 'd'))
            //if argument - d, then we need only folders
            if (!content[i].isDir())
                canWrite = false;

        if (canWrite)
            printf("%s\n", str.toStdString().c_str());
    }

    return 0;
}

bool checkArg(int argc, char *argv[], char arg, int num)
{
    bool result = false;
    if (argc > 1)
    {
        if (num >= argc)
            num = 2;
        num--;
        QString argms = QString(argv[num]);
        if (argms[0].toLatin1() == '-')
        {
            for(int i = 0; i < argms.size() && !result; i++)
                if (argms[i].toLatin1() == arg)
                    result = true;
        }
        else
            result = false;
    }
    return result;
}

QString getDir(int argc, char *argv[])
{
    QString dir;
    if (argc >= 2)
    {
        if (argc >= 3)
        {
            dir = QString(argv[2]);
        }
        else
        {
            if (argv[1][0] != '-')
                dir = QString(argv[1]);
            else
                dir = QString("./");
        }
    }
    else
        dir = QString("./");
    return dir;
}

QString getInfoAboutFile(const QFileInfo &fl, bool l)
{
    QString str;
    str = fl.fileName();
    //if there is argument -l we write all info about file
    if (l)
    {
        //date's created
        str = fl.created().toString("MMM dd hh:mm ") + str;

        //size
        str = QString::number(fl.size()) + QString(" ") + str;

        //group (if platform does not supported, this will be null-string)
        str = fl.group() + " " + str;

        //user
        str = fl.owner() + " " + str;

        //rights
        QString rights;
        if (fl.isDir())
            rights += QString("d");
        else if (fl.isSymLink())
            rights += QString("l");
        else if (fl.isBundle())
            rights += QString("s");
        else if (fl.isExecutable())
            rights += QString("p");
        else if (fl.isFile())
            rights += QString("-");
        else
            rights += QString("b");

        rights += fl.permission(QFile::ReadOwner) ? "r" : "-";
        rights += fl.permission(QFile::WriteOwner) ? "w" : "-";
        rights += fl.permission(QFile::ExeOwner) ? "x" : "-";
        rights += fl.permission(QFile::ReadGroup) ? "r" : "-";
        rights += fl.permission(QFile::WriteGroup) ? "w" : "-";
        rights += fl.permission(QFile::ExeGroup) ? "x" : "-";
        rights += fl.permission(QFile::ReadOther) ? "r" : "-";
        rights += fl.permission(QFile::WriteOther) ? "w" : "-";
        rights += fl.permission(QFile::ExeOther) ? "x" : "-";

        str = rights + " " + str;
    }

    return str;
}
