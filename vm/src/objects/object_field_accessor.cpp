/******************************************************************************
 *  Copyright (c) 2008 - 2010 IBM Corporation and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *    David Ungar, IBM Research
 *    Stefan Marr, Vrije Universiteit Brussel
 ******************************************************************************/


# include "headers.h"


int Object_Field_Accessor::index_of_instance_variable(oop_int_t field_name_idx, Oop class_oop) {
  if (field_name_idx > number_of_fields)
    return -1;
  return indices[field_name_idx];
}

bool Object_Field_Accessor::set_field(Oop target_oop, oop_int_t field_name_idx, Oop value) {
  Object_p target = target_oop.as_object();
  
  int field_idx = index_of_instance_variable(field_name_idx, class_oop);
  
  if (field_idx < 0 || field_idx >= target->lengthOf())
    return false;
  
  target->storePointer(field_idx, value);
}

Oop Object_Field_Accessor::get_field(Oop source_oop, oop_int_t field_name_idx) {
  assert(source_oop != The_Squeak_Interpreter()->roots.nilObj);
  
  Object_p source = source_oop.as_object();
  
  int field_idx = index_of_instance_variable(field_name_idx, class_oop);

  if (field_idx < 0 || field_idx >= source->lengthOf())
    return Oop::from_bits(Oop::Illegals::uninitialized);

  return source->fetchPointer(field_idx);
}


void Object_Field_Accessor::print_results() {
  for (int i = 0; i < number_of_fields; i++) 
    lprintf("indices[%s] = %d\n", field_names[i], indices[i]);
}


Object_p Object_Field_Accessor::instance_variable_names() {
  Object_p class_obj = class_oop.as_object();
  return class_obj->fetchPointer(class_obj->instance_variable_names_index_of_class(field_names[0])).as_object();
}


int Object_Field_Accessor::instance_variable_count_of_superclasses() {
  return Object::ClassFormat::fixedFields( class_oop.as_object()->superclass().as_object()->formatOfClass() );
}


void Object_Field_Accessor::update_indices(Oop class_oop) {
  this->class_oop = class_oop;
  
  Object_p inst_var_names = instance_variable_names();

  int n = instance_variable_count_of_superclasses();
  for (int i = 0; i < number_of_fields;  ++i) {
    int index =  inst_var_names->index_of_string_in_array(field_names[i]);
    indices[i] = index < 0  ?  index  :  index + n; // if -1 must keep -1 for none
  }

  if (check_assertions)
    print_results();
}

