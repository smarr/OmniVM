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


/** Primitives for Project Omni **/

# include "headers.h"

static const char *moduleName =
#ifdef SQUEAK_BUILTIN_PLUGIN
"OmniPlugin v0.1 15 July 2011 (i)"
#else
"OmniPlugin v0.1 15 July 2011 (e)"
#endif
;

static const char* getModuleName() {
  return moduleName;
}


/** {{{ OMirror class>>#primitiveGetDomainOf: anObject 
            ^ aDomainInfo }}} **/
static int primitiveGetDomain() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 1) {
    interp->primitiveFail();
    return 0;
  }
   
  Oop o = interp->stackObjectValue(0);
  if (interp->failed()) {
    return 0;
  }
  
  interp->popThenPush(2, o.as_object()->domain_oop());
  
  return 0;
}


/** {{{ OMirror class>>#primitiveSetDomainOf: anObject to: aDomain }}} **/
static int primitiveSetDomain() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 2) {
    interp->primitiveFail();
    return 0;
  }
  
  Oop domain = interp->stackValue(0);

  if (!domain.is_mem()) {
    interp->primitiveFail();
    return 0;
  }
  
  
  Oop target = interp->stackObjectValue(1);
  if (interp->failed()) {
    return 0;
  }
  
  assert(target.as_object()->domain_oop().bits() != 0);
  assert(target.as_object()->domain_oop().bits() != Oop::Illegals::free_extra_preheader_words);
  
  target.as_object()->set_domain(domain);
  interp->pop(2);
  
  return 0;
}

/** Required to be picked up by the loading mechanism as an internal plugin */
static int setInterpreter(struct VirtualMachine* /* anInterpreter */) {
	return 1;
}


void* OmniPlugin_exports[][3] = {
  {(void*) "OmniPlugin", (void*)"primitiveGetDomain", (void*)primitiveGetDomain},
  {(void*) "OmniPlugin", (void*)"primitiveSetDomain", (void*)primitiveSetDomain},
  
  
  /* Required by the internal loading mechanism */
  {(void*) "OmniPlugin", (void*)"setInterpreter",         (void*)setInterpreter},
  {NULL, NULL, NULL}
};

