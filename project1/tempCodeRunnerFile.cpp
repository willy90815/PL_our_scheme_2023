# include <fstream>
# include <iostream>
# include <iomanip>
# include <string>
# include <stack>

using namespace std ;

enum _token_type {
  LEFT_PAREN = 0,
  RIGHT_PAREN,
  INT,
  STRING,
  DOT,
  FLOAT,
  NIL,
  T,
  QUOTE,
  SYMBOL,
}; // enum _token_type


typedef struct _token_node_ {
  string token ;
  int line ;
  int column ;
  _token_type token_type ;
  _token_node_ * second_level ;
  _token_node_ * next ;
} token_node ;


typedef _token_node_ * tn_ptr ;

class Cutter {

  private :
  char * source_ins ;
  static int code_column ;
  static int code_line ;
  string error_string ;
  stack < token_node > l_paren_stack ;
  bool quoted ;
  bool is_digit( char ch ) {
    if ( '0' <= ch && ch <= '9' ) {
      return true ;
    } // if

    return false ;
  } // is_digit()

  bool is_letter( char ch ) {
    if ( 'a' <= ch && ch <= 'z' ) {
      return true ;
    } // if

    if ( 'A' <= ch && ch <= 'Z' ) {
      return true ;
    } // if

    return false ;
  } // is_letter()

  bool is_white_space( char ch ) {
    if ( ch == ' ' || ch == '\t' || ch == '\n' ) {
      return true ;
    } // if

    return false ;
  } // is_white_space()

  bool is_delimiter( char ch ) {
    if ( ch == '+' || ch == '-' || ch == '*' ) {
      return true ;
    } // if

    if ( ch == '/' || ch == '=' || ch == '<' ) {
      return true ;
    } // if

    if ( ch == '>' || ch == '\\' || ch == '&' ) {
      return true ;
    } // if

    if ( ch == '(' || ch == ')' ) {
      return true ;
    } // if

    return false ;
  } // is_delimiter()

  bool is_dot( char ch ) {
    if ( ch == '.' ) {
      return true ;
    } // if

    return false ;
  } // is_dot()

  bool is_end_of_line( char ch ) {
    if ( ch == '\0' ) {
      return true ;
    } // if

    return false ;
  } // is_end_of_line()

  bool is_double_quote( char ch ) {
    if ( ch == '\"' ) {
      return true ;
    } // if

    return false ;
  } // is_double_quote()

  bool is_quote( char ch ) {
    if ( ch == '\'' ) {
      return true ;
    } // if

    return false ;
  } // is_quote()

  bool is_comment( char ch ) {
    if ( ch == ';' ) {
      return true ;
    } // if

    return false ;
  } // is_comment()

  bool start_with_left_paren( char ch, tn_ptr & token_segement ) {
    if ( ch == '(' ) {
      token_segement = new token_node ;
      token_segement -> token = "(" ;
      token_segement -> token_type = LEFT_PAREN ;
      token_segement -> line = code_line ;
      token_segement -> column = code_column + 1 ;
      token_segement -> second_level = NULL ;
      token_segement -> next = NULL ;
      if ( quoted ) {
        l_paren_stack.push( * token_segement ) ;
      } // if

      while ( is_white_space( ch = getch() ) )
        ;
      if ( ch == ')' ) {
        token_segement -> token = "nil" ;
        token_segement -> token_type = NIL ;
        if ( quoted ) {
          l_paren_stack.pop() ;
        } // if
      } // if
      else {
        code_column -= 1 ;
      } // else

      return true ;
    } // if

    return false ;
  } // start_with_left_paren()

  bool start_with_right_paren( char ch, tn_ptr & token_segement ) {
    if ( ch == ')' ) {
      token_segement = new token_node ;
      token_segement -> token = ")" ;
      token_segement -> token_type = RIGHT_PAREN ;
      token_segement -> line = code_line ;
      token_segement -> column = code_column + 1 ;
      token_segement -> second_level = NULL ;
      token_segement -> next = NULL ;
      if ( quoted ) {
        l_paren_stack.pop() ;
      } // if

      return true ;
    } // if

    return false ;
  } // start_with_right_paren()

  bool start_with_digit( char ch, tn_ptr & token_segement ) {
    string temp_str = {
    }; // start_with_digit()

    if ( !is_digit( ch ) ) {
      return false ;
    } // if

    temp_str += ch ;
    token_segement = new token_node ;
    token_segement -> line = code_line ;
    token_segement -> column = code_column + 1 ;
    token_segement -> second_level = NULL ;
    token_segement -> next = NULL ;
    while ( is_digit( ch = getch() ) ) {
      temp_str += ch ;
    } // while

    if ( is_white_space( ch ) || is_delimiter( ch ) || is_end_of_line( ch ) || is_comment( ch ) ) {
      token_segement -> token = temp_str ;
      token_segement -> token_type = INT ;
      if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
        code_column -= 1 ;
      } // if

      if ( is_comment( ch ) ) {
        skip_comment( ch ) ;
      } // if

      return true ;
    } // if

    if ( ch == '.' ) {
      temp_str += ch ;
      while ( is_digit( ch = getch() ) ) {
        temp_str += ch ;
      } // while

      token_segement -> token_type = FLOAT ;
      if ( is_white_space( ch ) || is_delimiter( ch ) || is_end_of_line( ch ) || is_comment( ch ) ) {
        int dot_index = to_string( stof( temp_str ) ).find_first_of( ".", 0 ) ;
        token_segement -> token = to_string( stof( temp_str ) ).substr( 0, dot_index + 4 ) ;
        if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
          code_column -= 1 ;
        } // if

        if ( is_comment( ch ) ) {
          skip_comment( ch ) ;
        } // if

        return true ;
      } // if
    } // if
    else if ( !is_white_space( ch ) && !is_delimiter( ch ) && !is_end_of_line( ch ) && !is_comment( ch ) ) {
      temp_str += ch ;
      while ( !is_white_space( ch = getch() ) && !is_delimiter( ch ) && !is_end_of_line( ch ) && !is_comment( ch ) ) {
        temp_str += ch ;
      } // while

      if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
        code_column -= 1 ;
      } // if

      if ( is_comment( ch ) ) {
        skip_comment( ch ) ;
      } // if

      token_segement -> token = temp_str ;
      token_segement -> token_type = SYMBOL ;
      return true ;
    } // else if

    temp_str += ch ;
    while ( !is_white_space( ch = getch() ) && !is_delimiter( ch ) && !is_end_of_line( ch ) ) {
      temp_str += ch ;
    } // while

    if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
      code_column -= 1 ;
    } // if

    token_segement -> token = temp_str ;
    token_segement -> token_type = SYMBOL ;
    return true ;
  } // class Cutter

  bool start_with_letter( char ch, tn_ptr & token_segement ) {
    string temp_str = {
    }; // start_with_letter()

    if ( !is_letter( ch ) ) {
      return false ;
    } // if

    token_segement = new token_node ;
    token_segement -> token_type = SYMBOL ;
    token_segement -> line = code_line ;
    token_segement -> column = code_column + 1 ;
    token_segement -> next = NULL ;
    token_segement -> second_level = NULL ;
    while ( !( is_delimiter( ch ) || is_white_space( ch ) || is_end_of_line( ch ) ) ) {
      temp_str += ch ;
      ch = getch() ;
    } // while

    token_segement -> token = temp_str ;
    if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
      code_column -= 1 ;
    } // if

    if ( is_comment( ch ) ) {
      skip_comment( ch ) ;
    } // if

    if ( token_segement -> token.compare( "nil" ) == 0 ) {
      token_segement -> token_type = NIL ;
    } // if

    if ( token_segement -> token.compare( "t" ) == 0 ) {
      token_segement -> token = "#t" ;
      token_segement -> token_type = T ;
    } // if

    if ( token_segement -> token.compare( "f" ) == 0 ) {
      token_segement -> token = "nil" ;
      token_segement -> token_type = NIL ;
    } // if

    return true ;
  }

  bool start_with_dot( char ch, tn_ptr & token_segement ) {
    string temp_str = {
    }; // start_with_dot()

    if ( !is_dot( ch ) ) {
      return false ;
    } // if

    token_segement = new token_node ;
    token_segement -> line = code_line ;
    token_segement -> column = code_column + 1 ;
    token_segement -> next = NULL ;
    token_segement -> second_level = NULL ;
    temp_str += ch ;
    if ( is_white_space( ch = getch() ) || is_delimiter( ch ) || is_end_of_line( ch ) || is_comment( ch ) ) {
      token_segement -> token = temp_str ;
      token_segement -> token_type = DOT ;
      if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
        code_column -= 1 ;
      } // if

      if ( is_comment( ch ) ) {
        skip_comment( ch ) ;
      } // if

      return true ;
    } // if

    while ( is_digit( ch ) ) {
      temp_str += ch ;
      ch = getch() ;
    } // while

    if ( is_white_space( ch ) || is_delimiter( ch ) || is_end_of_line( ch ) || is_comment( ch ) ) {
      int dot_index = to_string( stof( temp_str ) ).find_first_of( ".", 0 ) ;
      token_segement -> token = to_string( stof( temp_str ) ).substr( 0, dot_index + 4 ) ;
      token_segement -> token_type = FLOAT ;
      if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
        code_column -= 1 ;
      } // if

      if ( is_comment( ch ) ) {
        skip_comment( ch ) ;
      } // if

      return true ;
    } // if

    temp_str += ch ;
    while ( !( is_delimiter( ch = getch() ) || is_white_space( ch ) || is_end_of_line( ch ) ) ) {
      temp_str += ch ;
    } // while

    token_segement -> token = temp_str ;
    token_segement -> token_type = SYMBOL ;
    token_segement -> next = NULL ;
    token_segement -> second_level = NULL ;
    if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
      code_column -= 1 ;
    } // if

    if ( is_comment( ch ) ) {
      skip_comment( ch ) ;
    } // if

    return true ;
  }

  bool start_with_quote( char ch, tn_ptr & token_segement ) {
    if ( !is_quote( ch ) ) {
      return false ;
    } // if

    token_segement = new token_node ;
    token_segement -> token = "(" ;
    token_segement -> token_type = LEFT_PAREN ;
    token_segement -> line = code_line ;
    token_segement -> column = code_column + 1 ;
    token_segement -> second_level = NULL ;
    token_segement -> next = new token_node ;
    token_segement -> next -> token = "quote" ;
    token_segement -> next -> token_type = QUOTE ;
    token_segement -> next -> line = code_line ;
    token_segement -> next -> column = code_column + 1 ;
    token_segement -> next -> second_level = NULL ;
    token_segement -> next -> next = NULL ;
    quoted = true ;
    return true ;
  } // start_with_quote()

  tn_ptr add_r_paren( tn_ptr target ) {
    target -> next = new token_node ;
    target -> next -> token = ")" ;
    target -> next -> token_type = RIGHT_PAREN ;
    target -> next -> line = code_line ;
    target -> next -> column = code_column + 1 ;
    target -> next -> second_level = NULL ;
    target -> next -> next = NULL ;
    return target -> next ;
  } // add_r_paren()

  bool start_with_double_quote( char ch, tn_ptr & token_segement ) {
    string temp_str = "" ;
    if ( !is_double_quote( ch ) ) {
      return false ;
    } // if

    token_segement = new token_node ;
    token_segement -> line = code_line ;
    token_segement -> column = code_column + 1 ;
    token_segement -> next = NULL ;
    token_segement -> second_level = NULL ;
    temp_str += ch ;
    while ( !is_double_quote( ch = getch() ) && !is_end_of_line( ch ) ) {
      if ( ch != '\\' ) {
        temp_str += ch ;
      } // if
      else {
        ch = getch() ;
        if ( ch == 'n' ) {
          temp_str += '\n' ;
        } // if

        if ( ch == 't' ) {
          temp_str += '\t' ;
        } // if

        if ( ch == '\\' ) {
          temp_str += '\\' ;
        } // if

        if ( ch == '\'' ) {
          temp_str += '\'' ;
        } // if

        if ( ch == '\"' ) {
          temp_str += '\"' ;
        } // if
      } // else
    } // while

    if ( is_double_quote( ch ) ) {
      temp_str += ch ;
      token_segement -> token = temp_str ;
      token_segement -> token_type = STRING ;
      return true ;
    } // if

    error_string = "ERROR (no closing quote) : END-OF-LINE encountered at Line " + to_string( code_line ) + " Column " + to_string( code_column ) + "\n" ;
    throw ( error_string ) ;
    return false ;
  } // start_with_double_quote()

  bool start_with_other_sign( char ch, tn_ptr & token_segement ) {
    string temp_str = "" ;
    token_segement = new token_node ;
    token_segement -> line = code_line ;
    token_segement -> column = code_column + 1 ;
    token_segement -> next = NULL ;
    token_segement -> second_level = NULL ;
    if ( ch == '#' ) {

      temp_str += ch ;
      ch = getch() ;
      if ( ch == 't' ) {
        token_segement -> token_type = T ;
        token_segement -> token = "#t" ;
      } // if
      else if ( ch == 'f' ) {
        token_segement -> token_type = NIL ;
        token_segement -> token = "nil" ;
      } // else if
      else if ( is_white_space( ch ) || is_delimiter( ch ) || is_end_of_line( ch ) || is_comment( ch ) ) {
        token_segement -> token_type = SYMBOL ;
        token_segement -> token = "#" ;

      } // else if
      else {
        token_segement -> token_type = SYMBOL ;
        temp_str += ch ;
        token_segement -> token = temp_str ;
      } // else

      temp_str += ch ;
      while ( !( is_white_space( ch = getch() ) || is_delimiter( ch ) || is_end_of_line( ch ) || is_comment( ch ) ) ) {
        temp_str += ch ;
        token_segement -> token = temp_str ;
        token_segement -> token_type = SYMBOL ;
      } // while

      if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
        code_column -= 1 ;
      } // if

      if ( is_comment( ch ) ) {
        skip_comment( ch ) ;
      } // if

      return true ;
    } // if

    if ( ch == '+' || ch == '-' ) {
      int temp_index = code_column ;
      tn_ptr temp = NULL ;
      start_with_digit( getch(), temp ) ;
      if ( temp -> token_type == INT || temp -> token_type == FLOAT ) {
        if ( ch == '-' ) {
          temp -> token.insert( 0, 1, ch ) ;
        } // if

        token_segement = temp ;
        return true ;
      } // if

      code_column = temp_index ;
      start_with_dot( getch(), temp ) ;
      if ( temp -> token_type == FLOAT ) {
        if ( ch == '-' ) {
          temp -> token.insert( 0, 1, ch ) ;
        } // if

        token_segement = temp ;
        return true ;
      } // if

      code_column = temp_index ;
    } // if

    if ( is_comment( ch ) ) {
      skip_comment( ch ) ;
      token_segement = NULL ;
      return true ;
    } // if

    temp_str += ch ;
    token_segement -> token_type = SYMBOL ;
    while ( !( is_white_space( ch = getch() ) || is_delimiter( ch ) || is_end_of_line( ch ) || is_comment( ch ) ) ) {
      temp_str += ch ;
    } // while

    if ( is_delimiter( ch ) || is_end_of_line( ch ) ) {
      code_column -= 1 ;
    } // if

    if ( is_comment( ch ) ) {
      skip_comment( ch ) ;
    } // if

    token_segement -> token = temp_str ;
    return true ;
  } // start_with_other_sign()

  char getch() {
    code_column += 1 ;
    return source_ins[ code_column ] ;
  } // getch()

  char skip_white_space( char ch ) {
    while ( ch == ' ' || ch == '\t' || ch == '\n' ) {
      ch = getch() ;
    } // while

    return ch ;
  } // skip_white_space()

  void skip_comment( char ch ) {
    while ( !is_end_of_line( ch ) ) {
      ch = getch() ;
    } // while
  } // skip_comment()

  tn_ptr walk_to_last( tn_ptr walk ) {
    while ( walk -> next != NULL ) {
      walk = walk -> next ;
    } // while

    return walk ;
  } // walk_to_last()

  tn_ptr gnerate_token_node() {
    tn_ptr return_list = NULL ;
    char ch = skip_white_space( getch() ) ;
    if ( ch == '\0' ) {
      return NULL ;
    } // if

    if ( start_with_left_paren( ch, return_list ) ) {
      if ( l_paren_stack.empty() && quoted ) {
        add_r_paren( walk_to_last( return_list ) ) ;
        quoted = false ;
      } // if

      return return_list ;
    } // if

    if ( start_with_right_paren( ch, return_list ) ) {
      if ( l_paren_stack.empty() && quoted ) {
        add_r_paren( walk_to_last( return_list ) ) ;
        quoted = false ;
      } // if

      return return_list ;
    } // if

    if ( start_with_digit( ch, return_list ) ) {
      if ( l_paren_stack.empty() && quoted ) {
        add_r_paren( walk_to_last( return_list ) ) ;
        quoted = false ;
      } // if

      return return_list ;
    } // if

    if ( start_with_letter( ch, return_list ) ) {
      if ( l_paren_stack.empty() && quoted ) {
        add_r_paren( walk_to_last( return_list ) ) ;
        quoted = false ;
      } // if

      return return_list ;
    } // if

    if ( start_with_dot( ch, return_list ) ) {
      if ( l_paren_stack.empty() && quoted ) {
        add_r_paren( walk_to_last( return_list ) ) ;
        quoted = false ;
      } // if

      return return_list ;
    } // if

    if ( start_with_double_quote( ch, return_list ) ) {
      if ( l_paren_stack.empty() && quoted ) {
        add_r_paren( walk_to_last( return_list ) ) ;
        quoted = false ;
      } // if

      return return_list ;
    } // if

    if ( start_with_quote( ch, return_list ) ) {
      return return_list ;
    } // if

    if ( start_with_other_sign( ch, return_list ) ) {
      if ( l_paren_stack.empty() && quoted ) {
        add_r_paren( walk_to_last( return_list ) ) ;
        quoted = false ;
      } // if

      return return_list ;
    } // if

    return return_list ;
  } // gnerate_token_node()


  public :
  tn_ptr get_token_line() {

    code_line++ ;
    code_column = - 1 ;
    source_ins = new char[ 256 ] ;
    if ( cin.eof() ) {
      error_string = "ERROR (no more input) : END-OF-FILE encountered" ;
      throw ( error_string ) ;
    } // if

    cin.getline( source_ins, 256 ) ;
    tn_ptr result_head = NULL ;
    tn_ptr walker = gnerate_token_node() ;
    if ( walker != NULL ) {
      result_head = walker ;
      walker = walk_to_last( walker ) ;
      walker -> next = NULL ;
      walker -> second_level = NULL ;
    } // if

    while ( source_ins[ code_column ] != '\0' ) {
      walker -> next = gnerate_token_node() ;
      if ( walker != NULL ) {
        walker = walk_to_last( walker ) ;
        walker -> next = NULL ;
        walker -> second_level = NULL ;
      } // if
    } // while

    return result_head ;
  } // get_token_line()

  void test_print( tn_ptr head ) {
    tn_ptr walk = head ;
    cout << setw( 7 ) << "line" << setw( 7 ) << "column" << setw( 10 ) << "token" ;
    cout << setw( 15 ) << "token type" << endl ;
    while ( walk != NULL ) {
      cout << setw( 7 ) << walk -> line << setw( 7 ) << walk -> column << setw( 10 ) << walk -> token ;
      cout << setw( 15 ) << walk -> token_type << endl ;
      walk = walk -> next ;
    } // while
  } // test_print()

  void set_code_line( int line ) {
    code_line = line ;
  } // set_code_line()

  void clear() {
    source_ins = NULL ;
    quoted = false ;
    code_column = 1 ;
    code_line = 0 ;
    error_string = "" ;
  } // clear()

  Cutter() {
    source_ins = NULL ;
    quoted = false ;
    code_column = 1 ;
    code_line = 0 ;
    error_string = "" ;
  } // Cutter()
};

int Cutter::code_line = 0 ;
int Cutter::code_column = 1 ;

class Syntax_processor {

  protected :
  tn_ptr ins_tree ;

  public :
  void set_ins( tn_ptr ins_head ) {
    ins_tree = ins_head ;
  } // set_ins()

  void clear() {
  } // clear()
}; // class Syntax_processor


class Analyzer : Syntax_processor {

  private :
  Cutter cut ;
  tn_ptr token_list ;
  tn_ptr seq_index ;
  string error_string ;
  stack < token_node > l_paren_stack ;
  int first_left_paren_index ;
  int last_column ;
  bool tree_complete ;
  tn_ptr find_previous( tn_ptr head, tn_ptr tail ) {
    while ( head != NULL && head -> next != tail )
      head = head -> next ;
    return head ;
  } // find_previous()

  tn_ptr build_s_expression() {



    int dot_count = 0 ;
    while ( seq_index == NULL ) {
      seq_index = cut.get_token_line() ;

    } // while

    tn_ptr builder = seq_index ;
    tn_ptr s_exp_head = builder ;
    if ( seq_index -> token_type == DOT ) {
      error_string = "ERROR (unexpected token) : atom or '(' expected when token at Line " + to_string( builder -> line ) + " Column " + to_string( builder -> column ) + " is >>.<<\n" ;
      throw ( error_string ) ;
    } // if

    if ( seq_index -> token.compare( "exit" ) == 0 ) {
      if ( seq_index -> next == NULL ) {
        seq_index -> next = cut.get_token_line() ;
      } // if

      if ( seq_index -> next -> token_type == RIGHT_PAREN ) {
        error_string = "EXIT" ;
        throw ( error_string ) ;
      } // if
    } // if

    while ( 1 ) {
      while ( seq_index == NULL ) {
        seq_index = cut.get_token_line() ;
        if ( builder -> next == NULL ) {
          builder -> next = seq_index ;
        } // if

      } // while

      if ( seq_index -> token_type == LEFT_PAREN ) {
        builder = seq_index ;
        seq_index = seq_index -> next ;
        l_paren_stack.push( * builder ) ;
        builder -> second_level = build_s_expression() ;
        first_left_paren_index = l_paren_stack.top().column ;
        l_paren_stack.pop() ;
        builder -> next = seq_index ;
        if ( find_previous( builder -> second_level, seq_index ) != NULL ) {
          find_previous( builder -> second_level, seq_index ) -> next = NULL ;
        } // if

        builder = seq_index ;
        seq_index = seq_index -> next ;
        if ( l_paren_stack.empty() ) {
          builder -> next = NULL ;
          last_column = builder -> column ;
          return s_exp_head ;
        } // if
      } // if
      else if ( seq_index -> token_type == RIGHT_PAREN ) {
        if ( l_paren_stack.empty() )
          {