#include "csvwriter.h"

CSVWriter::CSVWriter(){}

void CSVWriter::open(const char *fileName){
    fout.open(fileName,std::ios::app);
}

CSVWriter::CSVWriter(const char* fileName) {
    //this->fileName=fileName;
    fout.open(fileName,std::ios::app);
}

void CSVWriter::write(const char *data) {
    fout<<data;
    fout.flush();
}
