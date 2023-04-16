# include <iostream>
# include <iomanip>
# include <string>
# include <stack>

using namespace std;

enum Token_type
{
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
}; // enum Token_type

struct Token_node
{
  string token;
  int line;
  int column;
  Token_type token_type;
  Token_node *second_level;
  Token_node *next;
};

typedef Token_node *Tn_ptr;

class Civilize
{
public:
  string Turn_to_string( int num )
  {
    string str = "";
    int index = 0;
    if ( num < 0 )
    {
      num = -num;
      str = "-";
      index++;
    } // if

    while ( num > 0 )
    {
      char ch = num % 10 + '0';
      str.insert( index, 1, ch );
      num = num / 10;
    } // while

    return str;
  } // Turn_to_string()

  string Turn_to_string( float num )
  {
    string str = "";
    int int_num = num;
    str = Turn_to_string( int_num );
    if ( num < 0 )
    {
      num = num * -1;
    } // if

    num = num - int_num;
    str = str + '.';
    num = num * 10000;
    int_num = num;
    int_num = int_num / 10 * 10;
    if ( num - int_num - 5 > -0.1 )
    {
      num = num + 10;
    } // if

    num = num / 10000;

    for ( int i = 0; i < 3 ; i++ )
    {
      num = num * 10;
      int temp = num;
      char ch = temp % 10 + '0';
      str = str + ch;
    } // for

    return str;
  } // Turn_to_string()

  float To_float( string str )
  {
    float reg = 0;
    float magnitude = 1;
    bool dot_flag = false;
    for ( int i = 0; i < str.length(  ) ; i++ )
    {
      if ( str[i] == '.' )
      {
        dot_flag = true;
        magnitude = 1;
      } // if
      else
      {
        if ( dot_flag )
        {
          magnitude = magnitude * 0.1;
          int temp = str[i] - '0';
          reg = reg + temp * magnitude;
        } // if

        else
        {
          int temp = str[i] - '0';
          reg = reg * 10 + temp;
        } // else
      } // else
    } // for

    return reg;
  } // To_float()
};

class Cutter : protected Civilize
{

private:
  char *m_source_ins;
  static int s_code_column;
  static int s_code_line;
  string m_error_string;
  stack<Token_node> m_l_paren_stack;
  bool m_quoted;
  bool Is_digit( char ch )
  {
    if ( '0' <= ch && ch <= '9' )
    {
      return true;
    } // if

    return false;
  } // Is_digit()

  bool Is_letter( char ch )
  {
    if ( 'a' <= ch && ch <= 'z' )
    {
      return true;
    } // if

    if ( 'A' <= ch && ch <= 'Z' )
    {
      return true;
    } // if

    return false;
  } // Is_letter()

  bool Is_white_space( char ch )
  {
    if ( ch == ' ' || ch == '\t' || ch == '\n' )
    {
      return true;
    } // if

    return false;
  } // Is_white_space()

  bool Is_delimiter( char ch )
  {
    if ( ch == '+' || ch == '-' || ch == '*' )
    {
      return true;
    } // if

    if ( ch == '/' || ch == '=' || ch == '<' )
    {
      return true;
    } // if

    if ( ch == '>' || ch == '\\' || ch == '&' )
    {
      return true;
    } // if

    if ( ch == '(' || ch == ')' )
    {
      return true;
    } // if

    return false;
  } // Is_delimiter()

  bool Is_dot( char ch )
  {
    if ( ch == '.' )
    {
      return true;
    } // if

    return false;
  } // Is_dot()

  bool Is_end_of_line( char ch )
  {
    if ( ch == '\0' )
    {
      return true;
    } // if

    return false;
  } // Is_end_of_line()

  bool Is_double_quote( char ch )
  {
    if ( ch == '\"' )
    {
      return true;
    } // if

    return false;
  } // Is_double_quote()

  bool Is_quote( char ch )
  {
    if ( ch == '\'' )
    {
      return true;
    } // if

    return false;
  } // Is_quote()

  bool Is_comment( char ch )
  {
    if ( ch == ';' )
    {
      return true;
    } // if

    return false;
  }  // Is_comment()

  bool Start_with_left_paren( char ch, Tn_ptr &token_segement )
  {
    if ( ch == '(' )
    {
      token_segement = new Token_node;
      token_segement->token = "(";
      token_segement->token_type = LEFT_PAREN;
      token_segement->line = s_code_line;
      token_segement->column = s_code_column + 1;
      token_segement->second_level = NULL;
      token_segement->next = NULL;
      if ( m_quoted )
      {
        m_l_paren_stack.push( *token_segement );
      } // if

      ch = Getch();
      while ( Is_white_space( ch ) )
        ;
      if ( ch == ')' )
      {
        token_segement->token = "nil";
        token_segement->token_type = NIL;
        if ( m_quoted )
        {
          m_l_paren_stack.pop(  );
        } // if
      } // if

      else
      {
        s_code_column -= 1;
      } // else

      return true;
    } // if

    return false;
  } // Start_with_left_paren()

  bool Start_with_right_paren( char ch, Tn_ptr &token_segement )
  {
    if ( ch == ')' )
    {
      token_segement = new Token_node;
      token_segement->token = ")";
      token_segement->token_type = RIGHT_PAREN;
      token_segement->line = s_code_line;
      token_segement->column = s_code_column + 1;
      token_segement->second_level = NULL;
      token_segement->next = NULL;
      if ( m_quoted )
      {
        m_l_paren_stack.pop(  );
      } // if

      return true;
    } // if

    return false;
  } // Start_with_right_paren()

  bool Start_with_digit( char ch, Tn_ptr &token_segement )
  {

    string temp_str = ""; // Start_with_digit()

    if ( !Is_digit( ch ) )
    {
      return false;
    } // if

    temp_str += ch;
    token_segement = new Token_node;
    token_segement->line = s_code_line;
    token_segement->column = s_code_column + 1;
    token_segement->second_level = NULL;
    token_segement->next = NULL;
    ch = Getch();
    while ( Is_digit( ch ) )
    {
      temp_str += ch;
    } // while

    if ( Is_white_space( ch ) || Is_delimiter( ch ) || Is_end_of_line( ch ) || Is_comment( ch ) )
    {

      token_segement->token = temp_str;
      token_segement->token_type = INT;
      if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
      {
        s_code_column -= 1;
      } // if

      if ( Is_comment( ch ) )
      {
        Skip_comment( ch );
      } // if

      return true;
    } // if

    if ( ch == '.' )
    {

      temp_str += ch;
      ch = Getch();
      while ( Is_digit( ch ) )
      {
        temp_str += ch;
      } // while Getch before condiction

      token_segement->token_type = FLOAT;
      if ( Is_white_space( ch ) || Is_delimiter( ch ) || Is_end_of_line( ch ) || Is_comment( ch ) )
      {
        int dot_index = Turn_to_string( To_float( temp_str ) ).find_first_of( ".", 0 );
        token_segement->token = Turn_to_string( To_float( temp_str ) ).substr( 0, dot_index + 4 );
        if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
        {
          s_code_column -= 1;
        } // if

        if ( Is_comment( ch ) )
        {
          Skip_comment( ch );
        } // if

        return true;
      } // if

    } // if
    else if ( !Is_white_space( ch ) && !Is_delimiter( ch ) && !Is_end_of_line( ch ) && !Is_comment( ch ) )
    {
      temp_str += ch;
      ch = Getch();
      while ( !Is_white_space( ch ) && !Is_delimiter( ch ) 
              && !Is_end_of_line( ch ) && !Is_comment( ch ) )
      {
        temp_str += ch;

      } // while

      if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
      {
        s_code_column -= 1;
      } // if

      if ( Is_comment( ch ) )
      {
        Skip_comment( ch );
      } // if

      token_segement->token = temp_str;
      token_segement->token_type = SYMBOL;
      return true;
    } // else if

    temp_str += ch; // for double dot like "3.33.3"
    ch = Getch();
    while ( !Is_white_space( ch ) && !Is_delimiter( ch ) && !Is_end_of_line( ch ) )
    {
      temp_str += ch;
    } // while

    if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
    {
      s_code_column -= 1;
    } // if

    token_segement->token = temp_str;
    token_segement->token_type = SYMBOL;
    return true;
  } // Start_with_digit()

  bool Start_with_letter( char ch, Tn_ptr &token_segement )
  {
    string temp_str = ""; // Start_with_letter()

    if ( !Is_letter( ch ) )
    {
      return false;
    } // if

    token_segement = new Token_node;
    token_segement->token_type = SYMBOL;
    token_segement->line = s_code_line;
    token_segement->column = s_code_column + 1;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    while ( ! ( Is_delimiter( ch ) || Is_white_space( ch ) || Is_end_of_line( ch ) ) )
    {
      temp_str += ch;
      ch = Getch(  );
    } // while

    token_segement->token = temp_str;
    if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
    {
      s_code_column -= 1;
    } // if

    if ( Is_comment( ch ) )
    {
      Skip_comment( ch );
    } // if

    if ( token_segement->token.compare( "nil" ) == 0 )
    {
      token_segement->token_type = NIL;
    } // if

    if ( token_segement->token.compare( "t" ) == 0 )
    {
      token_segement->token = "#t";
      token_segement->token_type = T;
    } // if

    if ( token_segement->token.compare( "f" ) == 0 )
    {
      token_segement->token = "nil";
      token_segement->token_type = NIL;
    } // if

    return true;
  } // Start_with_letter()

  bool Start_with_dot( char ch, Tn_ptr &token_segement )
  {

    string temp_str = ""; // Start_with_dot()

    if ( !Is_dot( ch ) )
    {
      return false;
    } // if

    token_segement = new Token_node;
    token_segement->line = s_code_line;
    token_segement->column = s_code_column + 1;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    temp_str += ch;
    ch = Getch();
    if ( Is_white_space( ch ) || Is_delimiter( ch ) || Is_end_of_line( ch ) || Is_comment( ch ) )
    {
      token_segement->token = temp_str;
      token_segement->token_type = DOT;
      if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
      {
        s_code_column -= 1;
      } // if

      if ( Is_comment( ch ) )
      {
        Skip_comment( ch );
      } // if

      return true;
    } // if

    while ( Is_digit( ch ) )
    {
      temp_str += ch;
      ch = Getch(  );
    } // while

    if ( Is_white_space( ch ) || Is_delimiter( ch ) || Is_end_of_line( ch ) || Is_comment( ch ) )
    {
      int dot_index = Turn_to_string( To_float( temp_str ) ).find_first_of( ".", 0 );
      token_segement->token = Turn_to_string( To_float( temp_str ) ).substr( 0, dot_index + 4 );
      token_segement->token_type = FLOAT;
      if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
      {
        s_code_column -= 1;
      } // if

      if ( Is_comment( ch ) )
      {
        Skip_comment( ch );
      } // if

      return true;
    } // if

    temp_str += ch;
    ch = Getch();
    while ( ! ( Is_delimiter( ch ) || Is_white_space( ch ) || Is_end_of_line( ch ) ) )
    {
      temp_str += ch;
    } // while

    token_segement->token = temp_str;
    token_segement->token_type = SYMBOL;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
    {
      s_code_column -= 1;
    } // if

    if ( Is_comment( ch ) )
    {
      Skip_comment( ch );
    } // if

    return true;
  } // Start_with_dot()

  bool Start_with_quote( char ch, Tn_ptr &token_segement )
  {

    if ( !Is_quote( ch ) )
    {
      return false;
    } // if

    token_segement = new Token_node;
    token_segement->token = "(";
    token_segement->token_type = LEFT_PAREN;
    token_segement->line = s_code_line;
    token_segement->column = s_code_column + 1;
    token_segement->second_level = NULL;
    token_segement->next = new Token_node;
    token_segement->next->token = "quote";
    token_segement->next->token_type = QUOTE;
    token_segement->next->line = s_code_line;
    token_segement->next->column = s_code_column + 1;
    token_segement->next->second_level = NULL;
    token_segement->next->next = NULL;
    m_quoted = true;
    return true;
  } // Start_with_quote()

  Tn_ptr Add_r_paren( Tn_ptr target )
  {
    target->next = new Token_node;
    target->next->token = ")";
    target->next->token_type = RIGHT_PAREN;
    target->next->line = s_code_line;
    target->next->column = s_code_column + 1;
    target->next->second_level = NULL;
    target->next->next = NULL;
    return target->next;
  } // Add_r_paren()

  bool Start_with_double_quote( char ch, Tn_ptr &token_segement )
  {

    string temp_str = ""; // Start_with_dot()

    if ( !Is_double_quote( ch ) )
    {
      return false;
    } // if

    token_segement = new Token_node;
    token_segement->line = s_code_line;
    token_segement->column = s_code_column + 1;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    temp_str += ch;

    ch = Getch();
    while ( !Is_double_quote( ch ) && !Is_end_of_line( ch ) )
    {
      if ( ch != '\\' )
      {
        temp_str += ch;
      } // if
      
      else
      {
        ch = Getch(  );
        if ( ch == 'n' )
        {
          temp_str += '\n';
        } // if
        
        if ( ch == 't' )
        {
          temp_str += '\t';
        } // if
        
        if ( ch == '\\' )
        {
          temp_str += '\\';
        } // if
        
        if ( ch == '\'' )
        {
          temp_str += '\'';
        } // if
        
        if ( ch == '\"' )
        {
          temp_str += '\"';
        } // if

      } // else
    } // while

    if ( Is_double_quote( ch ) )
    {
      temp_str += ch;
      token_segement->token = temp_str;
      token_segement->token_type = STRING;
      return true;
    } // if

    m_error_string = "ERROR (no closing quote) : END-OF-LINE encountered at Line " 
    + Turn_to_string( s_code_line ) + " Column " + Turn_to_string( s_code_column ) + "\n";
    throw ( m_error_string );
    return false;

  } // Start_with_double_quote()

  bool Start_with_other_sign( char ch, Tn_ptr &token_segement )
  {
    string temp_str = "";
    token_segement = new Token_node;
    token_segement->line = s_code_line;
    token_segement->column = s_code_column + 1;
    token_segement->next = NULL;
    token_segement->second_level = NULL;
    if ( ch == '#' )
    {
      // what if only #
      temp_str += ch;
      ch = Getch(  );
      if ( ch == 't' )
      {
        token_segement->token_type = T;
        token_segement->token = "#t";
      } // if

      else if ( ch == 'f' )
      {
        token_segement->token_type = NIL;
        token_segement->token = "nil";
      } // else if

      else if ( Is_white_space( ch ) || Is_delimiter( ch ) 
                || Is_end_of_line( ch ) || Is_comment( ch ) )
      {
        token_segement->token_type = SYMBOL;
        token_segement->token = "#"; // maybe we should throw an error here?;
        // try to be symbol
      } // else if

      else
      {
        token_segement->token_type = SYMBOL;
        temp_str += ch;
        token_segement->token = temp_str;
      } // else

      temp_str += ch;
      ch = Getch();
      while ( ! ( Is_white_space( ch ) || Is_delimiter( ch ) 
                  || Is_end_of_line( ch ) || Is_comment( ch ) ) )
      {
        temp_str += ch;
        token_segement->token = temp_str;
        token_segement->token_type = SYMBOL;
      } // while

      if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
      {
        s_code_column -= 1;
      } // if

      if ( Is_comment( ch ) )
      {
        Skip_comment( ch );
      } // if

      return true;
    } // if start with #

    if ( ch == '+' || ch == '-' )
    {
      int temp_index = s_code_column;
      Tn_ptr temp = NULL;
      Start_with_digit( Getch(  ), temp );
      if ( temp->token_type == INT || temp->token_type == FLOAT )
      {
        if ( ch == '-' )
        {
          temp->token.insert( 0, 1, ch );
        } // if

        token_segement = temp;
        return true;
      } // if

      s_code_column = temp_index;
      Start_with_dot( Getch(  ), temp );
      if ( temp->token_type == FLOAT )
      {
        if ( ch == '-' )
        {
          temp->token.insert( 0, 1, ch );
        } // if

        token_segement = temp;
        return true;
      } // if

      s_code_column = temp_index;
    } // if

    if ( Is_comment( ch ) )
    {
      Skip_comment( ch );
      token_segement = NULL;
      return true;
    } // if

    temp_str += ch;
    token_segement->token_type = SYMBOL;
    ch = Getch();
    while ( ! ( Is_white_space( ch ) || Is_delimiter( ch ) 
                || Is_end_of_line( ch ) || Is_comment( ch ) ) )
    {
      temp_str += ch;
    } // while

    if ( Is_delimiter( ch ) || Is_end_of_line( ch ) )
    {
      s_code_column -= 1;
    } // if

    if ( Is_comment( ch ) )
    {
      Skip_comment( ch );
    } // if

    token_segement->token = temp_str;
    return true; // /////////////////////////////////
  } // Start_with_other_sign()

  char Getch(  )
  {
    s_code_column += 1;
    return m_source_ins[s_code_column];
  } // Getch()

  char Skip_white_space( char ch )
  {
    while ( ch == ' ' || ch == '\t' || ch == '\n' )
    {
      ch = Getch(  );
    } // while

    return ch;
  } // Skip_white_space()

  void Skip_comment( char ch )
  {
    while ( !Is_end_of_line( ch ) )
    {
      ch = Getch(  );
    } // while
  } // Skip_comment()

  Tn_ptr Walk_to_last( Tn_ptr walk )
  {
    while ( walk->next != NULL )
    {
      walk = walk->next;
    } // while

    return walk;
  } // Walk_to_last()

  Tn_ptr Gnerate_token_node(  )
  {
    Tn_ptr return_list = NULL;
    char ch = Skip_white_space( Getch(  ) );
    if ( ch == '\0' )
    {
      return NULL;
    } // if

    if ( Start_with_left_paren( ch, return_list ) )
    {
      if ( m_l_paren_stack.empty(  ) && m_quoted )
      {
        Add_r_paren( Walk_to_last( return_list ) );
        m_quoted = false;
      } // if

      return return_list;
    } // if

    if ( Start_with_right_paren( ch, return_list ) )
    {
      if ( m_l_paren_stack.empty(  ) && m_quoted )
      {
        Add_r_paren( Walk_to_last( return_list ) );
        m_quoted = false;
      } // if

      return return_list;
    } // if

    if ( Start_with_digit( ch, return_list ) )
    {
      if ( m_l_paren_stack.empty(  ) && m_quoted )
      {
        Add_r_paren( Walk_to_last( return_list ) );
        m_quoted = false;
      } // if

      return return_list;
    } // if

    if ( Start_with_letter( ch, return_list ) )
    {
      if ( m_l_paren_stack.empty(  ) && m_quoted )
      {
        Add_r_paren( Walk_to_last( return_list ) );
        m_quoted = false;
      } // if

      return return_list;
    } // if

    if ( Start_with_dot( ch, return_list ) )
    {
      if ( m_l_paren_stack.empty(  ) && m_quoted )
      {
        Add_r_paren( Walk_to_last( return_list ) );
        m_quoted = false;
      } // if

      return return_list;
    } // if

    if ( Start_with_double_quote( ch, return_list ) )
    {
      if ( m_l_paren_stack.empty(  ) && m_quoted )
      {
        Add_r_paren( Walk_to_last( return_list ) );
        m_quoted = false;
      } // if

      return return_list;
    } // if

    if ( Start_with_quote( ch, return_list ) )
    {
      return return_list;
    } // if

    if ( Start_with_other_sign( ch, return_list ) )
    {
      if ( m_l_paren_stack.empty(  ) && m_quoted )
      {
        Add_r_paren( Walk_to_last( return_list ) );
        m_quoted = false;
      } // if 

      return return_list;
    } // if

    return return_list;
  } // Gnerate_token_node()

public:
  Tn_ptr Get_token_line(  )
  {
    // should I free or delete privious pointer m_source_ins
    s_code_line++;
    s_code_column = -1;
    m_source_ins = new char[256];
    if ( cin.eof(  ) )
    {
      m_error_string = "ERROR (no more input) : END-OF-FILE encountered";
      throw ( m_error_string );
    } // if

    cin.getline( m_source_ins, 256 );
    Tn_ptr result_head = NULL;
    Tn_ptr walker = Gnerate_token_node(  );
    if ( walker != NULL )
    {
      result_head = walker;
      walker = Walk_to_last( walker );
      walker->next = NULL;
      walker->second_level = NULL;
    } // if

    while ( m_source_ins[s_code_column] != '\0' )
    {

      walker->next = Gnerate_token_node(  );

      if ( walker != NULL )
      {
        walker = Walk_to_last( walker );
        walker->next = NULL;
        walker->second_level = NULL;
      } // if
    } // while

    return result_head;
  } // Get_token_line()

  void Test_print( Tn_ptr head )
  {
    Tn_ptr walk = head;
    cout << setw( 7 ) << "line" << setw( 7 ) << "column" << setw( 10 ) << "token";
    cout << setw( 15 ) << "token type" << endl;
    while ( walk != NULL )
    {
      cout << setw( 7 ) << walk->line << setw( 7 ) << walk->column << setw( 10 ) << walk->token;
      cout << setw( 15 ) << walk->token_type << endl;
      walk = walk->next;
    } // while
  } // Test_print()

  void Set_code_line( int line )
  {
    s_code_line = line;
  } // Set_code_line()

  void Clear(  )
  {
    m_source_ins = NULL;
    m_quoted = false;
    s_code_column = 1;
    s_code_line = 0;
    m_error_string = "";
  } // Clear()

  Cutter(  )
  {
    m_source_ins = NULL;
    m_quoted = false;
    s_code_column = 1;
    s_code_line = 0;
    m_error_string = "";
  } // Cutter()
};

int Cutter::s_code_line = 0;
int Cutter::s_code_column = 1;

class Syntax_processor : protected Civilize
{
protected:
  Tn_ptr m_ins_tree;

public:
  void Set_ins( Tn_ptr ins_head )
  {
    m_ins_tree = ins_head;
  } // Set_ins()

  void Clear(  )
  {
  } // Clear()
};  // class Syntax_processor

class Analyzer : protected Syntax_processor
{
private:
  Cutter m_cut;
  Tn_ptr m_token_list; // un used
  Tn_ptr m_seq_index;
  string m_error_string;
  stack<Token_node> m_l_paren_stack;
  int m_first_left_paren_index;
  int m_last_column;
  bool m_tree_complete;
  Tn_ptr Find_previous( Tn_ptr head, Tn_ptr tail )
  {
    while ( head != NULL && head->next != tail )
      head = head->next;

    return head;
  } // Find_previous()

  Tn_ptr Build_s_expression(  )
  {
    // when should nil be showed
    // without '( ' ' )' did not finish

    // builder->next should always be m_seq_index
    int dot_count = 0;
    while ( m_seq_index == NULL )
    {
      m_seq_index = m_cut.Get_token_line(  );
      // m_cut.Test_print( m_seq_index );
    } // while

    Tn_ptr builder = m_seq_index;
    Tn_ptr s_exp_head = builder;
    if ( m_seq_index->token_type == DOT )
    {
      m_error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
      + Turn_to_string( builder->line ) + " Column " + Turn_to_string( builder->column ) + " is >>.<<\n";
      throw ( m_error_string ); // error occur
    } // if

    if ( m_seq_index->token.compare( "exit" ) == 0 )
    {
      if ( m_seq_index->next == NULL )
      {
        m_seq_index->next = m_cut.Get_token_line(  );
      } // if

      if ( m_seq_index->next->token_type == RIGHT_PAREN )
      {
        m_error_string = "EXIT";
        throw ( m_error_string ); // EXIT
      } // if

    } // if

    while ( 1 )
    {
      while ( m_seq_index == NULL )
      {
        m_seq_index = m_cut.Get_token_line(  );
        if ( builder->next == NULL )
        {
          builder->next = m_seq_index;
        } // if
        // m_cut.Test_print( m_seq_index );
      } // while

      if ( m_seq_index->token_type == LEFT_PAREN )
      {
        builder = m_seq_index;
        m_seq_index = m_seq_index->next;
        m_l_paren_stack.push( *builder );
        builder->second_level = Build_s_expression(  );
        m_first_left_paren_index = m_l_paren_stack.top(  ).column;
        m_l_paren_stack.pop(  );
        builder->next = m_seq_index;
        if ( Find_previous( builder->second_level, m_seq_index ) != NULL )
        {
          Find_previous( builder->second_level, m_seq_index )->next = NULL;
        } // if

        builder = m_seq_index;
        m_seq_index = m_seq_index->next;
        if ( m_l_paren_stack.empty(  ) )
        {
          builder->next = NULL;
          m_last_column = builder->column;
          return s_exp_head;
        } // if

      } // if

      else if ( m_seq_index->token_type == RIGHT_PAREN )
      {
        if ( m_l_paren_stack.empty(  ) )
        { // may not be happen because recursion won't go here
          m_error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line ";
          m_error_string = m_error_string + Turn_to_string( m_seq_index->line ) + " Column " 
          + Turn_to_string( m_seq_index->column - builder->column ) + " is >> )<<\n"; // error occur
          throw ( m_error_string );
        } // if

        return s_exp_head;
      } // else if

      else if ( m_seq_index->token_type == DOT )
      {
        while ( m_seq_index->next == NULL )
        {
          m_seq_index->next = m_cut.Get_token_line(  );
        } // while

        dot_count++;
        if ( dot_count > 1 )
        {
          m_error_string = "ERROR (unexpected token) : ')' expected when token at Line ";
          m_error_string = m_error_string + Turn_to_string( m_seq_index->line ) + " Column " 
          + Turn_to_string( m_seq_index->column ) + " is >>.<<\n";
          throw ( m_error_string );
        } // if

        if ( m_seq_index->next != NULL && m_seq_index->next->token_type == LEFT_PAREN )
        {
          m_l_paren_stack.push( *m_seq_index->next );
          m_seq_index = m_seq_index->next->next;
          while ( m_seq_index == NULL )
          {
            m_seq_index = m_cut.Get_token_line(  );
            // m_cut.Test_print( m_seq_index );
          } // while

          builder->next = m_seq_index;
          Build_s_expression(  );
          if ( Find_previous( builder, m_seq_index ) != NULL )
            Find_previous( builder, m_seq_index )->next = NULL;
          m_seq_index = m_seq_index->next;
          m_first_left_paren_index = m_l_paren_stack.top(  ).column;
          m_l_paren_stack.pop(  );
          if ( m_l_paren_stack.empty(  ) )
          {
            m_seq_index = m_seq_index->next;
            return s_exp_head;
          } // if

        } // if

        else if ( m_seq_index->next->token_type == RIGHT_PAREN || m_seq_index->next->token_type == DOT )
        {
          m_error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
          + Turn_to_string( m_seq_index->next->line ) 
          + " Column " + Turn_to_string( m_seq_index->next->column ) 
          + " is >>" + m_seq_index->next->token + "<<\n";
          throw ( m_error_string ); // error occur
        } // else if

        else
        {
          if ( m_seq_index->next->next == NULL )
          {
            m_seq_index->next->next = m_cut.Get_token_line(  );
          } // if

          if ( m_seq_index->next->token_type == NIL )
          {
            ;
          } // if

          else
          {
            builder = m_seq_index->next;
          } // else

          m_seq_index = m_seq_index->next->next;
          if ( m_seq_index->token_type != RIGHT_PAREN )
          {
            m_error_string = "ERROR (unexpected token) : ')' expected when token at Line " 
            + Turn_to_string( m_seq_index->line ) 
            + " Column " + Turn_to_string( m_seq_index->column ) + " is >>" + m_seq_index->token + "<<\n";
            throw ( m_error_string ); // error occur
          } // if

          builder->next = NULL;
        } // else

      } // else if

      else
      {
        builder = m_seq_index;
        m_seq_index = m_seq_index->next;
      } // else

    } // while

    return s_exp_head;
  } // Build_s_expression()

  void Update_columon_line(  )
  {
    int offset = m_last_column;
    Tn_ptr walk = m_seq_index;
    while ( walk != NULL )
    {
      walk->column = walk->column - offset;
      walk->line = 1;
      walk = walk->next;
    } // while
  } // Update_columon_line()

public:
  Analyzer(  )
  {
    m_ins_tree = NULL;
    m_seq_index = NULL;
    m_first_left_paren_index = 0;
    m_cut = Cutter(  );
  } // Analyzer()

  void Clear(  )
  {
    m_ins_tree = NULL;

    m_first_left_paren_index = 0;
    while ( !m_l_paren_stack.empty(  ) )
    {
      m_l_paren_stack.pop(  );
    } // while
  } // Clear()

  Tn_ptr Build_ins_tree(  )
  {
    cout << "> ";
    try
    {
      m_cut.Clear(  );
      Clear(  );
      if ( Token_list_is_empty(  ) )
      {
        m_seq_index = m_cut.Get_token_line(  );
        // m_cut.Test_print( m_seq_index );
      } // if

      else
      {
        Update_columon_line(  );
        // m_cut.Test_print( m_seq_index );
      } // else

      if ( m_seq_index == NULL )
      {
        return NULL;
      } // if

      if ( m_seq_index->token_type != LEFT_PAREN )
      {
        Tn_ptr re_ptr = m_seq_index;
        if ( m_seq_index->token_type == DOT || m_seq_index->token_type == RIGHT_PAREN )
        {
          m_error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line " 
          + Turn_to_string( m_seq_index->line ) + " Column " + Turn_to_string( m_seq_index->column ) 
          + " is >>" + m_seq_index->token + "<<\n";
          m_seq_index = m_seq_index->next;
          throw ( m_error_string ); // error occur
        } // if

        m_seq_index = m_seq_index->next;
        re_ptr->next = NULL;
        m_last_column = re_ptr->column;
        return re_ptr;
      } // if

      return Build_s_expression(  );
    } // try

    catch ( const string error_mes )
    {
      if ( error_mes.substr( 0, 5 ).compare( "ERROR" ) == 0 )
      {
        cout << error_mes << endl;
        m_seq_index = NULL;
        if ( error_mes.substr( 6, 15 ).compare( "(no more input)" ) == 0 )
        {
          throw ( "EXIT" );
        } // if

        return NULL;
      } // if
      else
      {
        throw error_mes;
      } // else
    } // catch
  } // Build_ins_tree()

  bool Tree_is_complete(  )
  {
    return m_tree_complete;
  } // Tree_is_complete()

  bool Token_list_is_empty(  )
  {
    if ( m_seq_index == NULL )
    {
      return true;
    } // if

    return false;
  } // Token_list_is_empty()

  Tn_ptr Get_ins_tree(  )
  {
    return m_ins_tree;
  } // Get_ins_tree()

  void Print_tree_struct( Tn_ptr head, int indent )
  {
    Tn_ptr walk = head;
    if ( walk == NULL )
    {
      return;
    } // if

    if ( walk->second_level != NULL )
    {
      cout << walk->token << " ";
      Print_tree_struct( walk->second_level, indent + 2 );
      walk = walk->next;
      cout << setw( indent ) << "" << walk->token << endl;
      walk = walk->next;
    } // if
    else
    {
      cout << walk->token << endl;
      walk = walk->next;
    } // else

    while ( walk != NULL )
    {
      if ( walk->second_level != NULL )
      {
        cout << setw( indent ) << "" << walk->token << " ";
        Print_tree_struct( walk->second_level, indent + 2 );
      } // if
      else
      {
        cout << setw( indent ) << "" << walk->token << endl;
      } // else

      walk = walk->next;
    } // while()

    return;
  } // Print_tree_struct()

}; // class Analyzer

class Generator : Syntax_processor
{
}; // class Generator

class IO : Civilize
{
  Cutter m_cut;

public:
  Tn_ptr Get_token_line(  )
  {
    char *line = new char[256];
    cin.getline( line, 256 );
    Tn_ptr head;
    return head;
  } // Get_token_line()

  IO(  )
  {
    m_cut = Cutter(  );
  } // IO()
}; // class IO

class Interpreter : protected Civilize
{
private:
  IO m_io;
  Analyzer m_anal;
  Generator m_gen;

public:
  Interpreter(  )
  {
    m_io = IO(  );
    m_anal = Analyzer(  );
    m_gen = Generator(  );
  } // Interpreter()
}; // class Interpreter

int main(  )
{
  Tn_ptr head;
  Cutter m_cut = Cutter(  );
  Analyzer anal = Analyzer(  );
  // instruction cross line will occur segmentation fault

  while ( 1 )
  {
    try
    {
      anal.Print_tree_struct( anal.Build_ins_tree(  ), 0 );
    }
    catch ( const string mes )
    {
      if ( mes.compare( "EXIT" ) == 0 )
      {
        cout << "\nThanks for using OurScheme!";
        return 0;
      } // if
    } // catch()
  } // while()

  return 0;

} // main()