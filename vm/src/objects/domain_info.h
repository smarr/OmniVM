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
         template(foreignSyncRead)    \
         template(foreignSyncWrite)   \
         template(foreignSyncExecute) \
         template(foreignAsyncRead)   \
         template(foreignAsyncWrite)  \
         template(foreignAsyncExecute)

class Domain_Info {
public:
  /** Define the fields position enum */
  typedef enum fields {
    logicId,
    
    # define DEFINE_FLAG(name) name,
    DO_ALL_POLICY_FLAGS(DEFINE_FLAG)
    # undef  DEFINE_FLAG
    
    field_count
  } fields_t;

  
  static const char* const field_names[];
  static const size_t logic_id_bits = (sizeof(u_oop_int_t) * 8) - ((field_count - 1) + Tag_Size);

  
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
    # define DEFINE_POLICY_BITS(name) bool name : 1;
    DO_ALL_POLICY_FLAGS(DEFINE_POLICY_BITS)
    # undef DEFINE_POLICY_BITS      
  } __attribute__ ((__packed__)) bits;  /** make sure we have it really in bits as desired */
} domain_info_t;

