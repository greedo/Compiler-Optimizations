#ifndef _H_DF_BASE_TYPE
#define _H_DF_BASE_TYPE

#include <list>

#include "cfg_type.h"

enum DF_STATUS_TYPE {
  DF_NOT_INITIALIZED,
  DF_NOT_IN_TODO,
  DF_IN_TODO
};



class df_base_type : public cfg_type {

 public:

  df_base_type(std::list<Instruction*>* code);

  std::list<Instruction*>& get_all_nodes();

  virtual bool compute_gen_set(Instruction* node) = 0;

  virtual bool compute_kill_set(Instruction* node) = 0;

  virtual void init_in_set(Instruction* node) = 0;

  virtual void init_out_set(Instruction* node) = 0;

  virtual bool apply_meet_operator(Instruction* node) = 0;

  virtual bool apply_transfer_function(Instruction* node) = 0;
  
  virtual void status(Instruction* node, DF_STATUS_TYPE status) = 0;
  virtual DF_STATUS_TYPE status(Instruction* node) = 0; 

  virtual void apply_df_info() = 0;
  

};


#endif
