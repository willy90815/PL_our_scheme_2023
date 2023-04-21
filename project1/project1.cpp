#include <stdio.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>

using namespace std;

enum Token_type {
    LEFT_PAREN = 0,
    RIGHT_PAREN,
    INT,
    STRING,
    DOT,
    FLOAT,
    NIL,
    T,
    QUOTE,
    SYMBOL
};

struct Token_Node {
    int line;
    int column;
    int last_column;
    string token;
    Token_type type;
    Token_Node *next;
};

typedef Token_Node *tn_ptr;

class Civilize {
   public:
    string Turn_to_string(int num) {
        string str = "";
        int index = 0;
        if (num < 0) {
            num = -num;
            str = "-";
            index++;
        }  // if

        while (num > 0) {
            char ch = num % 10 + '0';
            str.insert(index, 1, ch);
            num = num / 10;
        }  // while

        return str;
    }  // Turn_to_string()

    string Turn_to_string(float num) {
        string str = "";
        int int_num = num;
        str = Turn_to_string(int_num);
        if (str.compare("") == 0) {
            str = "0";
        }  // if

        if (num < 0) {
            num = num * -1;
        }  // if

        num = num - int_num;
        str = str + '.';
        num = num * 10000;
        int_num = num;
        int_num = int_num / 10 * 10;
        if (num - int_num - 5 > -0.1) {
            num = num + 10;
        }  // if

        num = num / 10000;

        for (int i = 0; i < 3; i++) {
            num = num * 10;
            int temp = num;
            char ch = temp % 10 + '0';
            str = str + ch;
        }  // for

        return str;
    }  // Turn_to_string()

    float To_float(string str) {
        float reg = 0;
        float magnitude = 1;
        bool dot_flag = false;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == '.') {
                dot_flag = true;
                magnitude = 1;
            }  // if
            else {
                if (dot_flag) {
                    magnitude = magnitude * 0.1;
                    int temp = str[i] - '0';
                    reg = reg + temp * magnitude;
                }  // if

                else {
                    int temp = str[i] - '0';
                    reg = reg * 10 + temp;
                }  // else
            }      // else
        }          // for

        return reg;
    }  // To_float()
};

class Cut : protected Civilize {
    int m_code_line;
    int m_code_column;
    string m_one_line_code;
    bool m_eol;  // end of line

    char Getch() {
        m_code_column++;
        if (m_one_line_code[m_code_column - 2] == '\0') {
            m_eol = true;
        }  // if
        return m_one_line_code[m_code_column - 2];
    }  // Getch()

    void Put_ch_back() {
        m_code_column--;
    }  // Put_ch_back()

    bool Is_digit(char ch) {
        if ('0' <= ch && ch <= '9') {
            return true;
        }  // if

        return false;
    }  // Is_digit()

    bool Is_letter(char ch) {
        if ('a' <= ch && ch <= 'z') {
            return true;
        }  // if

        if ('A' <= ch && ch <= 'Z') {
            return true;
        }  // if

        return false;
    }  // Is_letter()

    bool Is_white_space(char ch) {
        if (ch == ' ' || ch == '\t' || ch == '\n') {
            return true;
        }  // if

        return false;
    }  // Is_white_space()

    bool Is_seperator(char ch) {
        if (ch == '(' || ch == ')' || ch == '\'') {
            return true;
        }  // if
        else if (ch == '\"' || Is_white_space(ch) || Is_comment(ch)) {
            return true;
        }  // if
        else if (ch == '\0') {
            return true;
        }  // if replace '\n'

        return false;
    }  // Is_seperator()

    bool Is_dot(char ch) {
        if (ch == '.') {
            return true;
        }  // if

        return false;
    }  // Is_dot()

    bool Is_end_of_line(char ch) {
        if (ch == '\0') {
            // m_end_of_line = true;
            return true;
        }  // if

        return false;
    }  // Is_end_of_line()

    bool Is_double_quote(char ch) {
        if (ch == '\"') {
            return true;
        }  // if

        return false;
    }  // Is_double_quote()

    bool Is_quote(char ch) {
        if (ch == '\'') {
            return true;
        }  // if

        return false;
    }  // Is_quote()

    bool Is_comment(char ch) {
        if (ch == ';') {
            return true;
        }  // if

        return false;
    }  // Is_comment()

    char Skip_white_space(char ch) {
        while (Is_white_space(ch)) {
            ch = Getch();
        }  // while

        return ch;
    }  // Skip_white_space()

    tn_ptr Creat_new_node() {
        tn_ptr temp = new Token_Node;
        temp->column = m_code_column - 1;
        temp->line = m_code_line;
        temp->last_column = m_code_column - 1;
        temp->token = "";
        temp->type = Token_type::NIL;
        temp->next = NULL;
        return temp;
    }  // Creat_new_node()

    bool Start_with_digit(char ch, tn_ptr &re_ptr) {
        // int     '1234'
        // float   '1234.','1234.234'
        // symbol '123.12.','123.h', ........

        if (!Is_digit(ch)) {
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr = Creat_new_node();
        re_ptr->type = Token_type::INT;
        bool dot_appear = false;

        while (Is_digit(ch)) {
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        }  // while

        if (Is_dot(ch)) {
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
            re_ptr->type = Token_type::FLOAT;
            dot_appear = true;
            while (Is_digit(ch)) {
                re_ptr->token = re_ptr->token + ch;
                ch = Getch();
            }  // while
        }      // if

        while (!Is_seperator(ch)) {
            re_ptr->type = Token_type::SYMBOL;
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        }  // while

        if ( ! Is_white_space(ch) ) {
            Put_ch_back();
        } // if

        re_ptr->last_column = m_code_column - 2;
        return true;

    }  // Start_with_digit()

    bool Strat_with_letter(char ch, tn_ptr &re_ptr) {

        if( ! Is_letter( ch )){
            re_ptr = NULL;
            return false;
        } // if

        re_ptr = Creat_new_node();
        re_ptr->type = Token_type::SYMBOL;

        while ( ! Is_seperator( ch )){
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        } // while


        if ( ! Is_white_space(ch) ) {
            Put_ch_back();
        } // if

        if( re_ptr->token.compare("t") == 0 ){
            re_ptr->type = Token_type::T;
        } // if 

        if( re_ptr->token.compare("nil") == 0 ){
            re_ptr->type = Token_type::NIL;
        } // if

        if( re_ptr->token.compare("quote") == 0 ){
            re_ptr->type = Token_type::LEFT_PAREN;
            re_ptr->token = "(";
            re_ptr->next = Creat_new_node();
            re_ptr->token = "quote";
            re_ptr->type = Token_type::QUOTE;
        } // if

        if ( ! Is_white_space(ch) ) {
            Put_ch_back();
        } // if
        return true;
    }  // Strat_with_letter()

    bool Strat_with_dot(char ch, tn_ptr &re_ptr) {

        if ( ! Is_dot( ch ) ) {
            re_ptr = NULL;
            return false;
        } // if

        re_ptr = Creat_new_node();
        re_ptr->type = Token_type::DOT;
        while (Is_digit(ch)) {
            re_ptr->token = re_ptr->token + ch;
            re_ptr->type = Token_type::FLOAT;
        }  // while

        while( Is_seperator( ch )){
            re_ptr->token = re_ptr->token + ch;
            re_ptr->type = Token_type::SYMBOL;
        } // while

        if ( ! Is_white_space(ch) ) {
            Put_ch_back();
        } // if
        return true;
    }  // Strat_with_dot()

    bool Strat_with_quote(char ch, tn_ptr &re_ptr) {

        if( ! Is_quote( ch )){
            re_ptr = NULL;
            return false;
        } // if

        re_ptr = Creat_new_node();
        re_ptr->type = Token_type::LEFT_PAREN;
        re_ptr->token = "(";
        re_ptr->next = Creat_new_node();
        re_ptr->next->type = Token_type::QUOTE;
        re_ptr->token = "quote";

        return true;
    }  // Strat_with_quote()

    bool Start_with_double_quote(char ch, tn_ptr &re_ptr) {
        if( Is_double_quote( ch )){
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr->type = Token_type::STRING;
        re_ptr->token = ch;
        ch = Getch();

        while( ! Is_double_quote( ch ) && ! m_eol ) {
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        } // while

        return true;
    }  // Start_with_double_quote()

    bool Strat_with_comment(char ch, tn_ptr &re_ptr) {
        if(! Is_comment( ch )){
            re_ptr = NULL;
            return false;
        } // if

        while( ch != '\0' ){
            ch = Getch();
        } // while
        

        return true;
    }  // Strat_with_comment()

    bool Start_with_left_paren(char ch, tn_ptr &re_ptr) {
        return true;
    }  // Start_with_left_paren()

    bool Start_with_right_paren(char ch, tn_ptr &re_ptr) {
        return true;
    }  // Start_with_right_paren()

    bool Start_with_symbol(char ch, tn_ptr &re_ptr) { 
        return true;
    }  // Start_with_symbol()

    void Set_one_line_code() {
        if (m_one_line_code[m_code_column-1] == '\0') {
            getline(cin, m_one_line_code);
            m_code_column = 1;
            m_eol = false;
        }  // if
    }      // Get_code_line()

   public:
    tn_ptr Get_token_node() {
        tn_ptr re_ptr = NULL;
        stringstream ss;
        if (m_eol) {
            Set_one_line_code();
        }  // if
        char ch = Getch();
        Start_with_digit(ch, re_ptr);
        Strat_with_letter(ch, re_ptr);

        if (re_ptr != NULL) {
            if (re_ptr->type == Token_type::FLOAT) {
                re_ptr->token = Turn_to_string(To_float(re_ptr->token));
            }  // if
        }
        return re_ptr;
    }  // Get_token_node

    tn_ptr Get_token_line() {
        if (m_eol) {
            Set_one_line_code();
        }  // if

        while (!m_eol) {
            Get_token_node();
        }  // while
        return NULL;
    }  // Get_token_line()

    void Test_print(tn_ptr walk) {
        cout << setw(5) << "line" << setw(7) << "Column" << setw(15) << "last_column";
        cout << setw(25) << "token" << setw(10) << "type" << endl;
        cout << "=================================================================================" << endl;
        while (walk != NULL) {
            cout << setw(5) << walk->line << setw(7) << walk->column << setw(15) << walk->last_column;
            cout << setw(25) << walk->token << setw(10) << walk->type << endl;
            walk = walk->next;
        }  // while
        cout << "=================================================================================" << endl;
    }  // Test_print()

    Cut() {
        m_code_line = 0;
        m_code_column = 1;
        m_eol = true;
        m_one_line_code = "";
    }
};

int main() {
    Cut cutter = Cut();
    while (true) {
        cutter.Test_print(cutter.Get_token_node());
    }  // while

}  // main()