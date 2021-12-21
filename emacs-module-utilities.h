#include <emacs-module.h>

extern emacs_value emacs_nil;
extern emacs_env *env;

#define RETURN_NIL() return emacs_nil;

void emacs_message(emacs_env *env, const char* logMessageFormat, ... );
void copy_string_from_args(emacs_env *env, emacs_value *args, ptrdiff_t arg_number, char** argument);
