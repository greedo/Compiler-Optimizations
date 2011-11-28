/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"

class Type;
class NamedType;
class Identifier;
class Stmt;
class FnDecl;
class InterfaceDecl;
#include "ast_stmt.h"
class Location;

class Decl : public Node 
{
  protected:
    Identifier *id;
  
  public:
    Decl(Identifier *name);
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
    Identifier *GetId() { return id; }
    const char *GetName() { return id->GetName(); }
    
    virtual bool ConflictsWithPrevious(Decl *prev);

    virtual bool IsVarDecl() { return false; } // jdz: could use typeid/dynamic_cast for these
    virtual bool IsClassDecl() { return false; }
    virtual bool IsInterfaceDecl() { return false; }
    virtual bool IsFnDecl() { return false; } 
    virtual bool IsMethodDecl() { return false; }
    virtual bool IsIvarDecl() { return false; }
  int offset;// used for methods and ivars only
    void SetOffset(int off) { offset = off; }
    int GetOffset() { return offset; }
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    
  public:
    VarDecl(Identifier *name, Type *type);
    void Check();
    Type *GetDeclaredType() { return type; }
    bool IsVarDecl() { return true; }
    bool IsIvarDecl();
    Location *rtLoc;
    virtual bool IsReference() { return false; }
    void Emit(CodeGenerator *cg);
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;
    Type *cType;
    List<InterfaceDecl*> *convImp;
    List<const char*> *vtable;
    int nextIvarOffset;

  public:
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
    void Check();
    bool IsClassDecl() { return true; }
    Scope *PrepareScope();
    bool IsCompatibleWith(Type *type);
    bool Implements(Type *intf);
    Type *GetDeclaredType() { return cType; } //  used by "this"
    const char *GetClassName() { return id->GetName(); }
    void Emit(CodeGenerator *cg);
    void AddMethod(FnDecl*d, Decl *p);
    void AddIvar(VarDecl*d, Decl *p);
    void AddField(Decl*d);
    int GetClassSize() { return nextIvarOffset; }
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    InterfaceDecl(Identifier *name, List<Decl*> *members);
    void Check();
    bool IsInterfaceDecl() { return true; }
    Scope *PrepareScope();
    bool ClassMeetsObligation(Scope *s);
};

class FnDecl : public Decl 
{
  protected:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    
  public:
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    void Check();
    bool IsFnDecl() { return true; }
    bool IsMethodDecl();
    bool ConflictsWithPrevious(Decl *prev);
    bool MatchesPrototype(FnDecl *other);
    Type *GetReturnType() {return returnType; }
    List<VarDecl*> *GetFormals() { return formals; }
    const char *GetFunctionLabel();
    void Emit(CodeGenerator *cg);
};

#endif
