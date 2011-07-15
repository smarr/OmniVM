/******************************************************************************
 *  Copyright (c) 2008 - 2010 IBM Corporation and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *    Stefan Marr, Vrije Universiteit Brussel - Initial Implementation
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

static const char* getModuleName() { return moduleName; }


/** RVMOperations class>>primitiveGetDomainInfoFor: anObject
    ^ { #DomainId.
        #ForeignSyncRead. #ForeignSyncWrite. #ForeignSyncExecute.
        #ForeignAsyncRead. #ForeignAsyncWrite. #ForeignAsyncExecute.} */
static int primitiveGeDDDDDDDDDDDDDDDtDomainInfo() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 1) {
    interp->primitiveFail();
    return 0;
  }

  Oop x = interp->stackObjectValue(0);
  if (interp->failed())
    return 0;
  
  Preheader::domain_header_t domain = x.as_object()->domain_header();
  
  int s = interp->makeArrayStart();
  PUSH_POSITIVE_32_BIT_INT_FOR_MAKE_ARRAY(domain.bits.logic_id);
  PUSH_BOOL_FOR_MAKE_ARRAY(domain.bits.foreign_sync_read);
  PUSH_BOOL_FOR_MAKE_ARRAY(domain.bits.foreign_sync_write);
  PUSH_BOOL_FOR_MAKE_ARRAY(domain.bits.foreign_sync_execute);
  PUSH_BOOL_FOR_MAKE_ARRAY(domain.bits.foreign_async_read);
  PUSH_BOOL_FOR_MAKE_ARRAY(domain.bits.foreign_async_write);
  PUSH_BOOL_FOR_MAKE_ARRAY(domain.bits.foreign_async_execute);

  interp->popThenPush(2, interp->makeArray(s));
  
  return 0;
}

/** RVMOperations class>>primitiveSetDomainInfoFor: anObject 
        to: aDomainId
        foreignSyncRead:  aBool foreignSyncWrite:  aBool foreignSyncExecute:  aBool 
        foreignAsyncRead: aBool foreignAsyncWrite: aBool foreignAsyncExecute: aBool */
static int primitiveENCODEDomainInfo() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  const int ARG_CNT = 7;
  
  if (interp->get_argumentCount() != ARG_CNT) {
    interp->primitiveFail();
    return 0;
  }
  
  bool async_execute = interp->stackBooleanValue(0);
  bool async_write   = interp->stackBooleanValue(1);
  bool async_read    = interp->stackBooleanValue(2);
  
  bool sync_execute  = interp->stackBooleanValue(3);
  bool sync_write    = interp->stackBooleanValue(4);
  bool sync_read     = interp->stackBooleanValue(5);

  bool logic_domain_id = interp->stackIntegerValue(6);
  
  Oop target = interp->stackObjectValue(7);

  if (interp->failed())
    return 0;

  Preheader::domain_header_t domain;
  domain.bits.logic_id       = logic_domain_id;
  domain.bits.foreign_sync_read     = sync_read;
  domain.bits.foreign_sync_write    = sync_write;
  domain.bits.foreign_sync_execute  = sync_execute;
  domain.bits.foreign_async_read    = async_read;
  domain.bits.foreign_async_write   = async_write;
  domain.bits.foreign_async_execute = async_execute;
    
  domain.bits.int_tag  = Int_Tag;
  
  target.as_object()->set_domain_header(domain);
  interp->pop(ARG_CNT);
  return 0;
}


/** {{{ OMirror class>>#primitiveGetDomainInfoFor: anObject }}} **/
static int primitiveGetDomainInfo() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 1) {
    interp->primitiveFail();
    return 0;
  }
   
  Oop o = interp->stackObjectValue(0);
  if (interp->failed()) {
    return 0;
  }
  
  interp->popThenPush(2, o.as_object()->domain_header_oop());
  
  return 0;
}


/** {{{ OMirror class>>#primitiveSetDomainInfoFor: anObject to: aDomainInfo }}} **/
static int primitiveSetDomainInfo() {
  Squeak_Interpreter* const interp = The_Squeak_Interpreter();
  
  if (interp->get_argumentCount() != 2) {
    interp->primitiveFail();
    return 0;
  }
  
  Oop domainInfo = interp->stackValue(0);

  if (not domainInfo.is_int()) {
    interp->primitiveFail();
    return 0;
  }
  
  
  Oop target = interp->stackObjectValue(1);
  if (interp->failed()) {
    return 0;
  }

  target.as_object()->set_domain_header(domainInfo);
  interp->pop(2);
  
  return 0;
}

/** Required to be picked up by the loading mechanism as an internal plugin */
static int setInterpreter(struct VirtualMachine* /* anInterpreter */) {
	return 1;
}


void* OmniPlugin_exports[][3] = {
  {(void*) "OmniPlugin", (void*)"primitiveGetDomainInfo", (void*)primitiveGetDomainInfo},
  {(void*) "OmniPlugin", (void*)"primitiveSetDomainInfo", (void*)primitiveSetDomainInfo},
  
  {(void*) "OmniPlugin", (void*)"setInterpreter", (void*)setInterpreter},

  {NULL, NULL, NULL}
};

