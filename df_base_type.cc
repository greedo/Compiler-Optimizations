#include "df_base_type.h"


df_base_type::df_base_type(std::list<Instruction*>* code) : cfg_type() {
  ;/*nothing to do*/
}

std::list<Instruction*>& df_base_type::get_all_nodes() {
  return *_instructions;
}
