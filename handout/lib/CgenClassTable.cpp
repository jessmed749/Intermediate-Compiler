#include <cstddef>
#include <llvm/Support/FileSystem.h>
#include <sstream>
#include <iostream>
#include <string>

#include "CgenClassTable.h"
#include "CgenNode.h"
#include "symbol.h"
#include "stringtab.h"
#include "CgenEnvironment.h"

extern int cgen_debug, curr_lineno;
using namespace llvm;

/*********************************************************************
 For convenience, a large number of symbols are predefined here.
 These symbols include the primitive type and method names, as well
 as fixed names used by the runtime system. Feel free to add your
 own definitions as you see fit.
*********************************************************************/
Symbol
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

// Initializing the predefined symbols.
static void initialize_constants(void)
{
    Object = idtable.add_string("Object");
    IO = idtable.add_string("IO");
    String = idtable.add_string("String");
    Int = idtable.add_string("Int");
    Bool = idtable.add_string("Bool");
    Main = idtable.add_string("Main");

    cool_abort = idtable.add_string("abort");
    type_name = idtable.add_string("type_name");
    cool_copy = idtable.add_string("copy");
    out_string = idtable.add_string("out_string");
    out_int = idtable.add_string("out_int");
    in_string = idtable.add_string("in_string");
    in_int = idtable.add_string("in_int");
    length = idtable.add_string("length");
    ::concat = idtable.add_string("concat");
    substr = idtable.add_string("substr");

    val = idtable.add_string("val");

    No_class = idtable.add_string("_no_class");
    No_type = idtable.add_string("_no_type");
    SELF_TYPE = idtable.add_string("SELF_TYPE");
    self = idtable.add_string("self");

    arg = idtable.add_string("arg");
    arg2 = idtable.add_string("arg2");
    newobj = idtable.add_string("_newobj");
    Mainmain = idtable.add_string("main");
    prim_string = idtable.add_string("sbyte*");
    prim_int = idtable.add_string("int");
    prim_bool = idtable.add_string("bool");
}

/*********************************************************************

  CgenClassTable methods

*********************************************************************/

// CgenClassTable constructor orchestrates all code generation
CgenClassTable::CgenClassTable(Classes classes) :
    nds(), current_tag(0), context(), builder(this->context),
    the_module("module", this->context)
{
    if (cgen_debug)
        errs() << "Building CgenClassTable" << "\n";
    // Make sure we have a scope, both for classes and for constants
    enterscope();

    // Create an inheritance tree with one CgenNode per class.
    install_basic_classes();
    install_classes(classes);
    build_inheritance_tree();

    // First pass
    setup();

    // Second pass
    code_module();
    // Done with code generation: exit scopes
    exitscope();
}

// Creates AST nodes for the basic classes and installs them in the class list
void CgenClassTable::install_basic_classes()
{
    // The tree package uses these globals to annotate the classes built below.
    curr_lineno = 0;
    Symbol filename = stringtable.add_string("<basic class>");

    //
    // A few special class names are installed in the lookup table but not
    // the class list. Thus, these classes exist, but are not part of the
    // inheritance hierarchy.

    // No_class serves as the parent of Object and the other special classes.
    Class_ noclasscls = class_(No_class, No_class, nil_Features(), filename);
    install_special_class(new CgenNode(noclasscls, CgenNode::Basic, this));
    delete noclasscls;

    // Lab 2 changes from Lab 1
    // SELF_TYPE is the self class; it cannot be redefined or inherited.
    Class_ selftypecls = class_(SELF_TYPE, No_class, nil_Features(), filename);
    install_special_class(new CgenNode(selftypecls, CgenNode::Basic, this));
    delete selftypecls;
    //
    // Primitive types masquerading as classes. This is done so we can
    // get the necessary Symbols for the innards of String, Int, and Bool
    //
    Class_ primstringcls =
      class_(prim_string, No_class, nil_Features(), filename);
    install_special_class(new CgenNode(primstringcls, CgenNode::Basic, this));
    delete primstringcls;
    // End of Lab 2 changes

    Class_ primintcls = class_(prim_int, No_class, nil_Features(), filename);
    install_special_class(new CgenNode(primintcls, CgenNode::Basic, this));
    delete primintcls;
    Class_ primboolcls = class_(prim_bool, No_class, nil_Features(), filename);
    install_special_class(new CgenNode(primboolcls, CgenNode::Basic, this));
    delete primboolcls;
    //
    // The Object class has no parent class. Its methods are
    //    cool_abort() : Object    aborts the program
    //    type_name() : Str        returns a string representation of class name
    //    copy() : SELF_TYPE       returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.
    //
    Class_ objcls = class_(
            Object, No_class,
            append_Features(
                    append_Features(
                            single_Features(method(cool_abort, nil_Formals(),
                                                   Object, no_expr())),
                            single_Features(method(type_name, nil_Formals(),
                                                   String, no_expr()))),
                    single_Features(method(cool_copy, nil_Formals(), SELF_TYPE,
                                           no_expr()))),
            filename);
    install_class(new CgenNode(objcls, CgenNode::Basic, this));
    delete objcls;

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer.
    //
    Class_ intcls =
            class_(Int, Object, single_Features(attr(val, prim_int, no_expr())),
                   filename);
    install_class(new CgenNode(intcls, CgenNode::Basic, this));
    delete intcls;

    //
    // Bool also has only the "val" slot.
    //
    Class_ boolcls =
            class_(Bool, Object,
                   single_Features(attr(val, prim_bool, no_expr())), filename);
    install_class(new CgenNode(boolcls, CgenNode::Basic, this));
    delete boolcls;

    // Lab 2 changes from Lab 1
    //
    // The class String has a number of slots and operations:
    //       val                                  the string itself
    //       length() : Int                       length of the string
    //       concat(arg: Str) : Str               string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring
    //
    Class_ stringcls =
        class_(String, Object,
            append_Features(
                append_Features(
                    append_Features(
                        single_Features(attr(val, prim_string, no_expr())),
                        single_Features(
                            method(length, nil_Formals(), Int, no_expr()))),
                    single_Features(method(::concat,
                                           single_Formals(formal(arg, String)),
                                           String, no_expr()))),
                single_Features(
                    method(substr,
                            append_Formals(single_Formals(formal(arg, Int)),
                                        single_Formals(formal(arg2, Int))),
                            String, no_expr()))),
            filename);
    install_class(new CgenNode(stringcls, CgenNode::Basic, this));
    delete stringcls;

    //
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE          writes a string to the output
    //        out_int(Int) : SELF_TYPE               "    an int    "  "     "
    //        in_string() : Str                    reads a string from the input
    //        in_int() : Int                         "   an int     "  "     "
    //
    Class_ iocls = class_(
        IO, Object,
        append_Features(
            append_Features(
                append_Features(
                    single_Features(method(out_string,
                                           single_Formals(formal(arg, String)),
                                           SELF_TYPE, no_expr())),
                    single_Features(method(out_int,
                                           single_Formals(formal(arg, Int)),
                                           SELF_TYPE, no_expr()))),
                single_Features(
                    method(in_string, nil_Formals(), String, no_expr()))),
            single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
        filename);
    install_class(new CgenNode(iocls, CgenNode::Basic, this));
    delete iocls;
    // End of Lab 2 changes
}

// install_classes enters a list of classes in the symbol table.
void CgenClassTable::install_classes(Classes cs)
{
    for (auto cls: cs) {
        install_class(new CgenNode(cls, CgenNode::NotBasic, this));
    }
}

// Add this CgenNode to the class list and the lookup table
void CgenClassTable::install_class(CgenNode *nd)
{
    Symbol name = nd->get_name();
    if (!this->find(name)) {
        // The class name is legal, so add it to the list of classes
        // and the symbol table.
        nds.push_back(nd);
        this->insert(name, nd);
    }
}

// Add this CgenNode to the special class list and the lookup table
void CgenClassTable::install_special_class(CgenNode *nd)
{
    Symbol name = nd->get_name();
    if (!this->find(name))
    {
        // The class name is legal, so add it to the list of special classes
        // and the symbol table.
        special_nds.push_back(nd);
        this->insert(name, nd);
    }
}

// CgenClassTable::build_inheritance_tree
void CgenClassTable::build_inheritance_tree()
{
    for (auto node: nds)
        set_relations(node);
}

// CgenClassTable::set_relations
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table. Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNode *nd)
{
    Symbol parent_sym = nd->get_parent_name();
    auto parent_node = this->find(parent_sym);
    assert(parent_node && std::string{"Class " + nd->get_name()->get_string() +
                                     " inherits from an undefined class " +
                                     parent_sym->get_string()}.data());
    nd->set_parent(parent_node);
}

// Sets up declarations for extra functions needed for code generation
// You should modify this code for Lab 2
void CgenClassTable::setup_external_functions()
{
    i32 = Type::getInt32Ty(this->context);
    i8 = Type::getInt8Ty(this->context);
    i1 = Type::getInt1Ty(this->context);
    void_ty = Type::getVoidTy(this->context);
    ptr = i8->getPointerTo();

    // setup function: external int strcmp(sbyte*, sbyte*)
    create_llvm_function("strcmp", i32, {ptr, ptr}, false);
    // setup function: external int printf(sbyte*, ...)
    create_llvm_function("printf", i32, {ptr}, true);
    // setup function: external void abort(void)
    create_llvm_function("abort", void_ty, {}, false);
    // setup function: external i8* malloc(i32)
    create_llvm_function("malloc", ptr, {i32}, false);


    // Get rid of address sanitizer output for the generated executable after compilation with clang
    // Disables checking for leaks in your built executable
    // TODO: If you implement garbage collection, remove this block
    {
    auto asan_func = create_llvm_function("__asan_default_options", ptr, {}, false);
    auto asan_func_bb = BasicBlock::Create(this->context, "entry", asan_func);
    builder.SetInsertPoint(asan_func_bb);
    auto asan_string = builder.CreateGlobalStringPtr("detect_leaks=0", "", 0, &the_module);
    builder.CreateRet(asan_string);
    }

    // TODO: You have two options on how to generate code for the method
    //       declarations of the Cool basic classes 
    //       (Object, Int, Bool, String, and IO):
    //
    //       1. Add code here to setup the external functions that
    //       correspond to the methods defined and implemented in the 
    //       Cool runtime library (coolrt) as special cases. This is manual
    //       and a bit tedious, but might be the simplest way to get
    //       started.  
    //       2. Generate code for the method like any other class, except
    //       that you will only generate the method declarations, because 
    //       the actual implementations are in the runtime library. This
    //       is less code to write, but you need to make sure you handle
    //       basic classes differently from programmer-defined ones.
}

void CgenClassTable::setup_classes(CgenNode *c, int depth)
{
    c->setup(current_tag++, depth);
    for (auto child : c->get_children()) {
        setup_classes(child, depth + 1);
    }
    c->set_max_child(current_tag - 1);

    // Ensure no duplicate method creation
    if (c->get_type_name() == "Main") {
        llvm::Function *mainMain1 = the_module.getFunction("Main_main.1");
        if (mainMain1) {
            mainMain1->eraseFromParent();
        }
    }
}

// The code generation first pass. Define these two functions to traverse
// the tree and setup each CgenNode
void CgenClassTable::setup()
{
    setup_external_functions();
    setup_classes(root(), 0);
}

// The code generation second pass. Add code here to traverse the tree and
// emit code for each CgenNode
void CgenClassTable::code_module()
{
    code_constants();

    // This must be after code_constants() since that emits constants
    // needed by the code() method for expressions

    // HINT: This is where the code generation for the classes happens, including 
    // for class Main (and it's member Main.main()).
    code_classes(root());

    // We'll use the same structure as Lab 1 for Lab 2 Ccheckpoint 1
    // After that we will just treat Main_main as a regular class and method
    CgenNode *mainNode = get_main_main(root());
    mainNode->codegen_mainmain();

    // This function will create the main() function that starts the Cool program
    // HINT: This function is independent of the program itself
    code_main();
}

// This function walks the class tree and generates code for each class
void CgenClassTable::code_classes(CgenNode *c) {
    // TODO: add code here
    // HINT: Follow our regular approach of recursing down the tree, but
    //       you will generate code for the base class before the derived
    //       classes (unlike for expressions, where you generate bottom-up).
    if (!c) return;
    c->code_class();
    for (auto child : c->get_children()) {
        code_classes(child);
    }
}

  
// Create global definitions for constant Cool objects
void CgenClassTable::code_constants()
{
    // TODO: add code here
    // HINT: Think of what sort of constants you might need as part
    //       of the ClassTable.Las
    // HINT: You will need to generate code for the string constants
    //       in/using the string table.
    stringtable.code_string_table(this);

}

// Get the root of the class tree.
CgenNode *CgenClassTable::root()
{
    auto root = this->find(Object);

    assert(root && "Class Object is not defined.");
    return root;
}

// Special-case functions used for the method Int Main::main() for
// Lab 1 and Lab 2 CP1.
 CgenNode *CgenClassTable::get_main_main(CgenNode *c) {
    auto ret = this->find(Main);
    return ret;
 }


Function *CgenClassTable::create_llvm_function(std::string const &funcName,
                                               Type *retType,
                                               ArrayRef<Type *> argTypes,
                                               bool isVarArgs)
{
    assert(retType);
    FunctionType *ft = FunctionType::get(retType, argTypes, isVarArgs);
    Function *func = Function::Create(ft, Function::ExternalLinkage, funcName,
                                      this->the_module);
    if (!func)
    {
        errs() << "Function creation failed for function " << funcName;
        llvm_unreachable("Function creation failed");
    }
    return func;
}

void program_class::cgen(std::optional<std::string> const &outfile)
{
    initialize_constants();
    class_table = new CgenClassTable(classes);
    if (outfile)
    {
        std::error_code err;
        raw_fd_ostream s(*outfile, err, sys::fs::FA_Write);
        if (err)
        {
            std::cerr << "Cannot open output file " << *outfile << std::endl;
            exit(1);
        }
        s << class_table->the_module;
    }
    else
    {
        outs() << class_table->the_module;
    }
}

// TODO: Your own helpers come here.
//For get_type
llvm::Type* CgenClassTable::get_type(Symbol type_decl) {
    if (type_decl == Int) {
        return i32; // Assuming i32 is defined as Type::getInt32Ty(context)
    } else if (type_decl == Bool) {
        return i1;  // Assuming i1 is defined as Type::getInt1Ty(context)
    }
    // Add additional type mappings as needed.
    // Default to a pointer type (or an error) if the type is not recognized.
    return llvm::PointerType::getUnqual(context);
}



/*********************************************************************

  StrTable / IntTable methods

 Coding string, int, and boolean constants

 Cool has three kinds of constants: strings, ints, and booleans.
 This section defines code generation for each type.

 All string constants are listed in the global "stringtable" and have
 type stringEntry. stringEntry methods are defined both for string
 constant definitions and references.

 All integer constants are listed in the global "inttable" and have
 type IntEntry. IntEntry methods are defined for Int constant references only.

 Since there are only two Bool values, there is no need for a table.
 The two booleans are represented by instances of the class BoolConst,
 which defines the definition and reference methods for Bools.

*********************************************************************/

// Create definitions for all String constants
void StrTable::code_string_table(CgenClassTable *ct)
{
    // TODO: Depending on how you choose to turn charater strings into Cool Strings,
    //       you may need to modify this function.
    for (auto &[_, entry]: this->_table)
    {
        entry.code_def(ct);
    }
}

// generate code to define a global string constant
void StringEntry::code_def(CgenClassTable *ct)
{
    // TODO: define global character string constants
    // TODO: depending on how you choose to represent Cool strings, you may need
    //       to do additional things here beyond defining the global char string constant.
    LLVMContext &context = ct->context;
    // Retrieve the string value. (Assume get_string() returns a std::string.)
    std::string strVal = this->get_string();

    // Create a constant data array from the string.
    llvm::Constant *strConstant = llvm::ConstantDataArray::getString(context, strVal, true);

    // Create a global variable in the module to hold the constant.
    // The global's name should be unique. For example, you can use a prefix and the string's address.
    std::string globalName = "str_" + strVal; // You might want to sanitize this.
    llvm::GlobalVariable *globalStr = new llvm::GlobalVariable(
        ct->the_module,                   // Module in which to create the global
        strConstant->getType(),           // The type of the constant
        true,                             // isConstant
        llvm::GlobalValue::PrivateLinkage, // Linkage
        strConstant,                      // Initial value
        globalName                        // Global name
    );
}