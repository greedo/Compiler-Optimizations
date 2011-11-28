/* File: mips.h
 * ------------
 * The Mips class defines an object capable of emitting MIPS
 * instructions and managing the allocation and use of registers.
 * It is used by the Tac instruction classes to convert each
 * instruction to the appropriate MIPS equivalent.
 *
 * You can scan this code to see it works and get a sense of what
 * it does.  You will not need to modify this class unless
 * you're attempting some machine-specific optimizations. 
 *
 * It comments the emitted assembly but the commenting for the code
 * in the class itself is pretty sparse. The SPIM manual (see link
 * from other materials on our web site) has more detailed documentation
 * on the MIPS architecture, instruction set, calling conventions, etc.
 */

#ifndef _H_mips
#define _H_mips

#include "tac.h"
#include "list.h"
#include "df_live_var.h"

class Location;
class df_live_var;

class Mips {
  private:
    typedef enum {zero, at, v0, v1, a0, a1, a2, a3,
			t0, t1, t2, t3, t4, t5, t6, t7,
			s0, s1, s2, s3, s4, s5, s6, s7,
			t8, t9, k0, k1, gp, sp, fp, ra, NumRegs } Register;

    struct RegContents {
	bool isDirty;
	Location *var;
	const char *name;
	bool isGeneralPurpose;
    } regs[NumRegs];

    Register rs, rt, rd;

    typedef enum { ForRead, ForWrite } Reason;
    
    void FillRegister(Location *src, Register reg);
    void SpillRegister(Location *dst, Register reg);

    void EmitCallInstr(Location *dst, const char *fn, bool isL);
    
    static const char *mipsName[BinaryOp::NumOps];
    static const char *NameForTac(BinaryOp::OpCode code);

 public:
    
    Mips();
    
    df_live_var *analysis;
    
    int *line_num;
    
    std::map<std::string,int> var_to_reg;

    static void Emit(const char *fmt, ...);
    
    void EmitLoadConstant(Location *dst, int val);
    void EmitLoadStringConstant(Location *dst, const char *str);
    void EmitLoadLabel(Location *dst, const char *label);

    void EmitLoad(Location *dst, Location *reference, int offset);
    void EmitStore(Location *reference, Location *value, int offset);
    void EmitCopy(Location *dst, Location *src);

    void EmitBinaryOp(BinaryOp::OpCode code, Location *dst, 
			    Location *op1, Location *op2);

    void EmitLabel(const char *label);
    void EmitGoto(const char *label);
    void EmitIfZ(Location *test, const char*label);
    void EmitReturn(Location *returnVal);

    void EmitBeginFunction(int frameSize);
    void EmitEndFunction();

    void EmitParam(Location *arg);
    void EmitLCall(Location *result, const char* label);
    void EmitACall(Location *result, Location *fnAddr);
    void EmitPopParams(int bytes);

    void EmitVTable(const char *label, List<const char*> *methodLabels);

    void EmitPreamble();
    
    void add_register_assignment();
    
    void delete_register_assignment();
    
    void map_var_to_reg(Location* loc,int r);
    
    std::string select_target_register(int line_num);
    
};

#endif
