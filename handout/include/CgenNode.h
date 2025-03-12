#ifndef CGENNODE_H
#define CGENNODE_H

#include <span>
#include <llvm/ADT/SmallVector.h>
#include "CgenClassTable.h"
#include "cool_tree.h"
#include "symbol.h"

// Each CgenNode corresponds to a Cool class. As such, it is responsible for
// performing code generation on the class level. This includes laying out
// the class attributes, creating the necessary Types for the class and
// generating code for each of its methods.
class CgenNode : public class__class
{
public:
    enum Basicness
    {
        Basic,
        NotBasic
    };
    CgenNode(Class_ c, Basicness bstatus, CgenClassTable *class_table) :
        class__class((class__class const &) *c), parentnd(0), children(0),
        basic_status(bstatus), class_table(class_table), tag(-1), attribute_count(0)
    {
    }

    // Relationships with other nodes in the tree
    void set_parent(CgenNode *p)
    {
        assert(this->parentnd == nullptr);
        assert(p);
        p->children.push_back(this);
        this->parentnd = p;
    }
    CgenNode *get_parentnd() { return parentnd; }
    int basic() const { return basic_status == Basic; }
    std::span<CgenNode *const> get_children() { return children; }
    void set_max_child(int mc) { max_child = mc; }
    int get_max_child() const { return max_child; }

    // Accessors for other provided fields
    int get_tag() const { return tag; }
    CgenClassTable *get_classtable() { return class_table; }

    // Lab 2 additions
    std::string get_type_name() { return name->get_string(); }
    std::string get_vtable_type_name() {
      return "_" + get_type_name() + "_vtable";
    }
    std::string get_vtable_name() {
      return "_" + get_type_name() + "_vtable_prototype";
    }
    std::string get_init_function_name() { return get_type_name() + "_new"; }
    // End Lab 2 additions

    void setup(int tag, int depth);
    // Layout the methods and attributes for code generation
    void layout_features();
    // Class codegen. You need to write the body of this function.
    void code_class();
    // Codegen for the init function of every class
    void code_init_function(CgenEnvironment *env);
    // TODO: Complete the implementation of the following function
    void codegen_mainmain();

    //My additions
    int num_attributes;
    int attribute_count;

    CgenNode* get_parent() { return parentnd; }
    int get_attribute_count() const {
        return attribute_count;
    }

    std::unordered_map<std::string, llvm::Constant*> method_map;

    void add_method_to_vtable(Symbol method_name, llvm::Function* func) {
        // Use bitcasting to cast the function pointer to a uniform pointer type.
        llvm::Type *vtableSlotType = llvm::Type::getInt8Ty(class_table->context)->getPointerTo();
        llvm::Constant *func_ptr = llvm::ConstantExpr::getBitCast(func, vtableSlotType);
        method_map[method_name->get_string()] = func_ptr;
    }

    llvm::StructType* get_type() const {
        return classType;
    }

    void set_class_type(llvm::StructType* type) { classType = type; }

    CgenClassTable* get_class_table() const {
        return class_table;
    }

    Symbol get_parent_name() const {
        return parent; // assuming 'parent' is the parent's name (a Symbol)
    }

    int find_attribute_slot_for(Symbol attrName);








private:
    CgenNode *parentnd; // Parent of class
    llvm::SmallVector<CgenNode *> children; // Children of class
    Basicness basic_status; // `Basic' or 'NotBasic'
    CgenClassTable *class_table;
    // Class tag. Should be unique for each class in the tree
    int tag, max_child;

    //mine
    llvm::StructType* classType;


};

// TODO: implement these functions (LAB2), and add more functions as necessary

// Utitlity function
// Generate any code necessary to convert from given Value* to
// dest_type, assuming it has already been checked to be compatible.
// THis is where you should implement boxing and unboxing
llvm::Value* conform(llvm::Value* src, Symbol type_from, 
                     Symbol type_to, CgenEnvironment *env);

#endif //CGENNODE_H
