#include "df_live_var.h"
#include "tac.h"
#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

class Instruction;

void df_live_var::get_live_locations(std::list<Instruction*> stmt, int id)
{

	std::string str_tmp ("=");
	
	size_t found;

	std::map<std::string, std::pair<int,int> > current_live_vars;

	std::vector<std::string> fails;

	std::string what_temp = "";

	std::list<Instruction*>::iterator it;
	for(it=stmt.begin(); it != stmt.end(); it++)
	{
		found = (*it)->command().find(str_tmp);
		if(found!=std::string::npos)
		{
			std::string str_varName ("tmp");
			found = (*it)->command().find(str_varName);
			int beg = (*it)->command().find_first_of("tmp");
			int end = (*it)->command().find_last_of("tmp");
			what_temp = (*it)->command().substr(beg,end+1);
			fails.push_back( what_temp );	
		}
	}
	
	// remove duplicates
	fails.erase( unique(fails.begin(), fails.end() ), fails.end() );
	
	/*for(int i=0; i < fails.size(); i++)
	{
		std::cout << fails[i] << std::endl; 
	}*/

	// handle one at a time
	while( !fails.empty() )
	{	
		int line = 1;
		int start;
		int end;
	
		bool got_one = false;
		bool got_end = false;
		std::string what_temp = "";
		
		std::list<Instruction*>::iterator it;
		for(it=stmt.begin(); it != stmt.end(); it++)
		{
		
			if(got_one)
			{
				got_end = true;
			}
		
			found = (*it)->command().find( fails.at( fails.size()-1 ) );
			if (!got_one && !got_end && found!=std::string::npos)
			{
				start = line;
				//std::cout << "start: " << start << std::endl;
				//std::cout << "variable: " << (*it)->command() << std::endl;
				got_one = true;
			}
		
			found = (*it)->command().find( fails.at( fails.size()-1 ) );
			
			if(got_one && got_end && found!=std::string::npos)
			{
				end = line;
				//std::cout << "end: " << end << std::endl;
				//std::cout << "variable: " << (*it)->command() << std::endl;
				break;
			}
		
			line += 1;
		}
		
		current_live_vars.insert( std::pair<std::string, std::pair<int,int> >(fails.at(fails.size()-1), std::pair<int,int>(start,end) ));
		
		fails.pop_back();
	}
	
	variable_timeline.insert( std::pair<int, std::map<std::string, std::pair<int,int> > > (id,current_live_vars) );
}
