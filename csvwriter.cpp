#include "csvwriter.h"

CSVWriter::CSVWriter(){}

void CSVWriter::open(const char *fileName) {
    fout.open(fileName, std::ofstream::out | std::ofstream::trunc);
    fout.close();
    fout.open(fileName, std::ios::app);
}

CSVWriter::CSVWriter(const char* fileName) {
    fout.open(fileName, std::ofstream::out | std::ofstream::trunc);
    fout.close();
    fout.open(fileName, std::ios::app);
    fout.is_open();
}

void CSVWriter::write(const char *data) {
    fout<<data;
    fout.flush();
}

void CSVWriter::remove(const char* fileName) {
    fout.close();
    std::remove(fileName);
}
