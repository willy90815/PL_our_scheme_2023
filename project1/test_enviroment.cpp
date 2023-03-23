#include <iostream>
#include<fstream>
using namespace std;

enum token_type{
    LEFT_PAREN,       // '('
    RIGHT_PAREN,      // ')'z
    INT,              // e.g., '123', '+123', '-123'
    STRING,           // "string's (example)." (strings do not extend across lines)
    DOT,              // '.'
    FLOAT,            // '123.567', '123.', '.567', '+123.4', '-.123'
    NIL,              // 'nil' or '#f', but not 'NIL' nor 'nIL'
    T,                // 't' or '#t', but not 'T' nor '#T'
    QUOTE,            
    SYMBOL,
};
typedef struct _token_node {
    string token;
    int line;
    int column;
    _token_type token_type; 
    _token_node_ * next;
} token_node ;

class File{
    private:
    string filename;
    fstream file;
    char *entire_source_code;

    public:
    void set_file_name(string filename){
        this->filename = filename;
    } // setFilename

    void openfile(){
        file.open(filename.c_str(),ios_base::in );
        if (file.is_open()){
            cout<<"Open file "<<filename<<endl;
        }
        file.seekg(0,file.end);
        int file_size = file.tellg();
        entire_source_code = new char[file_size/sizeof(char)];
        entire_source_code[file_size]='\0';
        file.seekg(0,file.beg);
        file.read(entire_source_code,file_size);
    }
    void openfile(string filename){
        this->set_file_name(filename);
        file.open(filename.c_str(),ios_base::in );
        if (file.is_open()){
            cout<<"Open file "<<filename<<endl;
        }
        file.seekg(0,file.end);
        int file_size = file.tellg();
        entire_source_code = new char[file_size/sizeof(char)];
        entire_source_code[file_size]='\0';
        file.seekg(0,file.beg);
        file.read(entire_source_code,file_size);
    }

    void print_test(){
        cout<< entire_source_code;
    }
};
