#include "CgenEnvironment.h"
#include "CgenNode.h"
#include "symbol.h"
#include "symtab.h"

using namespace llvm;
/*********************************************************************

CgenEnvironment functions

*********************************************************************/

// Look up a CgenNode given a symbol
CgenNode *CgenEnvironment::type_to_class(Symbol t)
{
    
    return t == SELF_TYPE ? get_class()
                          : get_class()->get_classtable()->find_in_scopes(t);
}


std::pair<Type *, Value *>
CgenEnvironment::find_in_scopes(Symbol name)
{
    // TODO: implement
    llvm::Type *type_ptr = type_table.find_in_scopes(name);
    llvm::Value *val_ptr = var_table.find_in_scopes(name);

    if (!type_ptr) {
        // If type isn't found, default to pointer type
        type_ptr = llvm::PointerType::getUnqual(context);
    }
    return {type_ptr, val_ptr};
}

BasicBlock *CgenEnvironment::get_or_insert_abort_block(Function *f)
{
    for (auto &bb: *f)
    {
        if (bb.getName() == "abort")
        {
            return &bb;
        }
    }
    auto *abort_bb = BasicBlock::Create(this->context, "abort", f);
    Type *void_ = Type::getVoidTy(this->context);
    IRBuilder<llvm::NoFolder> builder(abort_bb);
    FunctionCallee abort = this->the_module.getOrInsertFunction("abort", void_);
    builder.CreateCall(abort, {});
    builder.CreateUnreachable();
    return abort_bb;
}

AllocaInst *CgenEnvironment::insert_alloca_at_head(Type *ty)
{
    BasicBlock &entry_bb =
            builder.GetInsertBlock()->getParent()->getEntryBlock();
    if (entry_bb.empty())
    {
        // Insert "at the end" of this bb
        return new AllocaInst(ty, 0, "", &entry_bb);
    }
    else
    {
        // Insert before the first instruction of this bb
        return new AllocaInst(ty, 0, "", &entry_bb.front());
    }
}
