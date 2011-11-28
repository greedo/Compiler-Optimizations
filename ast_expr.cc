/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "errors.h"
#include "codegen.h"

Type *EmptyExpr::CheckAndComputeResultType() { return Type::voidType; } 

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
Type *IntConstant::CheckAndComputeResultType() { 
    return Type::intType;
}
void IntConstant::Emit(CodeGenerator *cg) { 
    result = cg->GenLoadConstant(value);
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
Type *DoubleConstant::CheckAndComputeResultType() { 
    return Type::doubleType;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
Type *BoolConstant::CheckAndComputeResultType() { 
    return Type::boolType;
}
void BoolConstant::Emit(CodeGenerator *cg) { 
    result = cg->GenLoadConstant(value);
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}
Type *StringConstant::CheckAndComputeResultType() {
    return Type::stringType;
}
void StringConstant::Emit(CodeGenerator *cg) { 
    result = cg->GenLoadConstant(value);
}
Type *NullConstant::CheckAndComputeResultType() { 
    return Type::nullType;
}
void NullConstant::Emit(CodeGenerator *cg) { 
    result = cg->GenLoadConstant(0);
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}
CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}
void CompoundExpr::ReportErrorForIncompatibleOperands(Type *lhs, Type *rhs) {
    if (!lhs) { //unary op
        ReportError::IncompatibleOperand(op, rhs);
    } else { // binary op
        ReportError::IncompatibleOperands(op, lhs, rhs);
    }
}

bool CompoundExpr::EitherOperandIsError(Type *lhs, Type *rhs) {
    return (lhs && lhs == Type::errorType) || rhs == Type::errorType;
}


void CompoundExpr::Emit(CodeGenerator *cg) {
    Assert(left);
    left->Emit(cg);
    right->Emit(cg);
    result = cg->GenBinaryOp(op->str(), left->result, right->result);
}

Type *GetResultType(Type *lhs, Type *rhs) {
    Type *lesser = rhs;
    if (lhs) lesser = lesser->LesserType(lhs);
    if (!lesser || !lesser->IsNumeric())
	return Type::errorType;
    return lesser;
}

Type*ArithmeticExpr::CheckAndComputeResultType() {
    Type *lType = left?left->CheckAndComputeResultType():NULL, *rType = right->CheckAndComputeResultType();
    if (EitherOperandIsError(lType,rType)) return Type::errorType;
    if (GetResultType(lType, rType) == Type::errorType)
	ReportErrorForIncompatibleOperands(lType, rType);
    return GetResultType(lType, rType);
}
void ArithmeticExpr::Emit(CodeGenerator *cg) {
    if (left)
        CompoundExpr::Emit(cg);
    else {
        right->Emit(cg);
        Location *zero = cg->GenLoadConstant(0);
        result = cg->GenBinaryOp("-", zero, right->result);
    }
}

Type* RelationalExpr::CheckAndComputeResultType() {
   Type*lhs = left->CheckAndComputeResultType(), *rhs = right->CheckAndComputeResultType();
   if (EitherOperandIsError(lhs,rhs)) return Type::boolType;
    if (!lhs->IsEquivalentTo(rhs) || !lhs->IsNumeric())
	ReportErrorForIncompatibleOperands(lhs, rhs);
    return Type::boolType;
}
void RelationalExpr::Emit(CodeGenerator *cg) {
    left->Emit(cg);
    right->Emit(cg);
    if (!strcmp(op->str(), "<")) {
        result = cg->GenBinaryOp("<", left->result, right->result);
    } else if (!strcmp(op->str(), ">")) {
        result = cg->GenBinaryOp("<", right->result, left->result);
    } else if (!strcmp(op->str(), "<=")) {
        Location *less = cg->GenBinaryOp("<", left->result, right->result);
        Location *eq  = cg->GenBinaryOp("==", left->result, right->result);
        result =  cg->GenBinaryOp("||", less, eq);
    } else if (!strcmp(op->str(), ">=")) {
        Location *less = cg->GenBinaryOp("<", right->result, left->result);
        Location *eq  = cg->GenBinaryOp("==", right->result, left->result);
        result =  cg->GenBinaryOp("||", less, eq);
    }
}

Type* EqualityExpr::CheckAndComputeResultType() {
   Type*lhs = left->CheckAndComputeResultType(), *rhs = right->CheckAndComputeResultType();
    if (!lhs->IsCompatibleWith(rhs) && !rhs->IsCompatibleWith(lhs))
	ReportErrorForIncompatibleOperands(lhs, rhs);
    return Type::boolType;
}
void EqualityExpr::Emit(CodeGenerator *cg) {
    left->Emit(cg);
    right->Emit(cg);
    if (left->CheckAndComputeResultType() == Type::stringType) 
        result = cg->GenBuiltInCall(StringEqual, left->result, right->result);
    else
        result = cg->GenBinaryOp("==", left->result, right->result);
    if (!strcmp(op->str(), "!=")) {
        Location *zero = cg->GenLoadConstant(0);
        result = cg->GenBinaryOp("==", result, zero);
    }
}

Type* LogicalExpr::CheckAndComputeResultType() {
    Type *lhs = left ?left->CheckAndComputeResultType() :NULL, *rhs = right->CheckAndComputeResultType();
    if ((lhs && !lhs->IsCompatibleWith(Type::boolType)) ||
	  (!rhs->IsCompatibleWith(Type::boolType)))
	ReportErrorForIncompatibleOperands(lhs, rhs);
    return Type::boolType;
}
void LogicalExpr::Emit(CodeGenerator *cg) {
    if (left)
	CompoundExpr::Emit(cg);
    else {
	right->Emit(cg);
	Location *zero = cg->GenLoadConstant(0);
	result = cg->GenBinaryOp("==", right->result, zero);
    }
}

Type * AssignExpr::CheckAndComputeResultType() {
    Type *lhs = left->CheckAndComputeResultType(), *rhs = right->CheckAndComputeResultType();
    if (!rhs->IsCompatibleWith(lhs)) {
        ReportErrorForIncompatibleOperands(lhs, rhs);
        return Type::errorType;
    }
    return lhs;
}
  void AssignExpr::Emit(CodeGenerator *cg) {
    dynamic_cast<LValue *>(left)->EmitWithoutDereference(cg); //sad, but if want to be compound....
    right->Emit(cg);
    if (left->result->IsReference()) {
        cg->GenStore(left->result, right->result, left->result->GetRefOffset());
    } else
        cg->GenAssign(left->result, right->result);
    result = left->result;
  }
  void LValue::Emit(CodeGenerator *cg)
  {
    EmitWithoutDereference(cg);
    if (result->IsReference()) 
	result = cg->GenLoad(result, result->GetRefOffset());
  }
Type* This::CheckAndComputeResultType() {
    if (!enclosingClass) enclosingClass = FindSpecificParent<ClassDecl>();
   if (!enclosingClass)  
       ReportError::ThisOutsideClassScope(this);
   if (!enclosingClass) return Type::errorType;
   return enclosingClass->GetDeclaredType();
 }

 void This::Emit(CodeGenerator *cg) {
   if (!result)
    result = new Location(fpRelative, 4, "this"); 
 }
 
   
  
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}
Type *ArrayAccess::CheckAndComputeResultType() {
    Type *baseT = base->CheckAndComputeResultType();
    if ((baseT != Type::errorType) && !baseT->IsArrayType()) 
        ReportError::BracketsOnNonArray(base);
    if (!subscript->CheckAndComputeResultType()->IsCompatibleWith(Type::intType))
	ReportError::SubscriptNotInteger(subscript);
    return baseT->IsArrayType() ? dynamic_cast<ArrayType*>(baseT)->GetArrayElemType() : Type::errorType;
}

void ArrayAccess::EmitWithoutDereference(CodeGenerator *cg)  {
    base->Emit(cg);
    subscript->Emit(cg);
    result = cg->GenSubscript(base->result, subscript->result);
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}


Type* FieldAccess::CheckAndComputeResultType() {
    Type *baseType = base ? base->CheckAndComputeResultType() : NULL;
    Decl *ivar = field->GetDeclRelativeToBase(baseType);
    if (ivar && ivar->IsIvarDecl() && !base) { // add implicit "this"
        base = new This(*field->GetLocation());
        base->SetParent(this);
        baseType = base->CheckAndComputeResultType();
    }
    if (base) {
        if (baseType == Type::errorType)
            return Type::errorType;
        else if (!ivar || !ivar->IsVarDecl()) {
            ReportError::FieldNotFoundInBase(field, baseType);
            return Type::errorType;
        } else {
            ClassDecl *enclosingClass = FindSpecificParent<ClassDecl>(); // check cur scope for compatibility
            Type *withinClass = (enclosingClass? enclosingClass->GetDeclaredType() : NULL);
            if (ivar && (!withinClass|| !withinClass->IsCompatibleWith(baseType))) {
                ReportError::InaccessibleField(field, baseType);
                return Type::errorType;
            } 
        }
    } else if (!ivar || !ivar->IsVarDecl()) {
        ReportError::IdentifierNotDeclared(field, LookingForVariable);
        return Type::errorType;
    }
    return ivar ? (dynamic_cast<VarDecl *>(ivar))->GetDeclaredType() : Type::errorType;
  }

void FieldAccess::EmitWithoutDereference(CodeGenerator *cg) {
    CheckAndComputeResultType(); // need to ensure check called to get base set
    Decl *fd = field->GetDeclRelativeToBase(base ? base->CheckAndComputeResultType() : NULL);
    if (base) {
        base->Emit(cg);
        result = new Location(base->result, fd->GetOffset());
    } else
        result = dynamic_cast<VarDecl*>(fd)->rtLoc;
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}
// special-case code for length() on arrays... sigh.
Type* Call::CheckAndComputeResultType() {
    Type *baseType = base ? base->CheckAndComputeResultType() : NULL;
    FnDecl *fd = dynamic_cast<FnDecl *>(field->GetDeclRelativeToBase(baseType));
    if (fd && fd->IsMethodDecl() && !base) { // add implicit "this"
        base = new This(*field->GetLocation());
        base->SetParent(this);
        baseType = base->CheckAndComputeResultType();
   }
   List<Type*> aTypes;
    for (int i = 0; i < actuals->NumElements(); i++) 
        aTypes.Append(actuals->Nth(i)->CheckAndComputeResultType());
// jdz cascade, above loop checks actuals before function confirmed.
// what about excess actuals? what if function doesn't exist at all?
    if (baseType && baseType->IsArrayType() && strcmp(field->GetName(), "length") == 0) {
	if (actuals->NumElements() != 0) 
            ReportError::NumArgsMismatch(field, 0, actuals->NumElements());
	return Type::intType;
    }
    if (baseType == Type::errorType) {
	return Type::errorType;
    }
    if (baseType && !fd) { // had receiver, but no field in receiver (not class, wrong name, etc.)
	ReportError::FieldNotFoundInBase(field, baseType);
        return Type::errorType;
    } else if (!fd) { // no base, bad function
	ReportError::IdentifierNotDeclared(field, LookingForFunction);
        return Type::errorType;
    }  

    List<VarDecl*> *formals = fd->GetFormals();
    if (formals->NumElements() != actuals->NumElements()) {
	ReportError::NumArgsMismatch(field, formals->NumElements(), actuals->NumElements());
    }
    for (int i = 0; i < formals->NumElements(); i++) {
	if (i >= actuals->NumElements()) break;
        Type *at = aTypes.Nth(i);
        if (!at->IsCompatibleWith(formals->Nth(i)->GetDeclaredType()))
            ReportError::ArgMismatch(actuals->Nth(i), i+1, at,
                                    formals->Nth(i)->GetDeclaredType());
    }
    return fd->GetReturnType();
}
void Call::Emit(CodeGenerator *cg)
{
    Type *baseType = base ? base->CheckAndComputeResultType() :NULL;
    if (baseType && baseType->IsArrayType()) { // assume length() (i.e. semantically correct)
	base->Emit(cg);
	result = cg->GenArrayLen(base->result);
	return;
    }
    List<Location*> l;	// this is not convenient...
    for (int i = 0; i < actuals->NumElements(); i++) {
	actuals->Nth(i)->Emit(cg);
	l.Append(actuals->Nth(i)->result);
    }
    Type *resultType = CheckAndComputeResultType(); // force base to get set
    FnDecl *func = dynamic_cast<FnDecl *>(field->GetDeclRelativeToBase(baseType));
    if (base) {
        base->Emit(cg);
        result = cg->GenDynamicDispatch(base->result, func->GetOffset(), &l, !resultType->IsEquivalentTo(Type::voidType));
    } else {
        result = cg->GenFunctionCall(func->GetFunctionLabel(), &l, !resultType->IsEquivalentTo(Type::voidType));
    }
}

 

NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}

Type* NewExpr::CheckAndComputeResultType() {
    if (!cType->IsClass()) {
        ReportError::IdentifierNotDeclared(cType->GetId(), LookingForClass);
        return Type::errorType;
    }
    return cType; 
}
void NewExpr::Emit(CodeGenerator *cg) { 
    ClassDecl *cd = dynamic_cast<ClassDecl*>(cType->GetDeclForType());
    result = cg->GenNew(cd->GetClassName(), cd->GetClassSize()); 
}

NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}
Type *NewArrayExpr::CheckAndComputeResultType() {
    Type *st = size->CheckAndComputeResultType();
    if (!st->IsCompatibleWith(Type::intType))
	ReportError::NewArraySizeNotInteger(size);
    elemType->Check();
    yyltype none;
    return new ArrayType(none, elemType);
}
  
void NewArrayExpr::Emit(CodeGenerator *cg) {
    size->Emit(cg);
    result = cg->GenNewArray(size->GetResult());
}

Type *ReadIntegerExpr::CheckAndComputeResultType() { return Type::intType; }
Type *ReadLineExpr::CheckAndComputeResultType() { return Type::stringType; }

void ReadIntegerExpr::Emit(CodeGenerator *cg) {
    result = cg->GenBuiltInCall(ReadInteger);
}

void ReadLineExpr::Emit(CodeGenerator *cg) {
    result = cg->GenBuiltInCall(ReadLine);
}

       
