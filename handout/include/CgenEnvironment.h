#ifndef CGENENVIRONMENT_H
#define CGENENVIRONMENT_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/NoFolder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "CgenClassTable.h"
#include "CgenNode.h"

#include <map>
#include "symbol.h"


// CgenEnvironment provides the environment for code generation of a method.
// Its main task is to provide a mapping from Cool names to LLVM Values.
// This mapping needs to be maintained as scopes are entered and exited, new
// variables are declared, and so on. CgenEnvironment is also a good place
// to put non-local information you will need during code generation. Two
// examples are the current CgenNode and the current Function.
class CgenEnvironment
{
  public:
    // Class CgenEnvironment should be constructed by a class prior to code
    // generation for each method. You may need to add parameters to this
    // constructor.
    CgenEnvironment(CgenNode *cur_class) :
        var_table(), cur_class(cur_class), type_table(),
        class_table(*cur_class->get_classtable()), context(class_table.context),
        builder(class_table.builder), the_module(class_table.the_module)
    {
        var_table.enterscope();
        type_table.enterscope();
        // add code here as needed
    }

    CgenNode *get_class() { return cur_class; }
    void set_class(CgenNode *c) { cur_class = c; }

    // Must return the CgenNode for a class given the symbol of its name
    CgenNode *type_to_class(Symbol t);

    std::pair<llvm::Type *, llvm::Value *> find_in_scopes(Symbol name); // the symbol in the pair is the t    auto vtable = env->builder.CreateLoad(env->class_table.ptr, vtablePtr);


    void add_binding(Symbol name, llvm::Value *val_ptr, llvm::Type *type_ptr)
    {
        var_table.insert(name, val_ptr);
        type_table.insert(name, type_ptr);
    }

    // NEW HELPER: Add a local binding for a symbol (without specifying type)
    void add_local(Symbol name, llvm::Value *val) {
        var_table.insert(name, val);
    }


    void open_scope() { var_table.enterscope(); type_table.enterscope(); }
    void close_scope() { var_table.exitscope(); type_table.exitscope(); }

    // LLVM Utils:
    // Create a new llvm function in the current module
    llvm::Function *create_llvm_function(std::string const &funcName,
                                         llvm::Type *retType,
                                         llvm::ArrayRef<llvm::Type *> argTypes,
                                         bool isVarArgs)
    {
        return class_table.create_llvm_function(funcName, retType, argTypes,
                                                isVarArgs);
    }
    // Insert a new BasicBlock at the end of the current function (the function
    // that builder is in)
    llvm::BasicBlock *new_bb_at_fend(std::string const &name)
    {
        return llvm::BasicBlock::Create(context, name,
                                        builder.GetInsertBlock()->getParent());
    }
    // Insert an alloca instruction in the head BasicBlock of the current
    // function, such that this alloca is available in all BasicBlocks of the
    // function.
    llvm::AllocaInst *insert_alloca_at_head(llvm::Type *ty);
    // Get or insert a BasicBlock with the name "abort" which calls the ::abort
    // function. This block will be inserted at the end of the given function,
    // without moving the builder.
    llvm::BasicBlock *get_or_insert_abort_block(llvm::Function *f);

    // You may add more functions here as necessary

  private:
    // mapping from variable names to memory locations
    // TODO: if you changed SymbolTable in lab 1 to tracking types, copy your changes here
    cool::SymbolTable<llvm::Value> var_table;
    cool::SymbolTable<llvm::Type> type_table;
    CgenNode *cur_class;

  public:
    CgenClassTable &class_table;
    // These are references to the current LLVM context and module,
    // and they point to the ones in CgenClassTable.
    llvm::LLVMContext &context;
    llvm::IRBuilder<llvm::NoFolder> &builder;
    llvm::Module &the_module;






};
#endif //CGENENVIRONMENT_H
