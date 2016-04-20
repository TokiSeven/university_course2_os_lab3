/*
 * stdio.h needs for output/input data
 * sys/types.h needs for
 * sys/stat.h needs for description's structure of a file (directory)
 * dirent.h needs for structure of file
 * string.h needs for string's functions (strlen(..), strcpy(..), ..)
 * pwd.h needs for rebuild from uid to user's name
 * grp.h needs for rebuild from gid to group's name
 * time.h needs for format and correct displays time (creation of file)
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define MAX_LEN 55

//return 0 if arg has not been founded
int checkArg(int argc, char *argv[], char arg, int num);

//fix right path to directory (or file) in 'fileName'
void getFileName(int argc, char *argv[], char *fileName);

//printf information about file 'file' (with or without -ld)
void writeInfoAboutFile(struct dirent *file, int isL, int isD);

//they need for processing of file (when called writeInfoAboutFile(..))
//str - sourse's string
void getFileType(struct stat st, char *str);
void getFileRights(struct stat st, char *str);
void getFileLinksCount(struct stat st, char *str);
void getFileUserName(struct stat st, char *str);
void getFileGroupName(struct stat st, char *str);
void getFileSize(struct stat st, char *str);
void getFileDate(struct stat st, char *str);

//this function in stdlib.h did not worked
//it transports int to char*
void itoa(int val, char *string);

//this needs for right formating date
void formatdate(char str[], time_t val);

int main(int argc, char *argv[])
{
    /*
     * if there is parametr -l, isL will be 1, else 0
     * if there is parametr -d, isL will be 1, else 0
     */
    int isD, isL;
    isD = checkArg(argc, argv, 'd', 1);
    isL = checkArg(argc, argv, 'l', 1);

    //fileName will be right, after this function
    char fileName[MAX_LEN];
    getFileName(argc, argv, fileName);

    //status of our file
    struct stat statOfFile;

    //check status of this file/directory
    if (!stat(fileName, &statOfFile))
    {
        //if stat(..) returned normally
        //fl - current opened file
        struct dirent *fl;
        //is it a directory or a file?
        if (S_ISDIR(statOfFile.st_mode))
        {
            //it is a directory
            DIR *dir = opendir(fileName);
            while((fl = readdir(dir)) != NULL)
                if (fl->d_ino != 0)
                    writeInfoAboutFile(fl, isL, isD);
            closedir(dir);
        }
        else
        {
            //it is a file
            return -1;
        }
    }
    else
    {
        //if stat(..) returned unnormally
        return -1;
    }

    return 0;
}

int checkArg(int argc, char *argv[], char arg, int num)
{
    int result = 0;
    if (argc > 1)
    {
        if (num >= argc)
            num = 1;
        char *argms = argv[num];
        if (*argms == '-')
        {
            for(int i = 0; i < strlen(argms) && !result; i++)
                if (argms[i] == arg)
                    result = 1;
        }
        else
            result = 0;
    }
    return result;
}

void getFileName(int argc, char *argv[], char *fileName)
{
    //check number of arguments (may be you started program without they)
    if (argc >= 2)
    {
        if (argc >= 3)
        {
            strcpy(fileName, argv[2]);
        }
        else
        {
            if (argv[1][0] != '-')
                strcpy(fileName, argv[1]);
            else
                strcpy(fileName, "./");
        }
    }
    else
        strcpy(fileName, "./");
}

void writeInfoAboutFile(struct dirent *file, int isL, int isD)
{
    //variables
    char str[MAX_LEN];
    struct stat st;
    char fileName[MAX_LEN];

    //initialization
    strcpy(fileName, file->d_name);
    strcpy(str, "\0");
    stat(fileName, &st);

    if (!isD && (!strcmp(file->d_name, ".") || !strcmp(file->d_name, "..")))
        return;
    if (isD && !S_ISDIR(st.st_mode))
        return;

    if (isL)
    {
        getFileType(st, str);
        getFileRights(st, str);
        strcat(str, " ");
        getFileLinksCount(st, str);

        //sometimes i clear str
        //not to go beyond the border of an array
        printf("%s ", str);
        strcpy(str, "\0");

        strcat(str, " ");
        getFileUserName(st, str);
        strcat(str, " ");
        getFileGroupName(st, str);
        strcat(str, " ");
        getFileSize(st, str);

        //sometimes i clear str
        //not to go beyond the border of an array
        printf("%s ", str);
        strcpy(str, "\0");

        getFileDate(st, str);
        strcat(str, " ");
        printf("%s ", str);
    }
    //strcat(str, fileName);
    printf("%s\n", fileName);
}

void getFileType(struct stat st, char *str)
{
    char res[2];
    strcpy(res, "\0");

    switch(st.st_mode & S_IFMT)
    {
    case S_IFDIR://directory
        res[0] = 'd';
        break;
    case S_IFCHR://symbol-oriented
        res[0] = 'c';
        break;
    case S_IFBLK://block-oriented
        res[0] = 'b';
        break;
    case S_IFIFO://FIFO-file
        res[0] = 'p';
        break;
    case S_IFREG://default file
        res[0] = '-';
        break;
    default://understand
        res[0] = 'u';
    }

    res[1] = '\0';

    strcat(str, res);
}

void getFileRights(struct stat st, char *str)
{
    char res[MAX_LEN];
    strcpy(res, "\0");

    //for user
    strcat(res, (st.st_mode & S_IRUSR) ? "r" : "-");
    strcat(res, (st.st_mode & S_IWUSR) ? "w" : "-");
    strcat(res, (st.st_mode & S_IXUSR) ? "x" : "-");

    //for group
    strcat(res, (st.st_mode & S_IRGRP) ? "r" : "-");
    strcat(res, (st.st_mode & S_IWGRP) ? "w" : "-");
    strcat(res, (st.st_mode & S_IXGRP) ? "x" : "-");

    //for other
    strcat(res, (st.st_mode & S_IROTH) ? "r" : "-");
    strcat(res, (st.st_mode & S_IWOTH) ? "w" : "-");
    strcat(res, (st.st_mode & S_IXOTH) ? "x" : "-");

    strcat(str, res);
}

void getFileLinksCount(struct stat st, char *str)
{
    char res[MAX_LEN];
    strcpy(res, "\0");

    itoa(st.st_nlink, res);

    strcat(str, res);
}

void getFileUserName(struct stat st, char *str)
{
    char res[MAX_LEN];
    strcpy(res, "\0");

    strcpy(res, getpwuid(st.st_uid)->pw_name);

    strcat(str, res);
}

void getFileGroupName(struct stat st, char *str)
{
    char res[MAX_LEN];
    strcpy(res, "\0");

    strcpy(res, getgrgid(st.st_gid)->gr_name);

    strcat(str, res);
}

void getFileSize(struct stat st, char *str)
{
    char res[MAX_LEN];
    strcpy(res, "\0");

    itoa(st.st_size, res);

    strcat(str, res);
}

void getFileDate(struct stat st, char *str)
{
    char res[MAX_LEN];
    strcpy(res, "\0");

    formatdate(res, st.st_mtime);

    strcat(str, res);
}

void itoa(int val, char *string)
{
    char buf[MAX_LEN];
    strcpy(buf, "\0");
    char numbers[] = "0123456789";

    int i = MAX_LEN - 1;

    for(;val && i; --i, val /= 10)
    {
        char buff[MAX_LEN];
        strcpy(buff, "\0");
        buff[0] = numbers[val % 10];
        buff[1] = '\0';

        strcat(buff, buf);
        strcpy(buf, buff);
    }

    strcpy(string, buf);
}

void formatdate(char str[], time_t val)
{
    strftime(str, MAX_LEN, "%m %d %H:%M", localtime(&val));
}
