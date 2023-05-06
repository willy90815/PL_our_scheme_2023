#include <stdio.h>
#include <stdlib.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

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
    vector<Token_Node> *secondary;
    Token_Node *next;
};

typedef Token_Node *Tn_ptr;
typedef vector<Token_Node> *Vec_ptr;

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
        stringstream ss;
        string re_str;
        ss << fixed << setprecision(3) << num;
        re_str = ss.str();

        if (re_str.find_first_of('.', 0) == -1) {
            re_str = re_str + ".";
        }  // if

        while (re_str.find_first_of('.', 0) + 4 > re_str.size()) {
            re_str = re_str + "0";
        }  // while

        while (re_str.find_first_of('.', 0) + 4 < re_str.size()) {
            re_str.erase(re_str.length() - 1);
        }  // while

        return re_str;
    }  // Turn_to_string()

    float To_float(string str) {
        stringstream ss;
        ss << str;
        float num = 0;
        ss >> num;
        return num;
    }  // To_float()
};

class Cut : protected Civilize {
    int m_code_line;
    int m_code_column;
    string m_one_line_code;
    int m_column_offset;
    bool m_eol;  // end of line
    int m_quote_count;
    stack<Tn_ptr> m_lparen_stack;

    char Getch() {
        m_code_column++;
        if (m_eol) {
            return '\0';
        }  // if
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
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\0') {
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

    char Skip_white_space() {
        char ch;
        ch = Getch();
        while (Is_white_space(ch)) {
            ch = Getch();
            if (m_eol) {
                Set_one_line_code();
            }  // if
        }      // while

        return ch;
    }  // Skip_white_space()

    bool Quote_exist() {
        if (m_quote_count > 0) {
            return true;
        }  // if
        return false;
    }  // Quote_exist()

    Tn_ptr Creat_new_node() {
        Tn_ptr temp = new Token_Node;
        temp->column = m_code_column - 1 - m_column_offset;
        temp->line = m_code_line;
        temp->last_column = m_code_column - 1 - m_column_offset;
        temp->token = "";
        temp->type = NIL;
        temp->next = NULL;
        return temp;
    }  // Creat_new_node()

    bool Start_with_digit(char ch, Tn_ptr &re_ptr) {
        // int     '1234'
        // float   '1234.','1234.234'
        // symbol '123.12.','123.h', ........

        if (!Is_digit(ch)) {
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr = Creat_new_node();
        re_ptr->type = INT;
        bool dot_appear = false;

        while (Is_digit(ch)) {
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        }  // while

        if (Is_dot(ch)) {
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
            re_ptr->type = FLOAT;
            dot_appear = true;
            while (Is_digit(ch)) {
                re_ptr->token = re_ptr->token + ch;
                ch = Getch();
            }  // while
        }      // if

        while (!Is_seperator(ch)) {
            re_ptr->type = SYMBOL;
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        }  // while

        Put_ch_back();

        re_ptr->last_column = m_code_column - 2 - m_column_offset;
        return true;

    }  // Start_with_digit()

    bool Start_with_letter(char ch, Tn_ptr &re_ptr) {
        if (!Is_letter(ch)) {
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr = Creat_new_node();
        re_ptr->type = SYMBOL;
        while (!Is_seperator(ch)) {
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        }  // while

        Put_ch_back();

        if (re_ptr->token.compare("t") == 0) {
            re_ptr->token = "#t";
            re_ptr->type = T;
        }  // if

        if (re_ptr->token.compare("nil") == 0) {
            re_ptr->type = NIL;
        }  // if

        if (re_ptr->token.compare("quote") == 0) {
            re_ptr->type = LEFT_PAREN;
            re_ptr->token = "(";
            re_ptr->next = Creat_new_node();
            re_ptr->token = "quote";
            re_ptr->type = QUOTE;
        }  // if

        return true;
    }  // Start_with_letter()

    bool Start_with_dot(char ch, Tn_ptr &re_ptr) {
        if (!Is_dot(ch)) {
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr = Creat_new_node();
        re_ptr->type = DOT;
        re_ptr->token = ch;
        ch = Getch();
        while (Is_digit(ch)) {
            re_ptr->token = re_ptr->token + ch;
            re_ptr->type = FLOAT;
            ch = Getch();
        }  // while

        while (!Is_seperator(ch)) {
            re_ptr->token = re_ptr->token + ch;
            re_ptr->type = SYMBOL;
            ch = Getch();
        }  // while

        Put_ch_back();
        return true;
    }  // Start_with_dot()

    bool Start_with_quote(char ch, Tn_ptr &re_ptr) {
        if (!Is_quote(ch)) {
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr = Creat_new_node();
        re_ptr->type = LEFT_PAREN;
        re_ptr->token = "(";
        re_ptr->next = Creat_new_node();
        re_ptr->next->type = QUOTE;
        re_ptr->next->token = "quote";
        m_quote_count++;
        Align_paren(re_ptr);
        return true;
    }  // Start_with_quote()

    bool Start_with_double_quote(char ch, Tn_ptr &re_ptr) {
        if (!Is_double_quote(ch)) {
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr = Creat_new_node();
        re_ptr->type = STRING;
        re_ptr->token = ch;
        ch = Getch();

        while (!Is_double_quote(ch) && !m_eol) {
            if (ch == '\\') {
                ch = Get_backslash_series();
            }  // if

            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        }  // while

        if (Is_double_quote(ch)) {
            re_ptr->token = re_ptr->token + ch;
        }  // if

        else if (m_eol) {
            string error_mes;
            error_mes = "ERROR (no closing quote) : END-OF-LINE encountered at Line " 
            + Turn_to_string(m_code_line) + " Column " 
            + Turn_to_string(m_code_column - 1 - m_column_offset);
            throw(error_mes);
        }  // if

        return true;
    }  // Start_with_double_quote()

    char Get_backslash_series() {
        char ch = Getch();
        if (ch == 'n') {
            return '\n';
        }  // if

        else if (ch == 't') {
            return '\t';
        }  // if

        else if (ch == '\\') {
            return '\\';
        }  // if

        else if (ch == '\'') {
            return '\'';
        }  // if

        else if (ch == '\"') {
            return '\"';
        }  // if

        else {
            Put_ch_back();
            return '\\';
        }  // else
    }      // Get_backslash_series()

    bool Start_with_comment(char ch, Tn_ptr &re_ptr) {
        if (!Is_comment(ch)) {
            re_ptr = NULL;
            return false;
        }  // if

        while (ch != '\0') {
            ch = Getch();
        }  // while

        return true;
    }  // Start_with_comment()

    bool Start_with_left_paren(char ch, Tn_ptr &re_ptr) {
        if (ch != '(') {
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr = Creat_new_node();
        re_ptr->type = LEFT_PAREN;
        re_ptr->token = "(";
        if (Quote_exist()) {
            m_lparen_stack.push(re_ptr);
        }  // if

        re_ptr->next = Get_token_node();

        if (re_ptr->next->type == RIGHT_PAREN) {
            re_ptr->token = "nil";
            delete re_ptr->next;
            re_ptr->next = NULL;
            re_ptr->type = NIL;
        }  // if
        return true;
    }  // Start_with_left_paren()

    bool Start_with_right_paren(char ch, Tn_ptr &re_ptr) {
        if (ch != ')') {
            re_ptr = NULL;
            return false;
        }  // if

        re_ptr = Creat_new_node();
        re_ptr->type = RIGHT_PAREN;
        re_ptr->token = ")";

        return true;
    }  // Start_with_right_paren()

    bool Start_with_other_symbol(char ch, Tn_ptr &re_ptr) {
        re_ptr = Creat_new_node();

        if (ch == '#') {
            re_ptr->token = ch;
            re_ptr->type = SYMBOL;
            ch = Getch();
            if (ch == 'f') {
                re_ptr->token = re_ptr->token + ch;
                re_ptr->type = NIL;
            }  // if
            else if (ch == 't') {
                re_ptr->token = re_ptr->token + ch;
                re_ptr->type = T;
            }  // if
            else {
                if (!Is_seperator(ch)) {
                    re_ptr->token = re_ptr->token + ch;
                }  // if
                else {
                    Put_ch_back();
                }  // else

            }  // else

            ch = Getch();
        }  // if
        else if (ch == '-' || ch == '+') {
            re_ptr->token = ch;
            re_ptr->type = SYMBOL;
            ch = Getch();
            if (Is_digit(ch) || Is_dot(ch)) {
                Put_ch_back();
                Tn_ptr temp = Get_token_node();
                if (temp->type == DOT) {
                    temp->type = SYMBOL;
                }  // if
                if (re_ptr->token.compare("-") == 0) {
                    temp->token.insert(0, re_ptr->token);
                }  // if
                delete re_ptr;
                re_ptr = temp;
            }  // if
            else {
                Put_ch_back();
            }  // else

            ch = Getch();
        }  // else if

        while (!Is_seperator(ch)) {
            re_ptr->type = SYMBOL;
            re_ptr->token = re_ptr->token + ch;
            ch = Getch();
        }  // while
        if (re_ptr->type == NIL) {
            re_ptr->token = "nil";
        }  // if

        Put_ch_back();

        return true;
    }  // Start_with_other_symbol()

    void Set_one_line_code() {
        if (m_eol) {
            if (cin.eof()) {
                string error_str = "EOF";
                throw(error_str);  // error occurred
            }                      // if
            char *temp = new char[256];
            cin.getline(temp, 256);
            m_one_line_code = temp;
            m_code_column = 1;
            m_column_offset = 0;
            m_code_line++;
            m_eol = false;
        }  // if
    }      // Get_code_line()

    void Align_paren(Tn_ptr re_ptr) {
        Tn_ptr tail = Walk_to_last_node(re_ptr);
        int align_count = m_lparen_stack.size();
        tail->next = Get_token_node();
        while (align_count < m_lparen_stack.size()) {
            tail = Walk_to_last_node(re_ptr);
            tail->next = Get_token_node();
        }  // while

        tail = Walk_to_last_node(re_ptr);
        tail->next = Creat_new_node();
        tail->next->type = RIGHT_PAREN;
        tail->next->token = ")";
        m_quote_count--;
        return;

    }  // Align_paren()

   public:
    Tn_ptr Walk_to_last_node(Tn_ptr walk) {
        while (walk->next != NULL) {
            walk = walk->next;
        }  // while

        return walk;
    }  // Walk_to_last_node()
    Tn_ptr Get_token_node() {
        Tn_ptr re_ptr = NULL;

        while (re_ptr == NULL) {
            char ch = Skip_white_space();
            if (m_eol) {
                Set_one_line_code();
            }  // if
            if (Start_with_digit(ch, re_ptr)) {
                ;
            }  // if
            else if (Start_with_letter(ch, re_ptr)) {
                ;
            }  // if
            else if (Start_with_dot(ch, re_ptr)) {
                ;
            }  // if
            else if (Start_with_quote(ch, re_ptr)) {
                ;
            }  // if
            else if (Start_with_double_quote(ch, re_ptr)) {
                ;
            }  // if
            else if (Start_with_comment(ch, re_ptr)) {
                ;
            }  // if
            else if (Start_with_left_paren(ch, re_ptr)) {
            }  // if
            else if (Start_with_right_paren(ch, re_ptr)) {
                if (Quote_exist()) {
                    m_lparen_stack.pop();
                }  // if
            }      // if
            else if (Start_with_other_symbol(ch, re_ptr)) {
                ;
            }  // if

            if (re_ptr != NULL) {
                if (re_ptr->type == FLOAT) {
                    re_ptr->token = Turn_to_string(To_float(re_ptr->token));
                }  // if
            }      // if
        }          // while

        // Test_print(re_ptr);
        return re_ptr;
    }  // Get_token_node

    Tn_ptr Get_token_list() {
        Tn_ptr re_head;
        Tn_ptr re_ptr = NULL;
        if (m_eol) {
            Set_one_line_code();
        }  // if

        re_ptr = Get_token_node();
        re_head = re_ptr;

        while (!m_eol) {
            re_ptr->next = Get_token_node();
            re_ptr = Walk_to_last_node(re_ptr);
        }  // while

        return re_head;
    }  // Get_token_line()

    void Reset_column_line() {
        char ch = m_one_line_code[m_code_column];
        if ((m_eol) &&
            (m_one_line_code[m_code_column] == '\0' || m_one_line_code[m_code_column - 1] == '\0')) {
            m_code_line = 0;
        }  // if
        else {
            int rest_column = m_code_column - 1;
            while (m_one_line_code[rest_column] != '\0' && Is_white_space(m_one_line_code[rest_column])) {
                rest_column++;
            }  // while

            if (m_one_line_code[rest_column] == '\0') {
                m_code_line = 0;
                m_eol = true;
            }  // if
            else {
                m_code_line = 1;
            }  // else
        }      // else
        m_column_offset = m_code_column - 1;
    }  // Reset_column_line()

    void Test_print(Tn_ptr walk) {
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
        m_code_line = 1;
        m_code_column = 1;
        m_eol = true;
        m_one_line_code = "";
        m_column_offset = 0;
        m_quote_count = 0;
    }  // Cut()
    void Clear() {
        m_code_line = 1;
        m_code_column = 1;
        m_eol = true;
        m_one_line_code = "";
        m_column_offset = 0;
        m_quote_count = 0;
        while (!m_lparen_stack.empty()) {
            m_lparen_stack.pop();
        }  // while
    }      // Cut()
};
class Analyze : protected Civilize {
    Tn_ptr m_code_head;
    Tn_ptr m_ptr_index;
    Tn_ptr m_cur;
    Tn_ptr m_insulation;
    Cut m_cutter;
    void Push_back(Tn_ptr node) {
        m_insulation = node;
    }  // Push_back()
    void Update_m_cur() {
        if (m_insulation != NULL) {
            m_cur = m_insulation;
            m_insulation = NULL;
            return;
        }  // if
        if (m_cur == NULL || m_cur->next == NULL) {
            m_cur = m_cutter.Get_token_node();
            return;
        }  // if
        Tn_ptr temp = m_cur;
        m_cur = m_cur->next;
        temp->next = NULL;

    }  // Update_m_cur()
    void Check_exit() {
        if (m_cur->type == LEFT_PAREN && m_cur->next->token.compare("exit") == 0) {
            Tn_ptr tail = m_cutter.Walk_to_last_node(m_cur);
            tail->next = m_cutter.Get_token_node();
            if (tail->next->type == RIGHT_PAREN) {
                string error_str = "EXIT";
                throw(error_str);
            }  // if
            if (tail->next->type == DOT) {
                tail = m_cutter.Walk_to_last_node(m_cur);
                tail->next = m_cutter.Get_token_node();
                if (tail->next->type == NIL) {
                    tail = m_cutter.Walk_to_last_node(m_cur);
                    tail->next = m_cutter.Get_token_node();
                    if (tail->next->type == RIGHT_PAREN) {
                        string error_str = "EXIT";
                        throw(error_str);
                    }  // if
                }      // if
            }          // if
        }              // if
    }                  // Check_exit()
    void Build_right_node(Vec_ptr s_exp_vec) {
        Tn_ptr dot_temp = m_cur;
        Vec_ptr temp_vec = NULL;
        Update_m_cur();
        if (m_cur->type == LEFT_PAREN) {
            temp_vec = Build_s_exp();
            s_exp_vec->insert(s_exp_vec->end(), temp_vec->begin(), temp_vec->end());
            Update_m_cur();
            if (m_cur->type == RIGHT_PAREN) {
                Push_back(m_cur);
                return;
            }  // if true condiction
            else {
                string error_str = "ERROR (unexpected token) : ')' expected when token at Line " 
                + Turn_to_string(m_cur->line) + " Column " + Turn_to_string(m_cur->column) + " is >>" 
                + m_cur->token + "<<";
                throw(error_str);
            }  // else error condiction
        }      // if
        else if (m_cur->type == RIGHT_PAREN) {
            string error_str = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
            + Turn_to_string(m_cur->line) + " Column " + Turn_to_string(m_cur->column) + " is >>" 
            + m_cur->token + "<<";
            throw(error_str);
        }  // if error condiction
        else if (m_cur->type == DOT) {
            string error_str = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
            + Turn_to_string(m_cur->line) + " Column " + Turn_to_string(m_cur->column) + " is >>" 
            + m_cur->token + "<<";
            throw(error_str);
        }  // if error condiction
        else {
            if (m_cur->type != NIL) {
                s_exp_vec->push_back(*dot_temp);
                s_exp_vec->push_back(*m_cur);
            }  // if

            Update_m_cur();
            if (m_cur->type == RIGHT_PAREN) {
                Push_back(m_cur);
                return;
            }  // if true condiction
            else {
                string error_str = "ERROR (unexpected token) : ')' expected when token at Line " 
                + Turn_to_string(m_cur->line) + " Column " + Turn_to_string(m_cur->column) 
                + " is >>" + m_cur->token + "<<";
                throw(error_str);
            }  // else error condiction
        }      // else
    }          // Build_right_node()

    void Build_left_node_recursion(Vec_ptr s_exp_vec) {
        // rule still same with s_exp
        if (m_cur->type == LEFT_PAREN) {
            s_exp_vec->push_back(*m_cur);
            s_exp_vec->at(s_exp_vec->size() - 1).secondary = Build_s_exp();
            s_exp_vec->push_back(*m_cur);  // it must be right_paren
            return;
        }  // if
    }      // Build_left_node_recursion()
    Vec_ptr Build_s_exp() {
        Vec_ptr re_vec = new vector<Token_Node>();
        Update_m_cur();
        if (m_cur->type == DOT) {
            string error_str = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
            + Turn_to_string(m_cur->line) + " Column " + Turn_to_string(m_cur->column) + " is >>" 
            + m_cur->token + "<<";
            throw(error_str);
        }  // if error condiction

        Push_back(m_cur);
        while (1) {
            Update_m_cur();
            if (m_cur->type == LEFT_PAREN) {
                Build_left_node_recursion(re_vec);
            }  // if
            else if (m_cur->type == DOT) {
                Build_right_node(re_vec);
            }  // if
            else if (m_cur->type == RIGHT_PAREN) {
                return re_vec;
            }  // if
            else {
                re_vec->push_back(*m_cur);
            }  // else

        }  // while
        m_cur = m_cutter.Get_token_node();
    }  // Build_s_exp()
    void Error_printer(string error_mes) {
        m_cutter.Clear();
        m_cutter.Reset_column_line();
        cout << endl
             << "> ";
        if (error_mes.compare("EOF") == 0) {
            cout << "ERROR (no more input) : END-OF-FILE encountered" << endl;
            cout << "Thanks for using OurScheme!" << endl;
            exit(0);
        }  // if
        if (error_mes.compare("EXIT") == 0) {
            cout << endl
                 << "Thanks for using OurScheme!" << endl;
            exit(0);
        }  // if
        cout << error_mes << endl;
    }  // Error_printer()
   public:
    Vec_ptr Build_ins_tree() {
        try {
            m_cutter.Reset_column_line();
            Update_m_cur();
            Check_exit();
            Vec_ptr re_vec = new vector<Token_Node>();
            if (m_cur->type == LEFT_PAREN) {
                Build_left_node_recursion(re_vec);
                return re_vec;
            }  // if
            else if (m_cur->type == DOT) {
                string error_str = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
                + Turn_to_string(m_cur->line) + " Column " + Turn_to_string(m_cur->column) 
                + " is >>" + m_cur->token + "<<";
                throw(error_str);
            }  // if
            else if (m_cur->type == RIGHT_PAREN) {
                string error_str = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
                + Turn_to_string(m_cur->line) + " Column " 
                + Turn_to_string(m_cur->column) + " is >>" + m_cur->token + "<<";
                throw(error_str);
            }  //
            else {
                re_vec->push_back(*m_cur);
                return re_vec;
            }  // else
        } catch (string error_mes) {
            Error_printer(error_mes);
            return NULL;
        }

        return NULL;

    }  // Build_ins_tree()

    Analyze() {
        m_code_head = NULL;
        m_ptr_index = NULL;
        m_cur = NULL;
        m_insulation = NULL;
        m_cutter = Cut();
    }  // Analyzer()
    void Print_tree_structure(Vec_ptr front, int indent) {
        if (front == NULL) {
            return;
        }  // if
        if (indent == 0) {
            cout << endl
                 << "> ";
        }  // if
        for (int i = 0; i < front->size(); i++) {
            if (front->at(i).type == LEFT_PAREN) {
                if (i == 0)
                    cout << front->at(i).token << " ";
                else
                    cout << setw(indent) << "" << front->at(i).token << " ";
                Print_tree_structure(front->at(i).secondary, indent + 2);
            }  // if
            else if (i == 0) {
                cout << front->at(i).token << endl;
            }  // if
            else {
                cout << setw(indent) << "" << front->at(i).token << endl;
            }  // else
        }      // for
    }          // Print_tree_structure()
};

int main() {
    Cut cutter = Cut();
    Analyze analyzer = Analyze();
    char utestnum[32];
    cin.getline(utestnum, 32);
    cout << "Welcome to OurScheme!" << endl;
    while (true) {
        analyzer.Print_tree_structure(analyzer.Build_ins_tree(), 0);
        // cutter.Test_print(cutter.Get_token_list());
    }  // while

}  // main()