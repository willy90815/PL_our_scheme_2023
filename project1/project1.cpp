# include <iostream>
# include <iomanip>
# include <string>
# include <stack>

using namespace std;

enum Token_type
{
  LEFT_PAREN = 0, RIGHT_PAREN, INT, STRING,
  DOT, FLOAT, NIL, T, QUOTE, SYMBOL
}; // enum Token_type

struct Token_node
{
  string token;
  int line;
  int column;
  Token_type token_type;
  Token_node *second_level;
  Token_node *next;
} ;

typedef Token_node *Tn_ptr;



class Civilize{
  public:
  string Turn_to_string( int num ) {
    string str = "";
    int index = 0;
    if ( num < 0 ) {
      num = -num;
      str = "-";
      index++;
    }
    
    while ( num > 0 ) {
      char ch = num  % 10 + '0' ;
      str.insert( index, 1, ch );
      num = num/10;
    }
    
    return str;
  }
  string Turn_to_string( float num ) {
    string str ="";
    int int_num = num;
    str = Turn_to_string( int_num );
    if(num<0){
      num = num*-1;
    }
    num = num-int_num;
    str = str + '.';
    num = num*10000;
    int_num = num;
    int_num = int_num/10*10;
    if(num-int_num-5>-0.1){
      num = num+10;
    }
    num = num/10000;
    for(int i = 0; i < 3;i++){
      num = num*10;
      str = str + char(int(num)%10+'0');
    }
    

    return str;
  }
  float to_float(string str){
    float reg=0;
    float magnitude=1;
    bool dot_flag=false;
    for( int i=0;i<str.length();i++){
      if(str[i] == '.'){
        dot_flag = true;
        magnitude = 1;
      }
      else{
        if ( dot_flag ){
          magnitude = magnitude*0.1;
          reg = reg + int(str[i]-'0')*magnitude;
        }
        else{
          reg = reg*10 + int(str[i]-'0');
        }
      }
      
    }

    return reg;
  }

};

class Cutter : protected Civilize
{

private:
  char *source_ins;
  static int code_column;
  static int code_line;
  string error_string;
  stack<Token_node> l_paren_stack;
  bool quoted;
  bool is_digit(char ch)
  {
    if ('0' <= ch && ch <= '9')
    {
      return true;
    } // if

    return false;
  } // is_digit()

  bool is_letter(char ch)
  {
    if ('a' <= ch && ch <= 'z')
    {
      return true;
    } // if

    if ('A' <= ch && ch <= 'Z')
    {
      return true;
    } // if

    return false;
  } // is_letter()

  bool is_white_space(char ch)
  {
    if (ch == ' ' || ch == '\t' || ch == '\n')
    {
      return true;
    } // if

    return false;
  } // is_white_space()

  bool is_delimiter(char ch)
  {
    if (ch == '+' || ch == '-' || ch == '*')
    {
      return true;
    } // if

    if (ch == '/' || ch == '=' || ch == '<')
    {
      return true;
    } // if

    if (ch == '>' || ch == '\\' || ch == '&')
    {
      return true;
    } // if

    if (ch == '(' || ch == ')')
    {
      return true;
    } // if

    return false;
  } // is_delimiter()

  bool is_dot(char ch)
  {
    if (ch == '.')
    {
      return true;
    } // if

    return false;
  } // is_dot()

  bool is_end_of_line(char ch)
  {
    if (ch == '\0')
    {
      return true;
    } // if

    return false;
  } // is_end_of_line()

  bool is_double_quote(char ch)
  {
    if (ch == '\"')
    {
      return true;
    } // if

    return false;
  } // is_double_quote()

  bool is_quote(char ch)
  {
    if (ch == '\'')
    {
      return true;
    } // if

    return false;
  } // is_quote()

  bool is_comment(char ch)
  {
    if (ch == ';')
    {
      return true;
    }

    return false;
  }

  bool start_with_left_paren(char ch, Tn_ptr &token_segement)
  {
    if (ch == '(')
    {
      token_segement = new Token_node;
      token_segement->token = "(";
      token_segement->token_type = LEFT_PAREN;
      token_segement->line = code_line;
      token_segement->column = code_column + 1;
      token_segement->second_level = NULL;
      token_segement->next = NULL;
      if(quoted){
        l_paren_stack.push(*token_segement);
      }
      
      while (is_white_space(ch = getch()))
        ;
      if (ch == ')')
      {
        token_segement->token = "nil";
        token_segement->token_type = NIL;
        if( quoted ){
          l_paren_stack.pop();
        }
        
      }
      else
      {
        code_column -= 1;
      }
      return true;
    } // if

    return false;
  } // start_with_left_paren()

  bool start_with_right_paren(char ch, Tn_ptr &token_segement)
  {
    if (ch == ')')
    {
      token_segement = new Token_node;
      token_segement->token = ")";
      token_segement->token_type = RIGHT_PAREN;
      token_segement->line = code_line;
      token_segement->column = code_column + 1;
      token_segement->second_level = NULL;
      token_segement->next = NULL;
      if( quoted ){
        l_paren_stack.pop();
      }
      
      return true;
    } // if

    return false;
  } // start_with_right_paren()

  bool start_with_digit(char ch, Tn_ptr &token_segement)
  {

    string temp_str = ""; // start_with_digit()

    if (!is_digit(ch))
    {
      return false;
    } // if

    temp_str += ch;
    token_segement = new Token_node;
    token_segement->line = code_line;
    token_segement->column = code_column + 1;
    token_segement->second_level = NULL;
    token_segement->next = NULL;
    while (is_digit(ch = getch()))
    {
      temp_str += ch;
    } // while

    if (is_white_space(ch) || is_delimiter(ch) || is_end_of_line(ch) || is_comment(ch))
    {

      token_segement->token = temp_str;
      token_segement->token_type = INT;
      if (is_delimiter(ch) || is_end_of_line(ch))
      {
        code_column -= 1;
      } // if

      if (is_comment(ch))
      {
        skip_comment(ch);
      }

      return true;
    } // if

    if (ch == '.')
    {

      temp_str += ch;
      while (is_digit(ch = getch()))
      {
        temp_str += ch;
      } // while getch before condiction
      token_segement->token_type = FLOAT;
      if (is_white_space(ch) || is_delimiter(ch) || is_end_of_line(ch) || is_comment(ch))
      {
        int dot_index = Turn_to_string(to_float(temp_str)).find_first_of(".", 0);
        token_segement->token = Turn_to_string(to_float(temp_str)).substr(0, dot_index + 4);
        if (is_delimiter(ch) || is_end_of_line(ch))
        {
          code_column -= 1;
        } // if

        if (is_comment(ch))
        {
          skip_comment(ch);
        }

        return true;
      } // if

    } // if
    else if (!is_white_space(ch) && !is_delimiter(ch) && !is_end_of_line(ch) && !is_comment(ch))
    {
      temp_str += ch;
      while (!is_white_space(ch = getch()) && !is_delimiter(ch) && !is_end_of_line(ch) && !is_comment(ch))
      {
        temp_str += ch;

      } // while

      if (is_delimiter(ch) || is_end_of_line(ch))
      {
        code_column -= 1;
      } // if
      if (is_comment(ch))
      {
        skip_comment(ch);
      }

      token_segement->token = temp_str;
      token_segement->token_type = SYMBOL;
      return true;
    } // else if

    temp_str += ch; // for double dot like "3.33.3"
    while (!is_white_space(ch = getch()) && !is_delimiter(ch) && !is_end_of_line(ch))
    {
      temp_str += ch;
    } // while

    if (is_delimiter(ch) || is_end_of_line(ch))
    {
      code_column -= 1;
    } // if

    token_segement->token = temp_str;
    token_segement->token_type = SYMBOL;
    return true;
  } // class Cutter

  bool start_with_letter(char ch, Tn_ptr &token_segement)
  {
    string temp_str = ""; // start_with_letter()

    if (!is_letter(ch))
    {
      return false;
    } // if

    token_segement = new Token_node;
    token_segement->token_type = SYMBOL;
    token_segement->line = code_line;
    token_segement->column = code_column + 1;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    while (!(is_delimiter(ch) || is_white_space(ch) || is_end_of_line(ch)))
    {
      temp_str += ch;
      ch = getch();
    } // while

    token_segement->token = temp_str;
    if (is_delimiter(ch) || is_end_of_line(ch))
    {
      code_column -= 1;
    } // if
    if (is_comment(ch))
    {
      skip_comment(ch);
    }
    if (token_segement->token.compare("nil") == 0)
    {
      token_segement->token_type = NIL;
    } // if
    if (token_segement->token.compare("t") == 0)
    {
      token_segement->token = "#t";
      token_segement->token_type = T;
    } // if
    if (token_segement->token.compare("f") == 0)
    {
      token_segement->token = "nil";
      token_segement->token_type = NIL;
    } // if

    return true;
  }

  bool start_with_dot(char ch, Tn_ptr &token_segement)
  {

    string temp_str = ""; // start_with_dot()

    if (!is_dot(ch))
    {
      return false;
    } // if

    token_segement = new Token_node;
    token_segement->line = code_line;
    token_segement->column = code_column + 1;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    temp_str += ch;
    if (is_white_space(ch = getch()) || is_delimiter(ch) || is_end_of_line(ch) || is_comment(ch))
    {
      token_segement->token = temp_str;
      token_segement->token_type = DOT;
      if (is_delimiter(ch) || is_end_of_line(ch))
      {
        code_column -= 1;
      } // if

      if (is_comment(ch))
      {
        skip_comment(ch);
      }
      return true;
    } // if

    while (is_digit(ch))
    {
      temp_str += ch;
      ch = getch();
    } // while

    if (is_white_space(ch) || is_delimiter(ch) || is_end_of_line(ch) || is_comment(ch))
    {
      int dot_index = Turn_to_string(to_float(temp_str)).find_first_of(".", 0);
      token_segement->token = Turn_to_string(to_float(temp_str)).substr(0, dot_index + 4);
      token_segement->token_type = FLOAT;
      if (is_delimiter(ch) || is_end_of_line(ch))
      {
        code_column -= 1;
      } // if
      if (is_comment(ch))
      {
        skip_comment(ch);
      }

      return true;
    } // if

    temp_str += ch;
    while (!(is_delimiter(ch = getch()) || is_white_space(ch) || is_end_of_line(ch)))
    {
      temp_str += ch;
    } // while

    token_segement->token = temp_str;
    token_segement->token_type = SYMBOL;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    if (is_delimiter(ch) || is_end_of_line(ch))
    {
      code_column -= 1;
    } // if
    if (is_comment(ch))
    {
      skip_comment(ch);
    }

    return true;
  }

  bool start_with_quote(char ch, Tn_ptr &token_segement)
  {
    
    if(!is_quote(ch)){
      return false;
    }

    token_segement = new Token_node;
    token_segement->token = "(";
    token_segement->token_type = LEFT_PAREN;
    token_segement->line = code_line;
    token_segement->column = code_column + 1;
    token_segement->second_level = NULL;
    token_segement->next = new Token_node;
    token_segement->next->token = "quote";
    token_segement->next->token_type = QUOTE;
    token_segement->next->line = code_line;
    token_segement->next->column = code_column + 1;
    token_segement->next->second_level = NULL;
    token_segement->next->next = NULL;
    quoted = true;
    return true;
  } // start_with_quote()
  Tn_ptr add_r_paren(Tn_ptr target){
    target->next = new Token_node;
    target->next->token = ")";
    target->next->token_type = RIGHT_PAREN;
    target->next->line = code_line;
    target->next->column = code_column + 1;
    target->next->second_level = NULL;
    target->next->next = NULL;
    return target->next;
  }

  bool start_with_double_quote(char ch, Tn_ptr &token_segement)
  {

    string temp_str = ""; // start_with_dot()

    if (!is_double_quote(ch))
    {
      return false;
    }

    token_segement = new Token_node;
    token_segement->line = code_line;
    token_segement->column = code_column + 1;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    temp_str += ch;

    while (!is_double_quote(ch = getch()) && !is_end_of_line(ch))
    {
      if (ch != '\\')
      {
        temp_str += ch;
      }
      else
      {
        ch = getch();
        if (ch == 'n')
        {
          temp_str += '\n';
        }
        if (ch == 't')
        {
          temp_str += '\t';
        }
        if (ch == '\\')
        {
          temp_str += '\\';
        }
        if (ch == '\'')
        {
          temp_str += '\'';
        }
        if (ch == '\"')
        {
          temp_str += '\"';
        }
      }
    }
    if (is_double_quote(ch))
    {
      temp_str += ch;
      token_segement->token = temp_str;
      token_segement->token_type = STRING;
      return true;
    }
    error_string = "ERROR (no closing quote) : END-OF-LINE encountered at Line " + Turn_to_string(code_line) + " Column " + Turn_to_string(code_column) + "\n";
    throw(error_string);
    return false;

  } // start_with_double_quote()

  bool start_with_other_sign(char ch, Tn_ptr &token_segement)
  {
    string temp_str = "";
    token_segement = new Token_node;
    token_segement->line = code_line;
    token_segement->column = code_column + 1;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    if (ch == '#')
    {
      // what if only #
      temp_str += ch;
      ch = getch();
      if (ch == 't')
      {
        token_segement->token_type = T;
        token_segement->token = "#t";
      }
      else if (ch == 'f')
      {
        token_segement->token_type = NIL;
        token_segement->token = "nil";
      }
      else if (is_white_space(ch) || is_delimiter(ch) || is_end_of_line(ch) || is_comment(ch))
      {
        token_segement->token_type = SYMBOL;
        token_segement->token = "#"; // maybe we should throw an error here?;
        // try to be symbol
      }
      else
      {
        token_segement->token_type = SYMBOL;
        temp_str += ch;
        token_segement->token = temp_str;
      }

      temp_str += ch;
      while (!(is_white_space(ch = getch()) || is_delimiter(ch) || is_end_of_line(ch) || is_comment(ch)))
      {
        temp_str += ch;
        token_segement->token = temp_str;
        token_segement->token_type = SYMBOL;
      }

      if (is_delimiter(ch) || is_end_of_line(ch))
      {
        code_column -= 1;
      }
      if (is_comment(ch))
      {
        skip_comment(ch);
      }
      return true;
    } // start with #
    if (ch == '+' || ch == '-')
    {
      int temp_index = code_column;
      Tn_ptr temp = NULL;
      start_with_digit(getch(), temp);
      if (temp->token_type == INT || temp->token_type == FLOAT)
      {
        if (ch == '-')
        {
          temp->token.insert(0, 1, ch);
        }
        token_segement = temp;
        return true;
      }
      code_column = temp_index;
      start_with_dot(getch(), temp);
      if (temp->token_type == FLOAT)
      {
        if (ch == '-')
        {
          temp->token.insert(0, 1, ch);
        }
        token_segement = temp;
        return true;
      }
      code_column = temp_index;
    }
    if (is_comment(ch))
    {
      skip_comment(ch);
      token_segement = NULL;
      return true;
    }

    temp_str += ch;
    token_segement->token_type = SYMBOL;
    while (!(is_white_space(ch = getch()) || is_delimiter(ch) || is_end_of_line(ch) || is_comment(ch)))
    {
      temp_str += ch;
    }
    if (is_delimiter(ch) || is_end_of_line(ch))
    {
      code_column -= 1;
    }
    if (is_comment(ch))
    {
      skip_comment(ch);
    }

    token_segement->token = temp_str;
    return true; ///////////////////////////////////
  }               // start_with_other_sign()

  char getch()
  {
    code_column += 1;
    return source_ins[code_column];
  } // getch()

  char skip_white_space(char ch)
  {
    while (ch == ' ' || ch == '\t' || ch == '\n')
    {
      ch = getch();
    } // while

    return ch;
  } // skip_white_space()

  void skip_comment(char ch)
  {
    while (!is_end_of_line(ch))
    {
      ch = getch();
    }
  }

  Tn_ptr walk_to_last(Tn_ptr walk)
  {
    while (walk->next != NULL)
    {
      walk = walk->next;
    } // while

    return walk;
  } // walk_to_last()

  Tn_ptr gnerate_token_node()
  {
    Tn_ptr return_list = NULL;
    char ch = skip_white_space(getch());
    if (ch == '\0')
    {
      return NULL;
    }
    if (start_with_left_paren(ch, return_list))
    {
      if(l_paren_stack.empty()&&quoted){
        add_r_paren(walk_to_last(return_list));
        quoted = false; 
      }
      return return_list;
    } // if

    if (start_with_right_paren(ch, return_list))
    {
      if(l_paren_stack.empty()&&quoted){
        add_r_paren(walk_to_last(return_list)); 
        quoted = false; 
      }
      return return_list;
    } // if

    if (start_with_digit(ch, return_list))
    {
      if(l_paren_stack.empty()&&quoted){
        add_r_paren(walk_to_last(return_list)); 
        quoted = false; 
      }
      return return_list;
    } // if

    if (start_with_letter(ch, return_list))
    {
      if(l_paren_stack.empty()&&quoted){
        add_r_paren(walk_to_last(return_list));
        quoted = false;  
      }
      return return_list;
    } // if

    if (start_with_dot(ch, return_list))
    {
      if(l_paren_stack.empty()&&quoted){
        add_r_paren(walk_to_last(return_list));
        quoted = false;  
      }
      return return_list;
    } // if

    if (start_with_double_quote(ch, return_list))
    {
      if(l_paren_stack.empty()&&quoted){
        add_r_paren(walk_to_last(return_list)); 
        quoted = false; 
      }
      return return_list;
    } // if
    
    if(start_with_quote(ch, return_list))
    {
      return return_list;
    }

    if (start_with_other_sign(ch, return_list))
    {
      if(l_paren_stack.empty()&&quoted){
        add_r_paren(walk_to_last(return_list)); 
        quoted = false; 
      }
      return return_list;
    }
    
    
    return return_list;
  } // gnerate_token_node()

public:
  Tn_ptr get_token_line()
  {
    // should I free or delete privious pointer source_ins
    code_line++;
    code_column = -1;
    source_ins = new char[256];
    if(cin.eof()){
      error_string = "ERROR (no more input) : END-OF-FILE encountered";
      throw(error_string);
    }
    cin.getline(source_ins, 256);
    Tn_ptr result_head = NULL;
    Tn_ptr walker = gnerate_token_node();
    if (walker != NULL)
    {
      result_head = walker;
      walker = walk_to_last(walker);
      walker->next = NULL;
      walker->second_level = NULL;
    }

    while (source_ins[code_column] != '\0')
    {
      
      walker->next = gnerate_token_node();
      
      if (walker != NULL)
      {
        walker = walk_to_last(walker);
        walker->next = NULL;
        walker->second_level = NULL;
      }
    } // while
    return result_head;
  } // generate_list()

  void test_print(Tn_ptr head)
  {
    Tn_ptr walk = head;
    cout << setw(7) << "line" << setw(7) << "column" << setw(10) << "token";
    cout << setw(15) << "token type" << endl;
    while (walk != NULL)
    {
      cout << setw(7) << walk->line << setw(7) << walk->column << setw(10) << walk->token;
      cout << setw(15) << walk->token_type << endl;
      walk = walk->next;
    } // while
  }   // test_print()
  void set_code_line(int line)
  {
    code_line = line;
  }
  void clear()
  {
    source_ins = NULL;
    quoted = false;
    code_column = 1;
    code_line = 0;
    error_string = "";
  }
  Cutter()
  {
    source_ins = NULL;
    quoted = false;
    code_column = 1;
    code_line = 0;
    error_string = "";
  }
};

int Cutter::code_line = 0;
int Cutter::code_column = 1;

class Syntax_processor : protected Civilize
{
protected:
  Tn_ptr ins_tree;

public:
  void set_ins(Tn_ptr ins_head)
  {
    ins_tree = ins_head;
  } // set_ins()

  void clear()
  {
  } // clear()
};  // class Syntax_processor

class Analyzer : protected Syntax_processor
{
private:
  Cutter cut;
  Tn_ptr token_list;
  Tn_ptr seq_index;
  string error_string;
  stack<Token_node> l_paren_stack;
  int first_left_paren_index;
  int last_column;
  bool tree_complete;
  Tn_ptr find_previous(Tn_ptr head, Tn_ptr tail)
  {
    while (head != NULL && head->next != tail)
      head = head->next;

    return head;
  }
  Tn_ptr build_s_expression()
  {
    // when should nil be showed
    // without '(' ')' did not finish


  
    // builder->next should always be seq_index
    int dot_count = 0;
    while (seq_index == NULL)
    {
      seq_index = cut.get_token_line();
      // cut.test_print(seq_index);
    } 
    Tn_ptr builder = seq_index;
    Tn_ptr s_exp_head = builder;
    if (seq_index->token_type == DOT)
    {
      error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line " + Turn_to_string(builder->line) + " Column " + Turn_to_string(builder->column) + " is >>.<<\n";
      throw(error_string); // error occur

    }
    if(seq_index->token.compare("exit")==0 ){
      if(seq_index->next == NULL){
        seq_index->next = cut.get_token_line();
      }
      if(seq_index->next->token_type == RIGHT_PAREN){
        error_string = "EXIT";
        throw(error_string); // EXIT
      }

    }

    while (1)
    {
      while (seq_index == NULL)
      {
        seq_index = cut.get_token_line();
        if(builder->next == NULL){
          builder->next = seq_index;
        }
        // cut.test_print(seq_index);
      }
      if (seq_index->token_type == LEFT_PAREN)
      {
        builder = seq_index;
        seq_index = seq_index->next;
        l_paren_stack.push(*builder);
        builder->second_level = build_s_expression();
        first_left_paren_index = l_paren_stack.top().column;
        l_paren_stack.pop();
        builder->next = seq_index;
        if (find_previous(builder->second_level, seq_index) != NULL)
        {
          find_previous(builder->second_level, seq_index)->next = NULL;
        }
        builder = seq_index;
        seq_index = seq_index->next;
        if (l_paren_stack.empty())
        {
          builder->next = NULL;
          last_column = builder->column;
          return s_exp_head;
        }
      }
      else if (seq_index->token_type == RIGHT_PAREN)
      {
        if (l_paren_stack.empty())
        { // may not be happen because recursion won't go here
          error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line ";
          error_string = error_string + Turn_to_string(seq_index->line) + " Column " + Turn_to_string(seq_index->column - builder->column) + " is >>)<<\n"; // error occur
          throw(error_string);
        }

        return s_exp_head;
      }
      else if (seq_index->token_type == DOT)
      {
        while (seq_index->next == NULL)
        {
          seq_index->next = cut.get_token_line();
          // cut.test_print(seq_index->next);
          ; //? should be an error?
          // NO we should get next token line
        }
        dot_count++;
        if (dot_count > 1)
        {
          error_string = "ERROR (unexpected token) : ')' expected when token at Line ";
          error_string = error_string + Turn_to_string(seq_index->line) + " Column " + Turn_to_string(seq_index->column) + " is >>.<<\n";
          throw(error_string);
        }
        if (seq_index->next != NULL && seq_index->next->token_type == LEFT_PAREN)
        {
          l_paren_stack.push(*seq_index->next);
          seq_index = seq_index->next->next;
          while (seq_index == NULL)
          {
            seq_index = cut.get_token_line();
            // cut.test_print(seq_index);
          }
          builder->next = seq_index;
          build_s_expression();
          if( find_previous(builder, seq_index)!=NULL)
            find_previous(builder, seq_index)->next =NULL;
          seq_index = seq_index->next;
          first_left_paren_index = l_paren_stack.top().column;
          l_paren_stack.pop();
          if (l_paren_stack.empty())
          {
            seq_index = seq_index->next;
            return s_exp_head;
          }
        }
        else if (seq_index->next->token_type == RIGHT_PAREN || seq_index->next->token_type == DOT)
        {
          error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line " + Turn_to_string(seq_index->next->line) + " Column " + Turn_to_string(seq_index->next->column) + " is >>" + seq_index->next->token + "<<\n";
          throw(error_string); // error occur
        }
        else
        {
          if(seq_index->next->next == NULL){
            seq_index->next->next = cut.get_token_line();
          } 
          if (seq_index->next->token_type == NIL)
          {
            ;
          }
          else
          {
            builder = seq_index->next;
          }

          seq_index = seq_index->next->next;
          if(seq_index->token_type!=RIGHT_PAREN){
            error_string = "ERROR (unexpected token) : ')' expected when token at Line " + Turn_to_string(seq_index->line) + " Column " + Turn_to_string(seq_index->column) + " is >>" + seq_index->token + "<<\n";
            throw(error_string); // error occur
          }
          builder->next = NULL;
        }
      }
      else
      {
        builder = seq_index;
        seq_index = seq_index->next;
      }
    }

    return s_exp_head;
  }
  void update_columon_line()
  {
    int offset = last_column;
    Tn_ptr walk = seq_index;
    while (walk != NULL)
    {
      walk->column = walk->column - offset;
      walk->line = 1;
      walk = walk->next;
    }
  }

public:
  Analyzer()
  {
    this->ins_tree = NULL;
    this->seq_index = NULL;
    this->token_list = NULL;
    this->first_left_paren_index = 0;
    this->cut = Cutter();
  }
  void clear()
  {
    this->ins_tree = NULL;
    this->token_list = NULL;
    this->first_left_paren_index = 0;
    while(!l_paren_stack.empty()){
      l_paren_stack.pop();
    }
    
  }

  Tn_ptr build_ins_tree()
  {
    cout << "> ";
    try
    {
      cut.clear();
      clear();
      if (this->token_list_is_empty())
      {
        seq_index = cut.get_token_line();
        // cut.test_print(seq_index);
      }
      else
      {
        update_columon_line();
        // cut.test_print(seq_index);
      }
      if (seq_index == NULL)
      {
        return NULL;
      }

      if (seq_index->token_type != LEFT_PAREN)
      {
        Tn_ptr re_ptr = seq_index;
        if (seq_index->token_type == DOT || seq_index->token_type == RIGHT_PAREN)
        {
          error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line " + Turn_to_string(seq_index->line) + " Column " + Turn_to_string(seq_index->column) + " is >>" + seq_index->token + "<<\n";
          seq_index = seq_index->next;
          throw(error_string); // error occur
        }
        seq_index = seq_index->next;
        re_ptr->next = NULL;
        last_column = re_ptr->column;
        return re_ptr;
      }

      return build_s_expression();
    }

    catch (const string error_mes)
    {
      if (error_mes.substr(0, 5).compare("ERROR") ==0)
      {
        cout << error_mes << endl;
        seq_index = NULL;
        if(error_mes.substr(6,15).compare("(no more input)") == 0){
          throw("EXIT");
        }
        return NULL;
      }
      else
      {
        throw error_mes;
      }
    }
  }
  bool tree_is_complete()
  {
    return tree_complete;
  }
  bool token_list_is_empty()
  {
    if (seq_index == NULL)
    {
      return true;
    }
    return false;
  }

  Tn_ptr get_ins_tree()
  {
    return ins_tree;
  }

  void print_tree_struct(Tn_ptr head, int indent)
  {
    Tn_ptr walk = head;
    if(walk == NULL ){
      return;
    }
    if( walk->second_level!= NULL){
      cout << walk->token << " ";
      print_tree_struct(walk->second_level, indent + 2);
      walk = walk->next;
      cout << setw(indent) << "" << walk->token << endl;
      walk = walk->next;
    }
    else{
      cout << walk->token << endl;
      walk = walk->next;
    }
    
    while (walk != NULL)
    {
      if(walk->second_level!= NULL){
        cout << setw(indent)<< ""<<walk->token << " ";
        print_tree_struct(walk->second_level, indent + 2);
      }
      else
      {
        cout << setw(indent) << "" << walk->token << endl;
      }
      
      
      walk = walk->next;
    }

    return;
  }

}; // class Analyzer

class Generator : Syntax_processor
{
}; // class Generator

class IO :Civilize
{
  Cutter cut ;

public:
  Tn_ptr get_token_line()
  {
    char *line = new char[256];
    cin.getline(line, 256);
    Tn_ptr head;
    return head;
  } // get_token_line()
  IO(){
    cut = Cutter();
  }
};  // class IO

class Interpreter : protected Civilize
{
private:
  IO io ;
  Analyzer anal;
  Generator gen;
  public : 
  Interpreter(){
    io = IO();
    anal = Analyzer();
    gen = Generator();
  }
}; // class Interpreter

int main()
{
  Tn_ptr head;
  Cutter cut = Cutter();
  Analyzer anal = Analyzer();
  // instruction cross line will occur segmentation fault



  while (1)
  {
    try{
      anal.print_tree_struct(anal.build_ins_tree(), 0);
    }
    catch(const string mes){
      if(mes.compare("EXIT") == 0){
        cout<<"\nThanks for using OurScheme!";
        return 0;
      }
    }
    
  }

  return 0;

} // main()