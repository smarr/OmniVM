/******************************************************************************
 *  Copyright (c) 2008 - 2011 IBM Corporation and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *    Stefan Marr, Vrije Universiteit Brussel
 ******************************************************************************/


/**
 * Behaves like the Execution_Tracer but can be easily enabled/disabled
 * for instance via a primitive.
 */
class Triggerable_Execution_Tracer: public Execution_Tracer {
public:
  Triggerable_Execution_Tracer(int num_elements_in_history)
    : Execution_Tracer(num_elements_in_history),
      do_record(false), do_print(false) {}

  virtual void trace(Squeak_Interpreter* si) {
    if (do_record)
      Execution_Tracer::trace(si);
    if (do_print)
      print();
  }

  /* We do not print all the stack traces here, since we want to
     use the traces directly on every bytecode execution. */
  virtual void print() {
    Oop entries = get();
    check_it(entries);
    
    print_entries(entries, debug_printer);  
  }
  
  void enable_recording_and_printing() {
    do_record = true;
    do_print  = true;
  }
  
  void disable_recording_and_printing() {
    do_record = false;
    do_print  = false;
  }

private:
  bool do_record;
  bool do_print;
};

