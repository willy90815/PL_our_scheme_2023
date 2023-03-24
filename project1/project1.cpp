# include <fstream>
# include <iostream>
# include <iomanip>

using namespace std ;

enum _token_type {
  LEFT_PAREN,
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
  _token_node_ * next ;
} token_node ;


typedef _token_node_ * tn_ptr ;

class Cutter {

  private :
  char * source_ins ;
  static int code_column ;
  static int code_line ;
  bool is_digit(char ch){
    if ( '0'<= ch && ch <='9'){
      return true;
    }
    return false;
  }

  bool is_letter(char ch){
    if( 'a' <= ch && ch <= 'z'){
      return true;
    }

    if ( 'A' <= ch && ch <='Z'){
      return true;
    }
    return false;
    
  }

  bool is_white_space(char ch){
    if(ch == ' ' || ch == '\t' || ch == '\n'){
      return true;
    }
  }

  bool is_delimiter(char ch){
    if (ch == '+'||ch == '-' || ch == '*'){
      return true;
    }

    if( ch == '/'||ch == '='||ch =='<'){
      return true;
    }
    if(ch=='>'|| ch =='\\' || ch == '&'){
      return true;
    }
    if(ch =='('||ch == ')'){
      return true;
    }

    return false;
  }



  bool start_with_left_paren( char ch, tn_ptr & token_segement ) {
    if ( ch == '(' ) {
      token_segement = new token_node ;
      token_segement -> token = "(" ;
      token_segement -> token_type = LEFT_PAREN ;
      token_segement -> line = code_line ;
      token_segement -> column = code_column ;
      token_segement -> next = NULL ;
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
      token_segement -> column = code_column ;
      token_segement -> next = NULL ;
      return true ;
    } // if

    return false ;
  } // start_with_right_paren()

  bool start_with_digit( char ch, tn_ptr & token_segement ) {
    if ( !is_digit( ch ) ) {
      return false ;
    } // if
    int loc_column = code_column ;
    string temp_str ;
    temp_str.append( & ch ) ;

    while ( is_digit( ch ) ) {
      ch = getch() ;
      temp_str.append( & ch ) ;
    } // while
    if( is_white_space(ch) || is_delimiter(ch)){
      token_segement = new token_node ;
      token_segement -> token = temp_str ;
      token_segement -> token_type = INT ;
      token_segement -> line = code_line ;
      token_segement -> column = loc_column ;
      token_segement -> next = NULL ;
      if(is_delimiter(ch)){
        code_column -= 1 ;
      }
      return true ;
    }
    if ( ch == '.' ) {
      token_segement = new token_node ;
      
      token_segement -> token_type = SYMBOL ;
      token_segement -> line = code_line ;
      token_segement -> column = code_column ;
      token_segement -> next = NULL ;
      code_column -= 1 ;
      return true ;
    } // if
    if ( ch != '.' ) {
      token_segement = new token_node ;
      
      token_segement -> token_type = SYMBOL ;
      token_segement -> line = code_line ;
      token_segement -> column = code_column ;
      token_segement -> next = NULL ;
      code_column -= 1 ;
      return true ;
    } // if

    
  } // start_with_digit()

  bool start_with_letter( char ch, tn_ptr & token_segement ) {
    return false ;
  } // start_with_letter()

  bool start_with_dot( char ch, tn_ptr & token_segement ) {
    return false ;
  } // start_with_dot()

  bool start_with_quote( char ch, tn_ptr & token_segement ) {
    return false ;
  } // start_with_quote()

  bool start_with_double_quote( char ch, tn_ptr & token_segement ) {
    return false ;
  } // start_with_double_quote()


  public :
  void set_source_ins( char * source_ins ) {
    this -> source_ins = source_ins ;
  } // set_source_ins()

  char getch() {
    code_column += 1 ;
    return source_ins[ code_column - 1 ] ;
  } // getch()

  char skip_white_space(char ch){
    while( ch == ' ' || ch == '\t' || ch == '\n'){
      ch = getch();
    }
    return ch;
  }
  tn_ptr walk_to_last( tn_ptr walk ) {
    while ( walk -> next != NULL ) {
      walk = walk -> next ;
    } // while

    return walk ;
  } // walk_to_last()

  tn_ptr generate_list() {
    tn_ptr result_head = NULL ;
    tn_ptr walker = gnerate_token_node() ;
    walker -> next = NULL ;
    result_head = walker ;
    while ( source_ins[ code_column ] != '\0' ) {
      walker -> next = gnerate_token_node() ;
      walker = walker -> next ;
      walker -> next = NULL ;
    } // while

    return result_head ;
  } // generate_list()

  tn_ptr gnerate_token_node() {
    tn_ptr return_list = NULL ;
    char ch = skip_white_space(getch()) ;
    if ( start_with_left_paren( ch, return_list ) ) {
      return return_list ;
    } // if

    if ( start_with_right_paren( ch, return_list ) ) {
      return return_list ;
    } // if



    return return_list ;
  } // gnerate_token_node()

  void test_print( tn_ptr head ) {
    tn_ptr walk = head ;
    cout << setw( 5 ) << "line" << setw( 5 ) << "column" << setw( 10 ) << "token" ;
    cout << setw( 5 ) << "token type" << endl ;
    while ( walk != NULL ) {
      cout << setw( 5 ) << walk -> line << setw( 5 ) << walk -> column << setw( 10 ) << walk -> token ;
      cout << setw( 5 ) << walk -> token_type << endl ;
      walk = walk -> next ;
    } // while
  } // test_print()
}; // class Cutter

int Cutter::code_line = 0 ;
int Cutter::code_column = 0 ;

class Syntax_processor {

  protected :
  void set_ins() {
  } // set_ins()

  void clear() {
  } // clear()
}; // class Syntax_processor


class Analyzer : Syntax_processor {
}; // class Analyzer


class Generator : Syntax_processor {
}; // class Generator


class IO {
  Cutter cut = Cutter() ;

  public :
  tn_ptr get_token_line() {
    char * line = new char[ 256 ] ;
    cin.getline( line, 256 ) ;
    cut ;
    tn_ptr head ;
    return head ;
  } // get_token_line()
}; // class IO


class Interpreter {

  private :
  IO io = IO() ;
  Analyzer anal = Analyzer() ;
  Generator gen = Generator() ;
}; // class Interpreter

int main() {
  Cutter cut = Cutter() ;
  char test_str[ 50 ] = "( ( )   )  (" ;
  cut.set_source_ins( test_str ) ;
  cut.test_print( cut.generate_list() ) ;
} // main()