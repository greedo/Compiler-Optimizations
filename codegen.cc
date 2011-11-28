/* File: codegen.cc
 * ----------------
 * Implementation for the CodeGenerator class. The methods don't do anything
 * too fancy, mostly just create objects of the various Tac instruction
 * classes and append them to the list.
 */

#include "codegen.h"
#include <string.h>
#include "tac.h"
#include "mips.h"
#include "ast_decl.h"
#include "errors.h"
#include "df_const_prop.h"
#include "df_live_var.h"
#include "cfg_type.h"
#include <iostream>
#include <string>
#include <vector>

  
CodeGenerator::CodeGenerator()
{
  code = new List<Instruction*>();
  curGlobalOffset = 0;
}

char *CodeGenerator::NewLabel()
{
  static int nextLabelNum = 0;
  char temp[10];
  sprintf(temp, "_L%d", nextLabelNum++);
  return strdup(temp);
}


Location *CodeGenerator::GenTempVar()
{
  static int nextTempNum;
  char temp[10];
  Location *result = NULL;
  sprintf(temp, "_tmp%d", nextTempNum++);
  return GenLocalVariable(temp);
}

  
Location *CodeGenerator::GenLocalVariable(const char *varName)
{            
    curStackOffset -= VarSize;
    return new Location(fpRelative, curStackOffset+4,  varName);
}

Location *CodeGenerator::GenGlobalVariable(const char *varName)
{
    curGlobalOffset += VarSize;
    return new Location(gpRelative, curGlobalOffset -4, varName);
}


Location *CodeGenerator::GenLoadConstant(int value)
{
  Location *result = GenTempVar();
  code->Append(new LoadConstant(result, value));
  return result;
}

Location *CodeGenerator::GenLoadConstant(const char *s)
{
  Location *result = GenTempVar();
  code->Append(new LoadStringConstant(result, s));
  return result;
} 

Location *CodeGenerator::GenLoadLabel(const char *label)
{
  Location *result = GenTempVar();
  code->Append(new LoadLabel(result, label));
  return result;
} 


void CodeGenerator::GenAssign(Location *dst, Location *src)
{
  code->Append(new Assign(dst, src));
}


Location *CodeGenerator::GenLoad(Location *ref, int offset)
{
  Location *result = GenTempVar();
  code->Append(new Load(result, ref, offset));
  return result;
}

void CodeGenerator::GenStore(Location *dst,Location *src, int offset)
{
  code->Append(new Store(dst, src, offset));
}


Location *CodeGenerator::GenBinaryOp(const char *opName, Location *op1,
						     Location *op2)
{
  Location *result = GenTempVar();
  code->Append(new BinaryOp(BinaryOp::OpCodeForName(opName), result, op1, op2));
  return result;
}


void CodeGenerator::GenLabel(const char *label)
{
  code->Append(new Label(label));
}

void CodeGenerator::GenIfZ(Location *test, const char *label)
{
  code->Append(new IfZ(test, label));
  
}

void CodeGenerator::GenGoto(const char *label)
{
  code->Append(new Goto(label));
}

void CodeGenerator::GenReturn(Location *val)
{
  code->Append(new Return(val));
}


BeginFunc *CodeGenerator::GenBeginFunc(FnDecl *fn)
{
  BeginFunc *result = new BeginFunc;
  code->Append(insideFn = result);
  List<VarDecl*> *formals = fn->GetFormals();
  int start = OffsetToFirstParam;
  if (fn->IsMethodDecl()) start += VarSize;
  for (int i = 0; i < formals->NumElements(); i++)
    formals->Nth(i)->rtLoc = new Location(fpRelative, i*VarSize + start, formals->Nth(i)->GetName());
  curStackOffset = OffsetToFirstLocal;
  return result;
}

void CodeGenerator::GenEndFunc()
{
  code->Append(new EndFunc());
  insideFn->SetFrameSize(OffsetToFirstLocal-curStackOffset);
  insideFn = NULL;
}

void CodeGenerator::GenPushParam(Location *param)
{
  code->Append(new PushParam(param));
}

void CodeGenerator::GenPopParams(int numBytesOfParams)
{
  Assert(numBytesOfParams >= 0 && numBytesOfParams % VarSize == 0); // sanity check
  if (numBytesOfParams > 0)
    code->Append(new PopParams(numBytesOfParams));
}

Location *CodeGenerator::GenLCall(const char *label, bool fnHasReturnValue)
{
  Location *result = fnHasReturnValue ? GenTempVar() : NULL;
  code->Append(new LCall(label, result));
  return result;
}
  
Location *CodeGenerator::GenFunctionCall(const char *fnLabel, List<Location*> *args, bool hasReturnValue)
{
  for (int i = args->NumElements()-1; i >= 0; i--) // push params right to left
    GenPushParam(args->Nth(i));
  Location *result = GenLCall(fnLabel, hasReturnValue);
  GenPopParams(args->NumElements()*VarSize);
  return result;
}

Location *CodeGenerator::GenACall(Location *fnAddr, bool fnHasReturnValue)
{
  Location *result = fnHasReturnValue ? GenTempVar() : NULL;
  code->Append(new ACall(fnAddr, result));
  return result;
}
  
Location *CodeGenerator::GenMethodCall(Location *rcvr,
			     Location *meth, List<Location*> *args, bool fnHasReturnValue)
{
  for (int i = args->NumElements()-1; i >= 0; i--)
    GenPushParam(args->Nth(i));
  GenPushParam(rcvr);	// hidden "this" parameter
  Location *result= GenACall(meth, fnHasReturnValue);
  GenPopParams((args->NumElements()+1)*VarSize);
  return result;
}
 
 
static struct _builtin {
  const char *label;
  int numArgs;
  bool hasReturn;
} builtins[] =
 {{"_Alloc", 1, true},
  {"_ReadLine", 0, true},
  {"_ReadInteger", 0, true},
  {"_StringEqual", 2, true},
  {"_PrintInt", 1, false},
  {"_PrintString", 1, false},
  {"_PrintBool", 1, false},
  {"_Halt", 0, false}};

Location *CodeGenerator::GenBuiltInCall(BuiltIn bn,Location *arg1, Location *arg2)
{
  Assert(bn >= 0 && bn < NumBuiltIns);
  struct _builtin *b = &builtins[bn];
  Location *result = NULL;

  if (b->hasReturn) result = GenTempVar();
                // verify appropriate number of non-NULL arguments given
  Assert((b->numArgs == 0 && !arg1 && !arg2)
	|| (b->numArgs == 1 && arg1 && !arg2)
	|| (b->numArgs == 2 && arg1 && arg2));
  if (arg2) code->Append(new PushParam(arg2));
  if (arg1) code->Append(new PushParam(arg1));
  code->Append(new LCall(b->label, result));
  GenPopParams(VarSize*b->numArgs);
  return result;
}


void CodeGenerator::GenVTable(const char *className, List<const char *> *methodLabels)
{
  code->Append(new VTable(className, methodLabels));
}


void CodeGenerator::DoFinalCodeGen()
{

  // three things that can end a block
  string str_goto ("Goto");
  string str_return ("Return");
  string str_jmp ("IfZ");
  string str_end ("EndFunc");
  string str_call("LCall");
  
  // three things that can begin a block
  string str_begin ("BeginFunc");
  
  size_t found;
  
  bool first_time_thur = true;
  bool open = false;
  cfg_type cfg;
  
  std::list<Instruction*> current_block;
  std::list<int> current_in_edges;
  std::list<int> current_out_edges;
  
  int id = 0;
  
	// look for an entry point, target of a branch, directly after branch or return 
	for(int i = 0; i < code->NumElements(); i++)
	{
		Instruction *item = code->Nth(i);

		// look for a start 
		found = item->command().find(str_begin);
		if (!open && found!=string::npos)
		{
 			if (item != NULL)
			{
				current_block.push_back(item);
				open = true;
			}
		}
 
		// look for an end
		found = item->command().find(str_goto);	
		if(open && found!=string::npos)
		{ 		
			//std::cout << "goto";
			cfg.add_basic_block(id, current_block);
			
			// empty the list
			while(!current_block.empty())
			{
				current_block.pop_front();
			}
			
			open = false;
			id += 1;
			
		}
  	
		// look for an end
		found = item->command().find(str_jmp);	
		if(open && found!=string::npos)
		{		
			//std::cout << "jmp";
			cfg.add_basic_block(id, current_block);
			
			// empty the list 
			while(!current_block.empty())
			{
				current_block.pop_front();
			}
			
			open = false;
			id += 1;
		}
  	
		// look for an end
		found = item->command().find(str_return);	
		if(open && found!=string::npos)
		{	
			//std::cout << "return";
			cfg.add_basic_block(id, current_block);
			
			// empty the list
			while(!current_block.empty())
			{
				current_block.pop_front();
			}
			
			open = false;
			id += 1;
		}
		
		// look for an end
		found = item->command().find(str_end);	
		if(open && found!=string::npos)
		{ 		
			//std::cout << "end";
			cfg.add_basic_block(id, current_block);
			
			// empty the list
			while(!current_block.empty())
			{
				current_block.pop_front();
			}
	
			open = false;
			id += 1;
		}
		
		found = item->command().find(str_call);	
		if(open && found!=string::npos)
		{ 		
			//std::cout << "end";
			cfg.add_basic_block(id, current_block);
			
			// empty the list
			while(!current_block.empty())
			{
				current_block.pop_front();
			}
	
			open = false;
			id += 1;
		}
	
		if (open && item != NULL)
		{
			current_block.push_back(item);
		}	
	}
	
	//std::cout << id;
	
	//after forming the basic blocks it is time to determine the in-edges
	for(int p=0; p < id; p++)
	{
		
		// no in-edges if first time through
		if( first_time_thur )
		{
			cfg.add_in_edge(p, current_in_edges);
			first_time_thur = false;
		}
		
		else
		{
			current_in_edges.push_back(p);
		}
		
		cfg.add_in_edge(p, current_in_edges);
	}
	
	//after forming the basic blocks it is time to determine the out-edges
	for(int p=1; p < id+1; p++)
	{
		// no in-edges if first time through
		if( first_time_thur )
		{
			cfg.add_out_edge(p, current_out_edges);
			first_time_thur = false;
		}
		
		else
		{
			current_out_edges.push_back(p+1);
		}
		
		cfg.add_out_edge(p, current_out_edges);
	}
	
	for(int z=0; z < id; z++)
	{
		analysis.get_live_locations(cfg.basic_block[z], z);
	}

  	/*for (std::map<int, std::map<std::string, std::pair<int,int> > >::const_iterator iter = 
  	analysis.variable_timeline.begin(); iter != analysis.variable_timeline.end(); ++iter)
  	{
  		std::cout << iter->first << std::endl;
  		
  		for (std::map<std::string, std::pair<int,int> >::const_iterator it = 
  		iter->second.begin(); it != iter->second.end(); ++it)
		{
  			std::cout << it->first << "\n";
  		}
      	}*/
	
  /*if ( !(IsDebugOn("tac")) ) { // if debug don't translate to mips, just print Tac
    for (int i = 0; i < code->NumElements(); i++)
	code->Nth(i)->Print();
   }  else {*/
     Mips mips;
     mips.analysis = &analysis;
     mips.EmitPreamble();
     for (int i = 0; i < code->NumElements(); i++)
     {
     	printf("%i", i);
     	mips.line_num = &i;
	code->Nth(i)->Emit(&mips);
     }
   //}
}



Location *CodeGenerator::GenArrayLen(Location *array)
{
  return GenLoad(array, -4);
}

Location *CodeGenerator::GenNew(const char *vTableLabel, int instanceSize)
{
  Location *size = GenLoadConstant(instanceSize);
  Location *result = GenBuiltInCall(Alloc, size);
  Location *vt = GenLoadLabel(vTableLabel);
  GenStore(result, vt);
  return result;
}


Location *CodeGenerator::GenDynamicDispatch(Location *rcvr, int vtableOffset, List<Location*> *args, bool hasReturnValue)
{
  Location *vptr = GenLoad(rcvr); // load vptr
  Assert(vtableOffset >= 0);
  Location *m = GenLoad(vptr, vtableOffset*4);
  return GenMethodCall(rcvr, m, args, hasReturnValue);
}

// all variables (ints, bools, ptrs, arrays) are 4 bytes in for code generation
// so this simplifies the math for offsets
Location *CodeGenerator::GenSubscript(Location *array, Location *index)
{
  Location *zero = GenLoadConstant(0);
  Location *isNegative = GenBinaryOp("<", index, zero);
  Location *count = GenLoad(array, -4);
  Location *isWithinRange = GenBinaryOp("<", index, count);
  Location *pastEnd = GenBinaryOp("==", isWithinRange, zero);
  Location *outOfRange = GenBinaryOp("||", isNegative, pastEnd);
  const char *pastError = NewLabel();
  GenIfZ(outOfRange, pastError);
  GenHaltWithMessage(err_arr_out_of_bounds);
  GenLabel(pastError);
  Location *four = GenLoadConstant(VarSize);
  Location *offset = GenBinaryOp("*", four, index);
  Location *elem = GenBinaryOp("+", array, offset);
  return new Location(elem, 0); 
}



Location *CodeGenerator::GenNewArray(Location *numElems)
{
  Location *zero = GenLoadConstant(0);
  Location *isNegative = GenBinaryOp("<", numElems, zero);
  const char *pastError = NewLabel();
  GenIfZ(isNegative, pastError);
  GenHaltWithMessage(err_arr_bad_size);
  GenLabel(pastError);

  // need (numElems+1)*VarSize total bytes (extra 1 is for length)
  Location *arraySize = GenLoadConstant(1);
  Location *num = GenBinaryOp("+", arraySize, numElems);
  Location *four = GenLoadConstant(VarSize);
  Location *bytes = GenBinaryOp("*", num, four);
  Location *result = GenBuiltInCall(Alloc, bytes);
  GenStore(result, numElems);
  return GenBinaryOp("+", result, four);
}


void CodeGenerator::GenHaltWithMessage(const char *message)
{
   Location *msg = GenLoadConstant(message);
   GenBuiltInCall(PrintString, msg);
   GenBuiltInCall(Halt, NULL);
}
   

 
