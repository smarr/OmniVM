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

  
public:
  OstDomain() : field_accessor(Object_Field_Accessor(field_names, 1)) {}
  
  void initialize(Oop ostDomain) {
    field_accessor.update_indices(ostDomain);
  }
  
  Oop get_domain_for_new_objects(Oop domain) {
    return field_accessor.get_field(domain, 0);
  }
                               
  
} The_OstDomain;

