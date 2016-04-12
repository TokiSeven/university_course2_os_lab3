#include <stdio.h>
#include <QDir>
#include <QString>
#include <QDateTime>

bool checkArg(int argc, char *argv[], char arg, int num = 2);
QString getDir(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    //create container for all files
    QFileInfoList content;

    //get directory from func getDir(..)
    QDir dir(getDir(argc, argv));

    //printf("----->>>>>Directory: %s\n", dir.path().toStdString().c_str());

    //set content by directory from 'dir' (only directories and files)
    content = dir.entryInfoList(QStringList(QString("*")), QDir::Dirs | QDir::Files);

    //go to all files
    for(int i = 0; i < content.size(); i++)
    {
        bool canWrite = true;
        QString str;
        str = content[i].fileName();
        //if there is argument -l we write all info about file
        if (checkArg(argc, argv, 'l'))
        {
            //date's created
            str = content[i].created().toString("MMM dd hh:mm ") + str;

            //size
            str = QString::number(content[i].size()) + QString(" ") + str;

            //group (if platform does not supported, this will be null-string)
            str = content[i].group() + " " + str;

            //user
            str = content[i].owner() + " " + str;

            //rights
            QString rights;
            if (content[i].isDir())
                rights += QString("d");
            else if (content[i].isSymLink())
                rights += QString("l");
            else if (content[i].isBundle())
                rights += QString("s");
            else if (content[i].isExecutable())
                rights += QString("p");
            else if (content[i].isFile())
                rights += QString("-");
            else
                rights += QString("b");

            rights += content[i].permission(QFile::ReadOwner) ? "r" : "-";
            rights += content[i].permission(QFile::WriteOwner) ? "w" : "-";
            rights += content[i].permission(QFile::ExeOwner) ? "x" : "-";
            rights += content[i].permission(QFile::ReadGroup) ? "r" : "-";
            rights += content[i].permission(QFile::WriteGroup) ? "w" : "-";
            rights += content[i].permission(QFile::ExeGroup) ? "x" : "-";
            rights += content[i].permission(QFile::ReadOther) ? "r" : "-";
            rights += content[i].permission(QFile::WriteOther) ? "w" : "-";
            rights += content[i].permission(QFile::ExeOther) ? "x" : "-";

            str = rights + " " + str;
        }

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
