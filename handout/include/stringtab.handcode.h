// This file defines extra methods for classes in stringtab.h
// that are specific to code generation.
#ifndef STRINGTAB_HANDCODE_H
#define STRINGTAB_HANDCODE_H

class CgenClassTable;

// Extra methods added to classes in stringtab.h
#define StringEntry_EXTRAS                                                     \
    void code_def(CgenClassTable *classTable);              \
    void code_ref(CgenClassTable *classTable);

#define IntEntry_EXTRAS                                                        \
    void code_ref(CgenClassTable *classTable);

#define StrTable_EXTRAS                                                        \
    void code_string_table(CgenClassTable *classTable);

#endif /* STRINGTAB_HANDCODE_H */
