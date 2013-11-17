#include <stdio.h>

#include "../mpc/mpc.h"

int main(int argc, char** argv) {

  mpc_parser_t* Flatmate = mpc_or(4, 
    mpc_string("Dan"),
    mpc_string("Chess"),
    mpc_string("Adam"),
    mpc_string("Lewis")
  );

  mpc_parser_t* Greet = mpc_and(2, mpcf_strfold,
    mpc_string("Hello "),
    Flatmate, 
    free); 

  mpc_parser_t* Greetings = mpc_many(mpcf_strfold, Greet);

  mpc_delete(Greetings);
  
  return 0;
  
}

