#include <vector>

#include "df_framework.h"



//////////////////////////////////////////////////////////////////
//                   PRIVATE METHODS                            //
//////////////////////////////////////////////////////////////////


void df_framework_type::_init_df_framework(std::list<Instruction*>& todo) {
  // the todo list will include all the instructions in the cfg
  std::list<Instruction*>& instructions = _df_cfg->get_all_nodes();
  std::list<Instruction*>::iterator todo_it = instructions.begin();
  for (; todo_it != instructions.end(); ++todo_it) {
    _df_cfg->compute_gen_set((*todo_it));
    _df_cfg->compute_gen_set((*todo_it));
    _df_cfg->init_in_set((*todo_it));
    _df_cfg->init_in_set((*todo_it));
    todo.push_back((*todo_it));
  }

}


//////////////////////////////////////////////////////////////////
//                      CONSTRUCTORS                            //
//////////////////////////////////////////////////////////////////

df_framework_type::df_framework_type(df_base_type* df_cfg,DF_DIRECTION_TYPE direction) {
  _df_cfg = df_cfg;
  _direction = direction;
}
    

//////////////////////////////////////////////////////////////////
//                    PUBLIC METHODS                            //
//////////////////////////////////////////////////////////////////



void df_framework_type::run_df_framework() {
  // if backward we need to reverse the cfg
  if (_direction == DF_BACKWARD) {
    _df_cfg->reverse_cfg();
  }

  std::list<Instruction*> todo;
  // next initialize the nodes
  _init_df_framework(todo);
  
  // start the iterative data flow. All the nodes
  // will be processed until a fixed point is reached 
  // (i.e. no change)  
  while (todo.size() > 0) {
    Instruction* node =  todo.front();
    todo.pop_front();
    _df_cfg->status(node,DF_NOT_IN_TODO);
    // first apply the meet operator
    _df_cfg->apply_meet_operator(node);
    // next apply the transfer function
    bool changed = _df_cfg->apply_transfer_function(node);
    // if applying the transfer function changed the data sets
    // the out edges of this node need to be added to the todo
    // list since they might also change.
    if (changed) {
      std::vector<Instruction*>& next_nodes = _df_cfg->out_edges(node);
      for (int cnt=0; cnt < next_nodes.size(); ++cnt) {
	Instruction* next_node = next_nodes[cnt];
	// only need to add to todo list if not there already
	if (_df_cfg->status(next_node) != DF_IN_TODO) {
	  _df_cfg->status(next_node,DF_IN_TODO);
	  todo.push_back(next_node);
	}
      }
    }
  }
}

