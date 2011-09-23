/******************************************************************************
 *  Copyright (c) 2008 - 2010 IBM Corporation and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *    Stefan Marr, Vrije Universiteit Brussel - Project OMNI
 ******************************************************************************/



/* Define the policy flags.
   We use here the Smalltalk naming conventions for instance
   variables to avoid inconsistencies. */
# define DO_ALL_POLICY_FLAGS(template)  \
         template(foreignSyncRead,     0x4000000)    \
         template(foreignSyncWrite,    0x8000000)    \
         template(foreignSyncExecute,  0x10000000)   \
         template(foreignAsyncRead,    0x20000000)   \
         template(foreignAsyncWrite,   0x40000000)   \
         template(foreignAsyncExecute, 0x80000000)
# define NUMBER_OF_POLICY_FLAGS        6 /* make sure it is updated!!! */

class Domain_Info {
public:
  /** Define the fields position enum */
  typedef enum fields {
    logicId,
    
    # define DEFINE_FLAG(name, value) name = value,
    DO_ALL_POLICY_FLAGS(DEFINE_FLAG)
    # undef  DEFINE_FLAG
    
  } fields_t;

  static const size_t      field_count = NUMBER_OF_POLICY_FLAGS;
  static const char* const field_names[];
  static const size_t logic_id_bits = (sizeof(u_oop_int_t) * CHAR_BIT) - ((field_count - 1) + Tag_Size);

  enum Special_Raw_Values {
    REFLECTIVE_DOMAIN       = Int_Tag, /* This is the standard domain for Smalltalk: It is used as a magic value, and for initialization. */

    GLOBAL_IMMUTABLE_DOMAIN =   foreignSyncRead    | foreignAsyncRead 
                              | foreignSyncExecute | foreignAsyncExecute | Int_Tag,

    RECOGNIZABLE_BOGUS_DOMAIN = 0xe0e0e0e0, /* Should be used in freed preheaders only */
  };
};


typedef union domain_info {  
  oop_int_t raw_value;
  struct {
    unsigned int_tag  : Tag_Size; /* We will treat the domain encoding as a SmallInteger
                                   throughout the whole system to avoid GC issues and simplify
                                   handling in general.
                                   Since, in the worst case, it can be en/decoded manually on the 
                                   Smalltalk side.
                                   STEFAN 2011-07-10 */
    
    unsigned logicId : Domain_Info::logic_id_bits;
    
    /* Policy bits */
    # define DEFINE_POLICY_BITS(name, value) bool name : 1;
    DO_ALL_POLICY_FLAGS(DEFINE_POLICY_BITS)
    # undef DEFINE_POLICY_BITS      
  } __attribute__ ((__packed__)) bits;  /** make sure we have it really in bits as desired */
} domain_info_t;

