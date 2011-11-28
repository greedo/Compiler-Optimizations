/* File: ast_type.cc
 * -----------------
 * Implementation of type node classes.
 */
#include "ast_type.h"
#include "ast_decl.h"
#include "errors.h"
#include "codegen.h"
 
/* Class constants
 * ---------------
 * These are public constants for the built-in base types (int, double, etc.)
 * They can be accessed with the syntax Type::intType. This allows you to
 * directly access them and share the built-in types where needed rather that
 * creates lots of copies.
 */

Type *Type::intType    = new Type("int");
Type *Type::doubleType = new Type("double");
Type *Type::voidType   = new Type("void");
Type *Type::boolType   = new Type("bool");
Type *Type::nullType   = new Type("null");
Type *Type::stringType = new Type("string");
Type *Type::errorType  = new Type("error"); 

Type::Type(const char *n) {
    Assert(n);
    typeName = strdup(n);
}

 Type *Type::LesserType(Type *other) {
    if (this == Type::errorType || other == Type::errorType)
        return Type::errorType;
    if (other == NULL)
        return this;
    if (IsCompatibleWith(other)) return other;
    if (other->IsCompatibleWith(this)) return this;
    return NULL;
}


bool Type::IsCompatibleWith(Type *other) {
    if (this == errorType || other == errorType) return true;
    if ((this == nullType) && (other->IsNamedType()))
        return true;
    return IsEquivalentTo(other);
}
	
NamedType::NamedType(Identifier *i) : Type(*i->GetLocation()) {
    Assert(i != NULL);
    (id=i)->SetParent(this);
    cachedDecl = NULL;
    isError = false;
} 

void NamedType::Check() {
    if (!GetDeclForType()) {
        isError = true;
        ReportError::IdentifierNotDeclared(id, LookingForType);
    }
}
Decl *NamedType::GetDeclForType() {
    if (!cachedDecl && !isError) {
        Decl *declForName = FindDecl(id);
        if (declForName && (declForName->IsClassDecl() || declForName->IsInterfaceDecl())) 
            cachedDecl = declForName;
    }
    return cachedDecl;
}

bool NamedType::IsInterface() {
    Decl *d = GetDeclForType();
    return (d && d->IsInterfaceDecl());
}

bool NamedType::IsClass() {
    Decl *d = GetDeclForType();
    return (d && d->IsClassDecl());
}

bool NamedType::IsEquivalentTo(Type *other) {
    NamedType *ot = dynamic_cast<NamedType*>(other);
    return ot && strcmp(id->GetName(), ot->id->GetName()) == 0;
}
bool NamedType::IsCompatibleWith(Type *other) {
    if (IsEquivalentTo(other)) return true;
    if (other == errorType || isError) return true; 
    NamedType *ot = dynamic_cast<NamedType*>(other);
    if (!ot) return false;
    if (ot->isError) return true;
    ClassDecl *cd = dynamic_cast<ClassDecl*>(GetDeclForType());
    return cd && cd->IsCompatibleWith(other);
}


ArrayType::ArrayType(yyltype loc, Type *et) : Type(loc) {
    Assert(et != NULL);
    (elemType=et)->SetParent(this);
}

void ArrayType::Check() {
    elemType->Check();
}

bool ArrayType::IsEquivalentTo(Type *other) {
    ArrayType *o = dynamic_cast<ArrayType*>(other);
    return (o && elemType->IsEquivalentTo(o->elemType));
}

