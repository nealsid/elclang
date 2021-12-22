#include <emacs-module.h>

#define RETURN_NIL() return env->intern(env, "nil");

void emacs_message(emacs_env *env, const char* logMessageFormat, ... );
void copy_string_from_args(emacs_env *env, emacs_value *args, ptrdiff_t arg_number, char** argument);
