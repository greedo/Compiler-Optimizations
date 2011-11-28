#ifndef _H_CFG_TYPE
#define _H_CFG_TYPE

#include <map>
#include <vector>
#include <list>

class Instruction;

typedef std::vector<Instruction*> edge_type;  

// this class is a wrapper that represents the cfg. There is a mapping 
// for every (Stmt) node to all the nodes that can follow (or precede)
// in the control flow graph. 
class cfg_type {

 private:

  static edge_type empty_edges;

  //std::map<Instruction*,edge_type> _in_edges;
  //std::map<Instruction*,edge_type> _out_edges;
  Instruction* _first_instruction;
  Instruction* _last_instruction;

 protected:

  std::list<Instruction*>* _instructions;
  
 public:
 
  std::map<int, std::list<Instruction*> > basic_block;
  std::map<int, std::list<int> > in_edges;
  std::map<int, std::list<int> > out_edges;
  
  //cfg_type(std::list<Instruction*>* code);
  cfg_type();
  
  void  first_instruction(Instruction* first);
  Instruction* first_instruction();
  
  void  last_instruction(Instruction* last);
  Instruction* last_instruction();

  void reverse_cfg();

  //edge_type& in_edges(Instruction* stmt);

  //edge_type& out_edges(Instruction* stmt);

  //void add_in_edge(Instruction* from, Instruction* to);

  //void add_out_edge(Instruction* from, Instruction* to);
  
  void add_basic_block(int id, std::list<Instruction*> graph);
  
  void add_in_edge(int id, std::list<int> i_edges);
  
  void add_out_edge(int id, std::list<int> o_edges);
};

#endif
