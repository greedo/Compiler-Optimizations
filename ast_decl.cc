/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "scope.h"
#include "errors.h"
#include "scanner.h" // for MaxIdentLen
#include "codegen.h"
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
  
    offset = -555;
}

bool Decl::ConflictsWithPrevious(Decl *prev) {
    ReportError::DeclConflict(this, prev);
    return true;
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
  
void VarDecl::Check() { type->Check(); if (type->IsError()) type = Type::errorType; }
bool VarDecl::IsIvarDecl() { return dynamic_cast<ClassDecl*>(parent) != NULL;}
void VarDecl::Emit(CodeGenerator *cg) { 
    if (dynamic_cast<Program*>(parent)) {
        rtLoc = cg->GenGlobalVariable(GetName());
    } else if (!dynamic_cast<ClassDecl*>(parent)) {
        rtLoc = cg->GenLocalVariable(GetName());
    } 
}

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
    cType = new NamedType(n);
    cType->SetParent(this);
    convImp = NULL;
    vtable = new List<const char*>;
    nextIvarOffset = 4;
}

void ClassDecl::Check() {
   if (extends) {
        ClassDecl *pd = dynamic_cast<ClassDecl*>(parent->FindDecl(extends->GetId()));
        List<Decl*> *pm = pd->members;
        for (int i = 0; i < pm->NumElements(); i++)
	    ;//AddField(pm->Nth(i), NULL);
    }

    if (extends && !extends->IsClass()) {
        ReportError::IdentifierNotDeclared(extends->GetId(), LookingForClass);
        extends = NULL;
    }
    for (int i = 0; i < implements->NumElements(); i++) {
        NamedType *in = implements->Nth(i);
        if (!in->IsInterface()) {
            ReportError::IdentifierNotDeclared(in->GetId(), LookingForInterface);
            implements->RemoveAt(i--);
        }
    }
    PrepareScope();
    members->CheckAll();
    for (int i = 0; i < convImp->NumElements(); i++) {
        if (!convImp->Nth(i)->ClassMeetsObligation(nodeScope))
            ReportError::InterfaceNotImplemented(this, implements->Nth(i));
    }
}

// This is not done very cleanly. I should sit down and sort this out. Right now
// I was using the copy-in strategy from the old compiler, but I think the link to
// parent may be the better way now.
Scope *ClassDecl::PrepareScope()
{
    if (nodeScope) return nodeScope;
    nodeScope = new Scope();  
    if (extends) {
        ClassDecl *ext = dynamic_cast<ClassDecl*>(parent->FindDecl(extends->GetId())); 
        if (ext) nodeScope->CopyFromScope(ext->PrepareScope(), this);
    }
    convImp = new List<InterfaceDecl*>;
    for (int i = 0; i < implements->NumElements(); i++) {
        NamedType *in = implements->Nth(i);
        InterfaceDecl *id = dynamic_cast<InterfaceDecl*>(in->FindDecl(in->GetId()));
        if (id) {
		nodeScope->CopyFromScope(id->PrepareScope(), NULL);
            convImp->Append(id);
	  }
    }
    for (int i = 0; i < members->NumElements(); i++) {
        AddField(members->Nth(i));
    }
    members->DeclareAll(nodeScope);
    return nodeScope;
}


bool ClassDecl::IsCompatibleWith(Type *other) {
    if (Implements(other)) return true;
    if (cType->IsEquivalentTo(other)) return true;
    return (extends && extends->IsCompatibleWith(other));
}

bool ClassDecl::Implements(Type *other) {
    if (!other->IsNamedType()) return false;
    Decl *toMatch = dynamic_cast<NamedType*>(other)->GetDeclForType();
    if (!convImp) PrepareScope(); //jdz hack
    for (int i = 0; i < convImp->NumElements(); i++) {
        InterfaceDecl *id = convImp->Nth(i);
        if (id == toMatch) return true;
    }
    return false;
}
void ClassDecl::Emit(CodeGenerator *cg) {
    members->EmitAll(cg);
    cg->GenVTable(GetName(), vtable);
}

void ClassDecl::AddField(Decl *decl) {
    Decl *prev = nodeScope->Lookup(decl->GetId());
    if (decl->IsVarDecl())
        AddIvar(dynamic_cast<VarDecl*>(decl), prev);
    else
        AddMethod(dynamic_cast<FnDecl*>(decl), prev);
}

  // used when a new ivar/method is declared in a class to
  // assign decl offset field
void ClassDecl::AddIvar(VarDecl *decl, Decl *prev) {
    decl->SetOffset(nextIvarOffset);
    nextIvarOffset += 4;  // all variables are 4 bytes for code gen
}

void ClassDecl::AddMethod(FnDecl *decl, Decl *inherited) {
    if (inherited) {
        int methodOffset = inherited->GetOffset();
        decl->SetOffset(methodOffset);
	  if (vtable->NumElements() <= methodOffset) {
	    while(vtable->NumElements() < methodOffset)
		vtable->Append(NULL);
	    vtable->Append(decl->GetFunctionLabel());	    
	  } else {
	    vtable->RemoveAt(methodOffset);
	    vtable->InsertAt(decl->GetFunctionLabel(), methodOffset);
	  }
    } else {
        decl->SetOffset(vtable->NumElements());
        vtable->Append(decl->GetFunctionLabel());
    }
}


InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
}

void InterfaceDecl::Check() {
    PrepareScope();
    members->CheckAll();
}
  
Scope *InterfaceDecl::PrepareScope() {
    if (nodeScope) return nodeScope;
    nodeScope = new Scope();  
    members->DeclareAll(nodeScope);
    return nodeScope;
}
bool InterfaceDecl::ClassMeetsObligation(Scope *s) {
    for (int i = 0; i < members->NumElements();i++) {
        FnDecl *m = dynamic_cast<FnDecl*>(members->Nth(i));
        FnDecl *found = dynamic_cast<FnDecl*>(s->Lookup(m->GetId()));
        if (!found || !found->MatchesPrototype(m) || found->GetParent() == this)
            return false;
    }
    return true;
}
	
FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::Check() {
    returnType->Check();
    if (body) {
        nodeScope = new Scope();
        formals->DeclareAll(nodeScope);
        formals->CheckAll();
	body->Check();
    }
}

bool FnDecl::ConflictsWithPrevious(Decl *prev) {
 // special case error for method override
    if (IsMethodDecl() && prev->IsMethodDecl() && parent != prev->GetParent()) { 
        if (!MatchesPrototype(dynamic_cast<FnDecl*>(prev))) {
            ReportError::OverrideMismatch(this);
            return true;
        }
        return false;
    }
    ReportError::DeclConflict(this, prev);
    return true;
}

bool FnDecl::IsMethodDecl() 
  { return dynamic_cast<ClassDecl*>(parent) != NULL || dynamic_cast<InterfaceDecl*>(parent) != NULL; }

bool FnDecl::MatchesPrototype(FnDecl *other) {
    if (!returnType->IsEquivalentTo(other->returnType)) return false;
    if (formals->NumElements() != other->formals->NumElements())
        return false;
    for (int i = 0; i < formals->NumElements(); i++)
        if (!formals->Nth(i)->GetDeclaredType()->IsEquivalentTo(other->formals->Nth(i)->GetDeclaredType()))
            return false;
    return true;
}

void FnDecl::Emit(CodeGenerator *cg) {
    if (body) {
        cg->GenLabel(GetFunctionLabel());
        cg->GenBeginFunc(this);
        body->Emit(cg);
        cg->GenEndFunc();
        
    }
}

/* This synthesizes the appropriate label for function in such a way to
 * identify uniquely and without conflicts.  For global funtions, the label
 * is the function name itself.  For methods, the label is the name prefixed by
 * the class name followed by a dot.  
 */
const char *FnDecl::GetFunctionLabel()
{
    ClassDecl *cd;
    if ((cd = dynamic_cast<ClassDecl*>(parent)) != NULL) { // if parent is a class, this is is a method
        char buffer[MaxIdentLen*2+4];
        sprintf(buffer, "_%s.%s", cd->GetName(), id->GetName());
        return strdup(buffer);
    } else if (strcmp(id->GetName(), "main")) {
	 char buffer[strlen(id->GetName())+2];
	 sprintf(buffer, "_%s", id->GetName());
       return strdup(buffer);
    } else
	return id->GetName();
}


