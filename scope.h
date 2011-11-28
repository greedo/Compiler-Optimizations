/* File: scope.h
 * -------------
 * The Scope class will be used to manage scopes, sort of
 * table used to map identifier names to Declaration objects.
 */

#ifndef _H_scope
#define _H_scope

#include "hashtable.h"
//#include "list.h"

class Decl;
class Identifier;
class ClassDecl; 

class Scope { 
  protected:
    Hashtable<Decl*> *table;

  public:
    Scope();

    Decl *Lookup(Identifier *id);
    bool Declare(Decl *dec);
    void CopyFromScope(Scope *other, ClassDecl *cd);
};


#endif
