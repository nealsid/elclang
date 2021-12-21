#include <emacs-module.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emacs-module-utilities.h"

int plugin_is_GPL_compatible;

int register_elisp_functions();

int emacs_module_init(struct emacs_runtime *runtime) {
  env = runtime->get_environment (runtime);
  register_elisp_functions();
  emacs_nil = env->intern(env, "nil");
  return 0;
}
