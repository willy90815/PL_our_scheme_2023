# include <stdio.h>
# include <iostream>
# include <string>
# include <stack>

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
} ;

struct Token_Node {
  int line; 
  int column;
  int last_column;
  string token;
  Token_type type;
  Token_Node * second_level;
  Token_Node * next;
} ;

typedef Token_Node * tn_ptr;

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
    if ( str.compare( "" ) == 0 ) {
      str = "0";
    } // if

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



class Cut : protected Civilize{

  int m_code_line;
  int m_code_column;
  char Getch() {
    char ch = cin.get( ch );
    m_code_column++;
    if ( ch == '\n' ){
      m_code_line++;
    } // if
    
    return ch;
  } // Getch()

  void Put_back_ch( char ch ) {
    cin.putback( ch );
    m_code_column--;
    return ;
  }

  bool Is_digit( char ch ) {
    if ( '0' <= ch && ch <= '9' )
    {
      return true;
    } // if

    return false;
  } // Is_digit()

  bool Is_letter( char ch ) {
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
    if ( ch == '(' || ch == ')' || ch == '\'' )
    {
      return true;
    } // if

    if ( ch == '\"' )
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
      m_end_of_line = true;
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

  tn_ptr create_new_node( Token_Node type ) {

  } // create_new_node()


  bool Start_with_digit( char ch, tn_ptr return_ptr) {
    // int     '1234'
    // float   '1234.','1234.234' 
    // symbol '123.12.','123.h', ........


    if ( ! is_digit( ch ) ) {
      return false;
    } // if
    

    while ( is_digit( ch ) ) {

    } // while

  }

  public:
  tn_ptr Get_token_node() {
    char ch ;
    cin.get( ch );




  } // Get_token_node
}