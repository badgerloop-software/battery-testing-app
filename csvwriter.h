#ifndef UNTITLED1_CSVWRITER_H
#define UNTITLED1_CSVWRITER_H


#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <iostream>
#include <fstream>

class CSVWriter {
public:
    CSVWriter();
    CSVWriter(const char* fileName);
    void open(const char* fileName);
    void write(const char *data);
    void close() {fout.close();};
    void remove(const char* fileName);
private:
    std::ofstream fout;
};

#endif // CSVWRITER_H


#endif //UNTITLED1_CSVWRITER_H
