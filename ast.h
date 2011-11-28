/* File: ast.h
 * ----------- 
 * This file defines the abstract base class Node and the concrete 
 * Identifier and Error node subclasses that are used through the tree as 
 * leaf nodes. A parse tree is a hierarchical collection of ast nodes (or, 
 * more correctly, of instances of concrete subclassses such as VarDecl,
 * ForStmt, and AssignExpr).
 * 
 * Location: Each node maintains its lexical location (line and columns in 
 * file), that location can be NULL for those nodes that don't care/use 
 * locations. The location is typcially set by the node constructor.  The 
 * location is used to provide the context when reporting semantic errors.
 *
 * Parent: Each node has a pointer to its parent. For a Program node, the 
 * parent is NULL, for all other nodes it is the pointer to the node one level
 * up in the parse tree.  The parent is not set in the constructor (during a 
 * bottom-up parse we don't know the parent at the time of construction) but 
 * instead we wait until assigning the children into the parent node and then 
 * set up links in both directions. The parent link is typically not used 
 * during parsing, but is more important in later phases.

 */

#ifndef _H_ast
#define _H_ast

#include <stdlib.h>   // for NULL
#include "location.h"
#include <iostream>

class Scope;
class Decl;
class Identifier;
class Type;
class CodeGenerator;

class Node 
{
  protected:
    yyltype *location;
    Node *parent;
    Scope *nodeScope;

  public:
    Node(yyltype loc);
    Node();
    
    yyltype *GetLocation()   { return location; }
    void SetParent(Node *p)  { parent = p; }
    Node *GetParent()        { return parent; }
    virtual void Check() {} // not abstract, since some nodes have nothing to do
    
    typedef enum { kShallow, kDeep } lookup;
    virtual Decl *FindDecl(Identifier *id, lookup l = kDeep);
    virtual Scope *PrepareScope() { return NULL; }
    template <class Specific> Specific *FindSpecificParent() {
        Node *p = parent;
        while (p) {
            if (Specific *s = dynamic_cast<Specific*>(p)) return s;
            p = p->parent;
        }
        return NULL;
    }
	 
    virtual void Emit(CodeGenerator *cg) {} // not abstract, some nodes do nothing
};
   

class Identifier : public Node 
{
  protected:
    char *name;
    Decl *cached;
    
  public:
    Identifier(yyltype loc, const char *name);
    friend std::ostream& operator<<(std::ostream& out, Identifier *id) { return out << id->name; }
    const char *GetName() { return name; }
    Decl *GetDeclRelativeToBase(Type *base = NULL);
};


// This node class is designed to represent a portion of the tree that 
// encountered syntax errors during parsing. The partial completed tree
// is discarded along with the states being popped, and an instance of
// the Error class can stand in as the placeholder in the parse tree
// when your parser can continue after an error.
class Error : public Node
{
  public:
    Error() : Node() {}
};



#endif
