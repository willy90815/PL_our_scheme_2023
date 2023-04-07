# include <fstream>
# include <iostream>
# include <iomanip>
#include <stack>

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
  _token_node_ * second_level;
  _token_node_ * next ;
} token_node ;


typedef _token_node_ * tn_ptr ;

class Cutter {

  private :
  char * source_ins ;
  static int code_column ;
  static int code_line ;
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

  bool start_with_left_paren( char ch, tn_ptr & token_segement ) {
    if ( ch == '(' ) {
      token_segement = new token_node ;
      token_segement -> token = "(" ;
      token_segement -> token_type = LEFT_PAREN ;
      token_segement -> line = code_line ;
      token_segement -> column = code_column ;
      token_segement->second_level = NULL;
      token_segement -> next = NULL ;

      while(is_white_space(ch = getch()) );
      if( ch == ')'){
        token_segement -> token = "nil" ;
        token_segement -> token_type = NIL ;
      }
      else {
        code_column-=1;
      }
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
      token_segement->second_level = NULL;
      token_segement -> next = NULL ;
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
    token_segement -> column = code_column ;
      token_segement->second_level = NULL;
    token_segement -> next = NULL ;
    while ( is_digit( ch = getch() ) ) {
      temp_str += ch ;
    } // while

    if ( is_white_space( ch ) || is_delimiter( ch ) || is_end_of_line( ch ) ) {

      token_segement -> token = temp_str ;
      token_segement -> token_type = INT ;
      if ( is_delimiter( ch ) ) {
        code_column -= 1 ;
      } // if

      return true ;
    } // if

    if ( ch == '.' ) {

      temp_str += ch ;
      while ( is_digit( ch = getch() ) ) {
        temp_str += ch ;
      } // while getch before condiction
      token_segement -> token_type = FLOAT ;
      if ( is_white_space( ch ) || is_delimiter( ch ) || is_end_of_line( ch ) ) {
        token_segement -> token = temp_str ;
        return true ;
      } // if

    } // if
    else if ( !is_white_space( ch ) && !is_delimiter( ch ) && !is_end_of_line( ch ) ) {
      temp_str += ch ;
      while ( !is_white_space( ch = getch() ) && !is_delimiter( ch ) && !is_end_of_line( ch ) ) {
        temp_str += ch ;
        
      } // while

      if ( is_delimiter( ch ) ) {
        code_column -= 1 ;
      } // if

      token_segement -> token = temp_str ;
      token_segement -> token_type = SYMBOL ;
      return true ;
    } // else if

    temp_str += ch ; // for double dot like "3.33.3"
    while ( !is_white_space( ch = getch() ) && !is_delimiter( ch ) && !is_end_of_line( ch ) ) {
      temp_str += ch ;
    } // while

    if ( is_delimiter( ch ) ) {
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
    token_segement -> column = code_column ;
    token_segement -> next = NULL ;
    token_segement->second_level = NULL;
    while ( !( is_delimiter( ch ) || is_white_space( ch ) || is_end_of_line( ch ) ) ) {
      temp_str += ch ;
      ch = getch() ;
    } // while

    
    token_segement -> token = temp_str ;
    if ( is_delimiter( ch ) ) {
      code_column -= 1 ;
    } // if

    if ( token_segement -> token.compare( "nil" ) == 0 ) {
      token_segement -> token_type = NIL ;
    } // if
    if ( token_segement -> token.compare( "t" ) == 0 ) {
      token_segement->token = "#t";
      token_segement -> token_type = T ;
    } // if
    if ( token_segement -> token.compare( "f" ) == 0 ) {
      token_segement->token = "nil";
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
    token_segement -> column = code_column ;
    token_segement -> next = NULL ;
    token_segement->second_level = NULL;
    temp_str += ch ;
    if ( is_white_space( ch = getch() ) || is_delimiter( ch ) || is_end_of_line( ch ) ) {
      token_segement -> token = temp_str ;
      token_segement -> token_type = DOT ;
      if ( is_delimiter( ch ) ) {
        code_column -= 1 ;
      } // if

      return true ;
    } // if

    while ( is_digit( ch ) ) {
      temp_str += ch ;
      ch = getch() ;
    } // while

    if ( is_white_space( ch ) || is_delimiter( ch ) || is_end_of_line( ch ) ) {
      token_segement -> token = temp_str ;
      token_segement -> token_type = FLOAT ;
      if ( is_delimiter( ch ) ) {
        code_column -= 1 ;
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
    token_segement->second_level = NULL;
    if ( is_delimiter( ch ) ) {
      code_column -= 1 ;
    } // if

    return true ;
  }

  bool start_with_quote( char ch, tn_ptr & token_segement ) {
    return false ;
  } // start_with_quote()

  bool start_with_double_quote( char ch, tn_ptr & token_segement ) {
    
    string temp_str = ""; // start_with_dot()

    if(!is_double_quote(ch)){
      return false ;
    }

    token_segement = new token_node ;
    token_segement -> line = code_line ;
    token_segement -> column = code_column ;
    token_segement -> next = NULL ;
    token_segement->second_level = NULL;
    temp_str+=ch;
    
    while(!is_double_quote(ch = getch())&& !is_end_of_line(ch)){
      if(ch !='\\'){
        temp_str+=ch;
      }
      else{
        ch=getch();
        if(ch=='n'||ch=='t'||ch=='\\'||ch=='\''||ch == '\"'){
          temp_str+='\n';
        }
        if(ch=='t'){
          temp_str+='\t';
        }
        if(ch=='\\'){
          temp_str+='\\';
        }
        if(ch=='\''){
          temp_str+='\'';
        }
        if(ch=='\"'){
          temp_str+='\"';
        }
      }

    }
    if( is_double_quote(ch) ){
      temp_str+=ch;
      token_segement->token = temp_str;
      token_segement->token_type = STRING;
      return true ;
    }

    cout<<"error withot quote end line : "<<token_segement->line;
    cout<<"column:"<<token_segement->column<<endl;
    return false ;
    
  } // start_with_double_quote()

  bool start_with_other_sign( char ch, tn_ptr & token_segement ) {
    string temp_str="";
    token_segement = new token_node ;
    token_segement -> line = code_line ;
    token_segement -> column = code_column ;
    token_segement -> next = NULL ;
    token_segement->second_level = NULL;
    if(ch=='#'){
      // what if only #
      temp_str+=ch;
      ch = getch();
      if(ch == 't'){
        token_segement->token_type = T;
        token_segement->token = "#t";
      }
      else if(ch == 'f'){
        token_segement->token_type = NIL;
        token_segement->token = "nil";
      }
      else if(is_white_space(ch)||is_delimiter(ch)||is_end_of_line(ch)){
        ;//maybe we should throw an exception;
      }
      else {
        token_segement->token_type = SYMBOL;
        temp_str+=ch;
        token_segement->token = temp_str;
      }

      temp_str+=ch;
      while(!(is_white_space(ch = getch())||is_delimiter(ch)||is_end_of_line(ch))){
        temp_str+=ch;
      }
      token_segement->token = temp_str;
      token_segement->token_type = SYMBOL;
      if(is_delimiter(ch)){
        code_column-=1;
      }
    return true ;
    } // start with #

    if( ch == ';'){
      while(!is_end_of_line(ch)){
        ch=getch();
      }
      token_segement = NULL;
      return true ;
    }  

    return false ;///////////////////////////////////
  } // start_with_other_sign()



  char getch() {
    code_column += 1 ;
    return source_ins[ code_column - 1 ] ;
  } // getch()

  char skip_white_space( char ch ) {
    while ( ch == ' ' || ch == '\t' || ch == '\n' ) {
      ch = getch() ;
    } // while

    return ch ;
  } // skip_white_space()

  tn_ptr walk_to_last( tn_ptr walk ) {
    while ( walk -> next != NULL ) {
      walk = walk -> next ;
    } // while

    return walk ;
  } // walk_to_last()
  tn_ptr gnerate_token_node() {
    tn_ptr return_list = NULL ;
    char ch = skip_white_space( getch() ) ;
    if( ch == '\0'){
      return NULL;
    }
    if ( start_with_left_paren( ch, return_list ) ) {
      return return_list ;
    } // if

    if ( start_with_right_paren( ch, return_list ) ) {
      return return_list ;
    } // if

    if ( start_with_digit( ch, return_list ) ) {
      return return_list ;
    } // if

    if ( start_with_letter( ch, return_list ) ) {
      return return_list ;
    } // if

    if ( start_with_dot( ch, return_list ) ) {
      return return_list ;
    } // if

    if ( start_with_double_quote( ch, return_list ) ) {
      return return_list ;
    } // if
    if( start_with_other_sign( ch, return_list ) ) {
      return return_list;
    }

    return return_list ;
  } // gnerate_token_node()

  public :
  tn_ptr get_token_line() {
    // should I free or delete privious pointer source_ins 
    source_ins = new char[256];
    cin.getline( source_ins,256 );
    code_column = 0;
    tn_ptr result_head = NULL ;
    tn_ptr walker = gnerate_token_node() ;
    if( walker != NULL ){
      walker -> next = NULL ;
      walker->second_level = NULL;
    }
    
    result_head = walker ;
    while ( source_ins[ code_column ] != '\0' ) {
      walker -> next = gnerate_token_node() ;
      walker = walker -> next ;
      if( walker != NULL ){
        walker -> next = NULL ;
        walker->second_level = NULL;
      }
    } // while
    return result_head ;
  } // generate_list()



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
};

int Cutter::code_line = 0 ;
int Cutter::code_column = 0 ;

class Syntax_processor {
  protected:
  tn_ptr ins_tree;
  public :
  void set_ins(tn_ptr ins_head) {
    ins_tree = ins_head;
  } // set_ins()

  void clear() {
  } // clear()
}; // class Syntax_processor


class Analyzer : Syntax_processor {
  private:
  Cutter cut;
  tn_ptr token_list;
  tn_ptr seq_index;
  stack<token_node> l_paren_stack;
  int first_left_paren_index;

  tn_ptr build_s_expression(){
    // 4 situation
    // ( ( 1 . 2 ) 3 4 ) 
    // ( 1 2 3 4 )
    // ( 1 2 . ( 3 . 4 ) )
    // ( 1 . ( ( 2 . 3 ) . 4  )
    // when should nil be showed
    // without '(' ')' did not finish
    if(seq_index == NULL ){
      seq_index = cut.get_token_line();
      cut.test_print( seq_index ) ;
    }

    tn_ptr builder = seq_index;
    tn_ptr s_exp_head = builder;
    int dot_count = 0;
    if( seq_index == NULL){
      return NULL;
    }

    if( seq_index->token_type == DOT ){
      if(builder->token_type == DOT )
      ;//error occur
    }
    
  
    while( seq_index!=NULL){
      if( seq_index->token_type == LEFT_PAREN ){
        builder = seq_index;
        seq_index = seq_index->next;
        l_paren_stack.push(*builder);
        builder->second_level = build_s_expression();
        first_left_paren_index = l_paren_stack.top().column;
        l_paren_stack.pop();
        builder->next = seq_index;
        if( seq_index == NULL){
          return s_exp_head;
        }
        
      }
      else if( seq_index->token_type == RIGHT_PAREN){
        builder->next = NULL;   
        if( l_paren_stack.empty()){
          ;//error occur
        }
        
        return s_exp_head;
      }
      else if( seq_index->token_type == DOT ){
        if(builder->token_type == DOT ){
          ;//error occur DOT is first token in s_expression
        }
        if( seq_index->next!=NULL&&seq_index->next->token_type == LEFT_PAREN){
          
          l_paren_stack.push(*seq_index->next);
          seq_index = seq_index->next->next;
          builder->next = build_s_expression();
          first_left_paren_index = l_paren_stack.top().column;
          l_paren_stack.pop();
          if( seq_index == NULL){
            return s_exp_head;
          }
        }
        else if( seq_index->next == NULL){
          ;//? should be an error?
        }
        else{
          builder->next = seq_index->next;
        }
        

      }
        builder = seq_index;
        seq_index = seq_index->next;

    }
        
    

    
    


    return s_exp_head;
  }
  public:
  Analyzer(){
    this->ins_tree= NULL;
    this->seq_index = NULL;
    this->token_list = NULL;
    this->first_left_paren_index = 0;
    this->cut = Cutter();
  }
  void clear(){
    this->ins_tree= NULL;
    this->seq_index = NULL;
    this->token_list = NULL;
    this->first_left_paren_index = 0;
  }

  tn_ptr build_ins_tree(){
    return build_s_expression();
  }
  bool tree_is_complete(){
    return false;
  }
  bool token_list_is_empty(){
    if( seq_index == NULL ){
      return true;
    }
    return false;
  }

  tn_ptr get_ins_tree(){
    return ins_tree;
  }
  
  void print_tree_struct(tn_ptr head,int indent){
    tn_ptr walk = head;
    while( walk!=NULL){
      cout<<setw(indent)<<""<<walk->token<<endl;
      if( walk->second_level!= NULL){
        print_tree_struct( walk->second_level,indent+2 );
      }
      walk= walk->next;
    }

    return;
    
  }
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
  tn_ptr head;
  Cutter cut = Cutter() ;
  Analyzer anal = Analyzer() ;
  // instruction cross line will occur segmentation fault 
  while(1){
    anal.print_tree_struct(anal.build_ins_tree(),0);
    anal.clear();
  }
  
  return 0;
  while(0){



    if(anal.token_list_is_empty()){
      while(!anal.tree_is_complete()){
        head = anal.build_ins_tree();
      }
      anal.print_tree_struct(head,0);
    }
    else{
      anal.print_tree_struct(anal.build_ins_tree(),0);
    }
    //one iteration generate one complete ins tree
  }
  
} // main()