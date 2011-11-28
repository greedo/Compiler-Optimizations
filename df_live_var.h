#ifndef _H_DF_LIVE_VAR
#define _H_DF_LIVE_VAR

#include "df_framework.h"
#include <map>
#include <string>

//class df_live_var : public df_base_type {
class df_live_var
{
  //df_live_var(std::list<Instruction*>* code);

  //void get_live_locations(Instruction* instruction);
  public:
	std::map<int, std::map<std::string, std::pair<int,int> > > variable_timeline;
  
	void get_live_locations(std::list<Instruction*>, int id);
};

#endif
