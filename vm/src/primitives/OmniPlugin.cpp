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
  
  Oop result = o.as_object()->domain_oop();
  
  /** Approach for lazily repairing NULL initialized objects
      with nilObj */
  if (result.bits() == 0 /* NULL */) {
    result = interp->roots.nilObj;
    o.as_object()->set_domain(interp->roots.nilObj);
  }
  
  interp->popThenPush(2, result);
  
  return 0;
}


/** {{{ OMirror class>>#primitiveSetDomainOf: anObject to: aDomain }}} **/
static int primitiveSetDomain() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 2) {
    interp->primitiveFail();
    return 0;
  }
  
  Oop domain = interp->stackObjectValue(0);
  if (interp->failed()) {
    return 0;
  }
  
  
  Oop target = interp->stackObjectValue(1);
  if (interp->failed()) {
    return 0;
  }
  
  assert(target.as_object()->domain_oop().bits() != Oop::Illegals::free_extra_preheader_words);
  
  target.as_object()->set_domain(domain);
  interp->pop(2);
  
  return 0;
}

/** {{{ ONOMirror class>>#executesOnBaseLevel }}} */
static int primitiveExecutesOnBaseLevel() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  interp->popStack();
  interp->pushBool(interp->activeContext_obj()->domain_execute_on_baselevel());
  
  return 0;
}

/** {{{ ONOMirror class>>#executesOnMetaLevel }}} */
static int primitiveExecutesOnMetaLevel() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  interp->popStack();
  interp->pushBool(interp->activeContext_obj()->domain_execute_on_metalevel());
  
  
  return 0;
}

/** {{{ ONOMirror class>>#evaluate: block onBaseLevel: domain }}} */
/** original implementation was in Smalltalk, but that did not work
    properly anymore once I intrduced support for literals.
    Now we need this as a primitive
 
ONODomain >> #evaluateInContextOnBaseLevel: aBlock
 | oldDomain result |
 <omniMetaExit>
 oldDomain := ONOMirror domainOf: thisContext.
 ONOMirror setDomainOf: thisContext to: self.
 ONOMirror switchToBaseLevel.
 
 result := aBlock value.
 
 ONOMirror switchToMetaLevel.
 ONOMirror setDomainOf: thisContext to: oldDomain.
 
 ^ result
 **/
static int primitiveEvaluateOnBaseLevel() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 2) {
    interp->primitiveFail();
    return 0;
  }
  
  Oop domain = interp->stackObjectValue(0);
  if (interp->failed())
    return 0;

  Oop block = interp->stackObjectValue(1);
  if (interp->failed())
    return 0;
    
  // since we do not yet support arguments, but the used code expects being 
  // called directly with the right calling convention
  interp->popThenPush(3, block);
  interp->set_argumentCount(0);
  
  Oop klass = block.fetchClass();
# if Include_Closure_Support
  if (klass == interp->splObj(Special_Indices::ClassBlockClosure)) {
    interp->primitiveClosureValueNoContextSwitch();
  }
  else 
# endif
  if (klass == interp->splObj(Special_Indices::ClassBlockContext)) {
    Object_p bco = block.as_object();
    interp->primitiveValue();

  } 
  else
    interp->primitiveFail();
  
  
  /** Now the magic: setting the domain infos */
  interp->activeContext_obj()->set_domain(domain);
  interp->switch_to_baselevel();
  interp->_localDomain = domain.as_object();
  

  return 0;
}





/** Required to be picked up by the loading mechanism as an internal plugin */
static int setInterpreter(struct VirtualMachine* /* anInterpreter */) {
	return 1;
}


void* OmniPlugin_exports[][3] = {
  {(void*) "OmniPlugin", (void*)"primitiveGetDomain", (void*)primitiveGetDomain},
  {(void*) "OmniPlugin", (void*)"primitiveSetDomain", (void*)primitiveSetDomain},
    
  {(void*) "OmniPlugin", (void*)"primitiveExecutesOnMetaLevel", (void*)primitiveExecutesOnMetaLevel},
  {(void*) "OmniPlugin", (void*)"primitiveExecutesOnBaseLevel", (void*)primitiveExecutesOnBaseLevel},
  
  {(void*) "OmniPlugin", (void*)"primitiveEvaluateOnBaseLevel", (void*)primitiveEvaluateOnBaseLevel},
  
  
  /* Required by the internal loading mechanism */
  {(void*) "OmniPlugin", (void*)"setInterpreter",         (void*)setInterpreter},
  {NULL, NULL, NULL}
};

