#ifndef _H_DF_FRAMEWORK
#define _H_DF_FRAMEWORK

#include <map>
#include <list>

#include "df_base_type.h"

enum DF_DIRECTION_TYPE {
  DF_FORWARD,
  DF_BACKWARD
};


class df_framework_type {

 public:
  
  df_framework_type(df_base_type* df_cfg, DF_DIRECTION_TYPE direction);
  
  void run_df_framework();
  
 private: // private methods
  
  void _init_df_framework(std::list<Instruction*>& todo);

 private: // private members
  
  DF_DIRECTION_TYPE _direction;
  df_base_type* _df_cfg;

};


#endif
