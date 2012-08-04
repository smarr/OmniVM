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


/**
 * VM-side representation of the OstDomain class.
 */
extern class OstDomain {
private:
  static const char * const field_names[];
  Object_Field_Accessor field_accessor;
  Oop domain_selectors;
  
  inline Oop get_domain_selector(oop_int_t selector_id) const {
    return domain_selectors.as_object()->fetchPointer(selector_id);
  }
  
public:
  OstDomain() : field_accessor(Object_Field_Accessor(field_names, 2)) {}
  
  void initialize(Oop ostDomain);
  
  inline Oop get_domain_for_new_objects(Oop domain) const {
    return field_accessor.get_field(domain, 0);
  }
  
  inline Oop get_domain_customization_encoding(Oop domain) const {
    return field_accessor.get_field(domain, 1);
  }
  
  /** customization_encoding
          is the direct Oop in the domain, 
          usually obtained with get_domain_customization_encoding(..).
      selector
          can be any OstDomainSelector_Indices */
  static inline bool domain_customizes_selector(Oop customization_encoding, oop_int_t selector) {
    return (1 << selector) & customization_encoding.bits();
  }
  
  /** customization_encoding
          is the direct Oop in the domain, 
          usually obtained with get_domain_customization_encoding(..).
      mask
          can be any mask defined in OstDomainSelector_Indices */
  static inline bool domain_customizes_selectors(Oop customization_encoding, oop_int_t mask) {
    return mask & customization_encoding.bits();
  }
  
  inline Oop read_field() const {
    return get_domain_selector(OstDomainSelector_Indices::ReadField_Of_);
  }
  
  inline Oop write_field() const {
    return get_domain_selector(OstDomainSelector_Indices::Write_ToField_Of_);
  }
  
  inline Oop write_field_with_return() const {
    return get_domain_selector(OstDomainSelector_Indices::Write_ToField_Of_Return_);
  }
  
  inline Oop read_literal() const {
    return get_domain_selector(OstDomainSelector_Indices::ReadLiteral_);
  }
  
  inline Oop write_literal() const {
    return get_domain_selector(OstDomainSelector_Indices::Write_ToLiteral_);
  }


  inline Oop request_exec(int arg_cnt) const {
    if (arg_cnt == 0)
      return get_domain_selector(
              OstDomainSelector_Indices::RequestExecutionOf_On_);
    
    oop_int_t selector_id = OstDomainSelector_Indices::RequestExecution_Of_On_ + arg_cnt - 1;
    return get_domain_selector(selector_id);
  }

  inline Oop request_exec_in_lookup_class() const {
    return get_domain_selector(
              OstDomainSelector_Indices::RequestExecutionOf_With_Lookup_On_);
  }
  
  inline Oop prim_at_on() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimAt_On_);
  }
  
  inline Oop prim_at_put_on() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimAt_On_Put_);
  }
  
  inline Oop prim_basic_at_on() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimBasicAt_On_);
  }
  
  inline Oop prim_basic_at_put_on() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimBasicAt_On_Put_);
  }

  inline Oop prim_inst_var_at_on() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimInstVarAt_On_);
  }

  inline Oop prim_inst_var_at_put_on() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimInstVarAt_On_Put_);
  }

  inline Oop prim_shallow_copy() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimShallowCopy_);
  }
  
  inline Oop prim_next() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimNext_);
  }
  
  inline Oop prim_next_put() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimNext_Put_);
  }
  
  inline Oop prim_replace_from_to_with_starting_at_on() const {
    return get_domain_selector(OstDomainSelector_Indices::PrimReplaceFrom_To_With_StartingAt_On_);
  }

} The_OstDomain;

