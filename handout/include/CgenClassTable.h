#ifndef CGENCLASSTABLE_H
#define CGENCLASSTABLE_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/NoFolder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ADT/SmallVector.h>
#include "symtab.h"
#include "cool_tree.h"

class CgenNode;

// CgenClassTable represents the top level of a Cool program, which is
// basically a list of classes. The class table is used to look up classes
// (CgenNodes) by name, and it also handles global code generation tasks.
// The CgenClassTable constructor is where you'll find the entry point for
// code generation for an entire Cool program.
class CgenClassTable : public cool::SymbolTable<CgenNode>
{
  public:
    // CgenClassTable constructor begins and ends the code generation process
    CgenClassTable(Classes);

  private:
    // The following creates an inheritance graph from a list of classes.
    // The graph is implemented as a tree of `CgenNode', and class names
    // are placed in the base class symbol table.
    void install_basic_classes();
    void install_classes(Classes cs);
    void install_class(CgenNode *nd);
    void install_special_class(CgenNode *nd);
    void build_inheritance_tree();
    void set_relations(CgenNode *nd);
    // Create declarations for C runtime functions we need to generate code
    void setup_external_functions();
    void setup_classes(CgenNode *c, int depth);

    // Setup each class in the table and prepare for code generation phase
    void setup();
    void code_module();
    void code_constants();

    // TODO: implement the following functions
    void code_main();
    void code_classes(CgenNode *c);
    // TODO: Copy this from Lab 1 -- we'll use it for CP1
    CgenNode *get_main_main(CgenNode *c);


    CgenNode *root(); // Get the root of the class Tree, i.e. Object
  public:
    int get_num_classes() const { return current_tag; }

  private:
    // Class lists and current class tag
    llvm::SmallVector<CgenNode *> nds, special_nds;
    int current_tag;

    int max_depth;

  public:
    // LLVM Util
    llvm::Function *create_llvm_function(std::string const &funcName,
                                         llvm::Type *retType,
                                         llvm::ArrayRef<llvm::Type *> argTypes,
                                         bool isVarArgs);

    // TODO: Add more utility functions here if you find them useful,
    //       e.g., getting an LLVM type (i32, ptr, Main, ...) for a 
    //       Cool type (Symbol), or any other helper functions that you
    //       find you need in multiple places.
    //       If you need helpers but don't use them in CgenClassTable.cpp,
    //       you can define them in cool_tree.handcode.h instead for the
    //       appropriate class.



    // CgenClassTable owns the current LLVM module and everything attached.
    // One program, one class table, one module.
    llvm::LLVMContext context;
    llvm::IRBuilder<llvm::NoFolder> builder;
    llvm::Module the_module;


    // The following types are declared here for convenience.
    llvm::Type *i32, *i8, *i1, *ptr, *void_ty;

    //Mine
    //Getting the type
    llvm::Type* get_type(Symbol type_decl);
};

 /*********************************************************************
 For convenience, a large number of symbols are predefined here.
 These symbols include the primitive type and method names, as well
 as fixed names used by the runtime system. Feel free to add your
 own definitions as you see fit.
*********************************************************************/
extern Symbol
        // required classes
        Object,
        IO, String, Int, Bool, Main,

        // class methods
        cool_abort, type_name, cool_copy, out_string, out_int, in_string,
        in_int, length, concat, substr,

        // class members
        val,

        // special symbols
        No_class, // symbol that can't be the name of any user-defined class
        No_type, // If e : No_type, then no code is generated for e.
        SELF_TYPE, // Special code is generated for new SELF_TYPE.
        self, // self generates code differently than other references

        // extras
        arg, arg2, newobj, Mainmain, prim_string, prim_int, prim_bool;

#endif //CGENCLASSTABLE_H
