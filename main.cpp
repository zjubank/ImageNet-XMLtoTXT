//
//  main.cpp
//  ImageNet-XMLtoTXT
//
//  Created by 尹航 on 16/5/2.
//  Copyright © 2016年 zjubank. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <vector>

using namespace std;

typedef struct xml
{
    string path;
    int type;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
}xmlfile;

string pathhead = "/home/zilin/Data/ImageNet/ILSVRC2015/Data/DET/train/";
string databasepath;
vector<xmlfile> txtfile;

void readdir(const char * dir);
void readxml(char* filename);
void writetxt(void);

int main(int argc, const char * argv[]) {
    const char * dir;
    if( argc >= 2 )
    {
        dir = argv[1];
    }

    readdir(dir);
    writetxt();
    
    return 0;
}

void readdir(const char * dir)
{
    DIR *dp;
    struct  dirent  *entry;
    struct  stat    statbuf;
    
    //先遍历一次目录，添加所有文件
    if ((dp = opendir(dir)) == NULL)
    {
        printf("Cannot open the dir.\n");
        return ;
    }
    //如果不能打开用户所给目录，程序退出
    else
    {
        string annotationpath = dir;
        string tmp = annotationpath.substr(annotationpath.length()-1);
        if( tmp != "/" )
        {
            annotationpath += "/";
        }
        //自动补全
        annotationpath = annotationpath.substr(0, annotationpath.rfind('/'));
        databasepath = annotationpath.substr(annotationpath.rfind('/')+1);
        cout << "ReadingDir: " << dir << endl;
        //cout << databasepath << endl;
        
        chdir(dir);
        while ( (entry = readdir(dp)) != NULL )
        {
//            cout << "InLoop: %00" << endl;
            lstat( entry->d_name, &statbuf );
            if ( S_ISDIR(statbuf.st_mode) )
            {
                if ( strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0 )
                {
                    continue;
                }
                //排除Linux系统下的隐藏文件和上级目录
            }
            else
            {
                char filename[] = "";
                strcat(filename, entry->d_name);
                
                //TODO
                cout << "ReadingFile: " << filename << endl;
                readxml(filename);
            }
//            cout << "OutHere: %00" << endl;
        }
        chdir("..");
        closedir(dp);
    }
    
}

void readxml(char* filename)
{
    char buffer[256];
    ifstream in(filename);
    
    xmlfile thisxmlfile;
    thisxmlfile.type = 1;
    
    if (! in.is_open())
    {
        cout << "Cannot open the file" << endl;
        exit(1);
    }
    while (!in.eof())
    {
        in.getline(buffer, 256);

        string linebuf = buffer;
        //linebuf: 一行
        
        size_t filenamepos = linebuf.find("<filename>");
        size_t xminpos = linebuf.find("<xmin>");
        size_t xmaxpos = linebuf.find("<xmax>");
        size_t yminpos = linebuf.find("<ymin>");
        size_t ymaxpos = linebuf.find("<ymax>");
        if( filenamepos != string::npos )
        {
            string filename = linebuf.substr( filenamepos + 10, linebuf.find("</filename>") - filenamepos - 10 );
            thisxmlfile.path = pathhead + databasepath + "/" + filename + ".JPGE";
        }
        
        if( xminpos != string::npos )
        {
            string xmin = linebuf.substr( xminpos + 6, linebuf.find("</xmin>") - xminpos - 6 );
            thisxmlfile.xmin = stoi(xmin);
        }
        if( xmaxpos != string::npos )
        {
            string xmax = linebuf.substr( xmaxpos + 6, linebuf.find("</xmax>") - xmaxpos - 6 );
            thisxmlfile.xmax = stoi(xmax);
        }
        if( yminpos != string::npos )
        {
            string ymin = linebuf.substr( yminpos + 6, linebuf.find("</ymin>") - yminpos - 6 );
            thisxmlfile.ymin = stoi(ymin);
        }
        if( ymaxpos != string::npos )
        {
            string ymax = linebuf.substr( ymaxpos + 6, linebuf.find("</ymax>") - ymaxpos - 6 );
            thisxmlfile.ymax = stoi(ymax);
            
            txtfile.push_back(thisxmlfile);
//            cout << "Push +1." << endl;
        }
    }
    
    return;
}

void writetxt(void)
{
    ofstream out;
    out.open("/Users/ZJUBank/Desktop/train.txt", ofstream::app);

    vector<xmlfile>::iterator it;
    for( it = txtfile.begin(); it != txtfile.end(); it++ )
    {
        out << it->path << " " << it->type << " " << it->xmin << " " << it->xmax << " " << it->ymin << " " << it->ymax << endl;
    }
    
    out.close();
    
    return ;
}