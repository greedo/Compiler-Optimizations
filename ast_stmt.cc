/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "scope.h"
#include "errors.h"
#include "codegen.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::Check() {
    nodeScope = new Scope();
    decls->DeclareAll(nodeScope);
    decls->CheckAll();
}
void Program::Emit() {
    bool found = false;
    for (int i=0; i < decls->NumElements(); i++) {
	Decl *d = decls->Nth(i);
	if (!strcmp(d->GetName(), "main") && d->IsFnDecl()) {
	  found = true;
	  break;
	}
    }
    if (!found) {
	ReportError::NoMainFound();
	return;
    }
    CodeGenerator *cg = new CodeGenerator();
    decls->EmitAll(cg);
    if (ReportError::NumErrors() == 0)
        cg->DoFinalCodeGen();
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}
void StmtBlock::Check() {
    nodeScope = new Scope();
    decls->DeclareAll(nodeScope);
    decls->CheckAll();
    stmts->CheckAll();
}
void StmtBlock::Emit(CodeGenerator *cg) {
    decls->EmitAll(cg);
    stmts->EmitAll(cg);
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

void ConditionalStmt::Check() {
    if (!test->CheckAndComputeResultType()->IsCompatibleWith(Type::boolType))
	ReportError::TestNotBoolean(test);
    body->Check();
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}
void ForStmt::Emit(CodeGenerator *cg) {
    init->Emit(cg);
    char *topLoop = cg->NewLabel();
    afterLoopLabel = cg->NewLabel();
    cg->GenLabel(topLoop);
    test->Emit(cg);
    cg->GenIfZ(test->result, afterLoopLabel);
    body->Emit(cg);
    step->Emit(cg);
    cg->GenGoto(topLoop);
    cg->GenLabel(afterLoopLabel);
}
void WhileStmt::Emit(CodeGenerator *cg) {
    char *topLoop = cg->NewLabel();
    afterLoopLabel = cg->NewLabel();
    cg->GenLabel(topLoop);
    test->Emit(cg);
    cg->GenIfZ(test->result, afterLoopLabel);
    body->Emit(cg);
    cg->GenGoto(topLoop);
    cg->GenLabel(afterLoopLabel);
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}
void IfStmt::Check() {
    ConditionalStmt::Check();
    if (elseBody) elseBody->Check();
}
void IfStmt::Emit(CodeGenerator *cg) {
    test->Emit(cg);
    char *afterElse, *elseL = cg->NewLabel();
    cg->GenIfZ(test->result, elseL);
    body->Emit(cg);
    if (elseBody) {
	afterElse = cg->NewLabel();
	cg->GenGoto(afterElse);
    }
    cg->GenLabel(elseL);
    if (elseBody) {
	elseBody->Emit(cg);
	cg->GenLabel(afterElse);
    }
}


void BreakStmt::Check() {
    if (!FindSpecificParent<LoopStmt>())
        ReportError::BreakOutsideLoop(this);
}
void BreakStmt::Emit(CodeGenerator *cg) {
    LoopStmt *enclosingLoop = FindSpecificParent<LoopStmt>();
    cg->GenGoto(enclosingLoop->GetLoopExitLabel());
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}
void ReturnStmt::Check() {
    Type *got = expr->CheckAndComputeResultType();
    Type *expected =  FindSpecificParent<FnDecl>()->GetReturnType();
    if (!got->IsCompatibleWith(expected))
	ReportError::ReturnMismatch(this, got, expected);
}
void ReturnStmt::Emit(CodeGenerator *cg) {
    expr->Emit(cg);
    cg->GenReturn(expr->result);
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}
void PrintStmt::Check() {
    for (int i = 0; i < args->NumElements();i++) {
	Type *t = args->Nth(i)->CheckAndComputeResultType();
	if (t->IsEquivalentTo(Type::errorType)) continue;
	if (!(t->IsEquivalentTo(Type::intType) || t->IsEquivalentTo(Type::stringType) || t->IsEquivalentTo(Type::boolType)))
	  ReportError::PrintArgMismatch(args->Nth(i),i + 1, t);
    }
}
void PrintStmt::Emit(CodeGenerator *cg) {
    for (int i = 0; i < args->NumElements(); i++) {
        Expr *arg = args->Nth(i);
        Type *argType = arg->CheckAndComputeResultType();
	  arg->Emit(cg);
        BuiltIn b = PrintInt;
        if (argType->IsEquivalentTo(Type::stringType))
            b = PrintString;
        else if (argType->IsEquivalentTo(Type::boolType))
            b = PrintBool;
        cg->GenBuiltInCall(b, arg->result);
  }
}


