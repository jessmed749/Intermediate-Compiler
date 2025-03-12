/*********************************************************************
 Intermediate code generator for COOL: SKELETON

 Read the comments carefully and add code to build an LLVM program
*********************************************************************/
#include <cstddef>
#include <string>

#include "CgenClassTable.h"
#include "CgenEnvironment.h"
#include "CgenNode.h"
#include "symbol.h"

extern int cgen_debug, curr_lineno;
using namespace llvm;


/*********************************************************************

  The next two functions are actually called at the end of code
  generation, but are mostly copied from Lab 1 so placed here at
  the top. 
    
*********************************************************************/

// Create LLVM entry point. This function will initiate our Cool program
// by generating the code to execute (new Main).main()
//
// HINT: This will actually be called after you generate the code for Main_main() 
// using the CgenNode::codegen_mainmain() function below, so do this after
// you create a simple codegen_mainmain().

void CgenClassTable::code_main()
{
    // TODO: add code here

    // Define a function main that has no parameters and returns an i32
    // Copy from Lab 1

    // Define an entry basic block
    // Copy from Lab 1

    // TODO: In Lab 2 you will have to allocate the Main object on the heap
    //       and initialize it somehow. There are multiple good ways to do this.


    // TODO: Call Main_main(). This returned int for Lab 1 and Lab 2 CP1
    //       Later it will return the appropriate type defined in the 
    //       Cool program.
    // HINT: Copy from Lab 1 initially, but don't forget to pass in the object
    //       pointer you allocated above.

    // TODO:  Create a global string constant "Main_main() returned %d\n" 
    // HINT:  Copy from Lab 1

    
    
    // TODO: Call printf with the string address of "Main_main() returned %d\n"
    // and the return value of Main_main() as its arguments
    // TODO:  Remove this after Lab 2 CP1.

    //DELETE IF NOT WORK


        // Define main function: int main()
   // Define main function: int main()
    FunctionType *mainType = FunctionType::get(builder.getInt32Ty(), false);
    Function *mainFunc = Function::Create(mainType, Function::ExternalLinkage, "main", &the_module);

    BasicBlock *entry = BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entry);

    // Allocate and initialize Main object by calling Main_new()
    Function *mainNew = the_module.getFunction("Main_new");
    assert(mainNew && "Main_new() function not found!");
    Value *mainObj = builder.CreateCall(mainNew, {}, "mainObj");

    // Check if malloc failed (NULL check)
    Value *isNull = builder.CreateICmpEQ(mainObj, ConstantPointerNull::get(PointerType::getUnqual(context)));
    BasicBlock *abortBlock = BasicBlock::Create(context, "abort", mainFunc);
    BasicBlock *continueBlock = BasicBlock::Create(context, "continue", mainFunc);
    builder.CreateCondBr(isNull, abortBlock, continueBlock);

    // Abort if malloc failed
    builder.SetInsertPoint(abortBlock);
    builder.CreateCall(the_module.getFunction("abort"));
    builder.CreateUnreachable();

    // Continue execution
    builder.SetInsertPoint(continueBlock);
    Function *mainMain = the_module.getFunction("Main_main");
    assert(mainMain && "Main_main() function not found!");

    // Call Main_main, passing 'mainObj' as the 'self' argument
    Value *mainResult = builder.CreateCall(mainMain, {mainObj}, "mainResult");

    // Declare printf function if not already declared
    Function *printfFunc = the_module.getFunction("printf");
    if (!printfFunc) {
        FunctionType *printfType = FunctionType::get(builder.getInt32Ty(),
            {Type::getInt8Ty(context)->getPointerTo()}, true);
        printfFunc = Function::Create(printfType, Function::ExternalLinkage, "printf", &the_module);
    }

    // Create global format string
    Value *formatStr = builder.CreateGlobalStringPtr("Main.main() returned %d\n", "formatStr");

    // Call printf to display result
    builder.CreateCall(printfFunc, {formatStr, mainResult}, "printfCall");

    // Return 0 from main()
    builder.CreateRet(builder.getInt32(0));

}


// For Lab 2 CP1, we will use the same approach as with Lab 1
// code-gen function main() in class Main
void CgenNode::codegen_mainmain() {
    // Copy code from Lab 1 initially, later just handle as part of
    // Setting up class Main.
    //assert(this->name->get_string() == "Main");
    errs() << "Skipping codegen_mainmain() for Main_main to avoid conflicts.\n";

    // // Get function signature
    // llvm::FunctionType *funcType =
    //     llvm::FunctionType::get(
    //         llvm::Type::getInt32Ty(class_table->context), // Return type: int
    //         {get_type()->getPointerTo()}, false);
    //
    // llvm::Function *func =
    //     llvm::Function::Create(
    //         funcType, llvm::Function::ExternalLinkage,
    //         "Main_main", &class_table->the_module);
    //
    // llvm::BasicBlock *entry = llvm::BasicBlock::Create(
    //     class_table->context, "entry", func);
    // class_table->builder.SetInsertPoint(entry);
    //
    // // Ensure it returns 0 instead of aborting
    // class_table->builder.CreateRet(llvm::ConstantInt::get(
    //     class_table->builder.getInt32Ty(), 0));

}

/*********************************************************************

  First pass functions start here

*********************************************************************/

//
// Class setup. You may need to add parameters to this function so that
// the classtable can provide setup information (such as the class tag
// that should be used by this class).
//
// Things that setup should do:
//  - layout the features of the class
//  - create the types for the class and its vtable
//  - create global definitions used by the class such as the class vtable
//
void CgenNode::setup(int tag, int depth) { 
    this->tag = tag; 

    // TODO: Layout the features of the class by implementing 
    //      layout_features(), which is called from here.
    layout_features();

    // TODO: Use information from feature layout to create the class type.
    //       Make sure to use get_type_name() for the name of the class type
    // HINT: Look at llvm::Type::StructType 
    // HINT: Ignore padding for now


    // TODO: CP2 below this point
    // TODO: Use information from feature layout to create the vtable type
    //       Make sure to use get_vtable_type_name() for the name of the vtable type
    // HINT: Look at llvm::Type::StructType


    
    // TODO: Create the global vtable (use get_vtable_name() for the name)
    // HINT: First create a struct instance with llvm::ConstantStruct::get() 

    // HINT: Second, create an llvm::GlobalVariable with the struct instance
    //       by using new llvm::GlobalVariable() and llvm::ConstantStruct::get()

    // HINT: Don't forget the metadata (including a raw character
    //       string for the type name)
    // HINT: You can get the allocated size of a type with 
    //       the_module.getDataLayout().getTypeAllocSize(llvm::Type*);

    //DEFINE CLASS STRUCT
    std::vector<llvm::Type*> classFieldTypes;

    // 1. VTABLE POINTER (opaque pointer)
    classFieldTypes.push_back(
        llvm::PointerType::getUnqual(class_table->context) // Opaque pointer type
    );

    // 2. INHERITED ATTRIBUTES (traverse from the root down)
    std::vector<llvm::Type*> inheritedAttrs;
    CgenNode* parent = get_parent();
    while (parent) {
        // Prepend parent's own attributes in order.
        for (int i = 0; i < parent->features->len(); i++) {
            Feature_class* feature = parent->features->nth(i);
            if (feature->is_attr()) {
                attr_class* attr = (attr_class*)feature;
                llvm::Type* attr_type = class_table->get_type(attr->get_type_decl());
                inheritedAttrs.push_back(attr_type);
            }
        }
        parent = parent->get_parent();
    }
    // Reverse inheritedAttrs so that the topmost parent's attributes come first.
    std::reverse(inheritedAttrs.begin(), inheritedAttrs.end());
    for (llvm::Type* t : inheritedAttrs) {
        classFieldTypes.push_back(t);
    }

    // 3. OWN ATTRIBUTES
    for (int i = 0; i < features->len(); i++) {
        Feature_class* feature = features->nth(i);
        if (feature->is_attr()) {   //stored as virtual in cool_tree
            attr_class* attr = (attr_class*)feature;
            llvm::Type* attr_type = class_table->get_type(attr->get_type_decl());
            classFieldTypes.push_back(attr_type);
        }
    }

    // Create the LLVM struct type for this class
    // llvm::StructType* classStructType = llvm::StructType::create(
    //     class_table->context,
    //     classFieldTypes,
    //     get_type_name() // Unique name for the struct
    // );
    this->classType  = llvm::StructType::create(
    class_table->context,
    classFieldTypes,
    get_type_name()
    );

    //VTABLE STRUCT
    // Define the structure for the vtable
    std::vector<llvm::Type*> vtableFields;
    vtableFields.push_back(llvm::Type::getInt32Ty(class_table->context));  // Class tag
    vtableFields.push_back(llvm::Type::getInt32Ty(class_table->context));  // Object size
    llvm::ArrayType* classNameType = llvm::ArrayType::get(
    llvm::Type::getInt8Ty(class_table->context), get_type_name().size() + 1 // Null-terminated string
    );
    llvm::PointerType* classNamePtrType = llvm::PointerType::getUnqual(classNameType);
    vtableFields.push_back(classNamePtrType);
    llvm::Type* vtableSlotType = llvm::Type::getInt8Ty(class_table->context)->getPointerTo();
    for (auto &[method_name, func_ptr] : method_map) {
        vtableFields.push_back(vtableSlotType);
    }
    llvm::StructType* vtableType = llvm::StructType::create(
        class_table->context, vtableFields, get_vtable_type_name()
    );

    // Step 3: Create Global VTable
    std::vector<llvm::Constant*> vtableValues;

    vtableValues.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(class_table->context), tag));
    uint64_t classSize = class_table->the_module.getDataLayout().getTypeAllocSize(get_type());
    vtableValues.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(class_table->context), classSize));

    llvm::Constant* classNameGlobal = class_table->builder.CreateGlobalString(
     get_type_name(), get_vtable_name() + "_name"
     );
    vtableValues.push_back(llvm::ConstantExpr::getBitCast(classNameGlobal, classNamePtrType));

    for (auto &[method_name, func_ptr] : method_map) {
        vtableValues.push_back(llvm::ConstantExpr::getBitCast(func_ptr, vtableSlotType));
    }

    llvm::Constant* vtableStruct = llvm::ConstantStruct::get(vtableType, vtableValues);
    new llvm::GlobalVariable(
        class_table->the_module, vtableType, true,
        llvm::GlobalValue::PrivateLinkage, vtableStruct, get_vtable_name()
    );



}

// Laying out the features involves creating a Function for each method
// and assigning each attribute a slot in the class structure.
void CgenNode::layout_features() {
    // TODO: In CP1, you will only need to layout the attributes
    // TODO: For CP1, include methods as well
    // HINT: Remember the options of creating the method signatures for
    //       the base classes here or special-casing those earlier.
    //       If you choose to do them here, you'll need to make sure not
    //       to attempt to generate code for the base-class methods---this
    //       code is already in the Cool runtime library (coolrt)
    // HINT: Don't forget about inheritance!
    if (CgenNode* parent = get_parent()) {
        int parent_attr_count = parent->get_attribute_count();
        // If parent's attribute count is 0, default to 1
        attribute_count = (parent_attr_count > 0) ? parent_attr_count : 1;
    } else {
        attribute_count = 1;
    }
    for (int i = 0; i < features->len(); i++) {
        Feature_class* feature = features->nth(i);
        if (feature->is_attr()) {
            feature->layout_feature(this);
        }
    }
}




// Assign this attribute a slot in the class structure
void attr_class::layout_feature(CgenNode *cls)
{
    // TODO: add code here to add an attribute to the layout
    // HINT: Consider inheritance, byt remember that new definitions
    //       of an argument ID are added, leaving the previous ones
    //       alone to correctly and easily deal with inheritance.
    // Get the current attribute count (including inherited attributes)
    int currentSlot = cls->get_attribute_count();
    // Set this attribute's slot to the current count.
    set_slot(currentSlot);
    // Increment the attribute count for the class.
    cls->attribute_count = currentSlot + 1;
}

// Create the LLVM Function corresponding to this method.
void method_class::layout_feature(CgenNode *cls)
{
    // TODO: CP2 -- not needed in CP1

    // TODO: Declare the method and add it to the class
    // HINT: use the method formals and the helper function from ClassTable
    // HINT: We use <class_name>.<method_name> as the function name for basic classes
    //       and <class_name>_<method_name> for non-basic classes
    // HINT: You will need to think about how to handle inheritance
    // HINT: Declare the function similarly to what you did in Lab 1 for Main_main()
    //       though at this point you are only adding a declaration, with the actual
    //       definition happening in the second pass (in code_class()).
    // HINT: Look at the definition of code_class in ClassTable or step through debug.
    // HINT: At some point function pointers will play a role (they are needed for the vtable).
    //       You first need to create the function, then get a pointer to it, and then
    //       use a bitcast to convert it to a constant that can go into the vtable.
    // HINT: Looks up llvm::PointerType::get() and llvm::ConstantExpr::getBitCast() in the 
    //       LLVM documentation
    CgenClassTable *ct = cls->get_class_table();
    LLVMContext &context = ct->context;
    Module &module = ct->the_module;
    std::string method_name = cls->get_type_name() + "_" + name->get_string();
    llvm::Type *return_type = ct->get_type(return_type_decl);
    std::vector<llvm::Type*> arg_types;
    arg_types.push_back(cls->get_type()->getPointerTo()); // self
    for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
        Formal formal = formals->nth(i);
        arg_types.push_back(ct->get_type(formal->get_type_decl()));
    }
    llvm::FunctionType *func_type = llvm::FunctionType::get(return_type, arg_types, false);
    llvm::Function *func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, method_name, &module);
    cls->add_method_to_vtable(name, func);

}


/*********************************************************************

  Second pass functions start here

*********************************************************************/


// Class codegen. This should be performed after every class has been set up.
// Generate code for each method of the class.
void CgenNode::code_class() {
    // No code generation for basic classes. The runtime will handle that.
    if (basic()) {
      return;
    }
    // TODO: add code here for programmer-defined classes
    // HINT: You'll need an environment and generate code for each method,
    //       including the initializer for an object (a.k.a. constructor).
    CgenEnvironment env(this);
    code_init_function(&env);
    for (int i = 0; i < features->len(); i++) {
        Feature_class* feature = features->nth(i);
        if (!feature->is_attr()) {
            method_class* method = static_cast<method_class*>(feature);
            method->code(&env);
        }
    }
}
  
void CgenNode::code_init_function(CgenEnvironment *env) {
    // TODO: Add code here for class initialization (constructor)
    // HINT: You can choose to allocate memory with malloc here, or
    //       you can decide to do it elsewhere.
    // HINT: To allocate memory, generate a call to malloc with
    //       IRBuilder that will be emitted into the LLVM bitcode
    //       and run at runtime. We suggest that you do not
    //       use builder.CreateMalloc().
    // HINT: You may find the_module.getNamedGlobal() useful.
    // HINT: You may find llvm::StructType::getTypeByName() useful.
    // HINT: You will need to use builder.CreateStructGEP().

    //CHANGE IF NOT WORK

  std::string initFuncName = get_type_name() + "_new";
  llvm::FunctionType *funcType = llvm::FunctionType::get(get_type()->getPointerTo(), false);
  llvm::Function *initFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, initFuncName, &get_class_table()->the_module);
  llvm::BasicBlock *entry = llvm::BasicBlock::Create(env->context, "entry", initFunc);
  env->builder.SetInsertPoint(entry);

  llvm::Function *mallocFunc = get_class_table()->the_module.getFunction("malloc");
  assert(mallocFunc && "malloc function not found!");
  uint64_t objectSize = get_class_table()->the_module.getDataLayout().getTypeAllocSize(get_type());
  llvm::Value *sizeValue = llvm::ConstantInt::get(env->builder.getInt32Ty(), objectSize);
  llvm::Value *rawPtr = env->builder.CreateCall(mallocFunc, {sizeValue}, "rawObj");
  llvm::PointerType *genericPtrType = llvm::PointerType::getUnqual(env->context);
  llvm::Value *isNull = env->builder.CreateICmpEQ(rawPtr, llvm::ConstantPointerNull::get(genericPtrType));
  llvm::BasicBlock *abortBlock = llvm::BasicBlock::Create(env->context, "abortBlock", initFunc);
  llvm::BasicBlock *continueBlock = llvm::BasicBlock::Create(env->context, "continueBlock", initFunc);
  env->builder.CreateCondBr(isNull, abortBlock, continueBlock);

  env->builder.SetInsertPoint(abortBlock);
  env->builder.CreateCall(env->the_module.getFunction("abort"));
  env->builder.CreateUnreachable();

  env->builder.SetInsertPoint(continueBlock);
  llvm::Value *objPtr = env->builder.CreateBitCast(rawPtr, get_type()->getPointerTo(), "objPtr");

  // Set the vtable pointer (field 0)
  llvm::GlobalVariable *vtable = class_table->the_module.getNamedGlobal(get_vtable_name());
  assert(vtable && "VTable global variable not found!");
  llvm::Value *vtablePtr = env->builder.CreateStructGEP(get_type(), objPtr, 0, "vtablePtr");
  llvm::Value *castVtable = env->builder.CreateBitCast(vtable, llvm::PointerType::getUnqual(env->context), "castVtable");
  env->builder.CreateStore(castVtable, vtablePtr);

  // Now, initialize each attribute field (own attributes; you might also include inherited ones)
  for (int i = 0; i < features->len(); i++) {
      Feature_class* feature = features->nth(i);
      if (feature->is_attr()) {
          attr_class* attr = static_cast<attr_class*>(feature);
          llvm::Value *initVal;
          // Check if an initializer is provided.
          if (!attr->get_init()->is_no_expr()) {
              initVal = attr->code(env); // This calls the initializer's code
          } else {
              // Use a default value based on type. For Int, default 0; for Bool, default false.
              std::string typeStr = attr->get_type_decl()->get_string();
              if (typeStr == "Int")
                  initVal = ConstantInt::get(env->builder.getInt32Ty(), 0);
              else if (typeStr == "Bool")
                  initVal = ConstantInt::get(env->builder.getInt1Ty(), 0);
              else
                  initVal = Constant::getNullValue(env->builder.getInt8Ty()->getPointerTo());
          }
          // Compute pointer to this attribute’s slot using the slot number.
          llvm::Value *attrPtr = env->builder.CreateStructGEP(get_type(), objPtr, attr->get_slot(), attr->get_name()->get_string() + "_attr_ptr");
          env->builder.CreateStore(initVal, attrPtr);
      }
  }

  env->builder.CreateRet(objPtr);


}

/*********************************************************************

  APS class methods

    Fill in the following methods to produce code for the
    appropriate expression. You may add or remove parameters
    as you wish, but if you do, remember to change the parameters
    of the declarations in `cool-tree.handcode.h'.

    Look for the various TODO comments

*********************************************************************/
int CgenNode::find_attribute_slot_for(Symbol attrName) {
    for (int i = 0; i < features->len(); i++) {
        Feature_class *feature = features->nth(i);
        if (feature->is_attr()) {
            attr_class *attr = static_cast<attr_class*>(feature);
            // Compare the actual string values:
            if (strcmp(attr->get_name()->get_string().c_str(),
                       attrName->get_string().c_str()) == 0) {
                return attr->get_slot();
                       }
        }
    }
    return (get_parent() ? get_parent()->find_attribute_slot_for(attrName) : -1);
}


llvm::Value* bool_const(bool value, CgenEnvironment *env) {
    return llvm::ConstantInt::get(env->builder.getInt1Ty(), value);
}

Value *attr_class::code(CgenEnvironment *env)
{
    // TODO: Add code here for emitting initializing an attribute
    return init->code(env);
}


// Create a method body
Function *method_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
    {
        errs() << "method" << "\n";
    }

    // TODO: for CP1, you should be able to copy your code from Lab 1, but you will need
    //       to grab the self pointer from the first argument of the function. See hints below.
    // TODO: For CP2, you will need to add code here to generate any method body, not just Main_main.
    //       In P2, instead of defining the function here, you will have defined it in layout_feature().

    // HINT: Recall that you can and should use helper functions from the include files
    //       and especially CgenEnvironment.h
    // HINT: Recall that every LLVM procedure needs and entry basic block
    // HINT: Look at the IRBuilder tutorial and/or documentation for help
    // HINT: The names of the class and method are available as a Symbol in the 
    //       class (through the environment) and the method (using the "this" pointer). 
    //       You can turn symbols to strings with ->get_string().   
    
    // HINT: You may find Function* func->arg_begin() useful to iterate over arguments, 
    //       but you may not need it depending on how you chose to implement things overall.
    
    // 1) Create (or retrieve) the LLVM Function for this method.
    //    For Lab 1 with a single Main.main(), you can do something like:
    CgenNode *cls = env->get_class();
    LLVMContext &ctx = env->context;
    llvm::Type *retTy = env->class_table.get_type(return_type_decl);
    std::vector<llvm::Type*> arg_types;
    arg_types.push_back(cls->get_type()->getPointerTo());
    llvm::FunctionType *funcTy = llvm::FunctionType::get(retTy, arg_types, false);
    std::string methodName = cls->get_type_name() + "_" + name->get_string();
    Function *F = Function::Create(funcTy, Function::ExternalLinkage, methodName, &env->the_module);
    BasicBlock *entryBB = BasicBlock::Create(ctx, "entry", F);
    env->builder.SetInsertPoint(entryBB);
    auto argIter = F->arg_begin();
    Value *selfArg = &*argIter;
    selfArg->setName("self");
    env->add_binding(self, selfArg, cls->get_type()->getPointerTo());
    Value *resultVal = expr->code(env);
    if (!resultVal) {
        resultVal = ConstantInt::get(env->builder.getInt32Ty(), 0);
    }
    if (retTy->isIntegerTy(32) && resultVal->getType()->isIntegerTy(1)) {
        resultVal = env->builder.CreateZExt(resultVal, env->builder.getInt32Ty(), "bool_to_int");
    }
    env->builder.CreateRet(resultVal);
    return F;
}

// Expression to create a new object
Value *new__class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "newClass" << "\n";
    // TODO: add code here and replace `return nullptr`

    // HINT: This is where you will need to allocate memory for the object
    //       and call the constructor. You can choose to allocate memory here
    //       or in the constructor. Coolrt does it in the constructor.
    if (cgen_debug) errs() << "newClass: " << type_name->get_string() << "\n";

    // 1. Get the class's constructor function (e.g., Main_new)
    std::string typeStr = type_name->get_string();
    // For primitive types, return the default constant value.
    if (typeStr == "Int") {
        return ConstantInt::get(env->builder.getInt32Ty(), 0);
    }
    if (typeStr == "Bool") {
        return ConstantInt::get(env->builder.getInt1Ty(), 0);
    }
    // Otherwise, proceed normally.
    std::string newFuncName = typeStr + "_new";
    Function *newFunc = env->the_module.getFunction(newFuncName);
    assert(newFunc && "Constructor not found!");
    Value *newObj = env->builder.CreateCall(newFunc, {}, "newObj");
    return newObj;
}

// Codegen for expressions. Note that each expression has a value.
// HINT: Think about what each of these generates code for and read
//       the Cool manual carefully and the longer CoolAid document
// HINT: If you type this->, the IDE will show you all the members
//       you have access to. The include files have them too, of course.
// TODO: Many of these are just copied over from Lab 1

Value *int_const_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "Integer Constant" << "\n";

    // TODO: Copy this from your Lab 1 
    std::string strValue = this->token->get_string();
    int64_t intValue = std::stoll(strValue);
    intValue = intValue % (1LL << 32);
    if (intValue > INT32_MAX) intValue -= (1LL << 32);
    return ConstantInt::get(env->builder.getInt32Ty(), intValue);
}

Value *bool_const_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "Boolean Constant" << "\n";

    // TODO: Copy this from your Lab 1 
    return bool_const(this->val, env);
}

Value *plus_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "plus" << "\n";

    // TODO: Copy from Lab 1
    Value *left = e1->code(env);
    Value *right = e2->code(env);
    //LHS and RHS
    return env->builder.CreateAdd(left, right);
}

Value *sub_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "sub" << "\n";

    // TODO: Copy from Lab 1
    Value *left = e1->code(env);
    Value *right = e2->code(env);

    return env->builder.CreateSub(left, right);
}

Value *mul_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "mul" << "\n";

    // TODO: Copy this from Lab 1
    Value *left = e1->code(env);
    Value *right = e2->code(env);
    return env->builder.CreateMul(left, right);
}

Value *divide_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "div" << "\n";

    // TODO: Copy this from your Lab 1
    // TODO: You may choose to use Object_abort() in CP2
    Value *left = e1->code(env);
    Value *right = e2->code(env);
    Value *isZero = env->builder.CreateICmpEQ(right, ConstantInt::get(env->builder.getInt32Ty(), 0));
    BasicBlock *divByZeroBlock = BasicBlock::Create(env->context, "divByZero", env->builder.GetInsertBlock()->getParent());
    BasicBlock *continueBlock = BasicBlock::Create(env->context, "continue", env->builder.GetInsertBlock()->getParent());
    env->builder.CreateCondBr(isZero, divByZeroBlock, continueBlock);
    env->builder.SetInsertPoint(divByZeroBlock);
    env->builder.CreateCall(env->the_module.getFunction("abort"));
    env->builder.CreateUnreachable();
    env->builder.SetInsertPoint(continueBlock);
    return env->builder.CreateSDiv(left, right);
}

Value *neg_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "neg" << "\n";

    // TODO: Copy this from your Lab 1
    Value *operand = e1->code(env);
    return env->builder.CreateNeg(operand);
}

Value *comp_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "complement" << "\n";

    // TODO: Copy this from your Lab 1

    // operand is of boolean
    Value *operand = e1->code(env);
    if (!operand->getType()->isIntegerTy(1)) {
        operand = env->builder.CreateICmpNE(operand, ConstantInt::get(operand->getType(), 0), "toBool");
    }
    return env->builder.CreateXor(operand, ConstantInt::get(env->builder.getInt1Ty(), 1), "not");
}

Value *lt_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "lt" << "\n";

    // TODO: Copy this from your Lab 1
    Value *left = e1->code(env);
    Value *right = e2->code(env);
    return env->builder.CreateICmpSLT(left, right, "lt");
}

Value *eq_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "eq" << "\n";

    // TODO: Copy this from your Lab 1
    Value *left  = e1->code(env);
    Value *right = e2->code(env);
    if (left->getType()->isIntegerTy(1) && right->getType()->isIntegerTy(32)) {
        left = env->builder.CreateZExt(left, env->builder.getInt32Ty(), "bool_lhs_as_i32");
    } else if (left->getType()->isIntegerTy(32) && right->getType()->isIntegerTy(1)) {
        right = env->builder.CreateZExt(right, env->builder.getInt32Ty(), "bool_rhs_as_i32");
    }
    return env->builder.CreateICmpEQ(left, right, "eqtmp");
}

Value *leq_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "leq" << "\n";

    // TODO: Copy this from your Lab 1
    Value *left = e1->code(env);
    Value *right = e2->code(env);
    return env->builder.CreateICmpSLE(left, right, "leq");
}

Value *block_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "block" << "\n";

    // TODO: Copy this from your Lab 1
    Value *last = nullptr;
    for (int i = 0; i < body->len(); ++i) {
        last = body->nth(i)->code(env);
    }
    return last;
}

Value *let_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "let" << "\n";

    // TODO: Start by copying from Lab 1, but this will
    //       need to be updated for objects
    // NOTE: While the operational semantics suggest that objects are copied
    //       we will instead just assign the pointer to the object

    llvm::Type *varType = nullptr;
    std::string typeStr = type_decl->get_string();
    if (typeStr == "Bool") {
        varType = env->builder.getInt1Ty();
    } else if (typeStr == "Int") {
        varType = env->builder.getInt32Ty();
    } else {
        varType = PointerType::getUnqual(env->context);
    }
    Value *initValue = nullptr;
    if (init && !init->is_no_expr()) {
        initValue = init->code(env);
    }
    if (!initValue) {
        if (typeStr == "Bool" || typeStr == "Int")
            initValue = ConstantInt::get(varType, 0);
        else
            initValue = Constant::getNullValue(varType);
    }
    env->open_scope();
    Function *function = env->builder.GetInsertBlock()->getParent();
    IRBuilder<> entryBuilder(&function->getEntryBlock(), function->getEntryBlock().begin());
    AllocaInst *allocaInst = entryBuilder.CreateAlloca(varType, nullptr, identifier->get_string());
    env->builder.CreateStore(initValue, allocaInst);
    env->add_binding(identifier, allocaInst, varType);
    Value *bodyValue = body->code(env);
    env->close_scope();
    return bodyValue;
}

Value *assign_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "assign" << "\n";

    // TODO: add code here and replace `return nullptr`
    // Copy this from your Lab 1, but update for objects
    // HINT: You will need to use builder.CreateStructGEP()
    //       for attributes somehow.
    // Generate the new (RHS) value.
   Value *rhsVal = expr->code(env);
    auto [localType, localPtr] = env->find_in_scopes(name);
    if (localPtr) {
        if (rhsVal->getType()->isIntegerTy(1) && localType->isIntegerTy(32))
            rhsVal = env->builder.CreateZExt(rhsVal, env->builder.getInt32Ty());
        else if (rhsVal->getType()->isIntegerTy(32) && localType->isIntegerTy(1))
            rhsVal = env->builder.CreateICmpEQ(rhsVal, ConstantInt::get(Type::getInt32Ty(env->context), 0));
        env->builder.CreateStore(rhsVal, localPtr);
        return rhsVal;
    } else {
        auto [selfType, selfPtr] = env->find_in_scopes(self);
        if (!selfPtr) {
            errs() << "Error: 'self' not found in environment for assignment to " << name->get_string() << "\n";
            Function *function = env->builder.GetInsertBlock()->getParent();
            BasicBlock *abortBlock = BasicBlock::Create(env->context, "assign_abort", function);
            env->builder.CreateBr(abortBlock);
            env->builder.SetInsertPoint(abortBlock);
            env->builder.CreateCall(env->the_module.getFunction("abort"));
            env->builder.CreateUnreachable();
            return UndefValue::get(env->builder.getInt32Ty());
        }
        int slot = env->get_class()->find_attribute_slot_for(name);
        if (slot < 0) {
            errs() << "Error: Attribute " << name->get_string()
                   << " not found in class " << env->get_class()->get_type_name() << "\n";
            Function *function = env->builder.GetInsertBlock()->getParent();
            BasicBlock *abortBlock = BasicBlock::Create(env->context, "assign_abort", function);
            env->builder.CreateBr(abortBlock);
            env->builder.SetInsertPoint(abortBlock);
            env->builder.CreateCall(env->the_module.getFunction("abort"));
            env->builder.CreateUnreachable();
            return UndefValue::get(env->builder.getInt32Ty());
        }
        Value *attrPtr = env->builder.CreateStructGEP(env->get_class()->get_type(), selfPtr, slot, name->get_string() + "_attr_ptr");
        if (!attrPtr) {
            errs() << "Error: Failed to compute attribute pointer for " << name->get_string() << "\n";
            return nullptr;
        }
        env->builder.CreateStore(rhsVal, attrPtr);
        return rhsVal;
    }
}

Value *cond_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "cond" << "\n";

    // TODO: Copy this from your Lab 1 to start, but update to handle 
    //       cases where the branches are not the same type. 
    // HINT: This is where boxing might happen.
    // HINT: Unboxing can only happen with a case expression

    Value *condVal = pred->code(env);
    if (!condVal->getType()->isIntegerTy(1)) {
        condVal = env->builder.CreateICmpNE(condVal, ConstantInt::get(condVal->getType(), 0), "toBool");
    }
    Function *func = env->builder.GetInsertBlock()->getParent();
    BasicBlock *thenBB  = BasicBlock::Create(env->context, "then", func);
    BasicBlock *elseBB  = BasicBlock::Create(env->context, "else", func);
    BasicBlock *mergeBB = BasicBlock::Create(env->context, "ifcont", func);
    env->builder.CreateCondBr(condVal, thenBB, elseBB);
    env->builder.SetInsertPoint(thenBB);
    Value *thenVal = then_exp->code(env);
    if (!thenVal) { thenVal = ConstantInt::get(env->builder.getInt32Ty(), 0); }
    env->builder.CreateBr(mergeBB);
    env->builder.SetInsertPoint(elseBB);
    Value *elseVal = else_exp->code(env);
    if (!elseVal) { elseVal = ConstantInt::get(env->builder.getInt32Ty(), 0); }
    env->builder.CreateBr(mergeBB);
    env->builder.SetInsertPoint(mergeBB);
    PHINode *phi = env->builder.CreatePHI(thenVal->getType(), 2, "iftmp");
    phi->addIncoming(thenVal, thenBB);
    phi->addIncoming(elseVal, elseBB);
    return phi;
}

Value *loop_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "loop" << "\n";

    // TODO: add code here and replace `return nullptr`
    Function *func = env->builder.GetInsertBlock()->getParent();
    BasicBlock *condBB  = BasicBlock::Create(env->context, "loopCond", func);
    BasicBlock *bodyBB  = BasicBlock::Create(env->context, "loopBody", func);
    BasicBlock *exitBB  = BasicBlock::Create(env->context, "loopExit", func);
    env->builder.CreateBr(condBB);
    env->builder.SetInsertPoint(condBB);
    Value *condVal = pred->code(env);
    if (!condVal->getType()->isIntegerTy(1)) {
        condVal = env->builder.CreateICmpNE(condVal, ConstantInt::get(condVal->getType(), 0), "toBool");
    }
    env->builder.CreateCondBr(condVal, bodyBB, exitBB);
    env->builder.SetInsertPoint(bodyBB);
    Value *bodyVal = body->code(env); // Ignored per Lab 1
    env->builder.CreateBr(condBB);
    env->builder.SetInsertPoint(exitBB);
    return ConstantInt::get(env->builder.getInt32Ty(), 0);
}



Value *object_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "Object" << "\n";

    // TODO: Copy this from your Lab 1, but you will need to update
    //       for real objects and for attributes.
    // HINT: There are no nested attributes and attributes cannot be
    //       overridden in Cool.
    // HINT: You will need to use builder.CreateStructGEP()

   auto [ty, valPtr] = env->find_in_scopes(name);
    if (valPtr) {
        return env->builder.CreateLoad(ty, valPtr, name->get_string());
    } else {
        auto [selfTy, selfVal] = env->find_in_scopes(self);
        if (!selfVal) {
            errs() << "Error: 'self' not found for object lookup of " << name->get_string() << "\n";
            Function *function = env->builder.GetInsertBlock()->getParent();
            BasicBlock *abortBlock = BasicBlock::Create(env->context, "attrNotFound", function);
            env->builder.CreateBr(abortBlock);
            env->builder.SetInsertPoint(abortBlock);
            env->builder.CreateCall(env->the_module.getFunction("abort"));
            env->builder.CreateUnreachable();
            return UndefValue::get(env->builder.getInt32Ty());
        }
        int slot = env->get_class()->find_attribute_slot_for(name);
        if (slot < 0) {
            errs() << "Error: Attribute " << name->get_string() << " not found in class " << env->get_class()->get_type_name() << "\n";
            Function *function = env->builder.GetInsertBlock()->getParent();
            BasicBlock *abortBlock = BasicBlock::Create(env->context, "attrNotFound", function);
            env->builder.CreateBr(abortBlock);
            env->builder.SetInsertPoint(abortBlock);
            env->builder.CreateCall(env->the_module.getFunction("abort"));
            env->builder.CreateUnreachable();
            return UndefValue::get(env->builder.getInt32Ty());
        }
        Value *attrPtr = env->builder.CreateStructGEP(env->get_class()->get_type(), selfVal, slot, name->get_string() + "_attr_ptr");
        if (!attrPtr) {
            errs() << "Error: Failed to compute attribute pointer for " << name->get_string() << "\n";
            return nullptr;
        }
        // Instead of using the entire class type, get the field type for this attribute.
        llvm::Type *fieldType = env->get_class()->get_type()->getStructElementType(slot);
        return env->builder.CreateLoad(fieldType, attrPtr, name->get_string());
    }
}

Value *no_expr_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "No_expr" << "\n";

    // HINT: Already implemented 
    return nullptr;
}

//*****************************************************************
// The next few functions are for node types that were not supported 
// in Lab 1 but that you now need to implement in Lab 2.
//*****************************************************************

Value *isvoid_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "isvoid" << "\n";
    // TODO: add code here and replace `return nullptr`
    //       This is for the dynamic test of an object being void
    // TODO: CP2 -- not needed in CP1
    // HINT: Consider builder.CreateIsNull()
    return nullptr;
}

Value *dispatch_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "dispatch" << "\n";
    // TODO: add code here and replace `return nullptr`
    // TODO: CP2 -- not needed in CP1

    // HINT: Cool actually passes objects by reference and Ints and Bools 
    //       by unboxed value -- this is what LLVM already does for calls.
    // HINT: Don't forget about vtables.
    // HINT: Add a dynamic check for void dispatch (i.e., on this->expr).
    // HINT: To use a function pointer, you will need to:
    //       1. Get the value of the function pointer
    //       2. Create the type for the function using llvm::FunctionType::get()
    //       3. Create the call: builder.CreateCall(funcTy, funcPtr, formals);
    // HINT: The return type of the method is "type"
    // HINT: The method name is "name"
    // HINT: The method arguments are "actual" (and it's a list)
    // HINT: The method is called on "expr"
    // HINT: You may find the following helpful (but you can get by with it):
    //       llvm::StructType::getTypeByName()


    return nullptr;
}

Value *static_dispatch_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "static dispatch" << "\n";
    // TODO: add code here and replace `return nullptr`
    // TODO: CP2 -- not needed in CP1

    // HINT: Cool actually passes objects by reference and Ints and Bools 
    //       by unboxed value -- this is what LLVM already does for calls.
    // HINT: Don't forget about vtables.
    // HINT: Add a dynamic check for void dispatch (i.e., on this->expr).

    return nullptr;
}

Value *string_const_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "string_const" << "\n";
    // TODO: add code here and replace `return nullptr`
    // TODO: CP2 -- not needed in CP1

    // HINT: The easiest way to handle this is to just box a char
    //       string every time. You can also choose to box the string
    //       just once, but that is more complicated.
    // HINT: The coolrt runtime library implements String_new().

    return nullptr;
}


// Handle a Cool case expression (selecting based on the type of an object)
Value *typcase_class::code(CgenEnvironment *env)
{
    if (cgen_debug)
        errs() << "typecase::code()" << "\n";
    
    // TODO: add code here and replace `return nullptr`
    // TODO: CP3 -- not needed in CP1 or CP2

    return nullptr;
}


// Handle one branch of a Cool case expression.
// If the source tag is >= the branch tag
// and <= (max child of the branch class) tag,
// then the branch is a superclass of the source.
// See the Lab 2 handout for more information about our use of class tags.
Value *branch_class::code(Value *expr_val, Value *tag, Type *join_type,
    CgenEnvironment *env)
{
    // TODO: add code here and replace `return nullptr`
    // TODO: CP3 -- not needed in CP1 or CP2

    return nullptr;
}

