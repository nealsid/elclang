#include <emacs-module.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int plugin_is_GPL_compatible;

int register_elisp_functions();

int emacs_module_init(struct emacs_runtime *runtime) {
  emacs_env *env = runtime->get_environment (runtime);
  register_elisp_functions();
  return 0;
}
