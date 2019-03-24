#ifndef _OPTIONS_H_
#define _OPTIONS_H_

// NOTE ~> Options is a singleton hidden away behind this interface. No matter
//  where these functions are called in the program, they will always be
//  interacting with the same structure of option variables.

void Options_setOutputFile(char* file);
char* Options_getOutputFile();
void Options_setInterfaceName(char* name);
char* Options_getInterfaceName();

#endif