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


/** This class is designed after the Process_Field_Locator,
    but is meant to be used for a wide range of classes.
    I left in the Process_Field_Locator since it is
    staticically defined and might enable the compiler to
    optimize better for the specific case of Processes.
    STEFAN 2011-09-19 */
class Object_Field_Accessor {
public:
  Object_Field_Accessor(const char* const * const field_names, size_t const number_of_fields)
  : field_names(field_names),
    number_of_fields(number_of_fields),
    class_oop(Oop::from_bits(Oop::Illegals::uninitialized)),
    indices(new oop_int_t[number_of_fields]) {}

public:
  void update_indices(Oop class_oop);
  
  Oop  get_field(Oop source_oop, oop_int_t field_name_idx);
  bool set_field(Oop target_oop, oop_int_t field_name_idx, Oop value);

  void print_results();
  
private:
  const char* const *const field_names;
  size_t const             number_of_fields;
  
  /** Even so we cater for changing class layouts, it
      is seldomly safe in a concurrent setting.
      Make sure that your class updates are done in a protected way!!
      class_oop is checked to notice changes in the class.
      I do not do the check for a timestamp like it is done for the 
      process class, since that would require special mechanisms for
      all classes on which the field locator is used. */
  Oop        class_oop;
  
  oop_int_t* indices;

  Object_p instance_variable_names();  
  int instance_variable_count_of_superclasses();

  
  /** field_name_idx corresponds to the possition of the field name in the give field_names array.
   class_oop is passed in in case the class has been redefined, then we can notice.
   Otherwise, we would need to expose the class_oop member to the GC, which is inconvenient. */
  int index_of_instance_variable(oop_int_t field_name_idx, Oop class_oop);


};
